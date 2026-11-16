/* Dolphin SDK MTX: 4x4 projection matrices. Only the orthographic builder is linked. */
#include <dolphin/mtx.h>

/* Orthographic projection of the box [l, r] x [b, t] x [-n, -f] onto the clip cube, with z mapped
 * to [-1, 0] as the GameCube's depth convention expects. */
void fn_80378A18(Mtx44 m, f32 t, f32 b, f32 l, f32 r, f32 n, f32 f) {
    f32 inv;

    inv = 1.0f / (r - l);
    m[0][0] = 2.0f * inv;
    m[0][1] = 0.0f;
    m[0][2] = 0.0f;
    m[0][3] = -(r + l) * inv;

    inv = 1.0f / (t - b);
    m[1][0] = 0.0f;
    m[1][1] = 2.0f * inv;
    m[1][2] = 0.0f;
    m[1][3] = -(t + b) * inv;

    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    inv = 1.0f / (f - n);
    m[2][2] = -1.0f * inv;
    m[2][3] = -f * inv;

    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;
    m[3][3] = 1.0f;
}
