/* RenderWare GameCube driver, 0x804643E8-0x80465ED4: GX light loading and the light extension
 * plugin (0x505), morph-target interpolation, the streaming of pre-instanced ("native") geometry
 * and world-sector data, the material colour setters, the channel/TEV setup that picks one, and an
 * index remapping helper of the vertex welder.
 *
 * Source files: 255.0f appears twice in .sdata2 (0x805FB1B4 for fn_804643E8, 0x805FB1E8 for the
 * colour setters), so the lighting file that starts in RwCpp26 (0x804641CC) ends before
 * fn_8046528C; its .sdata2 is 0x805FB1A8-0x805FB1D4, the colour/channel file's 0x805FB1D8-0x805FB1EC
 * (8-byte padding after 0x805FB1E8 closes it). fn_804649C0 is hand-written paired-single assembly
 * and fn_80464814 programs GQR6 (mtspr): neither has a C form, so both are left out.
 *
 * Status: fn_80464A94 / fn_80464DAC miss by the loop counter and walker trading registers (and,
 * in the writer, the scaled mesh count being kept in a saved register); fn_80465CDC keeps the
 * remapped index in a volatile register where retail keeps it live across the count loop;
 * fn_8046528C has retail's instructions but its int-to-float constant is pooled under an @ name.
 *
 * Names are inferred from behaviour:
 *   fn_804643E8  load a light into the next free GX light, in view space
 *   fn_804647B0 / fn_804647C0 / fn_804647C4  light extension constructor / destructor / copy
 *   fn_804647C8  attach the light extension (28 bytes, plugin 0x505)
 *   fn_8046510C / fn_80465138 / fn_80465180  native size / write / read of a geometry
 *   fn_804651AC / fn_80465200 / fn_80465260  native size / write / read of a world sector
 *   fn_80464A94 / fn_80464DAC  read / write a native data chunk (instance data and offsets)
 *   fn_8046528C .. fn_8046554C  scale an ambient colour and load it (with the material colour)
 *                               into a TEV register or the channel colour registers
 *   fn_804655D4  set up the colour channels and TEV for a mesh, return its colour setter
 *   fn_80465CDC  renumber index lists after a vertex weld
 * Built with cflags_rw_r3 (configure.py): C, functions in address order. */
#include <dolphin/types.h>
#include <dolphin/gx.h>
#include <dolphin/os.h>
#include "Game/rw/r3.h"

extern s32 lbl_805F1530;  /* offset of the light extension in a light */
extern GXLightObj lbl_805E16A0[8]; /* the hardware lights being set up */
extern RwMatrix lbl_805E31CC;      /* the camera's view matrix */
extern const f32 lbl_805FB1A8;     /* 1.0f */
extern const f32 lbl_805FB1AC;     /* 0.0f */
extern const f32 lbl_805FB1B4;     /* 255.0f */
extern const f32 lbl_805FB1B8;     /* 0.5f */
extern const f32 lbl_805FB1BC;     /* 180 / pi */
extern const f32 lbl_805FB1E8; /* 255.0f */

extern u16 lbl_805EDF40;

s32 fn_8045AAA0(s32 size, u32 pluginID, void *constructCB, void *destructCB, void *copyCB);
RpWorld *fn_8045FEA0(const RpWorldSector *sector); /* RpWorldSectorGetWorld */
void fn_8046288C(RwResEntry *repEntry);           /* instance data destroy notify */
s32 fn_8047CF50(RwStream *stream, u32 type, u32 *lengthOut, u32 *versionOut); /* RwStreamFindChunk */
RwStream *fn_8047CEF0(RwStream *stream, u32 type, u32 size, u32 version, u32 buildNum);
RwStream *fn_8047D458(RwStream *stream, const void *ints, u32 numBytes); /* RwStreamWriteInt32 */
RwStream *fn_8047D868(RwStream *stream, void *ints, u32 numBytes);       /* RwStreamReadInt32 */
u32 fn_80480D4C(RwStream *stream, void *buffer, u32 length);             /* RwStreamRead */
RwStream *fn_80480ED0(RwStream *stream, const void *buffer, u32 length); /* RwStreamWrite */


