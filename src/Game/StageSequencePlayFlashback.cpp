/* StageSequence::PlayFlashback::Phase (class name from RTTI): the stage sequence phase that plays
 * the flashbacks of a story route. Its phaseImpl walks the route the save data records (the stage
 * table's mission links between the stages), asks the event manager for the events between each
 * pair of stages and queues them (UnkFlashbackQueue_803546C0, kept in the stage info at
 * 0x80584720). It then plays the front of the queue: an event movie, which it pops, or a stage,
 * whose StageAction::FlashbackAction pops it. The file name and the names marked "(name inferred)"
 * are not original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/FlashbackQueue.h"
#include "Game/StageManager.h"
#include "Game/StageSequence.h"
#include "Game/StageTable.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include <cstdio>
#include <dolphin/types.h>
#include "size_assert.h"

/* The game state (struct name unknown; see Game/AdvMemoryCheck.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x126];
    /* 0x126 */ u8 m_flashback; /* set while the phase plays (meaning inferred) */
    /* 0x127 */ u8 field_0x127[0x148 - 0x127];
    /* 0x148 */ u8 field_0x148[3]; /* meaning unknown */
    /* 0x14B */ u8 field_0x14B[0x150 - 0x14B];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The stages to play (struct name unknown; see Game/StageSequencePlayOnce.cpp). */
struct UnkStageInfo_80584720 {
    /* 0x000 */ u8 field_0x0[0x24];
    /* 0x024 */ UnkFlashbackQueue_803546C0 m_flashbacks;
    /* 0x228 */ s32 m_route; /* the route to play (meaning inferred) */
    /* 0x22C */ u8 field_0x22C[0x230 - 0x22C];
};
SIZE_ASSERT(UnkStageInfo_80584720, 0x230);

extern UnkStageInfo_80584720 lbl_80584720;

/* A record fn_801E515C fills (struct name unknown; see Game/StageSequencePlayStory.cpp): the
 * stages' choices along the route, and the ending. */
struct UnkRouteInfo_801E515C {
    /* 0x0 */ s8 m_choices[6];
    /* 0x6 */ s8 m_ending; /* (name inferred) */
    /* 0x7 */ u8 field_0x7;
};

/* The save data fn_800BB74C returns (class unknown; see Game/StageSequencePlayStory.cpp). */
struct UnkSaveData_800BB74C {
    /* 0x00 */ u8 field_0x0[0x98];
    /* 0x98 */ s32 field_0x98; /* meaning unknown (the route's field_0x68) */
};

/* An event the event manager hands out (struct name unknown; see
 * Game/StageSequencePlayStory.cpp). */
struct UnkEvent_8025D41C {
    /* 0x0 */ s32 m_movieId;
    /* 0x4 */ s32 m_stageId; /* a stage when positive */
    /* 0x8 */ s32 field_0x8;
};

extern "C" {
/* Sets count bytes at dest to value (behaviour inferred). */
void fn_800CBBFC(void *dest, int value, int count);
UnkSaveData_800BB74C *fn_800BB74C(void);
void fn_801E515C(UnkSaveData_800BB74C *save, UnkRouteInfo_801E515C *info, int route);
/* The stage after step of the route for a choice, or -2 (behaviour inferred). */
int fn_801E5140(int step, int choice);
/* The event manager, and its calls (behaviour inferred; see Game/StageSequencePlayStory.cpp). */
void *fn_801D3140(void);
void fn_8025D6FC(void *manager);
bool fn_8025D41C(void *manager, UnkEvent_8025D41C *event, int arg2, int arg3);
void fn_8025D504(void *manager, int from, int to);
/* Whether a button of a pad was pressed (behaviour inferred). */
bool fn_80356260(int arg0, int arg1);
/* The sound manager and its play call. */
void *fn_80053164(void);
void fn_801D560C(void *manager, int soundId, int arg2, int arg3);
int fn_80044E10(void);
}

/* The stage table entry at index (name inferred; the lookup ignores the object it is called on). */
inline const UnkStageInfo_804C4A18 *StageEntry(int index) {
    fn_800B74B4();
    return &lbl_804C4A18[index];
}

