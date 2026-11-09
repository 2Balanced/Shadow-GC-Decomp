/* The game's two-component vector routines (Vec2f, Game/MathTypes.h): sum, difference, scaling,
 * length, normalization, dot product, and the zero vector built when the game starts. Other units
 * call them by their fn_ names, so they keep them; the file name is invented and everything here
 * is named from what the code does. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#include "Game/MathTypes.h"
#include <dolphin/types.h>

extern "C" {
float fn_8000FEE4(float x); /* square root */

Vec2f fn_8040CBF8(const Vec2f *a, const Vec2f *b);
Vec2f fn_8040CC1C(const Vec2f *v, float scale);
Vec2f fn_8040CC38(const Vec2f *a, const Vec2f *b);
void fn_8040CC5C(Vec2f *v);
void fn_8040CC6C(Vec2f *v);
float fn_8040CCD4(const Vec2f *v);
float fn_8040CCEC(const Vec2f *v);
float fn_8040CD20(const Vec2f *a, const Vec2f *b);
void fn_8040CD3C(Vec2f *v, const Vec2f *value);
}

Vec2f lbl_805F1358(0.0f, 0.0f);

void fn_8040CD3C(Vec2f *v, const Vec2f *value) {
    v->x += value->x;
    v->y += value->y;
}

float fn_8040CD20(const Vec2f *a, const Vec2f *b) {
    return a->x * b->x + a->y * b->y;
}

float fn_8040CCEC(const Vec2f *v) {
    return fn_8000FEE4(v->x * v->x + v->y * v->y);
}

float fn_8040CCD4(const Vec2f *v) {
    return v->x * v->x + v->y * v->y;
}

void fn_8040CC6C(Vec2f *v) {
    float length = fn_8040CCEC(v);
    if (length < 0.000001f) {
        *v = lbl_805F1358;
    } else {
        v->x /= length;
        v->y /= length;
    }
}

void fn_8040CC5C(Vec2f *v) {
    v->x = v->y = 0.0f;
}

Vec2f fn_8040CC38(const Vec2f *a, const Vec2f *b) {
    return Vec2f(a->x + b->x, a->y + b->y);
}

Vec2f fn_8040CC1C(const Vec2f *v, float scale) {
    return Vec2f(v->x * scale, v->y * scale);
}

Vec2f fn_8040CBF8(const Vec2f *a, const Vec2f *b) {
    return Vec2f(a->x - b->x, a->y - b->y);
}
