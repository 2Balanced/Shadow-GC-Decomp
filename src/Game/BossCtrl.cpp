/* BossCtrl (class name from RTTI): the task the boss tasks create under themselves. Its state is a
 * separately allocated object (class name unknown) that creates the boss's other tasks next to
 * it and keeps the boss's control units in a PooledList (the list BossVarParam::m_units points to;
 * fn_803358D0 adds to it). Every frame it runs the units of kind 0, then 1, then 2, and drops each
 * unit whose Tick returns true. Member and function names are inferred. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#define BOSSCTRL_MEMBERS
#include "boost/scoped_ptr.hpp"
#include "Game/BossCtrlUnit.h"
#include "Game/BossRootTask.h"
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

/* boost 1.32's reset, which the trimmed shared_ptr.hpp declares out of line: this unit holds the
 * first copy of the instance for BossCtrlUnit. */
template <> inline void boost::shared_ptr<BossCtrlUnit>::reset() {
    this_type().swap(*this);
}

/* The tasks BossCtrl creates (class names from RTTI), declared as far as creating them needs:
 * BossMotion (Game/BossMotion.cpp), BossCharaColli (Game/BossCharaColli.cpp), BossTarget
 * (Game/BossTarget.cpp), BossRender (Game/BossRender.cpp), BossAfterCtrl (vtable 0x8053E194,
 * constructor 0x801F1CB0) and BossForceClear (vtable 0x8055925C, constructor 0x802DFA64). Each is a
 * 0x30-byte Task. */
class BossMotion : public Task {
  public:
    BossMotion(Task *parent, BossVarParam *param);

    /* 0x28 */ u8 field_0x28[0x30 - 0x28];
};

class BossCharaColli : public Task {
  public:
    BossCharaColli(Task *parent, BossVarParam *param);

    /* 0x28 */ u8 field_0x28[0x30 - 0x28];
};

class BossTarget : public Task {
  public:
    BossTarget(Task *parent, BossVarParam *param);

    /* 0x28 */ u8 field_0x28[0x30 - 0x28];
};

class BossAfterCtrl : public Task {
  public:
    BossAfterCtrl(Task *parent, BossVarParam *param);

    /* 0x28 */ u8 field_0x28[0x30 - 0x28];
};

class BossRender : public Task {
  public:
    BossRender(Task *parent, BossVarParam *param);

    /* 0x28 */ u8 field_0x28[0x30 - 0x28];
};

class BossForceClear : public Task {
  public:
    BossForceClear(Task *parent);

    /* 0x28 */ u8 field_0x28[0x30 - 0x28];
};

class BossCtrl::Impl {
  public:
    Impl(Task *parent, BossCtrl *owner, BossVarParam *param);
    ~Impl();
    void Tick(float dt);

    /* 0x00 */ BossCtrl *m_owner;
    /* 0x04 */ Task *m_parent; /* the boss task: the parent of the tasks created here */
    /* 0x08 */ BossVarParam *m_param;
    /* 0x0C */ PooledList<BossCtrlUnit> m_units;
};
SIZE_ASSERT(BossCtrl::Impl, 0x1C);

/* Deletes the state (name inferred; see checked_delete below). */
inline void DeleteImpl(BossCtrl::Impl *impl) {
    delete impl;
}

/* Retail keeps boost::checked_delete<Impl> out of line (0x80179B18) and ~Impl at its own place
 * (0x80179FB8). GC/2.7 inlines checked_delete when the destructor is not inline; this
 * specialization calls an inline helper instead, which gives retail's code and order (as in
 * Game/Rocket.cpp; see docs/research-notes.md). */
namespace boost {
template <> inline void checked_delete<BossCtrl::Impl>(BossCtrl::Impl *x) {
    DeleteImpl(x);
}
} // namespace boost

BossCtrl::Impl::Impl(Task *parent, BossCtrl *owner, BossVarParam *param)
    : m_owner(owner), m_parent(parent), m_param(param) {
    m_param->m_units = &m_units;
    new BossMotion(m_parent, m_param);
    new BossCharaColli(m_parent, m_param);
    new BossTarget(m_parent, m_param);
    new BossAfterCtrl(m_parent, m_param);
    m_param->field_0x34 = reinterpret_cast<u32>(new BossRender(m_parent, m_param));
    new BossForceClear(m_parent);
}

BossCtrl::Impl::~Impl() {
    m_param->m_units = 0;
    m_units.Clear();
}

void BossCtrl::Impl::Tick(float dt) {
    for (int kind = 0; kind < 3; kind++) {
        PooledList<BossCtrlUnit>::iterator it(m_units.Head());
        while (it != m_units.End()) {
            s32 unitKind = (*it)->m_kind;
            if (kind == unitKind) {
                /* BossCtrlUnit::Tick is declared to return int; retail tests the result as a
                 * byte, after the shared_ptr temporary is destroyed. */
                int done = (*it)->Tick(dt);
                if (static_cast<u8>(done)) {
                    m_units.Erase(it++);
                    continue;
                }
            }
            it++;
        }
    }
}

BossCtrl::BossCtrl(Task *parent, BossVarParam *param) : Task(parent), m_impl(new Impl(parent, this, param)) {
    m_name = "Ctrl";
}

BossCtrl::~BossCtrl() {}

void BossCtrl::Tick(float dt) {
    m_impl->Tick(lbl_8057D6A0.m_frameTime);
}
