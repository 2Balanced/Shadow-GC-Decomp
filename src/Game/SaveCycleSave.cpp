/* SaveCycle::Save::Phase (class name from RTTI): the phase of the save/load cycle that writes the
 * game's file to the memory card. It first announces what was unlocked since the last save, then
 * runs a CheckModule cycle to find the card and the file, creates or overwrites the file, and walks
 * the player through failures with the save messages (fn_802E58B4): a missing or full card, a
 * broken card to format, a damaged file to delete. Its state lives in a Save::Impl (class name
 * inferred). The file name, the Impl's name and all member names are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include <boost/scoped_ptr.hpp>

#include "Game/MemoryCardCtrl.h"
#include "Game/SaveCycle.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* The game state (struct name unknown; see Game/AdvOption.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x7D];
    /* 0x07D */ u8 m_cardUsable; /* (name inferred; see Game/AdvMemoryCheck.cpp) */
    /* 0x07E */ u8 field_0x7E[0x82 - 0x7E];
    /* 0x082 */ u8 field_0x82; /* cleared while the card is being written (meaning unknown) */
    /* 0x083 */ u8 field_0x83[0x150 - 0x83];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The save data (struct name unknown; see Game/AdvStoryLibrary.cpp). fn_800BB74C returns the
 * current data, followed by the copy last written to the card. */
struct UnkSaveData_800BB74C {
    /* 0x0000 */ u8 field_0x0[0x16C8];
    /* 0x16C8 */ u8 m_hardMode; /* (meaning inferred) */
    /* 0x16C9 */ u8 field_0x16C9;
    /* 0x16CA */ u8 field_0x16CA[0x1710 - 0x16CA];
    /* 0x1710 */ u8 m_keys[0x4]; /* the unlock keys fn_800BBDD8 reads (meaning inferred) */
    /* 0x1714 */ u8 field_0x1714[0x1F5C - 0x1714];
};
SIZE_ASSERT(UnkSaveData_800BB74C, 0x1F5C);

/* The save message window (class name unknown; see Game/SaveCycleMessage.cpp). */
class UnkObject_802E5B2C;

extern "C" {
/* The save message window, showing message id until it is answered (returns the choice, 1 to 3,
 * once answered) and showing it without waiting (see Game/SaveCycleMessage.cpp). */
UnkObject_802E5B2C *fn_80117514(void);
int fn_802E58B4(UnkObject_802E5B2C *object, int id, int pad, int quick);
void fn_802E585C(UnkObject_802E5B2C *object, int id, int pad, int quick);
/* The memory card controller (Game/MemoryCardCtrl.cpp): starting a save, a create, a format and
 * a delete, whether a request is running, the card's status, the last request's result, whether
 * the card has room for the file and whether a save checks the file's stamp. */
bool fn_80299AEC(TMemoryCardCtrl *ctrl);
bool fn_80299A8C(TMemoryCardCtrl *ctrl);
bool fn_802999CC(TMemoryCardCtrl *ctrl);
bool fn_8029996C(TMemoryCardCtrl *ctrl);
int fn_80298EBC(TMemoryCardCtrl *ctrl);
int fn_80298E5C(TMemoryCardCtrl *ctrl);
int fn_80298EB4(TMemoryCardCtrl *ctrl);
bool fn_80298E88(TMemoryCardCtrl *ctrl);
void fn_80298E80(TMemoryCardCtrl *ctrl, u8 checkStamp);
/* The save data; whether key index is set and setting it (behaviour inferred). */
UnkSaveData_800BB74C *fn_800BB74C(void);
u8 fn_800BBDD8(u8 *keys, int index);
void fn_801E5828(u8 *keys, int index);
}

namespace SaveCycle {
namespace Save {

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

    bool ShowUnlocks();
    void WaitCycle();
    void Ask(int message, int next1, int next2, int next3);

