/* GunBuilding (class name from RTTI): the GUN_BUILDING gadget, a building (GUNBUILDING.BIN) with a
 * target and a sphere collider high on its side. Attacks wear its health down (with a short pause
 * between hits); once it is gone it clears its switch, breaks off its debris, swaps to its broken
 * model, shakes the controller and drops its collider. File name inferred from the class. Names
 * marked "(name inferred)" are not original. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetDebris.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/Vibration.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

/* Target (class name from RTTI): a point other objects can aim at (see Spring.cpp). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);
};

/* A part of a model looked up by id (see BreakObj.cpp). */
struct UnkModelPart_8015F7BC {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ const char *name; /* the part's model file */
};

extern "C" {
/* The collision manager (see Bomb.cpp). */
void *fn_80021B08(void);
/* A sphere collider at position (behaviour inferred; see Bomb.cpp). */
boost::shared_ptr<CharaColliBase> fn_800C7EA8(void *manager, const Vec3f *position, u32 flags,
                                              int arg3, int arg4, const boost::weak_ptr<CharInfo> &owner,
                                              const Vec3f *offset, float radius);
/* The attack collision of a collider (behaviour inferred; see Bomb.cpp). */
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager, boost::shared_ptr<CharaColliBase> collider);
/* Sets callback on an attack collision (behaviour inferred; the meaning of arg2 is unknown). */
void fn_800C423C(void *manager, boost::weak_ptr<CharaColliAttack> attack, int arg2,
                 const AttackCallback &callback);

/* Creates a Target for owner (behaviour inferred; the flags are unknown). */
boost::shared_ptr<Target> fn_801E7128(const boost::weak_ptr<CharInfo> &owner, int flags);
/* The world matrix of a character info (behaviour inferred). */
void fn_8015CEA0(GadgetCharInfo *charInfo, Mtx34f *matrix);

/* GadgetModelInfo helpers (see BreakObj.cpp). */
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
UnkModelPart_8015F7BC *fn_8015F7BC(GadgetModelInfo *model, int id);
/* The model loader (see BreakObj.cpp). */
void *fn_800122D4(void);
s32 fn_80012700(void *manager, int arg1, const char *fileName);

/* Starts a controller vibration at position (behaviour inferred). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DE08(int pattern, const Vec3f *position,
                                                      int arg2);
}

class GunBuilding : public Gadget {
  public:
    GunBuilding(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~GunBuilding();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position, yaw and switch id into the gadget (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model at the character info's translation and yaw (name inferred). */
    void RebuildTransform();
    /* The attack callback: takes the attack's damage unless hit just before (name inferred). */
    void OnAttack(AttackCallbackParam &param);
    /* Breaks the debris of model part 2 off the building (name inferred). */
    void SpawnDebris();

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ GadgetModelInfo *m_model;
    /* 0x3C */ s32 m_state; /* 0 standing, 1 destroyed, 2 broken */
    /* 0x40 */ u8 m_switchId;
    /* 0x44 */ float m_health;
    /* 0x48 */ float m_hitTimer; /* time before the next hit counts */
    /* 0x4C */ boost::shared_ptr<Target> m_target;
    /* 0x54 */ boost::shared_ptr<CharaColliBase> m_collider;
    /* 0x5C */ u32 field_0x5C;
};
SIZE_ASSERT(GunBuilding, 0x60);

const char *GunBuilding::s_taskName = "GunBuilding";

GunBuilding::GunBuilding(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CB88(0x93, 4, "GunBuilding");
    m_model = new GadgetModelInfo(1, "GUNBUILDING.BIN");
    m_model->fn_80162A68(0, 0);
    m_model->fn_80161F08("GunBuilding", m_chara);
    ReadPlacement(owner);
    m_target = fn_801E7128(m_chara, 0x12C00);

    Mtx34f matrix;
    fn_8015CEA0(m_chara.get(), &matrix);
    Vec3f position;
    fn_800091EC(&position,
                &(const Vec3f &)fn_8040B67C(&matrix, &Vec3f(60.0f, 55.0f, 0.0f)));
    m_target->fn_800CCE38(&position);

    void *manager = fn_80021B08();
    m_collider = fn_800C7EA8(manager, &position, 0x200, 10, 0, m_chara, &lbl_805E07A0, 10.0f);
    boost::weak_ptr<CharaColliAttack> attack = fn_800C4374(manager, m_collider);
    fn_800C423C(manager, attack, 1, boost::bind(&GunBuilding::OnAttack, this, _1));

    RebuildTransform();
    m_state = 0;
    m_hitTimer = 0.0f;
    m_health = 10.0f;
    fn_801690D0(fn_800CB69C(), m_switchId, 1);
}

GunBuilding::~GunBuilding() {
    if (m_model != 0) {
        delete m_model;
        m_model = 0;
    }
}

void GunBuilding::Behave(float dt) {
    if (m_model != 0) {
        m_model->fn_80162FB0(dt);
    }

    m_hitTimer -= dt;
    if (m_hitTimer < 0.0f) {
        m_hitTimer = 0.0f;
    }

    switch (m_state) {
    case 0:
        break;
    case 1:
        SpawnDebris();
        m_model->fn_80161880();
        m_model->fn_80162804();
        m_model->fn_80162A68(1, 0);
        fn_80161FC8(m_model, 1, m_chara);
        fn_8034DE08(8, &m_chara->m_position, -2);
        m_state = 2;
        break;
    case 2:
        if (m_collider.get() != 0) {
            m_collider.reset();
        }
        break;
    }

    RebuildTransform();
}

void GunBuilding::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    RebuildTransform();
    if (m_model != 0) {
        m_model->fn_80162EF4();
        m_model->fn_8016108C();
    }
}

void GunBuilding::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    m_chara->m_rotation.x = m_chara->m_rotation.z = 0.0f;
    fn_800C9E60(owner, &m_switchId);
}

void GunBuilding::RebuildTransform() {
    if (m_model != 0) {
        Mtx34f matrix;
        Mtx34f translation;

        SetTranslationMtx(&translation, m_chara->m_position);
        fn_8040C148(&matrix, &translation);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        m_model->fn_801633DC(&matrix);
    }
}

void GunBuilding::OnAttack(AttackCallbackParam &param) {
    if (m_hitTimer > 0.0f) {
        return;
    }
    m_health -= param.m_damage;
    m_hitTimer = 0.05f;
    if (m_health <= 0.0f) {
        param.m_result = 3;
        m_state = 1;
        m_target.reset();
        fn_801690D0(fn_800CB69C(), m_switchId, 0);
    } else {
        param.m_result = 1;
    }
}

void GunBuilding::SpawnDebris() {
    if (m_model != 0) {
        UnkModelPart_8015F7BC *part = fn_8015F7BC(m_model, 2);
        if (part != 0) {
            void *loader = fn_800122D4();
            UnkDebrisParam_8015F0B8 debris(m_chara, 0);
            debris.scale = 1.5f;
            debris.model = fn_80012700(loader, 1, part->name);
            debris.field_0x54 = 1;
            debris.field_0x4C = 0x96;
            fn_8015D93C(&debris);
        }
    }
}

extern "C" void fn_8032ADBC(void) {
    fn_8006B474("GUNBUILDING.BIN", 1);
}

extern "C" void fn_8032ADB8(void) {}

void GunBuilding::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new GunBuilding(fn_801EB304(fn_8001C4F8(), 11), owner);
}

GadgetClassDesc lbl_8055D0CC = {
    "GUN_BUILDING", fn_8032ADBC, fn_8032ADB8, GunBuilding::Instantiate, 0, 0, 0x89B, 1, 0, 10, 0,
};
