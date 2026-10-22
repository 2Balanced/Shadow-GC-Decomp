/* Elevator::Elevator (class name from RTTI): the ELEVATOR gadget, a lift built from the "model"
 * entry of ELEVATOR.BIN. Once its switch (the owner's id byte) is on, a ride command or a character
 * standing on it (its collision callback) sends it "distance" units up or down at "speed"; it plays
 * the entry's "<name>_loop" sound while moving and "<name>_end" when it stops, and goes back to its
 * start once the player is farther away than "range". File name inferred from the class. Method
 * names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <cstdio>

#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/FileQueue.h"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* The model entry of the table for one "model" parameter: fn_80208978 creates one and
 * the accessors below read it. Its shared_ptr instances (0x800E4D8C-0x800E4E88) are shared with
 * Stick and BreakWall; the RTTI of its control block (vtable 0x80544154, constructor
 * 0x80208B84) calls it GadgetBuilder::CrashInfoCtrl. */
namespace GadgetBuilder {
class CrashInfoCtrl;
} // namespace GadgetBuilder

/* A model's table of named entries in its resource file (names as in Stick). */
class UnkModelTable_80208888;

/* A sound entry of the table: fn_80208888 finds it by name (layout as in Stick and CircusBall). */
struct UnkSoundEntry_80208888 {
    /* 0x00 */ const char *name;
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ u32 soundId; /* passed to the sound calls as 16 bits */
    /* 0x0C */ u32 field_0xC;
    /* 0x10 */ Vec3f offset;
};
SIZE_ASSERT(UnkSoundEntry_80208888, 0x1C);

extern "C" {
/* Builds the entry for model of the table, reports whether the table holds that entry, and
 * returns the sound entry named name or 0 (behaviour inferred). */
boost::shared_ptr<GadgetBuilder::CrashInfoCtrl> fn_80208978(UnkModelTable_80208888 *table, int model);
bool fn_80208BB8(UnkModelTable_80208888 *table, int model);
UnkSoundEntry_80208888 *fn_80208888(UnkModelTable_80208888 *table, const char *name);
/* Accessors of the entry, each 0 when the entry is empty (meanings inferred from their callers):
 * the model to show, the name of the collision node and the base name of the entry's sounds. */
void *fn_802087D8(GadgetBuilder::CrashInfoCtrl *entry);
const char *fn_802087BC(GadgetBuilder::CrashInfoCtrl *entry);
const char *fn_80208768(GadgetBuilder::CrashInfoCtrl *entry);
/* A 32-bit flag word: fn_80014E04 clears it, fn_80014E1C sets bit, fn_8006127C clears bit and
 * fn_80014E34 tests bit (behaviour inferred from their bodies). */
void fn_80014E04(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);

/* Copies the owner's id byte at +0x22 into value (behaviour inferred). */
void fn_800C9E50(UnkPlacement_800C9CD0 *owner, s32 *value);
/* Loads the model file fileName (behaviour inferred; arg0 unknown). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* Returns the file loader, and the table that file fileName keeps in it (behaviour inferred). */
void *fn_800122D4(void);
UnkModelTable_80208888 *fn_80012700(void *loader, int arg1, const char *fileName);
/* Shows model of the model file (behaviour inferred; arg2 unknown). */
void fn_801629D8(GadgetModelInfo *model, void *part, int arg2);
/* True when the collision event comes from a character standing on the model (behaviour
 * inferred). */
bool fn_8015AC84(ColliDetectArgs &args);
/* Marks the gadget finished (behaviour inferred). */
void fn_8015A794(Gadget *gadget);

/* GadgetModelInfo helpers (behaviour inferred). */
void fn_8015FA78(GadgetModelInfo *model, void *(*callback)(void *part, UnkParticleAttach_80169D00 *param),
                 UnkParticleAttach_80169D00 *param);
void *fn_80169D00(void *part, UnkParticleAttach_80169D00 *param);
/* Creates a particle controller, advances it by dt, and starts its emitters (behaviour
 * inferred). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void fn_8016761C(GadgetParticle::GadgetParticleCtrl *ctrl, float dt);
void fn_80167010(GadgetParticle::GadgetParticleCtrl *ctrl, int id);

/* Plays sound id at charInfo, or from source (behaviour inferred; the other arguments are
 * unknown). */
void fn_801D54F4(void *sound, int id, GadgetCharInfo *charInfo, int arg3, int arg4);
void fn_801D53C4(void *sound, int id, int arg2, void *source, int arg4, int arg5, int arg6);
/* A constant passed with the moving sound (meaning unknown). */
extern const s16 lbl_805F4A80;

/* A debug cone at position, turned by rotation (behaviour inferred). */
void fn_802E0C60(const Vec3f *position, const Vec3f *rotation, int arg2,
                 float radius, float length);
}

