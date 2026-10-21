#ifndef GAME_PLAYERSHADOWWEAPON_H
#define GAME_PLAYERSHADOWWEAPON_H

#include "Game/MathTypes.h"
#include "Game/PlayerWeaponBase.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

extern "C" {
/* A 32-bit flag word: fn_80014E04 constructs it cleared, fn_80014E10 clears it, fn_80014E1C sets
 * bit, fn_8006127C clears bit and fn_80014E34 tests bit (behaviour from their bodies). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
}

namespace Player {
namespace Shadow {

class ShadowWeapon;

/* ControlWeaponBase (class name from RTTI; vtable 0x80547B90): how Shadow handles one kind of
 * weapon (gun, cannon, lock-on, stick, vacuum; one unit each after Game/PlayerShadowWeapon.cpp).
 * Everything is inline; its vtable and destructor come out in Game/PlayerShadowWeapon.cpp. */
class ControlWeaponBase {
  public:
    ControlWeaponBase(ShadowWeapon *owner) : m_owner(owner) {}
    virtual ~ControlWeaponBase() {}
    /* Per-frame work (name inferred). */
    virtual void Update(float dt) = 0;

    /* 0x4 */ ShadowWeapon *m_owner;
};

/* The controls for each kind of weapon (one unit each; names inferred): kinds 1 and 6 (0x8023BEDC),
 * 2 (0x8023B734), 3 (0x8023D150), 5 (0x8023D560) and 4 (0x8023D7D0). */
ControlWeaponBase *CreateControlGun(ShadowWeapon *owner,
                                    const boost::shared_ptr<Weapon::WeaponBase> &weapon);
ControlWeaponBase *CreateControlCannon(ShadowWeapon *owner,
                                       const boost::shared_ptr<Weapon::WeaponBase> &weapon);
ControlWeaponBase *CreateControlLockon(ShadowWeapon *owner,
                                       const boost::shared_ptr<Weapon::WeaponBase> &weapon);
ControlWeaponBase *CreateControlStick(ShadowWeapon *owner,
                                      const boost::shared_ptr<Weapon::WeaponBase> &weapon);
ControlWeaponBase *CreateControlVacuum(ShadowWeapon *owner,
                                       const boost::shared_ptr<Weapon::WeaponBase> &weapon);

/* The flag word at 0x30 of ShadowWeapon (struct name unknown; bit meanings inferred: 0 in use,
 * 1 in alternative use, 2 to 4 reported by slots 0x48 to 0x50; bits 3 and 4 last one frame). */
struct ShadowWeaponFlags {
    ShadowWeaponFlags() {
        fn_80014E04(&m_bits);
    }
    void Clear() {
        fn_80014E10(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    void Clear(int bit) {
        fn_8006127C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits;
};

/* ShadowWeapon (class name from RTTI; vtable 0x80547B3C): Shadow's weapon slot. It holds the
 * Weapon::WeaponBase Shadow carries, a ControlWeaponBase for its kind, the ammo shown in the
 * player's record, and aims the weapon through an inverse-kinematics callback on the motion.
 * Method and member names are inferred. */
class ShadowWeapon : public WeaponBase {
  public:
    ShadowWeapon(Task *parent, const boost::shared_ptr<PlayerBase> &player);
    virtual ~ShadowWeapon();
    virtual void Tick(float dt);
    virtual void GiveWeaponType(int type, int ammo);
    virtual void GiveWeapon(const boost::shared_ptr<Weapon::WeaponBase> &weapon, int ammo);
    virtual boost::shared_ptr<Weapon::WeaponBase> GetWeapon();
    virtual int GetWeaponType();
    virtual int GetWeaponKind();
    virtual float GetWeaponValue();
    virtual void BeginUse();
    virtual void BeginAltUse();
    virtual void EndUse();
    virtual void DropWeapon();
    virtual void DiscardWeapon();
    virtual void HideWeapon();
    virtual void ShowWeapon();
    virtual bool CanUse();
    virtual bool IsFlag2();
    virtual bool IsFiring();
    virtual bool IsFlag4();

    /* Creates Shadow's weapon slot (0x80239FB0). */
    static WeaponBase *Create(Task *parent, const boost::shared_ptr<PlayerBase> &player);

    /* The motion's inverse-kinematics callback: aims the arm at the weapon's sight. */
    void OnIK(IKEventArgs &args);
    /* Drops the weapon and its control. */
    void RemoveWeapon();
    void SetWeaponType(int type, int ammo);
    void SetWeapon(const boost::shared_ptr<Weapon::WeaponBase> &weapon, int ammo);
    /* Places the weapon at the hand. */
    void UpdateTransform();
    /* Whether the weapon may be used: always with infinite ammo, else while ammo is left. */
    bool HasAmmo();
    void AddAmmo(int count);
    void SetAmmo(int count);
    int GetAmmo();
    /* Hands up and down while aiming (unless the weapon says otherwise). */
    void LowerArm();
    void RaiseArm();
    /* Throws the weapon away. */
    void ThrowWeapon();

    /* 0x30 */ ShadowWeaponFlags m_flags;
    /* 0x34 */ boost::shared_ptr<Weapon::WeaponBase> m_weapon;
    /* 0x3C */ ControlWeaponBase *m_control;
    /* 0x40 */ Vec3f m_recoil;
    /* 0x4C */ Vec3f m_sight;
    /* 0x58 */ float m_sightAngle;
    /* 0x5C */ u32 pad_0x5C;
};
SIZE_ASSERT(ShadowWeapon, 0x60);

} // namespace Shadow
} // namespace Player

#endif
