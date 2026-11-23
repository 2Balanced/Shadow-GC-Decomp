/* AX SDLIB, the game's GameCube AX sound library (see include/Game/rw/r1.h), 0x8043B954..0x8043D7B4:
 * voice groups, voice envelopes, mixing and pitch, starting a program's voices, and taking and
 * giving back AX voices. The file name is descriptive. Its .sdata2 pool (0x805FAE70..0x805FAF28)
 * pools 1.0f, 0.0f and -96.0f again after Game/gcaxSndUtil.c's, its .rodata is the drop callback's
 * message (0x8051BC08, padded to 8 bytes) and its .data the volume, pan and envelope tables. The
 * pool after it (0x805FAF28) repeats 0.0f, 1.0f and 5.0f: Game/gcaxSndPlayer.c starts there.
 * C with the game's flags: definitions in reverse address order.
 * Function and member names are inferred from behavior. NonMatching: fn_8043C2A4, fn_8043C89C,
 * fn_8043C9D0 and fn_8043CA7C (drafts). */
#include <dolphin/types.h>
#include "size_assert.h"
#include "Game/rw/r1.h"

double fn_803B1C9C(double);
double pow(double, double);
void fn_803894BC(u32, int, int, int, int, int, int, int);
void fn_803865F8(u32);
void fn_80386284(u32, int);
void fn_803867AC(u32, void *);
void fn_80386708(u32, void *);
void fn_803868DC(u32, void *);
void fn_803863A4(GcaxAxVoice *, GcaxMix *);
void fn_80386844(GcaxAxVoice *, float);
void fn_80386348(GcaxAxVoice *, u32);
void fn_8038A338(GcaxAxVoice *);
void fn_80384978(int);
GcaxAxVoice *fn_80383C48(u32, void (*)(GcaxAxVoice *), void *);
float fn_8043B3A0(float);
void fn_8043B44C(GcaxListNode **, void (*)(void *));
GcaxListNode *fn_8043B3F4(GcaxListNode **, int (*)(GcaxListNode *));
void fn_8043B4B0(void *, GcaxListNode **, void (*)(void *));
int fn_8043B810(GcaxListNode *);
void fn_8043B89C(void *);
void *fn_8043B560(u32, GcaxListNode **, void (*)(void *));
GcaxListNode *fn_8043B3DC(GcaxListNode **, GcaxListNode *);
void fn_8043BB54(GcaxGroup *group);
void fn_8043C2A4(GcaxVoice *voice, GcaxPlayer *player);
void fn_8043CA7C(u32 bank, GcaxVoice *object, u32 groupEntry, u32 table, u32 entry, int value,
                 u32 firstBase, u32 data);
GcaxListNode *fn_8043C12C(GcaxListNode **list, void *item);
GcaxGroup *fn_8043BCDC(int priority);
GcaxGroup *fn_8043BBC8(int priority, int count);
int fn_8043C9D0(int bank, u32 program, int key);
void fn_8043C89C(GcaxListNode **list, int bank, u32 program, int key, u32 data);
u32 fn_8043D20C(int value, float factor);
float fn_8043D3BC(int first, int second, int third);
int fn_8043D49C(void);
GcaxVoice *fn_8043D4D8(GcaxGroup *group, u32 arg2, u32 arg3);

