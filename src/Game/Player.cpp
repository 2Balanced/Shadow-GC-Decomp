/* Player::Player (class name from RTTI; bases CharInfo, Player::PlayerObject and
 * Player::PlayerBase): a player-controlled character. Beyond PlayerBase it keeps the object the
 * player carries (slots 0x40 to 0x4C pick it up, drop it, throw it and return it) and answers
 * true to CharInfo's questions 0x18 to 0x28. File name from the RTTI name; function and member
 * names are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#define PLAYER_PICKUP_SLOT
#define PLAYER_PLAYER_IMPLEMENTATION
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/PickupCharInfo.h"
#include "Game/Player.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" {
/* The player's state bits (see Game/PlayerBehaviorElectricCircuit.cpp). */
bool fn_8006CD78(const u32 *bits, int bit);
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
}

/* The player members the slots read (names inferred). */
struct UnkPlayerFields_80076D88 {
    /* 0x000 */ u8 field_0x0[0xE8];
    /* 0x0E8 */ Vec3f m_direction; /* (meaning inferred) */
};

/* boost/weak_ptr.hpp only declares weak_ptr::reset, which the game has out of line (boost 1.32's
 * definition; as in Game/BaseDoor.cpp). */
namespace boost {
template <> inline void weak_ptr<PickupCharInfo>::reset() {
    this_type().swap(*this);
}
} // namespace boost

namespace Player {

SIZE_ASSERT(Player, 0x29C);

Player::Player(int arg0, const char *name) : PlayerBase(1, arg0, name) {
    field_0x298 = -1;
}

Player::~Player() {}

void Player::SetField298(s32 value) {
    field_0x298 = value;
}

void Player::vf_0x34() {
    fn_80076CC8(m_stateBits, 0x27);
}

void Player::vf_0x38() {
    fn_80076CA8(m_stateBits, 0x27);
}

bool Player::IsWeaponHidden() {
    return fn_8006CD78(m_stateBits, 0x27);
}

void Player::vf_0x40(const boost::weak_ptr<PickupCharInfo> &object) {
    m_object = object;
    if (!m_object.expired()) {
        fn_80076CA8(m_stateBits, 0x1E);
    } else {
        fn_80076CC8(m_stateBits, 0x1E);
    }
}

void Player::vf_0x44() {
    m_object.reset();
    fn_80076CC8(m_stateBits, 0x1E);
    fn_80076CC8(m_stateBits, 0x3D);
}

void Player::vf_0x48() {
    if (!m_object.expired()) {
        boost::shared_ptr<PickupCharInfo> object = m_object.lock();
        if (object) {
            object->PushCommand(new CastCommand(
                m_self, reinterpret_cast<UnkPlayerFields_80076D88 *>(this)->m_direction));
            vf_0x44();
        }
    }
}

boost::weak_ptr<PickupCharInfo> Player::vf_0x4C() {
    return m_object;
}

bool Player::vf_0x28() {
    return true;
}

bool Player::vf_0x24() {
    return true;
}

bool Player::vf_0x20() {
    return true;
}

bool Player::vf_0x1C() {
    return true;
}

bool Player::vf_0x18() {
    return true;
}

} // namespace Player
