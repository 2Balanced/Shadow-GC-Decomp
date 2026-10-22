/* HeavyDog0510 (class name from RTTI): the task of the HEAVY_DOG_0510 gadget, the second stage of
 * the Heavy Dog boss. Like HeavyDog0410 (Game/HeavyDog0410.cpp) it is a BossRootTask that keeps
 * the boss's HeavyDog0510VarParam and, in Init, gives it its tables and its control units (the
 * shared Heavy Dog units of Game/HeavyDogCtrlUnit.cpp, the stage's own of
 * Game/HeavyDog0510CtrlUnit.cpp and the mode unit of Game/HeavyDog0510ModeCtrlUnit.cpp). The
 * shared state's constructor is out of line in Game/HeavyDog0410.cpp; its destructor and the
 * inline constructors of the shared units are linked from there too. File name inferred from the
 * class. Names marked "(name inferred)" and all member names are not original. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
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
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

extern "C" {
/* Places the boss at its gadget owner (see Game/DevilDoom.cpp). */
void fn_8027DE28(BossRootTask *task);
/* The PJS effects of slot index (see Game/EffectPJS.cpp). */
boost::shared_ptr<Effect::PJS::Launcher> fn_801D2CA4(int index);
float fn_80016260(float x, float z); /* the angle of (x, z) */
}

/* The boss's effects (defined in another unit). */
extern boost::shared_ptr<SonicteamUSA::System::Effect::EffectLaunchInfoUSA> lbl_805F0240;

/* The units of the stage defined in Game/HeavyDog0510CtrlUnit.cpp (only their constructors are
 * used here). */
class HeavyDog0510CtrlUnit_Move : public HeavyDog0510CtrlUnit {
  public:
    HeavyDog0510CtrlUnit_Move(HeavyDog0510VarParam *param);
    virtual ~HeavyDog0510CtrlUnit_Move();
    virtual int Tick(float dt);
};

class HeavyDog0510CtrlUnit_FixLookCharCamera : public HeavyDog0510CtrlUnit,
                                               public BossCtrlUnit_FixLookCharCamera {
  public:
    HeavyDog0510CtrlUnit_FixLookCharCamera(HeavyDog0510VarParam *param);
    virtual ~HeavyDog0510CtrlUnit_FixLookCharCamera();
    virtual int Tick(float dt);
    virtual bool IsEnabled();

    /* 0x40 */ s32 m_view;
};

inline BossVarParam::~BossVarParam() {}

inline HeavyDogVarParam::~HeavyDogVarParam() {}

/* Never used; it starts the unit's .bss. */
static u8 s_unused[0x30];
/* The boss's collision spheres and its target (names inferred). */
static UnkSphere_80035CEC s_bodySphere(Vec3f(0.0f, 0.0f, 0.0f), 35.0f);
static UnkSphere_80035CEC s_canopySphere(Vec3f(0.0f, 0.0f, 0.0f), 10.0f);
static Vec3f s_target(0.0f, 0.0f, 0.0f);
/* Never used; it ends the unit's .bss. */
static u8 s_unused2[0x94];

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

static UnkBossDef s_def = {3, 4, 5, 1, 1, s_collis, s_targets, 0};

/* HEAVY_DOG_0510 has no parameters: its table holds only the end entry. */
static GadgetParamDesc s_params[1] = {
    {0},
};

/* The gadget type's callbacks (defined at the end). */
static void Load();
static void Unload();
static void Instantiate(UnkPlacement_800C9CD0 *owner);
static void fn_802540F4();

/* The HEAVY_DOG_0510 type record (a central table references it); laid out as
 * HeavyDog0410's. */
struct HeavyDog0510TypeInfo {
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
SIZE_ASSERT(HeavyDog0510TypeInfo, 0x24);

HeavyDog0510TypeInfo lbl_80549A34 = {
    "HEAVY_DOG_0510", Load, Unload, Instantiate, fn_802540F4, 0, 0xB7, 2, 0, 0xA, s_params,
};

/* HeavyDog0510 (class name from RTTI; vtable at 0x80549A78): the boss task. */
class HeavyDog0510 : public BossRootTask {
  public:
    HeavyDog0510(Task *parent, UnkPlacement_800C9CD0 *owner, bool flag);
    virtual ~HeavyDog0510();
    virtual void Init();

