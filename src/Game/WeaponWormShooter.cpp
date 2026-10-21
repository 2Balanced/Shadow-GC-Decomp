/* WormShooterBase (class name from RTTI: @unnamed@WeaponWormShooter_cpp@::WormShooterBase): a lock-on
 * gun that fires one homing shot per locked target, shown by a small model per target. The unit
 * registers three weapons with it, WormShooter (type 0x15), WideWormShooter (0x16) and
 * BigWormShooter (0x17), which differ in resources, effects and the most targets. File name from
 * the RTTI name. Method names marked "(name inferred)" are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). The unit's code reaches its
 * .data through a pooled base address, so the linker keeps that section whole: Game/TNumEdit.h
 * supplies the RTTI base lists it starts with. */
/* boost::scoped_array needs it (see boost/checked_delete.hpp). */
#define BOOST_CHECKED_ARRAY_DELETE

#include "Game/TNumEdit.h"
#include "Game/CharInfo.h"
#include "Game/GadgetUtil.h"
#include "Game/WeaponBase.h"
#include "Game/WeaponBullet.h"
#include "Game/WeaponItem.h"
#include "Game/WeaponLockonBase.h"
#include "boost/scoped_array.hpp"
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
/* Loads and releases resource id (behaviour inferred). */
void fn_8006B50C(int id, int arg1);
/* The bullet types' records (behaviour inferred; see WeaponLaserRifle.cpp): fires bullet type type
 * with info, and runs its set-up and clean-up. */
void fn_80051C34(int type, const ShotInfo &info);
void fn_80051C80(int type);
void fn_80051CC8(int type);
/* The range of weapon type type (behaviour inferred from its use as the lock-on range). */
float fn_80051AC8(int type);
/* Sets up the pick-up item of weapon type type (behaviour inferred). */
void fn_8005B760(int type);
/* The translation of a matrix. */
Vec3f fn_8040FB08(const Mtx34f *matrix);
/* Plays sound id at position (behaviour inferred). */
void fn_801D54F4(void *sound, int id, const Vec3f *position, int arg3, int arg4);
/* Plays sound id (see GunBattery.cpp). */
void fn_801D560C(void *sound, int id, int arg2, int arg3);
/* The owner of a weapon (behaviour inferred). */
boost::weak_ptr<CharInfo> fn_80065034(const Weapon::WeaponBase *weapon);
/* Builds the shot of a weapon from a muzzle matrix toward a locked target (behaviour inferred). */
ShotInfo fn_800690E4(Weapon::WeaponBase *weapon, const Mtx34f *muzzle,
                     const boost::shared_ptr<LockonTarget> &target);
}

/* An empty character (used as "no owner"; see GunBattery.cpp). */
extern boost::shared_ptr<CharInfo> lbl_805EF980;

namespace {

class WormShooterBase : public Weapon::WeaponLockonBase {
  public:
    WormShooterBase(const boost::weak_ptr<CharInfo> &owner, int type, int count,
                    const UnkWeaponEffectSet_80063428 *effects,
                    const UnkWeaponEffectSet_80063428 *markEffects);
    virtual ~WormShooterBase();
    virtual void SetTransform(const Vec3f *position, const Vec3f *rotation);
    virtual void Tick(float dt);
    virtual bool vf_0x2C() {
        return 1;
    }
    virtual void vf_0x30();
    virtual float vf_0x34(const boost::shared_ptr<LockonTarget> &target, int index);

    /* 0x3C */ boost::scoped_array<UnkWeaponModel_80063428 *> m_marks; /* one per target */
    /* 0x40 */ s32 m_fired; /* the marks of the targets already fired at (name inferred) */
};
SIZE_ASSERT(WormShooterBase, 0x44);

} // namespace

/* WormShooter. The names and the most targets are read by the static initializer. */
static const char *s_wormName = "WormShooter";
static s32 s_wormAmmo = 6;

