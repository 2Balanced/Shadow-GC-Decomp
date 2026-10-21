/* SetCollision (class SetCollision::SetCollision from RTTI; its s_taskName is "SetCollision"): the
 * SETCOLLISION gadget, an invisible collider placed by the stage. Its "form" picks the shape (0: a
 * sphere, 1: a box, 2: a cylinder; X, Y and Z give its size) and its "kind" what it does: kinds 0
 * and 3 to 10 make a collider of special attributes (guard, damage, death, Chaos Control, flight
 * and launch areas), while kinds 2, 11 and 12 bind a callback that, on the first contact while the
 * gadget is armed, sets the "Id" switch on, raises game event 6 ("Goal") or sets the switch off.
 * "appear" 0 arms it at once; otherwise it waits for the switch to come on and then enables the
 * collider. File name inferred from the class. Names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetColliDetectArgs.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetClassDesc.h"
#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::CharaColliBase;

/* The handle fn_8015B8C4 wraps a collider in (the class name is unknown; layout as in Fan). */
class GadgetCharCollision {
  public:
    /* 0x00 */ boost::function1<void, GadgetColliDetectArgs &> m_onContact;
    /* 0x0C */ u8 field_0xC[0x24 - 0xC];
    /* 0x24 */ CharaColliBase *m_collider;
};

/* The cursor handed to a parameter's label callback (layout from fn_800CBC20; names inferred). */
class UnkLabelQuery_800CBC20 {
  public:
    /* 0x0 */ const GadgetParamDesc *params;
    /* 0x4 */ s32 index; /* the parameter being shown */
    /* 0x8 */ s32 value; /* its current value */
};

/* The object fn_80074470 returns (class unknown). */
struct UnkObject_80074470;

extern "C" {
/* The collider manager, and sphere, box and cylinder colliders for charInfo (behaviour inferred;
 * the meanings of the integer arguments are unknown). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7DEC(void *manager,
                                              const boost::weak_ptr<CharInfo> &charInfo, int flags,
                                              int arg3, int arg4, const Vec3f *offset,
                                              float radius);
boost::shared_ptr<CharaColliBase> fn_800C7C34(void *manager,
                                              const boost::weak_ptr<CharInfo> &charInfo,
                                              const Vec3f *size, int flags, int arg4,
                                              int arg5, const Vec3f *offset,
                                              const Vec3f *rotation, int arg8);
boost::shared_ptr<CharaColliBase> fn_800C7A84(void *manager,
                                              const boost::weak_ptr<CharInfo> &charInfo, int flags,
                                              int arg3, u16 arg4, const Vec3f *offset,
                                              const Vec3f *rotation, float height,
                                              float radius, int arg9);
extern const Vec3f lbl_805E07B8;
/* Wraps collider in a new handle; draws the handle's shape for debugging and advances it by dt
 * (behaviour inferred). */
boost::shared_ptr<GadgetCharCollision> fn_8015B8C4(boost::shared_ptr<CharaColliBase> collider);
void fn_8015BE64(GadgetCharCollision *handle);
void fn_8015BE9C(GadgetCharCollision *handle, float dt);

/* True when the collision event's contacts hold a player (behaviour inferred). */
bool fn_8015D3E8(UnkColliContacts_8001EE04 *contacts);
/* Raises game event 6 (the goal) on the game mode object (behaviour inferred). */
UnkObject_80074470 *fn_80074470(void);
void fn_80175EEC(UnkObject_80074470 *object, int event, int arg2);
/* Tells the owner that the gadget is done (behaviour inferred). */
void fn_8015A794(Gadget *gadget);
}

namespace SetCollision {

/* The parameters the owner supplies (names from the parameter table). */
struct SetCollisionParams {
    SetCollisionParams() : kind(0), form(0), x(1.0f), y(1.0f), z(1.0f), id(0), appear(1) {}

