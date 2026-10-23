/* SetSeLoop (class name from RTTI; its s_taskName is "SetSeLoop"): the SET_SE_LOOP gadget, a
 * looping sound source placed by the stage. "RangeType" picks the shape of its range (0: a sphere
 * of radius ColliParam0, 1: a box of half lengths ColliParam0-2, 3: a cylinder of radius
 * ColliParam0 and half length ColliParam2; 2, the editor's value for a cylinder, is turned into 3
 * when loaded). Every frame each character in range keeps sound SEID playing: with "TYPE" 0
 * (FlatDamping) the volume falls off from the centre (or the axis) once the distance passes
 * DampingRate of the range; with TYPE 1 (PanCtrl) the sound is panned at the gadget instead.
 * File name inferred from the class. Names marked "(name inferred)" are not original. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first).
 *
 * NonMatching: Behave (0x8033CBF8) differs only in register allocation. Retail keeps this in r31,
 * the contact list in r30 and the volume in r29; GC/2.7 gives this r29 and the volume r31 for every
 * source shape tried (an inline Play helper, a named s8, the volume as a call argument, the
 * contact list in the if, a separate per-target inline function). SetSeOneShot's Behave, the same
 * loop without the volume, matches. */
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/DebugDisp.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetUtil.h"
#include "Game/GeometryLine.h"
#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::CharaColliBase;
using Sonicteam::System::Geometry::Line;

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

/* The character info a collider belongs to (behaviour inferred). */
boost::weak_ptr<CharInfo> fn_800C4568(void *manager,
                                      const boost::weak_ptr<CharaColliBase> &collider);
/* The contact list: its first and end nodes, the iterator's post-increment, and the collider of a
 * node (behaviour from their bodies). */
UnkColliContactNode_800587A0 *fn_8001EE04(UnkColliContacts_8001EE04 *contacts);
UnkColliContactNode_800587A0 *fn_8001EDEC(UnkColliContacts_8001EE04 *contacts);
UnkColliContactIter_8001EDFC fn_80027374(UnkColliContactIter_8001EDFC *it, int);
boost::weak_ptr<CharaColliBase> fn_800587A0(UnkColliContactNode_800587A0 *node);

/* True when value is (nearly) zero, and when a and b are (nearly) equal (behaviour inferred). */
bool fn_8000A950(float value);
bool fn_801C455C(float a, float b);
/* value clamped to [low, high] (behaviour inferred). */
const float &fn_80070460(const float &value, const float &low, const float &high);
const s32 *fn_8016F21C(const s32 *value, const s32 *low, const s32 *high);
/* The point of line closest to point; *t receives its position along the line, 0 at the start and
 * 1 at the end (behaviour inferred). */
Vec3f fn_8040FC78(float *t, const Line *line, const Vec3f *point);

/* Plays sound id for source (behaviour inferred; position is the sound's position for panning, or
 * null; the meanings of the other arguments are unknown, volume is an offset). */
void fn_801D53C4(void *manager, u16 id, const Vec3f *position, void *source, int arg4, s8 volume,
                 int arg6);

/* Debug view: a box and a cylinder (defined in Game/DebugDisp.cpp). */
void fn_802E0D94(const Vec3f *center, const Vec3f *size, const Vec3f *rotation,
                 const UnkColor_8040D2D8 *color, int axis);
void fn_802E0B7C(const Vec3f *center, const Vec3f *rotation, const UnkColor_8040D2D8 *color,
                 int axis, float halfHeight, float radius);
void fn_802E0E84(const Vec3f *at, const UnkColor_8040D2D8 *color, float size);
}

/* MSL's std::fabs for floats, which the game calls out of line (as in DevilDoomCtrlUnit.cpp). */
namespace std {
float fabs(float x);
} // namespace std

/* A debug colour (meaning inferred). */
extern UnkColor_8040D2D8 lbl_805F1378;

