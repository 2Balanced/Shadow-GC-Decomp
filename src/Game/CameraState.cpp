/* The camera placement every camera unit and mode works on (struct name unknown;
 * UnkCameraState_8000AA70 in Game/CameraState.h): its constructors and destructor, the default
 * placement, and the routines that read and move it (the eye, the point looked at, the up vector
 * and the roll about the line of sight), which the camera units call by address. The unit has no
 * class with RTTI; it is split from its neighbours by its static initializer (the default
 * placement) and its float constants. File name from the header; function names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/CameraState.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"

typedef UnkCameraState_8000AA70 CameraState;

extern "C" {
/* Vector and matrix routines (see Game/MathTypes.h). */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b); /* cross product */
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Vec3f fn_8040D118(const Vec3f *vector, float angle); /* turned about x */
Vec3f fn_8040D0AC(const Vec3f *vector, float angle); /* turned about y */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* vector += value */
void fn_8040CE38(Vec3f *vector, float scale);         /* vector *= scale */
float fn_8040CE74(Vec3f *vector);                     /* normalised in place */
Vec2f fn_8040F760(const Vec3f *direction);            /* its pitch and yaw */
void fn_8040B33C(Mtx34f *out, const Vec3f *eye, const Vec3f *up, const Vec3f *target);
void fn_800498A0(Mtx34f *matrix, const Vec3f *axis, float angle);
/* The view window of the screen (behaviour inferred). */
Vec2f fn_8000F41C(void);
}

/* fn_8040CD74 called as returning a - b by value (see Game/BKTank.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* MSL's fabs for floats as the game has it (see Game/CameraModeGeneral.cpp): each level calls the
 * next inline function, so std::fabs and fabsf stay out of line. Declared here rather than
 * through <cmath>, whose sqrtf adds two double constants to .sdata2 that this unit lacks. */
extern "C" inline double fabs(double x) {
    return __fabs(x);
}

extern "C" inline float fabsf(float x) {
    return (float)fabs((double)x);
}

namespace std {
inline float fabs(float x) {
    return fabsf(x);
}
} // namespace std

inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* The default placement (names keep the addresses other units declare them by). */
extern const Vec3f lbl_8056EE28;
extern const Vec3f lbl_8056EE34;
extern const float lbl_805F1708;
extern const Vec2f lbl_805EDFA8;
extern const Vec3f lbl_8056EE40;

const Vec3f lbl_8056EE28(0.0f, 20.0f, 20.0f);
const Vec3f lbl_8056EE34(0.0f, 0.0f, 0.0f);
const float lbl_805F1708 = 0.0f;
const Vec2f lbl_805EDFA8(DegToRad(63.250149f), DegToRad(53.130096f));
const Vec3f lbl_8056EE40(0.0f, 1.0f, 0.0f);

/* A matrix built from its columns: the axes and the position (class name unknown). */
struct UnkMatrix_8000A6C4 : public Mtx34f {
    UnkMatrix_8000A6C4(const Vec3f &x, const Vec3f &y, const Vec3f &z, const Vec3f &position) {
        values[0][0] = x.x;
        values[0][1] = y.x;
        values[0][2] = z.x;
        values[0][3] = position.x;
        values[1][0] = x.y;
        values[1][1] = y.y;
        values[1][2] = z.y;
        values[1][3] = position.y;
        values[2][0] = x.z;
        values[2][1] = y.z;
        values[2][2] = z.z;
        values[2][3] = position.z;
    }
};

extern "C" {

/* Sets a word (the word at 0x30 of a placement). */
u32 *fn_8000AAEC(u32 *word, u32 value) {
    *word = value;
    return word;
}

} // extern "C"

UnkCameraState_8000AA70::UnkCameraState_8000AA70() {
    fn_800091EC(&m_eye, &lbl_8056EE28);
    fn_800091EC(&m_target, &lbl_8056EE34);
    m_fov = lbl_805F1708;
    *reinterpret_cast<Vec2f *>(field_0x1C) = lbl_805EDFA8;
    fn_800091EC(&m_up, &lbl_8056EE40);
    fn_8000AAEC(reinterpret_cast<u32 *>(field_0x30), 0);
}

UnkCameraState_8000AA70::UnkCameraState_8000AA70(const Vec3f *eye, const Vec3f *target, float fov,
                                                 const Vec2f *window) {
    fn_800091EC(&m_eye, eye);
    fn_800091EC(&m_target, target);
    m_fov = fov;
    *reinterpret_cast<Vec2f *>(field_0x1C) = *window;
    fn_800091EC(&m_up, &lbl_8056EE40);
    fn_8000AAEC(reinterpret_cast<u32 *>(field_0x30), 0);
}

UnkCameraState_8000AA70::~UnkCameraState_8000AA70() {}

