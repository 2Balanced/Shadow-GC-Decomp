/* GunBigfoot::GunBigfootAI (class name from RTTI): the G.U.N. Bigfoot walker's AI
 * (Game/GunBigfootAI.h). It waits crouching (WaitCrouching), hovering and swaying in place
 * (WaitHovering) or standing (EnemyBaseAI's Wait), or waits to act (WaitActAttack: walk to a point
 * near the target, turn, attack); once the target is in its area it attacks from the ground
 * (GunAttack) or, while hovering, flies at the target firing (HoverAttack). File name inferred
 * from the class. Names marked "(name inferred)" are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
/* See EnemyBaseAI (Game/EnemyAI.h). */
#define ENEMY_BASE_AI_STATES

#include "Game/GunBigfootAI.h"
#include "Game/EnemyAIState.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"

/* MSL's std::sin for floats, called out of line. */
namespace std {
float sin(float x);
}

extern "C" {
/* A point dist ahead of enemy toward position (behaviour inferred). */
Vec3f fn_8019AADC(Enemy *enemy, const Vec3f *position, float dist);
/* Turns enemy toward target (behaviour inferred). */
void fn_8019B0B0(Enemy *enemy, int target);
/* The distance from enemy to point (Game/Enemy.cpp). */
float fn_8019B298(Enemy *enemy, const Vec3f *point);
/* Turns enemy to the angles at rotation (behaviour inferred). */
void fn_8019B0F8(Enemy *enemy, const Vec3f *rotation);
/* Clears the body's flags but bit 2 (in the body's unit; behaviour read from its code). */
void fn_802875C8(GunBigfoot::GunBigfoot *body);
/* Vector routines that return their result (they store it through their first argument): v
 * turned about the y axis, a + b. */
Vec3f fn_8040D0AC(const Vec3f *v, float angle);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
/* v -= value, v += value, v scaled to length 1 and v * scale, in place. */
void fn_8040CE04(Vec3f *v, const Vec3f *value);
void fn_8040CFF8(Vec3f *v, const Vec3f *value);
float fn_8040CE74(Vec3f *v);
void fn_8040CE38(Vec3f *v, float scale);
}

/* fn_8040CD74 as a function that returns its result (see Game/EnemyRender.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *lhs, const Vec3f *rhs);

/* *v += value (name inferred). Inline: the calls in its arguments are then made out of line, as
 * retail makes them. */
inline void AddVector(Vec3f *v, const Vec3f &value) {
    fn_8040CFF8(v, &value);
}

/* *v -= (x, 0, 0) turned about the y axis by angle (name inferred; as FlyerCommonAI's). Inline
 * with the angle as a parameter: retail loads the angle before it builds the vector. The vector
 * is plain storage, as constructing a Vec3f would keep the function out of line. */
inline void SubtractSide(Vec3f *v, float x, float angle) {
    float side[3];
    fn_8040CE04(v, &(const Vec3f &)fn_8040D0AC(fn_80008E1C((Vec3f *)side, x, 0.0f, 0.0f), angle));
}

namespace GunBigfoot {

/* The walker's states (class names from RTTI; vtables 0x8052F77C to 0x8052F82C). */
class EnemyAIState_GunBigfootAI_HoverAttack : public TEnemyAIState {
  public:
    EnemyAIState_GunBigfootAI_HoverAttack();
    virtual ~EnemyAIState_GunBigfootAI_HoverAttack() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ GunBigfootAI *m_ai;
};

class EnemyAIState_GunBigfootAI_GunAttack : public TEnemyAIState {
  public:
    EnemyAIState_GunBigfootAI_GunAttack();
    virtual ~EnemyAIState_GunBigfootAI_GunAttack() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ GunBigfootAI *m_ai;
};

class EnemyAIState_GunBigfootAI_WaitActAttack : public TEnemyAIState {
  public:
    EnemyAIState_GunBigfootAI_WaitActAttack();
    virtual ~EnemyAIState_GunBigfootAI_WaitActAttack() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ GunBigfootAI *m_ai;
};

class EnemyAIState_GunBigfootAI_WaitHovering : public TEnemyAIState {
  public:
    EnemyAIState_GunBigfootAI_WaitHovering();
    virtual ~EnemyAIState_GunBigfootAI_WaitHovering() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ GunBigfootAI *m_ai;
};

class EnemyAIState_GunBigfootAI_WaitCrouching : public TEnemyAIState {
  public:
    EnemyAIState_GunBigfootAI_WaitCrouching();
    virtual ~EnemyAIState_GunBigfootAI_WaitCrouching() {}
    virtual void SetOwner(EnemyBaseAI *ai);
    virtual void Enter();
    virtual void Update();
    virtual void Exit();

