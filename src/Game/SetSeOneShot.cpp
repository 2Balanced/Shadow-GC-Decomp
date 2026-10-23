/* SetSeOneShot (class name from RTTI; its s_taskName is "SetSeOneShot"): the SET_SE_ONESHOT
 * gadget, a sound that plays once when a character enters its range. "RangeType" picks the shape
 * of the range (0: a sphere of radius ColliParam0, 1: a box of half lengths ColliParam0-2, 2: a
 * capsule and 3: a cylinder of radius ColliParam0 and half length ColliParam2). Unless the owner's
 * switch (if any) is on, the first character in range plays sound SEID at the gadget: with "TYPE"
 * 0 (OnlyOnce) the gadget then removes itself, with TYPE 1 (CallAgain) it plays again once the
 * range has been empty. File name inferred from the class. Names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetUtil.h"
#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::CharaColliBase;

/* The cursor handed to a parameter's label callback (layout from fn_800CBC20; names inferred). */
class UnkLabelQuery_800CBC20 {
  public:
    /* 0x0 */ const GadgetParamDesc *params;
    /* 0x4 */ s32 index; /* the parameter being shown */
    /* 0x8 */ s32 value; /* its current value */
};

/* CharInfo's virtual slot 0x28 answers a bool here (as in GadgetColliDetectUtil.cpp, whose view
 * this is; CharInfo itself declares the slot void). */
class UnkCharInfoTest_0x28 {
  public:
    /* 0x00 */ u8 field_0x0[0x38];

    /* 0x38: vtable pointer */
    virtual ~UnkCharInfoTest_0x28();
    virtual void vf_0x0C();
    virtual void vf_0x10();
    virtual void vf_0x14();
    virtual void vf_0x18();
    virtual void vf_0x1C();
    virtual void vf_0x20();
    virtual void vf_0x24();
    virtual bool vf_0x28();
};

/* A node of a collider's contact list (layout from fn_800587A0; name inferred). */
struct UnkColliContactNode_800587A0;

/* An iterator over a contact list (layout from fn_8001EDFC; name inferred). */
struct UnkColliContactIter_8001EDFC {
    UnkColliContactIter_8001EDFC(UnkColliContactNode_800587A0 *node) : m_node(node) {}

    UnkColliContactNode_800587A0 *m_node;
};

extern "C" {
/* The collider manager, and sphere, box, capsule and cylinder colliders for charInfo (behaviour inferred;
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
boost::shared_ptr<CharaColliBase> fn_800C78D4(void *manager,
                                              const boost::weak_ptr<CharInfo> &charInfo, int flags,
                                              int arg3, u16 arg4, const Vec3f *offset,
                                              const Vec3f *rotation, float height,
                                              float radius, int arg9);
boost::shared_ptr<CharaColliBase> fn_800C7A84(void *manager,
                                              const boost::weak_ptr<CharInfo> &charInfo, int flags,
                                              int arg3, u16 arg4, const Vec3f *offset,
                                              const Vec3f *rotation, float height,
                                              float radius, int arg9);
extern const Vec3f lbl_805E07B8;

/* The character info a collider belongs to (behaviour inferred). */
boost::weak_ptr<CharInfo> fn_800C4568(void *manager,
                                      const boost::weak_ptr<CharaColliBase> &collider);
/* The contact list: its first and end nodes, the iterator's post-increment, and the collider of a
 * node (behaviour from their bodies). */
UnkColliContactNode_800587A0 *fn_8001EE04(UnkColliContacts_8001EE04 *contacts);
UnkColliContactNode_800587A0 *fn_8001EDEC(UnkColliContacts_8001EE04 *contacts);
UnkColliContactIter_8001EDFC fn_80027374(UnkColliContactIter_8001EDFC *it, int);
boost::weak_ptr<CharaColliBase> fn_800587A0(UnkColliContactNode_800587A0 *node);

/* The owner's switch number, and the removal of the owner's record (behaviour inferred). */
void fn_800C9E50(UnkPlacement_800C9CD0 *owner, s32 *id);
void fn_800C9B34(UnkPlacement_800C9CD0 *owner);
/* Plays sound id at source (behaviour inferred; the meanings of the other arguments are
 * unknown). */
void fn_801D54F4(void *manager, u16 id, GadgetCharInfo *source, int arg3, int arg4);
}

/* The parameters the owner supplies (names from the parameter table). */
struct SetSeOneShotParams {
    /* 0x00 */ s32 seId;
    /* 0x04 */ s32 type;      /* 0 OnlyOnce, 1 CallAgain */
    /* 0x08 */ s32 rangeType; /* 0 sphere, 1 box, 2 capsule, 3 cylinder */
    /* 0x0C */ float colliParam0;
    /* 0x10 */ float colliParam1;
    /* 0x14 */ float colliParam2;
};
SIZE_ASSERT(SetSeOneShotParams, 0x18);

class SetSeOneShot : public Gadget {
  public:
    SetSeOneShot(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~SetSeOneShot();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Method names are inferred. */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    void SetupCollision();

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<CharaColliBase> m_collider;
    /* 0x40 */ SetSeOneShotParams m_params;
    /* 0x58 */ s32 m_switchId; /* 0: none */
    /* 0x5C */ u8 m_played;    /* set while a character stays in range (CallAgain) */
};
SIZE_ASSERT(SetSeOneShot, 0x60);

const char *SetSeOneShot::s_taskName = "SetSeOneShot";

SetSeOneShot::SetSeOneShot(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x97, 4);
    LoadFromOwner(owner);
    SetupCollision();
}

SetSeOneShot::~SetSeOneShot() {}

