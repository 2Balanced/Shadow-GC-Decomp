#ifndef GAME_CSDRW_H
#define GAME_CSDRW_H

/* The RenderWare calls of the Chao CSD library's RenderWare units (Game/csdPlatformRW.cpp,
 * Game/csdTexListRW.cpp, ...): rw/rwcore.h names, as in Game/AdvStaffRoll.h, kept in the library's
 * namespace so that the two declarations never meet. */

#include <dolphin/types.h>

namespace Chao {
namespace CSD {

struct RwRGBA {
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
};

/* An immediate-mode 2D vertex. */
struct RwIm2DVertex {
    /* 0x00 */ f32 x;
    /* 0x04 */ f32 y;
    /* 0x08 */ f32 z;
    /* 0x0C */ RwRGBA emissiveColor;
    /* 0x10 */ f32 u;
    /* 0x14 */ f32 v;
};

struct RwGlobals {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ s32 (*fpRenderStateSet)(s32 state, void *value);
    /* 0x24 */ s32 (*fpRenderStateGet)(s32 state, void *value);
    /* 0x28 */ u8 field_0x28[0x30 - 0x28];
    /* 0x30 */ s32 (*fpIm2DRenderPrimitive)(s32 primType, RwIm2DVertex *vertices, s32 count);
    /* 0x34 */ u8 field_0x34[0x108 - 0x34];
    /* 0x108 */ void *(*fpMalloc)(u32 size, u32 hint); /* memoryFuncs.rwmalloc */
    /* 0x10C */ void (*fpFree)(void *p);                /* memoryFuncs.rwfree */
};

enum {
    rwRENDERSTATETEXTURERASTER = 1,
    rwRENDERSTATETEXTUREADDRESSU = 3,
    rwRENDERSTATETEXTUREADDRESSV = 4,
    rwRENDERSTATEZTESTENABLE = 6,
    rwRENDERSTATEZWRITEENABLE = 8,
    rwRENDERSTATETEXTUREFILTER = 9,
    rwRENDERSTATESRCBLEND = 10,
    rwRENDERSTATEDESTBLEND = 11,
    rwRENDERSTATEVERTEXALPHAENABLE = 12,
    rwRENDERSTATECULLMODE = 20,
};

enum {
    rwPRIMTYPETRISTRIP = 4,
};

struct RwRaster;
struct RwStream;
struct RwTexDictionary;

/* A texture: its raster comes first. */
struct RwTexture {
    /* 0x0 */ RwRaster *raster;
};

/* A stream's memory (rwSTREAMMEMORY). */
struct RwMemory {
    /* 0x0 */ const void *start;
    /* 0x4 */ u32 length;
};

typedef RwTexture *(*RwTextureCallBack)(RwTexture *texture, void *data);

enum {
    rwSTREAMMEMORY = 3,
    rwSTREAMREAD = 1,
    rwID_TEXDICTIONARY = 0x16,
};

} // namespace CSD
} // namespace Chao

/* The engine instance (RwEngineInstance). */
extern Chao::CSD::RwGlobals *lbl_805F15B4;

/* RenderWare functions (names from their use; the symbols are still placeholders). */
extern "C" {
Chao::CSD::RwStream *fn_80481368(s32 type, s32 access, void *data);          /* RwStreamOpen */
s32 fn_8048124C(Chao::CSD::RwStream *stream, void *data);                     /* RwStreamClose */
s32 fn_8047CF50(Chao::CSD::RwStream *stream, u32 type, u32 *length, u32 *version); /* RwStreamFindChunk */
Chao::CSD::RwTexDictionary *fn_80483FBC(Chao::CSD::RwStream *stream);         /* RwTexDictionaryStreamRead */
Chao::CSD::RwTexDictionary *fn_8048CBE0(Chao::CSD::RwTexDictionary *dict, Chao::CSD::RwTextureCallBack callBack,
                                        void *data);                          /* RwTexDictionaryForAllTextures */
s32 fn_8048CB38(Chao::CSD::RwTexDictionary *dict);                            /* RwTexDictionaryDestroy */
Chao::CSD::RwTexDictionary *fn_8048D0C4(Chao::CSD::RwTexDictionary *dict);    /* RwTexDictionarySetCurrent */
Chao::CSD::RwTexDictionary *fn_8048D0D8(void);                                /* RwTexDictionaryGetCurrent */
}

#endif