namespace StageSequence {
namespace PlayFlashback {

class phaseImpl {
  public:
    phaseImpl();
    ~phaseImpl();

    /* Runs the phase; false once it ended. */
    bool Tick();
    /* Ends the stage early (name inferred). */
    void Skip();
    /* Queues the flashbacks of the route (name inferred). */
    bool Build();

    /* 0x00 */ boost::shared_ptr<StageBase::Base> m_stage;
    /* 0x08 */ u8 field_0x8[0x8C - 0x8];
    /* 0x8C */ u8 m_skipped; /* the player skipped the stage (meaning inferred) */
};

/* Deletes the phaseImpl (name inferred; see checked_delete below). */
inline void DeleteImpl(phaseImpl *impl) {
    delete impl;
}

} // namespace PlayFlashback
} // namespace StageSequence

/* Retail keeps boost::checked_delete<phaseImpl> out of line and the destructor at its own place;
 * this specialization gives that order under GC/2.7 (see docs/research-notes.md and
 * Game/EnergyCore.cpp). */
namespace boost {
template <>
inline void checked_delete<StageSequence::PlayFlashback::phaseImpl>(StageSequence::PlayFlashback::phaseImpl *x) {
    StageSequence::PlayFlashback::DeleteImpl(x);
}
} // namespace boost

SIZE_ASSERT(StageSequence::PlayFlashback::phaseImpl, 0x90);
SIZE_ASSERT(StageSequence::PlayFlashback::Phase, 0x8);

/* Pop's function-local static copy of the front entry (static UnkFlashbackEntry_803546C0 s_front =
 * m_entries[0]) and its guard, written out (names not original): GC/2.7 emits a function-local
 * static where it parses it, but retail has this one after the unit's RTTI records and
 * temporaries, where Pop is compiled, and aligns the guard to a word. Defined at the end of the
 * unit, the copy's two words are separate statics so that GC/2.7 addresses each through r13, as
 * retail's copy construction does. */
/* The guard is a byte, word-aligned as retail places it. */
union PopGuard {
    s8 m_done;
    s32 m_align;
};

struct PopCopy {
    static PopGuard s_initialized;
    static s32 s_event;
    static s32 s_stageId;
};

void UnkFlashbackQueue_803546C0::Clear() {
    m_count = 0;
}

void UnkFlashbackQueue_803546C0::Push(UnkFlashbackEntry_803546C0 *entry) {
    m_entries[m_count] = *entry;
    m_count++;
}

/* NonMatching (2 instructions): retail stores the copy's words in the order it loads them; GC/2.7
 * keeps one of the orders but not both (every order of these statements was tried). */
UnkFlashbackEntry_803546C0 *UnkFlashbackQueue_803546C0::Pop() {
    if (m_count != 0) {
        if (!PopCopy::s_initialized.m_done) {
            s32 event = m_entries[0].m_event;
            s32 stageId = m_entries[0].m_stageId;
            PopCopy::s_stageId = stageId;
            PopCopy::s_event = event;
            PopCopy::s_initialized.m_done = 1;
        }
        for (int i = 0; i < m_count - 1; i++) {
            m_entries[i] = m_entries[i + 1];
        }
        m_count = m_count - 1;
        return reinterpret_cast<UnkFlashbackEntry_803546C0 *>(&PopCopy::s_event);
    }
    return 0;
}

UnkFlashbackEntry_803546C0 *UnkFlashbackQueue_803546C0::Front() {
    if (m_count != 0) {
        return &m_entries[0];
    }
    return 0;
}