    /* 0x4 */ GunBigfootAI *m_ai;
};

/* The states' objects (names inferred). They are inline, and the AI switches to them through
 * EnemyBaseAI::ChangeState, so they are called out of line (see Game/EnemyAI.h). */
static inline TEnemyAIState *GetWaitCrouchingState() {
    static EnemyAIState_GunBigfootAI_WaitCrouching s_state;
    return &s_state;
}

static inline TEnemyAIState *GetWaitHoveringState() {
    static EnemyAIState_GunBigfootAI_WaitHovering s_state;
    return &s_state;
}

static inline TEnemyAIState *GetWaitActAttackState() {
    static EnemyAIState_GunBigfootAI_WaitActAttack s_state;
    return &s_state;
}

static inline TEnemyAIState *GetGunAttackState() {
    static EnemyAIState_GunBigfootAI_GunAttack s_state;
    return &s_state;
}

static inline TEnemyAIState *GetHoverAttackState() {
    static EnemyAIState_GunBigfootAI_HoverAttack s_state;
    return &s_state;
}

EnemyAIState_GunBigfootAI_WaitCrouching::EnemyAIState_GunBigfootAI_WaitCrouching() : m_ai(0) {}

void EnemyAIState_GunBigfootAI_WaitCrouching::SetOwner(EnemyBaseAI *ai) {
    GunBigfootAI *owner = dynamic_cast<GunBigfootAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_GunBigfootAI_WaitHovering::EnemyAIState_GunBigfootAI_WaitHovering() : m_ai(0) {}

void EnemyAIState_GunBigfootAI_WaitHovering::SetOwner(EnemyBaseAI *ai) {
    GunBigfootAI *owner = dynamic_cast<GunBigfootAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_GunBigfootAI_WaitActAttack::EnemyAIState_GunBigfootAI_WaitActAttack() : m_ai(0) {}

void EnemyAIState_GunBigfootAI_WaitActAttack::SetOwner(EnemyBaseAI *ai) {
    GunBigfootAI *owner = dynamic_cast<GunBigfootAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_GunBigfootAI_GunAttack::EnemyAIState_GunBigfootAI_GunAttack() : m_ai(0) {}

void EnemyAIState_GunBigfootAI_GunAttack::SetOwner(EnemyBaseAI *ai) {
    GunBigfootAI *owner = dynamic_cast<GunBigfootAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_GunBigfootAI_HoverAttack::EnemyAIState_GunBigfootAI_HoverAttack() : m_ai(0) {}

void EnemyAIState_GunBigfootAI_HoverAttack::SetOwner(EnemyBaseAI *ai) {
    GunBigfootAI *owner = dynamic_cast<GunBigfootAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

GunBigfootAI::GunBigfootAI() {
    m_body = dynamic_cast<GunBigfoot *>(m_enemy->m_disp);
    m_rising = m_enemy->m_handle & 1;
    m_wave = 0.0f;
}

GunBigfootAI::~GunBigfootAI() {}

void GunBigfootAI::vfAI_0x38() {
    switch (m_body->m_appearType) {
    case 0:
        ChangeState(::GunBigfoot::GetWaitCrouchingState());
        break;
    case 1:
    case 4:
        ChangeState(::GunBigfoot::GetWaitHoveringState());
        break;
    case 2:
        fn_8019F944(&m_stateMachine, GetGoHomeState());
        break;
    case 3:
        ChangeState(::GunBigfoot::GetWaitActAttackState());
        break;
    }
}

void GunBigfootAI::vfAI_0x3C() {
    if (m_body->m_hovering) {
        ChangeState(::GunBigfoot::GetWaitHoveringState());
    } else {
        fn_8019F944(&m_stateMachine, GetWaitState());
    }
}

void GunBigfootAI::vfAI_0x48() {
    if (m_body->m_hovering) {
        ChangeState(::GunBigfoot::GetHoverAttackState());
    } else {
        ChangeState(::GunBigfoot::GetGunAttackState());
    }
}

void EnemyAIState_GunBigfootAI_WaitCrouching::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(1, true);
    m_ai->m_step = 0;
}

void EnemyAIState_GunBigfootAI_WaitCrouching::Update() {
    switch (m_ai->m_step) {
    case 0: {
        void *key = m_ai->vfCtrl_0x10();
        if (m_ai->m_enemy->field_0xA8.Contains(key)) {
            m_ai->m_enemy->m_status->vfStat_0x20(6, true);
            m_ai->m_step++;
        }
        break;
    }
    case 1:
        if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
            m_ai->vfAI_0x48();
        }
        break;
    }
}

void EnemyAIState_GunBigfootAI_WaitCrouching::Exit() {}

void EnemyAIState_GunBigfootAI_WaitHovering::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(4, true);
    m_ai->m_step = 0;
}

void EnemyAIState_GunBigfootAI_WaitHovering::Update() {
    switch (m_ai->m_step) {
    case 0: {
        fn_8019B0F8(m_ai->m_enemy, &m_ai->m_enemy->field_0x60);
        m_ai->m_enemy->vf_0x10();
        float width = m_ai->m_body->m_floatWidth;
        if (width > 0.1f) {
            float step = 20.0f * m_ai->GetDeltaTime();
            if (m_ai->m_rising) {
                if ((m_ai->m_wave += step) > width) {
                    m_ai->m_rising = 0;
                }
            } else {
                if ((m_ai->m_wave -= step) < -width) {
                    m_ai->m_rising = 1;
                }
            }
            Vec3f move;
            fn_800091EC(&move, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                   &m_ai->m_enemy->field_0x48, m_ai->m_enemy->GetPosition()));
            float wave = width * std::sin(0.5f * m_ai->m_wave * lbl_805FAD38 / width);
            SubtractSide(&move, wave, m_ai->m_enemy->GetHomeRotation()->y);
            if (fn_8040CF50(&move) > 2.0f * step) {
                fn_8040CE74(&move);
                fn_8040CE38(&move, step);
            }
            AddVector(fn_80181F0C(m_ai->m_enemy), move);
        }
        void *key = m_ai->vfCtrl_0x10();
        if (m_ai->m_enemy->field_0xA8.Contains(key)) {
            if (m_ai->m_body->m_appearType == 1) {
                m_ai->m_enemy->m_status->vfStat_0x20(5, true);
                m_ai->m_body->m_hovering = 0;
                m_ai->m_step++;
            } else {
                m_ai->vfAI_0x48();
            }
        }
        break;
    }
    case 1:
        if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
            m_ai->vfAI_0x48();
        }
        break;
    }
}

void EnemyAIState_GunBigfootAI_WaitHovering::Exit() {}

void EnemyAIState_GunBigfootAI_WaitActAttack::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_body->vf_0x278(), true);
    m_ai->m_step = 0;
}