void SetSeOneShot::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new SetSeOneShot(fn_801EB304(fn_8001C4F8(), 11), owner);
}

void SetSeOneShot::Behave(float dt) {
    bool armed = true;
    if (m_switchId != 0 && fn_8016904C(fn_800CB69C(), m_switchId) == 1) {
        armed = false;
    }
    if (armed) {
        UnkColliContacts_8001EE04 *contacts = reinterpret_cast<UnkColliContacts_8001EE04 *>(
            reinterpret_cast<u8 *>(m_collider.get()) + 0xC);
        if (contacts != 0) {
            for (UnkColliContactIter_8001EDFC it(fn_8001EE04(contacts));
                 it.m_node != fn_8001EDEC(contacts); fn_80027374(&it, 0)) {
                boost::shared_ptr<CharInfo> target =
                    fn_800C4568(fn_80021B08(), fn_800587A0(it.m_node)).lock();
                if (target.get() != 0 &&
                    reinterpret_cast<UnkCharInfoTest_0x28 *>(target.get())->vf_0x28()) {
                    if (m_params.type == 1) {
                        if (!m_played) {
                            fn_801D54F4(fn_80053164(), m_params.seId, m_chara.get(), 0, 1);
                        }
                        m_played = 1;
                        return;
                    } else if (m_params.type == 0) {
                        fn_801D54F4(fn_80053164(), m_params.seId, m_chara.get(), 0, 1);
                        fn_800C9B34(m_owner);
                        m_flags |= 1;
                        return;
                    }
                }
            }
        }
    }
    m_played = 0;
}

void SetSeOneShot::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    SetSeOneShotParams old = m_params;

    LoadFromOwner(owner);
    if (m_params.rangeType != old.rangeType) {
        SetupCollision();
    } else {
        m_collider->SetSize(m_params.colliParam0, m_params.colliParam1, m_params.colliParam2, -2);
    }
    m_collider->Reset();
}

void SetSeOneShot::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9E50(owner, &m_switchId);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_played = 0;
}

void SetSeOneShot::SetupCollision() {
    m_collider.reset();
    switch (m_params.rangeType) {
    case 0:
        m_collider = fn_800C7DEC(fn_80021B08(), m_chara, 0x50100, 0x10, 0, &lbl_805E07A0,
                                 m_params.colliParam0);
        break;
    case 1: {
        Vec3f size(m_params.colliParam0, m_params.colliParam1, m_params.colliParam2);
        m_collider = fn_800C7C34(fn_80021B08(), m_chara, &size, 0x50100, 0x10, 0, &lbl_805E07A0,
                                 &lbl_805E07B8, 0);
        break;
    }
    case 2:
        m_collider = fn_800C78D4(fn_80021B08(), m_chara, 0x50100, 0x10, 0, &lbl_805E07A0,
                                 &lbl_805E07B8, m_params.colliParam2, m_params.colliParam0, 0);
        break;
    case 3:
        m_collider = fn_800C7A84(fn_80021B08(), m_chara, 0x50100, 0x10, 0, &lbl_805E07A0,
                                 &lbl_805E07B8, m_params.colliParam2, m_params.colliParam0, 0);
        break;
    default:
        m_collider.reset();
        break;
    }
}

/* The labels of the "SEID", "TYPE", "RangeType" and "ColliParam0-2" parameters; the ColliParams
 * are labelled by the range type last shown. */
extern "C" const char *fn_8033CFF8(UnkLabelQuery_800CBC20 *cursor) {
    static const char *s_typeLabels[2] = {"Type:OnlyOnce", "Type:CallAgain"};
    static const char *s_rangeLabels[4] = {"Range:Sphare", "Range:Box", "Range:Capsule",
                                           "Range:Cylinder"};
    static const char *s_param0Labels[4] = {"Radius", "HalfLengthX", "Radius", "Radius"};
    static const char *s_param1Labels[4] = {"Not Used", "HalfLengthY", "Not Used", "Not Used"};
    static const char *s_param2Labels[4] = {"Not Used", "HalfLengthZ", "HalfLength", "HalfLength"};
    static int s_type = 0;
    static int s_range = 0;

    if (cursor->index == 0) {
        return "SEID";
    } else if (cursor->index == 1) {
        int type = cursor->value;
        s_type = type;
        return s_typeLabels[type];
    } else if (cursor->index == 2) {
        int range = cursor->value;
        s_range = range;
        return s_rangeLabels[range];
    } else if (cursor->index == 3) {
        return s_param0Labels[s_range];
    } else if (cursor->index == 4) {
        return s_param1Labels[s_range];
    } else if (cursor->index == 5) {
        return s_param2Labels[s_range];
    }
    return 0;
}

static GadgetParamDesc lbl_8055D970[] = {
    {3, "SEID", 0, 0, 0xFFFFF, 0.0f, 0.0f, 0.0f, fn_8033CFF8, 0},
    {1, "TYPE", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_8033CFF8, 0},
    {1, "RangeType", 0, 0, 3, 0.0f, 0.0f, 0.0f, fn_8033CFF8, 0},
    {4, "ColliParam0", 0, 0, 0, 10.0f, 0.0f, 65536.0f, fn_8033CFF8, 0},
    {4, "ColliParam1", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_8033CFF8, 0},
    {4, "ColliParam2", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_8033CFF8, 0},
    {0},
};

GadgetClassDesc lbl_8055DA88 = {
    "SET_SE_ONESHOT", 0, 0, SetSeOneShot::Instantiate, 0, 0, 0x2598, 1, 0, 10, lbl_8055D970,
};
