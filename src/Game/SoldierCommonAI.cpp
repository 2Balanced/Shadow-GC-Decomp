/* SoldierCommonAI (class name from RTTI): the soldiers' AI (Game/EnemyAI.h), the base of the
 * Black Arms and G.U.N. soldiers' AIs (Game/BkSoldierAI.cpp, GunSoldier::GunSoldierAI). Its two
 * states wait at a patrol point (WaitAction_Standing: turn to the point's facing, play the wait
 * motions) and walk between the points (WaitAction_Moving); both hand over to the caution state
 * once the target is in the area. File name inferred from the class. Names marked "(name
 * inferred)" are not original. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
/* See SoldierCommonAI and EnemyBaseAI (Game/EnemyAI.h). */
#define SOLDIER_COMMON_AI_STATES
#define ENEMY_BASE_AI_HELPERS

#include "Game/EnemyAI.h"
#include "Game/EnemyAIState.h"
#include "Game/Fn8040CopyVector3.h"
#include "Game/MathTypes.h"

extern "C" {
/* Turns enemy to the angles at rotation (behaviour inferred). */
void fn_8019B0F8(Enemy *enemy, const Vec3f *rotation);
/* Picks the random point of a soldier that moves at random (Game/SoldierCommonBase.cpp). */
void fn_80198B5C(SoldierCommonBase *soldier);
/* A random chance by value (behaviour inferred). */
bool fn_8000A950(float value);
/* Vector routines that return their result (they store it through their first argument): v
 * turned about the y axis, v scaled to length 1. */
Vec3f fn_8040D0AC(const Vec3f *v, float angle);
Vec3f fn_8040CEE8(const Vec3f *v);
/* a . b */
float fn_8040CF90(const Vec3f *a, const Vec3f *b);
}

/* fn_8040CD74 as a function that returns its result (see Game/EnemyRender.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *lhs, const Vec3f *rhs);

/* The display's default for slot 0x30 (Game/Enemy.h; inline, first emitted here). */
inline void EnemyDispFunc::vfDisp_0x30(float value) {}

EnemyAIState_SoldierCommonAI_WaitAction_Standing::EnemyAIState_SoldierCommonAI_WaitAction_Standing()
    : m_ai(0) {}

void EnemyAIState_SoldierCommonAI_WaitAction_Standing::SetOwner(EnemyBaseAI *ai) {
    SoldierCommonAI *owner = dynamic_cast<SoldierCommonAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

EnemyAIState_SoldierCommonAI_WaitAction_Moving::EnemyAIState_SoldierCommonAI_WaitAction_Moving()
    : m_ai(0) {}

void EnemyAIState_SoldierCommonAI_WaitAction_Moving::SetOwner(EnemyBaseAI *ai) {
    SoldierCommonAI *owner = dynamic_cast<SoldierCommonAI *>(ai);
    if (owner != 0) {
        m_ai = owner;
    }
}

SoldierCommonAI::SoldierCommonAI() {
    m_soldier = dynamic_cast<SoldierCommonBase *>(m_enemy->m_disp);
}

/* The direction the enemy's home rotation faces (name inferred). */
static Fn8040CopyVector3 GetForward(SoldierCommonAI *ai) {
    Fn8040CopyVector3 forward;
    forward.x = 0.0f;
    forward.y = 0.0f;
    forward.z = -1.0f;
    fn_800091D0(reinterpret_cast<Vec3f *>(&forward),
                &(const Vec3f &)fn_8040D0AC(reinterpret_cast<Vec3f *>(&forward),
                                            ai->m_enemy->field_0x60.y));
    return forward;
}

void EnemyAIState_SoldierCommonAI_WaitAction_Standing::Enter() {
    m_ai->m_soldier->field_0x280 = 0;
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), true);
    fn_8017DE10(m_ai->m_enemy, &m_ai->m_enemy->field_0x48);
    m_ai->m_step = 0;
}

void EnemyAIState_SoldierCommonAI_WaitAction_Standing::Update() {
    switch (m_ai->m_step) {
    case 0:
        if (m_ai->m_enemy->vf_0x0C()) {
            fn_8019B0F8(m_ai->m_enemy, &m_ai->m_enemy->field_0x60);
            m_ai->m_step++;
        }
        break;
    case 1:
        if (m_ai->m_enemy->vf_0x10()) {
            m_ai->m_enemy->m_status->vfStat_0x20(
                m_ai->m_soldier->vf_0x238(m_ai->m_soldier->field_0x280), true);
            m_ai->SetTimer(5.0f);
            m_ai->m_step++;
        }
        m_ai->m_enemy->m_disp->vfDisp_0x3C(false);
        break;
    case 2:
        if (m_ai->IsTimeUp() || m_ai->m_enemy->m_status->vfStat_0x2C()) {
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x48(), true);
            m_ai->SetTimer(5.0f);
            m_ai->m_step++;
        }
        break;
    case 3:
        if (m_ai->IsTimeUp() || m_ai->m_enemy->m_status->vfStat_0x2C()) {
            m_ai->m_enemy->m_status->vfStat_0x20(
                m_ai->m_soldier->vf_0x238(m_ai->m_soldier->field_0x280), true);
            m_ai->SetTimer(5.0f);
            m_ai->m_step--;
        }
        break;
    }
    void *key = m_ai->vfCtrl_0x10();
    if (m_ai->m_enemy->field_0xA8.Contains(key)) {
        m_ai->vfAI_0x44();
    }
}

void EnemyAIState_SoldierCommonAI_WaitAction_Standing::Exit() {}

