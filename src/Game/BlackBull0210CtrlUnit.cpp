/* Control units of the Black Bull boss's 0210 stage (class names from RTTI): the bases
 * BlackBullCtrlUnit and BlackBull0210CtrlUnit, the camera that keeps the player and the boss in
 * view, and the choice of the boss's motion while it moves (as in Game/BlackBull0412CtrlUnit.cpp).
 * File name inferred from the classes; all member names and the names marked "(name inferred)"
 * are not original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/BlackBull0210CtrlUnit.h"
#include "Game/BlackBullCtrlUnit.h"
#include "Game/BossCtrlUnit.h"
#include "Game/CharInfo.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "size_assert.h"

/* MSL's std::fabs for floats, which the game calls out of line (as in DevilDoomCtrlUnit.cpp). */
namespace std {
float fabs(float x);
} // namespace std

/* The character manager (class unknown; see SetCharacter.cpp). Its byte at 0xC is cleared every
 * frame the boss moves (meaning unknown). */
struct UnkCharManager_8006F754 {
    /* 0x0 */ u8 field_0x0[0xC];
    /* 0xC */ bool field_0xC;
};

extern "C" {
UnkCharManager_8006F754 *fn_8006F754(void);
/* Motions (see BlackBull0412CtrlUnit.cpp). */
int fn_80415CDC(_stMtnPlayWork *work, int arg1);
int fn_80416F28(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
void fn_804165D8(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
void fn_80417568(_stMtnPlayWork *work, u16 motion);
void fn_80416148(_stMtnPlayWork *work, int arg1, float arg2);
/* A random number in [0, 1). */
float fn_8040EE90(void);
}

/* Degrees to radians and back (names inferred; as in BlackBull0412CtrlUnit.cpp). */
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

/* The bases' constructors and destructors are inline: each is emitted as a weak copy after its
 * first user, and the linked copies are this unit's (the Black Bull units elsewhere call them). They
 * are declared out of line in the headers so that the other units that include them keep their
 * compiler-generated @N locals. */
inline BlackBullCtrlUnit::BlackBullCtrlUnit(BlackBullVarParam *owner)
    : BossCtrlUnit(m_owner), m_owner(owner) {}

inline BlackBullCtrlUnit::~BlackBullCtrlUnit() {}

inline BlackBull0210CtrlUnit::BlackBull0210CtrlUnit(BlackBull0210VarParam *owner)
    : BossCtrlUnit(owner), BlackBullCtrlUnit(owner), m_owner(owner) {}

inline BlackBull0210CtrlUnit::~BlackBull0210CtrlUnit() {}

SIZE_ASSERT(BlackBull0210CtrlUnit_FixLookCharCamera, 0x50);
SIZE_ASSERT(BlackBull0210CtrlUnit_MoveMotion, 0x20);

BlackBull0210CtrlUnit_MoveMotion::BlackBull0210CtrlUnit_MoveMotion(BlackBull0210VarParam *param)
    : BossCtrlUnit(param), BlackBull0210CtrlUnit(param) {
    m_kind = 2;
}

BlackBull0210CtrlUnit_MoveMotion::~BlackBull0210CtrlUnit_MoveMotion() {}

int BlackBull0210CtrlUnit_MoveMotion::Tick(float dt) {
    UnkCharManager_8006F754 *manager = fn_8006F754();
    manager->field_0xC = false;
    if (m_owner->m_motionLocked || m_owner->m_motionLocked2) {
        return false;
    }
    u32 motion = 0;
    u16 current = fn_80415CDC(m_owner->m_motion, 0);
    if (ToDegree(std::fabs(m_owner->m_turnSpeed)) > m_owner->m_turnMotionSpeed) {
        if (m_owner->m_turnSpeed < ToRadian(-m_owner->m_turnSpeedMax)) {
            motion = 0xC;
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

BlackBull0210CtrlUnit_FixLookCharCamera::BlackBull0210CtrlUnit_FixLookCharCamera(
    BlackBull0210VarParam *param)
    : BossCtrlUnit(param), BlackBull0210CtrlUnit(param), BossCtrlUnit_FixLookCharCamera(param) {
    field_0x8 = 60.0f;
    field_0xC = 20.0f;
    fn_800091D0(&m_offset, &lbl_805E07A0);
}

bool BlackBull0210CtrlUnit_FixLookCharCamera::IsEnabled() {
    if (m_owner->field_0x15C < m_owner->m_cameraNear &&
        m_owner->m_height < reinterpret_cast<CharInfo *>(m_owner->m_chara)->m_position.y) {
        return false;
    }
    if (m_owner->field_0x15C > m_owner->m_cameraFar) {
        return false;
    }
    return true;
}

BlackBull0210CtrlUnit_FixLookCharCamera::~BlackBull0210CtrlUnit_FixLookCharCamera() {}

int BlackBull0210CtrlUnit_FixLookCharCamera::Tick(float dt) {
    return BossCtrlUnit_FixLookCharCamera::Tick(dt);
}
