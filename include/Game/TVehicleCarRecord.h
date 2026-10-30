#ifndef GAME_TVEHICLECARRECORD_H
#define GAME_TVEHICLECARRECORD_H

#include "Game/GadgetClassDesc.h"
#include "size_assert.h"

namespace Vehicle {
class TVehicleBase;
} // namespace Vehicle

/* The records of the four car kinds (Vehicle::TVehicleCar; Game/TVehicleCar.cpp), one array: the
 * car's code reaches a kind's record by index. Each is the gadget type record, then the vehicle
 * table's entry (its name and its create function; see Game/VehicleTable.cpp). Struct name
 * unknown. */
struct UnkCarRecord_8053B948 {
    /* 0x00 */ GadgetClassDesc desc;
    /* 0x24 */ const char *name;
    /* 0x28 */ Vehicle::TVehicleBase *(*create)(int kind);
};
SIZE_ASSERT(UnkCarRecord_8053B948, 0x2C);

extern UnkCarRecord_8053B948 lbl_8053B948[4];

#endif
