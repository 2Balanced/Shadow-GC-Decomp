/* EggMeca411 (class name from RTTI): the task of the EGG_MECA_0411 gadget, the Egg Breaker boss of
 * stage 411. It is the stage-511 boss (Game/EggMeca511.cpp) with other tuning and control units:
 * a BossRootTask that keeps an EggMecaVarParam and, in Init, gives it its tables and its control
 * units. EggMecaVarParam's functions and the inline control-unit constructors are out of line in
 * Game/EggMeca511.cpp. File name inferred from the class. Names marked "(name inferred)" and all
 * member names are not original. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#define BOOST_SP_CONVERTING_ASSIGNMENT
#define BOSSVARPARAM_CHARA_SHARED_PTR
#include "Game/BossCtrlUnit.h"
#include "Game/BossDef.h"
#include "Game/BossRootTask.h"
#include "Game/EffectPJSLauncher.h"
#include "Game/EggMecaCtrlUnit.h"
#include "Game/Gadget.h"
#include "Game/GadgetClassDesc.h"
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

/* Not called anywhere: the linker strips it from the game. Retail pools the static initializers'
 * float literals as 0, 1, 25, after Init's; GC/2.7 pools them in first-use order (0, 25, 1). An
 * out-of-line function using them in retail's order, defined first so that it is emitted last,
 * reproduces retail's pool (see docs/research-notes.md; inferred from the order of the literals). */
void EggMeca411PoolOrder(float *out) {
    out[0] = 0.0f;
    out[1] = 1.0f;
    out[2] = 25.0f;
}

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

/* EGG_MECA_0411 has no parameters: its table holds only the end entry. */
static GadgetParamDesc s_params[1] = {
    {0},
};

/* The gadget type's callbacks (defined at the end). */
static void Load();
static void Unload();
static void Instantiate(UnkPlacement_800C9CD0 *owner);
static void fn_802EA808();

/* The EGG_MECA_0411 type record (a central table references it); laid out as the other bosses'. */
struct EggMeca411TypeInfo {
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
SIZE_ASSERT(EggMeca411TypeInfo, 0x24);

EggMeca411TypeInfo lbl_8055994C = {
    "EGG_MECA_0411", Load, Unload, Instantiate, fn_802EA808, 0, 0xBA, 2, 0, 0xA, s_params,
};

/* EggMeca411 (class name from RTTI; vtable at 0x80559990): the boss task. */
class EggMeca411 : public BossRootTask {
  public:
    EggMeca411(Task *parent, UnkPlacement_800C9CD0 *owner, bool flag);
    virtual ~EggMeca411();
    virtual void Init();

    /* 0x38 */ EggMecaVarParam m_param;
};
SIZE_ASSERT(EggMeca411, 0x288);

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

EggMeca411::EggMeca411(Task *parent, UnkPlacement_800C9CD0 *owner, bool flag)
    : BossRootTask(parent, owner, flag, &m_param) {}

/* Creates the boss's task under task layer 14 (name inferred). */
static void Instantiate(UnkPlacement_800C9CD0 *owner) {
    new EggMeca411(fn_801EB304(fn_8001C4F8(), 0xE), owner, false);
}

/* The type record's callback at 0x10 (meaning unknown). */
static void fn_802EA808() {}

void EggMeca411::Init() {
    fn_8027DE28(this);
    m_param.m_def = &s_def;
    new BossCtrl(this, &m_param);
    m_param.m_maxHealth = 150.0f;
    m_param.m_settings.m_nearDistance = 200.0f;
    m_param.m_settings.m_farDistance = 400.0f;
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
    fn_803358D0(&m_param, new BossCtrlUnit_CheckFirstDamage(&m_param, 0xC8, 0));
    fn_803358D0(&m_param, new EggMecaCtrlUnit_DispCanopy(&m_param, 0xD));
    fn_803358D0(&m_param, new EggMecaCtrlUnit_DispEggman(&m_param, 0xA));
    fn_803358D0(&m_param, new EggMecaCtrlUnit_DispEye(&m_param));
    fn_803358D0(&m_param, new EggMecaCtrlUnit_IronBall(&m_param));
    fn_803358D0(&m_param, new EggMecaCtrlUnit_RollUpper(&m_param));
    fn_803358D0(&m_param, new EggMecaCtrlUnit_DispAura(&m_param));
    fn_803358D0(&m_param, new EggMecaCtrlUnit_AdjustCircleArea(&m_param));
    if (m_flag) {
        m_param.m_nextMode = 2;
        m_param.m_active = 1;
    } else {
        m_param.m_nextMode = 0xE;
    }
    fn_801FAF00(GetHintManager(), 0xC7, 0, 0, 0);
}

EggMeca411::~EggMeca411() {}
