/* SetVehicle (class SetVehicle::SetVehicleTask from RTTI; its task name is "SetVehicle"): the
 * SET_VEHICLE gadget, which places a vehicle of the type chosen by its "Vehicle" parameter at the
 * owner's position and rotation. It waits until the owner's switch id (if any) is on and the
 * TBlockManage block around it reports ready, then creates the vehicle task once and keeps track
 * of it; when the vehicle goes away the gadget finishes. File name inferred from the class. Names
 * marked "(name inferred)" are not original. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include "Game/Gadget.h"
#include "Game/GadgetClassDesc.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "boost/weak_ptr.hpp"
#include <cstring>
#include "size_assert.h"

/* The class that owns forBlockCheck (only the nesting is known here). */
class TBlockManage {
  public:
    class forBlockCheck;
};

/* The cursor handed to a parameter's label callback (layout from fn_800CBC20 and fn_8011E0A0;
 * names inferred). */
class UnkLabelQuery_800CBC20 {
  public:
    /* 0x0 */ const GadgetParamDesc *params;
    /* 0x4 */ s32 index; /* the parameter being shown */
    /* 0x8 */ s32 value; /* its current value */
};

extern "C" {
/* Returns the TBlockManage block containing position (behaviour inferred). */
boost::shared_ptr<TBlockManage::forBlockCheck> fn_8003A154(const Vec3f *position);
/* Reports whether the block is ready (behaviour inferred). */
bool fn_80039EDC(const boost::weak_ptr<TBlockManage::forBlockCheck> &block);

/* Owner helpers: fn_800C9938 returns a float built from the owner's byte at +0x23 and fn_800C9AD4
 * marks the owner done (behaviour inferred). */
float fn_800C9938(UnkPlacement_800C9CD0 *owner);
void fn_800C9AD4(UnkPlacement_800C9CD0 *owner);

/* True while neither task nor any of its parents is finished (behaviour inferred). */
bool fn_8004EAC0(Task *task);

/* Vehicle types and vehicles (behaviour inferred): fn_801B4B34 returns the name of a type and
 * fn_801B4B5C whether it can be created; fn_801B48D4 creates a vehicle of a type, fn_801B5B08
 * returns a vehicle's type, fn_801B5D1C sets a float of it and fn_801B5D24 places it.
 * fn_801B5AA4 and fn_801B5AD4 report states in which the vehicle is let go (meanings unknown). */
const char *fn_801B4B34(int type);
bool fn_801B4B5C(int type);
Task *fn_801B48D4(int type);
int fn_801B5B08(Task *vehicle);
void fn_801B5D1C(Task *vehicle, float value);
void fn_801B5D24(Task *vehicle, const Vec3f *position, const Vec3f *rotation);
bool fn_801B5AA4(Task *vehicle);
bool fn_801B5AD4(Task *vehicle);
}

