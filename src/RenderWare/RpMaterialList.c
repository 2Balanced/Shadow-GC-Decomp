/* RenderWare 3.7 world plugin: material lists, 0x8045B6F0-0x8045BF30 (wave 41 re-cut of the
 * size-cut Game/RwCpp21). The file has no data of its own. Its stream reader expands the list
 * helpers defined before it but calls RpMaterialDestroy and RpMaterialStreamRead of the material
 * file in front of it; the mesh file behind it starts with the mesh allocation helper.
 *
 * Names are inferred from behaviour and the public RenderWare API:
 *   fn_8045B6F0  _rpMaterialListDeinitialize   fn_8045B788  _rpMaterialListInitialize
 *   fn_8045B79C  _rpMaterialListGetMaterial    fn_8045B7AC  _rpMaterialListAppendMaterial
 *   fn_8045B8EC  _rpMaterialListFindMaterialIndex  fn_8045B924  _rpMaterialListStreamRead
 * Built with cflags_rw_r3 (configure.py): C, functions in definition order.
 * Status: fn_8045B8EC (the material array loaded into r5 and the scaled count into r0, retail the
 * other way round; 3 rows) and fn_8045B924, where retail gives the two parameters r31/r30 and the
 * inlined list helpers' variables the registers below them, ours the reverse (a whole-function
 * register permutation), are left; both are recorded in probes/Game_RwCpp21.py. */
#include <dolphin/types.h>
#include "Game/rw/r3.h"

s32 fn_8047CF50(RwStream *, u32, u32 *, u32 *); /* RwStreamFindChunk */
RwStream *fn_8047D868(RwStream *, void *, u32); /* RwStreamReadInt32 */

s32 fn_8045B1F4(RpMaterial *material);          /* RpMaterialDestroy */
RpMaterial *fn_8045B348(RwStream *stream);      /* RpMaterialStreamRead */

RpMaterialList *fn_8045B6F0(RpMaterialList *matList) {
    RpMaterial **materials = matList->materials;
    if (materials != NULL) {
        s32 count = matList->numMaterials;
        s32 i;
        for (i = 0; i < count; i++) {
            fn_8045B1F4(materials[i]);
            materials[i] = NULL;
        }
        RwFree(materials);
        matList->materials = NULL;
    }
    matList->numMaterials = 0;
    matList->space = 0;
    return matList;
}

RpMaterialList *fn_8045B788(RpMaterialList *matList) {
    matList->space = 0;
    matList->materials = NULL;
    matList->numMaterials = 0;
    return matList;
}

RpMaterial *fn_8045B79C(const RpMaterialList *matList, s32 index) {
    return matList->materials[index];
}

static inline RpMaterialList *RpMaterialListSetSize(RpMaterialList *matList, s32 space) {
    if (matList->space < space) {
        RpMaterial **materials;
        u32 size = space * sizeof(RpMaterial *);
        if (matList->materials != NULL) {
            materials = (RpMaterial **)RwRealloc(matList->materials, size, 0x1030008);
        } else {
            materials = (RpMaterial **)RwMalloc(size, 0x1030008);
        }
        if (materials == NULL) {
            RWERROR((E_RW_NOMEM, size));
            return NULL;
        }
        matList->materials = materials;
        matList->space = space;
    }
    return matList;
}

s32 fn_8045B7AC(RpMaterialList *matList, RpMaterial *material) {
    RpMaterial **materials;
    s32 space;
    u32 size;

    if (matList->space > matList->numMaterials) {
        matList->materials[matList->numMaterials] = material;
        material->refCount++;
        matList->numMaterials++;
        return matList->numMaterials - 1;
    }
    space = matList->space + 20;
    size = space * sizeof(RpMaterial *);
    if (matList->materials != NULL) {
        materials = (RpMaterial **)RwRealloc(matList->materials, space * sizeof(RpMaterial *),
                                             0x1030008);
    } else {
        materials = (RpMaterial **)RwMalloc(space * sizeof(RpMaterial *), 0x1030008);
    }
    if (materials == NULL) {
        RWERROR((E_RW_NOMEM, size));
        return -1;
    }
    matList->materials = materials;
    matList->space += 20;
    materials[matList->numMaterials] = material;
    material->refCount++;
    matList->numMaterials++;
    return matList->numMaterials - 1;
}

s32 fn_8045B8EC(const RpMaterialList *matList, const RpMaterial *material) {
    s32 i = matList->numMaterials;
    RpMaterial **entry = matList->materials + i;
    while (entry--, i-- > 0) {
        if (*entry == material) {
            return i;
        }
    }
    return i;
}

RpMaterialList *fn_8045B924(RwStream *stream, RpMaterialList *matList) {
    s32 count;
    u32 size;
    u32 version;
    s32 *indices;
    s32 i;
    s32 status;

    if (!fn_8047CF50(stream, 1, &size, &version)) {
        return NULL;
    }
    if (version >= 0x35000 && version <= 0x37002) {
        status = (fn_8047D868(stream, &count, sizeof(count)) != NULL);
        if (!status) {
            return NULL;
        }
        fn_8045B788(matList);
        if (count == 0) {
            return matList;
        }
        if (RpMaterialListSetSize(matList, count) == NULL) {
            fn_8045B6F0(matList);
            return NULL;
        }
        indices = (s32 *)RwMalloc(count * sizeof(s32), 0x10501);
        status = (fn_8047D868(stream, indices, count * sizeof(s32)) != NULL);
        if (!status) {
            RwFree(indices);
            fn_8045B6F0(matList);
            return NULL;
        }
        for (i = 0; i < count; i++) {
            RpMaterial *material;
            if (indices[i] < 0) {
                if (!fn_8047CF50(stream, 7, NULL, &version)) {
                    RwFree(indices);
                    fn_8045B6F0(matList);
                    return NULL;
                }
                if (version >= 0x35000 && version <= 0x37002) {
                    material = fn_8045B348(stream);
                    if (material == NULL) {
                        RwFree(indices);
                        fn_8045B6F0(matList);
                        return NULL;
                    }
                } else {
                    RWERROR((E_RW_BADVERSION));
                    RwFree(indices);
                    fn_8045B6F0(matList);
                    return NULL;
                }
            } else {
                material = fn_8045B79C(matList, indices[i]);
                material->refCount++;
            }
            fn_8045B7AC(matList, material);
            fn_8045B1F4(material);
        }
        RwFree(indices);
        return matList;
    }
    RWERROR((E_RW_BADVERSION));
    return NULL;
}
