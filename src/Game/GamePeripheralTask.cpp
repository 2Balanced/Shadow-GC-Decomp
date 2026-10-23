/* GamePeripheralTask (class name from RTTI): the task that reads the pads every frame. It updates
 * the pad manager (behind fn_80009208) and then the game's pad state (struct name unknown; the
 * function-local static behind fn_80051734, known to other units as UnkPads_80051734): the state of
 * both players' pads with the stick turned into buttons, button repeat counters, and the states of
 * the pads assigned to players 1 and 2. Init installs the peripheral implementations: the GameCube
 * pad manager (PJSPeripheral::ManagerImpGc), a pad-driven mouse and a pad-driven keyboard
 * (Sonicteam::System::Peripheral, class names from RTTI). File name from the class; all other
 * names are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first).
 *
 * Not matching yet: Update (0x8034997C) is 99.8%. Retail updates the repeat counters without a
 * branch (subfic/subfe and a mask) with other registers than GC/2.7 gives the masked form written
 * here; a conditional expression compiles to branches under GC/2.7. Everything else, including the
 * data and the link order, matches. */
#include <cmath>
#include <cstdlib>
#include <memory>

#include "Game/Task.h"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" void *memset(void *dst, int value, unsigned long n);
extern "C" void *__register_global_object(void *object, void *destructor, void *record);

namespace Sonicteam {
namespace System {
namespace Peripheral {

/* The interfaces of the peripheral implementations (class names from RTTI; the pure virtual
 * functions are not identified). */
class IManagerImp {
  public:
    virtual ~IManagerImp() {}
    virtual void Unk0() = 0;
    virtual void Unk1() = 0;
    virtual u32 Unk2() = 0;
    virtual void Unk3() = 0;
};

class IMouseManagerImp {
  public:
    virtual ~IMouseManagerImp() {}
    virtual void Unk0() = 0;
};

class IKeyboardManagerImp {
  public:
    virtual ~IKeyboardManagerImp() {}
    virtual void Unk0() = 0;
    virtual void Unk1() = 0;
    virtual void Unk2() = 0;
};

/* A mouse driven by a pad (class name from RTTI; the member's meaning is unknown). */
class Pad2MouseImplementor : public IMouseManagerImp {
  public:
    Pad2MouseImplementor(int arg) : field_0x4(arg) {}
    virtual ~Pad2MouseImplementor();
    virtual void Unk0();

    /* 0x4 */ s32 field_0x4;
};

namespace Pad2Keyboard {

/* A key binding of the pad-driven keyboard (struct name unknown). */
struct UnkKey_8040E330 {
    /* 0x0 */ u8 field_0x0[0xC];
};

/* A keyboard driven by a pad (class name from RTTI). */
class Implementor : public IKeyboardManagerImp {
  public:
    Implementor(int arg);
    virtual ~Implementor();
    virtual void Unk0();
    virtual void Unk1();
    virtual void Unk2();

    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ UnkKey_8040E330 m_keys[0x12];
    /* 0xE0 */ u8 field_0xE0[0x100 - 0xE0];
};

} // namespace Pad2Keyboard

} // namespace Peripheral
} // namespace System
} // namespace Sonicteam

namespace PJSPeripheral {

/* The GameCube pad manager implementation (class name from RTTI). */
class ManagerImpGc : public Sonicteam::System::Peripheral::IManagerImp {
  public:
    ManagerImpGc();
    virtual ~ManagerImpGc();
    virtual void Unk0();
    virtual void Unk1();
    virtual u32 Unk2();
    virtual void Unk3();

    /* 0x4 */ u8 field_0x4[0x10 - 0x4];
};

} // namespace PJSPeripheral

using namespace Sonicteam::System::Peripheral;

/* A stick's position (struct name unknown; see Game/ProgressiveModeCheck.cpp). */
struct UnkPadStick_8040DD24 {
    /* 0x0 */ float x;
    /* 0x4 */ float y;
    /* 0x8 */ s16 field_0x8;
    /* 0xA */ s16 field_0xA;
};

/* A pad's state for a frame (struct name unknown; the constructor is 0x8040DD24). The buttons are
 * the pad's with the main stick's directions added (names inferred). */
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

