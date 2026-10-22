/* HeavyDog0410 (class name from RTTI): the task of the HEAVY_DOG_0410 gadget, the first stage of
 * the Heavy Dog boss. Like BlackBull0412 (Game/BlackBull0412.cpp) it is a BossRootTask that keeps
 * the boss's HeavyDog0410VarParam and, in Init, gives it its tables and its control units (the
 * shared Heavy Dog units of Game/HeavyDogCtrlUnit.cpp, the stage's own of
 * Game/HeavyDog0410CtrlUnit.cpp and the mode unit of Game/HeavyDog0410ModeCtrlUnit.cpp). It also
 * defines the shared state's constructor and destructor (HeavyDogVarParam, which
 * Game/HeavyDog0510.cpp reuses) and the stage's unit base, HeavyDog0410CtrlUnit. File name
 * inferred from the class. Names marked "(name inferred)" and all member names are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#define BOOST_SP_CONVERTING_ASSIGNMENT
#define BOSSVARPARAM_CHARA_SHARED_PTR
#define HEAVYDOGVARPARAM_FULL
#include "Game/BossCtrlUnit.h"
#include "Game/BossDef.h"
#include "Game/BossRootTask.h"
#include "Game/EffectPJSLauncher.h"
#include "Game/Gadget.h"
#include "Game/GadgetClassDesc.h"
#include "Game/HeavyDogCtrlUnit.h"
#include "Game/HeavyDogCtrlUnits.h"
#include "Game/HeavyDogModeCtrlUnit.h"
#include "Game/MathTypes.h"
#include "Game/ResourceOneFile.h"
#include "Game/Task.h"
#include "Game/WeaponLockonTarget.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

extern "C" {
/* Places the boss at its gadget owner (see Game/DevilDoom.cpp). */
void fn_8027DE28(BossRootTask *task);
/* The PJS effects of slot index (see Game/EffectPJS.cpp). */
boost::shared_ptr<Effect::PJS::Launcher> fn_801D2CA4(int index);
}

/* The boss's effects (defined in another unit). */
extern boost::shared_ptr<SonicteamUSA::System::Effect::EffectLaunchInfoUSA> lbl_805F0240;

/* The units of the stage defined in Game/HeavyDog0410CtrlUnit.cpp (only their constructors are
 * used here). */
class HeavyDog0410CtrlUnit_Move : public HeavyDog0410CtrlUnit {
  public:
    HeavyDog0410CtrlUnit_Move(HeavyDog0410VarParam *param);
    virtual ~HeavyDog0410CtrlUnit_Move();
    virtual int Tick(float dt);
};

class HeavyDog0410CtrlUnit_Dust : public HeavyDog0410CtrlUnit {
  public:
    HeavyDog0410CtrlUnit_Dust(HeavyDog0410VarParam *param);
    virtual ~HeavyDog0410CtrlUnit_Dust();
    virtual int Tick(float dt);

    /* 0x10 */ u8 field_0x10[0x8];
};

inline BossVarParam::~BossVarParam() {}

inline HeavyDogVarParam::~HeavyDogVarParam() {}

/* Never used; it starts the unit's .bss. */
static u8 s_unused[0x30];
/* The boss's collision spheres, its target and the point the camera looks at (names inferred). */
static UnkSphere_80035CEC s_bodySphere(Vec3f(0.0f, 0.0f, 0.0f), 35.0f);
static UnkSphere_80035CEC s_canopySphere(Vec3f(0.0f, 0.0f, 0.0f), 10.0f);
static Vec3f s_target(0.0f, 0.0f, 0.0f);
static Vec3f s_focusPoint(0.0f, 37.0f, 0.0f);
/* Never used; it ends the unit's .bss. */
static u8 s_unused2[0x98];

/* The shapes of the two collisions, each ended by an empty entry. */
static const UnkBossColliShape s_bodyShapes[2] = {
    {0, &s_bodySphere, 0x80, 0xFAA},
    {0},
};
static const UnkBossColliShape s_canopyShapes[2] = {
    {0, &s_canopySphere, 0x100000, 0x1324},
    {0},
};

static UnkBossColli s_collis[3] = {
    {s_bodyShapes, -1, 4, 1.0f, 1, 1, 1, 1, 1},
    {s_canopyShapes, -1, 4, 1.0f, 1, 1, 1, 1, 1},
    {0, -1, 0, 1.0f, 6, 0, 0, 6, 0},
};

static UnkBossTargetEntry s_targets[2] = {
    {0x2C00, 0x1324, &s_target},
    {0, 0, 0},
};

static UnkBossFocus s_focus = {&s_focusPoint, 36.0f};

static UnkBossDef s_def = {3, 4, 5, 1, 1, s_collis, s_targets, &s_focus};

/* HEAVY_DOG_0410 has no parameters: its table holds only the end entry. */
static GadgetParamDesc s_params[1] = {
    {0},
};

