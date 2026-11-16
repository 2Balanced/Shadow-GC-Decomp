/* Dolphin SDK MIX: the mixer layered on AX. Every AX voice has a channel record holding its
 * settings in the mixer's own units (input, auxiliary sends and fader in tenths of a dB, pan and
 * surround pan 0..127); the mixer turns those into the DSP's linear mix levels.
 */
#include <dolphin/os.h>
#include <dolphin/ax.h>

/* Placeholder symbols are kept: nothing in the binary names these functions, and other units
 * call them by these symbols. The names are the public MIX API they correspond to. */
#define __MIXSetPan fn_8038932C
#define MIXInit fn_803893E8
#define MIXQuit fn_803894B0
#define MIXSetSoundMode fn_803894B4
#define MIXInitChannel fn_803894BC
#define MIXReleaseChannel fn_8038A338
#define MIXSetInput fn_8038A358
#define MIXSetPan fn_8038A380
#define MIXUpdateSettings fn_8038A3EC

void AISetStreamVolLeft(u8 volume);
void AISetStreamVolRight(u8 volume);

#define MIX_MAX_CHANNELS 64

#define MIX_SOUND_MODE_DPL2 3

/* Low bits of MIXChannel.mode, given by the caller. */
#define MIX_MODE_AUXA_PREFADER 0x00000001
#define MIX_MODE_AUXB_PREFADER 0x00000002
#define MIX_MODE_MUTE          0x00000004

/* Bits in AXPB.mixerCtrl: the buses a voice is mixed into. */
#define AX_PB_MIXCTRL_L       0x0001
#define AX_PB_MIXCTRL_R       0x0002
#define AX_PB_MIXCTRL_S       0x0004
#define AX_PB_MIXCTRL_A_L     0x0010
#define AX_PB_MIXCTRL_A_R     0x0020
#define AX_PB_MIXCTRL_A_S     0x0080
#define AX_PB_MIXCTRL_B_L     0x0200
#define AX_PB_MIXCTRL_B_R     0x0400
#define AX_PB_MIXCTRL_B_S     0x1000
#define AX_PB_MIXCTRL_B_DPL2  0x4000
#define AX_PB_MIXCTRL_RAMP     0x0008
#define AX_PB_MIXCTRL_A_RAMP   0x0040
#define AX_PB_MIXCTRL_A_S_RAMP 0x0100
#define AX_PB_MIXCTRL_B_RAMP   0x0800
#define AX_PB_MIXCTRL_B_S_RAMP 0x2000

/* AXVPB.sync: mixer control, mix levels and volume envelope changed. */
#define MIX_SYNC_FLAGS 0x212

/* Bits in MIXChannel.mode: which settings changed since the last update. */
#define MIX_MODE_UPDATE_INPUT 0x10000000
#define MIX_MODE_UPDATE_MIX   0x40000000
#define MIX_MODE_DEFAULT      0x50000000
/* ...and which targets are waiting to become the current levels. */
#define MIX_MODE_UPDATE_INPUT1 0x20000000
#define MIX_MODE_UPDATE_MIX1   0x80000000

/* A level change is ramped over one 5 ms audio frame: 160 samples at 32 kHz. */
#define MIX_RAMP_SAMPLES 160

typedef struct MIXChannel {
    AXVPB *axvpb; /* 0x00 */
    u32 mode;     /* 0x04 */
    int input;    /* 0x08 */
    int auxA;     /* 0x0C */
    int auxB;     /* 0x10 */
    int pan;      /* 0x14 */
    int span;     /* 0x18 */
    int fader;    /* 0x1C */
    int l;        /* 0x20 */
    int r;        /* 0x24 */
    int f;        /* 0x28 */
    int b;        /* 0x2C */
    int l1;       /* 0x30 */
    int r1;       /* 0x34 */
    /* Linear levels of the input and of the nine mix buses; the second halfword of each pair is
     * the level the DSP was last given. */
    u16 v, v1;     /* 0x38 */
    u16 vL, vL1;   /* 0x3C */
    u16 vR, vR1;   /* 0x40 */
    u16 vS, vS1;   /* 0x44 */
    u16 vAL, vAL1; /* 0x48 */
    u16 vAR, vAR1; /* 0x4C */
    u16 vAS, vAS1; /* 0x50 */
    u16 vBL, vBL1; /* 0x54 */
    u16 vBR, vBR1; /* 0x58 */
    u16 vBS, vBS1; /* 0x5C */
} MIXChannel;

