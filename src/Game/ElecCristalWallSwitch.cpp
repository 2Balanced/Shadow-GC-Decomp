/* ElecCristalWallSwitch (class name from RTTI; "Cristal" is the original spelling): the
 * ELEC_WALLSWITCH gadget, an invisible trigger of the electric circuit area. Once the nearest
 * character comes within its "range" it turns its circuit switch on, which makes the
 * ElecCristalWall gadgets waiting on that switch rise into place. File name inferred from the
 * class. Method names marked "(name inferred)" are not original. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/DebugDraw.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* The parameter the owner supplies (name from the parameter table). */
struct ElecCristalWallSwitchParams {
    /* 0x0 */ float range;
};
SIZE_ASSERT(ElecCristalWallSwitchParams, 0x4);

class ElecCristalWallSwitch : public Gadget {
  public:
    ElecCristalWallSwitch(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~ElecCristalWallSwitch();
    virtual void Behave(float dt);
    virtual void fn_800CBC1C();
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the switch under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position, switch id and parameters into the gadget (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Draws the range as a debug sphere (name inferred). */
    void DrawRange();
    /* True when the nearest character is within the range (name inferred). */
    bool IsCharacterInRange();

    static const char *s_taskName;

    /* 0x30 */ ElecCristalWallSwitchParams m_params;
    /* 0x34 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x3C */ u8 m_switchId;
};
SIZE_ASSERT(ElecCristalWallSwitch, 0x40);

const char *ElecCristalWallSwitch::s_taskName = "ElecCristalWallSwitch";

ElecCristalWallSwitch::ElecCristalWallSwitch(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CB88(0x59, 4, "ElecCristalWallSwitch");
    ReadPlacement(m_owner);
}

ElecCristalWallSwitch::~ElecCristalWallSwitch() {}

void ElecCristalWallSwitch::Behave(float dt) {
    if (IsCharacterInRange()) {
        if (fn_8016904C(fn_800CB69C(), m_switchId) != 1) {
            fn_801690D0(fn_800CB69C(), m_switchId, 1);
        }
    }
}

void ElecCristalWallSwitch::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
}

void ElecCristalWallSwitch::fn_800CBC1C() {
    DrawRange();
}

void ElecCristalWallSwitch::DrawRange() {
    fn_802E0E84(&m_chara->m_position, &lbl_805F06D8, m_params.range);
}

void ElecCristalWallSwitch::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9E60(owner, &m_switchId);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
}

bool ElecCristalWallSwitch::IsCharacterInRange() {
    boost::shared_ptr<CharInfo> chara = fn_800A8A7C(fn_80074194(), &m_chara->m_position).lock();
    if (chara.get() != 0) {
        float range = m_params.range * m_params.range;
        Vec3f offset;

        fn_8040CD74(&offset, &m_chara->m_position, &chara->m_position);
        return fn_8040CF2C(&offset) <= range;
    }
    return false;
}

extern "C" void fn_801F9FF4(void) {}

extern "C" void fn_801F9FF0(void) {}

void ElecCristalWallSwitch::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new ElecCristalWallSwitch(fn_801EB304(fn_8001C4F8(), 11), owner);
}

GadgetParamDesc lbl_80542528[] = {
    {4, "range", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80542578 = {
    "ELEC_WALLSWITCH", fn_801F9FF4, fn_801F9FF0, ElecCristalWallSwitch::Instantiate, 0, 0, 0x7E1,
    1, 0, 10, lbl_80542528,
};