/* A pad of the pad manager (class name unknown; see Game/ProgressiveModeCheck.cpp). */
struct UnkPad_8040DB6C {
    const UnkPadState_8040DD24 &GetState() const {
        return m_state;
    }

    /* 0x00 */ UnkPadState_8040DD24 m_state;
};

/* The mouse manager (class name unknown): a function-local static behind GetMouse. */
struct UnkMouse_8040D7C4;

extern "C" {
/* The pad manager: updating it, installing its implementation, and a pad (behaviour inferred). */
void *fn_80009208(void);
void fn_8040D9EC(void *manager);
void fn_8040D8DC(void *manager, std::auto_ptr<IManagerImp> imp);
UnkPad_8040DB6C *fn_8040DB6C(void *manager, u32 index);
/* The mouse manager: its constructor and destructor, installing an implementation, a mouse and its
 * speed (behaviour inferred). */
UnkMouse_8040D7C4 *fn_8040D7C4(void *object);
void fn_8040D714(void *object, int flags);
void fn_8040D640(UnkMouse_8040D7C4 *manager, std::auto_ptr<IMouseManagerImp> imp, int index);
void *fn_8040D708(UnkMouse_8040D7C4 *manager, int index);
void fn_8040D850(void *mouse, float speed);
/* The keyboard manager, installing its implementation, and binding a key (behaviour inferred). */
void *fn_80177D1C(void);
void fn_8040D3F0(void *manager, std::auto_ptr<IKeyboardManagerImp> imp);
void fn_8040E330(Pad2Keyboard::UnkKey_8040E330 *key, int code);
}

/* The game's pad state (struct name unknown; names inferred). Index 2 of the player pads means
 * either pad. */
struct UnkPads_80051734 {
    UnkPads_80051734();
    ~UnkPads_80051734();
    bool IsRepeated(u32 pad, u32 mask, int delay, int interval);
    void Update();
    void Reset();
    void Init();

    /* Whether all of mask, or all of its pad or stick part, was pressed on pad. */
    bool IsPressed(u32 pad, u32 mask) {
        u32 buttons = m_states[pad].m_pressed;
        if (mask == (mask & buttons)) {
            return true;
        }
        u32 part = mask & 0x3C0;
        if (part != 0 && part == (part & buttons)) {
            return true;
        }
        part = mask & 0x3C0000;
        if (part != 0 && part == (part & buttons)) {
            return true;
        }
        return false;
    }

    /* 0x000 */ s16 m_repeat[2][8]; /* frames each repeatable button has been held */
    /* 0x020 */ UnkPad_8040DB6C *m_pads[2];
    /* 0x028 */ UnkPadState_8040DD24 m_states[6]; /* pads 0 and 1, -, either, player 1, player 2 */
    /* 0x118 */ u8 m_initialized;
    /* 0x11C */ u32 m_player1Pad;
    /* 0x120 */ u32 m_player2Pad;
};
SIZE_ASSERT(UnkPads_80051734, 0x124);

class GamePeripheralTask : public Task {
  public:
    GamePeripheralTask(Task *parent);
    virtual ~GamePeripheralTask();
    virtual void Tick(float dt);
};
SIZE_ASSERT(GamePeripheralTask, 0x28);

extern "C" UnkPads_80051734 *fn_80051734(void);

/* The buttons with repeat counters. */
static const u32 s_repeatMasks[8] = {
    0x40, 0x80, 0x200, 0x100, 0x40000, 0x80000, 0x200000, 0x100000,
};

static const char *s_taskName = "GamePeripheralTask";

/* The mouse manager's instance (in another unit) and its function-local static. */
extern UnkMouse_8040D7C4 *lbl_805EDF88;
static s8 s_mouseInitialized;
static u32 s_mouseRecord[3];
static u32 s_mouseObject[15];

inline UnkMouse_8040D7C4 *&MouseRef(UnkMouse_8040D7C4 *&x) {
    return x;
}

