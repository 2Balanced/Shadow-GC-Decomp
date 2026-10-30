/* SuperComputer::SetSuperComputer (class name from RTTI): the SUPER COMPUTER gadget, a computer
 * (SUPERCOMPUTER.BIN, model entry 0 of its crash table) with fifty hit points that sparks harder at
 * 37.5, 25 and 12.5 and breaks into its debris at none. Once destroyed it counts towards the
 * player's mission, turns its owner's switch on and stops its loop sound. It carries a target and a
 * positioned sound. File name inferred from the namespace. Names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetDebris.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetUtil.h"
#include "Game/Mission.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/FileQueue.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;

namespace GadgetBuilder {
/* GadgetBuilder::CrashInfoCtrl (class name from RTTI; Game/GadgetBuilder.cpp): the entry of a
 * model file's crash table. */
class CrashInfoCtrl;
} // namespace GadgetBuilder

/* The table a model file keeps in the loader (struct name unknown; see Game/BreakWall.cpp). */
class UnkModelTable_80208888;

/* The debris of a crash table entry, from fn_802086C0 (layout from Game/BreakWall.cpp). */
struct UnkDebrisEntry_802086C0 {
    /* 0x00 */ u32 field_0x0;
    /* 0x04 */ const char *fileName; /* the debris model file */
    /* 0x08 */ u8 field_0x8[0x18 - 0x8];
    /* 0x18 */ Vec3f offset;
};

/* Target (class name from RTTI): a point characters aim at (see Game/BAGunShip.cpp). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);
};

/* The positioned object fn_802B4254 creates (see Game/DefenseSystem.cpp; it may be a sound
 * source). */
class UnkSound_802B4254;
class UnkPlayerManager_8006D6A8;

extern "C" {
/* The file loader, the table file fileName keeps in it, the crash table entry of a model and the
 * entry's accessors (see Game/BreakWall.cpp). */
void *fn_800122D4(void);
UnkModelTable_80208888 *fn_80012700(void *loader, int arg1, const char *fileName);
boost::shared_ptr<GadgetBuilder::CrashInfoCtrl> fn_80208978(UnkModelTable_80208888 *table, int model);
const char *fn_802087D8(GadgetBuilder::CrashInfoCtrl *entry);
const char *fn_802087BC(GadgetBuilder::CrashInfoCtrl *entry);
const char *fn_802087A0(GadgetBuilder::CrashInfoCtrl *entry);
UnkDebrisEntry_802086C0 *fn_802086C0(GadgetBuilder::CrashInfoCtrl *entry);

/* GadgetModelInfo helpers (see Game/BreakWall.cpp). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
void fn_801629D8(GadgetModelInfo *model, const char *name, int arg2);
boost::weak_ptr<CharaColliAttack> fn_80161334(GadgetModelInfo *model, const char *name);

/* Particle helpers (see Game/BaseDoor.cpp and Game/DefenseSystem.cpp). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void fn_80166E20(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_80167180(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_8015FA78(GadgetModelInfo *model, void *(*callback)(void *part, UnkParticleAttach_80169D00 *param),
                 UnkParticleAttach_80169D00 *param);
void *fn_80169D00(void *part, UnkParticleAttach_80169D00 *param);

/* The positioned sound (see Game/DefenseSystem.cpp). */
boost::shared_ptr<UnkSound_802B4254> fn_802B4254(int kind, int arg1);
void fn_802B4510(UnkSound_802B4254 *sound, u8 flag);
void fn_802B44C8(UnkSound_802B4254 *sound, const Vec3f *position);

/* Creates a target and removes it (see Game/BAGunShip.cpp). */
boost::shared_ptr<Target> fn_801E72DC(int flags);
void fn_801E7108(boost::shared_ptr<Target> *target);

/* The 32-bit flag word (see Game/BaseDoor.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* Plays sound id at a character info, once and as a loop for source (see Game/BaseDoor.cpp). */
void fn_801D54F4(void *manager, int id, GadgetCharInfo *source, int arg3, int arg4);
void fn_801D53C4(void *manager, int id, GadgetCharInfo *charInfo, void *source, int arg4, int arg5,
                 int arg6);

/* Gives the attacker the reward for breaking the object (see Game/BreakWall.cpp). */
void fn_8020642C(const boost::shared_ptr<CharInfo> &attacker, int reward);

/* Turns stage switch id on (behaviour inferred; see Game/DefenseSystem.cpp). */
void fn_8016907C(UnkSwitchBank_800CB69C *bank, int id, int on);
/* The switch id the owner record was placed with (see Game/MagmaGuard.cpp). */
void fn_800C9E50(UnkPlacement_800C9CD0 *owner, s32 *id);
/* Records the gadget as destroyed (behaviour inferred; see Game/SecretDisk.cpp). */
void fn_8015A768(Gadget *gadget);

/* The player manager and a player's mission (see Game/BASwitch.cpp). */
UnkPlayerManager_8006D6A8 *fn_8006D6A8(void);
boost::weak_ptr<Mission::Mission> fn_800740E4(UnkPlayerManager_8006D6A8 *manager, int index);
}

