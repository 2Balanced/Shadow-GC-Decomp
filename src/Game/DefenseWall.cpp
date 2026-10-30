/* DefenseWall::SetDefenseWall (class name from RTTI): the DEFENSE WALL gadget, a concrete door
 * (CONCRETEDOOR.BIN) sunk "width"/"height"/"depth" into the ground. When a character touches its
 * box collider it rises at "speed" until it stands at full height; attacks wear its four hit points
 * down, and once destroyed it rewards the attacker, breaks into its debris and finishes. File name
 * inferred from the namespace. Names marked "(name inferred)" are not original. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharCollision.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetUtil.h"
#include "Game/FileQueue.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

namespace GadgetBuilder {
/* GadgetBuilder::CrashInfoCtrl (class name from RTTI; Game/GadgetBuilder.cpp): the entry of a
 * model file's crash table. */
class CrashInfoCtrl;
} // namespace GadgetBuilder

/* The table a model file keeps in the loader (struct name unknown; see Game/BreakWall.cpp). */
struct UnkModelTable_80208888;

/* The debris entry of a crash table entry (struct name unknown; layout from Game/BreakWall.cpp). */
struct UnkDebrisEntry_802086C0 {
    /* 0x00 */ u32 field_0x0;
    /* 0x04 */ const char *fileName; /* the debris model file */
    /* 0x08 */ u8 field_0x8[0x18 - 0x8];
    /* 0x18 */ Vec3f offset;
};

/* The parameters of a debris spawn (layout from its constructor at 0x8015F0B8 and its users, see
 * Game/BreakWall.cpp and Game/ArkCannon.cpp; names inferred). */
struct UnkDebrisParam_8015F0B8 {
    UnkDebrisParam_8015F0B8(const boost::shared_ptr<CharInfo> &info, int index);

    /* 0x00 */ u32 field_0x0;
    /* 0x04 */ Vec3f position;
    /* 0x10 */ u8 field_0x10[0x34 - 0x10];
    /* 0x34 */ float scale;
    /* 0x38 */ u8 field_0x38[0x40 - 0x38];
    /* 0x40 */ float field_0x40; /* the height the pieces start from (meaning inferred) */
    /* 0x44 */ s32 model;
    /* 0x48 */ u8 field_0x48[0x4A - 0x48];
    /* 0x4A */ u8 field_0x4A;
    /* 0x4B */ u8 field_0x4B;
    /* 0x4C */ s32 count; /* (meaning inferred) */
    /* 0x50 */ u32 field_0x50;
    /* 0x54 */ u8 field_0x54;
    /* 0x55 */ u8 field_0x55[0x5C - 0x55];
};
SIZE_ASSERT(UnkDebrisParam_8015F0B8, 0x5C);

/* The character info shared by attacks without an owner (see Game/ContainerBehavior.cpp). */
extern boost::shared_ptr<CharInfo> lbl_805EE698;
/* A vector constant read by the collision setup (meaning unknown). */
extern Vec3f lbl_805E07B8;

extern "C" {
/* The file loader and the table file fileName keeps in it (see Game/BreakWall.cpp). */
void *fn_800122D4(void);
UnkModelTable_80208888 *fn_80012700(void *loader, int arg1, const char *fileName);
/* The crash table entry of model, and its debris (see Game/BreakWall.cpp). */
boost::shared_ptr<GadgetBuilder::CrashInfoCtrl> fn_80208978(UnkModelTable_80208888 *table, int model);
UnkDebrisEntry_802086C0 *fn_802086C0(GadgetBuilder::CrashInfoCtrl *entry);
/* Launches the debris (behaviour inferred). */
void fn_8015D93C(UnkDebrisParam_8015F0B8 *param);

/* GadgetModelInfo helpers (see Game/BASwitch.cpp and Game/ArkCannon.cpp). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int variant);
/* Shakes the model for time (behaviour inferred). */
void fn_8016268C(GadgetModelInfo *model, float time);
/* The matrix of a character info's position and rotation (see Game/BKTank.cpp). */
void fn_8015CEA0(GadgetCharInfo *charInfo, Mtx34f *matrix);

/* The collision manager, a box collider at position, and the handle a collider is wrapped in,
 * re-registered and advanced by dt (see Game/ContainerBehavior.cpp and Game/BaseDoor.cpp). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase>
fn_800C7D04(void *manager, const Vec3f &position, const Vec3f &size, const Vec3f &rotation,
            int arg4, int arg5, int arg6, int arg7, const boost::weak_ptr<CharInfo> &owner,
            const Vec3f &offset, const Vec3f &arg10, int arg11);
boost::shared_ptr<GadgetCharCollision> fn_8015B8C4(boost::shared_ptr<CharaColliBase> collider);
void fn_8015BE64(GadgetCharCollision *handle);
void fn_8015BE9C(GadgetCharCollision *handle, float dt);

/* Plays sound id at a character info, once and as a loop for source (see Game/BaseDoor.cpp). */
void fn_801D54F4(void *manager, int id, GadgetCharInfo *source, int arg3, int arg4);
void fn_801D53C4(void *manager, int id, GadgetCharInfo *charInfo, void *source, int arg4, int arg5,
                 int arg6);
/* Gives the attacker the reward for breaking the object (see Game/BreakWall.cpp). */
void fn_8020642C(const boost::shared_ptr<CharInfo> &attacker, int reward);
/* Marks the gadget finished (see Game/BreakWall.cpp). */
void fn_8015A794(Gadget *gadget);
}

