/* ElecCoreWall (class name from RTTI): the ELEC_COREWALL gadget of the electric circuit area, a
 * round wall drawn from four clumps (DE0403COREWALL11/12/21/22.DFF) sixteen segments to each side
 * of its front, from a draw callback it registers with its Render. A box collision wrapped in a
 * collision handle opens it when a character touches it and closes it again when the contact ends:
 * while it moves, the morph animations of the first two clumps (the .DMA files) follow its opening
 * time; once fully open it shows the other two clumps. File name inferred from the class. Method
 * names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first).
 *
 * The constructor addresses its three member pointer constants and the vtable through the start of
 * .data, and its file names through the start of .rodata, so the linker keeps both sections whole:
 * the orphan RTTI base lists of the two first headers lead .data (in this order), and a zeroed copy
 * of the weak sp_counted_base vtable closes it. */
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/Render.h"
#include "Game/MathConstants.h"
#include "Game/FileQueue.h"
#include "boost/bind.hpp"
#include "boost/function.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* Two unused constants of unknown meaning and type: the linker keeps this unit's .rodata whole, and
 * there the first one comes before the RTTI names of the next two headers and the second one after
 * them (so they are defined around those headers; the original's are probably in its headers). */
static const float lbl_804F59E0[4][4] = {
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
};

#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"

static const float lbl_804F5AC4[3] = {0.0f, 0.0f, 0.0f};

using SonicteamUSA::System::CharaColli::CharaColliBase;

/* The RenderWare types and functions used here, declared locally because the RenderWare headers
 * need the C library headers (layouts as in rw/rwcore.h and rw/rpworld.h, reduced to the members
 * used). */
extern "C" {
typedef struct RwV3d {
    float x;
    float y;
    float z;
} RwV3d;

typedef struct RwSphere {
    RwV3d center;
    float radius;
} RwSphere;

typedef struct RwFrame RwFrame;
typedef struct RwCamera RwCamera;
typedef struct RpGeometry RpGeometry;

typedef struct RwObject {
    u8 type;
    u8 subKind;
    u8 flags;
    u8 privateFlags;
    void *parent;
} RwObject;

typedef struct RpClump {
    /* 0x00 */ RwObject object; /* the parent is the clump's frame */
} RpClump;

typedef struct RpAtomic {
    /* 0x00 */ u8 field_0x0[0x18];
    /* 0x18 */ RpGeometry *geometry;
    /* 0x1C */ RwSphere boundingSphere;
    /* 0x2C */ u8 field_0x2C[0x4C - 0x2C];
    /* 0x4C */ s32 interpolatorFlags; /* 0x2 (rpINTERPOLATORDIRTYSPHERE): the sphere is stale */
} RpAtomic;

typedef RpAtomic *(*RpAtomicCallBack)(RpAtomic *atomic, void *data);

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
/* RpClumpForAllAtomics, and the update of an atomic's bounding sphere from its morph targets (the
 * work of RenderWare's RpAtomicGetBoundingSphere). */
RpClump *fn_80456C54(RpClump *clump, RpAtomicCallBack callBack, void *data);
void fn_80456560(RpAtomic *atomic);
/* The number of delta morph targets of a geometry, and the delta morph values of an atomic (by
 * their use with the .DMA files; behaviour inferred). */
int fn_80443A54(RpGeometry *geometry);
float *fn_804442E4(RpAtomic *atomic);
}

/* The collision event a collider reports (name from the bound callback's RTTI; see
 * ElecCircuitColli.cpp). */
struct ColliDetectArgs {};

/* The event type of a handle's callbacks (see ElecCircuitColli.cpp). */
struct GadgetColliDetectArgs : public ColliDetectArgs {};

/* The function a handle calls back with a collision event (name inferred). */
typedef boost::function1<void, GadgetColliDetectArgs &> GadgetColliFunction;

/* The handle fn_8015B8C4 wraps a collider in (see ElecCircuitColli.cpp). */
class GadgetCharCollision {
  public:
    /* 0x00 */ GadgetColliFunction m_onContact;
    /* 0x0C */ u8 field_0xC[0x18 - 0xC];
    /* 0x18 */ GadgetColliFunction m_onRelease;
    /* 0x24 */ CharaColliBase *m_collider;
    /* 0x28 */ u32 field_0x28;
    /* 0x2C */ u32 m_flags;
};

extern "C" {
/* Loads a file of the model loader (behaviour inferred). */
void *fn_800122D4(void);
void *fn_80012700(void *loader, int arg1, const char *fileName);
/* Attaches the morph animation anim to clump (see Door.cpp). */
void fn_80037A80(RpClump *clump, void *anim);
/* Turns a frame about its y axis to angle (behaviour inferred). */
void fn_800E9CEC(RwFrame *frame, float angle, int combine);
/* The collider manager (behaviour inferred). */
void *fn_80021B08(void);
/* Creates a box collider of the given half size for charInfo (see ElecCircuitColli.cpp). */
boost::shared_ptr<CharaColliBase> fn_800C7C34(void *manager, const boost::weak_ptr<CharInfo> &charInfo,
                                              const Vec3f &size, int flags, int arg4, int arg5,
                                              const Vec3f *offset, const Vec3f *rotation,
                                              int arg8);
/* A vector constant read by the collision setup (meaning unknown). */
extern Vec3f lbl_805E07B8;
/* Wraps collider in a new handle (behaviour inferred). */
boost::shared_ptr<GadgetCharCollision> fn_8015B8C4(boost::shared_ptr<CharaColliBase> collider);
/* Assigns callback to one of a handle's callbacks (behaviour inferred). */
void fn_800F5980(GadgetColliFunction *target, const GadgetColliFunction &callback);
/* Draws the handle's shape for debugging, and advances the handle by dt (behaviour inferred). */
void fn_8015BE64(GadgetCharCollision *handle);
void fn_8015BE9C(GadgetCharCollision *handle, float dt);
/* Calls callback with every character in the collision event (behaviour inferred). */
void fn_8015AA84(ColliDetectArgs &args, void (*callback)(boost::shared_ptr<CharInfo> hit, void *user),
                 void *user);
}

class ElecCoreWall : public Gadget {
  public:
    ElecCoreWall(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~ElecCoreWall();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the wall under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* The handle's callbacks: a touching character opens the wall, the end of the contact closes
     * it (names inferred). */
    void OnColliDetect(ColliDetectArgs &args);
    void OnColliRelease(ColliDetectArgs &args);
    /* The draw callback (name inferred). */
    void Draw(RednerDrawArgs &args);
    /* Copies the owner's position and rotation into the gadget (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);

    static const char *s_taskName;

    /* 0x30 */ int m_state; /* 0 closed, 1 opening, 2 closing, 3 open (inferred; names inferred) */
    /* 0x34 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x3C */ boost::shared_ptr<GadgetCharCollision> m_handle;
    /* 0x44 */ boost::shared_ptr<Render> m_render;
    /* 0x4C */ RpClump *m_clump11; /* morphed while the wall moves */
    /* 0x50 */ RpClump *m_clump21;
    /* 0x54 */ RpClump *m_clump12; /* shown while the wall is open */
    /* 0x58 */ RpClump *m_clump22;
    /* 0x5C */ float m_openTime;
    /* 0x60 */ u32 field_0x60;
    /* 0x64 */ u32 field_0x64;
};
SIZE_ASSERT(ElecCoreWall, 0x68);

const char *ElecCoreWall::s_taskName = "ElecCoreWall";

/* The angle of one of the 32 segments of the wall's circle. */
static float lbl_805F00A4 = 2.0f * lbl_805FAD38 / 32.0f;

/* What the collision callback passes to the per-character hit function (name inferred). */
struct ElecCoreWallHitContext {
    /* 0x0 */ ElecCoreWall *m_wall;
};

/* Opens the wall when a character touches it (name inferred). Retail emits it after the
 * constructor and its template instances, so it is defined before the constructor. */
static void HitCharacter(boost::shared_ptr<CharInfo> hit, void *user) {
    ElecCoreWallHitContext *context = static_cast<ElecCoreWallHitContext *>(user);

    if (hit.get() != 0 && context != 0) {
        context->m_wall->m_state = 1;
    }
}

ElecCoreWall::ElecCoreWall(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CB88(0x58, 4, "ElecCoreWall");
    void *anim11 = fn_80012700(fn_800122D4(), 1, "DE0403COREWALL11.DMA");
    m_clump11 = (RpClump *)fn_80012700(fn_800122D4(), 1, "DE0403COREWALL11.DFF");
    m_clump12 = (RpClump *)fn_80012700(fn_800122D4(), 1, "DE0403COREWALL12.DFF");
    void *anim21 = fn_80012700(fn_800122D4(), 1, "DE0403COREWALL21.DMA");
    m_clump21 = (RpClump *)fn_80012700(fn_800122D4(), 1, "DE0403COREWALL21.DFF");
    m_clump22 = (RpClump *)fn_80012700(fn_800122D4(), 1, "DE0403COREWALL22.DFF");
    fn_80037A80(m_clump11, anim11);
    fn_80037A80(m_clump21, anim21);
    m_render = fn_8004DFB0(fn_80046178()->field_0x28, "ElecCoreWall");
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&ElecCoreWall::Draw, this, _1));

    boost::shared_ptr<CharaColliBase> collider =
        fn_800C7C34(fn_80021B08(), m_chara, Vec3f(900.0f, 1000.0f, 750.0f), 0x40000, 10,
                    0, &lbl_805E07A0, &lbl_805E07B8, 0);
    m_handle = fn_8015B8C4(collider);
    fn_800F5980(&m_handle->m_onContact, boost::bind(&ElecCoreWall::OnColliDetect, this, _1));
    fn_800F5980(&m_handle->m_onRelease, boost::bind(&ElecCoreWall::OnColliRelease, this, _1));
    ReadPlacement(m_owner);
    m_openTime = 0.0f;
    field_0x64 = 0;
    field_0x60 = 0;
    m_state = 0;
}

