/* BossTarget (class name from RTTI): the task that keeps a boss's targets (the points characters
 * aim at). BossCtrl creates it with the boss's other tasks (0x8017A108). Its state is a separately
 * allocated object (class name unknown) that creates a Target for each entry of the boss's target
 * table into a PooledList (the list BossVarParam::m_targets points to), keeps each on its node of
 * the boss's motion, and turns them off while the boss is stopped. Member and function names are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#define BOSSVARPARAM_CHARA_SHARED_PTR
#include "Game/BossCtrlUnit.h"
#include "Game/BossDef.h"
#include "Game/BossRootTask.h"
#include "Game/MathTypes.h"
#include "Game/PooledList.h"
#include "Game/Task.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* The game state (struct name unknown; see Game/BossMotion.cpp): the frame time at 0x20. */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x20];
    /* 0x020 */ float m_frameTime;
    /* 0x024 */ float m_dt;
    /* 0x028 */ u8 field_0x28[0x150 - 0x28];
};
extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* Target (class name from RTTI): a point characters aim at. Bit 0 of the flag word makes it
 * active (names inferred, as in Game/BAGunShip.cpp). */
class Target {
  public:
    void TurnOn() {
        m_flags |= 1;
    }
    void TurnOff() {
        m_flags &= ~1;
    }
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);

    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ u32 m_flags;
};

extern "C" {
/* Creates a target for a character (see Game/ArkCannon.cpp). */
boost::shared_ptr<Target> fn_801E7128(const boost::weak_ptr<CharInfo> &owner, int flags);

/* A node of a motion and its matrix (see Game/DevilDoomCtrlUnit.cpp). */
void *fn_80415E70(_stMtnPlayWork *work, int id);
void fn_80016370(Mtx34f *matrix, void *frame);

Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
}

class BossTarget : public Task {
  public:
    class Impl;

    BossTarget(Task *parent, BossVarParam *param);
    virtual ~BossTarget();
    virtual void Tick(float dt);

    /* 0x28 */ boost::scoped_ptr<Impl> m_impl;
    /* 0x2C */ u32 pad_0x2C; /* the allocation is 0x30 bytes */
};

class BossTarget::Impl {
  public:
    Impl(BossVarParam *param);
    ~Impl();
    void Update(float dt);

    /* 0x00 */ PooledList<Target> m_list;
    /* 0x10 */ BossVarParam *m_param;
};
SIZE_ASSERT(BossTarget::Impl, 0x14);
SIZE_ASSERT(BossTarget, 0x30);

/* Deletes the state (name inferred; see checked_delete below). */
inline void DeleteImpl(BossTarget::Impl *impl) {
    delete impl;
}

/* Retail keeps boost::checked_delete<Impl> out of line (0x8017AD40) and ~Impl at its own place
 * (0x8017B010). GC/2.7 inlines checked_delete when the destructor is not inline; this
 * specialization calls an inline helper instead, which gives retail's code and order (as in
 * Game/Rocket.cpp; see docs/research-notes.md). */
namespace boost {
template <> inline void checked_delete<BossTarget::Impl>(BossTarget::Impl *x) {
    DeleteImpl(x);
}
} // namespace boost

BossTarget::Impl::Impl(BossVarParam *param) : m_param(param) {
    UnkBossTargetEntry *entry = m_param->m_def->m_targets;
    boost::shared_ptr<Target> target;
    while (entry && entry->field_0x0) {
        m_list.PushBack(fn_801E7128(boost::weak_ptr<CharInfo>(m_param->m_charaPtr), entry->field_0x0));
        entry++;
    }
    m_param->m_targets = &m_list;
}

BossTarget::Impl::~Impl() {
    if (m_list.Size() > 0) {
        m_list.Clear();
    }
    m_param->m_targets = 0;
}

void BossTarget::Impl::Update(float dt) {
    if (m_param->m_motion) {
        PooledList<Target>::iterator it;
        it = PooledList<Target>::iterator(m_list.Head());
        UnkBossTargetEntry *entry = m_param->m_def->m_targets;
        for (; it != m_list.End(); it++, entry++) {
            Mtx34f matrix;
            fn_80016370(&matrix, fn_80415E70(m_param->m_motion, entry->field_0x4));
            /* The address of the returned vector, as retail passes it (CodeWarrior allows it);
             * a Ref() wrapper changes the evaluation order. */
            (*it)->fn_800CCE38(&fn_8040B67C(&matrix, entry->m_target));
            if (m_param->m_stopped && ((*it)->m_flags & 1)) {
                (*it)->TurnOff();
            } else if (!m_param->m_stopped && !((*it)->m_flags & 1)) {
                (*it)->TurnOn();
            }
        }
    }
}

BossTarget::BossTarget(Task *parent, BossVarParam *param) : Task(parent), m_impl(new Impl(param)) {
    m_name = "Target";
}

BossTarget::~BossTarget() {}

void BossTarget::Tick(float dt) {
    m_impl->Update(lbl_8057D6A0.m_frameTime);
}
