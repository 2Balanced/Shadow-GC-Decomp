/* Control units of the Heavy Dog boss's 0510 stage (class names from RTTI): the camera that keeps
 * the player and the boss in view, the bombs the boss throws and the boss's movement. File name
 * inferred from the classes; all member names and the names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/BossCtrlUnit.h"
#include "Game/CharInfo.h"
#include "Game/EnemyCharInfo.h"
#include "Game/HeavyDogCtrlUnit.h"
#include "Game/MathConstants.h"
#include "Game/Task.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* MSL's std::sin, std::cos and std::fabs for floats, which the game calls out of line (as in
 * DevilDoomCtrlUnit.cpp). */
namespace std {
float sin(float x);
float cos(float x);
float fabs(float x);
} // namespace std

/* The parameters of a bomb's throw (struct name unknown; layout from its constructor, 0x80194C1C;
 * as in EggLastMech.cpp). */
struct UnkEffectParam_80194C1C {
    UnkEffectParam_80194C1C(const Vec3f &position, const Vec3f &velocity, float scale,
                            bool flag);

    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ Vec3f m_velocity;
    /* 0x18 */ float m_scale;
    /* 0x1C */ float field_0x1C;
    /* 0x20 */ u8 m_flag;
};
SIZE_ASSERT(UnkEffectParam_80194C1C, 0x24);

/* ThrowBomb (class name from RTTI, the base of ThrowBombImpl; vtable of ThrowBombImpl at
 * 0x805442AC): a bomb that something throws. Only the virtual functions used here are named
 * (names inferred). */
class ThrowBomb {
  public:
    virtual ~ThrowBomb();
    virtual void Throw(const UnkEffectParam_80194C1C &param);
    virtual void Remove();
};

extern "C" {
/* Creates a bomb under layer (behaviour inferred). */
boost::weak_ptr<ThrowBomb> fn_80209908(Task *layer, int arg1);

/* Sounds (see BAGunShip.cpp). */
void *fn_80053164(void);
void fn_801D53C4(void *manager, int id, const Vec3f *position, void *source, int arg4,
                 int arg5, int arg6);

/* Math (behaviour inferred). */
void fn_8040CDA8(Vec3f *result, const Vec3f *vector, float scale);
}

class HeavyDog0510CtrlUnit_FixLookCharCamera : public HeavyDog0510CtrlUnit,
                                               public BossCtrlUnit_FixLookCharCamera {
  public:
    HeavyDog0510CtrlUnit_FixLookCharCamera(HeavyDog0510VarParam *param);
    virtual ~HeavyDog0510CtrlUnit_FixLookCharCamera() {}
    virtual int Tick(float dt) {
        return BossCtrlUnit_FixLookCharCamera::Tick(dt);
    }
    virtual bool IsEnabled();

    /* 0x40 */ s32 m_view; /* 0 none, 1 above, 2 level, 3 below (meanings inferred) */
};

class HeavyDog0510CtrlUnit_Bomb : public HeavyDog0510CtrlUnit {
  public:
    HeavyDog0510CtrlUnit_Bomb(HeavyDog0510VarParam *param, const Vec3f &position,
                              const Vec3f &velocity);
    virtual ~HeavyDog0510CtrlUnit_Bomb();
    virtual int Tick(float dt);

    /* 0x10 */ boost::weak_ptr<ThrowBomb> m_bomb;
};

class HeavyDog0510CtrlUnit_Move : public HeavyDog0510CtrlUnit {
  public:
    HeavyDog0510CtrlUnit_Move(HeavyDog0510VarParam *param);
    virtual ~HeavyDog0510CtrlUnit_Move();
    virtual int Tick(float dt);
};

HeavyDog0510CtrlUnit_Move::HeavyDog0510CtrlUnit_Move(HeavyDog0510VarParam *param)
    : BossCtrlUnit(param), HeavyDog0510CtrlUnit(param) {
    m_kind = 2;
}

HeavyDog0510CtrlUnit_Move::~HeavyDog0510CtrlUnit_Move() {}

/* The step that brings an angle back towards [-pi, pi] (as in DevilDoomCtrlUnit.cpp). */
inline float WrapStep(float angle) {
    return angle > 0.0f ? -2.0f * lbl_805FAD38 : 2.0f * lbl_805FAD38;
}

#define WRAP_ANGLE(angle)                                                                          \
    while (std::fabs(angle) > lbl_805FAD38) {                                                      \
        angle += WrapStep(angle);                                                                  \
    }

