/* Reflector (class name from RTTI: @unnamed@WeaponReflector_cpp@::Reflector): a laser gun, weapon
 * type 0x1B. File name from the RTTI name. Its unit registers the weapon's records (initialize and
 * finalize hooks that load and release its resources, and the constructors of the weapon and its
 * item). Method names marked "(name inferred)" are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). WeaponSplitter.cpp is the
 * same code for weapon type 0x1A, and WeaponLaserRifle.cpp for type 0x19. */
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

class Reflector : public Weapon::WeaponLaserBase {
  public:
    Reflector(const boost::weak_ptr<CharInfo> &owner, const UnkWeaponEffectSet_80063428 *effects);
    virtual ~Reflector();
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
SIZE_ASSERT(Reflector, 0x48);

} // namespace

static const char *s_taskName = "Reflector";

/* The effects of the weapon model and of the item's model (meanings inferred). */
static const UnkWeaponEffectSub_80063428 s_effectSubs[] = {
    {0x6B, 0, 0},
};

static const UnkWeaponEffect_80063428 s_effects[] = {
    {-1, 0x2328, 0x14, -1, 0, 0},
    {0x238C, -1, 0x15, -1, 0, 0},
    {0x251C, -1, 0x16, 0x5C, s_effectSubs, 1},
};

static const u32 s_sounds[] = {0x2329};

static const UnkWeaponEffectSet_80063428 s_effectSet = {s_effects, 3, s_sounds, 1};

static const UnkWeaponEffect_80063428 s_itemEffects[] = {
    {-1, 0x801, 0x14, -1, 0, 0},
    {0x238C, -1, 0x15, -1, 0, 0},
    {0x251C, -1, 0x16, 0x5C, s_effectSubs, 1},
};

static const u32 s_itemSounds[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSet = {s_itemEffects, 3, s_itemSounds, 2};

/* A distance (name inferred). As a call it is not a constant expression, so the records' values are
 * stored by the static initializer, as in retail. */
inline float Span(float length) {
    return length;
}

Reflector::Reflector(const boost::weak_ptr<CharInfo> &owner, const UnkWeaponEffectSet_80063428 *effects)
    : Weapon::WeaponLaserBase(owner, 0x1B) {
    field_0x18 = 6;
    m_model = new UnkWeaponModel_80063428(owner, effects);
}

Reflector::~Reflector() {
    delete m_model;
}

void Reflector::SetTransform(const Vec3f *position, const Vec3f *rotation) {
    Weapon::WeaponBase::SetTransform(position, rotation);
    m_model->fn_800632E8(position, rotation);
}

void Reflector::Tick(float dt) {
    Weapon::WeaponGunBase::Tick(dt);
    m_model->fn_800632A0(dt);
}

float Reflector::Fire(const Vec3f *aim) {
    ShotInfo info = fn_80069424(this, m_model->fn_80063234(0), aim);
    info.m_callback = m_hitCallback;
    return Shoot(info);
}

float Reflector::FireAt(const Vec3f *aim, const Vec3f *target) {
    ShotInfo info = fn_80069298(this, m_model->fn_80063234(0), aim, target);
    info.m_callback = m_hitCallback;
    return Shoot(info);
}

float Reflector::Shoot(ShotInfo &info) {
    fn_80051C34(0x1B, &info);
    PlayFireEffect();
    fn_8005EB94(1);
    return -1.0f;
}

void Reflector::PlayFireEffect() {
    Vec3f position;

    const Mtx34f *muzzle = m_model->fn_80063234(0);
    fn_800091EC(&position, &(const Vec3f &)fn_8040FB08(muzzle));
    fn_801D53C4(fn_80053164(), 0x202A, &position, 0, 1, 0, 0);
    fn_8005E418(m_model->fn_80063234(0), 0x1A2, m_owner);
}

extern "C" void fn_80225390(void) {
    fn_803A953C("%s::Initialize\n", s_taskName);
    fn_8006B50C(0x5C, 0);
    fn_8006B50C(0x14, 0);
    fn_8006B50C(0x15, 0);
    fn_8006B50C(0x16, 0);
    fn_8006B50C(0x6B, 0);
    fn_80051CC8(0x1B);
    fn_8005B760(0x1B);
}

extern "C" void fn_80225358(void) {
    fn_803A953C("%s::Finalize\n", s_taskName);
    fn_80051C80(0x1B);
}

Weapon::WeaponBase *Reflector::Instantiate(const boost::weak_ptr<CharInfo> &owner) {
    return new Reflector(owner, &s_effectSet);
}

Weapon::Item *Reflector::CreateItem() {
    return new Weapon::Item(&s_itemEffectSet);
}

UnkWeaponTaskInfo lbl_805463A8 = {
    s_taskName, fn_80225390, fn_80225358, 0, 0, 0x80, 0x00F30400, 0, 0,
};

UnkWeaponTypeInfo lbl_805463CC = {
    s_taskName, Reflector::Instantiate, Reflector::CreateItem, 0, Span(1.0f), Span(5.0f),
    0x14, 3, 0x2003, 0.5f, 0.4f, 0,
};
