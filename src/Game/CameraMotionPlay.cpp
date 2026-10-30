/* The playing of camera motions (no class with RTTI; file name and all names inferred): a
 * camera motion at a frame placed by a matrix onto a camera mode (fn_8000FBFC: eye, point looked
 * at, roll and field of view), or offset onto the first screen's current mode (fn_80010010); and
 * the accessors of a screen camera's placement and point looked at (fn_80010244, fn_800101F8),
 * which other units call by address. The unit holds the game's first copies of the float square
 * root and sine. It is split by its .sdata2 (0x805F17B0..0x805F17E0). Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#define MSL_LIST_ACCESSORS
#include "Game/CameraManagerUnit.h"
#include "Game/CameraModeUser.h"
#include "Game/CameraState.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"

using SonicteamUSA::System::Camera::CameraManagerUnit;
using SonicteamUSA::System::Camera::CameraMode;

typedef UnkCameraState_8000AA70 CameraState;

/* The camera manager (Game/CameraManager.cpp). */
class UnkCameraManager_80009548;

/* A camera motion (Game/CameraMotion.cpp): its kind in the low half of the first word. */
struct UnkCameraMotion_8000F9E8 {
    /* 0x00 */ u32 m_kind;
};

/* What a motion sets at a frame: eight values (the last two angles in 1/65536 of a turn) and the
 * flags of those set. */
struct UnkCameraMotionState_8000F9E8 {
    UnkCameraMotionState_8000F9E8() {
        Nothing();
    }

    static void Nothing() {}

    /* 0x00 */ float m_values[6];
    /* 0x18 */ s32 m_angles[2];
    /* 0x20 */ u32 m_set;
};

extern "C" {
UnkCameraManager_80009548 *fn_80009548(void);
boost::shared_ptr<CameraManagerUnit> fn_8000D0F0(UnkCameraManager_80009548 *manager, int index);
void fn_8000F9E8(UnkCameraMotionState_8000F9E8 *state, UnkCameraMotion_8000F9E8 *motion, float frame);
/* The first screen's current mode's setters (Game/CameraManager.cpp). */
void fn_8000DAB8(UnkCameraManager_80009548 *manager, const Vec3f *eye, const Vec3f *target, int index);
void fn_8000DA24(UnkCameraManager_80009548 *manager, int index, float fov);
void fn_8000D988(UnkCameraManager_80009548 *manager, int index, float value);
/* Placement routines (Game/CameraState.cpp). */
void fn_8000A3F0(CameraState *state, float fov);
void fn_8000A304(CameraState *state, const Vec3f *angles);
/* matrix * point, matrix * vector, a - b, v normalised, the angles of a direction and an up
 * vector. */
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Vec3f fn_8040B618(const Mtx34f *matrix, const Vec3f *vector);
float fn_8040CE74(Vec3f *v);
Vec3f fn_8040F364(const Vec3f *direction, const Vec3f *up);
double sin(double x);
double cos(double x);
}

/* NaN (MSL's __float_nan). */
extern float lbl_805EDCC8[];

/* fn_8040CD74 called as returning a - b by value (see Game/BKTank.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* The placement of a mode (first copy in Game/CameraManager.cpp). */
inline CameraState *ModeState(const boost::shared_ptr<CameraMode> &mode) {
    return reinterpret_cast<CameraState *>(mode.get());
}

/* C99 classification of a float (FP_NAN 1, FP_INFINITE 2, FP_ZERO 3, FP_NORMAL 4, FP_SUBNORMAL
 * 5). */
inline int ClassifyFloat(float x) {
    unsigned long bits = *(unsigned long *)&x;
    switch (bits & 0x7F800000) {
    case 0x7F800000:
        if (bits & 0x007FFFFF) {
            return 1;
        }
        return 2;
    case 0:
        if (bits & 0x007FFFFF) {
            return 5;
        }
        return 3;
    }
    return 4;
}

