/* HealCannon (class name from RTTI: @unnamed@WeaponSpecialHealCannon_cpp@::HealCannon): a cannon,
 * registered as two weapons, HealCannon01 (type 0x41) and HealCannon02 (type 0x42), which differ
 * only in their records. File name from the RTTI name. Method names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/CharInfo.h"
#include "Game/GadgetUtil.h"
#include "Game/WeaponBase.h"
#include "Game/WeaponCannonBase.h"
#include "Game/WeaponItem.h"
#include "Game/WeaponSpecial.h"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Weapon::UnkWeaponEffect_80063428;
using Weapon::UnkWeaponEffectSet_80063428;

extern "C" {
/* Loads resource id (behaviour inferred). */
void fn_8006B50C(int id, int arg1);
/* Sets up the pick-up item of weapon type type (behaviour inferred). */
void fn_8005B760(int type);
/* The translation of a matrix. */
Vec3f fn_8040FB08(const Mtx34f *matrix);
/* Plays sound id at position (behaviour inferred). */
void fn_801D54F4(void *sound, int id, const Vec3f *position, int arg3, int arg4);
}

namespace {

class HealCannon : public Weapon::WeaponCannonBase {
  public:
    HealCannon(const boost::weak_ptr<CharInfo> &owner, int type,
               const UnkWeaponEffectSet_80063428 *effects)
        : Weapon::WeaponCannonBase(owner, type, effects) {}
    virtual ~HealCannon();
    virtual bool vf_0x2C() {
        return 1;
    }
    virtual void vf_0x38();
};
SIZE_ASSERT(HealCannon, 0x3C);

} // namespace

/* The effects of the weapon model and of the item's model (meanings inferred). */
static const UnkWeaponEffect_80063428 s_effects[] = {
    {-1, 0x2328, 0x48, -1, 0, 0},
    {0x2328, -1, 0x4A, -1, 0, 0},
    {0x2328, -1, 0x49, -1, 0, 0},
};

static const u32 s_sounds[] = {0x2329};

static const UnkWeaponEffectSet_80063428 s_effectSet = {s_effects, 3, s_sounds, 1};

static const UnkWeaponEffect_80063428 s_itemEffects[] = {
    {-1, 0x801, 0x48, -1, 0, 0},
    {0x2328, -1, 0x4A, -1, 0, 0},
    {0x2328, -1, 0x49, -1, 0, 0},
};

static const u32 s_itemSounds[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSet = {s_itemEffects, 3, s_itemSounds, 2};

/* The names are read by the static initializer. */
static const char *s_name01 = "HealCannon01";
static const char *s_name02 = "HealCannon02";

/* A distance (name inferred). As a call it is not a constant expression, so the records' values are
 * stored by the static initializer, as in retail. */
inline float Span(float length) {
    return length;
}

HealCannon::~HealCannon() {}

void HealCannon::vf_0x38() {
    Vec3f position;

    const Mtx34f *muzzle = m_model->fn_80063234(0);
    fn_800091EC(&position, &(const Vec3f &)fn_8040FB08(muzzle));
    fn_801D54F4(fn_80053164(), 0xE02D, &position, 0, 1);
}

void WeaponSpecialHealCannon::Initialize() {
    fn_8006B50C(0x48, 0);
    fn_8006B50C(0x49, 0);
    fn_8006B50C(0x4A, 0);
    fn_8005B760(0x41);
    fn_8005B760(0x42);
}

void WeaponSpecialHealCannon::Finalize() {}

/* The records' callbacks (names inferred). */
static Weapon::WeaponBase *Instantiate01(const boost::weak_ptr<CharInfo> &owner) {
    return new HealCannon(owner, 0x41, &s_effectSet);
}

static Weapon::Item *CreateItem01() {
    return new Weapon::Item(&s_itemEffectSet);
}

static Weapon::WeaponBase *Instantiate02(const boost::weak_ptr<CharInfo> &owner) {
    return new HealCannon(owner, 0x42, &s_effectSet);
}

static Weapon::Item *CreateItem02() {
    return new Weapon::Item(&s_itemEffectSet);
}

UnkWeaponTypeInfo lbl_8054D200 = {
    s_name01, Instantiate01, CreateItem01, 0, Span(1.0f), Span(5.0f),
    0xA, 2, 0x2002, 1.0f, 1.0f, 0x3F800000,
};

UnkWeaponTypeInfo lbl_8054D230 = {
    s_name02, Instantiate02, CreateItem02, 0, Span(1.0f), Span(5.0f),
    0x14, 2, 0x2002, 1.0f, 1.0f, 0x3F800000,
};