RwMatrix *RwFrameGetLTM(RwFrame *frame);
RwV3d *fn_80482458(RwV3d *pointOut, const RwV3d *pointIn, const RwMatrix *matrix);  /* RwV3dTransformPoint */
RwV3d *fn_80482504(RwV3d *vectorOut, const RwV3d *vectorIn, const RwMatrix *matrix); /* RwV3dTransformVector */
f32 fn_8045A818(RpLight *light); /* RpLightGetConeAngle */

/* Loads a light into the next free GX light (up to eight) in view space. */
void fn_804643E8(RpLight *light, RxGCInstanceData *state) {
    GXColor color;
    RwV3d dir;
    RwV3d pos;
    RwMatrix *ltm;
    RpGCLightExt *ext;

    if (state->numLights < 8) {
        color.r = (u8)(lbl_805FB1B4 * light->color.red);
        color.g = (u8)(lbl_805FB1B4 * light->color.green);
        color.b = (u8)(lbl_805FB1B4 * light->color.blue);
        color.a = 0;
        GXInitLightColor(&lbl_805E16A0[state->numLights], color);
        ltm = RwFrameGetLTM((RwFrame *)light->object.object.parent);
        fn_80482458(&pos, &ltm->pos, &lbl_805E31CC);
        GXInitLightPos(&lbl_805E16A0[state->numLights], -pos.x, pos.y, -pos.z);
        ext = (RpGCLightExt *)((u8 *)light + lbl_805F1530);
        switch (light->object.object.subType) {
        case rpLIGHTDIRECTIONAL:
        case rpLIGHTAMBIENT:
            break;
        case rpLIGHTPOINT:
            if (ext->useAttn == 0) {
                GXInitLightAttnA(&lbl_805E16A0[state->numLights], lbl_805FB1A8, lbl_805FB1AC,
                                 lbl_805FB1AC);
                GXInitLightDistAttn(&lbl_805E16A0[state->numLights], lbl_805FB1B8 * light->radius,
                                    lbl_805FB1B8, 2);
            } else {
                GXInitLightAttn(&lbl_805E16A0[state->numLights], ext->a0, ext->a1, ext->a2,
                                ext->k0, ext->k1, ext->k2);
            }
            break;
        case rpLIGHTSPOT:
            fn_80482504(&dir, &ltm->at, &lbl_805E31CC);
            GXInitLightDir(&lbl_805E16A0[state->numLights], -dir.x, dir.y, -dir.z);
            if (ext->useAttn == 0) {
                GXInitLightSpot(&lbl_805E16A0[state->numLights], lbl_805FB1BC * fn_8045A818(light), 1);
                GXInitLightDistAttn(&lbl_805E16A0[state->numLights], lbl_805FB1B8 * light->radius,
                                    lbl_805FB1B8, 2);
            } else {
                GXInitLightAttn(&lbl_805E16A0[state->numLights], ext->a0, ext->a1, ext->a2,
                                ext->k0, ext->k1, ext->k2);
            }
            break;
        case rpLIGHTSPOTSOFT:
            fn_80482504(&dir, &ltm->at, &lbl_805E31CC);
            GXInitLightDir(&lbl_805E16A0[state->numLights], -dir.x, dir.y, -dir.z);
            if (ext->useAttn == 0) {
                GXInitLightSpot(&lbl_805E16A0[state->numLights], lbl_805FB1BC * fn_8045A818(light), 2);
                GXInitLightDistAttn(&lbl_805E16A0[state->numLights], lbl_805FB1B8 * light->radius,
                                    lbl_805FB1B8, 2);
            } else {
                GXInitLightAttn(&lbl_805E16A0[state->numLights], ext->a0, ext->a1, ext->a2,
                                ext->k0, ext->k1, ext->k2);
            }
            break;
        }
        GXLoadLightObjImm(&lbl_805E16A0[state->numLights], 1 << state->numLights);
        state->lightMask |= 1 << state->numLights;
        state->numLights++;
    }
}

