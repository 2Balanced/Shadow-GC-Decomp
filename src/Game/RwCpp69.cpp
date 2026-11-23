/* RenderWare, 0x804A6F8C..0x804A8B08: the GameCube instancing of the material effects' extra
 * vertex streams (fn_804A6F8C, fn_804A8120 and the test fn_804A8478) and the material plugin
 * 0x12C that fn_804A8EFC registers (constructor, destructor, copy, stream write and size; the
 * stream read is in RwCpp70).  fn_804A8558 is paired-single assembly in the original and is not
 * written here.  Functions keep their fn_ symbols; comments say what they do.  Definitions stand
 * in reverse address order: the compiler emits them in reverse.
 * Status: fn_804A8120, fn_804A8478, fn_804A869C, fn_804A86DC, fn_804A871C, fn_804A8924 and
 * fn_804A89C4 match.  fn_804A87D8 has retail's instructions with def/count/size in permuted saved
 * registers; fn_804A8728 keeps retail's `mr r31,r30` (the NULL for the texture slots copied from
 * the zeroed counter) as a second li.  fn_804A6F8C is not written. */
#include <dolphin/types.h>
#include "Game/rw/r8.h"

#ifdef __cplusplus
extern "C" {
#endif
extern u32 lbl_805EDF68;

void fn_804A9158(void);
void fn_804A9274(void);
s32 fn_80485D6C(void);                                                /* RwEngineGetVersion */
RwStream *fn_804A0C50(RwStream *stream, const RwTexture *texture);
s32 fn_804A0DB8(const RwTexture *texture);
s32 fn_80482F64(const void *object);
const void *fn_80482FA4(const void *object, RwStream *stream);
void fn_804A9674(void *node);
void *fn_804A90C8(void *object, s32 index);

/* The definition the plugin data points at. */
typedef struct MTDataDef {
    u32 id;        /* 0x00: written to the stream as one byte */
    u32 unk04[2];
    u32 extraSize; /* 0x0C: bytes of extra data behind MTData */
} MTDataDef;

/* Material plugin 0x12C: up to eight textures with a flag byte each and a resource node. */
typedef struct MTData {
    MTDataDef *def;         /* 0x00 */
    u32 numTextures;        /* 0x04 */
    RwTexture *textures[8]; /* 0x08 */
    u8 textureFlags[8];     /* 0x28 */
    u8 *node;               /* 0x30: a resource whose stream part starts 8 bytes in */
    void *extra;            /* 0x34 */
} MTData;

#define MTDATA(object, offset) (*(MTData **)((u8 *)(object) + (offset)))

void *fn_804A90A0(MTData *data, s32 index);
MTData *fn_804A9030(MTData *data, s32 index, void *texture);
u8 fn_804A90BC(MTData *data, s32 index);
MTData *fn_804A90B0(MTData *data, s32 index, u8 flags);
void *fn_804A9028(MTData *data);
MTData *fn_804A8FCC(MTData *data, void *node);

extern const f32 lbl_805FB4B0; /* FLT_MAX */

void DCFlushRange(void *addr, u32 size);
void fn_80394F2C(void);
void fn_804A6F8C(RwGameCubeInstance *instance, RwGameCubeDisplayList *list,
                 RwGameCubeVertexFormat *format);

/* Prepares the extra vertex stream of an instance (stream 1: normal, binormal and tangent per
 * vertex): the binormal's first component of every vertex is set to its largest value, then
 * fn_804A6F8C fills the stream in for each mesh whose material generates texture coordinates from
 * the geometry. */
void fn_804A8120(RwGameCubeRenderNode *node, RwGameCubeVertexFormat *format, s32 numVertices) {
    RwGameCubeInstance *instance = (RwGameCubeInstance *)(node->resEntry + 0x18);
    RwGameCubeDisplayList *list;
    RpMeshHeader *header = node->meshHeader;
    RpMesh *mesh;
    s32 i;

    list = (RwGameCubeDisplayList *)&instance->streams[1] + (instance->numStreams - 1);

    if (format != NULL) {
        if (format->normalType == 1) {
            u8 *nbt = (u8 *)instance->streams[1].data;
            nbt += 3;
            for (i = 0; i < numVertices; i++) {
                *nbt = 0xFF;
                nbt += 9;
            }
        } else if (format->normalType == 3) {
            u16 *nbt = (u16 *)instance->streams[1].data;
            nbt += 3;
            for (i = 0; i < numVertices; i++) {
                *nbt = 0xFFFF;
                nbt += 9;
            }
        } else {
            f32 *nbt = (f32 *)instance->streams[1].data;
            nbt += 3;
            for (i = 0; i < numVertices; i++) {
                *nbt = lbl_805FB4B0;
                nbt += 9;
            }
        }
    } else {
        f32 *nbt = (f32 *)instance->streams[1].data;
        nbt += 3;
        for (i = 0; i < numVertices; i++) {
            *nbt = lbl_805FB4B0;
            nbt += 9;
        }
    }
    mesh = (RpMesh *)(header + 1);
    for (i = 0; i < header->numMeshes; list++, mesh++, i++) {
        u8 *data = (u8 *)fn_804A90C8(mesh->material, 6);
        if (data != NULL) {
            u8 *matInstance = *(u8 **)(data + 0x30);
            if (matInstance != NULL && *(s32 *)matInstance == 6) {
                GcnMatInstanceBody *body = (GcnMatInstanceBody *)fn_804A60D0(matInstance);
                s32 j;
                for (j = 0; j < body->numTexGens; j++) {
                    GcnMatTexGen *texGen = &body->texGens[j];
                    if (texGen->src == 2 || texGen->src == 3 ||
                        (texGen->type >= 2 && texGen->type <= 9)) {
                        fn_804A6F8C(instance, list, format);
                        break;
                    }
                }
            }
        }
    }
    DCFlushRange(instance->streams[1].data, numVertices * ((u8 *)&instance->streams[1].info)[1]);
    fn_80394F2C();
}

/* Stream write callback of the plugin data. */
RwStream *fn_804A89C4(RwStream *stream, s32 length, const void *object, s32 offset, s32 size) {
    MTData *data = MTDATA(object, offset);

    if (data != NULL) {
        s32 version = fn_80485D6C();
        u8 header[4];

        if (!fn_8047D458(stream, &version, sizeof(version))) return NULL;
        header[0] = data->def->id;
        header[1] = data->numTextures;
        header[2] = (data->node != NULL);
        header[3] = 0;
        if (!fn_80480ED0(stream, header, sizeof(header))) return NULL;
        if (data->numTextures != 0) {
            u32 i;
            if (!fn_80480ED0(stream, data->textureFlags, data->numTextures)) return NULL;
            for (i = 0; i < data->numTextures; i++) {
                fn_804A0C50(stream, data->textures[i]);
            }
        }
        if (data->node != NULL) {
            if (!fn_80482FA4(data->node + 8, stream)) return NULL;
        }
    }
    return stream;
}

/* Stream size callback of the plugin data. */
s32 fn_804A8924(const void *object, s32 offset, s32 size) {
    MTData *data = MTDATA(object, offset);
    s32 total = 0;

    if (data != NULL) {
        u32 i;
        total = data->numTextures + 8;
        for (i = 0; i < data->numTextures; i++) {
            total += fn_804A0DB8(data->textures[i]);
        }
        if (data->node != NULL) {
            total += fn_80482F64(data->node + 8) + 12;
        }
    }
    return total;
}

static inline MTData *MTDataCreate(MTDataDef *def, u32 numTextures) {
    u32 size = def->extraSize + sizeof(MTData);
    MTData *data = (MTData *)RwEngineInstance->memAlloc(size, 0x103012C);
    if (data == NULL) {
        RWERROR(0x120, (0x80000013, size));
        return NULL;
    }
    memset(data, 0, size);
    data->def = def;
    data->numTextures = numTextures;
    if (def->extraSize != 0) {
        data->extra = data + 1;
    }
    return data;
}

/* Copy callback of the plugin data. */
void *fn_804A87D8(void *dstObject, const void *srcObject, s32 offset, s32 size) {
    MTData *src = MTDATA(srcObject, offset);
    MTData *dst;
    u32 i;

    if (src == NULL) return dstObject;
    dst = MTDataCreate(src->def, src->numTextures);
    if (dst == NULL) return NULL;
    MTDATA(dstObject, offset) = dst;
    for (i = 0; i < src->numTextures; i++) {
        fn_804A9030(dst, i, fn_804A90A0(src, i));
        fn_804A90B0(dst, i, fn_804A90BC(src, i));
    }
    fn_804A8FCC(dst, fn_804A9028(src));
    return dstObject;
}

/* Destructor of the plugin data. */
void *fn_804A8728(void *object, s32 offset, s32 size) {
    MTData *data = MTDATA(object, offset);

    if (data != NULL) {
        u32 i;
        for (i = 0; i < data->numTextures; i++) {
            if (data->textures[i] != NULL) {
                fn_8048CD8C(data->textures[i]);
                data->textures[i] = NULL;
            }
        }
        if (data->node != NULL) {
            fn_804A9674(data->node);
            data->node = NULL;
        }
        RwEngineInstance->memFree(data);
        MTDATA(object, offset) = NULL;
    }
    return object;
}

void fn_804A871C(u8 *base, u32 offset) {
    *(u32 *)(base + offset) = 0;
}

void *fn_804A86DC(void *argument) {
    fn_804A9274();
    (&lbl_805EDF68)[1]--;
    return argument;
}

void *fn_804A869C(void *argument) {
    (&lbl_805EDF68)[1]++;
    fn_804A9158();
    return argument;
}


/* Whether a mesh of the node has a material whose instance data generates texture coordinates
 * from the geometry, which needs the extra vertex streams. */
s32 fn_804A8478(RwGameCubeRenderNode *node) {
    RpMeshHeader *header = node->meshHeader;
    RpMesh *mesh = (RpMesh *)(header + 1);
    s32 i;

    for (i = 0; i < header->numMeshes; i++, mesh++) {
        u8 *data = (u8 *)fn_804A90C8(mesh->material, 6);
        if (data != NULL) {
            u8 *instance = *(u8 **)(data + 0x30);
            if (instance != NULL && *(s32 *)instance == 6) {
                GcnMatInstanceBody *body = (GcnMatInstanceBody *)fn_804A60D0(instance);
                s32 j;
                for (j = 0; j < body->numTexGens; j++) {
                    GcnMatTexGen *texGen = &body->texGens[j];
                    if (texGen->src == 2 || texGen->src == 3 ||
                        (texGen->type >= 2 && texGen->type <= 9)) {
                        return TRUE;
                    }
                }
            }
        }
    }
    return FALSE;
}
#ifdef __cplusplus
}
#endif
