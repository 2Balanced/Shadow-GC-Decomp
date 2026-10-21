/* @unnamed@PlayerShadowWeaponCannon_cpp@::ControlCannon (class and file name from RTTI): how
 * Shadow handles a cannon (Weapon::WeaponCannonBase). While the fire button is held it waits for
 * the reload, raises the arm and fires; it takes the shells fired from the ammo, drops the cannon
 * once the ammo runs out, and keeps a sight effect where a shot would land. Method and member
 * names are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/Player.h"
#include "Game/PlayerShadowWeapon.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/TVehicleBase.h"
#include "Game/WeaponBase.h"
#include "Game/WeaponCannonBase.h"
#include "Game/WeaponBullet.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* An empty character pointer the weapon code passes as an effect's owner (see GunBattery.cpp). */
extern boost::shared_ptr<CharInfo> lbl_805EF980;

extern "C" {
/* A bit array: tests bit. */
bool fn_8006CD78(u32 *bits, int bit);
/* Whether the motion holds the arm up to aim (behaviour inferred). */
bool fn_800AA640(Player::PlayerMotion *motion);
/* Starts effect id for owner (behaviour inferred; see Weapon code). */
boost::weak_ptr<Effect::System::EffectElement> fn_8005E3D0(int id,
                                                          const boost::weak_ptr<CharInfo> &owner);
/* Places and turns an effect. */
void fn_80425E4C(Effect::System::EffectElement *effect, const Vec3f *position);
void fn_80425D94(Effect::System::EffectElement *effect, const Vec3f *rotation);
/* The normalised vector, and the angles of a direction (behaviour inferred). */
Vec3f fn_8040CEE8(const Vec3f *vector);
Vec2f fn_8040F760(const Vec3f &direction);
}

namespace {

using Player::Shadow::ControlWeaponBase;
using Player::Shadow::ShadowWeapon;

class ControlCannon : public ControlWeaponBase {
  public:
    ControlCannon(ShadowWeapon *owner, const boost::shared_ptr<Weapon::WeaponBase> &weapon);
    virtual ~ControlCannon();
    virtual void Update(float dt);

    /* Starts the sight effect. */
    void CreateSight();
    /* Puts the sight where a shot would land. */
    void UpdateSight();

    /* 0x08 */ boost::shared_ptr<Weapon::WeaponCannonBase> m_cannon;
    /* 0x10 */ float m_reload;
    /* 0x14 */ s32 m_shots;
    /* 0x18 */ boost::weak_ptr<Effect::System::EffectElement> m_sight;
};
SIZE_ASSERT(ControlCannon, 0x20);

ControlCannon::ControlCannon(ShadowWeapon *owner,
                             const boost::shared_ptr<Weapon::WeaponBase> &weapon)
    : ControlWeaponBase(owner) {
    m_cannon = boost::dynamic_pointer_cast<Weapon::WeaponCannonBase>(weapon);
    m_reload = 0.0f;
}

ControlCannon::~ControlCannon() {
    m_owner->LowerArm();
    m_owner->m_flags.Clear(2);
    if (!m_sight.expired()) {
        m_sight.lock()->Stop();
    }
}

void ControlCannon::Update(float dt) {
    if (fn_80014E34(&m_owner->m_player->m_flags, 7)) {
        m_owner->RaiseArm();
    } else {
        m_owner->LowerArm();
    }
    if (m_reload > 0.0f) {
        m_reload -= dt;
        if (m_reload <= 0.0f) {
            m_reload = 0.0f;
        }
    }
    if (m_owner->m_flags.Test(0) && !m_owner->m_flags.Test(2) && m_reload == 0.0f) {
        m_shots = 0;
        m_owner->m_flags.Set(2);
    }
    if (m_owner->m_flags.Test(2) && fn_800AA640(m_owner->m_player->m_motion) &&
        m_reload == 0.0f && !fn_8006CD78(m_owner->m_player->m_stateBits, 0x11) &&
        !fn_8006CD78(m_owner->m_player->m_stateBits, 0x12)) {
        m_reload = m_cannon->ReloadTime();
    }
    if (m_cannon->ShotCount()) {
        m_shots++;
        m_owner->m_flags.Set(3);
    }
    if (m_owner->m_flags.Test(1) || m_shots > 0) {
        if (m_owner->m_flags.Test(2)) {
            if (m_reload < 0.0f) {
                m_reload = 0.0f;
            }
            m_owner->m_flags.Clear(2);
            m_owner->m_flags.Clear(0);
        }
    }
    m_owner->AddAmmo(-m_cannon->m_shots);
    if (!m_owner->HasAmmo()) {
        m_owner->DropWeapon();
    } else {
        UpdateSight();
    }
}

void ControlCannon::CreateSight() {
    if (m_sight.expired()) {
        int id;
        switch (m_cannon->TypeId()) {
        case 15:
        case 16:
            id = 0x17B;
            break;
        default:
            id = 0x177;
            break;
        }
        m_sight = fn_8005E3D0(id, boost::weak_ptr<CharInfo>(lbl_805EF980));
        if (!m_sight.expired()) {
            m_sight.lock()->Hide();
        }
    }
}

void ControlCannon::UpdateSight() {
    if (m_sight.expired()) {
        CreateSight();
    }
    if (m_sight.expired()) {
        return;
    }
    boost::shared_ptr<Effect::System::EffectElement> effect = m_sight.lock();
    effect->Hide();
    if (fn_8006CD78(m_owner->m_player->m_stateBits, 0x10)) {
        boost::shared_ptr<Vehicle::TVehicleBase> vehicle = m_owner->m_player->m_vehicle.lock();
        if (vehicle && vehicle->vf_0x50()) {
            return;
        }
    }
    if (m_owner->m_player->IsWeaponHidden()) {
        return;
    }
    if (fn_800AA640(m_owner->m_player->m_motion)) {
        Weapon::Bullet::HitResult hit = m_cannon->AimHit();
        if (hit.m_hit) {
            Vec2f angles = fn_8040F760(
                (const Vec3f &)fn_8040CEE8(reinterpret_cast<const Vec3f *>(&hit.field_0x18)));
            Vec3f rotation(angles.x, angles.y, 0.0f);
            fn_80425E4C(effect.get(), reinterpret_cast<const Vec3f *>(&hit.m_position));
            fn_80425D94(effect.get(), &rotation);
            effect->Show();
        }
    }
}

} // namespace

namespace Player {
namespace Shadow {

ControlWeaponBase *CreateControlCannon(ShadowWeapon *owner,
                                       const boost::shared_ptr<Weapon::WeaponBase> &weapon) {
    return new ControlCannon(owner, weapon);
}

} // namespace Shadow
} // namespace Player
