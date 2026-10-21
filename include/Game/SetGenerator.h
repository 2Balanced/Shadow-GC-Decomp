#ifndef GAME_SETGENERATOR_H
#define GAME_SETGENERATOR_H

#include "boost/shared_ptr.hpp"
#include "dolphin/types.h"
#include "size_assert.h"

/* SetGenerator::Manager (class name from the RTTI of its shared_ptr control block, 0x8054DE5C): the
 * 4-byte handle of a set generator, which the SET GENERATOR gadget (SetGenerator::SetGenerator,
 * code around 0x8027C000) and enemy gadgets such as BA GUNSHIP create from their "ComId", "num"
 * and "delay" parameters. Its shared_ptr's constructor, destructor and assignment are shared
 * instances at 0x8027C3B4, 0x8027C200 and 0x8027C1C0. Only what gadgets call is declared (names
 * and behaviour inferred). */
namespace SetGenerator {
class Manager;
} // namespace SetGenerator

/* What the generator is created from (struct name unknown; layout from its callers, which build it
 * zeroed and then copy three parameters in). */
struct UnkGeneratorParam_8027C3C4 {
    UnkGeneratorParam_8027C3C4() : comId(0), num(0), delay(0.0f) {}

    /* 0x0 */ s32 comId;
    /* 0x4 */ s32 num;
    /* 0x8 */ float delay;
};
SIZE_ASSERT(UnkGeneratorParam_8027C3C4, 0xC);

extern "C" {
/* Creates a generator (behaviour inferred). */
boost::shared_ptr<SetGenerator::Manager> fn_8027C3C4(const UnkGeneratorParam_8027C3C4 *param);
/* Clears and sets bit 0 of the generator's flags (stops and starts it; behaviour inferred), and
 * advances it by dt. */
void fn_8027C650(SetGenerator::Manager *manager);
void fn_8027C67C(SetGenerator::Manager *manager);
void fn_8027C6A8(SetGenerator::Manager *manager, float dt);
}

#endif
