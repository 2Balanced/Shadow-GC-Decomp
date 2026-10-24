/* The weapon data: two tasks, WeaponDataStart and WeaponDataEnd, whose hooks start and end the
 * weapon system (the weapon table of Game/WeaponTable.cpp, the special weapons and the weapon
 * resource file), and the effect helpers the weapons and bullets start their effects with, which
 * pass the effect's owner on to the effect launchers (Game/EffectLauncherTable.cpp). Other units
 * call the functions by their addresses, so they keep those names. The unit has no class of its
 * own; the file name comes from the task names, and the other names are inferred. Definitions are
 * in reverse address order (deferred inlining emits functions last-defined first). */
#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/ResourceOneFile.h"
#include "Game/ResourceOneFileReq.h"
#include "Game/WeaponBase.h"
#include "Game/WeaponSpecial.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Effect::System::EffectElement;

extern "C" {
/* Sets up the bullet types (behaviour inferred). */
void fn_8006B5A0(void);
/* Starts and ends the weapon table (Game/WeaponTable.cpp). */
void fn_8005B7FC(void);
void fn_8005B77C(void);
/* The effect launchers of the effect kinds (Game/EffectLauncherTable.cpp): an effect at a position
 * facing a direction, one placed by id alone, one at a matrix, and one following a matrix. */
boost::weak_ptr<EffectElement> fn_801D2AFC(int kind, const Vec3f *position, const Vec3f *direction, int id,
                                           u32 owner);
boost::weak_ptr<EffectElement> fn_801D2BB0(int kind, int id, u32 owner);
boost::weak_ptr<EffectElement> fn_801D2B74(int kind, const Mtx34f *matrix, int id, u32 owner);
boost::weak_ptr<EffectElement> fn_801D2C28(int kind, const Mtx34f *matrix, int id, u32 owner);
}

/* The game state (as in Game/TGi.h, which this unit does not include: its colour table would be
 * emitted here). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x150];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;
/* What the resource files load into (see Game/GadgetCommonData.cpp). */
extern u8 lbl_80575EE4[];

extern "C" {

/* The weapon effects' set-up and clean-up (both empty in this build; names inferred). */
void fn_8005E528(void) {}

void fn_8005E524(void) {}

/* The effect owner of a character: the word at 4 in the object at its 0x60, or the game state's at
 * 0x1C when there is no character (meaning unknown). */
u32 fn_8005E4B8(const boost::weak_ptr<CharInfo> &owner) {
    u32 result;
    if (!owner.expired()) {
        result = reinterpret_cast<u32>(*reinterpret_cast<u8 **>(owner.lock()->field_0x60) + 4);
    } else {
        result = reinterpret_cast<u32>(lbl_8057D6A0.field_0x0 + 0x1C);
    }
    return result;
}

/* Starts effect id of kind 0 following matrix, at matrix, and by id alone for owner. */
boost::weak_ptr<EffectElement> fn_8005E468(const Mtx34f *matrix, int id, const boost::weak_ptr<CharInfo> &owner) {
    return fn_801D2C28(0, matrix, id, fn_8005E4B8(owner));
}

boost::weak_ptr<EffectElement> fn_8005E418(const Mtx34f *matrix, int id, const boost::weak_ptr<CharInfo> &owner) {
    return fn_801D2B74(0, matrix, id, fn_8005E4B8(owner));
}

boost::weak_ptr<EffectElement> fn_8005E3D0(int id, const boost::weak_ptr<CharInfo> &owner) {
    return fn_801D2BB0(0, id, fn_8005E4B8(owner));
}

/* Starts effect id of kind at position, facing direction, for owner; and the same of kind 0. */
boost::weak_ptr<EffectElement> fn_8005E374(int kind, const Vec3f *position, const Vec3f *direction, int id,
                                           const boost::weak_ptr<CharInfo> &owner) {
    return fn_801D2AFC(kind, position, direction, id, fn_8005E4B8(owner));
}

boost::weak_ptr<EffectElement> fn_8005E32C(const Vec3f *position, const Vec3f *direction, int id,
                                           const boost::weak_ptr<CharInfo> &owner) {
    return fn_8005E374(0, position, direction, id, owner);
}

/* WeaponDataStart's hooks: start the weapon system, and nothing. */
void fn_8005E300(void) {
    fn_8006B5A0();
    fn_8005B7FC();
    fn_8005E528();
    WeaponSpecial::Initialize();
}

void fn_8005E2FC(void) {}

/* WeaponDataEnd's hooks: put the weapon resource file in loader slot 2, and end the weapon
 * system. */
void fn_8005E2A0(void) {
    ResourceOneFileReq *resource = new ResourceOneFileReq("common/WeaponResource.one", lbl_80575EE4);
    fn_8001299C(fn_800122D4(), 2, reinterpret_cast<ResourceOneFile *>(resource));
}

void fn_8005E26C(void) {
    fn_800127F8(fn_800122D4(), 2);
    WeaponSpecial::Finalize();
    fn_8005B77C();
    fn_8005E524();
}

} // extern "C"

UnkWeaponTaskInfo lbl_8051E968 = {
    "WeaponDataStart", fn_8005E300, fn_8005E2FC, 0, 0, 0x80, 0x25840100, 0, 0,
};

UnkWeaponTaskInfo lbl_8051E98C = {
    "WeaponDataEnd", fn_8005E2A0, fn_8005E26C, 0, 0, 0x80, 0x25850100, 0, 0,
};
