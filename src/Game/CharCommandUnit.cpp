/* CharCommandUnit (class name from RTTI; Game/CharCommandUnit.h): the receiver a character info
 * gives the commands sent to it. The command queues (Game/CharCommandManagerTask.cpp) pass each
 * command to its callback. Only the constructor and the destructor are here; the callback class's
 * constructor and destructor follow them as out-of-line copies. */
#include "Game/CharCommandUnit.h"
#include <dolphin/types.h>
#include "size_assert.h"

CharCommandUnit::CharCommandUnit() {}

CharCommandUnit::~CharCommandUnit() {}
