/* BlackDoom (class name from RTTI): the task of the BLACK_DOOM gadget, the Black Doom boss. It is
 * a BossRootTask that keeps the boss's BlackDoomVarParam (also defined here) and, in Init, gives it
 * its tables and the control units of the BlackDoomCtrlUnit unit and of
 * Game/BlackDoomModeCtrlUnit.cpp, plus the stage objects of the three stages the boss appears in.
 * The gadget type's load and unload callbacks load and release the boss's data file and its
 * effects. File name inferred from the class (as Game/DevilDoom.cpp). Names marked "(name
 * inferred)" and all member names are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first); the units' inline constructors are
 * emitted after Init, their first user. */
#define BOOST_SP_CONVERTING_ASSIGNMENT
#include "Game/BlackDoomCtrlUnit.h"
#include "Game/BossCtrlUnit.h"
#include "Game/BossDef.h"
#include "Game/BossRootTask.h"
#include "Game/EffectPJSLauncher.h"
#include "Game/EnemyCharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetClassDesc.h"
#include "Game/ResourceOneFile.h"
#include "Game/Task.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include <typeinfo>
#include "size_assert.h"

extern "C" {
/* Places the boss at its gadget owner (behaviour inferred from the body). */
void fn_8027DE28(BossRootTask *task);
/* The PJS effects of slot index (see Game/EffectPJS.cpp). */
boost::shared_ptr<Effect::PJS::Launcher> fn_801D2CA4(int index);
}

/* The boss's effects (defined in Game/BossRootTask.cpp). */
extern boost::shared_ptr<SonicteamUSA::System::Effect::EffectLaunchInfoUSA> lbl_805F0240;

/* The game state (struct name unknown; see AdvMemoryCheck.cpp): the stage number at 0xA8. */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xA8];
    /* 0x0A8 */ s32 m_stage;
    /* 0x0AC */ u8 field_0xAC[0x150 - 0xAC];
};
extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* BossVarParam's destructor is inline here as in Game/DevilDoom.cpp: this unit emits a weak copy
 * and, with it, BossVarParam's vtable and RTTI, which puts BossVarParam's RTTI name before
 * BlackDoomVarParam's in .rodata. */
inline BossVarParam::~BossVarParam() {}

inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* The boss's collision sphere and its target (names inferred). */
static UnkSphere_80035CEC s_sphere(Vec3f(0.0f, 0.0f, 0.0f), 10.0f);
static Vec3f s_target(0.0f, 0.0f, 0.0f);

/* The shape of the boss's collision, ended by an empty entry (see Game/BossDef.h). */
static const UnkBossColliShape s_colliShapes[2] = {
    {0, &s_sphere, 0x80, 0x1E},
    {0},
};

static UnkBossColli s_collis[2] = {
    {&s_colliShapes[0], -1, 4, 1.0f, 1, 1, 0, 1, 1},
    {0, -1, 0, 1.0f, 6, 0, 0, 6, 0},
};

static UnkBossTargetEntry s_targets[2] = {
    {0x2C00, 0x1E, &s_target},
    {0, 0, 0},
};

static UnkBossDef s_def = {3, 4, 5, 1, 2, s_collis, s_targets, 0};

/* BLACK_DOOM has no parameters: its table holds only the end entry. */
static GadgetParamDesc s_params[1] = {
    {0},
};

/* The gadget type's callbacks (defined at the end). */
static void Load();
static void Unload();
static void Instantiate(UnkPlacement_800C9CD0 *owner);
static void fn_80283514();

/* The BLACK_DOOM type record (a central table references it); laid out as DEVIL_DOOM's (see
 * Game/DevilDoom.cpp). */
