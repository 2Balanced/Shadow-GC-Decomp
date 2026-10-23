/* Weapon::AirBolt::AirBolt (class name from RTTI; declared in Game/WeaponAirBolt.h): the AirBolt
 * vehicle's lock-on gun (Game/VehicleAirBolt.cpp). While it locks on it shows a sight effect in
 * front of the vehicle; each locked target gets one shot of bullet type lbl_8051DD8C from its
 * muzzle. Member names are inferred. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#include "Game/DebugDisp.h"
#include "Game/WeaponAirBolt.h"
#include "Game/WeaponBullet.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

using Effect::System::EffectElement;
using Weapon::LockonTarget;
using Weapon::ShotInfo;

/* A kind of shot (see Game/ArkCannon.cpp): the function at 0x8 fires one and the float at 0x10 is
 * its range (names inferred). */
struct UnkBulletType_8051DD8C {
    /* 0x00 */ u32 field_0x0;
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ void (*fire)(ShotInfo *info);
    /* 0x0C */ u32 field_0xC;
    /* 0x10 */ float range;
};
/* the AirBolt's shot */
extern UnkBulletType_8051DD8C lbl_8051DD8C;

extern "C" {
/* Builds the shot of a weapon from a muzzle matrix toward a locked target (see
 * Game/WeaponWormShooter.cpp). */
ShotInfo fn_800690E4(Weapon::WeaponBase *weapon, const Mtx34f *muzzle,
                     const boost::shared_ptr<LockonTarget> &target);
/* Sets the colour of a lock-on weapon's target marks, and whether it is locking on (behaviour
 * inferred; see Game/WeaponSpecialSatelliteLaser.cpp). */
void fn_8005F830(Weapon::WeaponLockonBase *weapon, const UnkColor_8040D2D8 *color);
bool fn_8005F8C8(Weapon::WeaponLockonBase *weapon);
/* Sounds (see Game/GunBattery.cpp; behaviour inferred). */
void *fn_80053164(void);
void fn_801D560C(void *sound, int id, int arg2, int arg3);
void fn_801D53C4(void *sound, int id, const Vec3f *position, int arg3, int arg4, int arg5,
                 int arg6);
/* An effect that follows a matrix (see Game/DiablonPartsBody.cpp). */
boost::weak_ptr<EffectElement> fn_801D2C28(int arg0, const Mtx34f *matrix, int id, u32 *arg3);
/* Matrix helpers (behaviour inferred). */
Vec3f fn_8040FB08(const Mtx34f *matrix);              /* its translation */
Mtx34f fn_8040B138(float angle);                      /* a rotation about y */
Mtx34f fn_8001E044(const Mtx34f *a, const Mtx34f *b); /* product */
}

/* Degrees to radians (out of line in the game). */
float DegToRad(float degrees);

/* The colour of the target marks (black). */
static UnkColor_8040D2D8 s_markColor(0, 0, 0, 0);

namespace Weapon {
namespace AirBolt {

AirBolt::AirBolt(const boost::weak_ptr<CharInfo> &owner, const UnkMatrix_800B1304 *muzzles)
    : WeaponLockonBase(owner, 6, 0x44), m_muzzles(muzzles) {
    fn_8005F830(this, &s_markColor);
}

AirBolt::~AirBolt() {
    if (!m_sight.expired()) {
        m_sight.lock()->Retire();
    }
}

void AirBolt::Tick(float dt) {
    fn_8005FAF0();
    fn_8005F854(&m_muzzles[6], lbl_8051DD8C.range);
    fn_8005FACC(dt);
    if (fn_8005F8C8(this)) {
        fn_8040C148(&m_sightMatrix,
                    &(const Mtx34f &)fn_8001E044(&m_muzzles[6],
                                                 &(const Mtx34f &)fn_8040B138(DegToRad(180.0f))));
        if (m_sight.expired()) {
            m_sight = fn_801D2C28(0, &m_sightMatrix, 0x21B, 0);
        }
    } else if (!m_sight.expired()) {
        m_sight.lock()->Kill();
        m_sight.reset();
    }
}

float AirBolt::vf_0x34(const boost::shared_ptr<LockonTarget> &target, int index) {
    const UnkMatrix_800B1304 *muzzle = &m_muzzles[index];
    lbl_8051DD8C.fire(&(ShotInfo &)fn_800690E4(this, muzzle, target));
    PlayShotSound(muzzle);
    fn_8005F878(1);
    return 0.25f;
}

void AirBolt::vf_0x30() {
    fn_801D560C(fn_80053164(), 0x2020, 0, 0);
}

void AirBolt::PlayShotSound(const Mtx34f *muzzle) {
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)fn_8040FB08(muzzle));
    fn_801D53C4(fn_80053164(), 0x201E, &position, 0, 1, 0, 0);
}

boost::shared_ptr<WeaponLockonBase> AirBolt::Create(const boost::weak_ptr<CharInfo> &owner,
                                                    const UnkMatrix_800B1304 *muzzles) {
    boost::shared_ptr<WeaponLockonBase> weapon(new AirBolt(owner, muzzles));
    return weapon;
}

} // namespace AirBolt
} // namespace Weapon