/* 0x8056D098: attenuation in 0.375 dB steps (plus 3) for a 0..255 volume. */
static u8 s_volumeSteps[0x100] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFB, 0xF7, 0xF3, 0xEF, 0xEB, 0xE7, 0xE3, 0xDF, 0xDB,
    0xD7, 0xD1, 0xCB, 0xC5, 0xC1, 0xBB, 0xB7, 0xB3, 0xB1, 0xAD, 0xA9, 0xA5, 0xA3, 0x9F, 0x9D, 0x99,
    0x97, 0x95, 0x93, 0x91, 0x8F, 0x8D, 0x8B, 0x89, 0x87, 0x85, 0x83, 0x81, 0x7F, 0x7D, 0x7B, 0x7A,
    0x79, 0x77, 0x75, 0x74, 0x73, 0x71, 0x6F, 0x6E, 0x6D, 0x6C, 0x6B, 0x69, 0x67, 0x66, 0x65, 0x64,
    0x63, 0x62, 0x61, 0x60, 0x5F, 0x5E, 0x5D, 0x5C, 0x5B, 0x5A, 0x59, 0x58, 0x57, 0x56, 0x55, 0x54,
    0x53, 0x52, 0x51, 0x50, 0x4F, 0x4E, 0x4D, 0x4D, 0x4C, 0x4C, 0x4B, 0x4A, 0x49, 0x48, 0x47, 0x47,
    0x46, 0x46, 0x45, 0x44, 0x43, 0x42, 0x41, 0x41, 0x40, 0x40, 0x3F, 0x3E, 0x3D, 0x3D, 0x3C, 0x3C,
    0x3B, 0x3B, 0x3A, 0x3A, 0x39, 0x38, 0x37, 0x37, 0x36, 0x36, 0x35, 0x35, 0x34, 0x34, 0x33, 0x33,
    0x32, 0x32, 0x31, 0x31, 0x30, 0x30, 0x2F, 0x2F, 0x2E, 0x2E, 0x2D, 0x2D, 0x2C, 0x2C, 0x2B, 0x2B,
    0x2A, 0x2A, 0x29, 0x29, 0x28, 0x28, 0x27, 0x27, 0x26, 0x26, 0x25, 0x25, 0x24, 0x24, 0x23, 0x23,
    0x23, 0x22, 0x22, 0x22, 0x21, 0x21, 0x20, 0x20, 0x1F, 0x1F, 0x1E, 0x1E, 0x1D, 0x1D, 0x1D, 0x1C,
    0x1C, 0x1C, 0x1B, 0x1B, 0x1B, 0x1A, 0x1A, 0x1A, 0x19, 0x19, 0x18, 0x18, 0x17, 0x17, 0x17, 0x16,
    0x16, 0x16, 0x15, 0x15, 0x15, 0x14, 0x14, 0x14, 0x13, 0x13, 0x13, 0x12, 0x12, 0x12, 0x11, 0x11,
    0x11, 0x10, 0x10, 0x10, 0x0F, 0x0F, 0x0F, 0x0E, 0x0E, 0x0E, 0x0D, 0x0D, 0x0D, 0x0D, 0x0C, 0x0C,
    0x0C, 0x0C, 0x0B, 0x0B, 0x0B, 0x0A, 0x0A, 0x0A, 0x09, 0x09, 0x09, 0x08, 0x08, 0x08, 0x07, 0x07,
    0x07, 0x07, 0x06, 0x06, 0x06, 0x06, 0x05, 0x05, 0x05, 0x05, 0x04, 0x04, 0x04, 0x04, 0x03, 0x03,
};
/* 0x8056D198: pan of a region (0..31): two curves of 16. */
static u8 s_regionPans[0x20] = {
    0x40, 0x54, 0x5C, 0x64, 0x6A, 0x6E, 0x72, 0x75, 0x77, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
    0x40, 0x2B, 0x23, 0x1B, 0x15, 0x11, 0x0D, 0x0A, 0x08, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00,
};
/* 0x8056D1B8: index of the left gain in s_panGains for a 0..127 pan; the right gain is at index ^ 0x10. */
static u8 s_panSteps[0x80] = {
    0x1F, 0x1E, 0x1D, 0x1C, 0x1B, 0x1A, 0x19, 0x19, 0x18, 0x18, 0x17, 0x17, 0x16, 0x16, 0x16, 0x16,
    0x15, 0x15, 0x15, 0x15, 0x14, 0x14, 0x14, 0x14, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13,
    0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05,
    0x06, 0x06, 0x06, 0x06, 0x07, 0x07, 0x08, 0x08, 0x09, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
};
/* 0x8056D238: 0..15 step for a 0..63 level, see AddLevelDecibels. */
static u8 s_levelSteps[0x40] = {
    0x00, 0x01, 0x01, 0x02, 0x02, 0x03, 0x03, 0x03, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x06, 0x06,
    0x06, 0x07, 0x07, 0x07, 0x08, 0x08, 0x08, 0x08, 0x09, 0x09, 0x09, 0x09, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0A, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D,
    0x0D, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
};
/* 0x8056D278 */
static float s_panGains[0x20] = {
    1.0f, 0.708008f, 0.5f, 0.354004f, 0.25f, 0.177002f, 0.125f, 0.088501f,
    0.0625f, 0.0442505f, 0.03125f, 0.02212525f, 0.015625f, 0.011062625f, 0.0078125f, 0.0f,
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
};
/* 0x8056D2F8: envelope times (ms) by rate index. */
static float s_attackTimes[0x40] = {
    1e+08f, 1e+08f, 8.1e+03f, 6.9e+03f, 6e+03f, 4.8e+03f, 4e+03f, 3.4e+03f,
    3e+03f, 2.4e+03f, 2e+03f, 1.7e+03f, 1.5e+03f, 1.2e+03f, 1e+03f, 8.6e+02f,
    7.6e+02f, 6e+02f, 5e+02f, 4.3e+02f, 3.8e+02f, 3e+02f, 2.5e+02f, 2.2e+02f,
    1.9e+02f, 1.5e+02f, 1.3e+02f, 1.1e+02f, 95.0f, 76.0f, 63.0f, 55.0f,
    47.0f, 38.0f, 31.0f, 27.0f, 24.0f, 19.0f, 15.0f, 13.0f,
    12.0f, 9.4f, 7.9f, 6.8f, 6.0f, 4.7f, 3.8f, 3.4f,
    3.0f, 2.4f, 2.0f, 1.8f, 1.6f, 1.3f, 1.1f, 0.93f,
    0.85f, 0.65f, 0.53f, 0.44f, 0.4f, 0.35f, 0.0f, 0.0f,
};
/* 0x8056D3F8 */
static float s_decayTimes[0x40] = {
    1e+08f, 1e+08f, 1.182e+05f, 1.013e+05f, 8.86e+04f, 7.09e+04f, 5.91e+04f, 5.07e+04f,
    4.43e+04f, 3.55e+04f, 2.96e+04f, 2.53e+04f, 2.22e+04f, 1.77e+04f, 1.48e+04f, 1.27e+04f,
    1.11e+04f, 8.9e+03f, 7.4e+03f, 6.3e+03f, 5.5e+03f, 4.4e+03f, 3.7e+03f, 2.8e+03f,
    2.2e+03f, 1.8e+03f, 1.6e+03f, 1.4e+03f, 1.1e+03f, 9.2e+02f, 7.9e+02f, 6.9e+02f,
    5.5e+02f, 4.6e+02f, 3.9e+02f, 3.4e+02f, 2.7e+02f, 2.3e+02f, 2e+02f, 1.7e+02f,
    1.4e+02f, 1.1e+02f, 98.0f, 85.0f, 68.0f, 57.0f, 49.0f, 43.0f,
    34.0f, 28.0f, 25.0f, 22.0f, 18.0f, 14.0f, 12.0f, 11.0f,
    8.5f, 7.1f, 6.1f, 5.4f, 4.3f, 3.6f, 3.1f, 0.0f,
};
/* 0x8056D4F8: sine-law gains of a surround span. */
static float s_spanGains[0x80] = {
    0.0f, 0.012368f, 0.024734f, 0.037097f, 0.049454f, 0.061803f, 0.074143f, 0.086471f,
    0.098786f, 0.111087f, 0.12337f, 0.135634f, 0.147877f, 0.160098f, 0.172295f, 0.184465f,
    0.196606f, 0.208718f, 0.220798f, 0.232844f, 0.244854f, 0.256827f, 0.268761f, 0.280654f,
    0.292503f, 0.304308f, 0.316066f, 0.327776f, 0.339436f, 0.351044f, 0.362598f, 0.374097f,
    0.385538f, 0.396921f, 0.408243f, 0.419502f, 0.430697f, 0.441826f, 0.452888f, 0.463881f,
    0.474802f, 0.485651f, 0.496425f, 0.507124f, 0.517745f, 0.528287f, 0.538748f, 0.549126f,
    0.559421f, 0.56963f, 0.579752f, 0.589785f, 0.599728f, 0.609579f, 0.619337f, 0.629f,
    0.638567f, 0.648036f, 0.657406f, 0.666676f, 0.675843f, 0.684908f, 0.693867f, 0.70272f,
    0.711466f, 0.720103f, 0.72863f, 0.737045f, 0.745348f, 0.753536f, 0.76161f, 0.769566f,
    0.777405f, 0.785125f, 0.792725f, 0.800204f, 0.807561f, 0.814793f, 0.821901f, 0.828884f,
    0.835739f, 0.842467f, 0.849066f, 0.855535f, 0.861873f, 0.868079f, 0.874153f, 0.880093f,
    0.885898f, 0.891567f, 0.897101f, 0.902497f, 0.907754f, 0.912873f, 0.917853f, 0.922692f,
    0.92739f, 0.931946f, 0.936359f, 0.940629f, 0.944755f, 0.948737f, 0.952574f, 0.956265f,
    0.959809f, 0.963207f, 0.966457f, 0.96956f, 0.972514f, 0.97532f, 0.977976f, 0.980482f,
    0.982839f, 0.985045f, 0.987101f, 0.989006f, 0.990759f, 0.992361f, 0.993811f, 0.995109f,
    0.996254f, 0.997248f, 0.998088f, 0.998776f, 0.999312f, 0.999694f, 0.999924f, 1.0f,
};

