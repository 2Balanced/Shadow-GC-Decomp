/* Player::Npc::BossSonic::SubControlDodge (class name from RTTI): the task Boss Sonic's AI starts
 * to dodge (mode 5). When the NPC may act, it locks the AI's mode and makes the NPC jump aside:
 * away from the player, across the line to the player, with state bit 0x3C set while it lasts.
 * After two frames it ends once the NPC may act again. The AI keeps the running sub control in a
 * slot, which the task fills and clears. File, member and function names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/BossSonic.h"
#include "Game/CharCommand.h"
#include "Game/MathTypes.h"
#include "Game/Npc.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

extern "C" {
/* Vector helpers (behaviour from their bodies; see Game/VehicleColliMove.cpp). */
void fn_8040CE38(Vec3f *vector, float scale); /* *= */
void fn_8040CE74(Vec3f *vector);              /* normalizes */
float fn_8040CF90(const Vec3f *a, const Vec3f *b); /* the dot product */
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b); /* the cross product */
/* Set and clear a bit of the NPC's state bits (see Game/Player.h). */
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* Whether the character may act now (see Game/BossSonicDesireAttack.cpp). */
bool fn_800B7AD0(const boost::shared_ptr<CharInfo> &chara);
}

/* fn_8040CD74 called as returning a - b by value (see Game/VehicleColliMove.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* AccelerateInputCommand (command type 1) is declared in Game/CharCommand.h. */

namespace Player {
namespace Npc {
namespace BossSonic {

using boost::shared_ptr;

/* Sends the NPC jumping away from the player, across the line to the player (name inferred). */
static void StartDodge(const shared_ptr<BossSonicWork> &work) {
    if (!work->m_target.expired() && !work->m_player.expired()) {
        shared_ptr<Npc> npc = work->m_npc.lock();
        shared_ptr<CharInfo> target = work->m_target.lock();
        shared_ptr<CharInfo> player = work->m_player.lock();
        Vec3f toTarget;
        Vec3f toPlayer;
        Vec3f side;
        fn_800091EC(&toTarget, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&target->m_position, &npc->m_position));
        fn_8040CE74(&toTarget);
        fn_800091EC(&toPlayer, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&player->m_position, &npc->m_position));
        fn_800091EC(&side, &(const Vec3f &)fn_8040CFB8(&toPlayer, &(const Vec3f &)Vec3f(0.0f, 1.0f, 0.0f)));
        fn_8040CE74(&side);
        if (fn_8040CF90(&toPlayer, &side) >= 0.0f) {
            fn_8040CE38(&side, -1.0f);
        }
        fn_800091D0(&npc->field_0x18, &side);
        fn_80076CA8(npc->m_stateBits, 0x3C);
        npc->PushCommand(new AccelerateInputCommand(side, 4));
        npc->PushCommand(new StartJumpCommand(4));
    }
}

/* Clears the NPC's dodge bit (name inferred). */
static void EndDodge(const shared_ptr<BossSonicWork> &work) {
    shared_ptr<Npc> npc = work->m_npc.lock();
    fn_80076CC8(npc->m_stateBits, 0x3C);
}

class SubControlDodge : public Task {
  public:
    SubControlDodge(Task **slot, const shared_ptr<BossSonicWork> &work);
    virtual ~SubControlDodge();
    virtual void Tick(float dt);

    static void Create(Task **slot, const shared_ptr<BossSonicWork> &work);

    /* 0x28 */ Task **m_slot; /* the AI's slot for its running sub control */
    /* 0x2C */ shared_ptr<BossSonicWork> m_work;
    /* 0x34 */ s32 m_wait;
};
SIZE_ASSERT(SubControlDodge, 0x38);

SubControlDodge::SubControlDodge(Task **slot, const shared_ptr<BossSonicWork> &work)
    : Task(work->m_npc.lock()->m_task), m_slot(slot), m_work(work) {
    shared_ptr<Npc> npc = m_work->m_npc.lock();
    m_name = "SubControlDodge";
    if (m_slot != 0 && *m_slot == 0) {
        *m_slot = this;
    }
    bool ready = fn_800B7AD0(shared_ptr<CharInfo>(npc));
    if (!ready) {
        m_flags |= 1;
        return;
    }
    m_work->m_mode.Lock();
    StartDodge(m_work);
    m_wait = 0;
}

SubControlDodge::~SubControlDodge() {
    EndDodge(m_work);
    m_work->m_mode.Unlock();
    if (m_slot != 0 && *m_slot == this) {
        *m_slot = 0;
    }
}

void SubControlDodge::Tick(float dt) {
    shared_ptr<Npc> npc = m_work->m_npc.lock();
    switch (m_wait) {
    case 0:
    case 1:
        m_wait++;
        break;
    default:
        bool ready = fn_800B7AD0(shared_ptr<CharInfo>(npc));
        if (ready) {
            m_flags |= 1;
        }
        break;
    }
}

void SubControlDodge::Create(Task **slot, const shared_ptr<BossSonicWork> &work) {
    new SubControlDodge(slot, work);
}

} // namespace BossSonic
} // namespace Npc
} // namespace Player
