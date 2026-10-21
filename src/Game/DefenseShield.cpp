/* DefenseShield (class name from RTTI): the DEFENSE_SHIELD gadget, a shield (DEFENSESHIELD.BIN)
 * with three looks: whole (model variant 0), damaged (1) and broken (2). Its health lives in a
 * DefenseShieldSetState kept by the owner record, so it survives the gadget being recreated; the
 * shield also breaks when the numbered switch it was placed with turns on. Attacks wear the health
 * down; once it is gone the attacker is rewarded and the shield breaks. File name inferred from the
 * class. Method names marked "(name inferred)" are not original. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/SetAdapterState.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/FileQueue.h"
#include "boost/bind.hpp"
#include "boost/function.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;

/* Target (class name from RTTI): a point other objects can aim at (see Spring.cpp). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);

    /* Clears bit 0 of the flags: other objects no longer aim at it (name and meaning inferred). */
    void TurnOff() {
        m_flags &= ~1;
    }

    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ u32 m_flags;
};

extern "C" {
/* GadgetModelInfo methods that are not declared in its header yet (see ArkCannon.cpp). */
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int variant);
/* Sets callback as the attack callback of an attack collision (see BreakRoad.cpp). */
void fn_80096B64(CharaColliAttack *attack, int arg1, const AttackCallback &callback);
/* Loads a gadget model (see GadgetUtil.h). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* Creates a target with the given flags (see CircusBall.cpp). */
boost::shared_ptr<Target> fn_801E72DC(int flags);
/* The attacker of an attack (behaviour inferred). */
boost::shared_ptr<CharInfo> fn_8015D570(AttackCallbackParam &param);
/* Gives the attacker the reward for breaking the object (behaviour inferred). */
void fn_8020642C(const boost::shared_ptr<CharInfo> &attacker, int reward);
/* Whether value is zero, and whether a equals b, within a small tolerance (from their bodies). */
bool fn_8000A950(float value);
bool fn_801C455C(float a, float b);
/* Plays sound id at the character info (behaviour inferred). */
void fn_801D54F4(void *manager, int id, GadgetCharInfo *charInfo, int arg3, int arg4);

/* A 32-bit flag word: fn_8000AAEC constructs one from a value, fn_80014E1C sets bit, fn_8006127C
 * clears bit and fn_80014E34 tests bit (see FireRing.cpp and Elevator.cpp). */
u32 *fn_8000AAEC(u32 *flags, u32 value);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* Particle controller helpers (see TurnOver.cpp; fn_80166F88 stops the controller, inferred). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void fn_80166F88(GadgetParticle::GadgetParticleCtrl *ctrl);
void fn_80167010(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_8016761C(GadgetParticle::GadgetParticleCtrl *ctrl, float dt);
/* Calls callback with each part of the model; fn_80169D00 attaches a particle controller to a part
 * (see TurnOver.cpp). */
void fn_8015FA78(GadgetModelInfo *model, void *(*callback)(void *part, void *data), void *data);
void *fn_80169D00(void *part, void *data);
}

/* The flag word at 0x58 (struct name unknown; as in ArkCannon.cpp). */
struct DefenseShieldFlags {
    DefenseShieldFlags(u32 value) {
        fn_8000AAEC(&m_bits, value);
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

    u32 m_bits; /* bit 0 damaged, bit 1 broken (meanings inferred) */
};

/* DefenseShieldSetState (class name from RTTI): the shield's state that the owner record keeps
 * (member name inferred). */
class DefenseShieldSetState : public SetAdapterState {
  public:
    DefenseShieldSetState() : m_health(25.0f) {}

    /* 0x4 */ float m_health;
};
SIZE_ASSERT(DefenseShieldSetState, 0x8);

class DefenseShield : public Gadget {
  public:
    DefenseShield(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~DefenseShield();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's placement and switch id (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Moves the model and the target to the character info's place (name inferred). */
    void RebuildTransform();
    /* Leaves state from and enters state to: 1 whole, 2 damaged, 3 broken (name inferred). */
    void SetState(int from, int to);
    /* The attack callback of the model's collision (name inferred). */
    void OnAttack(AttackCallbackParam &param);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::shared_ptr<Target> m_target;
    /* 0x48 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particle;
    /* 0x50 */ DefenseShieldSetState *m_setState;
    /* 0x54 */ s32 m_state;
    /* 0x58 */ DefenseShieldFlags m_flags;
    /* 0x5C */ u8 m_switchId;
};
SIZE_ASSERT(DefenseShield, 0x60);

const char *DefenseShield::s_taskName = "DefenseShield";

DefenseShield::DefenseShield(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner), m_setState(0), m_state(0), m_flags(0), m_switchId(0) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x60, 4);
    ReadPlacement(owner);
    m_target = fn_801E72DC(0xA800);
    m_model = fn_8015F800(1, "DEFENSESHIELD.BIN");
    if (owner->m_adapterState == 0) {
        fn_800C9930(owner, new DefenseShieldSetState);
    }
    m_setState = dynamic_cast<DefenseShieldSetState *>(owner->m_adapterState);
    if (fn_8016904C(fn_800CB69C(), m_switchId) == 1 || m_setState->m_health < 0.0f ||
        fn_8000A950(m_setState->m_health)) {
        SetState(m_state, 3);
        m_target->TurnOff();
    } else if (m_setState->m_health < 5.0f || fn_801C455C(m_setState->m_health, 5.0f)) {
        SetState(m_state, 2);
    } else {
        SetState(m_state, 1);
    }
}