/* Adds the attenuation in decibels of a 0..15 level: 0 is silence, 15 is 0 dB, 3 dB a step. */
#define GCAX_ADD_LEVEL_DECIBELS(decibels, value)                                                   \
    {                                                                                              \
        int level = (value) << 2;                                                                  \
        int step;                                                                                  \
        if (level < 0) {                                                                           \
            level = 0;                                                                             \
        }                                                                                          \
        if (level > 0x3F) {                                                                        \
            level = 0x3F;                                                                          \
        }                                                                                          \
        step = s_levelSteps[level];                                                                \
        if (step == 0) {                                                                           \
            decibels += -96.0f;                                                                    \
        } else {                                                                                   \
            decibels += (float)(-(15 - step) * 3);                                                 \
        }                                                                                          \
    }

/* AX drop callback: AX took the voice back; marks ours and asks for another one. */
void fn_8043D6FC(GcaxAxVoice *axVoice) {
    GcaxVoice *voice = (GcaxVoice *)axVoice->m_userContext;
    fn_80386348(axVoice, 0);
    fn_8038A338(axVoice);
    voice->m_flags |= 0x40000000;
    lbl_805F1404->m_axVoiceCount--;
    voice->m_axVoice = fn_80383C48(0x1B, fn_8043D6FC, voice);
    if (voice->m_axVoice != NULL) {
        voice->m_flags &= 0xBFFFFFFF;
        fn_80386348(voice->m_axVoice, 0);
        lbl_805F1404->m_axVoiceCount++;
    } else {
        OSReport("AX SDLIB: AXAcquireVoice() failed in drop callback\n");
    }
}

/* Clears the 64 voices and acquires an AX voice for each one up to the configured limit. */
void fn_8043D61C(void) {
    int i;
    int j;
    int count;
    fn_80384978(0);
    memset(lbl_805F1404->m_voices, 0, sizeof(lbl_805F1404->m_voices));
    for (j = 0; j < 64; j++) {
        lbl_805F1404->m_voices[j].m_index = j;
    }
    count = lbl_805F1404->m_voiceLimit;
    if (count > 64) {
        count = 64;
    }
    for (i = 0; i < count; i++) {
        lbl_805F1404->m_voices[i].m_axVoice =
            fn_80383C48(0x1B, fn_8043D6FC, &lbl_805F1404->m_voices[i]);
        if (lbl_805F1404->m_voices[i].m_axVoice == NULL) {
            break;
        }
        lbl_805F1404->m_axVoiceCount++;
    }
}

static inline GcaxVoice *FindFreeVoice(void) {
    int i;
    for (i = 0; i < 64; i++) {
        GcaxVoice *voice;
        lbl_805F1404->m_nextVoice++;
        lbl_805F1404->m_nextVoice %= 64;
        voice = &lbl_805F1404->m_voices[lbl_805F1404->m_nextVoice];
        if ((voice->m_flags & 0x80000000) != 0 && (voice->m_flags & 0x40000000) != 0) {
            GcaxAxVoice *axVoice = fn_80383C48(0x1B, fn_8043D6FC, voice);
            if (axVoice != NULL) {
                lbl_805F1404->m_axVoiceCount++;
                voice->m_axVoice = axVoice;
                voice->m_flags &= 0xBFFFFFFF;
                return voice;
            }
        }
        if ((voice->m_flags & 0x80000000) == 0 && voice->m_axVoice != NULL) {
            return voice;
        }
    }
    return NULL;
}

/* Takes the next free voice (round robin) for a group. */
GcaxVoice *fn_8043D4D8(GcaxGroup *group, u32 arg2, u32 arg3) {
    GcaxVoice *voice = FindFreeVoice();
    if (voice != NULL) {
        voice->field_0x10 = 1.0f;
        voice->m_group = group;
        voice->field_0x8 = arg2;
        voice->field_0xC = arg3;
        voice->m_flags |= 0x80000000;
        voice->m_flags &= 0xFFFFFFFE;
        voice->m_flags &= 0xFFFFFFFD;
        voice->field_0x104 = 0;
    }
    return voice;
}

int fn_8043D49C(void) {
    int count = 0;
    if (((u8 *)lbl_805F1404) != NULL) {
        int i;
        for (i = 0; i < 64; i++) {
            if (*(int *)(((u8 *)lbl_805F1404) + i * 0x10C + 0x54) != 0) {
                count++;
            }
        }
    }
    return count;
}

float fn_8043D3BC(int first, int second, int third) {
    double result = 90316.8;
    int value;
    int steps;
    if (second < 0) {
        first++;
        second += 0x80;
    }
    value = first - third;
    if (value >= 0) {
        int count = value / 0xC;
        while (value >= 0xC) {
            result *= 0.5;
            value -= 0xC;
            count--;
        }
        while (value > 0) {
            result /= 1.0594630943592953;
            value--;
        }
    } else {
        int count;
        value = -value;
        count = value / 0xC;
        while (value >= 0xC) {
            result *= 2.0;
            value -= 0xC;
            count--;
        }
        while (value > 0) {
            result *= 1.0594630943592953;
            value--;
        }
    }
    steps = (second - 1) >> 4;
    while (second > 0x10) {
        result *= 1.007246412223704;
        second -= 0x10;
        steps--;
    }
    while (second > 0) {
        result *= 1.0004513695322617;
        second--;
    }
    return (float)result;
}

float fn_8043D318(int value) {
    double result = 1.0;
    if (value > 0) {
        int count = (int)((u32)value >> 8);
        while (value >= 0x100) {
            result *= 1.0594630943592953;
            value -= 0x100;
            count--;
        }
        count = (int)((u32)(value + 1) >> 1);
        while (value > 0) {
            result *= 1.0004513695322617;
            value -= 2;
            count--;
        }
    }
    if (value < 0) {
        int count = (int)((u32)-value >> 8);
        while (value <= -0x100) {
            result /= 1.0594630943592953;
            value += 0x100;
            count--;
        }
        count = (int)((u32)(1 - value) >> 1);
        while (value < 0) {
            result /= 1.0004513695322617;
            value += 2;
            count--;
        }
    }
    return (float)result;
}

