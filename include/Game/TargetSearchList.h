#ifndef GAME_TARGETSEARCHLIST_H
#define GAME_TARGETSEARCHLIST_H

#include "Game/MathTypes.h"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class Target;

/* TargetSearchList (class name from RTTI; vtable at 0x8053DBFC): a 0x14-byte list of the targets a
 * search finds. The target manager creates one (fn_801E7930); callers narrow it down and take the
 * nearest target (behaviour inferred from Game/WeaponVacuumBase.cpp). Only the virtual destructor
 * (0x801E8664) is known; the methods are called by their address names below. */
class TargetSearchList {
  public:
    virtual ~TargetSearchList();

    /* 0x04 */ u8 field_0x4[0x14 - 0x4];
};
SIZE_ASSERT(TargetSearchList, 0x14);

/* The target manager (class name unknown). */
class UnkTargetManager_80060058;

extern "C" {
/* The target manager (a function-local static). */
UnkTargetManager_80060058 *fn_80060058(void);
/* A new list of all the manager's targets (behaviour inferred). */
TargetSearchList *fn_801E7930(UnkTargetManager_80060058 *manager);
/* Narrowing the list (behaviour inferred): by flag value, to a cone of cosine cosAngle about
 * direction from position, to range of position, and by flag value. */
void fn_801E8048(TargetSearchList *list, int value);
void fn_801E7FB4(TargetSearchList *list, const Vec3f *position, float cosAngle,
                 const Vec3f *direction);
void fn_801E8218(TargetSearchList *list, const Vec3f *position, float range);
void fn_801E80C0(TargetSearchList *list, int value);
/* The listed target nearest to position, and the removal of a target from the list (behaviour
 * inferred). */
boost::weak_ptr<Target> fn_801E848C(TargetSearchList *list, const Vec3f *position);
void fn_801E7E48(TargetSearchList *list, const boost::weak_ptr<Target> &target);
}

#endif
