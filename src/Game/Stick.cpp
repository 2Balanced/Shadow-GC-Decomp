/* Stick::Stick (class name from RTTI): the STICK gadget, a breakable model that holds a weapon. Its
 * "model" parameter picks an entry of the STICK.BIN table (the motion and collision node names and
 * the break motion, effect and sound); its "weapon" parameter is the weapon dropped when it
 * breaks. Its attack collision calls back through a boost::bind into OnAttack: a hit (unless the
 * attacker's collision is flagged) breaks it, plays the break motion and its sound, and once the
 * motion ends drops the weapon 20 units above the stick. File name inferred from the class. Method
 * names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <cstring>

#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetClassDesc.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* The height above the stick at which the weapon drops (name inferred). The original sets it in
 * the unit's static initialiser, which a constructor gives. */
struct StickDropHeight {
    StickDropHeight(float height) : m_height(height) {}

    /* 0x0 */ float m_height;
};

static StickDropHeight lbl_805EED74(20.0f);

using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;

/* A model's table of named entries in its resource file (layout from fn_80208888 and its
 * neighbours; names inferred). */
class UnkModelTable_80208888;

/* A sound entry of the table: fn_80208888 and fn_80208618 return one. */
struct UnkSoundEntry_80208888 {
    /* 0x00 */ const char *name;
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ u32 soundId; /* passed to the sound calls as 16 bits */
    /* 0x0C */ u32 field_0xC;
    /* 0x10 */ Vec3f offset;
};
SIZE_ASSERT(UnkSoundEntry_80208888, 0x1C);

/* An effect entry of the table: fn_802086C0 returns one. */
struct UnkEffectEntry_802086C0 {
    /* 0x00 */ const char *name;
    /* 0x04 */ const char *fileName;
    /* 0x08 */ u8 field_0x8[0x18 - 0x8];
    /* 0x18 */ float offset[3];
};

/* The model entry of the table for one "model" parameter (GadgetBuilder::CrashInfoCtrl, from
 * RTTI; Game/GadgetBuilder.cpp): fn_80208978 creates one, and the accessors below read the
 * names it keeps (names inferred). */
namespace GadgetBuilder {
class CrashInfoCtrl;
} // namespace GadgetBuilder

/* The motion work of a model (struct name from the RTTI of its shared_ptr control block). */
struct _stMtnPlayWork;

/* A sound playing at an object (class name unknown; layout from its constructor and assignment).
 * Its destructor does nothing; owners stop it explicitly. */
class UnkSoundHandle_801D3DC8 {
  public:
    UnkSoundHandle_801D3DC8();
    UnkSoundHandle_801D3DC8 &operator=(const UnkSoundHandle_801D3DC8 &other);
    /* Stops the sound and clears the handle (behaviour inferred). */
    void fn_801D455C();

    /* 0x00 */ u32 field_0x0;
    /* 0x04 */ u16 field_0x4;
    /* 0x06 */ u16 field_0x6;
    /* 0x08 */ u16 field_0x8;
    /* 0x0A */ u8 field_0xA;
    /* 0x0B */ u8 field_0xB;
    /* 0x0C */ Vec3f m_position;
    /* 0x18 */ u32 field_0x18;
};
SIZE_ASSERT(UnkSoundHandle_801D3DC8, 0x1C);

/* The parameters of a debris spawn (layout from its constructor at 0x8015F0B8 and the gadgets that
 * fill it; names inferred). */
struct UnkDebrisParam_8015F0B8 {
    /* Sets up the parameters at a character info's place from template index (behaviour
     * inferred). */
    UnkDebrisParam_8015F0B8(const boost::shared_ptr<CharInfo> &info, int index);

    /* 0x00 */ u8 field_0x0[0x4];
    /* 0x04 */ float position[3];
    /* 0x10 */ u8 field_0x10[0x34 - 0x10];
    /* 0x34 */ float scale;
    /* 0x38 */ u8 field_0x38[0x44 - 0x38];
    /* 0x44 */ s32 model;
    /* 0x48 */ u32 field_0x48;
    /* 0x4C */ s32 field_0x4C;
    /* 0x50 */ u32 field_0x50;
    /* 0x54 */ u8 field_0x54;
    /* 0x58 */ s32 field_0x58;
};
SIZE_ASSERT(UnkDebrisParam_8015F0B8, 0x5C);

