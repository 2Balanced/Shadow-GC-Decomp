/* VacuumPod (class name from RTTI: Weapon::VacuumPod): a vacuum weapon, weapon type 0x18. While it
 * pulls, it looks for the nearest target in front of its muzzle: a player is pushed back (or hurt
 * when very close), any other character is sent a VacuumCommand that hands it to the weapon. File
 * name inferred from the other weapon units. Method names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
/* Enables boost::static_pointer_cast (see boost/shared_ptr.hpp). */
#define BOOST_SP_STATIC_POINTER_CAST
#include "Effect/System/EffectElement.h"
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/GadgetCharCommands.h"
#include "Game/GadgetSound.h"
#include "Game/GadgetUtil.h"
#include "Game/WeaponBase.h"
#include "Game/WeaponItem.h"
#include "Game/WeaponVacuumBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Weapon::UnkWeaponEffect_80063428;
using Weapon::UnkWeaponEffectSet_80063428;
using Weapon::UnkWeaponEffectSub_80063428;
using Weapon::UnkWeaponModel_80063428;
using Weapon::WeaponVacuumBase;

/* Target (class name from RTTI): a point other objects can aim at; the character it belongs to is
 * at 0x28 (name inferred). */
class Target {
  public:
    /* 0x00 */ u8 field_0x0[0x28];
    /* 0x28 */ boost::weak_ptr<CharInfo> m_owner;
};

extern "C" {
/* Debug output (behaviour inferred). */
void fn_803A953C(const char *format, ...);
/* Loads and releases resource id (behaviour inferred). */
void fn_8006B50C(int id, int arg1);
/* Sets up the pick-up item of weapon type type (behaviour inferred). */
void fn_8005B760(int type);
/* The translation of a matrix. */
Vec3f fn_8040FB08(const Mtx34f *matrix);
/* Plays sound id at position (see Fan.cpp). */
void fn_801D54F4(void *sound, int id, const Vec3f *position, int arg3, int arg4);
/* Start effect id at a matrix for owner (see GunBattery.cpp; the second keeps following it,
 * behaviour inferred). */
boost::weak_ptr<Effect::System::EffectElement> fn_8005E418(const Mtx34f *matrix, int id,
                                                           const boost::weak_ptr<CharInfo> &owner);
boost::weak_ptr<Effect::System::EffectElement> fn_8005E468(const Mtx34f *matrix, int id,
                                                           const boost::weak_ptr<CharInfo> &owner);
/* The fn_80014E04 flag word's bit test (see GadgetPickup.h). */
bool fn_80014E34(u32 *flags, int bit);
/* Vector and matrix helpers (behaviour inferred from their bodies). */
Vec3f fn_8040B618(const Mtx34f *matrix, const Vec3f *point);
/* Splits vector into its parts along and across normal (either output may be null). */
void fn_8040F990(Vec3f *along, Vec3f *across, const Vec3f *vector,
                 const Vec3f *normal);
Vec3f fn_8040CEE8(const Vec3f &vector); /* normalised */
Vec3f fn_80016284(const Vec3f &vector); /* negated */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
/* The larger of two values, and value clamped to [low, high]. */
const float &fn_80070494(const float &a, const float &b);
const float &fn_80070460(const float &value, const float &low, const float &high);
}

/* fn_8040CD74 called as returning a - b by value (it stores the result through its first
 * argument). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);
/* fn_801D3DC8 called with a position in place of the character info (see GadgetSound.h). */
typedef UnkSoundHandle_801D3DC8 (*SoundAtPositionFn)(int soundId, const Vec3f *position,
                                                     void *owner);

namespace Weapon {

class VacuumPod : public WeaponVacuumBase {
  public:
    VacuumPod(const boost::weak_ptr<CharInfo> &owner, const UnkWeaponEffectSet_80063428 *effects);
    virtual ~VacuumPod();
    virtual void Tick(float dt);
    virtual void Show();
    virtual void Hide();
    virtual void StartVacuum();
    virtual void StopVacuum();
    virtual void vf_0x3C(int count);
    virtual void LetGo(const Vec3f &velocity);
    virtual const Mtx34f *GetMuzzle();

    /* Starts and stops the effect shown while pulling (names inferred). */
    void StartEffect();
    void StopEffect();
    /* Plays the release animation, effect and sound (name inferred). */
    void PlayReleaseEffect();

