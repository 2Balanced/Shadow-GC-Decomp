#ifndef DOLPHIN_GX_H
#define DOLPHIN_GX_H

/* The Dolphin SDK graphics library (GX): public types and the functions the decompiled GX units
 * define. Names are the SDK's public interface names. Enumerations are plain ints (-enum int);
 * only the values the GX sources use by name are listed. */

#include <dolphin/types.h>
#include <dolphin/gx/GXLinkNames.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef u8 GXBool;
#define GX_FALSE ((GXBool)0)
#define GX_TRUE ((GXBool)1)
#define GX_DISABLE ((GXBool)0)
#define GX_ENABLE ((GXBool)1)

typedef struct GXColor {
    u8 r, g, b, a;
} GXColor;

typedef struct GXColorS10 {
    s16 r, g, b, a;
} GXColorS10;

/* Opaque objects: the SDK only fixes their size. */
typedef struct GXFifoObj {
    u8 pad[128];
} GXFifoObj;

typedef struct GXTexObj {
    u32 dummy[8];
} GXTexObj;

typedef struct GXTlutObj {
    u32 dummy[3];
} GXTlutObj;

typedef struct GXLightObj {
    u32 dummy[16];
} GXLightObj;

typedef struct GXTexRegion {
    u32 dummy[4];
} GXTexRegion;

typedef struct GXTlutRegion {
    u32 dummy[4];
} GXTlutRegion;

typedef struct GXRenderModeObj {
    u32 viTVmode;
    u16 fbWidth;
    u16 efbHeight;
    u16 xfbHeight;
    u16 viXOrigin;
    u16 viYOrigin;
    u16 viWidth;
    u16 viHeight;
    u32 xFBmode;
    u8 field_rendering;
    u8 aa;
    u8 sample_pattern[12][2];
    u8 vfilter[7];
} GXRenderModeObj;

typedef f32 Mtx[3][4];
typedef f32 (*MtxPtr)[4];
typedef f32 Mtx44[4][4];

typedef int GXAttr;
typedef int GXAttrType;
typedef int GXCompCnt;
typedef int GXCompType;
typedef int GXVtxFmt;
typedef int GXPrimitive;
typedef int GXTexCoordID;
typedef int GXTexGenType;
typedef int GXTexGenSrc;
typedef int GXTexMapID;
typedef int GXChannelID;
typedef int GXTexFmt;
typedef int GXCITexFmt;
typedef int GXTexWrapMode;
typedef int GXTexFilter;
typedef int GXAnisotropy;
typedef int GXTlutFmt;
typedef int GXTlut;
typedef int GXTlutSize;
typedef int GXTexCacheSize;
typedef int GXTevStageID;
typedef int GXTevMode;
typedef int GXTevColorArg;
typedef int GXTevAlphaArg;
typedef int GXTevOp;
typedef int GXTevBias;
typedef int GXTevScale;
typedef int GXTevRegID;
typedef int GXTevKColorID;
typedef int GXTevKColorSel;
typedef int GXTevKAlphaSel;
typedef int GXTevSwapSel;
typedef int GXTevColorChan;
typedef int GXCompare;
typedef int GXAlphaOp;
typedef int GXIndTexStageID;
typedef int GXIndTexFormat;
typedef int GXIndTexBiasSel;
typedef int GXIndTexAlphaSel;
typedef int GXIndTexMtxID;
typedef int GXIndTexWrap;
typedef int GXIndTexScale;
typedef int GXFogType;
typedef int GXBlendMode;
typedef int GXBlendFactor;
typedef int GXLogicOp;
typedef int GXPixelFmt;
typedef int GXZFmt16;
typedef int GXCullMode;
typedef int GXProjectionType;
typedef int GXClipMode;
typedef int GXColorSrc;
typedef int GXDiffuseFn;
typedef int GXAttnFn;
typedef int GXSpotFn;
typedef int GXDistAttnFn;
typedef int GXFBClamp;
typedef int GXGamma;
typedef int GXCopyMode;
typedef int GXTexMtxType;
typedef int GXPosNrmMtx;
typedef int GXMiscToken;
typedef int GXPerf0;
typedef int GXPerf1;

