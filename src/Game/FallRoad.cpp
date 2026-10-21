/* FallRoad (class name from RTTI): the FALL_LARGE gadget, a road slab (FALLROAD.BIN) held "Height"
 * units above the owner's position. When the switch it watches turns on, it plays its fall motion,
 * drops under gravity once the motion flags it, and on landing bounces, starts particles, shakes the
 * camera and the controller and plays a sound. File name inferred from the class. Method names
 * marked "(name inferred)" are not original. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetClassDesc.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/Vibration.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* The motion work of a model (struct name from the RTTI of its shared_ptr control block). */
struct _stMtnPlayWork;

extern "C" {
/* The collision manager (behaviour inferred): constructs a function-local static manager on first
 * use and returns it. */
void *fn_80021B08(void);
/* Creates a box collision of the given size and rotation for charInfo (behaviour inferred; the
 * meaning of the integer arguments is unknown). */
boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliBase>
fn_800C7D04(void *manager, GadgetCharInfo *charInfo, const Vec3f &size,
            const Vec3f &rotation, int arg4, int arg5, int arg6, int arg7,
            const boost::weak_ptr<CharInfo> &owner, const Vec3f &offset,
            const Vec3f &arg10, int arg11);
/* A vector constant read by the collision setup (meaning unknown). */
extern Vec3f lbl_805E07B8;

/* GadgetModelInfo helpers (behaviour inferred). */
boost::weak_ptr<_stMtnPlayWork> fn_801602B4(GadgetModelInfo *model, int index);
void fn_8015FA78(GadgetModelInfo *model, void *(*callback)(void *part, UnkParticleAttach_80169D00 *param),
                 UnkParticleAttach_80169D00 *param);
void *fn_80169D00(void *part, UnkParticleAttach_80169D00 *param);

/* Motion flag test of the motion library. */
int fn_80416F28(_stMtnPlayWork *work, int arg1, int arg2, int arg3);

/* Moves position by velocity, which gravity accelerates, over dt (behaviour inferred). */
void fn_804103E8(Vec3f *position, Vec3f *velocity, const Vec3f *gravity,
                 float arg3, float dt);

/* Creates a particle controller (behaviour inferred). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
/* Starts the particle controller's emitters (behaviour inferred). */
void fn_80167448(GadgetParticle::GadgetParticleCtrl *ctrl, int id);

/* Camera shake (behaviour inferred). */
void *fn_80009548(void);
void fn_8000D664(void *camera, int arg1, float arg2, float arg3, float arg4);
/* Plays sound id at charInfo (behaviour inferred). */
void *fn_80053164(void);
void fn_801D54F4(void *sound, int id, GadgetCharInfo *charInfo, int arg3, int arg4);
/* Starts a controller vibration near charInfo (behaviour inferred). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DD24(int type, float strength,
                                                      GadgetCharInfo *charInfo, int arg3);
}

class FallRoad : public Gadget {
  public:
    FallRoad(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~FallRoad();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the road under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position, yaw, height and switch id into the gadget (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model at m_position, turned by the yaw and lifted by m_bounce (name inferred). */
    void RebuildTransform();

    static const char *s_taskName;

    /* 0x30 */ float m_height; /* the "Height" parameter */
    /* 0x34 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x3C */ GadgetModelInfo *m_model;
    /* 0x40 */ s32 m_state; /* 0 waiting, 1 fall motion, 2 falling, 3 landed (inferred) */
    /* 0x44 */ u8 m_switchId; /* copied from the owner; the switch it waits for (inferred) */
    /* 0x48 */ Vec3f m_position;
    /* 0x54 */ Vec3f m_velocity;
    /* 0x60 */ boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> m_collision;
    /* 0x68 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particle;
    /* 0x70 */ float m_bounce;     /* names inferred */
    /* 0x74 */ float m_bouncePrev;
};
SIZE_ASSERT(FallRoad, 0x78);

const char *FallRoad::s_taskName = "FallRoad";

