/* RenderWare, 0x8049F3F8..0x804A0DEC.  Three source files meet in this cut:
 *   0x8049F3F8..0x8049F8AC  a key sort (radix exchange down to runs of five, then insertion sort)
 *   0x8049F8AC..0x804A076C  the GameCube immediate mode (RwIm3D) pipelines and their two nodes
 *   0x804A076C..0x804A0DEC  the start of the material effects plugin (rpmatfx): engine open and
 *                           close, the material constructor/destructor/copy and the texture
 *                           stream helpers (the file continues in RwCpp65 and RwCpp66)
 * Functions keep their fn_ symbols; comments give the public API name where the behaviour fits.
 * Open: fn_8049F5B4 (the sort's swap loop gets other scratch registers), fn_804A09D0 (retail keeps
 * the effect offset in r0 and the switch value in r3; six loop spellings tried), fn_8049FB18 (the
 * Im3D render node body, 3144 bytes) is not written. */
#include <dolphin/types.h>
#include "Game/rw/r8.h"

#ifdef __cplusplus
extern "C" {
#endif
extern RxNodeDefinition lbl_8056EC6C; /* node definition of the Im3D transform node */
extern RxNodeDefinition lbl_8056ECC0; /* node definition of the Im3D render node */
extern u32 lbl_805F16A0;
extern s32 lbl_805EDF60; /* entries per block of the material data free list */
extern s32 lbl_805EDF64; /* blocks to preallocate */
extern u8 lbl_805E32A0[0x28]; /* in-place storage of that free list */

RxNodeDefinition *fn_8049FB0C(void);
RxNodeDefinition *fn_804A0760(void);
RpMaterial *fn_804A20D0(RpMaterial *material, s32 flags);
RpMaterial *fn_804A25E0(RpMaterial *material, RwFrame *frame);
RpMaterial *fn_804A2628(RpMaterial *material, f32 coef);
RwFrame *fn_804A26BC(const RpMaterial *material);
f32 fn_804A2704(const RpMaterial *material);
RpMaterial *fn_804A2750(RpMaterial *material, RwTexture *texture);
RpMaterial *fn_804A27F8(RpMaterial *material, RwFrame *frame);
RpMaterial *fn_804A2840(RpMaterial *material, s32 useFrameBufferAlpha);
RpMaterial *fn_804A2888(RpMaterial *material, f32 coef);
RwTexture *fn_804A28D0(const RpMaterial *material);
RwFrame *fn_804A2918(const RpMaterial *material);
s32 fn_804A2960(const RpMaterial *material);
f32 fn_804A29A8(const RpMaterial *material);
RpMaterial *fn_804A29F0(RpMaterial *material, RwTexture *texture);
RpMaterial *fn_804A2AB0(RpMaterial *material, s32 srcBlendMode, s32 dstBlendMode);
RwTexture *fn_804A2B44(const RpMaterial *material);
RpMaterial *fn_804A2B8C(RpMaterial *material, s32 *srcBlendMode, s32 *dstBlendMode);
RpMaterial *fn_804A2BE0(RpMaterial *material, RwMatrix *baseTransform, RwMatrix *dualTransform);
RpMaterial *fn_804A2C2C(RpMaterial *material, RwMatrix **baseTransform, RwMatrix **dualTransform);
s32 fn_804A5494(void);
void fn_804A5524(void);

#define SORT_KEY(element, keyOffset) (*(u32 *)((u8 *)(element) + (keyOffset)))

/* Exchanges two elements a word at a time. */
#define SORT_SWAP(first, second, size) \
    do { \
        u32 *a = (u32 *)(first); \
        u32 *b = (u32 *)(second); \
        u32 left; \
        for (left = (size); left >= 4; left -= 4) { \
            u32 word = *b; \
            *b = *a; \
            *a = word; \
            a++; \
            b++; \
        } \
    } while (0)

typedef struct SortRange {
    u8 *low;
    u8 *high;
    u32 bit;
} SortRange;

/* Radix exchange sort on the key's bits from `bit` down; ranges shorter than five elements are
 * left for the insertion sort of the caller. */
void fn_8049F3F8(u8 *low, u8 *high, u32 size, u32 keyOffset, u32 bit) {
    SortRange stack[32];
    SortRange *top = stack;
    u8 *first;
    u8 *last;

    top->low = low;
    top->high = high;
    top->bit = bit;
    top++;
    do {
        top--;
        low = top->low;
        high = top->high;
        bit = top->bit;
        for (;;) {
            first = low;
            last = high;
            for (;;) {
                while ((SORT_KEY(low, keyOffset) & bit) == 0) {
                    low += size;
                    if (low > high) goto partitioned;
                }
                while ((SORT_KEY(high, keyOffset) & bit) != 0) {
                    high -= size;
                    if (low > high) goto partitioned;
                }
                SORT_SWAP(low, high, size);
                low += size;
                high -= size;
                if (low > high) break;
            }
        partitioned:
            bit >>= 1;
            if (bit == 0) break;
            if (last >= high + size + size * 5) {
                top->low = high + size;
                top->high = last;
                top->bit = bit;
                top++;
            }
            high = low - size;
            low = first;
            if (high < first + size * 5) break;
        }
    } while (top != stack);
}

/* Sorts `count` elements of `size` bytes by the unsigned word at `keyOffset`; the keys lie in
 * [minKey, maxKey]. */
void fn_8049F5B4(u8 *base, u32 count, u32 size, u32 keyOffset, u32 minKey, u32 maxKey) {
    u32 index;
    u32 bestKey;
    u32 best;
    u32 remaining;
    u8 *element;
    u32 key;
    u8 *slot;

    if (base == NULL) return;
    if (keyOffset + 4 > size) return;
    if (minKey >= maxKey) return;
    if (count > 5) {
        s32 top;
        if (maxKey != 0) {
            top = 0;
            while ((maxKey >>= 1) != 0) top++;
        } else {
            top = -1;
        }
        fn_8049F3F8(base, base + (count - 1) * size, size, keyOffset, 1 << top);
    }
    if (count <= 1) return;

    /* the smallest of the first five goes to the front as the sentinel */
    index = 4;
    if (count - 1 < 4) index = count - 1;
    best = index;
    bestKey = SORT_KEY(base + index * size, keyOffset);
    index--;
    do {
        key = SORT_KEY(base + index * size, keyOffset);
        if (key < bestKey) {
            bestKey = key;
            best = index;
        }
    } while (index-- != 0);
    if (best != 0) {
        SORT_SWAP(base, base + best * size, size);
    }

    remaining = count;
    element = base;
    while (--remaining != 0) {
        element += size;
        key = SORT_KEY(element, keyOffset);
        slot = element;
        for (slot -= size; SORT_KEY(slot, keyOffset) > key; slot -= size) {
            SORT_SWAP(slot, slot + size, size);
        }
    }
}

/* Creates the Im3D transform pipeline (one node) and makes it current. */
s32 fn_8049F8AC(RxPipeline **pipelines) {
    RxPipeline *pipeline = fn_8049B76C();
    if (pipeline != NULL) {
        RxPipeline *locked;
        pipeline->pluginId = 1;
        locked = fn_8049CCB8(pipeline);
        if (locked != NULL) {
            pipeline = fn_8049C1E0(fn_8049D0D0(locked, NULL, fn_8049FB0C(), NULL));
            if (pipeline != NULL) {
                pipelines[0] = pipeline;
                fn_8049AC90(pipeline);
                return 1;
            }
        }
        fn_8049B800(pipeline);
    }
    return 0;
}

void fn_8049F948(RxPipeline **pipelines) {
    fn_8049AC90(NULL);
    if (pipelines[0] != NULL) {
        fn_8049B800(pipelines[0]);
        pipelines[0] = NULL;
    }
}

/* Drops the Im3D render pipelines: one pipeline shared by the five primitive types. */
void fn_8049F990(RxPipeline **pipelines) {
    fn_8049ACE0(NULL, 3);
    fn_8049ACE0(NULL, 5);
    fn_8049ACE0(NULL, 4);
    fn_8049ACE0(NULL, 1);
    fn_8049ACE0(NULL, 2);
    if (pipelines[0] != NULL) {
        fn_8049B800(pipelines[0]);
    }
    pipelines[0] = NULL;
    pipelines[1] = NULL;
    pipelines[2] = NULL;
    pipelines[3] = NULL;
    pipelines[4] = NULL;
}

/* Creates the Im3D render pipeline and installs it for every primitive type. */
s32 fn_8049FA1C(RxPipeline **pipelines) {
    RxPipeline *pipeline = fn_8049B76C();
    if (pipeline != NULL) {
        RxPipeline *locked;
        pipeline->pluginId = 1;
        locked = fn_8049CCB8(pipeline);
        if (locked != NULL) {
            pipeline = fn_8049C1E0(fn_8049D0D0(locked, NULL, fn_804A0760(), NULL));
            if (pipeline != NULL) {
                pipelines[0] = pipeline;
                pipelines[1] = pipeline;
                pipelines[2] = pipeline;
                pipelines[3] = pipeline;
                pipelines[4] = pipeline;
                fn_8049ACE0(pipeline, 3);
                fn_8049ACE0(pipeline, 5);
                fn_8049ACE0(pipeline, 4);
                fn_8049ACE0(pipeline, 1);
                fn_8049ACE0(pipeline, 2);
                return 1;
            }
        }
        fn_8049B800(pipeline);
    }
    return 0;
}

/* Transform node: remembers the word its parameter block starts with. */
s32 fn_8049FAFC(void *self, const u32 *params) {
    lbl_805F16A0 = params[0];
    return 1;
}

RxNodeDefinition *fn_8049FB0C(void) {
    return &lbl_8056EC6C;
}

/* fn_8049FB18 (0xC48 bytes), the body of the Im3D render node, is not decompiled yet. */

RxNodeDefinition *fn_804A0760(void) {
    return &lbl_8056ECC0;
}

/* ---- material effects plugin ---- */

/* Engine close callback. */
void *fn_804A076C(void *instance, s32 offset, s32 size) {
    if (--lbl_8056ED00.numInstances == 0) {
        fn_804A5524();
    }
    if (lbl_8056ED00.materialDataFreeList != NULL) {
        fn_8047FCDC(lbl_8056ED00.materialDataFreeList);
        lbl_8056ED00.materialDataFreeList = NULL;
    }
    return instance;
}

/* Engine open callback. */
void *fn_804A07DC(void *instance, s32 offset, s32 size) {
    if (lbl_8056ED00.numInstances == 0) {
        lbl_8056ED00.materialDataFreeList = fn_8047FC04(
            sizeof(MatFXMaterialData), lbl_805EDF60, 4, lbl_805EDF64, lbl_805E32A0, 0x40120);
        if (lbl_8056ED00.materialDataFreeList == NULL) return NULL;
        if (!fn_804A5494()) return NULL;
    }
    lbl_8056ED00.numInstances++;
    return instance;
}

/* Material constructor. */
void *fn_804A0884(void *object, s32 offset, s32 size) {
    MATFXMATERIALGETDATA(object) = NULL;
    return object;
}

/* Material destructor. */
void *fn_804A0894(void *object, s32 offset, s32 size) {
    u8 i;
    MatFXEffectData *effect;
    MatFXMaterialData *data;

    data = MATFXMATERIALGETDATA(object);
    if (data != NULL) {
        for (i = 0; i < 2; i++) {
            effect = &data->data[i];
            switch (effect->flag) {
            case rpMATFXEFFECTBUMPMAP:
                if (effect->u.bump.bumpTexture != NULL) fn_8048CD8C(effect->u.bump.bumpTexture);
                if (effect->u.bump.texture != NULL) fn_8048CD8C(effect->u.bump.texture);
                break;
            case rpMATFXEFFECTENVMAP:
                if (effect->u.env.texture != NULL) fn_8048CD8C(effect->u.env.texture);
                break;
            case rpMATFXEFFECTDUAL:
                if (effect->u.dual.texture != NULL) fn_8048CD8C(effect->u.dual.texture);
                break;
            case rpMATFXEFFECTUVTRANSFORM:
                effect->u.uv.baseTransform = NULL;
                effect->u.uv.dualTransform = NULL;
                break;
            case rpMATFXEFFECTBUMPENVMAP:
                break;
            }
        }
        memset(data, 0, sizeof(MatFXMaterialData));
        RwEngineInstance->freeListFree(lbl_8056ED00.materialDataFreeList, data);
        MATFXMATERIALGETDATA(object) = NULL;
    }
    return object;
}

/* Material copy. */
void *fn_804A09D0(void *dstObject, const void *srcObject, s32 offset, s32 size) {
    RpMaterial *src = (RpMaterial *)srcObject;
    const MatFXMaterialData *srcData = MATFXMATERIALGETDATA(src);
    RpMaterial *dst = (RpMaterial *)dstObject;
    MatFXMaterialData *dstData;
    u8 i;

    if (srcData == NULL) return NULL;
    dstData = MatFXMaterialDataCreate(dst);
    if (dstData == NULL) return NULL;
    fn_804A20D0(dst, srcData->flags);
    for (i = 0; i < 2; i++) {
        const MatFXEffectData *srcEffect = &srcData->data[i];
        switch (srcEffect->flag) {
        case rpMATFXEFFECTBUMPMAP: {
            MatFXEffectData *dstEffect = &dstData->data[i];
            RwFrame *frame = fn_804A26BC(src);
            f32 coef = fn_804A2704(src);
            fn_804A25E0(dst, frame);
            fn_804A2628(dst, coef);
            dstEffect->u.bump.bumpTexture = srcEffect->u.bump.bumpTexture;
            dstEffect->u.bump.texture = srcEffect->u.bump.texture;
            if (dstEffect->u.bump.bumpTexture != NULL) {
                dstEffect->u.bump.bumpTexture->refCount++;
            }
            if (dstEffect->u.bump.texture != NULL) {
                dstEffect->u.bump.texture->refCount++;
            }
            break;
        }
        case rpMATFXEFFECTENVMAP: {
            RwTexture *texture = fn_804A28D0(src);
            RwFrame *frame = fn_804A2918(src);
            f32 coef = fn_804A29A8(src);
            s32 useFrameBufferAlpha = fn_804A2960(src);
            if (texture != NULL) {
                fn_804A2750(dst, texture);
            }
            fn_804A27F8(dst, frame);
            fn_804A2840(dst, useFrameBufferAlpha);
            fn_804A2888(dst, coef);
            break;
        }
        case rpMATFXEFFECTDUAL: {
            RwTexture *texture = fn_804A2B44(src);
            s32 srcBlendMode;
            s32 dstBlendMode;
            fn_804A2B8C(src, &srcBlendMode, &dstBlendMode);
            if (texture != NULL) {
                fn_804A29F0(dst, texture);
            }
            fn_804A2AB0(dst, srcBlendMode, dstBlendMode);
            break;
        }
        case rpMATFXEFFECTUVTRANSFORM: {
            RwMatrix *baseTransform;
            RwMatrix *dualTransform;
            src = fn_804A2C2C(src, &baseTransform, &dualTransform);
            dst = fn_804A2BE0(dst, baseTransform, dualTransform);
            break;
        }
        case rpMATFXEFFECTBUMPENVMAP:
            break;
        }
    }
    return dstObject;
}

/* Writes "texture present" and the texture chunk. */
RwStream *fn_804A0C50(RwStream *stream, const RwTexture *texture) {
    s32 present = (texture != NULL);
    if (!fn_8047D458(stream, &present, sizeof(present))) return NULL;
    if (present) {
        if (!fn_80483BA0(texture, stream)) return NULL;
    }
    return stream;
}

/* Reads what fn_804A0C50 wrote; a texture that cannot be found is not an error. */
RwStream *fn_804A0CDC(RwStream *stream, RwTexture **texture) {
    s32 present;
    if (!fn_8047D868(stream, &present, sizeof(present))) return NULL;
    if (present) {
        RwError error;
        if (!fn_8047CF50(stream, 6, NULL, NULL)) return NULL;
        fn_8047DBE4(&error);
        *texture = fn_80483D28(stream);
        if (*texture == NULL) {
            fn_8047DBE4(&error);
            if (error.errorCode != (s32)0x80000000 && error.errorCode != 0x16) {
                fn_8047DB84(&error);
                return NULL;
            }
        }
    } else {
        *texture = NULL;
    }
    return stream;
}

/* The stream size of what fn_804A0C50 writes. */
s32 fn_804A0DB8(const RwTexture *texture) {
    s32 size = sizeof(s32);
    if (texture != NULL) {
        size = fn_80483B38(texture) + 0x10;
    }
    return size;
}
#ifdef __cplusplus
}
#endif
