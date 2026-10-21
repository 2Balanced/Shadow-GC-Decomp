/* BlackBull0412 (class name from RTTI): the task of the BLACK_BULL_0412 gadget, the second stage
 * of the Black Bull boss. Like BlackBull0210 (Game/BlackBull0210.cpp) it is a BossRootTask that
 * keeps the boss's BlackBull0412VarParam and, in Init, gives it its tables and its control units
 * (Game/BlackBull0412CtrlUnit.cpp, Game/BlackBull0412ModeCtrlUnit.cpp and the shared Black Bull
 * units of Game/BlackBullCtrlUnit.cpp). It also defines the stage's unit base,
 * BlackBull0412CtrlUnit, whose constructor and destructor are inline here (retail emits them
 * after the mode unit's constructor and lays out their vtable before BlackBull0412VarParam's).
 * The shared boss classes' inline functions are linked from Game/BlackBull0210.cpp; this unit
 * defines BossVarParam's and BlackBullVarParam's destructors inline as well, as retail did: the
 * linker drops these copies, but they put the classes' RTTI names in retail's order. File name inferred from the class. Names marked "(name inferred)" and
 * all member names are not original. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#define BOOST_SP_CONVERTING_ASSIGNMENT
#define BOSSVARPARAM_CHARA_SHARED_PTR
#include "Game/BlackBull0412CtrlUnit.h"
#include "Game/BlackBullCtrlUnit.h"
#include "Game/BlackBullCtrlUnits.h"
#include "Game/BossCtrlUnit.h"
#include "Game/BossDef.h"
#include "Game/BossRootTask.h"
#include "Game/EffectPJSLauncher.h"
#include "Game/Gadget.h"
#include "Game/GadgetClassDesc.h"
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
}

/* The boss's effects (defined in another unit). */
extern boost::shared_ptr<SonicteamUSA::System::Effect::EffectLaunchInfoUSA> lbl_805F0240;

inline BossVarParam::~BossVarParam() {}

inline BlackBullVarParam::~BlackBullVarParam() {}

/* Never used; it starts the unit's .bss. */
static u8 s_unused[0x30];
/* The boss's collision spheres and its target (names inferred). */
static UnkSphere_80035CEC s_bodySphere(Vec3f(0.0f, -10.0f, 0.0f), 45.0f);
static UnkSphere_80035CEC s_eyeSphere(Vec3f(0.0f, 0.0f, 0.0f), 25.0f);
static UnkSphere_80035CEC s_otherSphere(Vec3f(0.0f, 0.0f, 0.0f), 15.0f);
static Vec3f s_target(0.0f, 0.0f, 0.0f);
/* Never used; it ends the unit's .bss. */
static u8 s_unused2[0x94];

/* The shapes of the three collisions, each ended by an empty entry. */
static const UnkBossColliShape s_bodyShapes[2] = {
    {0, &s_bodySphere, 0x80, 0x13EC},
    {0},
};
static const UnkBossColliShape s_eyeShapes[2] = {
    {0, &s_eyeSphere, 0x80, 0x1393},
    {0},
};
static const UnkBossColliShape s_otherShapes[3] = {
    {0, &s_otherSphere, 0, 0x13B2},
    {0, &s_otherSphere, 0, 0x13B5},
    {0},
};

static UnkBossColli s_collis[4] = {
    {s_bodyShapes, -1, 4, 1.0f, 1, 1, 1, 1, 1},
    {s_eyeShapes, -1, 4, 1.0f, 1, 1, 1, 1, 1},
    {s_otherShapes, -1, 4, 1.0f, 1, 1, 0, 1, 1},
    {0, -1, 0, 1.0f, 6, 0, 0, 6, 0},
};

static UnkBossTargetEntry s_targets[5] = {
    {0x800, 0x13EC, &s_target},
    {0x2C00, 0x1394, &s_target},
    {0x800, 0x13B2, &s_target},
    {0x800, 0x13B5, &s_target},
    {0, 0, 0},
};

static UnkBossDef s_def = {3, 4, 6, 1, 2, s_collis, s_targets, 0};

/* BLACK_BULL_0412 has no parameters: its table holds only the end entry. */
static GadgetParamDesc s_params[1] = {
    {0},
};

/* The gadget type's callbacks (defined at the end). */
static void Load();
static void Unload();
static void Instantiate(UnkPlacement_800C9CD0 *owner);
static void fn_80243ADC();

/* The BLACK_BULL_0412 type record (a central table references it); laid out as
 * BlackBull0210's. */
struct BlackBull0412TypeInfo {
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
SIZE_ASSERT(BlackBull0412TypeInfo, 0x24);

BlackBull0412TypeInfo lbl_80548404 = {
    "BLACK_BULL_0412", Load, Unload, Instantiate, fn_80243ADC, 0, 0xB5, 2, 0, 0xA, s_params,
};

/* BlackBull0412 (class name from RTTI; vtable at 0x80548448): the boss task. */
class BlackBull0412 : public BossRootTask {
  public:
    BlackBull0412(Task *parent, UnkPlacement_800C9CD0 *owner, bool flag);
    virtual ~BlackBull0412();
    virtual void Init();

