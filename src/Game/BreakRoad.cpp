/* BreakRoad (class name from RTTI): the FALL_HIWAY gadget, a breakable highway section
 * (BREAKROAD.BIN) whose "Road Type" parameter selects the model variant. Hitting its attack
 * collision, or the switch it watches turning on, plays the break motion; the road then starts its
 * particles and a sound, plays its fall, and finishes the gadget once the motion is over. File name
 * inferred from the class. Method names marked "(name inferred)" are not original. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetClassDesc.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/FileQueue.h"
#include "boost/bind.hpp"
#include "boost/function.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* The motion work of a model (struct name from the RTTI of its shared_ptr control block). */
struct _stMtnPlayWork;

extern "C" {
/* GadgetModelInfo helpers (behaviour inferred): attach the character info to model variant index,
 * return the attack collision and the motion work of a variant. */
void fn_80161FC8(GadgetModelInfo *model, int index, const boost::weak_ptr<CharInfo> &info);
boost::weak_ptr<SonicteamUSA::System::CharaColli::CharaColliAttack>
fn_80161428(GadgetModelInfo *model, int index);
boost::weak_ptr<_stMtnPlayWork> fn_801602B4(GadgetModelInfo *model, int index);
void fn_8015FA78(GadgetModelInfo *model, void *(*callback)(void *part, UnkParticleAttach_80169D00 *param),
                 UnkParticleAttach_80169D00 *param);
void *fn_80169D00(void *part, UnkParticleAttach_80169D00 *param);

/* Adds a callback to an attack collision (behaviour inferred; the meaning of arg1 is unknown). */
void fn_80096B64(SonicteamUSA::System::CharaColli::CharaColliAttack *attack, int arg1,
                 const boost::function1<void, SonicteamUSA::System::CharaColli::AttackCallbackParam &>
                     &callback);

/* Writes params back to the owner's parameter block when size matches (behaviour inferred). */
void fn_800C9D68(UnkPlacement_800C9CD0 *owner, void *params, int size);
/* Tells the owner the gadget is done (behaviour inferred; a Gadget method). */
void fn_8015A794(Gadget *gadget);

/* Motion flag test of the motion library. */
int fn_80416F28(_stMtnPlayWork *work, int arg1, int arg2, int arg3);

/* Creates a particle controller (behaviour inferred). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
/* Starts the particle controller's emitters (behaviour inferred). */
void fn_80167448(GadgetParticle::GadgetParticleCtrl *ctrl, int id);

/* Plays sound id (behaviour inferred). */
void *fn_80053164(void);
void fn_801D560C(void *sound, int id, int arg2, int arg3);
}

