/* BombingWall (class name from RTTI): the BombingWall gadget, a wall (BOMBINGWALL.BIN) that a
 * bombing breaks. An attack whose flags have bit 9 set rewards the attacker and breaks it; the
 * "Out" type (0) also breaks once the nearest character comes within "Range (point)", and either
 * type breaks when its switch turns on. Breaking scatters the debris of the type's model part,
 * plays a sound, turns its switch on and finishes. File name inferred from the class. Method names
 * marked "(name inferred)" are not original. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/DebugDraw.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/FileQueue.h"
#include "boost/bind.hpp"
#include "boost/function.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;

/* Target (class name from RTTI): a point other objects can aim at (see Spring.cpp). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);
};

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

/* fn_8040CD74 called as returning a - b by value (see ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

extern "C" {
/* Copies the owner's switch id into value (behaviour inferred). */
void fn_800C9E50(UnkPlacement_800C9CD0 *owner, s32 *value);
/* Tells the owner that the gadget is gone (behaviour inferred). */
void fn_800C9B04(UnkPlacement_800C9CD0 *owner);
/* Sets switch id (behaviour inferred; see ChaoHolder.cpp). */
void fn_8016907C(UnkSwitchBank_800CB69C *manager, int id, int arg2);

/* Builds the character info of a gadget type and loads a gadget model (see GadgetUtil.h). */
boost::shared_ptr<GadgetCharInfo> fn_8015CD90(int arg0, int arg1);
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* GadgetModelInfo helpers (see BreakObj.cpp and ArkCannon.cpp). */
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int variant);
void fn_80162790(GadgetModelInfo *model);
UnkModelPart_8015F7BC *fn_8015F7BC(GadgetModelInfo *model, int id);
/* Sets callback as the attack callback of an attack collision (see BreakRoad.cpp). */
void fn_80096B64(CharaColliAttack *attack, int arg1, const AttackCallback &callback);

/* Creates a Target for owner (behaviour inferred; the flags are unknown). */
boost::shared_ptr<Target> fn_801E7128(const boost::weak_ptr<CharInfo> &owner, int flags);
/* The attacker of an attack (behaviour inferred). */
boost::shared_ptr<CharInfo> fn_8015D570(AttackCallbackParam &param);
/* Gives the attacker the reward for breaking the object (behaviour inferred). */
void fn_8020642C(const boost::shared_ptr<CharInfo> &attacker, int reward);

/* Debris (see GadgetDebris.h) and the model loader (see BreakObj.cpp). */
void fn_8015D93C(UnkDebrisParam_8015F0B8 *param);
void *fn_800122D4(void);
s32 fn_80012700(void *manager, int arg1, const char *fileName);

/* The sound manager and its positioned play call (behaviour inferred). */
void *fn_80053164(void);
void fn_801D54F4(void *manager, int id, GadgetCharInfo *source, int arg3, int arg4);
}

/* The parameters of the owner record (names from the parameter table). */
struct BombingWallParams {
    /* 0x0 */ u32 type; /* 0 out, 1 in */
    /* 0x4 */ float range;
};

class BombingWall : public Gadget {
  public:
    BombingWall(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~BombingWall();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's placement, switch id and parameters (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Shows the model and registers OnAttack with its collision (name inferred). */
    void SetupModel();
    /* Moves the model and the target to the character info's place (name inferred). */
    void UpdatePosition();
    /* The attack callback of the model's collision (name inferred). */
    void OnAttack(AttackCallbackParam &param);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ s32 m_state; /* 0 standing, 1 bombed, 2 set off */
    /* 0x34 */ BombingWallParams m_params;
    /* 0x3C */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x44 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x4C */ boost::shared_ptr<Target> m_target;
    /* 0x54 */ s32 m_switchId;
};
SIZE_ASSERT(BombingWall, 0x58);

/* The model part whose debris each type scatters. */
static const int lbl_805F99C8[] = {1000, 1001};

const char *BombingWall::s_taskName = "BombingWall";

BombingWall::BombingWall(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner), m_state(0) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x5B, 4);
    ReadPlacement(owner);
    m_model = fn_8015F800(1, "BOMBINGWALL.BIN");
    m_target = fn_801E7128(m_chara, 0xA000);
    SetupModel();
    UpdatePosition();
}

