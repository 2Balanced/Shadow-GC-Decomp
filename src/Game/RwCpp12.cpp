/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <string.h>
#include <dolphin/types.h>

extern u32 lbl_8056D708[];
extern u8 *lbl_805F15B4;

extern "C" {
u32 fn_8047D868(void *, void *, u32);
u32 fn_80480D4C(void *, void *, u32);
s32 fn_80485DB8(u32);
void fn_80451D60(u32);
extern u32 lbl_805EDD80;
extern u32 lbl_805EDD84;
extern u32 lbl_805E1538;
void *fn_8047FC04(u32, void *, void *, u32, void *, u32);

void *fn_8044BBFC(void *stream, u32 *object) {
    u32 local[5];
    if (fn_8047D868(stream, local + 2, 4) == 0 ||
        fn_8047D868(stream, local + 4, 4) == 0 ||
        fn_8047D868(stream, local + 3, 4) == 0) return NULL;
    u32 count = local[4];
    if ((s32)count <= 0) return stream;
    if (object[0xF] != 0) {
        ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))((void *)object[0xF]);
    }
    object[0xC] = 0;
    object[0xD] = 0;
    object[0xE] = 0;
    object[0xF] = 0;
    object[0x10] = 0;
    object[0x11] = 0;
    u32 bytes = count * 2 + local[3] * 2 + object[0];
    void *memory = ((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(bytes, 0x30116);
    if (memory == NULL) return NULL;
    memset(memory, 0, bytes);
    object[0xC] = local[2];
    object[0xD] = count;
    object[0xE] = local[3];
    object[0xF] = (u32)memory + object[0];
    object[0x10] = object[0xF] + count * 2;
    object[0x11] = object[0x10] + local[3] * 2;
    u32 *extra = object + 0xC;
    u32 extraBytes = (extra[1] << 1) + (extra[2] << 1) + object[0];
    if (fn_80480D4C(stream, extra, extraBytes) == 0) {
        ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(extra);
        return NULL;
    }
    return stream;
}

void *fn_8044C174(u32 *object) {
    if (lbl_8056D708[8] == 0) {
        u32 flags = 1;
        if (fn_80485DB8(0x120) != -1) flags |= 2;
        if (fn_80485DB8(0x12E) != -1) flags |= 4;
        fn_80451D60(flags);
        void *result = fn_8047FC04(0x4C, &lbl_805EDD80, &lbl_805EDD84, 4, &lbl_805E1538, 0x400116);
        lbl_8056D708[6] = (u32)result;
        void *memory = ((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(0x400F, 0x400116);
        lbl_8056D708[4] = (u32)memory;
        memset(memory, 0, 0x400F);
        lbl_8056D708[3] = ((u32)memory + 0xF) & ~0xF;
    }
    lbl_8056D708[8]++;
    return object;
}

void fn_8044C38C(void) {
}

void fn_8044CB6C(void) {
}

u32 fn_8044CDBC(u32 *object) {
    return object[5];
}

void fn_8044CC98(u8 *base, u32 value) {
    *(u32 *)(base + lbl_8056D708[1]) = value;
}

u32 fn_8044CCAC(u8 *object) {
    return *(u32 *)(object + lbl_8056D708[2]);
}

void fn_8044C30C(u8 *base) {
    *(u32 *)(base + lbl_8056D708[2]) = 0;
}

void fn_8044C3FC(u8 *base, u8 *other) {
    u32 offset = lbl_8056D708[1];
    *(u32 *)(base + offset) = *(u32 *)(other + offset);
}

void fn_8044EE3C(void);

void fn_8044CB70(void) {
    fn_8044EE3C();
}

void fn_8044C3D8(u8 *base) {
    u32 offset = lbl_8056D708[1];
    if (*(u32 *)(base + offset) != 0) {
        *(u32 *)(base + offset) = 0;
    }
}

u32 fn_8044BDBC(u32 *object) {
    u32 result = 0x0C;
    if (object[0x0D] != 0) {
        result = object[0] + (object[0x0D] << 1) + (object[0x0E] << 1) + 0x0C;
    }
    return result;
}
}
