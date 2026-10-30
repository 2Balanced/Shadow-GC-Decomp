/* PickupCharInfo (class name from RTTI; see Game/PickupCharInfo.h): the character info of an object
 * a character can pick up, with the functions that give it a target, move the target and call its
 * grab-position callback. File name from the class. Names of the free functions are placeholders;
 * their behaviour is inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#define MSL_RUNTIME_ERROR_STRING
#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/PickupCharInfo.h"
#include "Game/Target.h"
#include "boost/detail/sp_counted_base_impl.hpp"
#include "boost/shared_ptr.hpp"

extern "C" {
/* Hands command to the command unit (behaviour inferred). */
void fn_801D9C0C(boost::shared_ptr<CharCommandUnit> unit, CharCommand *command);
/* Creates a target of owner with the given flags (see Game/Target.h). */
boost::shared_ptr<Target> fn_801E7128(const boost::weak_ptr<CharInfo> &owner, int flags);
}

/* Sets a target's radius (name inferred). */
inline void SetTargetRadius(Target *target, float radius) {
    target->m_radius = radius;
}

/* Whether callback is empty (name inferred). */
inline bool IsGrabCallbackEmpty(const PickupCharInfo::GrabCallback &callback) {
    return callback.m_function.empty();
}

/* Calls callback with args unless it is empty (name inferred). */
inline void CallGrabCallback(const PickupCharInfo::GrabCallback &callback, PickupCharInfo::SetGrabPositionArgs &args) {
    if (!callback.m_function.empty()) {
        callback.m_function(args);
    }
}

PickupCharInfo::PickupCharInfo(int type) : CharInfo(type) {
    m_commandUnit = boost::shared_ptr<CharCommandUnit>(new CharCommandUnit);
}

PickupCharInfo::~PickupCharInfo() {}

void PickupCharInfo::PushCommand(CharCommand *command) {
    fn_801D9C0C(m_commandUnit, command);
}

/* Calls charInfo's grab-position callback with the carrier's hand matrix (name inferred). */
extern "C" void fn_801E41C4(PickupCharInfo *charInfo, const Mtx34f *matrix) {
    if (!IsGrabCallbackEmpty(charInfo->m_grabCallback)) {
        PickupCharInfo::SetGrabPositionArgs args;
        args.m_matrix = matrix;
        CallGrabCallback(charInfo->m_grabCallback, args);
    }
}

/* Creates charInfo's target for owner with the given flags and radius (name inferred). */
extern "C" void fn_801E4160(PickupCharInfo *charInfo, const boost::weak_ptr<CharInfo> &owner, int flags,
                            float radius) {
    charInfo->m_target = fn_801E7128(owner, flags | 0x2020);
    SetTargetRadius(charInfo->m_target.operator->(), radius);
}

/* Moves charInfo's target to position (name inferred). */
extern "C" void fn_801E413C(PickupCharInfo *charInfo, const Vec3f *position) {
    charInfo->m_target->fn_800CCE38(position);
}
