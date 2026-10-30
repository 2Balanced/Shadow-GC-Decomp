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
#ifdef COMMANDCOLLISION_IMPL
    /* 0x00 */ float m_size[3]; /* X, Y, Z: plain floats in Game/CommandCollision.cpp, whose
                                 * constructor sets the block's defaults inline */
#else
    /* 0x00 */ Vec3f m_size; /* X, Y, Z */
#endif
    /* 0x0C */ s32 m_type;
    /* 0x10 */ float m_time;
    /* 0x14 */ float m_rate;
};
SIZE_ASSERT(UnkCommandCollisionParams_80260668, 0x18);

namespace CommandCollision {

/* What a command collision passes to its callback when a character touches it (name from the RTTI
 * of EscapePlaneColi's bind type; layout from CommandCollision's collision handler, 0x8026003C):
 * the collision, the gadget's parameters and its flag word (bit 0 set stops further calls). */
struct Args {
    /* 0x0 */ GadgetColliDetectArgs *m_args;
    /* 0x4 */ UnkCommandCollisionParams_80260668 *m_params;
    /* 0x8 */ u32 *m_flags;
};

} // namespace CommandCollision

/* The callback member of a command collision (class name unknown; as UnkColliCallback_800F5980 is
 * for GadgetCharCollision): it holds a boost::function1 as its only member (its destructor destroys
 * the function as a member, not as a base), and its assignment from a function is an out-of-line
 * function of its own at 0x8025E1BC. */
typedef boost::function1<void, CommandCollision::Args &> CommandCollisionCallback;
class UnkCommandCollisionCallback_8025E1BC {
  public:
#ifdef COMMANDCOLLISION_CALLBACK_ASSIGN
    /* Only Game/EscapePlaneColi.cpp, the first user, defines COMMANDCOLLISION_CALLBACK_ASSIGN: its
     * copy of the assignment is the one the game keeps (it calls function1's out of line). */
    UnkCommandCollisionCallback_8025E1BC &operator=(const CommandCollisionCallback &f) {
        m_function = f;
        return *this;
    }
#else
    UnkCommandCollisionCallback_8025E1BC &operator=(const CommandCollisionCallback &f);
#endif
#ifdef COMMANDCOLLISION_IMPL
    /* Calls the function when one is set (name inferred; Game/CommandCollision.cpp only). */
    void CallIfSet(CommandCollision::Args &event) {
        if (!m_function.empty()) {
            m_function(event);
        }
    }
#endif

    /* 0x0 */ CommandCollisionCallback m_function;
};

namespace CommandCollision {

#ifdef COMMANDCOLLISION_IMPL
extern "C" {
/* A 32-bit flag word (see BASwitch.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
}

/* The flag word at 0x68 (struct name unknown; bit 0: the collision has done its work). */
struct CommandCollisionFlags {
    void Init() {
        fn_80014E04(&m_bits);
    }
    void Clear() {
        fn_80014E10(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits;
};
#endif

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
#ifdef COMMANDCOLLISION_IMPL
    /* Only Game/CommandCollision.cpp defines COMMANDCOLLISION_IMPL and sees these members (the
     * gadget's character info and its contact handle); the other units see their size. */
    /* 0x3C */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x44 */ boost::shared_ptr<GadgetCharCollision> m_collision;
#else
    /* 0x3C */ u8 field_0x3C[0x4C - 0x3C];
#endif
    /* 0x4C */ UnkCommandCollisionParams_80260668 m_params;
    /* 0x64 */ s32 m_state;
#ifdef COMMANDCOLLISION_IMPL
    /* 0x68 */ CommandCollisionFlags m_flags;
#else
    /* 0x68 */ u32 m_flags;
#endif
    /* 0x6C */ s32 m_switchId;
#ifdef COMMANDCOLLISION_IMPL

    static const char *s_taskName;

    /* Member names are inferred. */
    /* Copies the owner's position, rotation, parameters and switch id. */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* The contact handle's callback: passes the event to m_callback until bit 0 of m_flags is set. */
    void OnColliDetect(ColliDetectArgs &args);
#endif
};
SIZE_ASSERT(CommandCollision, 0x70);

} // namespace CommandCollision

#endif
