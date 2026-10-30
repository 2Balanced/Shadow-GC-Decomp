/* EggShadowAndroid::EggShadowAndroidAI (class name from RTTI): the Shadow android's AI
 * (Game/EggShadowAndroid.h), an EnemyBaseAI (Game/EnemyAI.h). It waits standing or goes home
 * (EnemyBaseAI's states) and attacks with lock-on missiles (MissileAttack); when an attack hits it
 * (message 0x16) it jumps (Jump), homes in on the attacker with its belt when the attacker is above
 * it (RejectHoming), jumps back off when hit while homing (HomingJump) and falls back to the ground
 * (Falling). File name inferred from the class. Names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). NonMatching (41 of 43 functions; every data section matches): the two turn helpers'
 * float registers (see TurnYawToward). */
/* See EnemyBaseAI (Game/EnemyAI.h). */
#define ENEMY_BASE_AI_STATES

#include "Game/EggShadowAndroid.h"
#include "Game/AngleUtil.h"
#include "Game/CharInfo.h"
#include "Game/EnemyAIState.h"
#include "Game/EnemyCharInfo.h"
#include "Game/MathTypes.h"
#include "Game/Random.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

/* MSL's std::fabs for floats, which the game calls out of line (as in BkSoldier.cpp). */
namespace std {
float fabs(float x);
}

extern "C" {
/* The state machine's lock around a state's own motion (behaviour inferred). */
void fn_8019F9F0(UnkAIStateMachine_8019F944 *machine);
void fn_8019F9FC(UnkAIStateMachine_8019F944 *machine);
/* Whether the enemy has a target in front of it (Game/Enemy.cpp; behaviour inferred). */
bool fn_8019AF04(Enemy *enemy);
/* Whether a character can be homed in on (behaviour inferred). */
bool fn_800B7A48(const boost::shared_ptr<CharInfo> &chara);
/* Whether angle a is within reach of b (behaviour inferred). */
bool fn_801C455C(float a, float b);
/* value moved toward target by rate (behaviour inferred). */
float fn_8040F314(float value, float target, float rate);
/* atan2 (behaviour inferred). */
float fn_80016260(float y, float x);
/* The character info's vector at 0x18 (inline; first emitted by another unit). */
Vec3f *fn_8028F324(Enemy *enemy);
/* Sounds (see GunBigfoot.cpp). */
void *fn_80053164(void);
void fn_801D54F4(void *manager, int sound, const Vec3f *position, int arg3, int arg4);
/* Vector routines that return their result (they store it through their first argument). */
Vec3f fn_8040D0AC(const Vec3f *v, float angle);
Vec3f fn_8040D118(const Vec3f *v, float angle);
Vec3f fn_8040D044(const Vec3f *v, float angle);
Vec3f fn_800091A4(const Vec3f *v, float scale);
Vec3f fn_8040CDA8(const Vec3f *v, float scale);
Vec2f fn_8040F760(const Vec3f *direction);
void fn_8040CFF8(Vec3f *v, const Vec3f *value);
}

/* fn_8040CD74 as a function that returns its result (see Game/EnemyRender.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *lhs, const Vec3f *rhs);

/* The message an attack sends (layout from vfAI_0x28; struct name unknown). */
struct UnkHitMessage_8019DC9C : public UnkEnemyMessage_8019DC9C {
    /* 0x08 */ u8 field_0x8[0x14 - 0x8];
    /* 0x14 */ boost::weak_ptr<CharInfo> m_attacker;
    /* 0x1C */ s32 field_0x1C;
};

/* EnemyBaseAI's message handler (Game/EnemyBaseAI.cpp), under its slot's symbol. */
extern "C" void vfAI_0x28__11EnemyBaseAIFv(EnemyBaseAI *ai, UnkEnemyMessage_8019DC9C *message);