ElecCoreWall::~ElecCoreWall() {}

void ElecCoreWall::Behave(float dt) {
    switch (m_state) {
    case 0:
        break;
    case 1:
        m_openTime += dt;
        break;
    case 2:
        m_openTime -= dt;
        break;
    case 3:
        break;
    }
    fn_8015BE9C(m_handle.get(), dt);
}

void ElecCoreWall::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    if (m_handle.get() != 0) {
        m_handle->m_collider->SetCharInfo(m_chara.get());
        m_handle->m_collider->SetRotation(&m_chara->m_rotation);
        fn_8015BE64(m_handle.get());
    }
}

void ElecCoreWall::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
}

/* Atomic callback: sets the atomic's delta morph values to *data and its bounding radius to 1000
 * (name inferred). */
static RpAtomic *SetMorphValues(RpAtomic *atomic, void *data) {
    RpGeometry *geometry = atomic->geometry;

    if (geometry != 0) {
        int count = fn_80443A54(geometry);
        if (count > 0) {
            float *values = fn_804442E4(atomic);
            for (int i = 0; i < count; i++) {
                *values++ = *(float *)data;
            }
        }
    }

    RwSphere *sphere = ((atomic->interpolatorFlags & 0x2) ? fn_80456560(atomic), 0 : 0,
                        &atomic->boundingSphere);
    if (sphere != 0) {
        sphere->radius = 1000.0f;
    }
    return atomic;
}

