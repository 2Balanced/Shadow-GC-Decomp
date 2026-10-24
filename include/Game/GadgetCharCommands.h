#ifndef GAME_GADGETCHARCOMMANDS_H
#define GAME_GADGETCHARCOMMANDS_H

#include "Game/CharCommand.h"
#include "size_assert.h"

namespace Sonicteam {
namespace System {
namespace Geometry {
class Line;
} // namespace Geometry
} // namespace System
} // namespace Sonicteam

/* More commands that gadgets queue on characters (class names from RTTI; the sizes come from the
 * operator new calls, the argument meanings are unknown unless noted). */

namespace Player {

/* CancelBehaviorCommand: its constructor is defined in the class but never inlined, so each user
 * emits a weak copy (the game keeps the dash panel's). In the original the base constructors it
 * calls are probably inline as well, which keeps it out of line under -inline level=1; the inline
 * id accessor stands in for that here (name inferred). */
class CancelBehaviorCommand : public PlayerCommand {
  public:
    static int Id() {
        return 0x101;
    }

    CancelBehaviorCommand(int arg) : PlayerCommand(Id(), arg) {}
    virtual ~CancelBehaviorCommand();
};

/* StartActionCommand (command id 0x106; constructor 0x80079484, vtable 0x805214C8; arg unknown). */
class StartActionCommand : public PlayerCommand {
  public:
    StartActionCommand(int arg);
};

class PressGroundCommand : public PlayerCommand {
  public:
    PressGroundCommand(int arg);
};

/* TouchHangCommand (command id 0x112): hangs the character on target. Its constructor (0x80115B70)
 * keeps the two vector pointers rather than copies (the names are inferred from UpDownReel, which
 * passes the hang position and rotation; arg3 unknown). The constructor is inline, never inlined
 * (the ROCKET unit emits the copy the game keeps); the destructor is out of line (vtable at
 * 0x805212E4). */
class TouchHangCommand : public PlayerCommand {
  public:
    TouchHangCommand(const boost::weak_ptr<CharInfo> &target, const Vec3f *position,
                     const Vec3f *rotation, int arg3, int arg4)
        : PlayerCommand(0x112, arg4), m_target(target), m_position(position),
          m_rotation(rotation), field_0x24(arg3) {}
    virtual ~TouchHangCommand();

    /* 0x14 */ boost::weak_ptr<CharInfo> m_target;
    /* 0x1C */ const Vec3f *m_position;
    /* 0x20 */ const Vec3f *m_rotation;
    /* 0x24 */ s32 field_0x24;
};

/* TouchLineCommand (command id 0x111; vtable 0x80521310): hangs the character on a line (the
 * catch stick's). Its constructor is defined in the class but not inlined, so its user emits a weak
 * copy (0x800F618C); the arguments are taken by value (the caller builds and destroys the copies).
 * Member names and arg's meaning are inferred. */
class TouchLineCommand : public PlayerCommand {
  public:
    TouchLineCommand(boost::weak_ptr<CharInfo> target,
                     boost::weak_ptr<Sonicteam::System::Geometry::Line> line, int arg)
        : PlayerCommand(0x111, arg), m_target(target), m_line(line) {}
    virtual ~TouchLineCommand();

    /* 0x14 */ boost::weak_ptr<CharInfo> m_target;
    /* 0x1C */ boost::weak_ptr<Sonicteam::System::Geometry::Line> m_line;
};

/* StartExternalControlCommand (command id 0x12B; constructor 0x8009E8D8): hands the player's
 * movement to the sender, and EndExternalControlCommand (command id 0x12C; constructor 0x8009E614)
 * gives it back (meanings inferred from the names; arguments unknown). */
class StartExternalControlCommand : public PlayerCommand {
  public:
    StartExternalControlCommand(int arg0, int arg1);
    /* Hands the character to target, which holds it at *position facing *rotation (the stretch
     * grass's use; names inferred). Defined in the class but not inlined, so its user emits a weak
     * copy (0x8014D730). */
    StartExternalControlCommand(const boost::weak_ptr<CharInfo> &target, const Vec3f *position,
                                const Vec3f *rotation, int arg0, int arg1)
        : PlayerCommand(0x12B, arg1), field_0x14(target), field_0x1C(reinterpret_cast<u32>(position)),
          field_0x20(reinterpret_cast<u32>(rotation)), field_0x24(arg0) {}
    virtual ~StartExternalControlCommand();

