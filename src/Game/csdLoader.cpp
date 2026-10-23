/* csdLoader.cpp (file name from the allocator's records): the run-time core of the Chao CSD 2D
 * library. It evaluates the animation tracks of a scene's casts (constant, linear, Hermite and
 * color keys, and texture patterns packed as index + 10000 * next index), updates the casts' state
 * and geometry down each layer's cast tree, draws the layers through a cached render state and the
 * platform layer, and loads and relocates project files. All names are inferred.
 *
 * Code generation (see Game/csdAccess.cpp for the library's flags): float constants are each
 * addressed on their own (#pragma pool_data off), while statics are reached from one base register
 * where a function uses three of them (pool_data on around GetRenderState). The library's compiler
 * did not inline calls made inside loops, nor Invalidate into Begin; #pragma dont_inline marks
 * those callers, as GC/2.7 would inline them. */
#include <string.h>

#include <dolphin/types.h>

#include "Game/CsdLib.h"

#pragma pool_data off

#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5

inline int __fpclassifyf(float x) {
    switch ((*(s32 *)&x) & 0x7F800000) {
    case 0x7F800000:
        if ((*(s32 *)&x) & 0x007FFFFF) {
            return FP_NAN;
        }
        return FP_INFINITE;
    case 0:
        if ((*(s32 *)&x) & 0x007FFFFF) {
            return FP_SUBNORMAL;
        }
        return FP_ZERO;
    }
    return FP_NORMAL;
}


extern "C" double fmod(double x, double y);
inline float fmodf(float x, float y) {
    return (float)fmod((double)x, (double)y);
}

inline float modff(float x, float *iptr) {
    float frac;
    int i;
    switch (__fpclassifyf(x)) {
    case FP_NAN:
    case FP_ZERO:
        *iptr = x;
        return x;
    case FP_INFINITE:
        *iptr = x;
        return 0.0f;
    case FP_SUBNORMAL:
        *iptr = 0.0f;
        return x;
    }
    i = (int)x;
    frac = x - (float)i;
    if (*(s32 *)&frac == 0 || (*(s32 *)&x & 0x7F800000) >= 0x4B800000) {
        *iptr = x;
        return 0.0f;
    }
    *iptr = (float)i;
    return frac;
}

