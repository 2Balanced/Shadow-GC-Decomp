/* Material lighting of a clump (file and names inferred; no class): fn_80039B94 sets two values on
 * the lit materials of every atomic of a clump (the second only when it is not negative), the way
 * the fade passes of Game/UnkClumpFade_80038120.cpp do. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include "Game/DebugDispShape.h"
#include <dolphin/types.h>

struct RpMaterial;

struct RpGeometry;

struct RpAtomic {
    /* 0x00 */ u8 field_0x0[0x18];
    /* 0x18 */ RpGeometry *geometry;
};

/* The two values (meanings inferred). */
struct UnkMaterialLight_80039B94 {
    /* 0x0 */ float m_value;
    /* 0x4 */ float m_scale;
};

typedef RpAtomic *(*RpAtomicCallBack)(RpAtomic *atomic, void *data);
typedef RpMaterial *(*RpMaterialCallBack)(RpMaterial *material, void *data);

extern "C" {
/* RenderWare. */
RpClump *fn_80456C54(RpClump *clump, RpAtomicCallBack callback, void *data);          /* RpClumpForAllAtomics */
RpGeometry *fn_804590E8(RpGeometry *geometry, RpMaterialCallBack callback, void *data); /* RpGeometryForAllMaterials */
s32 fn_804549DC(RpMaterial *material);
void fn_804548EC(RpMaterial *material, float value);
void fn_80454964(RpMaterial *material, float value);
void fn_8045462C(RpMaterial *material);
s32 fn_804A22E4(RpMaterial *material);

RpMaterial *fn_80039C1C(RpMaterial *material, UnkMaterialLight_80039B94 *light);
RpAtomic *fn_80039BD0(RpAtomic *atomic, void *data);
void fn_80039B94(RpClump *clump, float value, float scale);
}

extern "C" RpMaterial *fn_80039C1C(RpMaterial *material, UnkMaterialLight_80039B94 *light) {
    if (fn_804549DC(material)) {
        if (light->m_scale >= 0.0f) {
            fn_804548EC(material, light->m_scale);
        }
        fn_80454964(material, light->m_value);
        s32 type = fn_804A22E4(material);
        if (type == 5 || type == 6) {
            fn_8045462C(material);
        }
    }
    return material;
}

extern "C" RpAtomic *fn_80039BD0(RpAtomic *atomic, void *data) {
    if (atomic->geometry != 0) {
        fn_804590E8(atomic->geometry, (RpMaterialCallBack)fn_80039C1C, data);
    }
    return atomic;
}

extern "C" void fn_80039B94(RpClump *clump, float value, float scale) {
    UnkMaterialLight_80039B94 light;
    light.m_value = value;
    light.m_scale = scale;
    if (clump != 0) {
        fn_80456C54(clump, fn_80039BD0, &light);
    }
}
