/* Angle helpers of the SonicUSA library (built, like SonicUSA::System::Random in
 * Game/SonicUSARandom.cpp, without small data): zeroing a set of angles, wrapping an angle into
 * [0, 2 pi) or (-pi, pi], and the zero vectors other units read (lbl_805E07B0, lbl_805E07B8), built
 * when the game starts. Other units call them by their fn_ names, so they keep them; the file name
 * is invented and everything here is named from what the code does. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include <dolphin/types.h>

float modff(float x, float *ip);

extern "C" {
void fn_8040E970(Vec3f *angles);
void fn_8040E988(Vec2f *angles);
float fn_8040E99C(float angle);
float fn_8040E9D8(float angle);
float fn_8040EA40(float x);
}

Vec2f lbl_805E07B0(0.0f, 0.0f);
Vec3f lbl_805E07B8(0.0f, 0.0f, 0.0f);

float fn_8040EA40(float x) {
    float whole;
    return modff(x, &whole);
}

float fn_8040E9D8(float angle) {
    float wrapped = 2.0f * lbl_805FAD38 * fn_8040EA40(angle / (2.0f * lbl_805FAD38));
    if (wrapped < 0.0f) {
        wrapped += 2.0f * lbl_805FAD38;
    }
    return wrapped;
}

float fn_8040E99C(float angle) {
    float wrapped = fn_8040E9D8(angle);
    if (wrapped > lbl_805FAD38) {
        wrapped -= 2.0f * lbl_805FAD38;
    }
    return wrapped;
}

void fn_8040E988(Vec2f *angles) {
    angles->x = angles->y = 0.0f;
}

void fn_8040E970(Vec3f *angles) {
    angles->x = angles->y = angles->z = 0.0f;
}
