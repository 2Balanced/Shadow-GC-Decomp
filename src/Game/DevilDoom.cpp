/* DevilDoom (class name from RTTI): the task of the DEVIL_DOOM gadget, Devil Doom, the final
 * boss. It is a BossRootTask that keeps the boss's DevilDoomVarParam (also defined here) and, in
 * Init, gives it its tables and the control units of Game/DevilDoomCtrlUnit.cpp and
 * Game/DevilDoomModeCtrlUnit.cpp. The gadget type's load and unload callbacks load and release the
 * boss's data file and its effects. File name inferred from the class. Names marked "(name
 * inferred)" and all member names are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first); the units' inline constructors are
 * emitted after Init, their first user. */
#define BOOST_SP_CONVERTING_ASSIGNMENT
#include "Game/BossCtrlUnit.h"
#include "Game/BossDef.h"
#include "Game/BossRootTask.h"
#include "Game/DevilDoomCtrlUnit.h"
#include "Game/EffectPJSLauncher.h"
#include "Game/Gadget.h"
#include "Game/GadgetClassDesc.h"
#include "Game/ResourceOneFile.h"
#include "Game/MathConstants.h"
#include "Game/Task.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

extern "C" {
/* Places the boss at its gadget owner (behaviour inferred from the body). */
void fn_8027DE28(BossRootTask *task);
/* The PJS effects of slot index (see Game/EffectPJS.cpp). */
boost::shared_ptr<Effect::PJS::Launcher> fn_801D2CA4(int index);
}

/* The boss's effects (defined in another unit). */
extern boost::shared_ptr<SonicteamUSA::System::Effect::EffectLaunchInfoUSA> lbl_805F0240;

/* BossVarParam's destructor is inline: this unit emits a weak copy (the linked one is at
 * 0x80232440) and, with it, BossVarParam's vtable and RTTI, which puts BossVarParam's RTTI name
 * before DevilDoomVarParam's in .rodata. The header declares it out of line, since defining it
 * there would renumber the other units' compiler-generated @N locals. */
inline BossVarParam::~BossVarParam() {}

inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* Never used; it starts the unit's .bss. */
static u8 s_unused[0x30];
/* The boss's collision spheres and its target (names inferred). */
static UnkSphere_80035CEC s_smallSphere(Vec3f(0.0f, 0.0f, 0.0f), 10.0f);
static UnkSphere_80035CEC s_largeSphere(Vec3f(0.0f, 0.0f, 0.0f), 35.0f);
static Vec3f s_target(0.0f, 0.0f, 0.0f);
/* Never used; it ends the unit's .bss. */
static u8 s_unused2[0x94];

/* The shapes of the three collisions, each ended by an empty entry (see Game/BossDef.h). */
static const UnkBossColliShape s_colliShapes[6] = {
    {0, &s_smallSphere, 0x100000, 0x403}, {0},
    {0, &s_smallSphere, 0x100000, 0x3F9}, {0},
    {0, &s_largeSphere, 0x80, 0x3E9},     {0},
};

static UnkBossColli s_collis[4] = {
    {&s_colliShapes[0], -1, 4, 1.0f, 1, 1, 0, 1, 1},
    {&s_colliShapes[2], -1, 4, 1.0f, 1, 1, 0, 1, 1},
    {&s_colliShapes[4], -1, 4, 1.0f, 1, 1, 0, 1, 1},
    {0, -1, 0, 1.0f, 6, 0, 0, 6, 0},
};

static UnkBossTargetEntry s_targets[3] = {
    {0x2C00, 0x403, &s_target},
    {0x2C00, 0x3F9, &s_target},
    {0, 0, 0},
};

static UnkBossDef s_def = {3, 5, 0xD, 1, 2, s_collis, s_targets, 0};

/* DEVIL_DOOM has no parameters: its table holds only the end entry. */
static GadgetParamDesc s_params[1] = {
    {0},
};

/* The gadget type's callbacks (defined at the end). */
static void Load();
static void Unload();
static void Instantiate(UnkPlacement_800C9CD0 *owner);
static void fn_8030C568();

/* The DEVIL_DOOM type record (a central table references it). Its word at 0x10 is a callback
 * (an empty one here), where GadgetClassDesc has a plain word. */
