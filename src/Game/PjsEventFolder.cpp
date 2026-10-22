/* TPjsEventFolder (class name from RTTI): the task the event tasks hang below, and the event
 * holder fn_801723E0 returns, which owns the folder and starts events below it. File name inferred
 * from the class. Names marked "(name inferred)" are not original; the functions other units call
 * keep their fn_ names. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include "Game/PjsEventAction.h"
#include "Game/Task.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* The save data fn_800BB74C returns (class unknown; see Game/SecretKey.cpp). */
struct UnkSaveData_800BB74C {
    /* 0x0000 */ u8 field_0x0[0x1754];
    /* 0x1754 */ u32 m_eventsSeen[1]; /* one bit per event (name inferred) */
};

extern "C" {
UnkSaveData_800BB74C *fn_800BB74C(void);
/* The task layers (see Game/EffectPJS.cpp). */
Task *fn_8001C4F8(void);
Task *fn_801EB304(Task *root, int layer);
/* The index of event among the events the save data records, or -1 (behaviour inferred). */
int fn_8025D280(int event);

/* The functions this unit defines that other units call. */
bool fn_80174424(u32 *bits, u32 index);
void fn_80174448(u32 *bits, u32 index);
void fn_801743BC(UnkEventHolder_801723E0 *holder);
void fn_801743B8(UnkEventHolder_801723E0 *holder);
void fn_8017430C(UnkEventHolder_801723E0 *holder, int event);
void fn_801742E0(UnkEventHolder_801723E0 *holder);
}

const char *TPjsEventFolder::s_taskName = "TPjsEventFolder";

TPjsEventFolder::TPjsEventFolder(Task *parent) : Task(parent) {
    m_name = s_taskName;
}

TPjsEventFolder::~TPjsEventFolder() {}

/* Sets bit index of bits. */
extern "C" void fn_80174448(u32 *bits, u32 index) {
    bits[index >> 5] |= 1 << (index & 31);
}

/* Whether bit index of bits is set. */
extern "C" bool fn_80174424(u32 *bits, u32 index) {
    return (bits[index >> 5] & (1 << (index & 31))) != 0;
}

/* Makes the folder below task layer 3 (name inferred). */
extern "C" void fn_801743BC(UnkEventHolder_801723E0 *holder) {
    holder->m_folder = new TPjsEventFolder(fn_801EB304(fn_8001C4F8(), 3));
    holder->m_task = 0;
    holder->m_firstTime = 1;
}

extern "C" void fn_801743B8(UnkEventHolder_801723E0 *holder) {}

/* Starts event below the folder and records it as seen (name inferred). */
extern "C" void fn_8017430C(UnkEventHolder_801723E0 *holder, int event) {
    if (holder->m_folder != 0) {
        holder->m_task = new TPjsEventActionManager(holder->m_folder, event);
    }
    int index = fn_8025D280(event);
    if (index != -1) {
        UnkSaveData_800BB74C *save = fn_800BB74C();
        if (fn_80174424(save->m_eventsSeen, index)) {
            holder->m_firstTime = 0;
        } else {
            holder->m_firstTime = 1;
        }
        fn_80174448(save->m_eventsSeen, index);
    }
}

/* Lets the player skip the event playing (name inferred). */
extern "C" void fn_801742E0(UnkEventHolder_801723E0 *holder) {
    if (holder->m_task != 0) {
        static_cast<TPjsEventActionManager *>(holder->m_task)->RequestSkip();
    }
}
