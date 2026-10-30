/* StageTriggerEventTask (class name from RTTI): the task the goal action starts after a stage's
 * end has played the next event scene of the memory-card event list (see Game/StageEventList.cpp):
 * it then ends the stage, at once or when the screen allows it, and starts the fade when the event
 * task reaches its state 5. File name from the class. Names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/StageAction.h"
#include "Game/Task.h"
#include <dolphin/types.h>
#include "size_assert.h"
#include <stdio.h>

/* The game state (struct name unknown; as far as this unit reads it). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x88];
    /* 0x088 */ u32 field_0x88; /* bit 0 picks the voice event (meaning unknown) */
    /* 0x08C */ u8 field_0x8C[0xC0 - 0x8C];
    /* 0x0C0 */ u8 field_0xC0; /* (meaning unknown; see Game/ActStageController.cpp) */
    /* 0x0C1 */ u8 field_0xC1[0xCC - 0xC1];
    /* 0x0CC */ s32 m_mode; /* the event list is used in modes 1 and 7 (meaning inferred) */
    /* 0x0D0 */ u8 field_0xD0; /* set: no events (meaning unknown) */
    /* 0x0D1 */ u8 field_0xD1[0x150 - 0xD1];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* What lbl_8051CE88 points to (struct name unknown; as far as this unit reads it). */
struct UnkObject_8051CE88 {
    /* 0x000 */ u8 field_0x0[0x40];
    /* 0x040 */ s32 field_0x40;
    /* 0x044 */ u8 field_0x44[0x978 - 0x44];
    /* 0x978 */ u8 field_0x978;
};

extern UnkObject_8051CE88 *lbl_8051CE88[4];

/* What fn_801723E0 returns (class name unknown; see Game/GoalAction.cpp). */
struct UnkEventHolder_801723E0 {
    /* The task, if any (name inferred). */
    Task *GetTask() {
        return m_task ? m_task : 0;
    }

    /* 0x0 */ Task *m_task;
};

/* The event task fn_801723E0 holds (class name unknown; as far as this unit reads it). */
struct UnkEventTask_801723E0 {
    /* 0x00 */ u8 field_0x0[0x28];
    /* 0x28 */ s32 m_state;
    /* 0x2C */ u8 field_0x2C[0x34 - 0x2C];
    /* 0x34 */ u8 field_0x34;
};

/* The file system fn_800371D8 returns (class name unknown; see Game/ActStageController.cpp). */
class UnkFileSystem_800371D8 {
  public:
    /* Whether a file exists (name inferred). */
    bool Exists(const char *path) {
        return exists(path);
    }

    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ bool (*exists)(const char *path);
};

/* The flag object fn_80178678 returns (struct name unknown; see Game/GoalAction.cpp). */
struct UnkFlag_80178678 {
    /* 0x0 */ u8 m_skip; /* set: the event task ends the stage at once (meaning inferred) */
};

/* An event and the value that goes with it (struct name unknown). */
struct UnkEventPair_8025D280 {
    /* 0x0 */ s32 m_event;
    /* 0x4 */ s32 m_value;
};

/* A memory-card event record (struct name unknown; see Game/GoalAction.cpp): an event pair, and
 * which list of the stage's events it came from (0 or 1). */
struct UnkCardSlot_8025D41C {
    /* 0x0 */ UnkEventPair_8025D280 m_pair;
    /* 0x8 */ s32 field_0x8;
};

/* The memory-card event list (see Game/StageEventList.cpp). */
struct UnkCard_801D3140;

extern "C" {
UnkCard_801D3140 *fn_801D3140(void);
UnkFileSystem_800371D8 *fn_800371D8(void);
UnkEventHolder_801723E0 *fn_801723E0(void);
/* Plays event (see Game/GoalAction.cpp). */
void fn_8017430C(UnkEventHolder_801723E0 *holder, s32 event);
/* Fades the screen (behaviour inferred; see Game/AdvLogo.cpp). */
void fn_80173834(int arg0, float time);
Task *fn_8001C4F8(void);
Task *fn_801EB304(Task *root, int layer);
/* Takes the first event of type from the event list (see Game/StageEventList.cpp). */
bool fn_8025D41C(UnkCard_801D3140 *card, UnkCardSlot_8025D41C *slot, int type, bool pop);
}

class StageTriggerEventTask : public Task {
  public:
    StageTriggerEventTask(Task *parent, bool skip) : Task(parent), m_skip(skip), m_faded(false) {
        m_name = "StageTriggerEventTask";
    }
    virtual ~StageTriggerEventTask();
    virtual void Tick(float dt);

    /* 0x28 */ u8 m_skip;  /* end the stage at once (meaning inferred) */
    /* 0x29 */ u8 m_faded; /* the fade has been started */
    /* 0x2A */ u8 field_0x2A[0x30 - 0x2A];
};
SIZE_ASSERT(StageTriggerEventTask, 0x30);

StageTriggerEventTask::~StageTriggerEventTask() {}

void StageTriggerEventTask::Tick(float dt) {
    if (fn_801723E0()->m_task == 0) {
        if (m_skip) {
            fn_80074470()->SetResult(2);
            fn_80175EEC(fn_80074470(), 16, false);
            m_flags |= 1;
        } else if (!(lbl_8051CE88[0]->field_0x978 | (lbl_8051CE88[0]->field_0x40 != 0))) {
            fn_80175EEC(fn_80074470(), 3, false);
            lbl_8057D6A0.field_0xC0 = 0;
            m_flags |= 1;
        }
    } else {
        UnkEventTask_801723E0 *task = (UnkEventTask_801723E0 *)fn_801723E0()->GetTask();
        if (task != 0 && task->field_0x34 == 0 && !m_faded && task->m_state == 5) {
            m_faded = true;
            fn_80173834(1, 0.0f);
        }
    }
}

/* Whether an event is waiting (name inferred). */
extern "C" bool fn_8025DD20(UnkFlag_80178678 *flag) {
    return (u8)(fn_8025D41C(fn_801D3140(), 0, 0, false) != 0);
}

/* The path of the event scene to load. */
static char s_path[0x20];

/* Plays the next event of type if there is one and, unless type is 1, starts the task that ends the
 * stage after it (name inferred). */
extern "C" bool fn_8025DBC0(UnkFlag_80178678 *flag, int type) {
    UnkCardSlot_8025D41C slot;

    if (fn_8025D41C(fn_801D3140(), &slot, type, true)) {
        fn_80175EEC(fn_80074470(), 4, false);
        sprintf(s_path, "event/event%04d_sceneA.one", slot.m_pair.m_event);
        if (fn_800371D8()->Exists(s_path)) {
            fn_8017430C(fn_801723E0(), slot.m_pair.m_event);
        }
        if (type != 1) {
            if (flag->m_skip) {
                new StageTriggerEventTask(fn_801EB304(fn_8001C4F8(), 3), true);
            } else {
                new StageTriggerEventTask(fn_801EB304(fn_8001C4F8(), 3), false);
            }
        }
        return true;
    }
    return false;
}
