/* A player's pad input (struct name unknown): the two sticks and the buttons of the pad the game
 * assigned to the player, as the game's button bits, with the presses and releases of the frame
 * and the time since the player last touched the pad. File and other names inferred. Functions
 * other units call by address keep their fn_ names. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include "Game/MathTypes.h"
#include "Game/PlayerPadInput.h"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" {
float fn_8040CCD4(const Vec2f *vector);
void fn_8040CC6C(Vec2f *vector);
void fn_8040CC5C(Vec2f *vector);
const float &fn_800704AC(const float &a, const float &b); /* the smaller */

/* The pads (behaviour inferred). */
void *fn_80051734(void);
void *fn_80009208(void);
UnkPad_8040DB6C *fn_8040DB6C(void *manager, u32 index);
}

/* The pad's button bits for each of the game's buttons. */
static const u32 sButtonMasks[22] = {
    0x400, 0x400, 0x2, 0x1, 0x1, 0x2, 0x8, 0x10, 0x2000, 0x100, 0x80,
    0x200, 0x40, 0x80, 0x100, 0x200, 0x20, 0x4, 0x1000, 0x2000, 0x4000, 0x8000,
};

UnkPlayerInput_800A85B0::UnkPlayerInput_800A85B0() {
    m_padObject = 0;
    m_pad = -1;
    m_held = 0;
    m_notHeld = 0;
    m_pressed = 0;
    m_released = 0;
    m_idleTime = 0.0f;
    for (int i = 0; i < 22; i++) {
        m_masks[i] = sButtonMasks[i];
    }
}

UnkPlayerInput_800A85B0::~UnkPlayerInput_800A85B0() {}

/* Takes the pad the game assigned to player slot pad. */
extern "C" void fn_800A871C(UnkPlayerInput_800A85B0 *input, int pad) {
    input->m_pad = reinterpret_cast<u32 *>(reinterpret_cast<u8 *>(fn_80051734()) + 0x11C)[pad];
    u32 index = input->m_pad;
    if (index < 2) {
        input->m_padObject = fn_8040DB6C(fn_80009208(), index);
    } else {
        input->m_padObject = 0;
    }
}

static float sMaxIdleTime = 1000.0f;

void UnkPlayerInput_800A85B0::Update(float dt) {
    m_idleTime += dt;
    m_idleTime = fn_800704AC(m_idleTime, sMaxIdleTime);
    m_previous = m_held;
    m_held = 0;
    UnkPad_8040DB6C *pad = m_padObject;
    if (pad != 0) {
        u32 buttons = *reinterpret_cast<u32 *>(pad);
        u32 *masks = m_masks;
        u32 bit = 1;
        for (int i = 0; i < 22; i++, masks++) {
            if (buttons & *masks) {
                m_held |= bit;
            }
            bit <<= 1;
        }
        m_stick.x = reinterpret_cast<float *>(pad)[4];
        m_stick.y = reinterpret_cast<float *>(pad)[5];
        m_subStick.x = reinterpret_cast<float *>(pad)[7];
        m_subStick.y = reinterpret_cast<float *>(pad)[8];
        if (fn_8040CCD4(&m_stick) > 1.0f) {
            fn_8040CC6C(&m_stick);
        }
        if (fn_8040CCD4(&m_subStick) > 1.0f) {
            fn_8040CC6C(&m_subStick);
        }
        if (m_held != 0 || !(m_stick.x * m_stick.x + m_stick.y * m_stick.y < 0.0001f)) {
            m_idleTime = 0.0f;
        }
    } else {
        m_held = 0;
        fn_8040CC5C(&m_stick);
        fn_8040CC5C(&m_subStick);
    }
    m_notHeld = ~m_held;
    m_pressed = m_held & ~m_previous;
    m_released = m_previous & ~m_held;
}