inline UnkMouse_8040D7C4 *GetMouse() {
    if (lbl_805EDF88 == 0) {
        if (!s_mouseInitialized) {
            __register_global_object(fn_8040D7C4(s_mouseObject), fn_8040D714, s_mouseRecord);
            s_mouseInitialized = 1;
        }
        lbl_805EDF88 = (UnkMouse_8040D7C4 *)s_mouseObject;
    }
    return MouseRef(lbl_805EDF88);
}

UnkPads_80051734::UnkPads_80051734() {
    memset(m_repeat, 0, sizeof(m_repeat));
    m_initialized = 0;
    m_player1Pad = 0;
    m_player2Pad = 1;
}

UnkPads_80051734::~UnkPads_80051734() {
    memset(m_repeat, 0, sizeof(m_repeat));
    for (int i = 0; i < 2; i++) {
        m_pads[i] = 0;
    }
    m_initialized = 0;
}

void UnkPads_80051734::Init() {
    if (m_initialized != 1) {
        m_initialized = 1;
        {
            std::auto_ptr<IManagerImp> imp(new PJSPeripheral::ManagerImpGc);
            fn_8040D8DC(fn_80009208(), imp);
            for (u32 i = 0; i < 2; i++) {
                m_pads[i] = fn_8040DB6C(fn_80009208(), i);
            }
        }
        {
            std::auto_ptr<IMouseManagerImp> mouse(new Pad2MouseImplementor(3));
            fn_8040D640(GetMouse(), mouse, 0);
            fn_8040D850(fn_8040D708(GetMouse(), 0), 1.5f);
        }
        Pad2Keyboard::Implementor *keyboard = new Pad2Keyboard::Implementor(3);
        fn_8040E330(&keyboard->m_keys[4], 0x47);
        fn_8040E330(&keyboard->m_keys[5], 0x48);
        fn_8040E330(&keyboard->m_keys[13], 0x3B);
        std::auto_ptr<IKeyboardManagerImp> keys(keyboard);
        fn_8040D3F0(fn_80177D1C(), keys);
    }
}

void UnkPads_80051734::Reset() {}

