/* BackDrop (class name from RTTI): the BACKDROP gadget, a backdrop model from BACKDROP.BIN that rises
 * slowly and drops back to its placed height once it has risen too far. File name inferred from
 * the class. Method names marked "(name inferred)" are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathConstants.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

extern "C" {
/* Builds the character info of a gadget type and loads a gadget model (see GadgetUtil.h). */
boost::shared_ptr<GadgetCharInfo> fn_8015CD90(int arg0, int arg1);
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
}

/* A speed (name inferred). As a call it is not a constant expression, so the constants below are
 * stored by the static initializer, as in retail (as in BDHologram.cpp). */
inline float Units(float units) {
    return units;
}

/* How fast the backdrop rises, and how far above its place it may rise (names inferred). */
static float s_riseSpeed = Units(100.0f);
static float s_riseLimit = Units(400.0f);

class BackDrop : public Gadget {
  public:
    BackDrop(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~BackDrop();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's placement (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model at the character info's place (name inferred). */
    void RebuildTransform();

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
};
SIZE_ASSERT(BackDrop, 0x40);

const char *BackDrop::s_taskName = "BackDrop";

BackDrop::BackDrop(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x6A, 4);
    ReadPlacement(owner);
    m_model = fn_8015F800(1, "BACKDROP.BIN");
    m_model->fn_80162A68(0, 0);
}

BackDrop::~BackDrop() {}

extern "C" void fn_8026B59C(void) {
    fn_8006B474("BACKDROP.BIN", 1);
}

extern "C" void fn_8026B598(void) {}

void BackDrop::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new BackDrop(fn_801EB304(fn_8001C4F8(), 11), owner);
}

void BackDrop::Behave(float dt) {
    m_chara->m_position.y += s_riseSpeed * dt;

    Vec3f position;
    fn_800C9F14(m_owner, &position);
    if (m_chara->m_position.y - position.y > s_riseLimit) {
        m_chara->m_position.y = position.y;
    }
    RebuildTransform();
    m_model->fn_80162FB0(dt);
}

void BackDrop::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    RebuildTransform();
    m_model->fn_80162EF4();
}

void BackDrop::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
}

void BackDrop::RebuildTransform() {
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_chara->m_position);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    fn_8040B884(&matrix, m_chara->m_rotation.x);
    fn_8040B80C(&matrix, m_chara->m_rotation.z);
    m_model->fn_801633DC(&matrix);
}

GadgetClassDesc lbl_8054C790 = {
    "BACKDROP", fn_8026B59C, fn_8026B598, BackDrop::Instantiate, 0, 0, 0x189C, 1, 0, 10, 0,
};
