/* BreakWall::BreakWall (class name from RTTI): the BREAKWALL gadget, a breakable wall shown with
 * the "model" entry of BREAKWALL.BIN. Attacks whose flags have bit 4, 6 or 9 set take their damage
 * off its 3 hit points: the first hit (or a ride command) swaps in the entry's "<name>_crack" model
 * with its particles and sound, and once the hit points run out (or a command of type 0xA names the
 * attacker) the wall spawns the entry's debris, plays its break sound, rewards the attacker and
 * shows the entry's broken model, or finishes when the entry has none. File name inferred from the
 * class. Method names marked "(name inferred)" are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */

/* Keeps the dead copy of bad_alloc's vtable in .data, as retail has it (see <new>). */
#define MSL_NEW_INLINE_BAD_ALLOC

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <cstdio>

#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/FileQueue.h"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;

/* The model entry of the table for one "model" parameter (GadgetBuilder::CrashInfoCtrl, from
 * RTTI; Game/GadgetBuilder.cpp). */
namespace GadgetBuilder {
class CrashInfoCtrl;
} // namespace GadgetBuilder

/* A model's table of named entries in its resource file (name as in Elevator). */
class UnkModelTable_80208888;

/* A model entry of the table: fn_80208900 finds it by name (only the model index is known). */
struct UnkModelPartEntry_80208900 {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ u32 field_0x4;
    /* 0x8 */ s32 model; /* passed to GadgetModelInfo::fn_80162A68 */
};

/* A sound entry of the table: fn_80208888 finds it by name (layout as in Stick and CircusBall). */
struct UnkSoundEntry_80208888 {
    /* 0x00 */ const char *name;
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ u32 soundId; /* passed to the sound calls as 16 bits */
    /* 0x0C */ u32 field_0xC;
    /* 0x10 */ Vec3f offset;
};
SIZE_ASSERT(UnkSoundEntry_80208888, 0x1C);

/* The debris of a model entry, from fn_802086C0 (names inferred). */
struct UnkDebrisEntry_802086C0 {
    /* 0x00 */ u32 field_0x0;
    /* 0x04 */ const char *fileName; /* the debris model file */
    /* 0x08 */ u8 field_0x8[0x18 - 0x8];
    /* 0x18 */ Vec3f offset;
};

/* The parameters of a debris spawn (layout from its constructor at 0x8015F0B8 and the users in
 * BreakObj and here; names inferred). */
struct UnkDebrisParam_8015F0B8 {
    /* Sets up the parameters at a character info's place from template index (behaviour
     * inferred). */
    UnkDebrisParam_8015F0B8(const boost::shared_ptr<CharInfo> &info, int index);

    /* 0x00 */ u32 field_0x0;
    /* 0x04 */ Vec3f position;
    /* 0x10 */ u8 field_0x10[0x34 - 0x10];
    /* 0x34 */ float scale;
    /* 0x38 */ u8 field_0x38[0x44 - 0x38];
    /* 0x44 */ s32 model;
    /* 0x48 */ u32 field_0x48;
    /* 0x4C */ s32 count; /* (meaning inferred) */
    /* 0x50 */ u32 field_0x50;
    /* 0x54 */ u8 field_0x54;
    /* 0x55 */ u8 field_0x55[0x5C - 0x55];
};
SIZE_ASSERT(UnkDebrisParam_8015F0B8, 0x5C);

/* WallJumpCommand (command type 0xA) is declared in Game/CharCommand.h. */

