/* RenderWare lifecycle, stream state, and packed texture sizing, 0x80493E8C..0x8049572B.
 * Names are inferred from behavior; definitions remain in retail address order. */
#include <string.h>
#include <dolphin/types.h>

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
extern s32 lbl_805F161C;
extern s32 lbl_805F1610;
extern s32 lbl_805F1614;
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
extern u32 lbl_805E31CC[];
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
void fn_803961E4(u32, u32, u32, u32);
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
void fn_80498E08(void *, void *, void *);
void fn_8039616C(void *, u32);
void fn_803944A4(void);
void *RwFrameGetLTM(void *);
void fn_8047F0B0(void *, void *);
void fn_8047F72C(void *, void *, u32);
void fn_80399114(void *);
void fn_803991E8(void *, u32);
void fn_80399238(void *, u32);
void fn_80399288(u32);
void fn_803961E4(u32, u32, u32, u32);
void fn_80395E44(u32, u32, u32, u32);
void fn_80395EF4(u32, u32, void *, u32);
void fn_80396568(void *, u32);
void fn_8039575C(void);
void fn_80397774(void);
void fn_80397648(void);
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
u32 fn_80495688(u32 *, u32 *, u32);
s32 fn_80495428(u32 *, u32);

#define RW_API(type, offset) ((type)*(u32 *)(lbl_805F15B4 + (offset)))

#define RW_ERROR(value)                                                                          \
    do {                                                                                         \
        u32 error[2];                                                                            \
        error[0] = 1;                                                                            \
        error[1] = fn_8047DC28(value);                                                           \
        fn_8047DB84(error);                                                                      \
    } while (0)

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
        fn_803961E4(lbl_805F164C[0x19], (u32)(lbl_805F164C + 0x1A), 1, (u32)(lbl_805F164C + 0x32));
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