FallRoad::FallRoad(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    fn_800C9DBC(owner, &m_height, sizeof(m_height));
    m_chara = fn_8015CB88(0x95, 4, "FallRoad");
    m_model = new GadgetModelInfo(1, "FALLROAD.BIN");
    m_model->fn_80162A68(0, 0);
    m_model->fn_80160C28(0);
    m_model->fn_8015FF60();
    ReadPlacement(m_owner);

    void *manager = fn_80021B08();
    m_collision = fn_800C7D04(manager, m_chara.get(), Vec3f(200.0f, 25.0f, 80.0f),
                              Vec3f(0.0f, m_chara->m_rotation.y, 0.0f), 0, 1, 7, 0,
                              m_chara, Vec3f(0.0f, -25.0f, 0.0f), lbl_805E07B8, 0);

    fn_800091D0(&m_position, &m_chara->m_position);
    m_position.y += m_height;
    fn_800091D0(&m_velocity, &lbl_805E07A0);
    RebuildTransform();

    if (0.0f == m_height) {
        m_state = 3;
    } else {
        m_state = 0;
    }
    m_bounce = 0.0f;
    m_bouncePrev = 0.0f;
}

FallRoad::~FallRoad() {
    if (m_model != 0) {
        delete m_model;
        m_model = 0;
    }
}

void FallRoad::Behave(float dt) {
    if (m_model != 0) {
        m_model->fn_80162FB0(dt);
    }

    switch (m_state) {
    case 0:
        if (fn_8016904C(fn_800CB69C(), m_switchId) == 1) {
            m_model->fn_80160B18(0);
            m_state = 1;
        }
        break;
    case 1: {
        boost::weak_ptr<_stMtnPlayWork> work = fn_801602B4(m_model, 0);
        if (!work.expired()) {
            _stMtnPlayWork *motion = work.lock().get();
            if (fn_80416F28(motion, 0, 3, 1) != 0) {
                m_state = 2;
            }
        }
        break;
    }
    case 2: {
        Vec3f gravity(0.0f, -320.0f, 0.0f);

        fn_804103E8(&m_position, &m_velocity, &gravity, 1.0f, dt);
        if (m_position.y <= m_chara->m_position.y) {
            m_position.y = m_chara.px->m_position.y;
            m_state = 3;
            m_bounce = -5.0f;
            m_particle = fn_80166AF4();

            UnkParticleAttach_80169D00 attach;
            attach.ctrl = m_particle;
            fn_8015FA78(m_model, fn_80169D00, &attach);
            fn_80167448(m_particle.get(), -1);
            fn_8000D664(fn_80009548(), 0, 0.35f, 2.2f, 10.0f);
            fn_801D54F4(fn_80053164(), 0x7155, m_chara.get(), 0, 1);
            fn_8034DD24(7, 2.0f, m_chara.get(), -2);
        }
        break;
    }
    case 3: {
        float bounce = m_bounce;
        float prev = m_bouncePrev;
        m_bouncePrev = bounce;
        float velocity = bounce - prev;
        m_bounce = 0.8f * velocity + -0.2f * bounce;
        break;
    }
    }

    RebuildTransform();
}

void FallRoad::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    m_collision->SetCharInfo(m_chara.get());
    float yaw = m_chara->m_rotation.y;
    Vec3f rotation(0.0f, yaw, 0.0f);
    m_collision->SetRotation(&rotation);
    fn_800091D0(&m_position, &m_chara->m_position);
    m_position.y += m_height;
    RebuildTransform();
    if (m_model != 0) {
        m_model->fn_80162EF4();
    }
    if (m_collision.get() != 0) {
        m_collision->Reset();
    }
}

void FallRoad::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    m_chara->m_rotation.x = m_chara->m_rotation.z = 0.0f;
    fn_800C9DBC(owner, &m_height, sizeof(m_height));
    fn_800C9E60(owner, &m_switchId);
}

void FallRoad::RebuildTransform() {
    if (m_model != 0) {
        Mtx34f matrix;
        Mtx34f translation;

        SetTranslationMtx(&translation, m_position);
        fn_8040C148(&matrix, &translation);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        Vec3f offset(0.0f, m_bounce, 0.0f);
        fn_8040B59C(&matrix, &offset);
        m_model->fn_801633DC(&matrix);
    }
}

extern "C" void fn_80329730(void) {
    fn_8006B474("FALLROAD.BIN", 1);
}

extern "C" void fn_8032972C(void) {}

void FallRoad::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new FallRoad(fn_801EB304(fn_8001C4F8(), 11), owner);
}

static GadgetParamDesc lbl_8055CF38[] = {
    {4, "Height", 0, 0, 0, 150.0f, 0.0f, 1000.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_8055CF88 = {
    "FALL_LARGE", fn_80329730, fn_8032972C, FallRoad::Instantiate, 0, 0, 0x89C, 1, 0, 10, lbl_8055CF38,
};
