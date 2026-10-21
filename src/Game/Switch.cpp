/* Switch::Switch (class name from RTTI; its task name is "Switch"): the SWITCH gadget, a floor switch
 * that sets the shared switch state of its owner's switch id. Its "type" parameter picks how: 0
 * (OnOff) toggles the state when touched or attacked, 1 (OnTouch) turns it on while touched and off
 * when released, and 2 (OnAlways) turns it on for good. The model shows variant 1 while the state
 * is on and variant 2 while it is off, and a Target follows the switch. File name inferred from the
 * class. Names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first).
 *
 * Every function matches. The last 0x44 bytes of .data (0x80527BA4), the std::exception RTTI record
 * and its name come from boost::bad_weak_ptr, which weak_ptr::lock() throws in the newer
 * boost/shared_ptr.hpp and weak_ptr.hpp on main: compiled with those headers this unit emits the
 * std::exception and bad_weak_ptr vtables, bad_weak_ptr's base list and sp_counted_base's vtable in
 * that order (0x10 + 0xC + 0x10 + 0x18 bytes). Retail has the base list there with zeros in place
 * of the three vtables (the code addresses .data relative to its first object, so the linker keeps
 * the whole section). */
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "boost/bind.hpp"
#include "boost/function.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;

/* Target (class name from RTTI): a point other objects can aim at (meaning inferred). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);
};

/* The cursor handed to a parameter's label callback (layout from fn_800CBC20 and fn_801230D4;
 * names inferred). */
class UnkLabelQuery_800CBC20 {
  public:
    /* 0x0 */ const GadgetParamDesc *params;
    /* 0x4 */ s32 index; /* the parameter being shown */
    /* 0x8 */ s32 value; /* its current value */
};

extern "C" {
/* A 32-bit flag word: fn_80014E04 constructs it cleared, fn_80014E10 clears it, fn_80014E1C sets
 * bit, fn_8006127C clears bit and fn_80014E34 tests bit (behaviour inferred from their bodies). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);

/* Returns the attack of a model variant's collision (behaviour inferred). */
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int variant);
/* Show and hide a model (behaviour inferred). */
void fn_8016271C(GadgetModelInfo *model);
void fn_80162790(GadgetModelInfo *model);

/* Plays sound id at a character info (behaviour inferred). */
void fn_801D54F4(void *sound, int id, GadgetCharInfo *source, int arg3, int arg4);

/* Creates a target with the given flags, and removes a gadget's target (behaviour inferred). */
boost::shared_ptr<Target> fn_801E72DC(int flags);
void fn_801E7108(boost::shared_ptr<Target> *target);
}

namespace Switch {

class Switch : public Gadget {
  public:
    Switch(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~Switch();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Shows the model variant for the switch state (name inferred). */
    void UpdateState();
    /* The attack callback (name inferred). */
    void OnAttack(AttackCallbackParam &param);
    /* The collision callback for a character leaving the switch (name inferred). */
    void OnRelease(ColliDetectArgs &args);
    /* The collision callback for a character touching the switch (name inferred). */
    void OnTouch(ColliDetectArgs &args);
    /* Places the models and the target (name inferred). */
    void RebuildTransform();
    /* Copies the owner's position, rotation, switch id and type (name inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;    /* variant 0, with the collision */
    /* 0x40 */ boost::shared_ptr<GadgetModelInfo> m_onModel;  /* variant 1 */
    /* 0x48 */ boost::shared_ptr<GadgetModelInfo> m_offModel; /* variant 2 */
    /* 0x50 */ boost::shared_ptr<Target> m_target;
    /* 0x58 */ s32 m_type; /* the "type" parameter */
    /* 0x5C */ u8 m_switchId;
    /* 0x60 */ u32 m_flags; /* bit 0: pressed this frame */
    /* 0x64 */ u32 pad_0x64; /* the class size is a multiple of 8 */
};
SIZE_ASSERT(Switch, 0x68);

const char *Switch::s_taskName = "Switch";

Switch::Switch(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner), m_type(0) {
    fn_80014E04(&m_flags);
    m_name = s_taskName;
    m_chara = fn_8015CD90(9, 4);
    m_model = fn_8015F9D8(0, 0xE1);
    m_model->fn_80162A68(0, 0);
    m_onModel = fn_8015F9D8(0, 0xE1);
    m_onModel->fn_80162A68(1, 0);
    m_offModel = fn_8015F9D8(0, 0xE1);
    m_offModel->fn_80162A68(2, 0);
    m_model->fn_80161F08("col", m_chara);
    fn_800D0B64(m_model->field_0x4 + 0xC, boost::bind(&Switch::OnTouch, this, _1));
    fn_800D0B64(m_model->field_0x4 + 0x18, boost::bind(&Switch::OnRelease, this, _1));

    boost::weak_ptr<CharaColliAttack> attack = fn_80161428(m_model.get(), 0);
    if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
        collision->field_0x54 = 0;
        collision->field_0x55 = 1;
        collision->m_attackCallback =
            SonicteamUSA::System::CharaColli::AttackCallback(boost::bind(&Switch::OnAttack, this, _1));
    }
    m_target = fn_801E72DC(0x800);
    m_switchId = 0;
    LoadFromOwner(m_owner);
    RebuildTransform();
    UpdateState();
}

Switch::~Switch() {
    fn_801E7108(&m_target);
}

void Switch::Behave(float dt) {
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }
    fn_8006127C(&m_flags, 0);
}

