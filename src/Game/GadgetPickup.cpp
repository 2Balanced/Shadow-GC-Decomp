/* GadgetPickup (namespace name from RTTI): the tasks that run pickup objects (see
 * Game/GadgetPickup.h). A SetPickupBaseTask is the gadget of a pickup placed in a stage: it runs
 * the object through a PickupBaseTask of its own, hands that task over to task layer 11 once the
 * object is done (or out of range of the owner's range plus a margin), and reports the gadget
 * finished. Game code runs spawned pickups through a PickupBaseTask alone. File name inferred from
 * the namespace. Names other than the class names are inferred. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetPickup.h"
#include "Game/Task.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"

extern "C" {
/* Moves task under parent in the task tree (behaviour inferred). */
void fn_8004EB80(Task *task, Task *parent);
/* Reads the range the owner was placed with (behaviour inferred). */
void fn_800C9E18(UnkPlacement_800C9CD0 *owner, float *range);
/* Tells the owner that the gadget is done (behaviour inferred; as in Fire.cpp). */
void fn_8015A794(Gadget *gadget);
/* Whether the character info is farther than the square root of rangeSq from the player
 * (behaviour inferred). */
bool fn_80169BE4(CharInfo *info, float rangeSq);
}

/* Returns x unchanged (name inferred; as in Fire.cpp). Being initialised through a function call,
 * the margin below is set up at startup. */
inline float Plain(float x) {
    return x;
}

namespace GadgetPickup {

/* What a placed pickup's range is widened by (name inferred). */
float s_rangeMargin = Plain(50.0f);

const char *SetPickupBaseTask::s_taskName = "SetPickupBaseTask";

void PickupBase::SetPosition(const Vec3f &position, const Vec3f &rotation) {
    fn_800091D0(&m_position, &position);
    m_rotation.x = rotation.x;
    m_rotation.y = rotation.y;
    m_rotation.z = rotation.z;
}

PickupBaseTask::PickupBaseTask(Task *parent, const boost::shared_ptr<PickupBase> &pickup)
    : Task(parent) {
    m_pickup = pickup;
    m_rangeSq = 0.0f;
    m_name = (const char *)m_pickup->field_0x3C;
}

PickupBaseTask::~PickupBaseTask() {}

void PickupBaseTask::Tick(float dt) {
    m_pickup->Tick(dt);
    PickupFlags request;
    request.Set(1);
    if (m_pickup->IsDone(&request)) {
        m_flags |= 1;
    }
    if (m_rangeSq > 0.0f && fn_80169BE4(m_pickup.get(), m_rangeSq)) {
        m_flags |= 1;
    }
}

void PickupBaseTask::ResetPickup(UnkPlacement_800C9CD0 *owner) {
    m_pickup->ReloadPlacement(owner);
}

bool PickupBaseTask::IsPickupDone() {
    PickupFlags request;
    request.Set(0);
    request.Set(1);
    return m_pickup->IsDone(&request);
}

void PickupBaseTask::SetRange(float range) {
    m_rangeSq = range * range;
}

SetPickupBaseTask::SetPickupBaseTask(Task *parent, UnkPlacement_800C9CD0 *owner,
                                     const boost::shared_ptr<PickupBase> &pickup)
    : Gadget(parent, owner) {
    m_name = s_taskName;
    m_task = new PickupBaseTask(0, pickup);
    ReloadPlacement(owner);
}

SetPickupBaseTask::~SetPickupBaseTask() {
    if (m_task != 0) {
        Task *layer = fn_801EB304(fn_8001C4F8(), 15);
        fn_8004EB80(m_task, layer);
        m_task->m_flags |= 1;
    }
}

void SetPickupBaseTask::Behave(float dt) {
    if (m_task != 0) {
        m_task->Tick(dt);
        if (m_task->IsPickupDone()) {
            Task *layer = fn_801EB304(fn_8001C4F8(), 11);
            fn_8004EB80(m_task, layer);
            m_task = 0;
            fn_8015A794(this);
            m_flags |= 1;
        }
    }
}

void SetPickupBaseTask::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    if (m_task != 0) {
        m_task->ResetPickup(owner);
        float range = 0.0f;
        fn_800C9E18(owner, &range);
        range += s_rangeMargin;
        m_task->m_rangeSq = range * range;
    }
}

} // namespace GadgetPickup
