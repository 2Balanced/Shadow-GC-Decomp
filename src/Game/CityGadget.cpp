/* CityGadget (class name from RTTI): the CITYGADGET gadget, a breakable city prop
 * (CITYGADGET.BIN) of one of five types, each with its own model variant, debris, sound, the kinds
 * of attack that hurt it and its health. Attacks wear the health down; once it is gone the
 * attacker is rewarded and the prop breaks into its debris: types with a broken model variant
 * switch to it, the others finish. Its state lives in a CityGadgetState (class name from RTTI)
 * kept by the owner record, so it survives the gadget being recreated. File name inferred from the
 * class. Method names marked "(name inferred)" are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/SetAdapterState.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

/* The parameters of a debris spawn (layout as in ArkCannon.cpp). */
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

/* A part of a model looked up by id (see BreakObj.cpp). */
struct UnkModelPart_8015F7BC {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ const char *name; /* the part's model file */
};

extern "C" {
/* Copies the owner's flag word into flags (behaviour inferred). */
void fn_800C9E70(UnkPlacement_800C9CD0 *owner, u32 *flags);
/* Tells the owner that the gadget is gone (behaviour inferred). */
void fn_800C9B04(UnkPlacement_800C9CD0 *owner);

/* GadgetModelInfo helpers (see BreakObj.cpp and ArkCannon.cpp). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int variant);
boost::weak_ptr<CharaColliBase> fn_80161614(GadgetModelInfo *model, int variant);
void fn_80162790(GadgetModelInfo *model);
UnkModelPart_8015F7BC *fn_8015F7BC(GadgetModelInfo *model, int id);
/* Sets callback as the attack callback of an attack collision (see BreakRoad.cpp). */
void fn_80096B64(CharaColliAttack *attack, int arg1, const AttackCallback &callback);

/* The attacker of an attack (behaviour inferred). */
boost::shared_ptr<CharInfo> fn_8015D570(AttackCallbackParam &param);
/* Sends message to the character (behaviour and meaning of message inferred). */
int fn_8020642C(const boost::shared_ptr<CharInfo> &character, int message);
/* A test on a timer or the health (behaviour unknown; see FootingMovable.cpp). */
bool fn_8000A950(float value);

/* Plays sound id at a character info through the sound manager (fn_80053164, GadgetUtil.h;
 * behaviour inferred). */
void fn_801D54F4(void *manager, int id, GadgetCharInfo *source, int arg3, int arg4);

/* Debris (see GadgetDebris.h) and the model loader (see BreakObj.cpp). */
void fn_8015D93C(UnkDebrisParam_8015F0B8 *param);
void *fn_800122D4(void);
s32 fn_80012700(void *manager, int arg1, const char *fileName);

/* Scales the matrix's three rotation columns by the vector's components (see Cage.cpp). */
void fn_8040B790(Mtx34f *matrix, const Vec3f *scale);
}

/* One of the five types (field names inferred). */
struct CityGadgetType {
    /* 0x00 */ s32 variant;       /* the whole model variant */
    /* 0x04 */ s32 debrisPart;    /* the model part the debris is made of */
    /* 0x08 */ s32 brokenVariant; /* the broken model variant, -1 for none */
    /* 0x0C */ u32 sound;         /* played when it breaks */
    /* 0x10 */ u32 attackMask;    /* the attack collisions' flags that hurt it */
    /* 0x14 */ float health;
};
SIZE_ASSERT(CityGadgetType, 0x18);

static const CityGadgetType s_types[] = {
    {0, 1000, -1, 0x7135, 0x200, 1.0f},  {0, 1000, 2000, 0x7135, 0x200, 1.0f},
    {1, 1001, -1, 0x7136, 0x200, 1.0f},  {2, 1002, -1, 0x7138, 0xBF0, 1.0f},
    {3, 1003, -1, 0x713A, 0xBF0, 1.0f},
};

/* CityGadgetState (class name from RTTI): the prop's state that the owner record keeps: 0 whole,
 * 1 hit to death, 2 broken (member name and meanings inferred). */
class CityGadgetState : public SetAdapterState {
  public:
    CityGadgetState() : m_state(0) {}

    /* 0x4 */ s32 m_state;
};
SIZE_ASSERT(CityGadgetState, 0x8);

/* The four parameters the owner supplies (names from the parameter table). */
struct CityGadgetParams {
    /* 0x0 */ u32 type;
    /* 0x4 */ float scaleX;
    /* 0x8 */ float scaleY;
    /* 0xC */ float scaleZ;
};

class CityGadget : public Gadget {
  public:
    CityGadget(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~CityGadget();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's placement and parameters and looks up the type (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Shows the model variant of the state and sets up its collision (name inferred). */
    void SetModel();
    /* Places and scales the model (name inferred). */
    void RebuildTransform();
    /* Scales the model's collider (name inferred). */
    void UpdateCollider();
    /* The attack collision's callback (name inferred). */
    void OnAttack(AttackCallbackParam &param);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ const CityGadgetType *m_type;
    /* 0x34 */ CityGadgetState *m_setState;
    /* 0x38 */ CityGadgetParams m_params;
    /* 0x48 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x50 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x58 */ float m_health;
    /* 0x5C */ u8 m_debrisFlag; /* bit 22 of the owner's flags */
};
SIZE_ASSERT(CityGadget, 0x60);

const char *CityGadget::s_taskName = "CityGadget";

CityGadget::CityGadget(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner), m_type(0), m_setState(0), m_health(0.0f) {
    m_name = s_taskName;
    if (owner->m_adapterState == 0) {
        fn_800C9930(owner, new CityGadgetState);
    }
    m_setState = static_cast<CityGadgetState *>(owner->m_adapterState);
    m_chara = fn_8015CD90(0x52, 4);
    ReadPlacement(owner);
    m_model = fn_8015F800(1, "CITYGADGET.BIN");
    SetModel();
    RebuildTransform();
    UpdateCollider();
}

CityGadget::~CityGadget() {}

extern "C" void fn_802F6F88(void) {
    fn_8006B474("CITYGADGET.BIN", 1);
}

extern "C" void fn_802F6F84(void) {}

void CityGadget::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new CityGadget(fn_801EB304(fn_8001C4F8(), 11), owner);
}

