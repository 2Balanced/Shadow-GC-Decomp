/* Player::Npc::BossSonicTarget (class name from RTTI): the aim of Boss Sonic (see
 * Game/BossSonicTarget.h). It works as NpcTarget does (Game/PlayerNpcTarget.cpp): each frame it
 * searches the targets of its kinds ahead of the boss and keeps the nearest, which slot 0x28
 * returns. Unlike an NPC partner it always marks its own target with the same kinds and searches
 * all of 0x78000. File name from the class; other names are inferred. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
/* Enables shared_ptr::reset(Y *) (see boost/shared_ptr.hpp). */
#define BOOST_SP_RESET_POINTER
#include "Game/BossSonicTarget.h"
#include "Game/Npc.h"
#include "Game/TargetSearchList.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* Target (class name from RTTI): a point other objects can aim at; the word at 0x30 holds the
 * kinds it counts as (as in Game/PlayerNpcTarget.cpp). */
class Target {
  public:
    void SetFlags(u32 flags) {
        m_flags = flags;
    }

    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ u32 m_flags;
};

extern "C" {
/* The time (read around the update; the results are unused, as in Game/PlayerNpcTarget.cpp). */
u64 fn_80051A94(void);
/* The player's own target (Game/PlayerTargetBase.cpp). */
boost::weak_ptr<Target> fn_80206574(Player::PlayerTargetBase *aim);
/* Narrows a target search list by a flag value (behaviour inferred). */
void fn_801E8138(TargetSearchList *list, int value);
/* The NPC's hint branch (as in Game/NpcHint.cpp). */
int fn_80073A08(Player::Npc::Npc *npc);
}

namespace Player {
namespace Npc {

/* The aim's state (class name unknown; size from its allocation). */
class BossSonicTarget::Impl {
  public:
    Impl();
    ~Impl();
    void Init(PlayerTargetBase *owner, const boost::shared_ptr<Npc> &npc);
    void Update();

    /* 0x00 */ PlayerTargetBase *m_owner;
    /* 0x04 */ boost::shared_ptr<Npc> m_npc;
    /* 0x0C */ boost::weak_ptr<Target> m_target;
    /* 0x14 */ u32 m_kinds; /* the target kinds searched for */
};
SIZE_ASSERT(BossSonicTarget::Impl, 0x18);

/* Deletes the state (name inferred; see checked_delete below). */
inline void DeleteImpl(BossSonicTarget::Impl *impl) {
    delete impl;
}

} // namespace Npc
} // namespace Player

/* Retail keeps boost::checked_delete<BossSonicTarget::Impl> out of line (0x8029594C) and ~Impl at
 * its own place (0x80295D4C); this specialization calls an inline helper to get that under
 * GC/2.7, as Game/PlayerNpcTarget.cpp does (see docs/research-notes.md). */
namespace boost {
template<> inline void checked_delete<Player::Npc::BossSonicTarget::Impl>(
    Player::Npc::BossSonicTarget::Impl *x) {
    Player::Npc::DeleteImpl(x);
}
} // namespace boost

namespace Player {
namespace Npc {

BossSonicTarget::Impl::Impl() {}

BossSonicTarget::Impl::~Impl() {}

void BossSonicTarget::Impl::Init(PlayerTargetBase *owner, const boost::shared_ptr<Npc> &npc) {
    m_owner = owner;
    m_npc = npc;
    m_kinds = 0;
    boost::shared_ptr<Target> target = fn_80206574(m_owner).lock();
    if (target) {
        u32 flags = target->m_flags;
        target->SetFlags(flags | 0x92C00);
        m_kinds = 0x78000;
        if (fn_80073A08(m_npc.get()) == 0) {
            m_kinds |= 0x400000;
        } else {
            m_kinds |= 0x200000;
        }
    }
}

void BossSonicTarget::Impl::Update() {
    Vec3f direction;
    fn_800091EC(&direction, &(const Vec3f &)m_owner->ToAimSpace(Vec3f(0.0f, 0.0f, -1.0f)));
    boost::shared_ptr<TargetSearchList> list;
    list.reset(fn_801E7930(fn_80060058()));
    fn_801E80C0(list.get(), m_kinds);
    fn_801E8138(list.get(), 0x80000);
    fn_801E7E48(list.get(), fn_80206574(m_owner));
    m_target = fn_801E848C(list.get(), &m_npc->m_position);
}

BossSonicTarget::BossSonicTarget(const boost::shared_ptr<Npc> &npc)
    : PlayerTargetBase(npc), m_impl(new Impl) {
    m_impl->Init(this, npc);
}

BossSonicTarget::~BossSonicTarget() {}

void BossSonicTarget::vf_0x0C() {
    PlayerTargetBase::vf_0x0C();
    fn_80051A94();
    m_impl->Update();
    fn_80051A94();
}

boost::weak_ptr<Target> BossSonicTarget::vf_0x28() {
    return m_impl->m_target;
}

} // namespace Npc
} // namespace Player
