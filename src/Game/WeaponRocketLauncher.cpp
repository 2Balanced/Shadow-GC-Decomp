/* RocketLauncher (class name from RTTI: Weapon::RocketLauncher): a lock-on launcher that fires one
 * homing rocket per locked target from the model's nodes 2 and up, with a sound and effects at the
 * muzzle and a flash of the model. The unit registers two weapons with it, RocketLauncher4 (type
 * 0x13, four targets) and RocketLauncher8 (0x14, eight), which differ in resources and model nodes.
 * The code is close to Game/WeaponWormShooter.cpp's. Other units call the owner of a weapon
 * (0x80065034) by its address. File name from the class name; method names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined first).
 * The unit's code reaches its .data through a pooled base address, so the linker keeps that section
 * whole: Game/TNumEdit.h supplies the RTTI base lists it starts with. */
#include "Game/TNumEdit.h"
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
/* The range of weapon type type (see Game/WeaponWormShooter.cpp). */
float fn_80051AC8(int type);
/* Enables weapon type type (Game/WeaponTable.cpp). */
void fn_8005B760(int type);
/* The translation of a matrix. */
Vec3f fn_8040FB08(const Mtx34f *matrix);
/* Plays sound id at position, and sound id (see Game/WeaponWormShooter.cpp). */
void fn_801D54F4(void *sound, int id, const Vec3f *position, int arg3, int arg4);
void fn_801D560C(void *sound, int id, int arg2, int arg3);
/* Starts effect id at a matrix for owner (see GunBattery.cpp). */
boost::weak_ptr<Effect::System::EffectElement> fn_8005E418(const Mtx34f *matrix, int id,
                                                           const boost::weak_ptr<CharInfo> &owner);
/* Builds the shot of a weapon from a muzzle matrix toward a locked target (see
 * Game/WeaponWormShooter.cpp). */
ShotInfo fn_800690E4(Weapon::WeaponBase *weapon, const Mtx34f *muzzle,
                     const boost::shared_ptr<LockonTarget> &target);
}

/* An empty character (used as "no owner"; see GunBattery.cpp). */
extern boost::shared_ptr<CharInfo> lbl_805EF980;

namespace Weapon {

class RocketLauncher : public WeaponLockonBase {
  public:
    RocketLauncher(const boost::weak_ptr<CharInfo> &owner, int count, int type,
                   const UnkWeaponEffectSet_80063428 *effects)
        : WeaponLockonBase(owner, count, type) {
        m_model = new UnkWeaponModel_80063428(owner, effects);
    }
    virtual ~RocketLauncher();
    virtual void SetTransform(const Vec3f *position, const Vec3f *rotation);
    virtual void Tick(float dt);
    virtual bool vf_0x2C() {
        return 1;
    }
    virtual void vf_0x30();
    virtual float vf_0x34(const boost::shared_ptr<LockonTarget> &target, int index);

    /* Flashes the model and starts the muzzle effect. */
    void PlayMuzzleFlash();
};
SIZE_ASSERT(RocketLauncher, 0x3C);

} // namespace Weapon

/* The time between two rockets. */
static float s_reloadTime = 0.25f;

/* RocketLauncher4. The names and the most targets are read by the static initializer. */
static const char *s_name4 = "RocketLauncher4";
static s32 s_ammo4 = 0xC;

/* The parts and nodes of the weapon model, and the parts of the item's model (meanings
 * inferred). */
static const UnkWeaponEffect_80063428 s_effects4[] = {
    {-1, 0x2328, 0x43, -1, 0, 0},
    {0x233C, -1, 0x44, -1, 0, 0},
};

static const u32 s_sounds4[] = {0x2332, 0x2346, 0x2329, 0x232A, 0x232B, 0x232C};

static const UnkWeaponEffectSet_80063428 s_effectSet4 = {s_effects4, 2, s_sounds4, 6};

static const UnkWeaponEffect_80063428 s_itemEffects4[] = {
    {-1, 0x801, 0x43, -1, 0, 0},
    {0x233C, -1, 0x44, -1, 0, 0},
};

