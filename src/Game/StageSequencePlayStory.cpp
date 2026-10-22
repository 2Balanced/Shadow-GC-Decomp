/* StageSequence::PlayStory::Phase (class name from RTTI): the stage sequence phase that plays the
 * story. Its phaseImpl is a state machine: it shows the route to the next stage
 * (StageRouteDisplay), plays the stage (StageManager), works out the next stage from the stage
 * table and the way the stage ended, plays the events and movies queued between stages, and at
 * the end of the route plays the staff roll (StageStaffroll) and saves. The file name and the
 * names marked "(name inferred)" are not original; the meanings of most game-side calls are
 * unknown. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/Render.h"
#include "Game/SaveCycle.h"
#include "Game/StageManager.h"
#include "Game/StageSequence.h"
#include "Game/StageTable.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <cstdio>
#include <dolphin/types.h>
#include "size_assert.h"

/* The game state (struct name unknown; see Game/AdvMemoryCheck.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xAC];
    /* 0x0AC */ s32 m_stageIndex; /* the stage table index of the stage played (name inferred) */
    /* 0x0B0 */ u8 field_0xB0[0xD4 - 0xB0];
    /* 0x0D4 */ s32 field_0xD4[20]; /* meaning unknown */
    /* 0x124 */ u8 field_0x124[0x148 - 0x124];
    /* 0x148 */ u8 field_0x148[3]; /* meaning unknown */
    /* 0x14B */ u8 field_0x14B[0x150 - 0x14B];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The stages to play (struct name unknown). */
struct UnkStageInfo_80584720 {
    /* 0x000 */ u8 field_0x0[0x4];
    /* 0x004 */ s32 m_storyMode; /* 0 new story, 1 continued, 2 last story (meanings inferred) */
    /* 0x008 */ u8 field_0x8[0x230 - 0x8];
};
SIZE_ASSERT(UnkStageInfo_80584720, 0x230);

extern UnkStageInfo_80584720 lbl_80584720;

/* A set of flags in the save data (struct name unknown; a template, as the out-of-line Count's
 * place among the unit's template instances shows). */
template <class T> struct UnkFlags_801E5828 {
    /* The number of flags set (name inferred). */
    u32 Count() const;

    /* 0x0 */ T m_bits;
};

template <class T> u32 UnkFlags_801E5828<T>::Count() const {
    u32 bit = 1;
    u32 count = 0;
    for (int i = 0; i < 16; i++) {
        if (m_bits & bit) {
            count++;
        }
        bit <<= 1;
    }
    return count;
}

/* A record fn_801E515C fills (struct name unknown). */
struct UnkRouteInfo_801E515C {
    /* 0x0 */ u8 field_0x0[0x6];
    /* 0x6 */ s8 m_ending; /* (name inferred) */
    /* 0x7 */ u8 field_0x7;
};

/* The route through the story the save data records (struct name unknown). */
struct UnkStoryRoute_800BB74C {
    /* 0x00 */ s32 m_length; /* the stages played before the current one (name inferred) */
    /* 0x04 */ s32 m_stages[0x18]; /* their stage table indexes (name inferred) */
    /* 0x64 */ s32 field_0x64; /* meaning unknown */
    /* 0x68 */ s32 field_0x68; /* meaning unknown */
    /* 0x6C */ u8 field_0x6C[0x74 - 0x6C];
    /* 0x74 */ u8 m_flags[4]; /* read by fn_800BBDD8 (size unknown) */
};

/* The save data fn_800BB74C returns (class unknown; see Game/SecretKey.cpp). */
struct UnkSaveData_800BB74C {
    /* 0x0000 */ u8 field_0x0[0x30];
    /* 0x0030 */ UnkStoryRoute_800BB74C m_route;
    /* 0x00A8 */ u8 field_0xA8[0x16C8 - 0xA8];
    /* 0x16C8 */ u8 field_0x16C8; /* meaning unknown */
    /* 0x16C9 */ u8 field_0x16C9[0x16D0 - 0x16C9];
    /* 0x16D0 */ u8 field_0x16D0[0x1710 - 0x16D0]; /* passed to fn_801E5798 */
    /* 0x1710 */ UnkFlags_801E5828<u16> m_endings; /* the endings seen (meaning inferred) */
};

/* An event the event manager hands out (struct name unknown): a stage to play, or a movie. */
struct UnkEvent_8025D41C {
    /* 0x0 */ s32 m_movieId;
    /* 0x4 */ s32 m_stageId; /* a stage when positive */
    /* 0x8 */ s32 field_0x8;
};

