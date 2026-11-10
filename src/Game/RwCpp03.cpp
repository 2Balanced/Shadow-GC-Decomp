/* RenderWare state and interpolation helpers. Function and member names are inferred from behavior.
 * Definitions are in reverse address order because deferred inlining emits the last one first. */
#include <string.h>
#include <dolphin/types.h>

extern u8 *lbl_805F1404;
extern float lbl_805FAF28;
extern float lbl_805FAF2C;
extern float lbl_805FAF30;
extern float lbl_805FAF34;
extern float lbl_805FAE74;
extern float lbl_805FAF38;
extern float lbl_805FAF40;
extern float lbl_805FAF44;
extern float lbl_805FAF48;
extern float lbl_805FAF4C;
extern float lbl_805FAF50;
extern float lbl_805FAF54;
extern u32 __OSCurrHeap;
extern u32 lbl_805F1400;
extern u32 lbl_805F1408;

extern "C" {
void fn_80384978(int);
void OSReport(const char *, ...);
u32 OSDisableInterrupts(void);
void OSRestoreInterrupts(u32);
void *fn_8043B8F0(void *, void *);
void fn_8043C2A4(u32 *, u32 *);
void fn_8043C170(u32 *, int, float);
void *fn_8043C10C(void *, void *);
void fn_8043BE6C(u32 *);
void fn_80387794(void);
void fn_803882A4(void);
void fn_80388D50(void);
void fn_80388A00(void);
void fn_80384250(void *);
void fn_80384244(void *, void *);
void fn_8038A3EC(void);
void fn_80388258(u32);
void fn_80387748(u32);
void fn_803888A0(u32);
void fn_80389230(u32);
void OSFreeToHeap(u32, u32);
void fn_80381AE0(void *);
u32 fn_80381A78(u32);
void fn_80389320(void *, void *);
void fn_803881EC(u32);
void fn_803876D8(u32);
void fn_80388658(u32);
void fn_803891E8(u32);
void fn_8043FF5C(u32 *);
void fn_8043F5AC(void);
void fn_8043FB58(void);
void fn_8043FC98(void);
void fn_8043FD70(void);
u32 OSGetTime(void);
void *OSAllocFromHeap(u32, u32);
void OSEnableInterrupts(void);
void fn_8043B840(void **);
void fn_8043B868(void *, void *);
void *fn_8043B910(void *, void *);
void *fn_8043B954(void *, u32, u32, u32, int);
void *fn_8043B8F0(void *, void *);
void *fn_8043B3DC(void *, void *);
int fn_8043BB18(void);
int fn_8043D49C(void);
u32 fn_80383C48(u32, void (*)(u32 *), u32 *);
void fn_8043D6FC(u32 *);
void fn_8043D870(float *);
void fn_8043D99C(float *);
void fn_8043DAB0(u32 *);
void fn_8043DA0C(u32 *);
void fn_8043D61C(void);
void fn_8043AEBC(u32, int);
void fn_8043AB6C(void *, int, u32, void *, void *);
void fn_8043B25C(void);
void fn_8043B1AC(int, u32);
void fn_8043B5FC(void *);
void fn_8043AD94(u32);
void fn_8043ACE8(void *);
void fn_8043E494(void);
float fn_8043D318(int);
void fn_8043D7B4(float *, float, float);
void fn_8043D8E0(float *, float, float);
void fn_8043E9C4(u32, u32);
void fn_8043E950(void);
void fn_8043E624(void);
u32 fn_8043EBFC(void);
int fn_8043EC3C(u32 *);
int fn_8043ECDC(u32 *);
int fn_8043ED7C(u32 *, s8, u32);
int fn_8043EE74(u32 *, s16, u32);
int fn_8043EF50(u32 *);
void fn_80386348(u32, u32);
void fn_8038A338(u32);

u32 fn_8043EF48(void) {
    return 0;
}

void fn_8043E47C(int value) {
    s32 result = -1;
    if (value != 0) {
        result = 0;
    }
    lbl_805F1404[0xFDFA] = result;
}

void fn_8043DAE4(u8 *object, u32 index, u32 value) {
    u32 masked = value & 0x3FFF;
    object += (index & 0xFF) * 0x48;
    *(u32 *)(object + 0xC0) = masked;
    *(u32 *)(object + 0xD0) = *(u32 *)(object + 0xC0) - 0x2000;
}

void fn_8043DAB0(u32 *object) {
    if ((*object & 0x80000000) == 0) {
        return;
    }
    for (int i = 0; i < 0x10; i++) {
        *(u32 *)((u8 *)object + i * 0x48 + 0xD0) =
            *(u32 *)((u8 *)object + i * 0x48 + 0xC0) - 0x2000;
    }
}

void fn_8043E430(int value) {
    s32 result = -1;
    if (value != 0) {
        result = 0;
    }
    lbl_805F1404[0xFDFB] = result;
    if (value != 0) {
        fn_80384978(0);
    } else {
        fn_80384978(2);
    }
}

void fn_8043D99C(float *object) {
    float maximum = lbl_805FAF28;
    if (object[8] >= maximum) {
        object[8] -= lbl_805FAF34;
        float minimum = lbl_805FAF2C;
        if (object[8] <= maximum) {
            object[8] = object[0x14];
            *(float *)(object + 0x1C) = maximum;
            object[0x20] = minimum;
            return;
        }
        object[8] =
            (*(float *)(object + 0x14) - *(float *)(object + 0x18)) *
                (lbl_805FAF30 - object[8] / *(float *)(object + 0x1C)) +
            *(float *)(object + 0x18);
    }
}

void fn_8043D8E0(float *object, float first, float second) {
    u32 interrupt = OSDisableInterrupts();
    object[0x06] = object[0x02];
    object[0x05] = first;
    object[0x07] = second;
    object[0x08] = second;
    if (object[0x08] <= lbl_805FAF28) {
        object[0x02] = object[0x05];
        object[0x07] = lbl_805FAF28;
        object[0x08] = lbl_805FAF2C;
    } else {
        object[0x02] = (object[0x05] - object[0x06]) *
            (lbl_805FAF30 - object[0x08] / object[0x07]) + object[0x06];
    }
    OSRestoreInterrupts(interrupt);
}

void fn_8043D870(float *object) {
    float maximum = lbl_805FAF28;
    if (object[0x0E] >= maximum) {
        object[0x0E] -= lbl_805FAF34;
        if (object[0x0E] <= maximum) {
            object[0x09] = object[0x0B];
            object[0x0D] = maximum;
            object[0x0E] = lbl_805FAF2C;
            return;
        }
        object[0x09] = (object[0x0B] - object[0x0C]) *
            (lbl_805FAF30 - object[0x0E] / object[0x0D]) + object[0x0C];
    }
}

void fn_8043D7B4(float *object, float first, float second) {
    u32 interrupt = OSDisableInterrupts();
    object[0x0C] = object[0x09];
    object[0x0B] = first;
    object[0x0D] = second;
    object[0x0E] = second;
    if (object[0x0E] <= lbl_805FAF28) {
        object[0x09] = object[0x0B];
        object[0x0D] = lbl_805FAF28;
        object[0x0E] = lbl_805FAF2C;
    } else {
        object[0x09] = (object[0x0B] - object[0x0C]) *
            (lbl_805FAF30 - object[0x0E] / object[0x0D]) + object[0x0C];
    }
    OSRestoreInterrupts(interrupt);
}

u32 *fn_8043D4D8(u32 arg1, u32 arg2, u32 arg3) {
    u32 *object;
    for (int i = 0; i < 64; i++) {
        *(int *)(lbl_805F1404 + 0x50) += 1;
        u32 value = *(u32 *)(lbl_805F1404 + 0x50) >> 31;
        u32 wrapped = (*(u32 *)(lbl_805F1404 + 0x50) << 26) - value;
        *(u32 *)(lbl_805F1404 + 0x50) = (wrapped << 6 | wrapped >> 26) + value;
        object = (u32 *)(lbl_805F1404 + *(int *)(lbl_805F1404 + 0x50) * 0x10C + 0x54);
        if ((*object & 0x80000000) != 0 && (*object & 0x40000000) != 0) {
            u32 result = fn_80383C48(0x1B, fn_8043D6FC, object);
            if (result != 0) {
                *(int *)(lbl_805F1404 + 0x3C) += 1;
                object[0xD] = result;
                *object &= 0xBFFFFFFF;
                goto initialized;
            }
        }
        if ((*object & 0x80000000) == 0 && object[0xD] != 0) {
            goto initialized;
        }
    }
    object = NULL;
initialized:
    if (object == NULL) goto done;
    *(float *)(object + 4) = lbl_805FAE74;
    object[0x42] = arg1;
    object[2] = arg2;
    object[3] = arg3;
    *object |= 0x80000000;
    *object &= 0xFFFFFFFE;
    *object &= 0xFFFFFFFD;
    object[0x41] = 0;
done:
    return object;
}

void fn_8043DB08(u8 *object, u32 index, u8 command, u32 value) {
    switch (command) {
    case 0:
    case 0x20: {
        object += (index & 0xFF) * 0x48;
        *(u32 *)(object + 0xAC) = value & 0x7F;
        return;
    }
    case 7: {
        object += (index & 0xFF) * 0x48;
        *(u32 *)(object + 0xB4) = value & 0x7F;
        return;
    }
    case 0xB: {
        object += (index & 0xFF) * 0x48;
        *(u32 *)(object + 0xB8) = value & 0x7F;
        return;
    }
    case 0xA:
        object += (index & 0xFF) * 0x48;
        *(u32 *)(object + 0xBC) = value & 0x7F;
        *(u32 *)(object + 0xC8) = 0xFFFFFFFF;
        return;
    case 0x40:
        object += (index & 0xFF) * 0x48;
        if ((value & 0x7F) >= 0x40) {
            *(u32 *)(object + 0xA4) |= 1;
        } else {
            *(u32 *)(object + 0xA4) &= 0xFFFFFFFE;
        }
        return;
    case 0x3E: {
        s32 clamped;
        s32 other;
        if ((value & 0x7F) < 0x41) {
            u8 byte = value;
            clamped = (byte << 1) & 0xFE;
            if (clamped > 0x7F) {
                clamped = 0x7F;
            }
            other = 0;
        } else {
            other = 0x7F;
            clamped = (0x7F - (value & 0x7F)) << 1;
        }
        object += (index & 0xFF) * 0x48;
        *(u32 *)(object + 0xC8) = clamped;
        *(u32 *)(object + 0xC4) = other;
        return;
    }
    case 0x3F: {
        object += (index & 0xFF) * 0x48;
        *(u32 *)(object + 0xC4) = value & 0x7F;
        return;
    }
    case 0x79:
        OSDisableInterrupts();
        object += (index & 0xFF) * 0x48;
        *(u32 *)(object + 0xA4) = 0;
        *(u32 *)(object + 0xB4) = 100;
        *(u32 *)(object + 0xBC) = 0x40;
        *(u32 *)(object + 0xC0) = 0x2000;
        *(u32 *)(object + 0xB8) = 0x7F;
        *(u32 *)(object + 0xC4) = 0;
        *(u32 *)(object + 0xCC) = 0;
        *(u32 *)(object + 0xC8) = 0xFFFFFFFF;
        *(u32 *)(object + 0xDC) = 0x88888888;
        float initial = lbl_805FAF38;
        *(float *)(object + 0xE0) = initial;
        *(float *)(object + 0xE4) = initial;
        OSRestoreInterrupts(0);
        return;
    default:
        return;
    }
}

void fn_8043DD14(u32 *object) {
    while ((s32)object[0x54B] != 0) {
        OSDisableInterrupts();
        object[0x54B]--;
        u32 command = object[object[0x549] + 0x149];
        object[0x549]++;
        u32 sign = object[0x549] >> 31;
        u32 wrapped = (object[0x549] << 22) - sign;
        object[0x549] = (wrapped << 10 | wrapped >> 22) + sign;
        OSRestoreInterrupts(0);
        s32 high = (command >> 0x18) & 0xF0;
        u32 index = (command >> 0x18) & 0xF;
        u32 sub = (command >> 0x10) & 0xFF;
        u32 low = (command >> 8) & 0xFF;
        if (high == 0xB0) {
            fn_8043DB08((u8 *)object, index, sub, low & 0xFF);
        } else if (high < 0xB0) {
            if (high == 0x90) {
                if ((low & 0xFF) == 0) {
                    if ((object[index * 0x12 + 0x29] & 1) == 0) {
                        int outer = 0;
                        while ((outer = (int)fn_8043B8F0((u8 *)(object + 0x28), (void *)outer)) != 0) {
                            void *entry = *(void **)(outer + 4);
                            if (entry != NULL && *(u8 *)((u8 *)entry + 2) == index &&
                                *(u8 *)((u8 *)entry + 3) == sub) {
                                if ((*object & 0x40000000) == 0) {
                                    int inner = 0;
                                    while ((inner = (int)fn_8043B3DC((u8 *)entry + 0x10, (void *)inner)) != 0) {
                                        void *item = *(void **)(inner + 4);
                                        if (item != NULL && *(int *)((u8 *)item + 0xD8) < 4) {
                                            *(u32 *)((u8 *)item + 0xD8) = 4;
                                        }
                                    }
                                } else {
                                    fn_8043B868((u8 *)(object + 0x28), (void *)outer);
                                }
                            }
                        }
                    }
                } else {
                    void *created = fn_8043B910((u8 *)(object + 0x28), NULL);
                    if (created != NULL) {
                        void *entry = fn_8043B954(object, index, sub, low & 0xFF, (s8)command);
                        if (entry == NULL) {
                            fn_8043B868((u8 *)(object + 0x28), created);
                        } else {
                            *(u32 **)((u8 *)entry + 0xC) = object;
                            *(u8 *)((u8 *)entry + 3) = (u8)sub;
                            *(u8 *)((u8 *)entry + 2) = (u8)index;
                            *(void **)((u8 *)created + 4) = entry;
                        }
                    }
                }
            } else if (high == 0x80 && (object[index * 0x12 + 0x29] & 1) == 0) {
                int outer = 0;
                while ((outer = (int)fn_8043B8F0((u8 *)(object + 0x28), (void *)outer)) != 0) {
                    void *entry = *(void **)(outer + 4);
                    if (entry != NULL && *(u8 *)((u8 *)entry + 2) == index &&
                        *(u8 *)((u8 *)entry + 3) == sub) {
                        if ((*object & 0x40000000) == 0) {
                            int inner = 0;
                            while ((inner = (int)fn_8043B3DC((u8 *)entry + 0x10, (void *)inner)) != 0) {
                                void *item = *(void **)(inner + 4);
                                if (item != NULL && *(int *)((u8 *)item + 0xD8) < 4) {
                                    *(u32 *)((u8 *)item + 0xD8) = 4;
                                }
                            }
                        } else {
                            fn_8043B868((u8 *)(object + 0x28), (void *)outer);
                        }
                    }
                }
            }
        } else if (high != 0xD0) {
            if (high < 0xD0) {
                if (high == 0xC0) {
                    object[index * 0x12 + 0x2C] = sub & 0x7F;
                }
            } else if (high == 0xE0) {
                fn_8043DAE4((u8 *)object, index, (low & 0x7F) * 0x80 + (sub & 0x7F));
            }
        }
    }
}

void fn_8043E004(u32 *object, u32 value) {
    u32 interrupt = OSDisableInterrupts();
    if ((int)object[0x54B] < 0x400) {
        object[object[0x54A] + 0x149] = value;
        object[0x54B]++;
        object[0x54A]++;
        u32 sign = object[0x54A] >> 31;
        u32 wrapped = (object[0x54A] << 22) - sign;
        object[0x54A] = (wrapped << 10 | wrapped >> 22) + sign;
    }
    OSRestoreInterrupts(interrupt);
}

void fn_8043E088(u32 *object) {
    u32 interrupt = OSDisableInterrupts();
    if (object[0xA] != 0) {
        fn_8043B840((void **)(object + 0xA));
        object[0xA] = 0;
    }
    *object &= 0x7FFFFFFF;
    OSRestoreInterrupts(interrupt);
}

u32 *fn_8043E0E8(void) {
    u32 interrupt = OSDisableInterrupts();
    u32 *entry;
    int i;
    for (i = 0; i < 8; i++) {
        if ((*(u32 *)((u8 *)lbl_805F1404 + i * 0x1530 + 0x5478) & 0x80000000) == 0) {
            goto found;
        }
    }
    OSRestoreInterrupts(interrupt);
    return NULL;

found:
    entry = (u32 *)((u8 *)lbl_805F1404 + i * 0x1530 + 0x5478);
    memset(entry, 0, 0x1530);
    float f2c = lbl_805FAF2C;
    float f30 = lbl_805FAF30;
    float f28 = lbl_805FAF28;
    float initial = lbl_805FAF38;
    *entry |= 0x80000000;
    *(float *)(entry + 2) = f28;
    *(float *)(entry + 3) = f28;
    *(float *)(entry + 9) = f30;
    *(float *)(entry + 10) = f28;
    *(float *)(entry + 4) = f30;
    *(float *)(entry + 8) = f2c;
    *(float *)(entry + 0xE) = f2c;
    for (int j = 0; j < 0x10; j++) {
        u8 *slot = (u8 *)entry + j * 0x48;
        *(u32 *)(slot + 0xA8) = 0;
        OSDisableInterrupts();
        *(u32 *)(slot + 0xA4) = 0;
        *(u32 *)(slot + 0xB4) = 100;
        *(u32 *)(slot + 0xBC) = 0x40;
        *(u32 *)(slot + 0xC0) = 0x2000;
        *(u32 *)(slot + 0xB8) = 0x7F;
        *(u32 *)(slot + 0xC4) = 0;
        *(u32 *)(slot + 0xCC) = 0;
        *(u32 *)(slot + 0xC8) = 0xFFFFFFFF;
        *(u32 *)(slot + 0xDC) = 0x88888888;
        *(float *)(slot + 0xE0) = initial;
        *(float *)(slot + 0xE4) = initial;
        OSRestoreInterrupts(0);
    }
    OSRestoreInterrupts(interrupt);
    return entry;
}

void fn_8043E228(u8 *object, int index) {
    OSDisableInterrupts();
    object += index * 0x48;
    *(u32 *)(object + 0xA4) = 0;
    *(u32 *)(object + 0xB4) = 100;
    *(u32 *)(object + 0xBC) = 0x40;
    *(u32 *)(object + 0xC0) = 0x2000;
    *(u32 *)(object + 0xB8) = 0x7F;
    *(u32 *)(object + 0xC4) = 0;
    *(u32 *)(object + 0xCC) = 0;
    *(u32 *)(object + 0xC8) = 0xFFFFFFFF;
    *(u32 *)(object + 0xDC) = 0x88888888;
    *(float *)(object + 0xE0) = lbl_805FAF38;
    *(float *)(object + 0xE4) = lbl_805FAF38;
    OSRestoreInterrupts(0);
}
void fn_8043E2B4(int param_1, int param_2) {
    void (*callback)(void) = NULL;
    void *data = NULL;
    switch (param_2) {
    case 0:
        break;
    case 1:
        callback = fn_803882A4;
        data = lbl_805F1404 + 0xFE04;
        break;
    case 2:
        callback = fn_80387794;
        data = lbl_805F1404 + 0xFF58;
        break;
    case 3:
        callback = fn_80388A00;
        data = lbl_805F1404 + 0x10138;
        break;
    case 4:
        callback = fn_80388D50;
        data = lbl_805F1404 + 0x101D4;
        break;
    default:
        return;
    }
    switch (param_1) {
    case 0:
        fn_80384244(callback, data);
        break;
    case 1:
        fn_80384250(callback);
        break;
    default:
        break;
    }
}

void fn_8043E394(void) {
    if (lbl_805F1404 != NULL && lbl_805F1404[0x13A38] != 0) {
        lbl_805F1404[0x13A39]++;
        OSEnableInterrupts();
        if (lbl_805F1404 != NULL && *(s32 *)lbl_805F1404 != 0) {
            fn_8043E494();
            ((u32 *)lbl_805F1404)[0x10]++;
        }
        OSDisableInterrupts();
        fn_8038A3EC();
    } else {
        if (lbl_805F1404 != NULL && *(s32 *)lbl_805F1404 != 0) {
            fn_8043E494();
        }
        fn_8038A3EC();
    }
}

void fn_8043E494(void) {
    if (lbl_805F1404 == NULL || *(s32 *)lbl_805F1404 == 0) {
        return;
    }
    OSDisableInterrupts();
    ((u32 *)lbl_805F1404)[2]++;
    OSRestoreInterrupts(0);
    if (((s32 *)lbl_805F1404)[2] != 1) {
        lbl_805F1404[0x13A3B]++;
        OSDisableInterrupts();
        ((u32 *)lbl_805F1404)[2]--;
        OSRestoreInterrupts(0);
    } else {
        ((u32 *)lbl_805F1404)[9] = ((u32 *)lbl_805F1404)[7];
        ((u32 *)lbl_805F1404)[8] = ((u32 *)lbl_805F1404)[6];
        ((u32 *)lbl_805F1404)[7] = (u32)lbl_805F1404;
        ((u32 *)lbl_805F1404)[6] = OSGetTime();
        ((u32 *)lbl_805F1404)[4] = fn_8043BB18();
        ((u32 *)lbl_805F1404)[3] = fn_8043D49C();
        for (int i = 0; i < 8; i++) {
            if ((*(u32 *)((u8 *)lbl_805F1404 + i * 0x1530 + 0x5478) & 0x80000000) != 0) {
                fn_8043FF5C((u32 *)((u8 *)lbl_805F1404 + i * 0x1530 + 0x5478));
                fn_8043DD14((u32 *)((u8 *)lbl_805F1404 + i * 0x1530 + 0x5478));
            }
            fn_8043D99C((float *)((u8 *)lbl_805F1404 + i * 0x1530 + 0x5478));
            fn_8043D870((float *)((u8 *)lbl_805F1404 + i * 0x1530 + 0x5478));
            fn_8043DAB0((u32 *)((u8 *)lbl_805F1404 + i * 0x1530 + 0x5478));
            fn_8043DA0C((u32 *)((u8 *)lbl_805F1404 + i * 0x1530 + 0x5478));
            fn_8043B5FC((u8 *)lbl_805F1404 + i * 0x1530 + 0x5478 + 0xA0);
        }
        ((u32 *)lbl_805F1404)[4] = fn_8043BB18();
        ((u32 *)lbl_805F1404)[3] = fn_8043D49C();
        OSDisableInterrupts();
        ((u32 *)lbl_805F1404)[2]--;
        OSRestoreInterrupts(0);
    }
}

void fn_8043E624(void) {
    u8 local[8];
    if (lbl_805F1404 != NULL) {
        OSDisableInterrupts();
        *(u32 *)lbl_805F1404 = 0;
        ((u32 *)lbl_805F1404)[2]++;
        OSRestoreInterrupts(0);
    }
    fn_80388258((u32)(lbl_805F1404 + 0x3F81));
    fn_80387748((u32)(lbl_805F1404 + 0x3FD6));
    fn_803888A0((u32)(lbl_805F1404 + 0x404E));
    fn_80389230((u32)(lbl_805F1404 + 0x4075));
    if (lbl_805F1404 != NULL) {
        lbl_805F1404 = NULL;
        if ((s32)lbl_805F1408 == 0) {
            OSFreeToHeap(__OSCurrHeap, lbl_805F1400);
        } else {
            lbl_805F1408 = 0;
        }
        lbl_805F1400 = 0;
        fn_80381AE0(local);
    }
}

u32 fn_8043E6F4(u32 arg1, u32 arg2, u32 arg3, u32 arg4) {
    fn_8043E9C4(arg1, arg3);
    ((u32 *)lbl_805F1404)[1] = 0x14470;
    *(float *)(lbl_805F1404 + 0x2C) = lbl_805FAF40;
    ((u32 *)lbl_805F1404)[0x1427] = 0xF;
    ((u32 *)lbl_805F1404)[0xE] = arg2;
    ((u32 *)lbl_805F1404)[0x14A9] = arg4;
    u32 size = fn_80381A78(((u32 *)lbl_805F1404)[0x14A9]);
    ((u32 *)lbl_805F1404)[0x14AC] = size;
    ((u32 *)lbl_805F1404)[0x14AB] = (size + 0x1F) & 0xFFFFFFE0;
    ((u32 *)lbl_805F1404)[0x14AA] = ((u32 *)lbl_805F1404)[0x14A9] - (((u32 *)lbl_805F1404)[0x14AB] - size);
    void *memory = (void *)((u32)lbl_805F1404 + 0x13A6B & 0xFFFFFFE0);
    memset(memory, 0, 0x500);
    fn_8043AB6C(memory, ((u32 *)lbl_805F1404)[0x14AB], 0x500, 0, 0);
    ((u32 *)lbl_805F1404)[0x14AD] = ((u32 *)lbl_805F1404)[0x14AB];
    ((u32 *)lbl_805F1404)[0x14AB] += 0x500;
    ((u32 *)lbl_805F1404)[0x14AA] -= 0x500;
    *(float *)(lbl_805F1404 + 0x13A44) = lbl_805FAF44;
    ((u32 *)lbl_805F1404)[0x14AE] = 0x7D000;
    ((u32 *)lbl_805F1404)[0x14AF] = 460000;
    ((u32 *)lbl_805F1404)[0x14B0] = 360000;
    ((u32 *)lbl_805F1404)[0x14B1] = 400000;
    ((u32 *)lbl_805F1404)[0x14B2] = 360000;
    ((u32 *)lbl_805F1404)[0x14B3] = 360000;
    ((u32 *)lbl_805F1404)[0x14B4] = 140000;
    ((u32 *)lbl_805F1404)[0x14B5] = 100000;
    ((u32 *)lbl_805F1404)[0x14B6] = 0x29820;
    ((u32 *)lbl_805F1404)[0x14B7] = 200000;
    ((u32 *)lbl_805F1404)[0x14B8] = 240000;
    ((u32 *)lbl_805F1404)[0x14B9] = 24000;
    ((u32 *)lbl_805F1404)[0x14BA] = 400000;
    ((u32 *)lbl_805F1404)[0x14BB] = 40000;
    fn_8043B25C();
    for (int i = 0; i < 0x10; i++) {
        int size = *(int *)((u8 *)lbl_805F1404 + i * 4 + 0x52B8);
        if (size != 0) {
            fn_8043B1AC(i, (size + 0x1F) & 0xFFFFFFE0);
        }
    }
    OSReport("AX SDLIB: aramRest=%d\n", ((u32 *)lbl_805F1404)[0x14AA]);
    ((u32 *)lbl_805F1404)[0x11] = 1;
    fn_8043E950();
    *(u32 *)lbl_805F1404 = 1;
    return 0;
}

void fn_8043E950(void) {
    fn_8043D61C();
    fn_80389320(fn_8043AD94, fn_8043ACE8);
    fn_803881EC((u32)(lbl_805F1404 + 0xFE04));
    fn_803876D8((u32)(lbl_805F1404 + 0xFF58));
    fn_80388658((u32)(lbl_805F1404 + 0x10138));
    fn_803891E8((u32)(lbl_805F1404 + 0x101D4));
}

void fn_8043E9C4(u32 arg1, u32 arg2) {
    u32 interrupt = OSDisableInterrupts();
    if (arg1 != 0) {
        lbl_805F1408 = 1;
    } else {
        arg1 = (u32)OSAllocFromHeap(__OSCurrHeap, arg2);
        if (arg1 == 0) {
            OSReport("AX SDLIB: Panic: cannot allocate memory (%d)\n", arg2);
            while (1) {
            }
        }
    }
    OSReport("AX SDLIB: allocated (%d)\n", arg2);
    lbl_805F1404 = (u8 *)((arg1 + 0x1F) & 0xFFFFFFE0);
    lbl_805F1400 = arg1;
    *(u32 *)lbl_805F1404 = 0;
    lbl_805F1404[0x13A38] = 0;
    OSRestoreInterrupts(interrupt);
    memset(lbl_805F1404, 0, 0x14470);
    u32 heap = ((u32)lbl_805F1404 + 0x1448F) & 0xFFFFFFE0;
    fn_8043AEBC(heap, arg2 - (heap - arg1));
    lbl_805F1404[0xFF40] = 0;
    float f48 = lbl_805FAF48;
    float f4c = lbl_805FAF4C;
    float f50 = lbl_805FAF50;
    float f54 = lbl_805FAF54;
    *(float *)(lbl_805F1404 + 0xFF4C) = f48;
    *(float *)(lbl_805F1404 + 0xFF54) = f4c;
    *(float *)(lbl_805F1404 + 0xFF50) = f50;
    *(float *)(lbl_805F1404 + 0xFF44) = f50;
    *(float *)(lbl_805F1404 + 0xFF48) = f50;
    lbl_805F1404[0x1011C] = 0;
    *(float *)(lbl_805F1404 + 0x10128) = f48;
    *(float *)(lbl_805F1404 + 0x10130) = f4c;
    *(float *)(lbl_805F1404 + 0x1012C) = f50;
    *(float *)(lbl_805F1404 + 0x10120) = f50;
    *(float *)(lbl_805F1404 + 0x10134) = f54;
    *(float *)(lbl_805F1404 + 0x10124) = f50;
    ((u32 *)lbl_805F1404)[0x4072] = 0xF;
    ((u32 *)lbl_805F1404)[0x4073] = 0;
    ((u32 *)lbl_805F1404)[0x4074] = 500;
    ((u32 *)lbl_805F1404)[0x4084] = 500;
    ((u32 *)lbl_805F1404)[0x4085] = 500;
    ((u32 *)lbl_805F1404)[0x4086] = 500;
    ((u32 *)lbl_805F1404)[0x4087] = 0x32;
    ((u32 *)lbl_805F1404)[0x4088] = 0x32;
    ((u32 *)lbl_805F1404)[0x4089] = 0x32;
    ((u32 *)lbl_805F1404)[0x408A] = 100;
    ((u32 *)lbl_805F1404)[0x408B] = 100;
    ((u32 *)lbl_805F1404)[0x408C] = 100;
}

u32 fn_8043EBFC(void) {
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    fn_8043F5AC();
    fn_8043E624();
    return 0;
}

int fn_8043EC3C(u32 *object) {
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    if (object == NULL) {
        return 0x7000103;
    }
    int result = 0x7000102;
    for (int i = 0; i < 8; i++) {
        if (object == (u32 *)((u8 *)lbl_805F1404 + i * 0x1530 + 0x5478)) {
            if (*(s32 *)object != 0) {
                result = 0;
            }
            break;
        }
    }
    if (result != 0) return result;
    fn_8043FB58();
    return 0;
}

int fn_8043ECDC(u32 *object) {
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    if (object == NULL) {
        return 0x7000103;
    }
    int result = 0x7000102;
    for (int i = 0; i < 8; i++) {
        if (object == (u32 *)((u8 *)lbl_805F1404 + i * 0x1530 + 0x5478)) {
            if (*(s32 *)object != 0) {
                result = 0;
            }
            break;
        }
    }
    if (result != 0) return result;
    fn_8043FC98();
    return 0;
}

int fn_8043ED7C(u32 *object, s8 value, u32 arg2) {
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    if (object == NULL) {
        return 0x7000103;
    }
    int result = 0x7000102;
    for (int i = 0; i < 8; i++) {
        if (object == (u32 *)((u8 *)lbl_805F1404 + i * 0x1530 + 0x5478)) {
            if (*(s32 *)object != 0) {
                result = 0;
            }
            break;
        }
    }
    if (result != 0) return result;
    if (value < -0x7F || value > 0x7F) {
        return 0x3000001;
    }
    u32 unsignedValue = (u32)value;
    u32 unsignedArg = (u32)arg2;
    fn_8043D8E0((float *)object, (float)unsignedValue / 127.0f,
                (float)unsignedArg);
    return 0;
}

int fn_8043EE74(u32 *object, s16 value, u32 arg2) {
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    if (object == NULL) {
        return 0x7000103;
    }
    int result = 0x7000102;
    for (int i = 0; i < 8; i++) {
        if (object == (u32 *)((u8 *)lbl_805F1404 + i * 0x1530 + 0x5478)) {
            if (*(s32 *)object != 0) {
                result = 0;
            }
            break;
        }
    }
    if (result != 0) return result;
    fn_8043D7B4((float *)object, fn_8043D318(value), (float)(u32)arg2);
    return 0;
}

int fn_8043EF50(u32 *object) {
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    if (object == NULL) {
        return 0x7000103;
    }
    int result = 0x7000102;
    for (int i = 0; i < 8; i++) {
        if (object == (u32 *)((u8 *)lbl_805F1404 + i * 0x1530 + 0x5478)) {
            if (*(s32 *)object != 0) {
                result = 0;
            }
            break;
        }
    }
    if (result != 0) return result;
    fn_8043FD70();
    return 0;
}

void fn_8043D61C(void) {
    fn_80384978(0);
    memset(lbl_805F1404 + 0x54, 0, 0x4300);
    for (int i = 0; i < 64; i++) {
        *(u32 *)(lbl_805F1404 + i * 0x10C + 0x58) = i;
    }
    int count = *(int *)(lbl_805F1404 + 0x38);
    if (count > 64) {
        count = 64;
    }
    for (int i = 0; i < count; i++) {
        u32 result = fn_80383C48(0x1B, fn_8043D6FC,
                                 (u32 *)(lbl_805F1404 + i * 0x10C + 0x54));
        *(u32 *)(lbl_805F1404 + i * 0x10C + 0x88) = result;
        if (result == 0) {
            break;
        }
        *(int *)(lbl_805F1404 + 0x3C) += 1;
    }
}

void fn_8043D6FC(u32 *object) {
    u32 *owner = *(u32 **)((u8 *)object + 0x14);
    fn_80386348(owner[0xD], 0);
    fn_8038A338(owner[0xD]);
    *owner |= 0x40000000;
    *(int *)(lbl_805F1404 + 0x3C) -= 1;
    u32 result = fn_80383C48(0x1B, fn_8043D6FC, owner);
    owner[0xD] = result;
    if (owner[0xD] != 0) {
        *owner &= 0xBFFFFFFF;
        fn_80386348(owner[0xD], 0);
        *(int *)(lbl_805F1404 + 0x3C) += 1;
    } else {
        OSReport("AX SDLIB: AXAcquireVoice() failed in drop callback\n");
    }
}

void fn_8043DA0C(u32 *object) {
    if ((*object & 0x80000000) == 0) {
        return;
    }
    int outer = 0;
    while ((outer = (int)fn_8043B8F0((u8 *)(object + 0x28), (void *)outer)) != 0) {
        int inner = 0;
        u8 index = *(u8 *)(*(u32 *)(outer + 4) + 2);
        while ((inner = (int)fn_8043C10C(*(u8 **)(outer + 4) + 0x10, (void *)inner)) != 0) {
            fn_8043BE6C(*(u32 **)(inner + 4));
            fn_8043C2A4(*(u32 **)(inner + 4), object);
            fn_8043C170(*(u32 **)(inner + 4), object[index * 0x12 + 0x34],
                        *(float *)((u8 *)object + 0x24));
        }
    }
}
}
