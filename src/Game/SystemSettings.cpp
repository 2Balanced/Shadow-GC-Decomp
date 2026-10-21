/* The rumble and sound-output settings the game state keeps at 0x7C and 0x78 (the option screen
 * changes them), and the getter of a flag nothing in the game sets. There is no RTTI here: the
 * file, class and function names are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include <dolphin/types.h>
#include "size_assert.h"

class UnkVibrationManager_80177F38;

extern "C" {
/* The rumble manager and stopping all of its requests (behaviour inferred). */
UnkVibrationManager_80177F38 *fn_80177F38(void);
void fn_8034B390(UnkVibrationManager_80177F38 *manager);

/* The console's sound mode: 0 mono, 1 stereo (the SDK's OSGetSoundMode and OSSetSoundMode,
 * inferred from their use). */
u32 fn_80376F58(void);
void fn_80376FD8(u32 mode);

/* The sound manager and its output mode (behaviour inferred). */
void *fn_80053164(void);
void fn_801D4D0C(void *manager, s32 mode);
}

/* A flag nothing sets (name inferred). */
static s32 s_flag_805F0950;

/* Returns the flag (name of the function unknown). */
extern "C" s32 fn_80305BE8(void) {
    return s_flag_805F0950;
}

/* The sound output setting (class name unknown): 0 stereo, 1 mono, 2 surround (inferred). */
class UnkSoundSetting_80305B34 {
  public:
    /* Takes the console's sound mode (name inferred; not a constructor: it does not return this). */
    void Init();
    void Set(s32 mode);

    /* 0x0 */ s32 m_mode;
};
SIZE_ASSERT(UnkSoundSetting_80305B34, 0x4);

void UnkSoundSetting_80305B34::Set(s32 mode) {
    if (mode != m_mode) {
        m_mode = mode;
        if (mode == 1) {
            fn_80376FD8(0);
        } else {
            fn_80376FD8(1);
        }
        fn_801D4D0C(fn_80053164(), m_mode);
    }
}

void UnkSoundSetting_80305B34::Init() {
    switch (fn_80376F58()) {
    case 1:
    default:
        m_mode = 0;
        break;
    case 0:
        m_mode = 1;
        break;
    }
}

/* The rumble setting (class name unknown). */
class UnkVibrationSetting_80305AD8 {
  public:
    UnkVibrationSetting_80305AD8();
    void SetEnabled(bool enabled);

    /* 0x0 */ bool m_enabled;
    /* 0x1 */ u8 field_0x1;
    /* 0x2 */ u8 field_0x2;
};

void UnkVibrationSetting_80305AD8::SetEnabled(bool enabled) {
    if (m_enabled != enabled) {
        m_enabled = enabled;
        if (!m_enabled) {
            fn_8034B390(fn_80177F38());
        }
    }
}

UnkVibrationSetting_80305AD8::UnkVibrationSetting_80305AD8() {
    m_enabled = true;
    field_0x1 = 0;
    field_0x2 = 0;
}