/* A task the save waits for (class unknown); the halfword at 0x4 holds its flags. */
struct UnkTask_802E3DBC {
    /* 0x0 */ u8 field_0x0[0x4];
    /* 0x4 */ u16 m_flags;
};

/* The message source (class name unknown; see Game/StageManager.cpp). */
class UnkHintTarget_801FB180 {
  public:
    /* 0x00 */ u8 field_0x0[0x64];
    /* 0x64 */ float m_scale;
};

/* TMessageDisp (class name from the RTTI of its shared_ptr control block; see
 * Game/StageManager.cpp). */
class TMessageDisp : public UnkHintTarget_801FB180 {
  public:
    /* 0x68 */ u8 field_0x68[0x74 - 0x68];
};

extern "C" {
/* Sets count bytes at dest to value (behaviour inferred). */
void fn_800CBBFC(void *dest, int value, int count);
UnkSaveData_800BB74C *fn_800BB74C(void);
bool fn_800BBDD8(void *flags, int index);
int fn_801E5040(UnkSaveData_800BB74C *save);
void fn_801E515C(UnkSaveData_800BB74C *save, UnkRouteInfo_801E515C *info, int route);
void fn_801E5798(void *record, int route);
/* Sets a flag and returns the set (behaviour inferred). */
UnkFlags_801E5828<u16> *fn_801E5828(UnkFlags_801E5828<u16> *flags, int index);
/* The event manager, and its calls (behaviour inferred). */
void *fn_801D3140(void);
void fn_8025D38C(void *manager, int arg1);
bool fn_8025D41C(void *manager, UnkEvent_8025D41C *event, int arg2, int arg3);
void fn_8025D504(void *manager, int from, int to);
TMessageDisp *fn_8004ABD0(TMessageDisp *message, const char *name, int arg2);
void *fn_80117514(void);
void fn_802E5958(void *manager);
void fn_802E597C(void *manager, const boost::weak_ptr<UnkHintTarget_801FB180> &target);
bool fn_802E5810(void *manager);
bool fn_802B8DF0(UnkTask_802E3DBC *task);
}

/* The stage table entry at index (name inferred; the lookup ignores the object it is called on). */
inline const UnkStageInfo_804C4A18 *StageEntry(int index) {
    fn_800B74B4();
    return &lbl_804C4A18[index];
}

namespace StageSequence {
namespace PlayStory {

class phaseImpl {
  public:
    phaseImpl();
    ~phaseImpl();

    /* Runs the phase; false once it ended. */
    bool Tick();
    /* Ends the stage early (name inferred). */
    void Skip();

    /* The states (names inferred). */
    void ShowRoute();
    bool PlayRouteEvents();
    bool StartStage();
    bool PlayStage();
    bool PlayEvents();
    void PlayStaffroll();
    bool WaitSave();

    /* Works out the next stage from the way the stage ended (name inferred). */
    bool ChooseNext(int result);
    /* Records the ending and starts saving (name inferred). */
    bool StartSave();
    /* The staff roll to play, and in arg whether the ending is new (names inferred). */
    int GetEnding(int *arg);

    /* 0x00 */ s32 m_state;
    /* 0x04 */ boost::shared_ptr<StageBase::Base> m_stage;
    /* 0x0C */ s32 m_stageId; /* the next stage, or -1 at the end of the route */
    /* 0x10 */ s32 m_prevStageId;
    /* 0x14 */ UnkTask_802E3DBC *m_saveTask;
    /* 0x18 */ boost::shared_ptr<UnkHintTarget_801FB180> m_message;
    /* 0x20 */ u8 m_lastMovie; /* the last story movie played (meaning inferred) */
};

/* Deletes the phaseImpl (name inferred; see checked_delete below). */
inline void DeleteImpl(phaseImpl *impl) {
    delete impl;
}

} // namespace PlayStory
} // namespace StageSequence

/* Retail keeps boost::checked_delete<phaseImpl> out of line and the destructor at its own place;
 * this specialization gives that order under GC/2.7 (see docs/research-notes.md and
 * Game/EnergyCore.cpp). */
namespace boost {
template <>
inline void checked_delete<StageSequence::PlayStory::phaseImpl>(StageSequence::PlayStory::phaseImpl *x) {
    StageSequence::PlayStory::DeleteImpl(x);
}
} // namespace boost

SIZE_ASSERT(StageSequence::PlayStory::phaseImpl, 0x24);
SIZE_ASSERT(StageSequence::PlayStory::Phase, 0x8);

