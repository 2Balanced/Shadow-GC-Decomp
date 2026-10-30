/* The bullet types: the table of each type's record (the types' units define them; see
 * Game/BulletGun.cpp) and the functions that read it, with the HitResult and ShotInfo
 * constructors (Game/WeaponBullet.h) that this unit is the first to need. Other units call the
 * functions by their addresses, so they keep those names. The unit has no class of its own; the
 * file name and the other names are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first).
 *
 * NonMatching (22/25 by name; every section's data matches):
 * - ShotInfo(owner, side, position, direction) (96.6%): Game/WeaponBullet.h declares side as bool,
 *   which GC/2.7 zero-extends (clrlwi) before storing it in the 32-bit m_side; retail stores the
 *   register as it is (the compiler-version issue Game/WeaponUtil.cpp documents).
 * - HitCallback's destructor (99.95%): retail passes -1 to the boost::function1 destructor, as for
 *   a member; Game/WeaponBullet.h derives HitCallback from it, which passes 0.
 * - 0x80051D10 matches as ShotInfo's matrix constructor, but keeps the name fn_80051D10, which
 *   Game/ArkCannon.cpp and Game/HeavyDogCtrlUnit.cpp call (their extern "C" declarations cannot
 *   spell the constructor's mangled name). */
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/WeaponBullet.h"
#include "Game/WeaponLockonTarget.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Weapon::LockonTarget;
using Weapon::ShotInfo;
using Weapon::Bullet::HitResult;

extern "C" {
/* The translation of a matrix, and a direction taken through a matrix (see Game/WeaponItem.cpp). */
Vec3f fn_8040FB08(const Mtx34f *matrix);
Vec3f fn_8040B618(const Mtx34f *matrix, const Vec3f *vector);
/* A vector set to zero (see Game/BirdBase.cpp). */
void fn_8040CE60(Vec3f *vector);
}

/* A bullet type's record (struct name unknown; see Game/BulletCannon.cpp). */
struct UnkBulletType_80546018 {
    /* 0x00 */ void (*initialize)(void);
    /* 0x04 */ void (*finalize)(void);
    /* 0x08 */ void (*create)(const ShotInfo &info);
    /* What a shot would hit within time seconds. */
    /* 0x0C */ HitResult (*hitTest)(const ShotInfo &info, float time);
    /* 0x10 */ float m_reach;
};
SIZE_ASSERT(UnkBulletType_80546018, 0x14);

extern UnkBulletType_80546018 lbl_8051D904;
extern UnkBulletType_80546018 lbl_8051D918;
extern UnkBulletType_80546018 lbl_8051D92C;
extern UnkBulletType_80546018 lbl_8051D940;
extern UnkBulletType_80546018 lbl_8051D954;
extern UnkBulletType_80546018 lbl_8051D968;
extern UnkBulletType_80546018 lbl_8051D97C;
extern UnkBulletType_80546018 lbl_8051D990;
extern UnkBulletType_80546018 lbl_8051DB34;
extern UnkBulletType_80546018 lbl_8051DB48;
extern UnkBulletType_80546018 lbl_8051DB5C;
extern UnkBulletType_80546018 lbl_8051DB70;
extern UnkBulletType_80546018 lbl_8051DB84;
extern UnkBulletType_80546018 lbl_8051DB98;
extern UnkBulletType_80546018 lbl_8051DBAC;
extern UnkBulletType_80546018 lbl_8051DBC0;
extern UnkBulletType_80546018 lbl_8051DBD4;
extern UnkBulletType_80546018 lbl_8051DBE8;
extern UnkBulletType_80546018 lbl_8051DBFC;
extern UnkBulletType_80546018 lbl_8051DC10;
extern UnkBulletType_80546018 lbl_8051DC24;
extern UnkBulletType_80546018 lbl_8051DC38;
extern UnkBulletType_80546018 lbl_8051DD14;
extern UnkBulletType_80546018 lbl_8051DD28;
extern UnkBulletType_80546018 lbl_8051DD3C;
extern UnkBulletType_80546018 lbl_8051DD50;
extern UnkBulletType_80546018 lbl_8051DD64;
extern UnkBulletType_80546018 lbl_8051DD78;
extern UnkBulletType_80546018 lbl_8051DEB0;
extern UnkBulletType_80546018 lbl_80546018;
extern UnkBulletType_80546018 lbl_8054602C;
extern UnkBulletType_80546018 lbl_80546040;
extern UnkBulletType_80546018 lbl_80546054;
extern UnkBulletType_80546018 lbl_80546068;
extern UnkBulletType_80546018 lbl_8054DB28;
extern UnkBulletType_80546018 lbl_8054DB3C;
extern UnkBulletType_80546018 lbl_8054DB70;

