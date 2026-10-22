/* HintCollision::SetHintCollision (class name from RTTI; its s_taskName is "SetHintCollision"): the
 * HINT COLLISION gadget, an invisible collider placed by the stage that has the hint manager show
 * a message the first time a character touches it. Its "form" picks the shape (0: a sphere, 1: a
 * box, 2: a cylinder; X, Y and Z give its size). "Message Id" is the message; "Brunch" its branch,
 * or -1 to take the branch from the player character. "flag" picks the targets it makes (H, D, E
 * and their sums; None makes none), "range" their size; "appear" 1 keeps the gadget off until the
 * placement's switch comes on. File name inferred from the class. Names marked "(name inferred)"
 * are not original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharCollision.h"
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

/* Target (class name from RTTI): a point other objects can aim at (members as in
 * BAGunShip.cpp; the radius at 0x20 is inferred). */
class Target {
  public:
    void TurnOn() {
        m_flags |= 1;
    }
    void TurnOff() {
        m_flags &= ~1;
    }
    void SetRadius(float radius) {
        m_radius = radius;
    }
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);

    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ float m_radius;
    /* 0x24 */ u8 field_0x24[0x30 - 0x24];
    /* 0x30 */ u32 m_flags;
};

/* The cursor handed to a parameter's label callback (layout from fn_800CBC20; names inferred). */
class UnkLabelQuery_800CBC20 {
  public:
    /* 0x0 */ const GadgetParamDesc *params;
    /* 0x4 */ s32 index; /* the parameter being shown */
    /* 0x8 */ s32 value; /* its current value */
};

/* The player manager (see BDHologram.cpp); the word at 0x20 is the character the player plays
 * (meaning inferred). */
class UnkPlayerManager_8006D6A8 {
  public:
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ s32 field_0x20;
};

/* The hint manager (class name unknown; see HintRing.cpp and EnemyHint.cpp). */
class UnkHintManager_801FB180;

/* The out-of-line copy of the inline accessor HintRing.cpp defines (0x80109D64). */
UnkHintManager_801FB180 *GetHintManager();

extern "C" {
/* A 32-bit flag word: fn_80014E04 constructs it cleared, fn_80014E10 clears it, fn_80014E1C sets
 * bit, fn_8006127C clears bit and fn_80014E34 tests bit (as in BAGunShip.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
/* The collider manager, and sphere, box and cylinder colliders for charInfo (as in
 * SetCollision.cpp). */
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
 * (as in SetCollision.cpp). */
boost::shared_ptr<GadgetCharCollision> fn_8015B8C4(boost::shared_ptr<CharaColliBase> collider);
void fn_8015BE64(GadgetCharCollision *handle);
void fn_8015BE9C(GadgetCharCollision *handle, float dt);
/* True when the collision event's contacts hold a player, and the same for any character
 * (behaviour inferred; see BAGunShip.cpp). */
bool fn_8015AC84(ColliDetectArgs &args);
bool fn_8015B1C4(ColliDetectArgs &args);
/* Tells the owner that the gadget is done (as in SetCollision.cpp). */
void fn_8015A794(Gadget *gadget);
/* The switch id of a placement (as in BAGunShip.cpp). */
void fn_800C9E50(UnkPlacement_800C9CD0 *owner, s32 *id);
/* A new target of flags, and the target's removal from the target manager (as in
 * BAGunShip.cpp). */
boost::shared_ptr<Target> fn_801E72DC(int flags);
void fn_801E7108(boost::shared_ptr<Target> *target);
/* A length scaled by a game setting (see EggLastMechCamera.cpp). */
float fn_8005AE20(float value);
/* The player manager, and the hint manager showing message id of branch (see BDHologram.cpp). */
UnkPlayerManager_8006D6A8 *fn_8006D6A8(void);
s32 fn_801FAF00(UnkHintManager_801FB180 *manager, s32 id, s32 branch, int arg3, int arg4);
}

namespace HintCollision {

/* The flag word at 0x78 (struct name unknown): bit 0 makes any character trigger the gadget (not
 * just players), bit 1 marks the message as shown (meanings inferred). */
struct SetHintCollisionFlags {
    SetHintCollisionFlags() {
        fn_80014E04(&m_bits);
    }

    u32 m_bits;
};

/* The parameters the owner supplies (names from the parameter table). */
struct SetHintCollisionParams {
    SetHintCollisionParams()
        : form(0), x(1.0f), y(1.0f), z(1.0f), messageId(0), brunch(-1), range(10.0f), sec(0.0f),
          flag(7), appear(0) {}