/* How far the door is sunk when placed (the game sets it in __sinit; the inline call reproduces
 * that, as in Game/CameraModeFixLook2.cpp). */
inline float DynamicFloat(float value) {
    return value;
}
static const float s_sunkDepth = DynamicFloat(80.0f);

namespace DefenseWall {

/* The parameters of the owner record (names from the parameter table). */
struct SetDefenseWallParams {
    SetDefenseWallParams() : width(40.0f), height(40.0f), depth(40.0f), speed(4.0f) {}

    /* 0x0 */ float width;
    /* 0x4 */ float height;
    /* 0x8 */ float depth;
    /* 0xC */ float speed;
};

class SetDefenseWall : public Gadget {
  public:
    SetDefenseWall(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~SetDefenseWall();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Method names are inferred. */
    /* Copies the owner's position, rotation and parameters and sinks the door. */
    void LoadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model at the character info. */
    void RebuildTransform();
    /* The attack collider's callback: takes the damage. */
    void OnAttack(AttackCallbackParam &param);
    /* Builds the box collider and its handle. */
    void CreateCollision();
    /* Breaks the door into its debris. */
    void Break();
    /* The box collider's callback: starts rising. */
    void OnColliDetect(ColliDetectArgs &args);

    /* Whether attacks can hit the door. */
    bool CanBeHit() {
        return m_state == 1 || m_state == 2 || m_state == 3;
    }

    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::shared_ptr<GadgetCharCollision> m_collision;
    /* 0x48 */ SetDefenseWallParams m_params;
    /* 0x58 */ s32 m_state; /* 0 placed, 1 sunk, 2 rising, 3 up, 4 destroyed, 5 done */
    /* 0x5C */ float m_hitPoints;
    /* 0x60 */ float m_hitTimer; /* attacks are ignored while it runs */
    /* 0x64 */ Vec3f m_position;
    /* 0x70 */ float m_depth; /* how far the door is still sunk */
    /* 0x74 */ Vec3f m_colliderPosition;
    /* 0x80 */ Vec3f m_colliderRotation;
    /* 0x8C */ u32 field_0x8C;
};
SIZE_ASSERT(SetDefenseWall, 0x90);

const char *SetDefenseWall::s_taskName = "SetDefenseWall";

SetDefenseWall::SetDefenseWall(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner), m_colliderRotation(0.0f, 0.0f, 0.0f) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x7E, 4);
    LoadPlacement(owner);
    m_model = fn_8015F800(1, "CONCRETEDOOR.BIN");
    m_model->fn_80162A68(0, 0);
    fn_80161FC8(m_model.px, 0, boost::weak_ptr<CharInfo>(m_chara));
    boost::weak_ptr<CharaColliAttack> attack = fn_80161428(m_model.px, 0);
    if (boost::shared_ptr<CharaColliAttack> target = attack.lock()) {
        target->field_0x54 = 0;
        target->field_0x55 = 1;
        target->m_attackCallback = AttackCallback(boost::bind(&SetDefenseWall::OnAttack, this, _1));
    }
    RebuildTransform();
    CreateCollision();
}

SetDefenseWall::~SetDefenseWall() {}

void SetDefenseWall::Behave(float dt) {
    if (m_model.px != 0) {
        m_model->fn_80162FB0(dt);
    }
    if (m_collision.px != 0) {
        fn_8015BE9C(m_collision.px, dt);
    }
    m_hitTimer -= dt;
    if (m_hitTimer <= 0.0f) {
        m_hitTimer = 0.0f;
    }
    switch (m_state) {
    case 0:
        m_state = 1;
        break;
    case 1:
        break;
    case 2:
        m_depth -= m_params.speed * dt;
        if (m_depth <= 0.0f) {
            m_depth = 0.0f;
        }
        fn_800091D0(&m_chara->m_position, &m_position);
        m_chara->m_position.y += m_depth;
        if (m_depth <= 0.0f) {
            m_state = 3;
            fn_801D54F4(fn_80053164(), 0x7847, m_chara.px, 0, 1);
        } else {
            fn_801D53C4(fn_80053164(), 0x7849, m_chara.px, this, 3, 0, 2);
        }
        break;
    case 3:
        break;
    case 4:
        fn_801D54F4(fn_80053164(), 0x7848, m_chara.px, 0, 1);
        Break();
        fn_8015A794(this);
        m_state = 5;
        break;
    case 5:
        m_flags |= 1;
        break;
    }
    RebuildTransform();
}

