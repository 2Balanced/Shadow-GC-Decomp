/* EmbeddedGun::AutoRifle and EmbeddedBazooka::Bazooka (class names from RTTI): the weapons built
 * into a player character (weapon types 0x45 and 0x46). They have no model of their own: they fire
 * from nodes 0xC and 0xD of the player's skeleton, through the bullet type records at 0x8051DB5C
 * and 0x8051D918. The player code (0x800AD3F8) creates the rifle for character types 0x10 and 0x11
 * and the bazooka for 0x12 and 0x13. The file name is inferred (one unit holds both classes);
 * function and member names are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/GadgetUtil.h"
#include "Game/Player.h"
#include "Game/WeaponBase.h"
#include "Game/WeaponBullet.h"
#include "Game/WeaponCannonBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Weapon::ShotInfo;
using Weapon::Bullet::HitResult;

/* A skeleton node (see Game/PlayerWeaponBase.h). */
struct UnkMotionNode_800AA760 {
    /* 0x00 */ const Mtx34f *m_matrix;
};

/* A bullet type's record (struct name unknown; see Game/BulletLaser.cpp): its creation call and
 * the hit a shot would make (names inferred). */
struct UnkBulletType_8051D918 {
    /* 0x00 */ void (*initialize)(void);
    /* 0x04 */ void (*finalize)(void);
    /* 0x08 */ void (*create)(const ShotInfo &info);
    /* 0x0C */ HitResult (*aimHit)(const ShotInfo &info, float time);
    /* 0x10 */ float m_reach;
};

/* The bazooka's shell and the rifle's bullet. */
extern UnkBulletType_8051D918 lbl_8051D918;
extern UnkBulletType_8051D918 lbl_8051DB5C;

extern "C" {
/* A node of a motion (see Game/NpcPartModel.h). */
UnkMotionNode_800AA760 *fn_800AA760(Player::PlayerMotion *motion, int index);
/* The translation of a matrix. */
Vec3f fn_8040FB08(const Mtx34f *matrix);
/* Plays sound id at position (see Game/Bomb.cpp). */
void fn_801D54F4(void *manager, int id, const Vec3f *position, int arg3, int arg4);
/* Starts effect id at a matrix for owner (see Game/WeaponHeavyShot.cpp). */
boost::weak_ptr<Effect::System::EffectElement> fn_8005E418(const Mtx34f *matrix, int id,
                                                           const boost::weak_ptr<CharInfo> &owner);
/* The shot of a weapon fired from the muzzle matrix (see fn_80069424 in Game/WeaponBase.h). */
ShotInfo fn_80069540(Weapon::WeaponBase *weapon, const Mtx34f *muzzle);
}

namespace EmbeddedGun {

class AutoRifle : public Weapon::WeaponGunBase {
  public:
    AutoRifle(const boost::weak_ptr<CharInfo> &owner, const boost::weak_ptr<Player::PlayerBase> &player);
    virtual ~AutoRifle();
    virtual float vf_0x1C();
    virtual Vec3f vf_0x20();
    virtual float vf_0x24() {
        return 0.5f;
    }
    virtual float vf_0x28() {
        return 0.0f;
    }
    virtual float Fire(const Vec3f *aim);
    virtual float FireAt(const Vec3f *aim, const Vec3f *target);

    /* The firing sound and the flashes at both muzzles. */
    void PlayFireEffect(const Mtx34f *muzzle, const Mtx34f *muzzle2);

    /* 0x3C */ boost::weak_ptr<Player::PlayerBase> m_player;
};
SIZE_ASSERT(AutoRifle, 0x44);

} // namespace EmbeddedGun

namespace EmbeddedBazooka {

class Bazooka : public Weapon::WeaponCannonBase {
  public:
    Bazooka(const boost::weak_ptr<CharInfo> &owner, const boost::weak_ptr<Player::PlayerBase> &player);
    virtual ~Bazooka();
    virtual float vf_0x1C();
    virtual Vec3f vf_0x20();
    virtual float vf_0x24() {
        return 1.0f;
    }
    virtual float vf_0x28() {
        return 1.0f;
    }
    virtual float ReloadTime();
    virtual HitResult AimHit();
    virtual void vf_0x38();

    /* The firing sound and the muzzle flash. */
    void PlayFireEffect(const Mtx34f *muzzle);

    /* 0x3C */ boost::weak_ptr<Player::PlayerBase> m_player;
};
SIZE_ASSERT(Bazooka, 0x44);

} // namespace EmbeddedBazooka

