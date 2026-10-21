/* Boss::Desire::DesireBase (class name from RTTI; see Game/BossDesire.h): the base of a boss's
 * desires. File name inferred from the class. Names marked "(name inferred)" are not original. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/BossDesire.h"
#define MSL_STRING_DEFAULT_CTOR
#include <string>
#include <dolphin/types.h>

extern "C" {
/* A 32-bit flag word: fn_80014E04 constructs it cleared (see Game/GadgetPickup.h). */
void fn_80014E04(u32 *flags);
}

/* Not original: an unused function that makes the compiler emit the array wrapper of the default
 * std::string constructor, whose retail copy (0x80329698) follows this unit's code; the linker
 * drops the function. Game/StageSequenceMovieTest.cpp is the wrapper's user. */
static std::string *NewStrings(u32 count) {
    return new std::string[count];
}

namespace Boss {
namespace Desire {

DesireBase::DesireBase(const char *name, int arg) {
    fn_80014E04(&field_0x4);
    m_name = name;
    field_0xC = arg;
    m_desire = 0.0f;
}

DesireBase::~DesireBase() {}

void DesireBase::Begin() {}

} // namespace Desire
} // namespace Boss