u32 fn_8043D20C(int value, float factor) {
    float scale = 1.0f;
    int whole;
    int negative = 0;
    int remainder;
    int count;
    int steps;
    u32 result;
    if (value < 0) {
        negative = 1;
        value = -value;
    }
    whole = value / 0x1FFF;
    remainder = ((value % 0x1FFF) << 7) / 0x1FFE;
    count = whole / 0xC;
    while (whole >= 0xC) {
        scale *= 2.0;
        whole -= 0xC;
    }
    while (whole > 0) {
        scale *= 1.0594630943592953;
        whole--;
    }
    steps = (remainder - 1) >> 4;
    while (remainder > 0x10) {
        scale *= 1.007246412223704;
        remainder -= 0x10;
        steps--;
    }
    while (remainder > 0) {
        scale *= 1.0004513695322617;
        remainder--;
    }
    factor = negative ? factor / scale : factor * scale;
    result = (u32)factor;
    if (result < 0x200) {
        result = 0x200;
    }
    if (result > 0x40000) {
        result = 0x3FFFF;
    }
    return result;
}

/* Starts a voice: the region's sample, its pitch, envelope and mix. NonMatching draft. */
void fn_8043CA7C(u32 bank, GcaxVoice *object, u32 groupEntry, u32 table, u32 entry, int value,
                 u32 firstBase, u32 data) {
    u32 *params = (u32 *)object;
    u8 *entryPtr = (u8 *)entry;
    u8 *tablePtr = (u8 *)table;
    u8 *groupPtr = (u8 *)groupEntry;
    u8 *firstPtr = (u8 *)firstBase;
    u8 *slot;
    u8 *owner;
    u32 base;
    u32 selector;
    int pan;
    u32 *pitchEntry;
    int pitch;
    u32 pitchScale;
    double f31;
    float pitchTerm;
    int scaled;
    double curve;
    int key;
    int index;
    double inverse;
    int curved;
    int curveBase;
    int curveIndex;

    params[0x39] = bank;
    params[0x3A] = groupEntry;
    params[0x3B] = table;
    params[0x3C] = entry;
    params[0x3D] = value;
    params[0x3E] = firstBase;
    params[0x3F] = data;

    slot = tablePtr + (*(u32 *)groupPtr & 0xFFFFFF);
    slot += entryPtr[0] * 0x50;
    owner = (u8 *)params[0x42];
    base = *(u32 *)(owner + 0xC);
    selector = owner[2];

    pan = value;
    if (pan > entryPtr[6]) pan = entryPtr[6];
    if (pan > entryPtr[7]) pan = entryPtr[7];
    if (pan < 0) pan = 0;
    if (pan > 0x7F) pan = 0x7F;
    pan = firstPtr[entryPtr[5] * 0x80 + pan];
    if (pan < 0) pan = 0;
    if (pan > 0x7F) pan = 0x7F;
    params[0x40] = pan;

    params[0x08] = entryPtr[0xA];
    params[0x09] = s_regionPans[entryPtr[0xB] & 0x1F];
    params[0x0A] = entryPtr[0x16];
    *(float *)(params + 4) = 1.0f;
    *(float *)(params + 5) = fn_8043D3BC(entryPtr[0xC], entryPtr[0xD], value);
    params[0x06] = groupPtr[4];
    params[0x07] = groupPtr[5];

    pitchEntry = (u32 *)(base + selector * 0x48);
    pitch = (int)pitchEntry[0x34];
    if (pitch >= 0) {
        pitch *= (int)params[0x06];
    } else {
        pitch *= (int)params[0x07];
    }
    pitchScale = fn_8043D20C(pitch, *(float *)(params + 5));
    params[0x37] = entryPtr[0xE];
    params[0x38] = entryPtr[0xF];
    ((short *)params)[0x24] = (short)(pitchScale >> 0x10);
    ((short *)params)[0x25] = (short)pitchScale;
    ((short *)params)[0x26] = 0;
    ((short *)params)[0x27] = 0;
    ((short *)params)[0x28] = 0;
    ((short *)params)[0x29] = 0;
    ((short *)params)[0x2A] = 0;

    f31 = fn_803B1C9C(2.0);
    pitchTerm = (float)pitchScale / 65536.0f;
    scaled = (int)(0.5f + 32000.0f * pitchTerm);
    curve = fn_803B1C9C((double)scaled / 172.265625);
    key = (int)(curve / f31);
    index = (key - 8) & 0xF;
    inverse = 172.265625 / pow((double)-key, 2.0);
    if (index > 8) index = -(0x10 - index);
    curved = (int)(1024.0 * ((double)scaled - inverse) / inverse);
    curveBase = index + ((curved >> 9) & 1);
    curveIndex = curveBase + entryPtr[0x10] * 2 + entryPtr[0x15];
    if (curveIndex < 0) curveIndex = 0;
    if (curveIndex > 0x3F) curveIndex = 0x3F;
    *(float *)(params + 0x2D) = s_attackTimes[curveIndex];
    curveIndex = curveBase + entryPtr[0x11] * 2 + entryPtr[0x15];
    if (curveIndex < 0) curveIndex = 0;
    if (curveIndex > 0x3F) curveIndex = 0x3F;
    *(float *)(params + 0x2E) = s_decayTimes[curveIndex];
    curveIndex = curveBase + entryPtr[0x12] * 2 + entryPtr[0x15];
    if (curveIndex < 0) curveIndex = 0;
    if (curveIndex > 0x3F) curveIndex = 0x3F;
    *(float *)(params + 0x2F) = s_decayTimes[curveIndex];
    curveIndex = curveBase + entryPtr[0x14] * 2 + entryPtr[0x15];
    if (curveIndex < 0) curveIndex = 0;
    if (curveIndex > 0x3F) curveIndex = 0x3F;
    *(float *)(params + 0x30) = s_decayTimes[curveIndex];

    if (*(float *)(params + 0x2D) < 5.0f) {
        *(float *)(params + 0x32) = 1.0f;
    } else {
        *(float *)(params + 0x32) = 5.0f / *(float *)(params + 0x2D);
    }
    params[0x36] = 0;
    if (*(float *)(params + 0x32) >= 1.0f) {
        *(float *)(params + 0x32) = 1.0f;
        *(float *)(params + 0x2B) = 1.0f;
        *(float *)(params + 0x2C) = 0.0f;
        params[0x36] = 1;
    } else {
        *(float *)(params + 0x2B) = 0.0f;
        *(float *)(params + 0x2C) = -96.0f;
    }
    *(float *)(params + 0x33) = 5.0f / *(float *)(params + 0x2E);
    *(float *)(params + 0x34) = 5.0f / *(float *)(params + 0x2F);
    *(float *)(params + 0x35) = 5.0f / *(float *)(params + 0x30);
    if (entryPtr[0x13] == 0) {
        *(float *)(params + 0x31) = 0.0f;
    } else {
        float delta = -(float)(0x1F - entryPtr[0x13]);
        *(float *)(params + 0x31) = fn_8043B3A0(1.5f * delta);
    }
    *(float *)(params + 0x2A) = 0.0f;
    fn_803894BC(params[0xD], 0, 0, -0x3C, -0x3C, 0x40, 0, 0);

    switch (slot[0x4A]) {
    case 0: {
        u32 bankValue = lbl_805F1404->m_aramBanks[bank].m_address;
        u32 sum = *(u32 *)(slot + 0x4C) + bankValue;
        u32 sum1 = sum * 2 + *(u32 *)(slot + 0x10);
        u32 sum2 = sum * 2 + *(u32 *)(slot + 0x14);
        u32 sum3 = sum * 2 + *(u32 *)(slot + 0x18);
        ((short *)params)[0x1C] = *(u16 *)(slot + 0xC);
        ((short *)params)[0x1D] = 0;
        ((short *)params)[0x1E] = (short)(sum1 >> 0x10);
        ((short *)params)[0x1F] = (short)sum1;
        ((short *)params)[0x20] = (short)(sum2 >> 0x10);
        ((short *)params)[0x21] = (short)sum2;
        ((short *)params)[0x22] = (short)(sum3 >> 0x10);
        ((short *)params)[0x23] = (short)sum3;
        memcpy(params + 0x16, slot + 0x1C, 0x20);
        ((short *)params)[0x3B] = *(u16 *)(slot + 0x3C);
        ((short *)params)[0x3C] = *(u16 *)(slot + 0x3E);
        ((short *)params)[0x3D] = *(u16 *)(slot + 0x40);
        ((short *)params)[0x3E] = *(u16 *)(slot + 0x42);
        ((short *)params)[0x3F] = *(u16 *)(slot + 0x44);
        ((short *)params)[0x40] = *(u16 *)(slot + 0x46);
        ((short *)params)[0x41] = *(u16 *)(slot + 0x48);
        fn_803865F8(params[0xD]);
        fn_80386284(params[0xD], 1);
        fn_803867AC(params[0xD], params + 0x12);
        fn_80386708(params[0xD], params + 0x16);
        fn_803868DC(params[0xD], params + 0x1F);
        fn_8043C2A4((GcaxVoice *)params, (GcaxPlayer *)owner);
        if (*(u32 *)slot >= 3) {
            fn_80386348((GcaxAxVoice *)params[0xD], 1);
        }
        *params |= 2;
        break;
    }
    case 0xA: {
        u32 bankValue = lbl_805F1404->m_aramBanks[bank].m_address;
        u32 sum1 = *(u32 *)(slot + 0x4C) + *(u32 *)(slot + 0x10) * 2 + bankValue;
        u32 sum2 = *(u32 *)(slot + 0x4C) + *(u32 *)(slot + 0x14) * 2 + bankValue;
        u32 sum3 = *(u32 *)(slot + 0x4C) + *(u32 *)(slot + 0x18) * 2 + bankValue;
        if (*(u16 *)(slot + 0xC) == 0) {
            sum1 = lbl_805F1404->m_aramZero;
        }
        ((short *)params)[0x1C] = *(u16 *)(slot + 0xC);
        ((short *)params)[0x1D] = 0xA;
        ((short *)params)[0x1E] = (short)(sum1 >> 0x11);
        ((short *)params)[0x1F] = (short)(sum1 >> 1);
        ((short *)params)[0x20] = (short)(sum2 >> 0x11);
        ((short *)params)[0x21] = (short)(sum2 >> 1);
        ((short *)params)[0x22] = (short)(sum3 >> 0x11);
        ((short *)params)[0x23] = (short)(sum3 >> 1);
        fn_803865F8(params[0xD]);
        fn_80386284(params[0xD], 1);
        fn_803867AC(params[0xD], params + 0x12);
        fn_8043C2A4((GcaxVoice *)params, (GcaxPlayer *)owner);
        fn_80386348((GcaxAxVoice *)params[0xD], 1);
        *params |= 2;
        break;
    }
    case 0x19: {
        u32 bankValue = lbl_805F1404->m_aramBanks[bank].m_address;
        u32 sum1 = *(u32 *)(slot + 0x4C) + bankValue + *(u32 *)(slot + 0x10);
        u32 sum2 = *(u32 *)(slot + 0x4C) + bankValue + *(u32 *)(slot + 0x14);
        u32 sum3 = *(u32 *)(slot + 0x4C) + bankValue + *(u32 *)(slot + 0x18);
        if (*(u16 *)(slot + 0xC) == 0) {
            sum1 = lbl_805F1404->m_aramZero;
        }
        ((short *)params)[0x1C] = *(u16 *)(slot + 0xC);
        ((short *)params)[0x1D] = 0x19;
        ((short *)params)[0x1E] = (short)(sum1 >> 0x10);
        ((short *)params)[0x1F] = (short)sum1;
        ((short *)params)[0x20] = (short)(sum2 >> 0x10);
        ((short *)params)[0x21] = (short)sum2;
        ((short *)params)[0x22] = (short)(sum3 >> 0x10);
        ((short *)params)[0x23] = (short)sum3;
        fn_803865F8(params[0xD]);
        fn_80386284(params[0xD], 1);
        fn_803867AC(params[0xD], params + 0x12);
        fn_8043C2A4((GcaxVoice *)params, (GcaxPlayer *)owner);
        fn_80386348((GcaxAxVoice *)params[0xD], 1);
        *params |= 2;
        break;
    }
    default:
        break;
    }
}