void *fn_804647B0(void *object) {
    *(u32 *)((u8 *)object + lbl_805F1530) = 0;
    return object;
}

void *fn_804647C0(void *object) {
    return object;
}

void *fn_804647C4(void *dstObject) {
    return dstObject;
}

s32 fn_804647C8(void) {
    lbl_805F1530 = fn_8045AAA0(0x1C, 0x505, fn_804647B0, fn_804647C0, fn_804647C4);
    return lbl_805F1530 >= 0;
}

/* Reads a native data chunk into a new resource entry of the owner and turns the stored offsets
 * back into pointers. */
void *fn_80464A94(RwStream *stream, void *owner, RwResEntry **repEntryRef, u32 numMeshes) {
    u32 version;
    u32 size;
    u32 headerSize;
    u32 dataSize;
    s32 platform;
    RxGCInstanceHeader *header;
    RxGCInstanceMesh *meshes;
    u8 *base;
    u32 i;

    if (!fn_8047CF50(stream, 1, &size, &version)) {
        return NULL;
    }
    if (version < 0x35000 || version > 0x37002) {
        RWERROR((E_RW_BADVERSION));
        return NULL;
    }
    if (fn_8047D868(stream, &platform, sizeof(platform)) == NULL) {
        return NULL;
    }
    if (platform != 6) {
        return NULL;
    }
    if (fn_8047D868(stream, &headerSize, sizeof(headerSize)) == NULL) {
        return NULL;
    }
    if (fn_8047D868(stream, &dataSize, sizeof(dataSize)) == NULL) {
        return NULL;
    }
    *repEntryRef = (RwResEntry *)RwMalloc(sizeof(RwResEntry) + headerSize + dataSize + 31, 0x3050D);
    header = (RxGCInstanceHeader *)(*repEntryRef + 1);
    if (fn_80480D4C(stream, header, headerSize) != headerSize) {
        return NULL;
    }
    if (fn_80480D4C(stream, (void *)(((u32)header + headerSize + 31) & ~31), dataSize) != dataSize) {
        return NULL;
    }
    meshes = (RxGCInstanceMesh *)((u8 *)(header + 1) +
                                  (header->numArrays - 1) * sizeof(RxGCInstanceArray));
    base = (u8 *)(((u32)meshes + numMeshes * sizeof(RxGCInstanceMesh) + 31) & ~31);
    for (i = 0; i < header->numArrays; i++) {
        header->arrays[i].data = base + (u32)header->arrays[i].data;
    }
    for (i = 0; i < numMeshes; i++) {
        meshes[i].displayList = base + (u32)meshes[i].displayList;
    }
    (*repEntryRef)->link.next = NULL;
    (*repEntryRef)->link.prev = NULL;
    (*repEntryRef)->owner = owner;
    (*repEntryRef)->size = size;
    (*repEntryRef)->ownerRef = repEntryRef;
    (*repEntryRef)->destroyNotify = fn_8046288C;
    header->serialNum = lbl_805EDF40;
    DCFlushRange(*repEntryRef + 1, (*repEntryRef)->size);
    GXInvalidateVtxCache();
    return owner;
}

/* Writes a resource entry's instance data as a native data chunk: the platform, the sizes of the
 * header and of the data, then both, with the pointers stored as offsets from the data base. */