struct BlackDoomTypeInfo {
    /* 0x00 */ const char *name;
    /* 0x04 */ void (*load)(void);
    /* 0x08 */ void (*unload)(void);
    /* 0x0C */ void (*create)(UnkPlacement_800C9CD0 *owner);
    /* 0x10 */ void (*field_0x10)(void);
    /* 0x14 */ u32 field_0x14;
    /* 0x18 */ u16 id;
    /* 0x1A */ u8 field_0x1A;
    /* 0x1B */ u8 field_0x1B;
    /* 0x1C */ u8 field_0x1C;
    /* 0x20 */ GadgetParamDesc *params;
};
SIZE_ASSERT(BlackDoomTypeInfo, 0x24);

BlackDoomTypeInfo lbl_8054E2F8 = {
    "BLACK_DOOM", Load, Unload, Instantiate, fn_80283514, 0, 0xB8, 2, 0, 0xA, s_params,
};

/* BlackDoom (class name from RTTI; vtable at 0x8054E33C): the boss task. */
class BlackDoom : public BossRootTask {
  public:
    BlackDoom(Task *parent, UnkPlacement_800C9CD0 *owner, bool flag);
    virtual ~BlackDoom();
    virtual void Init();

    /* 0x38 */ BlackDoomVarParam m_param;
};
SIZE_ASSERT(BlackDoom, 0x2C0);



/* Loads the boss's data file into resource slot 0x16 and its effects (name inferred). */
static void Load() {
    fn_8001299C(fn_800122D4(), 0x16, new ResourceOneFile("enemy/boss/BlackDoomOne.one", false));
    lbl_805F0240 = fn_801D2CA4(3);
}

/* Releases the boss's data and effects (name inferred). */
static void Unload() {
    fn_800127F8(fn_800122D4(), 0x16);
    lbl_805F0240.reset();
}

/* BossVarParam::m_chara is the px of a shared_ptr<BossCharInfo> (see BossCtrlUnit.h). */
inline boost::shared_ptr<BossCharInfo> &CharInfoPtr(BossVarParam *param) {
    return *reinterpret_cast<boost::shared_ptr<BossCharInfo> *>(&param->m_chara);
}

BlackDoomVarParam::BlackDoomVarParam() {
    CharInfoPtr(this) = boost::shared_ptr<BossCharInfo>(new BossCharInfo(0xAA));
    m_health = 0.0f;
    AssignVector(field_0x48, lbl_805E07A0);
    m_hit = 0;
    m_moveLocked = 0;
    m_turnLocked = 0;
    m_targetLocked = 0;
    m_accel = 0.0f;
    m_speed = 0.0f;
    m_turnSpeed = 0.0f;
    m_comboCount = 0;
    m_damage = 0.0f;
    m_attribute = 0;
    m_corner = m_nextCorner = m_prevCorner = -1;
    m_direction = 0;
    m_attackUsed[0] = m_attackUsed[1] = m_attackUsed[2] = 1;
    m_recentDamage = 0.0f;
    m_damageScale = 1.0f;
    m_healthScale = 1.0f;
    field_0x184 = m_alterEgos = m_attackStep = 0;
}

BlackDoom::BlackDoom(Task *parent, UnkPlacement_800C9CD0 *owner, bool flag)
    : BossRootTask(parent, owner, flag, &m_param) {}

/* Creates the boss's task under task layer 14 (name inferred). */
static void Instantiate(UnkPlacement_800C9CD0 *owner) {
    new BlackDoom(fn_801EB304(fn_8001C4F8(), 0xE), owner, false);
}

/* The type record's callback at 0x10 (meaning unknown). */
static void fn_80283514() {}

inline BlackDoomModeCtrlUnit::BlackDoomModeCtrlUnit(BlackDoomVarParam *owner)
    : BossCtrlUnit(owner), BlackDoomCtrlUnit(owner), BossModeCtrlUnit(owner) {}

/* Never called: it creates Task's RTTI while the source is read, which puts the RTTI name "Task"
 * before kAlterEgoCount in .sdata2 as in retail (see Game/Mission.cpp). */
inline const std::type_info &TaskType() {
    return typeid(Task);
}

/* The number of alter egos the boss starts with (name inferred). Retail reads it from .sdata2,
 * which a const defined after its use reproduces (as HeavyDog::kMaxLaunchers in
 * Game/HeavyDogCtrlUnit.cpp). */
