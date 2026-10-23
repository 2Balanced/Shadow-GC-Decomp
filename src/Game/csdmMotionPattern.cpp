/* csdmMotionPattern.cpp (file name from the allocator's records): CMotionPattern, the cast that
 * places a whole scene (Game/csdmScene.cpp gives a scene one): making it, placing its matrix for
 * the screen, and overriding its attributes one by one. An overridden attribute is kept in the
 * cast's override values (allocated on the first override) and wins over the cast's own value and
 * over the node's data. Class names other than RCPtr, and all member names, are inferred.
 *
 * Code generation follows Game/csdmScene.cpp: deferred inlining, so the functions are defined in
 * the reverse of their address order. */
#define CSD_OBJECT_LAYER
#define CSD_RCPTR_DEALLOC_CTOR
#include "Game/CsdScene.h"

#pragma pool_data off
#pragma auto_inline off

namespace Chao {
namespace CSD {

u64 PackFloat2(f32 x, f32 y);
void InitCast(void *cast);
void DestroyCast(SCast *cast);
void ClearCastDone(SCast *cast);
void AllocMatrices(SCast *cast);
void SetMatrixPool(f32 *pool, s32 size);
void ResetMatrixPool();
void SetPattern(SCast *cast, const SCastInfo *info, const SSceneInfo *data);
void ApplyCastColor(SCast *cast);
void SetCornerColors(SCast *cast);

/* The tag the pattern's matrices are freed with. */
static RCFreeTag s_freeTag;

/* Frees a pattern's cast (its data block's deallocation). */
static void DeleteCast(unsigned char *cast) {
    DestroyCast((SCast *)cast);
    if (cast != 0) {
        fn_80360528(cast);
    }
}

CMotionPattern::CMotionPattern(float aspect) : m_aspect(aspect) {
    void *cast = fn_80360564(0x144, "csdmMotionPattern.cpp", 0x36);
    InitCast(cast);
    RCPtr<unsigned char> data((unsigned char *)cast, RCFreeFunc<unsigned char>(DeleteCast));
    Set((SCast *)cast, data);
}

void CMotionPattern::CopyFrom(const CResourceBase<SCast> &other) {
    CResourceBase<SCast>::CopyFrom(other);
    const CMotionPattern &pattern = (const CMotionPattern &)other;
    m_node = pattern.m_node;
    m_aspect = pattern.m_aspect;
}

u64 CMotionPattern::GetPosition() const {
    SCast *cast = m_resource;
    union {
        u32 m_words[2];
        u64 m_value;
    } position;
    if (cast->m_overrides & 0x2) {
        union {
            f32 m_floats[2];
            u64 m_value;
        } value;
        value.m_floats[0] = cast->m_overrideValues[1].m_float;
        value.m_floats[1] = cast->m_overrideValues[2].m_float;
        return value.m_value;
    }
    if (cast->m_updated != 0) {
        position.m_words[0] = *(u32 *)&cast->m_x;
        position.m_words[1] = *(u32 *)&cast->m_y;
    } else {
        CValue x;
        CValue y;
        GetNodeAttribute(&y, m_node, 2);
        GetNodeAttribute(&x, m_node, 1);
        position.m_value = PackFloat2(x.m_float, y.m_float);
    }
    return position.m_value;
}

void CMotionPattern::Update() {
    SCast *cast = m_resource;
    cast->m_changed &= ~0x78000;
    float width, height, baseX, baseY, offsetX, offsetY;
    GetScreenSize(&width, &height);
    GetScreenBase(&baseX, &baseY);
    GetScreenOffset(&offsetX, &offsetY);
    f32 *screen = (f32 *)cast->m_vertices;
    if (screen == 0) {
        m_matrices.Attach((SCast *)fn_80360564(0x30, "csdmMotionPattern.cpp", 0xf0), s_freeTag);
        SetMatrixPool((f32 *)&*m_matrices, 2);
        AllocMatrices(cast);
        ResetMatrixPool();
        cast->m_vertices = fn_80360564(0x18, "csdmMotionPattern.cpp", 0xf7);
        cast->m_allocated = 1;
    } else if (screen[0] != width || screen[1] != height || screen[2] != baseX || screen[3] != baseY ||
               screen[4] != offsetX || screen[5] != offsetY) {
        cast->m_changed &= ~0x80000;
        cast->m_changed &= ~0x2;
        cast->m_changed &= ~0x4;
    }
    if (!(cast->m_changed & 0x80000)) {
        if (!(cast->m_changed & 0x1)) {
            cast->m_hide = cast->m_attrHide;
            cast->m_changed |= 0x1;
            cast->m_changed |= 0x20000;
            cast->m_changed |= 0x8000;
        }
        if (!(cast->m_changed & 0x80)) {
            cast->m_color = cast->m_attrColor;
            cast->m_changed |= 0x80;
            cast->m_changed |= 0x40000;
            cast->m_changed |= 0x8000;
        }
        if ((cast->m_changed & 0x3e) != 0x3e) {
            PushMatrix(&cast->m_matrix);
            float x = cast->m_x;
            float y = cast->m_y;
            ScaleToScreen(&x, &y);
            float aspect = GetScreenAspect();
            y *= aspect / m_aspect;
            TranslateMatrix3(x, y);
            LoadMatrix();
            RotateMatrix(cast->m_rotation);
            ScaleMatrix(cast->m_scaleX, cast->m_scaleY);
            PopMatrix();
            cast->m_changed |= 0x3e;
            cast->m_changed |= 0x10000;
            cast->m_changed |= 0x8000;
        }
        cast->m_changed |= 0x80000;
        ((f32 *)cast->m_vertices)[0] = width;
        ((f32 *)cast->m_vertices)[1] = height;
        ((f32 *)cast->m_vertices)[2] = baseX;
        ((f32 *)cast->m_vertices)[3] = baseY;
        ((f32 *)cast->m_vertices)[4] = offsetX;
        ((f32 *)cast->m_vertices)[5] = offsetY;
    }
}

void CMotionPattern::OverridePosition(float x, float y) {
    SCast *cast = m_resource;
    if (!(cast->m_overrides & 0x2) || cast->m_overrideValues[1].m_float != x || !(cast->m_overrides & 0x4) ||
        cast->m_overrideValues[2].m_float != y) {
        cast->m_overrides |= 0x6;
        if (cast->m_overrideValues == 0) {
            cast->m_overrideValues = (CValue *)fn_80360564(0x3c, "csdmMotionPattern.cpp", 0x153);
        }
        cast->m_overrideValues[1].m_float = x;
        cast->m_overrideValues[2].m_float = y;
        cast->m_changed &= ~0x6;
        cast->m_changed &= ~0x200000;
        cast->m_changed &= ~0x80000;
        ClearCastDone(cast);
    }
}

void CMotionPattern::OverrideHide(s32 hide) {
    SCast *cast = m_resource;
    if (!(cast->m_overrides & 0x1) || cast->m_overrideValues[0].m_int != hide) {
        cast->m_overrides |= 0x1;
        if (cast->m_overrideValues == 0) {
            cast->m_overrideValues = (CValue *)fn_80360564(0x3c, "csdmMotionPattern.cpp", 0x18f);
        }
        cast->m_overrideValues[0].m_int = hide;
        cast->m_changed &= ~0x1;
        cast->m_changed &= ~0x80000;
        ClearCastDone(cast);
    }
}

void CMotionPattern::ClearHideOverride() {
    SCast *cast = m_resource;
    if (cast->m_overrides & 0x1) {
        cast->m_overrides &= ~0x1;
        cast->m_changed &= ~0x1;
        cast->m_changed &= ~0x80000;
        ClearCastDone(cast);
    }
}

s32 CMotionPattern::GetHide() const {
    SCast *cast = m_resource;
    if (cast->m_overrides & 0x1) {
        return cast->m_overrideValues[0].m_int;
    }
    if (cast->m_updated != 0) {
        return cast->m_attrHide;
    }
    CValue value;
    GetNodeAttribute(&value, m_node, 0);
    return value.m_int;
}

float CMotionPattern::GetRotation() const {
    SCast *cast = m_resource;
    if (cast->m_overrides & 0x8) {
        return cast->m_overrideValues[3].m_float;
    }
    if (cast->m_updated != 0) {
        return cast->m_rotation;
    }
    CValue value;
    GetNodeAttribute(&value, m_node, 3);
    return value.m_float;
}

void CMotionPattern::OverrideRotation(float rotation) {
    SCast *cast = m_resource;
    if (!(cast->m_overrides & 0x8) || cast->m_overrideValues[3].m_float != rotation) {
        cast->m_overrides |= 0x8;
        if (cast->m_overrideValues == 0) {
            cast->m_overrideValues = (CValue *)fn_80360564(0x3c, "csdmMotionPattern.cpp", 0x1ef);
        }
        cast->m_overrideValues[3].m_float = rotation;
        cast->m_changed &= ~0x8;
        cast->m_changed &= ~0x200000;
        cast->m_changed &= ~0x80000;
        ClearCastDone(cast);
    }
}

u64 CMotionPattern::GetScale() const {
    SCast *cast = m_resource;
    union {
        u32 m_words[2];
        u64 m_value;
    } scale;
    if (cast->m_overrides & 0x10) {
        union {
            f32 m_floats[2];
            u64 m_value;
        } value;
        value.m_floats[0] = cast->m_overrideValues[4].m_float;
        value.m_floats[1] = cast->m_overrideValues[5].m_float;
        return value.m_value;
    }
    if (cast->m_updated != 0) {
        scale.m_words[0] = *(u32 *)&cast->m_scaleX;
        scale.m_words[1] = *(u32 *)&cast->m_scaleY;
    } else {
        CValue x;
        CValue y;
        GetNodeAttribute(&y, m_node, 5);
        GetNodeAttribute(&x, m_node, 4);
        scale.m_value = PackFloat2(x.m_float, y.m_float);
    }
    return scale.m_value;
}

u32 CMotionPattern::GetColor() const {
    SCast *cast = m_resource;
    if (cast->m_overrides & 0x80) {
        return cast->m_overrideValues[7].m_int;
    }
    if (cast->m_updated != 0) {
        return cast->m_attrColor;
    }
    CValue value;
    GetNodeAttribute(&value, m_node, 7);
    return value.m_int;
}

void CMotionPattern::OverrideColor(const u32 &color) {
    SCast *cast = m_resource;
    if (!(cast->m_overrides & 0x80) || (u32)cast->m_overrideValues[7].m_int != color) {
        cast->m_overrides |= 0x80;
        if (cast->m_overrideValues == 0) {
            cast->m_overrideValues = (CValue *)fn_80360564(0x3c, "csdmMotionPattern.cpp", 0x293);
        }
        cast->m_overrideValues[7].m_int = color;
        cast->m_changed &= ~0x80;
        cast->m_changed &= ~0x80000;
        ClearCastDone(cast);
    }
}

void CMotionPattern::ClearColorOverride() {
    SCast *cast = m_resource;
    if (cast->m_overrides & 0x80) {
        cast->m_overrides &= ~0x80;
        cast->m_changed &= ~0x80;
        cast->m_changed &= ~0x80000;
        ClearCastDone(cast);
    }
}

float CMotionPattern::GetPattern() const {
    SCast *cast = m_resource;
    if (cast->m_overrides & 0x40) {
        return cast->m_overrideValues[6].m_float;
    }
    if (cast->m_updated != 0) {
        return cast->m_pattern;
    }
    CValue value;
    GetNodeAttribute(&value, m_node, 6);
    return value.m_float;
}

void CMotionPattern::OverridePattern(float pattern) {
    SCast *cast = m_resource;
    if (!(cast->m_overrides & 0x40) || cast->m_overrideValues[6].m_float != pattern) {
        cast->m_overrides |= 0x40;
        if (cast->m_overrideValues == 0) {
            cast->m_overrideValues = (CValue *)fn_80360564(0x3c, "csdmMotionPattern.cpp", 0x2df);
        }
        cast->m_overrideValues[6].m_float = pattern;
        cast->m_pattern = pattern;
        if (m_node->m_scene != 0) {
            SetPattern(m_resource, m_node->m_resource, m_node->m_scene->m_resource);
        }
        cast->m_changed &= ~0x40;
        cast->m_changed &= ~0x80000;
        ClearCastDone(cast);
    }
}

u32 CMotionPattern::GetCornerColor(s32 corner) const {
    SCast *cast = m_resource;
    if ((0x100 << corner) & cast->m_overrides) {
        return cast->m_overrideValues[8 + corner].m_int;
    }
    if (cast->m_updated != 0) {
        return cast->m_cornerColors[corner];
    }
    CValue value;
    GetNodeAttribute(&value, m_node, corner + 8);
    return value.m_int;
}

void CMotionPattern::OverrideCornerColor(s32 corner, const u32 &color) {
    SCast *cast = m_resource;
    u32 bit = 0x100 << corner;
    if (!(cast->m_overrides & bit) || (u32)cast->m_overrideValues[8 + corner].m_int != color) {
        cast->m_overrides |= bit;
        if (cast->m_overrideValues == 0) {
            cast->m_overrideValues = (CValue *)fn_80360564(0x3c, "csdmMotionPattern.cpp", 0x330);
        }
        cast->m_overrideValues[8 + corner].m_int = color;
        cast->m_cornerColors[corner] = color;
        SetCornerColors(m_resource);
        ApplyCastColor(m_resource);
        cast->m_changed |= bit;
    }
}

void CMotionPattern::ClearCornerColorOverride(s32 corner) {
    SCast *cast = m_resource;
    u32 bit = 0x100 << corner;
    if (cast->m_overrides & bit) {
        cast->m_overrides &= ~bit;
        cast->m_changed &= ~bit;
        cast->m_changed &= ~0x80000;
        ClearCastDone(cast);
    }
}

s32 CMotionPattern::GetParam(s32 index) const {
    SCast *cast = m_resource;
    if ((0x1000 << index) & cast->m_overrides) {
        return cast->m_overrideValues[12 + index].m_int;
    }
    if (cast->m_updated != 0) {
        return cast->m_params[index];
    }
    CValue value;
    GetNodeAttribute(&value, m_node, index + 12);
    return value.m_int;
}

void CMotionPattern::OverrideParam(s32 index, s32 value) {
    SCast *cast = m_resource;
    if (!(cast->m_overrides & (0x1000 << index)) || cast->m_overrideValues[12 + index].m_int != value) {
        cast->m_overrides |= 0x1000 << index;
        if (cast->m_overrideValues == 0) {
            cast->m_overrideValues = (CValue *)fn_80360564(0x3c, "csdmMotionPattern.cpp", 0x37e);
        }
        cast->m_overrideValues[12 + index].m_int = value;
        cast->m_params[index] = value;
        cast->m_changed |= 0x1000 << index;
    }
}

void CMotionPattern::OverrideX(float x) {
    SCast *cast = m_resource;
    if (!(cast->m_overrides & 0x2) || cast->m_overrideValues[1].m_float != x) {
        cast->m_overrides |= 0x2;
        if (cast->m_overrideValues == 0) {
            cast->m_overrideValues = (CValue *)fn_80360564(0x3c, "csdmMotionPattern.cpp", 0x3a6);
        }
        cast->m_overrideValues[1].m_float = x;
        cast->m_changed &= ~0x2;
        cast->m_changed &= ~0x200000;
        cast->m_changed &= ~0x80000;
        ClearCastDone(cast);
    }
}

void CMotionPattern::OverrideY(float y) {
    SCast *cast = m_resource;
    if (!(cast->m_overrides & 0x4) || cast->m_overrideValues[2].m_float != y) {
        cast->m_overrides |= 0x4;
        if (cast->m_overrideValues == 0) {
            cast->m_overrideValues = (CValue *)fn_80360564(0x3c, "csdmMotionPattern.cpp", 0x3c0);
        }
        cast->m_overrideValues[2].m_float = y;
        cast->m_changed &= ~0x4;
        cast->m_changed &= ~0x200000;
        cast->m_changed &= ~0x80000;
        ClearCastDone(cast);
    }
}

void CMotionPattern::OverrideScaleX(float x) {
    SCast *cast = m_resource;
    if (!(cast->m_overrides & 0x10) || cast->m_overrideValues[4].m_float != x) {
        cast->m_overrides |= 0x10;
        if (cast->m_overrideValues == 0) {
            cast->m_overrideValues = (CValue *)fn_80360564(0x3c, "csdmMotionPattern.cpp", 0x3ff);
        }
        cast->m_overrideValues[4].m_float = x;
        cast->m_changed &= ~0x10;
        cast->m_changed &= ~0x200000;
        cast->m_changed &= ~0x80000;
        ClearCastDone(cast);
    }
}

void CMotionPattern::OverrideScaleY(float y) {
    SCast *cast = m_resource;
    if (!(cast->m_overrides & 0x20) || cast->m_overrideValues[5].m_float != y) {
        cast->m_overrides |= 0x20;
        if (cast->m_overrideValues == 0) {
            cast->m_overrideValues = (CValue *)fn_80360564(0x3c, "csdmMotionPattern.cpp", 0x42b);
        }
        cast->m_overrideValues[5].m_float = y;
        cast->m_changed &= ~0x20;
        cast->m_changed &= ~0x200000;
        cast->m_changed &= ~0x80000;
        ClearCastDone(cast);
    }
}

} // namespace CSD
} // namespace Chao
