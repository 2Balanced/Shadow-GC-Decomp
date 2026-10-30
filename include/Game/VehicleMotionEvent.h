#ifndef GAME_VEHICLEMOTIONEVENT_H
#define GAME_VEHICLEMOTIONEVENT_H

#include <boost/function.hpp>

#include <dolphin/types.h>

/* The game has one shared copy of std::swap for boost::function's any_pointer (0x80019164), which
 * function1::swap calls (as in Game/PlayerMotionEvent.h). */
namespace std {
void swap(boost::detail::function::any_pointer &a, boost::detail::function::any_pointer &b);
}

namespace Vehicle {

/* MotionEventArgs (class name from the RTTI of Vehicle::WalkerModelImpl's bound callback): what a
 * vehicle's motion passes to its event callback (Game/UnkMotion_8022F058.cpp): the event number
 * and the values it carries (laid out as Player::MotionEventArgs; member names inferred). */
struct MotionEventArgs {
    /* 0x0 */ u16 m_event;
    /* 0x4 */ s32 *m_value;
};

/* The function a vehicle's motion calls back with its events. */
typedef boost::function1<void, MotionEventArgs &> MotionEventCallback;

} // namespace Vehicle

#endif