/* The parameters the owner supplies (names from the parameter table). */
struct SetSeLoopParams {
    /* 0x00 */ s32 seId;
    /* 0x04 */ s32 type;      /* 0 FlatDamping, 1 PanCtrl */
    /* 0x08 */ s32 rangeType; /* 0 sphere, 1 box, 3 cylinder */
    /* 0x0C */ float colliParam0;
    /* 0x10 */ float colliParam1;
    /* 0x14 */ float colliParam2;
    /* 0x18 */ float dampingRate;
};
SIZE_ASSERT(SetSeLoopParams, 0x1C);

class SetSeLoop : public Gadget {
  public:
    SetSeLoop(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~SetSeLoop();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Method names are inferred. */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    void SetupCollision();
    void UpdateCollision();

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<CharaColliBase> m_collider;
    /* 0x40 */ SetSeLoopParams m_params;
    /* 0x5C */ Line m_axis; /* the cylinder's axis, in world space */
    /* 0x74 */ u32 field_0x74;
};
SIZE_ASSERT(SetSeLoop, 0x78);

const char *SetSeLoop::s_taskName = "SetSeLoop";

SetSeLoop::SetSeLoop(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x96, 4);
    LoadFromOwner(owner);
    SetupCollision();
}

SetSeLoop::~SetSeLoop() {}

void SetSeLoop::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new SetSeLoop(fn_801EB304(fn_8001C4F8(), 11), owner);
}

/* The clamp limits Behave passes by address (names inferred). Retail keeps the zeros in .sdata. */
#pragma explicit_zero_data on
static float s_ratioLow = 0.0f;
static float s_ratioHigh = 1.0f;
static float s_tLow = 0.0f;
static float s_tHigh = 1.0f;
static s32 s_volumeLow = -128;
static s32 s_volumeHigh = 0;
#pragma explicit_zero_data reset

void SetSeLoop::Behave(float dt) {
    if (UnkColliContacts_8001EE04 *contacts = reinterpret_cast<UnkColliContacts_8001EE04 *>(
            reinterpret_cast<u8 *>(m_collider.get()) + 0xC)) {
        for (UnkColliContactIter_8001EDFC it(fn_8001EE04(contacts));
             it.m_node != fn_8001EDEC(contacts); fn_80027374(&it, 0)) {
            boost::shared_ptr<CharInfo> target =
                fn_800C4568(fn_80021B08(), fn_800587A0(it.m_node)).lock();
            if (target.get() != 0 && reinterpret_cast<UnkCharInfoTest_0x28 *>(target.get())->vf_0x28()) {
                if (m_params.type == 0) {
                    float ratio;
                    if (m_params.rangeType == 0) {
                        Vec3f offset;
                        fn_8040CD74(&offset, &target->m_position, &m_chara->m_position);
                        float distance = fn_8040CF50(&offset);
                        if (fn_8000A950(m_params.colliParam0)) {
                            ratio = 1.0f;
                        } else {
                            ratio = distance / m_params.colliParam0;
                        }
                        ratio = fn_80070460(ratio, s_ratioLow, s_ratioHigh);
                    } else {
                        float t;
                        fn_8040FC78(&t, &m_axis, &target->m_position);
                        t = fn_80070460(t, s_tLow, s_tHigh);
                        ratio = 2.0f * std::fabs(t - 0.5f);
                    }
                    if (ratio <= m_params.dampingRate || fn_801C455C(m_params.dampingRate, 1.0f)) {
                        fn_801D53C4(fn_80053164(), m_params.seId, 0, this, 2, 0, 2);
                    } else {
                        s32 volume = -128.0f * ((ratio - m_params.dampingRate) /
                                                (1.0f - m_params.dampingRate));
                        fn_801D53C4(fn_80053164(), m_params.seId, 0, this, 2,
                                    volume = *fn_8016F21C(&volume, &s_volumeLow, &s_volumeHigh), 2);
                    }
                } else {
                    fn_801D53C4(fn_80053164(), m_params.seId, &m_chara->m_position, this, 3, 0, 2);
                }
            }
        }
    }
}

void SetSeLoop::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    SetSeLoopParams old = m_params;

    LoadFromOwner(owner);
    if (m_params.rangeType != old.rangeType) {
        SetupCollision();
    } else {
        m_collider->SetSize(m_params.colliParam0, m_params.colliParam1, m_params.colliParam2, -2);
    }
    UpdateCollision();
}

