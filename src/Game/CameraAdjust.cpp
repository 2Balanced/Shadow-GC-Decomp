/* SonicteamUSA::System::Camera::CameraAdjust (class name from RTTI; Game/CameraAdjust.h): the base
 * of the camera's moves from one mode to the next. The unit holds its constructor, destructor and
 * rates, the routines the camera unit calls to start a move (fn_8000AF74) and to advance it
 * (fn_8000ADD0), and the first copies of helpers other camera units call by address: the
 * placement's assignment, its up-vector and window setters, the near-zero tests and the Vec3f
 * subtraction. File name from the class; other names are inferred. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include "Game/CameraAdjust.h"
#include "Game/CameraModeUser.h"
#include "Game/CameraState.h"
#include "Game/MathTypes.h"

using SonicteamUSA::System::Camera::CameraAdjust;
using SonicteamUSA::System::Camera::CameraMode;
using SonicteamUSA::System::Camera::CameraModeUser;

typedef UnkCameraState_8000AA70 CameraState;

/* The float overload of std::fabs, out of line in the game (first copy in Game/CameraState.cpp). */
namespace std {
float fabs(float x);
} // namespace std

extern "C" {
/* Sets a placement's eye, point looked at and field of view (Game/CameraState.cpp). */
void fn_8000A588(CameraState *state, const Vec3f *eye, const Vec3f *target, float fov);
/* a x b, the angle between a and b, the rotation by angle about axis, matrix * point. */
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b);
float fn_8040F8FC(const Vec3f *a, const Vec3f *b);
Mtx34f fn_8040FD8C(const Vec3f *axis, float angle);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);

void fn_8000B21C(u32 *flags, const u32 *source);
bool fn_8000B044(float value);
void fn_8000AF74(CameraAdjust *adjust, const CameraState *from, CameraMode *mode);
void fn_8000AF60(CameraState *state, const Vec2f *window);
const CameraState *fn_8000ADD0(CameraAdjust *adjust, CameraMode *mode, float dt);
void fn_8000AD90(CameraAdjust *adjust, CameraMode *mode);
bool fn_8000AD08(const Vec3f *a, const Vec3f *b);
void fn_8000ABA8(CameraAdjust *adjust, CameraMode *mode);
}

/* fn_8040CD74 called as returning a - b by value (see Game/BKTank.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* Copies value into vector through the game's out-of-line assignment (as in Game/GadgetUtil.h). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* Whether v is (nearly) the zero vector. */
inline bool IsZero(const Vec3f &v) {
    return v.x * v.x + v.y * v.y + v.z * v.z < 1e-4f;
}

/* The view window of a placement (a mode starts with a placement). */
#define WINDOW(state) (*reinterpret_cast<Vec2f *>((state).field_0x1C))
/* Copied whole as two words by the placement's assignment. */
struct UnkCameraWindow_8000B074 {
    u32 m_words[2];
};
#define WINDOW_WORDS(state) (*reinterpret_cast<UnkCameraWindow_8000B074 *>((state).field_0x1C))
#define CONST_WINDOW_WORDS(state) (*reinterpret_cast<const UnkCameraWindow_8000B074 *>((state).field_0x1C))

inline CameraState &ModeState(CameraMode *mode) {
    return *reinterpret_cast<CameraState *>(mode);
}

/* A 3x4 matrix copied from another by fn_8040C148 (as UnkCameraMtx_8040C148 in
 * Game/CameraManagerUnit.h). */
struct UnkCameraMtx_8000ABA8 {
    UnkCameraMtx_8000ABA8(const Mtx34f &m) {
        fn_8040C148(&value, &m);
    }

    Mtx34f value;
};

void fn_8000B21C(u32 *flags, const u32 *source) {
    *flags = *source;
}

namespace SonicteamUSA {
namespace System {
namespace Camera {

CameraAdjust::CameraAdjust(const UnkCameraBlend_8000B2C0 &blend)
    : m_status(0), m_blend(blend), m_time(0.0f), m_eyeRate(0.0f), m_lookRate(0.0f), m_eyeStatus(0),
      m_lookStatus(0) {}

CameraAdjust::~CameraAdjust() {}

} // namespace Camera
} // namespace System
} // namespace SonicteamUSA

UnkCameraState_8000AA70 &UnkCameraState_8000AA70::operator=(const UnkCameraState_8000AA70 &state) {
    AssignVector(m_eye, state.m_eye);
    AssignVector(m_target, state.m_target);
    m_fov = state.m_fov;
    WINDOW_WORDS(*this) = CONST_WINDOW_WORDS(state);
    AssignVector(m_up, state.m_up);
    fn_8000B21C(reinterpret_cast<u32 *>(field_0x30), reinterpret_cast<const u32 *>(state.field_0x30));
    return *this;
}

bool fn_8000B044(float value) {
    return std::fabs(value) < 1e-4f;
}