extern "C" {

/* Whether a value is (nearly) zero. */
bool fn_8000A950(float value) {
    return std::fabs(value) < 0.0001f;
}

void fn_8000A2D4(const CameraState *state, Mtx34f *out, const Vec3f *axis);
Vec3f fn_8000A3BC(const CameraState *state);

/* The placement's right, up and forward vectors (each optional), turned by its roll. */
void fn_8000A800(const CameraState *state, Vec3f *right, Vec3f *up, Vec3f *forward) {
    Mtx34f roll;
    Vec3f upward;
    fn_800091EC(&upward, &state->m_up);
    Vec3f look;
    fn_800091EC(&look, &(const Vec3f &)fn_8000A3BC(state));
    fn_8040CE74(&look);
    Vec3f side;
    fn_800091EC(&side, &(const Vec3f &)fn_8040CFB8(&upward, &look));
    fn_800091D0(&side, &(const Vec3f &)fn_8040CFB8(&upward, &look));
    fn_8040CE74(&side);
    fn_800091D0(&upward, &(const Vec3f &)fn_8040CFB8(&look, &side));
    if (!fn_8000A950(state->m_fov)) {
        fn_8000A2D4(state, &roll, &look);
        fn_800091D0(&side, &(const Vec3f &)fn_8040B67C(&roll, &side));
        fn_800091D0(&upward, &(const Vec3f &)fn_8040B67C(&roll, &upward));
    }
    if (right != 0) {
        fn_800091D0(right, &side);
    }
    if (up != 0) {
        fn_800091D0(up, &upward);
    }
    if (forward != 0) {
        fn_800091D0(forward, &look);
    }
}

/* The up vector. */
void fn_8000A7B8(Vec3f *out, const CameraState *state) {
    Vec3f up;
    fn_8000A800(state, 0, &up, 0);
    fn_800091EC(out, &up);
}

/* The right vector. */
void fn_8000A770(Vec3f *out, const CameraState *state) {
    Vec3f right;
    fn_8000A800(state, &right, 0, 0);
    fn_800091EC(out, &right);
}

/* The world matrix of the placement. */
void fn_8000A6C4(Mtx34f *out, const CameraState *state) {
    Vec3f right;
    Vec3f up;
    Vec3f forward;
    fn_8000A800(state, &right, &up, &forward);
    UnkMatrix_8000A6C4 matrix(right, up, forward, state->m_eye);
    fn_8040C148(out, &matrix);
}

/* The view matrix of the placement. */
void fn_8000A67C(Mtx34f *out, const CameraState *state) {
    Vec3f up;
    fn_8000A7B8(&up, state);
    fn_8040B33C(out, &state->m_eye, &up, &state->m_target);
}

/* The placement's angles: pitch and yaw of the line of sight, and the roll. */
Vec3f fn_8000A610(const CameraState *state) {
    Vec3f look;
    fn_800091EC(&look, &(const Vec3f &)fn_8000A3BC(state));
    fn_8040CE74(&look);
    const Vec2f &angles = fn_8040F760(&look);
    return Vec3f(angles.x, angles.y, state->m_fov);
}

/* Sets the eye and the point looked at. */
void fn_8000A5D4(CameraState *state, const Vec3f *eye, const Vec3f *target) {
    fn_800091D0(&state->m_eye, eye);
    fn_800091D0(&state->m_target, target);
}

/* Sets the eye, the point looked at and the roll. */
void fn_8000A588(CameraState *state, const Vec3f *eye, const Vec3f *target, float fov) {
    fn_800091D0(&state->m_eye, eye);
    fn_800091D0(&state->m_target, target);
    state->m_fov = fov;
}

/* Copies a placement's eye, point looked at and roll. */
void fn_8000A544(CameraState *state, const CameraState *from) {
    fn_800091D0(&state->m_eye, &from->m_eye);
    fn_800091D0(&state->m_target, &from->m_target);
    state->m_fov = from->m_fov;
}

/* Moves the eye and the point looked at. */
void fn_8000A508(CameraState *state, const Vec3f *offset) {
    fn_8040CFF8(&state->m_eye, offset);
    fn_8040CFF8(&state->m_target, offset);
}

/* Sets the point looked at. */
void fn_8000A4E0(CameraState *state, const Vec3f *target) {
    fn_8000A5D4(state, &state->m_eye, target);
}

/* Sets the point looked at from the eye. */
void fn_8000A49C(CameraState *state, const Vec3f *look) {
    fn_8000A4E0(state, &(const Vec3f &)fn_8040CDD0(&state->m_eye, look));
}

/* Sets the distance from the eye to the point looked at. */
void fn_8000A430(CameraState *state, float distance) {
    Vec3f look;
    fn_800091EC(&look, &(const Vec3f &)fn_8000A3BC(state));
    fn_8040CE74(&look);
    fn_8040CE38(&look, distance);
    fn_8000A49C(state, &look);
}

/* Sets the view window to the screen's. */
void fn_8000A3F0(CameraState *state) {
    *reinterpret_cast<Vec2f *>(state->field_0x1C) = fn_8000F41C();
}

/* The line of sight: the point looked at minus the eye. */
Vec3f fn_8000A3BC(const CameraState *state) {
    return ((VectorSubtractFn)fn_8040CD74)(&state->m_target, &state->m_eye);
}

/* Points the line of sight by pitch and yaw at the same distance, and sets the roll. */
void fn_8000A304(CameraState *state, const Vec3f *angles) {
    float distance = fn_8040CF50(&(const Vec3f &)fn_8000A3BC(state));
    Vec3f look(0.0f, 0.0f, -1.0f);
    fn_8040CE38(&look, distance);
    fn_800091D0(&look, &(const Vec3f &)fn_8040D118(&look, angles->x));
    fn_800091D0(&look, &(const Vec3f &)fn_8040D0AC(&look, angles->y));
    fn_8000A588(state, &state->m_eye, &(const Vec3f &)fn_8040CDD0(&state->m_eye, &look), angles->z);
}

/* The rotation by the roll about an axis. */
void fn_8000A2D4(const CameraState *state, Mtx34f *out, const Vec3f *axis) {
    fn_800498A0(out, axis, -state->m_fov);
}

} // extern "C"