BombingWall::~BombingWall() {}

extern "C" void fn_80314EEC(void) {
    fn_8006B474("BOMBINGWALL.BIN", 1);
}

extern "C" void fn_80314EE8(void) {}

void BombingWall::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new BombingWall(fn_801EB304(fn_8001C4F8(), 11), owner);
}

void BombingWall::Behave(float dt) {
    switch (m_state) {
    case 1:
    case 2: {
        UnkDebrisParam_8015F0B8 debris(m_chara, 0);

        UnkModelPart_8015F7BC *part = fn_8015F7BC(m_model.get(), lbl_805F99C8[m_params.type]);
        debris.model = fn_80012700(fn_800122D4(), 1, part->name);
        debris.field_0x40 = m_chara->m_position.y;
        debris.scale = 1.5f;
        debris.field_0x4A = 0;
        debris.field_0x54 = 1;
        fn_8015D93C(&debris);
        fn_801D54F4(fn_80053164(), 0x764A, m_chara.get(), 0, 1);
        fn_80162790(m_model.get());
        m_model->fn_80161880();
        if (m_switchId >= 1) {
            fn_8016907C(fn_800CB69C(), m_switchId, 1);
        }
        fn_800C9B04(m_owner);
        m_flags |= 1;
        break;
    }
    case 0:
        if (m_switchId >= 1 && fn_8016904C(fn_800CB69C(), m_switchId) == 1) {
            m_state = 2;
            break;
        }
        if (m_params.type == 0) {
            boost::shared_ptr<CharInfo> enemy =
                fn_800A8A7C(fn_80074194(), &m_chara->m_position).lock();
            if (enemy.get() != 0) {
                Vec3f offset;

                fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                         &enemy->m_position, &m_chara->m_position));
                if (fn_8040CF50(&offset) < m_params.range) {
                    m_state = 2;
                    break;
                }
            }
        }
        break;
    }
    m_model->fn_80162FB0(dt);
}

void BombingWall::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    BombingWallParams params = m_params;

    ReadPlacement(owner);
    if (m_params.type != params.type) {
        m_model->fn_80162804();
        m_model->fn_80161880();
        m_state = 0;
        SetupModel();
    }
    UpdatePosition();
    m_model->fn_80162EF4();
    m_model->fn_8016108C();
    fn_802E0E84(&m_chara->m_position, &lbl_805F06D8, m_params.range);
}

void BombingWall::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9E50(owner, &m_switchId);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
}

void BombingWall::SetupModel() {
    m_model->fn_80162A68(0, 0);
    fn_80161FC8(m_model.get(), 0, m_chara);
    boost::shared_ptr<CharaColliAttack> collision = fn_80161428(m_model.get(), 0).lock();
    if (collision.get() != 0) {
        fn_80096B64(collision.get(), 1, boost::bind(&BombingWall::OnAttack, this, _1));
    }
}

void BombingWall::UpdatePosition() {
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_chara->m_position);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    fn_8040B884(&matrix, m_chara->m_rotation.x);
    fn_8040B80C(&matrix, m_chara->m_rotation.z);
    m_model->fn_801633DC(&matrix);
    m_target->fn_800CCE38(&m_chara->m_position);
}

void BombingWall::OnAttack(AttackCallbackParam &param) {
    param.m_result = 5;
    if (m_state == 0) {
        boost::shared_ptr<CharaColliAttack> collision = param.m_attacker.lock();
        if (collision.get() != 0 && (collision->m_flags & 0x200) != 0) {
            param.m_result = 3;
            boost::shared_ptr<CharInfo> attacker = fn_8015D570(param);
            if (attacker.get() != 0) {
                fn_8020642C(attacker, 0x24);
            }
            m_state = 1;
        }
    }
}

static GadgetParamDesc lbl_8055BE7C[] = {
    {2, "Type 0:Out 1:In", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {4, "Range (point)", 0, 0, 0, 50.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_8055BEF4 = {
    "BombingWall", fn_80314EEC, fn_80314EE8, BombingWall::Instantiate, 0, 0, 0x1004, 1, 0, 10, lbl_8055BE7C,
};