/* The effects of the target marks, of the weapon model and of the item's model (meanings
 * inferred). */
static const UnkWeaponEffect_80063428 s_wormMarkEffects[] = {
    {-1, -1, 0x2D, -1, 0, 0},
};

static const UnkWeaponEffectSet_80063428 s_wormMarkEffectSet = {s_wormMarkEffects, 1, 0, 0};

static const UnkWeaponEffect_80063428 s_wormEffects[] = {
    {-1, 0x2328, 0x29, -1, 0, 0},
    {0x238C, -1, 0x23, -1, 0, 0},
    {0x238C, -1, 0x24, -1, 0, 0},
};

static const u32 s_wormSounds[] = {0x2329, 0x2329, 0x232A};

static const UnkWeaponEffectSet_80063428 s_wormEffectSet = {s_wormEffects, 3, s_wormSounds, 3};

static const UnkWeaponEffect_80063428 s_wormItemEffects[] = {
    {-1, 0x801, 0x29, -1, 0, 0},
    {0x238C, -1, 0x23, -1, 0, 0},
    {0x238C, -1, 0x24, -1, 0, 0},
};

static const u32 s_wormItemSounds[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_wormItemEffectSet = {s_wormItemEffects, 3,
                                                                s_wormItemSounds, 2};

/* WideWormShooter. */
static const char *s_wideName = "WideWormShooter";
static s32 s_wideAmmo = 0x12;

static const UnkWeaponEffect_80063428 s_wideMarkEffects[] = {
    {-1, -1, 0x2D, -1, 0, 0},
};

static const UnkWeaponEffectSet_80063428 s_wideMarkEffectSet = {s_wideMarkEffects, 1, 0, 0};

static const UnkWeaponEffect_80063428 s_wideEffects[] = {
    {-1, 0x2328, 0x2A, -1, 0, 0},
    {0x238C, -1, 0x25, -1, 0, 0},
    {0x238C, -1, 0x26, -1, 0, 0},
};

static const u32 s_wideSounds[] = {0x2329, 0x2329, 0x232A, 0x232B, 0x232C, 0x232D, 0x232E};

static const UnkWeaponEffectSet_80063428 s_wideEffectSet = {s_wideEffects, 3, s_wideSounds, 7};

static const UnkWeaponEffect_80063428 s_wideItemEffects[] = {
    {-1, 0x801, 0x2A, -1, 0, 0},
    {0x238C, -1, 0x25, -1, 0, 0},
    {0x238C, -1, 0x26, -1, 0, 0},
};

static const u32 s_wideItemSounds[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_wideItemEffectSet = {s_wideItemEffects, 3,
                                                                s_wideItemSounds, 2};

/* BigWormShooter. */
static const char *s_bigName = "BigWormShooter";
static s32 s_bigAmmo = 8;

static const UnkWeaponEffect_80063428 s_bigMarkEffects[] = {
    {-1, -1, 0x2C, -1, 0, 0},
};

static const UnkWeaponEffectSet_80063428 s_bigMarkEffectSet = {s_bigMarkEffects, 1, 0, 0};

static const UnkWeaponEffect_80063428 s_bigEffects[] = {
    {-1, 0x2328, 0x2B, -1, 0, 0},
    {0x238C, -1, 0x27, -1, 0, 0},
    {0x238C, -1, 0x28, -1, 0, 0},
};

static const u32 s_bigSounds[] = {0x2329, 0x2329, 0x232A};

static const UnkWeaponEffectSet_80063428 s_bigEffectSet = {s_bigEffects, 3, s_bigSounds, 3};

static const UnkWeaponEffect_80063428 s_bigItemEffects[] = {
    {-1, 0x801, 0x2B, -1, 0, 0},
    {0x238C, -1, 0x27, -1, 0, 0},
    {0x238C, -1, 0x28, -1, 0, 0},
};

static const u32 s_bigItemSounds[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_bigItemEffectSet = {s_bigItemEffects, 3,
                                                               s_bigItemSounds, 2};

/* A distance (name inferred). As a call it is not a constant expression, so the records' values are
 * stored by the static initializer, as in retail. */
inline float Span(float length) {
    return length;
}

WormShooterBase::WormShooterBase(const boost::weak_ptr<CharInfo> &owner, int type, int count,
                                 const UnkWeaponEffectSet_80063428 *effects,
                                 const UnkWeaponEffectSet_80063428 *markEffects)
    : Weapon::WeaponLockonBase(owner, count, type) {
    m_model = new UnkWeaponModel_80063428(owner, effects);
    m_marks.reset(new UnkWeaponModel_80063428 *[count]);
    for (int i = 0; i < count; i++) {
        UnkWeaponModel_80063428 *mark = new UnkWeaponModel_80063428(owner, markEffects);
        mark->fn_80063154();
        m_marks[i] = mark;
    }
    m_fired = 0;
}

WormShooterBase::~WormShooterBase() {
    delete m_model;
    int count = fn_8005F9EC();
    for (int i = 0; i < count; i++) {
        delete m_marks[i];
        m_marks[i] = 0;
    }
}

void WormShooterBase::SetTransform(const Vec3f *position, const Vec3f *rotation) {
    Weapon::WeaponBase::SetTransform(position, rotation);
    m_model->fn_800632E8(position, rotation);
}

void WormShooterBase::Tick(float dt) {
    fn_8005FAF0();
    m_model->fn_800632A0(dt);
    float range = fn_80051AC8(m_type);
    fn_8005F854(m_model->fn_80063234(0), range);
    fn_8005FACC(dt);
    int locked = fn_8005F9C8();
    if (locked == 0) {
        m_fired = 0;
    }
    int i;
    for (i = 0; i < m_fired; i++) {
        m_marks[i]->fn_80063154();
    }
    for (; i < locked; i++) {
        const Mtx34f *place = m_model->fn_80063234(i + 1);
        m_marks[i]->fn_800632C4(place);
        m_marks[i]->fn_80063130();
    }
    int count = fn_8005F9EC();
    for (; i < count; i++) {
        m_marks[i]->fn_80063154();
    }
}

/* Plays the firing sound at the muzzle (name inferred; the type and owner are unused). */
static void PlayFireSound(int type, const Mtx34f *muzzle, const boost::weak_ptr<CharInfo> &owner);

float WormShooterBase::vf_0x34(const boost::shared_ptr<LockonTarget> &target, int index) {
    const Mtx34f *muzzle = m_model->fn_80063234(index + 1);
    m_fired = index + 1;
    fn_80051C34(m_type, fn_800690E4(this, muzzle, target));
    PlayFireSound(m_type, muzzle, fn_80065034(this));
    fn_8005F878(1);
    return 0.25f;
}

void WormShooterBase::vf_0x30() {
    fn_801D560C(fn_80053164(), 0x2020, 0, 0);
}

static void PlayFireSound(int type, const Mtx34f *muzzle, const boost::weak_ptr<CharInfo> &owner) {
    Vec3f position;

    fn_800091EC(&position, &(const Vec3f &)fn_8040FB08(muzzle));
    fn_801D54F4(fn_80053164(), 0x2021, &position, 0, 1);
}

/* WormShooter's callbacks (names inferred). */
static void WormInitialize() {
    fn_803A953C("%s::Initialize\n", s_wormName);
    fn_8006B50C(0x23, 0);
    fn_8006B50C(0x24, 0);
    fn_8006B50C(0x29, 0);
    fn_8006B50C(0x2D, 0);
    fn_80051CC8(0x15);
    fn_8005B760(0x15);
}

static void WormFinalize() {
    fn_803A953C("%s::Finalize\n", s_wormName);
    fn_80051C80(0x15);
}

static Weapon::WeaponBase *WormInstantiate(const boost::weak_ptr<CharInfo> &owner) {
    return new WormShooterBase(owner, 0x15, 2, &s_wormEffectSet, &s_wormMarkEffectSet);
}

static Weapon::Item *WormCreateItem() {
    return new Weapon::Item(&s_wormItemEffectSet);
}

static void WormFireEffect(const Mtx34f *muzzle) {
    PlayFireSound(0x15, muzzle, lbl_805EF980);
}

/* WideWormShooter's callbacks (names inferred). */
static void WideInitialize() {
    fn_803A953C("%s::Initialize\n", s_wideName);
    fn_8006B50C(0x25, 0);
    fn_8006B50C(0x26, 0);
    fn_8006B50C(0x2A, 0);
    fn_8006B50C(0x2D, 0);
    fn_80051CC8(0x16);
    fn_8005B760(0x16);
}

static void WideFinalize() {
    fn_803A953C("%s::Finalize\n", s_wideName);
    fn_80051C80(0x16);
}

static Weapon::WeaponBase *WideInstantiate(const boost::weak_ptr<CharInfo> &owner) {
    return new WormShooterBase(owner, 0x16, 6, &s_wideEffectSet, &s_wideMarkEffectSet);
}

static Weapon::Item *WideCreateItem() {
    return new Weapon::Item(&s_wideItemEffectSet);
}

static void WideFireEffect(const Mtx34f *muzzle) {
    PlayFireSound(0x16, muzzle, lbl_805EF980);
}

/* BigWormShooter's callbacks (names inferred). */
static void BigInitialize() {
    fn_803A953C("%s::Initialize\n", s_bigName);
    fn_8006B50C(0x27, 0);
    fn_8006B50C(0x28, 0);
    fn_8006B50C(0x2B, 0);
    fn_8006B50C(0x2C, 0);
    fn_80051CC8(0x17);
    fn_8005B760(0x17);
}

static void BigFinalize() {
    fn_803A953C("%s::Finalize\n", s_bigName);
    fn_80051C80(0x17);
}

static Weapon::WeaponBase *BigInstantiate(const boost::weak_ptr<CharInfo> &owner) {
    return new WormShooterBase(owner, 0x17, 2, &s_bigEffectSet, &s_bigMarkEffectSet);
}

static Weapon::Item *BigCreateItem() {
    return new Weapon::Item(&s_bigItemEffectSet);
}

static void BigFireEffect(const Mtx34f *muzzle) {
    PlayFireSound(0x17, muzzle, lbl_805EF980);
}

UnkWeaponTaskInfo lbl_8054D094 = {
    s_wormName, WormInitialize, WormFinalize, 0, 0, 0x80, 0x00E90400, 0, 0,
};

UnkWeaponTypeInfo lbl_8054D0B8 = {
    s_wormName, WormInstantiate, WormCreateItem, WormFireEffect, Span(1.0f), Span(5.0f),
    s_wormAmmo, 1, 0x2003, 1.0f, 1.0f, 0x3ECCCCCD,
};

UnkWeaponTaskInfo lbl_8054D0E8 = {
    s_wideName, WideInitialize, WideFinalize, 0, 0, 0x80, 0x00EA0400, 0, 0,
};

UnkWeaponTypeInfo lbl_8054D10C = {
    s_wideName, WideInstantiate, WideCreateItem, WideFireEffect, Span(1.0f), Span(5.0f),
    s_wideAmmo, 1, 0x2003, 1.0f, 1.0f, 0x3ECCCCCD,
};

UnkWeaponTaskInfo lbl_8054D13C = {
    s_bigName, BigInitialize, BigFinalize, 0, 0, 0x80, 0x00EB0400, 0, 0,
};

UnkWeaponTypeInfo lbl_8054D160 = {
    s_bigName, BigInstantiate, BigCreateItem, BigFireEffect, Span(1.0f), Span(5.0f),
    s_bigAmmo, 1, 0x2003, 1.0f, 1.0f, 0x3ECCCCCD,
};