/* Linear level for an attenuation of -90.4 dB .. +6.0 dB in tenths of a dB. */
u16 __MIXVolumeTable[965] = {
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    9, 9, 9, 9, 9, 9, 9, 9, 9, 10, 10, 10,
    10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11,
    11, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13,
    13, 13, 13, 14, 14, 14, 14, 14, 14, 15, 15, 15,
    15, 15, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17,
    18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 20, 20,
    20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 23, 23,
    23, 24, 24, 24, 24, 25, 25, 25, 26, 26, 26, 26,
    27, 27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 30,
    31, 31, 32, 32, 32, 33, 33, 33, 34, 34, 35, 35,
    35, 36, 36, 37, 37, 38, 38, 38, 39, 39, 40, 40,
    41, 41, 42, 42, 43, 43, 44, 44, 45, 45, 46, 46,
    47, 47, 48, 49, 49, 50, 50, 51, 51, 52, 53, 53,
    54, 55, 55, 56, 56, 57, 58, 58, 59, 60, 61, 61,
    62, 63, 63, 64, 65, 66, 66, 67, 68, 69, 70, 70,
    71, 72, 73, 74, 75, 75, 76, 77, 78, 79, 80, 81,
    82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93,
    94, 95, 96, 97, 98, 100, 101, 102, 103, 104, 106, 107,
    108, 109, 111, 112, 113, 114, 116, 117, 118, 120, 121, 123,
    124, 126, 127, 128, 130, 131, 133, 135, 136, 138, 139, 141,
    143, 144, 146, 148, 149, 151, 153, 155, 156, 158, 160, 162,
    164, 166, 168, 170, 171, 173, 175, 178, 180, 182, 184, 186,
    188, 190, 192, 195, 197, 199, 202, 204, 206, 209, 211, 214,
    216, 219, 221, 224, 226, 229, 231, 234, 237, 240, 242, 245,
    248, 251, 254, 257, 260, 263, 266, 269, 272, 275, 278, 282,
    285, 288, 292, 295, 298, 302, 305, 309, 312, 316, 320, 323,
    327, 331, 335, 339, 343, 347, 351, 355, 359, 363, 367, 371,
    376, 380, 384, 389, 393, 398, 403, 407, 412, 417, 422, 427,
    431, 436, 442, 447, 452, 457, 462, 468, 473, 479, 484, 490,
    495, 501, 507, 513, 519, 525, 531, 537, 543, 550, 556, 562,
    569, 576, 582, 589, 596, 603, 610, 617, 624, 631, 638, 646,
    653, 661, 669, 676, 684, 692, 700, 708, 716, 725, 733, 742,
    750, 759, 768, 777, 786, 795, 804, 813, 823, 832, 842, 852,
    861, 871, 881, 892, 902, 912, 923, 934, 945, 955, 967, 978,
    989, 1001, 1012, 1024, 1036, 1048, 1060, 1072, 1085, 1097, 1110, 1123,
    1136, 1149, 1162, 1176, 1189, 1203, 1217, 1231, 1245, 1260, 1274, 1289,
    1304, 1319, 1334, 1350, 1365, 1381, 1397, 1414, 1430, 1446, 1463, 1480,
    1497, 1515, 1532, 1550, 1568, 1586, 1604, 1623, 1642, 1661, 1680, 1700,
    1719, 1739, 1759, 1780, 1800, 1821, 1842, 1864, 1885, 1907, 1929, 1951,
    1974, 1997, 2020, 2043, 2067, 2091, 2115, 2140, 2164, 2190, 2215, 2241,
    2266, 2293, 2319, 2346, 2373, 2401, 2429, 2457, 2485, 2514, 2543, 2573,
    2602, 2632, 2663, 2694, 2725, 2757, 2789, 2821, 2853, 2887, 2920, 2954,
    2988, 3023, 3058, 3093, 3129, 3165, 3202, 3239, 3276, 3314, 3353, 3391,
    3431, 3470, 3511, 3551, 3592, 3634, 3676, 3719, 3762, 3805, 3849, 3894,
    3939, 3985, 4031, 4078, 4125, 4173, 4221, 4270, 4319, 4369, 4420, 4471,
    4523, 4575, 4628, 4682, 4736, 4791, 4846, 4902, 4959, 5017, 5075, 5133,
    5193, 5253, 5314, 5375, 5438, 5501, 5564, 5629, 5694, 5760, 5827, 5894,
    5962, 6031, 6101, 6172, 6243, 6316, 6389, 6463, 6538, 6613, 6690, 6767,
    6846, 6925, 7005, 7086, 7168, 7251, 7335, 7420, 7506, 7593, 7681, 7770,
    7860, 7951, 8043, 8136, 8230, 8326, 8422, 8520, 8618, 8718, 8819, 8921,
    9025, 9129, 9235, 9342, 9450, 9559, 9670, 9782, 9895, 10010, 10126, 10243,
    10362, 10482, 10603, 10726, 10850, 10976, 11103, 11231, 11361, 11493, 11626, 11761,
    11897, 12035, 12174, 12315, 12458, 12602, 12748, 12895, 13045, 13196, 13349, 13503,
    13659, 13818, 13978, 14140, 14303, 14469, 14636, 14806, 14977, 15151, 15326, 15504,
    15683, 15865, 16049, 16234, 16422, 16613, 16805, 17000, 17196, 17396, 17597, 17801,
    18007, 18215, 18426, 18640, 18856, 19074, 19295, 19518, 19744, 19973, 20204, 20438,
    20675, 20914, 21156, 21401, 21649, 21900, 22153, 22410, 22669, 22932, 23197, 23466,
    23738, 24013, 24291, 24572, 24857, 25144, 25436, 25730, 26028, 26329, 26634, 26943,
    27255, 27570, 27890, 28213, 28539, 28870, 29204, 29542, 29884, 30230, 30580, 30934,
    31293, 31655, 32022, 32392, 32767, 33147, 33531, 33919, 34312, 34709, 35111, 35518,
    35929, 36345, 36766, 37192, 37622, 38058, 38499, 38944, 39395, 39851, 40313, 40780,
    41252, 41730, 42213, 42702, 43196, 43696, 44202, 44714, 45232, 45756, 46286, 46821,
    47364, 47912, 48467, 49028, 49596, 50170, 50751, 51339, 51933, 52535, 53143, 53758,
    54381, 55011, 55648, 56292, 56944, 57603, 58270, 58945, 59627, 60318, 61016, 61723,
    62438, 63161, 63892, 64632, 65380,
};

