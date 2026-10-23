/* Player::Npc::SonicType::SonicTypeBehaviorFactory (class name from RTTI): the behavior factory of
 * the Sonic-type NPCs and of Boss Sonic (fn_80295DE4 makes one for an NPC). Its table maps each
 * behavior number to the record of the behavior's unit (the record's create function makes the
 * behavior); the constructor and destructor call every record's two other hooks with the player.
 * File name from the class; function names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include "Game/Npc.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerFactory.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using boost::shared_ptr;
using Player::PlayerBase;
using Player::Behavior::Behavior;
using Player::Behavior::UnkBehaviorDesc_804EC024;
using Player::Behavior::UnkBehaviorEntry_804FA558;

/* The behaviors' records (in their own units; Game/PlayerBehaviorFirstMeet.cpp defines
 * lbl_804EC024). */
extern const UnkBehaviorDesc_804EC024 lbl_804B0518;
extern const UnkBehaviorDesc_804EC024 lbl_804B0000;
extern const UnkBehaviorDesc_804EC024 lbl_804B0694;
extern const UnkBehaviorDesc_804EC024 lbl_804B07A4;
extern const UnkBehaviorDesc_804EC024 lbl_804AFDD0;
extern const UnkBehaviorDesc_804EC024 lbl_804B0858;
extern const UnkBehaviorDesc_804EC024 lbl_804B0988;
extern const UnkBehaviorDesc_804EC024 lbl_804AFD74;
extern const UnkBehaviorDesc_804EC024 lbl_804B08A0;
extern const UnkBehaviorDesc_804EC024 lbl_804AFA7C;
extern const UnkBehaviorDesc_804EC024 lbl_804B011C;
extern const UnkBehaviorDesc_804EC024 lbl_804B03E0;
extern const UnkBehaviorDesc_804EC024 lbl_804B04CC;
extern const UnkBehaviorDesc_804EC024 lbl_804B0D08;
extern const UnkBehaviorDesc_804EC024 lbl_804D30E8;
extern const UnkBehaviorDesc_804EC024 lbl_804EC024;

extern "C" {
/* Gives a new behavior its weak pointer to itself (behaviour inferred). */
void fn_800AE5D8(Behavior *behavior, const boost::weak_ptr<Behavior> &self);
}

/* A record's hooks as they are called (the record declares them without arguments). */
typedef void (*BehaviorHook)(const shared_ptr<PlayerBase> &player);

/* The behaviors of a Sonic-type NPC by number. */
static const UnkBehaviorEntry_804FA558 s_behaviors[] = {
    {0x00, &lbl_804B0518}, {0x01, &lbl_804B0000}, {0x05, &lbl_804B0694}, {0x06, &lbl_804B07A4},
    {0x02, &lbl_804AFDD0}, {0x03, &lbl_804B0858}, {0x04, &lbl_804B0988}, {0x20, &lbl_804AFD74},
    {0x17, &lbl_804B08A0}, {0x1E, &lbl_804AFA7C}, {0x19, &lbl_804B011C}, {0x08, &lbl_804B03E0},
    {0x09, &lbl_804B04CC}, {0x07, &lbl_804B0D08}, {0x23, &lbl_804D30E8}, {0x24, &lbl_804EC024},
    {0x25, 0},
};

/* The record of behavior id, or none. */
inline const UnkBehaviorDesc_804EC024 *FindBehavior(int id) {
    for (const UnkBehaviorEntry_804FA558 *entry = s_behaviors; entry->id < 0x25; entry++) {
        if (entry->id == id) {
            return entry->desc;
        }
    }
    return 0;
}

namespace Player {
namespace Npc {
namespace SonicType {

class SonicTypeBehaviorFactory : public Behavior::Factory {
  public:
    SonicTypeBehaviorFactory(const shared_ptr<PlayerBase> &player);
    virtual ~SonicTypeBehaviorFactory();
    virtual shared_ptr<Behavior::Behavior> Create(int id);

    /* 0x4 */ shared_ptr<PlayerBase> m_player;
};
SIZE_ASSERT(SonicTypeBehaviorFactory, 0xC);

SonicTypeBehaviorFactory::SonicTypeBehaviorFactory(const shared_ptr<PlayerBase> &player)
    : m_player(player) {
    for (u32 i = 0; i < sizeof(s_behaviors) / sizeof(s_behaviors[0]); i++) {
        const UnkBehaviorDesc_804EC024 *desc = s_behaviors[i].desc;
        if (desc != 0 && desc->fn_0x4 != 0) {
            ((BehaviorHook)desc->fn_0x4)(m_player);
        }
    }
}

SonicTypeBehaviorFactory::~SonicTypeBehaviorFactory() {
    for (u32 i = 0; i < sizeof(s_behaviors) / sizeof(s_behaviors[0]); i++) {
        const UnkBehaviorDesc_804EC024 *desc = s_behaviors[i].desc;
        if (desc != 0 && desc->field_0x8 != 0) {
            ((BehaviorHook)desc->field_0x8)(m_player);
        }
    }
}

shared_ptr<Behavior::Behavior> SonicTypeBehaviorFactory::Create(int id) {
    const UnkBehaviorDesc_804EC024 *desc = FindBehavior(id);
    if (desc != 0 && desc->create != 0) {
        shared_ptr<Behavior::Behavior> behavior(desc->create(m_player));
        fn_800AE5D8(behavior.get(), boost::weak_ptr<Behavior::Behavior>(behavior));
        return behavior;
    }
    return shared_ptr<Behavior::Behavior>();
}

} // namespace SonicType
} // namespace Npc
} // namespace Player

/* Creates the behavior factory of a Sonic-type NPC (called by the NPC units and
 * Game/BossSonic.cpp; name inferred). */
extern "C" shared_ptr<Player::Behavior::Factory> fn_80295DE4(const shared_ptr<Player::Npc::Npc> &npc) {
    return shared_ptr<Player::Behavior::Factory>(new Player::Npc::SonicType::SonicTypeBehaviorFactory(npc));
}