    /* 0x14 */ boost::weak_ptr<CharInfo> field_0x14;
    /* 0x1C */ u32 field_0x1C;
    /* 0x20 */ u32 field_0x20;
    /* 0x24 */ s32 field_0x24; /* arg0 */
};

/* SetMotionCommand (command id 0x129; constructor 0x8009E880): plays motion number motion on the
 * player (names inferred; the meaning of the float is unknown). */
class SetMotionCommand : public PlayerCommand {
  public:
    SetMotionCommand(int motion, int arg1, float arg2);

    /* 0x14 */ s32 m_motion;
    /* 0x18 */ float field_0x18;
};

/* With GADGETCHARCOMMANDS_INLINE_END_EXTERNAL_CONTROL defined, the constructor is defined in the
 * class with an implicit destructor, as the WEIGHT unit needs: its user then emits a weak copy of
 * the constructor and of the vtable, whose RTTI base lists stay in a .data kept whole (the player
 * code's copies, 0x8009E614 and 0x80521668, are the ones linked). The inline id accessor keeps
 * the constructor out of line, as with CancelBehaviorCommand. */
class EndExternalControlCommand : public PlayerCommand {
  public:
#ifdef GADGETCHARCOMMANDS_INLINE_END_EXTERNAL_CONTROL
    static int Id() {
        return 0x12C;
    }

    EndExternalControlCommand(int arg) : PlayerCommand(Id(), arg) {}
#else
    EndExternalControlCommand(int arg);
#endif
};

/* AddWeaponCommand: gives a character a weapon of a type with an amount of ammunition (meanings
 * inferred from SET_WEAPON). Its constructor is out of line in the same way as
 * CancelBehaviorCommand's. */
class AddWeaponCommand : public PlayerCommand {
  public:
    static int Id() {
        return 0x11A;
    }

    AddWeaponCommand(s32 type, s32 ammo, int arg) : PlayerCommand(Id(), arg), m_type(type), m_ammo(ammo) {}
    virtual ~AddWeaponCommand();

    /* 0x14 */ s32 m_type;
    /* 0x18 */ s32 m_ammo;
};

} // namespace Player

SIZE_ASSERT(Player::StartExternalControlCommand, 0x28);
SIZE_ASSERT(Player::EndExternalControlCommand, 0x14);
SIZE_ASSERT(Player::SetMotionCommand, 0x1C);
SIZE_ASSERT(Player::TouchHangCommand, 0x28);
SIZE_ASSERT(Player::TouchLineCommand, 0x24);
SIZE_ASSERT(Player::CancelBehaviorCommand, 0x14);
SIZE_ASSERT(Player::PressGroundCommand, 0x14);
SIZE_ASSERT(Player::AddWeaponCommand, 0x1C);

/* AccelerateCommand (command type 6) is declared in Game/CharCommand.h. */

namespace Player {

/* StreamCommand (command id 0x122; constructor 0x8009AD8C, vtable 0x805210D4): puts the player in
 * or out of a fan's stream (meaning inferred from the FAN gadget; arg1 unknown). */
class StreamCommand : public PlayerCommand {
  public:
    StreamCommand(bool on, int arg1);

    /* 0x14 */ s32 m_on; /* signed: Player::Behavior::LineHang compares it with cmpwi */
};

} // namespace Player

SIZE_ASSERT(Player::StreamCommand, 0x18);

namespace Player {

/* HomingJumpCommand (command id 0x10B; vtable 0x80521418): sent by the ITEM_BOX gadget to the
 * character that took an item (the meaning of the vector and of arg are unknown). Its constructor
 * is out of line in the same way as CancelBehaviorCommand's (the ITEM_BOX unit emits the copy the
 * game keeps, 0x8010DFD0); the vector is copied through fn_800091EC. */
class HomingJumpCommand : public PlayerCommand {
  public:
    static int Id() {
        return 0x10B;
    }

    HomingJumpCommand(const Vec3f &velocity, int arg) : PlayerCommand(Id(), arg) {
        fn_800091EC(&m_velocity, &velocity);
    }
    virtual ~HomingJumpCommand();

    /* 0x14 */ Vec3f m_velocity;
};

} // namespace Player

SIZE_ASSERT(Player::HomingJumpCommand, 0x20);

#endif
