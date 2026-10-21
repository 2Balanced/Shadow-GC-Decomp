/* Cage::Cage (class name from RTTI): the CAGE gadget, a cage that holds the characters that touch
 * it. It shows the model of CASE.BIN picked by its "model" parameter, scaled by its "scale X/Y/Z"
 * parameters. While the switch whose id the owner supplies is off, its collision callback queues a
 * BindCommand on every character it touches; once the switch is on, the cage breaks (it switches to
 * the broken model, plays a sound) and its callback queues a ReleaseCommand instead. File name
 * inferred from the class. Method names marked "(name inferred)" are not original. Definitions are
 * in reverse address order (deferred inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/FileQueue.h"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* The RenderWare atomic (rw/rpworld.h has its full layout; that header does not build with the game
 * C++ flags, so only the members used here are declared). */
struct RpAtomic {
    /* 0x00 */ u8 field_0x0[0x1C];
    /* 0x1C */ Vec3f boundingSphereCenter;
    /* 0x28 */ float boundingSphereRadius;
    /* 0x2C */ u8 field_0x2C[0x4C - 0x2C];
    /* 0x4C */ s32 interpolatorFlags; /* 0x2 (rpINTERPOLATORDIRTYSPHERE): the sphere is stale */
    /* 0x50 */ u8 field_0x50[0x70 - 0x50];
};
SIZE_ASSERT(RpAtomic, 0x70);

struct RpClump;

typedef RpAtomic *(*RpAtomicCallBack)(RpAtomic *atomic, void *data);

/* BindCommand and ReleaseCommand (class names from RTTI): hold a character at, and let it go from,
 * the object whose character info they keep (behaviour inferred from the cage). */
class BindCommand : public CharCommand {
  public:
    BindCommand(const boost::weak_ptr<CharInfo> &binder);

    /* 0x14 */ boost::weak_ptr<CharInfo> m_binder;
};
SIZE_ASSERT(BindCommand, 0x1C);

class ReleaseCommand : public CharCommand {
  public:
    ReleaseCommand(const boost::weak_ptr<CharInfo> &binder);

    /* 0x14 */ boost::weak_ptr<CharInfo> m_binder;
};
SIZE_ASSERT(ReleaseCommand, 0x1C);

/* What fn_8015FA78 passes for each part of a model (layout from fn_8015FA78; names inferred). */
struct UnkModelPart_8015FA78 {
    /* 0x0 */ RpClump *m_clump;
    /* 0x4 */ void *field_0x4;
    /* 0x8 */ s32 m_index;
};
SIZE_ASSERT(UnkModelPart_8015FA78, 0xC);

typedef UnkModelPart_8015FA78 *(*UnkModelPartCallback_8015FA78)(UnkModelPart_8015FA78 *part,
                                                                void *data);

extern "C" {
/* RpClumpForAllAtomics, and the update of an atomic's bounding sphere from its morph targets (the
 * work of RenderWare's RpAtomicGetBoundingSphere). */
RpClump *fn_80456C54(RpClump *clump, RpAtomicCallBack callBack, void *data);
void fn_80456560(RpAtomic *atomic);
/* Loads the model file fileName for a gadget (behaviour inferred; arg0 unknown). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* Attaches a character info to model number index (behaviour inferred). */
bool fn_80161FC8(GadgetModelInfo *model, int index, const boost::weak_ptr<CharInfo> &info);
/* Passes offset to each part of the model whose id equals arg1 (behaviour inferred; meanings of
 * the integer arguments unknown). */
void fn_80161740(GadgetModelInfo *model, int arg1, const Vec3f *offset, int arg3);
/* Calls callback for each part of model while it returns non-null (behaviour inferred). */
void fn_8015FA78(GadgetModelInfo *model, UnkModelPartCallback_8015FA78 callback, void *data);
/* Copies params back into the owner's parameter block when its size equals size (behaviour
 * inferred; the counterpart of fn_800C9DBC). */
void fn_800C9D68(UnkPlacement_800C9CD0 *owner, const void *params, int size);
/* Plays sound id at the character info's position (behaviour inferred; the meanings of the last
 * two arguments are unknown). */
void fn_801D54F4(void *manager, int id, GadgetCharInfo *charInfo, int arg3, int arg4);
/* Calls callback with every character in the collision event (behaviour inferred; arg3 unknown). */
void fn_8015B70C(ColliDetectArgs &args,
                 void (*callback)(boost::shared_ptr<CharInfo> hit, void *user), void *user,
                 int arg3);
/* Scales the matrix's three rotation columns by the vector's components (behaviour inferred). */
void fn_8040B790(Mtx34f *matrix, const Vec3f *scale);
}

namespace Cage {

/* The four parameters the owner supplies (names from the parameter table). */
struct CageParams {
    CageParams() : model(0), scaleX(1.0f), scaleY(1.0f), scaleZ(1.0f) {}