/* Attenuation in tenths of a dB for each pan position. */
int __MIXPanTable[128] = {
    0, 0, -1, -1, -1, -2, -2, -2,
    -3, -3, -4, -4, -4, -5, -5, -5,
    -6, -6, -7, -7, -7, -8, -8, -9,
    -9, -10, -10, -10, -11, -11, -12, -12,
    -13, -13, -14, -14, -14, -15, -15, -16,
    -16, -17, -17, -18, -18, -19, -20, -20,
    -21, -21, -22, -22, -23, -23, -24, -25,
    -25, -26, -26, -27, -28, -28, -29, -30,
    -30, -31, -32, -33, -33, -34, -35, -36,
    -36, -37, -38, -39, -40, -40, -41, -42,
    -43, -44, -45, -46, -47, -48, -49, -50,
    -51, -52, -54, -55, -56, -57, -59, -60,
    -61, -63, -64, -66, -67, -69, -71, -72,
    -74, -76, -78, -80, -83, -85, -87, -90,
    -93, -96, -99, -102, -106, -110, -115, -120,
    -126, -133, -140, -150, -163, -180, -210, -904,
};

/* The same for Dolby Pro Logic II, front and rear. */
s16 __MIXPanTableDPL2Front[128] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, -1, -1, -1, -1, -1, -2, -2, -2, -2, -3, -3,
    -3, -4, -4, -4, -5, -5, -6, -6, -6, -7, -7, -8,
    -8, -9, -9, -10, -11, -11, -12, -12, -13, -14, -14, -15,
    -16, -17, -17, -18, -19, -20, -21, -21, -22, -23, -24, -25,
    -26, -27, -28, -29, -30, -31, -32, -34, -35, -36, -37, -38,
    -40, -41, -42, -44, -45, -47, -48, -50, -52, -53, -55, -57,
    -58, -60, -62, -64, -66, -68, -70, -73, -75, -77, -80, -82,
    -85, -88, -90, -93, -96, -100, -103, -106, -110, -114, -118, -122,
    -126, -131, -136, -141, -146, -152, -159, -166, -173, -181, -190, -201,
    -212, -225, -241, -261, -286, -321, -381, -960,
};

