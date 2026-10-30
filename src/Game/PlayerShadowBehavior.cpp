/* Player::Shadow::ShadowBehavior (class name from RTTI): Shadow's behavior factory
 * (fn_800AE330 makes one for him, Game/PlayerShadow.cpp). Its table holds the record of every
 * behavior number (the record's create function makes the behavior); the constructor and
 * destructor call every record's two other hooks with the player. While the player's state bit
 * 0x25 is set, behaviors 0, 2 and 3 become 0x22. The NPC factories (Game/SonicTypeBehaviorFactory.cpp,
 * Game/FlyTypeBehaviorFactory.cpp) follow the same pattern and call fn_800AE5D8 here. File name
 * from the class; function names are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
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

/* The behaviors' records (in their own units). */
extern const UnkBehaviorDesc_804EC024 lbl_804B0518;
extern const UnkBehaviorDesc_804EC024 lbl_804B0000;
extern const UnkBehaviorDesc_804EC024 lbl_804AFDD0;
extern const UnkBehaviorDesc_804EC024 lbl_804B0858;
extern const UnkBehaviorDesc_804EC024 lbl_804B0988;
extern const UnkBehaviorDesc_804EC024 lbl_804B0694;
extern const UnkBehaviorDesc_804EC024 lbl_804B07A4;
extern const UnkBehaviorDesc_804EC024 lbl_804B0D08;
extern const UnkBehaviorDesc_804EC024 lbl_804B03E0;
extern const UnkBehaviorDesc_804EC024 lbl_804B04CC;
extern const UnkBehaviorDesc_804EC024 lbl_804B0F44;
extern const UnkBehaviorDesc_804EC024 lbl_804B10B8;
extern const UnkBehaviorDesc_804EC024 lbl_804AFCBC;
extern const UnkBehaviorDesc_804EC024 lbl_804B1234;
extern const UnkBehaviorDesc_804EC024 lbl_804B0944;
extern const UnkBehaviorDesc_804EC024 lbl_804B0D60;
extern const UnkBehaviorDesc_804EC024 lbl_804B13DC;
extern const UnkBehaviorDesc_804EC024 lbl_804AFF64;
extern const UnkBehaviorDesc_804EC024 lbl_804B0DB4;
extern const UnkBehaviorDesc_804EC024 lbl_804AFACC;
extern const UnkBehaviorDesc_804EC024 lbl_804B08F4;
extern const UnkBehaviorDesc_804EC024 lbl_804B0AF8;
extern const UnkBehaviorDesc_804EC024 lbl_804B0C90;
extern const UnkBehaviorDesc_804EC024 lbl_804B08A0;
extern const UnkBehaviorDesc_804EC024 lbl_804B0DF8;
extern const UnkBehaviorDesc_804EC024 lbl_804B011C;
extern const UnkBehaviorDesc_804EC024 lbl_804AF784;
extern const UnkBehaviorDesc_804EC024 lbl_804AFB60;
extern const UnkBehaviorDesc_804EC024 lbl_804AF894;
extern const UnkBehaviorDesc_804EC024 lbl_804AF9CC;
extern const UnkBehaviorDesc_804EC024 lbl_804AFA7C;
extern const UnkBehaviorDesc_804EC024 lbl_804AFB10;
extern const UnkBehaviorDesc_804EC024 lbl_804AFD74;
extern const UnkBehaviorDesc_804EC024 lbl_804AF420;
extern const UnkBehaviorDesc_804EC024 lbl_80507B50;

extern "C" {
/* Tests a bit of the player's state bits. */
bool fn_8006CD78(u32 *bits, int bit);
}

/* Gives a new behavior its weak pointer to itself (name inferred). An inline function: the game's
 * copy is the one this unit emits after its first caller; the NPC factories call it by address. */
extern "C" inline void fn_800AE5D8(Behavior *behavior, const boost::weak_ptr<Behavior> &self) {
    *reinterpret_cast<boost::weak_ptr<Behavior> *>(behavior->field_0x14) = self;
}

/* A record's hooks as they are called (the record declares them without arguments). */
typedef void (*BehaviorHook)(const shared_ptr<PlayerBase> &player);

/* Shadow's behaviors by number (0x25 of them). */
static const UnkBehaviorDesc_804EC024 *s_behaviors[0x25] = {
    &lbl_804B0518, &lbl_804B0000, &lbl_804AFDD0, &lbl_804B0858, &lbl_804B0988, &lbl_804B0694,
    &lbl_804B07A4, &lbl_804B0D08, &lbl_804B03E0, &lbl_804B04CC, &lbl_804B0F44, &lbl_804B10B8,
    &lbl_804AFCBC, &lbl_804B1234, &lbl_804B0944, &lbl_804B0D60, &lbl_804B13DC, &lbl_804AFF64,
    &lbl_804B0DB4, &lbl_804AFACC, &lbl_804B08F4, &lbl_804B0AF8, &lbl_804B0C90, &lbl_804B08A0,
    &lbl_804B0DF8, &lbl_804B011C, &lbl_804AF784, &lbl_804AFB60, &lbl_804AF894, &lbl_804AF9CC,
    &lbl_804AFA7C, &lbl_804AFB10, &lbl_804AFD74, &lbl_804AF420, &lbl_80507B50, 0,
    0,
};

namespace Player {
namespace Shadow {

class ShadowBehavior : public Behavior::Factory {
  public:
    ShadowBehavior(const shared_ptr<PlayerBase> &player);
    virtual ~ShadowBehavior();
    virtual shared_ptr<Behavior::Behavior> Create(int id);

    /* 0x4 */ shared_ptr<PlayerBase> m_player;
};
SIZE_ASSERT(ShadowBehavior, 0xC);

ShadowBehavior::ShadowBehavior(const shared_ptr<PlayerBase> &player) : m_player(player) {
    for (int i = 0; i < 0x25; i++) {
        const UnkBehaviorDesc_804EC024 *desc = s_behaviors[i];
        if (desc != 0 && desc->fn_0x4 != 0) {
            ((BehaviorHook)desc->fn_0x4)(m_player);
        }
    }
}

ShadowBehavior::~ShadowBehavior() {
    for (int i = 0; i < 0x25; i++) {
        const UnkBehaviorDesc_804EC024 *desc = s_behaviors[i];
        if (desc != 0 && desc->field_0x8 != 0) {
            ((BehaviorHook)desc->field_0x8)(m_player);
        }
    }
}

} // namespace Shadow
} // namespace Player

namespace Player {
namespace Shadow {

shared_ptr<Behavior::Behavior> ShadowBehavior::Create(int id) {
    if (fn_8006CD78(m_player->m_stateBits, 0x25)) {
        switch (id) {
        case 0:
        case 2:
        case 3:
            id = 0x22;
            break;
        }
    }
    const UnkBehaviorDesc_804EC024 *desc = s_behaviors[id];
    if (desc != 0 && desc->create != 0) {
        shared_ptr<Behavior::Behavior> behavior(desc->create(m_player));
        fn_800AE5D8(behavior.get(), boost::weak_ptr<Behavior::Behavior>(behavior));
        return behavior;
    }
    return shared_ptr<Behavior::Behavior>();
}

} // namespace Shadow
} // namespace Player

/* Creates Shadow's behavior factory (called by Game/PlayerShadow.cpp; name inferred). */
extern "C" shared_ptr<Player::Behavior::Factory> fn_800AE330(const shared_ptr<PlayerBase> &player) {
    return shared_ptr<Player::Behavior::Factory>(new Player::Shadow::ShadowBehavior(player));
}
