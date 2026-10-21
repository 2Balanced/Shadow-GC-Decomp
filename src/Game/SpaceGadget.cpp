/* SpaceGadget (class name from RTTI): the SPACEGADGET gadget, a piece of space debris shown with the
 * model of its "Model" from SPACEGADGET.BIN. It spins at "RotDegSpdX/Y/Z" degrees a second; when
 * the model has a part 1000 + Model, attacks wear its health down, and once destroyed it rewards the
 * attacker, breaks into that part's debris with a sound and finishes. File name inferred from the
 * class. Method names marked "(name inferred)" are not original. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathConstants.h"
#include "Game/FileQueue.h"
#include "boost/bind.hpp"
#include "boost/function.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;

/* The parameters of a debris spawn (layout as in CaptureCage.cpp). */
struct UnkDebrisParam_8015F0B8 {
    UnkDebrisParam_8015F0B8(const boost::shared_ptr<CharInfo> &info, int index);

    /* 0x00 */ u8 field_0x0[0x34];
    /* 0x34 */ float scale;
    /* 0x38 */ u8 field_0x38[0x40 - 0x38];
    /* 0x40 */ float field_0x40;
    /* 0x44 */ s32 model;
    /* 0x48 */ u8 field_0x48[0x4A - 0x48];
    /* 0x4A */ u8 field_0x4A;
    /* 0x4B */ u8 field_0x4B;
    /* 0x4C */ s32 field_0x4C;
    /* 0x50 */ u8 field_0x50[0x54 - 0x50];
    /* 0x54 */ u8 field_0x54;
    /* 0x55 */ u8 field_0x55[0x5C - 0x55];
};
SIZE_ASSERT(UnkDebrisParam_8015F0B8, 0x5C);

/* A part of a model looked up by id (see BreakObj.cpp; only the name is known). */
struct UnkModelPart_8015F7BC {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ const char *name; /* the part's model file */
};

extern "C" {
/* GadgetModelInfo methods that are not declared in its header yet (see BreakObj.cpp and
 * Switch.cpp). Whether the model has variant (behaviour from its body). */
bool fn_80162EA8(GadgetModelInfo *model, int variant);
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int variant);
UnkModelPart_8015F7BC *fn_8015F7BC(GadgetModelInfo *model, int id);
void fn_80162790(GadgetModelInfo *model);
/* Sets callback as the attack callback of an attack collision (see BreakRoad.cpp). */
void fn_80096B64(CharaColliAttack *attack, int arg1, const AttackCallback &callback);

/* Builds the character info of a gadget type and loads a gadget model (see GadgetUtil.h). */
boost::shared_ptr<GadgetCharInfo> fn_8015CD90(int arg0, int arg1);
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* The attacker of an attack (behaviour inferred). */
boost::shared_ptr<CharInfo> fn_8015D570(AttackCallbackParam &param);
/* Gives the attacker the reward for breaking the object (behaviour inferred). */
void fn_8020642C(const boost::shared_ptr<CharInfo> &attacker, int reward);
/* A test on the health (behaviour unknown; see FootingMovable.cpp). */
bool fn_8000A950(float value);
/* Writes the parameter block back to the owner (see BreakRoad.cpp). */
void fn_800C9D68(UnkPlacement_800C9CD0 *owner, void *params, int size);
/* Tells the owner that the gadget is gone (behaviour inferred). */
void fn_800C9AD4(UnkPlacement_800C9CD0 *owner);
/* The sound manager and its play call at a character info (see BreakObj.cpp). */
void *fn_80053164(void);
void fn_801D54F4(void *sound, int id, GadgetCharInfo *source, int arg3, int arg4);

/* Debris (see GadgetDebris.h) and the model loader (see BreakObj.cpp). */
void fn_8015D93C(UnkDebrisParam_8015F0B8 *param);
void *fn_800122D4(void);
s32 fn_80012700(void *manager, int arg1, const char *fileName);
}

/* The parameters of the owner record. */
struct SpaceGadgetParams {
    /* 0x0 */ u32 model;
    /* 0x4 */ float rotationSpeedX; /* degrees a second in the record, radians here */
    /* 0x8 */ float rotationSpeedY;
    /* 0xC */ float rotationSpeedZ;
};