s16 __MIXPanTableDPL2Rear[128] = {
    -61, -61, -60, -59, -59, -58, -58, -57, -56, -56, -55, -55,
    -54, -53, -53, -52, -52, -51, -50, -50, -49, -49, -48, -48,
    -47, -47, -46, -46, -45, -45, -44, -44, -43, -43, -42, -42,
    -41, -41, -40, -40, -39, -39, -38, -38, -38, -37, -37, -36,
    -36, -35, -35, -35, -34, -34, -33, -33, -32, -32, -32, -31,
    -31, -31, -30, -30, -29, -29, -29, -28, -28, -28, -27, -27,
    -27, -26, -26, -26, -25, -25, -25, -24, -24, -24, -23, -23,
    -23, -22, -22, -22, -21, -21, -21, -20, -20, -20, -20, -19,
    -19, -19, -18, -18, -18, -18, -17, -17, -17, -17, -16, -16,
    -16, -16, -15, -15, -15, -15, -14, -14, -14, -14, -13, -13,
    -13, -13, -13, -12, -12, -12, -12, -11,
};

u8 __MIXAIVolumeTable[50] = {
    0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 4, 4, 5, 5, 6, 7, 8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 25, 28, 32, 36, 40, 45, 50, 57, 64, 71, 80, 90, 101, 113, 127, 143, 160, 180, 202, 227, 255
};

MIXChannel __MIXChannel[MIX_MAX_CHANNELS];

/* Small data comes out in reverse order of definition. */
u32 __MIXSoundMode;
int __MIXDvdStreamAttenUser;
int __MIXDvdStreamAttenCurrent;

void __MIXSetPan(MIXChannel *channel) {
    int pan = channel->pan;
    int span = channel->span;
    int ipan = 127 - pan;
    int ispan = 127 - span;

    if (__MIXSoundMode == MIX_SOUND_MODE_DPL2) {
        channel->l = __MIXPanTableDPL2Front[pan];
        channel->r = __MIXPanTableDPL2Front[ipan];
        channel->f = __MIXPanTableDPL2Front[ispan];
        channel->b = __MIXPanTableDPL2Front[span];
        channel->l1 = __MIXPanTableDPL2Rear[ipan];
        channel->r1 = __MIXPanTableDPL2Rear[pan];
    } else {
        channel->l = __MIXPanTable[pan];
        channel->r = __MIXPanTable[ipan];
        channel->f = __MIXPanTable[ispan];
        channel->b = __MIXPanTable[span];
    }
}

void MIXInit(void) {
    int i;

    for (i = 0; i < MIX_MAX_CHANNELS; i++) {
        /* Indexed each time: through a pointer the compiler copies it differently for the call. */
        __MIXChannel[i].mode = MIX_MODE_DEFAULT;
        __MIXChannel[i].input = 0;
        __MIXChannel[i].auxA = -960;
        __MIXChannel[i].auxB = -960;
        __MIXChannel[i].fader = 0;
        __MIXChannel[i].pan = 64;
        __MIXChannel[i].span = 127;
        __MIXChannel[i].v = __MIXChannel[i].vL = __MIXChannel[i].vR = __MIXChannel[i].vS =
            __MIXChannel[i].vAL = __MIXChannel[i].vAR = __MIXChannel[i].vAS = __MIXChannel[i].vBL =
                __MIXChannel[i].vBR = __MIXChannel[i].vBS = 0;
        __MIXSetPan(&__MIXChannel[i]);
    }

    __MIXDvdStreamAttenCurrent = 0;
    __MIXDvdStreamAttenUser = 0;
    __MIXSoundMode = 1;
}

void MIXQuit(void) {}

void MIXSetSoundMode(u32 mode) { __MIXSoundMode = mode; }

static inline u16 __MIXGetVolume(int dB) {
    int db = dB;
    if (db <= -904) {
        return 0;
    }
    if (db >= 60) {
        return 0xFF64;
    }
    return __MIXVolumeTable[db + 904];
}