/* Sets rotation to angles turned a quarter turn about the X axis (name inferred). */
inline void TurnAboutX(Vec3f *rotation, const Vec3f &angles) {
    float angle = 90.0f * (lbl_805FAD38 / 180.0f);

    rotation->x = angles.x;
    rotation->y = angles.y;
    rotation->z = angles.z;
    rotation->x += angle;
}

namespace Elevator {

/* The parameters the owner supplies (names from the parameter table). */
struct ElevatorParams {
    ElevatorParams() : model(0), distance(0.0f), speed(0.0f), range(0.0f) {}

    /* 0x0 */ s32 model;
    /* 0x4 */ float distance;
    /* 0x8 */ float speed;
    /* 0xC */ float range;
};
SIZE_ASSERT(ElevatorParams, 0x10);

class Elevator : public Gadget {
  public:
    Elevator(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~Elevator();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the elevator under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* The command unit's callback (name inferred). */
    void OnCommand(CharCommand &command);
    /* True once the player is at least "range" away horizontally (name inferred). */
    bool IsPlayerAway();
    /* Places the model at the character info's translation, turned about the Y axis (name
     * inferred). */
    void RebuildTransform();
    /* Copies the owner's position, rotation, parameters and switch id, and rests the elevator at
     * its start (name inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    /* Shows the model entry m_params.model and sets up its collision and sounds (name inferred). */
    void SetupModel();
    /* Drops the model entry (name inferred). */
    void ReleaseModel();
    /* The model's collision callback (name inferred). */
    void OnColliDetect(ColliDetectArgs &args);

    static const char *s_fileName; /* (name inferred) */
    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::shared_ptr<GadgetBuilder::CrashInfoCtrl> m_entry;
    /* 0x48 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particle;
    /* 0x50 */ UnkModelTable_80208888 *m_table; /* the table of ELEVATOR.BIN */
    /* 0x54 */ s32 m_shownModel;
    /* 0x58 */ ElevatorParams m_params;
    /* 0x68 */ s32 m_state; /* 0 new, 1 off, 2 at rest, 3 at the far end, 4 returning, 5 moving
                             * (inferred) */
    /* 0x6C */ Vec3f m_start;
    /* 0x78 */ s32 m_switchId;
    /* 0x7C */ u16 m_endSound;
    /* 0x7E */ u16 m_loopSound;
    /* 0x80 */ u32 m_touchFlags; /* bit 0: a character stands on it (inferred) */
    /* 0x84 */ u32 pad_0x84;     /* the class size is a multiple of 8 */
};

} // namespace Elevator

SIZE_ASSERT(Elevator::Elevator, 0x88);

namespace Elevator {

const char *Elevator::s_fileName = "ELEVATOR.BIN";
const char *Elevator::s_taskName = "Elevator";

Elevator::Elevator(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    fn_80014E04(&m_touchFlags);
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x24, 4);
    m_chara->m_commandUnit = boost::shared_ptr<CharCommandUnit>(new CharCommandUnit());
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&Elevator::OnCommand, this, _1)));
    LoadFromOwner(m_owner);
    m_shownModel = m_params.model;
    m_model = fn_8015F800(1, s_fileName);
    m_table = fn_80012700(fn_800122D4(), 1, s_fileName);
    if (!fn_80208BB8(m_table, m_params.model)) {
        fn_8015A794(this);
        m_flags |= 1;
        return;
    }
    SetupModel();
    RebuildTransform();
}