    /* 0x38 */ HeavyDog0510VarParam m_param;
};
SIZE_ASSERT(HeavyDog0510, 0x240);

/* Loads the boss's data file into resource slot 0x16 and its effects (name inferred). */
static void Load() {
    fn_8001299C(fn_800122D4(), 0x16, new ResourceOneFile("enemy/boss/HeavyDog0510One.one", false));
    lbl_805F0240 = fn_801D2CA4(3);
}

/* Releases the boss's data and effects (name inferred). */
static void Unload() {
    fn_800127F8(fn_800122D4(), 0x16);
    lbl_805F0240.reset();
}

HeavyDog0510VarParam::HeavyDog0510VarParam()
    : m_radiusSpeed(100.0f), m_bombScale(10.0f), m_bombInterval(5.0f), m_bombCount(3),
      m_bombSpeed(400.0f), m_bombSpread(400.0f), field_0x1F0(20.0f), m_angleSpeed(0.0f),
      m_bombLayer(0) {}

HeavyDog0510::HeavyDog0510(Task *parent, UnkPlacement_800C9CD0 *owner, bool flag)
    : BossRootTask(parent, owner, flag, &m_param) {}

/* Creates the boss's task under task layer 14 (name inferred). */
static void Instantiate(UnkPlacement_800C9CD0 *owner) {
    new HeavyDog0510(fn_801EB304(fn_8001C4F8(), 0xE), owner, false);
}

/* The type record's callback at 0x10 (meaning unknown). */
static void fn_802540F4() {}

inline HeavyDog0510ModeCtrlUnit::HeavyDog0510ModeCtrlUnit(HeavyDog0510VarParam *owner)
    : BossCtrlUnit(owner), HeavyDog0510CtrlUnit(owner), BossModeCtrlUnit(owner) {}

void HeavyDog0510::Init() {
    fn_8027DE28(this);
    m_param.m_def = &s_def;
    new BossCtrl(this, &m_param);
    m_param.m_maxHealth = 140.0f;
    m_param.m_turnDistance = 0.5f * m_param.m_barrierHeight;
    m_param.m_health = m_param.m_maxHealth;
    m_param.m_launcherDamage = m_param.m_launcherHealth;
    m_param.m_effects = lbl_805F0240;
    fn_800091D0(&m_param.m_targetPosition,
                fn_800091D0(&m_param.m_home, &m_param.m_charaPtr.get()->m_position));
    m_param.m_turnToVelocity = 0;
    Vec3f home;
    fn_800091EC(&home, &m_param.m_home);
    home.y = 0.0f;
    m_param.m_targetRadius = fn_8040CF50(&home);
    m_param.m_angle = fn_80016260(-home.x, -home.z);
    m_param.m_targetAngle = m_param.m_angle;
    m_param.m_bombLayer = this;
    fn_803358D0(&m_param, new HeavyDogCtrlUnit_AttackCallbackBody(&m_param, 0));
    fn_803358D0(&m_param, new HeavyDogCtrlUnit_AttackCallbackCanopy(&m_param, 1));
    fn_803358D0(&m_param, new HeavyDogCtrlUnit_GetStatus(&m_param));
    fn_803358D0(&m_param, new HeavyDog0510ModeCtrlUnit(&m_param));
    fn_803358D0(&m_param, new HeavyDog0510CtrlUnit_Move(&m_param));
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
    fn_803358D0(&m_param, new HeavyDog0510CtrlUnit_FixLookCharCamera(&m_param));
    fn_803358D0(&m_param, new BossCtrlUnit_CheckFirstDamage(&m_param, 0xED, 0));
    fn_803358D0(&m_param, new HeavyDogCtrlUnit_CheckLauncherNum(&m_param, 0xBF8));
    fn_803358D0(&m_param, new HeavyDogCtrlUnit_CheckMotionAttribute(&m_param));
    fn_803358D0(&m_param, new HeavyDogCtrlUnit_DispCanopy(&m_param, 6));
    fn_803358D0(&m_param, new HeavyDogCtrlUnit_DispEnergy(&m_param, 7));
    fn_803358D0(&m_param, new BossCtrlUnit_Shadow(&m_param, m_param.field_0xE4));
    if (m_flag) {
        m_param.m_nextMode = 2;
    } else {
        m_param.m_nextMode = 6;
    }
}

HeavyDog0510VarParam::~HeavyDog0510VarParam() {}

HeavyDog0510::~HeavyDog0510() {}