void MIXInitChannel(AXVPB *voice, u32 mode, int input, int auxA, int auxB, int pan, int span,
                    int fader) {
    MIXChannel *c = &__MIXChannel[voice->index];
    u16 mixerCtrl;
    u16 *mix;
    BOOL enabled;

    c->axvpb = voice;
    c->mode = mode & 7;
    c->input = input;
    c->auxA = auxA;
    c->auxB = auxB;
    c->pan = pan;
    c->span = span;
    c->fader = fader;
    __MIXSetPan(c);

    if (c->mode & MIX_MODE_MUTE) {
        c->v = 0;
    } else {
        c->v = __MIXGetVolume(input);
    }

    mixerCtrl = 0;
    switch (__MIXSoundMode) {
    case 0:
        c->vL = __MIXGetVolume(c->fader + c->f);
        c->vR = __MIXGetVolume(c->fader + c->f);
        c->vS = __MIXGetVolume(c->fader + c->b - 30);
        if (c->mode & MIX_MODE_AUXA_PREFADER) {
            c->vAL = __MIXGetVolume(c->auxA + c->f);
            c->vAR = __MIXGetVolume(c->auxA + c->f);
            c->vAS = __MIXGetVolume(c->auxA + c->b - 30);
        } else {
            c->vAL = __MIXGetVolume(c->fader + c->auxA + c->f);
            c->vAR = __MIXGetVolume(c->fader + c->auxA + c->f);
            c->vAS = __MIXGetVolume(c->fader + c->auxA + c->b - 30);
        }
        if (c->mode & MIX_MODE_AUXB_PREFADER) {
            c->vBL = __MIXGetVolume(c->auxB + c->f);
            c->vBR = __MIXGetVolume(c->auxB + c->f);
            c->vBS = __MIXGetVolume(c->auxB + c->b - 30);
        } else {
            c->vBL = __MIXGetVolume(c->fader + c->auxB + c->f);
            c->vBR = __MIXGetVolume(c->fader + c->auxB + c->f);
            c->vBS = __MIXGetVolume(c->fader + c->auxB + c->b - 30);
        }
        break;
    case 1:
    case 2:
        c->vL = __MIXGetVolume(c->fader + c->l + c->f);
        c->vR = __MIXGetVolume(c->fader + c->r + c->f);
        c->vS = __MIXGetVolume(c->fader + c->b - 30);
        if (c->mode & MIX_MODE_AUXA_PREFADER) {
            c->vAL = __MIXGetVolume(c->auxA + c->l + c->f);
            c->vAR = __MIXGetVolume(c->auxA + c->r + c->f);
            c->vAS = __MIXGetVolume(c->auxA + c->b - 30);
        } else {
            c->vAL = __MIXGetVolume(c->fader + c->auxA + c->l + c->f);
            c->vAR = __MIXGetVolume(c->fader + c->auxA + c->r + c->f);
            c->vAS = __MIXGetVolume(c->fader + c->auxA + c->b - 30);
        }
        if (c->mode & MIX_MODE_AUXB_PREFADER) {
            c->vBL = __MIXGetVolume(c->auxB + c->l + c->f);
            c->vBR = __MIXGetVolume(c->auxB + c->r + c->f);
            c->vBS = __MIXGetVolume(c->auxB + c->b - 30);
        } else {
            c->vBL = __MIXGetVolume(c->fader + c->auxB + c->l + c->f);
            c->vBR = __MIXGetVolume(c->fader + c->auxB + c->r + c->f);
            c->vBS = __MIXGetVolume(c->fader + c->auxB + c->b - 30);
        }
        break;
    case MIX_SOUND_MODE_DPL2:
        /* Aux B's left and right carry the rear pair; aux A's surround slots its rear pair. */
        c->vL = __MIXGetVolume(c->fader + c->l + c->f);
        c->vR = __MIXGetVolume(c->fader + c->r + c->f);
        c->vBL = __MIXGetVolume(c->fader + c->l1 + c->b);
        c->vBR = __MIXGetVolume(c->fader + c->r1 + c->b);
        if (c->mode & MIX_MODE_AUXA_PREFADER) {
            c->vAL = __MIXGetVolume(c->auxA + c->l + c->f);
            c->vAR = __MIXGetVolume(c->auxA + c->r + c->f);
            c->vAS = __MIXGetVolume(c->auxA + c->l1 + c->b);
            c->vBS = __MIXGetVolume(c->auxA + c->r1 + c->b);
        } else {
            c->vAL = __MIXGetVolume(c->fader + c->auxA + c->l + c->f);
            c->vAR = __MIXGetVolume(c->fader + c->auxA + c->r + c->f);
            c->vAS = __MIXGetVolume(c->fader + c->auxA + c->l1 + c->b);
            c->vBS = __MIXGetVolume(c->fader + c->auxA + c->r1 + c->b);
        }
        mixerCtrl |= AX_PB_MIXCTRL_B_DPL2;
        break;
    }

    enabled = OSDisableInterrupts();

    voice->pb.ve.currentVolume = c->v;
    voice->pb.ve.currentDelta = 0;

    mix = (u16 *)&voice->pb.mix;
    if ((*mix++ = c->vL)) {
        mixerCtrl |= AX_PB_MIXCTRL_L;
    }
    *mix++ = 0;
    if ((*mix++ = c->vR)) {
        mixerCtrl |= AX_PB_MIXCTRL_R;
    }
    *mix++ = 0;
    if ((*mix++ = c->vAL)) {
        mixerCtrl |= AX_PB_MIXCTRL_A_L;
    }
    *mix++ = 0;
    if ((*mix++ = c->vAR)) {
        mixerCtrl |= AX_PB_MIXCTRL_A_R;
    }
    *mix++ = 0;
    if ((*mix++ = c->vBL)) {
        mixerCtrl |= AX_PB_MIXCTRL_B_L;
    }
    *mix++ = 0;
    if ((*mix++ = c->vBR)) {
        mixerCtrl |= AX_PB_MIXCTRL_B_R;
    }
    *mix++ = 0;
    if ((*mix++ = c->vBS)) {
        mixerCtrl |= AX_PB_MIXCTRL_B_S;
    }
    *mix++ = 0;
    if ((*mix++ = c->vS)) {
        mixerCtrl |= AX_PB_MIXCTRL_S;
    }
    *mix++ = 0;
    if ((*mix++ = c->vAS)) {
        mixerCtrl |= AX_PB_MIXCTRL_A_S;
    }
    *mix++ = 0;

    voice->pb.mixerCtrl = mixerCtrl;
    voice->sync |= MIX_SYNC_FLAGS;

    OSRestoreInterrupts(enabled);
}

