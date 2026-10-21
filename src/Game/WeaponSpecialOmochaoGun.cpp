/* OmochaoGun (class name from RTTI: @unnamed@WeaponSpecialOmochaoGun_cpp@::OmochaoGun): a laser gun
 * (Weapon::WeaponLaserBase) that fires its own bullet type with a sound and keeps an effect running
 * at its muzzle, restarted after each shot and hidden with the weapon. Registered as two weapons,
 * OmochaoGun01 (type 0x3F) and OmochaoGun02 (type 0x40). File name from the RTTI name. Method names
 * marked "(name inferred)" are not original. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
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
using Weapon::UnkWeaponModel_80063428;

extern "C" {
/* Loads resource id (behaviour inferred). */
void fn_8006B50C(int id, int arg1);
/* The bullet types' records (behaviour inferred; see WeaponLaserRifle.cpp): fires bullet type type
 * with info, and runs its set-up. */
void fn_80051C34(int type, const ShotInfo &info);
void fn_80051CC8(int type);
/* Sets up the pick-up item of weapon type type (behaviour inferred). */
void fn_8005B760(int type);
/* The translation of a matrix. */
Vec3f fn_8040FB08(const Mtx34f *matrix);
/* Plays sound id at position (behaviour inferred). */
void fn_801D54F4(void *sound, u16 id, const Vec3f *position, int arg3, int arg4);
/* Starts effect id at a matrix for owner, following it (see WeaponVacuumPod.cpp). */
boost::weak_ptr<Effect::System::EffectElement> fn_8005E468(const Mtx34f *matrix, int id,
                                                           const boost::weak_ptr<CharInfo> &owner);
}

namespace {

class OmochaoGun : public Weapon::WeaponLaserBase {
  public:
    OmochaoGun(const boost::weak_ptr<CharInfo> &owner, int type,
               const UnkWeaponEffectSet_80063428 *effects);
    virtual ~OmochaoGun();
    virtual void SetTransform(const Vec3f *position, const Vec3f *rotation);
    virtual void Tick(float dt);
    virtual void Show();
    virtual void Hide();
    virtual float Fire(const Vec3f *aim);
    virtual float FireAt(const Vec3f *aim, const Vec3f *target);

    /* (Re)starts the muzzle effect (name inferred). */
    void StartEffect();
    /* Fires the bullet of info (name inferred). */
    float Shoot(ShotInfo &info);
    /* Plays the firing sound at the muzzle (name inferred). */
    void PlayFireSound();

    /* 0x48 */ u8 m_hidden;
    /* 0x4C */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
};
SIZE_ASSERT(OmochaoGun, 0x54);

} // namespace

/* The effects of the weapon model and of the item's model (meanings inferred). */
static const UnkWeaponEffect_80063428 s_effects[] = {
    {-1, 0x2328, 0x4F, -1, 0, 0},
};

static const u32 s_sounds[] = {0x2329};

static const UnkWeaponEffectSet_80063428 s_effectSet = {s_effects, 1, s_sounds, 1};

static const UnkWeaponEffect_80063428 s_itemEffects[] = {
    {-1, 0x801, 0x4F, -1, 0, 0},
};

static const u32 s_itemSounds[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSet = {s_itemEffects, 1, s_itemSounds, 2};

/* The names are read by the static initializer. */
static const char *s_name01 = "OmochaoGun01";
static const char *s_name02 = "OmochaoGun02";

/* A distance (name inferred). As a call it is not a constant expression, so the records' values are
 * stored by the static initializer, as in retail. */
inline float Span(float length) {
    return length;
}

OmochaoGun::OmochaoGun(const boost::weak_ptr<CharInfo> &owner, int type,
                       const UnkWeaponEffectSet_80063428 *effects)
    : Weapon::WeaponLaserBase(owner, type) {
    field_0x18 = 6;
    m_hidden = false;
    m_model = new UnkWeaponModel_80063428(owner, effects);
    StartEffect();
}

OmochaoGun::~OmochaoGun() {
    if (!m_effect.expired()) {
        m_effect.lock()->Retire();
    }
    if (m_model) {
        delete m_model;
    }
}

void OmochaoGun::SetTransform(const Vec3f *position, const Vec3f *rotation) {
    Weapon::WeaponBase::SetTransform(position, rotation);
    if (m_model) {
        m_model->fn_800632E8(position, rotation);
    }
}

void OmochaoGun::Tick(float dt) {
    Weapon::WeaponGunBase::Tick(dt);
    if (m_model) {
        m_model->fn_800632A0(dt);
    }
    if (ShotCount() > 0) {
        StartEffect();
    }
    if (!m_effect.expired()) {
        if (m_hidden) {
            m_effect.lock()->Hide();
        } else {
            m_effect.lock()->Show();
        }
    }
}

void OmochaoGun::Show() {
    if (m_model) {
        m_model->fn_80063130();
    }
    m_hidden = false;
}

void OmochaoGun::Hide() {
    if (m_model) {
        m_model->fn_80063154();
    }
    m_hidden = true;
}

float OmochaoGun::Fire(const Vec3f *aim) {
    ShotInfo info = fn_80069424(this, m_model->fn_80063234(0), aim);
    info.m_callback = m_hitCallback;
    return Shoot(info);
}

float OmochaoGun::FireAt(const Vec3f *aim, const Vec3f *target) {
    ShotInfo info = fn_80069298(this, m_model->fn_80063234(0), aim, target);
    info.m_callback = m_hitCallback;
    return Shoot(info);
}

void OmochaoGun::StartEffect() {
    if (!m_effect.expired()) {
        m_effect.lock()->Retire();
    }
    m_effect = fn_8005E468(m_model->fn_80063234(0), 0x1DF, m_owner);
}

float OmochaoGun::Shoot(ShotInfo &info) {
    fn_80051C34(m_type, info);
    PlayFireSound();
    fn_8005EB94(1);
    return -1.0f;
}

void OmochaoGun::PlayFireSound() {
    const Mtx34f *muzzle = m_model->fn_80063234(0);
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)fn_8040FB08(muzzle));
    fn_801D54F4(fn_80053164(), 0xE029, &position, 0, 1);
}

void WeaponSpecialOmochaoGun::Initialize() {
    fn_8006B50C(0x4F, 0);
    fn_80051CC8(0x3F);
    fn_80051CC8(0x40);
    fn_8005B760(0x3F);
    fn_8005B760(0x40);
}

void WeaponSpecialOmochaoGun::Finalize() {}

/* The records' callbacks (names inferred). */
static Weapon::WeaponBase *Instantiate01(const boost::weak_ptr<CharInfo> &owner) {
    return new OmochaoGun(owner, 0x3F, &s_effectSet);
}

static Weapon::Item *CreateItem01() {
    return new Weapon::Item(&s_itemEffectSet);
}

static Weapon::WeaponBase *Instantiate02(const boost::weak_ptr<CharInfo> &owner) {
    return new OmochaoGun(owner, 0x40, &s_effectSet);
}

static Weapon::Item *CreateItem02() {
    return new Weapon::Item(&s_itemEffectSet);
}

UnkWeaponTypeInfo lbl_8054D380 = {
    s_name01, Instantiate01, CreateItem01, 0, Span(1.0f), Span(5.0f), 0xA, 1, 0x2002, 1.0f, 0.5f, 0,
};

UnkWeaponTypeInfo lbl_8054D3B0 = {
    s_name02, Instantiate02, CreateItem02, 0, Span(1.0f), Span(5.0f), 0x14, 1, 0x2002, 1.0f, 0.5f, 0,
};