/* Degrees to radians (name inferred). Inline: retail computes the limit before the call. */
inline float ToRadians(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

namespace EggShadowAndroid {

/* The android's states (class names from RTTI; vtables 0x80553CAC to 0x80553D5C). */
class EnemyAIState_EggShadowAndroidAI_Falling : public TEnemyAIState {
  public:
    EnemyAIState_EggShadowAndroidAI_Falling();
    virtual ~EnemyAIState_EggShadowAndroidAI_Falling() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ EggShadowAndroidAI *m_ai;
};

class EnemyAIState_EggShadowAndroidAI_HomingJump : public TEnemyAIState {
  public:
    EnemyAIState_EggShadowAndroidAI_HomingJump();
    virtual ~EnemyAIState_EggShadowAndroidAI_HomingJump() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ EggShadowAndroidAI *m_ai;
};

class EnemyAIState_EggShadowAndroidAI_RejectHoming : public TEnemyAIState {
  public:
    EnemyAIState_EggShadowAndroidAI_RejectHoming();
    virtual ~EnemyAIState_EggShadowAndroidAI_RejectHoming() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ EggShadowAndroidAI *m_ai;
};

class EnemyAIState_EggShadowAndroidAI_Jump : public TEnemyAIState {
  public:
    EnemyAIState_EggShadowAndroidAI_Jump();
    virtual ~EnemyAIState_EggShadowAndroidAI_Jump() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ EggShadowAndroidAI *m_ai;
};

class EnemyAIState_EggShadowAndroidAI_MissileAttack : public TEnemyAIState {
  public:
    EnemyAIState_EggShadowAndroidAI_MissileAttack();
    virtual ~EnemyAIState_EggShadowAndroidAI_MissileAttack() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ EggShadowAndroidAI *m_ai;
};

/* The states' objects (names inferred). They are inline, and the AI switches to them through
 * EnemyBaseAI::ChangeState, so they are called out of line (as in GunBigfootAI.cpp). */
static inline TEnemyAIState *GetFallingState() {
    static EnemyAIState_EggShadowAndroidAI_Falling s_state;
    return &s_state;
}

static inline TEnemyAIState *GetHomingJumpState() {
    static EnemyAIState_EggShadowAndroidAI_HomingJump s_state;
    return &s_state;
}

static inline TEnemyAIState *GetRejectHomingState() {
    static EnemyAIState_EggShadowAndroidAI_RejectHoming s_state;
    return &s_state;
}

static inline TEnemyAIState *GetJumpState() {
    static EnemyAIState_EggShadowAndroidAI_Jump s_state;
    return &s_state;
}

static inline TEnemyAIState *GetMissileAttackState() {
    static EnemyAIState_EggShadowAndroidAI_MissileAttack s_state;
    return &s_state;
}

/* The motions of the jump (picked at random). */
static const s32 s_jumpMotions[] = {7, 8, 9};

EnemyAIState_EggShadowAndroidAI_MissileAttack::EnemyAIState_EggShadowAndroidAI_MissileAttack()
    : m_ai(0) {}

void EnemyAIState_EggShadowAndroidAI_MissileAttack::SetOwner(EnemyBaseAI *ai) {
    EggShadowAndroidAI *owner = dynamic_cast<EggShadowAndroidAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_EggShadowAndroidAI_Jump::EnemyAIState_EggShadowAndroidAI_Jump() : m_ai(0) {}

void EnemyAIState_EggShadowAndroidAI_Jump::SetOwner(EnemyBaseAI *ai) {
    EggShadowAndroidAI *owner = dynamic_cast<EggShadowAndroidAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_EggShadowAndroidAI_RejectHoming::EnemyAIState_EggShadowAndroidAI_RejectHoming()
    : m_ai(0) {}

void EnemyAIState_EggShadowAndroidAI_RejectHoming::SetOwner(EnemyBaseAI *ai) {
    EggShadowAndroidAI *owner = dynamic_cast<EggShadowAndroidAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_EggShadowAndroidAI_HomingJump::EnemyAIState_EggShadowAndroidAI_HomingJump()
    : m_ai(0) {}

void EnemyAIState_EggShadowAndroidAI_HomingJump::SetOwner(EnemyBaseAI *ai) {
    EggShadowAndroidAI *owner = dynamic_cast<EggShadowAndroidAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_EggShadowAndroidAI_Falling::EnemyAIState_EggShadowAndroidAI_Falling() : m_ai(0) {}

void EnemyAIState_EggShadowAndroidAI_Falling::SetOwner(EnemyBaseAI *ai) {
    EggShadowAndroidAI *owner = dynamic_cast<EggShadowAndroidAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EggShadowAndroidAI::EggShadowAndroidAI() {
    m_body = dynamic_cast<EggShadowAndroid *>(m_enemy->m_disp);
}

EggShadowAndroidAI::~EggShadowAndroidAI() {}

/* Slot 0x28, the handler of the messages sent to the enemy. It takes the message, which
 * Game/EnemyAI.h does not declare without ENEMY_AI_MESSAGE_ARG (that macro gives the slot another
 * symbol), so it is defined under the slot's symbol, as Game/EnemyBaseAI.cpp does: an attack
 * (message 0x16) makes the android jump away from the attacker. */
extern "C" void vfAI_0x28__Q216EggShadowAndroid18EggShadowAndroidAIFv(
    EggShadowAndroidAI *ai, UnkEnemyMessage_8019DC9C *message) {
    UnkHitMessage_8019DC9C *hit = static_cast<UnkHitMessage_8019DC9C *>(message);
    if (message->m_id == 0x16 && hit->field_0x1C == 0) {
        ai->m_attacker = hit->m_attacker;
        message->m_flags |= 2;
        ai->ChangeState(GetJumpState());
    } else {
        vfAI_0x28__11EnemyBaseAIFv(ai, message);
    }
}

void EggShadowAndroidAI::vfAI_0x38() {
    if (m_body->m_params.AppearType == 0) {
        fn_8019F944(&m_stateMachine, GetWaitState());
    } else {
        fn_8019F944(&m_stateMachine, GetGoHomeState());
    }
}

void EggShadowAndroidAI::vfAI_0x3C() {
    fn_8019F944(&m_stateMachine, GetWaitState());
}

void EggShadowAndroidAI::vfAI_0x48() {
    ChangeState(GetMissileAttackState());
}

void EnemyAIState_EggShadowAndroidAI_MissileAttack::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x58(), true);
}

void EnemyAIState_EggShadowAndroidAI_MissileAttack::Update() {
    if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
        if (fn_8019AF04(m_ai->m_enemy) &&
            std::fabs(fn_8019F698(&m_ai->m_target)) < ToRadians(5.0f)) {
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x58(), true);
        } else {
            m_ai->vfAI_0x3C();
        }
    }
}

void EnemyAIState_EggShadowAndroidAI_MissileAttack::Exit() {
    m_ai->m_body->ClearLockon();
}

void EnemyAIState_EggShadowAndroidAI_Jump::Enter() {
    m_ai->m_body->m_guardFlags.Set(0);
    fn_8019F9FC(&m_ai->m_stateMachine);
    m_ai->m_enemy->m_status->vfStat_0x20(5, true);
    m_ai->SetTimer(0.75f);
    fn_800091D0(fn_8028F324(m_ai->m_enemy), &Vec3f(0.0f, 100.0f, 0.0f));
    m_ai->m_step = 0;
}

/* Turns self's yaw toward the point target at rate (or its own turn rate when rate is negative); whether it
 * faces it. Game/EggLastMechAIState.cpp calls it too. NonMatching: retail gives rate f31 and the yaw f30; GC/2.7 gives the
 * parameter the last register (see EggShadowAndroid::vfDisp_0x64 in Game/EggShadowAndroid.cpp). */
extern "C" bool fn_8029EB5C(EnemyCharInfo *self, const Vec3f *target, float rate) {
    Vec3f direction;
    Vec3f offset;
    fn_800091EC(&direction,
                fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                         target, &self->m_position)));
    float yaw = fn_80016260(-direction.x, -direction.z);
    if (rate < 0.0f) {
        rate = self->field_0xB4;
    }
    self->m_rotation.y = TurnAngleToward(yaw, self->m_rotation.y, rate * fn_8019FFE0(self));
    if (fn_801C455C(yaw, self->m_rotation.y)) {
        return true;
    }
    return false;
}

void EnemyAIState_EggShadowAndroidAI_Jump::Update() {
    fn_8029EB5C(static_cast<EnemyCharInfo *>(static_cast<CharInfo *>(
                    fn_8017D014(m_ai->m_enemy).lock().get())),
                &m_ai->m_attacker.lock()->m_position, 10.0f);
    if (!m_ai->IsTimeUp()) {
        Vec3f thrust;
        fn_80008E1C(&thrust, 0.0f, 270.0f, 0.0f);
        fn_8040CFF8(m_ai->m_enemy->GetAcceleration(), &thrust);
    }
    if (!m_ai->m_attacker.expired()) {
        bool below = m_ai->m_attacker.lock()->m_position.y <
                     5.0f + m_ai->m_enemy->GetPosition()->y;
        if (below && fn_800B7A48(m_ai->m_attacker.lock())) {
            fn_8019F9F0(&m_ai->m_stateMachine);
            m_ai->ChangeState(GetRejectHomingState());
            return;
        }
    }
    switch (m_ai->m_step) {
    case 0:
        if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
            m_ai->m_enemy->m_status->vfStat_0x20(6, true);
            m_ai->m_body->StartJetEffect();
            m_ai->m_step = 1;
        }
        break;
    case 1:
        if (fn_8028F324(m_ai->m_enemy)->y < 0.0f || m_ai->m_enemy->m_move->vfMove_0x18()) {
            m_ai->m_body->StopJetEffect();
            m_ai->m_step = 2;
        }
        break;
    case 2:
        if (m_ai->m_enemy->m_status->vfStat_0x28()) {
            fn_8019F9F0(&m_ai->m_stateMachine);
            m_ai->ChangeState(GetFallingState());
            return;
        }
        break;
    }
    m_ai->m_body->Fall();
}