    /* 0x00 */ s32 kind;
    /* 0x04 */ s32 form; /* 0 sphere, 1 box, 2 cylinder */
    /* 0x08 */ float x;
    /* 0x0C */ float y;
    /* 0x10 */ float z;
    /* 0x14 */ s32 id;     /* the switch */
    /* 0x18 */ s32 appear; /* 0 armed at once, 1 armed and enabled by the switch */
};
SIZE_ASSERT(SetCollisionParams, 0x1C);

class SetCollision : public Gadget {
  public:
    SetCollision(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~SetCollision();
    virtual void Behave(float dt);
    virtual void fn_800CBC1C();
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Method names are inferred. */
    void SetupCollision();
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    void OnColliNone(ColliDetectArgs &args);
    void OnColliSwitchOn(ColliDetectArgs &args);
    void OnColliGoal(ColliDetectArgs &args);
    void OnColliSwitchOff(ColliDetectArgs &args);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetCharCollision> m_handle;
    /* 0x40 */ SetCollisionParams m_params;
    /* 0x5C */ s32 m_state; /* 0 new, 1 waiting for the switch, 2 armed, 3 done */
};
SIZE_ASSERT(SetCollision, 0x60);

const char *SetCollision::s_taskName = "SetCollision";

SetCollision::SetCollision(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_state = 0;
    m_chara = fn_8015CB88(0, 4, "SetCollision");
    SetupCollision();
    LoadFromOwner(owner);
    if (m_params.appear == 1 && m_handle.get() != 0) {
        m_handle->m_collider->TurnOff(-1);
    }
}

SetCollision::~SetCollision() {}

void SetCollision::Behave(float dt) {
    if (m_handle.get() != 0) {
        fn_8015BE9C(m_handle.get(), dt);
    }
    switch (m_state) {
    case 0:
        if (m_params.appear == 0) {
            m_state = 2;
        } else {
            m_state = 1;
        }
        break;
    case 1:
        if (fn_8016904C(fn_800CB69C(), m_params.id) == 1) {
            m_state = 2;
            if (m_handle.get() != 0) {
                m_handle->m_collider->TurnOn(-1);
            }
        }
        break;
    case 2:
        if (m_params.appear == 0 && fn_8016904C(fn_800CB69C(), m_params.id) == 1) {
            m_state = 3;
        }
        break;
    case 3:
        fn_8015A794(this);
        m_flags |= 1;
        break;
    }
}

void SetCollision::fn_800CBC1C() {
    if (m_handle.get() != 0) {
        fn_8015BE64(m_handle.get());
    }
}

void SetCollision::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    int form = m_params.form;
    int kind = m_params.kind;

    LoadFromOwner(owner);
    if (form != m_params.form || kind != m_params.kind) {
        m_handle.reset();
        SetupCollision();
    }
    if (m_handle.get() != 0) {
        m_handle->m_collider->SetSize(m_params.x, m_params.y, m_params.z, -2);
    }
}

void SetCollision::SetupCollision() {
    Vec3f size;
    fn_800091EC(&size, &Vec3f(1.0f, 1.0f, 1.0f));
    boost::shared_ptr<CharaColliBase> collider;
    u32 flags = 0;
    int arg = 7;
    s32 charKind = 0;

    switch (m_params.kind) {
    case 0:
        flags = 0x102;
        arg = 7;
        break;
    case 1:
    case 2:
    case 11:
    case 12:
        arg = 10;
        flags = 0x40100;
        break;
    case 3:
        flags = 0x104;
        arg = 7;
        break;
    case 4:
        flags = 0x120;
        arg = 7;
        break;
    case 5:
        arg = 10;
        flags = 0x40100;
        charKind = 0x6F;
        break;
    case 6:
        arg = 10;
        flags = 0x40100;
        charKind = 0x70;
        break;
    case 7:
        arg = 10;
        flags = 0x40100;
        charKind = 0x71;
        break;
    case 8:
        arg = 10;
        flags = 0x40100;
        charKind = 0x72;
        break;
    case 9:
        arg = 10;
        flags = 0x40100;
        charKind = 0x73;
        break;
    case 10:
        arg = 10;
        flags = 0x40100;
        charKind = 0x74;
        break;
    }

    switch (m_params.form) {
    case 0:
        collider = fn_800C7DEC(fn_80021B08(), m_chara, flags, arg, 0, &lbl_805E07A0, 1.0f);
        break;
    case 1:
        collider = fn_800C7C34(fn_80021B08(), m_chara, &size, flags, arg, 0, &lbl_805E07A0,
                               &lbl_805E07B8, 0);
        break;
    case 2:
        collider = fn_800C7A84(fn_80021B08(), m_chara, flags, arg, 0, &lbl_805E07A0,
                               &lbl_805E07B8, 1.0f, 1.0f, 0);
        break;
    }
    if (m_chara.get() != 0) {
        m_chara->SetField50(charKind);
    }
    m_handle = fn_8015B8C4(collider);

    switch (m_params.kind) {
    case 1:
        fn_800F5980(m_handle.get(), boost::bind(&SetCollision::OnColliNone, this, _1));
        break;
    case 2:
        fn_800F5980(m_handle.get(), boost::bind(&SetCollision::OnColliSwitchOn, this, _1));
        break;
    case 11:
        fn_800F5980(m_handle.get(), boost::bind(&SetCollision::OnColliGoal, this, _1));
        break;
    case 12:
        fn_800F5980(m_handle.get(), boost::bind(&SetCollision::OnColliSwitchOff, this, _1));
        break;
    }
    m_handle->m_collider->SetSize(m_params.x, m_params.y, m_params.z, -2);
}

void SetCollision::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    if (m_chara.get() != 0) {
        fn_800C9F14(owner, &m_chara.get()->m_position);
        fn_800C9ED4(owner, &m_chara.get()->m_rotation);
    }
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
}

