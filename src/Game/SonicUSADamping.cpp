/* A step of damped motion from the SonicUSA library (built, like Game/SonicUSAAngle.cpp, without
 * small data): a position and velocity advanced by dt under a constant acceleration with a linear
 * drag k, using the exact solution (e^-k dt); with no drag it is the plain constant-acceleration
 * step. Other units call it by its fn_ name, so it keeps it; the file name is invented and the
 * function is named from what the code does. */
#include <cmath>

#define fn_8040CD74 fn_8040CD74_declared
#include "Game/MathTypes.h"
#undef fn_8040CD74
#include <dolphin/types.h>

extern "C" {
Vec3f fn_8040CDA8(const Vec3f &v, float scale);
Vec3f fn_8040CDD0(const Vec3f &a, const Vec3f &b);
Vec3f fn_8040CD74(const Vec3f &a, const Vec3f &b);
void fn_804103E8(Vec3f *position, Vec3f *velocity, const Vec3f *acceleration, float drag, float dt);
}

/* The vector operations behind the step (names inferred). */
inline Vec3f Scale(const Vec3f &v, float scale) {
    return fn_8040CDA8(v, scale);
}

inline Vec3f Add(const Vec3f &a, const Vec3f &b) {
    return fn_8040CDD0(a, b);
}

inline Vec3f Subtract(const Vec3f &a, const Vec3f &b) {
    return fn_8040CD74(a, b);
}

void fn_804103E8(Vec3f *position, Vec3f *velocity, const Vec3f *acceleration, float drag, float dt) {
    if (drag <= 0.0f) {
        fn_800091D0(position, &(const Vec3f &)Add(Add(*position, Scale(*velocity, dt)),
                                                  Scale(Scale(Scale(*acceleration, dt), dt), 0.5f)));
        fn_800091D0(velocity, &(const Vec3f &)Add(*velocity, Scale(*acceleration, dt)));
    } else {
        float growth = pow(2.718281828f, drag * dt);
        float decay = 1.0f / growth;
        float inverse = 1.0f / drag;
        fn_800091D0(position,
                    &(const Vec3f &)Add(Add(*position, Scale(Scale(*acceleration, inverse), dt)),
                                        Scale(Scale(Scale(Subtract(Scale(*velocity, drag), *acceleration), inverse),
                                                    inverse),
                                              1.0f - decay)));
        fn_800091D0(velocity, &(const Vec3f &)Add(Scale(*acceleration, inverse),
                                                  Scale(Subtract(*velocity, Scale(*acceleration, inverse)), decay)));
    }
}
