/* The special weapons' set-up and clean-up for all of them (file and function names inferred; the
 * order of the calls is retail's). Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include "Game/WeaponSpecial.h"

void WeaponSpecial::Initialize() {
    WeaponSpecialKatana::Initialize();
    WeaponSpecialSatelliteLaser::Initialize();
    WeaponSpecialVacuumEgg::Initialize();
    WeaponSpecialOmochaoGun::Initialize();
    WeaponSpecialHealCannon::Initialize();
    WeaponSpecialShadowRifle::Initialize();
}

void WeaponSpecial::Finalize() {
    WeaponSpecialKatana::Finalize();
    WeaponSpecialSatelliteLaser::Finalize();
    WeaponSpecialVacuumEgg::Finalize();
    WeaponSpecialOmochaoGun::Finalize();
    WeaponSpecialHealCannon::Finalize();
    WeaponSpecialShadowRifle::Finalize();
}
