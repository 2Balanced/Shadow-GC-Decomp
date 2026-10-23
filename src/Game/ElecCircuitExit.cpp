/* ElecCircuitExit (class name from RTTI): the ELEC_EXIT gadget. It only keeps its placement and
 * switch id and refreshes its collision handle; it has no parameters and no model. File name
 * inferred from the class. Names marked "(name inferred)" are not original. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharCollision.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathConstants.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

extern "C" {
/* Updates the collision handle (behaviour inferred; see BaseDoor.cpp). */
void fn_8015BE64(GadgetCharCollision *handle);
}

class ElecCircuitExit : public Gadget {
  public:
    ElecCircuitExit(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~ElecCircuitExit();
    virtual void Behave(float dt);
    virtual void fn_800CBC1C();
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position, rotation and switch id (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Refreshes the collision handle (name inferred). */
    void UpdateCollision();

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ u8 m_switchId;
    /* 0x3C */ boost::shared_ptr<GadgetCharCollision> m_handle;
    /* 0x44 */ boost::weak_ptr<CharInfo> m_target;
    /* 0x4C */ u32 pad_0x4C; /* the class size is a multiple of 8 */
};
SIZE_ASSERT(ElecCircuitExit, 0x50);

/* An exit's rotation (name inferred; nothing reads it). */
static Vec3f s_rotation(0.5f * lbl_805FAD38, 0.0f, 0.0f);

const char *ElecCircuitExit::s_taskName = "ElecCircuitExit";

ElecCircuitExit::ElecCircuitExit(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CB88(0x59, 4, "ElecCircuitExit");
    ReadPlacement(m_owner);
}

ElecCircuitExit::~ElecCircuitExit() {}

void ElecCircuitExit::Behave(float dt) {}

void ElecCircuitExit::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
}

void ElecCircuitExit::fn_800CBC1C() {
    UpdateCollision();
}

void ElecCircuitExit::UpdateCollision() {
    if (m_handle.get() != 0) {
        fn_8015BE64(m_handle.get());
    }
}

void ElecCircuitExit::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9E60(owner, &m_switchId);
}

extern "C" void fn_802C8100(void) {}

extern "C" void fn_802C80FC(void) {}

void ElecCircuitExit::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new ElecCircuitExit(fn_801EB304(fn_8001C4F8(), 11), owner);
}

GadgetClassDesc lbl_80557B00 = {
    "ELEC_EXIT", fn_802C8100, fn_802C80FC, ElecCircuitExit::Instantiate, 0, 0, 0x7EA, 1, 0, 10, 0,
};
