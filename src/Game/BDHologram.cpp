/* BDHologram::BDHologram (class name from RTTI; its s_taskName is "BDHologram"): the BD Hologram
 * gadget, a hologram (BDHOLOGRAM.BIN) with particles that hums until the camera comes within
 * "distance" of it (measured flat, ignoring height). Then it vanishes, plays a sound and has the
 * hint manager show its "voice id" message. File name inferred from the class. Names marked "(name
 * inferred)" are not original. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
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

class UnkCameraManager_80009548;

/* The manager returned by fn_8006D6A8 (see ElecBomb.cpp); only the member read here is laid out. */
class UnkPlayerManager_8006D6A8 {
  public:
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ void *field_0x20;
};

/* The hint manager (class name unknown; see HintRing.cpp and EnemyHint.cpp). */
class UnkHintManager_801FB180;

/* The out-of-line copy of the inline accessor HintRing.cpp defines (0x80109D64). */
UnkHintManager_801FB180 *GetHintManager();

extern "C" {
/* GadgetModelInfo helpers (behaviour inferred; see BAMine.cpp and BAGunShip.cpp). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
void fn_8015FA78(GadgetModelInfo *model, void *(*callback)(void *part, void *data), void *data);
void *fn_80169D00(void *part, void *data);

/* Particle controller helpers (behaviour inferred). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void fn_80167010(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_8016761C(GadgetParticle::GadgetParticleCtrl *ctrl, float dt);

/* The camera manager and the position of camera index (behaviour inferred). */
UnkCameraManager_80009548 *fn_80009548(void);
const Vec3f *fn_80010244(UnkCameraManager_80009548 *manager, int index);

/* The player manager (see ElecBomb.cpp), the hint branch for its member at 0x20, -1 for none
 * (behaviour inferred), and the hint manager showing message id of branch (see HintRing.cpp). */
UnkPlayerManager_8006D6A8 *fn_8006D6A8(void);
s32 fn_801FAAC4(void *arg);
s32 fn_801FAF00(UnkHintManager_801FB180 *manager, s32 id, s32 branch, int arg3, int arg4);

/* Plays sound id at a character info, once and as a loop for source (behaviour inferred). */
void fn_801D54F4(void *manager, int id, GadgetCharInfo *source, int arg3, int arg4);
void fn_801D53C4(void *manager, int id, GadgetCharInfo *charInfo, void *source, int arg4, int arg5,
                 int arg6);
}

/* A length (name inferred). As a call it is not a constant expression, so the constant below is
 * stored by the static initializer, as in retail (as in SecretDisk.cpp). */
inline float Span(float length) {
    return length;
}

/* The default "distance" (name inferred). */
static float s_defaultDistance = Span(500.0f);

namespace BDHologram {

/* The parameters the owner supplies (names from the parameter table). */
struct BDHologramParams {
    BDHologramParams() : distance(s_defaultDistance), voiceId(0) {}

    /* 0x0 */ float distance;
    /* 0x4 */ s32 voiceId;
};
SIZE_ASSERT(BDHologramParams, 0x8);

class BDHologram : public Gadget {
  public:
    BDHologram(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~BDHologram();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's placement and parameters and resets the state (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model (name inferred). */
    void UpdatePosition();

    static const char *s_fileName;
    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particle;
    /* 0x48 */ s32 m_state; /* 0 new, 1 waiting for the camera, 2 done */
    /* 0x4C */ BDHologramParams m_params;
    /* 0x54 */ u32 pad_0x54; /* the class size is a multiple of 8 */
};
SIZE_ASSERT(BDHologram, 0x58);

const char *BDHologram::s_fileName = "BDHOLOGRAM.BIN";
const char *BDHologram::s_taskName = "BDHologram";

BDHologram::BDHologram(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x8B, 4);
    ReadPlacement(owner);
    m_model = fn_8015F800(1, s_fileName);
    m_model->fn_80162A68(0, 0);
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

BDHologram::~BDHologram() {}

void BDHologram::Behave(float dt) {
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
    case 1: {
        float range = m_params.distance * m_params.distance;
        const Vec3f *cameraPosition = fn_80010244(fn_80009548(), 0);
        Vec3f camera;
        fn_800091EC(&camera, cameraPosition);
        Vec3f position;
        fn_800091EC(&position, &m_chara->m_position);
        position.y = 0.0f;
        camera.y = 0.0f;
        Vec3f offset;
        fn_8040CD74(&offset, &camera, &position);
        if (fn_8040CF2C(&offset) < range) {
            if (m_particle.px != 0) {
                m_particle.reset();
            }
            m_state = 2;
            s32 branch = fn_801FAAC4(fn_8006D6A8()->field_0x20);
            if (branch != -1) {
                fn_801FAF00(GetHintManager(), m_params.voiceId, branch, 0, 0);
            }
            fn_801D54F4(fn_80053164(), 0x7941, m_chara.px, 0, 1);
        } else {
            fn_801D53C4(fn_80053164(), 0x7940, m_chara.px, this, 3, 0, 2);
        }
        break;
    }
    case 2:
        break;
    }
}

void BDHologram::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    UpdatePosition();
}

void BDHologram::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_state = 0;
}

void BDHologram::UpdatePosition() {
    if (m_model.px != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        m_model->fn_801633DC(&matrix);
    }
}

} // namespace BDHologram

extern "C" void fn_803037AC(void) {
    fn_8006B474(BDHologram::BDHologram::s_fileName, 1);
}

extern "C" void fn_803037A8(void) {}

namespace BDHologram {

void BDHologram::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new BDHologram(parent, owner);
}

} // namespace BDHologram

static GadgetParamDesc lbl_8055ADE0[] = {
    {4, "distance", 0, 0, 0, s_defaultDistance, 0.0f, 65536.0f, fn_800CBC20, 0},
    {1, "voice id", 0, 0, 0x10000, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_8055AE58 = {
    "BD Hologram", fn_803037AC, fn_803037A8, BDHologram::BDHologram::Instantiate, 0, 0, 0x1903, 1, 0, 10,
    lbl_8055ADE0,
};