/* The records by bullet type. */
UnkBulletType_80546018 *lbl_8051D720[72] = {
    0, &lbl_8051DB34, &lbl_8051DB48, &lbl_8051DB5C,
    &lbl_8051DB70, &lbl_8051DB84, 0, &lbl_8051DB98,
    &lbl_8051DBAC, &lbl_8051DBC0, &lbl_8051DEB0, &lbl_8051DBD4,
    &lbl_8051D904, &lbl_8051D918, &lbl_8051D92C, &lbl_8051D940,
    &lbl_8051D954, &lbl_8051D968, &lbl_8051DD14, &lbl_8051DD28,
    &lbl_8051DD3C, &lbl_8051DD50, &lbl_8051DD64, &lbl_8051DD78,
    &lbl_8054DB70, &lbl_80546018, &lbl_8054602C, &lbl_80546040,
    &lbl_8051DBE8, &lbl_8051DBFC, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, &lbl_8051DC10, &lbl_8051DC24, &lbl_8054DB28,
    &lbl_8054DB3C, &lbl_8054DB70, &lbl_8054DB70, &lbl_80546054,
    &lbl_80546068, &lbl_8051D97C, &lbl_8051D990, &lbl_8051DC38,
    0, 0, 0, 0,
};

/* The lock-on target manager, once fn_80060058 (Game/WeaponLockonBase.cpp) has built it. */
struct UnkTargetManager_80060058;
UnkTargetManager_80060058 *lbl_805EE200;

namespace Weapon {

ShotInfo::ShotInfo(const boost::weak_ptr<CharInfo> &owner, bool side, const Vec3f &position,
                   const Vec3f &direction)
    : m_owner(owner), m_position(position), m_direction(direction), m_side(side) {
    boost::shared_ptr<CharInfo> chara = m_owner.lock();
    if (chara.get() != 0) {
        fn_800091D0(reinterpret_cast<Vec3f *>(&m_aim), &chara.get()->field_0x18);
    } else {
        fn_8040CE60(reinterpret_cast<Vec3f *>(&m_aim));
    }
}

} // namespace Weapon

namespace Weapon {

/* The shot from matrix's translation, forwards along it (0x80051D10; ArkCannon and HeavyDog call
 * it as fn_80051D10, a name the symbol keeps). */
ShotInfo::ShotInfo(const boost::weak_ptr<CharInfo> &owner, const boost::shared_ptr<LockonTarget> &target,
                   int side, const Mtx34f *matrix, const Vec3f &aim)
    : m_owner(owner), m_target(target), m_aim(aim), m_side(side) {
    fn_800091D0(reinterpret_cast<Vec3f *>(&m_position), &(const Vec3f &)fn_8040FB08(matrix));
    fn_800091D0(reinterpret_cast<Vec3f *>(&m_direction),
                &(const Vec3f &)fn_8040B618(matrix, &(const Vec3f &)Vec3f(0.0f, 0.0f, -1.0f)));
}

} // namespace Weapon

/* Initializes, ends and fires a bullet type. */
extern "C" void fn_80051CC8(int type) {
    UnkBulletType_80546018 *record = lbl_8051D720[type];
    if (record != 0 && record->initialize != 0) {
        record->initialize();
    }
}

extern "C" void fn_80051C80(int type) {
    UnkBulletType_80546018 *record = lbl_8051D720[type];
    if (record != 0 && record->finalize != 0) {
        record->finalize();
    }
}

extern "C" void fn_80051C34(int type, const ShotInfo &info) {
    UnkBulletType_80546018 *record = lbl_8051D720[type];
    if (record != 0 && record->create != 0) {
        record->create(info);
    }
}

namespace Weapon {
namespace Bullet {

HitResult::HitResult(bool hit)
    : m_position(lbl_805E07A0), m_normal(lbl_805E07A0), field_0x18(lbl_805E07A0), field_0x2C(0),
      field_0x30(0), m_hit(hit), m_struck(0) {}

} // namespace Bullet
} // namespace Weapon

/* What a shot of a bullet type would hit within time seconds. */
extern "C" HitResult fn_80051AF0(int type, const ShotInfo *shot, float time) {
    UnkBulletType_80546018 *record = lbl_8051D720[type];
    if (record != 0 && record->hitTest != 0) {
        return record->hitTest(*shot, time);
    }
    return HitResult(false);
}

/* How far a bullet type reaches. */
extern "C" float fn_80051AC8(int type) {
    UnkBulletType_80546018 *record = lbl_8051D720[type];
    if (record != 0) {
        return record->m_reach;
    }
    return 0.0f;
}
