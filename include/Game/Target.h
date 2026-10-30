#ifndef GAME_TARGET_H
#define GAME_TARGET_H

/* Target (class name from RTTI; vtable at 0x8053DBA8): a point that weapons, homing attacks and
 * vacuums can aim at. fn_801E7128 and fn_801E72DC create one and register it with the target
 * manager (fn_80060058); the shared_ptr they return deletes it through a deleter that also tells
 * the manager. LineTarget (Game/LineTarget.h) derives from it. Member names are inferred.
 *
 * Many units declare a class Target of their own with only the functions they call; this header is
 * for the units that need the layout. */

#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class CharInfo;

/* A position returned by value through the out-of-line vector copy constructor (fn_800091EC; name
 * inferred). */
struct TargetPosition {
    TargetPosition(const Vec3f &v) {
        fn_800091EC(reinterpret_cast<Vec3f *>(this), &v);
    }

    float x;
    float y;
    float z;
};

class Target {
  public:
    Target(u32 flags);                                         /* 0x801E752C */
    Target(const boost::weak_ptr<CharInfo> &owner, u32 flags); /* 0x801E7490 */

    /* Slot 0x08: where to aim at, seen from `from` (Target ignores it). */
    virtual TargetPosition GetPosition(const Vec3f *from);
    virtual ~Target();

    /* The same position, not virtual (0x801E7410; no caller is known). */
    TargetPosition Position();

    /* Whether the target is active (name inferred). */
    u32 IsActive() const {
        return m_flags & 1;
    }

    /* The flags as the constructors complete them (meanings unknown). */
    static u32 CompleteFlags(u32 flags) {
        if (flags & 0x1C08) {
            flags |= 0x2000;
        }
        if (!(flags & 0xF8000)) {
            flags |= 0x8000;
        }
        return flags;
    }

    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ Vec3f m_position;
    /* 0x14 */ Vec3f m_setPosition; /* set by fn_801E73B0 */
    /* 0x20 */ float m_radius;
    /* 0x24 */ float field_0x24;
    /* 0x28 */ boost::weak_ptr<CharInfo> m_owner;
    /* 0x30 */ u32 m_flags; /* bit 0: active (see fn_801E7094); bit 1: position set */
    /* 0x34 */ u32 field_0x34;
};
SIZE_ASSERT(Target, 0x38);

/* The target manager (class name unknown; see Game/TargetSearchList.h). */
class UnkTargetManager_80060058;

extern "C" {
UnkTargetManager_80060058 *fn_80060058(void);
/* Adds a target to the manager, and drops the manager's released targets (behaviour inferred). */
void fn_801E7AF4(UnkTargetManager_80060058 *manager, boost::shared_ptr<Target> target);
void fn_801E7A20(UnkTargetManager_80060058 *manager);
}

#endif