void SetCollision::OnColliNone(ColliDetectArgs &args) {}

void SetCollision::OnColliSwitchOn(ColliDetectArgs &args) {
    if (m_state == 2 && fn_8015D3E8(args.m_contacts)) {
        fn_801690D0(fn_800CB69C(), m_params.id, 1);
        m_state = 3;
    }
}

void SetCollision::OnColliGoal(ColliDetectArgs &args) {
    if (m_state == 2 && fn_8015D3E8(args.m_contacts)) {
        fn_80175EEC(fn_80074470(), 6, 0);
        m_state = 3;
    }
}

void SetCollision::OnColliSwitchOff(ColliDetectArgs &args) {
    if (m_state == 2 && fn_8015D3E8(args.m_contacts)) {
        fn_801690D0(fn_800CB69C(), m_params.id, 0);
        m_state = 3;
    }
}

void SetCollision::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new SetCollision(parent, owner);
}

} // namespace SetCollision

/* The labels of the "kind", "form", "X", "Y", "Z" and "appear" parameters; X, Y and Z are labelled
 * by the form last shown. */
extern "C" const char *fn_8011CB54(UnkLabelQuery_800CBC20 *cursor) {
    static const char *s_kindLabels[13] = {
        "Guard", "Not Available", "Start", "Damage", "Dead", "ChaosControlCancelOn",
        "ChaosControlCancelOff", "ChaosControlStop", "FlyEnd", "LaunchContinue", "NoInput", "Goal",
        "Off",
    };
    static const char *s_formLabels[3] = {"Sphere", "Box", "Cylinder"};
    static const char *s_xLabels[3] = {"Radius", "Width", "Radius"};
    static const char *s_yLabels[3] = {"", "Height", ""};
    static const char *s_zLabels[3] = {"", "Depth", "Height"};
    static const char *s_appearLabels[3] = {"Disappear", "Appear", "No Use"};
    static int s_kind = 0;
    static int s_form = 0;

    if (cursor->index == 0) {
        int kind = cursor->value;
        s_kind = kind;
        return s_kindLabels[kind];
    } else if (cursor->index == 1) {
        int form = cursor->value;
        s_form = form;
        return s_formLabels[form];
    } else if (cursor->index == 2) {
        return s_xLabels[s_form];
    } else if (cursor->index == 3) {
        return s_yLabels[s_form];
    } else if (cursor->index == 4) {
        return s_zLabels[s_form];
    } else if (cursor->index == 6) {
        return s_appearLabels[cursor->value];
    }
    return 0;
}

static GadgetParamDesc lbl_80527300[] = {
    {1, "kind", 0, 0, 12, 0.0f, 0.0f, 0.0f, fn_8011CB54, 0},
    {1, "form", 0, 0, 2, 0.0f, 0.0f, 0.0f, fn_8011CB54, 0},
    {4, "X", 0, 0, 0, 10.0f, 0.1f, 65536.0f, fn_8011CB54, 0},
    {4, "Y", 0, 0, 0, 10.0f, 0.1f, 65536.0f, fn_8011CB54, 0},
    {4, "Z", 0, 0, 0, 10.0f, 0.1f, 65536.0f, fn_8011CB54, 0},
    {1, "Id", 0, 0, 0xFF, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "appear", 0, 0, 2, 0.0f, 0.0f, 0.0f, fn_8011CB54, 0},
    {0},
};

GadgetClassDesc lbl_80527440 = {
    "SETCOLLISION", 0, 0, SetCollision::SetCollision::Instantiate, 0, 0, 0x50, 1, 0, 10, lbl_80527300,
};
