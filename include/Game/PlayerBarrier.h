#ifndef GAME_PLAYERBARRIER_H
#define GAME_PLAYERBARRIER_H

#include "Game/Player.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

namespace Player {

/* The state of a player's barrier (class name unknown; Game/PlayerBarrier.cpp, behind
 * Player::Barrier's boost::scoped_ptr). The heat and magnet barriers' tasks
 * (Game/PlayerBarrierHeatEffect.cpp, Game/PlayerBarrierMagnetEffect.cpp) are created with it and
 * take its player. Method and member names are inferred. */
class BarrierImpl {
  public:
    BarrierImpl(const boost::shared_ptr<PlayerBase> &player);
    ~BarrierImpl();

    void EndGuard();
    void StartGuard();
    void Remove();
    void RemoveEffect();
    void Start(Task *parent, int kind);
    void Off();
    void Cancel();
    void Set(Task *parent, int kind);
    void Update(float dt);

    /* 0x00 */ boost::shared_ptr<PlayerBase> m_player;
    /* 0x08 */ Task *m_task; /* the heat or magnet barrier's task */
    /* 0x0C */ u32 m_flags;  /* bit 0: guarding, bit 1 (meaning unknown) */
    /* 0x10 */ s32 m_kind;   /* the barrier up (0: none) */
    /* 0x14 */ float m_guardTime;
};
SIZE_ASSERT(BarrierImpl, 0x18);

} // namespace Player

extern "C" {
/* Create the heat and the magnet barrier's task under parent (Game/PlayerBarrierHeatEffect.cpp and
 * Game/PlayerBarrierMagnetEffect.cpp). */
Task *fn_8033FBA0(Task *parent, Player::BarrierImpl *barrier);
Task *fn_8034064C(Task *parent, Player::BarrierImpl *barrier);
}

#endif
