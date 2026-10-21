/* Splitter (class name from RTTI: @unnamed@WeaponSplitter_cpp@::Splitter): a laser gun, weapon
 * type 0x1A. File name from the RTTI name. The code is WeaponReflector.cpp's for another weapon
 * type, resources, sound and effect. Method names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined first). */
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
/* Loads and releases resource id (behaviour inferred). */
void fn_8006B50C(int id, int arg1);
/* The bullet types' records (behaviour inferred; see WeaponLaserRifle.cpp): fires bullet type type
 * with info, and runs its set-up and clean-up. */
void fn_80051C34(int type, const ShotInfo *info);
void fn_80051C80(int type);
void fn_80051CC8(int type);
/* Sets up the pick-up item of weapon type type (behaviour inferred). */
void fn_8005B760(int type);
/* The translation of a matrix. */
Vec3f fn_8040FB08(const Mtx34f *matrix);
/* Plays sound id at position (see Fan.cpp). */
void fn_801D53C4(void *sound, int id, const Vec3f *position, void *source, int arg4,
                 int arg5, int arg6);
/* Starts effect id at a matrix for owner (see GunBattery.cpp). */
boost::weak_ptr<Effect::System::EffectElement> fn_8005E418(const Mtx34f *matrix, int id,
                                                           const boost::weak_ptr<CharInfo> &owner);
}

namespace {

class Splitter : public Weapon::WeaponLaserBase {
  public:
    Splitter(const boost::weak_ptr<CharInfo> &owner, const UnkWeaponEffectSet_80063428 *effects);
    virtual ~Splitter();
    virtual void SetTransform(const Vec3f *position, const Vec3f *rotation);
    virtual void Tick(float dt);
    virtual float Fire(const Vec3f *aim);
    virtual float FireAt(const Vec3f *aim, const Vec3f *target);

    /* Fires the bullet of info (name inferred). */
    float Shoot(ShotInfo &info);
    /* Plays the firing sound and muzzle flash (name inferred). */
    void PlayFireEffect();

    static Weapon::WeaponBase *Instantiate(const boost::weak_ptr<CharInfo> &owner);
    static Weapon::Item *CreateItem();
};
SIZE_ASSERT(Splitter, 0x48);

} // namespace

static const char *s_taskName = "Splitter";

/* The effects of the weapon model and of the item's model (meanings inferred). */
static const UnkWeaponEffectSub_80063428 s_effectSubs[] = {
    {0x6C, 0, 0},
};

static const UnkWeaponEffect_80063428 s_effects[] = {
    {-1, 0x2328, 0x17, -1, 0, 0},
    {0x251C, -1, 0x18, -1, 0, 0},
    {0x251C, -1, 0x19, 0x5D, s_effectSubs, 1},
};

static const u32 s_sounds[] = {0x2329};

static const UnkWeaponEffectSet_80063428 s_effectSet = {s_effects, 3, s_sounds, 1};

static const UnkWeaponEffect_80063428 s_itemEffects[] = {
    {-1, 0x801, 0x17, -1, 0, 0},
    {0x251C, -1, 0x18, -1, 0, 0},
    {0x251C, -1, 0x19, 0x5D, s_effectSubs, 1},
};

static const u32 s_itemSounds[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSet = {s_itemEffects, 3, s_itemSounds, 2};

/* A distance (name inferred). As a call it is not a constant expression, so the records' values are
 * stored by the static initializer, as in retail. */
inline float Span(float length) {
    return length;
}

Splitter::Splitter(const boost::weak_ptr<CharInfo> &owner, const UnkWeaponEffectSet_80063428 *effects)
    : Weapon::WeaponLaserBase(owner, 0x1A) {
    field_0x18 = 6;
    m_model = new UnkWeaponModel_80063428(owner, effects);
}

Splitter::~Splitter() {
    delete m_model;
}

void Splitter::SetTransform(const Vec3f *position, const Vec3f *rotation) {
    Weapon::WeaponBase::SetTransform(position, rotation);
    m_model->fn_800632E8(position, rotation);
}

void Splitter::Tick(float dt) {
    Weapon::WeaponGunBase::Tick(dt);
    m_model->fn_800632A0(dt);
}

float Splitter::Fire(const Vec3f *aim) {
    ShotInfo info = fn_80069424(this, m_model->fn_80063234(0), aim);
    info.m_callback = m_hitCallback;
    return Shoot(info);
}

float Splitter::FireAt(const Vec3f *aim, const Vec3f *target) {
    ShotInfo info = fn_80069298(this, m_model->fn_80063234(0), aim, target);
    info.m_callback = m_hitCallback;
    return Shoot(info);
}

float Splitter::Shoot(ShotInfo &info) {
    fn_80051C34(0x1A, &info);
    PlayFireEffect();
    fn_8005EB94(1);
    return -1.0f;
}

void Splitter::PlayFireEffect() {
    Vec3f position;

    const Mtx34f *muzzle = m_model->fn_80063234(0);
    fn_800091EC(&position, &(const Vec3f &)fn_8040FB08(muzzle));
    fn_801D53C4(fn_80053164(), 0x2029, &position, 0, 1, 0, 0);
    fn_8005E418(m_model->fn_80063234(0), 0x1AD, m_owner);
}

extern "C" void fn_8022587C(void) {
    fn_803A953C("%s::Initialize\n", s_taskName);
    fn_8006B50C(0x5D, 0);
    fn_8006B50C(0x17, 0);
    fn_8006B50C(0x18, 0);
    fn_8006B50C(0x19, 0);
    fn_8006B50C(0x6C, 0);
    fn_80051CC8(0x1A);
    fn_8005B760(0x1A);
}

extern "C" void fn_80225844(void) {
    fn_803A953C("%s::Finalize\n", s_taskName);
    fn_80051C80(0x1A);
}

Weapon::WeaponBase *Splitter::Instantiate(const boost::weak_ptr<CharInfo> &owner) {
    return new Splitter(owner, &s_effectSet);
}

Weapon::Item *Splitter::CreateItem() {
    return new Weapon::Item(&s_itemEffectSet);
}

UnkWeaponTaskInfo lbl_80546480 = {
    s_taskName, fn_8022587C, fn_80225844, 0, 0, 0x80, 0x00F20400, 0, 0,
};

UnkWeaponTypeInfo lbl_805464A4 = {
    s_taskName, Splitter::Instantiate, Splitter::CreateItem, 0, Span(1.0f), Span(5.0f),
    0x14, 3, 0x2003, 0.5f, 0.4f, 0,
};