/* The gadget type's callbacks (defined at the end). */
static void Load();
static void Unload();
static void Instantiate(UnkPlacement_800C9CD0 *owner);
static void fn_8024ECD0();

/* The HEAVY_DOG_0410 type record (a central table references it); laid out as
 * BlackBull0412's. */
struct HeavyDog0410TypeInfo {
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
SIZE_ASSERT(HeavyDog0410TypeInfo, 0x24);

HeavyDog0410TypeInfo lbl_80549164 = {
    "HEAVY_DOG_0410", Load, Unload, Instantiate, fn_8024ECD0, 0, 0xB6, 2, 0, 0xA, s_params,
};

/* HeavyDog0410 (class name from RTTI; vtable at 0x805491A8): the boss task. */
class HeavyDog0410 : public BossRootTask {
  public:
    HeavyDog0410(Task *parent, UnkPlacement_800C9CD0 *owner, bool flag);
    virtual ~HeavyDog0410();
    virtual void Init();

    /* 0x38 */ HeavyDog0410VarParam m_param;
};
SIZE_ASSERT(HeavyDog0410, 0x238);

/* Loads the boss's data file into resource slot 0x16 and its effects (name inferred). */
static void Load() {
    fn_8001299C(fn_800122D4(), 0x16, new ResourceOneFile("enemy/boss/HeavyDog0410One.one", false));
    lbl_805F0240 = fn_801D2CA4(3);
}

/* Releases the boss's data and effects (name inferred). */
static void Unload() {
    fn_800127F8(fn_800122D4(), 0x16);
    lbl_805F0240.reset();
}

HeavyDogVarParam::HeavyDogVarParam()
    : m_spring(0.55f), m_damping(1.2f), m_angleSpring(0.55f), m_angleDamping(1.2f),
      m_turnDistance(30.0f), m_soundSpeed(20.0f), m_engineTilt(20.0f), m_turnSpeed(65.0f),
      field_0x94(5.0f), field_0x98(0.5f), m_canopyScore(200), m_launcherScore(200),
      field_0xA4(3), field_0xA8(200.0f), field_0xAC(10.0f), field_0xB0(150.0f),
      m_targetSpeed(280.0f), m_energyMax(15.0f), m_launcherHealth(20.0f), m_reloadTime(2.0f),
      m_launcherAim(25.0f), m_launcherTurn(45.0f), m_rocketEffect(10), m_barrierRadius(300.0f),
      m_barrierHeight(10.0f), m_barrierTime(3.5f), field_0xDC(150.0f), field_0xE0(200.0f),
      field_0xE4(40.0f) {
    m_charaPtr = boost::shared_ptr<BossCharInfo>(new BossCharInfo(0xA9));
    m_launcherDamage = 0.0f;
    fn_800091D0(&m_velocity, &lbl_805E07A0);
    fn_800091D0(&m_acceleration, &lbl_805E07A0);
    m_fixed = 0;
    m_turnFixed = 0;
    m_damaged = 0;
    m_turnToVelocity = 1;
    field_0x1C8 = 0;
    field_0x1D0 = 0;
    m_attribute = 0;
    m_energyFull = m_energyMax;
    m_energy = m_energyFull;
    m_launcherCount = 0;
    m_launcherIndex = 0;
    m_launchersAlive = 0;
    field_0x58.y = 0.0f;
    m_turnRate = 0.0f;
    m_lockon = boost::shared_ptr<Weapon::LockonTarget>(new Weapon::LockonTarget());
}

HeavyDog0410VarParam::HeavyDog0410VarParam()
    : field_0x1D8(85.0f), field_0x1DC(60.0f), field_0x1E0(40.0f), field_0x1E4(40.0f),
      m_mineWaitTime(5.0f), m_mineBlinkTime(1.0f), m_mineExplodeTime(0.5f), m_mineSpring(1.5f),
      m_mineDamping(1.2f), field_0x1FC(10.0f) {}

HeavyDog0410::HeavyDog0410(Task *parent, UnkPlacement_800C9CD0 *owner, bool flag)
    : BossRootTask(parent, owner, flag, &m_param) {}

/* Creates the boss's task under task layer 14 (name inferred). */
static void Instantiate(UnkPlacement_800C9CD0 *owner) {
    new HeavyDog0410(fn_801EB304(fn_8001C4F8(), 0xE), owner, false);
}

/* The type record's callback at 0x10 (meaning unknown). */
static void fn_8024ECD0() {}

inline HeavyDog0410CtrlUnit::HeavyDog0410CtrlUnit(HeavyDog0410VarParam *owner)
    : BossCtrlUnit(owner), HeavyDogCtrlUnit(owner), m_owner(owner) {}

inline HeavyDog0410CtrlUnit::~HeavyDog0410CtrlUnit() {}

inline HeavyDog0410ModeCtrlUnit::HeavyDog0410ModeCtrlUnit(HeavyDog0410VarParam *owner)
    : BossCtrlUnit(owner), HeavyDog0410CtrlUnit(owner), BossModeCtrlUnit(owner) {}

void HeavyDog0410::Init() {
    fn_8027DE28(this);
    m_param.m_def = &s_def;
    new BossCtrl(this, &m_param);
    m_param.m_maxHealth = 140.0f;
    m_param.m_health = m_param.m_maxHealth;
    m_param.m_launcherDamage = m_param.m_launcherHealth;
    m_param.m_effects = lbl_805F0240;
    fn_800091D0(&m_param.m_targetPosition,
                fn_800091D0(&m_param.m_home, &m_param.m_charaPtr.get()->m_position));
    m_param.m_turnToVelocity = 0;
    fn_803358D0(&m_param, new HeavyDogCtrlUnit_AttackCallbackBody(&m_param, 0));
    fn_803358D0(&m_param, new HeavyDogCtrlUnit_AttackCallbackCanopy(&m_param, 1));
    fn_803358D0(&m_param, new HeavyDogCtrlUnit_GetStatus(&m_param));
    fn_803358D0(&m_param, new HeavyDog0410ModeCtrlUnit(&m_param));
    fn_803358D0(&m_param, new HeavyDog0410CtrlUnit_Move(&m_param));
    fn_803358D0(&m_param, new HeavyDogCtrlUnit_Rotate(&m_param));
    fn_803358D0(&m_param, new HeavyDogCtrlUnit_RocketLauncher(&m_param, 0x1130));
    fn_803358D0(&m_param, new HeavyDogCtrlUnit_RocketLauncher(&m_param, 0x113A));
    fn_803358D0(&m_param, new HeavyDogCtrlUnit_RocketLauncher(&m_param, 0x1144));
    fn_803358D0(&m_param, new HeavyDogCtrlUnit_RocketLauncher(&m_param, 0x1162));
    fn_803358D0(&m_param, new HeavyDogCtrlUnit_RocketLauncher(&m_param, 0x1158));
    fn_803358D0(&m_param, new HeavyDogCtrlUnit_RocketLauncher(&m_param, 0x114E));
    fn_803358D0(&m_param, new HeavyDogCtrlUnit_RocketLauncher(&m_param, 0x116C));
    fn_803358D0(&m_param, new HeavyDogCtrlUnit_Engine(&m_param, 0x1004));
    fn_803358D0(&m_param, new HeavyDogCtrlUnit_Engine(&m_param, 0x1068));
    fn_803358D0(&m_param, new HeavyDogCtrlUnit_Engine(&m_param, 0x10CC));
    fn_803358D0(&m_param, new BossCtrlUnit_DispGauge(&m_param));
    fn_803358D0(&m_param, new BossCtrlUnit_CheckEvent(&m_param));
    fn_803358D0(&m_param, new BossCtrlUnit_FixLookCharCamera(&m_param));
    fn_803358D0(&m_param, new BossCtrlUnit_CheckFirstDamage(&m_param, 0xE0, 0));
    fn_803358D0(&m_param, new HeavyDogCtrlUnit_CheckLauncherNum(&m_param, 0xBE2));
    fn_803358D0(&m_param, new HeavyDogCtrlUnit_CheckMotionAttribute(&m_param));
    fn_803358D0(&m_param, new HeavyDogCtrlUnit_DispCanopy(&m_param, 6));
    fn_803358D0(&m_param, new HeavyDogCtrlUnit_DispEnergy(&m_param, 7));
    fn_803358D0(&m_param, new HeavyDog0410CtrlUnit_Dust(&m_param));
    Vec3f corners[4];
    Vec3f corner0(150.0f, 0.0f, 150.0f);
    fn_800091EC(&corners[0], &corner0);
    Vec3f corner1(-150.0f, 0.0f, 150.0f);
    fn_800091EC(&corners[1], &corner1);
    Vec3f corner2(-150.0f, 0.0f, -150.0f);
    fn_800091EC(&corners[2], &corner2);
    Vec3f corner3(150.0f, 0.0f, -150.0f);
    fn_800091EC(&corners[3], &corner3);
    for (int i = 0; i < 4; i++) {
        fn_803358D0(&m_param, new BossCtrlUnit_StageObject(&m_param, &corners[i], &lbl_805E07A0,
                                                           10, 11, 0, 82.0f));
    }
    fn_803358D0(&m_param, new BossCtrlUnit_Shadow(&m_param, m_param.field_0xE4));
    if (m_flag) {
        m_param.m_nextMode = 2;
    } else {
        m_param.m_nextMode = 6;
    }
}

HeavyDog0410VarParam::~HeavyDog0410VarParam() {}

HeavyDog0410::~HeavyDog0410() {}