RwStream *fn_80464DAC(RwStream *stream, RwResEntry *repEntry, u32 numMeshes) {
    s32 platform;
    u32 headerSize;
    u32 dataSize;
    u8 *base;
    RxGCInstanceHeader *header;
    RxGCInstanceMesh *meshes;
    u32 i;

    platform = 6;
    header = (RxGCInstanceHeader *)(repEntry + 1);
    meshes = (RxGCInstanceMesh *)((u8 *)(header + 1) +
                                  (header->numArrays - 1) * sizeof(RxGCInstanceArray));
    if (fn_8047CEF0(stream, 1, repEntry->size - 19, 0x37002, 0x37) == NULL) {
        return NULL;
    }
    if (fn_8047D458(stream, &platform, sizeof(platform)) == NULL) {
        return NULL;
    }
    headerSize = sizeof(RxGCInstanceHeader) + (header->numArrays - 1) * sizeof(RxGCInstanceArray) +
                 numMeshes * sizeof(RxGCInstanceMesh);
    if (fn_8047D458(stream, &headerSize, sizeof(headerSize)) == NULL) {
        return NULL;
    }
    dataSize = repEntry->size - headerSize - 31;
    if (fn_8047D458(stream, &dataSize, sizeof(dataSize)) == NULL) {
        return NULL;
    }
    base = (u8 *)(((u32)meshes + (numMeshes * sizeof(RxGCInstanceMesh) + 31)) & ~31);
    for (i = 0; i < header->numArrays; i++) {
        header->arrays[i].data = (void *)((u8 *)header->arrays[i].data - base);
    }
    for (i = 0; i < numMeshes; i++) {
        meshes[i].displayList = (void *)((u8 *)meshes[i].displayList - base);
    }
    if (fn_80480ED0(stream, header, headerSize) == NULL) {
        return NULL;
    }
    for (i = 0; i < header->numArrays; i++) {
        header->arrays[i].data = base + (u32)header->arrays[i].data;
    }
    for (i = 0; i < numMeshes; i++) {
        meshes[i].displayList = base + (u32)meshes[i].displayList;
    }
    if (fn_80480ED0(stream, meshes->displayList, dataSize) == NULL) {
        return NULL;
    }
    DCFlushRange(repEntry + 1, repEntry->size);
    GXInvalidateVtxCache();
    return stream;
}

/* Size of a geometry's native data chunk: the size of its resource entry less 7, or nothing when
 * the geometry is not native or has no resource entry. */
s32 fn_8046510C(const RpGeometry *geometry) {
    if (geometry->flags & rpGEOMETRYNATIVE) {
        if (geometry->repEntry != NULL) {
            return geometry->repEntry->size - 7;
        }
    }
    return 0;
}

RwStream *fn_80465138(RwStream *stream, const RpGeometry *geometry) {
    if (geometry->flags & rpGEOMETRYNATIVE) {
        fn_80464DAC(stream, geometry->repEntry, geometry->mesh->numMeshes);
    }
    return stream;
}

void *fn_80465180(RwStream *stream, RpGeometry *geometry) {
    return fn_80464A94(stream, geometry, &geometry->repEntry, geometry->mesh->numMeshes);
}

s32 fn_804651AC(const RpWorldSector *sector) {
    if (fn_8045FEA0(sector)->flags & rpGEOMETRYNATIVE) {
        if (sector->repEntry != NULL) {
            return sector->repEntry->size - 7;
        }
    }
    return 0;
}

RwStream *fn_80465200(RwStream *stream, const RpWorldSector *sector) {
    if (fn_8045FEA0(sector)->flags & rpGEOMETRYNATIVE) {
        fn_80464DAC(stream, sector->repEntry, sector->mesh->numMeshes);
    }
    return stream;
}

void *fn_80465260(RwStream *stream, RpWorldSector *sector) {
    return fn_80464A94(stream, sector, &sector->repEntry, sector->mesh->numMeshes);
}

void fn_8046528C(RwRGBAReal *ambient, f32 scale, RwRGBA *color) {
    GXColor amb;
    amb.r = (u8)(color->red * (ambient->red * scale));
    amb.g = (u8)(color->green * (ambient->green * scale));
    amb.b = (u8)(color->blue * (ambient->blue * scale));
    amb.a = 0;
    GXSetTevColor(1, *(GXColor *)color);
    GXSetTevColor(2, amb);
}

