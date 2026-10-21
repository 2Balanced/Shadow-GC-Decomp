#ifndef GAME_ADVBASE_H
#define GAME_ADVBASE_H

#include <dolphin/types.h>

namespace AdvReal {

/* AdvBase (class name from RTTI; vtable at 0x80557670): the base of the front-end phases
 * (AdvReal::Title::Phase, AdvReal::Option::Phase, AdvReal::MemoryCheck::Phase, ...). The
 * constructor is at 0x802B5224 and the destructor at 0x802B51DC. The two other virtual functions
 * (0x802B51D4 and 0x802B51CC) run the phase each frame and draw it; their names and the meaning
 * of the result are inferred. */
class AdvBase {
  public:
    AdvBase();
    virtual ~AdvBase();
    virtual int Tick();
    virtual int Draw();
};

} // namespace AdvReal

#endif
