/* HintRing::SetHintRing (class name from RTTI): the HINTRING gadget, a hint ring (HINTRING.BIN, drawn
 * as the clump HINTRING01.DFF facing the camera) that shows a hint message when a player touches it.
 * The "Message Id" parameter names the message and "Brunch" its branch (-1: the branch for the
 * player's character); "navi" picks the kinds of target the ring offers ("navi H", "navi D",
 * "navi E" and their combinations, or none) and "range" the target's range. A ring is active only
 * while the switch its owner names is off. File name inferred from the namespace. Names marked
 * "(name inferred)" are not original. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). The unit's code reaches its .data and .rodata through pooled
 * base addresses, so the linker keeps those sections whole. */
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <cmath>
#include <string>

#include "Game/TNumEdit.h"
/* The unreferenced constants that follow the TNumEdit names in .rodata (as in StretchGrass.cpp):
 * the unit axes, of which the frame rotation below uses the z axis, then seven zero words. */
extern "C" {
typedef struct RwV3d {
    float x;
    float y;
    float z;
} RwV3d;
}
static const RwV3d s_xAxis = {1.0f, 0.0f, 0.0f};
static const RwV3d s_yAxis = {0.0f, 1.0f, 0.0f};
static const RwV3d s_zAxis = {0.0f, 0.0f, 1.0f};
static const float s_zeros[7] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/Render.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* The RenderWare types and functions used here, declared locally because the RenderWare headers
 * need the C library headers (layouts as in rw/rwcore.h and rw/rpworld.h, reduced to the members
 * used; as in Door.cpp). */
extern "C" {
typedef struct RwSphere {
    RwV3d center;
    float radius;
} RwSphere;

typedef struct RwMatrix RwMatrix;
typedef struct RwCamera RwCamera;

typedef struct RwObject {
    u8 type;
    u8 subKind;
    u8 flags;
    u8 privateFlags;
    void *parent;
} RwObject;

typedef struct RwFrame {
    /* 0x00 */ RwObject object;
    /* 0x08 */ u8 inDirtyListLink[0x8];
    /* 0x10 */ u8 modelling[0x40]; /* the frame's matrix */
} RwFrame;

typedef struct RpClump {
    /* 0x00 */ RwObject object; /* the parent is the clump's frame */
} RpClump;

typedef struct RwGlobals {
    /* 0x000 */ RwCamera *curCamera;
} RwGlobals;

/* the engine instance */
extern RwGlobals *lbl_805F15B4;

/* whether a sphere is in a camera's view frustum (inferred) */
int fn_804856C8(const RwCamera *camera, const RwSphere *sphere);
/* translates a frame */
RwFrame *fn_804875B0(RwFrame *frame, const RwV3d *translation, int combine);
/* Renders a clump. */
RpClump *fn_80456BC4(RpClump *clump);
/* RwMatrixRotateOneMinusCosineSine and RwFrameUpdateObjects. */
RwMatrix *fn_8047ECD4(RwMatrix *matrix, const RwV3d *axisUnit, float omc, float sine,
                      int combine);
RwFrame *fn_8048722C(RwFrame *frame);
/* Turn a frame about its x and y axes (behaviour inferred; the last argument is the combine
 * mode). */
void fn_800E9C64(RwFrame *frame, float angle, int combine);
void fn_800E9CEC(RwFrame *frame, float angle, int combine);
}

/* Turns a frame about its z axis by angle (radians; behaviour inferred). Other units call this
 * copy. */
extern "C" inline void fn_80109AF8(RwFrame *frame, float angle, int combine) {
    fn_8047ECD4((RwMatrix *)frame->modelling, &s_zAxis, 1.0f - std::cos(angle), std::sin(angle),
                combine);
    fn_8048722C(frame);
}

/* Target (class name from RTTI): a point characters aim at (meaning inferred). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);
    /* Sets the target's range (name inferred). */
    void SetRange(float range) {
        m_reach = range;
    }

    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ float m_reach;
};

/* The player manager fn_8006D6A8 returns (only the character kind is mapped; name inferred). */
struct UnkPlayerManager_8006D6A8 {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ s32 m_character; /* 0, 1 or 2: the branch of a hint for that character (inferred) */
};

