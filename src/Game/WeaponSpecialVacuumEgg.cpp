/* VacuumEgg (class name from RTTI: @unnamed@WeaponSpecialVacuumEgg_cpp@::VacuumEgg): a vacuum
 * weapon of two weapon types, 0x3D (VacuumEgg01) and 0x3E (VacuumEgg02). It pulls targets like
 * Weapon::VacuumPod; every 20 (type 0x3D) or 30 (other types) objects it has taken in, it gives its
 * owner one or three items (item 1). Its model grows with the objects taken in since the last batch.
 * Its pick-up item (VacuumEggItem, RTTI name @unnamed@WeaponSpecialVacuumEgg_cpp@::VacuumEggItem)
 * scales its model the same way from the ammo it holds. File name from the RTTI names. Method and
 * member names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
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
#include "Game/WeaponSpecial.h"
#include "Game/WeaponVacuumBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Weapon::UnkWeaponEffect_80063428;
using Weapon::UnkWeaponEffectSet_80063428;
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
/* Loads resource id (behaviour inferred). */
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
void fn_8040F990(Vec3f *along, Vec3f *across, const Vec3f *vector, const Vec3f *normal);
Vec3f fn_8040CEE8(const Vec3f &vector); /* normalised */
Vec3f fn_80016284(const Vec3f &vector); /* negated */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
/* The larger of two values, and value clamped to [low, high]. */
const float &fn_80070494(const float &a, const float &b);
const float &fn_80070460(const float &value, const float &low, const float &high);
/* Scales node of a weapon model (behaviour inferred). */
void fn_800631FC(UnkWeaponModel_80063428 *model, int node, const Vec3f *scale);
/* The owner of a weapon (see Game/WeaponVacuumBase.cpp). */
boost::weak_ptr<CharInfo> fn_80065034(Weapon::WeaponBase *weapon);
/* Builds the command that gives item item (see Game/GadgetModelInfo.cpp; the other arguments'
 * meaning is unknown). */
CharCommand *fn_8015F760(void *command, int arg0, int item, int arg2, int arg3);
}

/* fn_8040CD74 called as returning a - b by value (it stores the result through its first
 * argument). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);
/* fn_801D3DC8 called with a position in place of the character info (see GadgetSound.h). */
typedef UnkSoundHandle_801D3DC8 (*SoundAtPositionFn)(int soundId, const Vec3f *position,
                                                     void *owner);

namespace {

class VacuumEggItem : public Weapon::Item {
  public:
    VacuumEggItem(const UnkWeaponEffectSet_80063428 *effects) : Weapon::Item(effects) {}
    virtual ~VacuumEggItem();
    /* Takes the ammo the item holds and scales the model by it. */
    virtual void vf_0x0C(int ammo);
};
SIZE_ASSERT(VacuumEggItem, 0x10);

class VacuumEgg : public WeaponVacuumBase {
  public:
    VacuumEgg(const boost::weak_ptr<CharInfo> &owner, int type,
              const UnkWeaponEffectSet_80063428 *effects);
    virtual ~VacuumEgg();
    virtual void Tick(float dt);
    virtual void Show();
    virtual void Hide();
    virtual bool vf_0x2C() {
        return true;
    }
    virtual void StartVacuum();
    virtual void StopVacuum();
    virtual void vf_0x3C(int count);
    virtual void LetGo(const Vec3f &velocity);
    virtual const Mtx34f *GetMuzzle();
    virtual void vf_0x48(int ammo);

    /* Scales the model by the objects taken in. */
    void UpdateScale();
    /* Starts and stops the effect shown while pulling. */
    void StartEffect();
    void StopEffect();

    /* 0x58 */ float m_scale;
    /* 0x5C */ s32 m_count; /* the objects taken in since the last batch */
    /* 0x60 */ bool m_hidden;
    /* 0x64 */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
    /* 0x6C */ float m_timer; /* the time until the next target may be taken */
    /* 0x70 */ UnkSoundHandle_801D3DC8 m_sound;
    /* 0x8C */ Vec3f m_position; /* the muzzle's position */
};
SIZE_ASSERT(VacuumEgg, 0x98);

} // namespace

