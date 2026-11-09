/* The keyboard manager (Sonicteam::System::Peripheral; class name unknown): two keyboards and the
 * IKeyboardManagerImp that reads them. Game/StageManager.cpp keeps it as a function-local static
 * (fn_80177D1C) and Game/GamePeripheralTask.cpp installs a pad-driven keyboard in it. Other units
 * call its functions by their fn_ names, so they keep them; the file, class and member names are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/SonicteamPeripheral.h"
#include <dolphin/types.h>
#include "size_assert.h"

using namespace Sonicteam::System::Peripheral;

/* A keyboard of the manager (meaning of the members unknown; Game/TPauseManager.cpp reads the
 * key at 0x2). */
struct UnkKeyboard_8040D608 {
    UnkKeyboard_8040D608() {
        field_0x0 = 0;
        m_key = 0;
        field_0x4 = 0xFFFF;
        field_0x8 = 0;
        field_0xC = 4;
        field_0x10 = 30;
        field_0x14 = 0;
        field_0x18 = 0;
    }
    ~UnkKeyboard_8040D608() {}

    /* 0x00 */ u8 field_0x0;
    /* 0x01 */ u8 m_index;
    /* 0x02 */ u16 m_key;
    /* 0x04 */ u16 field_0x4;
    /* 0x08 */ u32 field_0x8;
    /* 0x0C */ u32 field_0xC;
    /* 0x10 */ u32 field_0x10;
    /* 0x14 */ u32 field_0x14;
    /* 0x18 */ u32 field_0x18;
    /* 0x1C */ u8 field_0x1C[0x2C - 0x1C];
    /* 0x2C */ struct UnkKeyboardManager_8040D594 *m_manager;
};
SIZE_ASSERT(UnkKeyboard_8040D608, 0x30);

struct UnkKeyboardManager_8040D594 {
    UnkKeyboardManager_8040D594();
    ~UnkKeyboardManager_8040D594();

    /* 0x00 */ UnkKeyboard_8040D608 m_keyboards[2];
    /* 0x60 */ std::auto_ptr<IKeyboardManagerImp> m_imp;
    /* 0x64 */ u32 field_0x64;
};
SIZE_ASSERT(UnkKeyboardManager_8040D594, 0x68);

extern "C" {
UnkKeyboard_8040D608 *fn_8040D4AC(UnkKeyboardManager_8040D594 *manager, int index);
void fn_8040D3F0(UnkKeyboardManager_8040D594 *manager, std::auto_ptr<IKeyboardManagerImp> &imp);
}

UnkKeyboardManager_8040D594::UnkKeyboardManager_8040D594() {
    for (int i = 0; i < 2; i++) {
        m_keyboards[i].m_index = i;
        m_keyboards[i].m_manager = this;
    }
}

UnkKeyboardManager_8040D594::~UnkKeyboardManager_8040D594() {}

/* A keyboard (name inferred). */
UnkKeyboard_8040D608 *fn_8040D4AC(UnkKeyboardManager_8040D594 *manager, int index) {
    return &manager->m_keyboards[index];
}

/* Installs the implementation (name inferred). */
void fn_8040D3F0(UnkKeyboardManager_8040D594 *manager, std::auto_ptr<IKeyboardManagerImp> &imp) {
    manager->m_imp = imp;
}