namespace SetVehicle {

class SetVehicleTask : public Gadget {
  public:
    SetVehicleTask(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~SetVehicleTask();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Marks the vehicle finished and forgets it (name inferred). */
    void ReleaseVehicle();
    /* Creates the vehicle once the switch allows it, replacing one of another type (name
     * inferred). */
    void SpawnVehicle();
    /* False, after releasing the vehicle and marking the owner done, when the owner's switch id is
     * set and that switch is not on (name inferred). */
    bool CheckSwitch();

    /* Forgets the vehicle once it is gone or let go, and then marks the owner done (name
     * inferred). */
    void CheckVehicle() {
        if (m_vehicle != 0) {
            if (!fn_8004EAC0(m_vehicle)) {
                m_vehicle = 0;
            } else if (fn_801B5AA4(m_vehicle) && fn_801B5AD4(m_vehicle)) {
                m_vehicle = 0;
            }
            if (m_vehicle == 0 && m_owner != 0) {
                fn_800C9AD4(m_owner);
            }
        }
    }

    /* Member names are inferred. */
    /* 0x30 */ Task *m_vehicle;
    /* 0x34 */ s32 m_vehicleType; /* the "Vehicle" parameter */
    /* 0x38 */ Vec3f m_position;
    /* 0x44 */ Vec3f m_rotation;
    /* 0x50 */ boost::shared_ptr<TBlockManage::forBlockCheck> m_block;
    /* 0x58 */ bool m_done;  /* set once the vehicle was created or the switch was off */
    /* 0x59 */ u8 m_switchId; /* copied from the owner; 0 for none */
    /* 0x5C */ u32 field_0x5C; /* not touched by this unit */
};
SIZE_ASSERT(SetVehicleTask, 0x60);

SetVehicleTask::SetVehicleTask(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner), m_vehicle(0), m_rotation(0.0f, 0.0f, 0.0f), m_done(false) {
    m_name = "SetVehicle";
    fn_800C9F14(owner, &m_position);
    fn_800C9ED4(owner, &m_rotation);
    fn_800C9DBC(owner, &m_vehicleType, sizeof(m_vehicleType));
    fn_800C9E60(owner, &m_switchId);
    m_block = fn_8003A154(&m_position);
    if (!CheckSwitch()) {
        m_done = true;
    }
}

SetVehicleTask::~SetVehicleTask() {
    ReleaseVehicle();
}

void SetVehicleTask::Behave(float dt) {
    if (!m_done) {
        if (m_block.px == 0 || !fn_80039EDC(m_block)) {
            SpawnVehicle();
            if (m_vehicle != 0) {
                fn_801B5D24(m_vehicle, &m_position, &m_rotation);
            }
        }
    } else {
        CheckVehicle();
        if (m_vehicle == 0) {
            m_flags |= 1;
        }
    }
}

void SetVehicleTask::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_position);
    fn_800C9ED4(owner, &m_rotation);
    fn_800C9DBC(owner, &m_vehicleType, sizeof(m_vehicleType));
    fn_800C9E60(owner, &m_switchId);
    SpawnVehicle();
    if (m_vehicle != 0) {
        fn_801B5D24(m_vehicle, &m_position, &m_rotation);
    }
    CheckVehicle();
}

bool SetVehicleTask::CheckSwitch() {
    u8 switchId = m_switchId;

    if (switchId != 0 && fn_8016904C(fn_800CB69C(), switchId) != 1) {
        ReleaseVehicle();
        if (m_owner != 0) {
            fn_800C9AD4(m_owner);
        }
        return false;
    }
    return true;
}

void SetVehicleTask::SpawnVehicle() {
    if (CheckSwitch()) {
        if (m_vehicle != 0) {
            if (m_vehicleType == fn_801B5B08(m_vehicle)) {
                return;
            }
            ReleaseVehicle();
        }

        int type = m_vehicleType;
        if (fn_801B4B5C(type)) {
            m_vehicle = fn_801B48D4(type);
            if (m_vehicle != 0) {
                fn_801B5D1C(m_vehicle, fn_800C9938(m_owner));
            }
            m_done = true;
        }
    }
}

void SetVehicleTask::ReleaseVehicle() {
    if (m_vehicle != 0) {
        m_vehicle->m_flags |= 1;
        m_vehicle = 0;
    }
}

} // namespace SetVehicle

extern "C" void fn_8011E178(void) {}

extern "C" void fn_8011E174(void) {}

namespace SetVehicle {

void SetVehicleTask::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new SetVehicleTask(fn_801EB304(fn_8001C4F8(), 11), owner);
}

} // namespace SetVehicle

/* The label of the "Vehicle" parameter: the vehicle type's name, or "Not Available" for a type
 * without one (behaviour inferred). */
extern "C" const char *fn_8011E0A0(UnkLabelQuery_800CBC20 *cursor) {
    static const char *notAvailable = "Not Available";

    if (cursor->index == 0) {
        const char *name = fn_801B4B34(cursor->value);
        if (strcmp(name, "") == 0) {
            return notAvailable;
        }
        return name;
    }
    return 0;
}

static GadgetParamDesc lbl_80527620[] = {
    {1, "Vehicle", 1, 1, 13, 0.0f, 0.0f, 0.0f, fn_8011E0A0, 0},
    {0},
};

GadgetClassDesc lbl_80527670 = {
    "SET_VEHICLE", fn_8011E178, fn_8011E174, SetVehicle::SetVehicleTask::Instantiate, 0, 0, 0x4F, 1, 0,
    10, lbl_80527620,
};