void SetDefenseWall::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadPlacement(owner);
    RebuildTransform();
    if (m_collision.px != 0) {
        m_collision->m_collider->SetCharInfo(reinterpret_cast<CharInfo *>(&m_colliderPosition));
        m_collision->m_collider->SetRotation(&m_colliderRotation);
        m_collision->m_collider->SetSize(m_params.width, m_params.height, m_params.depth, -2);
        fn_8015BE64(m_collision.px);
    }
}

void SetDefenseWall::LoadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_state = 0;
    m_hitPoints = 4.0f;
    m_hitTimer = 0.0f;
    m_depth = s_sunkDepth;
    fn_800091D0(&m_chara->m_position, &m_position);
    m_chara->m_position.y += m_depth;
    fn_800091D0(&m_colliderPosition, &m_position);
    m_colliderRotation = m_chara->m_rotation;
}

void SetDefenseWall::RebuildTransform() {
    if (m_model.px != 0) {
        Mtx34f matrix;
        fn_8015CEA0(m_chara.px, &matrix);
        m_model->fn_801633DC(&matrix);
    }
}

void SetDefenseWall::OnAttack(AttackCallbackParam &param) {
    param.m_result = 5;
    if (m_hitTimer > 0.0f) {
        return;
    }
    if (!CanBeHit()) {
        return;
    }
    if (boost::shared_ptr<CharaColliAttack> attack = param.m_attacker.lock()) {
        m_hitPoints -= param.m_damage;
        if (m_hitPoints <= 0.0f) {
            if (boost::shared_ptr<CharInfo> attacker = attack->m_owner.lock()) {
                fn_8020642C(attacker, 0);
            }
            param.m_result = 3;
            m_state = 4;
        } else if (param.m_damage > 0.9f) {
            fn_8016268C(m_model.px, 0.6f);
            m_hitTimer = 0.2f;
            param.m_result = 1;
        } else {
            param.m_result = 5;
        }
    }
}

void SetDefenseWall::CreateCollision() {
    Vec3f size(m_params.width, m_params.height, m_params.depth);
    boost::shared_ptr<CharaColliBase> collider =
        fn_800C7D04(fn_80021B08(), m_colliderPosition, size, m_colliderRotation, 2, 0x40100, 10, 0,
                    lbl_805EE698, lbl_805E07A0, lbl_805E07B8, 0);
    m_collision = fn_8015B8C4(collider);
    fn_800F5980(m_collision.px, boost::bind(&SetDefenseWall::OnColliDetect, this, _1));
}

void SetDefenseWall::Break() {
    void *loader = fn_800122D4();
    UnkModelTable_80208888 *table = fn_80012700(loader, 1, "CONCRETEDOOR.BIN");
    boost::shared_ptr<GadgetBuilder::CrashInfoCtrl> entry = fn_80208978(table, 0);
    if (entry.px != 0) {
        UnkDebrisEntry_802086C0 *debris = fn_802086C0(entry.px);
        if (debris != 0) {
            s32 model = (s32)fn_80012700(loader, 1, debris->fileName);
            if (model != 0) {
                UnkDebrisParam_8015F0B8 param(m_chara, 0);
                param.position.x += debris->offset.x;
                param.position.y += debris->offset.y;
                param.position.z += debris->offset.z;
                param.scale = 1.3f;
                param.model = model;
                param.field_0x54 = 1;
                param.count = 150;
                param.field_0x40 = m_chara->m_position.y;
                param.field_0x4A = 1;
                fn_8015D93C(&param);
            }
        }
    }
}

void SetDefenseWall::OnColliDetect(ColliDetectArgs &args) {
    if (m_state == 1) {
        m_state = 2;
    }
}

} // namespace DefenseWall

extern "C" void fn_80229764(void) {
    fn_8006B474("CONCRETEDOOR.BIN", 1);
}

extern "C" void fn_80229760(void) {}

namespace DefenseWall {

void SetDefenseWall::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new SetDefenseWall(parent, owner);
}

} // namespace DefenseWall

static GadgetParamDesc lbl_80546960[] = {
    {4, "width", 0, 0, 0, 40.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "height", 0, 0, 0, 40.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "depth", 0, 0, 0, 40.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "speed", 0, 0, 0, 4.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80546A28 = {
    "DEFENSE WALL", fn_80229764, fn_80229760, DefenseWall::SetDefenseWall::Instantiate, 0, 0, 0x1772,
    1, 0, 10, lbl_80546960,
};