void fn_80465370(RwRGBAReal *ambient, f32 scale, RwRGBA *color) {
    GXColor amb;
    f32 s = lbl_805FB1E8 * scale;
    amb.r = (u8)(ambient->red * s);
    amb.g = (u8)(ambient->green * s);
    amb.b = (u8)(ambient->blue * s);
    amb.a = 0;
    GXSetTevColor(2, amb);
}

void fn_804653F8(RwRGBAReal *ambient, f32 scale, RwRGBA *color) {
    GXColor amb;
    f32 s = lbl_805FB1E8 * scale;
    amb.r = (u8)(ambient->red * s);
    amb.g = (u8)(ambient->green * s);
    amb.b = (u8)(ambient->blue * s);
    amb.a = 0;
    GXSetChanAmbColor(0, amb);
}

void fn_80465480(RwRGBAReal *ambient, f32 scale, RwRGBA *color) {
    GXColor amb;
    f32 s = lbl_805FB1E8 * scale;
    amb.r = (u8)(ambient->red * s);
    amb.g = (u8)(ambient->green * s);
    amb.b = (u8)(ambient->blue * s);
    amb.a = 0;
    GXSetChanMatColor(4, *(GXColor *)color);
    GXSetChanAmbColor(0, amb);
}

void fn_8046551C(RwRGBAReal *ambient, f32 scale, RwRGBA *color) {
    GXSetChanMatColor(4, *(GXColor *)color);
}

void fn_8046554C(RwRGBAReal *ambient, f32 scale, RwRGBA *color) {
    GXColor amb;
    f32 s = lbl_805FB1E8 * scale;
    amb.r = (u8)(ambient->red * s);
    amb.g = (u8)(ambient->green * s);
    amb.b = (u8)(ambient->blue * s);
    amb.a = 0;
    GXSetChanMatColor(0, amb);
}

extern const GXColor lbl_805FB1D8; /* opaque white */
extern const GXColor lbl_805FB1DC; /* opaque black (alpha 255) */

typedef void (*RwCpp27ColourCallBack)(RwRGBAReal *ambient, f32 scale, RwRGBA *color);

/* Sets up the colour channels and the TEV stages for a mesh with the given geometry flags, lights
 * and prelight alpha, and returns the callback that loads each material's colours (or NULL).
 * Flag 0x40 is rpGEOMETRYMODULATEMATERIALCOLOR, 0x08 prelit, 0x84 textured. */
