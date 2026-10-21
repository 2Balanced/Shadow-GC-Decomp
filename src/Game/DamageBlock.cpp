/* DamageBlock (class name from RTTI): the DAMAGE BLOCK gadget, a static block shown with the model
 * variant picked by its "kind" parameter from DAMAGEBLOCK.BIN, with the block's character info
 * attached to the variant's collision. It only follows its owner's position, yaw and kind. File
 * name inferred from the class. Method names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/CharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

extern "C" {
/* GadgetModelInfo helpers (behaviour inferred): attach the character info to model variant index,
 * and return the number of variants the model file holds. */
void fn_80161FC8(GadgetModelInfo *model, int index, const boost::weak_ptr<CharInfo> &info);
int fn_8015FF44(GadgetModelInfo *model);
/* Copies params back into the owner's parameter block when its size equals size (behaviour
 * inferred; the counterpart of fn_800C9DBC). */
void fn_800C9D68(UnkPlacement_800C9CD0 *owner, const void *params, int size);
}

/* The parameters the owner supplies (name from the parameter table). */
struct DamageBlockParams {
    /* 0x0 */ s32 kind; /* the model variant */
};
SIZE_ASSERT(DamageBlockParams, 0x4);

class DamageBlock : public Gadget {
  public:
    DamageBlock(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~DamageBlock();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the block under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position, rotation and parameters (name inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    /* Places the model at the character info's translation, turned about the Y axis (name
     * inferred). */
    void RebuildTransform();

    static const char *s_taskName;

    /* 0x30 */ s32 m_modelIndex; /* the variant the model shows */
    /* 0x34 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x3C */ GadgetModelInfo *m_model;
    /* 0x40 */ DamageBlockParams m_params;
    /* 0x44 */ u32 field_0x44; /* never accessed */
};
SIZE_ASSERT(DamageBlock, 0x48);

const char *DamageBlock::s_taskName = "DamageBlock";

DamageBlock::DamageBlock(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_chara = fn_8015CD90(0x21, 4);
    m_modelIndex = m_params.kind;
    m_model = new GadgetModelInfo(1, "DAMAGEBLOCK.BIN");
    m_model->fn_80162A68(m_modelIndex, 0);
    fn_80161FC8(m_model, m_modelIndex, m_chara);
    LoadFromOwner(m_owner);
    RebuildTransform();
}

DamageBlock::~DamageBlock() {
    if (m_model != 0) {
        delete m_model;
        m_model = 0;
    }
}

void DamageBlock::Behave(float dt) {
    if (m_model != 0) {
        m_model->fn_80162FB0(dt);
    }
}

void DamageBlock::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    if (m_model != 0) {
        if (m_params.kind != m_modelIndex) {
            int count = fn_8015FF44(m_model);

            m_modelIndex = m_params.kind;
            if (m_modelIndex < 0) {
                m_modelIndex = 0;
            }
            if (m_modelIndex > count - 1) {
                m_modelIndex = count - 1;
            }
            m_model->fn_80161880();
            m_model->fn_80162804();
            m_model->fn_80162A68(m_modelIndex, 0);
            fn_80161FC8(m_model, m_modelIndex, m_chara);
            m_params.kind = m_modelIndex;
            fn_800C9D68(owner, &m_params, sizeof(m_params));
        }
        RebuildTransform();
        m_model->fn_8016108C();
    }
}

void DamageBlock::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
}

void DamageBlock::RebuildTransform() {
    if (m_model != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        m_model->fn_801633DC(&matrix);
    }
}

extern "C" void fn_801430A4(void) {
    fn_8006B474("DAMAGEBLOCK.BIN", 1);
}

extern "C" void fn_801430A0(void) {}

void DamageBlock::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new DamageBlock(fn_801EB304(fn_8001C4F8(), 11), owner);
}

static GadgetParamDesc lbl_805297A8[] = {
    {1, "kind", 0, 0, 0x10000, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_805297F8 = {
    "DAMAGE BLOCK", fn_801430A4, fn_801430A0, DamageBlock::Instantiate, 0, 0, 0x1134, 1, 0, 10,
    lbl_805297A8,
};