extern "C" {
/* Loads the model file fileName (behaviour inferred; arg0 unknown). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* Returns the file loader, and the table that file fileName keeps in it (behaviour inferred). */
void *fn_800122D4(void);
UnkModelTable_80208888 *fn_80012700(void *loader, int arg1, const char *fileName);
/* Builds the entry for model of the table, reports whether the table holds that entry, and
 * returns the model or sound entry named name or 0 (behaviour inferred). */
boost::shared_ptr<GadgetBuilder::CrashInfoCtrl> fn_80208978(UnkModelTable_80208888 *table, int model);
bool fn_80208BB8(UnkModelTable_80208888 *table, int model);
UnkModelPartEntry_80208900 *fn_80208900(UnkModelTable_80208888 *table, const char *name);
UnkSoundEntry_80208888 *fn_80208888(UnkModelTable_80208888 *table, const char *name);
/* Accessors of the entry, each 0 when the entry is empty (meanings inferred from their callers):
 * the name of the model to show and of its collision node, the same two for the broken wall, the
 * debris and the break sound. */
const char *fn_802087D8(GadgetBuilder::CrashInfoCtrl *entry);
const char *fn_802087BC(GadgetBuilder::CrashInfoCtrl *entry);
const char *fn_802087A0(GadgetBuilder::CrashInfoCtrl *entry);
const char *fn_80208784(GadgetBuilder::CrashInfoCtrl *entry);
UnkDebrisEntry_802086C0 *fn_802086C0(GadgetBuilder::CrashInfoCtrl *entry);
UnkSoundEntry_80208888 *fn_80208618(GadgetBuilder::CrashInfoCtrl *entry);

/* Shows the model named name of the model file (behaviour inferred; arg2 unknown). */
void fn_801629D8(GadgetModelInfo *model, const char *name, int arg2);
/* Returns the attack collision of the model's collision node name (behaviour inferred). */
boost::weak_ptr<CharaColliAttack> fn_80161334(GadgetModelInfo *model, const char *name);
/* Calls callback with param for each part of the model (behaviour inferred). */
void fn_8015FA78(GadgetModelInfo *model,
                 void *(*callback)(void *part, UnkParticleAttach_80169D00 *param),
                 UnkParticleAttach_80169D00 *param);
void *fn_80169D00(void *part, UnkParticleAttach_80169D00 *param);
/* Creates a particle controller, one for a model part, advances it by dt, and starts its emitters
 * (behaviour inferred). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_8016690C(void *part);
void fn_8016761C(GadgetParticle::GadgetParticleCtrl *ctrl, float dt);
void fn_80167010(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_80167448(GadgetParticle::GadgetParticleCtrl *ctrl, int id);

/* Spawns debris (behaviour inferred). */
void fn_8015D93C(UnkDebrisParam_8015F0B8 *param);
/* Plays sound id at position (behaviour inferred; the other arguments are unknown). */
void fn_801D54F4(void *sound, u16 id, const Vec3f *position, int arg3, int arg4);
/* Gives the attacker the reward for breaking the object (behaviour inferred). */
void fn_8020642C(const boost::shared_ptr<CharInfo> &attacker, int reward);
/* Marks the gadget finished (behaviour inferred). */
void fn_8015A794(Gadget *gadget);
}

namespace BreakWall {

/* The parameter the owner supplies (name from the parameter table). */
struct BreakWallParams {
    BreakWallParams() : model(0) {}

    /* 0x0 */ s32 model;
};
SIZE_ASSERT(BreakWallParams, 0x4);

class BreakWall : public Gadget {
  public:
    BreakWall(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~BreakWall();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the wall under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position, rotation and parameter, and restores the hit points (name
     * inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    /* Places the model at the character info's translation, turned about the Y axis (name
     * inferred). */
    void RebuildTransform();
    /* The command unit's callback (name inferred). */
    void OnCommand(CharCommand &command);
    /* The attack callback of the model's collision (name inferred). */
    void OnAttack(AttackCallbackParam &param);
    /* Spawns the entry's debris and plays its break sound (name inferred). */
    void Break();
    /* Shows the entry's broken model; false when the entry has none (name inferred). */
    bool ShowBroken();
    /* Shows the entry's "<name>_crack" model with its particles and sound; false when the table has
     * no such model (name inferred). */
    bool Crack();

    static const char *s_fileName; /* (name inferred) */
    static const char *s_crackSuffix; /* (name inferred) */
    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particle;
    /* 0x48 */ boost::weak_ptr<CharInfo> m_attacker; /* who broke the wall (inferred) */
    /* 0x50 */ UnkModelTable_80208888 *m_table;    /* the table of BREAKWALL.BIN */
    /* 0x54 */ boost::shared_ptr<GadgetBuilder::CrashInfoCtrl> m_entry;
    /* 0x5C */ BreakWallParams m_params;
    /* 0x60 */ s32 m_state; /* 0 new, 1 intact, 2 cracked, 3 breaking, 4 broken (inferred) */
    /* 0x64 */ s32 m_shownModel;
    /* 0x68 */ float m_hitPoints;
    /* 0x6C */ u32 pad_0x6C; /* the class size is a multiple of 8 */
};

} // namespace BreakWall

SIZE_ASSERT(BreakWall::BreakWall, 0x70);

namespace BreakWall {

const char *BreakWall::s_fileName = "BREAKWALL.BIN";
const char *BreakWall::s_crackSuffix = "_crack";
const char *BreakWall::s_taskName = "BreakWall";

/* Starts the particles of a model part (name inferred). Retail emits it after the constructor and
 * its template instances, so it is defined before the constructor. */
static void *StartPartParticle(void *part, UnkParticleAttach_80169D00 *param) {
    void *node = *static_cast<void **>(part);

    if (node != 0) {
        boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> ctrl = fn_8016690C(node);
        fn_80167448(ctrl.get(), -1);
    }
    return part;
}

BreakWall::BreakWall(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_chara = fn_8015CD90(0x23, 4);
    m_chara->m_commandUnit = boost::shared_ptr<CharCommandUnit>(new CharCommandUnit());
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&BreakWall::OnCommand, this, _1)));
    m_shownModel = m_params.model;
    m_model = fn_8015F800(1, s_fileName);
    m_table = fn_80012700(fn_800122D4(), 1, s_fileName);
    if (!fn_80208BB8(m_table, m_params.model)) {
        fn_8015A794(this);
        m_flags |= 1;
        return;
    }