void EnemyAIState_EggShadowAndroidAI_Jump::Exit() {
    m_ai->m_body->m_guardFlags.Clear(0);
    m_ai->m_body->StopJetEffect();
    fn_8019F9F0(&m_ai->m_stateMachine);
}

void EnemyAIState_EggShadowAndroidAI_RejectHoming::Enter() {
    m_ai->m_body->m_guardFlags.Set(0);
    fn_8019F9FC(&m_ai->m_stateMachine);
    if (m_ai->m_enemy->m_status->vfStat_0x24() != 6) {
        m_ai->m_enemy->m_status->vfStat_0x20(6, true);
    }
    fn_801D54F4(fn_80053164(), 0x3005, m_ai->m_enemy->GetPosition(), 0, 1);
    m_ai->m_body->CreateBelt();
    m_ai->m_body->m_moveFlags.Set(1);
    m_ai->SetTimer(3.0f);
    m_ai->m_step = 0;
}

/* Turns self toward the point target at rate (or its own turn rate when rate is negative), its pitch at most
 * its pitch limit; whether it faces it (name inferred). NonMatching: retail gives rate f31, the
 * pitch f30 and the yaw f29 (GC/2.7: the parameter last, as in TurnYawToward), and returns the
 * second test through r0. */
extern "C" bool fn_8029E488(EnemyCharInfo *self, const Vec3f *target, float rate) {
    Vec3f direction;
    Vec3f offset;
    fn_800091EC(&direction,
                fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                         target, &self->m_position)));
    Vec2f angles = fn_8040F760(&direction);
    float pitch = angles.x;
    float yaw = angles.y;
    if (rate < 0.0f) {
        rate = self->field_0xB4;
    }
    rate *= fn_8019FFE0(self);
    if (pitch > self->field_0xB8) {
        pitch = self->field_0xB8;
    } else if (pitch < -self->field_0xB8) {
        pitch = -self->field_0xB8;
    }
    self->m_rotation.y = TurnAngleToward(yaw, self->m_rotation.y, rate);
    self->m_rotation.x = TurnAngleToward(pitch, self->m_rotation.x, rate);
    self->m_rotation.z = TurnAngleToward(0.0f, self->m_rotation.z, rate);
    return fn_801C455C(yaw, self->m_rotation.y) && fn_801C455C(pitch, self->m_rotation.x);
}

