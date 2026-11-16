#ifndef DOLPHIN_GX_GXLINKNAMES_H
#define DOLPHIN_GX_GXLINKNAMES_H

/* Link names for SDK functions and objects that other units of this project (the game's
 * RenderWare and system code) still reference by their placeholder names. The GX sources use the
 * SDK names; these map them onto the symbols the rest of the build links against. Drop an entry
 * once every reference to the placeholder has been renamed. */

#define GXBegin fn_80395B28
#define GXCallDisplayList fn_80399000
#define GXClearVtxDesc fn_8039494C
#define GXCopyDisp fn_80396400
#define GXCopyTex fn_80396568
#define GXDisableBreakPt fn_80394228
#define GXDrawDone fn_803956DC
#define GXEnableBreakPt fn_80394174
#define GXFlush fn_8039528C
#define GXGetCPUFifo fn_803944F0
#define GXGetFifoPtrs fn_80394098
#define GXGetProjectionv fn_803991A0
#define GXInitLightColor fn_803969E4
#define GXInitLightDir fn_803969C8
#define GXInitLightDistAttn fn_803968E8
#define GXInitLightPos fn_803969B8
#define GXInitLightSpot fn_80396758
#define GXInitTexObj fn_80396DF0
#define GXInitTexObjLOD fn_80397084
#define GXInitTlutObj fn_8039744C
#define GXInvalidateTexAll fn_80397774
#define GXInvalidateTexRegion fn_80397648
#define GXInvalidateVtxCache fn_80394F2C
#define GXLoadLightObjImm fn_803969F0
#define GXLoadNrmMtxImm fn_80399238
#define GXLoadPosMtxImm fn_803991E8
#define GXLoadTexMtxImm fn_803992BC
#define GXLoadTexObj fn_803973F8
#define GXMpal480IntDf lbl_80567838
#define GXNtsc480IntDf lbl_805677C0
#define GXNtsc480Prog lbl_805677FC
#define GXPal528IntDf lbl_80567874
#define GXPixModeSync fn_8039575C
#define GXReadDrawSync fn_803956D0
#define GXSetAlphaCompare fn_80398734
#define GXSetAlphaUpdate fn_80398D80
#define GXSetBlendMode fn_80398D00
#define GXSetBreakPtCallback fn_80394130
#define GXSetChanAmbColor fn_80396A6C
#define GXSetChanCtrl fn_80396C78
#define GXSetChanMatColor fn_80396B54
#define GXSetColorUpdate fn_80398D54
#define GXSetCopyClamp fn_80396048
#define GXSetCopyClear fn_8039616C
#define GXSetCopyFilter fn_803961E4
#define GXSetCullMode fn_80395D48
#define GXSetCurrentGXThread fn_803944A4
#define GXSetCurrentMtx fn_80399288
#define GXSetDispCopyDst fn_80395EC0
#define GXSetDispCopyGamma fn_803963EC
#define GXSetDispCopySrc fn_80395DC8
#define GXSetDispCopyYScale fn_803960A0
#define GXSetDrawSync fn_8039561C
#define GXSetFieldMode fn_80398F88
#define GXSetNumChans fn_80396C3C
#define GXSetNumTevStages fn_803989A0
#define GXSetNumTexGens fn_803951BC
#define GXSetPixelFmt fn_80398E14
#define GXSetProjection fn_80399070
#define GXSetProjectionv fn_80399114
#define GXSetScissor fn_803994A0
#define GXSetScissorBoxOffset fn_80399518
#define GXSetTevOp fn_803982A8
#define GXSetTevOrder fn_80398804
#define GXSetTexCoordGen2 fn_80394F3C
#define GXSetTexCopyDst fn_80395EF4
#define GXSetTexCopySrc fn_80395E44
#define GXSetViewport fn_80399458
#define GXSetViewportJitter fn_80399400
#define GXSetVtxAttrFmt fn_80394984
#define GXSetVtxDesc fn_80394500
#define GXSetZCompLoc fn_80398DE0
#define GXSetZMode fn_80398DAC
#define OSGetCurrentThread fn_80377644
#define OSResumeThread fn_80378084
#define OSSleepThread fn_8037847C
#define OSWakeupThread fn_80378568

/* MSL math, not split yet. */
#define cosf fn_803B1F24

#endif