void UnkPads_80051734::Update() {
    for (int i = 0; i < 2; i++) {
        if (m_pads[i] != 0) {
            u32 up = m_states[i].m_up;
            u32 down = m_states[i].m_down;
            m_states[i] = m_pads[i]->GetState();
            if (m_states[i].m_down & 0x20) {
                m_states[i].m_down |= 0x800;
            }
            if (m_states[i].m_up & 0x20) {
                m_states[i].m_up |= 0x800;
            }
            if (m_states[i].m_pressed & 0x20) {
                m_states[i].m_pressed |= 0x800;
            }
            if (m_states[i].m_released & 0x20) {
                m_states[i].m_released |= 0x800;
            }
            if (m_states[i].m_stick.x > 0.5f) {
                m_states[i].m_down |= 0x200000;
                m_states[i].m_up &= ~0x200000;
            } else if (m_states[i].m_stick.x < -0.5f) {
                m_states[i].m_down |= 0x100000;
                m_states[i].m_up &= ~0x100000;
            }
            if (m_states[i].m_stick.y > 0.5f) {
                m_states[i].m_down |= 0x80000;
                m_states[i].m_up &= ~0x80000;
            } else if (m_states[i].m_stick.y < -0.5f) {
                m_states[i].m_down |= 0x40000;
                m_states[i].m_up &= ~0x40000;
            }
            if ((up & 0x40000) && (m_states[i].m_down & 0x40000)) {
                m_states[i].m_pressed |= 0x40000;
            }
            if ((up & 0x80000) && (m_states[i].m_down & 0x80000)) {
                m_states[i].m_pressed |= 0x80000;
            }
            if ((up & 0x200000) && (m_states[i].m_down & 0x200000)) {
                m_states[i].m_pressed |= 0x200000;
            }
            if ((up & 0x100000) && (m_states[i].m_down & 0x100000)) {
                m_states[i].m_pressed |= 0x100000;
            }
            if ((down & 0x40000) && (m_states[i].m_up & 0x40000)) {
                m_states[i].m_released |= 0x40000;
            }
            if ((down & 0x80000) && (m_states[i].m_up & 0x80000)) {
                m_states[i].m_released |= 0x80000;
            }
            if ((down & 0x200000) && (m_states[i].m_up & 0x200000)) {
                m_states[i].m_released |= 0x200000;
            }
            if ((down & 0x100000) && (m_states[i].m_up & 0x100000)) {
                m_states[i].m_released |= 0x100000;
            }
            for (int j = 0; j < 8; j++) {
                m_repeat[i][j] = (s16)(m_repeat[i][j] + 1) & -((m_states[i].m_down & s_repeatMasks[j]) != 0);
            }
        }
    }
    if (m_player1Pad >= 2 || m_player2Pad >= 2) {
        UnkPadState_8040DD24 &either = m_states[3];
        either.Clear();
        for (int i = 0; i < 2; i++) {
            if (i != m_player1Pad && i != m_player2Pad) {
                either.m_down |= m_states[i].m_down;
                either.m_up |= m_states[i].m_up;
                either.m_pressed |= m_states[i].m_pressed;
                either.m_released |= m_states[i].m_released;
                if (std::abs(either.m_stick.x) < std::abs(m_states[i].m_stick.x)) {
                    either.m_stick.x = m_states[i].m_stick.x;
                }
                if (std::abs(either.m_stick.y) < std::abs(m_states[i].m_stick.y)) {
                    either.m_stick.y = m_states[i].m_stick.y;
                }
                if (std::abs(either.m_stick.field_0x8) < std::abs(m_states[i].m_stick.field_0x8)) {
                    either.m_stick.field_0x8 = m_states[i].m_stick.field_0x8;
                }
                if (std::abs(either.m_stick.field_0xA) < std::abs(m_states[i].m_stick.field_0xA)) {
                    either.m_stick.field_0xA = m_states[i].m_stick.field_0xA;
                }
                if (std::abs(either.m_subStick.x) < std::abs(m_states[i].m_subStick.x)) {
                    either.m_subStick.x = m_states[i].m_subStick.x;
                }
                if (std::abs(either.m_subStick.y) < std::abs(m_states[i].m_subStick.y)) {
                    either.m_subStick.y = m_states[i].m_subStick.y;
                }
                if (std::abs(either.m_subStick.field_0x8) < std::abs(m_states[i].m_subStick.field_0x8)) {
                    either.m_subStick.field_0x8 = m_states[i].m_subStick.field_0x8;
                }
                if (std::abs(either.m_subStick.field_0xA) < std::abs(m_states[i].m_subStick.field_0xA)) {
                    either.m_subStick.field_0xA = m_states[i].m_subStick.field_0xA;
                }
            }
        }
    }
    m_states[4] = m_states[m_player1Pad];
    m_states[5] = m_states[m_player2Pad];
}

/* Whether a button of mask was pressed on pad, or has been held for delay frames and then a
 * multiple of interval frames (pad 4 and 5 mean players 1 and 2, other pads from 2 either pad). */
bool UnkPads_80051734::IsRepeated(u32 pad, u32 mask, int delay, int interval) {
    if (mask == 0) {
        return false;
    }
    if (pad == 4 && m_player1Pad < 2) {
        pad = m_player1Pad;
    } else if (pad == 5) {
        pad = m_player2Pad;
    }
    if (pad < 2) {
        if (IsPressed(pad, mask)) {
            return true;
        }
        for (int i = 0; i < 8; i++) {
            if (mask & s_repeatMasks[i]) {
                int held = m_repeat[pad][i] - (delay + 1);
                if (held >= 0 && held % interval == 0) {
                    return true;
                }
            }
        }
    } else {
        for (int i = 0; i < 2; i++) {
            if (IsRepeated(i, mask, delay, interval)) {
                return true;
            }
        }
    }
    return false;
}

GamePeripheralTask::GamePeripheralTask(Task *parent) : Task(parent) {
    m_name = s_taskName;
    m_flags |= 0x100;
}

GamePeripheralTask::~GamePeripheralTask() {}

void GamePeripheralTask::Tick(float dt) {
    fn_8040D9EC(fn_80009208());
    fn_80051734()->Update();
}
