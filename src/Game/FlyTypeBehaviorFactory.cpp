/* Player::Npc::FlyType::FlyTypeBehaviorFactory (class name from RTTI): the behavior factory of the
 * flying NPCs (fn_80296A90 makes one for an NPC). It works as SonicTypeBehaviorFactory does
 * (Game/SonicTypeBehaviorFactory.cpp) with a table of three behaviors: 0 (0x804D30A8), 0x23 and
 * 0x24 (Player::Behavior::FirstMeet). File name from the class; function names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
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
extern const UnkBehaviorDesc_804EC024 lbl_804D30A8;
extern const UnkBehaviorDesc_804EC024 lbl_804D30E8;
extern const UnkBehaviorDesc_804EC024 lbl_804EC024;

extern "C" {
/* Gives a new behavior its weak pointer to itself (behaviour inferred). */
void fn_800AE5D8(Behavior *behavior, const boost::weak_ptr<Behavior> &self);
}

/* A record's hooks as they are called (the record declares them without arguments). */
typedef void (*BehaviorHook)(const shared_ptr<PlayerBase> &player);

/* The behaviors of a flying NPC by number. */
static const UnkBehaviorEntry_804FA558 s_behaviors[] = {
    {0x00, &lbl_804D30A8}, {0x23, &lbl_804D30E8}, {0x24, &lbl_804EC024},
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
namespace FlyType {

class FlyTypeBehaviorFactory : public Behavior::Factory {
  public:
    FlyTypeBehaviorFactory(const shared_ptr<PlayerBase> &player);
    virtual ~FlyTypeBehaviorFactory();
    virtual shared_ptr<Behavior::Behavior> Create(int id);

    /* 0x4 */ shared_ptr<PlayerBase> m_player;
};
SIZE_ASSERT(FlyTypeBehaviorFactory, 0xC);

FlyTypeBehaviorFactory::FlyTypeBehaviorFactory(const shared_ptr<PlayerBase> &player)
    : m_player(player) {
    for (u32 i = 0; i < sizeof(s_behaviors) / sizeof(s_behaviors[0]); i++) {
        const UnkBehaviorDesc_804EC024 *desc = s_behaviors[i].desc;
        if (desc != 0 && desc->fn_0x4 != 0) {
            ((BehaviorHook)desc->fn_0x4)(m_player);
        }
    }
}

FlyTypeBehaviorFactory::~FlyTypeBehaviorFactory() {
    for (u32 i = 0; i < sizeof(s_behaviors) / sizeof(s_behaviors[0]); i++) {
        const UnkBehaviorDesc_804EC024 *desc = s_behaviors[i].desc;
        if (desc != 0 && desc->field_0x8 != 0) {
            ((BehaviorHook)desc->field_0x8)(m_player);
        }
    }
}

shared_ptr<Behavior::Behavior> FlyTypeBehaviorFactory::Create(int id) {
    const UnkBehaviorDesc_804EC024 *desc = FindBehavior(id);
    if (desc != 0 && desc->create != 0) {
        shared_ptr<Behavior::Behavior> behavior(desc->create(m_player));
        fn_800AE5D8(behavior.get(), boost::weak_ptr<Behavior::Behavior>(behavior));
        return behavior;
    }
    return shared_ptr<Behavior::Behavior>();
}

} // namespace FlyType
} // namespace Npc
} // namespace Player

/* Creates the behavior factory of a flying NPC (called by the NPC units; name inferred). */
extern "C" shared_ptr<Player::Behavior::Factory> fn_80296A90(const shared_ptr<Player::Npc::Npc> &npc) {
    return shared_ptr<Player::Behavior::Factory>(new Player::Npc::FlyType::FlyTypeBehaviorFactory(npc));
}