    static WeaponBase *Instantiate(const boost::weak_ptr<CharInfo> &owner);
    static Item *CreateItem();

    /* 0x58 */ bool m_hidden;
    /* 0x5C */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
    /* 0x64 */ float m_timer; /* the time until the next target may be taken */
    /* 0x68 */ UnkSoundHandle_801D3DC8 m_sound;
    /* 0x84 */ Vec3f m_position; /* the muzzle's position */
};
SIZE_ASSERT(VacuumPod, 0x90);

} // namespace Weapon

using Weapon::VacuumPod;

static const char *s_taskName = "VacuumPod";

/* The effects of the weapon model and of the item's model (meanings inferred). */
static const UnkWeaponEffectSub_80063428 s_effectSubs0[] = {
    {0x71, 0, 0},
    {0x70, 0, 0},
    {0x72, 0, 2},
};

static const UnkWeaponEffectSub_80063428 s_effectSubs1[] = {
    {0x6F, 0, 0},
    {0x6F, 0, 0},
    {0x6F, 0, 0},
};

static const UnkWeaponEffect_80063428 s_effects[] = {
    {-1, 0x2328, 0x1F, 0x60, s_effectSubs0, 3},
    {0x251C, -1, 0x20, 0x61, s_effectSubs1, 3},
    {0x26AC, -1, 0x22, -1, 0, 0},
    {0x2649, -1, 0x21, -1, 0, 0},
};

static const u32 s_sounds[] = {0x2329};

static const UnkWeaponEffectSet_80063428 s_effectSet = {s_effects, 4, s_sounds, 1};

static const UnkWeaponEffect_80063428 s_itemEffects[] = {
    {-1, 0x801, 0x1F, 0x60, s_effectSubs0, 3},
    {0x251C, -1, 0x20, 0x61, s_effectSubs1, 3},
    {0x26AC, -1, 0x22, -1, 0, 0},
    {0x2649, -1, 0x21, -1, 0, 0},
};

static const u32 s_itemSounds[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSet = {s_itemEffects, 4, s_itemSounds, 2};

/* A distance (name inferred). As a call it is not a constant expression, so the values are stored
 * by the static initializer, as in retail. */
inline float Span(float length) {
    return length;
}

/* vector scaled by scale (name inferred). */
inline Vec3f Scale(const Vec3f &vector, float scale) {
    return fn_8040CDA8(&vector, scale);
}

/* How far the weapon looks for targets (name inferred). */
static float s_range = Span(200.0f);

VacuumPod::VacuumPod(const boost::weak_ptr<CharInfo> &owner, const UnkWeaponEffectSet_80063428 *effects)
    : WeaponVacuumBase(owner, 0x18) {
    m_model = new UnkWeaponModel_80063428(m_owner, effects);
    m_timer = 0.0f;
    m_hidden = false;
}

VacuumPod::~VacuumPod() {
    if (m_model) {
        delete m_model;
    }
    if (!m_effect.expired()) {
        m_effect.lock()->Stop();
    }
    m_sound.fn_801D455C();
}

void VacuumPod::Tick(float dt) {
    WeaponVacuumBase::Tick(dt);
    AssignVector(m_position, fn_8040FB08(m_model->fn_80063234(0)));
    if (m_timer > 0.0f) {
        m_timer = fn_80070494(m_timer - dt, 0.0f);
    }
    if (fn_80014E34(&m_flags.m_bits, 0) && !HasObject() && m_timer <= 0.0f) {
        boost::weak_ptr<Target> target = FindTarget(s_range);
        if (!target.expired()) {
            boost::shared_ptr<Target> locked = target.lock();
            boost::shared_ptr<CharInfo> info = locked->m_owner.lock();
            if (info) {
                if (info->field_0x5C == 1) {
                    const Mtx34f *muzzle = m_model->fn_80063234(0);
                    Vec3f offset;
                    Vec3f along;
                    Vec3f across;
                    fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                             &m_position, &info->m_position));
                    fn_8040F990(&along, &across, &offset,
                                &(const Vec3f &)fn_8040B618(
                                    muzzle, &(const Vec3f &)Vec3f(0.0f, 0.0f, 1.0f)));
                    float distance = fn_8040CF50(&along);
                    if (distance < 4.0f) {
                        info->PushCommand(new DamageCommand(fn_80016284(fn_8040CEE8(offset)), 4, 1.0f));
                    } else {
                        float strength = 1.0f - distance / s_range;
                        strength = fn_80070460(strength, 0.0f, 1.0f);
                        strength = 500.0f * strength + 800.0f;
                        info->PushCommand(new AccelerateCommand(Scale(fn_8040CEE8(offset), strength), 4));
                    }
                } else {
                    boost::shared_ptr<WeaponVacuumBase> self =
                        boost::static_pointer_cast<WeaponVacuumBase>(m_self.lock());
                    /* Written as a conditional: "!test" or "test == 0" give other registers. */
                    bool arg = fn_80014E34(&m_flags.m_bits, 2) ? false : true;
                    info->PushCommand(new VacuumCommand(locked, self, arg));
                    m_timer = 0.0625f;
                }
            }
        }
    }
    if (!m_effect.expired()) {
        if (m_hidden) {
            m_effect.lock()->Hide();
        } else {
            m_effect.lock()->Show();
        }
    }
}

