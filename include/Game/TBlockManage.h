#ifndef GAME_TBLOCKMANAGE_H
#define GAME_TBLOCKMANAGE_H

#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

/* TBlockManage::forBlockCheck (class names from the RTTI of its shared_ptr instances): the stage
 * block a point lies in. Only declared; gadgets hold it to wait until their block is ready. */
class TBlockManage {
  public:
    class forBlockCheck;
};

extern "C" {
/* Returns the TBlockManage block containing position (behaviour inferred). */
boost::shared_ptr<TBlockManage::forBlockCheck> fn_8003A154(const Vec3f *position);
/* Reports a state of the block: SetVehicle waits for it, ContainerBehavior pauses while it is set
 * (meaning unknown). */
bool fn_80039EDC(const boost::weak_ptr<TBlockManage::forBlockCheck> &block);
}

#endif
