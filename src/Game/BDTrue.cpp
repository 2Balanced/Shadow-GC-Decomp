/* BDTrue::BDTrue (class name from RTTI; its s_taskName is "BDTrue"): the BD True gadget, a model
 * (BDTRUE.BIN) with particles that always turns to face the camera's heading. It takes no
 * parameters. File name inferred from the class. Names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/CharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

class UnkCamera_8000D0F0;
class UnkCameraManager_80009548;

extern "C" {
/* GadgetModelInfo helpers (behaviour inferred; see BAMine.cpp and BAGunShip.cpp). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
void fn_8015FA78(GadgetModelInfo *model, void *(*callback)(void *part, void *data), void *data);
void *fn_80169D00(void *part, void *data);
void fn_80160A84(GadgetModelInfo *model, int index, int motion);

/* Particle controller helpers (behaviour inferred). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void fn_80167010(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_8016761C(GadgetParticle::GadgetParticleCtrl *ctrl, float dt);

/* The camera manager, a camera of it, and a camera's angles (behaviour inferred; see
 * HintRing.cpp). */
UnkCameraManager_80009548 *fn_80009548(void);
boost::shared_ptr<UnkCamera_8000D0F0> fn_8000D0F0(UnkCameraManager_80009548 *manager, int index);
Vec3f fn_8000A610(const u8 *camera);
}

/* The angles of camera index (name inferred; the shared out-of-line copy HintRing.cpp defines). */
inline Vec3f GetCameraAngles(UnkCameraManager_80009548 *manager, int index) {
    return fn_8000A610(reinterpret_cast<const u8 *>(fn_8000D0F0(manager, index).get()) + 0x8);
}

namespace BDTrue {

class BDTrue : public Gadget {
  public:
    BDTrue(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~BDTrue();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's placement and resets the state (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model, turned to the camera's heading (name inferred). */
    void UpdatePosition();

    static const char *s_fileName;
    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particle;
    /* 0x48 */ s32 m_state; /* 0 new, 1 running */
    /* 0x4C */ u32 pad_0x4C; /* the class size is a multiple of 8 */
};
SIZE_ASSERT(BDTrue, 0x50);

const char *BDTrue::s_fileName = "BDTRUE.BIN";
const char *BDTrue::s_taskName = "BDTrue";

BDTrue::BDTrue(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x8C, 4);
    ReadPlacement(owner);
    m_model = fn_8015F800(1, s_fileName);
    m_model->fn_80162A68(0, 0);
    m_model->fn_80160C28(0);
    m_model->fn_8015FF60();
    fn_80160A84(m_model.px, 0, 1);
    if (m_model.px != 0) {
        m_particle = fn_80166AF4();
        UnkParticleAttach_80169D00 ref;
        ref.ctrl = m_particle;
        fn_8015FA78(m_model.px, fn_80169D00, &ref);
    }
    if (m_particle.px != 0) {
        fn_80167010(m_particle.px, -1);
    }
    UpdatePosition();
}

BDTrue::~BDTrue() {}

void BDTrue::Behave(float dt) {
    if (m_model.px != 0) {
        m_model->fn_80162FB0(dt);
    }
    if (m_particle.px != 0) {
        fn_8016761C(m_particle.px, dt);
    }
    switch (m_state) {
    case 0:
        m_state = 1;
        break;
    case 1:
        break;
    case 2:
        break;
    }
    UpdatePosition();
}

void BDTrue::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    UpdatePosition();
    if (m_model.px != 0) {
        m_model->fn_80162EF4();
    }
}

void BDTrue::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    m_state = 0;
}

void BDTrue::UpdatePosition() {
    if (m_model.px != 0) {
        Vec3f angles = GetCameraAngles(fn_80009548(), 0);
        float heading = angles.y;
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, heading);
        m_model->fn_801633DC(&matrix);
    }
}

} // namespace BDTrue

extern "C" void fn_80303CD0(void) {
    fn_8006B474(BDTrue::BDTrue::s_fileName, 1);
}

extern "C" void fn_80303CCC(void) {}

namespace BDTrue {

void BDTrue::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new BDTrue(parent, owner);
}

} // namespace BDTrue

GadgetClassDesc lbl_8055AEC0 = {
    "BD True", fn_80303CD0, fn_80303CCC, BDTrue::BDTrue::Instantiate, 0, 0, 0x1B58, 1, 0, 10, 0,
};