/* Counts the regions of a program that cover the key. */
int fn_8043C9D0(int bank, u32 program, int key) {
    int count = 0;
    GcaxProgramBank *table = (GcaxProgramBank *)lbl_805F1404->m_programBanks[bank].m_data;
    if (table != NULL && GCAX_REF_COUNT(table->m_programs) > program) {
        GcaxTableRef *programs = (GcaxTableRef *)((u8 *)table + GCAX_REF_OFFSET(table->m_programs));
        GcaxTableRef layerRef = programs[program];
        GcaxLayer *layers = (GcaxLayer *)((u8 *)table + GCAX_REF_OFFSET(layerRef));
        int i;
        for (i = 0; i < (int)GCAX_REF_COUNT(layerRef); i++) {
            GcaxTableRef regionRef = layers[i].m_regions;
            GcaxRegion *regions = (GcaxRegion *)((u8 *)table + GCAX_REF_OFFSET(regionRef));
            int j;
            for (j = 0; j < (int)GCAX_REF_COUNT(regionRef); j++) {
                if (regions[j].m_keyLow <= key && key <= regions[j].m_keyHigh) {
                    count++;
                }
            }
        }
    }
    return count;
}

/* Starts one voice of the list for every region of the program that covers the key. */
void fn_8043C89C(GcaxListNode **list, int bank, u32 program, int key, u32 data) {
    GcaxListNode *node = NULL;
    GcaxProgramBank *table = (GcaxProgramBank *)lbl_805F1404->m_programBanks[bank].m_data;
    if (table != NULL && GCAX_REF_COUNT(table->m_programs) > program) {
        GcaxTableRef *programs = (GcaxTableRef *)((u8 *)table + GCAX_REF_OFFSET(table->m_programs));
        GcaxTableRef layerRef = programs[program];
        u8 *first = (u8 *)table + GCAX_REF_OFFSET(table->m_first);
        int i = 0;
        int layerCount = GCAX_REF_COUNT(layerRef);
        GcaxLayer *layers = (GcaxLayer *)((u8 *)table + GCAX_REF_OFFSET(layerRef));
        while (i < layerCount) {
            GcaxLayer *layer = &layers[i];
            int j = 0;
            GcaxTableRef regionRef = layer->m_regions;
            int regionCount = GCAX_REF_COUNT(regionRef);
            GcaxRegion *regions = (GcaxRegion *)((u8 *)table + GCAX_REF_OFFSET(regionRef));
            while (j < regionCount) {
                GcaxRegion *region = &regions[j];
                if (region->m_keyLow <= key && key <= region->m_keyHigh) {
                    node = fn_8043B3DC(list, node);
                    if (node != NULL && node->m_item != NULL) {
                        fn_8043CA7C(bank, (GcaxVoice *)node->m_item, (u32)layer, (u32)table,
                                    (u32)region, key, (u32)first, data);
                    }
                }
                j++;
            }
            i++;
        }
    }
}