void fn_8000AF74(CameraAdjust *adjust, const CameraState *from, CameraMode *mode) {
    adjust->m_from = adjust->m_state = *from;
    adjust->m_time = 0.0f;
    adjust->m_status = adjust->m_eyeStatus = adjust->m_lookStatus = 1;
    if (fn_8000B044(adjust->m_blend.m_time) || adjust->m_blend.m_time <= 0.0f) {
        adjust->m_eyeStatus = 2;
    }
    if (fn_8000B044(adjust->m_blend.field_0x4) || adjust->m_blend.field_0x4 <= 0.0f) {
        adjust->m_lookStatus = 2;
    }
    adjust->Start(from, mode);
}

void fn_8000AF60(CameraState *state, const Vec2f *window) {
    WINDOW(*state).x = window->x;
    WINDOW(*state).y = window->y;
}

const CameraState *fn_8000ADD0(CameraAdjust *adjust, CameraMode *mode, float dt) {
    if (adjust->m_status != 1) {
        return &ModeState(mode);
    }
    adjust->m_time += dt;
    int eyeDone = false;
    int lookDone = false;
    if (adjust->m_eyeStatus == 1) {
        adjust->m_eyeRate = adjust->GetEyeRate(adjust->m_time, adjust->m_blend.m_time);
        if (adjust->m_time > adjust->m_blend.m_time) {
            eyeDone = true;
            adjust->m_eyeRate = 1.0f;
        }
        fn_8000ABA8(adjust, mode);
        adjust->UpdateEye(mode, dt);
        if (eyeDone) {
            adjust->m_eyeStatus = 2;
        }
    } else {
        fn_8000A588(&adjust->m_state, &mode->m_eye, &mode->m_target, ModeState(mode).m_fov);
    }
    if (adjust->m_lookStatus == 1) {
        adjust->m_lookRate = adjust->GetLookRate(adjust->m_time, adjust->m_blend.field_0x4);
        if (adjust->m_time > adjust->m_blend.field_0x4) {
            lookDone = true;
            adjust->m_lookRate = 1.0f;
        }
        fn_8000AD90(adjust, mode);
        if (lookDone) {
            adjust->m_lookStatus = 2;
        }
    } else {
        fn_8000AF60(&adjust->m_state, &WINDOW(ModeState(mode)));
    }
    if (adjust->m_eyeStatus == 2 && adjust->m_lookStatus == 2) {
        adjust->m_status = 2;
    }
    return &adjust->m_state;
}

namespace SonicteamUSA {
namespace System {
namespace Camera {

float CameraAdjust::GetEyeRate(float time, float duration) {
    return time / duration;
}

float CameraAdjust::GetLookRate(float time, float duration) {
    return time / duration;
}

} // namespace Camera
} // namespace System
} // namespace SonicteamUSA

void fn_8000AD90(CameraAdjust *adjust, CameraMode *mode) {
    float x = adjust->m_lookRate * (WINDOW(ModeState(mode)).x - WINDOW(adjust->m_from).x) +
              WINDOW(adjust->m_from).x;
    float y = adjust->m_lookRate * (WINDOW(ModeState(mode)).y - WINDOW(adjust->m_from).y) +
              WINDOW(adjust->m_from).y;
    WINDOW(adjust->m_state).x = x;
    WINDOW(adjust->m_state).y = y;
}

Vec3f operator-(const Vec3f &a, const Vec3f &b) {
    return ((VectorSubtractFn)fn_8040CD74)(&a, &b);
}

bool fn_8000AD08(const Vec3f *a, const Vec3f *b) {
    return IsZero(*a - *b);
}

void fn_8000ACE4(CameraModeUser *mode, const Vec3f *up) {
    AssignVector(reinterpret_cast<CameraState *>(mode)->m_up, *up);
}

void fn_8000ABA8(CameraAdjust *adjust, CameraMode *mode) {
    const Vec3f *from = &adjust->m_from.m_up;
    const Vec3f *to = &ModeState(mode).m_up;
    if (!fn_8000AD08(from, to)) {
        Vec3f axis;
        fn_800091EC(&axis, &(const Vec3f &)fn_8040CFB8(from, to));
        float angle = fn_8040F8FC(from, to);
        if (IsZero(axis)) {
            fn_800091D0(&axis, &(const Vec3f &)Vec3f(0.0f, 0.0f, 1.0f));
            if (fn_8000AD08(&axis, from)) {
                fn_800091D0(&axis, &(const Vec3f &)Vec3f(0.0f, 1.0f, 0.0f));
            }
        }
        UnkCameraMtx_8000ABA8 rotation(fn_8040FD8C(&axis, angle * adjust->m_eyeRate));
        fn_8000ACE4(reinterpret_cast<CameraModeUser *>(&adjust->m_state),
                    &(const Vec3f &)fn_8040B67C(&rotation.value, from));
    }
}

namespace SonicteamUSA {
namespace System {
namespace Camera {

void CameraAdjust::vf_0x20() {}

} // namespace Camera
} // namespace System
} // namespace SonicteamUSA