DefenseShield::~DefenseShield() {}

extern "C" void fn_80223000(void) {
    fn_8006B474("DEFENSESHIELD.BIN", 1);
}

extern "C" void fn_80222FFC(void) {}

void DefenseShield::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new DefenseShield(fn_801EB304(fn_8001C4F8(), 11), owner);
}

void DefenseShield::Behave(float dt) {
    if (m_state != 3) {
        if (fn_8016904C(fn_800CB69C(), m_switchId) == 1) {
            m_flags.Set(1);
        }
    }
    if (m_flags.Test(1)) {
        fn_801D54F4(fn_80053164(), 0x7734, m_chara.get(), 0, 1);
        SetState(m_state, 3);
        m_flags.Clear(1);
        m_flags.Clear(0);
    } else if (m_flags.Test(0)) {
        SetState(m_state, 2);
        m_flags.Clear(0);
    }
    RebuildTransform();
    m_model->fn_80162FB0(dt);
    if (m_particle.get() != 0) {
        fn_8016761C(m_particle.get(), dt);
    }
}

void DefenseShield::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    RebuildTransform();
    m_model->fn_80162EF4();
    m_model->fn_8016108C();
}

void DefenseShield::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9E60(owner, &m_switchId);
}

void DefenseShield::RebuildTransform() {
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_chara->m_position);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    fn_8040B884(&matrix, m_chara->m_rotation.x);
    fn_8040B80C(&matrix, m_chara->m_rotation.z);
    m_model->fn_801633DC(&matrix);
    m_target->fn_800CCE38(&m_chara->m_position);
}

void DefenseShield::SetState(int from, int to) {
    switch (from) {
    case 1:
    case 2:
    case 3:
        if (m_particle.get() != 0) {
            fn_80166F88(m_particle.get());
            m_particle.reset();
        }
        m_model->fn_80162804();
        m_model->fn_80161880();
        break;
    }

    switch (to) {
    case 1: {
        m_model->fn_80162A68(0, 0);
        RebuildTransform();
        m_particle = fn_80166AF4();
        {
            UnkParticleAttach_80169D00 attach;
            attach.ctrl = m_particle;
            fn_8015FA78(m_model.get(), fn_80169D00, &attach);
            fn_80167010(m_particle.get(), -1);
        }
        fn_80161FC8(m_model.get(), 0, m_chara);
        boost::shared_ptr<CharaColliAttack> collision = fn_80161428(m_model.get(), 0).lock();
        if (collision.get() != 0) {
            fn_80096B64(collision.get(), 1, boost::bind(&DefenseShield::OnAttack, this, _1));
        }
        break;
    }
    case 2: {
        m_model->fn_80162A68(1, 0);
        RebuildTransform();
        m_particle = fn_80166AF4();
        {
            UnkParticleAttach_80169D00 attach;
            attach.ctrl = m_particle;
            fn_8015FA78(m_model.get(), fn_80169D00, &attach);
            fn_80167010(m_particle.get(), -1);
        }
        fn_80161FC8(m_model.get(), 0, m_chara);
        boost::shared_ptr<CharaColliAttack> collision = fn_80161428(m_model.get(), 0).lock();
        if (collision.get() != 0) {
            fn_80096B64(collision.get(), 1, boost::bind(&DefenseShield::OnAttack, this, _1));
        }
        break;
    }
    case 3:
        m_model->fn_80162A68(2, 0);
        RebuildTransform();
        fn_80161FC8(m_model.get(), 1, m_chara);
        m_target->TurnOff();
        break;
    }
    m_state = to;
}

void DefenseShield::OnAttack(AttackCallbackParam &param) {
    param.m_result = 1;
    if (m_state != 3) {
        m_setState->m_health -= param.m_damage;
        if (m_setState->m_health < 0.0f || fn_8000A950(m_setState->m_health)) {
            param.m_result = 3;
            boost::shared_ptr<CharInfo> attacker = fn_8015D570(param);
            if (attacker.get() != 0) {
                fn_8020642C(attacker, 0x48);
            }
            m_flags.Set(1);
        } else if (m_state == 1 && !m_flags.Test(1)) {
            if (m_setState->m_health < 5.0f || fn_801C455C(m_setState->m_health, 5.0f)) {
                m_flags.Set(0);
            }
            if (param.m_damage >= 1.0f) {
                fn_801D54F4(fn_80053164(), 0x7733, m_chara.get(), 0, 1);
            }
        }
    }
}

GadgetClassDesc lbl_8054629C = {
    "DEFENSE_SHIELD", fn_80223000, fn_80222FFC, DefenseShield::Instantiate, 0, 0, 0x1389, 1, 0, 10, 0,
};
