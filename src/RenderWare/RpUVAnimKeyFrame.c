/* RenderWare: RpUVAnim (plugin 0x135), the keyframe code shared by both keyframe schemes and the
 * linear (matrix) keyframes: RpUVAnimKeyFrameInit (fn_80453018), the keyframe stream read, write and
 * size callbacks, and the linear scheme's apply, interpolate, blend, mul-recip and add callbacks.
 *
 * 0x80453018-0x804535C8 (.sdata2 0x805FB0D8-0x805FB0E0). The file is cut on its literal pool:
 * {0.0f, 1.0f} here, while the parameterised scheme that follows (RpUVAnimParam.c) pools 0.0f
 * again with 180.0f, pi, -pi and 2pi, and the plugin file after that (RpUVAnim.c) pools 1.0f,
 * 0.0f, 180.0f and pi once more: each repeat starts a translation unit. RpUserData.c comes before.
 * The two interpolator records this file compares against are RpUVAnim.c's (lbl_8056D77C,
 * lbl_8056D7AC), as is the keyframe-list code it calls (fn_80454374, fn_8045441C, fn_80454494).
 *
 * Built as C with cflags_rw_c (configure.py); GC/2.0-2.7 give the same code, 1.3.2 is worse.
 * fn_8045345C (mul-recip) has retail's instructions but other float registers: retail keeps the
 * fourth matrix word in f7 and the determinant in f5, GC/2.7 f5 and f6 (14 rows in about 30
 * shapes, probes/RenderWare_RpUVAnimKeyFrame_W41.py).
 */
#include <dolphin/types.h>
#include "Game/rw/r2.h"

extern R2AnimInterpInfo lbl_8056D77C; /* the linear scheme (RpUVAnim.c) */
extern R2AnimInterpInfo lbl_8056D7AC; /* the parameterised scheme (RpUVAnim.c) */

u32 fn_8047D868(void *, void *, u32);
u32 fn_8047D458(void *, void *, u32);
u32 fn_8047D784(void *, void *, u32);
u32 fn_8047D350(void *, void *, u32);
void *fn_80454374(void *);
u32 fn_8045441C(u32, void *);
u32 fn_80454494(u32);
void fn_8047EBA0(float *);
float *fn_80453594(float *dest, const float *source);
float *fn_80453820(float *, const float *);



/* Build one key record: id, time, then the payload for the object's class. The
   payload pointer is the caller's, so the two classes choose the layout. */
u32 fn_80453018(const R2UVAnimation *anim, R2UVKeyFrame *frame, void *prev, float time,
                const float *matrix)
{
    frame->prev = prev;
    frame->time = time;
    if (anim->interpInfo->typeID == lbl_8056D77C.typeID) {
        if (fn_80453594(frame->uv, matrix) != NULL) {
            return 0;
        }
    }
    if (anim->interpInfo->typeID == lbl_8056D7AC.typeID) {
        if (fn_80453820(frame->uv, matrix) != NULL) {
            return 0;
        }
    }
    return 0;
}

/* Node stream read: a discarded word, the shared node list, then every node. */
u32 fn_804530CC(void *stream, u32 *record)
{
    u32 discarded;
    if (fn_8047D868(stream, &discarded, 4) == 0) {
        return 0;
    }
    { u32 list = (u32)fn_80454374(stream);
    if (list == 0) {
        return 0;
    }
    record[5] = list;
    { u8 *node;
    s32 i;
    u8 *base = (u8 *)record[4];
    node = base;
    for (i = 0; i < (s32)record[1]; i++, node += 0x20) {
        if (fn_8047D784(stream, node + 4, 0x1C) == 0) {
            return 0;
        }
        { u32 index;
        if (fn_8047D868(stream, &index, 4) == 0) {
            return 0;
        }
        *(u32 *)node = (u32)(base + (index << 5));
     }}
    return (u32)record;
 } }}

/* Node stream write. The arguments are the other way round from the read
   callback, and the discarded read above is mirrored by writing a zero. */