Elevator::~Elevator() {}

void Elevator::Behave(float dt) {
    fn_8006127C(&m_touchFlags, 0);
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }
    if (m_particle.get() != 0) {
        fn_8016761C(m_particle.get(), dt);
    }

    switch (m_state) {
    case 0:
        if (m_switchId > 0) {
            m_state = 1;
        } else {
            m_state = 2;
        }
        if (m_particle.get() == 0 && m_model.get() != 0) {
            m_particle = fn_80166AF4();

            UnkParticleAttach_80169D00 attach;
            attach.ctrl = m_particle;
            fn_8015FA78(m_model.get(), fn_80169D00, &attach);
        }
        if (m_particle.get() != 0) {
            fn_80167010(m_particle.get(), -1);
        }
        break;
    case 1:
        if (fn_8016904C(fn_800CB69C(), m_switchId) == 1) {
            if (fn_80014E34(&m_touchFlags, 0)) {
                m_state = 5;
            } else {
                m_state = 2;
            }
        }
        break;
    case 3:
        if (IsPlayerAway()) {
            m_state = 4;
        }
        break;
    case 5:
        if (IsPlayerAway()) {
            m_state = 4;
            break;
        }
        if (m_params.distance < 0.0f) {
            m_chara->m_position.y -= dt * (30.0f * m_params.speed);
            float end = m_start.y + m_params.distance;
            if (m_chara->m_position.y < end) {
                m_chara->m_position.y = end;
                m_state = 3;
                fn_801D54F4(fn_80053164(), m_endSound, m_chara.get(), 0, 1);
            } else {
                fn_801D53C4(fn_80053164(), m_loopSound, 0, this, 2, 0, lbl_805F4A80);
            }
        } else {
            m_chara->m_position.y += dt * (30.0f * m_params.speed);
            float end = m_start.y + m_params.distance;
            if (m_chara->m_position.y > end) {
                m_chara->m_position.y = end;
                m_state = 3;
                fn_801D54F4(fn_80053164(), m_endSound, m_chara.get(), 0, 1);
            } else {
                fn_801D53C4(fn_80053164(), m_loopSound, 0, this, 2, 0, lbl_805F4A80);
            }
        }
        RebuildTransform();
        break;
    case 4:
        if (m_params.distance < 0.0f) {
            m_chara->m_position.y += dt * (30.0f * m_params.speed);
            if (m_chara->m_position.y > m_start.y) {
                m_chara->m_position.y = m_start.y;
                m_state = 2;
                fn_801D54F4(fn_80053164(), m_endSound, m_chara.get(), 0, 1);
            } else {
                fn_801D53C4(fn_80053164(), m_loopSound, 0, this, 2, 0, lbl_805F4A80);
            }
        } else {
            m_chara->m_position.y -= dt * (30.0f * m_params.speed);
            if (m_chara->m_position.y < m_start.y) {
                m_chara->m_position.y = m_start.y;
                m_state = 2;
                fn_801D54F4(fn_80053164(), m_endSound, m_chara.get(), 0, 1);
            } else {
                fn_801D53C4(fn_80053164(), m_loopSound, 0, this, 2, 0, lbl_805F4A80);
            }
        }
        RebuildTransform();
        break;
    }
}

