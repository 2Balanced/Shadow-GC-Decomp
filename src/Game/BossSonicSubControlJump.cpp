/* Player::Npc::BossSonic::SubControlJump (class name from RTTI): the task Boss Sonic's AI starts
 * to jump. It locks the AI's mode while it runs and makes the NPC jump (or ends at once when it
 * cannot); with a target it waits two frames and, once the NPC may attack, sends it a homing
 * attack at the target; it ends when the NPC lands. The AI keeps the running sub control in a
 * slot, which the task fills and clears. File, member and function names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/BossSonic.h"
#include "Game/CharCommand.h"
#include "Game/Npc.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

extern "C" {
/* Tests a bit of the NPC's state bits (see Game/Player.h). */
bool fn_8006CD78(u32 *bits, int bit);
/* Whether the player character may attack now (behaviour inferred). */
bool fn_800B7B58(const boost::shared_ptr<Player::PlayerBase> &player);
}

namespace Player {

/* HomingAttackCommand (class name from RTTI; vtable at 0x80521444): sends a character to attack
 * target (the meaning of arg1 is unknown). */
class HomingAttackCommand : public PlayerCommand {
  public:
    HomingAttackCommand(const boost::weak_ptr<Target> &target, int arg1);

    /* 0x14 */ boost::weak_ptr<Target> m_target;
};
SIZE_ASSERT(HomingAttackCommand, 0x1C);

namespace Npc {
namespace BossSonic {

class SubControlJump : public Task {
  public:
    SubControlJump(Task **slot, const boost::shared_ptr<BossSonicWork> &work,
                   const boost::weak_ptr<Target> &target);
    SubControlJump(Task **slot, const boost::shared_ptr<BossSonicWork> &work);
    virtual ~SubControlJump();
    virtual void Tick(float dt);

    void Begin();
    void End();

    static void Create(Task **slot, const boost::shared_ptr<BossSonicWork> &work,
                       const boost::weak_ptr<Target> &target);
    static void Create(Task **slot, const boost::shared_ptr<BossSonicWork> &work);

    /* 0x28 */ Task **m_slot; /* the AI's slot for its running sub control */
    /* 0x2C */ boost::shared_ptr<BossSonicWork> m_work;
    /* 0x34 */ boost::weak_ptr<Target> m_target;
    /* 0x3C */ s32 m_wait; /* frames waited before the homing attack */
};
SIZE_ASSERT(SubControlJump, 0x40);

SubControlJump::SubControlJump(Task **slot, const boost::shared_ptr<BossSonicWork> &work)
    : Task(work->m_npc.lock()->m_task), m_slot(slot), m_work(work) {
    Begin();
    m_wait = 0;
}

SubControlJump::SubControlJump(Task **slot, const boost::shared_ptr<BossSonicWork> &work,
                               const boost::weak_ptr<Target> &target)
    : Task(work->m_npc.lock()->m_task), m_slot(slot), m_work(work), m_target(target) {
    Begin();
    m_wait = 0;
}

SubControlJump::~SubControlJump() {
    End();
}

void SubControlJump::Tick(float dt) {
    boost::shared_ptr<Npc> npc = m_work->m_npc.lock();
    switch (m_wait) {
    case 0:
    case 1:
        m_wait++;
        break;
    default:
        if (fn_8006CD78(npc->m_stateBits, 2)) {
            m_flags |= 1;
            return;
        }
        if (!m_target.expired()) {
            bool ready = fn_800B7B58(boost::shared_ptr<PlayerBase>(npc));
            if (ready) {
                npc->PushCommand(new HomingAttackCommand(m_target, 4));
                m_flags |= 1;
                return;
            }
        }
        break;
    }
}

void SubControlJump::Begin() {
    boost::shared_ptr<Npc> npc = m_work->m_npc.lock();
    m_name = "SubControlJump";
    if (m_slot != 0 && *m_slot == 0) {
        *m_slot = this;
    }
    m_work->m_mode.Lock();
    if (!fn_80014E34(&npc->m_flags, 2)) {
        if (fn_80014E34(&npc->m_flags, 1)) {
            npc->PushCommand(new StartJumpCommand(4));
        } else {
            m_flags |= 1;
        }
    }
}

void SubControlJump::End() {
    m_work->m_mode.Unlock();
    if (m_slot != 0 && *m_slot == this) {
        *m_slot = 0;
    }
}

void SubControlJump::Create(Task **slot, const boost::shared_ptr<BossSonicWork> &work) {
    new SubControlJump(slot, work);
}

void SubControlJump::Create(Task **slot, const boost::shared_ptr<BossSonicWork> &work,
                            const boost::weak_ptr<Target> &target) {
    new SubControlJump(slot, work, target);
}

} // namespace BossSonic
} // namespace Npc
} // namespace Player
