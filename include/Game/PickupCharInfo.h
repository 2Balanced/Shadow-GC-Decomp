#ifndef GAME_PICKUPCHARINFO_H
#define GAME_PICKUPCHARINFO_H

#include <boost/function.hpp>

#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class CharCommandUnit;
class Target;

/* PickupCharInfo (class name from RTTI; vtable at 0x8053D90C): the 0x80-byte character info of an
 * object a character can pick up and carry (GadgetPickup::PickupBase subclasses create one). Like
 * GadgetCharInfo it holds a command receiver; it also holds the target characters aim at to pick
 * it up, and the callback that places it in the carrier's hand. Member names and meanings are
 * inferred from the BOMB gadget. */
class PickupCharInfo : public CharInfo {
  public:
    /* What the carrier passes to the grab-position callback (struct name from the RTTI of the
     * gadgets' callback binders): the carrier's hand matrix. */
    struct SetGrabPositionArgs {
        /* 0x0 */ const Mtx34f *m_matrix;
    };

    /* The callback member type: a function1 whose assignment from a function1 is a function of its
     * own (class name unknown; the first copy is the BOMB unit's, 0x800F03D4). */
    class GrabCallback : public boost::function1<void, SetGrabPositionArgs &> {
      public:
        GrabCallback &operator=(const boost::function1<void, SetGrabPositionArgs &> &f) {
            boost::function1<void, SetGrabPositionArgs &>::operator=(f);
            return *this;
        }
    };

    /* type is the character type (0xE for a bomb). */
    PickupCharInfo(int type);
    /* 0x801E429C */
    virtual ~PickupCharInfo();

    /* Sets the word at 0x5C (meaning unknown; 4 for a bomb). */
    void SetKind(s32 kind) {
        field_0x5C = kind;
    }

    /* 0x64 */ boost::shared_ptr<CharCommandUnit> m_commandUnit;
    /* 0x6C */ boost::shared_ptr<Target> m_target;
    /* 0x74 */ GrabCallback m_grabCallback;
};
SIZE_ASSERT(PickupCharInfo, 0x80);

#endif
