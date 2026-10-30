/* Player::Shadow::Electric (class name from RTTI; its task name is "Electric"): the electric aura
 * effect 0x1D6 on Shadow (PlayerEffect starts it through the constructor and ends it through
 * Stop). While it lives it sets the player's state bit 0x26 and keeps the effect on the player's
 * motion node 0; it fades in over 0.3 seconds and, once stopped, fades out the same way before
 * the task ends. It keeps its state in an Impl behind a boost::scoped_ptr and clears the slot of
 * its owner that points at it when it goes. File name inferred from the class. Names other than
 * the class name are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include "Game/Player.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/Task.h"
#include "Effect/System/EffectElement.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Effect::System::EffectElement;

extern "C" {
/* Set and clear a bit of the player's state bits (see Game/Player.h). */
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* The player's motion node index (see Game/PlayerBarrierHeatEffect.cpp), and a motion setting
 * (behaviour unknown). */
Player::UnkMotionNode_800AA760 *fn_800AA760(Player::PlayerMotion *motion, int index);
void fn_800AA58C(Player::PlayerMotion *motion, float value);
/* Starts effect id among the player's effects (see Game/PlayerBehaviorDead.cpp). */
boost::weak_ptr<EffectElement> fn_800A2754(void *effects, int id);
/* Makes the effect follow matrix (behaviour inferred). */
void fn_80425E68(EffectElement *effect, const Mtx34f *matrix);
/* The larger and the smaller of two floats (see Game/CameraModeGeneral.cpp). */
const float &fn_80070494(const float &a, const float &b);
const float &fn_800704AC(const float &a, const float &b);
}

/* The bounds of the aura's level (retail keeps them in .sdata). */
#pragma explicit_zero_data on
static float s_levelMax = 1.0f;
static float s_levelMin = 0.0f;
#pragma explicit_zero_data reset

namespace Player {
namespace Shadow {

class Electric : public Task {
  public:
    class Impl;

    Electric(Task *parent, const boost::shared_ptr<PlayerBase> &player, Electric **slot);
    virtual ~Electric();
    virtual void Tick(float dt);

    /* Ends the aura: it fades out, and the task ends with it. */
    void Stop();

    /* 0x28 */ boost::scoped_ptr<Impl> m_impl;
    /* 0x2C */ u32 field_0x2C; /* (the constructor leaves it alone; meaning unknown) */
};
SIZE_ASSERT(Electric, 0x30);

class Electric::Impl {
  public:
    Impl(const boost::shared_ptr<PlayerBase> &player);
    ~Impl();

    /* Advances the aura; false once it has faded out. */
    bool Update(float dt);
    bool FadeOut(float dt);
    void StartEffect();
    void StopEffect();
    void UpdateEffect();

    /* 0x00 */ boost::weak_ptr<EffectElement> m_effect;
    /* 0x08 */ Electric **m_slot; /* the owner's pointer to the task */
    /* 0x0C */ boost::shared_ptr<PlayerBase> m_player;
    /* 0x14 */ float m_level;
    /* 0x18 */ s32 m_state; /* 0 starting, 1 on, 2 fading out */
};
SIZE_ASSERT(Electric::Impl, 0x1C);

/* Deletes the state (name inferred; see checked_delete below). */
inline void DeleteImpl(Electric::Impl *impl) {
    delete impl;
}

} // namespace Shadow
} // namespace Player

/* Retail keeps boost::checked_delete<Impl> out of line and ~Impl at its own place; this
 * specialization calls an inline helper, which gives retail's code and order (as in
 * Game/BirdModelBase.cpp; see docs/research-notes.md). */
namespace boost {
template <>
inline void checked_delete<Player::Shadow::Electric::Impl>(Player::Shadow::Electric::Impl *x) {
    Player::Shadow::DeleteImpl(x);
}
} // namespace boost

namespace Player {
namespace Shadow {

Electric::Impl::Impl(const boost::shared_ptr<PlayerBase> &player) : m_player(player) {
    m_level = 0.0f;
    StartEffect();
    m_player->m_motion->vf_0x10(0xC, 5.0f);
    m_state = 0;
}

Electric::Impl::~Impl() {
    StopEffect();
    fn_80076CC8(m_player->m_stateBits, 0x26);
    if (m_player->m_motion != 0) {
        fn_800AA58C(m_player->m_motion, 1.0f);
    }
}

bool Electric::Impl::Update(float dt) {
    if (m_state != 2) {
        fn_80076CA8(m_player->m_stateBits, 0x26);
    }
    switch (m_state) {
    case 0:
        m_state = 1;
        break;
    case 1:
        m_level += dt / 0.3f;
        m_level = fn_800704AC(m_level, s_levelMax);
        break;
    case 2:
        return FadeOut(dt);
    }
    UpdateEffect();
    return true;
}

bool Electric::Impl::FadeOut(float dt) {
    m_level -= dt / 0.3f;
    m_level = fn_80070494(m_level, s_levelMin);
    if (m_effect.expired() && m_level <= 0.0f) {
        return false;
    }
    return true;
}

void Electric::Impl::StartEffect() {
    if (!m_effect.expired()) {
        m_effect.lock()->Retire();
    }
    m_effect = fn_800A2754(reinterpret_cast<void *>(m_player->field_0x23C), 0x1D6);
}

void Electric::Impl::StopEffect() {
    if (!m_effect.expired()) {
        m_effect.lock()->Stop();
    }
}

void Electric::Impl::UpdateEffect() {
    if (!m_effect.expired() && m_player->m_motion != 0) {
        UnkMotionNode_800AA760 *node = fn_800AA760(m_player->m_motion, 0);
        if (node != 0 && node->m_matrix != 0) {
            fn_80425E68(m_effect.lock().get(), node->m_matrix);
        }
    }
}

Electric::Electric(Task *parent, const boost::shared_ptr<PlayerBase> &player, Electric **slot)
    : Task(parent), m_impl(new Impl(player)) {
    m_name = "Electric";
    m_impl->m_slot = slot;
    if (m_impl->m_slot != 0) {
        *m_impl->m_slot = this;
    }
}

Electric::~Electric() {
    if (m_impl->m_slot != 0) {
        *m_impl->m_slot = 0;
    }
}

void Electric::Tick(float dt) {
    if (!m_impl->Update(dt)) {
        m_flags |= 1;
    }
}

void Electric::Stop() {
    if (m_impl->m_slot != 0) {
        *m_impl->m_slot = 0;
        m_impl->m_slot = 0;
    }
    fn_80076CC8(m_impl->m_player->m_stateBits, 0x26);
    m_impl->m_state = 2;
    m_impl->StopEffect();
}

} // namespace Shadow
} // namespace Player
