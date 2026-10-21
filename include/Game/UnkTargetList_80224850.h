#ifndef GAME_UNKTARGETLIST_80224850_H
#define GAME_UNKTARGETLIST_80224850_H

#include "boost/shared_ptr.hpp"
#include <vector>
#include "size_assert.h"

/* Target (class name from RTTI): units that use it declare its members. */
class Target;

/* A list of Target pointers (class name unknown): a std::vector<boost::shared_ptr<Target> > whose
 * out-of-line members DEFENSE_SYSTEM's unit emits (0x80223FB8..0x80225284) and ARKCANNON calls.
 * Its implicit constructor and destructor are the game's copies at 0x80224850 and 0x80224508. */
struct UnkTargetList_80224850 : public std::vector<boost::shared_ptr<Target> > {};
SIZE_ASSERT(UnkTargetList_80224850, 0xC);

#endif