struct DevilDoomTypeInfo {
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
SIZE_ASSERT(DevilDoomTypeInfo, 0x24);

DevilDoomTypeInfo lbl_8055B7A4 = {
    "DEVIL_DOOM", Load, Unload, Instantiate, fn_8030C568, 0, 0xBE, 2, 0, 0xA, s_params,
};

/* DevilDoom (class name from RTTI; vtable at 0x8055B7E8): the boss task. */
class DevilDoom : public BossRootTask {
  public:
    DevilDoom(Task *parent, UnkPlacement_800C9CD0 *owner, bool flag);
    virtual ~DevilDoom();
    virtual void Init();

    /* 0x38 */ DevilDoomVarParam m_param;
};
SIZE_ASSERT(DevilDoom, 0x670);

/* Loads the boss's data file into resource slot 0x16 and its effects (name inferred). */
static void Load() {
    fn_8001299C(fn_800122D4(), 0x16, new ResourceOneFile("enemy/boss/DevilDoomOne.one", false));
    lbl_805F0240 = fn_801D2CA4(3);
}

/* Releases the boss's data and effects (name inferred). */
static void Unload() {
    fn_800127F8(fn_800122D4(), 0x16);
    lbl_805F0240.reset();
}

DevilDoomSettings::DevilDoomSettings() {
    field_0x0 = 30.0f;
    field_0x4 = 1000.0f;
    m_score = 500;
    field_0xC = 280.0f;
    field_0x10 = 120.0f;
    m_eyeSpeed = 360.0f;
    m_eyeOpenSpeed = 3.0f;
    m_neckSpeed = 2.0f;
    field_0x20 = 2.7f;
    m_minDistance = 100.0f;
    m_maxDistance = 3000.0f;
    field_0x2C = 2.0f;
    field_0x30 = 15.0f;
    field_0x34 = 2.0f;
    field_0x38 = -1.0f;
    field_0x3C = 4;
    field_0x40 = -300.0f;
    field_0x44 = -200.0f;
    field_0x48 = 100.0f;
    field_0x4C = -50.0f;
    field_0x50 = 50.0f;
    field_0x54 = 250.0f;
    field_0x58 = 500.0f;
    field_0x5C = 45.0f;
    field_0x60 = 3.0f;
    field_0x64 = 30.0f;
    field_0x68 = 1.5f;
    field_0x6C = 1.2f;
    field_0x70 = 250.0f;
    field_0x74 = 400.0f;
    field_0x78 = 5.0f;
    field_0x7C = -200.0f;
    field_0x80 = 3.0f;
    field_0x84 = 6.0f;
    field_0x88 = 800;
    field_0x8C = 2.0f;
    field_0x90 = 10.0f;
    field_0x94 = 2.0f;
    field_0x98 = 1.0f;
    field_0x9C = 1.5f;
    field_0xA0 = 800.0f;
    field_0xA4 = 0.3f;
    field_0xA8 = 100.0f;
    field_0xAC = 100.0f;
    field_0xB0 = 0.2f;
    field_0xB4 = 60.0f;
    field_0xB8 = 12.0f;
    field_0xBC = 2.0f;
    field_0xC0 = 300.0f;
    field_0xC4 = 75.0f;
    field_0xC8 = 1.0f;
    field_0xCC = 3.0f;
    field_0xD0 = 300.0f;
    field_0xD4 = 15.0f;
    field_0xD8 = 2.0f;
    field_0xDC = 0.1f;
    field_0xE0 = 400.0f;
    field_0xE4 = 1;
    field_0xEC = 600.0f;
    field_0xF0 = 5.0f;
    field_0xF4 = 800.0f;
    m_wingAngle = 10.0f;
    m_wingTime = 200.0f;
    field_0x100 = -5.0f;
    field_0x104 = 120.0f;
    field_0x108 = 90.0f;
    field_0x10C = 0.2f;
    field_0x110 = 0.7f;
    field_0x114 = 0.5f;
}

/* BossVarParam::m_chara is the px of a shared_ptr<BossCharInfo> (see BossCtrlUnit.h). */
inline boost::shared_ptr<BossCharInfo> &CharInfoPtr(BossVarParam *param) {
    return *reinterpret_cast<boost::shared_ptr<BossCharInfo> *>(&param->m_chara);
}

DevilDoomVarParam::DevilDoomVarParam() {
    CharInfoPtr(this) = boost::shared_ptr<BossCharInfo>(new BossCharInfo(0xAE));
    m_health = 0.0f;
    AssignVector(field_0x48, lbl_805E07A0);
    m_fixed = 1;
    field_0x18D = 0;
    m_turnSpeed = 0.0f;
    m_attribute = 0;
    field_0x18E = 0;
    m_inverted = false;
    m_flipped = false;
    m_eyeOpen[0] = 1;
    m_eyeOpen[1] = 0;
    field_0x193 = 0;
    m_eyeClosing = 1;
    m_neckUp = 1;
    field_0x199 = 0;
    m_wingsActive = 0;
    m_eyeAngle[0] = 0.0f;
    m_eyeAngle[1] = lbl_805FAD38;
    field_0x19C = 0;
    field_0x1A4 = 0.0f;
    field_0x210 = 0;
    field_0x20C = 0;
    field_0x628 = 0;
    field_0x62C = 0.0f;
}

DevilDoom::DevilDoom(Task *parent, UnkPlacement_800C9CD0 *owner, bool flag)
    : BossRootTask(parent, owner, flag, &m_param) {}

/* Creates the boss's task under task layer 14 (name inferred). */
static void Instantiate(UnkPlacement_800C9CD0 *owner) {
    new DevilDoom(fn_801EB304(fn_8001C4F8(), 0xE), owner, false);
}

/* The type record's callback at 0x10 (meaning unknown). */
static void fn_8030C568() {}

inline DevilDoomModeCtrlUnit::DevilDoomModeCtrlUnit(DevilDoomVarParam *owner)
    : BossCtrlUnit(owner), DevilDoomCtrlUnit(owner), BossModeCtrlUnit(owner) {}

void DevilDoom::Init() {
    fn_8027DE28(this);
    m_param.m_def = &s_def;
    new BossCtrl(this, &m_param);
    m_param.m_maxHealth = 200.0f;
    m_param.m_health = m_param.m_maxHealth;
    m_param.m_effects = lbl_805F0240;
    AssignVector(m_param.field_0x1BC, lbl_805E07A0);
    fn_803358D0(&m_param, new DevilDoomCtrlUnit_AttackCallbackEyeR(&m_param, 0));
    fn_803358D0(&m_param, new DevilDoomCtrlUnit_AttackCallbackEyeL(&m_param, 1));
    fn_803358D0(&m_param, new DevilDoomCtrlUnit_AttackCallbackChest(&m_param, 2));
    fn_803358D0(&m_param, new DevilDoomCtrlUnit_GetStatus(&m_param));
    fn_803358D0(&m_param, new DevilDoomModeCtrlUnit(&m_param));
    fn_803358D0(&m_param, new DevilDoomCtrlUnit_Move(&m_param));
    fn_803358D0(&m_param, new BossCtrlUnit_DispGauge(&m_param));
    fn_803358D0(&m_param, new BossCtrlUnit_CheckEvent(&m_param));
    fn_803358D0(&m_param, new DevilDoomCtrlUnit_FixLookCharCamera(&m_param));
    fn_803358D0(&m_param, new DevilDoomCtrlUnit_AdjustEye(&m_param));
    fn_803358D0(&m_param, new DevilDoomCtrlUnit_AdjustNeck(&m_param));
    fn_803358D0(&m_param, new DevilDoomCtrlUnit_CheckInterrupt(&m_param));
    fn_803358D0(&m_param, new DevilDoomCtrlUnit_BallControl(&m_param));
    fn_803358D0(&m_param, new DevilDoomCtrlUnit_WingControl(&m_param));
    fn_803358D0(&m_param, new DevilDoomCtrlUnit_Cloud(&m_param));
    fn_803358D0(&m_param, new BossCtrlUnit_CheckFirstDamage(&m_param, 0x1A5, 0));
    fn_803358D0(&m_param, new DevilDoomCtrlUnit_CheckMotionAttribute(&m_param));
    fn_803358D0(&m_param, new DevilDoomCtrlUnit_AdjustTarget(&m_param));
    if (m_flag) {
        m_param.m_nextMode = 2;
    } else {
        m_param.m_nextMode = 0xC;
    }
}

DevilDoomVarParam::~DevilDoomVarParam() {}

DevilDoom::~DevilDoom() {}
