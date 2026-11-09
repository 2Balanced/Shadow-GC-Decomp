/* Sonicteam::System::Peripheral::Pad2MouseImplementor (class name from RTTI): a mouse driven by a
 * pad. The main stick moves it, the C stick's vertical axis is the wheel and the A and B buttons
 * are its two buttons. Game/GamePeripheralTask.cpp creates one for pad 3 and constructs it there
 * (the constructor is inline). File name from the class; the member names are inferred. */
#include "Game/SonicteamPeripheral.h"
#include <dolphin/types.h>
#include "size_assert.h"

namespace Sonicteam {
namespace System {
namespace Peripheral {

class Pad2MouseImplementor : public IMouseManagerImp {
  public:
    Pad2MouseImplementor(int pad) : m_pad(pad) {}
    virtual ~Pad2MouseImplementor();
    virtual bool Read(MouseState *mouse);

    /* 0x4 */ int m_pad;
};
SIZE_ASSERT(Pad2MouseImplementor, 0x8);

Pad2MouseImplementor::~Pad2MouseImplementor() {}

bool Pad2MouseImplementor::Read(MouseState *mouse) {
    UnkPad_8040DB6C *pad = fn_8040DB6C(fn_80009208(), m_pad);
    if (pad != 0) {
        const float scale = 4.0f;
        mouse->m_x = scale * pad->m_state.m_stick.x;
        mouse->m_y = scale * pad->m_state.m_stick.y;
        mouse->m_wheel = scale * pad->m_state.m_subStick.y;
        mouse->m_buttons = 0;
        u32 buttons = pad->m_state.m_down;
        if (buttons & 1) {
            mouse->m_buttons |= 1;
        }
        if (buttons & 2) {
            mouse->m_buttons |= 2;
        }
    } else {
        mouse->m_x = 0;
        mouse->m_y = 0;
        mouse->m_wheel = 0;
        mouse->m_buttons = 0;
    }
    return true;
}

} // namespace Peripheral
} // namespace System
} // namespace Sonicteam
