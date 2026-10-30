#ifndef GAME_PLAYERPADINPUT_H
#define GAME_PLAYERPADINPUT_H

#include "Game/MathTypes.h"
#include <dolphin/types.h>
#include "size_assert.h"

struct UnkPad_8040DB6C;

/* A player's pad input (struct name unknown; its code is Game/PlayerPadInput.cpp, and fn_8006F8D0
 * returns a player's): the two sticks and the buttons of the pad the game assigned to the player,
 * as the game's button bits, with the presses and releases of the frame and the time since the
 * pad was last touched (names inferred). */
struct UnkPlayerInput_800A85B0 {
    UnkPlayerInput_800A85B0();
    ~UnkPlayerInput_800A85B0();
    void Update(float dt);

    float GetIdleTime() const {
        return m_idleTime;
    }

    /* 0x00 */ Vec2f m_stick;
    /* 0x08 */ Vec2f m_subStick;
    /* 0x10 */ UnkPad_8040DB6C *m_padObject;
    /* 0x14 */ u32 m_pad;
    /* 0x18 */ u32 m_held;
    /* 0x1C */ u32 m_notHeld;
    /* 0x20 */ u32 m_pressed;
    /* 0x24 */ u32 m_released;
    /* 0x28 */ u32 m_previous;
    /* 0x2C */ u32 m_masks[22];
    /* 0x84 */ float m_idleTime;
};
SIZE_ASSERT(UnkPlayerInput_800A85B0, 0x88);

#endif