/* The float square root (MSL's: three Newton-Raphson steps, NaN below zero). */
extern "C" inline float fn_8000FF04(float x) {
    static const double _half = 0.5;
    static const double _three = 3.0;
    if (x > 0.0f) {
        double guess = __frsqrte((double)x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        return (float)(x * guess);
    }
    if (x < 0.0) {
        return lbl_805EDCC8[0];
    }
    int kind = ClassifyFloat(x);
    if (kind == 1) {
        return lbl_805EDCC8[0];
    }
    return x;
}

extern "C" inline float fn_8000FEE4(float x) {
    return fn_8000FF04(x);
}

namespace std {
inline float sinf_(float x) {
    return (float)::sin(x);
}

inline float sin(float x) {
    return sinf_(x);
}

inline float cosf_(float x) {
    return (float)::cos(x);
}

inline float cos(float x) {
    return cosf_(x);
}
} // namespace std

/* An angle in 1/65536 of a turn in radians. */
#define ANGLE(a) ((float)(a) * lbl_805FAD38 / 32768.0f)

/* A screen camera's placement and point looked at. */
extern "C" CameraState *fn_80010244(UnkCameraManager_80009548 *manager, int index) {
    return &fn_8000D0F0(manager, index)->m_state;
}

extern "C" Vec3f *fn_800101F8(UnkCameraManager_80009548 *manager, int index) {
    return &fn_8000D0F0(manager, index)->m_state.m_target;
}

/* Plays motion at frame on the first screen's current mode, offset by offset when given. */
extern "C" void fn_80010010(UnkCameraMotion_8000F9E8 *motion, const Vec3f *offset, float frame) {
    if ((motion->m_kind & 0xFFFF) != 2) {
        return;
    }
    Vec3f eye(0.0f, 0.0f, -1.0f);
    Vec3f target(0.0f, 0.0f, 0.0f);
    UnkCameraMotionState_8000F9E8 state;
    Vec3f base;
    if (offset != 0) {
        AssignVector(base, *offset);
    } else {
        base.x = base.y = base.z = 0.0f;
    }
    fn_8000F9E8(&state, motion, frame);
    if (state.m_set & 1) {
        eye.x = state.m_values[0] + base.x;
    }
    if (state.m_set & 2) {
        eye.y = state.m_values[1] + base.y;
    }
    if (state.m_set & 4) {
        eye.z = state.m_values[2] + base.z;
    }
    if (state.m_set & 8) {
        target.x = state.m_values[3] + base.x;
    }
    if (state.m_set & 0x10) {
        target.y = state.m_values[4] + base.y;
    }
    if (state.m_set & 0x20) {
        target.z = state.m_values[5] + base.z;
    }
    fn_8000DAB8(fn_80009548(), &eye, &target, 0);
    if (state.m_set & 0x40) {
        fn_8000DA24(fn_80009548(), 0, ANGLE(state.m_angles[0]));
    }
    if (state.m_set & 0x80) {
        fn_8000D988(fn_80009548(), 0, ANGLE(state.m_angles[1]));
    }
}

/* Plays motion at frame on mode, placed by matrix. */
extern "C" void fn_8000FBFC(const boost::shared_ptr<CameraMode> &mode, UnkCameraMotion_8000F9E8 *motion,
                            const Mtx34f *matrix, float frame) {
    if ((motion->m_kind & 0xFFFF) != 2) {
        return;
    }
    Vec3f eye(0.0f, 0.0f, -1.0f);
    Vec3f target(0.0f, 0.0f, 0.0f);
    UnkCameraMotionState_8000F9E8 state;
    float fov;
    float roll = 0.0f;
    fn_8000F9E8(&state, motion, frame);
    if (state.m_set & 1) {
        eye.x = state.m_values[0];
    }
    if (state.m_set & 2) {
        eye.y = state.m_values[1];
    }
    if (state.m_set & 4) {
        eye.z = state.m_values[2];
    }
    if (state.m_set & 8) {
        target.x = state.m_values[3];
    }
    if (state.m_set & 0x10) {
        target.y = state.m_values[4];
    }
    if (state.m_set & 0x20) {
        target.z = state.m_values[5];
    }
    if (state.m_set & 0x40) {
        roll = ANGLE(state.m_angles[0]);
    }
    if (state.m_set & 0x80) {
        fov = ANGLE(state.m_angles[1]);
        fn_8000A3F0(ModeState(mode), fov);
    }
    Vec3f up(0.0f, 1.0f, 0.0f);
    Vec3f direction;
    Vec3f angles(0.0f, 0.0f, 0.0f);
    AssignVector(eye, fn_8040B67C(matrix, &eye));
    AssignVector(target, fn_8040B67C(matrix, &target));
    if (0.0f != roll) {
        up.x = -std::sin(roll);
        up.y = std::cos(roll);
    }
    AssignVector(direction, ((VectorSubtractFn)fn_8040CD74)(&target, &eye));
    if (0.0f != direction.y) {
        fn_8040CE74(&direction);
        float s = -direction.y;
        float y = up.y;
        float z = up.z;
        float c = fn_8000FEE4(1.0f - s * s);
        up.y = -s * z + c * y;
        up.z = s * y + c * z;
    }
    AssignVector(up, fn_8040B618(matrix, &up));
    angles = fn_8040F364(&direction, &up);
    fn_8000A5D4(reinterpret_cast<CameraMode *>(ModeState(mode)), &eye, &target);
    fn_8000A304(ModeState(mode), &angles);
}