u32 fn_804531B0(u32 *record, void *stream)
{
    u32 zero = 0;
    if (fn_8047D458(stream, &zero, 4) == 0) {
        return 0;
    }
    if (fn_8045441C(record[5], stream) == 0) {
        return 0;
    }
    { s32 count;
    u8 *node;
    s32 i;
    u8 *base = (u8 *)record[4];
    node = base;
    count = (s32)record[1];
    for (i = 0; i < count; node += 0x20, i++) {
        if (fn_8047D350(stream, node + 4, 0x1C) == 0) {
            return 0;
        }
        { u32 index = (u32)(((s32)(*(u32 *)node - (u32)base)) / 32);
        if (fn_8047D458(stream, &index, 4) == 0) {
            return 0;
        }
     }}
    return 1;
 }}

u32 fn_80454494(u32);

u32 fn_804532A4(u32 *object) {
    u32 result = fn_80454494(object[5]);
    result += 4;
    result += object[1] << 5;
    return result;
}

/* A key record's six payload floats as a 0x40 byte matrix; the untouched words
   3, 7, 11 and 15 are the flags and padding. */
void fn_804532E4(float *dest, float *source)
{
    float zero = 0.0f;
    dest[0] = source[2];
    dest[1] = source[3];
    dest[2] = zero;
    dest[4] = source[4];
    dest[5] = source[5];
    dest[6] = zero;
    dest[8] = zero;
    dest[9] = zero;
    dest[10] = zero;
    dest[12] = source[6];
    dest[13] = source[7];
    dest[14] = zero;
    fn_8047EBA0(dest);
}

/* Linearly blend payload words 2..7 of two records at time. */
void fn_80453350(float *out, float *from, float *to, float time)
{
    float ratio = (time - from[1]) / (to[1] - from[1]);
    { u32 i; for (i = 2; i < 8; i++) {
        out[i] = ratio * (to[i] - from[i]) + from[i];
    } }
}

/* The same blend for a caller that has already worked out the ratio. */
void fn_804533E0(float *out, float *from, float *to, float ratio)
{
    { u32 i; for (i = 2; i < 8; i++) {
        out[i] = ratio * (to[i] - from[i]) + from[i];
    } }
}

/* Undo the 2x2 block of a record on out, then take its translation back off. */
void fn_8045345C(float *out, float *from)
{
    float determinant = from[2] * from[5] - from[3] * from[4];
    if (determinant != 0.0f) {
        float inverse = 1.0f / determinant;
        float upperRight = -from[3] * inverse;
        float lowerLeft = -from[4] * inverse;
        float lowerRight = from[5] * inverse;
        float m10 = out[4];
        float m11 = out[5];
        float m00 = out[2];
        float m01 = out[3];
        out[2] = lowerRight * m00 + upperRight * m10;
        out[3] = lowerRight * m01 + upperRight * m11;
        /* Retail multiplies both second row terms by the same cofactor; the
           (1,1) term of the adjugate is never computed. */
        out[4] = lowerLeft * m00 + lowerLeft * m10;
        out[5] = lowerLeft * m01 + lowerLeft * m11;
    }
    out[6] = out[6] - from[6];
    out[7] = out[7] - from[7];
}

/* Combine two record payloads; out may be one of the inputs. */
void fn_80453500(float *out, float *from, float *to)
{
    out[2] = from[2] * to[2] + from[3] * to[4];
    out[3] = from[2] * to[3] + from[3] * to[5];
    out[4] = from[4] * to[2] + from[5] * to[4];
    out[5] = from[4] * to[3] + from[5] * to[5];
    out[6] = from[6] + to[6];
    out[7] = from[7] + to[7];
}

float *fn_80453594(float *dest, const float *source) {
    dest[0] = source[0];
    dest[1] = source[1];
    dest[2] = source[4];
    dest[3] = source[5];
    dest[4] = source[0x0C];
    dest[5] = source[0x0D];
    return dest;
}