class UnkSound_80053164;

extern "C" {
/* Loads model fileName for a gadget (behaviour inferred; arg0's meaning is unknown). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* Returns the model manager, and the table or model a file keeps in it (behaviour inferred). */
void *fn_800122D4(void);
s32 fn_80012700(void *manager, int arg1, const char *fileName);
/* Model table lookups (behaviour inferred): whether there is a model entry for id, the entry,
 * and the sound entry called name. */
bool fn_80208BB8(UnkModelTable_80208888 *table, int id);
boost::shared_ptr<GadgetBuilder::CrashInfoCtrl> fn_80208978(UnkModelTable_80208888 *table, int id);
UnkSoundEntry_80208888 *fn_80208888(UnkModelTable_80208888 *table, const char *name);
/* The model entry's motion name, collision node name, break motion name and break collision node
 * name, and its break effect and break sound entries (behaviour inferred). */
const char *fn_802087D8(GadgetBuilder::CrashInfoCtrl *entry);
const char *fn_802087BC(GadgetBuilder::CrashInfoCtrl *entry);
const char *fn_802087A0(GadgetBuilder::CrashInfoCtrl *entry);
const char *fn_80208784(GadgetBuilder::CrashInfoCtrl *entry);
UnkEffectEntry_802086C0 *fn_802086C0(GadgetBuilder::CrashInfoCtrl *entry);
UnkSoundEntry_80208888 *fn_80208618(GadgetBuilder::CrashInfoCtrl *entry);
/* GadgetModelInfo methods not declared in its header (behaviour inferred where named): select
 * the part named name, play its motion, attach a character info to a node of the part, return
 * the attack of a node or of a variant, and the motion work of a named motion. */
void fn_801629D8(GadgetModelInfo *model, const char *name, int arg2);
void fn_80160960(GadgetModelInfo *model, const char *motionName, int arg2);
void fn_801609F8(GadgetModelInfo *model, const char *motionName);
void fn_80160400(GadgetModelInfo *model, const char *motionName);
bool fn_80161AB8(GadgetModelInfo *model, const char *nodeName,
                 const boost::weak_ptr<CharInfo> &info, const char *motionName);
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int index);
boost::weak_ptr<CharaColliAttack> fn_80161334(GadgetModelInfo *model, const char *nodeName);
boost::weak_ptr<_stMtnPlayWork> fn_801601A0(GadgetModelInfo *model, const char *motionName);
/* Motion flag test and event iteration of the motion library (behaviour inferred): the iteration
 * starts at work, continues while work is null, and returns zero at the end. */