/* The effects of the weapon model and of the item's model (meanings inferred). */
static const UnkWeaponEffect_80063428 s_effects[] = {
    {-1, 0x2328, 0x54, -1, 0, 0},
    {0x251C, -1, 0x55, -1, 0, 0},
};

static const u32 s_sounds[] = {0x2329, 0x251C};

static const UnkWeaponEffectSet_80063428 s_effectSet = {s_effects, 2, s_sounds, 2};

static const UnkWeaponEffect_80063428 s_itemEffects[] = {
    {-1, 0x801, 0x54, -1, 0, 0},
    {0x251C, -1, 0x55, -1, 0, 0},
};

static const u32 s_itemSounds[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSet = {s_itemEffects, 2, s_itemSounds, 2};

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

/* The objects still missing from the batch begun with ammo objects (zero when ammo fills whole
 * batches). */
static int Shortfall(int type, int ammo) {
    int size;
    switch (type) {
    case 0x3D:
        size = 20;
        break;
    case 0x3E:
    default:
        size = 30;
        break;
    }
    int rest = ammo % size;
    if (rest > 0) {
        return size - rest;
    }
    return 0;
}

/* The objects in a batch of weapon type type, and the items a batch gives. */
inline void GetBatch(int type, int &size, int &items) {
    switch (type) {
    case 0x3D:
        size = 20;
        items = 1;
        break;
    case 0x3E:
    default:
        size = 30;
        items = 3;
        break;
    }
}

VacuumEgg::VacuumEgg(const boost::weak_ptr<CharInfo> &owner, int type,
                     const UnkWeaponEffectSet_80063428 *effects)
    : WeaponVacuumBase(owner, type) {
    m_model = new UnkWeaponModel_80063428(m_owner, effects);
    m_scale = 1.0f;
    m_count = 0;
    m_timer = 0.0f;
    m_hidden = false;
}

VacuumEgg::~VacuumEgg() {
    if (m_model) {
        delete m_model;
    }
    if (!m_effect.expired()) {
        m_effect.lock()->Stop();
    }
    m_sound.fn_801D455C();
}

void VacuumEgg::Tick(float dt) {
    WeaponVacuumBase::Tick(dt);
    if (m_model) {
        AssignVector(m_position, fn_8040FB08(m_model->fn_80063234(0)));
    }
    if (m_timer > 0.0f) {
        m_timer = fn_80070494(m_timer - dt, 0.0f);
    }
    if (fn_80014E34(&m_flags.m_bits, 0) && !HasObject() && m_timer <= 0.0f) {
        boost::weak_ptr<Target> target = FindTarget(s_range);
        if (m_timer > 0.0f) {
            m_timer = fn_80070494(m_timer - dt, 0.0f);
        }
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
                    if (distance < 7.0f) {
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
                    int arg = fn_80014E34(&m_flags.m_bits, 2) ? 0 : 1;
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

void VacuumEgg::Show() {
    if (m_model) {
        m_model->fn_80063130();
    }
    m_hidden = false;
}

void VacuumEgg::Hide() {
    if (m_model) {
        m_model->fn_80063154();
    }
    m_hidden = true;
}

void VacuumEgg::StartVacuum() {
    WeaponVacuumBase::StartVacuum();
    StartEffect();
    m_timer = 0.0f;
    m_sound.fn_801D455C();
    m_sound = ((SoundAtPositionFn)fn_801D3DC8)(0xE02A, &m_position, this);
}

void VacuumEgg::StopVacuum() {
    WeaponVacuumBase::StopVacuum();
    StopEffect();
    m_sound.fn_801D455C();
}

void VacuumEgg::vf_0x3C(int count) {
    WeaponVacuumBase::vf_0x3C(count);
    fn_801D54F4(fn_80053164(), 0xE02B, &m_position, 0, 1);
    int size;
    int items;
    GetBatch(TypeId(), size, items);
    m_count += count;
    if (m_count >= size) {
        m_count -= size;
        boost::shared_ptr<CharInfo> info = fn_80065034(this).lock();
        if (info) {
            for (int i = 0; i < items; i++) {
                CharCommand *command = (CharCommand *)CharCommand::operator new(0x20);
                if (command != 0) {
                    command = fn_8015F760(command, 6, 1, 1, 4);
                }
                info->PushCommand(command);
            }
            fn_801D54F4(fn_80053164(), 0xE02C, &m_position, 0, 1);
            fn_8005E418(m_model->fn_80063234(1), 0xB6, info);
        }
    }
    UpdateScale();
}

void VacuumEgg::LetGo(const Vec3f &velocity) {
    WeaponVacuumBase::LetGo(velocity);
}

const Mtx34f *VacuumEgg::GetMuzzle() {
    return m_model->fn_80063234(0);
}

void VacuumEgg::vf_0x48(int ammo) {
    m_count = Shortfall(m_type, ammo);
    UpdateScale();
}

void VacuumEgg::UpdateScale() {
    m_scale = 1.0f + 0.08f * m_count;
    Vec3f scale(m_scale, m_scale, m_scale);
    fn_800631FC(m_model, 1, &scale);
}

void VacuumEgg::StartEffect() {
    m_effect = fn_8005E468(m_model->fn_80063234(0), 0x13E, m_owner);
    if (m_hidden && !m_effect.expired()) {
        m_effect.lock()->Hide();
    }
}

void VacuumEgg::StopEffect() {
    if (!m_effect.expired()) {
        m_effect.lock()->Kill();
        m_effect.reset();
    }
}

VacuumEggItem::~VacuumEggItem() {}

void VacuumEggItem::vf_0x0C(int ammo) {
    m_ammo = ammo;
    int shortfall = Shortfall(m_type, ammo);
    float scale = 1.0f + 0.08f * shortfall;
    Vec3f scales(scale, scale, scale);
    fn_800631FC((UnkWeaponModel_80063428 *)field_0x4, 1, &scales);
}

void WeaponSpecialVacuumEgg::Initialize() {
    fn_8006B50C(0x54, 0);
    fn_8006B50C(0x55, 0);
    fn_8005B760(0x3D);
    fn_8005B760(0x3E);
}

void WeaponSpecialVacuumEgg::Finalize() {}

/* The records' callbacks (names inferred). */
static Weapon::WeaponBase *Instantiate01(const boost::weak_ptr<CharInfo> &owner) {
    return new VacuumEgg(owner, 0x3D, &s_effectSet);
}

static Weapon::Item *CreateItem01() {
    return new VacuumEggItem(&s_itemEffectSet);
}

static Weapon::WeaponBase *Instantiate02(const boost::weak_ptr<CharInfo> &owner) {
    return new VacuumEgg(owner, 0x3E, &s_effectSet);
}

static Weapon::Item *CreateItem02() {
    return new VacuumEggItem(&s_itemEffectSet);
}

/* The names are read by the static initializer. */
static const char *s_name01 = "VacuumEgg01";
static const char *s_name02 = "VacuumEgg02";

/* The last field holds 1.0f (0x3F800000). */
UnkWeaponTypeInfo lbl_8054D5B0 = {
    s_name01, Instantiate01, CreateItem01, 0, Span(1.0f), Span(5.0f), 0x14, 3, 0x2002, 0.5f, 1.0f,
    0x3F800000,
};

UnkWeaponTypeInfo lbl_8054D5E0 = {
    s_name02, Instantiate02, CreateItem02, 0, Span(1.0f), Span(5.0f), 0x1E, 3, 0x2002, 0.5f, 1.0f,
    0x3F800000,
};