static const u32 s_itemSounds4[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSet4 = {s_itemEffects4, 2, s_itemSounds4, 2};

/* RocketLauncher8. */
static const char *s_name8 = "RocketLauncher8";
static s32 s_ammo8 = 0x18;

static const UnkWeaponEffect_80063428 s_effects8[] = {
    {-1, 0x2328, 0x45, -1, 0, 0},
    {0x233C, -1, 0x46, -1, 0, 0},
};

static const u32 s_sounds8[] = {0x2332, 0x2346, 0x2329, 0x232A, 0x232B,
                                0x232C, 0x232D, 0x232E, 0x232F, 0x2330};

static const UnkWeaponEffectSet_80063428 s_effectSet8 = {s_effects8, 2, s_sounds8, 10};

static const UnkWeaponEffect_80063428 s_itemEffects8[] = {
    {-1, 0x801, 0x45, -1, 0, 0},
    {0x233C, -1, 0x46, -1, 0, 0},
};

static const u32 s_itemSounds8[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSet8 = {s_itemEffects8, 2, s_itemSounds8, 2};

/* A distance (name inferred). As a call it is not a constant expression, so the records' values are
 * stored by the static initializer, as in retail. */
inline float Span(float length) {
    return length;
}

/* The owner of a weapon (other units call it by its address). It calls the inline weak_ptr copy
 * constructor, so it is emitted as a weak copy after its first caller. */
extern "C" inline boost::weak_ptr<CharInfo> fn_80065034(const Weapon::WeaponBase *weapon) {
    return weapon->m_owner;
}

namespace Weapon {

RocketLauncher::~RocketLauncher() {
    delete m_model;
}

void RocketLauncher::SetTransform(const Vec3f *position, const Vec3f *rotation) {
    WeaponBase::SetTransform(position, rotation);
    m_model->fn_800632E8(position, rotation);
}

void RocketLauncher::Tick(float dt) {
    fn_8005FAF0();
    m_model->fn_800632A0(dt);
    float range = fn_80051AC8(m_type);
    fn_8005F854(m_model->fn_80063234(1), range);
    fn_8005FACC(dt);
}

} // namespace Weapon

/* Plays the firing sound and effect at the muzzle (the type is unused). */
static void PlayFireEffect(int type, const Mtx34f *muzzle, const boost::weak_ptr<CharInfo> &owner);

namespace Weapon {

float RocketLauncher::vf_0x34(const boost::shared_ptr<LockonTarget> &target, int index) {
    const Mtx34f *muzzle = m_model->fn_80063234(index + 2);
    fn_80051C34(m_type, fn_800690E4(this, muzzle, target));
    PlayMuzzleFlash();
    PlayFireEffect(m_type, muzzle, fn_80065034(this));
    fn_8005F878(1);
    return s_reloadTime;
}

void RocketLauncher::vf_0x30() {
    fn_801D560C(fn_80053164(), 0x201D, 0, 0);
}

} // namespace Weapon

static void PlayFireEffect(int type, const Mtx34f *muzzle, const boost::weak_ptr<CharInfo> &owner) {
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)fn_8040FB08(muzzle));
    fn_801D54F4(fn_80053164(), 0x201E, &position, 0, 1);
    fn_8005E418(muzzle, 0x166, owner);
}

namespace Weapon {

void RocketLauncher::PlayMuzzleFlash() {
    m_model->fn_80063258(0);
    fn_8005E418(m_model->fn_80063234(0), 0x169, m_owner);
}

} // namespace Weapon

/* RocketLauncher4's callbacks (names inferred). */
static void Initialize4() {
    fn_803A953C("%s::Initialize\n", s_name4);
    fn_8006B50C(0x43, 0);
    fn_8006B50C(0x44, 0);
    fn_80051CC8(0x13);
    fn_8005B760(0x13);
}

static void Finalize4() {
    fn_803A953C("%s::Finalize\n", s_name4);
    fn_80051C80(0x13);
}

static Weapon::WeaponBase *Instantiate4(const boost::weak_ptr<CharInfo> &owner) {
    return new Weapon::RocketLauncher(owner, 4, 0x13, &s_effectSet4);
}

static Weapon::Item *CreateItem4() {
    return new Weapon::Item(&s_itemEffectSet4);
}

static void FireEffect4(const Mtx34f *muzzle) {
    PlayFireEffect(0x13, muzzle, lbl_805EF980);
}

/* RocketLauncher8's callbacks (names inferred). */
static void Initialize8() {
    fn_803A953C("%s::Initialize\n", s_name8);
    fn_8006B50C(0x45, 0);
    fn_8006B50C(0x46, 0);
    fn_80051CC8(0x14);
    fn_8005B760(0x14);
}

static void Finalize8() {
    fn_803A953C("%s::Finalize\n", s_name8);
    fn_80051C80(0x14);
}

static Weapon::WeaponBase *Instantiate8(const boost::weak_ptr<CharInfo> &owner) {
    return new Weapon::RocketLauncher(owner, 8, 0x14, &s_effectSet8);
}

static Weapon::Item *CreateItem8() {
    return new Weapon::Item(&s_itemEffectSet8);
}

UnkWeaponTaskInfo lbl_8051EE84 = {
    s_name4, Initialize4, Finalize4, 0, 0, 0x80, 0x00E70400, 0, 0,
};

UnkWeaponTypeInfo lbl_8051EEA8 = {
    s_name4, Instantiate4, CreateItem4, FireEffect4, Span(1.0f), Span(5.0f),
    s_ammo4, 1, 0x2002, 1.0f, 1.0f, 0x3ECCCCCD,
};

UnkWeaponTaskInfo lbl_8051EED8 = {
    s_name8, Initialize8, Finalize8, 0, 0, 0x80, 0x00E80400, 0, 0,
};

UnkWeaponTypeInfo lbl_8051EEFC = {
    s_name8, Instantiate8, CreateItem8, 0, Span(1.0f), Span(5.0f),
    s_ammo8, 1, 0x2002, 1.0f, 1.0f, 0x3ECCCCCD,
};
