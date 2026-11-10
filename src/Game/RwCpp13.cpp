/* RenderWare vertex conversion routines. The object and stream layouts are inferred from the
 * callers; the functions retain their address names until their interfaces are known. */
#include <dolphin/types.h>

extern u32 lbl_805FB0C0;
extern u8 lbl_805FB0C4;
extern u32 lbl_805FB0C8;
extern u8 lbl_805FB0CC;
extern u32 lbl_805FB0B8;
extern u8 lbl_805FB0BC;
extern u32 lbl_8056D708[];
extern u32 lbl_805EDDD8;

extern "C" {
void fn_80482458(void *, const void *, const void *);
void fn_80482504(void *, const void *, const void *);
void fn_80451080(void *, void *, void *);
void fn_804513C4(void *, void *, void *);
void fn_804517C8(void *, void *, void *);
void DCFlushRange(void *, u32);
void fn_80394F2C(void);
u32 fn_8044CCAC(u8 *);
void *fn_8044F9E4(void *, void *, void *);
void *fn_8046C0D4(void *, void *, void *);
void *fn_80450938(void *, void *, void *);
}

extern "C" {
static asm u32 RwCpp13ReadGqr5(void) {
    nofralloc
    mfspr r3, 917
    blr
}

static asm void RwCpp13WriteGqr5(register u32 value) {
    nofralloc
    mtspr 917, value
    blr
}

static asm void RwCpp13WriteGqr6(register u32 value) {
    nofralloc
    mtspr 918, value
    blr
}

static asm void RwCpp13WriteGqr7(register u32 value) {
    nofralloc
    mtspr 919, value
    blr
}
}

extern "C" void fn_8044D6E8(u32 *object, u8 *matrices, void *arg3, void *arg4, u8 *outA,
                            u8 *outB, u8 *format, u32 count) {
    u8 local[0x80];
    *(u32 *)(local + 0x24) = (u32)outA;
    *(u32 *)(local + 0x28) = (u32)outB;
    *(u32 *)(local + 0x2C) = (u32)arg3;
    *(u32 *)(local + 0x30) = (u32)arg4;
    *(u32 *)(local + 0x40) = count;

    u32 gqr6 = 0;
    u32 gqr7 = 0;
    if (format != NULL) {
        *(u32 *)(local + 0x14) = lbl_805FB0C0;
        *(u8 *)(local + 0x18) = lbl_805FB0C4;
        *(u32 *)(local + 0xC) = lbl_805FB0C8;
        *(u8 *)(local + 0x10) = lbl_805FB0CC;
        *(u32 *)(local + 0x1C) = lbl_805FB0B8;
        *(u8 *)(local + 0x20) = lbl_805FB0BC;

        u32 indexA = format[0];
        u32 indexB = format[1];
        u32 valueA = format[0xC];
        u32 valueB = format[0xD];
        *(u32 *)(local + 0x34) = *(u8 *)(local + 0x14 + indexA);
        *(u32 *)(local + 0x38) = *(u8 *)(local + 0x14 + indexB);

        gqr6 = *(u8 *)(local + 0x1C + indexA);
        gqr6 = (gqr6 & ~0xFF00) | ((valueA & 0xFF) << 8);
        gqr6 = ((gqr6 & 0xFFFF) << 16) | (gqr6 & 0xFFFF);
        gqr7 = *(u8 *)(local + 0x1C + indexB);
        gqr7 = (gqr7 & ~0xFF00) | ((valueB & 0xFF) << 8);
        gqr7 = ((gqr7 & 0xFFFF) << 16) | (gqr7 & 0xFFFF);
        *(u32 *)(local + 0x3C) = valueB != 0 ? *(u32 *)(local + 0x38) * 6 : 0;
    } else {
        *(u32 *)(local + 0x34) = 4;
        *(u32 *)(local + 0x38) = 4;
        *(u32 *)(local + 0x3C) = 0;
    }

    u32 savedGqr5 = RwCpp13ReadGqr5();
    RwCpp13WriteGqr5(0x07040000);
    RwCpp13WriteGqr6(gqr6);
    RwCpp13WriteGqr7(gqr7);

    switch (object[4]) {
    case 1:
        for (u32 i = 0; i < count; i++) {
            u32 matrixIndex = *(u32 *)(object[5] + i * 4) & 0xFF;
            u8 *matrix = matrices + matrixIndex * 0x40;
            fn_80482458(outA + i * 0xC, (u8 *)arg3 + i * 0xC, matrix);
            fn_80482504(outB + i * 0xC, (u8 *)arg4 + i * 0xC, matrix);
        }
        break;
    case 2:
        fn_80451080(matrices, object, local + 0x24);
        break;
    case 3:
        fn_804513C4(matrices, object, local + 0x24);
        break;
    case 4:
        fn_804517C8(matrices, object, local + 0x24);
        break;
    default:
        break;
    }

    RwCpp13WriteGqr5(savedGqr5);
    u32 strideA = *(u32 *)(local + 0x34);
    u32 strideB = *(u32 *)(local + 0x38);
    u32 extra = *(u32 *)(local + 0x3C);
    u32 bytes = ((count * (strideA * 3) + 0x1F) & ~0x1F) + count * (extra + strideB * 3);
    DCFlushRange(outA, bytes);
    fn_80394F2C();
}

extern "C" void *fn_8044D924(u32 *object, u32 *out) {
    u32 *owner = (u32 *)object[6];
    u32 *record = (u32 *)fn_8044CCAC((u8 *)owner);
    void *arg2;
    void *arg3;
    if (owner[6] == 1) {
        arg2 = owner;
        arg3 = (u8 *)owner + 0x58;
    } else {
        arg2 = object;
        arg3 = (u8 *)object + 0x14;
    }

    void *result;
    if (record[4] > 1 || record[11] == 3) {
        if ((owner[2] & 0x02000000) != 0 && lbl_805EDDD8 == 1) {
            result = fn_8044F9E4(owner, arg2, arg3);
        } else {
            result = fn_8046C0D4(owner, arg2, arg3);
        }
    } else if ((owner[2] & 0x02000000) != 0 && lbl_805EDDD8 == 1) {
        result = fn_8044F9E4(owner, arg2, arg3);
    } else {
        result = fn_80450938(owner, arg2, arg3);
    }
    *out = (u32)result;
    *(u16 *)((u8 *)owner + 0xC) = 0;
    return object;
}