class BreakRoad : public Gadget {
  public:
    BreakRoad(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~BreakRoad();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the road under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position, yaw, road type and switch id into the gadget (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model at the character info's translation and yaw (name inferred). */
    void RebuildTransform();
    /* The attack callback: breaks the road unless the attacker's flag bit 4 is clear (name
     * inferred). */
    void OnAttack(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);

    static const char *s_taskName;

    /* 0x30 */ s32 m_roadType; /* the "Road Type" parameter */
    /* 0x34 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x3C */ GadgetModelInfo *m_model;
    /* 0x40 */ s32 m_state; /* 0 waiting, 1 breaking, 2 falling, 3 fall motion (inferred) */
    /* 0x44 */ u8 m_switchId; /* copied from the owner; the switch it waits for (inferred) */
    /* 0x48 */ float m_timer;
    /* 0x4C */ s32 m_modelType; /* the road type the model shows (name inferred) */
    /* 0x50 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particle;
};
SIZE_ASSERT(BreakRoad, 0x58);

const char *BreakRoad::s_taskName = "BreakRoad";

BreakRoad::BreakRoad(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    fn_800C9DBC(owner, &m_roadType, sizeof(m_roadType));
    m_chara = fn_8015CB88(0x94, 4, "BreakRoad");
    m_model = new GadgetModelInfo(1, "BREAKROAD.BIN");
    m_modelType = m_roadType;
    m_model->fn_80162A68(m_modelType + 2, 0);
    fn_80161FC8(m_model, m_modelType, m_chara);

    boost::weak_ptr<SonicteamUSA::System::CharaColli::CharaColliAttack> attack =
        fn_80161428(m_model, m_modelType);
    if (boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliAttack> collision =
            attack.lock()) {
        fn_80096B64(collision.get(), 1, boost::bind(&BreakRoad::OnAttack, this, _1));
    }

    ReadPlacement(m_owner);
    RebuildTransform();
    m_state = 0;
    m_timer = 0.0f;
}

BreakRoad::~BreakRoad() {
    if (m_model != 0) {
        delete m_model;
        m_model = 0;
    }
}

void BreakRoad::Behave(float dt) {
    if (m_model != 0) {
        m_model->fn_80162FB0(dt);
    }

    switch (m_state) {
    case 0:
        if (fn_8016904C(fn_800CB69C(), m_switchId) == 1) {
            m_model->fn_80162804();
            m_model->fn_80162A68(m_modelType, 0);
            m_model->fn_80160C28(m_modelType);
            m_model->fn_80160B18(m_modelType);
            m_state = 1;
        }
        break;
    case 1: {
        m_particle = fn_80166AF4();

        UnkParticleAttach_80169D00 attach;
        attach.ctrl = m_particle;
        fn_8015FA78(m_model, fn_80169D00, &attach);
        fn_80167448(m_particle.get(), -1);
        m_state = 2;
        fn_801D560C(fn_80053164(), 0x7154, 0, 0);
        break;
    }
    case 2:
        m_timer += dt;
        if (m_timer >= 0.5f) {
            m_model->fn_80161880();
            m_state = 3;
        }
        break;
    case 3: {
        boost::weak_ptr<_stMtnPlayWork> work = fn_801602B4(m_model, m_modelType);
        if (!work.expired()) {
            _stMtnPlayWork *motion = work.lock().get();
            if (fn_80416F28(motion, 0, 3, 1) != 0) {
                fn_8015A794(this);
                m_flags |= 1;
                return;
            }
        }
        break;
    }
    }

    RebuildTransform();
}

void BreakRoad::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    RebuildTransform();
    if (m_model != 0) {
        m_model->fn_80162EF4();
        if (m_roadType != m_modelType) {
            m_model->fn_80161880();
            m_model->fn_801604FC();
            m_model->fn_80162804();
            m_modelType = m_roadType;
            m_model->fn_80162A68(m_modelType + 2, 0);
            fn_80161FC8(m_model, m_modelType, m_chara);

            boost::weak_ptr<SonicteamUSA::System::CharaColli::CharaColliAttack> attack =
                fn_80161428(m_model, m_modelType);
            if (boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliAttack> collision =
                    attack.lock()) {
                fn_80096B64(collision.get(), 1, boost::bind(&BreakRoad::OnAttack, this, _1));
            }

            m_roadType = m_modelType;
            fn_800C9D68(owner, &m_roadType, sizeof(m_roadType));
        }
        m_model->fn_8016108C();
    }
}

void BreakRoad::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    m_chara->m_rotation.x = m_chara->m_rotation.z = 0.0f;
    fn_800C9DBC(owner, &m_roadType, sizeof(m_roadType));
    fn_800C9E60(owner, &m_switchId);
}

void BreakRoad::RebuildTransform() {
    if (m_model != 0) {
        Mtx34f matrix;
        Mtx34f translation;

        SetTranslationMtx(&translation, m_chara->m_position);
        fn_8040C148(&matrix, &translation);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        m_model->fn_801633DC(&matrix);
    }
}

void BreakRoad::OnAttack(SonicteamUSA::System::CharaColli::AttackCallbackParam &param) {
    if (boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliAttack> attacker =
            param.m_attacker.lock()) {
        if ((attacker->m_flags & 0x10) == 0) {
            param.m_result = 5;
            return;
        }
    }
    param.m_result = 3;
    m_model->fn_80160B18(m_modelType);
    m_state = 2;
}

extern "C" void fn_80329F2C(void) {
    fn_8006B474("BREAKROAD.BIN", 1);
}

extern "C" void fn_80329F28(void) {}

void BreakRoad::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new BreakRoad(fn_801EB304(fn_8001C4F8(), 11), owner);
}

static GadgetParamDesc lbl_8055D008[] = {
    {1, "Road Type", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_8055D058 = {
    "FALL_HIWAY", fn_80329F2C, fn_80329F28, BreakRoad::Instantiate, 0, 0, 0x89A, 1, 0, 10, lbl_8055D008,
};