typedef struct GXVtxAttrFmtList {
    GXAttr attr;
    GXCompCnt cnt;
    GXCompType compType;
    u8 frac;
} GXVtxAttrFmtList;

typedef struct GXVtxDescList {
    GXAttr attr;
    GXAttrType type;
} GXVtxDescList;

typedef void (*GXBreakPtCallback)(void);
typedef void (*GXDrawSyncCallback)(u16 token);
typedef void (*GXDrawDoneCallback)(void);
typedef GXTexRegion *(*GXTexRegionCallback)(const GXTexObj *t_obj, GXTexMapID id);
typedef GXTlutRegion *(*GXTlutRegionCallback)(u32 idx);

extern GXRenderModeObj GXNtsc480IntDf;
extern GXRenderModeObj GXNtsc480Prog;
extern GXRenderModeObj GXMpal480IntDf;
extern GXRenderModeObj GXPal528IntDf;
extern GXRenderModeObj GXEurgb60Hz480IntDf;

/* GXInit.c */
GXFifoObj *GXInit(void *base, u32 size);

/* GXFifo.c */
void GXInitFifoBase(GXFifoObj *fifo, void *base, u32 size);
void GXInitFifoPtrs(GXFifoObj *fifo, void *readPtr, void *writePtr);
void GXInitFifoLimits(GXFifoObj *fifo, u32 hiWatermark, u32 loWatermark);
void GXSetCPUFifo(GXFifoObj *fifo);
void GXSetGPFifo(GXFifoObj *fifo);
GXBreakPtCallback GXSetBreakPtCallback(GXBreakPtCallback cb);
void GXGetFifoPtrs(GXFifoObj *fifo, void **readPtr, void **writePtr);
void GXEnableBreakPt(void *breakPt);
void GXDisableBreakPt(void);
struct OSThread *GXSetCurrentGXThread(void);
GXFifoObj *GXGetCPUFifo(void);
GXFifoObj *GXGetGPFifo(void);

/* GXAttr.c */
void GXSetVtxDesc(GXAttr attr, GXAttrType type);
void GXClearVtxDesc(void);
void GXSetVtxAttrFmt(GXVtxFmt vtxfmt, GXAttr attr, GXCompCnt cnt, GXCompType type, u8 frac);
void GXSetVtxAttrFmtv(GXVtxFmt vtxfmt, const GXVtxAttrFmtList *list);
void GXSetArray(GXAttr attr, const void *base_ptr, u8 stride);
void GXInvalidateVtxCache(void);
void GXSetTexCoordGen2(GXTexCoordID dst_coord, GXTexGenType func, GXTexGenSrc src_param, u32 mtx,
                       GXBool normalize, u32 pt_texmtx);
void GXSetNumTexGens(u8 nTexGens);

/* GXMisc.c */
void GXSetMisc(GXMiscToken token, u32 val);
void GXFlush(void);
void GXSetDrawSync(u16 token);
u16 GXReadDrawSync(void);
void GXAbortFrame(void);
void GXSetDrawDone(void);
void GXWaitDrawDone(void);
void GXDrawDone(void);
void GXPixModeSync(void);
void GXPokeAlphaMode(GXCompare func, u8 threshold);
void GXPokeAlphaRead(int mode);
void GXPokeAlphaUpdate(GXBool update_enable);
void GXPokeBlendMode(GXBlendMode type, GXBlendFactor src_factor, GXBlendFactor dst_factor,
                     GXLogicOp op);
void GXPokeColorUpdate(GXBool update_enable);
void GXPokeDstAlpha(GXBool enable, u8 alpha);
void GXPokeDither(GXBool dither);
void GXPokeZMode(GXBool compare_enable, GXCompare func, GXBool update_enable);
GXDrawSyncCallback GXSetDrawSyncCallback(GXDrawSyncCallback cb);
GXDrawDoneCallback GXSetDrawDoneCallback(GXDrawDoneCallback cb);

/* GXGeometry.c */
void GXBegin(GXPrimitive type, GXVtxFmt vtxfmt, u16 nverts);
void GXSetLineWidth(u8 width, int texOffsets);
void GXSetPointSize(u8 pointSize, int texOffsets);
void GXEnableTexOffsets(GXTexCoordID coord, GXBool line_enable, GXBool point_enable);
void GXSetCullMode(GXCullMode mode);
void GXSetCoPlanar(GXBool enable);

