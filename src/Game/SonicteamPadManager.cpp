/* The pad manager (Sonicteam::System::Peripheral; class name unknown): four pads and the
 * IManagerImp that reads them (Game/ManagerImpGc.cpp on the GameCube). Every frame it turns the
 * reads into each pad's state: buttons held, pressed and released, and the two sticks with their
 * dead zone taken out, scaled to -1..1. Game/CameraModeMtnViewer.cpp keeps it as a function-local
 * static (fn_80009208). Other units call its functions by their fn_ names, so they keep them; the
 * file, class and member names are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first).
 *
 * NonMatching: 13 of 14 functions match. fn_8040D9EC (the read) has retail's instructions, saved
 * registers and stack layout, but GC/2.7 gives the state pointer r3 and the previous status r4
 * where retail has r6 and r3 (14 rows of 96). About 20 spellings tried (pad and state as locals,
 * references or indexing, declaration orders, the switch value as a local); reading the default
 * case's status through states[i] is what brought the saved registers into line. */
#include <algorithm>

#include "Game/SonicteamPeripheral.h"
#include <dolphin/types.h>
#include "size_assert.h"

using namespace Sonicteam::System::Peripheral;

struct UnkPadManager_8040DC5C {
    UnkPadManager_8040DC5C();
    ~UnkPadManager_8040DC5C();

    /* 0x00 */ UnkPad_8040DB6C m_pads[4];
    /* 0xB0 */ u32 m_count;
    /* 0xB4 */ std::auto_ptr<IManagerImp> m_imp;
};
SIZE_ASSERT(UnkPadManager_8040DC5C, 0xB8);

extern "C" {
float fn_8000FEE4(float x); /* square root */
void fn_8040DE1C(s16 *x, s16 *y, s16 range, s16 deadZone);
void fn_8040DD60(UnkPadStick_8040DD24 *stick, s16 deadZone, s16 range);
u32 fn_8040DB78(UnkPadManager_8040DC5C *manager);
void fn_8040D9EC(UnkPadManager_8040DC5C *manager);
void fn_8040D8DC(UnkPadManager_8040DC5C *manager, std::auto_ptr<IManagerImp> &imp);
}

/* The most pads the manager reads. */
static u32 s_maxPads = 4;

/* Takes the dead zone out of a stick's raw position and limits it to range (name inferred). */
void fn_8040DE1C(s16 *px, s16 *py, s16 range, s16 deadZone) {
    int x = *px;
    int y = *py;
    if (-deadZone < x && x < deadZone) {
        x = 0;
    } else if (x > 0) {
        x -= deadZone;
    } else {
        x += deadZone;
    }
    if (-deadZone < y && y < deadZone) {
        y = 0;
    } else if (y > 0) {
        y -= deadZone;
    } else {
        y += deadZone;
    }
    int lengthSq = x * x + y * y;
    if (range * range < lengthSq) {
        int length = fn_8000FEE4(lengthSq);
        x = x * range / length;
        y = y * range / length;
    }
    *px = x;
    *py = y;
}

/* Sets a stick's position from its raw one (name inferred). */
void fn_8040DD60(UnkPadStick_8040DD24 *stick, s16 deadZone, s16 range) {
    s16 x = stick->m_rawX;
    s16 y = stick->m_rawY;
    fn_8040DE1C(&x, &y, range, deadZone);
    stick->x = (float)x / (float)range;
    stick->y = (float)y / (float)range;
}

UnkPadState_8040DD24::UnkPadState_8040DD24() {
    m_down = 0;
    m_up = 0;
    m_pressed = 0;
    m_released = 0;
    m_stick.x = 0.0f;
    m_stick.y = 0.0f;
    m_stick.m_rawX = 0;
    m_stick.m_rawY = 0;
    m_subStick.x = 0.0f;
    m_subStick.y = 0.0f;
    m_subStick.m_rawX = 0;
    m_subStick.m_rawY = 0;
}

void UnkPadState_8040DD24::Clear() {
    m_down = m_up = m_pressed = m_released = 0;
    m_stick.x = m_stick.y = 0.0f;
    m_stick.m_rawX = m_stick.m_rawY = 0;
    m_subStick.x = m_subStick.y = 0.0f;
    m_subStick.m_rawX = m_subStick.m_rawY = 0;
}

UnkPadManager_8040DC5C::UnkPadManager_8040DC5C() : m_count(0) {}

UnkPadManager_8040DC5C::~UnkPadManager_8040DC5C() {}

/* The number of pads read (name inferred). */
u32 fn_8040DB78(UnkPadManager_8040DC5C *manager) {
    return manager->m_count;
}

/* A pad (name inferred). */
UnkPad_8040DB6C *fn_8040DB6C(void *object, u32 index) {
    UnkPadManager_8040DC5C *manager = (UnkPadManager_8040DC5C *)object;
    return &manager->m_pads[index];
}

/* Reads the pads (name inferred). */
void fn_8040D9EC(UnkPadManager_8040DC5C *manager) {
    PadState states[4];
    StickRange range;
    if (manager->m_imp->Read(states, manager->m_count)) {
        for (u32 i = 0; i < manager->m_count; i++) {
            UnkPad_8040DB6C *pad = &manager->m_pads[i];
            PadState *state = &states[i];
            s32 status = pad->m_status;
            switch (state->m_state) {
            case 0: {
                pad->m_status = state->m_state;
                u32 buttons = state->m_buttons;
                u32 old = pad->m_state.m_down;
                pad->m_state.m_down = buttons;
                pad->m_state.m_up = ~buttons;
                if (status != 0) {
                    pad->m_state.m_released = 0;
                    pad->m_state.m_pressed = 0;
                } else {
                    u32 changed = old ^ buttons;
                    pad->m_state.m_pressed = buttons & changed;
                    pad->m_state.m_released = old & changed;
                }
                pad->m_state.m_stick.m_rawX = state->m_stick[0];
                pad->m_state.m_stick.m_rawY = state->m_stick[1];
                pad->m_state.m_subStick.m_rawX = state->m_substick[0];
                pad->m_state.m_subStick.m_rawY = state->m_substick[1];
                manager->m_imp->GetStickRange(i, &range);
                fn_8040DD60(&pad->m_state.m_stick, range.m_deadZone[0], range.m_range[0]);
                fn_8040DD60(&pad->m_state.m_subStick, range.m_deadZone[1], range.m_range[1]);
                break;
            }
            case 2:
                pad->m_status = 0;
                break;
            case 3:
                pad->m_status = state->m_state;
                pad->m_state.m_released = 0;
                pad->m_state.m_pressed = 0;
                break;
            default:
                pad->m_status = states[i].m_state;
                pad->m_state.Clear();
                break;
            }
        }
    }
}

/* Installs the implementation and counts its pads (name inferred). */
void fn_8040D8DC(UnkPadManager_8040DC5C *manager, std::auto_ptr<IManagerImp> &imp) {
    manager->m_imp = imp;
    if (manager->m_imp.get() != 0) {
        u32 count = manager->m_imp->GetPadCount();
        manager->m_count = std::min(count, s_maxPads);
    } else {
        manager->m_count = 0;
    }
}
