/* @unnamed@PlayerShadowWeaponStick_cpp@::ControlStick (class and file name from RTTI): how Shadow
 * handles a weapon swung at close range (Weapon::WeaponStickBase). It swings while the button is
 * held or up, takes the hits from the ammo with a rumble, and drops the weapon once the ammo runs
 * out. Method and member names are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include "Game/Player.h"
#include "Game/PlayerShadowWeapon.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/Vibration.h"
#include "Game/WeaponBase.h"
#include "Game/WeaponStickBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

extern "C" {
/* The hits a swing landed this frame (Weapon::WeaponStickBase; behaviour inferred). */
int fn_80067894(Weapon::WeaponStickBase *weapon);
/* Rumble pattern on a player's controller. */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int arg1);
}

namespace {

using Player::Shadow::ControlWeaponBase;
using Player::Shadow::ShadowWeapon;

class ControlStick : public ControlWeaponBase {
  public:
    ControlStick(ShadowWeapon *owner, const boost::shared_ptr<Weapon::WeaponBase> &weapon);
    virtual ~ControlStick();
    virtual void Update(float dt);

    /* 0x08 */ boost::shared_ptr<Weapon::WeaponStickBase> m_stick;
};
SIZE_ASSERT(ControlStick, 0x10);

ControlStick::ControlStick(ShadowWeapon *owner, const boost::shared_ptr<Weapon::WeaponBase> &weapon)
    : ControlWeaponBase(owner) {
    m_stick = boost::dynamic_pointer_cast<Weapon::WeaponStickBase>(weapon);
}

ControlStick::~ControlStick() {}

void ControlStick::Update(float) {
    if (m_owner->m_flags.Test(0)) {
        m_stick->SwingHeld();
    } else {
        m_stick->SwingReleased();
    }
    if (fn_80067894(m_stick.get()) > 0) {
        m_owner->m_flags.Set(4);
        m_owner->AddAmmo(-fn_80067894(m_stick.get()));
        fn_8034DF48(1, m_owner->m_player->field_0x54);
    }
    if (!m_owner->HasAmmo()) {
        m_owner->DropWeapon();
    }
}

} // namespace

namespace Player {
namespace Shadow {

ControlWeaponBase *CreateControlStick(ShadowWeapon *owner,
                                      const boost::shared_ptr<Weapon::WeaponBase> &weapon) {
    return new ControlStick(owner, weapon);
}

} // namespace Shadow
} // namespace Player