namespace StageSequence {
namespace PlayStory {

phaseImpl::phaseImpl() : m_state(0), m_saveTask(0), m_lastMovie(0) {
    UnkSaveData_800BB74C *save = fn_800BB74C();
    switch (lbl_80584720.m_storyMode) {
    case 0: {
        m_stageId = 100;
        save->m_route.m_length = 0;
        save->m_route.m_stages[0] = 5;
        m_prevStageId = -1;
        UnkSaveData_800BB74C *data = fn_800BB74C();
        data->m_route.field_0x64 = 5;
        for (int i = 0; i < 20; i++) {
            lbl_8057D6A0.field_0xD4[i] = 0;
        }
        break;
    }
    case 1: {
        UnkStoryRoute_800BB74C *route = &save->m_route;
        m_stageId = StageEntry(route->m_stages[route->m_length])->m_id;
        m_prevStageId = StageEntry(route->m_stages[route->m_length - 1])->m_id;
        for (int i = 0; i < 20; i++) {
            lbl_8057D6A0.field_0xD4[i] = fn_800BBDD8(route->m_flags, i);
        }
        break;
    }
    case 2: {
        m_stageId = 700;
        m_prevStageId = -1;
        UnkSaveData_800BB74C *data = fn_800BB74C();
        data->m_route.field_0x64 = 5;
        for (int i = 0; i < 20; i++) {
            lbl_8057D6A0.field_0xD4[i] = 1;
        }
        break;
    }
    }
}

phaseImpl::~phaseImpl() {
    fn_802E5958(fn_80117514());
}

bool phaseImpl::Tick() {
    switch (m_state) {
    case 0:
        ShowRoute();
        m_state = 1;
        return true;
    case 1:
        return PlayRouteEvents();
    case 2:
        return StartStage();
    case 3:
        return PlayStage();
    case 4:
        return PlayEvents();
    case 5:
        PlayStaffroll();
        return true;
    case 6:
        return WaitSave();
    }
    return false;
}

void phaseImpl::ShowRoute() {
    fn_8025D38C(fn_801D3140(), 0);
    fn_8025D504(fn_801D3140(), fn_80176C04(fn_800B74B4(), m_prevStageId), fn_80176C04(fn_800B74B4(), m_stageId));
}

bool phaseImpl::PlayRouteEvents() {
    if (!m_stage) {
        UnkEvent_8025D41C event;
        char name[0x20];
        if (fn_8025D41C(fn_801D3140(), &event, 0, 0) && event.m_stageId != m_stageId) {
            if (event.m_stageId > 0) {
                m_stage = boost::shared_ptr<StageBase::Base>(new StageManager(event.m_stageId, 1));
            } else {
                sprintf(name, "E%04d.sfd", event.m_movieId);
                m_stage = boost::shared_ptr<StageBase::Base>(new StageMovie(name));
                fn_8025D41C(fn_801D3140(), &event, 0, 1);
            }
        } else {
            m_stage = boost::shared_ptr<StageBase::Base>(new StageRouteDisplay(m_stageId));
            m_state = 2;
        }
    } else {
        switch (m_stage->Tick()) {
        case 9:
            return false;
        case 1:
            break;
        default:
            m_stage.reset();
            break;
        }
    }
    return true;
}

bool phaseImpl::StartStage() {
    if (!m_stage) {
        fn_800CBBFC(lbl_8057D6A0.field_0x148, 1, 3);
        m_stage = boost::shared_ptr<StageBase::Base>(new StageManager(m_stageId, 0));
        m_state = 3;
    } else if (m_stage->Tick() != 1) {
        m_stage.reset();
    }
    return true;
}

bool phaseImpl::PlayStage() {
    int result = m_stage->Tick();
    switch (result) {
    case 1:
        return true;
    case 2:
    case 7:
        break;
    case 3:
    case 4:
    case 5:
    case 6:
        m_state = 4;
        return ChooseNext(result);
    case 9:
        return false;
    }
    return false;
}

void phaseImpl::Skip() {
    m_stage->vf_0x10();
}

bool phaseImpl::ChooseNext(int result) {
    UnkSaveData_800BB74C *save = fn_800BB74C();
    StageManager *manager = dynamic_cast<StageManager *>(m_stage.get());
    int index = lbl_8057D6A0.m_stageIndex;
    const UnkStageInfo_804C4A18 *entry = StageEntry(index);
    if (entry->m_flags & 1) {
        result = 6;
    }
    int next;
    switch (result) {
    case 3:
        next = entry->m_missions[0].field_0x0;
        break;
    case 4:
        next = entry->m_missions[1].field_0x0;
        break;
    case 5:
        next = entry->m_missions[2].field_0x0;
        break;
    case 6:
        if (lbl_80584720.m_storyMode != 2) {
            int mission;
            switch (save->m_route.field_0x68) {
            case 3:
                mission = 0;
                break;
            case 4:
                mission = 1;
                break;
            case 5:
                mission = 2;
                break;
            default:
                mission = 1;
                break;
            }
            next = entry->m_missions[mission].field_0x0;
        } else {
            next = entry->m_missions[2].field_0x0;
        }
        break;
    }
    m_stage.reset();
    m_prevStageId = m_stageId;
    if (next == -2) {
        m_stageId = -1;
    } else {
        m_stageId = StageEntry(next)->m_id;
    }
    return true;
}

bool phaseImpl::PlayEvents() {
    if (!m_stage) {
        UnkEvent_8025D41C event;
        char name[0x20];
        if (m_lastMovie) {
            fn_8025D38C(fn_801D3140(), 1);
        }
        if (fn_8025D41C(fn_801D3140(), &event, 1, 0)) {
            fn_8025D38C(fn_801D3140(), 0);
            if (event.m_stageId > 0) {
                m_stage = boost::shared_ptr<StageBase::Base>(new StageManager(event.m_stageId, 1));
            } else {
                sprintf(name, "E%04d.sfd", event.m_movieId);
                m_stage = boost::shared_ptr<StageBase::Base>(new StageMovie(name));
                fn_8025D41C(fn_801D3140(), &event, 0, 1);
                if (event.m_movieId == 0x2009) {
                    m_lastMovie = 1;
                }
            }
        } else {
            m_stage.reset();
            if (m_stageId < 0) {
                int arg;
                int ending = GetEnding(&arg);
                m_stage = boost::shared_ptr<StageBase::Base>(new StageStaffroll(ending, arg));
                m_state = 5;
            } else {
                m_state = 0;
            }
        }
    } else {
        switch (m_stage->Tick()) {
        case 9:
            return false;
        case 1:
            break;
        default:
            m_stage.reset();
            break;
        }
    }
    return true;
}

bool phaseImpl::StartSave() {
    bool started = false;
    UnkSaveData_800BB74C *save = fn_800BB74C();
    if (save != 0) {
        if (lbl_80584720.m_storyMode != 2) {
            int route = fn_801E5040(fn_800BB74C());
            if (route < 0) {
                route = 0;
            }
            fn_801E5798(save->field_0x16D0, route);
            save->m_route.m_length = 0;
            UnkRouteInfo_801E515C info;
            fn_801E515C(fn_800BB74C(), &info, route);
            fn_801E5828(&save->m_endings, info.m_ending);
        } else {
            fn_801E5828(&save->m_endings, 10);
        }
        if (save->m_endings.Count() >= 10) {
            save->field_0x16C8 = 1;
        }
        started = true;
        TMessageDisp *message = (TMessageDisp *)operator new(0x74);
        if (message != 0) {
            message = fn_8004ABD0(message, "Save", 1);
        }
        m_message = boost::shared_ptr<UnkHintTarget_801FB180>(message);
        fn_802E597C(fn_80117514(), boost::weak_ptr<UnkHintTarget_801FB180>(m_message));
        m_saveTask = (UnkTask_802E3DBC *)new SaveCycle::CycleTask(
            (Task *)fn_80046178()->field_0x40, SaveCycle::PHASE_SAVE);
    }
    return started;
}

void phaseImpl::PlayStaffroll() {
    if (m_stage->Tick() == 8) {
        StartSave();
        m_state = 6;
    }
}

bool phaseImpl::WaitSave() {
    if (m_saveTask != 0) {
        if (fn_802B8DF0(m_saveTask)) {
            m_saveTask->m_flags |= 1;
            m_saveTask = 0;
        }
    } else if (fn_802E5810(fn_80117514())) {
        return false;
    }
    return true;
}

int phaseImpl::GetEnding(int *arg) {
    if (lbl_80584720.m_storyMode != 2) {
        UnkSaveData_800BB74C *save = fn_800BB74C();
        UnkRouteInfo_801E515C info;
        int route = fn_801E5040(save);
        fn_801E515C(save, &info, route);
        int ending = info.m_ending;
        *arg = fn_801E5828(&save->m_endings, ending)->m_bits == 0;
        switch (ending) {
        case 0:
        case 1:
        case 2:
        case 3:
            return 0;
        case 4:
        case 5:
            return 3;
        case 6:
        case 7:
            return 2;
        case 8:
        case 9:
            return 1;
        }
        return 0;
    }
    return 4;
}

Phase::Phase() : m_impl(new phaseImpl) {}

Phase::~Phase() {}

bool Phase::Tick() {
    return m_impl->Tick();
}

void Phase::Skip() {
    m_impl->Skip();
}

} // namespace PlayStory
} // namespace StageSequence
