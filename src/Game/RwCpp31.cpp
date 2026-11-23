/* RenderWare GameCube driver, vertex-array conversion (third part) and display-list sizing,
 * 0x804699F0..0x8046B554. Names are inferred from behaviour. Compiled by CodeWarrior GC 1.3.2
 * with -O4,p. Matching: fn_804699F0, fn_8046A270, fn_8046ACBC, fn_8046B094, fn_8046B0E0.
 * fn_8046B110 has retail's instructions (273) and all but three of its saved registers: the
 * geometry, the stream total and the mesh-table size come out in r28/r30/r29 for retail's
 * r30/r29/r28 (declaration orders searched; the void * object and the typed copy of the stream
 * count are what brought it there). .data is fn_8046ACBC's jump table (0x8056DAF8, padded to
 * 0x8056DB50: a source-file boundary in .data). */
#include <dolphin/types.h>
#include <string.h>
#include "Game/rw/r4.h"

extern "C" {
/* RwRGBA colours to a colour stream. `type` is the GX colour format: 0 RGB565, 1 RGB8,
 * 2 RGBX8, 3 RGBA4, 4 RGBA6, 5 RGBA8. Returns the packed size of the data. */
s32 fn_804699F0(void *dest, const RwRGBA *src, s32 type, s32 num, u32 stride) {
    s32 size = 0;
    s32 i;

    switch (type) {
    case 0: {
        if (stride == 0) {
            stride = 2;
        }
        u16 *out = (u16 *)dest;
        for (i = 0; i < num; i++) {
            u8 r = src->red, g = src->green, bl = src->blue;
            *out = ((r << 8) & 0xF800) | ((g << 3) & 0x7E0) | ((bl >> 3) & 0x1F);
            out = (u16 *)((u32)out + stride);
            src++;
        }
        size = num * 2;
        break;
    }
    case 1: {
        if (stride == 0) {
            stride = 3;
        }
        u8 *out = (u8 *)dest;
        s32 j;
        for (j = 0; j < num; j++) {
            out[0] = src->red;
            out[1] = src->green;
            out[2] = src->blue;
            out = (u8 *)((u32)out + stride);
            src++;
        }
        size = num * 3;
        break;
    }
    case 3: {
        if (stride == 0) {
            stride = 2;
        }
        u16 *out = (u16 *)dest;
        for (i = 0; i < num; i++) {
            u8 r = src->red, g = src->green, bl = src->blue;
            *out = ((r << 8) & 0xF000) | ((g << 4) & 0xF00) | (bl & 0xF0) | ((src->alpha >> 4) & 0xF);
            out = (u16 *)((u32)out + stride);
            src++;
        }
        size = num * 2;
        break;
    }
    case 4: {
        if (stride == 0) {
            stride = 3;
        }
        u8 *out = (u8 *)dest;
        for (i = 0; i < num; i++) {
            out[0] = (src->red & 0xFC) | ((src->green >> 6) & 0x03);
            out[1] = ((src->green << 2) & 0xF0) | ((src->blue >> 4) & 0x0F);
            out[2] = ((src->blue << 4) & 0xC0) | ((src->alpha >> 2) & 0x3F);
            out += stride;
            src++;
        }
        size = num * 3;
        break;
    }
    case 2:
    case 5: {
        if (stride == 0) {
            stride = 4;
        }
        u32 *out = (u32 *)dest;
        s32 j;
        for (j = 0; j < num; j++) {
            *out = *(const u32 *)src;
            out = (u32 *)((u32)out + stride);
            src++;
        }
        size = num * 4;
        break;
    }
    }
    return size;
}

/* Texture coordinates to a coordinate stream, scaled by 2^frac for the fixed-point types
 * (0 u8, 1 s8, 2 u16, 3 s16); 4 copies the floats. */
s32 fn_8046A270(void *dest, const RwTexCoords *src, s32 type, s32 num, u32 stride, f32 scale) {
    s32 size = 0;
    s32 i;

    switch (type) {
    case 0: {
        if (stride == 0) {
            stride = 2;
        }
        u8 *out = (u8 *)dest;
        for (i = 0; i < num; i++) {
            out[0] = (u8)(s32)(src->u * scale);
            out[1] = (u8)(s32)(src->v * scale);
            out += stride;
            src++;
        }
        size = num * 2;
        break;
    }
    case 1: {
        if (stride == 0) {
            stride = 2;
        }
        s8 *out = (s8 *)dest;
        for (i = 0; i < num; i++) {
            out[0] = (s8)(s32)(src->u * scale);
            out[1] = (s8)(s32)(src->v * scale);
            out += stride;
            src++;
        }
        size = num * 2;
        break;
    }
    case 2: {
        if (stride == 0) {
            stride = 4;
        }
        u16 *out = (u16 *)dest;
        s32 j;
        for (j = 0; j < num; j++) {
            out[0] = (u16)(s32)(src->u * scale);
            out[1] = (u16)(s32)(src->v * scale);
            out = (u16 *)((u32)out + stride);
            src++;
        }
        size = num * 4;
        break;
    }
    case 3: {
        if (stride == 0) {
            stride = 4;
        }
        s16 *out = (s16 *)dest;
        s32 j;
        for (j = 0; j < num; j++) {
            out[0] = (s16)(s32)(src->u * scale);
            out[1] = (s16)(s32)(src->v * scale);
            out = (s16 *)((u32)out + stride);
            src++;
        }
        size = num * 4;
        break;
    }
    case 4: {
        if (stride == 0) {
            stride = 8;
        }
        RwGameCubeV2d *out = (RwGameCubeV2d *)dest;
        s32 j;
        for (j = 0; j < num; j++) {
            *out = *(const RwGameCubeV2d *)src;
            out = (RwGameCubeV2d *)((u32)out + stride);
            src++;
        }
        size = num * 8;
        break;
    }
    }
    return size;
}

/* The size in bytes of one vertex of a display list drawn with `desc`: a byte for each matrix
 * index and for each attribute indexed by bytes, two for 16-bit indices, the attribute's own
 * size (from RwCpp28.cpp) when it is direct. Normals count three times with NBT indices. */
u32 fn_8046ACBC(RwGameCubeVtxDesc *desc) {
    s32 attr;
    u32 size = 0;

    for (attr = 0; attr < 21; attr++) {
        switch (attr) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
            if ((1 << attr) & desc->vcdLo) {
                size += 1;
            }
            break;
        case 9:
            if (RWGAMECUBE_POS_TYPE(desc) == 1) {
                size += fn_8046693C(desc);
            } else if (RWGAMECUBE_POS_TYPE(desc) == 2) {
                size += 1;
            } else if (RWGAMECUBE_POS_TYPE(desc) == 3) {
                size += 2;
            }
            break;
        case 10: {
            /* One variable holds the flag and then the mode: only this keeps retail's registers. */
            u32 mode = (desc->vatA >> 9) & 1;
            u32 nbt = mode;
            mode = RWGAMECUBE_NRM_TYPE(desc);
            if (nbt == 1) {
                u32 count = 3;
                if ((desc->vatA >> 31) == 0) {
                    count = 1;
                }
                if (mode == 1) {
                    size += count * fn_804669A0(desc);
                } else if (mode == 2) {
                    size += count;
                } else if (mode == 3) {
                    size += count * 2;
                }
            } else {
                if (mode == 1) {
                    size += fn_804669A0(desc);
                } else if (mode == 2) {
                    size += 1;
                } else if (mode == 3) {
                    size += 2;
                }
            }
            break;
        }
        case 11:
        case 12:
            if (RWGAMECUBE_CLR_TYPE(desc, attr - 11) == 1) {
                size += fn_804669EC(desc, attr - 11);
            } else if (RWGAMECUBE_CLR_TYPE(desc, attr - 11) == 2) {
                size += 1;
            } else if (RWGAMECUBE_CLR_TYPE(desc, attr - 11) == 3) {
                size += 2;
            }
            break;
        case 13:
            if (RWGAMECUBE_TEX_TYPE(desc, 0) == 1) {
                size += fn_80466A54(desc, 0);
            } else if (RWGAMECUBE_TEX_TYPE(desc, 0) == 2) {
                size += 1;
            } else if (RWGAMECUBE_TEX_TYPE(desc, 0) == 3) {
                size += 2;
            }
            break;
        case 14:
            if (RWGAMECUBE_TEX_TYPE(desc, 1) == 1) {
                size += fn_80466A54(desc, 1);
            } else if (RWGAMECUBE_TEX_TYPE(desc, 1) == 2) {
                size += 1;
            } else if (RWGAMECUBE_TEX_TYPE(desc, 1) == 3) {
                size += 2;
            }
            break;
        case 15:
            if (RWGAMECUBE_TEX_TYPE(desc, 2) == 1) {
                size += fn_80466A54(desc, 2);
            } else if (RWGAMECUBE_TEX_TYPE(desc, 2) == 2) {
                size += 1;
            } else if (RWGAMECUBE_TEX_TYPE(desc, 2) == 3) {
                size += 2;
            }
            break;
        case 16:
            if (RWGAMECUBE_TEX_TYPE(desc, 3) == 1) {
                size += fn_80466A54(desc, 3);
            } else if (RWGAMECUBE_TEX_TYPE(desc, 3) == 2) {
                size += 1;
            } else if (RWGAMECUBE_TEX_TYPE(desc, 3) == 3) {
                size += 2;
            }
            break;
        case 17:
            if (RWGAMECUBE_TEX_TYPE(desc, 4) == 1) {
                size += fn_80466A54(desc, 4);
            } else if (RWGAMECUBE_TEX_TYPE(desc, 4) == 2) {
                size += 1;
            } else if (RWGAMECUBE_TEX_TYPE(desc, 4) == 3) {
                size += 2;
            }
            break;
        case 18:
            if (RWGAMECUBE_TEX_TYPE(desc, 5) == 1) {
                size += fn_80466A54(desc, 5);
            } else if (RWGAMECUBE_TEX_TYPE(desc, 5) == 2) {
                size += 1;
            } else if (RWGAMECUBE_TEX_TYPE(desc, 5) == 3) {
                size += 2;
            }
            break;
        case 19:
            if (RWGAMECUBE_TEX_TYPE(desc, 6) == 1) {
                size += fn_80466A54(desc, 6);
            } else if (RWGAMECUBE_TEX_TYPE(desc, 6) == 2) {
                size += 1;
            } else if (RWGAMECUBE_TEX_TYPE(desc, 6) == 3) {
                size += 2;
            }
            break;
        case 20:
            if (RWGAMECUBE_TEX_TYPE(desc, 7) == 1) {
                size += fn_80466A54(desc, 7);
            } else if (RWGAMECUBE_TEX_TYPE(desc, 7) == 2) {
                size += 1;
            } else if (RWGAMECUBE_TEX_TYPE(desc, 7) == 3) {
                size += 2;
            }
            break;
        }
    }
    return size;
}

