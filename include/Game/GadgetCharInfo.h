#ifndef SHADOW_GAME_GADGETCHARINFO_H
#define SHADOW_GAME_GADGETCHARINFO_H

#include "Game/CharInfo.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class CharCommandUnit;

/* GadgetCharInfo, named by the game's RTTI (vtable 0x8052AF4C): CharInfo for stage objects. It
 * adds an optional command unit; PushCommand (0x8015D0E0) gives each command to that unit, or
 * deletes the command when there is none. */
class GadgetCharInfo : public CharInfo {
  public:
    /* 0x8015D1C8; derived classes pass their character type. */
    GadgetCharInfo(int type);
    virtual ~GadgetCharInfo(); /* 0x8015D160 */
    virtual void PushCommand(CharCommand *command); /* 0x8015D0E0 */

    /* Writes the word at offset 0x50 of CharInfo (purpose unknown). */
    void SetField50(s32 value) {
        *reinterpret_cast<s32 *>(field_0x3C + (0x50 - 0x3C)) = value;
    }

    /* 0x64 */ boost::shared_ptr<CharCommandUnit> m_commandUnit;
};
SIZE_ASSERT(GadgetCharInfo, 0x6C);

extern "C" {
/* Creates the GadgetCharInfo for a gadget called `name` (the two numbers are not understood). */
boost::shared_ptr<GadgetCharInfo> fn_8015CB88(int arg0, int arg1, const char *name);
}

#endif