void EnemyAIState_SoldierCommonAI_WaitAction_Moving::Enter() {
    m_ai->m_step = 0;
    m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), true);
    m_ai->m_enemy->m_disp->vfDisp_0x3C(false);
    switch (m_ai->m_soldier->vf_0x23C()) {
    case 3:
        m_ai->m_soldier->field_0x280 = 3;
        break;
    case 1:
    case 2:
        m_ai->m_soldier->field_0x280 = 1;
        break;
    default:
        m_ai->m_soldier->field_0x280 = 0;
        break;
    }
    {
        int index;
        SoldierCommonBase *soldier = m_ai->m_soldier;
        index = soldier->field_0x280;
        if (index == 3) {
            fn_80198B5C(soldier);
        }
        fn_8017DE10(m_ai->m_enemy, &soldier->m_points[index].m_position);
    }
}

void EnemyAIState_SoldierCommonAI_WaitAction_Moving::Update() {
    void *key = m_ai->vfCtrl_0x10();
    if (m_ai->m_enemy->field_0xA8.Contains(key)) {
        m_ai->vfAI_0x44();
        return;
    }
    Vec3f direction;
    fn_800091EC(&direction, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                &m_ai->m_enemy->field_0x6C, m_ai->m_enemy->GetPosition()));
    direction.y = 0.0f;
    switch (m_ai->m_step) {
    case 0:
        if (fn_8040CF2C(&direction) < 100.0f) {
            m_ai->m_enemy->m_disp->vfDisp_0x3C(false);
        } else {
            float facing = fn_8040CF90(
                &(const Vec3f &)fn_8040CEE8(&direction),
                &(const Vec3f &)fn_8040D0AC(&(const Vec3f &)Vec3f(0.0f, 0.0f, -1.0f),
                                            m_ai->m_enemy->GetRotation()->y));
            if (facing < 0.0f) {
                facing = 0.2f;
            }
            m_ai->m_enemy->m_disp->vfDisp_0x30(facing);
        }
        if (fn_8019F53C(&m_ai->m_turn, &direction)) {
            m_ai->m_step = 1;
            m_ai->m_enemy->m_disp->vfDisp_0x3C(true);
            float speed = m_ai->m_enemy->m_para->vfPara_0x20();
            m_ai->SetTimer(3.0f + fn_8040CF50(&direction) / speed);
        }
        break;
    case 1:
        fn_8019F53C(&m_ai->m_turn, &direction);
        if (m_ai->IsTimeUp() || fn_8040CF50(&direction) < 10.0f) {
            m_ai->m_step = 2;
            m_ai->m_enemy->m_disp->vfDisp_0x30(0.2f);
        } else {
            float speed = m_ai->m_enemy->m_para->vfPara_0x20();
            if (fn_8040CF50(&direction) < speed) {
                if (fn_8000A950(m_ai->m_enemy->GetFrameTime())) {
                    m_ai->m_enemy->m_disp->vfDisp_0x30(0.0f);
                } else {
                    float full = m_ai->m_enemy->m_para->vfPara_0x20();
                    m_ai->m_enemy->m_disp->vfDisp_0x30(fn_8040CF50(&direction) / full);
                }
            }
        }
        break;
    case 2: {
        m_ai->m_enemy->m_disp->vfDisp_0x30(0.2f);
        if (fn_8019F53C(&m_ai->m_turn, reinterpret_cast<const Vec3f *>(
                                           &(const Fn8040CopyVector3 &)GetForward(m_ai)))) {
            m_ai->m_step = 3;
            m_ai->m_enemy->m_status->vfStat_0x20(
                m_ai->m_soldier->vf_0x238(m_ai->m_soldier->field_0x280), true);
            m_ai->SetTimer(m_ai->m_soldier->GetWaitTime(m_ai->m_soldier->GetPointIndex()));
            m_ai->m_enemy->m_disp->vfDisp_0x3C(true);
        }
        break;
    }
    case 3:
        if (m_ai->IsTimeUp()) {
            switch (m_ai->m_soldier->vf_0x23C()) {
            case 1:
                if (m_ai->m_soldier->field_0x280 == 1) {
                    m_ai->m_soldier->field_0x280 = 0;
                } else {
                    m_ai->m_soldier->field_0x280 = 1;
                }
                break;
            case 2:
                if (m_ai->m_soldier->field_0x280 == 1) {
                    m_ai->m_soldier->field_0x280 = 2;
                } else if (m_ai->m_soldier->field_0x280 == 2) {
                    m_ai->m_soldier->field_0x280 = 0;
                } else {
                    m_ai->m_soldier->field_0x280 = 1;
                }
                break;
            }
            m_ai->m_enemy->m_status->vfStat_0x20(m_ai->m_enemy->m_para->vfPara_0x4C(), true);
            int index;
            SoldierCommonBase *soldier = m_ai->m_soldier;
            index = soldier->field_0x280;
            if (index == 3) {
                fn_80198B5C(soldier);
            }
            fn_8017DE10(m_ai->m_enemy, &soldier->m_points[index].m_position);
            m_ai->m_step = 0;
        } else if (m_ai->m_enemy->m_status->vfStat_0x2C()) {
            m_ai->m_enemy->m_status->vfStat_0x20(
                m_ai->m_soldier->vf_0x238(m_ai->m_soldier->field_0x280), true);
        }
        break;
    }
}

void EnemyAIState_SoldierCommonAI_WaitAction_Moving::Exit() {
    m_ai->m_enemy->m_disp->vfDisp_0x3C(true);
}