void Switch::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    RebuildTransform();
    if (m_model.get() != 0) {
        m_model->fn_8016108C();
    }
}

void Switch::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9E60(owner, &m_switchId);
    fn_800C9DBC(owner, &m_type, sizeof(m_type));
    fn_80014E10(&m_flags);
}

void Switch::RebuildTransform() {
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_chara->m_position);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    fn_8040B884(&matrix, m_chara->m_rotation.x);
    fn_8040B80C(&matrix, m_chara->m_rotation.z);
    if (m_model.get() != 0) {
        m_model->fn_801633DC(&matrix);
    }
    if (m_onModel.get() != 0) {
        m_onModel->fn_801633DC(&matrix);
    }
    if (m_offModel.get() != 0) {
        m_offModel->fn_801633DC(&matrix);
    }
    if (m_target.get() != 0) {
        Vec3f position;
        fn_800091EC(&position, &m_chara->m_position);
        position.y += 5.0f;
        m_target->fn_800CCE38(&position);
    }
}

void Switch::OnTouch(ColliDetectArgs &args) {
    if (!fn_80014E34(&m_flags, 0)) {
        switch (m_type) {
        case 0:
            if (fn_8016904C(fn_800CB69C(), m_switchId) == 1) {
                fn_801690D0(fn_800CB69C(), m_switchId, 0);
            } else {
                fn_801690D0(fn_800CB69C(), m_switchId, 1);
            }
            fn_801D54F4(fn_80053164(), 0xE016, m_chara.px, 0, 1);
            break;
        case 1:
            if (fn_8016904C(fn_800CB69C(), m_switchId) != 1) {
                fn_801690D0(fn_800CB69C(), m_switchId, 1);
                fn_801D54F4(fn_80053164(), 0xE016, m_chara.px, 0, 1);
            }
            break;
        case 2:
            if (fn_8016904C(fn_800CB69C(), m_switchId) != 1) {
                fn_801690D0(fn_800CB69C(), m_switchId, 1);
                fn_801D54F4(fn_80053164(), 0xE016, m_chara.px, 0, 1);
            }
            break;
        }
        UpdateState();
        fn_80014E1C(&m_flags, 0);
    }
}

void Switch::OnRelease(ColliDetectArgs &args) {
    switch (m_type) {
    case 0:
        break;
    case 1:
        fn_801690D0(fn_800CB69C(), m_switchId, 0);
        break;
    case 2:
        break;
    }
    UpdateState();
}

void Switch::OnAttack(AttackCallbackParam &param) {
    param.m_result = 5;
    if (!fn_80014E34(&m_flags, 0)) {
        if (boost::shared_ptr<CharaColliAttack> collision = param.m_attacker.lock()) {
            if (!(collision->m_flags & 0x240)) {
                param.m_result = 5;
                return;
            }
        }
        param.m_result = 1;
        switch (m_type) {
        case 0:
            if (fn_8016904C(fn_800CB69C(), m_switchId) == 1) {
                fn_801690D0(fn_800CB69C(), m_switchId, 0);
            } else {
                fn_801690D0(fn_800CB69C(), m_switchId, 1);
            }
            break;
        case 1:
            break;
        case 2:
            fn_801690D0(fn_800CB69C(), m_switchId, 1);
            break;
        }
        UpdateState();
        fn_80014E1C(&m_flags, 0);
    }
}

void Switch::UpdateState() {
    if (fn_8016904C(fn_800CB69C(), m_switchId) == 1) {
        fn_8016271C(m_onModel.get());
        fn_80162790(m_offModel.get());
    } else {
        fn_80162790(m_onModel.get());
        fn_8016271C(m_offModel.get());
    }
}

} // namespace Switch

extern "C" void fn_8012315C(void) {
    fn_8006B50C(0xE1, 1);
}

extern "C" void fn_80123158(void) {}

void Switch::Switch::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new Switch(parent, owner);
}

/* The labels of the "type" parameter's values. */
static const char *lbl_80527AE4[] = {
    "type : OnOff",
    "type : OnTouch",
    "type : OnAlways",
};

/* The label of the "type" parameter (behaviour inferred). */
extern "C" const char *fn_801230D4(UnkLabelQuery_800CBC20 *cursor) {
    if (cursor->index == 0) {
        return lbl_80527AE4[cursor->value];
    }
    return 0;
}

static GadgetParamDesc lbl_80527AF0[] = {
    {1, "type", 0, 0, 2, 0.0f, 0.0f, 0.0f, fn_801230D4, 0},
    {0},
};

GadgetClassDesc lbl_80527B40 = {
    "SWITCH", fn_8012315C, fn_80123158, Switch::Switch::Instantiate, 0, 0, 0x15, 1, 0, 10, lbl_80527AF0,
};
