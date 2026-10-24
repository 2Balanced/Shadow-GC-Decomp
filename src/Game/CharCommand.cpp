/* CharCommand (class name from RTTI): the out-of-line functions of the commands' base class, which
 * emit its vtable here, and fn_801D9C0C, which sends a command to a command unit through the
 * command queues (Game/CharCommandManagerTask.cpp). The constructor and destructor are inline (the
 * copies linked are 0x8006E454 and 0x8006E3A4). Names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include <string>

#define CHARCOMMAND_IMPLEMENTATION
#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" {
/* Allocates from heap and frees (see Game/AudioDataBank.cpp). */
void *fn_80049B90(size_t size, int heap);
void fn_80049B34(void *p);
}

/* The command queues (class name unknown; defined in Game/CharCommandManagerTask.cpp, which has
 * the members). */
class UnkCharCommandManager_8009978C {
  public:
    bool Send(CharCommand *command, boost::shared_ptr<CharCommandUnit> unit, u8 set, u8 mask);
};

extern "C" {
UnkCharCommandManager_8009978C *fn_8009978C(void);
}

void CharCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {}

std::string CharCommand::vf_0x10() const {
    /* GetName is not const here (its name and mangling come from the other units); the game's is
     * probably const as well. */
    return std::string(const_cast<CharCommand *>(this)->GetName());
}

const char *CharCommand::GetName() {
    return "Unknown";
}

void *CharCommand::operator new(size_t size) {
    return fn_80049B90(size, 2);
}

void CharCommand::operator delete(void *p, size_t size) {
    fn_80049B34(p);
}

/* Sends command to unit: it is delivered with the next commands of its level, or deleted while the
 * queues' task does not run (name inferred). */
extern "C" void fn_801D9C0C(boost::shared_ptr<CharCommandUnit> unit, CharCommand *command) {
    fn_8009978C()->Send(command, unit, 4, 0xFE);
}
