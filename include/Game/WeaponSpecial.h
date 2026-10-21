#ifndef GAME_WEAPONSPECIAL_H
#define GAME_WEAPONSPECIAL_H

/* The special weapons' set-up and clean-up. Each WeaponSpecial<Name>.cpp unit defines a pair, which
 * Game/WeaponSpecial.cpp calls for all of them (namespace names follow the RTTI name
 * WeaponSpecialSatelliteLaser::SatelliteLaser; function names inferred). */
namespace WeaponSpecial {
/* Called from 0x8005E26C and 0x8005E300 (behaviour inferred). */
void Finalize();
void Initialize();
} // namespace WeaponSpecial

namespace WeaponSpecialKatana {
void Initialize();
void Finalize();
} // namespace WeaponSpecialKatana

namespace WeaponSpecialSatelliteLaser {
void Initialize();
void Finalize();
} // namespace WeaponSpecialSatelliteLaser

namespace WeaponSpecialVacuumEgg {
void Initialize();
void Finalize();
} // namespace WeaponSpecialVacuumEgg

namespace WeaponSpecialOmochaoGun {
void Initialize();
void Finalize();
} // namespace WeaponSpecialOmochaoGun

namespace WeaponSpecialHealCannon {
void Initialize();
void Finalize();
} // namespace WeaponSpecialHealCannon

namespace WeaponSpecialShadowRifle {
void Initialize();
void Finalize();
} // namespace WeaponSpecialShadowRifle

#endif
