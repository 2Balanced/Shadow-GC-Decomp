/* SaveCycle::Check::Phase (class name from RTTI): the phase of the save/load cycle that checks the
 * memory card when the game starts. It runs a CheckModule cycle to find the card and the game's
 * file, then walks the player through the outcome with the save messages (fn_802E58B4): formatting
 * a broken card, creating a new file, deleting a damaged one, or loading the file and applying its
 * settings. Its state lives in a Check::Impl (class name inferred). The file name, the Impl's name
 * and all member names are inferred. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#include <boost/scoped_ptr.hpp>

#include "Game/MemoryCardCtrl.h"
#include "Game/SaveCycle.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* The sound output and vibration settings of the game state (class names unknown; declared as in
 * Game/AdvOption.cpp). */
class UnkSoundSetting_80305B34 {
  public:
    void Set(long mode);

    /* 0x0 */ s32 m_mode;
};

class UnkVibrationSetting_80305AD8 {
  public:
    void SetEnabled(bool enabled);

    /* 0x0 */ u8 m_enabled;
};

/* The game state (struct name unknown; see Game/AdvOption.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x58];
    /* 0x058 */ s32 m_language; /* the text language (name inferred) */
    /* 0x05C */ u8 field_0x5C[0x78 - 0x5C];
    /* 0x078 */ UnkSoundSetting_80305B34 m_sound;
    /* 0x07C */ UnkVibrationSetting_80305AD8 m_vibration;
    /* 0x07D */ u8 m_cardUsable; /* (name inferred; see Game/AdvMemoryCheck.cpp) */
    /* 0x07E */ u8 field_0x7E[0x82 - 0x7E];
    /* 0x082 */ u8 field_0x82; /* cleared while the card is being written (meaning unknown) */
    /* 0x083 */ u8 field_0x83[0x8C - 0x83];
    /* 0x08C */ s32 field_0x8C; /* loaded from the save data (meaning unknown) */
    /* 0x090 */ u8 field_0x90[0x150 - 0x90];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The save data (struct name unknown; see Game/AdvOption.cpp). Only the settings are named. */
struct UnkSaveData_800BB74C {
    /* 0x0000 */ u8 field_0x0[0x18];
    /* 0x0018 */ s32 m_language;
    /* 0x001C */ s32 m_voiceLanguage;
    /* 0x0020 */ s32 m_soundMode;
    /* 0x0024 */ bool m_vibration;
    /* 0x0028 */ s32 field_0x28;
};

/* The text language the CSD files are loaded in (meaning inferred; see Game/AdvOption.cpp). */
extern s32 lbl_805EE180;

/* The save message window (class name unknown; see Game/SaveCycleMessage.cpp). */
class UnkObject_802E5B2C;

extern "C" {
/* The save message window, showing message id until it is answered (returns the choice, 1 to 3,
 * once answered) and showing it without waiting (see Game/SaveCycleMessage.cpp). */
UnkObject_802E5B2C *fn_80117514(void);
int fn_802E58B4(UnkObject_802E5B2C *object, int id, int pad, int quick);
void fn_802E585C(UnkObject_802E5B2C *object, int id, int pad, int quick);
/* The memory card controller (Game/MemoryCardCtrl.cpp): starting a load, a create, a format and
 * a delete, whether a request is running, the card's status and the last request's result. */
bool fn_80299B4C(TMemoryCardCtrl *ctrl);
bool fn_80299A8C(TMemoryCardCtrl *ctrl);
bool fn_802999CC(TMemoryCardCtrl *ctrl);
bool fn_8029996C(TMemoryCardCtrl *ctrl);
int fn_80298EBC(TMemoryCardCtrl *ctrl);
int fn_80298E5C(TMemoryCardCtrl *ctrl);
int fn_80298EB4(TMemoryCardCtrl *ctrl);
/* The save data and the data kept after it (behaviour inferred). */
UnkSaveData_800BB74C *fn_800BB74C(void);
void fn_801E52F0(void *data, UnkSaveData_800BB74C *save);
/* OSGetSoundMode (Dolphin SDK; the symbol is still a placeholder). */
u32 fn_80376F58(void);
}

