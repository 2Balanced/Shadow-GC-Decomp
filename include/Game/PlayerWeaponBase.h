#ifndef GAME_PLAYERWEAPONBASE_H
#define GAME_PLAYERWEAPONBASE_H

#include <boost/function.hpp>

#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

namespace Weapon {
class WeaponBase;
}

/* Target (class name from RTTI): see Game/CharCommand.h. */
class Target;

namespace Player {

/* What a character's motion passes to an inverse-kinematics callback (class name from the RTTI of
 * Game/PlayerShadowWeapon.cpp's bound callback; layout and member names inferred from that
 * callback): the node being solved, whose frame comes first, and the point the node should aim
 * at, which the callback writes. */
struct UnkIKNode {
    /* 0x0 */ void *m_frame;
};

struct IKEventArgs {
    /* 0x0 */ UnkIKNode *m_node;
    /* 0x4 */ Vec3f *m_target;
};

typedef boost::function1<void, IKEventArgs &> IKCallback;

/* PlayerMotion (class name from RTTI; vtable 0x80521A84): the animation of a player character,
 * a Task kept at 0x230 of the player. Only the slots the weapon code calls are named (names and
 * meanings inferred from those callers). */
class PlayerMotion : public Task {
  public:
    virtual ~PlayerMotion();
    virtual void Tick(float dt);
    /* Plays motion, blending over blendTime (meaning inferred from Player::Behavior::FirstMeet,
     * which passes 0x2E and 0). */
    virtual void vf_0x10(int motion, float blendTime) = 0;
    virtual void vf_0x14() = 0;
    /* Selects the pose for a held weapon (0: none; the value comes from the weapon type). */
    virtual void SetWeaponPose(int pose);
    virtual void vf_0x1C();
    virtual void vf_0x20(int arg);
    virtual void vf_0x24();
    /* Installs the inverse-kinematics callback of a slot. */
    virtual void SetIKCallback(int slot, const IKCallback &callback);
};

/* PlayerTargetBase (class name from RTTI; vtable 0x80543DF8): the aim of a player character,
 * kept at 0x24C of the player (slot names inferred from Game/PlayerShadowWeapon.cpp). */
class PlayerTargetBase {
  public:
    virtual ~PlayerTargetBase();
    virtual void vf_0x0C();
    virtual void vf_0x10();
    /* Whether a target is locked on, its position, and the target (behaviour inferred from
     * Game/PlayerShadowWeaponGun.cpp). */
    virtual bool HasTarget();
    virtual void vf_0x18();
    virtual void vf_0x1C();
    virtual Vec3f GetTargetPosition();
    virtual boost::weak_ptr<Target> GetTarget();
    virtual void vf_0x28();
    /* Turns a vector into the aim's frame (behaviour inferred). */
    virtual Vec3f ToAimSpace(const Vec3f &vector);
};

/* WeaponBase (class name from RTTI; vtable 0x805223DC): the weapon slot of a player character, a
 * Task that holds the Weapon::WeaponBase the player carries. The constructor (0x800B8690)
 * registers it at 0x238 of the player, the destructor (0x800B861C) removes it. Slots 0x14 and
 * 0x18 have bodies of their own (0x800ADDD8, 0x800813B8); the others are pure. Slot names are
 * inferred from Player::Shadow::ShadowWeapon (Game/PlayerShadowWeapon.cpp). */
class WeaponBase : public Task {
  public:
    WeaponBase(Task *parent, const boost::shared_ptr<PlayerBase> &player);
    virtual ~WeaponBase();
    virtual void Tick(float dt) = 0;
    /* Gives the player a weapon of a type with ammo rounds (-1: the type's default). */
    virtual void GiveWeaponType(int type, int ammo) = 0;
    /* Gives the player a weapon (ammo as above). */
    virtual void GiveWeapon(const boost::shared_ptr<Weapon::WeaponBase> &weapon, int ammo);
    virtual boost::shared_ptr<Weapon::WeaponBase> GetWeapon();
    virtual int GetWeaponType() = 0;
    virtual int GetWeaponKind() = 0;
    virtual float GetWeaponValue() = 0;
    virtual void BeginUse() = 0;
    virtual void BeginAltUse() = 0;
    virtual void EndUse() = 0;
    virtual void DropWeapon() = 0;
    virtual void DiscardWeapon() = 0;
    virtual void HideWeapon() = 0;
    virtual void ShowWeapon() = 0;
    virtual bool CanUse() = 0;
    virtual bool IsFlag2() = 0;
    virtual bool IsFiring() = 0;
    virtual bool IsFlag4() = 0;

    /* 0x28 */ boost::shared_ptr<PlayerBase> m_player;
};
SIZE_ASSERT(WeaponBase, 0x30);

} // namespace Player

#endif
