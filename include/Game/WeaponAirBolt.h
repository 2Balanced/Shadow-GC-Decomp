#ifndef GAME_WEAPONAIRBOLT_H
#define GAME_WEAPONAIRBOLT_H

#include "Effect/System/EffectElement.h"
#include "Game/UnkMatrix_800B1304.h"
#include "Game/WeaponLockonBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

namespace Weapon {
namespace AirBolt {

/* Weapon::AirBolt::AirBolt (class name from RTTI; Game/WeaponAirBolt.cpp): the lock-on gun of the
 * AirBolt vehicle. It locks on to up to six targets in front of the vehicle's sight and fires one
 * shot from each of the vehicle's six muzzles. Member names are inferred. */
class AirBolt : public WeaponLockonBase {
  public:
    /* muzzles: the vehicle's seven matrices, the six muzzles and the sight. */
    AirBolt(const boost::weak_ptr<CharInfo> &owner, const UnkMatrix_800B1304 *muzzles);
    virtual ~AirBolt();
    virtual void Tick(float dt);
    virtual void vf_0x30();
    virtual float vf_0x34(const boost::shared_ptr<LockonTarget> &target, int index);

    /* Builds an AirBolt for the vehicle (name inferred). */
    static boost::shared_ptr<WeaponLockonBase> Create(const boost::weak_ptr<CharInfo> &owner,
                                                      const UnkMatrix_800B1304 *muzzles);

    void PlayShotSound(const Mtx34f *muzzle);

    /* 0x3C */ const UnkMatrix_800B1304 *m_muzzles;
    /* 0x40 */ Mtx34f m_sightMatrix;
    /* 0x70 */ boost::weak_ptr<Effect::System::EffectElement> m_sight;
};
SIZE_ASSERT(AirBolt, 0x78);

} // namespace AirBolt
} // namespace Weapon

#endif
