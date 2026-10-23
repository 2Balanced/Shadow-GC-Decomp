/* Player::Barrier (class name from RTTI; its task name is "Barrier"): the shields a player character
 * can put up, kept at 0x3C of the player's weapon state. Kinds 1 to 3 put up a barrier with its own
 * effect (effect 0x22E for kind 1, the heat barrier (0x238) for kind 2 and the magnet barrier
 * (0x231) for kind 3; the last two also start their task, in Game/PlayerBarrierHeatEffect.cpp and
 * Game/PlayerBarrierMagnetEffect.cpp); kind 4 is a guard of 15 seconds with effect 0x26D. The work
 * is done by an implementation object behind a boost::scoped_ptr. The file name comes from the
 * RTTI names of the effect classes (@unnamed@PlayerBarrierHeatEffect_cpp@...). Function and
 * member names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include "Effect/System/EffectElement.h"
#include "Game/GadgetUtil.h"
#include "Game/Player.h"
#include "Game/PlayerBarrier.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/Task.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Effect::System::EffectElement;

extern "C" {
/* A 32-bit flag word (see Game/BossSonic.h): construct it cleared, set, test and clear a bit. */
void fn_80014E04(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
/* Test, set and clear a bit of the player's state bits (see Game/Player.h). */
bool fn_8006CD78(u32 *bits, int bit);
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* Switch two of the player's guard states on and off (behaviour inferred). */
void fn_8007939C(Player::PlayerBase *player, int on);
void fn_80079334(Player::PlayerBase *player, int on);
/* Starts effect id from the player's effect list (behaviour inferred; the meaning of arg is
 * unknown). */
boost::weak_ptr<EffectElement> fn_800A2854(void *effects, int arg, int id);
/* Start and stop sound channel 5's guard loop (behaviour inferred). */
void fn_801D3888(int channel);
void fn_801D3838(int channel);
}

