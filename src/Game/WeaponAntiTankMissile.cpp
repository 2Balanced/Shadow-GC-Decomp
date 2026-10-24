/* AntiTankMissile (class name from RTTI: Weapon::AntiTankMissile): a lock-on weapon, weapon type
 * 0x12, that holds one target and fires its weapon type's bullet at it from the model's node 2,
 * with a sound and two effects. The code is Game/WeaponWormShooter.cpp's without the target marks.
 * File name inferred from the class name. Method names are inferred. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/GadgetUtil.h"
#include "Game/WeaponBase.h"
#include "Game/WeaponBullet.h"
#include "Game/WeaponItem.h"
#include "Game/WeaponLockonBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Weapon::LockonTarget;
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
/* The range of weapon type type (behaviour inferred from its use as the lock-on range). */
float fn_80051AC8(int type);
/* Enables weapon type type (Game/WeaponTable.cpp). */
void fn_8005B760(int type);
/* The translation of a matrix. */
Vec3f fn_8040FB08(const Mtx34f *matrix);
/* Plays sound id at position (see Fan.cpp), and sound id (see GunBattery.cpp). */
void fn_801D53C4(void *sound, int id, const Vec3f *position, void *source, int arg4, int arg5,
                 int arg6);
void fn_801D560C(void *sound, int id, int arg2, int arg3);
/* Starts effect id at a matrix for owner (see GunBattery.cpp). */
boost::weak_ptr<Effect::System::EffectElement> fn_8005E418(const Mtx34f *matrix, int id,
                                                           const boost::weak_ptr<CharInfo> &owner);
/* Builds the shot of a weapon from a muzzle matrix toward a locked target (see
 * Game/WeaponWormShooter.cpp). */
ShotInfo fn_800690E4(Weapon::WeaponBase *weapon, const Mtx34f *muzzle,
                     const boost::shared_ptr<LockonTarget> &target);
}

static const char *s_taskName = "AntiTankMissile";

/* The effects of the weapon model and of the item's model (meanings inferred). */
static const UnkWeaponEffect_80063428 s_effects[] = {
    {-1, 0x2328, 0x41, -1, 0, 0},
    {0x233C, -1, 0x42, -1, 0, 0},
};

static const u32 s_sounds[] = {0x2332, 0x2346, 0x2329};

static const UnkWeaponEffectSet_80063428 s_effectSet = {s_effects, 2, s_sounds, 3};

static const UnkWeaponEffect_80063428 s_itemEffects[] = {
    {-1, 0x801, 0x41, -1, 0, 0},
    {0x233C, -1, 0x42, -1, 0, 0},
};

static const u32 s_itemSounds[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSet = {s_itemEffects, 2, s_itemSounds, 2};

namespace Weapon {

class AntiTankMissile : public WeaponLockonBase {
  public:
    AntiTankMissile(const boost::weak_ptr<CharInfo> &owner) : WeaponLockonBase(owner, 1, 0x12) {
        m_model = new UnkWeaponModel_80063428(owner, &s_effectSet);
    }
    virtual ~AntiTankMissile();
    virtual void SetTransform(const Vec3f *position, const Vec3f *rotation);
    virtual void Tick(float dt);
    virtual bool vf_0x2C() {
        return 1;
    }
    virtual void vf_0x30();
    virtual float vf_0x34(const boost::shared_ptr<LockonTarget> &target, int index);

    /* Plays the firing sound and effects. */
    void PlayFireEffect();

    static WeaponBase *Instantiate(const boost::weak_ptr<CharInfo> &owner);
    static Item *CreateItem();
};
SIZE_ASSERT(AntiTankMissile, 0x3C);

} // namespace Weapon

/* A distance (name inferred). As a call it is not a constant expression, so the records' values are
 * stored by the static initializer, as in retail. */
inline float Span(float length) {
    return length;
}

/* The type's load and release hooks (names inferred). */
static void Initialize() {
    fn_803A953C("%s::Initialize\n", s_taskName);
    fn_8006B50C(0x41, 0);
    fn_8006B50C(0x42, 0);
    fn_80051CC8(0x12);
    fn_8005B760(0x12);
}

static void Finalize() {
    fn_803A953C("%s::Finalize\n", s_taskName);
    fn_80051C80(0x12);
}

namespace Weapon {

AntiTankMissile::~AntiTankMissile() {
    delete m_model;
}

void AntiTankMissile::SetTransform(const Vec3f *position, const Vec3f *rotation) {
    WeaponBase::SetTransform(position, rotation);
    m_model->fn_800632E8(position, rotation);
}

void AntiTankMissile::Tick(float dt) {
    fn_8005FAF0();
    m_model->fn_800632A0(dt);
    float range = fn_80051AC8(m_type);
    fn_8005F854(m_model->fn_80063234(1), range);
    fn_8005FACC(dt);
}

float AntiTankMissile::vf_0x34(const boost::shared_ptr<LockonTarget> &target, int index) {
    fn_80051C34(m_type, fn_800690E4(this, m_model->fn_80063234(2), target));
    PlayFireEffect();
    fn_8005F878(1);
    return 0.25f;
}

void AntiTankMissile::vf_0x30() {
    fn_801D560C(fn_80053164(), 0x201D, 0, 0);
}

void AntiTankMissile::PlayFireEffect() {
    const Mtx34f *muzzle = m_model->fn_80063234(0);
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)fn_8040FB08(muzzle));
    fn_801D53C4(fn_80053164(), 0x201E, &position, 0, 1, 0, 0);
    fn_8005E418(m_model->fn_80063234(2), 0x166, m_owner);
    fn_8005E418(m_model->fn_80063234(0), 0x169, m_owner);
}

WeaponBase *AntiTankMissile::Instantiate(const boost::weak_ptr<CharInfo> &owner) {
    return new AntiTankMissile(owner);
}

Item *AntiTankMissile::CreateItem() {
    return new Item(&s_itemEffectSet);
}

} // namespace Weapon

UnkWeaponTaskInfo lbl_8051E4C8 = {
    s_taskName, Initialize, Finalize, 0, 0, 0x80, 0x00E60400, 0, 0,
};

UnkWeaponTypeInfo lbl_8051E4EC = {
    s_taskName, Weapon::AntiTankMissile::Instantiate, Weapon::AntiTankMissile::CreateItem, 0, Span(1.0f),
    Span(5.0f), 6, 1, 0x2002, 1.0f, 1.0f, 0x3F800000,
};