/* The camera fn_8000D0F0 returns (class name unknown). */
class UnkCamera_8000D0F0;
class UnkCameraManager_80009548;

/* The manager of the clumps whose textures animate (class name unknown; by its neighbour
 * TextureUVAnimManagerTask, inferred). It is constructed and destroyed out of line. */
class UnkUVAnimManager_801EB994 {
  public:
    UnkUVAnimManager_801EB994();
    ~UnkUVAnimManager_801EB994();

    static UnkUVAnimManager_801EB994 *s_cached;

    /* 0x0 */ u8 field_0x0[0x10];
};

/* What the hint manager keeps for each hint (layout unknown). */
struct UnkHintEntry_801FB180 {
    /* 0x0 */ u8 field_0x0[0xC];
};

class UnkHintTarget_801FB180;

/* A list of hints (names inferred): a vector-like container (capacity, size, storage) in three
 * class levels, as retail's destructors show. The base destructor is a template member defined out
 * of the class, so it is instantiated, with its helpers, at the end of the unit (0x8010B200); the
 * middle level's inline destructor is expanded, with its null test, into the list's (0x80109E2C). */
template <class T> class UnkHintListBase : private std::allocator<T> {
  public:
    ~UnkHintListBase();

    std::allocator<T> &Alloc() {
        return *this;
    }
    T *&Data() {
        return m_data;
    }
    u32 &Capacity() {
        return m_capacity;
    }

    void Clear() {
        T *first = m_data;
        T *last = m_data + m_size;
        while (last > first) {
            Alloc().destroy(--last);
        }
        m_size = 0;
    }

    /* 0x0 */ u32 m_capacity;
    /* 0x4 */ u32 m_size;
    /* 0x8 */ T *m_data;
};

template <class T> UnkHintListBase<T>::~UnkHintListBase() {
    Clear();
    if (m_data != 0) {
        Alloc().deallocate(Data(), Capacity());
    }
}

template <class T> class UnkHintListMid : public UnkHintListBase<T> {
  public:
    ~UnkHintListMid() {}
};

template <class T> class UnkHintList : public UnkHintListMid<T> {};

/* The hint manager (class name unknown): it shows hint messages. It is constructed out of line;
 * this unit holds its destructor. */
class UnkHintManager_801FB180 {
  public:
    UnkHintManager_801FB180();

    static UnkHintManager_801FB180 *s_cached;

    /* 0x00 */ UnkHintList<UnkHintEntry_801FB180> m_hints;
    /* 0x0C */ boost::weak_ptr<UnkHintTarget_801FB180> m_target;
    /* 0x14 */ u8 field_0x14[0x38 - 0x14];
};
SIZE_ASSERT(UnkHintManager_801FB180, 0x38);

extern "C" {
/* The flag word helpers (behaviour from their bodies). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
/* Builds the character info of a gadget (behaviour inferred). */
boost::shared_ptr<GadgetCharInfo> fn_8015CD90(int arg0, int arg1);
/* Loads the model fileName (behaviour inferred; arg0 unknown). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* GadgetModelInfo methods not declared in its header (behaviour unknown). */
void fn_80162790(GadgetModelInfo *model);
void fn_8016271C(GadgetModelInfo *model);
/* Creates a target with the given flags, and removes a gadget's target (behaviour inferred). */
boost::shared_ptr<Target> fn_801E72DC(int flags);
void fn_801E7108(boost::shared_ptr<Target> *target);
/* Scales a range (behaviour inferred). */
float fn_8005AE20(float range);
/* The switch id the owner was placed with (behaviour inferred). */
void fn_800C9E50(UnkPlacement_800C9CD0 *owner, s32 *id);
/* Creates a Render (behaviour inferred). */
boost::shared_ptr<Render> fn_8004E24C(void *arg0);
/* The file loader, and a file of it: the clump HINTRING01.DFF (behaviour inferred). */
void *fn_800122D4(void);
RpClump *fn_80012700(void *loader, int arg1, const char *fileName);
/* Whether a character touches the collision, for players only or for all (behaviour
 * inferred). */
bool fn_8015B1C4(ColliDetectArgs &args);
bool fn_8015AC84(ColliDetectArgs &args);
/* The player manager (behaviour inferred). */
UnkPlayerManager_8006D6A8 *fn_8006D6A8(void);
/* Shows message id of branch for the given character, and returns the hint shown or -1; and
 * whether hint id is over (behaviour inferred; the other arguments are unknown). */
s32 fn_801FAF00(UnkHintManager_801FB180 *manager, s32 id, s32 branch, int arg3, int arg4);
bool fn_801FAAC8(UnkHintManager_801FB180 *manager, s32 hint);
/* Adds and removes a named clump (behaviour inferred). */
void fn_801EB660(UnkUVAnimManager_801EB994 *manager, const std::string &name, RpClump *clump);
void fn_801EB550(UnkUVAnimManager_801EB994 *manager, const std::string &name);
/* The camera manager, a camera of it, and a camera's angles (behaviour inferred). */
UnkCameraManager_80009548 *fn_80009548(void);
boost::shared_ptr<UnkCamera_8000D0F0> fn_8000D0F0(UnkCameraManager_80009548 *manager, int index);
Vec3f fn_8000A610(const u8 *camera);
}

