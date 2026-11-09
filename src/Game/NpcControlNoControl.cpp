/* NonMatching: every function body and the text order match. The two macros below move the
 * shared_ptr<PlayerBase>, shared_count and sp_counted_base bodies and sp_counted_base's vtable out
 * of this unit, as retail's object has them. What remains is the order of the two classes' own
 * records: ours puts ControlBase's vtable before NpcNoControl's, retail puts it after them. That
 * costs three addi immediates, at DOL 0x8006d188, 0x8006d1cc and 0x8006d234. Moving ControlBase's
 * destructor before NpcNoControl fixes the data order but reverses the text order; moving vf_0x18
 * or adding a stripped construction helper does not fix the data order.
 *
 * @unnamed@NpcControlNoControl_cpp@::NpcNoControl (class name from RTTI; base
 * Player::Control::ControlBase): the control of an NPC that does nothing (control 3 of the NPC
 * control factories). The unit also holds ControlBase's own virtual functions and its vtable.
 * fn_8006D150 makes one; other units call it by its address, so it keeps that name. File name from
 * the RTTI name; function names are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#define BOOST_SP_EXTERN_BASE_DESTRUCTOR
#define BOOST_SP_EXTERN_COUNTED_DESTRUCTORS
#include "Game/Player.h"
#include "Game/PlayerControl.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

using boost::shared_ptr;
using Player::PlayerBase;
using Player::Control::ControlBase;

namespace {

class NpcNoControl : public ControlBase {
  public:
    NpcNoControl(const shared_ptr<PlayerBase> &player) : ControlBase(player, s_taskName, 3) {}
    virtual ~NpcNoControl();
    virtual void vf_0x0C();
    virtual void vf_0x10();
    virtual void vf_0x14(float dt);

    static const char *s_taskName;
};
SIZE_ASSERT(NpcNoControl, 0x14);

const char *NpcNoControl::s_taskName = "NpcNoControl";

NpcNoControl::~NpcNoControl() {}

void NpcNoControl::vf_0x0C() {}

void NpcNoControl::vf_0x10() {}

void NpcNoControl::vf_0x14(float dt) {}

} // namespace

namespace Player {
namespace Control {

ControlBase::~ControlBase() {}

} // namespace Control
} // namespace Player

extern "C" ControlBase *fn_8006D150(const shared_ptr<PlayerBase> &player) {
    return new NpcNoControl(player);
}

namespace Player {
namespace Control {

void ControlBase::vf_0x18() {}

} // namespace Control
} // namespace Player
