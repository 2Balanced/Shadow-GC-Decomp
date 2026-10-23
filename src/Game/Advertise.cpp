/* Advertise (class name from RTTI): the GameSystem that runs the front end (the attract mode and
 * menus). On start it stops the sound, opens the front-end task (AdvReal::AdvFolder) when the game
 * state's mode is 6..8, and resets the memory-card event list; on shutdown it marks that task done
 * and stops the sound again (behaviours inferred). The unit also holds that event list: a vector
 * of 12-byte records behind fn_801D3140, which other units read (fn_8025D2C4, fn_8025D41C, ...).
 * File name from the class; names marked "(name inferred)" are not original. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#define MSL_VECTOR_MIDDLE
#define MSL_VECTOR_SWAP_GLOBAL
#include "Game/GameSystem.h"
#include "Game/Render.h"
#include "Game/Task.h"
#include <dolphin/types.h>
#include "size_assert.h"
#include <vector>

namespace AdvReal {

/* The front-end task (see Game/AdvFolder.cpp). */
class AdvFolder : public Task {
  public:
    AdvFolder(Task *parent);
    virtual ~AdvFolder();
    virtual void Tick(float dt);
};

} // namespace AdvReal

/* The game state (struct name unknown; as far as this unit reads it). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x38];
    /* 0x038 */ s32 m_mode; /* meaning inferred */
    /* 0x03C */ u8 field_0x3C[0xCC - 0x3C];
    /* 0x0CC */ u8 m_record[0x148 - 0xCC]; /* (name inferred; see Game/StageSequencePlayOnce.cpp) */
    /* 0x148 */ u8 field_0x148[0x150 - 0x148];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* A memory-card event record (struct name unknown; see Game/GoalAction.cpp). */
struct UnkCardSlot_8025D41C {
    /* 0x0 */ s32 field_0x0;
    /* 0x4 */ s32 m_stageId;
    /* 0x8 */ s32 field_0x8;
};

/* The memory-card event list (struct name unknown). */
struct UnkCard_801D3140 {
    UnkCard_801D3140() {
        m_slots.reserve(16);
    }

    /* 0x0 */ std::vector<UnkCardSlot_8025D41C> m_slots;
};
SIZE_ASSERT(UnkCard_801D3140, 0xC);

/* The running GameSystem (set by GameSystem's constructor). */
extern GameSystem *lbl_805EF9A8;
/* The event list fn_801D3140 hands out. */
extern UnkCard_801D3140 *lbl_805EF2C8;

extern "C" {
/* The sound manager, and stopping the sound (behaviours inferred). */
void *fn_80053164(void);
void fn_801D615C(void *sound, int arg1);
void fn_801D6340(void *sound, int arg1, int arg2, int arg3);
/* The root of the task tree and finishing its tasks (behaviour inferred). */
void *fn_8001C4F8(void);
void fn_801EB314(void *root);
/* Saves the game record (behaviour inferred; see Game/AdvSelectMode.cpp). */
void fn_802062FC(void *record);
/* Clears the event list (behaviour inferred). */
void fn_8025D6FC(UnkCard_801D3140 *card);
/* Stops the loading screen (behaviour inferred; see Game/NowLoading.h). */
void fn_802BEB90(void);
}

class Advertise : public GameSystem {
  public:
    Advertise();
    virtual ~Advertise();
    virtual bool Initialize();
    virtual bool IsFinished();
    virtual bool Finalize();
    virtual void Abort();

    /* 0x4 */ AdvReal::AdvFolder *m_folder;
    /* 0x8 */ u8 m_aborted; /* set by Abort (name inferred) */
};
SIZE_ASSERT(Advertise, 0xC);

Advertise::Advertise() {
    fn_802BEB90();
}

Advertise::~Advertise() {}

/* The event list (name inferred). */
static UnkCard_801D3140 *GetCard() {
    static UnkCard_801D3140 s_card;
    return &s_card;
}

extern "C" UnkCard_801D3140 *fn_801D3140(void) {
    if (lbl_805EF2C8 == 0) {
        lbl_805EF2C8 = GetCard();
    }
    return lbl_805EF2C8;
}

bool Advertise::Initialize() {
    m_aborted = 0;
    fn_801D6340(fn_80053164(), -1, 0, 0);
    switch (lbl_8057D6A0.m_mode) {
    case 6:
    case 7:
    case 8:
        m_folder = new AdvReal::AdvFolder((Task *)fn_80046178()->field_0x38);
        fn_802062FC(lbl_8057D6A0.m_record);
        break;
    }
    fn_8025D6FC(fn_801D3140());
    return true;
}

bool Advertise::IsFinished() {
    return m_aborted != 0;
}

bool Advertise::Finalize() {
    if (m_folder != 0) {
        m_folder->m_flags |= 1;
    }
    fn_801D615C(fn_80053164(), 0);
    fn_801EB314(fn_8001C4F8());
    return true;
}

void Advertise::Abort() {
    m_aborted = 1;
}

/* Ends the front end if it is running (name inferred). */
extern "C" bool fn_801D2FD4(void) {
    Advertise *advertise = dynamic_cast<Advertise *>(lbl_805EF9A8);
    if (advertise != 0) {
        advertise->Abort();
        return true;
    }
    return false;
}