namespace Chao {
namespace CSD {

/* The owner of a motion name table (name inferred). */
struct SScene2 {
    u8 field_0x0[0x34];
    s32 m_motionCount;
    u8 field_0x38[4];
    SNameIndex *m_motionNames;
};





void SplitPatternFrame(float frame, s32 *index, s32 *pattern, float *frac) {
    float ip;
    *frac = modff(frame, &ip);
    s32 i = (s32)ip;
    *pattern = i / 10000;
    *index = i % 10000;
}

/* Packs a pattern index and the fraction towards the next pattern into a pattern frame. */
float MakePatternFrame(float frac, s32 index, s32 pattern) {
    if (index > 10000) {
        fn_80361DC4("Large PatternIndex Error.\n");
        for (;;) {
        }
    }
    return frac + (float)(index + pattern * 10000);
}

void InterpolateColor(float frame, CValue *out, const SKey *k0, const SKey *k1) {
    u8 c0[4];
    u8 c1[4];
    *(s32 *)c0 = k0->m_value.m_int;
    *(s32 *)c1 = k1->m_value.m_int;
    float t;
    if (k1->m_frame - k0->m_frame <= 0) {
        t = 0.0f;
    } else {
        t = (frame - (float)k0->m_frame) / (float)(k1->m_frame - k0->m_frame);
    }
    u8 c[4];
    for (s32 i = 0; i < 4; i++) {
        c[i] = (u8)((float)c0[i] * (1.0f - t) + (float)c1[i] * t);
    }
    CValue v;
    v.m_int = *(s32 *)c;
    *out = v;
}

void InterpolateHermite(float frame, CValue *out, const SKey *k0, const SKey *k1) {
    float t;
    if (k1->m_frame - k0->m_frame <= 0) {
        t = 0.0f;
    } else {
        t = (frame - (float)k0->m_frame) / (float)(k1->m_frame - k0->m_frame);
    }
    float d = (float)(k1->m_frame - k0->m_frame);
    float dv = k1->m_value.m_float - k0->m_value.m_float;
    float t2 = t * t;
    float t3 = t2 * t;
    CValue v;
    v.m_float = k0->m_value.m_float +
                (d * k0->m_tangentIn * t +
                 ((-2.0f * dv + d * (k0->m_tangentIn + k0->m_tangentOut)) * t3 +
                  (3.0f * dv - d * (2.0f * k0->m_tangentIn + k0->m_tangentOut)) * t2));
    *out = v;
}

void InterpolateLinear(float frame, CValue *out, const SKey *k0, const SKey *k1) {
    float t;
    if (k1->m_frame - k0->m_frame <= 0) {
        t = 0.0f;
    } else {
        t = (frame - (float)k0->m_frame) / (float)(k1->m_frame - k0->m_frame);
    }
    CValue v;
    v.m_float = k0->m_value.m_float * (1.0f - t) + k1->m_value.m_float * t;
    *out = v;
}

void InterpolateConst(float frame, CValue *out, const SKey *k0, const SKey *k1) {
    *out = k0->m_value;
}

void Interpolate(float frame, CValue *out, const SKey *k0, const SKey *k1) {
    switch (k0->m_interp) {
    case 0:
        InterpolateConst(frame, out, k0, k1);
        break;
    case 1:
        InterpolateLinear(frame, out, k0, k1);
        break;
    case 2:
        InterpolateHermite(frame, out, k0, k1);
        break;
    default:
        InterpolateConst(frame, out, k0, k1);
        break;
    }
}


void EvaluateTrack(float frame, float speed, CValue *out, const STrack *track, s32 attr, s32 *playing) {
    if ((float)track->m_keys[track->m_keyCount - 1].m_frame <= frame) {
        if (track->m_loop == 2) {
            s32 last = track->m_keys[track->m_keyCount - 1].m_frame;
            if (last == 0) {
                frame = 0.0f;
                if (speed > frame) {
                    *playing = 0;
                }
            } else {
                frame = fmodf(frame, (float)last);
            }
        } else {
            if (speed > 0.0f) {
                *playing = 0;
            }
            *out = track->m_keys[track->m_keyCount - 1].m_value;
            return;
        }
    }
    if ((float)track->m_keys[0].m_frame > frame) {
        if (speed < 0.0f) {
            *playing = 0;
        }
        *out = track->m_keys[0].m_value;
        return;
    }
    const SKey *k0 = 0;
    const SKey *k1 = 0;
    for (s32 i = 0; i < track->m_keyCount; i++) {
        k0 = k1;
        k1 = &track->m_keys[i];
        if ((float)k1->m_frame > frame) {
            break;
        }
    }
    if (k0 == 0) {
        k0 = k1;
    }
    switch (attr) {
    case 0:
    case 12:
    case 13:
    case 14:
        InterpolateConst(frame, out, k0, k1);
        break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        Interpolate(frame, out, k0, k1);
        break;
    case 6: {
        CValue v;
        CValue tmp;
        Interpolate(frame, &tmp, k0, k1);
        v = tmp;
        float t;
        if (k1->m_value.m_float == k0->m_value.m_float) {
            t = 0.0f;
        } else {
            t = (v.m_float - k0->m_value.m_float) / (k1->m_value.m_float - k0->m_value.m_float);
        }
        CValue r;
        r.m_float = MakePatternFrame(t, (s32)k0->m_value.m_float, (s32)k1->m_value.m_float);
        *out = r;
        break;
    }
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
        InterpolateColor(frame, out, k0, k1);
        break;
    default:
        InterpolateConst(frame, out, k0, k1);
        break;
    }
}




/* A texture pattern: the texture and its rectangle. */






void SetPattern(SCast *cast, const SCastInfo *info, const SSceneInfo *data) {
    SVertexT texTmp0[4];
    SVertexT texTmp1[4];
    SVertex tmp0[4];
    SVertex tmp1[4];
    SVec2 uv[2];
    s32 index0;
    s32 index1;
    float frac;
    if (info->m_type != 1) {
        return;
    }
    const SPattern *pattern = 0;
    SplitPatternFrame(cast->m_pattern, &index0, &index1, &frac);
    if (index0 >= 0 && index0 < info->m_patternCount) {
        s32 n = info->m_patternTable[index0];
        if (n >= 0) {
            pattern = &data->m_patterns[n];
            if (frac != 0.0f || index1 < info->m_patternCount) {
                const SPattern *next = &data->m_patterns[info->m_patternTable[index1]];
                for (s32 i = 0; i < 2; i++) {
                    uv[i].m_x = pattern->m_uv[i].m_x * (1.0f - frac) + next->m_uv[i].m_x * frac;
                    uv[i].m_y = pattern->m_uv[i].m_y * (1.0f - frac) + next->m_uv[i].m_y * frac;
                }
            } else {
                uv[0] = pattern->m_uv[0];
                uv[1] = pattern->m_uv[1];
            }
        }
    }
    if (pattern != 0) {
        if (IsFlipX(info)) {
            float t = uv[0].m_x;
            uv[0].m_x = uv[1].m_x;
            uv[1].m_x = t;
        }
        if (IsFlipY(info)) {
            float t = uv[0].m_y;
            uv[0].m_y = uv[1].m_y;
            uv[1].m_y = t;
        }
        if (cast->m_vertices == 0) {
            cast->m_vertices = cast->m_vertexBuffer;
            cast->m_transformed = cast->m_transformedBuffer;
            cast->m_textures = &cast->m_textureBuffer;
            cast->m_allocated = 0;
            cast->m_quadCount = 1;
        } else if (cast->m_textures == 0) {
            for (s32 i = 0; i < 4; i++) {
                tmp0[i] = ((SVertex *)cast->m_vertices)[i];
                tmp1[i] = ((SVertex *)cast->m_transformed)[i];
            }
            if (cast->m_allocated != 0) {
                fn_80360528(cast->m_vertices);
                fn_80360528(cast->m_transformed);
            }
            cast->m_vertices = cast->m_vertexBuffer;
            cast->m_transformed = cast->m_transformedBuffer;
            cast->m_textures = &cast->m_textureBuffer;
            for (s32 i = 0; i < 4; i++) {
                ((SVertexT *)cast->m_vertices)[i].m_pos = tmp0[i].m_pos;
                ((SVertexT *)cast->m_vertices)[i].m_color = tmp0[i].m_color;
                ((SVertexT *)cast->m_transformed)[i].m_pos = tmp1[i].m_pos;
                ((SVertexT *)cast->m_transformed)[i].m_color = tmp1[i].m_color;
            }
            cast->m_allocated = 0;
        }
        cast->m_textures[0] = pattern->m_texture;
        ((SVertexT *)cast->m_vertices)[0].m_uv.m_x = uv[0].m_x;
        ((SVertexT *)cast->m_vertices)[0].m_uv.m_y = uv[0].m_y;
        ((SVertexT *)cast->m_vertices)[1].m_uv.m_x = uv[0].m_x;
        ((SVertexT *)cast->m_vertices)[1].m_uv.m_y = uv[1].m_y;
        ((SVertexT *)cast->m_vertices)[2].m_uv.m_x = uv[1].m_x;
        ((SVertexT *)cast->m_vertices)[2].m_uv.m_y = uv[0].m_y;
        ((SVertexT *)cast->m_vertices)[3].m_uv.m_x = uv[1].m_x;
        ((SVertexT *)cast->m_vertices)[3].m_uv.m_y = uv[1].m_y;
        for (s32 i = 0; i < 4; i++) {
            ((SVertexT *)cast->m_transformed)[i].m_uv = ((SVertexT *)cast->m_vertices)[i].m_uv;
        }
    } else {
        if (cast->m_vertices == 0) {
            cast->m_vertices = cast->m_vertexBuffer;
            cast->m_transformed = cast->m_transformedBuffer;
            cast->m_textures = 0;
            cast->m_allocated = 0;
            cast->m_quadCount = 1;
        } else if (cast->m_textures != 0) {
            if (cast->m_allocated != 0) {
                fn_80360528(cast->m_textures);
            }
            cast->m_textures = 0;
            for (s32 i = 0; i < 4; i++) {
                texTmp0[i] = ((SVertexT *)cast->m_vertices)[i];
                texTmp1[i] = ((SVertexT *)cast->m_transformed)[i];
            }
            if (cast->m_allocated != 0) {
                fn_80360528(cast->m_vertices);
                fn_80360528(cast->m_transformed);
            }
            cast->m_vertices = cast->m_vertexBuffer;
            cast->m_transformed = cast->m_transformedBuffer;
            for (s32 i = 0; i < 4; i++) {
                ((SVertex *)cast->m_vertices)[i].m_pos = texTmp0[i].m_pos;
                ((SVertex *)cast->m_vertices)[i].m_color = texTmp0[i].m_color;
                ((SVertex *)cast->m_transformed)[i].m_pos = texTmp1[i].m_pos;
                ((SVertex *)cast->m_transformed)[i].m_color = texTmp1[i].m_color;
            }
            cast->m_allocated = 0;
        }
    }
}

void TransformVertex(SVec2 *out, const SCast *cast, const SVec2 *in);

void TransformCast(SCast *cast, const SSceneInfo *info) {
    float aspect = 1.3333f;
    if (info->m_version >= 1) {
        aspect = info->m_aspect;
    }
    aspect /= GetScreenAspect();
    float ox;
    float oy;
    GetScreenOffset(&ox, &oy);
    if (cast->m_textures != 0) {
        SVertexT *src = (SVertexT *)cast->m_vertices;
        SVertexT *dst = (SVertexT *)cast->m_transformed;
        s32 n = 0;
        for (s32 q = 0; q < cast->m_quadCount; q++) {
            for (s32 i = 0; i < 4; i++, n++) {
                src[n].m_pos.m_y /= aspect;
                TransformVertex(&dst[n].m_pos, cast, &src[n].m_pos);
                dst[n].m_pos.m_y *= aspect;
                src[n].m_pos.m_y *= aspect;
                dst[n].m_pos.m_x += ox;
                dst[n].m_pos.m_y += oy;
            }
        }
    } else {
        SVertex *src = (SVertex *)cast->m_vertices;
        SVertex *dst = (SVertex *)cast->m_transformed;
        s32 n = 0;
        for (s32 q = 0; q < cast->m_quadCount; q++) {
            for (s32 i = 0; i < 4; i++, n++) {
                src[n].m_pos.m_y /= aspect;
                TransformVertex(&dst[n].m_pos, cast, &src[n].m_pos);
                dst[n].m_pos.m_y *= aspect;
                src[n].m_pos.m_y *= aspect;
                dst[n].m_pos.m_x += ox;
                dst[n].m_pos.m_y += oy;
            }
        }
    }
}


void ApplyCastColor(SCast *cast) {
    u32 color = cast->m_color;
    if (cast->m_textures != 0) {
        SVertexT *src = (SVertexT *)cast->m_vertices;
        SVertexT *dst = (SVertexT *)cast->m_transformed;
        s32 n = 0;
        for (s32 q = 0; q < cast->m_quadCount; q++) {
            for (s32 i = 0; i < 4; i++, n++) {
                u32 c = src[n].m_color;
                u8 *a = (u8 *)&c;
                u8 *b = (u8 *)&color;
                for (s32 k = 0; k < 4; k++) {
                    a[k] = (a[k] * b[k]) / 255;
                }
                dst[n].m_color = c;
            }
        }
    } else {
        SVertex *src = (SVertex *)cast->m_vertices;
        SVertex *dst = (SVertex *)cast->m_transformed;
        s32 n = 0;
        for (s32 q = 0; q < cast->m_quadCount; q++) {
            for (s32 i = 0; i < 4; i++, n++) {
                u32 c = src[n].m_color;
                u8 *a = (u8 *)&c;
                u8 *b = (u8 *)&color;
                for (s32 k = 0; k < 4; k++) {
                    a[k] = (a[k] * b[k]) / 255;
                }
                dst[n].m_color = c;
            }
        }
    }
}
void SetCornerColors(SCast *cast) {
    if (cast->m_textures != 0) {
        for (s32 q = 0; q < cast->m_quadCount; q++) {
            for (s32 i = 0; i < 4; i++) {
                ((SVertexT *)cast->m_vertices)[i + q * 4].m_color = cast->m_cornerColors[i];
            }
        }
    } else {
        for (s32 q = 0; q < cast->m_quadCount; q++) {
            for (s32 i = 0; i < 4; i++) {
                ((SVertex *)cast->m_vertices)[i + q * 4].m_color = cast->m_cornerColors[i];
            }
        }
    }
}

void UpdateCast(SCast *cast, const SCastInfo *info, const SCast *parent, const SSceneInfo *scene) {
    u32 mode = info->m_mode;
    if (cast->m_changed & 0x10000) {
        SMatrix *m = GetMatrix();
        float x = cast->m_x;
        float y = cast->m_y;
        ScaleToScreen(&x, &y);
        float aspect = 1.3333f;
        if (scene->m_version >= 1) {
            aspect = scene->m_aspect;
        }
        float screen = GetScreenAspect();
        y *= screen / aspect;

        switch (mode & 0x300) {
        case 0x300:
            TranslateMatrix3(x, y);
            break;
        case 0x100:
            TranslateMatrix1(x, y);
            break;
        case 0x200:
            TranslateMatrix2(x, y);
            break;
        case 0x0:
            TranslateMatrix0(x, y);
            break;
        }
        if (!(mode & 2)) {
            m->m[0] = m->m_scale[0];
            m->m[1] = m->m_scale[1];
            m->m[3] = m->m_scale[3];
            m->m[4] = m->m_scale[4];
            m->m_base[0] = 1.0f;
            m->m_base[1] = 0.0f;
            m->m_base[3] = 0.0f;
            m->m_base[4] = 1.0f;
        }
        float sx = 1.0f;
        float sy = sx;
        s32 rescale = 0;
        switch (mode & 0xC00) {
        case 0x400:
            if (!(mode & 2)) {
                m->m[1] = 0.0f;
                m->m[3] = 0.0f;
                m->m[4] = sx;
                m->m_scale[1] = 0.0f;
                m->m_scale[3] = 0.0f;
                m->m_scale[4] = sx;
            } else {
                rescale = 1;
                m->m[0] = m->m_base[0];
                m->m[1] = m->m_base[1];
                m->m[3] = m->m_base[3];
                m->m[4] = m->m_base[4];
                m->m_scale[4] = sx;
                sx = m->m_scale[0];
            }
            break;
        case 0x800:
            if (!(mode & 2)) {
                m->m[0] = sx;
                m->m[1] = 0.0f;
                m->m[3] = 0.0f;
                m->m_scale[0] = sx;
                m->m_scale[1] = 0.0f;
                m->m_scale[3] = 0.0f;
            } else {
                rescale = 1;
                m->m[0] = m->m_base[0];
                m->m[1] = m->m_base[1];
                m->m[3] = m->m_base[3];
                m->m[4] = m->m_base[4];
                m->m_scale[0] = sx;
                sy = m->m_scale[4];
            }
            break;
        case 0x0:
            if (!(mode & 2)) {
                m->m[0] = sx;
                m->m[1] = 0.0f;
                m->m[3] = 0.0f;
                m->m[4] = sx;
                m->m_scale[0] = sx;
                m->m_scale[1] = 0.0f;
                m->m_scale[3] = 0.0f;
                m->m_scale[4] = sx;
            } else {
                m->m[0] = m->m_base[0];
                m->m[1] = m->m_base[1];
                m->m[3] = m->m_base[3];
                m->m[4] = m->m_base[4];
                m->m_scale[0] = sx;
                m->m_scale[1] = 0.0f;
                m->m_scale[3] = 0.0f;
                m->m_scale[4] = sx;
            }
            break;
        }
        LoadMatrix();
        RotateMatrix(cast->m_rotation);
        ScaleMatrix(cast->m_scaleX, cast->m_scaleY);
        if (rescale) {
            ScaleMatrix(sx, sy);
        }
        TransformCast(cast, scene);
    }
    if (cast->m_changed & 0x20000) {
        cast->m_hide = cast->m_attrHide;
        if (parent != 0 && (mode & 0x10)) {
            cast->m_hide = cast->m_attrHide | parent->m_hide;
        }
    }
    if (cast->m_changed & 0x40000) {
        cast->m_color = cast->m_attrColor;
        if (parent != 0 && (mode & 0x8)) {
            u32 a = cast->m_attrColor;
            u32 b = parent->m_color;
            u32 r;
            for (s32 k = 0; k < 4; k++) {
                ((u8 *)&r)[k] = (((u8 *)&a)[k] * ((u8 *)&b)[k]) / 255;
            }
            cast->m_color = r;

        }
        ApplyCastColor(cast);
    }
}


void EvaluateCast(float frame, float speed, SCast *cast, const SCastInfo *info, const SCastMotion *motion,
                  const SSceneInfo *scene) {
    CValue *values = (CValue *)&cast->m_attrHide;
    s32 track = 0;
    s32 pending = 0x7FFF;
    for (s32 i = 0; i < 15; i++) {
        if (cast->m_overrides & (1 << i)) {
            values[i] = cast->m_overrideValues[i];
            cast->m_changed |= (1 << i);
            if (motion != 0 && (motion->m_attributes & (1 << i))) {
                track++;
            }
        } else if (motion != 0 && (motion->m_attributes & (1 << i))) {
            if (!(cast->m_changed & (1 << i))) {
                s32 playing = 1;
                CValue v;
                EvaluateTrack(frame, speed, &v, &motion->m_tracks[track], i, &playing);
                values[i] = v;
                if (playing == 0) {
                    cast->m_changed |= (1 << i);
                }
            }
            track++;
        } else if (!(cast->m_changed & (1 << i))) {
            if (info->m_attributes == GetAllAttributes()) {
                values[i] = info->m_values[i];
            } else {
                CValue v;
                GetAttribute(&v, i, info->m_attributes, info->m_values);
                values[i] = v;
            }
            cast->m_changed |= (1 << i);
        }
        pending &= ~(1 << i);
        if (pending == (pending & (cast->m_changed & 0x7FFF))) {
            break;
        }
    }
    if (!(cast->m_changed & 0x200000) && (cast->m_changed & 0x3E) == 0x3E) {
        cast->m_changed |= 0x200000;
    }
    if ((cast->m_changed & 0x7FFF) == 0x7FFF) {
        cast->m_changed |= 0x80000;
    }
}



void UpdateCastFrame(float frame, float speed, SCast *cast, const SCastInfo *info, const SCastMotion *motion,
                     const SCast *parent, const SSceneInfo *scene, s32 force) {
    if (cast->m_vertices == 0) {
        return;
    }
    if (info->field_0x8 == 0) {
        return;
    }
    if (!(force & 1)) {
        s32 changed = cast->m_changed;
        if (!(changed & 0x80000)) {
            s32 hide = cast->m_attrHide;
            u32 color = cast->m_attrColor;
            float pattern = cast->m_pattern;
            s32 corners = (changed & 0xF00) == 0xF00;
            EvaluateCast(frame, speed, cast, info, motion, scene);
            if (!(changed & 0x200000)) {
                cast->m_changed |= 0x18000;
            }
            if (hide != cast->m_attrHide) {
                cast->m_changed |= 0x28000;
            }
            if (color != cast->m_attrColor) {
                cast->m_changed |= 0x48000;
            }
            if (pattern != cast->m_pattern) {
                SetPattern(cast, info, scene);
            }
            if (!corners) {
                SetCornerColors(cast);
                cast->m_changed |= 0x48000;
            }
        }
    }
    if (cast->m_changed & 0x8000) {
        UpdateCast(cast, info, parent, scene);
    }
}








void UpdateCastTree(float frame, float speed, SLayer *layer, s32 index, const SLayerInfo *info,
                    const SLayerMotion *motion, void *texList, SCast *parent, const SSceneInfo *scene, s32 force,
                    s32 depth) {
    SCast *cast = &layer->m_casts[index];
    if (depth > 0) {
        cast->m_parent = parent;
    } else {
        cast->m_parent = 0;
    }
    if (force & 8) {
        InitCastFromInfo(&layer->m_casts[index], info->m_casts[index], scene, texList);
        layer->m_casts[index].m_changed = 0;
    }
    if (force & 2) {
        layer->m_casts[index].m_changed = 0;
    }
    s32 skip = 0;
    if (parent != 0 && (parent->m_changed & 0x8000)) {
        cast->m_changed &= ~0x78000;
        cast->m_changed |= parent->m_changed & 0x78000;
    } else if (cast->m_changed & 0x100000) {
        skip = 1;
    } else {
        cast->m_changed &= ~0x78000;
    }
    if (!skip) {
        PushMatrix(&layer->m_casts[index].m_matrix);
        if (motion != 0) {
            UpdateCastFrame(frame, speed, &layer->m_casts[index], info->m_casts[index], &motion->m_casts[index], parent,
                            scene, force);
        } else {
            UpdateCastFrame(frame, speed, &layer->m_casts[index], info->m_casts[index], 0, parent, scene, force);
        }
        if (cast->m_changed & 0x80000) {
            cast->m_changed |= 0x100000;
        } else {
            cast->m_changed &= ~0x100000;
            if (parent != 0) {
                cast->m_changed &= ~0x100000;
            }
        }
        s32 child = info->m_tree[index].m_child;
        if (child >= 0) {
            UpdateCastTree(frame, speed, layer, child, info, motion, texList, cast, scene, force,
                           depth + 1);
        }
        if (!(cast->m_changed & 0x100000)) {
            if (parent != 0) {
                parent->m_changed &= ~0x100000;
            }
            layer->m_updated = 0;
        }
        PopMatrix();
    }
    s32 sibling = info->m_tree[index].m_sibling;
    if (sibling >= 0) {
        UpdateCastTree(frame, speed, layer, sibling, info, motion, texList, parent, scene, force,
                       depth);
    }
    cast->m_updated = 1;
}

s32 FindMotionIndex(const SScene2 *owner, const char *name) {
    SNameIndex key;
    key.m_name = name;
    key.m_index = -1;
    SNameIndex *found =
        (SNameIndex *)fn_803A7B40(&key, owner->m_motionNames, owner->m_motionCount, sizeof(SNameIndex), CompareName);
    if (found != 0) {
        return found->m_index;
    }
    return -1;
}

void UpdateScene(float frame, SSceneState *state, const SSceneInfo *info, const SMotion *motion, void *texList,
                 SCast *parent) {
    if (frame < 0.0f) {
        frame = 0.0f;
    }
    float speed = frame - state->m_frame;
    s32 force = 0;
    if (state->m_motion != motion || state->m_texList != texList) {
        force |= 2;
    } else if (0.0f == speed && state->m_refresh == 0) {
        force |= 1;
    }
    state->m_refresh = 0;
    {
        float x;
        float y;
        GetScreenSize(&x, &y);
        if (x != state->m_screenSize[0] || y != state->m_screenSize[1]) {
            state->m_screenSize[0] = x;
            force |= 8;
            force &= ~1;
            force |= 2;
            state->m_screenSize[1] = y;
        }
        GetScreenBase(&x, &y);
        if (x != state->m_screenBase[0] || y != state->m_screenBase[1]) {
            state->m_screenBase[0] = x;
            force |= 8;
            force &= ~1;
            force |= 2;
            state->m_screenBase[1] = y;
        }
    }
    {
        float x;
        float y;
        GetScreenOffset(&x, &y);
        if (x != state->m_screenOffset[0] || y != state->m_screenOffset[1]) {
            state->m_screenOffset[0] = x;
            force |= 8;
            force &= ~1;
            force |= 2;
            state->m_screenOffset[1] = y;
        }
    }
    if (state->m_parent != parent || (parent != 0 && (parent->m_changed & 0x8000))) {
        force |= 4;
    }
    if (speed * state->m_speed < 0.0f) {
        force |= 2;
    }
    state->m_frame = frame;
    state->m_motion = motion;
    state->m_texList = texList;
    if (0.0f != speed) {
        state->m_speed = speed;
    }
    state->m_parent = parent;
    if (!(force & 4) && ((force & 1) || (!(force & 2) && state->m_updated != 0))) {
        return;
    }
    s32 changed = 0;
    if (parent != 0) {
        changed = parent->m_changed;
        PushMatrix(&parent->m_matrix);
    }
    state->m_updated = 1;
    if (motion != 0) {
        for (s32 i = 0; i < info->m_layerCount; i++) {
            const SLayerMotion *layerMotion = &motion->m_layers[i];
            const SLayerInfo *layerInfo = &info->m_layers[i];
            s32 root = layerInfo->m_root;
            SLayer *layer = &state->m_layers[i];
            if (root >= 0) {
                if ((force & 4) || (force & 2) || (!(force & 1) && layer->m_updated == 0)) {
                    layer->m_updated = 1;
                    UpdateCastTree(frame, speed, layer, root, layerInfo, layerMotion, texList, parent, info, force, 0);
                }
            }
            if (state->m_layers[i].m_updated == 0) {
                state->m_updated = 0;
            }
        }
    } else {
        for (s32 i = 0; i < info->m_layerCount; i++) {
            const SLayerInfo *layerInfo = &info->m_layers[i];
            s32 root = layerInfo->m_root;
            SLayer *layer = &state->m_layers[i];
            if (root >= 0) {
                if ((force & 4) || (force & 2) || (!(force & 1) && layer->m_updated == 0)) {
                    layer->m_updated = 1;
                    UpdateCastTree(frame, speed, layer, root, layerInfo, 0, texList, parent, info, force, 0);
                }
            }
            if (state->m_layers[i].m_updated == 0) {
                state->m_updated = 0;
            }
        }
    }
    if (parent != 0) {
        parent->m_changed = changed;
        PopMatrix();
    }
}




/* Swaps the byte order of a value of size bytes. */
void SwapEndian(void *p, s32 size) {
    switch (size) {
    case 2:
        __sthbrx(*(u16 *)p, p, 0);
        break;
    case 3:
        break;
    case 4:
        __stwbrx(*(u32 *)p, p, 0);
        break;
    }
}




/* The cached render state (names inferred). */
struct SRenderState {
    SRenderState() : m_beginCount(0), m_valid(0), m_dirty(0) {
        m_texture = 0;
        m_wrapS = -1;
        m_wrapT = -1;
        m_filter = -1;
        m_blend = -1;
        m_alpha = -1;
        m_textured = 0;
        m_lastTextured = 0;
        ClearParams();
    }