void SetSeLoop::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    if (m_params.rangeType == 2) {
        m_params.rangeType = 3;
    }
    if (m_params.type == 0 && m_params.rangeType != 0) {
        Mtx34f matrix;
        float length = m_params.colliParam2;
        Vec3f start(0.0f, 0.0f, -length);
        Vec3f end(0.0f, 0.0f, length);
        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B884(&matrix, m_chara->m_rotation.x);
        fn_8040B80C(&matrix, m_chara->m_rotation.z);
        AssignVector(m_axis.m_start, fn_8040B67C(&matrix, &start));
        AssignVector(m_axis.m_end, fn_8040B67C(&matrix, &end));
    } else {
        fn_8040CE60(&m_axis.m_start);
        fn_8040CE60(&m_axis.m_end);
    }
}

void SetSeLoop::SetupCollision() {
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
    case 3:
        m_collider = fn_800C7A84(fn_80021B08(), m_chara, 0x50100, 0x10, 0, &lbl_805E07A0,
                                 &lbl_805E07B8, m_params.colliParam2, m_params.colliParam0, 0);
        break;
    default:
        m_collider.reset();
        break;
    }
}

void SetSeLoop::UpdateCollision() {
    m_collider->Reset();
    if (m_params.type == 0) {
        UnkColor_8040D2D8 color = lbl_805F1378;
        color.r = 0x80;
        switch (m_params.rangeType) {
        case 0:
            fn_802E0E84(&m_chara->m_position, &color, m_params.colliParam0 * m_params.dampingRate);
            break;
        case 1: {
            Vec3f size(m_params.colliParam0, m_params.colliParam1,
                       m_params.colliParam2 * m_params.dampingRate);
            fn_802E0D94(&m_chara->m_position, &size, &m_chara->m_rotation, &color, 2);
            break;
        }
        case 3:
            fn_802E0B7C(&m_chara->m_position, &m_chara->m_rotation, &color, 2,
                         m_params.colliParam2 * m_params.dampingRate, m_params.colliParam0);
            break;
        }
    }
}

/* The labels of the "SEID", "TYPE", "RangeType", "ColliParam0-2" and "DampingRate" parameters;
 * the ColliParams are labelled by the range type last shown, DampingRate by the type. */
extern "C" const char *fn_8033C5DC(UnkLabelQuery_800CBC20 *cursor) {
    static const char *s_typeLabels[2] = {"Type:FlatDamping", "Type:PanCtrl"};
    static const char *s_rangeLabels[3] = {"Range:Sphare", "Range:Box", "Range:Cylinder"};
    static const char *s_param0Labels[3] = {"Radius", "HalfLengthX", "Radius"};
    static const char *s_param1Labels[3] = {"Not Used", "HalfLengthY", "Not Used"};
    static const char *s_param2Labels[3] = {"Not Used", "HalfLengthZ", "HalfLength"};
    static const char *s_dampingLabels[2] = {"FlatDampingRate(0-1)", "Not Used"};
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
    } else if (cursor->index == 6) {
        return s_dampingLabels[s_type];
    }
    return 0;
}

static GadgetParamDesc lbl_8055D718[] = {
    {3, "SEID", 0, 0, 0xFFFFF, 0.0f, 0.0f, 0.0f, fn_8033C5DC, 0},
    {1, "TYPE", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_8033C5DC, 0},
    {1, "RangeType", 0, 0, 2, 0.0f, 0.0f, 0.0f, fn_8033C5DC, 0},
    {4, "ColliParam0", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_8033C5DC, 0},
    {4, "ColliParam1", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_8033C5DC, 0},
    {4, "ColliParam2", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_8033C5DC, 0},
    {4, "DampingRate", 0, 0, 0, 0.5f, 0.0f, 1.0f, fn_8033C5DC, 0},
    {0},
};

GadgetClassDesc lbl_8055D858 = {
    "SET_SE_LOOP", 0, 0, SetSeLoop::Instantiate, 0, 0, 0x2597, 1, 0, 10, lbl_8055D718,
};
