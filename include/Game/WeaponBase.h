#ifndef GAME_WEAPONBASE_H
#define GAME_WEAPONBASE_H

#include "Game/WeaponBullet.h"
#include "Game/MathTypes.h"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

class CharInfo;

namespace Weapon {

/* An extra resource of a weapon model's part (struct name unknown; layout from the tables in
 * Game/WeaponReflector.cpp's .rodata; member names inferred). */
struct UnkWeaponEffectSub_80063428 {
    /* 0x0 */ s32 m_resource;
    /* 0x4 */ s32 field_0x4;
    /* 0x8 */ s32 field_0x8;
};
SIZE_ASSERT(UnkWeaponEffectSub_80063428, 0xC);

/* The effects a weapon model shows when it fires (struct names unknown; layout from the tables in
 * Game/WeaponLaserRifle.cpp's and Game/WeaponReflector.cpp's .rodata; member names inferred). The
 * resources at 0x8, 0xC and in the extra entries are loaded by the weapon's Initialize. */
struct UnkWeaponEffect_80063428 {
    /* 0x00 */ s32 field_0x0;
    /* 0x04 */ s32 field_0x4;
    /* 0x08 */ s32 m_resource;
    /* 0x0C */ s32 field_0xC;
    /* 0x10 */ const UnkWeaponEffectSub_80063428 *m_subs;
    /* 0x14 */ s32 m_subCount;
};
SIZE_ASSERT(UnkWeaponEffect_80063428, 0x18);

struct UnkWeaponEffectSet_80063428 {
    /* 0x0 */ const UnkWeaponEffect_80063428 *m_effects;
    /* 0x4 */ s32 m_effectCount;
    /* 0x8 */ const u32 *m_sounds;
    /* 0xC */ s32 m_soundCount;
};
SIZE_ASSERT(UnkWeaponEffectSet_80063428, 0x10);

/* The model of a weapon held by a character (class name unknown; a 4-byte object built by
 * 0x80063428 and destroyed by 0x8006330C). The methods keep their address names; what is known of
 * them is inferred from the callers. */
class UnkWeaponModel_80063428 {
  public:
    UnkWeaponModel_80063428(const boost::weak_ptr<CharInfo> &owner, const UnkWeaponEffectSet_80063428 *effects);
    /* A model without an owner, built from a model resource (0x80063484; Game/NpcTailsTail.cpp passes
     * the NPC's). */
    UnkWeaponModel_80063428(void *resource, const UnkWeaponEffectSet_80063428 *effects);
    ~UnkWeaponModel_80063428();

    /* The matrix of node id (the muzzle for 0). */
    const Mtx34f *fn_80063234(int id);
    void fn_800632A0(float dt);
    void fn_800632E8(const Vec3f *position, const Vec3f *rotation);
    /* Show and hide the model (behaviour inferred from Game/WeaponVacuumPod.cpp). */
    void fn_80063130();
    void fn_80063154();
    /* Sets the model's state (VacuumPod passes 0 idle, 1 release, 2 pulling; behaviour
     * inferred). */
    void fn_80063258(int state);
    /* Game/NpcTailsTail.cpp calls it after fn_80063258, and fn_8006310C after creating the model
     * (behaviour unknown). */
    void fn_8006327C();
    void fn_8006310C(void *arg);
    /* Places the model with a world matrix (behaviour inferred). */
    void fn_800632C4(const Mtx34f *matrix);

    /* 0x0 */ void *field_0x0;
};
SIZE_ASSERT(UnkWeaponModel_80063428, 0x4);

/* WeaponBase (class name from RTTI; vtable 0x8051E630): a weapon held by a character. The
 * constructor is at 0x8005D0F4 and the destructor at 0x8005D03C. Virtual function and member names
 * are inferred; the functions after the destructor are at 0x8005CFF0, 0x8005CEAC, 0x8005CF64,
 * 0x8005CF38, 0x8005CF14, 0x8005CEF8, 0x8005CED4, 0x8005CEB0 and 0x8005C0DC. */
class WeaponBase {
  public:
    WeaponBase(const boost::weak_ptr<CharInfo> &owner, int type, int arg);
    virtual ~WeaponBase();
    /* Places the weapon. */
    virtual void SetTransform(const Vec3f *position, const Vec3f *rotation);
    /* Per-frame work. */
    virtual void Tick(float dt);
    virtual void Show();
    virtual void Hide();
    virtual float vf_0x1C();
    virtual Vec3f vf_0x20();
    virtual float vf_0x24();
    virtual s32 vf_0x28();
    virtual bool vf_0x2C();

    /* The type, and the kind of weapon (Player::Shadow::ShadowWeapon picks its control by it;
     * names inferred). */
    int TypeId() const {
        return m_type;
    }
    int Kind() const {
        return field_0x18;
    }