int fn_80416F28(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
int fn_80415854(_stMtnPlayWork *work, int arg1, u16 *type, void **data, int arg4);
/* A 32-bit flag word: fn_80014E04 clears it, fn_80014E1C sets bit, fn_8006127C clears bit and
 * fn_80014E34 tests bit (behaviour inferred from their bodies). */
void fn_80014E04(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
/* Starts sound id at position for owner (behaviour inferred). */
UnkSoundHandle_801D3DC8 fn_801D3E58(u16 id, const Vec3f *position, Task *owner);
/* Returns the sound manager and plays sound id at position through it (behaviour inferred). */
UnkSound_80053164 *fn_80053164(void);
void fn_801D54F4(UnkSound_80053164 *sound, u16 id, const Vec3f *position, int arg3,
                 int arg4);
/* Spawns debris (behaviour inferred). */
void fn_8015D93C(UnkDebrisParam_8015F0B8 *param);
/* Particle controller helpers (see ElecBomb.cpp; fn_80167180 starts particles, inferred). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void fn_80167180(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_8015FA78(GadgetModelInfo *model,
                 void *(*callback)(void *part, UnkParticleAttach_80169D00 *param),
                 UnkParticleAttach_80169D00 *param);
void *fn_80169D00(void *part, UnkParticleAttach_80169D00 *param);
/* Gives the attacker the reward for breaking the object (behaviour inferred). */
void fn_8020642C(const boost::shared_ptr<CharInfo> &attacker, int reward);
/* Drops weapon at position (behaviour inferred). */
void fn_8011E938(int weapon, const Vec3f *position, const Vec3f *rotation);
/* Tells the gadget's owner that the gadget is done (behaviour inferred). */
void fn_8015A794(Gadget *gadget);
/* The name of weapon number id (behaviour inferred). */
const char *fn_8005BEEC(int id);
/* vector += value (behaviour from its body). */
void fn_8040CFF8(Vec3f *result, const Vec3f *value);
}

/* vector += value through the out-of-line helper (name inferred). */
inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

namespace Stick {

/* The two parameters the owner supplies (names from the parameter table). */
struct StickParams {
    StickParams() : model(0), weapon(0) {}

    /* 0x0 */ int model;  /* the entry of the model table */
    /* 0x4 */ int weapon; /* the weapon dropped */
};
SIZE_ASSERT(StickParams, 0x8);

class Stick : public Gadget {
  public:
    Stick(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~Stick();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the stick under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position, rotation and parameters (name inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    /* Places the model at the character info's translation and rotation (name inferred). */
    void RebuildTransform();
    /* The attack collision's callback (name inferred). */
    void OnAttack(AttackCallbackParam &param);
    /* Spawns the break debris and plays the break sound of the model entry (name inferred). */
    void SpawnBreakDebris();
    /* Switches the model to the entry's break part and plays its break motion and sound; false
     * when the entry has none (name inferred). */
    bool StartBreak();

    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ int m_modelId; /* the model entry on show (inferred) */
    /* 0x44 */ StickParams m_params;
    /* 0x4C */ s32 m_state; /* 0 start, 1 standing, 2 hit, 3 breaking, 4 broken, 5 done, 6 removed
                             * (inferred) */
    /* 0x50 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particles;
    /* 0x58 */ UnkModelTable_80208888 *m_table;
    /* 0x5C */ boost::shared_ptr<GadgetBuilder::CrashInfoCtrl> m_entry;
    /* 0x64 */ float m_timer;
    /* 0x68 */ UnkSoundHandle_801D3DC8 m_sound;
    /* 0x84 */ u32 m_motionFlags; /* bit 0: the entry has a motion (inferred) */
};
SIZE_ASSERT(Stick, 0x88);

const char *Stick::s_taskName = "Stick";

Stick::Stick(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    fn_80014E04(&m_motionFlags);
    m_name = s_taskName;
    m_chara = fn_8015CB88(0x45, 4, "Stick");
    LoadFromOwner(m_owner);
    m_modelId = m_params.model;
    m_model = fn_8015F800(1, "STICK.BIN");
    m_table = (UnkModelTable_80208888 *)fn_80012700(fn_800122D4(), 1, "STICK.BIN");
    if (!fn_80208BB8(m_table, m_params.model)) {
        fn_8015A794(this);
        m_flags |= 1;
        return;
    }

    m_entry = fn_80208978(m_table, m_params.model);
    GadgetBuilder::CrashInfoCtrl *entry = m_entry.get();
    fn_801629D8(m_model.get(), fn_802087D8(entry), 0);
    const char *motionName = fn_802087D8(m_entry.get());
    const char *nodeName = fn_802087BC(m_entry.get());
    bool attached = false;
    if (m_model->fn_80160BA0(motionName)) {
        m_model->fn_8015FF60();
        fn_80160960(m_model.get(), motionName, 1);
        fn_80014E1C(&m_motionFlags, 0);
        if (fn_80161AB8(m_model.get(), nodeName, m_chara, motionName)) {
            attached = true;
        }
    } else {
        fn_8006127C(&m_motionFlags, 0);
        if (m_model->fn_80161F08(nodeName, m_chara)) {
            attached = true;
        }
    }

    if (attached) {
        boost::weak_ptr<CharaColliAttack> attack = fn_80161334(m_model.get(), nodeName);
        if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
            collision->field_0x54 = 0;
            collision->field_0x55 = 1;
            collision->m_attackCallback = boost::bind(&Stick::OnAttack, this, _1);
        }
    }
    RebuildTransform();
}

Stick::~Stick() {
    m_sound.fn_801D455C();
}

void Stick::Behave(float dt) {
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }

    switch (m_state) {
    case 0: {
        m_state = 1;
        if (m_model.get() != 0) {
            m_particles = fn_80166AF4();
            UnkParticleAttach_80169D00 attach;
            attach.ctrl = m_particles;
            fn_8015FA78(m_model.get(), fn_80169D00, &attach);
        }
        if (m_particles.get() != 0) {
            fn_80167180(m_particles.get(), -1);
        }
        UnkSoundEntry_80208888 *entry = fn_80208888(m_table, fn_802087D8(m_entry.get()));
        if (entry != 0) {
            Vec3f position(entry->offset.x, entry->offset.y, entry->offset.z);
            AddVector(position, m_chara->m_position);
            m_sound = fn_801D3E58(entry->soundId, &position, this);
        }
        break;
    }
    case 1:
        if (fn_80014E34(&m_motionFlags, 0)) {
            boost::shared_ptr<_stMtnPlayWork> work =
                fn_801601A0(m_model.get(), fn_802087D8(m_entry.get())).lock();
            if (work.get() != 0) {
                _stMtnPlayWork *iterator = work.get();
                u16 type;
                void *data;
                while (fn_80415854(iterator, 0, &type, &data, 0) != 0) {
                    if (type == 0x5001) {
                        const u32 *soundId = (const u32 *)data;
                        fn_801D54F4(fn_80053164(), *soundId, &m_chara->m_position, 0, 1);
                    }
                    iterator = 0;
                }
            }
        }
        break;
    case 2:
        SpawnBreakDebris();
        if (m_particles.get() != 0) {
            m_particles.reset();
        }
        m_sound.fn_801D455C();
        if (StartBreak()) {
            m_timer = 0.5f;
        } else {
            m_model.reset();
            fn_8015A794(this);
            m_timer = 0.5f;
            m_state = 6;
        }
        break;
    case 3: {
        boost::shared_ptr<_stMtnPlayWork> work =
            fn_801601A0(m_model.get(), fn_802087A0(m_entry.get())).lock();
        if (work.get() != 0) {
            if (fn_80416F28(work.get(), 0, 1, 1) == 0 &&
                fn_80416F28(work.get(), 0, 3, 1) != 0) {
                fn_80160400(m_model.get(), fn_802087A0(m_entry.get()));
                m_state = 4;
            }
        }
        break;
    }
    case 4:
        m_timer -= dt;
        if (m_timer <= 0.0f) {
            int weapon = m_params.weapon;
            Vec3f position;
            fn_800091EC(&position, &m_chara->m_position);
            position.y += lbl_805EED74.m_height;
            fn_8011E938(weapon, &position, &m_chara->m_rotation);
            m_state = 5;
        }
        break;
    case 6:
        m_timer -= dt;
        if (m_timer <= 0.0f) {
            m_flags |= 1;
            int weapon = m_params.weapon;
            Vec3f position;
            fn_800091EC(&position, &m_chara->m_position);
            position.y += lbl_805EED74.m_height;
            fn_8011E938(weapon, &position, &m_chara->m_rotation);
        }
        break;
    }

    RebuildTransform();
}

void Stick::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    int model = m_params.model;

    LoadFromOwner(owner);
    RebuildTransform();
    if (m_model.get() != 0) {
        if (m_params.model != model) {
            m_model->fn_80161880();
            m_model->fn_80162804();
            m_entry.reset();
            m_sound.fn_801D455C();
            if (!fn_80208BB8(m_table, m_params.model)) {
                return;
            }
            m_entry = fn_80208978(m_table, m_params.model);
            m_modelId = m_params.model;
            fn_801629D8(m_model.get(), fn_802087D8(m_entry.get()), 0);
            if (m_model->fn_80161F08(fn_802087BC(m_entry.get()), m_chara)) {
                boost::weak_ptr<CharaColliAttack> attack = fn_80161428(m_model.get(), m_modelId);
                if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
                    collision->field_0x54 = 0;
                    collision->field_0x55 = 1;
                    collision->m_attackCallback = boost::bind(&Stick::OnAttack, this, _1);
                }
            }
        }
        m_model->fn_8016108C();
    }
}