void VacuumPod::Show() {
    if (m_model) {
        m_model->fn_80063130();
    }
    m_hidden = false;
}

void VacuumPod::Hide() {
    if (m_model) {
        m_model->fn_80063154();
    }
    m_hidden = true;
}

void VacuumPod::StartVacuum() {
    WeaponVacuumBase::StartVacuum();
    StartEffect();
    m_timer = 0.0f;
    m_model->fn_80063258(2);
    m_sound.fn_801D455C();
    m_sound = ((SoundAtPositionFn)fn_801D3DC8)(0x2024, &m_position, this);
}

void VacuumPod::StopVacuum() {
    WeaponVacuumBase::StopVacuum();
    StopEffect();
    m_model->fn_80063258(0);
    m_sound.fn_801D455C();
}

void VacuumPod::vf_0x3C(int count) {
    WeaponVacuumBase::vf_0x3C(count);
    fn_801D54F4(fn_80053164(), 0x2025, &m_position, 0, 1);
}

void VacuumPod::LetGo(const Vec3f &velocity) {
    WeaponVacuumBase::LetGo(velocity);
    PlayReleaseEffect();
}

const Mtx34f *VacuumPod::GetMuzzle() {
    return m_model->fn_80063234(0);
}

void VacuumPod::StartEffect() {
    m_effect = fn_8005E468(m_model->fn_80063234(0), 0x13E, m_owner);
    if (m_hidden && !m_effect.expired()) {
        m_effect.lock()->Hide();
    }
}

void VacuumPod::StopEffect() {
    if (!m_effect.expired()) {
        m_effect.lock()->Kill();
        m_effect.reset();
    }
}

void VacuumPod::PlayReleaseEffect() {
    m_model->fn_80063258(1);
    fn_8005E418(m_model->fn_80063234(0), 0x141, m_owner);
    fn_801D54F4(fn_80053164(), 0x2027, &m_position, 0, 1);
}

extern "C" void fn_80225D64(void) {
    fn_803A953C("%s::Initialize\n", s_taskName);
    fn_8006B50C(0x60, 0);
    fn_8006B50C(0x61, 0);
    fn_8006B50C(0x1F, 0);
    fn_8006B50C(0x20, 0);
    fn_8006B50C(0x22, 0);
    fn_8006B50C(0x21, 0);
    fn_8006B50C(0x70, 0);
    fn_8006B50C(0x71, 0);
    fn_8006B50C(0x72, 0);
    fn_8006B50C(0x6F, 0);
    fn_8005B760(0x18);
}

extern "C" void fn_80225D34(void) {
    fn_803A953C("%s::Finalize\n", s_taskName);
}

Weapon::WeaponBase *VacuumPod::Instantiate(const boost::weak_ptr<CharInfo> &owner) {
    return new VacuumPod(owner, &s_effectSet);
}

Weapon::Item *VacuumPod::CreateItem() {
    return new Weapon::Item(&s_itemEffectSet);
}

UnkWeaponTaskInfo lbl_80546558 = {
    s_taskName, fn_80225D64, fn_80225D34, 0, 0, 0x80, 0x00F00400, 0, 0,
};

/* The last field holds 1.0f (0x3F800000). */
UnkWeaponTypeInfo lbl_8054657C = {
    s_taskName, VacuumPod::Instantiate, VacuumPod::CreateItem, 0, Span(1.0f), Span(5.0f),
    0x14, 3, 0x2003, 0.5f, 1.0f, 0x3F800000,
};
