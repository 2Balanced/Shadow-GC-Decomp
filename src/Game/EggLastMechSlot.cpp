/* UnkSlot_803142C0 (class name unknown; declared in Game/EggLastMech.h): EggLastMech's slot
 * machine. Its three reels (SlotSwitch, Game/EggLastMechSlotSwitch.cpp) spin until the boss's motion
 * events stop them one at a time; once all three stopped, three equal symbols give a result
 * (s_results, by shot type and symbol) that starts the boss's fever states, anything else result 7. A 2D display
 * (EggLastMechSlotDisp2D) shows it. File name inferred; names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). The functions EggLastMech.cpp calls keep their fn_ names. */

#include "Game/EggLastMech.h"
#include "Game/EggLastMechSlotSwitch.h"
#include "Game/EnemyCharInfo.h"
#include "Game/GadgetSound.h"
#include "Game/Random.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

namespace EggLastMech {

inline void UnkSlotLine_803141C4::Reset() {
    m_matched = 0;
    for (int i = 0; i < 3; i++) {
        m_symbols[i] = -1;
    }
    m_shotType = 1;
}

inline bool UnkSlotLine_803141C4::IsMatch() {
    if (m_matched) {
        return true;
    }
    s32 first = m_symbols[0];
    s32 second = m_symbols[1];
    if (first == second && second == m_symbols[2] && first != -1) {
        m_matched = 1;
        return true;
    }
    return false;
}

extern "C" {
/* The display (behaviour inferred). */
boost::shared_ptr<EggLastMechSlotDisp2D> fn_8032D744(void);
void fn_8032D9D8(EggLastMechSlotDisp2D *disp);
void fn_8032D9E8(EggLastMechSlotDisp2D *disp);
void fn_8032DA18(EggLastMechSlotDisp2D *disp, int reel, float angle);
void fn_8032DA3C(EggLastMechSlotDisp2D *disp, int symbol, int shotType);

/* The sound player (behaviour inferred). */
UnkSoundHandle_801D3DC8 fn_80336AE4(UnkSound_80336F48 *sound, int id);
void fn_80336B1C(UnkSound_80336F48 *sound, int id);

/* The functions EggLastMech.cpp calls. */
void fn_80313C10(UnkSlot_803142C0 *slot);
void fn_80313C34(UnkSlot_803142C0 *slot);
void fn_80313C60(UnkSlot_803142C0 *slot, int reel, float angle);
void fn_80313C8C(UnkSlot_803142C0 *slot);
void fn_80313C98(UnkSlot_803142C0 *slot, int reel);
void fn_80313CC0(UnkSlot_803142C0 *slot);
int fn_80313F48(UnkSlot_803142C0 *slot);
void fn_80313F80(UnkSlot_803142C0 *slot);
void fn_80314004(UnkSlot_803142C0 *slot, int reel, int shotType, int symbol);
}

/* The symbols each shot type's line may show at first (values from .sdata2). */
static const u32 s_lineMasks[2] = {0x1E, 0x15};

UnkSlot_803142C0::UnkSlot_803142C0(EggLastMech *mech) : m_mech(mech) {
    m_result = -2;
    m_state = 0;
    m_timer = 0.0f;
    m_spinMask = 7;
    m_symbolMask = 0x1F;
    for (int i = 0; i < 2; i++) {
        m_lineMasks[i] = s_lineMasks[i];
        m_nextSymbols[i] = 2;
    }
    for (int i = 0; i < 3; i++) {
        m_reels[i] = new SlotSwitch(mech, i);
    }
    m_disp = fn_8032D744();
}

UnkSlot_803142C0::~UnkSlot_803142C0() {
    for (int i = 2; i >= 0; i--) {
        delete m_reels[i];
    }
    m_sound.fn_801D455C();
}

UnkSlotLine_803141C4::UnkSlotLine_803141C4() {
    Reset();
}

bool UnkSlotLine_803141C4::IsAllDifferent() {
    if (m_symbols[0] != m_symbols[1] && m_symbols[1] != m_symbols[2] && m_symbols[2] != m_symbols[0]) {
        return true;
    }
    return false;
}

/* Stops reel on the next symbol of shotType, or on symbol (name inferred). */
extern "C" void fn_80314004(UnkSlot_803142C0 *slot, int reel, int shotType, int symbol) {
    slot->m_mech->field_0x105 = 1;
    if (symbol != -1) {
        slot->m_nextSymbols[shotType] = symbol;
    }
    if (slot->m_spinMask == 7) {
        if (slot->m_nextSymbols[shotType] == 4 || slot->m_nextSymbols[shotType] == 2) {
            slot->m_nextSymbols[shotType == 0 ? 1 : 0] = slot->m_nextSymbols[shotType];
        }
    }
    slot->m_spinMask &= ~(1 << reel);
    slot->m_line.m_symbols[reel] = slot->m_nextSymbols[shotType];
    slot->m_line.m_shotType = shotType;
    slot->m_reels[reel]->Stop(slot->m_nextSymbols[shotType]);
    if (slot->m_spinMask == 0 && slot->m_line.IsMatch()) {
        slot->m_symbolMask &= ~(1 << slot->m_nextSymbols[shotType]);
        for (int i = 0; i < 2; i++) {
            slot->m_lineMasks[i] &= ~(1 << slot->m_nextSymbols[shotType]);
            if (slot->m_lineMasks[i] == 0) {
                slot->m_lineMasks[i] = ~(1 << slot->m_nextSymbols[shotType]) & s_lineMasks[i];
            }
        }
    }
}

/* Starts the reels (name inferred). */
extern "C" void fn_80313F80(UnkSlot_803142C0 *slot) {
    slot->m_spinMask = 7;
    for (int i = 0; i < 3; i++) {
        slot->m_reels[i]->Start();
    }
    slot->m_line.Reset();
}

/* The first reel still spinning (name inferred). */
extern "C" int fn_80313F48(UnkSlot_803142C0 *slot) {
    for (int i = 0; i < 3; i++) {
        if (slot->m_spinMask & (1 << i)) {
            return i;
        }
    }
    return 0;
}

/* The result of each matched symbol, by shot type (values from .rodata). */
static const s32 s_results0[5] = {7, 5, 1, 6, 3};
static const s32 s_results1[5] = {4, 7, 0, 7, 2};
static const s32 *s_results[2] = {s_results0, s_results1};

inline s32 GetResult(int shotType, int symbol) {
    return s_results[shotType][symbol];
}

/* A random symbol of mask (name inferred). */
inline int PickSymbol(u32 mask) {
    s32 symbols[5];
    int count = 0;
    for (int i = 0; i < 5; i++) {
        if (mask & (1 << i)) {
            symbols[count++] = i;
        }
    }
    return symbols[fn_8040EA90(count)];
}

/* Runs the slot machine (name inferred). */
extern "C" void fn_80313CC0(UnkSlot_803142C0 *slot) {
    switch (slot->m_state) {
    case 0:
        slot->m_result = -2;
        slot->m_state = 1;
        break;
    case 1: {
        slot->m_sound.fn_801D455C();
        fn_80313F80(slot);
        for (int i = 0; i < 2; i++) {
            u32 mask = slot->m_symbolMask & slot->m_lineMasks[i];
            if (mask == 0) {
                mask = slot->m_lineMasks[i];
            }
            slot->m_nextSymbols[i] = PickSymbol(mask);
        }
        fn_80313C34(slot);
        slot->m_result = -1;
        slot->m_state = 2;
    }
    case 2:
        if (slot->m_sound.m_id == 0) {
            slot->m_sound = fn_80336AE4(slot->m_mech->m_sound, 0x7F31);
        }
        if (slot->m_spinMask == 0) {
            slot->m_sound.fn_801D455C();
            slot->m_timer = 1.0f;
            slot->m_state = 3;
        }
        break;
    case 3:
        if (!slot->m_mech->IsReady()) {
            if ((slot->m_timer -= fn_8019FFE0(slot->m_mech)) < 0.0f) {
                if (slot->m_line.IsMatch()) {
                    const s32 *results = s_results[slot->m_line.m_shotType];
                    slot->m_result = results[slot->m_line.m_symbols[0]];
                    fn_8032DA3C(slot->m_disp.get(), slot->m_line.m_symbols[0], slot->m_line.m_shotType);
                    fn_80336B1C(slot->m_mech->m_sound, 0x7F34);
                } else {
                    slot->m_result = 7;
                    fn_80336B1C(slot->m_mech->m_sound, 0x7F35);
                }
                slot->m_mech->OnSlotStop();
                slot->m_state = 4;
            }
        }
        break;
    case 4:
        break;
    }
    for (int i = 0; i < 3; i++) {
        slot->m_reels[i]->Tick();
        fn_80313C60(slot, i, slot->m_reels[i]->m_angle);
    }
}

/* Stops reel on the next symbol of shot type 0 (name inferred). */
extern "C" void fn_80313C98(UnkSlot_803142C0 *slot, int reel) {
    fn_80314004(slot, reel, 0, -1);
}

/* Starts a spin (name inferred). */
extern "C" void fn_80313C8C(UnkSlot_803142C0 *slot) {
    slot->m_state = 1;
}

/* Shows reel's angle on the display (name inferred). */
extern "C" void fn_80313C60(UnkSlot_803142C0 *slot, int reel, float angle) {
    if (slot->m_disp.get() != 0) {
        fn_8032DA18(slot->m_disp.get(), reel, angle);
    }
}

/* Shows the display (name inferred). */
extern "C" void fn_80313C34(UnkSlot_803142C0 *slot) {
    if (slot->m_disp.get() != 0) {
        fn_8032D9E8(slot->m_disp.get());
    }
}

/* Hides the display (name inferred). */
extern "C" void fn_80313C10(UnkSlot_803142C0 *slot) {
    fn_8032D9D8(slot->m_disp.get());
}

} // namespace EggLastMech