    /* Retail destroys the attack handle before LoadFromOwner, so these lines form a block. */
    {
        m_entry = fn_80208978(m_table, m_params.model);
        GadgetBuilder::CrashInfoCtrl *entry = m_entry.get();
        fn_801629D8(m_model.get(), fn_802087D8(entry), 0);
        const char *collision = fn_802087BC(m_entry.get());
        m_model->fn_80161F08(collision, m_chara);
        boost::weak_ptr<CharaColliAttack> attack = fn_80161334(m_model.get(), collision);
        if (boost::shared_ptr<CharaColliAttack> target = attack.lock()) {
            target->field_0x54 = 0;
            target->field_0x55 = 1;
            target->m_attackCallback = AttackCallback(boost::bind(&BreakWall::OnAttack, this, _1));
        }
    }

    LoadFromOwner(owner);
    RebuildTransform();
}

BreakWall::~BreakWall() {}

void BreakWall::Behave(float dt) {
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }
    if (m_particle.get() != 0) {
        fn_8016761C(m_particle.get(), dt);
    }

    switch (m_state) {
    case 0:
        if (m_model.get() != 0) {
            m_particle = fn_80166AF4();

            UnkParticleAttach_80169D00 attach;
            attach.ctrl = m_particle;
            fn_8015FA78(m_model.get(), fn_80169D00, &attach);
        }
        if (m_particle.get() != 0) {
            fn_80167010(m_particle.get(), -1);
        }
        m_state = 1;
        break;
    case 3:
        Break();
        if (boost::shared_ptr<CharInfo> attacker = m_attacker.lock()) {
            fn_8020642C(attacker, 0x23);
        }
        if (!ShowBroken()) {
            fn_8015A794(this);
            m_flags |= 1;
        }
        break;
    }
}

void BreakWall::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    if (m_model.get() != 0) {
        if (m_params.model != m_shownModel) {
            m_particle.reset();
            m_model->fn_80161880();
            m_model->fn_80162804();
            m_entry.reset();
            if (!fn_80208BB8(m_table, m_params.model)) {
                return;
            }

            m_entry = fn_80208978(m_table, m_params.model);
            m_shownModel = m_params.model;
            fn_801629D8(m_model.get(), fn_802087D8(m_entry.get()), 0);
            const char *collision = fn_802087BC(m_entry.get());
            m_model->fn_80161F08(collision, m_chara);
            boost::weak_ptr<CharaColliAttack> attack = fn_80161334(m_model.get(), collision);
            if (boost::shared_ptr<CharaColliAttack> target = attack.lock()) {
                target->field_0x54 = 0;
                target->field_0x55 = 1;
                target->m_attackCallback =
                    AttackCallback(boost::bind(&BreakWall::OnAttack, this, _1));
            }
        }
        RebuildTransform();
        m_model->fn_8016108C();
    }
}

void BreakWall::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_state = 0;
    m_hitPoints = 3.0f;
}

void BreakWall::RebuildTransform() {
    if (m_model.get() != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        m_model->fn_801633DC(&matrix);
    }
}