/* GXFrameBuf.c */
void GXSetDispCopySrc(u16 left, u16 top, u16 wd, u16 ht);
void GXSetTexCopySrc(u16 left, u16 top, u16 wd, u16 ht);
void GXSetDispCopyDst(u16 wd, u16 ht);
void GXSetTexCopyDst(u16 wd, u16 ht, GXTexFmt fmt, GXBool mipmap);
void GXSetDispCopyFrame2Field(int mode);
void GXSetCopyClamp(GXFBClamp clamp);
u32 GXSetDispCopyYScale(f32 vscale);
void GXSetCopyClear(GXColor clear_clr, u32 clear_z);
void GXSetCopyFilter(GXBool aa, const u8 sample_pattern[12][2], GXBool vf, const u8 vfilter[7]);
void GXSetDispCopyGamma(GXGamma gamma);
void GXCopyDisp(void *dest, GXBool clear);
void GXCopyTex(void *dest, GXBool clear);
void GXClearBoundingBox(void);

/* GXLight.c */
void GXInitLightAttn(GXLightObj *lt_obj, f32 a0, f32 a1, f32 a2, f32 k0, f32 k1, f32 k2);
void GXInitLightAttnA(GXLightObj *lt_obj, f32 a0, f32 a1, f32 a2);
void GXInitLightSpot(GXLightObj *lt_obj, f32 cutoff, GXSpotFn spot_func);
void GXInitLightDistAttn(GXLightObj *lt_obj, f32 ref_dist, f32 ref_br, GXDistAttnFn dist_func);
void GXInitLightPos(GXLightObj *lt_obj, f32 x, f32 y, f32 z);
void GXInitLightDir(GXLightObj *lt_obj, f32 nx, f32 ny, f32 nz);
void GXInitLightColor(GXLightObj *lt_obj, GXColor color);
void GXLoadLightObjImm(GXLightObj *lt_obj, u32 light);
void GXSetChanAmbColor(GXChannelID chan, GXColor amb_color);
void GXSetChanMatColor(GXChannelID chan, GXColor mat_color);
void GXSetNumChans(u8 nChans);
void GXSetChanCtrl(GXChannelID chan, GXBool enable, GXColorSrc amb_src, GXColorSrc mat_src,
                   u32 light_mask, GXDiffuseFn diff_fn, GXAttnFn attn_fn);

/* GXTexture.c */
void GXInitTexObj(GXTexObj *obj, void *image_ptr, u16 width, u16 height, GXTexFmt format,
                  GXTexWrapMode wrap_s, GXTexWrapMode wrap_t, GXBool mipmap);
GXTexFmt GXGetTexObjFmt(const GXTexObj *obj);
GXBool GXGetTexObjMipMap(const GXTexObj *obj);
void GXLoadTexObjPreLoaded(GXTexObj *obj, GXTexRegion *region, GXTexMapID id);
void GXLoadTexObj(GXTexObj *obj, GXTexMapID id);
void GXInitTexCacheRegion(GXTexRegion *region, GXBool is_32b_mipmap, u32 tmem_even,
                          GXTexCacheSize size_even, u32 tmem_odd, GXTexCacheSize size_odd);
void GXInitTlutRegion(GXTlutRegion *region, u32 tmem_addr, GXTlutSize tlut_size);
void GXInvalidateTexAll(void);
GXTexRegionCallback GXSetTexRegionCallback(GXTexRegionCallback f);
GXTlutRegionCallback GXSetTlutRegionCallback(GXTlutRegionCallback f);

/* GXBump.c */
void GXSetTevIndirect(GXTevStageID tev_stage, GXIndTexStageID ind_stage, GXIndTexFormat format,
                      GXIndTexBiasSel bias_sel, GXIndTexMtxID matrix_sel, GXIndTexWrap wrap_s,
                      GXIndTexWrap wrap_t, GXBool add_prev, GXBool utc_lod,
                      GXIndTexAlphaSel alpha_sel);
