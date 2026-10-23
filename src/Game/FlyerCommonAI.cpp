/* FlyerCommonAI (class name from RTTI): the flying enemies' AI (Game/EnemyFlyer.h), the base of
 * the Black Arms wings' and the Gun Beetle's AIs. Its two states follow the flyer's path, slowing
 * to aim while in the attack stretch (MoveOnPath), or float in place, swaying by its float width
 * (WaitFloating). File name inferred from the class. Names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
/* See FlyerCommonAI (Game/EnemyFlyer.h) and EnemyBaseAI (Game/EnemyAI.h). */
#define FLYER_COMMON_AI_STATES
#define ENEMY_BASE_AI_HELPERS

#include "Game/EnemyFlyer.h"
#include "Game/EnemyAIState.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"

/* MSL's std::sin for floats, called out of line. */
namespace std {
float sin(float x);
}

extern "C" {
/* Turns the enemy a step toward position (behaviour inferred; see fn_8019F53C). */
bool fn_8019F2E0(UnkAITurn_8019F53C *turn, const Vec3f *position);
/* Turns enemy to the angles at rotation (behaviour inferred). */
void fn_8019B0F8(Enemy *enemy, const Vec3f *rotation);
/* v turned about the y axis (it stores the result through its first argument). */
Vec3f fn_8040D0AC(const Vec3f *v, float angle);
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

/* The flyer's float width (name inferred). Inline: retail reads the flyer through the AI pointer
 * it already holds when it compares the sway with it. */
inline float FloatWidth(FlyerCommonAI *ai) {
    return ai->m_flyer->field_0x2DC;
}

/* *v -= or += (x, 0, 0) turned about the y axis by angle (names inferred). Inline with the angle
 * as a parameter: retail loads the angle before it builds the vector. The vector is plain
 * storage, as constructing a Vec3f would keep these functions out of line. */
inline void SubtractSide(Vec3f *v, float x, float angle) {
    float side[3];
    fn_8040CE04(v, &(const Vec3f &)fn_8040D0AC(fn_80008E1C((Vec3f *)side, x, 0.0f, 0.0f), angle));
}

inline void AddSide(Vec3f *v, float x, float angle) {
    float side[3];
    fn_8040CFF8(v, &(const Vec3f &)fn_8040D0AC(fn_80008E1C((Vec3f *)side, x, 0.0f, 0.0f), angle));
}


/* The defaults of slots the states call (Game/Enemy.h, Game/EnemyFlyer.h; inline, first emitted
 * here). */
inline int EnemyParamCommon::vfPara_0x48() {
    return 0;
}

inline bool FlyerCommonBase::vf_0x238() {
    return 0;
}

inline bool FlyerCommonBase::vf_0x23C() {
    return 0;
}

inline float FlyerCommonBase::vf_0x240() {
    return 20.0f;
}

inline void FlyerCommonBase::vf_0x244() {}

inline void FlyerCommonBase::vf_0x248() {}

EnemyAIState_FlyerCommonAI_WaitFloating::EnemyAIState_FlyerCommonAI_WaitFloating() : m_ai(0) {}

void EnemyAIState_FlyerCommonAI_WaitFloating::SetOwner(EnemyBaseAI *ai) {
    FlyerCommonAI *owner = dynamic_cast<FlyerCommonAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_FlyerCommonAI_MoveOnPath::EnemyAIState_FlyerCommonAI_MoveOnPath() : m_ai(0) {}

void EnemyAIState_FlyerCommonAI_MoveOnPath::SetOwner(EnemyBaseAI *ai) {
    FlyerCommonAI *owner = dynamic_cast<FlyerCommonAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

FlyerCommonAI::FlyerCommonAI() {
    m_flyer = dynamic_cast<FlyerCommonBase *>(m_enemy->m_disp);
    m_rising = 1;
    m_wave = 0.0f;
}

void FlyerCommonAI::vfAI_0x58() {
    m_enemy->m_disp->vfDisp_0x4C();
    m_enemy->m_set->vfSet_0x10();
}

void EnemyAIState_FlyerCommonAI_WaitFloating::Enter() {
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_flyer->vfPara_0x48(), true);
    m_ai->m_rising = 1;
    m_ai->m_wave = 0.0f;
    fn_8040CE60(&m_ai->field_0x80);
    fn_8019B0F8(m_ai->m_enemy, &m_ai->m_enemy->field_0x60);
}

void EnemyAIState_FlyerCommonAI_WaitFloating::Update() {
    Vec3f direction;
    Vec3f move;
    void *key = m_ai->vfCtrl_0x10();
    if (m_ai->m_enemy->field_0xA8.Contains(key)) {
        fn_8019F53C(&m_ai->m_turn, (const Vec3f *)fn_8019F65C(&m_ai->m_target));
        if (m_ai->m_flyer->vf_0x23C()) {
            m_ai->vfAI_0x44();
            return;
        }
    } else {
        fn_800091EC(&direction, &(const Vec3f &)fn_8040D0AC(&(const Vec3f &)Vec3f(0.0f, 0.0f, -1.0f),
                                                             m_ai->m_enemy->GetHomeRotation()->y));
        fn_8019F2E0(&m_ai->m_turn, &direction);
    }
    if (m_ai->m_flyer->field_0x2DC > 0.1f) {
        float step = m_ai->m_flyer->vf_0x240() * m_ai->GetDeltaTime();
        if (m_ai->m_rising) {
            if ((m_ai->m_wave += step) > FloatWidth(m_ai)) {
                m_ai->m_rising = 0;
            }
        } else {
            if ((m_ai->m_wave -= step) < -FloatWidth(m_ai)) {
                m_ai->m_rising = 1;
            }
        }
        fn_800091EC(&move, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                               &m_ai->m_enemy->field_0x48, m_ai->m_enemy->GetPosition()));
        float width = m_ai->m_flyer->field_0x2DC;
        float wave = std::sin(0.5f * m_ai->m_wave * lbl_805FAD38 / width);
        wave *= width;
        switch (m_ai->m_flyer->field_0x2D8) {
        case 0:
            SubtractSide(&move, wave, m_ai->m_enemy->GetHomeRotation()->y);
            break;
        case 1:
            move.y += wave;
            break;
        case 2:
            AddSide(&move, wave, m_ai->m_enemy->GetHomeRotation()->y);
            break;
        case 3:
            move.y -= wave;
            break;
        }
        if (fn_8040CF50(&move) > 2.0f * step) {
            fn_8040CE74(&move);
            fn_8040CE38(&move, step);
        }
        AddVector(fn_80181F0C(m_ai->m_flyer), move);
    }
}

void EnemyAIState_FlyerCommonAI_WaitFloating::Exit() {}

void EnemyAIState_FlyerCommonAI_MoveOnPath::Enter() {
    m_ai->m_step = 0;
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), true);
    m_ai->m_flyer->SetMoveRate(1.0f);
    m_ai->m_flyer->field_0x30C = 1;
}

void EnemyAIState_FlyerCommonAI_MoveOnPath::Update() {
    switch (m_ai->m_step) {
    case 0:
        if (m_ai->m_flyer->IsInAttackRange()) {
            m_ai->m_step++;
            m_ai->m_flyer->vf_0x244();
            m_ai->SetTimer(m_ai->m_flyer->GetAttackTime());
        }
        break;
    case 1: {
        Vec3f target;
        fn_800091EC(&target, (const Vec3f *)fn_8019F65C(&m_ai->m_target));
        float rate;
        if (m_ai->field_0x70 > 0.01f) {
            rate = m_ai->field_0x6C / m_ai->field_0x70;
        } else {
            rate = 0.0f;
        }
        target.y += 10.0f * (1.0f - rate);
        fn_8019F2E0(&m_ai->m_turn, &target);
        if (!m_ai->m_flyer->IsInAttackRange()) {
            m_ai->m_step--;
            m_ai->m_flyer->vf_0x248();
        }
        break;
    }
    }
    m_ai->m_flyer->MoveAlongPath(m_ai->GetDeltaTime());
}

void EnemyAIState_FlyerCommonAI_MoveOnPath::Exit() {
    m_ai->m_flyer->field_0x30C = 0;
}
