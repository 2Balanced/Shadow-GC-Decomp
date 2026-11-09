#ifndef GAME_SONICTEAMPERIPHERAL_H
#define GAME_SONICTEAMPERIPHERAL_H

#include <memory>

#include <dolphin/types.h>
#include "size_assert.h"

/* Sonicteam::System::Peripheral (namespace and interface names from RTTI): the game's input
 * managers - the pads, the mouse and the keyboard - and the interfaces of the implementations
 * that feed them. Game/ManagerImpGc.cpp (the GameCube pads) and Game/GamePeripheralTask.cpp
 * (which installs a pad, a pad-driven mouse and a pad-driven keyboard) declare the interfaces
 * locally under the same names. The managers have no RTTI; their names, and every member and
 * function name here, are inferred. Units that call them by address keep the fn_ names. */
namespace Sonicteam {
namespace System {
namespace Peripheral {

/* One pad as an IManagerImp reports it (as in Game/ManagerImpGc.cpp). */
struct PadState {
    /* 0x0 */ u32 m_state; /* 0: connected, 1: no pad, 3: error */
    /* 0x4 */ u32 m_buttons;
    /* 0x8 */ s16 m_stick[2];
    /* 0xC */ s16 m_substick[2];
};

/* The dead zone and range of the two sticks (as in Game/ManagerImpGc.cpp). */
struct StickRange {
    /* 0x0 */ s16 m_deadZone[2];
    /* 0x4 */ s16 m_range[2];
};

/* What a pad back end does for the pad manager (as in Game/ManagerImpGc.cpp). */
class IManagerImp {
  public:
    virtual ~IManagerImp() {}
    virtual void Initialize() = 0;
    virtual u32 GetPadCount() = 0;
    virtual bool Read(PadState *pads, u32 count) = 0;
    virtual bool GetStickRange(int pad, StickRange *range) = 0;
};

/* One mouse as an IMouseManagerImp reports it. */
struct MouseState {
    /* 0x0 */ s32 m_x;
    /* 0x4 */ s32 m_y;
    /* 0x8 */ s32 m_wheel;
    /* 0xC */ u32 m_buttons;
};
SIZE_ASSERT(MouseState, 0x10);

class IMouseManagerImp {
  public:
    virtual ~IMouseManagerImp() {}
    virtual bool Read(MouseState *mouse) = 0;
};

/* One keyboard as an IKeyboardManagerImp reports it: the keys typed this frame, the lock
 * toggles and the modifiers held. */
struct KeyboardState {
    /* 0x00 */ u32 m_disconnected;
    /* 0x04 */ u32 m_locks;
    /* 0x08 */ u32 m_modifiers;
    /* 0x0C */ u16 m_keys[8];
    /* 0x1C */ u32 m_keyCount;
};
SIZE_ASSERT(KeyboardState, 0x20);

class IKeyboardManagerImp {
  public:
    virtual ~IKeyboardManagerImp() {}
    virtual bool Read(KeyboardState *keyboards, u32 count) = 0;
    virtual void Flush() = 0;
};

} // namespace Peripheral
} // namespace System
} // namespace Sonicteam

/* A stick's position: scaled to -1..1 from the raw value (struct names unknown; see
 * Game/GamePeripheralTask.cpp). */
struct UnkPadStick_8040DD24 {
    /* 0x0 */ float x;
    /* 0x4 */ float y;
    /* 0x8 */ s16 m_rawX;
    /* 0xA */ s16 m_rawY;
};

/* A pad's state for a frame: buttons held, not held, pressed and released this frame, and the
 * two sticks. */
struct UnkPadState_8040DD24 {
    UnkPadState_8040DD24();
    void Clear();

    /* 0x00 */ u32 m_down;
    /* 0x04 */ u32 m_up;
    /* 0x08 */ u32 m_pressed;
    /* 0x0C */ u32 m_released;
    /* 0x10 */ UnkPadStick_8040DD24 m_stick;
    /* 0x1C */ UnkPadStick_8040DD24 m_subStick;
};
SIZE_ASSERT(UnkPadState_8040DD24, 0x28);

/* A pad of the pad manager. */
struct UnkPad_8040DB6C {
    UnkPad_8040DB6C() : m_status(1) {}
    ~UnkPad_8040DB6C() {}

    /* 0x00 */ UnkPadState_8040DD24 m_state;
    /* 0x28 */ s32 m_status; /* the PadState state of the last read */
};
SIZE_ASSERT(UnkPad_8040DB6C, 0x2C);

extern "C" {
/* The pad manager (a function-local static) and one of its pads. */
void *fn_80009208(void);
UnkPad_8040DB6C *fn_8040DB6C(void *manager, u32 index);
}

#endif
