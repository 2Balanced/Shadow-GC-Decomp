/* PJSCamera::CameraModeLookFront (class name and bases from RTTI; mode 11 of the camera factory): a
 * camera that looks the way the player moves. Its work object (UnkLookFrontCamera_801C557C, class
 * name unknown) keeps a direction, turns it towards the player's velocity at limited speeds (about
 * the vertical and then the side axis) and puts the point looked at ahead of the player along it,
 * the eye behind that point and above the player. The six parameters come from the mode's
 * arguments, or from defaults. File name from the class; other names are inferred. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/CameraModeLookFront.h"
#include "Game/CameraState.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/PJSCameraMode.h"
#include "Game/Player.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using PJSCamera::CameraModeLookFront;
using PJSCamera::PJSCameraMode;
using SonicteamUSA::System::Camera::CameraMode;

extern "C" {
/* The player as a Player::Player (boost::dynamic_pointer_cast's instance; see
 * Game/CameraModeGeneral.cpp). */
boost::shared_ptr<Player::Player> fn_800DFAD4(const boost::shared_ptr<CharInfo> &chara);
/* The character of a screen (behaviour inferred). */
boost::weak_ptr<CharInfo> fn_800A8C54(UnkCharaList_80074194 *list, int index);
/* A player's front and up vectors (behaviour inferred). */
Vec3f fn_800796DC(Player::PlayerBase *player);
Vec3f fn_80079698(Player::PlayerBase *player);
/* Sets a camera placement's view window (behaviour from its body). */
void fn_8000AF60(UnkCameraState_8000AA70 *state, const Vec2f *window);
/* Whether value is about zero (behaviour inferred). */
bool fn_8000B044(float value);

/* Vector routines (behaviour inferred). */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);  /* a + b */
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b);  /* a x b */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* vector += value */
void fn_8040CE38(Vec3f *vector, float scale);        /* vector *= scale */
float fn_8040CE74(Vec3f *vector);                    /* normalises */
float fn_8040F8FC(const Vec3f *a, const Vec3f *b);   /* the angle between */
/* vector turned by angle about the y and the x axis (behaviour inferred). */
Vec3f fn_8040D0AC(const Vec3f *vector, float angle);
Vec3f fn_8040D118(const Vec3f *vector, float angle);
}

/* fn_8040CD74 called as returning a - b by value (see Game/ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

extern const float lbl_805FAD38; /* pi */

/* Degrees to radians. */
inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* vector += value through the out-of-line addition (fn_8040CFF8). */
inline Vec3f &operator+=(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
    return vector;
}

/* The view window of a mode's arguments (behaviour from their bodies; emitted out of line here,
 * where they are first used). */
extern "C" inline Vec2f fn_801C51C0(const UnkCameraModeArgs_8000EBB0 *args) {
    return Vec2f(args->m_view.m_windowX, args->m_view.m_windowY);
}

extern "C" inline Vec2f fn_801C5194(const UnkCameraModeArgs_8000EBB0 *args) {
    return fn_801C51C0(args);
}

/* A camera mode starts with its placement. */
inline UnkCameraState_8000AA70 *ModeState(CameraMode *mode) {
    return reinterpret_cast<UnkCameraState_8000AA70 *>(mode);
}

/* The game sets these in __sinit rather than in .sdata, as an initializer that is not a constant
 * expression does; the inline call reproduces that (as in Game/PlayerBehaviorSuper.cpp). */
inline float DynamicFloat(float value) {
    return value;
}

/* The defaults of the first three parameters, and the speed below which the direction is kept
 * (names inferred). */
static const float s_lookAhead = DynamicFloat(200.0f);
static const float s_eyeDistance = DynamicFloat(25.0f);
static const float s_eyeHeight = DynamicFloat(5.0f);
static const float s_minSpeed = DynamicFloat(70.0f);

/* The work object of CameraModeLookFront (class name unknown; names inferred). */
class UnkLookFrontCamera_801C557C {
  public:
    UnkLookFrontCamera_801C557C();
    ~UnkLookFrontCamera_801C557C() {}

    void Track(float dt, CameraMode *mode, boost::shared_ptr<Player::Player> player);
    void Turn(float maxYaw, float maxPitch, const Vec3f *velocity);

    /* 0x00 */ float m_lookAhead;  /* the point looked at, ahead of the player */
    /* 0x04 */ float m_eyeDistance; /* the eye, behind that point */
    /* 0x08 */ float m_eyeHeight;   /* above the player */
    /* 0x0C */ float m_yawSpeed;    /* degrees a second */
    /* 0x10 */ float m_pitchSpeed;  /* degrees a second */
    /* 0x14 */ float m_maxYaw;      /* degrees; larger turns are ignored */
    /* 0x18 */ Vec3f m_direction;
    /* 0x24 */ u32 field_0x24;
};
SIZE_ASSERT(UnkLookFrontCamera_801C557C, 0x28);

UnkLookFrontCamera_801C557C::UnkLookFrontCamera_801C557C()
    : m_lookAhead(s_lookAhead), m_eyeDistance(s_eyeDistance), m_eyeHeight(s_eyeHeight),
      m_yawSpeed(120.0f), m_pitchSpeed(60.0f), m_maxYaw(155.0f) {}

