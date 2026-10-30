/* The vehicle table: the vehicle kinds the game can create (Walker, the cars, the birds, the gun
 * lifts, ...), each named by the record its own unit defines, and whether the stage has loaded
 * it. Creating a vehicle gives it a Vehicle::CharaColliMove and a Vehicle::VehicleTask under the
 * stage's task layer 0xC. No class or RTTI names this code, so the file is named after the table;
 * the functions keep their placeholder names because other units call them by those names.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/CharaColliMove.h"
#include "Game/TVehicleBase.h"
#include "Game/Task.h"
#include "Game/VehicleTask.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>

using Vehicle::TVehicleBase;

/* A vehicle kind's entry (see Game/GunLift.cpp): its name and its create function, which takes
 * the kind. */
struct UnkVehicleEntry {
    const char *name;
    TVehicleBase *(*create)(int kind);
};

/* The vehicle kinds' entries, defined by their units (the four cars of Vehicle::TVehicleCar, the
 * Walker, the AirSaucer, the AirWing, the AirBolt, the gun lifts, ...). */
extern UnkVehicleEntry lbl_8053B96C;
extern UnkVehicleEntry lbl_8053B998;
extern UnkVehicleEntry lbl_8053B9C4;
extern UnkVehicleEntry lbl_8053B9F0;
extern UnkVehicleEntry lbl_805E83D8;
extern UnkVehicleEntry lbl_805E83E4;
extern UnkVehicleEntry lbl_805E8440;
extern UnkVehicleEntry lbl_805E8464;
extern UnkVehicleEntry lbl_805EC0E4;
extern UnkVehicleEntry lbl_805E52AC;
extern UnkVehicleEntry lbl_805E52E4;
extern UnkVehicleEntry lbl_805ED118;
extern UnkVehicleEntry lbl_805ED120;

extern "C" {
/* The stage's task root and one of its layers (see Game/AdvStaffRoll.cpp). */
void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);

/* The vehicles' common resources (behaviour inferred). */
void fn_801C0ADC(void);
void fn_801C0AD4(void);
}

/* A slot of the table (struct name unknown). */
struct UnkVehicleSlot {
    /* 0x0 */ UnkVehicleEntry *entry;
    /* 0x4 */ bool loaded;
};

static UnkVehicleSlot s_vehicles[14] = {
    {0, false},
    {&lbl_8053B96C, false},
    {&lbl_8053B998, false},
    {&lbl_8053B9C4, false},
    {&lbl_8053B9F0, false},
    {&lbl_805E83D8, false},
    {&lbl_805E83E4, false},
    {&lbl_805E8440, false},
    {&lbl_805E8464, false},
    {&lbl_805EC0E4, false},
    {&lbl_805E52AC, false},
    {&lbl_805E52E4, false},
    {&lbl_805ED118, false},
    {&lbl_805ED120, false},
};

extern "C" {

/* Whether the kind is loaded (name inferred). */
bool fn_801B4B5C(int kind) {
    return s_vehicles[kind].loaded;
}

/* The kind's name, or an empty string (name inferred). */
const char *fn_801B4B34(int kind) {
    UnkVehicleEntry *entry = s_vehicles[kind].entry;
    if (entry != 0) {
        return entry->name;
    }
    return "";
}

/* Creates a vehicle of the kind, if it is loaded; returns its task (name inferred). */
Task *fn_801B48D4(int kind) {
    UnkVehicleEntry *entry = s_vehicles[kind].entry;
    if (s_vehicles[kind].loaded && entry != 0 && entry->create != 0) {
        Task *parent = fn_801EB304(fn_8001C4F8(), 0xC);
        TVehicleBase *created = entry->create(kind);
        boost::shared_ptr<TVehicleBase> vehicle = boost::shared_ptr<TVehicleBase>(created);
        new Vehicle::CharaColliMove(parent, vehicle);
        return new Vehicle::VehicleTask(parent, vehicle);
    }
    return 0;
}

void fn_801B48B4(void) {
    fn_801C0AD4();
}

/* Marks every kind as not loaded (name inferred). */
void fn_801B486C(void) {
    fn_801C0ADC();
    for (int i = 0; i < 14; i++) {
        s_vehicles[i].loaded = false;
    }
}

/* Marks the kind as loaded; the kinds' initializers call it (name inferred). */
void fn_801B4850(int kind) {
    s_vehicles[kind].loaded = true;
}

} // extern "C"
