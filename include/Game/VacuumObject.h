#ifndef GAME_VACUUMOBJECT_H
#define GAME_VACUUMOBJECT_H

#include "Game/Task.h"
#include "Game/MathTypes.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class CharInfo;
class VacuumCommand;

namespace Weapon {
class WeaponVacuumBase;
}

/* VacuumObject (class name from RTTI; vtable at 0x80547A24): an object a vacuum pulls in. Gadgets
 * hand their model to a subclass of it when a VacuumCommand reaches them (VacuumCityBombSmall,
 * Balloon::VacuumBalloon, SetItem::VacuumItemBox, VacuumContainer::Container, ...). It keeps its
 * state in a 0x84-byte Impl behind a boost::scoped_ptr (a pimpl; Game/VacuumObject.cpp). The
 * virtual functions other than the destructor and Tick keep their addresses as names, as their
 * roles are only inferred; the other method names are inferred too. */
class VacuumObject {
  public:
    class Impl;

    /* Allocates the state for kind and takes the vacuum's weapon and owner from command
     * (0x80238424). */
    VacuumObject(int kind, VacuumCommand &command);
    /* 0x80238304 */
    virtual ~VacuumObject();
    /* Moves the object to position, given in the vacuum's space (behaviour inferred). */
    virtual void fn_802382E0(const Vec3f &position);
    /* Sets the object's rotation (behaviour inferred). */
    virtual void fn_80238268(const Vec3f &rotation);
    /* Pulls the object along by dt (0x80238188). */
    virtual void Tick(float dt);
    /* Lets the object go with velocity (behaviour inferred; WeaponVacuumBase::LetGo calls it). */
    virtual void fn_80238184(const Vec3f &velocity);
    /* Called when the object has arrived (behaviour inferred). */
    virtual void fn_80238180();
    /* Whether the vacuum or its owner still exists (behaviour inferred). */
    virtual bool fn_80238088();
    /* The vacuum's matrix: its muzzle, or the owner's placement (behaviour inferred). */
    virtual Mtx34f fn_80237EE4();

    /* The vacuum's muzzle matrix or the owner's placement (0x80237F10). */
    Mtx34f CalcMatrix();
    /* The matrix the object follows (0x80238000). */
    Mtx34f *GetMatrix();
    /* How far the object has come, from 0 to 1 (0x8023800C). */
    float GetProgress();
    /* The vacuum weapon (0x8023804C). */
    boost::weak_ptr<Weapon::WeaponVacuumBase> &GetWeapon();
    /* The kind passed to the constructor (0x802380AC). */
    int GetKind();
    /* The fn_80014E04 flag word: bit 0 arrived, bit 1 done, bit 2 sound played (0x802380B8). */
    u32 *GetFlags();
    /* Stops the object's effect and starts effect id (0x802380C4). */
    void StartEffect(int id);

    /* 0x4 */ boost::scoped_ptr<Impl> m_impl;
};
SIZE_ASSERT(VacuumObject, 0x8);

/* VacuumObjectTask (class name from RTTI; vtable at 0x80547A14): the task that drives a
 * VacuumObject until it is done (constructor at 0x80237C90; behaviour inferred). */
class VacuumObjectTask : public Task {
  public:
    VacuumObjectTask(const boost::shared_ptr<VacuumObject> &object);
    /* 0x80237B24 */
    virtual ~VacuumObjectTask();
    /* 0x802379B8 */
    virtual void Tick(float dt);

    /* 0x28 */ boost::shared_ptr<VacuumObject> m_object;
};
SIZE_ASSERT(VacuumObjectTask, 0x30);

extern "C" {
/* The kind a vacuum object is created with: always 1, and 1 or 2 from the vacuum weapon's type
 * (behaviour inferred). */
int fn_80237EDC(VacuumCommand *command);
int fn_80237E34(VacuumCommand *command);
/* The object's position and rotation (behaviour inferred). */
Vec3f fn_802382B0(VacuumObject *object);
Vec3f fn_80238248(VacuumObject *object);
/* The character that pulls the object in (behaviour inferred). */
boost::weak_ptr<CharInfo> fn_80238058(VacuumObject *object);
}

/* A length the vacuum object units scale (3.5; meaning unknown). Each of them keeps a tenth of it
 * in a static that a static initializer stores (Balloon, VacuumContainer, CityBombSmall, ...). */
extern const float lbl_805F72B0;
/* The speed the objects spin at (-1080 degrees a second, in radians; meaning inferred). A const
 * object set up by VacuumObject.cpp's static initializer: VacuumContainer.cpp's fn_80238184
 * schedules its load the way only a const object allows. */
extern const float lbl_805EFEE8;

#endif