/* Works out a voice's AX mix (main, aux A, aux B / surround) from the channel's volume,
 * expression and pan, the voice's velocity and envelope, and the player's volume. */
void fn_8043C2A4(GcaxVoice *voice, GcaxPlayer *player) {
    int channel;
    u32 product;
    int volume;
    int scaled;
    int index;
    int steps;
    float decibels;
    float gain;
    int pan;
    float right;
    float left;
    int rightStep;
    float sendScale;
    float sendDecibels;
    float send;
    float sendLeft;
    float sendRight;
    float auxLeft;
    float auxRight;
    if ((player->m_flags & 0x80000000) == 0) {
        return;
    }
    channel = voice->m_group->m_channel;
    voice->m_mix.vL = 0;
    voice->m_mix.vR = 0;
    voice->m_mix.vAuxAL = 0;
    voice->m_mix.vAuxAR = 0;
    voice->m_mix.vAuxBL = 0;
    voice->m_mix.vAuxBR = 0;
    voice->m_mix.vAuxBS = 0;
    voice->m_mix.vS = 0;
    voice->m_mix.vAuxAS = 0;
    voice->m_mix.vDeltaL = 0;
    voice->m_mix.vDeltaR = 0;
    voice->m_mix.vDeltaAuxAL = 0;
    voice->m_mix.vDeltaAuxAR = 0;
    voice->m_mix.vDeltaAuxBL = 0;
    voice->m_mix.vDeltaAuxBR = 0;
    voice->m_mix.vDeltaAuxBS = 0;
    voice->m_mix.vDeltaS = 0;
    voice->m_mix.vDeltaAuxAS = 0;

    product = (player->m_channels[channel].m_volume + 1) *
              (player->m_channels[channel].m_expression + 1);
    {
        int reduced = product - 1;
        int clamped = reduced / 128;
        if (clamped < 0) {
            clamped = 0;
        }
        if (clamped > 0x7F) {
            clamped = 0x7F;
        }
        volume = clamped + 1;
    }
    scaled = volume * voice->m_velocity * (0x100 - voice->field_0x28);
    index = (int)(255.0f * player->m_volume) + (((scaled - 1) >> 14) & 0xFF);
    if (index < 0) {
        index = 0;
    }
    if (index > 0xFF) {
        index = 0xFF;
    }
    steps = s_volumeSteps[index] - 3;
    if (steps > 0xFF) {
        steps = 0xFF;
    }

    decibels = 0.0f;
    GCAX_ADD_LEVEL_DECIBELS(decibels, voice->field_0x20);
    decibels += lbl_805F1404->m_masterDecibels;
    if (steps & 0x01) {
        decibels += -0.375f;
    }
    if (steps & 0x02) {
        decibels += -0.75f;
    }
    if (steps & 0x04) {
        decibels += -1.5f;
    }
    if (steps & 0x08) {
        decibels += -3.0f;
    }
    if (steps & 0x10) {
        decibels += -6.0f;
    }
    if (steps & 0x20) {
        decibels += -12.0f;
    }
    if (steps & 0x40) {
        decibels += -24.0f;
    }
    if (steps & 0x80) {
        decibels += -48.0f;
    }
    decibels += voice->m_decibels;
    gain = fn_8043B3A0(decibels);

    pan = voice->m_pan + player->m_channels[channel].m_pan - 0x40;
    if (pan < 0) {
        pan = 0;
    }
    if (pan > 0x7F) {
        pan = 0x7F;
    }
    pan = s_panSteps[pan];
    rightStep = pan ^ 0x10;
    left = s_panGains[pan];
    right = s_panGains[rightStep];
    if (lbl_805F1404->m_mono != 0) {
        float both = 0.707f * (left + right);
        if (both > 1.0f) {
            both = 1.0f;
        }
        right = both;
        left = both;
    }
    voice->m_panLeft = left;
    left *= gain;
    voice->m_panRight = right;
    right *= gain;
    player->m_channels[channel].m_gainLeft = left;
    player->m_channels[channel].m_gainRight = right;

    if (lbl_805F1404->m_surround == 0) {
        voice->m_mix.vL = (int)(32768.0f * (voice->field_0x10 * player->m_channels[channel].m_gainLeft));
        voice->m_mix.vR = (int)(32768.0f * (voice->field_0x10 * player->m_channels[channel].m_gainRight));
        voice->m_mix.vAuxBL = 0;
        voice->m_mix.vAuxBR = 0;
    } else {
        float mainLeft;
        float mainRight;
        int span = player->m_channels[channel].field_0x24;
        if ((float)span < 0.0f) {
            mainLeft = voice->field_0x10 * player->m_channels[channel].m_gainLeft;
            mainRight = voice->field_0x10 * player->m_channels[channel].m_gainRight;
        } else {
            float scaledGain = voice->field_0x10 * gain;
            mainLeft = scaledGain * s_spanGains[0x7F - span];
            mainRight = scaledGain * s_spanGains[span];
        }
        if (player->m_channels[channel].m_depth <= 0x40) {
            voice->m_mix.vL = (int)(32768.0f * mainLeft);
            voice->m_mix.vR = (int)(32768.0f * mainRight);
            voice->m_mix.vAuxBL = 0;
            voice->m_mix.vAuxBR = 0;
        } else {
            voice->m_mix.vL = 0;
            voice->m_mix.vR = 0;
            voice->m_mix.vAuxBL = (int)(32768.0f * mainLeft);
            voice->m_mix.vAuxBR = (int)(32768.0f * mainRight);
        }
    }

    sendScale = -2.0f;
    if (voice->field_0xDC == 0) {
        sendScale = 1.0f;
    }
    sendDecibels = 0.0f;
    GCAX_ADD_LEVEL_DECIBELS(sendDecibels, voice->field_0xE0);
    send = sendScale * fn_8043B3A0(sendDecibels) + player->field_0x3C;
    if (send < 0.0f) {
        send = 0.0f;
    }
    if (send > 1.0f) {
        send = 1.0f;
    }
    sendLeft = left * send;
    sendRight = right * send;
    auxLeft = 32768.0f * sendLeft;
    auxRight = 32768.0f * sendRight;
    voice->m_mix.vAuxAL = (int)auxLeft;
    voice->m_mix.vAuxAR = (int)auxRight;
    if (lbl_805F1404->m_surround == 0) {
        voice->m_mix.vAuxAL = (int)(32768.0f * sendLeft);
        voice->m_mix.vAuxAR = (int)(32768.0f * sendRight);
        voice->m_mix.vAuxAS = 0;
        voice->m_mix.vAuxBS = 0;
    } else {
        float front = (127.0f - (float)player->m_channels[channel].m_depth) / 127.0f;
        float frontLeft = 32768.0f * (front * sendLeft);
        float frontRight = 32768.0f * (front * sendRight);
        voice->m_mix.vAuxAL = (int)frontLeft;
        voice->m_mix.vAuxAR = (int)frontRight;
        /* Retail stores both to vAuxAS. */
        voice->m_mix.vAuxAS = (int)(32768.0f * (front * sendLeft));
        voice->m_mix.vAuxAS = (int)(32768.0f * (front * sendRight));
    }
    fn_803863A4(voice->m_axVoice, &voice->m_mix);
}

