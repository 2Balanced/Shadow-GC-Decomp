#ifndef GAME_CHARACOLLIMOVE_H
#define GAME_CHARACOLLIMOVE_H

#include <boost/function.hpp>

#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

struct CharaColliMoveArgs;
struct UnkColliMoveContact_803334A4;
struct UnkCopyVector3_800091EC;

namespace Vehicle {

class TVehicleBase;

/* Vehicle::CharaColliMove (class name from RTTI): the task that keeps a vehicle on what it stands
 * on. Game/VehicleCharaColliMove.cpp defines it and declares the whole class, with its state and
 * the types that state uses; this header gives other units the part they use (creating one, its
 * contact, callback and velocity), with the same layout. Do not include it in
 * Game/VehicleCharaColliMove.cpp. */
class CharaColliMove : public Task {
  public:
    CharaColliMove(Task *parent, const boost::shared_ptr<TVehicleBase> &vehicle);
    virtual ~CharaColliMove();
    virtual void Tick(float dt);

    void SetCallback(const boost::function1<void, CharaColliMoveArgs &> &callback);
    void SetContact(const UnkColliMoveContact_803334A4 &contact);
    void ClearContact();
    const UnkColliMoveContact_803334A4 *GetContact() const;
    UnkCopyVector3_800091EC GetVelocity() const;

    /* 0x28 */ u8 field_0x28[0x30 - 0x28];
};
SIZE_ASSERT(CharaColliMove, 0x30);

} // namespace Vehicle

#endif