void Elevator::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    s32 model = m_params.model;

    LoadFromOwner(owner);
    RebuildTransform();
    if (m_model.get() != 0) {
        if (m_params.model != model) {
            m_particle.reset();
            ReleaseModel();
            if (!fn_80208BB8(m_table, m_params.model)) {
                return;
            }
            m_shownModel = m_params.model;
            SetupModel();
        }

        Vec3f top;
        fn_800091EC(&top, &m_chara->m_position);
        top.y += m_params.distance;
        u8 segment[0x18];
        fn_802E0B58(fn_80020A20(segment, &m_chara->m_position, &top), &lbl_805F06D8);

        Vec3f position;
        fn_800091EC(&position, &m_chara->m_position);
        Vec3f rotation;
        TurnAboutX(&rotation, m_chara->m_rotation);
        fn_802E0C60(&position, &rotation, 0, 10.0f, m_params.range);
        m_model->fn_8016108C();
    }
}

void Elevator::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    fn_800C9E50(owner, &m_switchId);
    m_state = 0;
    fn_800091D0(&m_start, &m_chara->m_position);
}

void Elevator::RebuildTransform() {
    if (m_model.get() != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        m_model->fn_801633DC(&matrix);
    }
}

bool Elevator::IsPlayerAway() {
    boost::weak_ptr<CharInfo> player = fn_800A8A7C(fn_80074194(), &m_chara->m_position);

    if (boost::shared_ptr<CharInfo> info = player.lock()) {
        Vec3f a;
        fn_800091EC(&a, &info->m_position);
        Vec3f b;
        fn_800091EC(&b, &m_chara->m_position);
        b.y = 0.0f;
        a.y = 0.0f;
        Vec3f offset;
        fn_8040CD74(&offset, &a, &b);
        if (fn_8040CF2C(&offset) >= m_params.range * m_params.range) {
            return true;
        }
    }
    return false;
}

void Elevator::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0xB: /* RideCommand */
        if (m_state == 2 || m_state == 4) {
            m_state = 5;
        }
        break;
    }
}

void Elevator::SetupModel() {
    m_entry = fn_80208978(m_table, m_params.model);
    fn_801629D8(m_model.get(), fn_802087D8(m_entry.get()), 0);

    const char *collision = fn_802087BC(m_entry.get());
    if (collision != 0) {
        m_model->fn_80161F08(collision, m_chara);
        fn_800D0B64(m_model->field_0x4, boost::bind(&Elevator::OnColliDetect, this, _1));
    }

    const char *sound = fn_80208768(m_entry.get());
    m_endSound = 0xFFFF;
    m_loopSound = 0xFFFF;
    if (sound != 0) {
        char name[0x40];
        UnkSoundEntry_80208888 *entry;

        sprintf(name, "%s_end", sound);
        entry = fn_80208888(m_table, name);
        if (entry != 0) {
            m_endSound = entry->soundId;
        }
        sprintf(name, "%s_loop", sound);
        entry = fn_80208888(m_table, name);
        if (entry != 0) {
            m_loopSound = entry->soundId;
        }
    }
}

void Elevator::ReleaseModel() {
    m_model->fn_80161880();
    m_model->fn_80162804();
    m_entry.reset();
}

void Elevator::OnColliDetect(ColliDetectArgs &args) {
    if (fn_8015AC84(args)) {
        fn_80014E1C(&m_touchFlags, 0);
    }
}

} // namespace Elevator

extern "C" void fn_80147B34(void) {
    fn_8006B474("ELEVATOR.BIN", 1);
}

extern "C" void fn_80147B30(void) {}

void Elevator::Elevator::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new Elevator(parent, owner);
}

static GadgetParamDesc lbl_80529BE8[] = {
    {1, "model", 0, 0, 0x10000, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {4, "distance", 0, 0, 0, 10.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "speed", 0, 0, 0, 0.5f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "range", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80529CB0 = {
    "ELEVATOR", fn_80147B34, fn_80147B30, Elevator::Elevator::Instantiate, 0, 0, 0x1132, 1, 0, 10,
    lbl_80529BE8,
};