namespace BlackDoomData {
extern const s32 kAlterEgoCount;
} // namespace BlackDoomData

void BlackDoom::Init() {
    fn_8027DE28(this);
    m_param.m_def = &s_def;
    new BossCtrl(this, &m_param);
    m_param.m_maxHealth = 100.0f;
    m_param.m_health = m_param.m_maxHealth;
    m_param.m_effects = lbl_805F0240;
    AssignVector(m_param.m_home, m_param.m_chara->m_position);
    fn_803358D0(&m_param, new BlackDoomCtrlUnit_AttackCallback(&m_param));
    fn_803358D0(&m_param, new BlackDoomCtrlUnit_GetStatus(&m_param));
    fn_803358D0(&m_param, new BlackDoomModeCtrlUnit(&m_param));
    fn_803358D0(&m_param, new BlackDoomCtrlUnit_Move(&m_param));
    fn_803358D0(&m_param, new BossCtrlUnit_DispGauge(&m_param));
    fn_803358D0(&m_param, new BossCtrlUnit_CheckEvent(&m_param));
    fn_803358D0(&m_param, new BlackDoomCtrlUnit_FixLookCharCamera(&m_param));
    for (int i = 0; i < BlackDoomData::kAlterEgoCount; i++) {
        fn_803358D0(&m_param, new BlackDoomCtrlUnit_AlterEgo(&m_param));
    }
    Vec3f corners[4];
    Vec3f corner0(176.7f, 0.0f, 176.7f);
    fn_800091EC(&corners[0], &corner0);
    Vec3f corner1(-176.7f, 0.0f, 176.7f);
    fn_800091EC(&corners[1], &corner1);
    Vec3f corner2(-176.7f, 0.0f, -176.7f);
    fn_800091EC(&corners[2], &corner2);
    Vec3f corner3(176.7f, 0.0f, -176.7f);
    fn_800091EC(&corners[3], &corner3);
    switch (lbl_8057D6A0.m_stage) {
    case 0x262:
        fn_803358D0(&m_param, new BossCtrlUnit_CheckFirstDamage(&m_param, 0xFC, 0));
        for (int i = 0; i < 4; i++) {
            fn_803358D0(&m_param, new BossCtrlUnit_StageObject(&m_param, &corners[i], &lbl_805E07A0, 8,
                                                               -1, "S0610_ZN036.ADB", 70.0f));
        }
        break;
    case 0x268:
        fn_803358D0(&m_param, new BossCtrlUnit_CheckFirstDamage(&m_param, 0x172, 0));
        for (int i = 0; i < 4; i++) {
            fn_803358D0(&m_param, new BossCtrlUnit_StageObject(&m_param, &corners[i], &lbl_805E07A0, 9,
                                                               10, 0, 70.0f));
        }
        break;
    case 0x269:
        fn_803358D0(&m_param, new BossCtrlUnit_CheckFirstDamage(&m_param, 0x180, 0));
        for (int i = 0; i < 4; i++) {
            fn_803358D0(&m_param, new BossCtrlUnit_StageObject(&m_param, &corners[i], &lbl_805E07A0,
                                                               0xB, -1, "S0617_ZN061.ADB", 70.0f));
        }
        break;
    }
    fn_803358D0(&m_param, new BlackDoomCtrlUnit_CheckMotionAttribute(&m_param));
    fn_803358D0(&m_param, new BlackDoomCtrlUnit_DispEye(&m_param));
    fn_803358D0(&m_param, new BlackDoomCtrlUnit_DispAura(&m_param));
    if (m_flag) {
        m_param.m_nextMode = 2;
    } else {
        m_param.m_nextMode = 9;
    }
}

namespace BlackDoomData {
const s32 kAlterEgoCount = 10;
} // namespace BlackDoomData

BlackDoomVarParam::~BlackDoomVarParam() {}

BlackDoom::~BlackDoom() {}
