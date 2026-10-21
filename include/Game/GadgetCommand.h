#ifndef GAME_GADGETCOMMAND_H
#define GAME_GADGETCOMMAND_H

#include "Game/CharCommand.h"
#include "size_assert.h"

namespace GadgetCommand {

/* GadgetCommand::GadgetCommand (class name from RTTI; vtable at 0x8052AFAC): the base of the
 * commands a gadget sends to the character info it controls (BKTank, EscapePlane, ...). Its
 * constructor (0x8025E758, a weak copy) passes the command type on to CharCommand's; the
 * destructor (0x8015D6AC) and the name lookup by type (0x8015D708) are out of line. */
class GadgetCommand : public CharCommand {
  public:
#ifdef GADGETCOMMAND_CTOR_NOT_INLINED
    GadgetCommand(int type, int arg) : CharCommand(CommandType(type), arg) {}
#else
    GadgetCommand(int type, int arg) : CharCommand(type, arg) {}
#endif
    virtual ~GadgetCommand();
    virtual const char *GetName();

#ifdef GADGETCOMMAND_CTOR_NOT_INLINED
    /* The command type as CharCommand stores it. A unit whose code must call the constructor out
     * of line, as the game's does (the copy at 0x8025E758), defines GADGETCOMMAND_CTOR_NOT_INLINED
     * before including this header: the constructor then calls this inline function and so is not
     * inlined itself. */
    static u16 CommandType(int type) {
        return type;
    }
#endif
};
SIZE_ASSERT(GadgetCommand, 0x14);

/* GadgetCommand::MoveStartCommand and MoveEndCommand (class names from RTTI; vtables at
 * 0x8054B880 and 0x8054B8AC): commands 0x301 and 0x302, which start and stop a path object for
 * m_time seconds (constructors 0x8025E798 and 0x8025E704, weak copies; meanings inferred from
 * BKTankController, which takes them). */
class MoveStartCommand : public GadgetCommand {
  public:
    MoveStartCommand(float time, int arg) : GadgetCommand(0x301, arg), m_time(time) {}
    virtual ~MoveStartCommand() {}

    /* 0x14 */ float m_time;
};
SIZE_ASSERT(MoveStartCommand, 0x18);

class MoveEndCommand : public GadgetCommand {
  public:
    MoveEndCommand(float time, int arg) : GadgetCommand(0x302, arg), m_time(time) {}
    virtual ~MoveEndCommand() {}

    /* 0x14 */ float m_time;
};
SIZE_ASSERT(MoveEndCommand, 0x18);

} // namespace GadgetCommand

#endif