void EnemyAIState_GunBigfootAI_WaitActAttack::Update() {
    void *key = m_ai->vfCtrl_0x10();
    if (m_ai->m_enemy->field_0xA8.Contains(key)) {
        m_ai->vfAI_0x48();
        return;
    }
    switch (m_ai->m_step) {
    case 0:
        if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), true);
            fn_8017DE10(m_ai->m_enemy, &(const Vec3f &)fn_8019AADC(m_ai->m_enemy,
                                                                    m_ai->m_enemy->GetPosition(),
                                                                    30.0f));
            m_ai->m_step++;
        }
        break;
    case 1:
        if (m_ai->m_enemy->vf_0x0C()) {
            fn_8019B0F8(m_ai->m_enemy, &m_ai->m_enemy->field_0x60);
            m_ai->m_step++;
        }
        break;
    case 2:
        if (m_ai->m_enemy->vf_0x10()) {
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x58(), true);
            m_ai->m_step = 0;
        }
        break;
    }
}

void EnemyAIState_GunBigfootAI_WaitActAttack::Exit() {}

void EnemyAIState_GunBigfootAI_GunAttack::Enter() {
    fn_802875C8(m_ai->m_body);
    m_ai->m_step = -1;
}

void EnemyAIState_GunBigfootAI_GunAttack::Update() {
    m_ai->m_body->SetAimPoint(fn_8040CDD0(
        (const Vec3f *)m_ai->vfCtrl_0x10(),
        &(const Vec3f &)fn_8040D0AC(&(const Vec3f &)Vec3f(0.0f, 0.0f, -20.0f), m_ai->vfCtrl_0x18())));
    switch (m_ai->m_step) {
    case -1:
        if (m_ai->m_enemy->m_status->vfStat_0x28()) {
            m_ai->m_body->m_aiming = 1;
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_body->vf_0x278(), true);
            m_ai->m_step = 0;
        }
        break;
    case 0:
        if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), true);
            m_ai->SetTimer(2.0f);
            m_ai->m_step++;
        }
        break;
    case 1: {
        void *key = m_ai->vfCtrl_0x10();
        if (!m_ai->m_enemy->field_0xA8.Contains(key)) {
            m_ai->vfAI_0x3C();
        } else if (m_ai->IsTimeUp()) {
            fn_8019B0B0(m_ai->m_enemy, fn_8019F65C(&m_ai->m_target));
            m_ai->m_step++;
        }
        break;
    }
    case 2:
        if (m_ai->m_enemy->vf_0x10()) {
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x58(), true);
            m_ai->m_step = 0;
        }
        break;
    }
}

