#ifndef GAME_SCREENFADE_H
#define GAME_SCREENFADE_H

#include <dolphin/types.h>
#include "size_assert.h"

/* The screen fader (class name unknown): fn_80044254 returns the game's one, fn_80173878 starts a
 * fade through it (Game/PjsEventActionManager.cpp) and fn_80044E10 reads its state.
 * Game/WarpHole.cpp keeps its own view of it. Member names are inferred. */

/* RenderWare's colour (rw/rwcore.h), as Game/DebugDispShape.h lays it out. */
typedef struct RwRGBA {
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
} RwRGBA;

struct UnkFader_80044254 {
    /* 0x00 */ float m_level;
    /* 0x04 */ float m_time;
    /* 0x08 */ float m_rate; /* 1 / m_time */
    /* 0x0C */ u8 field_0xC[0x10 - 0xC];
    /* 0x10 */ RwRGBA m_color;
    /* 0x14 */ s32 m_state; /* 1 once a fade is over (meaning inferred) */
};

extern "C" {
UnkFader_80044254 *fn_80044254(void);
/* Sets the fade colour, and the fade level (behaviour inferred). */
void fn_800445D0(UnkFader_80044254 *fader, const RwRGBA *color);
void fn_80044648(UnkFader_80044254 *fader, float level);
/* The fader's state (fn_80044254()->m_state). */
int fn_80044E10(void);
/* Starts a fade (Game/PjsEventActionManager.cpp; argument meanings inferred). */
void fn_80173878(int arg0, int arg1, float time);
}

/* The stage actions' fader helpers. They are weak functions that keep their fn_ names
 * (Game/PjsEventActionFade.cpp calls them too); the first unit that calls one keeps the copy the
 * others link to (fn_80204874 in Game/DeadAction.cpp, fn_802051F8 in Game/GoalAction.cpp). An
 * inline call in their bodies keeps them out of line, as retail has them. */
inline UnkFader_80044254 *GetFader() {
    return fn_80044254();
}

/* Sets the fade colour (name inferred). */
extern "C" inline void fn_80204874(const RwRGBA *color) {
    fn_800445D0(GetFader(), color);
}

/* Sets the fade time (name inferred). */
extern "C" inline void fn_802051F8(float time) {
    UnkFader_80044254 *fader = GetFader();
    fader->m_time = time;
    fader->m_rate = 1.0f / time;
}

#endif
