/* The key counter of the play record (the word at 0x724 of the object fn_80079808 returns; struct
 * name unknown): it remembers the frame a secret key was last collected, and gives the number of
 * keys the stage has (SecretKey's slots; none in the mode that game state flag 0xB1 marks). The
 * HUD (TGiRingChallenge) shows the count for a while after a key is collected. File, struct and
 * function names are inferred. Definitions are in reverse address order. */
#include <dolphin/types.h>
#include "size_assert.h"

/* The game state (struct name unknown; see Game/CenFile.cpp): the frame counter at 0x90 and a mode
 * flag at 0xB1 (meanings inferred). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x90];
    /* 0x090 */ u32 m_frame;
    /* 0x094 */ u8 field_0x94[0xB1 - 0x94];
    /* 0x0B1 */ u8 field_0xB1;
    /* 0x0B2 */ u8 field_0xB2[0x150 - 0xB2];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

struct UnkKeyCounter_803357B0 {
    /* 0x0 */ u32 m_frame; /* the frame the last key was collected */
};

extern "C" {
/* SecretKey's slots: reload them from the save data, and count the collected keys (see
 * Game/SecretKey.cpp). */
void fn_8011BE14(void);
u32 fn_8011BE9C(void);

/* Resets the counter for a new stage (name inferred). */
void fn_80335808(UnkKeyCounter_803357B0 *counter) {
    counter->m_frame = lbl_8057D6A0.m_frame;
    if (lbl_8057D6A0.field_0xB1) {
        fn_8011BE14();
    }
}

/* Records that a key was collected now (name inferred). */
void fn_803357F4(UnkKeyCounter_803357B0 *counter) {
    counter->m_frame = lbl_8057D6A0.m_frame;
}

/* The number of keys collected (name inferred). */
u32 fn_803357B8(UnkKeyCounter_803357B0 *counter) {
    if (lbl_8057D6A0.field_0xB1) {
        return 0;
    }
    return fn_8011BE9C();
}

/* The frame the last key was collected (name inferred). */
u32 fn_803357B0(UnkKeyCounter_803357B0 *counter) {
    return counter->m_frame;
}
}
