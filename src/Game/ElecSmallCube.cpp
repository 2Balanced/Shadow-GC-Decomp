/* ElecSmallCube (class name from RTTI): the ELEC_SMALLCUBE gadget of the electric circuit area. It
 * shows the SMALLROLLCUBE.BIN model and keeps turning it towards new random rotations, like the
 * larger ELEC_CUBE gadget. Its model's attack collision calls back through a boost::bind into
 * OnAttack; on the next frame the cube spawns its break effect, reports itself to its owner and
 * finishes. File name inferred from the class. Method names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathConstants.h"
#include "Game/FileQueue.h"
#include "Game/Random.h"
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

/* The parameters of a debris spawn (layout from its constructor at 0x8015F0B8 and the gadgets that
 * fill it; names inferred). */
struct UnkDebrisParam_8015F0B8 {
    /* Sets up the parameters at a character info's place from template index (behaviour
     * inferred). */
    UnkDebrisParam_8015F0B8(const boost::shared_ptr<CharInfo> &info, int index);

    /* 0x00 */ u8 field_0x0[0x34];
    /* 0x34 */ float scale;
    /* 0x38 */ u8 field_0x38[0x44 - 0x38];
    /* 0x44 */ s32 model;
    /* 0x48 */ u32 field_0x48;
    /* 0x4C */ s32 field_0x4C;
    /* 0x50 */ u32 field_0x50;
    /* 0x54 */ u8 field_0x54;
    /* 0x58 */ s32 field_0x58;
};
SIZE_ASSERT(UnkDebrisParam_8015F0B8, 0x5C);

extern "C" {
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
/* Spawns debris (behaviour inferred). */
void fn_8015D93C(UnkDebrisParam_8015F0B8 *param);
/* Tells the gadget's owner that the gadget is done (behaviour inferred). */
void fn_8015A794(Gadget *gadget);
}

class ElecSmallCube : public Gadget {
  public:
    ElecSmallCube(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~ElecSmallCube();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the cube under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position and rotation into the gadget (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model at the character info's translation and rotation (name inferred). */
    void RebuildTransform();
    /* Moves the character info's rotation from m_spinStart towards m_spinGoal and picks a
     * new target when the turn is over (name inferred). */
    void AdvanceSpin(float dt);
    /* Picks a random new target rotation near m_spinStart, turned further about one random axis
     * (name inferred). */
    void ChooseSpinTarget();
    /* The attack collision's callback: marks the cube hit (name inferred). */
    void OnAttack(AttackCallbackParam &param);
    /* Spawns the break effect of the model's part 1 (name inferred). */
    void Shatter();
    /* An unused out-of-line copy of ChooseSpinTarget (name inferred; see its definition). */
    void ChooseSpinTargetOutOfLine();

    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ GadgetModelInfo *m_model;
    /* 0x3C */ s32 m_state;                  /* 0 turning, 1 hit (inferred) */
    /* 0x40 */ float m_timer;                /* time into the current turn (inferred) */
    /* 0x44 */ Vec3f m_spinStart; /* names inferred */
    /* 0x50 */ Vec3f m_spinGoal;
    /* 0x5C */ u32 pad_0x5C; /* the class size is a multiple of 8 */
};
SIZE_ASSERT(ElecSmallCube, 0x60);

const char *ElecSmallCube::s_taskName = "ElecSmallCube";

inline void ElecSmallCube::ChooseSpinTarget() {
    m_spinGoal.x = fn_8040EC80(0.1f * -lbl_805FAD38, 0.1f * lbl_805FAD38);
    m_spinGoal.y = fn_8040EC80(0.1f * -lbl_805FAD38, 0.1f * lbl_805FAD38);
    m_spinGoal.z = fn_8040EC80(0.1f * -lbl_805FAD38, 0.1f * lbl_805FAD38);
    switch (fn_8040EA90(3)) {
    case 0:
        m_spinGoal.x +=
            m_spinGoal.x < 0.0f ? 0.66667f * -lbl_805FAD38 : 0.66667f * lbl_805FAD38;
        break;
    case 1:
        m_spinGoal.y +=
            m_spinGoal.y < 0.0f ? 0.66667f * -lbl_805FAD38 : 0.66667f * lbl_805FAD38;
        break;
    case 2:
        m_spinGoal.z +=
            m_spinGoal.z < 0.0f ? 0.66667f * -lbl_805FAD38 : 0.66667f * lbl_805FAD38;
        break;
    }
    m_spinGoal.x += m_spinStart.x;
    m_spinGoal.y += m_spinStart.y;
    m_spinGoal.z += m_spinStart.z;
}

ElecSmallCube::ElecSmallCube(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner) {
    m_spinStart.x = 0.0f;
    m_spinStart.y = 0.0f;
    m_spinStart.z = 0.0f;
    m_spinGoal.x = 0.0f;
    m_spinGoal.y = 0.0f;
    m_spinGoal.z = 0.0f;
    m_name = s_taskName;
    m_chara = fn_8015CB88(0x58, 4, "ElecSmallCube");
    m_model = new GadgetModelInfo(1, "SMALLROLLCUBE.BIN");
    m_model->fn_80162A68(0, 0);
    m_model->fn_80161F08("col", m_chara);

    boost::weak_ptr<CharaColliAttack> attack = fn_80161428(m_model, 0);
    if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
        fn_80096B64(collision.get(), 1, boost::bind(&ElecSmallCube::OnAttack, this, _1));
    }

    ReadPlacement(m_owner);
    switch (fn_8040EA90(3)) {
    case 0:
        m_chara->field_0x30 = 0;
        break;
    case 1:
        m_chara->field_0x30 = 1;
        break;
    case 2:
        m_chara->field_0x30 = 2;
        break;
    }

    m_state = 0;
    m_timer = 0.0f;
    m_spinStart.x = fn_8040EC80(-lbl_805FAD38, lbl_805FAD38);
    m_spinStart.y = fn_8040EC80(-lbl_805FAD38, lbl_805FAD38);
    m_spinStart.z = fn_8040EC80(-lbl_805FAD38, lbl_805FAD38);

    ChooseSpinTarget();
    RebuildTransform();
}

ElecSmallCube::~ElecSmallCube() {
    if (m_model != 0) {
        delete m_model;
        m_model = 0;
    }
}

void ElecSmallCube::Behave(float dt) {
    if (m_model != 0) {
        m_model->fn_80162FB0(dt);
    }

    switch (m_state) {
    case 0:
        AdvanceSpin(dt);
        break;
    case 1:
        Shatter();
        fn_8015A794(this);
        m_flags |= 1;
        return;
    }

    RebuildTransform();
}

void ElecSmallCube::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    RebuildTransform();
    if (m_model != 0) {
        m_model->fn_80162EF4();
        m_model->fn_8016108C();
    }
}

