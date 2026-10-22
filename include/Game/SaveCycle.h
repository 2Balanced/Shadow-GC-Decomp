#ifndef GAME_SAVECYCLE_H
#define GAME_SAVECYCLE_H

#include <boost/scoped_ptr.hpp>

#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class TMemoryCardCtrl;

/* The save/load cycle (Game/SaveCycle.cpp). Class names marked "RTTI" come from the game's RTTI;
 * the others, and all member names, are inferred. */

/* The shared memory card controller: the phases hold it while they exist, the first one creates
 * the TMemoryCardCtrl task and the last one ends it (class name unknown; fn_801E5B80 returns the
 * game's one). */
class UnkCardCtrlHolder_801E5B80 {
  public:
    UnkCardCtrlHolder_801E5B80();

    TMemoryCardCtrl *Acquire();
    bool Release();

    /* 0x0 */ TMemoryCardCtrl *m_ctrl;
    /* 0x4 */ s32 m_users;
};
SIZE_ASSERT(UnkCardCtrlHolder_801E5B80, 0x8);

extern "C" UnkCardCtrlHolder_801E5B80 *fn_801E5B80(void);

namespace SaveCycle {

/* SaveCycle::CycleBase (RTTI): one phase of the cycle. Update returns false once the phase is
 * over. */
class CycleBase {
  public:
    CycleBase(int id);
    virtual ~CycleBase();
    virtual bool Update();
    virtual void Draw();

    /* 0x4 */ s32 m_id;
};
SIZE_ASSERT(CycleBase, 0x8);

/* The phases (RTTI; their code is at 0x803596E0-0x8035B8DC). */
namespace Load {
class Phase : public CycleBase {
  public:
    Phase();
    ~Phase();

    /* 0x8 */ void *m_impl;
};
SIZE_ASSERT(Phase, 0xC);
} // namespace Load

namespace Save {
class Phase : public CycleBase {
  public:
    Phase();
    ~Phase();

    /* 0x8 */ void *m_impl;
};
SIZE_ASSERT(Phase, 0xC);
} // namespace Save

namespace Check {
class Phase : public CycleBase {
  public:
    Phase();
    ~Phase();

    /* 0x8 */ void *m_impl;
};
SIZE_ASSERT(Phase, 0xC);
} // namespace Check

namespace CheckModule {
class Phase : public CycleBase {
  public:
    Phase();
    ~Phase();

    /* 0x8 */ void *m_impl;
};
SIZE_ASSERT(Phase, 0xC);
} // namespace CheckModule

/* Phase ids. */
enum {
    PHASE_NONE = 0,
    PHASE_LOAD = 1,
    PHASE_SAVE = 2,
    PHASE_CHECK = 3,
    PHASE_CHECK_MODULE = 8
};

} // namespace SaveCycle

/* The cycle: runs the current phase until it ends (class name unknown; Game/AdvMemoryCheck.cpp
 * declares the same class locally). */
class UnkMemoryCard_802E472C {
  public:
    UnkMemoryCard_802E472C(int id);
    ~UnkMemoryCard_802E472C();

    void ChangePhase(int id);

    static void SetCardUsable(u8 usable);
    static void SetResult(int result);
    static int GetResult();

    /* Whether the checked card can be used, and the last result. */
    static u8 s_cardUsable;
    static int s_result;

    /* 0x0 */ boost::shared_ptr<SaveCycle::CycleBase> m_phase;
};
SIZE_ASSERT(UnkMemoryCard_802E472C, 0x8);

extern "C" {
/* Runs the phase (ending it when it is over), whether no phase is left, drawing the phase, and
 * whether the card can be used. */
void fn_802E3E7C(UnkMemoryCard_802E472C *card);
bool fn_802E3E34(UnkMemoryCard_802E472C *card);
void fn_802E3E44(UnkMemoryCard_802E472C *card);
u8 fn_802E4784(void);
}

/* Deletes a cycle (name inferred; see checked_delete below). */
inline void DeleteCycle(UnkMemoryCard_802E472C *cycle) {
    delete cycle;
}

/* Retail keeps boost::checked_delete<UnkMemoryCard_802E472C> out of line (0x802E3D98) and the
 * destructor at its own place; this specialization calls an inline helper instead, which gives
 * retail's code and order under GC/2.7 (see docs/research-notes.md, EnergyCore). */
namespace boost {
template<> inline void checked_delete<UnkMemoryCard_802E472C>(UnkMemoryCard_802E472C *x) {
    DeleteCycle(x);
}
} // namespace boost

namespace SaveCycle {

/* SaveCycle::CycleTask (RTTI): runs a cycle, starting with phase id, until it ends. */
class CycleTask : public Task {
  public:
    CycleTask(Task *parent, int id);
    virtual ~CycleTask();
    virtual void Tick(float dt);

    /* 0x28 */ boost::scoped_ptr<UnkMemoryCard_802E472C> m_cycle;
    /* 0x2C */ u32 field_0x2C;
};
SIZE_ASSERT(CycleTask, 0x30);

} // namespace SaveCycle

#endif