    /* 0x00 */ s32 m_state;
    /* 0x04 */ s32 m_nextOnResult; /* WaitCycle's next state when the cycle gives m_result */
    /* 0x08 */ s32 m_nextOtherwise;
    /* 0x0C */ UnkMemoryCard_802E472C *m_cycle; /* a CheckModule cycle */
    /* 0x10 */ u32 m_result;
};
SIZE_ASSERT(Impl, 0x14);

/* A pair of unlock keys and the messages that announce one or both of them (name inferred). */
struct UnkUnlockMessage {
    /* 0x0 */ s32 m_keys[2];
    /* 0x8 */ s32 m_messages[2];
};

Impl::Impl() : m_state(0), m_cycle(0) {}

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

bool Impl::Update() {
    TMemoryCardCtrl *ctrl = fn_801E5B80()->m_ctrl;
    if (ctrl == 0) {
        return false;
    }
    if (fn_80298E5C(ctrl) == 3) {
        switch (m_state) {
        case 0:
        case 22:
        case 23:
        case 24:
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
        if (!ShowUnlocks()) {
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
                m_state = 14;
                break;
            case 1:
                Ask(7, 22, 1, 24);
                break;
            case 2:
                Ask(3, 10, 22, 24);
                break;
            case 3:
                Ask(0x13, 22, 1, 5);
                break;
            case 4:
                Ask(0x15, 22, 1, 24);
                break;
            case 5:
                Ask(0xB, 22, 1, 24);
                break;
            case 6:
                Ask(0xE, 22, 1, 24);
                break;
            case 7:
                m_state = 16;
                break;
            case 8:
                Ask(0x27, 22, 1, 19);
                break;
            case 9:
                Ask(0xF, 22, 1, 5);
                break;
            case 10:
            default:
                Ask(0xE, 22, 1, 24);
                break;
            }
        }
        break;
    case 3:
        WaitCycle();
        break;
    case 4:
        Ask(7, 22, 1, 24);
        break;
    case 5:
        Ask(0x2A, 6, 1, 24);
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
                if (fn_80299A8C(ctrl)) {
                    m_state = 11;
                }
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
        Ask(0xE, 22, 1, 24);
        break;
    case 10:
        if (fn_80298E88(ctrl)) {
            if (fn_80299A8C(ctrl)) {
                m_state = 11;
                lbl_8057D6A0.field_0x82 = 0;
            }
        } else {
            Ask(0x15, 24, 1, 24);
        }
        break;
    case 11:
        lbl_8057D6A0.field_0x82 = 0;
        fn_802E585C(fn_80117514(), 0, 0, 0);
        if (fn_80298EBC(ctrl) == 0) {
            switch (fn_80298EB4(ctrl)) {
            case 0:
                m_state = 23;
                break;
            case 5:
                m_state = 9;
                break;
            default:
                m_state = 12;
                break;
            }
        }
        break;
    case 12:
        m_state = 1;
        break;
    case 13:
        fn_80298E80(ctrl, 0);
        m_state = 14;
    case 14:
        if (fn_80299AEC(ctrl)) {
            m_state = 15;
            lbl_8057D6A0.field_0x82 = 0;
        }
        break;
    case 15:
        lbl_8057D6A0.field_0x82 = 0;
        fn_802E585C(fn_80117514(), 0x20, 0, 0);
        if (fn_80298EBC(ctrl) == 0) {
            switch (fn_80298EB4(ctrl)) {
            case 0:
                m_state = 23;
                break;
            case 11:
                m_state = 16;
                break;
            case 8:
                m_state = 18;
                break;
            case 5:
                m_state = 9;
                break;
            default:
                m_state = 17;
                break;
            }
            fn_80298E80(ctrl, 1);
        }
        break;
    case 16:
        Ask(0x31, 22, 1, 13);
        break;
    case 17:
        m_state = 1;
        break;
    case 18:
        Ask(0x27, 22, 1, 19);
        break;
    case 19:
        if (fn_8029996C(ctrl)) {
            lbl_8057D6A0.field_0x82 = 0;
            m_state = 20;
        }
        break;
    case 20:
        lbl_8057D6A0.field_0x82 = 0;
        fn_802E585C(fn_80117514(), 0x33, 0, 0);
        if (fn_80298EBC(ctrl) == 0) {
            switch (fn_80298EB4(ctrl)) {
            case 0:
            case 9:
                m_state = 10;
                break;
            default:
                m_state = 21;
                break;
            }
        }
        break;
    case 21:
        Ask(0x34, 22, 1, 24);
        break;
    case 22:
        Ask(0x35, 24, 1, 24);
        break;
    case 23:
        lbl_8057D6A0.m_cardUsable = 1;
        UnkMemoryCard_802E472C::SetCardUsable(1);
        return false;
    case 24:
        lbl_8057D6A0.m_cardUsable = 0;
        UnkMemoryCard_802E472C::SetCardUsable(0);
        return false;
    }
    return true;
}

/* Announces what was unlocked since the file was last saved, one message per call, and records it
 * in the saved copy once confirmed. Returns false when there is nothing left to announce (name
 * inferred). */
bool Impl::ShowUnlocks() {
    UnkSaveData_800BB74C *current = fn_800BB74C();
    UnkSaveData_800BB74C *data = fn_800BB74C();
    UnkUnlockMessage unlocks[5] = {
        {{0, 1}, {0x3A, 0x3B}},
        {{2, 3}, {0x3C, 0x3D}},
        {{4, 5}, {0x3E, 0x3F}},
        {{6, 7}, {0x40, 0x41}},
        {{8, 9}, {0x42, 0x43}},
    };
    for (u32 i = 0; i < 5; i++) {
        if (fn_800BBDD8(current->m_keys, unlocks[i].m_keys[0]) != fn_800BBDD8(data[1].m_keys, unlocks[i].m_keys[0]) ||
            fn_800BBDD8(current->m_keys, unlocks[i].m_keys[1]) != fn_800BBDD8(data[1].m_keys, unlocks[i].m_keys[1])) {
            /* One message for either key, another for both. */
            s32 index = (bool)fn_800BBDD8(current->m_keys, unlocks[i].m_keys[1]);
            s32 first = (bool)fn_800BBDD8(current->m_keys, unlocks[i].m_keys[0]);
            index = first + index - 1;
            if (fn_802E58B4(fn_80117514(), unlocks[i].m_messages[index], 0, 0) == 2) {
                if (fn_800BBDD8(current->m_keys, unlocks[i].m_keys[0])) {
                    fn_801E5828(data[1].m_keys, unlocks[i].m_keys[0]);
                }
                if (fn_800BBDD8(current->m_keys, unlocks[i].m_keys[1])) {
                    fn_801E5828(data[1].m_keys, unlocks[i].m_keys[1]);
                }
            }
            return true;
        }
    }
    if (fn_800BBDD8(current->m_keys, 10) != fn_800BBDD8(data[1].m_keys, 10)) {
        if (fn_802E58B4(fn_80117514(), 0x44, 0, 0) == 2) {
            fn_801E5828(data[1].m_keys, 10);
        }
        return true;
    }
    if (current->m_hardMode != data[1].m_hardMode) {
        if (fn_802E58B4(fn_80117514(), 0x18, 0, 0) == 2) {
            data[1].m_hardMode = 1;
        }
        return true;
    }
    if (current->field_0x16C9 != data[1].field_0x16C9) {
        if (fn_802E58B4(fn_80117514(), 0x1A, 0, 0) == 2) {
            data[1].field_0x16C9 = 1;
        }
        return true;
    }
    return false;
}

Phase::Phase() : CycleBase(PHASE_SAVE), m_impl(new Impl) {}

Phase::~Phase() {}

bool Phase::Update() {
    return m_impl->Update();
}

void Phase::Draw() {
    m_impl->Draw();
}

} // namespace Save
} // namespace SaveCycle
