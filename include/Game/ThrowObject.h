#ifndef GAME_THROWOBJECT_H
#define GAME_THROWOBJECT_H

#include "Game/MathTypes.h"
#include "Game/Task.h"
#include "boost/scoped_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

class CharInfo;
class ThrowObjectImpl;

/* A position returned through the game's out-of-line vector copy constructor (fn_800091EC). It has
 * no base class, so that its inline constructor calls no other inline function and is inlined;
 * callers read it as a Vec3f (name invented). */
struct ThrowObjectPosition {
    ThrowObjectPosition(const Vec3f &position) {
        fn_800091EC(reinterpret_cast<Vec3f *>(this), &position);
    }

    float x;
    float y;
    float z;
};

/* ThrowObject (class name from RTTI): a task for something thrown, which flies from position with
 * velocity and acceleration until it hits something or has flown range (Game/ThrowContainer.cpp
 * derives from it). Its state lives in a ThrowObjectImpl (class name from the RTTI of a bound
 * callback), behind a boost::scoped_ptr; Game/ThrowObject.cpp defines both. Names are inferred.
 * radius is the size of the swept collision sphere. */
class ThrowObject : public Task {
  public:
    /* 0x8027D064 */
    ThrowObject(const boost::weak_ptr<CharInfo> &thrower, const Vec3f &position,
                const Vec3f &velocity, const Vec3f &acceleration, float radius, float range);
    /* 0x8027CF2C */
    virtual ~ThrowObject();
    /* Moves the object and calls the two hooks below (0x8027CE70). */
    virtual void Tick(float dt);
    /* Called when the object has flown its range, and when it hits something (0x8027CD7C and
     * 0x8027CD78 do nothing). */
    virtual void OnRangeEnd();
    virtual void OnHit();

    /* The character that threw it (0x8027CD50). */
    boost::weak_ptr<CharInfo> GetThrower();
    /* Where it is (0x8027CE48). */
    ThrowObjectPosition GetPosition();
    /* Shows effect id along the flight (0x8027CD80). */
    void SetEffect(int id);

    /* 0x28 */ boost::scoped_ptr<ThrowObjectImpl> m_impl;
    /* 0x2C */ u32 field_0x2C;
};
SIZE_ASSERT(ThrowObject, 0x30);

#endif