/* The heights of the loop sound and of the target above the computer (names inferred; the game
 * sets them in __sinit; the inline call reproduces that, as in Game/CameraModeFixLook2.cpp). */
inline float DynamicFloat(float value) {
    return value;
}
static const float s_soundHeight = DynamicFloat(100.0f);
static const float s_targetHeight = DynamicFloat(50.0f);

namespace SuperComputer {

/* The flag word at 0x70 (struct name unknown; bit 0: destroyed, bit 1: sparking; meanings
 * inferred). */
struct SetSuperComputerFlags {
    SetSuperComputerFlags() {
        fn_80014E04(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    void Clear(int bit) {
        fn_8006127C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits;
};

class SetSuperComputer : public Gadget {
  public:
    SetSuperComputer(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~SetSuperComputer();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Method names are inferred. */
    /* Copies the owner's position and rotation and resets the state. */
    void LoadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model, the loop sound and the target. */
    void RebuildTransform();
    /* The attack collider's callback: takes the damage. */
    void OnAttack(AttackCallbackParam &param);
    /* Shows damage stage 0 to 2 with more sparks, or breaks the computer at stage 3. */
    void SetDamageStage(int stage);
    /* Throws the debris of the model's crash table entry. */
    void Break();

    /* Whether attacks can hit the computer. */
    bool CanBeHit() {
        if (m_state == 1 || m_state == 2 || m_state == 3 || m_state == 4) {
            return true;
        }
        return false;
    }

    static const char *s_fileName;
    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particle;
    /* 0x48 */ boost::shared_ptr<UnkSound_802B4254> m_sound;
    /* 0x50 */ boost::shared_ptr<Target> m_target;
    /* 0x58 */ UnkModelTable_80208888 *m_table;
    /* 0x5C */ boost::shared_ptr<GadgetBuilder::CrashInfoCtrl> m_entry;
    /* 0x64 */ s32 m_state; /* 0 placed, 1-4 standing (damage stages), 5 destroyed, 6 done */
    /* 0x68 */ float m_hitPoints;
    /* 0x6C */ float m_hitTimer; /* attacks are ignored while it runs */
    /* 0x70 */ SetSuperComputerFlags m_flags;
    /* 0x74 */ u32 field_0x74;
};
SIZE_ASSERT(SetSuperComputer, 0x78);

const char *SetSuperComputer::s_fileName = "SUPERCOMPUTER.BIN";
const char *SetSuperComputer::s_taskName = "SetSuperComputer";

SetSuperComputer::SetSuperComputer(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x79, 4);
    LoadPlacement(owner);
    m_model = fn_8015F800(1, s_fileName);
    m_table = fn_80012700(fn_800122D4(), 1, s_fileName);
    m_entry = fn_80208978(m_table, 0);
    /* Retail loads the entry before the model in each of these three calls. */
    GadgetBuilder::CrashInfoCtrl *entry = m_entry.get();
    fn_801629D8(m_model.get(), fn_802087D8(entry), 0);
    (entry = m_entry.get(), m_model)->fn_80161F08(fn_802087BC(entry), m_chara);
    entry = m_entry.get();
    boost::weak_ptr<CharaColliAttack> attack = fn_80161334(m_model.get(), fn_802087BC(entry));
    if (boost::shared_ptr<CharaColliAttack> target = attack.lock()) {
        target->field_0x54 = 0;
        target->field_0x55 = 1;
        target->m_attackCallback = AttackCallback(boost::bind(&SetSuperComputer::OnAttack, this, _1));
    }
    m_sound = fn_802B4254(0, 0);
    fn_802B4510(m_sound.px, 1);
    m_target = fn_801E72DC(0x800);
    RebuildTransform();
}

SetSuperComputer::~SetSuperComputer() {
    fn_801E7108(&m_target);
}

void SetSuperComputer::Behave(float dt) {
    if (m_model.px != 0) {
        m_model->fn_80162FB0(dt);
    }
    m_hitTimer -= dt;
    if (m_hitTimer <= 0.0f) {
        m_hitTimer = 0.0f;
    }
    if (m_flags.Test(1)) {
        fn_801D53C4(fn_80053164(), 0x7845, m_chara.px, this, 3, 0, 2);
    }
    if (!m_flags.Test(0)) {
        fn_801D53C4(fn_80053164(), 0x785F, m_chara.px, this, 3, 0, 2);
    }
    switch (m_state) {
    case 0:
        m_state = 1;
        break;
    case 1:
        if (m_hitPoints < 37.5f) {
            SetDamageStage(0);
            m_state = 2;
        }
        break;
    case 2:
        if (m_hitPoints < 25.0f) {
            SetDamageStage(1);
            m_state = 3;
        }
        break;
    case 3:
        if (m_hitPoints < 12.5f) {
            SetDamageStage(2);
            m_state = 4;
        }
        break;
    case 4:
        if (m_hitPoints <= 0.0f) {
            SetDamageStage(3);
            m_state = 5;
        }
        break;
    case 5: {
        {
            boost::shared_ptr<Mission::Mission> player = fn_800740E4(fn_8006D6A8(), 0).lock();
            if (player.px != 0) {
                player->AddCount(1, 0);
            }
        }
        s32 switchId;
        fn_800C9E50(m_owner, &switchId);
        fn_8016907C(fn_800CB69C(), switchId, 1);
        fn_8015A768(this);
        m_state = 6;
        m_flags.Clear(1);
        m_flags.Set(0);
        fn_801E7108(&m_target);
        break;
    }
    case 6:
        break;
    }
    RebuildTransform();
}

void SetSuperComputer::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadPlacement(owner);
    RebuildTransform();
}

void SetSuperComputer::LoadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    m_state = 0;
    m_hitPoints = 50.0f;
    m_hitTimer = 0.0f;
}

void SetSuperComputer::RebuildTransform() {
    if (m_model.px != 0) {
        Mtx34f matrix;
        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        m_model->fn_801633DC(&matrix);
    }
    if (m_sound.px != 0) {
        Vec3f position;
        fn_800091EC(&position, &m_chara->m_position);
        position.y += s_soundHeight;
        fn_802B44C8(m_sound.px, &position);
    }
    if (m_target.px != 0) {
        Vec3f position;
        fn_800091EC(&position, &m_chara->m_position);
        position.y += s_targetHeight;
        m_target->fn_800CCE38(&position);
    }
}

void SetSuperComputer::OnAttack(AttackCallbackParam &param) {
    param.m_result = 5;
    if (m_hitTimer > 0.0f) {
        return;
    }
    if (!CanBeHit()) {
        return;
    }
    if (boost::shared_ptr<CharaColliAttack> attack = param.m_attacker.lock()) {
        if ((attack->m_flags & 0x3D0) == 0) {
            return;
        }
        m_hitPoints -= param.m_damage;
        m_hitTimer = 0.2f;
        if (m_hitPoints <= 0.0f) {
            if (boost::shared_ptr<CharInfo> attacker = attack->m_owner.lock()) {
                fn_8020642C(attacker, 0);
            }
            param.m_result = 3;
        } else {
            fn_801D54F4(fn_80053164(), 0x7841, m_chara.px, 0, 1);
            param.m_result = 1;
        }
    }
}

void SetSuperComputer::SetDamageStage(int stage) {
    switch (stage) {
    case 0:
        if (m_model.px != 0) {
            m_particle = fn_80166AF4();
            UnkParticleAttach_80169D00 attach;
            attach.ctrl = m_particle;
            fn_8015FA78(m_model.px, fn_80169D00, &attach);
        }
        if (m_particle.px != 0) {
            fn_80167180(m_particle.px, 0x19);
        }
        fn_801D54F4(fn_80053164(), 0x7843, m_chara.px, 0, 1);
        break;
    case 1:
        if (m_particle.px != 0) {
            fn_80166E20(m_particle.px, 0x19);
            fn_80167180(m_particle.px, 0x32);
        }
        fn_801D54F4(fn_80053164(), 0x7843, m_chara.px, 0, 1);
        break;
    case 2:
        if (m_particle.px != 0) {
            fn_80166E20(m_particle.px, 0x32);
            fn_80167180(m_particle.px, 0x4B);
        }
        fn_801D54F4(fn_80053164(), 0x7843, m_chara.px, 0, 1);
        break;
    case 3:
        Break();
        m_particle.reset();
        m_model->fn_80161880();
        m_model->fn_80162804();
        GadgetModelInfo *model = m_model.px;
        fn_801629D8(model, fn_802087A0(m_entry.px), 0);
        if (m_model.px != 0) {
            m_particle = fn_80166AF4();
            UnkParticleAttach_80169D00 attach;
            attach.ctrl = m_particle;
            fn_8015FA78(m_model.px, fn_80169D00, &attach);
        }
        if (m_particle.px != 0) {
            fn_80167180(m_particle.px, -1);
        }
        RebuildTransform();
        fn_801D54F4(fn_80053164(), 0x7843, m_chara.px, 0, 1);
        break;
    }
    m_flags.Set(1);
}

void SetSuperComputer::Break() {
    if (m_entry.px != 0) {
        UnkDebrisEntry_802086C0 *debris = fn_802086C0(m_entry.px);
        if (debris != 0) {
            s32 model = (s32)fn_80012700(fn_800122D4(), 1, debris->fileName);
            if (model != 0) {
                UnkDebrisParam_8015F0B8 param(m_chara, 0);
                Vec3f *position = (Vec3f *)&param.field_0x0[4];
                position->x += debris->offset.x;
                position->y += debris->offset.y;
                position->z += debris->offset.z;
                param.scale = 1.3f;
                param.model = model;
                param.field_0x54 = 1;
                param.field_0x4C = 150;
                fn_8015D93C(&param);
            }
        }
    }
}

} // namespace SuperComputer

extern "C" void fn_8022D9F4(void) {
    fn_8006B474("SUPERCOMPUTER.BIN", 1);
}

extern "C" void fn_8022D9F0(void) {}

namespace SuperComputer {

void SetSuperComputer::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new SetSuperComputer(parent, owner);
}

} // namespace SuperComputer

GadgetClassDesc lbl_80546EA4 = {
    "SUPER COMPUTER", fn_8022D9F4, fn_8022D9F0, SuperComputer::SetSuperComputer::Instantiate, 0, 0,
    0x1771, 1, 0, 10, 0,
};
