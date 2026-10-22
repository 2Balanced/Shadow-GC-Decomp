/* Player::Npc::NpcTarget (class name from RTTI): the aim of an NPC partner (see
 * Game/NpcTarget.h). File name inferred from the class; other names are inferred. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first). */
/* Enables shared_ptr::reset(Y *) (see boost/shared_ptr.hpp). */
#define BOOST_SP_RESET_POINTER
#include "Game/Npc.h"
#include "Game/NpcPartModel.h"
#include "Game/NpcTarget.h"
#include "Game/TargetSearchList.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* Target (class name from RTTI): a point other objects can aim at; the word at 0x30 holds its
 * flags (as in BAGunShip.cpp). */
class Target {
  public:
    void SetFlags(u32 flags) {
        m_flags = flags;
    }

    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ u32 m_flags;
};

extern "C" {
/* The time (as in DiscErrorHandler.cpp; read around the update, results unused). */
u64 fn_80051A94(void);
/* The player's own target (Game/PlayerTargetBase.cpp). */
boost::weak_ptr<Target> fn_80206574(Player::PlayerTargetBase *aim);
/* Narrowing a target search list by a flag value (behaviour inferred). */
void fn_801E8138(TargetSearchList *list, int value);
/* The NPC's hint branch (as in NpcHint.cpp). */
int fn_80073A08(Player::Npc::Npc *npc);
}

namespace Player {
namespace Npc {

/* The character type of an NPC (the word at 0x64; name inferred). */
inline s32 CharaType(Npc *npc) {
    return *reinterpret_cast<s32 *>(npc->field_0x64);
}

/* The aim's state (class name unknown; size from its allocation). */
class NpcTarget::Impl {
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
SIZE_ASSERT(NpcTarget::Impl, 0x18);

/* Deletes the state (name inferred; see checked_delete below). */
inline void DeleteImpl(NpcTarget::Impl *impl) {
    delete impl;
}

} // namespace Npc
} // namespace Player

/* Retail keeps boost::checked_delete<NpcTarget::Impl> out of line (0x80207038) and ~Impl at its
 * own place (0x80207484). As in Game/PlayerShadowSuper.cpp, this specialization calls an inline
 * helper, which gives retail's code and order under GC/2.7 (see docs/research-notes.md). */
namespace boost {
template<> inline void checked_delete<Player::Npc::NpcTarget::Impl>(
    Player::Npc::NpcTarget::Impl *x) {
    Player::Npc::DeleteImpl(x);
}
} // namespace boost

namespace Player {
namespace Npc {

NpcTarget::Impl::Impl() {}

NpcTarget::Impl::~Impl() {}

void NpcTarget::Impl::Init(PlayerTargetBase *owner, const boost::shared_ptr<Npc> &npc) {
    m_owner = owner;
    m_npc = npc;
    m_kinds = 0;
    boost::shared_ptr<Target> target = fn_80206574(m_owner).lock();
    if (target) {
        u32 flags = target->m_flags;
        u32 kind;
        switch (CharaType(fn_8006CA70(&m_npc))) {
        case 0xB:
            kind = 0x40000;
            break;
        case 0xC:
            kind = 0x20000;
            break;
        default:
            kind = 0x90000;
            break;
        }
        flags |= kind;
        target->SetFlags(flags);
        m_kinds = 0x78000;
        m_kinds &= ~kind;
        m_kinds |= 8;
        if (fn_80073A08(m_npc.get()) == 0) {
            m_kinds |= 0x400000;
        } else {
            m_kinds |= 0x200000;
        }
    }
}

void NpcTarget::Impl::Update() {
    Vec3f direction;
    fn_800091EC(&direction, &(const Vec3f &)m_owner->ToAimSpace(Vec3f(0.0f, 0.0f, -1.0f)));
    boost::shared_ptr<TargetSearchList> list;
    list.reset(fn_801E7930(fn_80060058()));
    fn_801E80C0(list.get(), m_kinds);
    fn_801E8138(list.get(), 0x80000);
    fn_801E7E48(list.get(), fn_80206574(m_owner));
    m_target = fn_801E848C(list.get(), &m_npc->m_position);
}

NpcTarget::NpcTarget(const boost::shared_ptr<Npc> &npc) : PlayerTargetBase(npc), m_impl(new Impl) {
    m_impl->Init(this, npc);
}

NpcTarget::~NpcTarget() {}

void NpcTarget::vf_0x0C() {
    PlayerTargetBase::vf_0x0C();
    fn_80051A94();
    m_impl->Update();
    fn_80051A94();
}

boost::weak_ptr<Target> NpcTarget::vf_0x28() {
    return m_impl->m_target;
}

} // namespace Npc
} // namespace Player
