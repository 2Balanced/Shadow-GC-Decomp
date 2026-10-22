#ifndef GAME_BOSSDESIREMANAGER_H
#define GAME_BOSSDESIREMANAGER_H

#include "Game/BossDesire.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>

namespace Boss {
namespace Desire {

/* The desire manager of the bosses that choose their behaviours by desire (Boss Sonic and Diablon;
 * class name unknown, no RTTI; its code is in Game/BossDesireManager.cpp). It holds the boss's
 * desires, raises them every frame and starts the behaviour of the desire that became full.
 * Member names are inferred. */
class DesireManager {
  public:
    struct Impl;

    DesireManager();
    ~DesireManager();
    /* Makes room for count desires. */
    void Init(int count);
    void Set(int index, const boost::shared_ptr<DesireBase> &desire);
    /* Flag bit 0 stops the desires from growing, bit 1 from being chosen. */
    void SetFlag(int bit, bool on);
    void Tick(float dt);

    /* 0x0 */ boost::scoped_ptr<Impl> m_impl;
};

} // namespace Desire
} // namespace Boss

#endif