    /* 0x00 */ s32 form; /* 0 sphere, 1 box, 2 cylinder */
    /* 0x04 */ float x;
    /* 0x08 */ float y;
    /* 0x0C */ float z;
    /* 0x10 */ s32 messageId;
    /* 0x14 */ s32 brunch; /* the message's branch; -1 picks it by the player character */
    /* 0x18 */ float range; /* the targets' size */
    /* 0x1C */ float sec; /* not read */
    /* 0x20 */ s32 flag; /* 0 H, 1 D, 2 E, 3 H+D, 4 H+E, 5 D+E, 6 H+D+E, 7 None */
    /* 0x24 */ s32 appear; /* 1 waits for the switch */
};
SIZE_ASSERT(SetHintCollisionParams, 0x28);

class SetHintCollision : public Gadget {
  public:
    SetHintCollision(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~SetHintCollision();
    virtual void Behave(float dt);
    virtual void fn_800CBC1C();
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Method names are inferred. */
    void SetupCollision();
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    void UpdateTarget();
    void SetupTarget();
    void OnColliDetect(ColliDetectArgs &args);
    /* Not called anywhere (see its definition). */
    void SetDefaultRange();

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetCharCollision> m_handle;
    /* 0x40 */ boost::shared_ptr<Target> m_target;
    /* 0x48 */ SetHintCollisionParams m_params;
    /* 0x70 */ s32 m_state; /* 0 new, 1 waiting for the switch, 2 armed, 3 done */
    /* 0x74 */ s32 m_switchId;
    /* 0x78 */ SetHintCollisionFlags m_hintFlags;
    /* 0x7C */ u32 pad_0x7C; /* the allocation is 0x80 bytes */
};
SIZE_ASSERT(SetHintCollision, 0x80);

const char *SetHintCollision::s_taskName = "SetHintCollision";

SetHintCollision::SetHintCollision(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner) {
    m_name = s_taskName;
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_chara = fn_8015CB88(0, 4, "SetHintColli");
    SetupCollision();
    LoadFromOwner(owner);
    SetupTarget();
    UpdateTarget();
    if (m_params.appear == 1) {
        if (m_handle.get() != 0) {
            m_handle->m_collider->TurnOff(-1);
        }
        if (m_target) {
            m_target->TurnOff();
        }
    }
}

SetHintCollision::~SetHintCollision() {
    fn_801E7108(&m_target);
}

void SetHintCollision::Behave(float dt) {
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
        if (fn_8016904C(fn_800CB69C(), m_switchId) == 1) {
            m_state = 2;
            if (m_handle.get() != 0) {
                m_handle->m_collider->TurnOn(-1);
            }
            if (m_target) {
                m_target->TurnOn();
            }
        }
        break;
    case 2:
        if (m_params.appear == 0 && fn_8016904C(fn_800CB69C(), m_switchId) == 1) {
            m_state = 3;
        }
        break;
    case 3:
        fn_8015A794(this);
        m_flags |= 1;
        break;
    }
}

void SetHintCollision::fn_800CBC1C() {
    if (m_handle.get() != 0) {
        fn_8015BE64(m_handle.get());
    }
}

void SetHintCollision::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    int form = m_params.form;
    int flag = m_params.flag;

    LoadFromOwner(owner);
    UpdateTarget();
    if (form != m_params.form) {
        m_handle.reset();
        SetupCollision();
    }
    if (flag != m_params.flag) {
        SetupTarget();
    }
    if (m_handle.get() != 0) {
        m_handle->m_collider->SetSize(m_params.x, m_params.y, m_params.z, -2);
    }
}

void SetHintCollision::SetupCollision() {
    Vec3f size;
    fn_800091EC(&size, &Vec3f(1.0f, 1.0f, 1.0f));
    boost::shared_ptr<CharaColliBase> collider;

    switch (m_params.form) {
    case 0:
        collider = fn_800C7DEC(fn_80021B08(), m_chara, 0x40100, 10, 0, &lbl_805E07A0, 1.0f);
        break;
    case 1:
        collider = fn_800C7C34(fn_80021B08(), m_chara, &size, 0x40100, 10, 0, &lbl_805E07A0,
                               &lbl_805E07B8, 0);
        break;
    case 2:
        collider = fn_800C7A84(fn_80021B08(), m_chara, 0x40100, 10, 0, &lbl_805E07A0,
                               &lbl_805E07B8, 1.0f, 1.0f, 0);
        break;
    }
    if (m_chara.get() != 0) {
        m_chara->SetField50(0);
    }
    m_handle = fn_8015B8C4(collider);
    fn_800F5980(m_handle.get(), boost::bind(&SetHintCollision::OnColliDetect, this, _1));
    m_handle->m_collider->SetSize(m_params.x, m_params.y, m_params.z, -2);
}

/* Not called anywhere: the linker strips it from the game. Retail pools the parameters' default
 * range (10.0f) before SetupCollision's 1.0f; GC/2.7 pools the literals in the reverse order of
 * the definitions, so an out-of-line function that uses it, defined at this point of the file,
 * gives retail's order (see docs/research-notes.md and ElecSmallCube.cpp). */
void SetHintCollision::SetDefaultRange() {
    m_params.range = 10.0f;
}

void SetHintCollision::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    if (m_chara.get() != 0) {
        fn_800C9F14(owner, &m_chara.get()->m_position);
        fn_800C9ED4(owner, &m_chara.get()->m_rotation);
    }
    fn_800C9E50(owner, &m_switchId);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_state = 0;
    fn_80014E10(&m_hintFlags.m_bits);
}