void GXSetIndTexCoordScale(GXIndTexStageID ind_state, GXIndTexScale scale_s, GXIndTexScale scale_t);
void GXSetNumIndStages(u8 nIndStages);
void GXSetTevDirect(GXTevStageID tev_stage);

/* GXDisplayList.c */
void GXCallDisplayList(void *list, u32 nbytes);

/* GXTev.c */
void GXSetTevColorIn(GXTevStageID stage, GXTevColorArg a, GXTevColorArg b, GXTevColorArg c,
                     GXTevColorArg d);
void GXSetTevAlphaIn(GXTevStageID stage, GXTevAlphaArg a, GXTevAlphaArg b, GXTevAlphaArg c,
                     GXTevAlphaArg d);
void GXSetTevColorOp(GXTevStageID stage, GXTevOp op, GXTevBias bias, GXTevScale scale,
                     GXBool clamp, GXTevRegID out_reg);
void GXSetTevAlphaOp(GXTevStageID stage, GXTevOp op, GXTevBias bias, GXTevScale scale,
                     GXBool clamp, GXTevRegID out_reg);
void GXSetTevColor(GXTevRegID id, GXColor color);
void GXSetTevColorS10(GXTevRegID id, GXColorS10 color);
void GXSetTevKColor(GXTevKColorID id, GXColor color);
void GXSetTevOp(GXTevStageID id, GXTevMode mode);
void GXSetTevKColorSel(GXTevStageID stage, GXTevKColorSel sel);
void GXSetTevKAlphaSel(GXTevStageID stage, GXTevKAlphaSel sel);
void GXSetTevSwapMode(GXTevStageID stage, GXTevSwapSel ras_sel, GXTevSwapSel tex_sel);
void GXSetTevSwapModeTable(GXTevSwapSel table, GXTevColorChan red, GXTevColorChan green,
                           GXTevColorChan blue, GXTevColorChan alpha);
void GXSetAlphaCompare(GXCompare comp0, u8 ref0, GXAlphaOp op, GXCompare comp1, u8 ref1);
void GXSetZTexture(int op, GXTexFmt fmt, u32 bias);
void GXSetTevOrder(GXTevStageID stage, GXTexCoordID coord, GXTexMapID map, GXChannelID color);
void GXSetNumTevStages(u8 nStages);

/* GXPixel.c */
void GXSetFog(GXFogType type, f32 startz, f32 endz, f32 nearz, f32 farz, GXColor color);
void GXSetFogRangeAdj(GXBool enable, u16 center, const void *table);
void GXSetBlendMode(GXBlendMode type, GXBlendFactor src_factor, GXBlendFactor dst_factor,
                    GXLogicOp op);
void GXSetColorUpdate(GXBool update_enable);
void GXSetAlphaUpdate(GXBool update_enable);
void GXSetZMode(GXBool compare_enable, GXCompare func, GXBool update_enable);
void GXSetZCompLoc(GXBool before_tex);
void GXSetPixelFmt(GXPixelFmt pix_fmt, GXZFmt16 z_fmt);
void GXSetDither(GXBool dither);
void GXSetDstAlpha(GXBool enable, u8 alpha);
void GXSetFieldMask(GXBool odd_mask, GXBool even_mask);
void GXSetFieldMode(GXBool field_mode, GXBool half_aspect_ratio);

/* GXTransform.c */
void GXSetProjectionv(const f32 *ptr);
void GXLoadPosMtxImm(const Mtx mtx, u32 id);
void GXLoadNrmMtxImm(const Mtx mtx, u32 id);
void GXSetCurrentMtx(u32 id);
void GXLoadTexMtxImm(const Mtx mtx, u32 id, GXTexMtxType type);
void GXSetViewport(f32 left, f32 top, f32 wd, f32 ht, f32 nearz, f32 farz);
void GXSetScissor(u32 left, u32 top, u32 wd, u32 ht);
void GXSetScissorBoxOffset(s32 x_off, s32 y_off);
void GXSetClipMode(GXClipMode mode);

/* GXPerf.c */
void GXSetGPMetric(GXPerf0 perf0, GXPerf1 perf1);
void GXClearGPMetric(void);

#ifdef __cplusplus
}
#endif

#endif