/* The size of a display list of `numLists` primitives (a command byte and a 16-bit count each)
 * holding `numIndices` vertices, rounded up to GX's 32 bytes. */
u32 fn_8046B094(RwGameCubeVtxDesc *desc, u32 numLists, u32 numIndices) {
    u32 size;
    u32 dataSize = fn_8046ACBC(desc) * numIndices;

    size = numLists * 3;
    size += dataSize;
    size = (size + 31) & ~31;
    return size;
}

/* Records a display-list buffer and its size in `block`, and clears the buffer. */
void fn_8046B0E0(void **block, u32 unused, u32 size, void *mem) {
    block[0] = mem;
    block[1] = (void *)size;
    memset(mem, 0, size);
}

void *fn_80466194(RwGameCubeVtxStream *streams, s32 numStreams, s32 numVertices);
void *fn_80465ED4(void *table, RwGameCubeVtxStream *streams, s32 numStreams, s32 numVertices);
void *fn_80465CDC(void *table, u16 **indices, s32 numStreams, u32 numIndices);

/* Builds the indexed vertex data of a geometry: one stream per attribute array (positions,
 * normals, prelight colours, texture coordinate sets), merged by the index optimiser into
 * `*vertexData`, and one index block per mesh in `*meshData`. Returns 0 when out of memory.
 * The failure path of the last loop leaks as retail does. */
