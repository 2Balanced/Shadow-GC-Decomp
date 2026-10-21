#ifndef GAME_GADGETCOLLIDETECTARGS_H
#define GAME_GADGETCOLLIDETECTARGS_H

#include <boost/function.hpp>

#include "Game/GadgetCollision.h"

/* GadgetColliDetectArgs (class name from RTTI, with ColliDetectArgs as its only base; vtable at
 * 0x8052AF0C): the collision event of a collision handle (GadgetCharCollision). Callbacks
 * bound to a ColliDetectArgs & member function receive it through a
 * boost::function1<void, GadgetColliDetectArgs &>, whose destructor (0x800F5B08) and
 * list1<GadgetColliDetectArgs &>::operator[](arg<1>) (0x800F5238) are shared instances. */
struct GadgetColliDetectArgs : public ColliDetectArgs {
    virtual ~GadgetColliDetectArgs();
};

/* The handle fn_8015B8C4 wraps a collider in (see Game/GadgetCharCollision.h). */
class GadgetCharCollision;

extern "C" {
/* Sets the handle's collision callback and returns the handle (behaviour inferred from its body,
 * which assigns callback to the function at the start of the handle). */
GadgetCharCollision *
fn_800F5980(GadgetCharCollision *handle,
            const boost::function1<void, GadgetColliDetectArgs &> &callback);
}

#endif