void UnkLookFrontCamera_801C557C::Turn(float maxYaw, float maxPitch, const Vec3f *velocity) {
    if (!fn_8000B044(fn_8040CF50(velocity))) {
        Vec3f move;
        fn_800091EC(&move, velocity);
        move.y = 0.0f;
        fn_8040CE74(&move);
        Vec3f front;
        fn_800091EC(&front, &m_direction);
        front.y = 0.0f;
        fn_8040CE74(&front);
        float yaw = fn_8040F8FC(&front, &move);
        if (yaw > m_maxYaw * (lbl_805FAD38 / 180.0f)) {
            yaw = 0.0f;
        }
        if (yaw > maxYaw) {
            yaw = maxYaw;
        }
        if (!fn_8000B044(yaw)) {
            Vec3f axis;
            fn_800091EC(&axis, &(const Vec3f &)fn_8040CFB8(&front, &move));
            axis.y = axis.y > 0.0f ? 1.0f : -1.0f;
            yaw *= axis.y;
            fn_800091D0(&m_direction, &(const Vec3f &)fn_8040D0AC(&m_direction, yaw));
        }
        Vec3f climb;
        fn_800091EC(&climb, velocity);
        climb.x = 0.0f;
        fn_8040CE74(&climb);
        Vec3f side;
        fn_800091EC(&side, &m_direction);
        side.x = 0.0f;
        fn_8040CE74(&side);
        float pitch = fn_8040F8FC(&side, &climb);
        if (pitch > 89.0f * (lbl_805FAD38 / 180.0f)) {
            pitch = 0.0f;
        } else if (pitch > maxPitch) {
            pitch = maxPitch;
        }
        if (!fn_8000B044(pitch)) {
            Vec3f axis;
            fn_800091EC(&axis, &(const Vec3f &)fn_8040CFB8(&side, &climb));
            axis.x = axis.x > 0.0f ? 1.0f : -1.0f;
            pitch *= axis.x;
            fn_800091D0(&m_direction, &(const Vec3f &)fn_8040D118(&m_direction, pitch));
        }
        fn_8040CE74(&m_direction);
    }
}

void UnkLookFrontCamera_801C557C::Track(float dt, CameraMode *mode,
                                        boost::shared_ptr<Player::Player> player) {
    if (fn_8040CF50(&player->field_0x18) > s_minSpeed) {
        Turn(dt * DegToRad(m_yawSpeed), dt * DegToRad(m_pitchSpeed), &player->field_0x18);

    }
    Vec3f ahead;
    fn_800091EC(&ahead, &m_direction);
    fn_8040CE38(&ahead, m_lookAhead);
    Vec3f target;
    fn_800091EC(&target, &(const Vec3f &)fn_8040CDD0(&ahead, &player->m_position));
    Vec3f up;
    fn_800091EC(&up, &(const Vec3f &)fn_80079698(player.get()));
    fn_8040CE74(&up);
    fn_8040CE38(&up, m_eyeHeight);
    up += player->m_position;
    Vec3f back;
    fn_800091EC(&back, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&up, &target));
    fn_8040CE74(&back);
    fn_8040CE38(&back, m_eyeDistance);
    fn_8000A5D4(mode, &(const Vec3f &)fn_8040CDD0(&back, &up), &target);
}

namespace PJSCamera {

CameraModeLookFront::CameraModeLookFront(UnkCameraOwner_8000E9BC *owner, int number)
    : PJSCameraMode(owner, number), m_impl(new UnkLookFrontCamera_801C557C) {
    if (owner != 0) {
        const UnkCameraModeArgs_8000EBB0 *args =
            reinterpret_cast<const UnkCameraModeArgs_8000EBB0 *>(owner);
        fn_8000AF60(ModeState(this), &(const Vec2f &)fn_801C5194(args));
        m_impl->m_lookAhead = args->m_distance;
        m_impl->m_eyeDistance = args->m_height;
        m_impl->m_eyeHeight = args->field_0x54;
        m_impl->m_yawSpeed = args->field_0x58;
        m_impl->m_pitchSpeed = args->field_0x5C;
        m_impl->m_maxYaw = args->field_0x60;
    }
}

CameraModeLookFront::~CameraModeLookFront() {}

void CameraModeLookFront::Update(float dt, UnkCameraUpdateArg_800099B8 *arg) {
    boost::shared_ptr<CharInfo> chara(fn_800A8C54(fn_80074194(), field_0xA0).lock());
    boost::shared_ptr<Player::Player> player(fn_800DFAD4(chara));
    if (player.get() == 0) {
        return;
    }
    m_impl->Track(dt, this, player);
}

void CameraModeLookFront::OnSwitch(UnkCameraSwitchArg_8000996C *arg) {
    boost::shared_ptr<CharInfo> chara(fn_800A8C54(fn_80074194(), field_0xA0).lock());
    boost::shared_ptr<Player::Player> player(fn_800DFAD4(chara));
    if (player.get() != 0) {
        if (fn_8040CF50(&player->field_0x18) > 1.0f) {
            fn_800091D0(&m_impl->m_direction, &player->field_0x18);
        } else {
            fn_800091D0(&m_impl->m_direction, &(const Vec3f &)fn_800796DC(player.get()));
        }
        fn_8040CE74(&m_impl->m_direction);
    } else {
        fn_800091D0(&m_impl->m_direction, &Vec3f(0.0f, 0.0f, -1.0f));
    }
}

void CameraModeLookFront::vf_0x14(UnkCameraArg_800098E4 *arg) {}

CameraMode *CameraModeLookFront::Create(UnkCameraOwner_8000E9BC *owner, int number) {
    return new CameraModeLookFront(owner, number);
}

int CameraModeLookFront::GetModeId() {
    return 11;
}

} // namespace PJSCamera
