/* Footing (class name from RTTI): the FOOTING gadget, a platform that shows one of the models of
 * FOOTING.BIN, picked by its "model" parameter, at the owner's position turned about the Y axis.
 * File name inferred from the class. Method names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/FileQueue.h"
#include "size_assert.h"

extern "C" {
/* Builds the character info of a gadget (behaviour inferred; the arguments' meanings are
 * unknown). */
boost::shared_ptr<GadgetCharInfo> fn_8015CD90(int arg0, int arg1);
/* Returns the number of models the model file holds (behaviour inferred). */
int fn_8015FF44(GadgetModelInfo *model);
/* Attaches a character info to model number index (behaviour inferred). */
bool fn_80161FC8(GadgetModelInfo *model, int index, const boost::weak_ptr<CharInfo> &info);
/* Copies params back into the owner's parameter block when its size equals size (behaviour
 * inferred; the counterpart of fn_800C9DBC). */
void fn_800C9D68(UnkPlacement_800C9CD0 *owner, const void *params, int size);
}

class Footing : public Gadget {
  public:
    Footing(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~Footing();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the footing under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position, rotation and "model" parameter into the gadget (name
     * inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model at the character info's translation, turned about the Y axis (name
     * inferred). */
    void RebuildTransform();

    static const char *s_taskName;

    /* 0x30 */ int m_modelIndex; /* the model shown (name inferred) */
    /* 0x34 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x3C */ GadgetModelInfo *m_model;
    /* 0x40 */ int m_modelParam; /* the "model" parameter */
    /* 0x44 */ u32 field_0x44;
};
SIZE_ASSERT(Footing, 0x48);

const char *Footing::s_taskName = "Footing";

Footing::Footing(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    fn_800C9DBC(owner, &m_modelParam, sizeof(m_modelParam));
    m_chara = fn_8015CD90(4, 4);
    m_modelIndex = m_modelParam;
    m_model = new GadgetModelInfo(1, "FOOTING.BIN");
    m_model->fn_80162A68(m_modelIndex, 0);
    fn_80161FC8(m_model, m_modelIndex, m_chara);
    ReadPlacement(m_owner);
    RebuildTransform();
}

Footing::~Footing() {
    if (m_model != 0) {
        delete m_model;
        m_model = 0;
    }
}

void Footing::Behave(float dt) {
    if (m_model != 0) {
        m_model->fn_80162FB0(dt);
    }
}

void Footing::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    RebuildTransform();
    if (m_model != 0) {
        if (m_modelParam != m_modelIndex) {
            int count = fn_8015FF44(m_model);

            m_modelIndex = m_modelParam;
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
            m_modelParam = m_modelIndex;
            fn_800C9D68(owner, &m_modelParam, sizeof(m_modelParam));
        }
        m_model->fn_8016108C();
    }
}

void Footing::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_modelParam, sizeof(m_modelParam));
}

void Footing::RebuildTransform() {
    if (m_model != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        m_model->fn_801633DC(&matrix);
    }
}

extern "C" void fn_80149360(void) {
    fn_8006B474("FOOTING.BIN", 1);
}

extern "C" void fn_8014935C(void) {}

void Footing::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new Footing(fn_801EB304(fn_8001C4F8(), 11), owner);
}

static GadgetParamDesc lbl_80529D18[] = {
    {1, "model", 0, 0, 0x10000, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80529D68 = {
    "FOOTING", fn_80149360, fn_8014935C, Footing::Instantiate, 0, 0, 0x1130, 1, 0, 10, lbl_80529D18,
};