    /* 0x04 */ boost::weak_ptr<WeaponBase> m_self;
    /* 0x0C */ boost::weak_ptr<CharInfo> m_owner;
    /* 0x14 */ s32 m_type;
    /* 0x18 */ s32 field_0x18;
    /* 0x1C */ UnkWeaponModel_80063428 *m_model;
    /* 0x20 */ Vec3f m_position;
    /* 0x2C */ Vec3f m_rotation;
};
SIZE_ASSERT(WeaponBase, 0x38);

/* WeaponGunBase (class name from RTTI; vtable 0x8051EB24): a weapon that fires bullets. The
 * constructor is at 0x8005EC08, the destructor at 0x8005EBB0 and Tick at 0x8005EBA4; the
 * functions after the two Fire slots are at 0x8005C0CC, 0x8005C0D4 and 0x8005C0D8. Names are
 * inferred. */
class WeaponGunBase : public WeaponBase {
  public:
    WeaponGunBase(const boost::weak_ptr<CharInfo> &owner, int type);
    virtual ~WeaponGunBase();
    virtual void Tick(float dt);
    /* Fires forwards, or at target from aim; both return the time until the next shot (-1 when
     * the weapon decides it itself; meaning inferred). */
    virtual float Fire(const Vec3f *aim) = 0;
    virtual float FireAt(const Vec3f *aim, const Vec3f *target) = 0;
    virtual float vf_0x38();
    virtual void vf_0x3C();
    virtual void vf_0x40();

    /* Starts the firing animation (0x8005EB94; behaviour inferred). */
    void fn_8005EB94(int arg);

    /* The shots fired this frame (name inferred from Player::Shadow's gun control). */
    int ShotCount() const {
        return field_0x38;
    }

    /* 0x38 */ s32 field_0x38;
};
SIZE_ASSERT(WeaponGunBase, 0x3C);

/* WeaponLaserBase (class name from RTTI): a gun whose bullets call back when they hit. Everything
 * is inline, so the units that use it emit its constructor, destructor and vtable (the copies the
 * game keeps are Game/WeaponLaserRifle.cpp's). */
class WeaponLaserBase : public WeaponGunBase {
  public:
    WeaponLaserBase(const boost::weak_ptr<CharInfo> &owner, int type) : WeaponGunBase(owner, type) {}
    virtual ~WeaponLaserBase() {}

#ifdef WEAPON_LASER_SET_HIT_CALLBACK
    /* Sets the bullets' hit callback (name inferred). Compiled only where
     * WEAPON_LASER_SET_HIT_CALLBACK is defined (Game/PlayerShadowWeaponGun.cpp), so that other
     * units' @N locals keep their numbers. */
    void SetHitCallback(
        const boost::function1<Bullet::BulletState, const Bullet::HitContext &> &callback) {
        m_hitCallback = callback;
    }
#endif

    /* 0x3C */ Bullet::HitCallback m_hitCallback;
};
SIZE_ASSERT(WeaponLaserBase, 0x48);

class Item;

} // namespace Weapon

/* The records a weapon unit registers (struct names unknown; each weapon unit's .data starts with
 * them; field names inferred). The names are stored by the unit's static initializer. */
struct UnkWeaponTaskInfo {
    /* 0x00 */ const char *name;
    /* 0x04 */ void (*initialize)(void);
    /* 0x08 */ void (*finalize)(void);
    /* 0x0C */ u32 field_0xC;
    /* 0x10 */ u32 field_0x10;
    /* 0x14 */ u32 field_0x14;
    /* 0x18 */ u32 field_0x18;
    /* 0x1C */ u32 field_0x1C;
    /* 0x20 */ u32 field_0x20;
};
SIZE_ASSERT(UnkWeaponTaskInfo, 0x24);

struct UnkWeaponTypeInfo {
    /* 0x00 */ const char *name;
    /* 0x04 */ Weapon::WeaponBase *(*create)(const boost::weak_ptr<CharInfo> &owner);
    /* 0x08 */ Weapon::Item *(*createItem)(void);
    /* Plays the weapon's firing sound at a muzzle matrix (Game/WeaponWormShooter.cpp; meaning
     * inferred). */
    /* 0x0C */ void (*fireEffect)(const Mtx34f *muzzle);
    /* 0x10 */ float field_0x10;
    /* 0x14 */ float field_0x14;
    /* 0x18 */ s32 field_0x18;
    /* 0x1C */ s32 field_0x1C;
    /* 0x20 */ u32 field_0x20;
    /* 0x24 */ float field_0x24;
    /* 0x28 */ float field_0x28;
    /* 0x2C */ u32 field_0x2C;
};
SIZE_ASSERT(UnkWeaponTypeInfo, 0x30);

extern "C" {
/* Builds the shot of a weapon from the muzzle matrix: forwards, or aimed at target (behaviour
 * inferred). */
Weapon::ShotInfo fn_80069424(Weapon::WeaponBase *weapon, const Mtx34f *muzzle,
                             const Vec3f *aim);
Weapon::ShotInfo fn_80069298(Weapon::WeaponBase *weapon, const Mtx34f *muzzle,
                             const Vec3f *aim, const Vec3f *target);
}

#endif