void EnemyAIState_GunBigfootAI_GunAttack::Exit() {
    m_ai->m_body->m_aiming = 0;
}

void EnemyAIState_GunBigfootAI_HoverAttack::Enter() {
    m_ai->m_body->m_aiming = 1;
    m_ai->SetTimer(1.0f);
    m_ai->m_step = 0;
}

void EnemyAIState_GunBigfootAI_HoverAttack::Update() {
    void *key = m_ai->vfCtrl_0x10();
    if (!m_ai->m_enemy->field_0xA8.Contains(key)) {
        m_ai->vfAI_0x3C();
        return;
    }
    switch (m_ai->m_step) {
    case 0: {
        m_ai->m_body->SetAimPoint(fn_8040CDD0(
            &(const Vec3f &)m_ai->vfCtrl_0x1C(-1.0f),
            &(const Vec3f &)fn_8040D0AC(&(const Vec3f &)Vec3f(0.0f, 0.0f, -20.0f),
                                        m_ai->vfCtrl_0x18())));
        Vec3f target;
        fn_800091EC(&target, (const Vec3f *)fn_8019F65C(&m_ai->m_target));
        fn_8019F53C(&m_ai->m_turn, &target);
        if (m_ai->IsTimeUp()) {
            m_ai->SetTimer(2.0f);
            m_ai->m_enemy->m_status->vfStat_0x20(8, true);
            fn_802875C8(m_ai->m_body);
            m_ai->m_step++;
        }
        break;
    }
    case 1: {
        float rate;
        if (m_ai->field_0x70 > 0.01f) {
            rate = m_ai->field_0x6C / m_ai->field_0x70;
        } else {
            rate = 0.0f;
        }
        m_ai->m_body->SetAimPoint(fn_8040CDD0(
            &(const Vec3f &)m_ai->vfCtrl_0x1C(-1.0f),
            &(const Vec3f &)fn_8040D0AC(&(const Vec3f &)Vec3f(0.0f, 0.0f, -20.0f * rate),
                                        m_ai->vfCtrl_0x18())));
        if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
            m_ai->m_enemy->m_status->vfStat_0x20(4, true);
            fn_8017DE10(m_ai->m_enemy, &(const Vec3f &)fn_8019AADC(m_ai->m_enemy,
                                                                    m_ai->m_enemy->GetPosition(),
                                                                    30.0f));
            m_ai->m_step++;
        }
        break;
    }
    case 2: {
        m_ai->m_body->SetAimPoint(fn_8040CDD0(
            &(const Vec3f &)m_ai->vfCtrl_0x1C(-1.0f),
            &(const Vec3f &)fn_8040D0AC(&(const Vec3f &)Vec3f(0.0f, 0.0f, -20.0f),
                                        m_ai->vfCtrl_0x18())));
        Vec3f target;
        fn_800091EC(&target, (const Vec3f *)fn_8019F65C(&m_ai->m_target));
        fn_8019F53C(&m_ai->m_turn, &target);
        if (fn_8019B298(m_ai->m_enemy, &m_ai->m_enemy->field_0x6C) < 2.0f) {
            m_ai->SetTimer(1.0f);
            m_ai->m_step = 0;
        } else {
            Vec3f move;
            fn_800091EC(&move, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                   &m_ai->m_enemy->field_0x6C, m_ai->m_enemy->GetPosition()));
            float step = m_ai->m_enemy->m_para->vfPara_0x20() * m_ai->m_enemy->GetFrameTime();
            float distance = fn_8040CF50(&move);
            if (step > distance) {
                step = distance;
            }
            fn_8040CE74(&move);
            fn_8040CE38(&move, step);
            AddVector(fn_80181F0C(m_ai->m_enemy), move);
        }
        break;
    }
    }
}

void EnemyAIState_GunBigfootAI_HoverAttack::Exit() {
    m_ai->m_body->m_aiming = 0;
}

} // namespace GunBigfoot