    void ClearParams() {
        for (s32 i = 0; i < 3; i++) {
            m_lastParams[i] = 0;
            m_params[i] = 0;
        }
    }

    void SetParams(const s32 *params);
    void Reset();
    void Invalidate();
    void End();
    void Begin();
    void SetTextured(s32 textured);
    void Apply();
    void SetAlpha(s32 alpha);
    void SetBlend(s32 blend);
    void SetFilter(s32 filter);
    void SetWrap(s32 s, s32 t);
    void SetTexture(const RCPtr<CTexList> &texList, s32 index);

    /* 0x00 */ s32 m_beginCount;
    /* 0x04 */ s32 m_valid;
    /* 0x08 */ s32 m_dirty;
    /* 0x0C */ RCPtr<CTexList> m_texList;
    /* 0x10 */ s32 m_texture;
    /* 0x14 */ s32 m_wrapS;
    /* 0x18 */ s32 m_wrapT;
    /* 0x1C */ s32 m_filter;
    /* 0x20 */ s32 m_blend;
    /* 0x24 */ s32 m_alpha;
    /* 0x28 */ s32 m_textured;
    /* 0x2C */ s32 m_lastTextured;
    /* 0x30 */ s32 m_params[3];
    /* 0x3C */ s32 m_lastParams[3];
};

SRenderState *GetRenderState();


void DrawQuads(void *scene, const SCastInfo *info, const RCPtr<CTexList> &texList, void *unused, SCast *cast) {
    if (cast->m_textures == 0) {
        return;
    }
    s32 *params = cast->m_params;
    GetRenderState()->SetParams(params);
    if (cast->m_textures != 0) {
        s32 filter = GetFilter(info);
        GetRenderState()->SetFilter(filter);
        s32 s = GetWrapS(info);
        s32 t = GetWrapT(info);
        GetRenderState()->SetWrap(t, s);
    }
    switch (GetBlendMode(info)) {
    case 0:
        GetRenderState()->SetBlend(1);
        break;
    case 1:
        GetRenderState()->SetBlend(0);
        break;
    }
    s32 alpha;
    switch (GetAlphaMode(info)) {
    case 0:
    case 2:
        alpha = 1;
        break;
    case 1:
        alpha = 0;
        break;
    case 3:
        alpha = 0;
        break;
    default:
        alpha = 1;
        break;
    }
    GetRenderState()->SetAlpha(alpha);
    s32 *textures = cast->m_textures;
    s32 count = cast->m_quadCount;
    s32 n = 0;
    for (s32 q = 0; q < count; q++, n += 4) {
        if (textures[q] >= 0) {
            GetRenderState()->SetTextured(1);
            GetRenderState()->SetTexture(texList, cast->m_textures[q]);
            GetRenderState()->Apply();
            GetPlatform()->DrawTextured(&((SVertexT *)cast->m_transformed)[n], 4, params, 3);
        } else {
            SVertex vertices[4];
            SVertexT *v = (SVertexT *)cast->m_transformed;
            vertices[0].m_pos = v[n].m_pos;
            vertices[0].m_color = v[n].m_color;
            vertices[1].m_pos = v[n + 1].m_pos;
            vertices[1].m_color = v[n + 1].m_color;
            vertices[2].m_pos = v[n + 2].m_pos;
            vertices[2].m_color = v[n + 2].m_color;
            vertices[3].m_pos = v[n + 3].m_pos;
            vertices[3].m_color = v[n + 3].m_color;
            GetRenderState()->SetTextured(0);
            GetRenderState()->Apply();
            GetPlatform()->Draw(vertices, 4, params, 3);
        }
    }
}

void DrawSprite(void *scene, const SCastInfo *info, const RCPtr<CTexList> &texList, SCast *cast) {
    s32 *textures = cast->m_textures;
    if (textures != 0) {
        GetRenderState()->SetTexture(texList, *textures);
    }
    GetRenderState()->SetParams(cast->m_params);
    if (cast->m_textures != 0) {
        s32 filter = GetFilter(info);
        GetRenderState()->SetFilter(filter);
        s32 s = GetWrapS(info);
        s32 t = GetWrapT(info);
        GetRenderState()->SetWrap(t, s);
    }
    switch (GetBlendMode(info)) {
    case 0:
        GetRenderState()->SetBlend(1);
        break;
    case 1:
        GetRenderState()->SetBlend(0);
        break;
    }
    s32 alpha;
    switch (GetAlphaMode(info)) {
    case 0:
    case 2:
        alpha = 1;
        break;
    case 1:
        alpha = 0;
        break;
    case 3:
        alpha = 0;
        break;
    default:
        alpha = 1;
        break;
    }
    GetRenderState()->SetAlpha(alpha);
    if (cast->m_textures != 0) {
        GetRenderState()->SetTextured(1);
        GetRenderState()->Apply();
        GetPlatform()->DrawTextured(cast->m_transformed, 4, cast->m_params, 3);
    } else {
        GetRenderState()->SetTextured(0);
        GetRenderState()->Apply();
        GetPlatform()->Draw(cast->m_transformed, 4, cast->m_params, 3);
    }
}


void TransformVertex(SVec2 *out, const SCast *cast, const SVec2 *in) {
    float y = in->m_y;
    float x = in->m_x;
    float rx = cast->m_matrix.m[2] + (cast->m_matrix.m[0] * x + cast->m_matrix.m[1] * y);
    float ry = cast->m_matrix.m[5] + (cast->m_matrix.m[3] * x + cast->m_matrix.m[4] * y);
    out->m_x = rx;
    out->m_y = ry;
}


void DrawCast(void *scene, const SCastInfo *info, const RCPtr<CTexList> &texList, void *unused, SCast *cast) {
    if (info->field_0x8 != 0 && cast->m_hide == 0 && cast->m_updated != 0) {
        switch (info->m_type) {
        case 0:
            return;
        case 1:
            DrawSprite(scene, info, texList, cast);
            break;
        case 2:
            DrawQuads(scene, info, texList, unused, cast);
            break;
        }
    }
}


#pragma dont_inline on
void DrawLayer(void *scene, const SLayerInfo *info, const RCPtr<CTexList> &texList, void *unused, SLayer *layer) {
    s32 count;
    SCast *states;
    SCastInfo **casts;
    s32 i;
    casts = info->m_casts;
    states = layer->m_casts;
    count = info->m_castCount;
    for (i = 0; i < count; i++) {
        DrawCast(scene, casts[i], texList, unused, &states[i]);
    }
}
#pragma dont_inline reset


void DrawScene(SSceneInfo *scene, const RCPtr<CTexList> &texList, void *unused, SSceneState *state) {
    s32 count;
    SLayer *states;
    SLayerInfo *layers;
    GetRenderState()->Begin();
    layers = scene->m_layers;
    states = state->m_layers;
    count = scene->m_layerCount;
    for (s32 i = 0; i < count; i++) {
        DrawLayer(scene, &layers[i], texList, unused, &states[i]);
    }
    GetRenderState()->End();
}


void SRenderState::SetParams(const s32 *params) {
    for (s32 i = 0; i < 3; i++) {
        if (m_params[i] != params[i]) {
            m_params[i] = params[i];
            m_dirty = 1;
        }
    }
}

void SRenderState::Reset() {
    EndPlatform(GetPlatform());
    m_texList = (CTexList *)0;
    if (m_valid != 0) {
        if (m_lastTextured != 0) {
            GetPlatform()->BeginTextured(m_params, 3);
        } else {
            GetPlatform()->BeginColored(m_params, 3);
        }
    }
}

void SRenderState::Invalidate() {
    BeginPlatform(GetPlatform());
    m_valid = 0;
    m_dirty = 1;
}

void SRenderState::End() {
    m_beginCount--;
    if (m_beginCount == 0) {
        Reset();
    }
}

#pragma dont_inline on
void SRenderState::Begin() {
    if (m_beginCount == 0) {
        Invalidate();
    }
    m_beginCount++;
}
#pragma dont_inline reset

void SRenderState::SetTextured(s32 textured) {
    if (m_textured == textured) {
        return;
    }
    m_textured = textured;
    m_dirty = 1;
}

void SRenderState::Apply() {
    if (m_dirty == 0) {
        return;
    }
    if (m_valid != 0) {
        if (m_lastTextured != 0) {
            GetPlatform()->BeginTextured(m_lastParams, 3);
        } else {
            GetPlatform()->BeginColored(m_lastParams, 3);
        }
    }
    if (m_texList.Get() != 0) {
        m_texList.Get()->SetTexture(m_texture, m_params, 3);
        m_texList.Get()->SetWrap(m_wrapS, m_wrapT, m_params, 3);
        m_texList.Get()->SetFilter(m_texture, m_filter, m_params, 3);
    }
    GetPlatform()->SetBlend(m_blend, m_params, 3);
    if (m_textured != 0) {
        GetPlatform()->SetAlphaTest(m_alpha, m_params, 3);
    } else {
        GetPlatform()->SetAlphaTestOff(m_alpha, m_params, 3);
    }
    for (s32 i = 0; i < 3; i++) {
        m_lastParams[i] = m_params[i];
    }
    m_lastTextured = m_textured;
    m_valid = 1;
    m_dirty = 0;
}

void SRenderState::SetAlpha(s32 alpha) {
    if (m_alpha == alpha) {
        return;
    }
    m_alpha = alpha;
    m_dirty = 1;
}

void SRenderState::SetBlend(s32 blend) {
    if (m_blend == blend) {
        return;
    }
    m_blend = blend;
    m_dirty = 1;
}

void SRenderState::SetFilter(s32 filter) {
    if (m_filter == filter) {
        return;
    }
    m_filter = filter;
    m_dirty = 1;
}

void SRenderState::SetWrap(s32 s, s32 t) {
    if (m_wrapS == s && m_wrapT == t) {
        return;
    }
    m_wrapS = s;
    m_wrapT = t;
    m_dirty = 1;
}

inline int SameTexList(const RCPtr<CTexList> &a, const RCPtr<CTexList> &b) {
    if (a.m_object != 0 && b.m_object != 0) {
        return a.m_object->m_object == b.m_object->m_object;
    }
    return a.m_object == b.m_object;
}

void SRenderState::SetTexture(const RCPtr<CTexList> &texList, s32 index) {
    if (!SameTexList(m_texList, texList) || m_texture != index) {
        m_texList = texList;
        m_texture = index;
        m_dirty = 1;
    }
}

#pragma pool_data on
SRenderState *GetRenderState() {
    static SRenderState s_state;
    return &s_state;
}
#pragma pool_data off


/* Shuts the library down (name inferred). */
void Shutdown() {
    ShutdownMatrix();
    ShutdownTexList();
}

/* Starts the library up (name inferred). */
void Initialize() {
    InitMatrix();
    InitTexList();
}

/* Copies a loaded project file: the project part, and the texture part when the file has one (a
 * "CPAF" file, in either byte order). */
void LoadFile(void **project, u32 *projectSize, void **textures, u32 *texturesSize, const void *data, u32 size) {
    const u32 *p = (const u32 *)data;
    if (p[0] == 'CPAF') {
        u32 n = p[1];
        p += 2;
        *project = fn_80360564(n, "csdLoader.cpp", 0x1D7);
        *projectSize = n;
        memcpy(*project, p, n);
        p = (const u32 *)((const u8 *)p + n);
        n = p[0];
        *textures = fn_80360564(n, "csdLoader.cpp", 0x1E0);
        *texturesSize = n;
        memcpy(*textures, p + 1, n);
    } else if (p[0] == 'FAPC') {
        u32 n = p[1];
        p += 2;
        SwapEndian(&n, 4);
        *project = fn_80360564(n, "csdLoader.cpp", 0x1EE);
        *projectSize = n;
        memcpy(*project, p, n);
        p = (const u32 *)((const u8 *)p + n);
        n = p[0];
        SwapEndian(&n, 4);
        *textures = fn_80360564(n, "csdLoader.cpp", 0x1F8);
        *texturesSize = n;
        memcpy(*textures, p + 1, n);
    } else {
        *textures = 0;
        *project = fn_80360564(size, "csdLoader.cpp", 0x200);
        *projectSize = size;
        *texturesSize = 0;
        memcpy(*project, data, size);
    }
}


/* A chunk of a project file. */
struct SChunk {
    u32 m_id;
    u32 m_size;
    u32 m_offset;
};

/* The header of a project file. */
struct SFileHeader {
    u32 m_id;
    u32 field_0x4;
    s32 m_chunkCount;
    u32 m_dataOffset;
    u32 field_0x10;
    u32 m_relocOffset;
};

/* The relocation table of a project file. */
struct SRelocTable {
    u32 field_0x0;
    u32 field_0x4;
    s32 m_count;
    u32 field_0xC;
    u32 m_offsets[1];
};

/* Relocates a loaded project file in place and finds its project chunk. */
void RelocateFile(void **project, void *data) {
    s32 count;
    u8 *base;
    SChunk *chunk;
    s32 i;
    u32 size;
    if (project == 0) {
        return;
    }
    if (project != 0) {
        *project = 0;
    }
    if (((SFileHeader *)data)->m_id != g_chunkInfo) {
        return;
    }
    count = ((SFileHeader *)data)->m_chunkCount;
    base = (u8 *)data + ((SFileHeader *)data)->m_dataOffset;
    chunk = (SChunk *)base;
    {
        SRelocTable *relocs = (SRelocTable *)((u8 *)data + ((SFileHeader *)data)->m_relocOffset);
        u32 *offset = relocs->m_offsets;
        for (i = relocs->m_count; i > 0; i--) {
            u32 *p = (u32 *)(base + (*offset++ & ~3));
            *p += (u32)base;
        }
    }
    
    for (i = 0; i < count; i++) {
        if (chunk->m_id == 'nCPJ') {
            if (project != 0) {
                *project = base + chunk->m_offset;
            }
        } else if (chunk->m_id == g_chunkEnd) {
            return;
        }
        if (GetPlatform()->IsSwapped()) {
            u32 v = chunk->m_size;
            size = (v >> 24) | ((v >> 8) & 0xFF00) | ((v & 0xFF00) << 8) | (v << 24);
        } else {
            size = chunk->m_size;
        }
        chunk = (SChunk *)((u8 *)chunk + size);
        chunk = (SChunk *)((u8 *)chunk + 8);
    }
}


struct SProjectNode;
void PrepareTree(SProjectNode *node);

struct SProjectRoot {
    SProjectNode *m_root;
};

void PrepareProject(SProjectRoot *project);

/* Relocates a loaded project file and prepares its casts. */
void LoadProject(SProjectRoot **project, void *data) {
    RelocateFile((void **)project, data);
    PrepareProject(*project);
}

/* Sets the derived flags of a layer's casts. */
void PrepareLayer(SLayerInfo *layer) {
    SCastInfo *cast;
    s32 i;
    u32 flags;
    for (i = 0; i < layer->m_castCount; i++) {
        cast = layer->m_casts[i];
        flags = cast->m_mode;
        if ((flags & 0xD000) == 0) {
            if (flags & 1) {
                cast->m_mode = flags | 0x300;
            }
            if (cast->m_mode & 4) {
                cast->m_mode |= 0xC00;
            }
        }
    }
}


#pragma dont_inline on
void PrepareScene(SSceneInfo *scene) {
    for (s32 i = 0; i < scene->m_layerCount; i++) {
        PrepareLayer(&scene->m_layers[i]);
    }
}
#pragma dont_inline reset

/* A node of a project's scene tree (see Game/csdAccess.cpp). */
struct SProjectNode {
    s32 m_sceneCount;
    SSceneInfo **m_scenes;
    void *m_sceneIndex;
    s32 m_childCount;
    SProjectNode *m_children;
    void *m_childIndex;
};

#pragma dont_inline on
void PrepareTree(SProjectNode *node) {
    for (s32 i = 0; i < node->m_sceneCount; i++) {
        PrepareScene(node->m_scenes[i]);
    }
    for (s32 i = 0; i < node->m_childCount; i++) {
        PrepareTree(&node->m_children[i]);
    }
}
#pragma dont_inline reset

void PrepareProject(SProjectRoot *project) {
    PrepareTree(project->m_root);
}

} // namespace CSD
} // namespace Chao
