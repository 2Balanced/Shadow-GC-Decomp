#ifndef GAME_PLAYERSHADOWMOTIONSUB_H
#define GAME_PLAYERSHADOWMOTIONSUB_H

#include "Game/Task.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>

namespace Player {

class PlayerBase;
/* What a motion's vf_0x14 hands the clumps to (see Game/PlayerWeaponBase.h). */
class UnkClumpList_8006CAE4;

namespace Shadow {

class MotionSubImpl;

/* MotionSub (class name from RTTI; vtable at 0x8055F3B4; Game/PlayerShadowMotionSub.cpp): the
 * task that carries up to three extra models on the nodes of a player's skeleton, for the
 * character types 0x10 to 0x13 (meaning inferred). The player's motion keeps it at its word 0x7E8
 * and shows, hides and hands on its models with its own. 0x30 bytes. Method names are inferred. */
class MotionSub : public Task {
  public:
    /* A MotionSub for player, or null when its character type has no extra models (0x8034F890). */
    static MotionSub *Create(Task *parent, const boost::shared_ptr<PlayerBase> &player);

    MotionSub(Task *parent, const boost::shared_ptr<PlayerBase> &player, int kind);
    virtual ~MotionSub();
    virtual void Tick(float dt);
    /* Adds the models to the world (0x8034F6E0) and takes them out again (0x8034F66C). */
    void Show();
    void Hide();
    /* Hands the models to a clump list (0x8034F704). */
    void AddClumps(UnkClumpList_8006CAE4 *list);
    /* Empty (0x8034F728; meaning unknown). */
    void Refresh();

    /* 0x28 */ boost::scoped_ptr<MotionSubImpl> m_impl;
    /* 0x2C */ u32 pad_0x2C; /* the class size is a multiple of 8 */
};

} // namespace Shadow
} // namespace Player

#endif