RwCpp27ColourCallBack fn_804655D4(u32 flags, u32 lightMask, s32 ambientSet, s32 alpha) {
    u32 textured;
    RwCpp27ColourCallBack callback;
    GXBool colorEnable;
    GXBool alphaEnable;
    GXColorSrc colorMatSrc;
    GXColorSrc alphaMatSrc;
    GXColorSrc colorAmbSrc;
    GXColorSrc alphaAmbSrc;
    u8 numStages;
    u32 prelit;

    textured = flags & 0x84;
    callback = NULL;
    if (textured) {
        prelit = flags & 0x08;
        if (prelit && ambientSet == 1) {
            if (lightMask) {
                if (flags & 0x40) {
                    callback = fn_8046528C;
                } else {
                    GXSetTevColor(1, lbl_805FB1D8);
                    callback = fn_80465370;
                }
                colorMatSrc = 0;
                alphaMatSrc = 0;
                GXSetChanMatColor(4, lbl_805FB1D8);
                colorEnable = 1;
                colorAmbSrc = 1;
                if (alpha == 1) {
                    alphaAmbSrc = 1;
                    alphaEnable = 1;
                } else {
                    alphaAmbSrc = 0;
                    GXSetChanAmbColor(2, lbl_805FB1DC);
                    alphaEnable = 0;
                }
            } else {
                if (flags & 0x40) {
                    callback = fn_8046528C;
                } else {
                    GXSetTevColor(1, lbl_805FB1D8);
                    callback = fn_80465370;
                }
                colorAmbSrc = 0;
                alphaAmbSrc = 0;
                colorMatSrc = 1;
                if (alpha == 1) {
                    alphaMatSrc = 1;
                } else {
                    alphaMatSrc = 0;
                    GXSetChanMatColor(2, lbl_805FB1DC);
                }
                colorEnable = 0;
                alphaEnable = 0;
            }
            numStages = 2;
            GXSetTevColorIn(0, 15, 10, 2, 4);
            GXSetTevAlphaIn(0, 7, 5, 1, 2);
        } else {
            if (lightMask) {
                if (flags & 0x40) {
                    colorMatSrc = 0;
                    alphaMatSrc = 0;
                    colorEnable = 1;
                    if (prelit) {
                        colorAmbSrc = 1;
                        if (alpha == 1) {
                            alphaAmbSrc = 1;
                            alphaEnable = 1;
                        } else {
                            alphaAmbSrc = 0;
                            GXSetChanAmbColor(2, lbl_805FB1DC);
                            alphaEnable = 0;
                        }
                        callback = fn_8046551C;
                    } else {
                        colorAmbSrc = 0;
                        alphaAmbSrc = 0;
                        alphaEnable = 0;
                        GXSetChanAmbColor(2, lbl_805FB1DC);
                        callback = fn_80465480;
                    }
                } else {
                    colorMatSrc = 0;
                    alphaMatSrc = 0;
                    GXSetChanMatColor(4, lbl_805FB1D8);
                    colorEnable = 1;
                    if (prelit) {
                        colorAmbSrc = 1;
                        if (alpha == 1) {
                            alphaAmbSrc = 1;
                            alphaEnable = 1;
                        } else {
                            alphaAmbSrc = 0;
                            alphaEnable = 0;
                        }
                    } else {
                        callback = fn_804653F8;
                        colorAmbSrc = 0;
                        alphaAmbSrc = 0;
                        alphaEnable = 0;
                    }
                }
            } else {
                if (flags & 0x40) {
                    colorMatSrc = 0;
                    alphaMatSrc = 0;
                    colorEnable = 1;
                    if (prelit) {
                        colorAmbSrc = 1;
                        if (alpha == 1) {
                            alphaAmbSrc = 1;
                            alphaEnable = 1;
                        } else {
                            alphaAmbSrc = 0;
                            GXSetChanAmbColor(2, lbl_805FB1DC);
                            alphaEnable = 0;
                        }
                        callback = fn_8046551C;
                    } else {
                        colorAmbSrc = 0;
                        alphaAmbSrc = 0;
                        alphaEnable = 0;
                        GXSetChanAmbColor(2, lbl_805FB1DC);
                        callback = fn_80465480;
                    }
                } else {
                    if (prelit) {
                        colorMatSrc = 1;
                        if (alpha == 1) {
                            alphaMatSrc = 1;
                        } else {
                            alphaMatSrc = 0;
                            GXSetChanMatColor(2, lbl_805FB1DC);
                        }
                    } else {
                        colorMatSrc = 0;
                        alphaMatSrc = 0;
                        GXSetChanMatColor(2, lbl_805FB1DC);
                        callback = fn_8046554C;
                    }
                    colorEnable = 0;
                    alphaEnable = 0;
                    colorAmbSrc = 0;
                    alphaAmbSrc = 0;
                }
            }
            numStages = 1;
            GXSetTevColorIn(0, 15, 10, 8, 15);
            GXSetTevAlphaIn(0, 7, 5, 4, 7);
        }
    } else {
        if (lightMask) {
            colorMatSrc = 0;
            alphaMatSrc = 0;
            GXSetChanMatColor(4, lbl_805FB1D8);
            colorEnable = 1;
            if (flags & 0x08) {
                colorAmbSrc = 1;
                if (alpha == 1) {
                    alphaAmbSrc = 1;
                    alphaEnable = 1;
                } else {
                    alphaAmbSrc = 0;
                    alphaEnable = 0;
                }
            } else {
                colorAmbSrc = 0;
                alphaAmbSrc = 0;
                alphaEnable = 0;
                GXSetChanAmbColor(4, lbl_805FB1DC);
            }
        } else {
            if (flags & 0x08) {
                colorMatSrc = 1;
                if (alpha == 1) {
                    alphaMatSrc = 1;
                } else {
                    alphaMatSrc = 0;
                    GXSetChanMatColor(2, lbl_805FB1DC);
                }
            } else {
                colorMatSrc = 0;
                alphaMatSrc = 0;
                GXSetChanMatColor(4, lbl_805FB1DC);
            }
            colorAmbSrc = 0;
            alphaAmbSrc = 0;
            colorEnable = 0;
            alphaEnable = 0;
        }
        numStages = 1;
        if (flags & 0x40) {
            callback = fn_8046528C;
        } else {
            GXSetTevColor(1, lbl_805FB1D8);
            callback = fn_80465370;
        }
        GXSetTevColorIn(0, 15, 10, 2, 4);
        GXSetTevAlphaIn(0, 7, 5, 1, 2);
    }
    GXSetNumTevStages(numStages);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    if (numStages > 1) {
        GXSetTevColorIn(1, 15, 0, 8, 15);
        GXSetTevColorOp(1, 0, 0, 0, 1, 0);
        GXSetTevAlphaIn(1, 7, 0, 4, 7);
        GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
        GXSetTevOrder(0, 0xFF, 0xFF, 4);
        GXSetTevOrder(1, 0, 0, 0xFF);
        GXSetNumTexGens(1);
        GXSetTexCoordGen2(0, 1, 4, 60, 0, 125);
    } else if (textured) {
        GXSetTevOrder(0, 0, 0, 4);
        GXSetNumTexGens(1);
        GXSetTexCoordGen2(0, 1, 4, 60, 0, 125);
    } else {
        GXSetNumTexGens(0);
        GXSetTevOrder(0, 0xFF, 0xFF, 4);
    }
    GXSetNumChans(1);
    GXSetChanCtrl(0, colorEnable, colorAmbSrc, colorMatSrc, lightMask, 2, 1);
    GXSetChanCtrl(2, alphaEnable, alphaAmbSrc, alphaMatSrc, 0, 0, 2);
    GXSetChanCtrl(1, 0, 0, 0, 0, 0, 2);
    GXSetChanCtrl(3, 0, 0, 0, 0, 0, 2);
    return callback;
}

