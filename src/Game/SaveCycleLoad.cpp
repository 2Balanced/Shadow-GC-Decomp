/* SaveCycle::Load::Phase (class name from RTTI): the phase of the save/load cycle that loads the
 * game's file from the memory card. It runs a CheckModule cycle to find the card and the file,
 * loads it and applies its settings (and the sound stream that was playing), and walks the player
 * through failures with the save messages (fn_802E58B4): a missing card or file, a broken card to
 * format, a damaged file to delete. Its state lives in a Load::Impl (class name inferred). The file
 * name, the Impl's name and all member names are inferred. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
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
/* The memory card controller (Game/MemoryCardCtrl.cpp): starting a load, a format and a delete,
 * whether a request is running, the card's status, the last request's result and the number of
 * files the last search found. */
bool fn_80299B4C(TMemoryCardCtrl *ctrl);
bool fn_802999CC(TMemoryCardCtrl *ctrl);
bool fn_8029996C(TMemoryCardCtrl *ctrl);
int fn_80298EBC(TMemoryCardCtrl *ctrl);
int fn_80298E5C(TMemoryCardCtrl *ctrl);
int fn_80298EB4(TMemoryCardCtrl *ctrl);
int fn_80299950(TMemoryCardCtrl *ctrl);
/* The save data and the data kept after it (behaviour inferred). */
UnkSaveData_800BB74C *fn_800BB74C(void);
void fn_801E52F0(void *data, UnkSaveData_800BB74C *save);
/* OSGetSoundMode (Dolphin SDK; the symbol is still a placeholder). */
u32 fn_80376F58(void);
/* The sound manager, the stream it is playing and playing a stream (behaviour inferred; see
 * Game/DiscErrorHandler.cpp). */
void *fn_80053164(void);
const char *fn_801D51CC(void *manager);
void fn_801D524C(void *manager, const char *name);
char *strcpy(char *dst, const char *src);
}

