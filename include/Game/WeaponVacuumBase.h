#ifndef GAME_WEAPONVACUUMBASE_H
#define GAME_WEAPONVACUUMBASE_H

#include "Game/WeaponBase.h"
#include "Game/MathTypes.h"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class CharInfo;
class Target;
class VacuumObject;

extern "C" {
/* Constructs the flag word cleared (see GiantLantern.cpp). */
void fn_80014E04(u32 *flags);
}

namespace Weapon {

/* The flag word at 0x38 (struct name inferred). */
struct WeaponVacuumFlags {
    WeaponVacuumFlags() {
        fn_80014E04(&m_bits);
    }

    u32 m_bits;
};

/* WeaponVacuumBase (class name from RTTI; vtable at 0x80547AC4): a weapon that pulls objects in
 * (Weapon::VacuumPod, VacuumEgg). Its out-of-line members lie around 0x80239A34-0x80239FA4; the
 * empty destructor and the empty slot 0x48 are inline, so Game/WeaponVacuumPod.cpp emits the
 * copies the game keeps. Method and member names are inferred from their bodies and from
 * VacuumPod's overrides. */
class WeaponVacuumBase : public WeaponBase {
  public:
    /* 0x80239F34 */
    WeaponVacuumBase(const boost::weak_ptr<CharInfo> &owner, int type);
    virtual ~WeaponVacuumBase() {}
    /* Places the weapon and its model (0x80239EF0). */
    virtual void SetTransform(const Vec3f *position, const Vec3f *rotation);
    /* Updates the model and latches the counters (0x80239EA4). */
    virtual void Tick(float dt);
    /* Starts and stops pulling (flag bits 0 and 1; 0x80239CEC and 0x80239CAC). */
    virtual void StartVacuum();
    virtual void StopVacuum();
    /* Add count to the counters at 0x4C and 0x54 (0x80239C9C and 0x80239C8C). */
    virtual void vf_0x38(int count);
    virtual void vf_0x3C(int count);
    /* Lets the object go with velocity added to its motion (0x80239A34). */
    virtual void LetGo(const Vec3f &velocity);
    /* The muzzle matrix. */
    virtual const Mtx34f *GetMuzzle() = 0;
    virtual void vf_0x48() {}

    /* The nearest target within range in front of the muzzle (0x80239604). */
    boost::weak_ptr<Target> FindTarget(float range);
    /* Sets or clears flag bit 2 (0x80239D8C). */
    void fn_80239D8C(bool set);
    /* The counter latched from 0x4C (0x80239DCC). */
    s32 fn_80239DCC();
    /* Whether the held object has arrived (0x80239DD4; behaviour inferred). */
    bool fn_80239DD4();
    /* Whether an object is held (0x80239E5C). */
    bool HasObject();
    /* The counter at 0x3C (0x80239E8C). */
    s32 fn_80239E8C();
    /* Holds object (0x80239C28; name inferred). */
    void SetObject(const boost::weak_ptr<VacuumObject> &object);
    /* Adds count to the counter at 0x3C (0x80239E94). */
    void fn_80239E94(int count);

    /* 0x38 */ WeaponVacuumFlags m_flags; /* bit 0 pulling, bit 1 not for a player, bit 2 set by
                                             0x80239D8C */
    /* 0x3C */ s32 field_0x3C;
    /* 0x40 */ boost::weak_ptr<VacuumObject> m_object;
    /* 0x48 */ s32 field_0x48;
    /* 0x4C */ s32 field_0x4C;
    /* 0x50 */ s32 field_0x50;
    /* 0x54 */ s32 field_0x54;
};
SIZE_ASSERT(WeaponVacuumBase, 0x58);

} // namespace Weapon

#endif