/* For each of count index lists, a copy of its indices renumbered after a vertex weld: an index
 * whose remap entry is set is redirected there, then lowered by the number of welded (remapped)
 * vertices below it. One allocation holds the row pointers and the rows. */
typedef struct RwCpp27Remap {
    s32 *remap;
    u32 unk4;
} RwCpp27Remap;

void *memset(void *dst, int value, u32 size);

u16 **fn_80465CDC(RwCpp27Remap *remaps, u16 **indexLists, u32 count, u32 numIndices) {
    u32 rowSize;
    u16 **rows;
    u32 offset;
    u32 i;

    rowSize = numIndices * sizeof(u16);
    rows = (u16 **)RwMalloc(count * sizeof(u16 *) + count * rowSize, 0x3050D);
    if (rows == NULL) {
        return NULL;
    }
    memset(rows, 0, count * sizeof(u16 *));
    offset = count * sizeof(u16 *);
    for (i = 0; i < count; i++) {
        rows[i] = (u16 *)((u8 *)rows + offset);
        offset += rowSize;
    }
    for (i = 0; i < count; i++) {
        s32 *remap = remaps[i].remap;
        u16 *src = indexLists[i];
        u16 *dst = rows[i];
        u32 j;
        for (j = 0; j < numIndices; j++) {
            u16 index = *src;
            u32 below = 0;
            u16 k;
            if (remap[index] != -1) {
                index = remap[index];
            }
            for (k = 0; k < index; k++) {
                if (remap[k] != -1) {
                    below++;
                }
            }
            src++;
            *dst = index - below;
            dst++;
        }
    }
    return rows;
}

/* Not written: fn_80464814 (morph-target interpolation; it programs GQR6 with mtspr, which C cannot
 * express) and fn_804649C0 (hand-written paired-single assembly). */
