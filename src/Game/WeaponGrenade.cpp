/* Grenade (class name from RTTI: Weapon::Grenade): a cannon, weapon type 0xC, whose firing effects
 * are a sound and a muzzle flash. The code is Game/WeaponBlackBarrel.cpp's with its own effects and
 * type records. File name inferred from the class name. Method names are inferred. Definitions are
 * in reverse address order (deferred inlining emits functions last-defined first). */
#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/GadgetUtil.h"
#include "Game/WeaponBase.h"
#include "Game/WeaponCannonBase.h"
#include "Game/WeaponItem.h"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Weapon::UnkWeaponEffect_80063428;
using Weapon::UnkWeaponEffectSet_80063428;

extern "C" {
/* Debug output (behaviour inferred). */
void fn_803A953C(const char *format, ...);
/* Loads resource id (behaviour inferred). */
void fn_8006B50C(int id, int arg1);
/* The bullet types' set-up and clean-up (see WeaponLaserRifle.cpp). */
void fn_80051C80(int type);
void fn_80051CC8(int type);
/* Enables weapon type type (Game/WeaponTable.cpp). */
void fn_8005B760(int type);
/* The translation of a matrix. */
Vec3f fn_8040FB08(const Mtx34f *matrix);
/* Plays sound id at position (see Fan.cpp). */
void fn_801D53C4(void *sound, int id, const Vec3f *position, void *source, int arg4, int arg5,
                 int arg6);
/* Starts effect id at a matrix for owner (see GunBattery.cpp). */
boost::weak_ptr<Effect::System::EffectElement> fn_8005E418(const Mtx34f *matrix, int id,
                                                           const boost::weak_ptr<CharInfo> &owner);
}

static const char *s_taskName = "Grenade";

/* The effects of the weapon model and of the item's model (meanings inferred). */
static const UnkWeaponEffect_80063428 s_effects[] = {
    {-1, 0x2328, 0x35, -1, 0, 0},
};

static const u32 s_sounds[] = {0x2329};

static const UnkWeaponEffectSet_80063428 s_effectSet = {s_effects, 1, s_sounds, 1};

static const UnkWeaponEffect_80063428 s_itemEffects[] = {
    {-1, 0x801, 0x35, -1, 0, 0},
};

static const u32 s_itemSounds[] = {0x2328, 0x801};

static const UnkWeaponEffectSet_80063428 s_itemEffectSet = {s_itemEffects, 1, s_itemSounds, 2};

namespace Weapon {

class Grenade : public WeaponCannonBase {
  public:
    Grenade(const boost::weak_ptr<CharInfo> &owner) : WeaponCannonBase(owner, 0xC, &s_effectSet) {}
    virtual ~Grenade() {}
    virtual void vf_0x38();

    static WeaponBase *Instantiate(const boost::weak_ptr<CharInfo> &owner);
    static Item *CreateItem();
};
SIZE_ASSERT(Grenade, 0x3C);

} // namespace Weapon

/* A distance (name inferred). As a call it is not a constant expression, so the records' values are
 * stored by the static initializer, as in retail. */
inline float Span(float length) {
    return length;
}

namespace Weapon {

void Grenade::vf_0x38() {
    const Mtx34f *muzzle = m_model->fn_80063234(0);
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)fn_8040FB08(muzzle));
    fn_801D53C4(fn_80053164(), 0x2013, &position, 0, 1, 0, 0);
    fn_8005E418(m_model->fn_80063234(0), 0xF2, m_owner);
}

} // namespace Weapon

/* The type's load and release hooks (names inferred). */
static void Initialize() {
    fn_803A953C("%s::Initialize\n", s_taskName);
    fn_8006B50C(0x35, 0);
    fn_80051CC8(0xC);
    fn_8005B760(0xC);
}

static void Finalize() {
    fn_803A953C("%s::Finalize\n", s_taskName);
    fn_80051C80(0xC);
}

Weapon::WeaponBase *Weapon::Grenade::Instantiate(const boost::weak_ptr<CharInfo> &owner) {
    return new Grenade(owner);
}

Weapon::Item *Weapon::Grenade::CreateItem() {
    return new Item(&s_itemEffectSet);
}

UnkWeaponTaskInfo lbl_8051EA68 = {
    s_taskName, Initialize, Finalize, 0, 0, 0x80, 0x00DC0400, 0, 0,
};

UnkWeaponTypeInfo lbl_8051EA8C = {
    s_taskName, Weapon::Grenade::Instantiate, Weapon::Grenade::CreateItem, 0, Span(1.0f), Span(5.0f),
    6, 1, 0x2002, 0.0f, 1.0f, 0x3F800000,
};