int HeavyDog0510CtrlUnit_Move::Tick(float dt) {
    if (!m_owner->m_fixed) {
        Vec3f velocity;
        fn_800091EC(&velocity, &m_owner->m_velocity);
        /* A damped spring towards the target height, and below one towards the target angle (the
         * statement split gives retail's register use). */
        float offset = m_owner->m_targetPosition.y - m_owner->m_chara->m_position.y;
        float force = offset * m_owner->m_spring;
        force -= m_owner->m_velocity.y * m_owner->m_damping;
        m_owner->m_velocity.y += force * dt;
        m_owner->m_chara->m_position.y += m_owner->m_velocity.y * dt;

        Vec3f position;
        fn_800091EC(&position, &m_owner->m_chara->m_position);
        position.y = 0.0f;
        m_owner->m_radius = fn_8040CF50(&position);
        float diff = m_owner->m_targetRadius - m_owner->m_radius;
        float step = dt * m_owner->m_radiusSpeed;
        if (std::fabs(diff) < step) {
            m_owner->m_radius = m_owner->m_targetRadius;
        } else {
            m_owner->m_radius += step * (diff > 0.0 ? 1.0f : -1.0f);
        }

        float angle = m_owner->m_targetAngle - m_owner->m_angle;
        WRAP_ANGLE(angle);
        float torque = angle * m_owner->m_angleSpring;
        torque -= m_owner->m_angleSpeed * m_owner->m_angleDamping;
        m_owner->m_angleSpeed += torque * dt;
        m_owner->m_angle += m_owner->m_angleSpeed * dt;
        WRAP_ANGLE(m_owner->m_angle);

        /* Retail keeps the character info, not the owner, across the sine and cosine calls. */
        {
            EnemyCharInfo *info = m_owner->m_chara;
            info->m_position.x = -m_owner->m_radius * std::sin(m_owner->m_angle);
        }
        {
            EnemyCharInfo *info = m_owner->m_chara;
            info->m_position.z = -m_owner->m_radius * std::cos(m_owner->m_angle);
        }
        m_owner->m_velocity.x = -m_owner->m_radius * m_owner->m_angleSpeed * std::cos(m_owner->m_angle);
        m_owner->m_velocity.z = m_owner->m_radius * m_owner->m_angleSpeed * std::sin(m_owner->m_angle);

        Vec3f acceleration;
        Vec3f delta;
        fn_8040CD74(&delta, &m_owner->m_velocity, &velocity);
        fn_8040CDA8(&acceleration, &delta, 1.0f / dt);
        fn_800091D0(&m_owner->m_acceleration, &acceleration);
        Vec3f move;
        fn_8040CDA8(&move, &m_owner->m_velocity, dt);
        m_owner->m_distance += fn_8040CF50(&move);
        if (fn_8040CF50(&m_owner->m_velocity) > m_owner->m_soundSpeed) {
            fn_801D53C4(fn_80053164(), 0x7D43, &m_owner->m_chara->m_position, this, 3, 0, 3);
        }
    } else {
        fn_800091D0(&m_owner->m_velocity, &lbl_805E07A0);
        fn_800091D0(&m_owner->m_acceleration, &lbl_805E07A0);
    }
    m_owner->m_targetDistance = std::fabs(m_owner->m_targetPosition.y - m_owner->m_chara->m_position.y);
    return false;
}

HeavyDog0510CtrlUnit_Bomb::HeavyDog0510CtrlUnit_Bomb(HeavyDog0510VarParam *param,
                                                     const Vec3f &position,
                                                     const Vec3f &velocity)
    : BossCtrlUnit(param), HeavyDog0510CtrlUnit(param) {
    m_kind = 2;
    m_bomb = fn_80209908(m_owner->m_bombLayer, 0);
    boost::shared_ptr<ThrowBomb> bomb(m_bomb.lock());
    if (bomb) {
        /* A named parameter: retail reloads the bomb pointer after building it. */
        UnkEffectParam_80194C1C throwParam(position, velocity, m_owner->m_bombScale, true);
        bomb->Throw(throwParam);
    }
}

HeavyDog0510CtrlUnit_Bomb::~HeavyDog0510CtrlUnit_Bomb() {
    boost::shared_ptr<ThrowBomb> bomb(m_bomb.lock());
    if (bomb) {
        bomb->Remove();
    }
}

int HeavyDog0510CtrlUnit_Bomb::Tick(float dt) {
    /* Finished once the bomb is gone. Retail narrows the flag to a byte and back to bool before
     * returning it, which the casts reproduce. */
    bool done = !m_bomb.lock();
    return (bool)(u8)done;
}

HeavyDog0510CtrlUnit_FixLookCharCamera::HeavyDog0510CtrlUnit_FixLookCharCamera(
    HeavyDog0510VarParam *param)
    : BossCtrlUnit(param), HeavyDog0510CtrlUnit(param), BossCtrlUnit_FixLookCharCamera(param),
      m_view(0) {}

bool HeavyDog0510CtrlUnit_FixLookCharCamera::IsEnabled() {
    if (m_owner->field_0x1B0 < m_owner->field_0xB0 &&
        m_owner->m_height < m_owner->m_chara->m_position.y) {
        m_view = 0;
        return false;
    }
    if (m_owner->m_chara->m_position.y > m_owner->field_0x1F0 && m_owner->m_height < -52.0f) {
        m_view = 0;
        return false;
    }
    switch (m_view) {
    case 0:
        if (HeavyDogCtrlUnit::m_owner->m_height < -52.0f) {
            field_0x8 = 120.0f;
            field_0xC = 30.0f;
            fn_800091D0(&m_offset, &lbl_805E07A0);
            m_offset.y = -80.0f;
            m_view = 3;
        } else if (HeavyDogCtrlUnit::m_owner->m_height > 200.0f) {
            field_0x8 = 120.0f;
            field_0xC = 30.0f;
            fn_800091D0(&m_offset, &lbl_805E07A0);
            m_view = 1;
        } else {
            field_0x8 = 60.0f;
            field_0xC = 16.0f;
            fn_800091D0(&m_offset, &lbl_805E07A0);
            m_view = 2;
        }
        break;
    case 1:
        if (HeavyDogCtrlUnit::m_owner->m_height < 200.0f) {
            m_view = 0;
            return false;
        }
        break;
    case 2:
        if (HeavyDogCtrlUnit::m_owner->m_height > 200.0f ||
            HeavyDogCtrlUnit::m_owner->m_height < -52.0f) {
            m_view = 0;
            return false;
        }
        break;
    case 3:
        if (HeavyDogCtrlUnit::m_owner->m_height > -52.0f) {
            m_view = 0;
            return false;
        }
        break;
    }
    return true;
}