void SetHintCollision::UpdateTarget() {
    if (m_target) {
        m_target->fn_800CCE38(&m_chara.get()->m_position);
    }
}

void SetHintCollision::SetupTarget() {
    m_target.reset();
    fn_80014E1C(&m_hintFlags.m_bits, 0);
    switch (m_params.flag) {
    case 0:
        m_target = fn_801E72DC(0x200000);
        break;
    case 1:
        m_target = fn_801E72DC(0x400000);
        break;
    case 2:
        break;
    case 3:
        m_target = fn_801E72DC(0x600000);
        break;
    case 4:
        m_target = fn_801E72DC(0x200000);
        break;
    case 5:
        m_target = fn_801E72DC(0x400000);
        break;
    case 6:
        m_target = fn_801E72DC(0x600000);
        break;
    case 7:
    default:
        fn_8006127C(&m_hintFlags.m_bits, 0);
        break;
    }
    if (m_target) {
        m_target->SetRadius(fn_8005AE20(m_params.range));
    }
}

void SetHintCollision::OnColliDetect(ColliDetectArgs &args) {
    if (!fn_80014E34(&m_hintFlags.m_bits, 1)) {
        bool hit;
        if (fn_80014E34(&m_hintFlags.m_bits, 0)) {
            hit = fn_8015B1C4(args);
        } else {
            hit = fn_8015AC84(args);
        }
        if (hit) {
            if (m_params.brunch >= 0) {
                fn_801FAF00(GetHintManager(), m_params.messageId, m_params.brunch, 0, 0);
            } else {
                switch (fn_8006D6A8()->field_0x20) {
                case 1:
                    fn_801FAF00(GetHintManager(), m_params.messageId, 1, 0, 0);
                    break;
                case 2:
                    fn_801FAF00(GetHintManager(), m_params.messageId, 2, 0, 0);
                    break;
                case 0:
                    fn_801FAF00(GetHintManager(), m_params.messageId, 0, 0, 0);
                    break;
                }
            }
            fn_80014E1C(&m_hintFlags.m_bits, 1);
            m_state = 3;
        }
    }
}

void SetHintCollision::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new SetHintCollision(parent, owner);
}

} // namespace HintCollision

/* The labels of the "form", "X", "Y", "Z", "Brunch", "flag" and "appear" parameters; X, Y and Z
 * are labelled by the form last shown. */
extern "C" const char *fn_8020751C(UnkLabelQuery_800CBC20 *cursor) {
    static const char *s_formLabels[3] = {"Sphere", "Box", "Cylinder"};
    static const char *s_xLabels[3] = {"Radius", "Width", "Radius"};
    static const char *s_yLabels[3] = {"Not in Used", "Height", "Not in Used"};
    static const char *s_zLabels[3] = {"Not in Used", "Depth", "Height"};
    static const char *s_flagLabels[8] = {"H", "D", "E", "H+D", "H+E", "D+E", "H+D+E", "None"};
    static const char *s_brunchLabels[2] = {"Brunch Id", "Brunch Id -> Auto"};
    static const char *s_appearLabels[2] = {"Disappear", "Appear"};
    static int s_form = 0;

    if (cursor->index == 0) {
        int form = cursor->value;
        s_form = form;
        return s_formLabels[form];
    } else if (cursor->index == 1) {
        return s_xLabels[s_form];
    } else if (cursor->index == 2) {
        return s_yLabels[s_form];
    } else if (cursor->index == 3) {
        return s_zLabels[s_form];
    } else if (cursor->index == 5) {
        if (cursor->value >= 0) {
            return s_brunchLabels[0];
        }
        return s_brunchLabels[1];
    } else if (cursor->index == 8) {
        return s_flagLabels[cursor->value];
    } else if (cursor->index == 9) {
        return s_appearLabels[cursor->value];
    }
    return 0;
}

static GadgetParamDesc lbl_80543EF4[] = {
    {1, "form", 0, 0, 2, 0.0f, 0.0f, 0.0f, fn_8020751C, 0},
    {4, "X", 0, 0, 0, 10.0f, 0.1f, 65536.0f, fn_8020751C, 0},
    {4, "Y", 0, 0, 0, 10.0f, 0.1f, 65536.0f, fn_8020751C, 0},
    {4, "Z", 0, 0, 0, 10.0f, 0.1f, 65536.0f, fn_8020751C, 0},
    {1, "Message Id", 0, 0, 0x10000, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "Brunch", -1, -1, 0x10000, 0.0f, 0.0f, 0.0f, fn_8020751C, 0},
    {4, "range", 0, 0, 0, 10.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "sec", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {1, "flag", 7, 0, 7, 0.0f, 0.0f, 0.0f, fn_8020751C, 0},
    {1, "appear", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_8020751C, 0},
    {0},
};

GadgetClassDesc lbl_805440AC = {
    "HINT COLLISION", 0, 0, HintCollision::SetHintCollision::Instantiate, 0, 0, 0x51, 1, 0, 10,
    lbl_80543EF4,
};