namespace EmbeddedGun {

AutoRifle::AutoRifle(const boost::weak_ptr<CharInfo> &owner, const boost::weak_ptr<Player::PlayerBase> &player)
    : WeaponGunBase(owner, 0x45), m_player(player) {}

AutoRifle::~AutoRifle() {}

float AutoRifle::vf_0x1C() {
    return lbl_8051DB5C.m_reach;
}

Vec3f AutoRifle::vf_0x20() {
    return Vec3f(0.0f, 0.0f, -3.1875f);
}

float AutoRifle::Fire(const Vec3f *aim) {
    if (!m_player.expired()) {
        UnkMotionNode_800AA760 *node = fn_800AA760(m_player.lock()->m_motion, 0xC);
        UnkMotionNode_800AA760 *node2 = fn_800AA760(m_player.lock()->m_motion, 0xD);
        lbl_8051DB5C.create(fn_80069424(this, node->m_matrix, aim));
        PlayFireEffect(node->m_matrix, node2->m_matrix);
        fn_8005EB94(1);
    }
    return 0.1f;
}

float AutoRifle::FireAt(const Vec3f *aim, const Vec3f *target) {
    if (!m_player.expired()) {
        UnkMotionNode_800AA760 *node = fn_800AA760(m_player.lock()->m_motion, 0xC);
        UnkMotionNode_800AA760 *node2 = fn_800AA760(m_player.lock()->m_motion, 0xD);
        lbl_8051DB5C.create(fn_80069298(this, node->m_matrix, aim, target));
        PlayFireEffect(node->m_matrix, node2->m_matrix);
        fn_8005EB94(1);
    }
    return 0.1f;
}

void AutoRifle::PlayFireEffect(const Mtx34f *muzzle, const Mtx34f *muzzle2) {
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)fn_8040FB08(muzzle));
    fn_801D54F4(fn_80053164(), 0x200A, &position, 0, 1);
    fn_8005E418(muzzle, 0x152, m_owner);
    fn_8005E418(muzzle2, 0xD0, m_owner);
}

} // namespace EmbeddedGun

namespace EmbeddedBazooka {

Bazooka::Bazooka(const boost::weak_ptr<CharInfo> &owner, const boost::weak_ptr<Player::PlayerBase> &player)
    : WeaponCannonBase(owner, 0x46, 0), m_player(player) {}

Bazooka::~Bazooka() {}

float Bazooka::vf_0x1C() {
    return lbl_8051D918.m_reach;
}

Vec3f Bazooka::vf_0x20() {
    return Vec3f(0.0f, 0.0f, -3.1875f);
}

float Bazooka::ReloadTime() {
    if (!m_player.expired()) {
        UnkMotionNode_800AA760 *node = fn_800AA760(m_player.lock()->m_motion, 0xC);
        lbl_8051D918.create(fn_80069540(this, node->m_matrix));
        PlayFireEffect(node->m_matrix);
        m_shots++;
    }
    return 0.5f;
}

HitResult Bazooka::AimHit() {
    if (!m_player.expired()) {
        UnkMotionNode_800AA760 *node = fn_800AA760(m_player.lock()->m_motion, 0xC);
        if (node->m_matrix != 0) {
            return lbl_8051D918.aimHit(fn_80069540(this, node->m_matrix), 2.0f);
        }
    }
    return HitResult(false);
}

void Bazooka::vf_0x38() {}

void Bazooka::PlayFireEffect(const Mtx34f *muzzle) {
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)fn_8040FB08(muzzle));
    fn_801D54F4(fn_80053164(), 0x2015, &position, 0, 1);
    fn_8005E418(muzzle, 0xF7, m_owner);
}

} // namespace EmbeddedBazooka

/* Create the weapons for a player (called by the player code at 0x800AD3F8; names inferred). */
namespace EmbeddedGun {
boost::shared_ptr<Weapon::WeaponGunBase> Create(const boost::weak_ptr<CharInfo> &owner,
                                                const boost::weak_ptr<Player::PlayerBase> &player) {
    return boost::shared_ptr<Weapon::WeaponGunBase>(new AutoRifle(owner, player));
}
} // namespace EmbeddedGun

namespace EmbeddedBazooka {
boost::shared_ptr<Weapon::WeaponCannonBase> Create(const boost::weak_ptr<CharInfo> &owner,
                                                   const boost::weak_ptr<Player::PlayerBase> &player) {
    return boost::shared_ptr<Weapon::WeaponCannonBase>(new Bazooka(owner, player));
}
} // namespace EmbeddedBazooka