void Stick::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_state = 0;
    m_timer = 0.0f;
}

void Stick::RebuildTransform() {
    if (m_model.get() != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B884(&matrix, m_chara->m_rotation.x);
        fn_8040B80C(&matrix, m_chara->m_rotation.z);
        m_model->fn_801633DC(&matrix);
    }
}

void Stick::OnAttack(AttackCallbackParam &param) {
    if (m_state == 1) {
        boost::weak_ptr<CharInfo> attacker;
        if (boost::shared_ptr<CharaColliAttack> attack = param.m_attacker.lock()) {
            if (attack->m_flags & 2) {
                param.m_result = 5;
                return;
            }
            attacker = attack->m_owner;
        }
        if (boost::shared_ptr<CharInfo> hit = attacker.lock()) {
            fn_8020642C(hit, 0x23);
        }
        param.m_result = 3;
        m_state = 2;
    } else {
        param.m_result = 5;
    }
}

void Stick::SpawnBreakDebris() {
    if (m_entry.get() != 0) {
        UnkEffectEntry_802086C0 *effect = fn_802086C0(m_entry.get());
        if (effect != 0) {
            s32 model = fn_80012700(fn_800122D4(), 1, effect->fileName);
            if (model != 0) {
                UnkDebrisParam_8015F0B8 debris(m_chara, 0);

                debris.position[0] += effect->offset[0];
                debris.position[1] += effect->offset[1];
                debris.position[2] += effect->offset[2];
                debris.scale = 1.3f;
                debris.model = model;
                debris.field_0x54 = 1;
                debris.field_0x4C = 150;
                fn_8015D93C(&debris);
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

bool Stick::StartBreak() {
    if (m_entry.get() != 0 && fn_802087A0(m_entry.get()) != 0) {
        m_model->fn_80161880();
        m_model->fn_80162804();
        fn_801629D8(m_model.get(), fn_802087A0(m_entry.get()), 0);
        RebuildTransform();
        if (m_model->fn_80160BA0(fn_802087A0(m_entry.get()))) {
            m_model->fn_8015FF60();
            fn_80161AB8(m_model.get(), fn_80208784(m_entry.get()), m_chara,
                        fn_802087A0(m_entry.get()));
            fn_801609F8(m_model.get(), fn_802087A0(m_entry.get()));
            RebuildTransform();
            m_state = 3;
        } else if (fn_80208784(m_entry.get()) != 0) {
            m_model->fn_80161F08(fn_80208784(m_entry.get()), m_chara);
            m_state = 4;
        } else {
            m_state = 4;
        }

        UnkSoundEntry_80208888 *entry = fn_80208888(m_table, fn_802087A0(m_entry.get()));
        if (entry != 0) {
            Vec3f position(entry->offset.x, entry->offset.y, entry->offset.z);
            AddVector(position, m_chara->m_position);
            m_sound = fn_801D3E58(entry->soundId, &position, this);
        }
        return true;
    }
    return false;
}

} // namespace Stick

extern "C" void fn_80121B04(void) {
    fn_8006B474("STICK.BIN", 1);
}

extern "C" void fn_80121B00(void) {}

void Stick::Stick::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new Stick(fn_801EB304(fn_8001C4F8(), 11), owner);
}

/* The fields of the cursor that the label callback reads (layout from its accesses). */
struct StickParamCursor {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ int index; /* the parameter */
    /* 0x8 */ int value;
};

/* The label callback of the weapon parameter: the weapon's name, or "Not Used" for none. */
extern "C" const char *fn_80121A2C(UnkLabelQuery_800CBC20 *cursor) {
    static const char *s_notUsed = "Not Used";
    const StickParamCursor *fields = (const StickParamCursor *)cursor;

    if (fields->index == 1) {
        const char *name = fn_8005BEEC(fields->value);
        if (std::strcmp(name, "") == 0) {
            return s_notUsed;
        }
        return name;
    }
    return 0;
}

static GadgetParamDesc lbl_80527998[] = {
    {1, "model", 0, 0, 0x10000, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "weapon", 1, 1, 0x47, 0.0f, 0.0f, 0.0f, fn_80121A2C, 0},
    {0},
};

GadgetClassDesc lbl_80527A10 = {
    "STICK", fn_80121B04, fn_80121B00, Stick::Stick::Instantiate, 0, 0, 0x12C, 1, 0, 10, lbl_80527998,
};
