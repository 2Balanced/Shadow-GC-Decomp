#ifndef GAME_BOSSDESIRE_H
#define GAME_BOSSDESIRE_H

#include <dolphin/types.h>
#include "size_assert.h"

namespace Boss {
namespace Desire {

/* DesireBase (class name from RTTI; vtable 0x8055CF20, constructor 0x80329640, destructor
 * 0x803295F8): how much a boss wants to start one of its behaviors. The boss's AI raises the
 * desires each frame and starts the behavior of the strongest. Its code is in Game/BossDesire.cpp.
 * Member names are inferred. */
class DesireBase {
  public:
    /* name is the desire's name; the meaning of arg is unknown. */
    DesireBase(const char *name, int arg);
    virtual ~DesireBase();
    /* Raises the desire by dt (the game keeps the weak copy at 0x802D5E58). */
    virtual void Tick(float dt) {}
    /* Told when the desire's behavior starts (0x803295F4). */
    virtual void Begin();

    /* Raises the desire by value (name inferred). Retail calls the smart pointer's operator-> out
     * of line in its argument, as for an inline function. */
    void AddDesire(float value) {
        m_desire += value;
    }
    /* Whether the desire reached value, and setting it (names inferred; through the smart pointer,
     * retail calls its operator-> out of line for them, as for inline functions). */
    bool IsAtLeast(float value) const {
        return m_desire >= value;
    }
    void SetDesire(float value) {
        m_desire = value;
    }

    /* 0x04 */ u32 field_0x4; /* set up by fn_80014E04 */
    /* 0x08 */ const char *m_name;
    /* 0x0C */ s32 field_0xC;
    /* 0x10 */ float m_desire;
};
SIZE_ASSERT(DesireBase, 0x14);

} // namespace Desire
} // namespace Boss

#endif
