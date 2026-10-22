/* EggMeca511 (class name from RTTI): the task of the EGG_MECA_0511 gadget, the Egg Breaker boss of
 * stage 511. Like the Black Bull and Heavy Dog stages (Game/BlackBull0412.cpp,
 * Game/HeavyDog0510.cpp) it is a BossRootTask that keeps the boss's state and, in Init, gives it
 * its tables and its control units (Game/EggMecaCtrlUnit.cpp, Game/EggMecaModeCtrlUnit.cpp). This
 * unit also defines EggMecaVarParam, the state the three Egg Breaker stages share: its constructor,
 * its tuning values' constructor and its destructor are out of line here, and the 411 and 310
 * stages' units call them. File name inferred from the class. Names marked "(name inferred)" and
 * all member names are not original. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first); the units' inline constructors are emitted after Init,
 * their first user. */
#define BOOST_SP_CONVERTING_ASSIGNMENT
#define BOSSVARPARAM_CHARA_SHARED_PTR
#include "Game/BossCtrlUnit.h"
#include "Game/BossDef.h"
#include "Game/BossRootTask.h"
#include "Game/EffectPJSLauncher.h"
#include "Game/EggMecaCtrlUnit.h"
#include "Game/Gadget.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/ResourceOneFile.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

struct UnkHintManager_801FB180;

extern "C" {
/* Places the boss at its gadget owner (see Game/DevilDoom.cpp). */
void fn_8027DE28(BossRootTask *task);
/* The PJS effects of slot index (see Game/EffectPJS.cpp). */
boost::shared_ptr<Effect::PJS::Launcher> fn_801D2CA4(int index);
s32 fn_801FAF00(UnkHintManager_801FB180 *manager, s32 id, s32 branch, int arg3, int arg4);
}

UnkHintManager_801FB180 *GetHintManager();

/* The boss's effects (defined in another unit). */
extern boost::shared_ptr<SonicteamUSA::System::Effect::EffectLaunchInfoUSA> lbl_805F0240;

/* BossVarParam's destructor is inline, as in Game/DevilDoom.cpp: the linker drops this copy, but
 * it puts BossVarParam's RTTI name before EggMecaVarParam's in .rodata. */
inline BossVarParam::~BossVarParam() {}

/* The boss's collision sphere and its target (names inferred). */
static UnkSphere_80035CEC s_bodySphere(Vec3f(0.0f, 0.0f, 0.0f), 25.0f);
static Vec3f s_target(0.0f, 1.0f, 0.0f);

/* The shapes of the boss's collision, ended by an empty entry. */
static const UnkBossColliShape s_bodyShapes[2] = {
    {0, &s_bodySphere, 0x80, 0x1E},
    {0},
};

static UnkBossColli s_collis[3] = {
    {s_bodyShapes, -1, 4, 1.0f, 1, 1, 0, 1, 1},
    {0, 0x14, 4, 1.0f, 1, 1, 1, 1, 0},
    {0, -1, 0, 1.0f, 6, 0, 0, 6, 0},
};

static UnkBossTargetEntry s_targets[2] = {
    {0x2C00, 0x1E, &s_target},
    {0, 0, 0},
};

static UnkBossDef s_def = {6, 0x10, 0x13, 1, 2, s_collis, s_targets, 0};

/* EGG_MECA_0511 has no parameters: its table holds only the end entry. */
static GadgetParamDesc s_params[1] = {
    {0},
};

/* The gadget type's callbacks (defined at the end). */
static void Load();
static void Unload();
static void Instantiate(UnkPlacement_800C9CD0 *owner);
static void fn_802E9D60();

/* The EGG_MECA_0511 type record (a central table references it); laid out as the other bosses'. */
struct EggMeca511TypeInfo {
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
SIZE_ASSERT(EggMeca511TypeInfo, 0x24);

EggMeca511TypeInfo lbl_8055980C = {
    "EGG_MECA_0511", Load, Unload, Instantiate, fn_802E9D60, 0, 0xBB, 2, 0, 0xA, s_params,
};

/* EggMeca511 (class name from RTTI; vtable at 0x80559850): the boss task. */
class EggMeca511 : public BossRootTask {
  public:
    EggMeca511(Task *parent, UnkPlacement_800C9CD0 *owner, bool flag);
    virtual ~EggMeca511();
    virtual void Init();

