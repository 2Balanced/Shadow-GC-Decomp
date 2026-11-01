/* NonMatching: every function body matches, but GC/2.7 materializes the boost bodies here that
 * retail's object imports: the shared_ptr<PlayerBase> and shared_count destructors and the three
 * sp_counted_base virtuals, which retail only calls out of line from ControlBase's destructor.
 * With them come sp_counted_base's vtable (36 more bytes of .data) and its type_info record (8
 * more of .sdata); retail's object has neither record, and lays ControlBase's vtable after the
 * NpcNoControl ones rather than first. The records land at other offsets, which costs three addi
 * immediates, at DOL 0x8006d188, 0x8006d1cc and 0x8006d234.
 *
 * @unnamed@NpcControlNoControl_cpp@::NpcNoControl (class name from RTTI; base
 * Player::Control::ControlBase): the control of an NPC that does nothing (control 3 of the NPC
 * control factories). The unit also holds ControlBase's own virtual functions and its vtable.
 * fn_8006D150 makes one; other units call it by its address, so it keeps that name. File name from
 * the RTTI name; function names are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
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