namespace Player {

/* The two effects the barrier drives, kept by the player's motion (offsets 0x30 and 0x38; names
 * inferred). */
inline boost::weak_ptr<EffectElement> &BarrierEffect(PlayerMotion *motion) {
    return *reinterpret_cast<boost::weak_ptr<EffectElement> *>(motion->field_0x28 + (0x30 - 0x28));
}
inline boost::weak_ptr<EffectElement> &GuardEffect(PlayerMotion *motion) {
    return *reinterpret_cast<boost::weak_ptr<EffectElement> *>(motion->field_0x28 + (0x38 - 0x28));
}

/* What a barrier kind shows (struct name unknown): its effect (-1: none) and the function that
 * starts its task (null: none). */
struct BarrierKind {
    /* 0x0 */ s32 effect;
    /* 0x4 */ Task *(*create)(Task *parent, BarrierImpl *barrier);
};

static const BarrierKind lbl_80510B28[4] = {
    {-1, 0},
    {0x22E, 0},
    {0x238, fn_8033FBA0},
    {0x231, fn_8034064C},
};

BarrierImpl::BarrierImpl(const boost::shared_ptr<PlayerBase> &player) : m_player(player) {
    fn_80014E04(&m_flags);
    m_task = 0;
    m_kind = 0;
    m_guardTime = 0.0f;
}

BarrierImpl::~BarrierImpl() {
    m_task = 0;
    RemoveEffect();
    EndGuard();
}

void BarrierImpl::Update(float dt) {
    if (fn_80014E34(&m_flags, 0)) {
        m_guardTime -= dt;
        if (m_guardTime <= 0.0f) {
            EndGuard();
        }
        if (fn_8006CD78(m_player->m_stateBits, 0x22) || fn_8006CD78(m_player->m_stateBits, 0x23)) {
            fn_80014E34(&m_flags, 1);
        } else if (fn_80014E34(&m_flags, 1)) {
            EndGuard();
        }
    }
    if (PlayerMotion *motion = m_player->m_motion) {
        boost::weak_ptr<EffectElement> &effect = BarrierEffect(motion);
        if (!effect.expired()) {
            if (m_player->IsWeaponHidden()) {
                effect.lock()->Hide();
            } else {
                effect.lock()->Show();
            }
        }
    }
}

void BarrierImpl::Set(Task *parent, int kind) {
    switch (kind) {
    case 1:
        Start(parent, 1);
        break;
    case 2:
        Start(parent, 2);
        break;
    case 3:
        Start(parent, 3);
        break;
    case 4:
        StartGuard();
        break;
    }
}

void BarrierImpl::Off() {
    if (fn_8006CD78(m_player->m_stateBits, 0x20)) {
        Remove();
    }
}

void BarrierImpl::Cancel() {
    if (fn_8006CD78(m_player->m_stateBits, 0x20)) {
        RemoveEffect();
    }
}

void BarrierImpl::Start(Task *parent, int kind) {
    if (m_player->m_motion != 0) {
        if (m_kind != kind) {
            Remove();
            m_kind = kind;
            fn_80076CA8(m_player->m_stateBits, 0x20);
            if (lbl_80510B28[m_kind].effect >= 0) {
                if (PlayerMotion *motion = m_player->m_motion) {
                    boost::weak_ptr<EffectElement> &effect = BarrierEffect(motion);
                    if (!effect.expired()) {
                        effect.lock()->Retire();
                    }
                    effect = fn_800A2854(reinterpret_cast<void *>(m_player->field_0x23C), 1,
                                         lbl_80510B28[m_kind].effect);
                }
            }
            if (lbl_80510B28[m_kind].create != 0) {
                m_task = lbl_80510B28[m_kind].create(parent, this);
            }
        }
        fn_801D560C(fn_80053164(), 0xE01E, 0, 0);
    }
}

void BarrierImpl::Remove() {
    if (PlayerMotion *motion = m_player->m_motion) {
        boost::weak_ptr<EffectElement> &effect = BarrierEffect(motion);
        if (!effect.expired()) {
            effect.lock()->Kill();
        }
    }
    if (m_task != 0) {
        m_task->m_flags |= 1;
        m_task = 0;
    }
    fn_80076CC8(m_player->m_stateBits, 0x20);
    m_kind = 0;
}

void BarrierImpl::RemoveEffect() {
    if (PlayerMotion *motion = m_player->m_motion) {
        boost::weak_ptr<EffectElement> &effect = BarrierEffect(motion);
        if (!effect.expired()) {
            effect.lock()->Retire();
        }
    }
    Remove();
}

void BarrierImpl::StartGuard() {
    if (m_player->m_motion != 0) {
        if (!fn_80014E34(&m_flags, 0)) {
            fn_80014E1C(&m_flags, 0);
            fn_8007939C(m_player.get(), 1);
            fn_80079334(m_player.get(), 1);
            if (!GuardEffect(m_player->m_motion).expired()) {
                GuardEffect(m_player->m_motion).lock()->Retire();
            }
            GuardEffect(m_player->m_motion) =
                fn_800A2854(reinterpret_cast<void *>(m_player->field_0x23C), 1, 0x26D);
            if (!fn_8006CD78(m_player->m_stateBits, 0x22) &&
                !fn_8006CD78(m_player->m_stateBits, 0x23)) {
                fn_801D3888(5);
            }
        }
    }
    m_guardTime = 15.0f;
}

void BarrierImpl::EndGuard() {
    if (m_player->m_motion != 0) {
        if (!GuardEffect(m_player->m_motion).expired()) {
            GuardEffect(m_player->m_motion).lock()->Kill();
        }
    }
    if (fn_80014E34(&m_flags, 0)) {
        fn_8006127C(&m_flags, 0);
        fn_8006127C(&m_flags, 1);
        m_guardTime = 0.0f;
        fn_8007939C(m_player.get(), 0);
        fn_80079334(m_player.get(), 0);
        fn_801D3838(5);
    }
}

/* Deletes the barrier's state (name inferred; see checked_delete below). */
inline void DeleteBarrierImpl(BarrierImpl *impl) {
    delete impl;
}

} // namespace Player

/* Retail keeps boost::checked_delete<BarrierImpl> out of line (0x8033DE0C) and ~BarrierImpl at its
 * own place (0x8033E594); this specialization calls an inline helper, which gives retail's code and
 * order (see docs/research-notes.md and Game/Rocket.cpp). */
namespace boost {
template<> inline void checked_delete<Player::BarrierImpl>(Player::BarrierImpl *x) {
    Player::DeleteBarrierImpl(x);
}
} // namespace boost

namespace Player {

/* Barrier (class name from RTTI): the task the player keeps for its barrier. */
class Barrier : public Task {
  public:
    Barrier(Task *parent, const boost::shared_ptr<PlayerBase> &player);
    virtual ~Barrier();
    virtual void Tick(float dt);

    void Cancel();
    void Off();
    void Set(int kind);

    /* 0x28 */ boost::scoped_ptr<BarrierImpl> m_impl;
    /* 0x2C */ u32 field_0x2C;
};
SIZE_ASSERT(Barrier, 0x30);

Barrier::Barrier(Task *parent, const boost::shared_ptr<PlayerBase> &player)
    : Task(parent), m_impl(new BarrierImpl(player)) {
    m_name = "Barrier";
}

Barrier::~Barrier() {}

void Barrier::Tick(float dt) {
    m_impl->Update(dt);
}

void Barrier::Set(int kind) {
    m_impl->Set(this, kind);
}

void Barrier::Off() {
    m_impl->Off();
}

void Barrier::Cancel() {
    m_impl->Cancel();
}

} // namespace Player