namespace SaveCycle {
namespace Check {

/* The phase's state (class name inferred). */
class Impl {
  public:
    Impl();
    ~Impl() {
        if (m_cycle != 0) {
            delete m_cycle;
            m_cycle = 0;
        }
    }

    bool Update();
    void Draw() {}

    void ApplySettings();
    void WaitMessage(int message, int next);
    void WaitCycle();
    void Ask(int message, int next1, int next2, int next3);

    /* 0x00 */ s32 m_state;
    /* 0x04 */ s32 m_nextOnResult; /* WaitCycle's next state when the cycle gives m_result */
    /* 0x08 */ s32 m_nextOtherwise;
    /* 0x0C */ UnkMemoryCard_802E472C *m_cycle; /* a CheckModule cycle */
    /* 0x10 */ u32 m_result;
};
SIZE_ASSERT(Impl, 0x14);

Impl::Impl() : m_state(1), m_cycle(0) {}

/* Shows message until it is answered and goes to the state for the answer (name inferred). */
void Impl::Ask(int message, int next1, int next2, int next3) {
    switch (fn_802E58B4(fn_80117514(), message, 0, 0)) {
    case 1:
        m_state = next1;
        break;
    case 2:
        m_state = next2;
        break;
    case 3:
        m_state = next3;
        break;
    }
}

/* Runs the cycle and, once it ends, goes on by its result (name inferred). */
void Impl::WaitCycle() {
    fn_802E3E7C(m_cycle);
    if (fn_802E3E34(m_cycle)) {
        if (m_result == UnkMemoryCard_802E472C::GetResult()) {
            m_state = m_nextOnResult;
        } else {
            m_state = m_nextOtherwise;
        }
    }
}

/* Shows message until it is confirmed, then goes to state next (name inferred). */
void Impl::WaitMessage(int message, int next) {
    if (fn_802E58B4(fn_80117514(), message, 0, 0) == 2) {
        m_state = next;
    }
}

bool Impl::Update() {
    TMemoryCardCtrl *ctrl = fn_801E5B80()->m_ctrl;
    if (ctrl == 0) {
        return false;
    }
    if (fn_80298E5C(ctrl) == 3) {
        switch (m_state) {
        case 0:
        case 21:
        case 22:
        case 23:
            break;
        default:
            m_state = 4;
            break;
        }
        if (m_cycle != 0) {
            delete m_cycle;
        }
        m_cycle = 0;
    }
    switch (m_state) {
    case 0:
        if (fn_802E58B4(fn_80117514(), 0x28, 0, 0) == 2) {
            m_state = 1;
        }
        break;
    case 1:
        m_state = 2;
        if (m_cycle != 0) {
            delete m_cycle;
        }
        m_cycle = new UnkMemoryCard_802E472C(PHASE_CHECK_MODULE);
        break;
    case 2:
        fn_802E3E7C(m_cycle);
        if (fn_802E3E34(m_cycle)) {
            switch (UnkMemoryCard_802E472C::GetResult()) {
            case 0:
            case 7:
                m_state = 15;
                break;
            case 1:
                Ask(1, 23, 1, 23);
                break;
            case 2:
                Ask(3, 10, 21, 23);
                break;
            case 3:
                Ask(6, 21, 1, 5);
                break;
            case 4:
                Ask(8, 23, 1, 23);
                break;
            case 5:
                Ask(2, 21, 1, 23);
                break;
            case 6:
                Ask(4, 21, 1, 23);
                break;
            case 8:
                Ask(0x2F, 21, 1, 18);
                break;
            case 9:
                Ask(5, 21, 1, 5);
                break;
            case 10:
            default:
                Ask(4, 21, 1, 23);
                break;
            }
        }
        break;
    case 3:
        WaitCycle();
        break;
    case 4:
        Ask(1, 23, 1, 23);
        break;
    case 5:
        Ask(0x2A, 6, 1, 23);
        break;
    case 6:
        if (fn_802999CC(ctrl)) {
            m_state = 7;
            lbl_8057D6A0.field_0x82 = 0;
        }
        break;
    case 7:
        lbl_8057D6A0.field_0x82 = 0;
        fn_802E585C(fn_80117514(), 0x17, 0, 0);
        if (fn_80298EBC(ctrl) == 0) {
            switch (fn_80298EB4(ctrl)) {
            case 0:
                m_state = 10;
                break;
            case 5:
            case 6:
                m_state = 9;
                break;
            default:
                m_state = 8;
                break;
            }
        }
        break;
    case 8:
        m_state = 1;
        break;
    case 9:
        Ask(4, 21, 1, 23);
        break;
    case 10:
        if (fn_80299A8C(ctrl)) {
            m_state = 11;
            lbl_8057D6A0.field_0x82 = 0;
        }
        break;
    case 11:
        lbl_8057D6A0.field_0x82 = 0;
        fn_802E585C(fn_80117514(), 0, 0, 0);
        if (fn_80298EBC(ctrl) == 0) {
            switch (fn_80298EB4(ctrl)) {
            case 0:
                m_state = 12;
                break;
            case 5:
                m_state = 9;
                break;
            default:
                m_state = 13;
                break;
            }
        }
        break;
    case 12:
        WaitMessage(0x30, 22);
        break;
    case 13:
        m_state = 1;
        break;
    case 14:
        Ask(8, 23, 1, 23);
        break;
    case 15:
        if (fn_80299B4C(ctrl)) {
            m_state = 16;
        }
        break;
    case 16:
        if (fn_80298EBC(ctrl) == 0) {
            switch (fn_80298EB4(ctrl)) {
            case 0:
                m_state = 22;
                ApplySettings();
                break;
            case 8:
                Ask(0x2F, 21, 1, 18);
                break;
            case 5:
                m_state = 9;
                break;
            default:
                m_state = 17;
                break;
            }
        }
        break;
    case 17:
        Ask(0x24, 21, 1, 23);
        break;
    case 18:
        if (fn_8029996C(ctrl)) {
            lbl_8057D6A0.field_0x82 = 0;
            m_state = 19;
        }
        break;
    case 19:
        lbl_8057D6A0.field_0x82 = 0;
        fn_802E585C(fn_80117514(), 0x33, 0, 0);
        if (fn_80298EBC(ctrl) == 0) {
            switch (fn_80298EB4(ctrl)) {
            case 0:
            case 9:
                m_state = 10;
                break;
            case 5:
                m_state = 9;
                break;
            default:
                m_state = 20;
                break;
            }
        }
        break;
    case 20:
        Ask(0x34, 21, 1, 23);
        break;
    case 21:
        Ask(9, 23, 1, 23);
        break;
    case 22:
        if (fn_802E58B4(fn_80117514(), 0x28, 0, 0) == 2) {
            UnkMemoryCard_802E472C::SetCardUsable(1);
            return false;
        }
        break;
    case 23:
        UnkMemoryCard_802E472C::SetCardUsable(0);
        return false;
    }
    return true;
}

/* Applies the loaded file's settings to the game state (name inferred). */
void Impl::ApplySettings() {
    UnkSaveData_800BB74C *save = fn_800BB74C();
    if (save != 0) {
        lbl_8057D6A0.field_0x8C = save->field_0x28;
        lbl_805EE180 = lbl_8057D6A0.m_language = save->m_language;
        lbl_8057D6A0.m_vibration.SetEnabled(save->m_vibration);
        if (fn_80376F58() == 1) {
            if (save->m_soundMode == 2) {
                lbl_8057D6A0.m_sound.Set(2);
            } else {
                lbl_8057D6A0.m_sound.Set(0);
            }
        } else {
            lbl_8057D6A0.m_sound.Set(1);
        }
    }
    fn_801E52F0((u8 *)fn_800BB74C() + 0x1F5C, save);
}

Phase::Phase() : CycleBase(PHASE_CHECK), m_impl(new Impl) {}

Phase::~Phase() {}

bool Phase::Update() {
    return m_impl->Update();
}

void Phase::Draw() {
    m_impl->Draw();
}

} // namespace Check
} // namespace SaveCycle