    /* 0x0 */ int model;
    /* 0x4 */ float scaleX;
    /* 0x8 */ float scaleY;
    /* 0xC */ float scaleZ;
};
SIZE_ASSERT(CageParams, 0x10);

class Cage : public Gadget {
  public:
    Cage(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~Cage();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the cage under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* The model's collision callback (name inferred). */
    void OnColliDetect(ColliDetectArgs &args);
    /* Places the model at the character info's translation and rotation, scaled by the parameters
     * (name inferred). */
    void RebuildTransform();
    /* Copies the owner's position, rotation, parameters and switch id (name inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    /* Scales the bounding spheres of the model's atomics by the largest scale parameter (name
     * inferred). */
    void ScaleBoundingSpheres();

    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ CageParams m_params;
    /* 0x50 */ int m_state;      /* 0 closed, 2 broken (inferred) */
    /* 0x54 */ int m_modelIndex; /* the model shown (name inferred) */
    /* 0x58 */ u8 m_switchId;    /* copied from the owner (name inferred) */
    /* 0x5C */ u32 field_0x5C;   /* not used by this unit */
};

/* What the collision callback passes to the per-character hit function (name inferred). */
struct CageHitContext {
    CageHitContext() : m_mode(0) {}

    /* 0x0 */ boost::weak_ptr<GadgetCharInfo> m_chara;
    /* 0x8 */ int m_mode; /* 0 binds the character, 1 releases it (inferred) */
};
SIZE_ASSERT(CageHitContext, 0xC);

} // namespace Cage

SIZE_ASSERT(Cage::Cage, 0x60);

/* The size of the cage model (name inferred). */
static Vec3f lbl_8057F9F8(10.0f, 12.5f, 10.0f);

/* The sizes of the two broken models (name inferred). The original copy-constructs them from
 * temporaries through the vector type's out-of-line copy constructor, fn_800091EC; this wrapper's
 * constructor writes those copies out so that the startup code stays the same (size1 is declared
 * first to give each temporary the original's stack slot). */
static struct CageBrokenSizes {
    CageBrokenSizes() {
        float size1[3];
        float size0[3];

        size0[0] = 10.0f;
        size0[1] = 4.0f;
        size0[2] = 10.0f;
        fn_800091EC((Vec3f *)sizes[0], (Vec3f *)size0);
        size1[0] = 10.0f;
        size1[1] = 3.0f;
        size1[2] = 10.0f;
        fn_800091EC((Vec3f *)sizes[1], (Vec3f *)size1);
    }

    /* 0x00 */ float sizes[2][3];
} lbl_8057FA04;

namespace Cage {

const char *Cage::s_taskName = "Cage";

/* The static callbacks are defined before the constructor: the original emits them after it. */

/* Binds or releases hit, as the context says (name inferred). */
static void HitCharacter(boost::shared_ptr<CharInfo> hit, void *user) {
    CageHitContext *context = static_cast<CageHitContext *>(user);

    if (context != 0) {
        if (context->m_mode == 0) {
            hit->PushCommand(new BindCommand(boost::weak_ptr<CharInfo>(context->m_chara)));
        } else if (context->m_mode == 1) {
            hit->PushCommand(new ReleaseCommand(boost::weak_ptr<CharInfo>(context->m_chara)));
        }
    }
}

/* Scales an atomic's bounding sphere by *data (name inferred). */
static RpAtomic *ScaleBoundingSphere(RpAtomic *atomic, void *data) {
    if (atomic != 0 && data != 0) {
        if (atomic->interpolatorFlags & 0x2) {
            fn_80456560(atomic);
        }
        atomic->boundingSphereRadius *= *(float *)data;
    }
    return atomic;
}

/* Part callback: scales the bounding spheres of the part's atomics (name inferred). */
static UnkModelPart_8015FA78 *ScalePartBoundingSpheres(UnkModelPart_8015FA78 *part, void *data) {
    fn_80456C54(part->m_clump, ScaleBoundingSphere, data);
    return part;
}

Cage::Cage(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CB88(0x3A, 4, "Cage");
    LoadFromOwner(m_owner);
    m_model = fn_8015F800(0, "CASE.BIN");
    m_modelIndex = m_params.model;
    m_model->fn_80162A68(m_modelIndex, 0);
    fn_80161FC8(m_model.get(), m_modelIndex, m_chara);
    fn_800D0B64(m_model->field_0x4, boost::bind(&Cage::OnColliDetect, this, _1));
    m_model->fn_80160C28(m_modelIndex);
    m_model->fn_8015FF60();

    float height = lbl_8057F9F8.y * m_params.scaleY;
    m_model->fn_801617D0(m_modelIndex, -2, lbl_8057F9F8.x * m_params.scaleX, height,
                         lbl_8057F9F8.z * m_params.scaleZ);
    Vec3f offset(0.0f, height, 0.0f);
    fn_80161740(m_model.get(), m_modelIndex, &offset, -2);
    ScaleBoundingSpheres();
    RebuildTransform();
}

Cage::~Cage() {}

void Cage::Behave(float dt) {
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }

    switch (m_state) {
    case 0:
        if (fn_8016904C(fn_800CB69C(), m_switchId) == 1) {
            m_state = 2;
            if (m_model.get() != 0) {
                m_model->fn_80160B18(m_modelIndex);
                m_model->fn_80161880();
                int broken = m_modelIndex + 2;
                fn_80161FC8(m_model.get(), broken, m_chara);
                float x = m_params.scaleX * lbl_8057FA04.sizes[m_modelIndex][0];
                float y = m_params.scaleY * lbl_8057FA04.sizes[m_modelIndex][1];
                float z = m_params.scaleZ * lbl_8057FA04.sizes[m_modelIndex][2];
                m_model->fn_801617D0(broken, -2, x, y, z);
                Vec3f offset(0.0f, y, 0.0f);
                fn_80161740(m_model.get(), broken, &offset, -2);
            }
            switch (m_params.model) {
            case 0:
                fn_801D54F4(fn_80053164(), 0x1002, m_chara.get(), 0, 1);
                break;
            case 1:
                fn_801D54F4(fn_80053164(), 0x1001, m_chara.get(), 0, 1);
                break;
            }
        }
        break;
    case 1:
        break;
    case 2:
        break;
    }

    RebuildTransform();
}

void Cage::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    RebuildTransform();
    if (m_model.get() != 0) {
        m_model->fn_80162EF4();
        if (m_params.model != m_modelIndex) {
            m_model->fn_80161880();
            m_model->fn_801604FC();
            m_model->fn_80162804();
            m_modelIndex = m_params.model;
            m_model->fn_80162A68(m_modelIndex, 0);
            fn_80161FC8(m_model.get(), m_modelIndex, m_chara);
            m_model->fn_80160C28(m_modelIndex);
            m_model->fn_8015FF60();
            m_params.model = m_modelIndex;
            fn_800C9D68(owner, &m_params, sizeof(m_params));
        }

        float height = lbl_8057F9F8.y * m_params.scaleY;
        m_model->fn_801617D0(m_modelIndex, -2, lbl_8057F9F8.x * m_params.scaleX, height,
                             lbl_8057F9F8.z * m_params.scaleZ);
        Vec3f offset(0.0f, height, 0.0f);
        fn_80161740(m_model.get(), m_modelIndex, &offset, -2);
        ScaleBoundingSpheres();
        m_model->fn_8016108C();
    }
}

void Cage::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    fn_800C9E60(owner, &m_switchId);
    m_state = 0;
}

void Cage::RebuildTransform() {
    if (m_model.get() != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B884(&matrix, m_chara->m_rotation.x);
        fn_8040B80C(&matrix, m_chara->m_rotation.z);
        Vec3f scale(m_params.scaleX, m_params.scaleY, m_params.scaleZ);
        fn_8040B790(&matrix, &scale);
        m_model->fn_801633DC(&matrix);
    }
}

void Cage::OnColliDetect(ColliDetectArgs &args) {
    if (fn_8016904C(fn_800CB69C(), m_switchId) != 1) {
        CageHitContext context;

        context.m_chara = m_chara;
        fn_8015B70C(args, HitCharacter, &context, 4);
    } else {
        CageHitContext context;

        context.m_chara = m_chara;
        context.m_mode = 1;
        fn_8015B70C(args, HitCharacter, &context, 4);
    }
}

void Cage::ScaleBoundingSpheres() {
    if (m_model.get() != 0) {
        float scale = m_params.scaleX;

        if (scale < m_params.scaleY) {
            scale = m_params.scaleY;
        }
        if (scale < m_params.scaleZ) {
            scale = m_params.scaleZ;
        }
        fn_8015FA78(m_model.get(), ScalePartBoundingSpheres, &scale);
    }
}

} // namespace Cage

extern "C" void fn_800F2228(void) {
    fn_8006B474("CASE.BIN", 1);
}

extern "C" void fn_800F2224(void) {}

void Cage::Cage::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new Cage(parent, owner);
}

static GadgetParamDesc lbl_80524F04[] = {
    {1, "model", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {4, "scale X", 0, 0, 0, 1.0f, 1.0f, 10.0f, fn_800CBC20, 0},
    {4, "scale Y", 0, 0, 0, 1.0f, 1.0f, 10.0f, fn_800CBC20, 0},
    {4, "scale Z", 0, 0, 0, 1.0f, 1.0f, 10.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80524FCC = {
    "CAGE", fn_800F2228, fn_800F2224, Cage::Cage::Instantiate, 0, 0, 7, 1, 0, 10, lbl_80524F04,
};