void ElecSmallCube::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
}

void ElecSmallCube::RebuildTransform() {
    if (m_model != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8001E070(&matrix, &m_chara->m_rotation, m_chara->field_0x30);
        m_model->fn_801633DC(&matrix);
    }
}

void ElecSmallCube::AdvanceSpin(float dt) {
    float t;

    m_timer += dt;
    if (m_timer < 0.25f) {
        t = 0.75f * (m_timer / 0.25f);
    } else {
        t = 0.25f * ((m_timer - 0.25f) / 0.85f) + 0.75f;
        if (t > 1.0f) {
            t = 1.0f;
        }
    }

    /* Retail divides each angle by t + (1 - t), which is always 1. */
    float diffX = m_spinGoal.x - m_spinStart.x;
    m_chara->m_rotation.x = m_spinStart.x + diffX * t / (t + (1.0f - t));
    float diffY = m_spinGoal.y - m_spinStart.y;
    m_chara->m_rotation.y = m_spinStart.y + diffY * t / (t + (1.0f - t));
    float diffZ = m_spinGoal.z - m_spinStart.z;
    m_chara->m_rotation.z = m_spinStart.z + diffZ * t / (t + (1.0f - t));

    if (m_timer > 1.1f) {
        m_timer = 0.0f;
        m_spinStart.x = m_spinGoal.x;
        m_spinStart.y = m_spinGoal.y;
        m_spinStart.z = m_spinGoal.z;

        ChooseSpinTarget();
    }
}

/* Not called anywhere: the linker strips it from the game. The original pools ChooseSpinTarget's
 * float literals between those of Shatter and AdvanceSpin, which is what compiling an
 * out-of-line function that inlines it at this point of the file does (inferred from the order
 * of the literals). */
void ElecSmallCube::ChooseSpinTargetOutOfLine() {
    ChooseSpinTarget();
}

void ElecSmallCube::OnAttack(AttackCallbackParam &param) {
    param.m_result = 3;
    m_state = 1;
}

void ElecSmallCube::Shatter() {
    if (m_model != 0) {
        UnkModelPart_8015F7BC *part = fn_8015F7BC(m_model, 1);
        if (part != 0) {
            void *manager = fn_800122D4();
            UnkDebrisParam_8015F0B8 debris(m_chara, 0);

            debris.scale = 1.5f;
            debris.model = fn_80012700(manager, 1, part->name);
            debris.field_0x54 = 0;
            debris.field_0x4C = 150;
            fn_8015D93C(&debris);
        }
    }
}

extern "C" void fn_8021AD94(void) {
    fn_8006B474("SMALLROLLCUBE.BIN", 1);
}

extern "C" void fn_8021AD90(void) {}

void ElecSmallCube::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new ElecSmallCube(fn_801EB304(fn_8001C4F8(), 11), owner);
}

GadgetClassDesc lbl_8054588C = {
    "ELEC_SMALLCUBE", fn_8021AD94, fn_8021AD90, ElecSmallCube::Instantiate, 0, 0, 0x7E4, 1, 0, 10, 0,
};
