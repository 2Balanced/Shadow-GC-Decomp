#ifndef GAME_BULLETCHAOSSPEAR_H
#define GAME_BULLETCHAOSSPEAR_H

#include "Game/WeaponBullet.h"
#include "boost/shared_ptr.hpp"

namespace Effect {
namespace PJS {
class Launcher;
} // namespace PJS
} // namespace Effect

/* The Chaos Spear's bullet (Game/BulletChaosSpear.cpp), as Game/WeaponChaosSpear.cpp uses it.
 * Function names are inferred. */
namespace Weapon {
namespace Bullet {
namespace ChaosSpear {

/* Fires a Chaos Spear with info, its effects launched through launcher; charge is the weapon's
 * charge from 0 to 1. */
void Fire(const ShotInfo &info, const boost::shared_ptr<Effect::PJS::Launcher> &launcher,
          float charge);
/* How far a Chaos Spear flies. */
float GetRange();

} // namespace ChaosSpear
} // namespace Bullet
} // namespace Weapon

#endif
