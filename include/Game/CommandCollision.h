#ifndef GAME_COMMANDCOLLISION_H
#define GAME_COMMANDCOLLISION_H

#include <boost/function.hpp>

#include "Game/Gadget.h"
#include "Game/GadgetColliDetectArgs.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class GadgetCharInfo;
class GadgetCharCollision;

/* The parameter block of a command collision gadget (0x18 bytes, read from the owner by
 * CommandCollision's constructor; struct and field names inferred from BK_TANK_COLI's parameter
 * names). */
struct UnkCommandCollisionParams_80260668 {
    /* 0x00 */ Vec3f m_size; /* X, Y, Z */
    /* 0x0C */ s32 m_type;
    /* 0x10 */ float m_time;
    /* 0x14 */ float m_rate;
};
SIZE_ASSERT(UnkCommandCollisionParams_80260668, 0x18);

/* What a command collision passes to its callback when a character touches it (struct name
 * unknown; layout from CommandCollision's collision handler, 0x8026003C): the collision, the
 * gadget's parameters and its flag word (bit 0 set stops further calls). */
struct UnkCommandCollisionEvent_8026003C {
    /* 0x0 */ GadgetColliDetectArgs *m_args;
    /* 0x4 */ UnkCommandCollisionParams_80260668 *m_params;
    /* 0x8 */ u32 *m_flags;
};

/* The callback member of a command collision: a boost::function1 whose assignment from another
 * is an out-of-line function of its own at 0x8025E1BC (class name unknown; as
 * UnkAttackCallback_80054E28 is for attack callbacks). */
typedef boost::function1<void, UnkCommandCollisionEvent_8026003C &> CommandCollisionCallback;
class UnkCommandCollisionCallback_8025E1BC : public CommandCollisionCallback {
  public:
    UnkCommandCollisionCallback_8025E1BC &operator=(const CommandCollisionCallback &f);
};

namespace CommandCollision {

/* CommandCollision::CommandCollision (class name from RTTI; vtable at 0x8054BA3C): a box-shaped
 * gadget (0x70 bytes) that calls m_callback while a character touches it. Stage gadget types such
 * as BK_TANK_COLI create one and set the callback; only the members those units use are laid out.
 * The constructor is at 0x80260668 (arg's meaning unknown). */
class CommandCollision : public Gadget {
  public:
    CommandCollision(Task *parent, UnkPlacement_800C9CD0 *owner, int arg);
    /* 0x802602B8 */
    virtual ~CommandCollision();
    /* 0x802601EC */
    virtual void Behave(float dt);
    /* 0x802601C0 */
    virtual void fn_800CBC1C();
    /* 0x8026015C */
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* 0x30 */ UnkCommandCollisionCallback_8025E1BC m_callback;
    /* 0x3C */ u8 field_0x3C[0x4C - 0x3C];
    /* 0x4C */ UnkCommandCollisionParams_80260668 m_params;
    /* 0x64 */ s32 m_state;
    /* 0x68 */ u32 m_flags;
    /* 0x6C */ s32 m_switchId;
};
SIZE_ASSERT(CommandCollision, 0x70);

} // namespace CommandCollision

#endif