/* The angles of camera index (name inferred). */
inline Vec3f GetCameraAngles(UnkCameraManager_80009548 *manager, int index) {
    return fn_8000A610(reinterpret_cast<const u8 *>(fn_8000D0F0(manager, index).get()) + 0x8);
}

/* The clump manager and the hint manager (names inferred; defined after the constructor). */
inline UnkUVAnimManager_801EB994 *GetUVAnimManager();
inline UnkHintManager_801FB180 *GetHintManager();

/* The cursor handed to a parameter's label callback (layout from fn_800CBC20; names inferred). */
class UnkLabelQuery_800CBC20 {
  public:
    /* 0x0 */ const GadgetParamDesc *params;
    /* 0x4 */ s32 index; /* the parameter being shown */
    /* 0x8 */ s32 value; /* its current value */
};

namespace HintRing {

/* A 32-bit flag word (name inferred): fn_80014E04 clears it, fn_80014E10 clears all bits,
 * fn_80014E1C sets bit, fn_8006127C clears bit and fn_80014E34 tests bit. */
struct HintRingFlags {
    HintRingFlags() {
        fn_80014E04(&m_bits);
    }
    void Clear() {
        fn_80014E10(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    void Reset(int bit) {
        fn_8006127C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits;
};

/* The parameters the owner supplies (names from the parameter table). */
struct HintRingParams {
    HintRingParams() : messageId(0), brunch(-1), range(10.0f), sec(0.0f), navi(7) {}

    /* 0x00 */ s32 messageId;
    /* 0x04 */ s32 brunch; /* the message branch, or -1 for the player's character */
    /* 0x08 */ float range; /* the target's range */
    /* 0x0C */ float sec; /* not used by this unit */
    /* 0x10 */ s32 navi; /* the target kinds: H, D, E, H+D, H+E, D+E, H+D+E or none */
};
SIZE_ASSERT(HintRingParams, 0x14);

class SetHintRing : public Gadget {
  public:
    SetHintRing(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~SetHintRing();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the ring under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Method names are inferred. */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    void UpdatePosition();
    void SetupTarget();
    void OnColliDetect(ColliDetectArgs &args);
    void Draw(RednerDrawArgs &args);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::shared_ptr<Target> m_target;
    /* 0x48 */ boost::shared_ptr<Render> m_render;
    /* 0x50 */ HintRingParams m_params;
    /* 0x64 */ s32 m_state; /* 0 new, 1 waiting, 2 showing a hint */
    /* 0x68 */ s32 m_hint; /* the hint shown, or -1 */
    /* 0x6C */ HintRingFlags m_flags; /* 0 active, 1 has a target, 2 hidden, 3 touched, 4 touched
                                         before, 5 showing a hint */
    /* 0x70 */ RpClump *m_clump;
    /* 0x74 */ s32 m_switchId;
};
SIZE_ASSERT(SetHintRing, 0x78);

const char *SetHintRing::s_taskName = "SetHintRing";

SetHintRing::SetHintRing(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_chara = fn_8015CD90(4, 4);
    m_model = fn_8015F800(0, "HINTRING.BIN");
    m_model->fn_80162A68(0, 0);
    m_model->fn_80161F08("col", m_chara);
    fn_800D0B64(m_model->field_0x4, boost::bind(&SetHintRing::OnColliDetect, this, _1));
    m_render = fn_8004E24C(fn_80046178()->field_0x8);
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&SetHintRing::Draw, this, _1));
    m_clump = fn_80012700(fn_800122D4(), 0, "HINTRING01.DFF");
    fn_801EB660(GetUVAnimManager(), "HintRing", m_clump);
    LoadFromOwner(owner);
    SetupTarget();
    UpdatePosition();
}

} // namespace HintRing

/* The hint manager and the clump manager (names inferred). Each is a function-local static
 * object, whose address the accessors below cache. */
inline UnkUVAnimManager_801EB994 &UVAnimManagerObject() {
    static UnkUVAnimManager_801EB994 s_object;
    return s_object;
}

inline UnkUVAnimManager_801EB994 *GetUVAnimManager() {
    if (UnkUVAnimManager_801EB994::s_cached == 0) {
        UnkUVAnimManager_801EB994::s_cached = &UVAnimManagerObject();
    }
    return UnkUVAnimManager_801EB994::s_cached;
}

inline UnkHintManager_801FB180 &HintManagerObject() {
    static UnkHintManager_801FB180 s_object;
    return s_object;
}

inline UnkHintManager_801FB180 *GetHintManager() {
    if (UnkHintManager_801FB180::s_cached == 0) {
        UnkHintManager_801FB180::s_cached = &HintManagerObject();
    }
    return UnkHintManager_801FB180::s_cached;
}

namespace HintRing {

SetHintRing::~SetHintRing() {
    fn_801EB550(GetUVAnimManager(), "HintRing");
    if (m_target.get() != 0) {
        fn_801E7108(&m_target);
    }
}

void SetHintRing::Behave(float dt) {
    if (m_flags.Test(3)) {
        m_flags.Set(4);
    } else {
        m_flags.Reset(4);
    }
    m_flags.Reset(3);
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }

