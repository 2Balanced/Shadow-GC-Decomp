/* ElecRollHexa (class name from RTTI): the ELEC_ROLLHEXA gadget of the electric circuit area. It
 * shows the ROLLHEXA.BIN model turning about its vertical axis at its "RotateSpeed" (degrees a
 * second) and bobbing up and down. Its model's attack collision calls back through a boost::bind
 * into OnAttack; on the next frame it plays a sound, spawns its break effect, reports itself to its
 * owner and finishes. File name inferred from the class. Method and member names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include <cmath>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetDebris.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathConstants.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;

/* A model part found by fn_8015F7BC (only the name is used here). */
struct UnkModelPart_8015F7BC {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ const char *name; /* the part's model file */
};

/* The game state block in .bss (only the member used here). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x28];
    /* 0x028 */ float m_dt; /* the frame time */
    /* 0x02C */ u8 field_0x2C[0x150 - 0x2C];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern "C" {
extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* Returns the attack of the collision of model variant index (behaviour inferred). */
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int index);
/* Sets the attack's callback (behaviour inferred; arg1's meaning is unknown). */
void fn_80096B64(CharaColliAttack *attack, int arg1,
                 const boost::function1<void, AttackCallbackParam &> &callback);
/* Returns the part of model whose id is id, or null (behaviour inferred). */
UnkModelPart_8015F7BC *fn_8015F7BC(GadgetModelInfo *model, int id);
/* Returns the model manager, and the id of a model file in it (behaviour inferred). */
void *fn_800122D4(void);
s32 fn_80012700(void *manager, int arg1, const char *fileName);
/* Returns the sound manager and plays sound id through it (behaviour inferred). */
void *fn_80053164(void);
void fn_801D560C(void *manager, int soundId, int arg2, int arg3);
/* Tells the gadget's owner that the gadget is done (behaviour inferred). */
void fn_8015A794(Gadget *gadget);
}

class ElecRollHexa : public Gadget {
  public:
    ElecRollHexa(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~ElecRollHexa();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11. */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position, rotation and rotate speed into the gadget. */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model at the character info's translation, raised by m_bob, and yaw. */
    void RebuildTransform();
    /* Advances m_time, turns the character info and updates the bob height. */
    void UpdateMotion();
    /* The attack collision's callback: marks the gadget hit. */
    void OnAttack(AttackCallbackParam &param);
    /* Plays the break sound and spawns the break effect of the model's part 1. */
    void Shatter();

    static const char *s_taskName;

    /* 0x30 */ float m_rotateSpeed; /* the "RotateSpeed" parameter, degrees a second */
    /* 0x34 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x3C */ GadgetModelInfo *m_model;
    /* 0x40 */ float m_time;
    /* 0x44 */ float m_bob;  /* height added to the model's position */
    /* 0x48 */ s32 m_state;  /* 0 turning, 1 hit */
    /* 0x4C */ u32 pad_0x4C; /* the class size is a multiple of 8 */
};
SIZE_ASSERT(ElecRollHexa, 0x50);

const char *ElecRollHexa::s_taskName = "ElecRollHexa";

ElecRollHexa::ElecRollHexa(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    fn_800C9DBC(owner, &m_rotateSpeed, sizeof(m_rotateSpeed));
    m_chara = fn_8015CB88(0x58, 4, "ElecRollHexa");
    m_model = new GadgetModelInfo(1, "ROLLHEXA.BIN");
    m_model->fn_80162A68(0, 0);
    m_model->fn_80161F08("col", m_chara);

    boost::weak_ptr<CharaColliAttack> attack = fn_80161428(m_model, 0);
    if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
        fn_80096B64(collision.get(), 1, boost::bind(&ElecRollHexa::OnAttack, this, _1));
    }

    ReadPlacement(m_owner);
    RebuildTransform();
    m_state = 0;
    m_time = 0.0f;
}

ElecRollHexa::~ElecRollHexa() {
    if (m_model != 0) {
        delete m_model;
        m_model = 0;
    }
}

void ElecRollHexa::Behave(float dt) {
    if (m_model != 0) {
        m_model->fn_80162FB0(dt);
    }

    switch (m_state) {
    case 0:
        UpdateMotion();
        break;
    case 1:
        Shatter();
        fn_8015A794(this);
        m_flags |= 1;
        return;
    }

    RebuildTransform();
}

void ElecRollHexa::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    UpdateMotion();
    RebuildTransform();
    if (m_model != 0) {
        m_model->fn_80162EF4();
        m_model->fn_8016108C();
    }
}

void ElecRollHexa::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_rotateSpeed, sizeof(m_rotateSpeed));
}

void ElecRollHexa::RebuildTransform() {
    if (m_model != 0) {
        Mtx34f matrix;
        Mtx34f translation;
        Vec3f position;

        SetTranslationMtx(&translation,
                              *fn_80008E1C(&position, m_chara->m_position.x,
                                           m_chara->m_position.y + m_bob,
                                           m_chara->m_position.z));
        fn_8040C148(&matrix, &translation);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        m_model->fn_801633DC(&matrix);
    }
}

void ElecRollHexa::UpdateMotion() {
    m_time += lbl_8057D6A0.m_dt;
    m_chara->m_rotation.y = m_time * (m_rotateSpeed * (lbl_805FAD38 / 180.0f));
    m_bob = 2.5f * std::sin(2.0f * lbl_805FAD38 * ((m_time - 5.0f * (int)(m_time / 5.0f)) / 5.0f));
}

void ElecRollHexa::OnAttack(AttackCallbackParam &param) {
    param.m_result = 3;
    m_state = 1;
}

void ElecRollHexa::Shatter() {
    fn_801D560C(fn_80053164(), 0x7241, 0, 0);
    if (m_model != 0) {
        UnkModelPart_8015F7BC *part = fn_8015F7BC(m_model, 1);
        if (part != 0) {
            void *manager = fn_800122D4();
            UnkDebrisParam_8015F0B8 debris(m_chara, 0);

            debris.scale = 1.5f;
            debris.model = fn_80012700(manager, 1, part->name);
            debris.field_0x54 = 1;
            debris.field_0x4C = 150;
            fn_8015D93C(&debris);
        }
    }
}

extern "C" void fn_80267138(void) {
    fn_8006B474("ROLLHEXA.BIN", 1);
}

extern "C" void fn_80267134(void) {}

void ElecRollHexa::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new ElecRollHexa(fn_801EB304(fn_8001C4F8(), 11), owner);
}

static GadgetParamDesc lbl_8054C25C[] = {
    {4, "RotateSpeed", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_8054C2AC = {
    "ELEC_ROLLHEXA", fn_80267138, fn_80267134, ElecRollHexa::Instantiate, 0, 0, 0x7E8, 1, 0, 10,
    lbl_8054C25C,
};
