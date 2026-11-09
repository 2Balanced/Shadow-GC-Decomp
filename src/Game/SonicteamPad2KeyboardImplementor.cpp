/* Sonicteam::System::Peripheral::Pad2Keyboard::Implementor (class name from RTTI): a keyboard driven
 * by a pad. Each of the pad's button bits can be bound to a key code (typed when the button is held,
 * at most eight a frame; the lock keys toggle their lock) or to modifier bits. The defaults bind the
 * d-pad to the arrow keys (the right arrow is bound twice), two
 * buttons to Enter and Backspace and two to modifiers; Game/GamePeripheralTask.cpp adds three keys.
 * Key codes are USB HID usages (0x39 Caps Lock, 0x47 Scroll Lock, 0x53 Num Lock). File name from the
 * class; the member names are inferred. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#include "Game/SonicteamPeripheral.h"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" void *memset(void *dst, int value, unsigned long n);
extern "C" void *memcpy(void *dst, const void *src, unsigned long n);

namespace Sonicteam {
namespace System {
namespace Peripheral {
namespace Pad2Keyboard {

/* What one button of the pad does (struct name unknown). */
struct Key {
    enum {
        TYPE_NONE,
        TYPE_KEY,
        TYPE_MODIFIER,
    };

    Key();
    ~Key();
    void SetKey(u16 code);
    void SetModifier(u32 mask);

    void Clear() {
        m_mask = 0;
        m_code = 0;
        m_type = TYPE_NONE;
    }

    /* 0x0 */ u16 m_code;
    /* 0x4 */ u32 m_mask;
    /* 0x8 */ int m_type;
};
SIZE_ASSERT(Key, 0xC);

class Implementor : public IKeyboardManagerImp {
  public:
    Implementor(int pad);
    virtual ~Implementor();
    virtual bool Read(KeyboardState *keyboards, u32 count);
    virtual void Flush();

    void SetDefaults();
    void BindModifier(int button, u32 mask);
    void BindKey(int button, u16 code);

    void Reset() {
        m_state.m_disconnected = 1;
        m_state.m_locks = 0;
        m_state.m_modifiers = 0;
        m_state.m_keyCount = 0;
        memset(m_state.m_keys, 0, sizeof(m_state.m_keys));
    }

    /* 0x04 */ int m_pad;
    /* 0x08 */ Key m_keys[18];
    /* 0xE0 */ KeyboardState m_state;
};
SIZE_ASSERT(Implementor, 0x100);

void Implementor::BindKey(int button, u16 code) {
    m_keys[button].SetKey(code);
}

void Implementor::BindModifier(int button, u32 mask) {
    m_keys[button].SetModifier(mask);
}

void Implementor::SetDefaults() {
    BindKey(6, 0x52);
    BindKey(7, 0x51);
    BindKey(8, 0x50);
    BindKey(9, 0x4F);
    BindKey(9, 0x4F);
    BindKey(3, 0x28);
    BindKey(4, 0x2A);
    BindModifier(13, 0x20);
    BindModifier(12, 0x1);
}

Key::Key() {
    Clear();
}

Key::~Key() {}

void Key::SetKey(u16 code) {
    Clear();
    m_code = code;
    m_type = TYPE_KEY;
}

void Key::SetModifier(u32 mask) {
    Clear();
    m_mask = mask;
    m_type = TYPE_MODIFIER;
}

Implementor::Implementor(int pad) : m_pad(pad) {
    Reset();
    SetDefaults();
}

Implementor::~Implementor() {}

bool Implementor::Read(KeyboardState *keyboards, u32 count) {
    UnkPad_8040DB6C *pad = fn_8040DB6C(fn_80009208(), m_pad);
    if (pad != 0) {
        u32 buttons = pad->m_state.m_down;
        memset(m_state.m_keys, 0, sizeof(m_state.m_keys));
        m_state.m_keyCount = 0;
        m_state.m_modifiers = 0;
        for (u32 i = 0; i < sizeof(m_keys); i++) {
            if (m_keys[i].m_type == Key::TYPE_NONE) {
            } else if (m_keys[i].m_type == Key::TYPE_KEY) {
                if (m_state.m_keyCount < 8 && (buttons & (1 << i))) {
                    u16 code = m_keys[i].m_code;
                    if (code == 0x53) {
                        m_state.m_locks ^= 1;
                    } else if (code == 0x39) {
                        m_state.m_locks ^= 2;
                    } else if (code == 0x47) {
                        m_state.m_locks ^= 4;
                    }
                    m_state.m_keys[m_state.m_keyCount] = code;
                    m_state.m_keyCount++;
                }
            } else if (m_keys[i].m_type == Key::TYPE_MODIFIER) {
                if (buttons & (1 << i)) {
                    m_state.m_modifiers |= m_keys[i].m_mask;
                }
            }
        }
        m_state.m_disconnected = 0;
    } else {
        Reset();
    }
    memcpy(keyboards, &m_state, sizeof(KeyboardState));
    for (u32 i = 1; i < count; i++) {
        keyboards[i].m_disconnected = 1;
    }
    return true;
}

void Implementor::Flush() {}

} // namespace Pad2Keyboard
} // namespace Peripheral
} // namespace System
} // namespace Sonicteam
