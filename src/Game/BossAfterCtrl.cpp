/* BossAfterCtrl (class name from RTTI): a task BossCtrl creates under each boss task. Every frame it
 * runs the boss's control units of kind 3 (DevilDoomCtrlUnit_DoomsEye sets it), after BossCtrl ran
 * the others, and drops each unit whose Tick returns true. Its state is a separately allocated
 * object (class name unknown) that keeps the BossVarParam. Member and function names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "boost/scoped_ptr.hpp"
#include "Game/BossCtrlUnit.h"
#include "Game/PooledList.h"
#include "Game/Task.h"
#include "size_assert.h"

/* The game state (struct name unknown; see Game/BossMotion.cpp): the frame time at 0x20. */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x20];
    /* 0x020 */ float m_frameTime;
    /* 0x024 */ float m_dt;
    /* 0x028 */ u8 field_0x28[0x150 - 0x28];
};
extern UnkGameState_8057D6A0 lbl_8057D6A0;

class BossAfterCtrl : public Task {
  public:
    class Impl;

    BossAfterCtrl(Task *parent, BossVarParam *param);
    virtual ~BossAfterCtrl();
    virtual void Tick(float dt);

    /* 0x28 */ boost::scoped_ptr<Impl> m_impl;
    /* 0x2C */ u32 field_0x2C;
};
SIZE_ASSERT(BossAfterCtrl, 0x30);

class BossAfterCtrl::Impl {
  public:
    Impl(BossVarParam *param);
    ~Impl() {}
    void Tick(float dt);

    /* 0x0 */ BossVarParam *m_param;
};
SIZE_ASSERT(BossAfterCtrl::Impl, 0x4);

BossAfterCtrl::Impl::Impl(BossVarParam *param) : m_param(param) {}

void BossAfterCtrl::Impl::Tick(float dt) {
    PooledList<BossCtrlUnit>::iterator it(m_param->m_units->Head());
    while (it != m_param->m_units->End()) {
        s32 kind = (*it)->m_kind;
        if (kind == 3) {
            /* BossCtrlUnit::Tick is declared to return int; retail tests the result as a byte, after
             * the shared_ptr temporary is destroyed (as in Game/BossCtrl.cpp). */
            int done = (*it)->Tick(dt);
            if (static_cast<u8>(done)) {
                m_param->m_units->Erase(it++);
                continue;
            }
        }
        it++;
    }
}

BossAfterCtrl::BossAfterCtrl(Task *parent, BossVarParam *param) : Task(parent), m_impl(new Impl(param)) {
    m_name = "AfterCtrl";
}

BossAfterCtrl::~BossAfterCtrl() {}

void BossAfterCtrl::Tick(float dt) {
    m_impl->Tick(lbl_8057D6A0.m_frameTime);
}
