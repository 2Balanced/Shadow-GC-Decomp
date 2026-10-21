#ifndef GAME_NPCPARTMODEL_H
#define GAME_NPCPARTMODEL_H

#include "Game/Npc.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"

/* What the NPC part tasks (Player::Npc::Tails::Tail in Game/NpcTailsTail.cpp and
 * Player::Npc::Bee::Feather in Game/NpcBeeFeather.cpp) read from their NPC to place a model on a
 * skeleton node. Struct names are unknown and behaviour is inferred from the callers. */

/* A node of a character's skeleton (0x38 bytes; only the matrix pointer is read). */
struct UnkSkeletonNode_800AA760 {
    /* 0x0 */ const Mtx34f *m_matrix;
};

/* The fields of an NPC read by the part tasks (the NPC class's layout is not mapped). */
struct UnkNpcFields_8032D0FC {
    /* 0x000 */ u8 field_0x0[0x230];
    /* 0x230 */ void *m_skeleton;
};

extern "C" {
/* The NPC's model resource. */
void *fn_80078C4C(Player::Npc::Npc *npc);
/* The NPC's scale and its world matrix. */
float fn_80073908(Player::Npc::Npc *npc);
Mtx34f fn_80073844(Player::Npc::Npc *npc);
/* Node index of a skeleton. */
UnkSkeletonNode_800AA760 *fn_800AA760(void *skeleton, int index);
/* The NPC pointer's out-of-line operator-> (an instance of boost::shared_ptr<Npc>::operator->;
 * the part tasks call it as retail does). */
Player::Npc::Npc *fn_8006CA70(const boost::shared_ptr<Player::Npc::Npc> *npc);
}

#endif
