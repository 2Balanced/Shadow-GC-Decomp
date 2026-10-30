/* MagmaGuard::SetMagmaGuard (class name from RTTI): the MAGMA GUARD gadget, a guard (MAGMAGUARD.BIN)
 * with twelve hit points that smokes harder as it is damaged. Once destroyed it turns its owner's
 * switch on, counts towards the player's mission, keeps a loop sound running and stays destroyed
 * (the owner's flag 0x400000). File name inferred from the namespace. Names marked "(name
 * inferred)" are not original. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetUtil.h"
#include "Game/Mission.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/Vibration.h"
#include "Game/FileQueue.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;

/* The positioned sound fn_802B4254 creates (class name unknown; see Game/DefenseSystem.cpp). */
class UnkSound_802B4254;
class UnkPlayerManager_8006D6A8;

extern "C" {
/* GadgetModelInfo helpers (see Game/DefenseSystem.cpp and Game/BaseDoor.cpp). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
bool fn_80161BD8(GadgetModelInfo *model, int index, const boost::weak_ptr<CharInfo> &info,
                 int arg3);
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int variant);
void fn_80160A84(GadgetModelInfo *model, int arg1, int arg2);
/* The matrix of a character info's position and rotation (see Game/BKTank.cpp). */
void fn_8015CEA0(GadgetCharInfo *charInfo, Mtx34f *matrix);

/* The 32-bit flag word (see Game/BaseDoor.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* Particle helpers (see Game/BaseDoor.cpp; fn_80166E20 stops emitter id, inferred). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void fn_80166E20(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_80167010(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_8016761C(GadgetParticle::GadgetParticleCtrl *ctrl, float dt);
void fn_8015FA78(GadgetModelInfo *model,
                 void *(*callback)(void *part, UnkParticleAttach_80169D00 *param),
                 UnkParticleAttach_80169D00 *param);
void *fn_80169D00(void *part, UnkParticleAttach_80169D00 *param);

/* The positioned sound (see Game/DefenseSystem.cpp). */
boost::shared_ptr<UnkSound_802B4254> fn_802B4254(int kind, int arg1);
void fn_802B4510(UnkSound_802B4254 *sound, u8 flag);
void fn_802B44C8(UnkSound_802B4254 *sound, const Vec3f *position);

/* Plays sound id at a character info, once and as a loop for source (see Game/BaseDoor.cpp). */
void fn_801D54F4(void *manager, int id, GadgetCharInfo *source, int arg3, int arg4);
void fn_801D53C4(void *manager, int id, GadgetCharInfo *charInfo, void *source, int arg4, int arg5,
                 int arg6);
/* Starts a controller vibration at charInfo (see Game/TurnOver.cpp). */
boost::weak_ptr<Vibration::RequestHandle> fn_80169AEC(int type, float strength,
                                                      GadgetCharInfo *charInfo);

/* The owner record: its flags, the switch id it was placed with, setting flags, and telling it the
 * gadget changed (behaviours inferred; see Game/BASwitch.cpp). */
void fn_800C9EC4(UnkPlacement_800C9CD0 *owner, u32 *flags);
void fn_800C9E50(UnkPlacement_800C9CD0 *owner, s32 *id);
void fn_800C9EB0(UnkPlacement_800C9CD0 *owner, u32 mask);
void fn_800C985C(UnkPlacement_800C9CD0 *owner);

/* The player manager and a player's mission (see Game/BASwitch.cpp). */
UnkPlayerManager_8006D6A8 *fn_8006D6A8(void);
boost::weak_ptr<Mission::Mission> fn_800740E4(UnkPlayerManager_8006D6A8 *manager, int index);
}

/* The height of the loop sound above the guard (the game sets it in __sinit; the inline call
 * reproduces that, as in Game/CameraModeFixLook2.cpp). */
inline float DynamicFloat(float value) {
    return value;
}
static const float s_soundHeight = DynamicFloat(94.0f);