    switch (m_state) {
    case 0:
        m_state = 1;
        break;
    case 1:
        if (m_switchId > 0 && fn_8016904C(fn_800CB69C(), m_switchId) == 1) {
            m_flags.Reset(0);
        } else {
            m_flags.Set(0);
        }
        if (!m_flags.Test(0) && !m_flags.Test(2)) {
            if (m_model.get() != 0) {
                fn_80162790(m_model.get());
            }
            m_flags.Set(2);
        }
        if (m_flags.Test(0) && m_flags.Test(2)) {
            if (m_model.get() != 0) {
                fn_8016271C(m_model.get());
            }
            m_flags.Reset(2);
        }
        break;
    case 2:
        if (fn_801FAAC8(GetHintManager(), m_hint)) {
            m_flags.Set(0);
            m_flags.Reset(5);
            m_state = 1;
        }
        break;
    case 3: /* (a state retail's switch lists without code) */
        break;
    }
}

void SetHintRing::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    s32 navi = m_params.navi;

    LoadFromOwner(owner);
    UpdatePosition();
    if (navi != m_params.navi) {
        SetupTarget();
    }
    if (m_model.get() != 0) {
        m_model->fn_8016108C();
    }
}

void SetHintRing::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    fn_800C9E50(owner, &m_switchId);
    m_state = 0;
    m_hint = 0;
    m_flags.Clear();
    m_flags.Set(0);
}

void SetHintRing::UpdatePosition() {
    if (m_model.get() != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B884(&matrix, m_chara->m_rotation.x);
        fn_8040B80C(&matrix, m_chara->m_rotation.z);
        m_model->fn_801633DC(&matrix);
    }
    if (m_target.get() != 0) {
        m_target->fn_800CCE38(&m_chara->m_position);
    }
}

void SetHintRing::SetupTarget() {
    m_target.reset();
    m_flags.Set(1);
    switch (m_params.navi) {
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
        m_flags.Reset(1);
        break;
    }
    if (m_target.get() != 0) {
        m_target->SetRange(fn_8005AE20(m_params.range));
    }
}

