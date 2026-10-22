/* The stage timer (struct name unknown; the game state keeps it at 0x88, see Game/Mission.cpp): it
 * counts frames, the stage time (stopped while the game state's flag at 0xC8 is set) and the total
 * time, and with a time limit set it gives the time left (-1 without one). The mission manager
 * saves and restores the time left across a restart. The other units call these functions by
 * their addresses. File, struct, member and function names are inferred. Definitions are in
 * reverse address order. */
#include <dolphin/types.h>
#include "size_assert.h"

/* The game state (struct name unknown; see Game/BossCtrl.cpp): the frame time at 0x20 and a flag
 * at 0xC8 that stops the stage time (meaning inferred). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x20];
    /* 0x020 */ float m_frameTime;
    /* 0x024 */ u8 field_0x24[0xC8 - 0x24];
    /* 0x0C8 */ u8 field_0xC8;
    /* 0x0C9 */ u8 field_0xC9[0x150 - 0xC9];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

struct UnkStageTimer_8033709C {
    /* 0x00 */ u32 m_frames;
    /* 0x04 */ u32 m_activeFrames;
    /* 0x08 */ u32 m_stageFrames;
    /* 0x0C */ float m_time;
    /* 0x10 */ float m_totalTime;
    /* 0x14 */ float m_limit;
    /* 0x18 */ float m_left; /* the time left, or -1 without a limit */
    /* 0x1C */ float m_savedLeft;
};
SIZE_ASSERT(UnkStageTimer_8033709C, 0x20);

extern "C" {

/* Clears the timer (name inferred). */
void fn_8033709C(UnkStageTimer_8033709C *timer) {
    timer->m_frames = 0;
    timer->m_activeFrames = 0;
    timer->m_stageFrames = 0;
    timer->m_time = 0.0f;
    timer->m_totalTime = 0.0f;
    timer->m_limit = 0.0f;
    timer->m_left = 0.0f;
    timer->m_savedLeft = 0.0f;
}

/* Advances the timer by a frame; paused frames count only in m_frames (name inferred). */
void fn_80336FF8(UnkStageTimer_8033709C *timer, bool paused) {
    timer->m_frames++;
    if (!paused) {
        timer->m_activeFrames++;
        timer->m_stageFrames++;
    }
    if (!lbl_8057D6A0.field_0xC8) {
        timer->m_time += lbl_8057D6A0.m_frameTime;
    }
    timer->m_totalTime += lbl_8057D6A0.m_frameTime;
    if (timer->m_limit > 0.0f) {
        timer->m_left = timer->m_limit - timer->m_time;
        if (timer->m_left < 0.0f) {
            timer->m_left = 0.0f;
        }
    } else {
        timer->m_left = -1.0f;
    }
}

/* Restarts the stage time (name inferred). */
void fn_80336FE4(UnkStageTimer_8033709C *timer) {
    timer->m_stageFrames = 0;
    timer->m_time = 0.0f;
}

/* Removes the time limit (name inferred). */
void fn_80336FD0(UnkStageTimer_8033709C *timer) {
    timer->m_limit = 0.0f;
    timer->m_left = 0.0f;
    timer->m_savedLeft = 0.0f;
}

/* Sets the time limit (name inferred). */
void fn_80336FC0(UnkStageTimer_8033709C *timer, float limit) {
    timer->m_limit = limit;
    timer->m_left = limit;
    timer->m_savedLeft = limit;
}

/* Saves the time left (name inferred). */
void fn_80336FB4(UnkStageTimer_8033709C *timer) {
    timer->m_savedLeft = timer->m_left;
}

/* Restores the saved time left, moving the limit so that it holds from the current stage time
 * (name inferred). */
void fn_80336F98(UnkStageTimer_8033709C *timer) {
    timer->m_limit = timer->m_time + timer->m_savedLeft;
    timer->m_left = timer->m_savedLeft;
}
}
