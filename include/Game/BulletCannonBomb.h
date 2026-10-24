#ifndef GAME_BULLETCANNONBOMB_H
#define GAME_BULLETCANNONBOMB_H

#include "Game/WeaponBullet.h"
#include "boost/weak_ptr.hpp"

namespace Weapon {
namespace Bullet {

/* Leaves the blast of a shell of type type where bullet is; the blast ignores what attack, the
 * shell's attack collision, ignored (Game/BulletCannonBomb.cpp, 0x800544A8; name inferred). */
void StartCannonBomb(const BulletBase &bullet, int type,
                     const boost::weak_ptr<SonicteamUSA::System::CharaColli::CharaColliAttack> &attack);

} // namespace Bullet
} // namespace Weapon

#endif