/* Sets a voice's sample rate ratio for a pitch bend: the bend times the voice's bend range gives
 * 1/8192 semitones, applied as octaves, semitones, 1/8 semitones and 1/128 semitones. */
void fn_8043C170(GcaxVoice *voice, int bend, float scale) {
    int pitch;
    float ratio;
    int magnitude;
    int negative;
    int semitones;
    int fraction;
    float rate;
    if (bend >= 0) {
        pitch = bend * voice->m_bendRangeUp;
    } else {
        pitch = bend * voice->m_bendRangeDown;
    }
    ratio = 1.0f;
    magnitude = pitch;
    negative = 0;
    if (pitch < 0) {
        negative = 1;
        magnitude = -pitch;
    }
    semitones = magnitude / 0x2000;
    fraction = ((magnitude % 0x2000) << 7) / 0x2000;
    while (semitones >= 12) {
        ratio *= 2.0;
        semitones -= 12;
    }
    while (semitones > 0) {
        ratio *= 1.0594630943592953;
        semitones--;
    }
    while (fraction > 16) {
        ratio *= 1.007246412223704;
        fraction -= 16;
    }
    while (fraction > 0) {
        ratio *= 1.0004513695322617;
        fraction--;
    }
    if (negative) {
        ratio = 1.0f / ratio;
    }
    rate = ratio * voice->m_baseRatio;
    rate /= 65536.0f;
    rate *= scale;
    fn_80386844(voice->m_axVoice, rate);
}

GcaxListNode *fn_8043C12C(GcaxListNode **list, void *item) {
    GcaxListNode *node = (GcaxListNode *)fn_8043B560(sizeof(GcaxListNode), list, NULL);
    if (node != NULL) {
        node->m_item = item;
    }
    return node;
}

GcaxListNode *fn_8043C10C(GcaxListNode **list, GcaxListNode *node) {
    return fn_8043B3DC(list, node);
}

/* List callback: silences the node's voice and marks it free. */
void fn_8043C084(GcaxListNode *node) {
    GcaxVoice *voice;
    if (node == NULL) {
        return;
    }
    voice = (GcaxVoice *)node->m_item;
    if (voice == NULL) {
        return;
    }
    voice->field_0x104 = 0;
    if (voice->m_axVoice != NULL) {
        fn_80386348(voice->m_axVoice, 0);
        fn_8038A338(voice->m_axVoice);
    }
    voice->m_flags &= 0x7FFFFFFF;
    voice->m_flags &= 0xFFFFFFFE;
    voice->m_flags &= 0xFFFFFFFD;
    voice->field_0x8 = 0;
}

void fn_8043C05C(GcaxListNode **list) {
    fn_8043B44C(list, (void (*)(void *))fn_8043C084);
}

static inline void SetLevel(GcaxVoice *voice, float level) {
    voice->m_level = level;
    voice->m_decibels = -(96.0f - 96.0f * voice->m_level);
}

/* Steps a voice's volume envelope: attack, decay to the sustain level, sustain, release. */
void fn_8043BE6C(GcaxVoice *voice) {
    if (voice->m_flags & 1) {
        return;
    }
    switch (voice->m_state) {
    case 0:
        SetLevel(voice, voice->m_level + voice->m_attackRate);
        if (voice->m_level >= 1.0f) {
            SetLevel(voice, 1.0f);
            voice->m_state++;
        }
        break;
    case 1:
        if (voice->m_decibels <= voice->m_sustainDecibels) {
            voice->m_state++;
        } else {
            SetLevel(voice, voice->m_level - voice->m_decayRate);
            if (voice->m_level <= 0.0f) {
                SetLevel(voice, 0.0f);
            }
            if (voice->m_decibels <= voice->m_sustainDecibels) {
                voice->m_state++;
            } else {
                break;
            }
        }
    case 2:
        voice->m_state++;
    case 3:
        SetLevel(voice, voice->m_level - voice->m_sustainRate);
        if (voice->m_level <= 0.0f) {
            SetLevel(voice, 0.0f);
        }
        break;
    case 4:
        voice->m_state++;
    case 5:
        SetLevel(voice, voice->m_level - voice->m_releaseRate);
        if (voice->m_level <= 0.0f) {
            SetLevel(voice, 0.0f);
        }
        break;
    }
    voice->m_time += 5.0f;
}

