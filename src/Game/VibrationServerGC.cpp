/* Vibration::ServerGC (class name from RTTI; declared in Game/VibrationServer.h): switches the
 * rumble motor of a GameCube pad with PADControlMotor. File name from the class; member names are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/VibrationServer.h"

/* One pad of the pad manager (see Game/GamePeripheralTask.cpp): 0 at 0x28 while connected. */
struct UnkPad_8040DB6C {
    /* 0x00 */ u8 field_0x0[0x28];
    /* 0x28 */ s32 m_state;
};

/* The game's pad state (see Game/GamePeripheralTask.cpp). */
struct UnkPads_80051734 {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ UnkPad_8040DB6C *m_pads[2];
};

extern "C" {
UnkPads_80051734 *fn_80051734(void);
/* A 32-bit flag word: fn_80014E04 constructs it cleared, fn_80014E10 clears it, fn_80014E1C sets
 * bit, fn_8006127C clears bit and fn_80014E34 tests bit (as in Game/BAMine.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
/* The SDK's PADControlMotor (0 stop, 1 rumble, 2 hard stop). */
void fn_803805C8(s32 chan, u32 command);
}

namespace Vibration {

ServerGC::ServerGC(int pad) : Server(pad) {
    fn_80014E04(&m_flags);
    m_pulse = m_level = 0.0f;
    fn_80014E10(&m_flags);
}

ServerGC::~ServerGC() {}

void ServerGC::SetLevel(float level, float extra) {
    if (!fn_80014E34(&m_flags, 1)) {
        m_level = level + extra;
        if (m_level > 0.8f) {
            fn_80014E1C(&m_flags, 3);
            fn_8006127C(&m_flags, 4);
            m_pulse = 0.0f;
        } else if (m_level < 0.2f) {
            fn_8006127C(&m_flags, 3);
            fn_8006127C(&m_flags, 4);
            m_pulse = 0.0f;
        } else {
            fn_8006127C(&m_flags, 3);
            fn_80014E1C(&m_flags, 4);
            m_pulse = m_pulse + m_level;
        }
        fn_80014E1C(&m_flags, 0);
    }
}

void ServerGC::Stop() {
    fn_8006127C(&m_flags, 0);
    fn_80014E1C(&m_flags, 1);
    fn_8006127C(&m_flags, 3);
    fn_8006127C(&m_flags, 4);
    m_pulse = m_level = 0.0f;
}

void ServerGC::Update(float dt) {
    s32 index = m_pad;
    UnkPads_80051734 *pads = fn_80051734();
    UnkPad_8040DB6C *pad = pads->m_pads[index];
    if (pad != 0 && pad->m_state == 0) {
        if (fn_80014E34(&m_flags, 1)) {
            if (fn_80014E34(&m_flags, 2)) {
                fn_803805C8(m_pad, 2);
                fn_8006127C(&m_flags, 2);
                fn_8006127C(&m_flags, 1);
            } else {
                fn_8006127C(&m_flags, 1);
            }
        } else if (fn_80014E34(&m_flags, 0)) {
            bool on = fn_80014E34(&m_flags, 3);
            if (fn_80014E34(&m_flags, 4) && m_pulse > 0.8f) {
                on = true;
                m_pulse = 0.0f;
            }
            if (on) {
                fn_803805C8(m_pad, 1);
            } else {
                fn_803805C8(m_pad, 0);
            }
            fn_80014E1C(&m_flags, 2);
            fn_8006127C(&m_flags, 0);
        }
    }
}

} // namespace Vibration
