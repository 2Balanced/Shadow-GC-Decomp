/* ShadowRifle (class name from RTTI: @unnamed@WeaponSpecialShadowRifle_cpp@::ShadowRifle): a gun
 * (Weapon::WeaponGunBase) that fires its own bullet type with a sound and a muzzle flash, weapon
 * type 0x43. File name from the RTTI name. Method names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/GadgetUtil.h"
#include "Game/WeaponBase.h"
#include "Game/WeaponBullet.h"
#include "Game/WeaponItem.h"
#include "Game/WeaponSpecial.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Weapon::ShotInfo;
using Weapon::UnkWeaponEffect_80063428;
using Weapon::UnkWeaponEffectSet_80063428;
using Weapon::UnkWeaponEffectSub_80063428;
using Weapon::UnkWeaponModel_80063428;

extern "C" {
/* Loads resource id (behaviour inferred). */
void fn_8006B50C(int id, int arg1);
/* The bullet types' records (behaviour inferred; see WeaponLaserRifle.cpp): fires bullet type type
 * with info, and runs its set-up and clean-up. */
void fn_80051C34(int type, const ShotInfo &info);
void fn_80051C80(int type);
void fn_80051CC8(int type);
/* Sets up the pick-up item of weapon type type (behaviour inferred). */
void fn_8005B760(int type);
/* The translation of a matrix. */
Vec3f fn_8040FB08(const Mtx34f *matrix);
/* A sound played at a position (see Fan.cpp). */
void fn_801D53C4(void *sound, int id, const Vec3f *position, void *source, int arg4, int arg5,
                 int arg6);
/* Starts effect id at a matrix for owner (see GunBattery.cpp). */
boost::weak_ptr<Effect::System::EffectElement> fn_8005E418(const Mtx34f *matrix, int id,
                                                           const boost::weak_ptr<CharInfo> &owner);
}

namespace {

class ShadowRifle : public Weapon::WeaponGunBase {
  public:
    ShadowRifle(const boost::weak_ptr<CharInfo> &owner, const UnkWeaponEffectSet_80063428 *effects)
        : Weapon::WeaponGunBase(owner, 0x43) {
        m_model = new UnkWeaponModel_80063428(owner, effects);
    }
    virtual ~ShadowRifle();
    virtual void SetTransform(const Vec3f *position, const Vec3f *rotation);
    virtual void Tick(float dt);
    virtual float Fire(const Vec3f *aim);
    virtual float FireAt(const Vec3f *aim, const Vec3f *target);

    /* Plays the firing sound and muzzle flash (name inferred). */
    void PlayFireEffect();
};
SIZE_ASSERT(ShadowRifle, 0x3C);

} // namespace

/* The name is read by the static initializer. */
static const char *s_name = "ShadowRifle";

/* The extra resource of the last effect (meaning inferred). */
static const UnkWeaponEffectSub_80063428 s_effectSubs[] = {
    {0x76, 0, 0},
};

/* The effects of the weapon model and of the item's model (meanings inferred). */
static const UnkWeaponEffect_80063428 s_effects[] = {
    {-1, 0x2328, 0x50, -1, 0, 0},
    {0x3E8, -1, 0x52, -1, 0, 0},
    {0x3E8, -1, 0x53, -1, 0, 0},
    {0x3E9, -1, 0x51, 0x65, s_effectSubs, 1},
};

static const u32 s_sounds[] = {0x2329};

static const UnkWeaponEffectSet_80063428 s_effectSet = {s_effects, 4, s_sounds, 1};

static const UnkWeaponEffect_80063428 s_itemEffects[] = {
    {-1, 0x801, 0x50, -1, 0, 0},
    {0x3E8, -1, 0x52, -1, 0, 0},
    {0x3E8, -1, 0x53, -1, 0, 0},
    {0x3E9, -1, 0x51, 0x65, s_effectSubs, 1},
};

static const u32 s_itemSounds[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSet = {s_itemEffects, 4, s_itemSounds, 2};

/* A distance (name inferred). As a call it is not a constant expression, so the record's values are
 * stored by the static initializer, as in retail. */
inline float Span(float length) {
    return length;
}

ShadowRifle::~ShadowRifle() {
    delete m_model;
}

void ShadowRifle::SetTransform(const Vec3f *position, const Vec3f *rotation) {
    Weapon::WeaponBase::SetTransform(position, rotation);
    m_model->fn_800632E8(position, rotation);
}

void ShadowRifle::Tick(float dt) {
    Weapon::WeaponGunBase::Tick(dt);
    m_model->fn_800632A0(dt);
}

float ShadowRifle::Fire(const Vec3f *aim) {
    fn_80051C34(m_type, fn_80069424(this, m_model->fn_80063234(0), aim));
    PlayFireEffect();
    fn_8005EB94(1);
    return 0.25f;
}

float ShadowRifle::FireAt(const Vec3f *aim, const Vec3f *target) {
    fn_80051C34(m_type, fn_80069298(this, m_model->fn_80063234(0), aim, target));
    PlayFireEffect();
    fn_8005EB94(1);
    return 0.25f;
}

void ShadowRifle::PlayFireEffect() {
    const Mtx34f *muzzle = m_model->fn_80063234(0);
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)fn_8040FB08(muzzle));
    fn_801D53C4(fn_80053164(), 0xE030, &position, 0, 1, 0, 0);
    fn_8005E418(m_model->fn_80063234(0), 0x27D, m_owner);
}

void WeaponSpecialShadowRifle::Initialize() {
    fn_8006B50C(0x50, 0);
    fn_8006B50C(0x52, 0);
    fn_8006B50C(0x53, 0);
    fn_8006B50C(0x51, 0);
    fn_8006B50C(0x65, 0);
    fn_8006B50C(0x76, 0);
    fn_80051CC8(0x43);
    fn_8005B760(0x43);
}

void WeaponSpecialShadowRifle::Finalize() {
    fn_80051C80(0x43);
}

/* The record's callbacks (names inferred). */
static Weapon::WeaponBase *Instantiate(const boost::weak_ptr<CharInfo> &owner) {
    return new ShadowRifle(owner, &s_effectSet);
}

static Weapon::Item *CreateItem() {
    return new Weapon::Item(&s_itemEffectSet);
}

UnkWeaponTypeInfo lbl_8054D518 = {
    s_name, Instantiate, CreateItem, 0, Span(1.0f), Span(5.0f), 0x14, 3, 0x2002, 0.0f, 0.0f, 0,
};