void EnemyAIState_EggShadowAndroidAI_RejectHoming::Update() {
    if (m_ai->m_body->WasHit()) {
        fn_8019F9F0(&m_ai->m_stateMachine);
        m_ai->ChangeState(GetHomingJumpState());
        return;
    }
    if (m_ai->IsTimeUp() || m_ai->m_attacker.expired()) {
        fn_8019F9F0(&m_ai->m_stateMachine);
        m_ai->ChangeState(GetFallingState());
        return;
    }
    fn_8029E488(static_cast<EnemyCharInfo *>(static_cast<CharInfo *>(
                    fn_8017D014(m_ai->m_enemy).lock().get())),
                &m_ai->m_attacker.lock()->m_position, 10.0f);
    Vec3f direction;
    fn_800091EC(&direction, &(const Vec3f &)fn_8040D0AC(&Vec3f(0.0f, 0.0f, -1.0f),
                                                        m_ai->m_enemy->GetRotation()->y));
    fn_800091D0(&direction,
                &(const Vec3f &)fn_8040D118(&direction, m_ai->m_enemy->GetRotation()->x));
    fn_800091D0(&direction,
                &(const Vec3f &)fn_8040D044(&direction, m_ai->m_enemy->GetRotation()->z));
    fn_8040CFF8(m_ai->m_enemy->GetAcceleration(),
                &(const Vec3f &)fn_800091A4(&direction, 1000.0f));
    fn_800091D0(fn_8028F324(m_ai->m_enemy),
                &(const Vec3f &)fn_8040CDA8(&direction,
                                            fn_8040CF50(fn_8028F324(m_ai->m_enemy))));
    m_ai->m_body->Fall();
}

