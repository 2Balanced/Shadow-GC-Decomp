/* Player::WeaponBase (class name from RTTI; see Game/PlayerWeaponBase.h): the weapon slot of a
 * player character. Its constructor registers it at 0x238 of the player and its destructor
 * removes it; its slots are pure, or emitted by the units that first call them. File name from
 * the class. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/Player.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"

namespace Player {

WeaponBase::WeaponBase(Task *parent, const boost::shared_ptr<PlayerBase> &player)
    : Task(parent), m_player(player) {
    m_player.get()->m_weapon = this;
}

WeaponBase::~WeaponBase() {
    m_player.get()->m_weapon = 0;
}

} // namespace Player
