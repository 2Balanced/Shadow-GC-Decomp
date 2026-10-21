/* ElecBarrier (class name from RTTI): a barrier of the electric circuit area that shows one of the
 * BARRIER.BIN model's variants, chosen by its "BarrierType" parameter. Once its circuit is
 * switched it plays a sound, fades its colour over one second and then reports itself finished.
 * File name inferred from the class. Method names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* Four colour bytes (red, green, blue, alpha; inferred) built by an out-of-line constructor. */
struct UnkColor_8040D2D8 {
    UnkColor_8040D2D8(u8 r, u8 g, u8 b, u8 a);

    /* 0x0 */ u8 r;
    /* 0x1 */ u8 g;
    /* 0x2 */ u8 b;
    /* 0x3 */ u8 a;
};
SIZE_ASSERT(UnkColor_8040D2D8, 0x4);

class UnkSoundManager_80053164;

extern "C" {
/* GadgetModelInfo methods not declared in its header (behaviour inferred): select the collision
 * of variant id, drop the current animations, find the playing material animation of variant id,
 * attach a character info to the collision of variant id, and return the collider of variant
 * id. */
int fn_80162424(GadgetModelInfo *model, int id);
void fn_8016232C(GadgetModelInfo *model);
void *fn_80162298(GadgetModelInfo *model, int id);
int fn_80161FC8(GadgetModelInfo *model, int id, const boost::weak_ptr<CharInfo> &info);
boost::weak_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> fn_80161614(GadgetModelInfo *model, int id);
/* Blends animation's colour from from to to as t goes from t0 to t1 (behaviour inferred). */
void fn_80154C6C(void *animation, const UnkColor_8040D2D8 *from, const UnkColor_8040D2D8 *to,
                 float t, float t0, float t1);
/* Writes params back into the owner's parameter block (behaviour inferred). */
void fn_800C9D68(UnkPlacement_800C9CD0 *owner, const void *params, int size);
/* Tells the gadget's owner that the gadget is done (behaviour inferred). */
void fn_8015A794(Gadget *gadget);
/* Returns the sound manager and plays sound id through it (behaviour inferred). */
UnkSoundManager_80053164 *fn_80053164(void);
int fn_801D560C(UnkSoundManager_80053164 *manager, int id, int arg2, int arg3);
}

class ElecBarrier : public Gadget {
  public:
    ElecBarrier(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~ElecBarrier();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the barrier under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position, rotation, circuit id and barrier type into the gadget (name
     * inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model at the character info's translation and rotation (name inferred). */
    void RebuildTransform();

    static const char *s_taskName;

    /* 0x30 */ s32 m_type; /* the "BarrierType" parameter */
    /* 0x34 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x3C */ GadgetModelInfo *m_model;
    /* 0x40 */ u8 field_0x40;     /* copied from the owner; the circuit id it waits for (inferred) */
    /* 0x44 */ s32 m_shownType;   /* the model variant on show (inferred) */
    /* 0x48 */ s32 m_state;       /* 0 waiting, 1 fading out (inferred) */
    /* 0x4C */ float m_fadeTimer; /* time since the circuit was switched (inferred) */
};
SIZE_ASSERT(ElecBarrier, 0x50);

const char *ElecBarrier::s_taskName = "ElecBarrier";

/* The colours the barrier fades through once its circuit is switched (meaning inferred). */
static UnkColor_8040D2D8 lbl_805F0090(0xFF, 0x4C, 0x47, 0x00);
static UnkColor_8040D2D8 lbl_805F0094(0xFF, 0xFF, 0xFF, 0xFF);
static UnkColor_8040D2D8 lbl_805F0098(0xFF, 0x00, 0x00, 0x00);

ElecBarrier::ElecBarrier(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    fn_800C9DBC(owner, &m_type, sizeof(m_type));
    m_chara = fn_8015CB88(0x56, 4, "ElecBarrier");
    m_model = new GadgetModelInfo(1, "BARRIER.BIN");
    m_shownType = m_type;
    m_model->fn_80162A68(m_shownType, 0);
    fn_80162424(m_model, m_shownType);
    ReadPlacement(m_owner);
    fn_80161FC8(m_model, m_shownType, m_chara);

    boost::weak_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> collider = fn_80161614(m_model, m_shownType);
    if (!collider.expired()) {
        boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> locked = collider.lock();
        switch (m_shownType) {
        case 0:
        case 1:
            locked->SetAttributes(0x30302, -1);
            break;
        case 2:
        default:
            locked->SetAttributes(0x30301, -1);
            break;
        }
    }

    if (fn_8016904C(fn_800CB69C(), field_0x40) == 1) {
        m_flags |= 1;
        return;
    }

    m_state = 0;
    RebuildTransform();
}

ElecBarrier::~ElecBarrier() {
    if (m_model != 0) {
        delete m_model;
        m_model = 0;
    }
}

void ElecBarrier::Behave(float dt) {
    if (m_model != 0) {
        m_model->fn_80162FB0(dt);
    }

    switch (m_state) {
    case 0:
        if (fn_8016904C(fn_800CB69C(), field_0x40) == 1) {
            m_state = 1;
            m_fadeTimer = 0.0f;
            fn_801D560C(fn_80053164(), 0x7242, 0, 0);
        }
        break;
    case 1:
        m_fadeTimer += dt;
        if (m_fadeTimer > 1.0f) {
            fn_8015A794(this);
            m_flags |= 1;
            return;
        }

        void *animation = fn_80162298(m_model, m_shownType);
        if (animation != 0) {
            if (m_fadeTimer < 0.1f) {
                fn_80154C6C(animation, &lbl_805F0090, &lbl_805F0094, m_fadeTimer, 0.0f, 0.1f);
            } else {
                fn_80154C6C(animation, &lbl_805F0094, &lbl_805F0098, m_fadeTimer, 0.1f, 1.0f);
            }
        }
        break;
    }

    RebuildTransform();
}

void ElecBarrier::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    m_state = 0;
    RebuildTransform();
    if (m_model != 0) {
        m_model->fn_80162EF4();
        if (m_type != m_shownType) {
            m_model->fn_80161880();
            fn_8016232C(m_model);
            m_model->fn_80162804();
            m_shownType = m_type;
            m_model->fn_80162A68(m_shownType, 0);
            fn_80162424(m_model, m_shownType);
            fn_80161FC8(m_model, m_shownType, m_chara);
            m_type = m_shownType;
            fn_800C9D68(owner, &m_type, sizeof(m_type));
        }
        m_model->fn_8016108C();
    }
}

void ElecBarrier::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9E60(owner, &field_0x40);
    fn_800C9DBC(owner, &m_type, sizeof(m_type));
}

void ElecBarrier::RebuildTransform() {
    if (m_model != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B884(&matrix, m_chara->m_rotation.x);
        fn_8040B80C(&matrix, m_chara->m_rotation.z);
        m_model->fn_801633DC(&matrix);
    }
}

extern "C" void fn_80263824(void) {
    fn_8006B474("BARRIER.BIN", 1);
}

extern "C" void fn_80263820(void) {}

void ElecBarrier::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new ElecBarrier(fn_801EB304(fn_8001C4F8(), 11), owner);
}

static GadgetParamDesc lbl_8054BEF8[] = {
    {1, "BarrierType", 0, 0, 2, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_8054BF48 = {
    "ELEC_BARRIER", fn_80263824, fn_80263820, ElecBarrier::Instantiate, 0, 0, 0x7D8, 1, 0, 10,
    lbl_8054BEF8,
};