void BreakWall::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0xB: /* RideCommand */
        if (m_state == 1) {
            Crack();
            m_state = 2;
        }
        break;
    case 0xA:
        if (m_state == 2) {
            m_attacker = static_cast<WallJumpCommand &>(command).m_character;
            m_state = 3;
        }
        break;
    }
}

void BreakWall::OnAttack(AttackCallbackParam &param) {
    if (m_state == 1 || m_state == 2) {
        boost::weak_ptr<CharInfo> attacker;

        if (boost::shared_ptr<CharaColliAttack> collision = param.m_attacker.lock()) {
            if ((collision->m_flags & 0x250) == 0) {
                param.m_result = 5;
                return;
            }
            attacker = collision->m_owner;
        }

        m_hitPoints -= param.m_damage;
        if (m_hitPoints <= 0.0f) {
            m_state = 3;
            param.m_result = 3;
            m_attacker = attacker;
        } else {
            if (m_state == 1) {
                Crack();
                m_state = 2;
            }
            param.m_result = 1;
        }
    } else {
        param.m_result = 5;
    }
}

void BreakWall::Break() {
    if (m_entry.get() != 0) {
        UnkDebrisEntry_802086C0 *debris = fn_802086C0(m_entry.get());
        if (debris != 0) {
            s32 model = (s32)fn_80012700(fn_800122D4(), 1, debris->fileName);
            if (model != 0) {
                UnkDebrisParam_8015F0B8 param(m_chara, 0);

                param.position.x += debris->offset.x;
                param.position.y += debris->offset.y;
                param.position.z += debris->offset.z;
                param.scale = 1.3f;
                param.model = model;
                param.field_0x54 = 1;
                param.count = 150;
                fn_8015D93C(&param);
            }
        }

        UnkSoundEntry_80208888 *sound = fn_80208618(m_entry.get());
        if (sound != 0) {
            Vec3f position;
            fn_800091EC(&position, &m_chara->m_position);
            position.x += sound->offset.x;
            position.y += sound->offset.y;
            position.z += sound->offset.z;
            fn_801D54F4(fn_80053164(), sound->soundId, &position, 0, 1);
        }
    }
}

bool BreakWall::ShowBroken() {
    if (m_entry.get() != 0 && fn_802087A0(m_entry.get()) != 0) {
        m_particle.reset();
        m_model->fn_80161880();
        m_model->fn_80162804();
        fn_801629D8(m_model.get(), fn_802087A0(m_entry.get()), 0);
        RebuildTransform();
        if (fn_80208784(m_entry.get()) != 0) {
            m_model->fn_80161F08(fn_80208784(m_entry.get()), m_chara);
        }
        m_state = 4;
        return true;
    }
    return false;
}

bool BreakWall::Crack() {
    if (m_model.get() == 0) {
        return false;
    }

    m_particle.reset();
    bool cracked = false;
    char name[0x40];
    sprintf(name, "%s%s", fn_802087D8(m_entry.get()), s_crackSuffix);
    UnkModelPartEntry_80208900 *entry = fn_80208900(m_table, name);
    if (entry != 0) {
        m_model->fn_80162804();
        m_model->fn_80162A68(entry->model, 0);
        RebuildTransform();
        fn_8015FA78(m_model.get(), StartPartParticle, 0);
        cracked = true;
    }

    if (cracked) {
        UnkSoundEntry_80208888 *sound = fn_80208888(m_table, name);
        if (sound != 0) {
            Vec3f position;
            fn_800091EC(&position, &m_chara->m_position);
            position.x += sound->offset.x;
            position.y += sound->offset.y;
            position.z += sound->offset.z;
            fn_801D54F4(fn_80053164(), sound->soundId, &position, 0, 1);
        }
    }
    return cracked;
}

} // namespace BreakWall

extern "C" void fn_801402D4(void) {
    fn_8006B474("BREAKWALL.BIN", 1);
}

extern "C" void fn_801402D0(void) {}

void BreakWall::BreakWall::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new BreakWall(parent, owner);
}

static GadgetParamDesc lbl_805294E4[] = {
    {1, "model", 0, 0, 0x10000, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80529534 = {
    "BREAKWALL", fn_801402D4, fn_801402D0, BreakWall::BreakWall::Instantiate, 0, 0, 0x1135, 1, 0, 10,
    lbl_805294E4,
};