void MIXReleaseChannel(AXVPB *voice) {
    MIXChannel *channel = &__MIXChannel[voice->index];

    channel->axvpb = NULL;
}

void MIXSetInput(AXVPB *voice, int dB) {
    MIXChannel *channel = &__MIXChannel[voice->index];

    channel->input = dB;
    channel->mode |= MIX_MODE_UPDATE_INPUT;
}

void MIXSetPan(AXVPB *voice, int pan) {
    MIXChannel *channel = &__MIXChannel[voice->index];

    if (pan < 0) {
        pan = 0;
    } else if (pan > 127) {
        pan = 127;
    }
    channel->pan = pan;
    __MIXSetPan(channel);
    channel->mode |= MIX_MODE_UPDATE_MIX;
}

/* Once per audio frame: move every channel one step. A setting changed by the caller gives a new
 * target level; the DSP ramps to it over the frame, and the frame after that the target becomes
 * the current level. */
void MIXUpdateSettings(void) {
    int i;

    for (i = 0; i < MIX_MAX_CHANNELS; i++) {
        MIXChannel *c = &__MIXChannel[i];
        AXVPB *voice = c->axvpb;
        BOOL setMix;
        BOOL setInput;

        setInput = FALSE;
        setMix = FALSE;
        if (voice != NULL) {
            u16 mixerCtrl = 0;
            u16 *mix;

            if (c->mode & MIX_MODE_UPDATE_INPUT1) {
                c->v = c->v1;
                c->mode &= ~MIX_MODE_UPDATE_INPUT1;
                setInput = TRUE;
            }
            if (c->mode & MIX_MODE_UPDATE_INPUT) {
                if (c->mode & MIX_MODE_MUTE) {
                    c->v1 = 0;
                } else {
                    c->v1 = __MIXGetVolume(c->input);
                }
                c->mode &= ~MIX_MODE_UPDATE_INPUT;
                c->mode |= MIX_MODE_UPDATE_INPUT1;
                setInput = TRUE;
            }
            if (c->mode & MIX_MODE_UPDATE_MIX1) {
                c->vL = c->vL1;
                c->vR = c->vR1;
                c->vS = c->vS1;
                c->vAL = c->vAL1;
                c->vAR = c->vAR1;
                c->vAS = c->vAS1;
                c->vBL = c->vBL1;
                c->vBR = c->vBR1;
                c->vBS = c->vBS1;
                c->mode &= ~MIX_MODE_UPDATE_MIX1;
                setMix = TRUE;
            }
            if (c->mode & MIX_MODE_UPDATE_MIX) {
            switch (__MIXSoundMode) {
            case 0:
                c->vL1 = __MIXGetVolume(c->fader + c->f);
                c->vR1 = __MIXGetVolume(c->fader + c->f);
                c->vS1 = __MIXGetVolume(c->fader + c->b - 30);
                if (c->mode & MIX_MODE_AUXA_PREFADER) {
                    c->vAL1 = __MIXGetVolume(c->auxA + c->f);
                    c->vAR1 = __MIXGetVolume(c->auxA + c->f);
                    c->vAS1 = __MIXGetVolume(c->auxA + c->b - 30);
                } else {
                    c->vAL1 = __MIXGetVolume(c->fader + c->auxA + c->f);
                    c->vAR1 = __MIXGetVolume(c->fader + c->auxA + c->f);
                    c->vAS1 = __MIXGetVolume(c->fader + c->auxA + c->b - 30);
                }
                if (c->mode & MIX_MODE_AUXB_PREFADER) {
                    c->vBL1 = __MIXGetVolume(c->auxB + c->f);
                    c->vBR1 = __MIXGetVolume(c->auxB + c->f);
                    c->vBS1 = __MIXGetVolume(c->auxB + c->b - 30);
                } else {
                    c->vBL1 = __MIXGetVolume(c->fader + c->auxB + c->f);
                    c->vBR1 = __MIXGetVolume(c->fader + c->auxB + c->f);
                    c->vBS1 = __MIXGetVolume(c->fader + c->auxB + c->b - 30);
                }
                break;
            case 1:
            case 2:
                c->vL1 = __MIXGetVolume(c->fader + c->l + c->f);
                c->vR1 = __MIXGetVolume(c->fader + c->r + c->f);
                c->vS1 = __MIXGetVolume(c->fader + c->b - 30);
                if (c->mode & MIX_MODE_AUXA_PREFADER) {
                    c->vAL1 = __MIXGetVolume(c->auxA + c->l + c->f);
                    c->vAR1 = __MIXGetVolume(c->auxA + c->r + c->f);
                    c->vAS1 = __MIXGetVolume(c->auxA + c->b - 30);
                } else {
                    c->vAL1 = __MIXGetVolume(c->fader + c->auxA + c->l + c->f);
                    c->vAR1 = __MIXGetVolume(c->fader + c->auxA + c->r + c->f);
                    c->vAS1 = __MIXGetVolume(c->fader + c->auxA + c->b - 30);
                }
                if (c->mode & MIX_MODE_AUXB_PREFADER) {
                    c->vBL1 = __MIXGetVolume(c->auxB + c->l + c->f);
                    c->vBR1 = __MIXGetVolume(c->auxB + c->r + c->f);
                    c->vBS1 = __MIXGetVolume(c->auxB + c->b - 30);
                } else {
                    c->vBL1 = __MIXGetVolume(c->fader + c->auxB + c->l + c->f);
                    c->vBR1 = __MIXGetVolume(c->fader + c->auxB + c->r + c->f);
                    c->vBS1 = __MIXGetVolume(c->fader + c->auxB + c->b - 30);
                }
                break;
            case MIX_SOUND_MODE_DPL2:
                /* Aux B's left and right carry the rear pair; aux A's surround slots its rear pair. */
                c->vL1 = __MIXGetVolume(c->fader + c->l + c->f);
                c->vR1 = __MIXGetVolume(c->fader + c->r + c->f);
                c->vBL1 = __MIXGetVolume(c->fader + c->l1 + c->b);
                c->vBR1 = __MIXGetVolume(c->fader + c->r1 + c->b);
                if (c->mode & MIX_MODE_AUXA_PREFADER) {
                    c->vAL1 = __MIXGetVolume(c->auxA + c->l + c->f);
                    c->vAR1 = __MIXGetVolume(c->auxA + c->r + c->f);
                    c->vAS1 = __MIXGetVolume(c->auxA + c->l1 + c->b);
                    c->vBS1 = __MIXGetVolume(c->auxA + c->r1 + c->b);
                } else {
                    c->vAL1 = __MIXGetVolume(c->fader + c->auxA + c->l + c->f);
                    c->vAR1 = __MIXGetVolume(c->fader + c->auxA + c->r + c->f);
                    c->vAS1 = __MIXGetVolume(c->fader + c->auxA + c->l1 + c->b);
                    c->vBS1 = __MIXGetVolume(c->fader + c->auxA + c->r1 + c->b);
                }
                mixerCtrl |= AX_PB_MIXCTRL_B_DPL2;
                break;
            }
                c->mode &= ~MIX_MODE_UPDATE_MIX;
                c->mode |= MIX_MODE_UPDATE_MIX1;
                setMix = TRUE;
            }

            if (setInput) {
                voice->pb.ve.currentVolume = c->v;
                voice->pb.ve.currentDelta = (c->v1 - c->v) / MIX_RAMP_SAMPLES;
                voice->sync |= 0x200;
            }
            if (setMix) {
                mix = (u16 *)&voice->pb.mix;
                if ((*mix++ = c->vL)) {
                    mixerCtrl |= AX_PB_MIXCTRL_L;
                }
                if ((*mix++ = (c->vL1 - c->vL) / MIX_RAMP_SAMPLES)) {
                    mixerCtrl |= AX_PB_MIXCTRL_RAMP;
                }
                if ((*mix++ = c->vR)) {
                    mixerCtrl |= AX_PB_MIXCTRL_R;
                }
                if ((*mix++ = (c->vR1 - c->vR) / MIX_RAMP_SAMPLES)) {
                    mixerCtrl |= AX_PB_MIXCTRL_RAMP;
                }
                if ((*mix++ = c->vAL)) {
                    mixerCtrl |= AX_PB_MIXCTRL_A_L;
                }
                if ((*mix++ = (c->vAL1 - c->vAL) / MIX_RAMP_SAMPLES)) {
                    mixerCtrl |= AX_PB_MIXCTRL_A_RAMP;
                }
                if ((*mix++ = c->vAR)) {
                    mixerCtrl |= AX_PB_MIXCTRL_A_R;
                }
                if ((*mix++ = (c->vAR1 - c->vAR) / MIX_RAMP_SAMPLES)) {
                    mixerCtrl |= AX_PB_MIXCTRL_A_RAMP;
                }
                if ((*mix++ = c->vBL)) {
                    mixerCtrl |= AX_PB_MIXCTRL_B_L;
                }
                if ((*mix++ = (c->vBL1 - c->vBL) / MIX_RAMP_SAMPLES)) {
                    mixerCtrl |= AX_PB_MIXCTRL_B_RAMP;
                }
                if ((*mix++ = c->vBR)) {
                    mixerCtrl |= AX_PB_MIXCTRL_B_R;
                }
                if ((*mix++ = (c->vBR1 - c->vBR) / MIX_RAMP_SAMPLES)) {
                    mixerCtrl |= AX_PB_MIXCTRL_B_RAMP;
                }
                if ((*mix++ = c->vBS)) {
                    mixerCtrl |= AX_PB_MIXCTRL_B_S;
                }
                if ((*mix++ = (c->vBS1 - c->vBS) / MIX_RAMP_SAMPLES)) {
                    mixerCtrl |= AX_PB_MIXCTRL_B_S_RAMP;
                }
                if ((*mix++ = c->vS)) {
                    mixerCtrl |= AX_PB_MIXCTRL_S;
                }
                if ((*mix++ = (c->vS1 - c->vS) / MIX_RAMP_SAMPLES)) {
                    mixerCtrl |= AX_PB_MIXCTRL_RAMP;
                }
                if ((*mix++ = c->vAS)) {
                    mixerCtrl |= AX_PB_MIXCTRL_A_S;
                }
                if ((*mix++ = (c->vAS1 - c->vAS) / MIX_RAMP_SAMPLES)) {
                    mixerCtrl |= AX_PB_MIXCTRL_A_S_RAMP;
                }
                voice->pb.mixerCtrl = mixerCtrl;
                voice->sync |= 0x12;
            }
        }
    }

    /* The DVD stream's volume follows its target one step per frame. */
    if (__MIXDvdStreamAttenUser > __MIXDvdStreamAttenCurrent) {
        __MIXDvdStreamAttenCurrent++;
        AISetStreamVolLeft(__MIXAIVolumeTable[__MIXDvdStreamAttenCurrent]);
        AISetStreamVolRight(__MIXAIVolumeTable[__MIXDvdStreamAttenCurrent]);
    } else if (__MIXDvdStreamAttenUser < __MIXDvdStreamAttenCurrent) {
        __MIXDvdStreamAttenCurrent--;
        AISetStreamVolLeft(__MIXAIVolumeTable[__MIXDvdStreamAttenCurrent]);
        AISetStreamVolRight(__MIXAIVolumeTable[__MIXDvdStreamAttenCurrent]);
    }
}