void SetHintRing::OnColliDetect(ColliDetectArgs &args) {
    if (m_flags.Test(0)) {
        bool touched;
        if (m_flags.Test(1)) {
            touched = fn_8015B1C4(args);
        } else {
            touched = fn_8015AC84(args);
        }
        if (touched) {
            m_flags.Set(3);
            if (!m_flags.Test(5) && !m_flags.Test(4)) {
                m_hint = 0;
                if (m_params.brunch >= 0) {
                    m_hint = fn_801FAF00(GetHintManager(), m_params.messageId, m_params.brunch, 1, 0);
                } else {
                    switch (fn_8006D6A8()->m_character) {
                    case 1:
                        m_hint = fn_801FAF00(GetHintManager(), m_params.messageId, 1, 1, 0);
                        break;
                    case 2:
                        m_hint = fn_801FAF00(GetHintManager(), m_params.messageId, 2, 1, 0);
                        break;
                    case 0:
                        m_hint = fn_801FAF00(GetHintManager(), m_params.messageId, 0, 1, 0);
                        break;
                    }
                }
                if (m_hint != -1) {
                    fn_801D560C(fn_80053164(), 0xE01B, 0, 0);
                    m_flags.Set(5);
                    m_state = 2;
                }
            }
        }
    }
}

void SetHintRing::Draw(RednerDrawArgs &args) {
    if (!m_flags.Test(2)) {
        RwSphere sphere;

        sphere.radius = 10.0f;
        sphere.center.x = m_chara->m_position.x;
        sphere.center.y = m_chara->m_position.y;
        sphere.center.z = m_chara->m_position.z;
        if (fn_804856C8(lbl_805F15B4->curCamera, &sphere) != 0) {
            RwFrame *frame = (RwFrame *)m_clump->object.parent;
            Vec3f angles = GetCameraAngles(fn_80009548(), 0);

            fn_80109AF8(frame, angles.z, 0);
            fn_800E9C64(frame, angles.x, 2);
            fn_800E9CEC(frame, angles.y, 2);
            fn_804875B0(frame, &sphere.center, 2);
            fn_80456BC4(m_clump);
        }
    }
}

extern "C" void fn_801099C8(void) {
    fn_8006B474("HINTRING.BIN", 1);
}

extern "C" void fn_801099C4(void) {}

void SetHintRing::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new SetHintRing(fn_801EB304(fn_8001C4F8(), 11), owner);
}

/* The labels of the "Brunch" and "navi" parameters. */
extern "C" const char *fn_8010991C(UnkLabelQuery_800CBC20 *cursor) {
    static const char *s_brunchLabels[2] = {"Brunch Id", "Brunch Id -> Auto"};
    static const char *s_naviLabels[8] = {"navi H",   "navi D",   "navi E",     "navi H+D",
                                          "navi H+E", "navi D+E", "navi H+D+E", "navi None"};

    if (cursor->index == 1) {
        if (cursor->value >= 0) {
            return s_brunchLabels[0];
        }
        return s_brunchLabels[1];
    } else if (cursor->index == 4) {
        return s_naviLabels[cursor->value];
    }
    return 0;
}

static GadgetParamDesc lbl_805263A0[] = {
    {1, "Message Id", 0, 0, 0x10000, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "Brunch", -1, -1, 0x10000, 0.0f, 0.0f, 0.0f, fn_8010991C, 0},
    {4, "range", 0, 0, 0, 10.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "sec", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {1, "navi", 7, 0, 7, 0.0f, 0.0f, 0.0f, fn_8010991C, 0},
    {0},
};

} // namespace HintRing

UnkUVAnimManager_801EB994 *UnkUVAnimManager_801EB994::s_cached;
UnkHintManager_801FB180 *UnkHintManager_801FB180::s_cached;

GadgetClassDesc lbl_80526490 = {
    "HINTRING", HintRing::fn_801099C8, HintRing::fn_801099C4, HintRing::SetHintRing::Instantiate, 0, 0,
    0x11, 1, 0, 10, HintRing::lbl_805263A0,
};
