#ifndef GAME_PLAYERFACTORY_H
#define GAME_PLAYERFACTORY_H

/* The factories a character's controller and executer make their controls and behaviors with
 * (class names from RTTI). A character type derives one of each: Player::Shadow::ShadowControl
 * and ShadowBehavior, the NPCs' SonicType and FlyType factories, and Boss Sonic's
 * SonicTypeControl. The destructors are inline (the subclasses' destructors store the base
 * vtable themselves); Create is pure. Names other than the classes' are inferred. */

#include "boost/shared_ptr.hpp"

namespace Player {

namespace Control {

class ControlBase;

/* Factory (vtable 0x80521DEC; out-of-line destructor 0x800AEFEC): makes the control of an id
 * (0 and 1: the character's AI, 2 to 5 special ones, as Boss Sonic's SonicTypeControl reads). */
class Factory {
  public:
    virtual ~Factory() {}
    virtual boost::shared_ptr<ControlBase> Create(int id) = 0;
};

} // namespace Control

namespace Behavior {

class Behavior;

/* Factory (vtable 0x80521D78; out-of-line destructor 0x800AE928): makes the behavior of an id. */
class Factory {
  public:
    virtual ~Factory() {}
    virtual boost::shared_ptr<Behavior> Create(int id) = 0;
};

} // namespace Behavior

} // namespace Player

#endif
