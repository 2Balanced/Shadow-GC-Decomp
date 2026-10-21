/* SatelliteLaser (class name from RTTI: WeaponSpecialSatelliteLaser::SatelliteLaser): a lock-on
 * weapon (Weapon::WeaponLockonBase) that fires one shot of its own bullet type at each locked
 * target. Registered as two weapons, SatelliteLaser01 (type 0x3B, one target) and SatelliteLaser02
 * (type 0x3C, two targets). File name from the RTTI name. Method names marked "(name inferred)" are
 * not original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/CharInfo.h"
#include "Game/GadgetUtil.h"
#include "Game/WeaponBase.h"
#include "Game/WeaponBullet.h"
#include "Game/WeaponItem.h"
#include "Game/WeaponLockonBase.h"
#include "Game/WeaponSpecial.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Weapon::LockonTarget;
using Weapon::ShotInfo;
using Weapon::UnkWeaponEffect_80063428;
using Weapon::UnkWeaponEffectSet_80063428;
using Weapon::UnkWeaponModel_80063428;

/* An RGBA colour (struct name unknown; constructed by 0x8040D2D8). */
struct UnkColor_8040D2D8 {
    UnkColor_8040D2D8(u8 r, u8 g, u8 b, u8 a);

    u8 r;
    u8 g;
    u8 b;
    u8 a;
};
SIZE_ASSERT(UnkColor_8040D2D8, 0x4);

extern "C" {
/* Loads resource id (behaviour inferred). */
void fn_8006B50C(int id, int arg1);
/* The bullet types' records (behaviour inferred; see WeaponLaserRifle.cpp): fires bullet type type
 * with info, and runs its set-up and clean-up. */
void fn_80051C34(int type, const ShotInfo &info);
void fn_80051C80(int type);
void fn_80051CC8(int type);
/* The range of weapon type type (see WeaponWormShooter.cpp). */
float fn_80051AC8(int type);
/* Sets up the pick-up item of weapon type type (behaviour inferred). */
void fn_8005B760(int type);
/* Plays sound id (see GunBattery.cpp). */
void fn_801D560C(void *sound, int id, int arg2, int arg3);
/* Builds the shot of a weapon from a muzzle matrix toward a locked target (see
 * WeaponWormShooter.cpp). */
ShotInfo fn_800690E4(Weapon::WeaponBase *weapon, const Mtx34f *muzzle,
                     const boost::shared_ptr<LockonTarget> &target);
/* Sets the colour of a lock-on weapon's target marks (behaviour inferred). */
void fn_8005F830(Weapon::WeaponLockonBase *weapon, const UnkColor_8040D2D8 *color);
}

namespace WeaponSpecialSatelliteLaser {

class SatelliteLaser : public Weapon::WeaponLockonBase {
  public:
    SatelliteLaser(const boost::weak_ptr<CharInfo> &owner, int count, int type,
                   const UnkWeaponEffectSet_80063428 *effects,
                   const UnkColor_8040D2D8 *color)
        : Weapon::WeaponLockonBase(owner, count, type) {
        m_model = new UnkWeaponModel_80063428(owner, effects);
        fn_8005F830(this, color);
    }
    virtual ~SatelliteLaser();
    virtual void SetTransform(const Vec3f *position, const Vec3f *rotation);
    virtual void Tick(float dt);
    virtual void vf_0x30();
    virtual float vf_0x34(const boost::shared_ptr<LockonTarget> &target, int index);
};
SIZE_ASSERT(SatelliteLaser, 0x3C);

} // namespace WeaponSpecialSatelliteLaser

using WeaponSpecialSatelliteLaser::SatelliteLaser;

/* The time until the next shot (name inferred). */
static float s_fireInterval = 0.25f;

/* The effects of the weapon model and of the item's model (meanings inferred). */
static const UnkWeaponEffect_80063428 s_effects[] = {
    {-1, 0x2328, 0x4D, -1, 0, 0},
    {0x233C, -1, 0x4E, -1, 0, 0},
};

static const u32 s_sounds[] = {0x2329, 0x2329};

static const UnkWeaponEffectSet_80063428 s_effectSet = {s_effects, 2, s_sounds, 2};

static const UnkWeaponEffect_80063428 s_itemEffects[] = {
    {-1, 0x801, 0x4D, -1, 0, 0},
    {0x233C, -1, 0x4E, -1, 0, 0},
};

static const u32 s_itemSounds[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSet = {s_itemEffects, 2, s_itemSounds, 2};

/* The names are read by the static initializer. */
static const char *s_name01 = "SatelliteLaser01";
static const char *s_name02 = "SatelliteLaser02";

/* The colour of the target marks (name inferred). */
static UnkColor_8040D2D8 s_markColor(0xAF, 0, 0xFF, 0x32);

/* A distance (name inferred). As a call it is not a constant expression, so the records' values are
 * stored by the static initializer, as in retail. */
inline float Span(float length) {
    return length;
}

SatelliteLaser::~SatelliteLaser() {
    delete m_model;
}

void SatelliteLaser::SetTransform(const Vec3f *position, const Vec3f *rotation) {
    Weapon::WeaponBase::SetTransform(position, rotation);
    m_model->fn_800632E8(position, rotation);
}

void SatelliteLaser::Tick(float dt) {
    fn_8005FAF0();
    m_model->fn_800632A0(dt);
    float range = fn_80051AC8(m_type);
    fn_8005F854(m_model->fn_80063234(1), range);
    fn_8005FACC(dt);
}

float SatelliteLaser::vf_0x34(const boost::shared_ptr<LockonTarget> &target, int index) {
    if (fn_8005F9C8() > 0) {
        const Mtx34f *muzzle = m_model->fn_80063234(index);
        fn_80051C34(m_type, fn_800690E4(this, muzzle, target));
        fn_8005F878(1);
    }
    return s_fireInterval;
}

void SatelliteLaser::vf_0x30() {
    fn_801D560C(fn_80053164(), 0x2020, 0, 0);
}

void WeaponSpecialSatelliteLaser::Initialize() {
    fn_8006B50C(0x4D, 0);
    fn_8006B50C(0x4E, 0);
    fn_80051CC8(0x3B);
    fn_80051CC8(0x3C);
    fn_8005B760(0x3B);
    fn_8005B760(0x3C);
}

void WeaponSpecialSatelliteLaser::Finalize() {
    fn_80051C80(0x3B);
    fn_80051C80(0x3C);
}

/* The records' callbacks (names inferred). */
static Weapon::WeaponBase *Instantiate01(const boost::weak_ptr<CharInfo> &owner) {
    return new SatelliteLaser(owner, 1, 0x3B, &s_effectSet, &s_markColor);
}

static Weapon::Item *CreateItem01() {
    return new Weapon::Item(&s_itemEffectSet);
}

static Weapon::WeaponBase *Instantiate02(const boost::weak_ptr<CharInfo> &owner) {
    return new SatelliteLaser(owner, 2, 0x3C, &s_effectSet, &s_markColor);
}

static Weapon::Item *CreateItem02() {
    return new Weapon::Item(&s_itemEffectSet);
}

UnkWeaponTypeInfo lbl_8054D460 = {
    s_name01, Instantiate01, CreateItem01, 0, Span(1.0f), Span(5.0f), 6, 1, 0x2002, 0.0f, 0.0f, 0,
};

UnkWeaponTypeInfo lbl_8054D490 = {
    s_name02, Instantiate02, CreateItem02, 0, Span(1.0f), Span(5.0f), 0xA, 1, 0x2002, 0.0f, 0.0f, 0,
};
