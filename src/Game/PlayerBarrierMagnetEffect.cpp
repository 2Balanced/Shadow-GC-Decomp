/* @unnamed@PlayerBarrierMagnetEffect_cpp@::MagnetEffect (class name from RTTI; its task name is
 * "MagnetEffect"): the task of a player's magnet barrier (Player::Barrier kind 3,
 * Game/PlayerBarrier.cpp). Every 1/16 second of the player's frame time it takes the nearest
 * target within 40 units and sends its character a VacuumCommand that pulls it to the player.
 * The file name comes from the RTTI name. Function and member names are inferred. Definitions are
 * in reverse address order (deferred inlining emits functions last-defined first). */
#define BOOST_SP_RESET_POINTER
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerBarrier.h"
#include "Game/TargetSearchList.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* Target (class name from RTTI): a point characters aim at. Only its character is mapped here (as
 * in Game/WeaponVacuumBase.cpp). */
class Target {
  public:
    /* 0x00 */ u8 field_0x0[0x28];
    /* 0x28 */ boost::weak_ptr<CharInfo> m_owner;
};

extern "C" {
/* The larger of a and b (behaviour inferred; see Game/DiablonBehaviorIdle.cpp). */
const float &fn_80070494(const float &a, const float &b);
}

/* A value (name inferred). As a call it is not a constant expression, so the value is stored by
 * the static initializer, as in retail. */
inline float Plain(float value) {
    return value;
}

/* How far the barrier reaches (name inferred). */
static float s_range = Plain(40.0f);

/* The time left never goes below it (name inferred). Retail keeps it in .sdata. */
#pragma explicit_zero_data on
static float s_zero = 0.0f;
#pragma explicit_zero_data reset

namespace {

class MagnetEffect : public Task {
  public:
    MagnetEffect(Task *parent, Player::BarrierImpl *barrier);
    virtual ~MagnetEffect();
    virtual void Tick(float dt);

    /* The target nearest to the player within s_range. */
    boost::weak_ptr<Target> FindTarget();

    /* 0x28 */ boost::shared_ptr<Player::PlayerBase> m_player;
    /* 0x30 */ float m_timer; /* until the next pull */
    /* 0x34 */ u32 field_0x34;
};
SIZE_ASSERT(MagnetEffect, 0x38);

MagnetEffect::MagnetEffect(Task *parent, Player::BarrierImpl *barrier)
    : Task(parent), m_player(barrier->m_player) {
    m_name = "MagnetEffect";
    m_timer = 0.0f;
}

MagnetEffect::~MagnetEffect() {}

void MagnetEffect::Tick(float dt) {
    /* CharInfo's word at 0x60 points at the frame time (see Game/Player.h). */
    float frame = **reinterpret_cast<float **>(m_player->field_0x60);
    if (m_timer > 0.0f) {
        float timer = m_timer - frame;
        m_timer = fn_80070494(timer, s_zero);
    }
    if (m_timer <= 0.0f) {
        boost::weak_ptr<Target> target = FindTarget();
        if (!target.expired()) {
            boost::shared_ptr<Target> locked = target.lock();
            boost::shared_ptr<CharInfo> chara = locked->m_owner.lock();
            if (chara.get() != 0) {
                boost::weak_ptr<CharInfo> owner = m_player;
                chara->PushCommand(new VacuumCommand(locked, owner));
            }
            m_timer = 0.0625f;
        }
    }
}

boost::weak_ptr<Target> MagnetEffect::FindTarget() {
    boost::shared_ptr<TargetSearchList> list;
    list.reset(fn_801E7930(fn_80060058()));
    fn_801E8218(list.get(), &m_player->m_position, s_range);
    fn_801E80C0(list.get(), 0x100);
    return fn_801E848C(list.get(), &m_player->m_position);
}

} // namespace

/* Creates the magnet barrier's task under parent (called through Player::Barrier's kind table). */
extern "C" Task *fn_8034064C(Task *parent, Player::BarrierImpl *barrier) {
    return new MagnetEffect(parent, barrier);
}
