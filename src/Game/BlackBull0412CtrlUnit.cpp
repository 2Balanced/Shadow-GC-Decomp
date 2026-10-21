/* Control units of the Black Bull boss's 0412 stage (class names from RTTI): the camera that keeps
 * the player and the boss in view, and the choice of the boss's motion while it moves. File name
 * inferred from the classes; all member names and the names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/BlackBullCtrlUnit.h"
#include "Game/BossCtrlUnit.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "size_assert.h"

/* MSL's std::fabs for floats, which the game calls out of line (as in DevilDoomCtrlUnit.cpp). */
namespace std {
float fabs(float x);
} // namespace std

extern "C" {
/* Motions (see DevilDoomCtrlUnit.cpp and ChaoHolder.cpp); fn_80415CDC returns the motion playing
 * on track arg1 (behaviour inferred). */
int fn_80415CDC(_stMtnPlayWork *work, int arg1);
int fn_80416F28(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
void fn_804165D8(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
void fn_80417568(_stMtnPlayWork *work, u16 motion);
void fn_80416148(_stMtnPlayWork *work, int arg1, float arg2);
/* A random number in [0, 1) (see BalloonGenerator.cpp). */
float fn_8040EE90(void);
}

/* Degrees to radians and back (names inferred; as in DevilDoomCtrlUnit.cpp). */
inline float ToRadian(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

inline float ToDegree(float radians) {
    return radians * (180.0f / lbl_805FAD38);
}

/* Whether the boss plays motion (name inferred). */
inline bool IsPlaying(BossVarParam *param, u16 motion) {
    return motion == (u16)fn_80415CDC(param->m_motion, 0);
}

class BlackBull0412CtrlUnit_FixLookCharCamera : public BlackBull0412CtrlUnit,
                                                public BossCtrlUnit_FixLookCharCamera {
  public:
    BlackBull0412CtrlUnit_FixLookCharCamera(BlackBull0412VarParam *param);
    virtual ~BlackBull0412CtrlUnit_FixLookCharCamera() {}
    virtual int Tick(float dt) {
        return BossCtrlUnit_FixLookCharCamera::Tick(dt);
    }
    virtual bool IsEnabled();

    /* 0x40 */ s32 m_view; /* 0 none, 1 level, 2 below (meanings inferred) */
};

/* Picks the boss's motion from how fast it turns: a turn when it turns fast, otherwise a run, or
 * now and then one of three idle motions (meanings inferred). */
class BlackBull0412CtrlUnit_MoveMotion : public BlackBull0412CtrlUnit {
  public:
    BlackBull0412CtrlUnit_MoveMotion(BlackBull0412VarParam *param);
    virtual ~BlackBull0412CtrlUnit_MoveMotion();
    virtual int Tick(float dt);
};

BlackBull0412CtrlUnit_MoveMotion::BlackBull0412CtrlUnit_MoveMotion(BlackBull0412VarParam *param)
    : BossCtrlUnit(param), BlackBull0412CtrlUnit(param) {
    m_kind = 2;
}

BlackBull0412CtrlUnit_MoveMotion::~BlackBull0412CtrlUnit_MoveMotion() {}

int BlackBull0412CtrlUnit_MoveMotion::Tick(float dt) {
    if (m_owner->m_motionLocked || m_owner->m_motionLocked2) {
        return false;
    }
    u32 motion = 0;
    u16 current = fn_80415CDC(m_owner->m_motion, 0);
    if (ToDegree(std::fabs(m_owner->m_turnSpeed)) > m_owner->m_turnMotionSpeed) {
        if (m_owner->m_turnSpeed < ToRadian(-m_owner->m_turnSpeedMax)) {
            motion = 0xF;
        } else if (m_owner->m_turnSpeed > ToRadian(m_owner->m_turnSpeedMax)) {
            motion = 0xB;
        } else if (current != (u16)(m_owner->m_motionBase + 7) &&
                   current != (u16)(m_owner->m_motionBase + 8) &&
                   current != (u16)(m_owner->m_motionBase + 9) &&
                   current != (u16)(m_owner->m_motionBase + 5)) {
            motion = 5;
        }
    } else if (fn_80416F28(m_owner->m_motion, 0, 3, 1)) {
        switch ((int)(fn_8040EE90() * 6.0f)) {
        case 0:
            motion = 7;
            break;
        case 1:
            motion = 8;
            break;
        case 2:
            motion = 9;
            break;
        default:
            motion = 5;
            break;
        }
    }
    if (motion != 0 && current != (u16)(m_owner->m_motionBase + motion)) {
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + motion);
        if (motion == 7 || IsPlaying(m_owner, m_owner->m_motionBase + 7)) {
            fn_80416148(m_owner->m_motion, 0, 10.0f);
        } else {
            fn_80416148(m_owner->m_motion, 0, 40.0f);
        }
    }
    return false;
}

BlackBull0412CtrlUnit_FixLookCharCamera::BlackBull0412CtrlUnit_FixLookCharCamera(
    BlackBull0412VarParam *param)
    : BossCtrlUnit(param), BlackBull0412CtrlUnit(param), BossCtrlUnit_FixLookCharCamera(param),
      m_view(0) {}

bool BlackBull0412CtrlUnit_FixLookCharCamera::IsEnabled() {
    if (m_owner->field_0x15C < m_owner->field_0xB0 && m_owner->m_height < -30.0f) {
        m_view = 0;
        return false;
    }
    switch (m_view) {
    case 0:
        if (m_owner->m_height < -30.0f) {
            field_0x8 = 80.0f;
            field_0xC = 20.0f;
            fn_800091D0(&m_offset, &lbl_805E07A0);
            m_offset.y = -150.0f;
            m_view = 2;
        } else {
            field_0x8 = 60.0f;
            field_0xC = 16.0f;
            fn_800091D0(&m_offset, &lbl_805E07A0);
            m_view = 1;
        }
        break;
    case 1:
        if (m_owner->m_height < -30.0f) {
            m_view = 0;
            return false;
        }
        break;
    case 2:
        if (m_owner->m_height >= -30.0f) {
            m_view = 0;
            return false;
        }
        break;
    }
    return true;
}