namespace MagmaGuard {

/* The guard's hit points (name inferred). */
const float kHitPoints = 12.0f;

/* The flag word at 0x60 (struct name unknown; bits 0-2: smoke stages started, bit 3: destroyed,
 * meanings inferred). */
struct SetMagmaGuardFlags {
    SetMagmaGuardFlags() {
        fn_80014E04(&m_bits);
    }
    void Clear() {
        fn_80014E10(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits;
};

class SetMagmaGuard : public Gadget {
  public:
    SetMagmaGuard(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~SetMagmaGuard();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Method names are inferred. */
    /* Copies the owner's position, rotation and switch id and resets the state. */
    void LoadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model and the loop sound at the character info. */
    void RebuildTransform();
    /* The attack collider's callback: takes the damage. */
    void OnAttack(AttackCallbackParam &param);

    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::shared_ptr<UnkSound_802B4254> m_sound;
    /* 0x48 */ s32 m_state; /* 0 placed, 1 standing, 2 destroyed, 3 done */
    /* 0x4C */ s32 m_switchId;
    /* 0x50 */ float m_hitPoints;
    /* 0x54 */ float m_hitTimer; /* attacks are ignored while it runs */
    /* 0x58 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particle;
    /* 0x60 */ SetMagmaGuardFlags m_flags;
    /* 0x64 */ u32 field_0x64;
};
SIZE_ASSERT(SetMagmaGuard, 0x68);

const char *SetMagmaGuard::s_taskName = "SetMagmaGuard";

SetMagmaGuard::SetMagmaGuard(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x7D, 4);
    LoadPlacement(owner);
    m_model = fn_8015F800(1, "MAGMAGUARD.BIN");
    m_model->fn_80162A68(0, 0);
    m_model->fn_80160C28(0);
    m_model->fn_8015FF60();
    fn_80161BD8(m_model.px, 0, boost::weak_ptr<CharInfo>(m_chara), 0);
    boost::weak_ptr<CharaColliAttack> attack = fn_80161428(m_model.px, 0);
    if (boost::shared_ptr<CharaColliAttack> target = attack.lock()) {
        target->field_0x54 = 0;
        target->field_0x55 = 1;
        target->m_attackCallback = AttackCallback(boost::bind(&SetMagmaGuard::OnAttack, this, _1));
    }
    m_sound = fn_802B4254(0, 0);
    fn_802B4510(m_sound.px, 1);
    RebuildTransform();
}

SetMagmaGuard::~SetMagmaGuard() {}

void SetMagmaGuard::Behave(float dt) {
    if (m_model.px != 0) {
        m_model->fn_80162FB0(dt);
    }
    if (m_particle.px != 0) {
        fn_8016761C(m_particle.px, dt);
    }
    m_hitTimer -= dt;
    if (m_hitTimer < 0.0f) {
        m_hitTimer = 0.0f;
    }
    switch (m_state) {
    case 0: {
        if (m_model.px != 0) {
            m_particle = fn_80166AF4();
            UnkParticleAttach_80169D00 attach;
            attach.ctrl = m_particle;
            fn_8015FA78(m_model.px, fn_80169D00, &attach);
        }
        u32 flags;
        fn_800C9EC4(m_owner, &flags);
        if (flags & 0x400000) {
            fn_801690D0(fn_800CB69C(), m_switchId, 1);
            fn_80160A84(m_model.px, 0, 1);
            if (m_particle.px != 0) {
                fn_80167010(m_particle.px, 1);
                fn_80167010(m_particle.px, 2);
                fn_80167010(m_particle.px, 3);
            }
            m_state = 3;
            m_flags.Set(3);
        } else {
            m_state = 1;
        }
        break;
    }
    case 1:
        break;
    case 2: {
        fn_801690D0(fn_800CB69C(), m_switchId, 1);
        {
            boost::shared_ptr<Mission::Mission> player = fn_800740E4(fn_8006D6A8(), 0).lock();
            if (player.px != 0) {
                player->AddCount(1, 0);
            }
        }
        fn_80160A84(m_model.px, 0, 1);
        fn_800C9EB0(m_owner, 0x400000);
        m_state = 3;
        fn_800C985C(m_owner);
        break;
    }
    case 3:
        break;
    }
    RebuildTransform();
    if (m_flags.Test(3)) {
        fn_801D53C4(fn_80053164(), 0x784C, m_chara.px, this, 3, 0, 2);
    }
}

void SetMagmaGuard::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadPlacement(owner);
    RebuildTransform();
    if (m_model.px != 0) {
        m_model->fn_80162EF4();
    }
}

void SetMagmaGuard::LoadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9E50(owner, &m_switchId);
    m_state = 0;
    m_hitPoints = kHitPoints;
    m_hitTimer = 0.0f;
    m_flags.Clear();
}

void SetMagmaGuard::RebuildTransform() {
    if (m_model.px != 0) {
        Mtx34f matrix;
        fn_8015CEA0(m_chara.px, &matrix);
        m_model->fn_801633DC(&matrix);
    }
    if (m_sound.px != 0) {
        Vec3f position;
        fn_800091EC(&position, &m_chara->m_position);
        position.y += s_soundHeight;
        fn_802B44C8(m_sound.px, &position);
    }
}

void SetMagmaGuard::OnAttack(AttackCallbackParam &param) {
    param.m_result = 5;
    if (m_hitTimer > 0.0f) {
        return;
    }
    if (m_state != 1) {
        return;
    }
    if (boost::shared_ptr<CharaColliAttack> attack = param.m_attacker.lock()) {
        if ((attack->m_flags & 0x3D1) == 0) {
            return;
        }
        param.m_result = 1;
        m_hitPoints -= param.m_damage;
        if (m_hitPoints <= 0.0f) {
            m_state = 2;
            m_flags.Set(3);
            fn_80169AEC(7, 2.0f, m_chara.px);
        } else {
            fn_80160A84(m_model.px, 0, 2);
            m_hitTimer = 0.5f;
        }
        fn_801D54F4(fn_80053164(), 0x784A, m_chara.px, 0, 1);
        if (!m_flags.Test(0) && m_hitPoints <= kHitPoints * 0.9f) {
            fn_80167010(m_particle.px, 1);
            m_flags.Set(0);
        }
        if (!m_flags.Test(1) && m_hitPoints <= kHitPoints * 0.5f) {
            fn_80166E20(m_particle.px, 1);
            fn_80167010(m_particle.px, 2);
            m_flags.Set(1);
        }
        if (!m_flags.Test(2) && m_hitPoints <= 0.0f) {
            fn_80166E20(m_particle.px, 2);
            fn_80167010(m_particle.px, 3);
            m_flags.Set(2);
        }
    }
}

} // namespace MagmaGuard

extern "C" void fn_8022AD78(void) {
    fn_8006B474("MAGMAGUARD.BIN", 1);
}

extern "C" void fn_8022AD74(void) {}

namespace MagmaGuard {

void SetMagmaGuard::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new SetMagmaGuard(parent, owner);
}

} // namespace MagmaGuard

GadgetClassDesc lbl_80546A9C = {
    "MAGMA GUARD", fn_8022AD78, fn_8022AD74, MagmaGuard::SetMagmaGuard::Instantiate, 0, 0, 0x1838, 1,
    0, 10, 0,
};