    /* 0x38 */ BlackBull0412VarParam m_param;
};
SIZE_ASSERT(BlackBull0412, 0x1F0);

inline BlackBull0412CtrlUnit::BlackBull0412CtrlUnit(BlackBull0412VarParam *owner)
    : BossCtrlUnit(owner), BlackBullCtrlUnit(owner), m_owner(owner) {}

inline BlackBull0412CtrlUnit::~BlackBull0412CtrlUnit() {}

/* Loads the boss's data file into resource slot 0x16 and its effects (name inferred). */
static void Load() {
    fn_8001299C(fn_800122D4(), 0x16, new ResourceOneFile("enemy/boss/BlackBull0412One.one", false));
    lbl_805F0240 = fn_801D2CA4(3);
}

/* Releases the boss's data and effects (name inferred). */
static void Unload() {
    fn_800127F8(fn_800122D4(), 0x16);
    lbl_805F0240.reset();
}

BlackBull0412Settings::BlackBull0412Settings() {
    field_0x0 = 40.0f;
    field_0x4 = 95.0f;
    field_0x8 = 1000.0f;
    field_0xC = 20.0f;
    field_0x10 = -6.0f;
    field_0x14 = 2.0f;
    field_0x18 = 0.0f;
    field_0x1C = 6.0f;
    field_0x20 = 2.0f;
    field_0x24 = 0.0f;
    field_0x28 = 15.0f;
}

BlackBull0412VarParam::BlackBull0412VarParam() {}

BlackBull0412::BlackBull0412(Task *parent, UnkPlacement_800C9CD0 *owner, bool flag)
    : BossRootTask(parent, owner, flag, &m_param) {}

/* Creates the boss's task under task layer 14 (name inferred). */
static void Instantiate(UnkPlacement_800C9CD0 *owner) {
    new BlackBull0412(fn_801EB304(fn_8001C4F8(), 0xE), owner, false);
}

/* The type record's callback at 0x10 (meaning unknown). */
static void fn_80243ADC() {}

inline BlackBull0412ModeCtrlUnit::BlackBull0412ModeCtrlUnit(BlackBull0412VarParam *owner)
    : BossCtrlUnit(owner), BlackBull0412CtrlUnit(owner), BossModeCtrlUnit(owner) {}

void BlackBull0412::Init() {
    fn_8027DE28(this);
    m_param.m_def = &s_def;
    new BossCtrl(this, &m_param);
    m_param.m_maxHealth = 80.0f;
    m_param.field_0xAC = 0.0f;
    m_param.field_0xB8 = 500.0f;
    m_param.field_0xD4 = 500.0f;
    m_param.m_health = m_param.m_maxHealth;
    m_param.m_effects = lbl_805F0240;
    fn_800091D0(&m_param.m_destination,
                fn_800091D0(&m_param.m_position, &m_param.m_charaPtr.get()->m_position));
    m_param.m_moving = 0;
    fn_803358D0(&m_param, new BlackBullCtrlUnit_AttackCallbackBody(&m_param, 0));
    fn_803358D0(&m_param, new BlackBullCtrlUnit_AttackCallbackEye(&m_param, 1));
    fn_803358D0(&m_param, new BlackBullCtrlUnit_AttackCallbackOther(&m_param, 2));
    fn_803358D0(&m_param, new BlackBullCtrlUnit_GetStatus(&m_param));
    fn_803358D0(&m_param, new BlackBull0412ModeCtrlUnit(&m_param));
    fn_803358D0(&m_param, new BlackBullCtrlUnit_Move(&m_param));
    fn_803358D0(&m_param, new BlackBullCtrlUnit_AdjustEye(&m_param));
    fn_803358D0(&m_param, new BlackBullCtrlUnit_AdjustNeck(&m_param));
    fn_803358D0(&m_param, new BlackBull0412CtrlUnit_MoveMotion(&m_param));
    fn_803358D0(&m_param, new BlackBullCtrlUnit_CheckMotionAttribute(&m_param));
    fn_803358D0(&m_param, new BlackBull0412CtrlUnit_FixLookCharCamera(&m_param));
    fn_803358D0(&m_param, new BossCtrlUnit_CheckFirstDamage(&m_param, 0xB1, 2));
    fn_803358D0(&m_param, new BossCtrlUnit_DispGauge(&m_param));
    fn_803358D0(&m_param, new BossCtrlUnit_CheckEvent(&m_param));
    fn_803358D0(&m_param, new BossCtrlUnit_Shadow(&m_param, m_param.field_0xE4));
    if (m_flag) {
        m_param.m_nextMode = 2;
    } else {
        m_param.m_nextMode = 0xE;
    }
}

BlackBull0412VarParam::~BlackBull0412VarParam() {}

BlackBull0412::~BlackBull0412() {}