namespace SaveCycle {
namespace Load {

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
    /* 0x14 */ float m_time; /* restarted on every state change (never read here) */
};
SIZE_ASSERT(Impl, 0x18);

Impl::Impl() : m_state(0), m_cycle(0) {}

/* Shows message until it is answered and goes to the state for the answer (name inferred). */
void Impl::Ask(int message, int next1, int next2, int next3) {
    switch (fn_802E58B4(fn_80117514(), message, 0, 0)) {
    case 1:
        m_state = next1;
        m_time = 0.0f;
        break;
    case 2:
        m_state = next2;
        m_time = 0.0f;
        break;
    case 3:
        m_state = next3;
        m_time = 0.0f;
        break;
    }
}

/* Runs the cycle and, once it ends, goes on by its result (name inferred). */
void Impl::WaitCycle() {
    fn_802E3E7C(m_cycle);
    if (fn_802E3E34(m_cycle)) {
        if (m_result == UnkMemoryCard_802E472C::GetResult()) {
            m_state = m_nextOnResult;
            m_time = 0.0f;
        } else {
            m_state = m_nextOtherwise;
            m_time = 0.0f;
        }
    }
}

/* Shows message until it is confirmed, then goes to state next (name inferred). */
void Impl::WaitMessage(int message, int next) {
    if (fn_802E58B4(fn_80117514(), message, 0, 0) == 2) {
        m_state = next;
        m_time = 0.0f;
    }
}

bool Impl::Update() {
    TMemoryCardCtrl *ctrl = fn_801E5B80()->m_ctrl;
    if (ctrl == 0) {
        return false;
    }
    if (fn_80298E5C(ctrl) == 3 && m_state != 18 && m_state != 19) {
        if (m_cycle != 0) {
            delete m_cycle;
        }
        m_cycle = 0;
        m_state = 6;
        m_time = 0.0f;
    }
    switch (m_state) {
    case 0:
        m_state = 1;
        m_time = 0.0f;
        if (m_cycle != 0) {
            delete m_cycle;
        }
        m_cycle = new UnkMemoryCard_802E472C(PHASE_CHECK_MODULE);
        break;
    case 1:
        fn_802E3E7C(m_cycle);
        if (fn_802E3E34(m_cycle)) {
            switch (UnkMemoryCard_802E472C::GetResult()) {
            case 0:
                m_state = 3;
                m_time = 0.0f;
                break;
            case 1:
                Ask(0x11, 19, 0, 19);
                break;
            case 2:
                Ask(0xD, 19, 0, 19);
                break;
            case 3:
                Ask(0x29, 19, 0, 12);
                break;
            case 4:
                if (fn_80299950(ctrl) > 0) {
                    m_state = 3;
                    m_time = 0.0f;
                } else {
                    Ask(0xD, 19, 0, 19);
                }
                break;
            case 5:
                Ask(0x10, 19, 0, 19);
                break;
            case 6:
                Ask(0x12, 19, 0, 19);
                break;
            case 7:
                m_state = 3;
                m_time = 0.0f;
                break;
            case 8:
                Ask(0x14, 19, 0, 8);
                break;
            case 9:
                Ask(0x32, 19, 0, 12);
                break;
            case 10:
            default:
                Ask(0x12, 19, 0, 19);
                break;
            }
        }
        break;
    case 2:
        WaitCycle();
        break;
    case 3:
        fn_802E585C(fn_80117514(), 0x22, 0, 0);
        if (fn_80299B4C(ctrl)) {
            m_state = 4;
            m_time = 0.0f;
        }
        break;
    case 4:
        fn_802E585C(fn_80117514(), 0x22, 0, 0);
        if (fn_80298EBC(ctrl) == 0) {
            switch (fn_80298EB4(ctrl)) {
            case 0:
                m_state = 18;
                m_time = 0.0f;
                ApplySettings();
                break;
            case 3:
                m_state = 6;
                m_time = 0.0f;
                break;
            case 2:
            case 5:
                m_state = 7;
                m_time = 0.0f;
                break;
            case 8:
            case 9:
                m_state = 5;
                m_time = 0.0f;
                break;
            default:
                m_state = 0;
                m_time = 0.0f;
                break;
            }
        }
        break;
    case 5:
        Ask(0x14, 19, 0, 8);
        break;
    case 6:
        Ask(0x11, 19, 0, 19);
        break;
    case 7:
        Ask(0x12, 19, 0, 19);
        break;
    case 8:
        if (fn_8029996C(ctrl)) {
            lbl_8057D6A0.field_0x82 = 0;
            m_state = 9;
            m_time = 0.0f;
        }
        break;
    case 9:
        lbl_8057D6A0.field_0x82 = 0;
        fn_802E585C(fn_80117514(), 0x33, 0, 0);
        if (fn_80298EBC(ctrl) == 0) {
            switch (fn_80298EB4(ctrl)) {
            case 0:
            case 9:
                m_state = 10;
                m_time = 0.0f;
                break;
            case 5:
                m_state = 7;
                m_time = 0.0f;
                break;
            default:
                m_state = 11;
                m_time = 0.0f;
                break;
            }
        }
        break;
    case 10:
        WaitMessage(0x25, 19);
        break;
    case 11:
        WaitMessage(0x26, 19);
        break;
    case 12:
        Ask(0x2A, 13, 0, 19);
        break;
    case 13:
        if (fn_802999CC(ctrl)) {
            m_state = 14;
            m_time = 0.0f;
            lbl_8057D6A0.field_0x82 = 0;
        }
        break;
    case 14:
        lbl_8057D6A0.field_0x82 = 0;
        fn_802E585C(fn_80117514(), 0x17, 0, 0);
        if (fn_80298EBC(ctrl) == 0) {
            switch (fn_80298EB4(ctrl)) {
            case 0:
                m_state = 15;
                m_time = 0.0f;
                break;
            case 5:
            case 6:
                m_state = 17;
                m_time = 0.0f;
                break;
            default:
                m_state = 16;
                m_time = 0.0f;
                break;
            }
        }
        break;
    case 15:
        WaitMessage(0x1E, 19);
        break;
    case 16:
        m_state = 0;
        m_time = 0.0f;
        break;
    case 17:
        Ask(0x12, 19, 0, 19);
        break;
    case 18:
        lbl_8057D6A0.m_cardUsable = 1;
        UnkMemoryCard_802E472C::SetCardUsable(1);
        return false;
    case 19:
        UnkMemoryCard_802E472C::SetCardUsable(0);
        return false;
    }
    return true;
}

/* Applies the loaded file's settings to the game state and restarts the sound stream that was
 * playing (name inferred). */
void Impl::ApplySettings() {
    char stream[0x100];
    UnkSaveData_800BB74C *save = fn_800BB74C();
    if (save != 0) {
        const char *current = fn_801D51CC(fn_80053164());
        if (current != 0) {
            strcpy(stream, current);
        }
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
        if (stream[0] != 0) {
            fn_801D524C(fn_80053164(), stream);
        }
    }
    fn_801E52F0((u8 *)fn_800BB74C() + 0x1F5C, save);
}

Phase::Phase() : CycleBase(PHASE_LOAD), m_impl(new Impl) {}

Phase::~Phase() {}

bool Phase::Update() {
    return m_impl->Update();
}

void Phase::Draw() {
    m_impl->Draw();
}

} // namespace Load
} // namespace SaveCycle
