/* FlyerCommonBase (class name from RTTI): the body the Black Arms wings and the Gun Beetle share
 * (Game/EnemyFlyer.h). A flyer either follows a path (AppearType 1: it moves along the path at its
 * speed, turns along it, and attacks between AttackStart and AttackEnd of its length) or floats in
 * place. This unit holds the class's own code and its vtable; its inline code is emitted with
 * Game/BkWingLarge.cpp. File name inferred from the class. Names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first).
 *
 * NonMatching: 8 of 9 functions and all data match. fn_80193790: retail loads m_enemy into r5 for
 * the path set-up call (then r4 = +0x48, r5 = +0x60); GC/2.7 uses r4 under every form tried. */

#include "Game/EnemyFlyer.h"
#include "Game/MathTypes.h"

extern "C" {
/* The path: places position and rotation distance along it (false past its end), and sets it up
 * from the path pathId of the stage (behaviours inferred). */
bool fn_801A6588(UnkEnemyPath_801A6844 *path, Vec3f *position, Vec3f *rotation, float distance);
void fn_801A6688(UnkEnemyPath_801A6844 *path, Vec3f *position, Vec3f *rotation, int pathId,
                 bool mirror, int arg5);
/* The enemy's velocity (an inline function of the AI units, weak copy 0x80181F0C; behaviour
 * inferred). */
Vec3f *fn_80181F0C(Enemy *enemy);
/* Vector helpers: v normalized, v scaled, set from another, normalized in place, and set to zero
 * (behaviours inferred). */
Vec3f fn_8040CEE8(const Vec3f *v);
Vec3f fn_8040CDA8(const Vec3f *v, float scale);
Vec3f fn_800091A4(const Vec3f *v, float scale);
void fn_8040CFF8(Vec3f *out, const Vec3f *v);
void fn_8040CE74(Vec3f *v);
void fn_8040CE60(Vec3f *v);
/* The angles between a and b at t, for rotation order order (see BirdBase.cpp). */
Vec3f fn_8040F16C(const Vec3f *a, const Vec3f *b, int order, float t);
}

/* fn_8040CD74 as a function that returns its result (as in BkSoldier.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *lhs, const Vec3f *rhs);

/* The flyer's velocity, the three words at 0x300 (name inferred). */
static inline Vec3f *Velocity(FlyerCommonBase *flyer) {
    return reinterpret_cast<Vec3f *>(flyer->field_0x300);
}

/* The rotation order of the enemy's character (inline; weak copy 0x80193714; name inferred). */
inline s32 *GetRotationOrder(Enemy *enemy) {
    return &enemy->m_chara->field_0x30;
}

/* Whether the flyer is inside its attack stretch (written out where MoveAlongPath tests it). */
static inline bool InAttackRange(FlyerCommonBase *flyer) {
    if (flyer->vf_0x23C() && flyer->field_0x2E8 <= flyer->field_0x2E4 &&
        flyer->field_0x2E4 <= flyer->field_0x2EC) {
        return true;
    }
    return false;
}

void FlyerCommonBase::vfStat_0x38() {
    if (vf_0x238()) {
        void *key = m_enemy->m_ctrl->vfCtrl_0x10();
        if (!m_enemy->field_0xA8.Contains(key)) {
            m_enemy->m_status->vfStat_0x30(false);
            m_enemy->m_status->vfStat_0x3C(true);
            return;
        }
    }
    m_enemy->m_status->vfStat_0x30(true);
    m_enemy->m_status->vfStat_0x3C(false);
}

/* Sets the flyer up from its parameters (name kept: Game/BkWingLarge.cpp, Game/BkWingSmall.cpp
 * and GunBeetle call it by this name). */
extern "C" void fn_80193790(FlyerCommonBase *flyer, FlyerParam *param) {
    if (flyer->vf_0x238()) {
        fn_801A6688(&flyer->m_path, &flyer->m_enemy->field_0x48, &flyer->m_enemy->field_0x60,
                    param->m_path, param->PathMirror != 0, 0);
        flyer->field_0x2E0 = param->MoveSpeedRatio * flyer->m_enemy->m_para->vfPara_0x20();
        flyer->field_0x2E4 = 0.0f;
        fn_8040CE60(Velocity(flyer));
        fn_801A6588(&flyer->m_path, flyer->m_enemy->GetPosition(), flyer->m_enemy->GetRotation(),
                    0.0f);
        if (flyer->vf_0x23C()) {
            flyer->field_0x2E8 = param->AttackStart * flyer->m_path.field_0x58;
            flyer->field_0x2EC = param->AttackEnd * flyer->m_path.field_0x58;
            if (flyer->field_0x2E0 > 0.1f) {
                float range = param->AttackEnd - param->AttackStart;
                flyer->field_0x2F0 = range * flyer->m_path.field_0x58 / flyer->field_0x2E0;
            } else {
                flyer->field_0x2F0 = 1.0f;
            }
        }
    } else {
        flyer->field_0x2D8 = param->m_path;
        flyer->field_0x2DC = param->FloatWidth;
    }
    flyer->field_0x2F4 = 0;
}

bool FlyerCommonBase::IsInAttackRange() {
    return InAttackRange(this);
}

void FlyerCommonBase::MoveAlongPath(float dt) {
    Vec3f position;
    float step = field_0x2E0 * dt;
    field_0x2E4 += step;
    Vec3f rotation(0.0f, 0.0f, 0.0f);
    switch (field_0x2F4) {
    case 0:
        if (!fn_801A6588(&m_path, &position, &rotation, field_0x2E4)) {
            fn_800091D0(Velocity(this), &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                            &position, m_enemy->GetPosition()));
            if (fn_8040CF2C(Velocity(this)) > step * step) {
                fn_800091D0(Velocity(this),
                            &(const Vec3f &)fn_8040CDA8(
                                &(const Vec3f &)fn_8040CEE8(Velocity(this)), step));
            }
            fn_8040CFF8(fn_80181F0C(m_enemy), Velocity(this));
            if (!InAttackRange(this)) {
                if ((field_0x2F8 -= dt * field_0x2FC) > 0.0f) {
                    Vec3f from(0.0f, 0.0f, 0.0f);
                    from = rotation;
                    Vec3f to(0.0f, 0.0f, 0.0f);
                    to = *m_enemy->GetRotation();
                    rotation = fn_8040F16C(&from, &to, *GetRotationOrder(m_enemy), field_0x2F8);
                }
                *m_enemy->GetRotation() = rotation;
            } else {
                field_0x2F8 = 1.0f;
                field_0x2FC = 1.0f;
            }
            if (field_0x2E4 > m_path.field_0x58) {
                field_0x2E4 -= m_path.field_0x58;
            }
            break;
        }
        fn_8040CE74(Velocity(this));
        field_0x2F4 = 1;
    case 1:
        fn_8040CFF8(fn_80181F0C(m_enemy),
                    &(const Vec3f &)fn_800091A4(
                        &(const Vec3f &)fn_800091A4(Velocity(this), field_0x2E0), dt));
        break;
    }
}

void FlyerCommonBase::GetAttackEndPoint(Vec3f *position) {
    fn_801A6588(&m_path, position, 0, field_0x2EC);
}
