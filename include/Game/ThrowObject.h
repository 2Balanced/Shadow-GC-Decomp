#ifndef GAME_THROWOBJECT_H
#define GAME_THROWOBJECT_H

#include "Game/MathTypes.h"
#include "Game/Task.h"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

class CharInfo;
class ThrowObjectImpl;

/* ThrowObject (class name from RTTI): a task for something thrown, which flies from position with
 * velocity and acceleration until it hits something or has flown range (Game/ThrowContainer.cpp
 * derives from it). Its state lives in a ThrowObjectImpl (class name from the RTTI of a bound
 * callback). The members are defined in the unit at 0x8027CD50, not yet decompiled; names are
 * inferred. */
class ThrowObject : public Task {
  public:
    /* 0x8027D064 */
    ThrowObject(const boost::weak_ptr<CharInfo> &thrower, const Vec3f &position,
                const Vec3f &velocity, const Vec3f &acceleration, float gravity, float range);
    /* 0x8027CF2C */
    virtual ~ThrowObject();
    /* Moves the object and calls the two hooks below (0x8027CE70). */
    virtual void Tick(float dt);
    /* Called when the object has flown its range, and when it hits something (0x8027CD78 and
     * 0x8027CD7C do nothing). */
    virtual void OnRangeEnd();
    virtual void OnHit();

    /* The character that threw it (0x8027CD50). */
    boost::weak_ptr<CharInfo> GetThrower();
    /* Where it is (0x8027CE48). */
    Vec3f GetPosition();
    /* Shows effect id along the flight (0x8027CD80). */
    void SetEffect(int id);

    /* 0x28 */ ThrowObjectImpl *m_impl;
    /* 0x2C */ u32 field_0x2C;
};
SIZE_ASSERT(ThrowObject, 0x30);

#endif