s32 fn_8046B110(void *object, void **vertexData, void *meshDataOut) {
    void ***meshData = (void ***)meshDataOut;
    RwGameCubeVtxStream streams[16];
    s32 i;
    u32 total;
    RpGeometry *geom = (RpGeometry *)object;
    u32 size;
    void *table;
    u16 **indices;
    s32 numStreams;
    s32 numMeshes;

    if (RWGAMECUBEGEOMFORMAT(geom) == NULL) {
        fn_80462E1C();
    }

    numStreams = 0;
    streams[numStreams].data = geom->morphTarget->verts;
    streams[numStreams].type = 8;
    streams[numStreams].link = -1;
    numStreams++;
    if (geom->flags & 0x10) {
        streams[numStreams].data = geom->morphTarget->normals;
        streams[numStreams].type = 8;
        streams[numStreams].link = -1;
        numStreams++;
    }
    if (geom->flags & 8) {
        streams[numStreams].data = geom->preLitLum;
        streams[numStreams].type = 4;
        streams[numStreams].link = -1;
        numStreams++;
    }
    if (geom->flags & 0x84) {
        for (i = 0; i < geom->numTexCoordSets; i++) {
            streams[numStreams].data = geom->texCoords[i];
            streams[numStreams].type = 7;
            streams[numStreams].link = -1;
            numStreams++;
        }
    }

    total = numStreams;
    table = fn_80466194(streams, total, geom->numVertices);
    if (table == NULL) {
        return 0;
    }
    *vertexData = fn_80465ED4(table, streams, numStreams, geom->numVertices);
    if (*vertexData == NULL) {
        RwFree(table);
        return 0;
    }

    numMeshes = geom->mesh->numMeshes;
    size = numMeshes * 4;
    indices = (u16 **)RwMalloc(numStreams * size, 0x1050D);
    if (indices == NULL) {
        RwFree(*vertexData);
        RwFree(table);
        return 0;
    }
    memset(indices, 0, numStreams * size);
    for (i = 0; i < numMeshes; i++) {
        RpMesh *mesh = &RPMESHES(geom->mesh)[i];
        s32 j;
        for (j = 0; j < numStreams; j++) {
            indices[i * numStreams + j] = mesh->indices;
        }
    }

    *meshData = (void **)RwMalloc(size, 0x1050D);
    if (*meshData == NULL) {
        RwFree(indices);
        RwFree(*vertexData);
        RwFree(table);
        return 0;
    }
    for (i = 0; i < numMeshes; i++) {
        (*meshData)[i] = fn_80465CDC(table, &indices[i * numStreams], total,
                                     RPMESHES(geom->mesh)[i].numIndices);
        if ((*meshData)[i] == NULL) {
            while (--i > 0) {
                RwFree((*meshData)[i]);
            }
            return 0;
        }
    }
    RwFree(indices);
    RwFree(table);
    return 1;
}
}