void EnemyAIState_EggShadowAndroidAI_RejectHoming::Exit() {
    m_ai->m_body->m_guardFlags.Clear(0);
    fn_8019F9F0(&m_ai->m_stateMachine);
    m_ai->m_body->m_moveFlags.Clear(1);
    m_ai->m_body->DeleteBelt();
}

void EnemyAIState_EggShadowAndroidAI_HomingJump::Enter() {
    m_ai->m_body->m_guardFlags.Set(0);
    fn_8019F9FC(&m_ai->m_stateMachine);
    m_ai->SetTimer(0.75f);
    fn_800091D0(fn_8028F324(m_ai->m_enemy), &Vec3f(0.0f, 100.0f, 0.0f));
    m_ai->m_step = 0;
}

void EnemyAIState_EggShadowAndroidAI_HomingJump::Update() {
    if (!m_ai->IsTimeUp()) {
        Vec3f thrust;
        fn_80008E1C(&thrust, 0.0f, 200.0f, 0.0f);
        fn_8040CFF8(m_ai->m_enemy->GetAcceleration(), &thrust);
    }
    switch (m_ai->m_step) {
    case 0:
        if (m_ai->m_enemy->m_status->vfStat_0x28()) {
            m_ai->m_enemy->m_status->vfStat_0x20(s_jumpMotions[fn_8040EA90(3)], true);
            m_ai->m_step++;
        }
        break;
    case 1:
        if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
            fn_8019F9F0(&m_ai->m_stateMachine);
            m_ai->ChangeState(GetFallingState());
            return;
        }
        break;
    }
    m_ai->m_body->Fall();
}

void EnemyAIState_EggShadowAndroidAI_HomingJump::Exit() {
    m_ai->m_body->m_guardFlags.Clear(0);
    fn_8019F9F0(&m_ai->m_stateMachine);
}

void EnemyAIState_EggShadowAndroidAI_Falling::Enter() {
    fn_800091D0(fn_8028F324(m_ai->m_enemy), &Vec3f(0.0f, -1.0f, 0.0f));
    m_ai->m_body->m_guardFlags.Set(0);
    fn_8019F9FC(&m_ai->m_stateMachine);
    if (m_ai->m_enemy->m_status->vfStat_0x24() != 6) {
        m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x50(), true);
    }
    m_ai->m_body->StartBoostEffect();
}

void EnemyAIState_EggShadowAndroidAI_Falling::Update() {
    m_ai->m_enemy->GetRotation()->x = fn_8040F314(m_ai->m_enemy->GetRotation()->x, 0.0f, 0.5f);
    m_ai->m_enemy->GetRotation()->z = fn_8040F314(m_ai->m_enemy->GetRotation()->z, 0.0f, 0.5f);
    if (m_ai->m_enemy->m_status->vfStat_0x24() == 6 && m_ai->m_enemy->m_status->vfStat_0x28()) {
        m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x50(), true);
    }
    float ground = m_ai->m_enemy->m_move->vfMove_0x28();
    if (m_ai->m_enemy->GetPosition()->y - ground < 10.0f) {
        m_ai->m_body->m_guardFlags.Clear(0);
        m_ai->m_body->StopBoostEffect();
    }
    if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
        fn_8019F9F0(&m_ai->m_stateMachine);
        m_ai->m_enemy->GetRotation()->x = 0.0f;
        m_ai->m_enemy->GetRotation()->z = 0.0f;
        m_ai->vfAI_0x3C();
    }
}

void EnemyAIState_EggShadowAndroidAI_Falling::Exit() {
    m_ai->m_body->m_guardFlags.Clear(0);
    fn_8019F9F0(&m_ai->m_stateMachine);
    m_ai->m_body->StopBoostEffect();
}

} // namespace EggShadowAndroid
