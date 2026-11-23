/* RenderWare: RpUVAnim (plugin 0x135), the parameterised keyframe scheme (an angle, two scales,
 * a skew and a translation per key): its apply, interpolate and blend callbacks (the angle takes
 * the short way round), the empty mul-recip and add callbacks, and the matrix-to-key conversion
 * RpUVAnimKeyFrameInit uses (fn_80453820).
 *
 * 0x804535C8-0x80453980 (.rodata 0x8051BD28-0x8051BD40, .sdata2 0x805FB0E0-0x805FB0F8): a file of
 * its own, between RpUVAnimKeyFrame.c and RpUVAnim.c (see RpUVAnimKeyFrame.c for the literal pools
 * that separate the three). Its .rodata holds the two {0, 0, 1} axis initializers; the two
 * half-texel offsets it translates by are RpUVAnim.c's globals (lbl_8051BD40, lbl_8051BD4C), whose
 * .rodata starts at 0x8051BD40 and is reached through that file's pool base.
 *
 * Built as C with cflags_rw_c (configure.py). Matching: fn_80453820 copies the matrix it is given
 * through a pointer to const (`RwMatrix m = *matrix;`, a word copy), which puts its loads where
 * retail has them (probes/RenderWare_RpUVAnimParam_W41.py).
 */
#include <dolphin/types.h>
#include "Game/rw/r2.h"

extern const float lbl_8051BD40[3]; /* {-0.5f, -0.5f, 0.0f} (RpUVAnim.c) */
extern const float lbl_8051BD4C[3]; /* {0.5f, 0.5f, 0.0f} (RpUVAnim.c) */

void fn_8047EBA0(float *);
void fn_8047EFC8(float *, const float *, float, u32);
void fn_8047F59C(float *, const float *, u32);
double fn_803B1C3C(double, double);



/* Turn a record payload into a matrix and place it: translate, rotate about the
   fixed axis by the payload angle in degrees, translate again. */
void fn_804535C8(float *out, float *key)
{
    R2V3d axis = {0.0f, 0.0f, 1.0f};
    { float zero = 0.0f;
    out[0] = key[3];
    out[1] = key[5];
    out[2] = zero;
    out[4] = zero;
    out[5] = key[4];
    out[6] = zero;
    out[8] = zero;
    out[9] = zero;
    out[10] = zero;
    out[12] = key[6];
    out[13] = key[7];
    out[14] = zero;
    fn_8047EBA0(out);
    fn_8047F59C(out, lbl_8051BD40, 2);
    fn_8047EFC8(out, (const float *)&axis, 180.0f * (key[2] / 3.1415927f), 2);
    /* Retail leaves r3 as the last call set it rather than returning out. */
    fn_8047F59C(out, lbl_8051BD4C, 2);
 }}

/* Blend at time, taking the shortest way round for the angle in word 2. */
void fn_804536B4(float *out, float *from, float *to, float time)
{
    float ratio = (time - from[1]) / (to[1] - from[1]);
    float delta = to[2] - from[2];
    if (delta < -3.1415927f) {
        delta += 6.2831855f;
    } else if (delta > 3.1415927f) {
        delta -= 6.2831855f;
    }
    out[2] = ratio * delta + from[2];
    { u32 i; for (i = 3; i < 8; i++) {
        out[i] = ratio * (to[i] - from[i]) + from[i];
    } }
}

/* The wrapped blend for a caller that has already worked out the ratio. */
void fn_80453770(float *out, float *from, float *to, float ratio)
{
    float delta = to[2] - from[2];
    if (delta < -3.1415927f) {
        delta += 6.2831855f;
    } else if (delta > 3.1415927f) {
        delta -= 6.2831855f;
    }
    out[2] = ratio * delta + from[2];
    { u32 i; for (i = 3; i < 8; i++) {
        out[i] = ratio * (to[i] - from[i]) + from[i];
    } }
}

void fn_80453818(void) {
}

void fn_8045381C(void) {
}

/* Recover the angle of a record payload: retail copies the whole record into the
   frame, rotates that copy, and reads the angle and the transformed position. */
float *fn_80453820(float *out, const R2Matrix *matrix)
{
    R2Matrix m = *matrix;
    R2V3d axis = {0.0f, 0.0f, 1.0f};

    out[0] = (float)fn_803B1C3C(-m.up.x, m.right.x);
    fn_8047F59C((float *)&m, lbl_8051BD40, 2);
    fn_8047EFC8((float *)&m, (const float *)&axis, 180.0f * (-out[0] / 3.1415927f), 2);
    fn_8047F59C((float *)&m, lbl_8051BD4C, 2);
    out[1] = m.right.x;
    out[3] = m.right.y;
    out[2] = m.up.y;
    out[4] = m.pos.x;
    out[5] = m.pos.y;
    return out;
}