    /* 0x38 */ EggMecaVarParam m_param;
};
SIZE_ASSERT(EggMeca511, 0x288);

/* Loads the boss's data file into resource slot 0x16 and its effects (name inferred). */
static void Load() {
    fn_8001299C(fn_800122D4(), 0x16, new ResourceOneFile("enemy/boss/EggMecaOne.one", false));
    lbl_805F0240 = fn_801D2CA4(3);
}

/* Releases the boss's data and effects (name inferred). */
static void Unload() {
    fn_800127F8(fn_800122D4(), 0x16);
    lbl_805F0240.reset();
}

EggMecaSettings::EggMecaSettings() {
    m_waitSpeed = 120.0f;
    m_dashSpeed = 120.0f;
    m_landSpeed = 90.0f;
    m_shootSpeed = 120.0f;
    m_laserSpeed = 0.0f;
    m_laserFireSpeed = 0.0f;
    m_waitTime = 1.0f;
    m_stunTime = 0.5f;
    m_score = 100;
    m_comboLimit = 3;
    m_targetSpeed = 280.0f;
    m_damageLimit = 10.0f;
    m_nearDistance = 300.0f;
    m_farDistance = 350.0f;
    m_jumpHeight = 20.0f;
    m_ironBallHeight = 15.0f;
    field_0x40 = m_nearDistance;
    m_dashRadius = 50.0f;
    m_jumpDistance = 50.0f;
    m_gravity = 430.0f;
    m_dashAngle = 22.5f;
    m_circleRadius = 5000.0f;
    m_circleSpeed = 2.0f;
    m_ironBallRadius = 12.5f;
    m_ironBallRadius2 = 25.0f;
    m_shockWaveRadius = 100.0f;
    field_0x68 = 1000.0f;
    m_shockWaveTime = 0.25f;
    m_shakeArgs[0] = 0.3f;
    m_shakeArgs[1] = 1.24f;
    m_shakeArgs[2] = 10.0f;
    m_ironBallTime = 0.35f;
    m_ironBallReach = 15.0f;
    m_laserChargeTime = 0.5f;
    m_laserFireTime = 1.5f;
    field_0x8C = 3.0f;
    m_laserRadius = 10.0f;
    m_laserLength = 90.0f;
    m_laserSweep[0] = 30.0f;
    m_laserSweep[1] = 10.0f;
    m_laserPower = 5.0f;
    m_turnAngle = 45.0f;
    m_turnAngle2 = 90.0f;
    m_hintTime = 8.0f;
    m_moveTime = 10.0f;
    m_rollAcceleration = 45.0f;
    m_rollDamping = 4.0f;
    m_rollStopSpeed = lbl_805FAD38;
    m_bombGrowTime = 1.0f;
    m_bombHeight = 60.0f;
    m_bombDropHeight = 450.0f;
    m_bombCount = 8;
    m_bombSpeed = 400.0f;
    m_bombAcceleration = 300.0f;
    m_bombDamping = 0.0f;
    m_bombSpread = 0.25f * lbl_805FAD38;
    m_bombDistance = 30.0f;
    m_bombRadius = 25.0f;
    m_areaHalfSize = 480.0f;
    m_areaRadius = 370.0f;
    m_areaMinRadius = 150.0f;
    m_areaMaxRadius = 380.0f;
    m_cameraDistance = 90.0f;
    m_cameraHeight = 16.0f;
    m_cameraOffsetY = 0.0f;
    m_cameraOffsetZ = 0.0f;
    field_0x108 = 22.5f;
    field_0x10C = 0;
    field_0x10D = 0;
    m_belowHalfCamera = 0x17;
    m_belowHalfMotion = 5;
    m_cameraFreeHeight = 140.0f;
    m_cameraFreeRadius = 370.0f;
}

EggMecaVarParam::EggMecaVarParam() {
    m_charaPtr = boost::shared_ptr<BossCharInfo>(new BossCharInfo(0xAB));
    m_health = 0.0f;
    fn_800091D0(reinterpret_cast<Vec3f *>(&field_0x48), &lbl_805E07A0);
    field_0x1F8 = 0;
    m_motionLocked = 0;
    m_fixed = 0;
    m_targetFixed = 0;
    field_0x1B7 = 1;
    m_comboCount = 0;
    m_damageTaken = 0.0f;
    m_attribute = 0;
    m_ironBallRequest = -1;
    m_ironBallState = 0;
    m_hasIronBall = 1;
    m_laserRequest[1] = -1;
    m_laserRequest[0] = -1;
    m_laserState[1] = 0;
    m_laserState[0] = 0;
    m_moving = 0;
    m_hintTime = 0.0f;
    m_visible = 1;
    m_dead = 0;
}

EggMeca511::EggMeca511(Task *parent, UnkPlacement_800C9CD0 *owner, bool flag)
    : BossRootTask(parent, owner, flag, &m_param) {}

/* Creates the boss's task under task layer 14 (name inferred). */
static void Instantiate(UnkPlacement_800C9CD0 *owner) {
    new EggMeca511(fn_801EB304(fn_8001C4F8(), 0xE), owner, false);
}

/* The type record's callback at 0x10 (meaning unknown). */
static void fn_802E9D60() {}

inline EggMecaModeCtrlUnit::EggMecaModeCtrlUnit(EggMecaVarParam *owner)
    : BossCtrlUnit(owner), EggMecaCtrlUnit(owner), BossModeCtrlUnit(owner) {}

void EggMeca511::Init() {
    fn_8027DE28(this);
    m_param.m_def = &s_def;
    new BossCtrl(this, &m_param);
    m_param.m_maxHealth = 200.0f;
    m_param.m_settings.m_nearDistance = 200.0f;
    m_param.m_health = m_param.m_maxHealth;
    m_param.m_effects = lbl_805F0240;
    fn_800091D0(&m_param.m_home,
                fn_800091D0(&m_param.m_start, &m_param.m_charaPtr.get()->m_position));
    fn_803358D0(&m_param, new EggMecaCtrlUnit_AttackCallback(&m_param));
    fn_803358D0(&m_param, new EggMecaCtrlUnit_GetStatus(&m_param));
    fn_803358D0(&m_param, new EggMecaModeCtrlUnit(&m_param));
    fn_803358D0(&m_param, new EggMecaCtrlUnit_Move(&m_param));
    fn_803358D0(&m_param, new EggMecaCtrlUnit_CheckMotionAttribute(&m_param));
    fn_803358D0(&m_param, new BossCtrlUnit_DispGauge(&m_param));
    fn_803358D0(&m_param, new BossCtrlUnit_CheckEvent(&m_param));
    fn_803358D0(&m_param, new EggMecaCtrlUnit_FixLookCharCamera(&m_param));
    fn_803358D0(&m_param, new BossCtrlUnit_CheckFirstDamage(&m_param, 0xD4, 0));
    fn_803358D0(&m_param, new EggMecaCtrlUnit_DispCanopy(&m_param, 0xD));
    fn_803358D0(&m_param, new EggMecaCtrlUnit_DispEggman(&m_param, 0xA));
    fn_803358D0(&m_param, new EggMecaCtrlUnit_DispEye(&m_param));
    fn_803358D0(&m_param, new EggMecaCtrlUnit_IronBall(&m_param));
    fn_803358D0(&m_param, new EggMecaCtrlUnit_Laser(&m_param, 0));
    fn_803358D0(&m_param, new EggMecaCtrlUnit_Laser(&m_param, 1));
    fn_803358D0(&m_param, new EggMecaCtrlUnit_RollUpper(&m_param));
    fn_803358D0(&m_param, new EggMecaCtrlUnit_AdjustCircleArea(&m_param));
    if (m_flag) {
        m_param.m_nextMode = 2;
        m_param.m_active = 1;
    } else {
        m_param.m_nextMode = 0xE;
    }
    fn_801FAF00(GetHintManager(), 0xD3, 0, 0, 0);
    m_param.m_settings.m_belowHalfCamera = 0x16;
    m_param.m_settings.m_belowHalfMotion = 0x14;
}

EggMecaVarParam::~EggMecaVarParam() {}

EggMeca511::~EggMeca511() {}
