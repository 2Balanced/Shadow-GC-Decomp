/* The mouse manager (Sonicteam::System::Peripheral; class name unknown): a mouse and the
 * IMouseManagerImp that reads it. Game/GamePeripheralTask.cpp keeps it as a function-local static
 * and installs a pad-driven mouse in it (Pad2MouseImplementor). Other units call its functions by
 * their fn_ names, so they keep them; the file, class and member names are inferred. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/SonicteamPeripheral.h"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" void *memset(void *dst, int value, unsigned long n);

using namespace Sonicteam::System::Peripheral;

/* A mouse of the manager (meaning of the members unknown but the speed). */
struct UnkMouseState_8040D858 {
    UnkMouseState_8040D858();
    ~UnkMouseState_8040D858() {}

    /* 0x00 */ u32 field_0x0;
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ u32 field_0x8;
    /* 0x0C */ u32 field_0xC;
    /* 0x10 */ u32 field_0x10;
    /* 0x14 */ u32 field_0x14;
    /* 0x18 */ u32 field_0x18;
    /* 0x1C */ u32 field_0x1C;
    /* 0x20 */ float field_0x20;
    /* 0x24 */ float m_speed;
    /* 0x28 */ u8 field_0x28[8];
    /* 0x30 */ u8 field_0x30[8];
};
SIZE_ASSERT(UnkMouseState_8040D858, 0x38);

struct UnkMouse_8040D7C4 {
    UnkMouse_8040D7C4();
    ~UnkMouse_8040D7C4();

    /* 0x00 */ UnkMouseState_8040D858 m_mice[1];
    /* 0x38 */ std::auto_ptr<IMouseManagerImp> m_imps[1];
};
SIZE_ASSERT(UnkMouse_8040D7C4, 0x3C);

extern "C" {
void fn_8040D850(UnkMouseState_8040D858 *mouse, float speed);
UnkMouseState_8040D858 *fn_8040D708(UnkMouse_8040D7C4 *manager, int index);
void fn_8040D640(UnkMouse_8040D7C4 *manager, std::auto_ptr<IMouseManagerImp> &imp, int index);
}

UnkMouseState_8040D858::UnkMouseState_8040D858() {
    field_0x0 = 0;
    field_0x4 = 0;
    field_0x8 = 0;
    field_0x14 = 30;
    field_0x18 = 4;
    field_0x1C = 30;
    field_0x20 = 1.0f;
    m_speed = 1.0f;
    memset(field_0x28, 0, sizeof(field_0x28));
    memset(field_0x30, 0, sizeof(field_0x30));
}

/* Sets the speed (name inferred). */
void fn_8040D850(UnkMouseState_8040D858 *mouse, float speed) {
    mouse->m_speed = speed;
}

UnkMouse_8040D7C4::UnkMouse_8040D7C4() {}

UnkMouse_8040D7C4::~UnkMouse_8040D7C4() {}

/* A mouse (name inferred). */
UnkMouseState_8040D858 *fn_8040D708(UnkMouse_8040D7C4 *manager, int index) {
    return &manager->m_mice[index];
}

/* Installs the implementation of a mouse (name inferred). */
void fn_8040D640(UnkMouse_8040D7C4 *manager, std::auto_ptr<IMouseManagerImp> &imp, int index) {
    manager->m_imps[index] = imp;
}