namespace StageSequence {
namespace PlayFlashback {

phaseImpl::phaseImpl() {
    m_skipped = 0;
    lbl_8057D6A0.m_flashback = 1;
    Build();
}

phaseImpl::~phaseImpl() {
    lbl_8057D6A0.m_flashback = 0;
}

bool phaseImpl::Tick() {
    if (m_stage && m_stage->Tick() == 1) {
        if (fn_80044E10() == 0 && !m_skipped && fn_80356260(2, 4)) {
            fn_801D560C(fn_80053164(), 0xE004, 0, 0);
            m_stage->vf_0x10();
            m_skipped = 1;
        }
        return true;
    }
    if (!m_skipped) {
        m_stage.reset();
        UnkFlashbackQueue_803546C0 *queue = &lbl_80584720.m_flashbacks;
        UnkFlashbackEntry_803546C0 *entry = queue->Front();
        if (entry != 0) {
            if (entry->m_stageId == 0) {
                char name[0x20];
                sprintf(name, "E%04d.sfd", entry->m_event);
                m_stage = boost::shared_ptr<StageBase::Base>(new StageMovie(name));
                queue->Pop();
            } else {
                fn_800CBBFC(lbl_8057D6A0.field_0x148, 0, 3);
                m_stage = boost::shared_ptr<StageBase::Base>(new StageManager(entry->m_stageId, 0));
            }
            return true;
        }
    }
    return false;
}

void phaseImpl::Skip() {
    m_stage->vf_0x10();
}

bool phaseImpl::Build() {
    UnkEvent_8025D41C event;
    UnkRouteInfo_801E515C route;
    UnkFlashbackQueue_803546C0 *queue;
    int mode = 0;
    int step = 1;
    int routeId = lbl_80584720.m_route;
    fn_801E515C(fn_800BB74C(), &route, routeId);
    queue = &lbl_80584720.m_flashbacks;
    queue->Clear();
    int prev = -1;
    int stage = 5;
    int next;
    s8 *choice = &route.m_choices[1];
    while (prev != -2) {
        fn_8025D6FC(fn_801D3140());
        s32 saved = fn_800BB74C()->field_0x98;
        fn_800BB74C()->field_0x98 = mode;
        fn_8025D504(fn_801D3140(), prev, stage);
        fn_800BB74C()->field_0x98 = saved;
        while (fn_8025D41C(fn_801D3140(), &event, 2, 1)) {
            UnkFlashbackEntry_803546C0 entry;
            entry.m_event = event.m_movieId;
            entry.m_stageId = event.m_stageId;
            queue->Push(&entry);
        }
        prev = stage;
        if (step >= 7) {
            break;
        }
        next = fn_801E5140(step, *choice);
        if (next == -2) {
            break;
        }
        const UnkStageInfo_804C4A18 *info = StageEntry(stage);
        bool found = false;
        if (mode != 0) {
            switch (mode) {
            case 3:
                stage = info->m_missions[0].field_0x0;
                break;
            case 4:
                stage = info->m_missions[1].field_0x0;
                break;
            case 5:
                stage = info->m_missions[2].field_0x0;
                break;
            }
            mode = 0;
            step++;
            choice++;
            continue;
        }
        if (next == 0x27) {
            if (route.m_ending == 4) {
                mode = 3;
            } else if (route.m_ending == 5) {
                mode = 5;
            }
            stage = next;
            found = true;
            step++;
            choice++;
        } else {
            for (int i = 0; i < 3; i++) {
                if (info->m_missions[i].m_id != -1 && info->m_missions[i].field_0x0 == next) {
                    stage = next;
                    found = true;
                    step++;
                    choice++;
                }
            }
        }
        if (!found) {
            stage = info->m_missions[0].field_0x0;
            const UnkStageInfo_804C4A18 *entry = StageEntry(stage);
            for (int i = 0; i < 3; i++) {
                if (entry->m_missions[i].field_0x0 == next) {
                    switch (i) {
                    case 0:
                        mode = 3;
                        break;
                    case 1:
                        mode = 4;
                        break;
                    case 2:
                        mode = 5;
                        break;
                    }
                }
            }
        }
    }
    return true;
}

Phase::Phase() : m_impl(new phaseImpl) {}

Phase::~Phase() {}

bool Phase::Tick() {
    return m_impl->Tick();
}

void Phase::Skip() {
    m_impl->Skip();
}

} // namespace PlayFlashback
} // namespace StageSequence
PopGuard PopCopy::s_initialized;
#pragma explicit_zero_data on
s32 PopCopy::s_event = 0;
s32 PopCopy::s_stageId = 0;
#pragma explicit_zero_data reset
