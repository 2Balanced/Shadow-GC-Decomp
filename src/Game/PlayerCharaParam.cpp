/* The player characters' names and parameters (file and table names unknown): a name for each
 * character kind (the player's word at 0x64; "---" for none) and the record of
 * Game/PlayerCharaParam.h. File name taken from that header. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#include "Game/PlayerCharaParam.h"

/* The game's state record (struct name unknown; only the field this unit reads). */
struct UnkGameState_8057D6A0 {
    /* 0x00 */ u8 field_0x0[0xA8];
    /* 0xA8 */ s32 m_stageId; /* the stage number, 402 for one Shadow stage (meaning inferred) */
};
extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The name of each character kind. */
static const char *s_charaNames[] = {
    "---",       "Sonic",     "Tails",     "Knuckles",       "Amy",
    "Rouge",     "Omega",     "Vector",    "Espio",          "Maria",
    "Charmy",    "EggMonitor", "DoomsEye", "BossSonic",      "Shadow",
    "2PYellow",  "2PGunPink", "2PGunBlue", "2PCannonOrange", "2PCannonGreen",
};

const UnkCharaParam_804F1834 lbl_804F1834[20] = {
    /* ---            */ {-1, 0.0f, 0, 1, 1, 0, 0.0f, 0.0f, 0.0f, 0x23, 0x24, {0xFF, 0xFF, 0xFF}},
    /* Sonic          */ {2, 1.0f, 1, 1, 1, 0, 0.0f, 0.0f, 0.0f, 0x219, 0x21A, {0, 0x5A, 0xAA}},
    /* Tails          */ {2, 0.9f, 0, 1, 1, 0, 0.0f, 0.0f, 0.0f, 0x213, 0x214, {0xFF, 0xA6, 0x19}},
    /* Knuckles       */ {2, 1.0f, 0, 1, 1, 0, 0.0f, 0.0f, 0.0f, 0x215, 0x216, {0xED, 0x19, 0x3A}},
    /* Amy            */ {2, 0.6f, 0, 1, 1, 0, 0.0f, 0.0f, 0.0f, 0x217, 0x218, {0xF4, 0x9A, 0xC1}},
    /* Rouge          */ {2, 0.9f, 0, 1, 1, 0, 0.0f, 0.0f, 0.0f, 0x21D, 0x21E, {0xF1, 0x72, 0xAC}},
    /* Omega          */ {2, 0.8f, 0, 1, 1, 0, 0.0f, 0.0f, 0.0f, 0x21F, 0x220, {0xC0, 0, 0}},
    /* Vector         */ {2, 0.8f, 0, 1, 1, 0, 0.0f, 0.0f, 0.0f, 0x221, 0x222, {0, 0x80, 0}},
    /* Espio          */ {2, 1.0f, 0, 1, 1, 0, 0.0f, 0.0f, 0.0f, 0x223, 0x224, {0, 0x80, 0x80}},
    /* Maria          */ {2, 0.4f, 0, 0, 0, 0, 0.0f, 0.0f, 0.0f, 0x23, 0x24, {0xFF, 0xFF, 0xFF}},
    /* Charmy         */ {2, 0.0f, 0, 0, 0, 0, 25.0f, 10.0f, 15.0f, 0x23, 0x24, {0xFF, 0xFF, 0xFF}},
    /* EggMonitor     */ {0, 0.0f, 0, 0, 0, 0, 50.0f, 10.0f, 80.0f, 0x23, 0x24, {0xFF, 0xFF, 0xFF}},
    /* DoomsEye       */ {0, 0.0f, 0, 0, 0, 0, 50.0f, 10.0f, 80.0f, 0x23, 0x24, {0xFF, 0xFF, 0xFF}},
    /* BossSonic      */ {2, 1.0f, 1, 1, 1, 0, 0.0f, 0.0f, 0.0f, 0x219, 0x21A, {0, 0x5A, 0xAA}},
    /* Shadow         */ {2, 1.0f, 1, 1, 1, 0, 0.0f, 0.0f, 0.0f, 0x23, 0x24, {0x7F, 0x40, 0}},
    /* 2PYellow       */ {2, 1.0f, 1, 1, 1, 0, 0.0f, 0.0f, 0.0f, 0x23, 0x24, {0x7F, 0x40, 0}},
    /* 2PGunPink      */ {2, 1.0f, 1, 1, 1, 0, 0.0f, 0.0f, 0.0f, 0x23, 0x24, {0x7F, 0x40, 0}},
    /* 2PGunBlue      */ {2, 1.0f, 1, 1, 1, 0, 0.0f, 0.0f, 0.0f, 0x23, 0x24, {0x7F, 0x40, 0}},
    /* 2PCannonOrange */ {2, 1.0f, 1, 1, 1, 0, 0.0f, 0.0f, 0.0f, 0x23, 0x24, {0x7F, 0x40, 0}},
    /* 2PCannonGreen  */ {2, 1.0f, 1, 1, 1, 0, 0.0f, 0.0f, 0.0f, 0x23, 0x24, {0x7F, 0x40, 0}},
};

/* The name of character kind (name inferred). */
extern "C" const char *fn_802285D4(int kind) {
    return s_charaNames[kind];
}

/* The word at 0x0 of character kind's record; kind 11 (EggMonitor) gives 2 on stage 402 (name
 * and meaning inferred). */
extern "C" int fn_802285A0(int kind) {
    int value = lbl_804F1834[kind].field_0x0;
    if (kind == 11 && lbl_8057D6A0.m_stageId == 402) {
        value = 2;
    }
    return value;
}
