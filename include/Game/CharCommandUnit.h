#ifndef GAME_CHARCOMMANDUNIT_H
#define GAME_CHARCOMMANDUNIT_H

#include <boost/function.hpp>

#include "Game/CharCommand.h"
#include "size_assert.h"

/* The callback type a command unit holds (name inferred). */
typedef boost::function1<void, CharCommand &> CharCommandCallback;

/* A command unit's callback: a class of the game's own holding the function1 (class name unknown).
 * Its default constructor and destructor are out of line with CharCommandUnit's (0x801DA7C0,
 * 0x801DA734, in Game/CharCommandManagerTask.cpp); fn_800729B0 assigns it (it calls function1's
 * assignment and returns the callback), and fn_8019FEF8 calls it with a command unless it is empty
 * (behaviours inferred). */
class UnkCharCommandCallback_801DA7C0 {
  public:
    /* 0x0 */ CharCommandCallback m_function;
};

/* CharCommandUnit (class name from RTTI): the 0x10-byte receiver of the commands sent to a
 * character info (GadgetCharInfo::m_commandUnit); it passes each command to m_callback (behaviour
 * inferred; see Game/CharCommandManagerTask.cpp). */
class CharCommandUnit {
  public:
    CharCommandUnit();
    virtual ~CharCommandUnit();

    /* 0x4 */ UnkCharCommandCallback_801DA7C0 m_callback;
};
SIZE_ASSERT(CharCommandUnit, 0x10);

/* Assigns source to callback (behaviour inferred: it calls function1's copy assignment and returns
 * callback). */
extern "C" UnkCharCommandCallback_801DA7C0 *fn_800729B0(UnkCharCommandCallback_801DA7C0 *callback,
                                                        const CharCommandCallback *source);

#endif
