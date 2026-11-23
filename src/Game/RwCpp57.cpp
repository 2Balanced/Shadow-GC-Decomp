/* RenderWare GameCube driver, 0x80493E8C..0x804959BF: the rest of the device file (its standard
 * function dispatcher, start/stop, the frame swap, camera clear and the view matrix, the copy of
 * the frame into a camera texture) and, from 0x804953F0, the start of the raster file (the raster
 * plugin, texture sizes in GX's tiled layout, mip levels, the tiling copy; its .data starts with
 * fn_80495428's jump table at 0x8056E4F8). Names are inferred from behaviour; definitions are in
 * retail address order.
 *
 * Still first drafts (far from retail): fn_80493E8C, fn_804946A4 and fn_80494C6C.
 * NonMatching: fn_8049572C (72 rows, saved registers). The rest match. */
#include <string.h>
#include <dolphin/types.h>
#include "Game/rw/r7.h"

extern "C" {
extern u8 *lbl_805F15B4;
extern s32 lbl_805F15E4;
extern s32 lbl_805F15EC;
extern s32 lbl_805F15E0;
extern s32 lbl_805F15E8;
extern s32 lbl_805F15F0;
extern s32 lbl_805F1650;
extern s32 lbl_805F1658;
extern s32 lbl_805F15F4;
extern s32 lbl_805F1640;
extern s32 lbl_805F1648;
extern s32 lbl_805F1644;
extern s32 lbl_805F162C;
extern s32 lbl_805F15F8;
extern s32 lbl_805F15FC;
extern s32 lbl_805F1600;
extern s32 lbl_805F1604;
extern volatile RwInt32 lbl_805F161C;  /* a frame is ready for the retrace handler */
extern volatile RwInt32 lbl_805F1610;  /* queue index of the last frame queued */
extern volatile RwInt32 lbl_805F1614;  /* queue index of the frame to show next */
extern s32 lbl_805F1608;
extern s32 lbl_805F160C;
extern s32 lbl_805F1618;
extern s32 lbl_805F1620;
extern s32 lbl_805F1624;
extern s32 lbl_805F1628;
extern s32 lbl_805F1630;
extern s32 lbl_805EDF34;
extern s32 lbl_805EDF2C;
extern s32 lbl_805EDF28;
extern s32 lbl_805EDF30;
extern s32 lbl_805EDF38;
extern s32 lbl_805EDF39;
extern s32 lbl_805EDF40;
extern u8 *lbl_805F164C;
extern u32 lbl_805E3178[];
extern u16 lbl_805F1638[];
extern RwDlMatrix lbl_805E31CC; /* the driver's axis flip, times the camera matrix */
extern u32 lbl_8056E348[];
extern u32 lbl_8056E34C;
extern u32 lbl_8056E350;
extern u32 lbl_8056E354;
extern u32 lbl_8056E358;
extern u32 lbl_8056E35C;
extern u32 lbl_8056E360;
extern u32 lbl_8056E364;
extern u32 lbl_8056E368;
extern u32 lbl_8056E36C;
extern u32 lbl_8056E370;
extern u32 lbl_8056E374;
extern u32 lbl_8056E378;
extern u32 lbl_8056E37C;
extern u32 lbl_8056E380;
extern u32 lbl_8056E384;
extern u32 lbl_8056E388;
extern u32 lbl_8056E38C;
extern u32 lbl_8056E390;
extern u32 lbl_8056E394;
extern u32 lbl_8056E398;
extern u32 lbl_8056E39C;
extern u32 lbl_8056E3A0;
extern u32 lbl_8056E3A4;
extern u32 lbl_8056E44C[14];
extern u32 lbl_8056E430[];
extern u32 lbl_8056E434;
extern u32 lbl_8056E43C;
extern u32 lbl_8056E444;
extern u32 lbl_8056E448;
extern u32 DAT_805e31d8;
extern u32 DAT_805f1654;
extern f32 lbl_805FB3B0;
extern f32 lbl_805FB3B4;
extern f32 lbl_805FB3C0;

u32 fn_8047DC28(u32, ...);
void fn_8047DB84(void *);
void fn_80493C84(void *, u32);
u32 fn_80493C7C(void);
void fn_8039528C(void);
void DCInvalidateRange(void *, u32);
u32 GXInit(void *, u32);
void GXInitFifoBase(void *, void *, u32);
void GXSetCPUFifo(void *);
void GXSetGPFifo(void *);
s16 fn_803956D0(void);
void fn_8039561C(s16);
void fn_8037E354(u8 *);
void fn_8037F094(s32);
void fn_8037EEF0(void);
u32 fn_8037F1BC(void);
void fn_80399400(u32);
void fn_80399458(void);
void fn_803994A0(u32, u32, ...);
void fn_80399518(u32, u32);
void fn_80395DC8(u32, u32, ...);
void fn_80395EC0(u32, u32);
void fn_803961E4(u8 aa, const u8 (*samplePattern)[2], u8 vf, const u8 *vfilter); /* GXSetCopyFilter */
void fn_803960A0(void);
void fn_80398F88(u32, u32);
void fn_80398E14(u32, u32);
void fn_8037DE8C(void);
void fn_803963EC(u32);
void fn_8037D6A8(void *);
void fn_8037D6EC(void *);
void fn_80394130(void *);
void OSInitThreadQueue(void *);
void fn_80493ABC(void);
void fn_80493B98(void);
void fn_804939DC(void);
void fn_80497198(void);
void fn_8049742C(void);
RwBool fn_80498E08(RwDlRaster *raster, RwDlRect *rect, void *color, RwInt32 clearMode);
void fn_8039616C(void *, u32);
void fn_803944A4(void);
void *RwFrameGetLTM(void *);
void fn_8047F0B0(void *, void *);
void fn_8047F72C(void *, void *, u32);
void fn_80399114(void *);
void fn_803991E8(void *, u32);
void fn_80399238(void *, u32);
void fn_80399288(u32);
void fn_80395E44(u16 left, u16 top, u16 wd, u16 ht);        /* GXSetTexCopySrc */
void fn_80395EF4(u16 wd, u16 ht, int fmt, u8 mipmap);       /* GXSetTexCopyDst */
void fn_80396568(void *dest, u8 clear);                     /* GXCopyTex */
void fn_8039575C(void);                                     /* GXPixModeSync */
void fn_80397774(void);                                     /* GXInvalidateTexAll */
void fn_80397648(void *region);                             /* GXInvalidateTexRegion */
u32 fn_8037847C(void *);
u32 OSDisableInterrupts(void);
void OSRestoreInterrupts(u32);
u32 fn_803944F0(void);
void fn_80394098(u32, void *, void *);
void fn_80394174(u32);
void fn_80396400(void *, u32);
void fn_80396048(u32);
u32 fn_80489C54(u32, u32, u32, u32, u32);
void fn_8047EBB8(void *, void *, void *);
s32 fn_80491AB4(s32);
RwBool fn_80495688(void *out, void *inOut, RwInt32 num);
RwInt32 fn_80495428(RwDlRaster *raster, RwUInt8 level);

#define RW_API(type, offset) ((type)*(u32 *)(lbl_805F15B4 + (offset)))


u32 fn_80493E8C(u32 command, u32 *params, u32 *out, u32 arg) {
    switch (command) {
    case 0: {
        u32 *source = (u32 *)params[0];
        if (source == NULL) {
            fn_80493C84(0, lbl_805F15E4);
        } else {
            fn_80493C84((void *)source[0], source[1]);
            lbl_805EDF2C = source[2];
        }
        u32 fifoBytes = ((*(u16 *)(lbl_805F164C + 1) + 0xF & 0xFFF0) * *(u16 *)(lbl_805F164C + 2));
        lbl_805F15F4 = (s32)RW_API(void *(*)(u32, u32), 0x108)(lbl_805EDF2C + fifoBytes * 4 + 0x1F, 0x40411);
        lbl_805F1640 = (lbl_805F15F4 + 0x1F) & ~0x1F;
        DCInvalidateRange((void *)lbl_805F1640, lbl_805EDF2C);
        lbl_805F15F8 = lbl_805F1640 + lbl_805EDF2C;
        lbl_805F15FC = lbl_805F15F8 + fifoBytes * 2;
        lbl_805F1600 = lbl_805F15FC;
        lbl_805F1604 = lbl_805F15F8;
        return 1;
    }
    case 1:
        fn_8039528C();
        RW_API(void (*)(void *), 0x10C)((void *)lbl_805F15F4);
        lbl_805F1604 = 0;
        lbl_805F15F8 = 0;
        lbl_805F15FC = 0;
        lbl_805F1600 = 0;
        lbl_805F1640 = 0;
        lbl_805F15F4 = 0;
        return 1;
    case 2: {
        u8 fifo[0x80];
        if (lbl_805F162C == 0) {
            lbl_805F1644 = GXInit((void *)lbl_805F1640, lbl_805EDF2C);
            lbl_805F162C = 1;
        } else {
            GXInitFifoBase(fifo, (void *)lbl_805F1640, lbl_805EDF2C);
            GXSetCPUFifo(fifo);
            GXSetGPFifo(fifo);
            GXInitFifoBase((void *)lbl_805F1644, (void *)lbl_805F1640, lbl_805EDF2C);
            GXSetCPUFifo((void *)lbl_805F1644);
            GXSetGPFifo((void *)lbl_805F1644);
        }
        s16 current;
        while ((current = fn_803956D0()) != *(s16 *)(lbl_805F164C + 0x1C)) fn_8039561C(*(s16 *)(lbl_805F164C + 0x1C));
        fn_8037E354(lbl_805F164C);
        fn_8037F094(1);
        fn_8037EEF0();
        if (lbl_805F164C[0x18] == 0) fn_80399458();
        else fn_80399400(fn_8037F1BC() ^ 1);
        fn_803994A0(0, 0, *(u16 *)(lbl_805F164C + 1), *(u16 *)(lbl_805F164C + 3));
        fn_80399518(0, 0);
        fn_80395DC8(0, 0, *(u16 *)(lbl_805F164C + 1), *(u16 *)(lbl_805F164C + 3));
        fn_80395EC0(*(u16 *)(lbl_805F164C + 1), *(u16 *)(lbl_805F164C + 2));
        fn_803961E4(lbl_805F164C[0x19], (const u8 (*)[2])(lbl_805F164C + 0x1A), 1, lbl_805F164C + 0x32);
        fn_803960A0();
        if (lbl_805F164C[0x19] == 0 || *(s16 *)(lbl_805F164C + 2) != *(s16 *)(lbl_805F164C + 4)) {
            lbl_805F15F0 = 0;
        } else {
            lbl_805F1648 = *(u16 *)(lbl_805F164C + 2) >> 1;
            lbl_805F15F0 = 1;
            lbl_805EDF28 = 1;
        }
        fn_80398F88(*(u16 *)(lbl_805F164C + 3),
                    (*(u16 *)(lbl_805F164C + 2) - *(u16 *)(lbl_805F164C + 4)) >> 31);
        fn_80398E14(lbl_805F15E4, 0);
        lbl_805EDF30 = 1;
        fn_8037DE8C();
        if ((*(u8 *)lbl_805F164C & 1) != 0) fn_8037DE8C();
        fn_803963EC(0);
        lbl_805EDF38 = lbl_805EDF39;
        fn_8037D6A8(fn_80493ABC);
        fn_8037D6EC(fn_80493B98);
        fn_80394130(fn_804939DC);
        OSInitThreadQueue(&lbl_805F1630);
        return 1;
    }
    case 3:
        do {
            do {
            } while (lbl_805F161C == 1);
        } while (lbl_805F1610 != lbl_805F1614);
        return 1;
    case 4:
        for (u32 i = 0; i < 14; i++) out[i] = lbl_8056E44C[i];
        DAT_805f1654 = (u32)params;
        return 1;
    case 5:
        out[0] = 1;
        return 1;
    case 6:
        if ((s32)arg > 0) return 0;
        if (lbl_805F15EC == 0x2A) {
            out[0] = lbl_8056E390;
            out[1] = lbl_8056E394;
            out[2] = lbl_8056E398;
            out[3] = lbl_8056E39C;
            out[4] = lbl_8056E3A0;
            out[5] = lbl_8056E3A4;
        } else if (lbl_805F15EC == 5 || lbl_805F15EC == 1) {
            out[0] = lbl_8056E360;
            out[1] = lbl_8056E364;
            out[2] = lbl_8056E368;
            out[3] = lbl_8056E36C;
            out[4] = lbl_8056E370;
            out[5] = lbl_8056E374;
        } else if (lbl_805F15EC < 3 && lbl_805F15EC >= 0) {
            out[0] = lbl_8056E348[0];
            out[1] = lbl_8056E34C;
            out[2] = lbl_8056E350;
            out[3] = lbl_8056E354;
            out[4] = lbl_8056E358;
            out[5] = lbl_8056E35C;
        } else {
            out[0] = lbl_8056E378;
            out[1] = lbl_8056E37C;
            out[2] = lbl_8056E380;
            out[3] = lbl_8056E384;
            out[4] = lbl_8056E388;
            out[5] = lbl_8056E38C;
        }
        return 1;
    case 7:
        return (s32)(((arg ^ 1) >> 1) - ((arg ^ 1) & 1)) >> 31;
    case 8:
    case 9:
        return 1;
    case 10:
        out[0] = 0;
        return 1;
    case 0xB:
        for (u32 i = 0; i < arg; i++) out[i] = fn_80493C7C();
        return 1;
    case 0x11:
        fn_80497198();
        return 1;
    case 0x12:
        fn_8049742C();
        return 1;
    case 0x13:
        out[0] = 0x400;
        return 1;
    case 0x16:
        *(u16 *)out = 3;
        return 1;
    default:
        return 0;
    }
}

/* Driver standard function: clears the camera's raster (RwCameraClear). Inside a frame it draws
 * the clear quad over the raster (fn_80498E08); between frames it sets the copy clear colour. */
RwBool fn_804945E8(void *out, void *inOut, RwInt32 num) {
    RwDlCamera *camera = (RwDlCamera *)out;
    RwDlRect rect;
    RwDlColor color;
    if (lbl_805F15E0 == 0) {
        RwDlRaster *raster = camera->frameBuffer;
        if (lbl_805F15F0 == 0) {
            rect.x = raster->nOffsetX;
            rect.y = raster->nOffsetY;
            rect.w = raster->width;
            rect.h = raster->height;
        } else {
            rect.x = raster->nOffsetX;
            rect.y = raster->nOffsetY * 2;
            rect.w = raster->width;
            rect.h = raster->height * 2;
        }
        fn_80498E08(raster, &rect, inOut, num);
    } else if (num & 1) {
        color = *(RwDlColor *)inOut;
        fn_8039616C(&color, 0xFFFFFF); /* GXSetCopyClear */
    }
    return 1;
}

u32 fn_804946A4(u8 *object) {
    fn_803944A4();
    u32 local[6] = {0, lbl_805FB3B4, lbl_805FB3B0, lbl_805FB3B0, lbl_805FB3B0, lbl_805FB3B4};
    lbl_805F1650 = (s32)object;
    DAT_805e31d8 = 0x20003;
    fn_8047F0B0(&lbl_805E31CC, RwFrameGetLTM(*(void **)(object + 4)));
    fn_8047F72C(&lbl_805E31CC, local, 2);
    lbl_8056E434 = *(u32 *)(object + 0x70);
    lbl_8056E43C = *(u32 *)(object + 0x74);
    if (*(u32 *)(object + 0x14) == 2) {
        lbl_8056E430[0] = lbl_805FB3B4;
        lbl_8056E444 = lbl_805FB3C0 / (*(f32 *)(object + 0x84) - *(f32 *)(object + 0x80));
        lbl_8056E448 = *(f32 *)(object + 0x84) * lbl_8056E444;
    } else {
        lbl_8056E430[0] = lbl_805FB3B0;
        lbl_8056E444 = -*(f32 *)(object + 0x80) / (*(f32 *)(object + 0x84) - *(f32 *)(object + 0x80));
        lbl_8056E448 = *(f32 *)(object + 0x84) * lbl_8056E444;
    }
    fn_80399114(&lbl_8056E430);
    u32 *viewport = *(u32 **)(object + 0x60);
    if ((*(u8 *)(viewport + 8) & 5) == 0) {
        if (lbl_805F15E8 != lbl_805F15E4) {
            fn_80398E14(lbl_805F15E4, 0);
            lbl_805F15E8 = lbl_805F15E4;
        }
        if (lbl_805F164C[0x18] == 0) fn_80399458();
        else fn_80399400(fn_8037F1BC() ^ 1);
        fn_803994A0((s32)*(s16 *)(viewport + 7), (s32)*(s16 *)((u8 *)viewport + 0x1E), viewport[3], viewport[4]);
    } else {
        if ((*(u32 *)(*viewport + lbl_805F1658 + 0x14) & 1) == 0) {
            if (lbl_805F15E8 != 0) {
                fn_80398E14(0, 0);
                lbl_805F15E8 = 0;
            }
        } else if (lbl_805F15E8 != 1) {
            fn_80398E14(1, 0);
            lbl_805F15E8 = 1;
        }
        fn_80399458();
        fn_803994A0((s32)*(s16 *)(viewport + 7), (s32)*(s16 *)((u8 *)viewport + 0x1E), viewport[3], viewport[4]);
        fn_80399518(0, 0);
    }
    return 1;
}

u32 fn_80494C5C(void) {
    lbl_805F1650 = 0;
    return 1;
}

u32 fn_80494C6C(void) {
    u32 interrupt = OSDisableInterrupts();
    s32 pending = (lbl_805F160C - lbl_805F1608 == -1) || (lbl_805EDF34 - 1 == lbl_805F160C - lbl_805F1608);
    if (-pending < 0) fn_8037847C(&lbl_805F1630);
    OSRestoreInterrupts(interrupt);
    if (lbl_805F15F0 == 0) {
        fn_8039528C();
        u32 stream = fn_803944F0();
        u8 scratch[0x10];
        u32 value;
        fn_80394098(stream, scratch, &value);
        interrupt = OSDisableInterrupts();
        u32 current = lbl_805F1600;
        u32 next = (lbl_805F160C + 1) / lbl_805EDF34;
        lbl_805F1638[lbl_805F1610] = (u16)lbl_805EDF40;
        lbl_805E3178[lbl_805F160C * 2 + 1] = value;
        lbl_805E3178[lbl_805F160C * 2] = current;
        lbl_805F160C = (lbl_805F160C + 1) - next * lbl_805EDF34;
        lbl_805F1610 = (lbl_805F1610 + 1) - ((lbl_805F1610 + 1) / lbl_805EDF34) * lbl_805EDF34;
        OSRestoreInterrupts(interrupt);
        if (lbl_805F1618 == 0) {
            lbl_805F1618 = 1;
            fn_80394174(value);
        }
        fn_80396400((void *)lbl_805F1600, lbl_805F15E0 & 0xFF);
        fn_8039561C((s16)lbl_805EDF40);
        lbl_805EDF40 = (s16)(lbl_805EDF40 + 1) + (s16)((lbl_805EDF40 + 1) / 0xE000) * 0x2000;
        fn_8039528C();
        bool same = lbl_805F1600 == lbl_805F15F8;
        lbl_805F1600 = lbl_805F15F8;
        if (same) lbl_805F1600 = lbl_805F15FC;
    } else if (lbl_805EDF28 == 0) {
        lbl_805EDF28 = 1;
    } else {
        lbl_805EDF28 = 0;
    }
    return 1;
}

void *fn_804950A0(void) {
    return lbl_8056E44C;
}

/* Loads the view matrix into GX: the camera frame's matrix times the driver's axis flip
 * (lbl_805E31CC), inverted into GX's conventions (x and z negated), or the flip alone. */
void fn_804950AC(void *frameMatrix, RwBool normals) {
    RwDlMatrix view;
    f32 mtx[3][4];
    RwDlMatrix *m;
    if (frameMatrix != NULL) {
        view.flags = 0x20003;
        fn_8047EBB8(&view, frameMatrix, &lbl_805E31CC);
        m = &view;
    } else {
        m = &lbl_805E31CC;
    }
    mtx[0][0] = -m->right.x;
    mtx[0][1] = -m->up.x;
    mtx[0][2] = -m->at.x;
    mtx[0][3] = -m->pos.x;
    mtx[1][0] = m->right.y;
    mtx[1][1] = m->up.y;
    mtx[1][2] = m->at.y;
    mtx[1][3] = m->pos.y;
    mtx[2][0] = -m->right.z;
    mtx[2][1] = -m->up.z;
    mtx[2][2] = -m->at.z;
    mtx[2][3] = -m->pos.z;
    fn_803991E8(mtx, 0);
    if (normals) fn_80399238(mtx, 0);
    fn_80399288(0);
}

/* Copies the embedded frame buffer into a camera texture raster: the raster's rectangle (twice
 * its size, box-filtered down, when the frame is drawn at double size) into the parent texture's
 * image at the raster's tile offset, then invalidates the texture cache. */
void fn_804951B8(RwDlRaster *raster, RwBool doubled) {
    RwDlRaster *parent = raster->parent;
    RwDlRasterExt *ext = (RwDlRasterExt *)((u8 *)parent + lbl_805F1658);
    RwInt32 offset;
    fn_803961E4(0, NULL, 0, NULL);
    if (doubled) {
        fn_80395E44(raster->nOffsetX * 2, raster->nOffsetY * 2, raster->width * 2,
                    raster->height * 2);
    } else {
        fn_80395E44(raster->nOffsetX, raster->nOffsetY, raster->width, raster->height);
    }
    fn_80395EF4(parent->width, parent->height, ext->texFormat, doubled);
    switch (parent->depth) {
    case 4:
        offset = (raster->nOffsetX * 8 + ((parent->width + 7) & ~7) * raster->nOffsetY) >> 1;
        break;
    case 8:
        offset = raster->nOffsetX * 4 + ((parent->width + 7) & ~7) * raster->nOffsetY;
        break;
    case 16:
        offset = (raster->nOffsetX * 4 + ((parent->width + 3) & ~3) * raster->nOffsetY) << 1;
        break;
    case 32:
        offset = (raster->nOffsetX * 4 + ((parent->width + 3) & ~3) * raster->nOffsetY) << 2;
        break;
    case 24:
    default:
        RW_ERROR(0x8000000C);
        return;
    }
    fn_80396568(ext->pixels + offset, lbl_805F15E0);
    fn_8039575C();
    fn_803961E4(lbl_805F164C[0x19], (const u8 (*)[2])(lbl_805F164C + 0x1A), 1, lbl_805F164C + 0x32);
    if (ext->region != NULL) {
        fn_80397648(ext->region);
    } else {
        fn_80397774();
    }
}

/* Waits until no frame is waiting for the retrace and the last queued one is on screen, then
 * returns the frame buffer shown and the one drawn to. */
void fn_804953C0(void **displayed, void **drawn) {
    while (lbl_805F161C == 1 || lbl_805F1610 != lbl_805F1614) {
    }
    *displayed = (void *)lbl_805F1604;
    *drawn = (void *)lbl_805F1600;
}

void fn_804953F0(void) {
    lbl_805F1658 = fn_80489C54(0x34, 0x40C, 0, 0, 0);
}

/* Bytes of one mip level of a texture raster in GX's tiled layout (tiles of 8x8 texels at 4 bits,
 * 8x4 at 8 bits, 4x4 at 16 and 32 bits), rounded up to 32 bytes. A locked raster is measured
 * at its full size. */
RwInt32 fn_80495428(RwDlRaster *raster, RwUInt8 level) {
    RwDlRaster *parent = raster->parent;
    RwUInt32 width;
    RwUInt32 height;
    RwUInt32 size;
    if (raster->privateFlags & 6) {
        width = parent->width;
        height = parent->height;
    } else {
        width = parent->width >> level;
        height = parent->height >> level;
        width = width ? width : 1;
        height = height ? height : 1;
    }
    switch (raster->depth) {
    case 4:
        size = (((width + 7) & ~7) * ((height + 7) & ~7)) >> 1;
        break;
    case 8:
        size = ((width + 7) & ~7) * ((height + 3) & ~3);
        break;
    case 16:
        size = (((width + 3) & ~3) * ((height + 3) & ~3)) << 1;
        break;
    case 32:
        size = (((width + 3) & ~3) * ((height + 3) & ~3)) << 2;
        break;
    case 24:
    default:
        RW_ERROR(0x8000000C);
        return 0;
    }
    return (size + 31) & ~31;
}


/* Bytes of all the mip levels of a texture raster. */
RwInt32 fn_80495558(RwDlRaster *raster) {
    RwInt32 numLevels;
    RwInt32 size = 0;
    fn_80495688(&numLevels, raster, 0);
    while (numLevels--) {
        size += fn_80495428(raster, numLevels);
    }
    return size;
}

/* Bytes of one row of tiles of a mip level of a texture raster. */
RwInt32 fn_804955CC(RwDlRaster *raster, RwUInt8 level) {
    RwInt32 w = raster->parent->width >> level;
    RwInt32 width = w ? w : 1;
    switch (raster->depth) {
    case 4:
        return ((width + 7) & ~7) >> 1;
    case 8:
        return (width + 7) & ~7;
    case 16:
        return ((width + 3) & ~3) << 1;
    case 32:
        return ((width + 3) & ~3) << 2;
    case 24:
    default:
        RW_ERROR(0x8000000C);
        return 0;
    }
}

/* Driver standard function: the number of mip levels of a raster (RwRasterGetNumLevels): the
 * count made for it, else down to 1x1 for a mip-mapped format, else 1. */
RwBool fn_80495688(void *out, void *inOut, RwInt32 num) {
    RwInt32 *numLevels = (RwInt32 *)out;
    RwDlRaster *raster = (RwDlRaster *)inOut;
    RwDlRasterExt *ext = (RwDlRasterExt *)((u8 *)raster->parent + lbl_805F1658);
    if (ext->mipLevels != 0xFF) {
        *numLevels = ext->mipLevels + 1;
    } else {
        RwUInt8 levels;
        if (RW_RASTER_FORMAT(raster) & 0x8000) {
            if (raster->width > raster->height) {
                levels = (RwUInt8)fn_80491AB4(raster->width) + 1;
            } else {
                levels = (RwUInt8)fn_80491AB4(raster->height) + 1;
            }
        } else {
            levels = 1;
        }
        *numLevels = levels;
    }
    return 1;
}

/* Converts a linear image into GX's tiled texture layout: 8x8 tiles at 4 bits, 8x4 at 8 bits,
 * 4x4 at 16 bits, and at 32 bits 4x4 tiles whose alpha/red and green/blue halves are 32 bytes
 * apart (the source pixels are already in that order).
 * NonMatching: the code is retail's, the saved registers of the four cases are not (72 rows).
 * The order in which locals are declared moves them (descending in the first case, ascending
 * in the others, which reuse its registers); no order tried gives all four cases. */
void fn_8049572C(u8 *dst, u8 *src, RwInt32 width, RwInt32 height, RwInt32 depth, RwInt32 stride) {
    switch (depth) {
    case 4: {
        RwInt32 offset;
        RwInt32 tile;
        RwInt32 srcRow;
        RwInt32 tilesPerRow;
        RwInt32 y;
        u8 *line;
        RwInt32 rowBase;
        RwInt32 lineOffset;
        RwInt32 x;
        tilesPerRow = ((width + 7) & ~7) >> 3;
        srcRow = 0;
        for (y = 0; y < height; y++) {
            rowBase = tilesPerRow * (y >> 3);
            line = src + srcRow;
            offset = 0;
            tile = 0;
            lineOffset = (y & 7) << 3;
            for (x = 0; x < width; x += 8) {
                memcpy(dst + ((RwUInt32)(((rowBase + tile) << 6) + lineOffset) >> 1), line + offset, 4);
                offset += 4;
                tile++;
            }
            srcRow += stride;
        }
        break;
    }
    case 8: {
        RwInt32 tilesPerRow;
        RwInt32 y;
        u8 *line;
        RwInt32 rowBase;
        RwInt32 lineOffset;
        RwInt32 x;
        RwInt32 tile;
        RwInt32 srcRow;
        tilesPerRow = ((width + 7) & ~7) >> 3;
        srcRow = 0;
        for (y = 0; y < height; y++) {
            line = src + srcRow;
            rowBase = tilesPerRow * (y >> 2);
            lineOffset = (y & 3) << 3;
            x = 0;
            tile = 0;
            for (; x < width; x += 8) {
                memcpy(dst + ((rowBase + tile) << 5) + lineOffset, line + x, 8);
                tile++;
            }
            srcRow += stride;
        }
        break;
    }
    case 16: {
        RwInt32 tilesPerRow;
        RwInt32 y;
        RwInt32 rowBase;
        RwInt32 lineOffset;
        RwInt32 x;
        u8 *line;
        RwInt32 tile;
        RwInt32 srcRow;
        tilesPerRow = ((width + 3) & ~3) >> 2;
        srcRow = 0;
        for (y = 0; y < height; y++) {
            rowBase = tilesPerRow * (y >> 2);
            lineOffset = (y & 3) << 2;
            line = src + srcRow;
            x = 0;
            tile = 0;
            for (; x < width; x += 4) {
                memcpy(dst + ((((rowBase + tile) << 4) + lineOffset) << 1), line, 8);
                line += 8;
                tile++;
            }
            srcRow += stride;
        }
        break;
    }
    case 32: {
        RwInt32 offset;
        RwInt32 srcRow;
        RwInt32 tilesPerRow;
        RwInt32 y;
        u8 *line;
        RwInt32 rowBase;
        RwInt32 lineOffset;
        RwInt32 x;
        RwInt32 tileOffset;
        tilesPerRow = ((width + 3) & ~3) >> 2;
        srcRow = 0;
        for (y = 0; y < height; y++) {
            rowBase = tilesPerRow * (y >> 2);
            line = src + srcRow;
            lineOffset = (y & 3) << 2;
            tileOffset = 0;
            offset = 0;
            for (x = 0; x < width; x++) {
                RwInt32 at;
                if ((x & 3) == 0) tileOffset = (rowBase + (x >> 2)) << 4;
                at = (tileOffset << 2) + ((lineOffset + (x & 3)) << 1);
                *(u16 *)(dst + at) = *(u16 *)(line + offset);
                *(u16 *)(dst + at + 0x20) = *(u16 *)(line + offset + 2);
                offset += 4;
            }
            srcRow += stride;
        }
        break;
    }
    case 24:
    default:
        RW_ERROR(0x8000000C);
        break;
    }
}
}
