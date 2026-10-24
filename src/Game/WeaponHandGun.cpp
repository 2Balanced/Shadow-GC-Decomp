/* HandGun (class name from RTTI: Weapon::HandGun): a gun, weapon type 1, that fires its weapon type's
 * bullets with a sound, a muzzle flash and a second effect at the model's node 1. The code is
 * Game/WeaponAutoRifle.cpp's for another weapon type, resources, sound and effects. File name
 * inferred from the class name. Method names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
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

static const char *s_taskName = "HandGun";

/* The effects of the weapon model and of the item's model (meanings inferred). */
static const UnkWeaponEffectSub_80063428 s_effectSubs[] = {
    {-1, 0, 0},
    {0x74, 0, 0},
};

static const UnkWeaponEffect_80063428 s_effects[] = {
    {-1, 0x2328, 0x39, 0x63, s_effectSubs, 2},
};

static const u32 s_sounds[] = {0x2329, 0x2332};

static const UnkWeaponEffectSet_80063428 s_effectSet = {s_effects, 1, s_sounds, 2};

static const UnkWeaponEffect_80063428 s_itemEffects[] = {
    {-1, 0x801, 0x39, -1, 0, 0},
};

static const u32 s_itemSounds[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSet = {s_itemEffects, 1, s_itemSounds, 2};

namespace Weapon {

class HandGun : public WeaponGunBase {
  public:
    HandGun(const boost::weak_ptr<CharInfo> &owner) : WeaponGunBase(owner, 1) {
        m_model = new UnkWeaponModel_80063428(owner, &s_effectSet);
    }
    virtual ~HandGun();
    virtual void SetTransform(const Vec3f *position, const Vec3f *rotation);
    virtual void Tick(float dt);
    virtual float Fire(const Vec3f *aim);
    virtual float FireAt(const Vec3f *aim, const Vec3f *target);

    /* Plays the firing sound and effects. */
    void PlayFireEffect();

    static WeaponBase *Instantiate(const boost::weak_ptr<CharInfo> &owner);
    static Item *CreateItem();
};
SIZE_ASSERT(HandGun, 0x3C);

} // namespace Weapon

/* A distance (name inferred). As a call it is not a constant expression, so the records' values are
 * stored by the static initializer, as in retail. */
inline float Span(float length) {
    return length;
}

/* The type's load and release hooks (names inferred). */
static void Initialize() {
    fn_803A953C("%s::Initialize\n", s_taskName);
    fn_8006B50C(0x39, 0);
    fn_8006B50C(0x63, 0);
    fn_8006B50C(0x74, 0);
    fn_80051CC8(1);
    fn_8005B760(1);
}

static void Finalize() {
    fn_803A953C("%s::Finalize\n", s_taskName);
    fn_80051C80(1);
}

namespace Weapon {

HandGun::~HandGun() {
    delete m_model;
}

void HandGun::SetTransform(const Vec3f *position, const Vec3f *rotation) {
    WeaponBase::SetTransform(position, rotation);
    m_model->fn_800632E8(position, rotation);
}

void HandGun::Tick(float dt) {
    WeaponGunBase::Tick(dt);
    m_model->fn_800632A0(dt);
}

float HandGun::Fire(const Vec3f *aim) {
    fn_80051C34(m_type, fn_80069424(this, m_model->fn_80063234(0), aim));
    PlayFireEffect();
    fn_8005EB94(1);
    return -1.0f;
}

float HandGun::FireAt(const Vec3f *aim, const Vec3f *target) {
    fn_80051C34(m_type, fn_80069298(this, m_model->fn_80063234(0), aim, target));
    PlayFireEffect();
    fn_8005EB94(1);
    return -1.0f;
}

void HandGun::PlayFireEffect() {
    m_model->fn_80063258(1);
    const Mtx34f *muzzle = m_model->fn_80063234(0);
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)fn_8040FB08(muzzle));
    fn_801D53C4(fn_80053164(), 0x2008, &position, 0, 1, 0, 0);
    fn_8005E418(m_model->fn_80063234(0), 0x147, m_owner);
    fn_8005E418(m_model->fn_80063234(1), 0xD1, m_owner);
}

} // namespace Weapon

Weapon::WeaponBase *Weapon::HandGun::Instantiate(const boost::weak_ptr<CharInfo> &owner) {
    return new HandGun(owner);
}

Weapon::Item *Weapon::HandGun::CreateItem() {
    return new Item(&s_itemEffectSet);
}

UnkWeaponTaskInfo lbl_8051EB68 = {
    s_taskName, Initialize, Finalize, 0, 0, 0x80, 0x00C80400, 0, 0,
};

UnkWeaponTypeInfo lbl_8051EB8C = {
    s_taskName, Weapon::HandGun::Instantiate, Weapon::HandGun::CreateItem, 0, Span(1.0f), Span(5.0f),
    0xA, 1, 0x2002, 0.0f, 0.2f, 0,
};
