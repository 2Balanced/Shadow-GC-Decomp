/* csdMotionPalette.cpp (file name from the allocator's records): sets up the run-time state of a
 * scene (its layers and casts, and the pool of companion matrices the cast tree needs; see
 * Game/csdMatrix.cpp), builds the quads of sprite and text casts, frees the state, and holds the
 * accessors of a cast's packed render flags. All names are inferred.
 *
 * Code generation follows Game/csdLoader.cpp: float constants are addressed on their own
 * (#pragma pool_data off) and the statics through one base register (pool_data on around their
 * definitions and the functions that reach three of them); #pragma dont_inline marks the callers
 * the library's compiler did not inline into. */
#include <string.h>

#include <dolphin/types.h>

#include "Game/CsdLib.h"

#pragma pool_data off

namespace Chao {
namespace CSD {

void InitCast(void *cast);
s32 FindIndex(const SNameIndex *table, s32 count, const char *name);
void ScaleCorner(float *x, float *y);
void GetCharSize(float *w, float *h);

/* The pool the casts' companion matrices come from while a scene is set up. */
#pragma pool_data on
static f32 *s_matrixPool;
static s32 s_matrixPoolSize;
static s32 s_matrixPoolUsed;
#pragma pool_data off

void ResetMatrixPool() {
    s_matrixPool = 0;
}

#pragma pool_data on
void SetMatrixPool(f32 *pool, s32 size) {
    s_matrixPool = pool;
    s_matrixPoolSize = size;
    s_matrixPoolUsed = 0;
}
#pragma pool_data off

/* Marks the casts that need their own companion matrices: those not both scale-locked
 * (0x400 | 0x800) and keeping their parent's scale (2), and the parents of such casts. */
void MarkCastTree(SLayer *layer, s32 index, const SLayerInfo *info, SCast *parent) {
    SCast *cast = &layer->m_casts[index];
    u32 mode = info->m_casts[index]->m_mode;
    if (!(mode & 0x400) || !(mode & 0x800) || !(mode & 2)) {
        cast->m_matrix.m_scale = (f32 *)1;
    }
    if (info->m_tree[index].m_child >= 0) {
        MarkCastTree(layer, info->m_tree[index].m_child, info, cast);
    }
    if (parent != 0 && cast->m_matrix.m_scale != 0) {
        parent->m_matrix.m_scale = (f32 *)1;
    }
    if (info->m_tree[index].m_sibling >= 0) {
        MarkCastTree(layer, info->m_tree[index].m_sibling, info, parent);
    }
}

void MarkLayer(SLayer *layer, const SLayerInfo *info) {
    if (info->m_root >= 0) {
        MarkCastTree(layer, info->m_root, info, 0);
    }
}

/* The number of companion matrices the scene's casts need. */
inline void ClearMatrices(SSceneState *state) {
    for (s32 i = 0; i < state->m_layerCount; i++) {
        for (s32 j = 0; j < state->m_layers[i].m_castCount; j++) {
            state->m_layers[i].m_casts[j].m_matrix.m_base = 0;
            state->m_layers[i].m_casts[j].m_matrix.m_scale = 0;
        }
    }
}

s32 CountMatrices(SSceneState *state, const SSceneInfo *info) {
    s32 count;
    SLayerInfo *layers;
    ClearMatrices(state);
    layers = info->m_layers;
    count = info->m_layerCount;
    for (s32 i = 0; i < count; i++) {
        MarkLayer(&state->m_layers[i], &layers[i]);
    }
    s32 n = 0;
    for (s32 i = 0; i < state->m_layerCount; i++) {
        for (s32 j = 0; j < state->m_layers[i].m_castCount; j++) {
            if (state->m_layers[i].m_casts[j].m_matrix.m_scale != 0) {
                n += 2;
            }
        }
    }
    ClearMatrices(state);
    return n;
}

/* Clears the done flag of a cast and its parents, and the updated flags of the topmost one's
 * layer and scene. */
void ClearCastDone(SCast *cast) {
    cast->m_changed &= ~0x100000;
    if (cast->m_parent != 0) {
        ClearCastDone(cast->m_parent);
    } else {
        SLayer *layer = cast->m_layer;
        layer->m_updated = 0;
        layer->m_scene->m_updated = 0;
        layer->m_scene->m_refresh = 1;
    }
}

void DestroyCast(SCast *cast) {
    cast->m_matrix.m_base = 0;
    cast->m_matrix.m_scale = 0;
    if (cast->m_overrideValues != 0) {
        fn_80360528(cast->m_overrideValues);
        cast->m_overrideValues = 0;
    }
    if (cast->m_allocated != 0 && cast->m_vertices != 0) {
        fn_80360528(cast->m_vertices);
        cast->m_vertices = 0;
    }
}

void DestroyScene(SSceneState *state) {
    for (s32 i = 0; i < state->m_layerCount; i++) {
        for (s32 j = 0; j < state->m_layers[i].m_castCount; j++) {
            SCast *cast = &state->m_layers[i].m_casts[j];
            DestroyCast(cast);
        }
    }
    if (state->m_matrices != 0) {
        fn_80360528(state->m_matrices);
        state->m_matrices = 0;
    }
}


inline f32 *AllocMatrix() {
    f32 *p = s_matrixPool;
    s_matrixPool += 6;
    s_matrixPoolUsed++;
    return p;
}

void AllocMatrices(SCast *cast) {
    if (cast->m_matrix.m_base != 0 && cast->m_matrix.m_scale != 0) {
        return;
    }
    cast->m_matrix.m_scale = AllocMatrix();
    cast->m_matrix.m_base = AllocMatrix();
}

#pragma dont_inline on
void InitCastTree(SLayer *layer, s32 index, const SLayerInfo *info, SCast *parent, const SSceneInfo *scene,
                  void *texList) {
    SCast *cast = &layer->m_casts[index];
    SCastInfo *castInfo = info->m_casts[index];
    InitCastFromInfo(cast, castInfo, scene, texList);
    u32 mode = castInfo->m_mode;
    if (!(mode & 0x400) || !(mode & 0x800) || !(mode & 2)) {
        AllocMatrices(cast);
    }
    if (info->m_tree[index].m_child >= 0) {
        InitCastTree(layer, info->m_tree[index].m_child, info, cast, scene, texList);
    }
    if (parent != 0 && cast->m_matrix.m_scale != 0) {
        AllocMatrices(parent);
    }
    if (info->m_tree[index].m_sibling >= 0) {
        InitCastTree(layer, info->m_tree[index].m_sibling, info, parent, scene, texList);
    }
}

#pragma dont_inline reset

/* A character of a font: its code and its pattern. */
struct SFontChar {
    s32 m_code;
    s32 m_pattern;
};

/* A font: its characters, sorted by code. */
struct SFont {
    s32 m_count;
    SFontChar *m_chars;
};

/* The fonts of a project. */
struct SFontList {
    s32 m_count;
    SFont *m_fonts;
    SNameIndex *m_names;
};

int CompareInt(const s32 *a, const s32 *b);

/* Whether c is the first byte of a two-byte Shift-JIS character. */
inline int IsLeadByte(u8 c) {
    u8 x = c ^ 0x20;
    if (x >= 0xA1 && x <= 0xD5) {
        return 1;
    }
    return 0;
}

/* Builds the quads of a text cast from its string and font. */
void InitTextCast(SCast *cast, const SCastInfo *info, const SSceneInfo *scene, void *fontList) {
    SFontList *fonts = (SFontList *)fontList;
    if (fonts == 0) {
        return;
    }
    if (fonts->m_fonts == 0) {
        return;
    }
    if (cast->m_textures != 0) {
        if (cast->m_allocated != 0) {
            if (cast->m_vertices != 0) {
                fn_80360528(cast->m_vertices);
                cast->m_vertices = 0;
            }
        } else {
            cast->m_vertices = 0;
            cast->m_transformed = 0;
            cast->m_textures = 0;
        }
    }
    const char *text = info->m_text;
    const char *font = info->m_font;
    if (cast->m_text != 0) {
        text = cast->m_text;
    }
    if (cast->m_font != 0) {
        font = cast->m_font;
    }
    SFont *f = &fonts->m_fonts[FindIndex(fonts->m_names, fonts->m_count, font)];
    s32 len = strlen(text);
    if (len <= 0) {
        return;
    }
    float spacing = 0.0f;
    if (info->m_version >= 1) {
        if (cast->m_textWidth <= -1000000.0f) {
            spacing = info->m_textWidth;
        } else {
            spacing = cast->m_textWidth;
        }
    }
    const u8 *p = (const u8 *)text;
    float x = 0.0f;
    float height = info->m_corners[1].m_y - info->m_corners[0].m_y;
    float w = 16.0f;
    float h = 16.0f;
    GetCharSize(&w, &h);
    u8 *buffer = (u8 *)fn_80360564(len * 0xA0 + len * 4, "csdMotionPalette.cpp", 0x1BA);
    cast->m_vertices = buffer;
    cast->m_transformed = buffer + len * 0x50;
    cast->m_textures = (s32 *)(buffer + len * 0xA0);
    cast->m_allocated = 1;
    SVertexT *v = (SVertexT *)cast->m_vertices;
    s32 n = 0;
    for (s32 i = 0; i < len; i++) {
        u8 c = *p;
        u32 code;
        if (IsLeadByte(c)) {
            code = (c << 8) | *++p;
            i++;
        } else {
            code = c;
        }

        SFontChar key;
        key.m_code = code;
        key.m_pattern = -1;
        p++;
        SFontChar *found = (SFontChar *)fn_803A7B40(&key, f->m_chars, f->m_count, sizeof(SFontChar),
                                                    (int (*)(const void *, const void *))CompareInt);
        s32 pattern = found != 0 ? found->m_pattern : -1;
        SVertexT *q = &v[n * 4];
        if (pattern < 0) {
            cast->m_textures[n] = -1;
            float x0 = info->m_corners[0].m_x + x;
            float scale = h / height;
            q[0].m_pos.m_x = x0;
            q[0].m_pos.m_y = info->m_corners[0].m_y * scale;
            q[0].m_color = cast->m_cornerColors[0];
            q[0].m_uv.m_x = 0.0f;
            q[0].m_uv.m_y = 0.0f;
            q[1].m_pos.m_x = x0;
            q[1].m_pos.m_y = info->m_corners[1].m_y * scale;
            q[1].m_color = cast->m_cornerColors[1];
            q[1].m_uv.m_x = 0.0f;
            q[1].m_uv.m_y = 0.0f;
            q[2].m_pos.m_x = w + x0;
            q[2].m_pos.m_y = info->m_corners[2].m_y * scale;
            q[2].m_color = cast->m_cornerColors[2];
            q[2].m_uv.m_x = 0.0f;
            q[2].m_uv.m_y = 0.0f;
            q[3].m_pos.m_x = w + x0;
            q[3].m_pos.m_y = info->m_corners[3].m_y * scale;
            q[3].m_color = cast->m_cornerColors[3];
            q[3].m_uv.m_x = 0.0f;
            q[3].m_uv.m_y = 0.0f;
            x += w + spacing;
        } else {
            SPattern *pat = &scene->m_patterns[pattern];
            cast->m_textures[n] = pat->m_texture;
            SVec2 *size = &scene->m_textureSizes[pat->m_texture];
            float x0 = info->m_corners[0].m_x + x;
            float scale = size->m_y * (pat->m_uv[1].m_y - pat->m_uv[0].m_y) / height;
            float x1 = size->m_x * (pat->m_uv[1].m_x - pat->m_uv[0].m_x) + x0;
            q[0].m_pos.m_x = x0;
            q[0].m_pos.m_y = info->m_corners[0].m_y * scale;
            q[0].m_color = cast->m_cornerColors[0];
            q[0].m_uv.m_x = pat->m_uv[0].m_x;
            q[0].m_uv.m_y = pat->m_uv[0].m_y;
            q[1].m_pos.m_x = x0;
            q[1].m_pos.m_y = info->m_corners[1].m_y * scale;
            q[1].m_color = cast->m_cornerColors[1];
            q[1].m_uv.m_x = pat->m_uv[0].m_x;
            q[1].m_uv.m_y = pat->m_uv[1].m_y;
            q[2].m_pos.m_x = x1;
            q[2].m_pos.m_y = info->m_corners[2].m_y * scale;
            q[2].m_color = cast->m_cornerColors[2];
            q[2].m_uv.m_x = pat->m_uv[1].m_x;
            q[2].m_uv.m_y = pat->m_uv[0].m_y;
            q[3].m_pos.m_x = x1;
            q[3].m_pos.m_y = info->m_corners[3].m_y * scale;
            q[3].m_color = cast->m_cornerColors[3];
            q[3].m_uv.m_x = pat->m_uv[1].m_x;
            q[3].m_uv.m_y = pat->m_uv[1].m_y;
            x += spacing + (v[n * 4 + 2].m_pos.m_x - v[n * 4].m_pos.m_x);
        }
        n++;
    }
    cast->m_quadCount = n;
    float width = v[(n - 1) * 4 + 2].m_pos.m_x - v[0].m_pos.m_x;
    float offset = info->m_corners[0].m_x * (width / (info->m_corners[2].m_x - info->m_corners[0].m_x)) - v[0].m_pos.m_x;
    for (s32 q = 0; q < n; q++) {
        for (s32 k = 0; k < 4; k++) {
            SVertexT *vt = &v[q * 4 + k];
            vt->m_pos.m_x += offset;
            ScaleCorner(&vt->m_pos.m_x, &vt->m_pos.m_y);
            ((SVertexT *)cast->m_transformed)[q * 4 + k].m_uv.m_x = vt->m_uv.m_x;
            ((SVertexT *)cast->m_transformed)[q * 4 + k].m_uv.m_y = vt->m_uv.m_y;
            ((SVertexT *)cast->m_transformed)[q * 4 + k].m_color = -1;
        }
    }
    cast->field_0x6C = width;
    cast->m_updated = 0;
    cast->m_changed &= ~0x200000;
    cast->m_changed &= ~0x80;
    cast->m_changed &= ~0x80000;
    cast->m_changed &= ~0xF00;
    cast->m_attrColor = -1;
}

void InitSpriteCast(SCast *cast, const SCastInfo *info) {
    if (cast->m_textures != 0) {
        if (cast->m_allocated != 0) {
            fn_80360528(cast->m_vertices);
        } else {
            cast->m_vertices = 0;
            cast->m_transformed = 0;
            cast->m_textures = 0;
        }
    }
    cast->m_vertices = cast->m_vertexBuffer;
    cast->m_transformed = cast->m_transformedBuffer;
    cast->m_textures = &cast->m_textureBuffer;
    cast->m_allocated = 0;
    cast->m_quadCount = 1;
    SVertexT *v = (SVertexT *)cast->m_vertices;
    v[0].m_pos = info->m_corners[0];
    v[1].m_pos = info->m_corners[1];
    v[2].m_pos = info->m_corners[2];
    v[3].m_pos = info->m_corners[3];
    for (s32 i = 0; i < 4; i++) {
        ScaleCorner(&v[i].m_pos.m_x, &v[i].m_pos.m_y);
    }
    cast->m_pattern = -100000.0f;
    cast->m_changed = 0;
    cast->m_updated = 0;
}

void InitCastFromInfo(SCast *cast, const SCastInfo *info, const SSceneInfo *scene, void *texList) {
    switch (info->m_type) {
    case 0:
        InitSpriteCast(cast, info);
        break;
    case 1:
        InitSpriteCast(cast, info);
        break;
    case 2:
        InitTextCast(cast, info, scene, texList);
        break;
    }
}

void InitLayerTree(SLayer *layer, const SLayerInfo *info, const SSceneInfo *scene, void *texList) {
    if (info->m_root >= 0) {
        InitCastTree(layer, info->m_root, info, 0, scene, texList);
    }
}

#pragma dont_inline on
void InitLayers(SSceneState *state, const SSceneInfo *info, void *texList) {
    s32 count;
    SLayerInfo *layers;
    layers = info->m_layers;
    count = info->m_layerCount;
    for (s32 i = 0; i < count; i++) {
        InitLayerTree(&state->m_layers[i], &layers[i], info, texList);
    }
    float x;
    float y;
    GetScreenSize(&x, &y);
    state->m_screenSize[0] = x;
    state->m_screenSize[1] = y;
    GetScreenBase(&x, &y);
    state->m_screenBase[0] = x;
    state->m_screenBase[1] = y;
    float ox;
    float oy;
    GetScreenOffset(&ox, &oy);
    state->m_screenOffset[0] = ox;
    state->m_screenOffset[1] = oy;
}

#pragma dont_inline reset

#pragma pool_data on
void InitScene(SSceneState *state, const SSceneInfo *info, void *texList) {
    SLayerInfo *layers = info->m_layers;
    s32 castIndex = 0;
    state->m_layerCount = info->m_layerCount;
    state->m_layers = (SLayer *)(state + 1);
    for (s32 i = 0; i < state->m_layerCount; i++) {
        state->m_layers[i].m_castCount = layers[i].m_castCount;
        state->m_layers[i].m_casts =
            (SCast *)((u8 *)state + state->m_layerCount * sizeof(SLayer) + castIndex * sizeof(SCast) + sizeof(SSceneState));
        for (s32 j = 0; j < state->m_layers[i].m_castCount; j++) {
            InitCast(&state->m_layers[i].m_casts[j]);
            state->m_layers[i].m_casts[j].m_layer = &state->m_layers[i];
        }
        state->m_layers[i].m_updated = 0;
        castIndex += layers[i].m_castCount;
        state->m_layers[i].m_scene = state;
    }
    state->m_updated = 0;
    state->m_frame = -1.0f;
    state->m_speed = 1.0f;
    state->m_parent = 0;
    state->m_refresh = 0;
    state->m_motion = 0;
    state->m_matrices = 0;
    s32 n = CountMatrices(state, info);
    if (n > 0) {
        state->m_matrices = fn_80360564(n * 0x18, "csdMotionPalette.cpp", 0xEA);
    }
    s_matrixPool = (f32 *)state->m_matrices;
    s_matrixPoolSize = n;
    s_matrixPoolUsed = 0;
    InitLayers(state, info, texList);
    s_matrixPool = 0;
}
#pragma pool_data off

/* The size of the run-time state of a scene. */
s32 GetSceneStateSize(const SSceneInfo *info) {
    s32 count = info->m_layerCount;
    s32 casts = 0;
    for (s32 i = 0; i < count; i++) {
        casts += info->m_layers[i].m_castCount;
    }
    return sizeof(SSceneState) + count * sizeof(SLayer) + casts * sizeof(SCast);
}



int CompareInt(const s32 *a, const s32 *b) {
    if (*a < *b) {
        return -1;
    }
    return *a > *b;
}

s32 GetWrapT(const SCastInfo *info) {
    return (info->m_flags >> 18) & 3;
}

s32 GetWrapS(const SCastInfo *info) {
    return (info->m_flags >> 16) & 3;
}

s32 IsFlipY(const SCastInfo *info) {
    return (info->m_flags >> 11) & 1;
}

s32 IsFlipX(const SCastInfo *info) {
    return (info->m_flags >> 10) & 1;
}

s32 GetFilter(const SCastInfo *info) {
    return (info->m_flags >> 12) & 3;
}

s32 GetAlphaMode(const SCastInfo *info) {
    return (info->m_flags >> 8) & 3;
}

s32 GetBlendMode(const SCastInfo *info) {
    return info->m_flags & 0xFF;
}

} // namespace CSD
} // namespace Chao