u32 fn_804945E8(u8 *object, u32 *params, u32 mode) {
    if (lbl_805F15E0 == 0) {
        u32 *viewport = *(u32 **)(object + 0x60);
        u32 local[4];
        local[0] = *(u16 *)(viewport + 7);
        local[1] = *(u16 *)((u8 *)viewport + 0x1E);
        local[2] = viewport[3];
        local[3] = viewport[4];
        if (lbl_805F15F0 != 0) {
            local[1] <<= 1;
            local[3] <<= 1;
        }
        fn_80498E08(viewport, local, params);
    } else if ((mode & 1) != 0) {
        u32 value = params[0];
        fn_8039616C(&value, 0xFFFFFF);
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

void fn_804950AC(void *frame, u32 flags) {
    f32 matrix[16];
    if (frame == NULL) {
        memcpy(matrix, lbl_805E31CC, sizeof(matrix));
    } else {
        fn_8047EBB8(matrix, frame, lbl_805E31CC);
    }
    for (u32 i = 0; i < 4; i++) {
        matrix[i] = -matrix[i];
        matrix[i + 8] = -matrix[i + 8];
    }
    fn_803991E8(matrix, 0);
    if (flags != 0) fn_80399238(matrix, 0);
    fn_80399288(0);
}

void fn_804951B8(u32 *object, u32 mode) {
    u32 *viewport = (u32 *)object[0];
    fn_803961E4(0, 0, 0, 0);
    if (mode == 0) {
        fn_80395E44(*(u16 *)(object + 7), *(u16 *)((u8 *)object + 0x1E), object[3] & 0xFFFF, object[4] & 0xFFFF);
    } else {
        fn_80395E44(((s32)*(s16 *)(object + 7) & 0x7FFF) << 1,
                    ((s32)*(s16 *)((u8 *)object + 0x1E) & 0x7FFF) << 1,
                    (object[3] & 0x7FFF) << 1, (object[4] & 0x7FFF) << 1);
    }
    fn_80395EF4(viewport[3] & 0xFFFF, viewport[4] & 0xFFFF, (void *)(viewport[0] + lbl_805F1658 + 0xC), mode & 0xFF);
    s32 offset;
    switch (object[5]) {
    case 4:
        offset = (s32)(*(s16 *)(object + 7) * 8 + ((viewport[3] + 7) & 0xFFFFFFF8) * *(s16 *)((u8 *)object + 0x1E)) >> 1;
        break;
    case 8:
        offset = *(s16 *)(object + 7) * 4 + ((viewport[3] + 7) & 0xFFFFFFF8) * *(s16 *)((u8 *)object + 0x1E);
        break;
    case 0x10:
        offset = (*(s16 *)(object + 7) * 4 + ((viewport[3] + 3) & 0xFFFFFFFC) * *(s16 *)((u8 *)object + 0x1E)) * 2;
        break;
    case 0x20:
        offset = (*(s16 *)(object + 7) * 4 + ((viewport[3] + 3) & 0xFFFFFFFC) * *(s16 *)((u8 *)object + 0x1E)) * 4;
        break;
    default:
        RW_ERROR(0x8000000C);
        return;
    }
    fn_80396568((void *)(viewport[0] + lbl_805F1658 + 0x1C + offset), lbl_805F15E0 & 0xFF);
    fn_8039575C();
    fn_803961E4(lbl_805F164C[0x19], (u32)(lbl_805F164C + 0x1A), 1, (u32)(lbl_805F164C + 0x32));
    if (viewport[0xB] == 0) fn_80397774();
    else fn_80397648();
}

void fn_804953C0(u32 *a, u32 *b) {
    do {
        do {
        } while (lbl_805F161C == 1);
    } while (lbl_805F1610 != lbl_805F1614);
    *a = lbl_805F1604;
    *b = lbl_805F1600;
}

void fn_804953F0(void) {
    lbl_805F1658 = fn_80489C54(0x34, 0x40C, 0, 0, 0);
}

s32 fn_80495428(u32 *object, u32 shift) {
    u32 *viewport = (u32 *)object[0];
    u32 width;
    u32 height;
    if ((object[0x22] & 6) == 0) {
        width = viewport[3] >> shift;
        height = viewport[4] >> shift;
        if (width == 0) width = 1;
        if (height == 0) height = 1;
    } else {
        width = viewport[3];
        height = viewport[4];
    }
    switch (object[5]) {
    case 4: return (s32)(((width + 7) & 0xFFFFFFF8) * ((height + 7) & 0xFFFFFFF8)) >> 1;
    case 8: return ((width + 7) & 0xFFFFFFF8) * ((height + 3) & 0xFFFFFFFC);
    case 0x10: return ((width + 3) & 0xFFFFFFFC) * ((height + 3) & 0xFFFFFFFC) * 2;
    case 0x20: return ((width + 3) & 0xFFFFFFFC) * ((height + 3) & 0xFFFFFFFC) * 4;
    default:
        RW_ERROR(0x8000000C);
        return 0;
    }
}

s32 fn_80495558(u32 *object) {
    u32 info[6];
    s32 total = 0;
    fn_80495688(info, object, 0);
    while (info[0] != 0) {
        info[0]--;
        total += fn_80495428(object, info[0] & 0xFF);
    }
    return total;
}

u32 fn_804955CC(u32 *object, u32 shift) {
    u32 width = object[3] >> shift;
    if (width == 0) width = 1;
    switch (object[5]) {
    case 4: return (width + 7) >> 1;
    case 8: return width + 7;
    case 0x10: return (width + 3) * 2;
    case 0x20: return (width + 3) * 4;
    default:
        RW_ERROR(0x8000000C);
        return 0;
    }
}

u32 fn_80495688(u32 *out, u32 *object, u32 unused) {
    u8 value = *(u8 *)(object[0] + lbl_805F1658 + 0x32);
    if (value == 0xFF) {
        if ((object[0x23] & 0x80) == 0) {
            out[0] = 1;
        } else {
            out[0] = (fn_80491AB4(object[4] > object[3] ? object[3] : object[4]) + 1) & 0xFF;
        }
    } else {
        out[0] = value + 1;
    }
    return 1;
}

void fn_8049572C(void *dest, u32 kind, u8 *source, u32 count, u32 stride, u32 rows) {
    u32 offset = 0;
    switch (kind - 4) {
    case 0:
        for (u32 row = 0; row < rows; row++) {
            for (u32 x = 0; x < count; x += 8) {
                memcpy(dest, source + offset, 4);
                offset += 4;
            }
            offset += stride;
        }
        break;
    case 4:
        for (u32 row = 0; row < rows; row++) {
            for (u32 x = 0; x < count; x += 8) {
                memcpy((u8 *)dest + (row & 3) * 8 + ((count + 7 >> 3) * (row >> 2) + x) * 0x20,
                       source + offset, 8);
                offset += 8;
            }
            offset += stride;
        }
        break;
    case 0xC:
        for (u32 row = 0; row < rows; row++) {
            for (u32 x = 0; x < count; x += 4) {
                memcpy((u8 *)dest + ((row & 3) * 4 + ((count + 3 >> 2) * (row >> 2) + x) * 0x10) * 2,
                       source + offset, 8);
                offset += 8;
            }
            offset += stride;
        }
        break;
    case 0x1C:
        for (u32 row = 0; row < rows; row++) {
            u32 block = 0;
            for (u32 x = 0; x < count; x++) {
                if ((x & 3) == 0) block = ((count + 3 >> 2) * (row >> 2) + (x >> 2)) * 0x10;
                u32 target = block * 4 + (row & 3) * 4 + (x & 3) * 2;
                *(u16 *)((u8 *)dest + target) = *(u16 *)(source + offset + x * 4);
                *(u16 *)((u8 *)dest + target + 0x20) = *(u16 *)(source + offset + x * 4 + 2);
            }
            offset += stride;
        }
        break;
    default:
        RW_ERROR(0x8000000C);
        break;
    }
}
}
