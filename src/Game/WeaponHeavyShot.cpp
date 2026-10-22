/* HeavyShot (class name from RTTI: Weapon::HeavyShot): a gun, weapon type 0xB, that fires bullet
 * type 0xB with a sound and a muzzle flash. The code is WeaponSpecialShadowRifle.cpp's for another
 * weapon type, resources, sound and effect. File name inferred from the class name. Method names are
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
using Weapon::UnkWeaponEffectSub_80063428;
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
/* Sets up the pick-up item of weapon type type (behaviour inferred). */
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

static const char *s_taskName = "HeavyShot";

/* The effects of the weapon model and of the item's model (meanings inferred). */
static const UnkWeaponEffectSub_80063428 s_effectSubs[] = {
    {0x6E, 0, 0},
};

static const UnkWeaponEffect_80063428 s_effects[] = {
    {-1, 0x2328, 0x13, -1, 0, 0},
    {0x251C, -1, 0x1E, 0x5F, s_effectSubs, 1},
};

static const u32 s_sounds[] = {0x2329};

static const UnkWeaponEffectSet_80063428 s_effectSet = {s_effects, 2, s_sounds, 1};

static const UnkWeaponEffect_80063428 s_itemEffects[] = {
    {-1, 0x801, 0x13, -1, 0, 0},
    {0x251C, -1, 0x1E, 0x5F, s_effectSubs, 1},
};

static const u32 s_itemSounds[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSet = {s_itemEffects, 2, s_itemSounds, 2};

namespace Weapon {

class HeavyShot : public WeaponGunBase {
  public:
    HeavyShot(const boost::weak_ptr<CharInfo> &owner) : WeaponGunBase(owner, 0xB) {
        m_model = new UnkWeaponModel_80063428(owner, &s_effectSet);
    }
    virtual ~HeavyShot();
    virtual void SetTransform(const Vec3f *position, const Vec3f *rotation);
    virtual void Tick(float dt);
    virtual float Fire(const Vec3f *aim);
    virtual float FireAt(const Vec3f *aim, const Vec3f *target);

    /* Plays the firing sound and muzzle flash. */
    void PlayFireEffect();

    static WeaponBase *Instantiate(const boost::weak_ptr<CharInfo> &owner);
    static Item *CreateItem();
};
SIZE_ASSERT(HeavyShot, 0x3C);

} // namespace Weapon

/* A distance (name inferred). As a call it is not a constant expression, so the records' values are
 * stored by the static initializer, as in retail. */
inline float Span(float length) {
    return length;
}

namespace Weapon {

HeavyShot::~HeavyShot() {
    delete m_model;
}

void HeavyShot::SetTransform(const Vec3f *position, const Vec3f *rotation) {
    WeaponBase::SetTransform(position, rotation);
    m_model->fn_800632E8(position, rotation);
}

void HeavyShot::Tick(float dt) {
    WeaponGunBase::Tick(dt);
    m_model->fn_800632A0(dt);
}

float HeavyShot::Fire(const Vec3f *aim) {
    fn_80051C34(0xB, fn_80069424(this, m_model->fn_80063234(0), aim));
    PlayFireEffect();
    fn_8005EB94(1);
    return 0.125f;
}

float HeavyShot::FireAt(const Vec3f *aim, const Vec3f *target) {
    fn_80051C34(0xB, fn_80069298(this, m_model->fn_80063234(0), aim, target));
    PlayFireEffect();
    fn_8005EB94(1);
    return 0.125f;
}

void HeavyShot::PlayFireEffect() {
    const Mtx34f *muzzle = m_model->fn_80063234(0);
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)fn_8040FB08(muzzle));
    fn_801D53C4(fn_80053164(), 0x2012, &position, 0, 1, 0, 0);
    fn_8005E418(m_model->fn_80063234(0), 0x287, m_owner);
}

} // namespace Weapon

extern "C" void fn_802F188C(void) {
    fn_803A953C("%s::Initialize\n", s_taskName);
    fn_8006B50C(0x13, 0);
    fn_8006B50C(0x1E, 0);
    fn_8006B50C(0x5F, 0);
    fn_8006B50C(0x6E, 0);
    fn_80051CC8(0xB);
    fn_8005B760(0xB);
}

extern "C" void fn_802F1854(void) {
    fn_803A953C("%s::Finalize\n", s_taskName);
    fn_80051C80(0xB);
}

Weapon::WeaponBase *Weapon::HeavyShot::Instantiate(const boost::weak_ptr<CharInfo> &owner) {
    return new HeavyShot(owner);
}

Weapon::Item *Weapon::HeavyShot::CreateItem() {
    return new Item(&s_itemEffectSet);
}

UnkWeaponTaskInfo lbl_80559E60 = {
    s_taskName, fn_802F188C, fn_802F1854, 0, 0, 0x80, 0x00D20400, 0, 0,
};

UnkWeaponTypeInfo lbl_80559E84 = {
    s_taskName, Weapon::HeavyShot::Instantiate, Weapon::HeavyShot::CreateItem, 0, Span(1.0f), Span(5.0f),
    0x1E, 3, 0x2003, 0.0f, 0.2f, 0,
};
