#ifndef GAME_NPCHAND_H
#define GAME_NPCHAND_H

#include "Game/Npc.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"
#ifdef NPCHAND_IMPLEMENTATION
#include "boost/scoped_ptr.hpp"
#endif

namespace Player {

class UnkClumpList_8006CAE4;

namespace Npc {

/* What NpcHand does its work with (Game/NpcHand.cpp; class name unknown). */
class UnkNpcHandImpl_80071508;

/* NpcHand (class name from RTTI; base Task; vtable 0x8051FD84; constructor 0x80070DF0): the hand
 * models of a non-player character, which follow the motion that creates them. Each method passes
 * on to the object at 0x28 (method names and meanings inferred from Player::Npc::Sonic::MotionImpl,
 * Game/NpcSonicMotion.cpp). */
class NpcHand : public Task {
  public:
    NpcHand(Task *parent, const boost::shared_ptr<Npc> &npc);
    virtual ~NpcHand();
    virtual void Tick(float dt);

    /* Loads count hand models from the model resource (parts: their indices in it). */
    void Load(void *model, const int *parts, int count, void *skeleton);
    void Hide();
    void Show();
    /* Selects the hands' pose for a motion. */
    void SetPose(int pose);
    /* Hands the hand models to a clump list. */
    void AddClumps(UnkClumpList_8006CAE4 *list);

#ifdef NPCHAND_IMPLEMENTATION
    /* 0x28 */ boost::scoped_ptr<UnkNpcHandImpl_80071508> m_impl;
#else
    /* 0x28 */ void *m_impl; /* owned (a boost::scoped_ptr in Game/NpcHand.cpp) */
#endif
    /* 0x2C */ u32 field_0x2C;
};

} // namespace Npc
} // namespace Player

SIZE_ASSERT(Player::Npc::NpcHand, 0x30);

#endif