/* Pauses a playing voice. */
void fn_8043BE04(GcaxVoice *voice) {
    u32 flags;
    if (voice == NULL) {
        return;
    }
    if (voice->m_axVoice == NULL) {
        return;
    }
    flags = voice->m_flags;
    if ((flags & 0x80000000) != 0 && (flags & 0x40000000) == 0) {
        voice->m_flags = flags | 1;
        fn_80386348(voice->m_axVoice, 0);
        fn_8038A338(voice->m_axVoice);
    }
}

/* Restarts a paused voice with the parameters it was started with. */
void fn_8043BD80(GcaxVoice *voice) {
    if (voice == NULL) {
        return;
    }
    if (voice->m_axVoice == NULL) {
        return;
    }
    if ((voice->m_flags & 0x80000000) != 0 && (voice->m_flags & 1) != 0) {
        if ((voice->m_flags & 0x40000000) == 0) {
            fn_8043CA7C(voice->field_0xE4, voice, voice->field_0xE8, voice->field_0xEC,
                        voice->field_0xF0, voice->field_0xF4, voice->field_0xF8, voice->field_0xFC);
        }
        voice->m_flags &= 0xFFFFFFFE;
    }
}

static inline int CountGroupVoices(GcaxGroup *group) {
    int count = 0;
    if (group != NULL) {
        GcaxListNode *node = NULL;
        while ((node = fn_8043B3DC(&group->m_voices, node)) != NULL) {
            if (node->m_item != NULL) {
                count++;
            }
        }
    }
    return count;
}

/* Frees the first group in use, from the rotating start index on, whose priority is not above
 * the given one. */
GcaxGroup *fn_8043BCDC(int priority) {
    int i;
    for (i = 0; i < 64; i++) {
        int index = (lbl_805F1404->m_nextGroup + i) % 64;
        if (lbl_805F1404->m_groups[index].m_used != 0 &&
            lbl_805F1404->m_groups[index].m_priority <= priority) {
            fn_8043BB54(&lbl_805F1404->m_groups[index]);
            lbl_805F1404->m_nextGroup = index + 1;
            return &lbl_805F1404->m_groups[index];
        }
    }
    return NULL;
}

/* The same, until the freed groups held at least `count` voices. */
GcaxGroup *fn_8043BBC8(int priority, int count) {
    int i;
    GcaxGroup *result = NULL;
    int remaining;
    if (count <= 0) {
        return fn_8043BCDC(priority);
    }
    remaining = count;
    while (remaining > 0) {
        for (i = 0; i < 64; i++) {
            int index = (lbl_805F1404->m_nextGroup + i) % 64;
            if (lbl_805F1404->m_groups[index].m_used != 0 &&
                lbl_805F1404->m_groups[index].m_priority <= priority) {
                remaining -= CountGroupVoices(&lbl_805F1404->m_groups[index]);
                fn_8043BB54(&lbl_805F1404->m_groups[index]);
                lbl_805F1404->m_nextGroup = index + 1;
                result = &lbl_805F1404->m_groups[index];
                break;
            }
        }
        if (remaining == count) {
            break;
        }
    }
    return result;
}

void fn_8043BB54(GcaxGroup *group) {
    GcaxListNode **list;
    GcaxListNode *found;
    if (group == NULL) {
        return;
    }
    list = &group->m_owner->m_groups;
    if (*list == NULL) {
        return;
    }
    lbl_805F1404->m_currentGroup = group;
    found = fn_8043B3F4(list, fn_8043B810);
    if (found != NULL) {
        fn_8043B4B0(found, list, fn_8043B89C);
    }
}

int fn_8043BB18(void) {
    int count = 0;
    if (lbl_805F1404 != NULL) {
        int i;
        for (i = 0; i < 64; i++) {
            if (lbl_805F1404->m_groups[i].m_used != 0) {
                count++;
            }
        }
    }
    return count;
}

static inline GcaxGroup *FindFreeGroup(GcaxGlobals *globals) {
    int index;
    int i;
    for (i = 0; i < 64; i++) {
        index = (globals->m_nextGroup + i) % 64;
        if (globals->m_groups[index].m_used == 0) {
            globals->m_nextGroup = index + 1;
            return &lbl_805F1404->m_groups[index];
        }
    }
    return NULL;
}

/* Starts a note: takes a group (a free one, or one stolen by priority), gives it one voice per
 * region of the channel's program that covers the key, and starts them. */
GcaxGroup *fn_8043B954(GcaxPlayer *owner, int channel, int key, int velocity, int priority) {
    GcaxGroup *group = NULL;
    int count;
    int bank;
    u32 program;
    int active;
    GcaxGlobals *globals;
    int missing;
    int i;
    program = owner->m_channels[channel].m_program;
    bank = owner->m_channels[channel].m_bank;
    count = fn_8043C9D0(bank, program, key);
    if (count <= 0) {
        return NULL;
    }
    active = fn_8043D49C();
    globals = lbl_805F1404;
    missing = count - (globals->m_voiceLimit - active);
    if (missing < 0) {
        missing = 0;
    }
    if (missing != 0) {
        group = fn_8043BBC8(priority, missing);
        if (group == NULL) {
            return NULL;
        }
    } else {
        group = FindFreeGroup(globals);
        if (group == NULL) {
            group = fn_8043BCDC(priority);
            if (group == NULL) {
                return NULL;
            }
        }
    }
    group->m_owner = owner;
    group->m_key = key;
    group->m_channel = channel;
    group->m_velocity = velocity;
    group->m_priority = priority;
    group->m_voiceCount = count;
    group->m_used = 1;
    group->m_voices = NULL;
    for (i = 0; i < count; i++) {
        GcaxVoice *voice = fn_8043D4D8(group, 5, 0);
        if (voice == NULL) {
            fn_8043BB54(group);
            return NULL;
        }
        if (fn_8043C12C(&group->m_voices, voice) == NULL) {
            fn_8043BB54(group);
            return NULL;
        }
    }
    fn_8043C89C(&group->m_voices, bank, program, key, velocity);
    return group;
}