void CityGadget::Behave(float dt) {
    switch (m_setState->m_state) {
    case 1: {
        {
            UnkDebrisParam_8015F0B8 debris(m_chara, 0);
            UnkModelPart_8015F7BC *part = fn_8015F7BC(m_model.get(), m_type->debrisPart);

            debris.model = fn_80012700(fn_800122D4(), 1, part->name);
            debris.field_0x40 = m_chara->m_position.y;
            debris.scale = 1.0f;
            debris.field_0x4A = m_debrisFlag;
            debris.field_0x54 = 1;
            fn_8015D93C(&debris);
        }
        fn_801D54F4(fn_80053164(), (u16)m_type->sound, m_chara.get(), 0, 1);
        if (m_type->brokenVariant == -1) {
            fn_80162790(m_model.get());
            m_model->fn_80161880();
            fn_800C9B04(m_owner);
            m_flags |= 1;
        } else {
            m_model->fn_80162804();
            m_model->fn_80161880();
            m_model->fn_80162A68(m_type->brokenVariant, 0);
            fn_80161FC8(m_model.get(), m_type->brokenVariant, m_chara);
            m_setState->m_state = 2;
            RebuildTransform();
            UpdateCollider();
        }
        break;
    }
    }
    m_model->fn_80162FB0(dt);
}

void CityGadget::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    CityGadgetParams params = m_params;

    ReadPlacement(owner);
    if (m_params.type != params.type) {
        m_model->fn_80162804();
        m_model->fn_80161880();
        m_setState->m_state = 0;
        SetModel();
    }
    RebuildTransform();
    m_model->fn_80162EF4();
    m_model->fn_8016108C();
}

void CityGadget::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    u32 flags;
    fn_800C9E70(owner, &flags);
    m_debrisFlag = (flags >> 22) & 1;
    m_type = &s_types[m_params.type];
    m_health = m_type->health;
}

void CityGadget::SetModel() {
    switch (m_setState->m_state) {
    case 0:
        m_model->fn_80162A68(m_type->variant, 0);
        fn_80161FC8(m_model.get(), m_type->variant, m_chara);
        {
            boost::shared_ptr<CharaColliAttack> collision =
                fn_80161428(m_model.get(), m_type->variant).lock();
            if (collision.get() != 0) {
                fn_80096B64(collision.get(), 1, boost::bind(&CityGadget::OnAttack, this, _1));
            }
        }
        break;
    case 2:
        m_model->fn_80162A68(m_type->brokenVariant, 0);
        fn_80161FC8(m_model.get(), m_type->brokenVariant, m_chara);
        break;
    }
}

void CityGadget::RebuildTransform() {
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_chara->m_position);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    fn_8040B884(&matrix, m_chara->m_rotation.x);
    fn_8040B80C(&matrix, m_chara->m_rotation.z);
    Vec3f scale(m_params.scaleX, m_params.scaleY, m_params.scaleZ);
    fn_8040B790(&matrix, &scale);
    m_model->fn_801633DC(&matrix);
}

void CityGadget::UpdateCollider() {
    boost::shared_ptr<CharaColliBase> collider =
        fn_80161614(m_model.get(), m_setState->m_state == 0 ? m_type->variant
                                                              : m_type->brokenVariant)
            .lock();
    if (collider.get() != 0) {
        Vec3f scale(m_params.scaleX, m_params.scaleY, m_params.scaleZ);
        collider->SetScale(&scale);
    }
}

void CityGadget::OnAttack(AttackCallbackParam &param) {
    param.m_result = 5;
    if (m_setState->m_state == 0) {
        boost::shared_ptr<CharaColliAttack> attack = param.m_attacker.lock();
        if (attack.get() != 0 && (m_type->attackMask & attack->m_flags) != 0) {
            param.m_result = 1;
            m_health -= param.m_damage;
            AssignVector(*reinterpret_cast<Vec3f *>(param.field_0x8 + (0x40 - 0x8)),
                         *reinterpret_cast<Vec3f *>(param.field_0x8 + (0x24 - 0x8)));
            if (m_health < 0.0f || fn_8000A950(m_health)) {
                param.m_result = 3;
                {
                    boost::shared_ptr<CharInfo> attacker = fn_8015D570(param);
                    if (attacker.get() != 0) {
                        fn_8020642C(attacker, 0x23);
                    }
                    m_setState->m_state = 1;
                }
            }
        }
    }
}

static GadgetParamDesc lbl_8055A24C[] = {
    {2, "Type", 0, 0, 4, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {4, "ScaleX", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "ScaleY", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "ScaleZ", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_8055A314 = {
    "CITYGADGET", fn_802F6F88, fn_802F6F84, CityGadget::Instantiate, 0, 0, 0xFA4, 1, 0, 10,
    lbl_8055A24C,
};
