/* WeaponLaserRifle (file name from the RTTI of its class, @unnamed@WeaponLaserRifle_cpp@::LaserRifle):
 * the laser rifle, weapon type 0x19. It fires laser bullets (Game/BulletLaser.cpp) from its
 * model's muzzle, with a sound and a muzzle flash. The two records at the end register the weapon
 * and its pick-up item. Names marked "(name inferred)" are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include "Effect/System/EffectElement.h"
#include "Game/WeaponBase.h"
#include "Game/WeaponBullet.h"
#include "Game/WeaponItem.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Weapon::ShotInfo;
using Weapon::UnkWeaponEffect_80063428;
using Weapon::UnkWeaponEffectSet_80063428;
using Weapon::UnkWeaponModel_80063428;

extern "C" {
/* Prints a debug message (behaviour inferred). */
void fn_803A953C(const char *format, ...);
/* Loads and releases resource id (see FileQueue.h; behaviour inferred). */
void fn_8006B50C(int id, int arg1);
/* The bullet types' records (behaviour inferred): fires bullet type type with info, and runs its
 * set-up and clean-up. */
void fn_80051C34(int type, const ShotInfo *info);
void fn_80051CC8(int type);
void fn_80051C80(int type);
/* Sets up the pick-up item of weapon type type (behaviour inferred). */
void fn_8005B760(int type);
/* The sound manager, and a sound played at a position (see Fan.cpp). */
void *fn_80053164(void);
void fn_801D53C4(void *sound, int id, const Vec3f *position, void *source, int arg4, int arg5,
                 int arg6);
/* Starts effect id at a matrix for owner (see GunBattery.cpp). */
boost::weak_ptr<Effect::System::EffectElement> fn_8005E418(const Mtx34f *matrix, int id,
                                                           const boost::weak_ptr<CharInfo> &owner);
/* The translation of a matrix. */
Vec3f fn_8040FB08(const Mtx34f *matrix);
}

/* Returns x unchanged (name inferred). Being initialised through a function call, the values below
 * are set up at startup (see Meteor.cpp). */
inline float Plain(float x) {
    return x;
}

namespace {

class LaserRifle : public Weapon::WeaponLaserBase {
  public:
    LaserRifle(const boost::weak_ptr<CharInfo> &owner, const UnkWeaponEffectSet_80063428 *effects);
    virtual ~LaserRifle();
    virtual void SetTransform(const Vec3f *position, const Vec3f *rotation);
    virtual void Tick(float dt);
    virtual float Fire(const Vec3f *aim);
    virtual float FireAt(const Vec3f *aim, const Vec3f *target);

    /* Fires the bullet of info (name inferred). */
    float Shoot(ShotInfo &info);
    /* Plays the firing sound and muzzle flash (name inferred). */
    void PlayFireEffect();

    static const char *s_taskName;
};
SIZE_ASSERT(LaserRifle, 0x48);

const char *LaserRifle::s_taskName = "LaserRifle";

/* The effects of the weapon model and of the item's model (meanings inferred). */
static const UnkWeaponEffect_80063428 lbl_804F0BF4[] = {
    {-1, 0x2328, 0x3E, -1, 0, 0},
    {0x251C, -1, 0x3F, -1, 0, 0},
    {0x251C, -1, 0x40, -1, 0, 0},
};
static const u32 lbl_805F6DA8[] = {0x2329};
static const UnkWeaponEffectSet_80063428 lbl_804F0C3C = {lbl_804F0BF4, 3, lbl_805F6DA8, 1};

static const UnkWeaponEffect_80063428 lbl_804F0C4C[] = {
    {-1, 0x801, 0x3E, -1, 0, 0},
    {0x251C, -1, 0x3F, -1, 0, 0},
    {0x251C, -1, 0x40, -1, 0, 0},
};
static const u32 lbl_805F6DAC[] = {0x2328, 0x801};
static const UnkWeaponEffectSet_80063428 lbl_804F0C94 = {lbl_804F0C4C, 3, lbl_805F6DAC, 2};

LaserRifle::LaserRifle(const boost::weak_ptr<CharInfo> &owner, const UnkWeaponEffectSet_80063428 *effects)
    : WeaponLaserBase(owner, 0x19) {
    field_0x18 = 6;
    m_model = new UnkWeaponModel_80063428(owner, effects);
}

LaserRifle::~LaserRifle() {
    delete m_model;
}

void LaserRifle::SetTransform(const Vec3f *position, const Vec3f *rotation) {
    WeaponBase::SetTransform(position, rotation);
    m_model->fn_800632E8(position, rotation);
}

void LaserRifle::Tick(float dt) {
    WeaponGunBase::Tick(dt);
    m_model->fn_800632A0(dt);
}

float LaserRifle::Fire(const Vec3f *aim) {
    ShotInfo info = fn_80069424(this, m_model->fn_80063234(0), aim);
    info.m_callback = m_hitCallback;
    return Shoot(info);
}

float LaserRifle::FireAt(const Vec3f *aim, const Vec3f *target) {
    ShotInfo info = fn_80069298(this, m_model->fn_80063234(0), aim, target);
    info.m_callback = m_hitCallback;
    return Shoot(info);
}

float LaserRifle::Shoot(ShotInfo &info) {
    fn_80051C34(0x19, &info);
    PlayFireEffect();
    fn_8005EB94(1);
    return -1.0f;
}

void LaserRifle::PlayFireEffect() {
    const Mtx34f *muzzle = m_model->fn_80063234(0);
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)fn_8040FB08(muzzle));
    fn_801D53C4(fn_80053164(), 0x2028, &position, 0, 1, 0, 0);
    fn_8005E418(m_model->fn_80063234(0), 0x1A0, m_owner);
}

} // namespace

/* Sets up and cleans up the weapon type (names inferred). */
static void Initialize() {
    fn_803A953C("%s::Initialize\n", LaserRifle::s_taskName);
    fn_8006B50C(0x3E, 0);
    fn_8006B50C(0x3F, 0);
    fn_8006B50C(0x40, 0);
    fn_80051CC8(0x19);
    fn_8005B760(0x19);
}

static void Finalize() {
    fn_803A953C("%s::Finalize\n", LaserRifle::s_taskName);
    fn_80051C80(0x19);
}

/* Creates the weapon, and its pick-up item (names inferred). */
static Weapon::WeaponBase *CreateWeapon(const boost::weak_ptr<CharInfo> &owner) {
    return new LaserRifle(owner, &lbl_804F0C3C);
}

static Weapon::Item *CreateItem() {
    return new Weapon::Item(&lbl_804F0C94);
}

UnkWeaponTaskInfo lbl_80546140 = {
    LaserRifle::s_taskName, Initialize, Finalize, 0, 0, 0x80, 0x00F10400, 0, 0,
};

UnkWeaponTypeInfo lbl_80546164 = {
    LaserRifle::s_taskName, CreateWeapon, CreateItem, 0, Plain(1.0f), Plain(5.0f), 0x14, 1, 0x2002,
    0.5f, 0.4f, 0,
};
