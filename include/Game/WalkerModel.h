#ifndef GAME_WALKERMODEL_H
#define GAME_WALKERMODEL_H

#include "Game/CharInfo.h"
#include "boost/scoped_ptr.hpp"
#include "size_assert.h"

namespace Vehicle {

class WalkerModelImpl;

/* The model and motion of the walker (Vehicle::Walker; code at 0x8020DF48..0x8020E3EC, not yet
 * matched). Its state lives in a Vehicle::WalkerModelImpl (class name from the RTTI of a bound
 * callback's type), so the class name is inferred from it. Constructor 0x8020E39C, destructor
 * 0x8020E2E0; the other functions are declared by the units that call them. */
class WalkerModel {
  public:
    WalkerModel(CharInfo *owner);
    ~WalkerModel();

    /* 0x0 */ boost::scoped_ptr<WalkerModelImpl> m_impl;
};
SIZE_ASSERT(WalkerModel, 0x4);

} // namespace Vehicle

#endif