void ElecCoreWall::Draw(RednerDrawArgs &args) {
    RwV3d position = {0.0f, 0.0f, 0.0f};
    RwSphere sphere;

    position.x = m_chara->m_position.x;
    position.y = m_chara->m_position.y;
    position.z = m_chara->m_position.z;
    sphere.radius = 1000.0f;
    sphere.center.x = position.x;
    sphere.center.y = position.y;
    sphere.center.z = position.z;
    if (fn_804856C8(lbl_805F15B4->curCamera, &sphere) != 0) {
        for (int i = 0; i < 16.0f; i++) {
            if (m_openTime < 0.0f) {
                if (15.0f == i && m_state == 2) {
                    m_openTime = 0.0f;
                    m_state = 0;
                }
                return;
            }
            if (m_openTime >= 0.76666665f) {
                if (i == 0) {
                    {
                        RwFrame *frame = (RwFrame *)m_clump12->object.parent;

                        fn_800E9CEC(frame, lbl_805F00A4 * (16.0f + i), 0);
                        fn_804875B0(frame, &position, 2);
                        fn_80456BC4(m_clump12);
                    }
                } else {
                    {
                        RwFrame *frame = (RwFrame *)m_clump12->object.parent;

                        fn_800E9CEC(frame, lbl_805F00A4 * (16.0f + i), 0);
                        fn_804875B0(frame, &position, 2);
                        fn_80456BC4(m_clump12);
                    }
                    {
                        RwFrame *frame = (RwFrame *)m_clump12->object.parent;

                        fn_800E9CEC(frame, lbl_805F00A4 * (16.0f - i), 0);
                        fn_804875B0(frame, &position, 2);
                        fn_80456BC4(m_clump12);
                    }
                    {
                        RwFrame *frame = (RwFrame *)m_clump22->object.parent;

                        fn_800E9CEC(frame, (i - 1) * lbl_805F00A4, 0);
                        fn_804875B0(frame, &position, 2);
                        fn_80456BC4(m_clump22);
                    }
                    {
                        RwFrame *frame = (RwFrame *)m_clump22->object.parent;

                        fn_800E9CEC(frame, -i * lbl_805F00A4, 0);
                        fn_804875B0(frame, &position, 2);
                        fn_80456BC4(m_clump22);
                    }
                    if (15.0f == i && m_state == 1) {
                        m_state = 3;
                    }
                }
            } else {
                float t = m_openTime;
                if (i == 0) {
                    fn_80456C54(m_clump11, SetMorphValues, &t);
                    {
                        RwFrame *frame = (RwFrame *)m_clump11->object.parent;

                        fn_800E9CEC(frame, lbl_805F00A4 * (16.0f + i), 0);
                        fn_804875B0(frame, &position, 2);
                        fn_80456BC4(m_clump11);
                    }
                } else {
                    fn_80456C54(m_clump11, SetMorphValues, &t);
                    {
                        RwFrame *frame = (RwFrame *)m_clump11->object.parent;

                        fn_800E9CEC(frame, lbl_805F00A4 * (16.0f + i), 0);
                        fn_804875B0(frame, &position, 2);
                        fn_80456BC4(m_clump11);
                    }
                    fn_80456C54(m_clump11, SetMorphValues, &t);
                    {
                        RwFrame *frame = (RwFrame *)m_clump11->object.parent;

                        fn_800E9CEC(frame, lbl_805F00A4 * (16.0f - i), 0);
                        fn_804875B0(frame, &position, 2);
                        fn_80456BC4(m_clump11);
                    }
                    fn_80456C54(m_clump21, SetMorphValues, &t);
                    {
                        RwFrame *frame = (RwFrame *)m_clump21->object.parent;

                        fn_800E9CEC(frame, (i - 1) * lbl_805F00A4, 0);
                        fn_804875B0(frame, &position, 2);
                        fn_80456BC4(m_clump21);
                    }
                    fn_80456C54(m_clump21, SetMorphValues, &t);
                    {
                        RwFrame *frame = (RwFrame *)m_clump21->object.parent;

                        fn_800E9CEC(frame, -i * lbl_805F00A4, 0);
                        fn_804875B0(frame, &position, 2);
                        fn_80456BC4(m_clump21);
                    }
                }
            }
        }
    }
}

void ElecCoreWall::OnColliDetect(ColliDetectArgs &args) {
    ElecCoreWallHitContext context;

    context.m_wall = this;
    fn_8015AA84(args, HitCharacter, &context);
}

void ElecCoreWall::OnColliRelease(ColliDetectArgs &args) {
    m_state = 2;
}

extern "C" void fn_802647B8(void) {
    fn_8006B474("DE0403COREWALL11.DFF", 0);
    fn_8006B474("DE0403COREWALL11.DMA", 0);
    fn_8006B474("DE0403COREWALL12.DFF", 0);
    fn_8006B474("DE0403COREWALL21.DFF", 0);
    fn_8006B474("DE0403COREWALL21.DMA", 0);
    fn_8006B474("DE0403COREWALL22.DFF", 0);
}

extern "C" void fn_802647B4(void) {}

void ElecCoreWall::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new ElecCoreWall(fn_801EB304(fn_8001C4F8(), 11), owner);
}

GadgetClassDesc lbl_8054C084 = {
    "ELEC_COREWALL", fn_802647B8, fn_802647B4, ElecCoreWall::Instantiate, 0, 0, 0x7E9, 1, 0, 10, 0,
};