class SpaceGadget : public Gadget {
  public:
    SpaceGadget(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~SpaceGadget();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's placement and parameters and resets the state (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Shows the model and registers OnAttack with its collision (name inferred). */
    void SetupModel();
    /* Places the model at the character info's place (name inferred). */
    void RebuildTransform();
    /* The attack callback of the model's collision (name inferred). */
    void OnAttack(AttackCallbackParam &param);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ SpaceGadgetParams m_params;
    /* 0x40 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x48 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x50 */ s32 m_state; /* 0 spinning, 1 destroyed */
    /* 0x54 */ float m_health;
};
SIZE_ASSERT(SpaceGadget, 0x58);

const char *SpaceGadget::s_taskName = "SpaceGadget";

SpaceGadget::SpaceGadget(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner), m_state(0) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x65, 4);
    ReadPlacement(owner);
    m_model = fn_8015F800(1, "SPACEGADGET.BIN");
    SetupModel();
}

SpaceGadget::~SpaceGadget() {}

extern "C" void fn_8026B164(void) {
    fn_8006B474("SPACEGADGET.BIN", 1);
}

extern "C" void fn_8026B160(void) {}

void SpaceGadget::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new SpaceGadget(fn_801EB304(fn_8001C4F8(), 11), owner);
}

void SpaceGadget::Behave(float dt) {
    switch (m_state) {
    case 0:
        m_chara->m_rotation.x += m_params.rotationSpeedX * dt;
        m_chara->m_rotation.y += m_params.rotationSpeedY * dt;
        m_chara->m_rotation.z += m_params.rotationSpeedZ * dt;
        RebuildTransform();
        break;
    case 1: {
        fn_80162790(m_model.get());
        m_model->fn_80161880();
        fn_801D54F4(fn_80053164(), 0x774C, m_chara.get(), 0, 1);

        UnkDebrisParam_8015F0B8 debris(m_chara, 0);
        UnkModelPart_8015F7BC *part = fn_8015F7BC(m_model.get(), m_params.model + 1000);
        debris.model = fn_80012700(fn_800122D4(), 1, part->name);
        debris.field_0x40 = m_chara->m_position.y;
        debris.scale = 1.0f;
        debris.field_0x4A = 0;
        debris.field_0x54 = 1;
        fn_8015D93C(&debris);
        fn_800C9AD4(m_owner);
        m_flags |= 1;
        break;
    }
    }
    m_model->fn_80162FB0(dt);
}

void SpaceGadget::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    SpaceGadgetParams params = m_params;

    ReadPlacement(owner);
    if (m_params.model != params.model) {
        if (fn_80162EA8(m_model.get(), m_params.model)) {
            m_model->fn_80162804();
            m_model->fn_80161880();
            SetupModel();
        } else {
            m_params.model = params.model;
            fn_800C9D68(owner, &m_params, sizeof(m_params));
        }
    }
    RebuildTransform();
    m_model->fn_80162EF4();
    m_model->fn_8016108C();
}

void SpaceGadget::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_params.rotationSpeedX *= lbl_805FAD38 / 180.0f;
    m_params.rotationSpeedY *= lbl_805FAD38 / 180.0f;
    m_params.rotationSpeedZ *= lbl_805FAD38 / 180.0f;
    m_health = 1.0f;
    m_state = 0;
}

void SpaceGadget::SetupModel() {
    m_model->fn_80162A68(m_params.model, 0);
    fn_80161FC8(m_model.get(), m_params.model, m_chara);
    if (fn_8015F7BC(m_model.get(), m_params.model + 1000) != 0) {
        boost::shared_ptr<CharaColliAttack> collision =
            fn_80161428(m_model.get(), m_params.model).lock();
        if (collision.get() != 0) {
            fn_80096B64(collision.get(), 1, boost::bind(&SpaceGadget::OnAttack, this, _1));
        }
    }
}

void SpaceGadget::RebuildTransform() {
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_chara->m_position);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    fn_8040B884(&matrix, m_chara->m_rotation.x);
    fn_8040B80C(&matrix, m_chara->m_rotation.z);
    m_model->fn_801633DC(&matrix);
}

void SpaceGadget::OnAttack(AttackCallbackParam &param) {
    param.m_result = 1;
    if (m_state != 1) {
        m_health -= param.m_damage;
        if (m_health < 0.0f || fn_8000A950(m_health)) {
            param.m_result = 3;
            boost::shared_ptr<CharInfo> attacker = fn_8015D570(param);
            if (attacker.get() != 0) {
                fn_8020642C(attacker, 0x23);
            }
            m_state = 1;
        }
    }
}

static GadgetParamDesc lbl_8054C664[] = {
    {2, "Model", 0, 0, 100, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {4, "RotDegSpdX", 0, 0, 0, 10.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "RotDegSpdY", 0, 0, 0, 10.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "RotDegSpdZ", 0, 0, 0, 10.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_8054C72C = {
    "SPACEGADGET", fn_8026B164, fn_8026B160, SpaceGadget::Instantiate, 0, 0, 0x1392, 1, 0, 10, lbl_8054C664,
};
