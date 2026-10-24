/* AntiAircraftRifle (class name from RTTI: Weapon::AntiAircraftRifle): a gun, weapon type 4, that
 * fires its weapon type's bullets with a sound, a muzzle flash and a second effect at the model's
 * node 1. The code is Game/WeaponHeavyShot.cpp's, but the firing effect is also the type record's
 * effect callback, so it takes the owner. File name inferred from the class name. Method names are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/GadgetUtil.h"
#include "Game/WeaponBase.h"
#include "Game/WeaponBullet.h"
#include "Game/WeaponItem.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Weapon::ShotInfo;
using Weapon::UnkWeaponEffect_80063428;
using Weapon::UnkWeaponEffectSet_80063428;
using Weapon::UnkWeaponModel_80063428;

extern "C" {
/* Debug output (behaviour inferred). */
void fn_803A953C(const char *format, ...);
/* Loads resource id (behaviour inferred). */
void fn_8006B50C(int id, int arg1);
/* The bullet types' records (behaviour inferred; see WeaponLaserRifle.cpp): fires bullet type type
 * with info, and runs its set-up and clean-up. */
void fn_80051C34(int type, const ShotInfo &info);
void fn_80051C80(int type);
void fn_80051CC8(int type);
/* Enables weapon type type (Game/WeaponTable.cpp). */
void fn_8005B760(int type);
/* The translation of a matrix. */
Vec3f fn_8040FB08(const Mtx34f *matrix);
/* Plays sound id at position (see Fan.cpp). */
void fn_801D53C4(void *sound, int id, const Vec3f *position, void *source, int arg4, int arg5,
                 int arg6);
/* Starts effect id at a matrix for owner (see GunBattery.cpp). */
boost::weak_ptr<Effect::System::EffectElement> fn_8005E418(const Mtx34f *matrix, int id,
                                                           const boost::weak_ptr<CharInfo> &owner);
}

/* An empty owner (see Game/GunBattery.cpp). */
extern boost::shared_ptr<CharInfo> lbl_805EF980;

static const char *s_taskName = "AntiAircraftRifle";

/* The effects of the weapon model and of the item's model (meanings inferred). */
static const UnkWeaponEffect_80063428 s_effects[] = {
    {-1, 0x2328, 0x3C, -1, 0, 0},
};

static const u32 s_sounds[] = {0x2329, 0x2332};

static const UnkWeaponEffectSet_80063428 s_effectSet = {s_effects, 1, s_sounds, 2};

static const UnkWeaponEffect_80063428 s_itemEffects[] = {
    {-1, 0x801, 0x3C, -1, 0, 0},
};

static const u32 s_itemSounds[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSet = {s_itemEffects, 1, s_itemSounds, 2};

namespace Weapon {

class AntiAircraftRifle : public WeaponGunBase {
  public:
    AntiAircraftRifle(const boost::weak_ptr<CharInfo> &owner) : WeaponGunBase(owner, 4) {
        m_model = new UnkWeaponModel_80063428(owner, &s_effectSet);
    }
    virtual ~AntiAircraftRifle();
    virtual void SetTransform(const Vec3f *position, const Vec3f *rotation);
    virtual void Tick(float dt);
    virtual float Fire(const Vec3f *aim);
    virtual float FireAt(const Vec3f *aim, const Vec3f *target);

    static WeaponBase *Instantiate(const boost::weak_ptr<CharInfo> &owner);
    static Item *CreateItem();
};
SIZE_ASSERT(AntiAircraftRifle, 0x3C);

} // namespace Weapon

/* A distance (name inferred). As a call it is not a constant expression, so the records' values are
 * stored by the static initializer, as in retail. */
inline float Span(float length) {
    return length;
}

/* Plays the firing sound and muzzle flash at a muzzle matrix for owner. */
static void PlayFireEffect(const Mtx34f *muzzle, const boost::weak_ptr<CharInfo> &owner);

namespace Weapon {

AntiAircraftRifle::~AntiAircraftRifle() {
    delete m_model;
}

void AntiAircraftRifle::SetTransform(const Vec3f *position, const Vec3f *rotation) {
    WeaponBase::SetTransform(position, rotation);
    m_model->fn_800632E8(position, rotation);
}

void AntiAircraftRifle::Tick(float dt) {
    WeaponGunBase::Tick(dt);
    m_model->fn_800632A0(dt);
}

float AntiAircraftRifle::Fire(const Vec3f *aim) {
    fn_80051C34(m_type, fn_80069424(this, m_model->fn_80063234(0), aim));
    PlayFireEffect(m_model->fn_80063234(0), lbl_805EF980);
    fn_8005E418(m_model->fn_80063234(1), 0xDE, m_owner);
    fn_8005EB94(1);
    return 1.0f / 6.0f;
}

float AntiAircraftRifle::FireAt(const Vec3f *aim, const Vec3f *target) {
    fn_80051C34(m_type, fn_80069298(this, m_model->fn_80063234(0), aim, target));
    PlayFireEffect(m_model->fn_80063234(0), lbl_805EF980);
    fn_8005E418(m_model->fn_80063234(1), 0xDE, m_owner);
    fn_8005EB94(1);
    return 1.0f / 6.0f;
}

} // namespace Weapon

static void PlayFireEffect(const Mtx34f *muzzle, const boost::weak_ptr<CharInfo> &owner) {
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)fn_8040FB08(muzzle));
    fn_801D53C4(fn_80053164(), 0x200B, &position, 0, 1, 0, 0);
    fn_8005E418(muzzle, 0x158, owner);
}

/* The type's load and release hooks (names inferred). */
static void Initialize() {
    fn_803A953C("%s::Initialize\n", s_taskName);
    fn_8006B50C(0x3C, 0);
    fn_80051CC8(4);
    fn_8005B760(4);
}

static void Finalize() {
    fn_803A953C("%s::Finalize\n", s_taskName);
    fn_80051C80(4);
}

Weapon::WeaponBase *Weapon::AntiAircraftRifle::Instantiate(const boost::weak_ptr<CharInfo> &owner) {
    return new AntiAircraftRifle(owner);
}

Weapon::Item *Weapon::AntiAircraftRifle::CreateItem() {
    return new Item(&s_itemEffectSet);
}

/* The type record's firing effect, with no owner. */
static void FireEffect(const Mtx34f *muzzle) {
    PlayFireEffect(muzzle, lbl_805EF980);
}

UnkWeaponTaskInfo lbl_8051E410 = {
    s_taskName, Initialize, Finalize, 0, 0, 0x80, 0x00CB0400, 0, 0,
};

UnkWeaponTypeInfo lbl_8051E434 = {
    s_taskName, Weapon::AntiAircraftRifle::Instantiate, Weapon::AntiAircraftRifle::CreateItem, FireEffect,
    Span(1.0f), Span(5.0f), 0x1E, 1, 0x2002, 0.66666f, 0.5f, 0,
};

/* WeaponGunBase's and WeaponBase's empty virtual functions, which this unit is the first to need (in
 * retail they are likely inline: every weapon's vtable shares this unit's copies). They are defined
 * last so that they come first. */
bool Weapon::WeaponBase::vf_0x2C() {
    return false;
}

void Weapon::WeaponGunBase::vf_0x40() {}

void Weapon::WeaponGunBase::vf_0x3C() {}

float Weapon::WeaponGunBase::vf_0x38(const Vec3f *aim) {
    return 0.0f;
}
