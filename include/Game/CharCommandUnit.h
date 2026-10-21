#ifndef GAME_CHARCOMMANDUNIT_H
#define GAME_CHARCOMMANDUNIT_H

#include <boost/function.hpp>

#include "Game/CharCommand.h"
#include "size_assert.h"

/* The callback type a command unit holds (name inferred). */
typedef boost::function1<void, CharCommand &> CharCommandCallback;

/* CharCommandUnit (class name from RTTI): the 0x10-byte receiver of the commands sent to a
 * character info (GadgetCharInfo::m_commandUnit); it passes each command to m_callback (behaviour
 * inferred). */
class CharCommandUnit {
  public:
    CharCommandUnit();
    virtual ~CharCommandUnit();

    /* 0x4 */ CharCommandCallback m_callback;
};
SIZE_ASSERT(CharCommandUnit, 0x10);

/* Assigns source to callback (behaviour inferred: it calls function1's copy assignment and returns
 * callback). */
extern "C" CharCommandCallback *fn_800729B0(CharCommandCallback *callback,
                                            const CharCommandCallback *source);

#endif
