/* BlackBull0210 (class name from RTTI): the task of the BLACK_BULL_0210 gadget, the first stage
 * of the Black Bull boss. It is a BossRootTask that keeps the boss's BlackBull0210VarParam and, in
 * Init, gives it its tables and its control units (Game/BlackBull0210CtrlUnit.cpp,
 * Game/BlackBull0210ModeCtrlUnit.cpp and the shared Black Bull units). The gadget type's load and
 * unload callbacks load and release the boss's data file and its effects (as in
 * Game/DevilDoom.cpp). Being the first boss unit, it also emits the copies of the shared boss
 * classes' inline functions that the game links: BossVarParam's constructor and destructor,
 * BossCharInfo's, and BlackBullCtrlUnit_AttackCallback's. File name inferred from the class. Names
 * marked "(name inferred)" and all member names are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#define BOOST_SP_CONVERTING_ASSIGNMENT
#define BOSSVARPARAM_CHARA_SHARED_PTR
#include "Game/BlackBull0210CtrlUnit.h"
#include "Game/BlackBullCtrlUnit.h"
#include "Game/BlackBullCtrlUnits.h"
#include "Game/BossCtrlUnit.h"
#include "Game/BossDef.h"
#include "Game/BossRootTask.h"
#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/EffectPJSLauncher.h"
#include "Game/Gadget.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathTypes.h"
#include "Game/ResourceOneFile.h"
#include "Game/Task.h"
#include "boost/detail/sp_counted_base_impl.hpp"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

extern "C" {
/* Places the boss at its gadget owner (see Game/DevilDoom.cpp). */
void fn_8027DE28(BossRootTask *task);
/* The PJS effects of slot index (see Game/EffectPJS.cpp). */
boost::shared_ptr<Effect::PJS::Launcher> fn_801D2CA4(int index);
/* Hands command to the command unit (see Game/GadgetCharInfo.cpp). */
void fn_801D9C0C(boost::shared_ptr<CharCommandUnit> unit, CharCommand *command);
}

/* The boss's effects (defined in another unit). */
extern boost::shared_ptr<SonicteamUSA::System::Effect::EffectLaunchInfoUSA> lbl_805F0240;

/* The game state record (class unknown); the word at 0x38 is the difficulty (meaning inferred). */
extern u8 lbl_8057D6A0[];

/* AttackCallbackContainer's handlers do nothing (see Game/BossCtrlUnit.h); this unit emits the
 * linked weak copies. */
inline void AttackCallbackContainer::OnDamage(
    SonicteamUSA::System::CharaColli::AttackCallbackParam &param) {}

inline void AttackCallbackContainer::OnEnd(
    SonicteamUSA::System::CharaColli::AttackCallbackParam &param) {}

inline void AttackCallbackContainer::OnHit(
    SonicteamUSA::System::CharaColli::AttackCallbackParam &param) {}

/* Inline, so that the game's copy is a weak one; it reads the pointer directly, since an inline
 * function's inline calls stay out of line here. */
inline void BossCharInfo::PushCommand(CharCommand *command) {
    if (m_commandUnit.px != 0) {
        fn_801D9C0C(m_commandUnit, command);
    } else {
        delete command;
    }
}

/* The weak copies the game links (see the file comment). The headers declare these out of line,
 * since defining them there would renumber the other units' compiler-generated @N locals. */
inline BossVarParam::~BossVarParam() {}

inline BossVarParam::BossVarParam()
    : m_maxHealth(100.0f), m_mode(0), field_0x10(0), m_nextMode(0), m_stopped(0), m_invincible(0),
      m_active(1), m_hidden(0), m_time(0.0f), m_clump(0), m_fade(0), field_0x34(0), m_motion(0),
      field_0x40(1.0f), field_0x48(lbl_805E07A0), m_colliders(0), field_0x58(1.0f, 1.0f, 1.0f),
      m_targets(0), field_0x68(0) {}

inline BossCharInfo::BossCharInfo(int type) : CharInfo(type) {
    field_0x5C = 3;
    m_commandUnit = boost::shared_ptr<CharCommandUnit>(new CharCommandUnit);
}

inline BossCharInfo::~BossCharInfo() {}

inline BlackBullVarParam::BlackBullVarParam() {
    field_0x74 = 0.55f;
    field_0x78 = 1.2f;
    m_arriveDistance = 20.0f;
    field_0x80 = 1.5f;
    field_0x84 = 1.5f;
    m_turnMotionSpeed = 10.0f;
    m_waitTime = 5.0f;
    m_stunTime = 0.5f;
    m_score = 500;
    m_comboLimit = 3;
    field_0x9C = 3.0f;
    field_0xA0 = 2.0f;
    field_0xA4 = 200.0f;
    field_0xA8 = 10.0f;
    field_0xAC = 0.0f;
    field_0xB0 = 80.0f;
    field_0xB4 = 300.0f;
    field_0xB8 = 200.0f;
    field_0xBC = 15.0f;
    field_0xC0 = 40.0f;
    m_waveCount = 3;
    m_waveBlend = 80.0f;
    field_0xCC = 20.0f;
    m_fireSpeed = 180.0f;
    field_0xD4 = 700.0f;
    field_0xD8 = 50.0f;
    field_0xDC = 280.0f;
    m_turnSpeedMax = 30.0f;
    field_0xE4 = 40.0f;
    m_damageLimit = 10.0f;
    field_0xEC = 5.0f;
    field_0xF0 = 30.0f;
    m_cameraNear = 100.0f;
    m_cameraFar = 500.0f;
    m_charaPtr = boost::shared_ptr<BossCharInfo>(new BossCharInfo(0xA8));
    field_0x16C = -31.0f;
    field_0x170 = 1;
    field_0x171 = 1;
    field_0x168 = 0;
    m_hit = 0;
    m_motionLocked = 0;
    m_motionLocked2 = 0;
    field_0x122 = 0;
    m_moving = 1;
    m_arrived = 0;
    m_dizzyEnded = 0;
    m_dizzy = 0;
    m_comboCount = 0;
    m_damage = 0.0f;
    field_0x178 = 0.0f;
    m_sound = -1;
    m_attribute = 0;
}

inline BlackBullVarParam::~BlackBullVarParam() {}

/* Never used; it starts the unit's .bss. */
static u8 s_unused[0x30];
/* The boss's collision spheres, its target and the point its camera looks at (names
 * inferred). */
static UnkSphere_80035CEC s_bodySphere(Vec3f(0.0f, -10.0f, 0.0f), 45.0f);
static UnkSphere_80035CEC s_eyeSphere(Vec3f(0.0f, 0.0f, 0.0f), 25.0f);
static UnkSphere_80035CEC s_otherSphere(Vec3f(0.0f, 0.0f, 0.0f), 15.0f);
static Vec3f s_target(0.0f, 0.0f, 0.0f);
static Vec3f s_focusPoint(0.0f, 0.0f, 0.0f);
/* Never used; it ends the unit's .bss. */
static u8 s_unused2[0x98];

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

static UnkBossFocus s_focus = {&s_focusPoint, 50.0f};

static UnkBossDef s_def = {3, 4, 5, 1, 2, s_collis, s_targets, &s_focus};

/* BLACK_BULL_0210 has no parameters: its table holds only the end entry. */
static GadgetParamDesc s_params[1] = {
    {0},
};

/* The gadget type's callbacks (defined at the end). */
static void Load();
static void Unload();
static void Instantiate(UnkPlacement_800C9CD0 *owner);
static void fn_80232CA8();

/* The BLACK_BULL_0210 type record (a central table references it). Its word at 0x10 is a
 * callback (an empty one here), where GadgetClassDesc has a plain word (as in
 * Game/DevilDoom.cpp). */
struct BlackBull0210TypeInfo {
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
SIZE_ASSERT(BlackBull0210TypeInfo, 0x24);

BlackBull0210TypeInfo lbl_80547374 = {
    "BLACK_BULL_0210", Load, Unload, Instantiate, fn_80232CA8, 0, 0xB4, 2, 0, 0xA, s_params,
};

/* BlackBull0210 (class name from RTTI; vtable at 0x805473B8): the boss task. */
class BlackBull0210 : public BossRootTask {
  public:
    BlackBull0210(Task *parent, UnkPlacement_800C9CD0 *owner, bool flag);
    virtual ~BlackBull0210();
    virtual void Init();

    /* 0x38 */ BlackBull0210VarParam m_param;
};
SIZE_ASSERT(BlackBull0210, 0x1C8);

/* Loads the boss's data file into resource slot 0x16 and its effects (name inferred). */
static void Load() {
    fn_8001299C(fn_800122D4(), 0x16, new ResourceOneFile("enemy/boss/BlackBull0210One.one", false));
    lbl_805F0240 = fn_801D2CA4(3);
}

/* Releases the boss's data and effects (name inferred). */
static void Unload() {
    fn_800127F8(fn_800122D4(), 0x16);
    lbl_805F0240.reset();
}

BlackBull0210VarParam::BlackBull0210VarParam() {
    m_point = -1;
}

BlackBull0210::BlackBull0210(Task *parent, UnkPlacement_800C9CD0 *owner, bool flag)
    : BossRootTask(parent, owner, flag, &m_param) {}

/* Creates the boss's task under task layer 14 (name inferred). */
static void Instantiate(UnkPlacement_800C9CD0 *owner) {
    new BlackBull0210(fn_801EB304(fn_8001C4F8(), 0xE), owner, false);
}

/* The type record's callback at 0x10 (meaning unknown). */
static void fn_80232CA8() {}

inline BlackBull0210ModeCtrlUnit::BlackBull0210ModeCtrlUnit(BlackBull0210VarParam *owner)
    : BossCtrlUnit(owner), BlackBull0210CtrlUnit(owner), BossModeCtrlUnit(owner) {}

void BlackBull0210::Init() {
    fn_8027DE28(this);
    m_param.m_def = &s_def;
    new BossCtrl(this, &m_param);
    m_param.m_maxHealth = 80.0f;
    if (*(s32 *)(lbl_8057D6A0 + 0x38) == 5) {
        m_param.m_maxHealth = 40.0f;
    }
    m_param.field_0xD8 = m_param.field_0xCC;
    m_param.field_0xAC = 45.0f;
    m_param.m_health = m_param.m_maxHealth;
    m_param.m_effects = lbl_805F0240;
    fn_800091D0(&m_param.m_destination,
                fn_800091D0(&m_param.m_position, &m_param.m_charaPtr.get()->m_position));
    m_param.m_moving = 1;
    fn_803358D0(&m_param, new BlackBullCtrlUnit_AttackCallbackBody(&m_param, 0));
    fn_803358D0(&m_param, new BlackBullCtrlUnit_AttackCallbackEye(&m_param, 1));
    fn_803358D0(&m_param, new BlackBullCtrlUnit_AttackCallbackOther(&m_param, 2));
    fn_803358D0(&m_param, new BlackBullCtrlUnit_GetStatus(&m_param));
    fn_803358D0(&m_param, new BlackBull0210ModeCtrlUnit(&m_param));
    fn_803358D0(&m_param, new BlackBullCtrlUnit_Move(&m_param));
    fn_803358D0(&m_param, new BlackBullCtrlUnit_AdjustEye(&m_param));
    fn_803358D0(&m_param, new BlackBullCtrlUnit_AdjustNeck(&m_param));
    fn_803358D0(&m_param, new BlackBull0210CtrlUnit_MoveMotion(&m_param));
    fn_803358D0(&m_param, new BlackBull0210CtrlUnit_FixLookCharCamera(&m_param));
    fn_803358D0(&m_param, new BossCtrlUnit_CheckFirstDamage(&m_param, 0xA5, 2));
    fn_803358D0(&m_param, new BlackBullCtrlUnit_CheckMotionAttribute(&m_param));
    fn_803358D0(&m_param, new BossCtrlUnit_DispGauge(&m_param));
    fn_803358D0(&m_param, new BossCtrlUnit_CheckEvent(&m_param));
    fn_803358D0(&m_param, new BossCtrlUnit_Shadow(&m_param, m_param.field_0xE4));
    if (m_flag) {
        m_param.m_nextMode = 2;
    } else {
        m_param.m_nextMode = 0xE;
    }
}

BlackBull0210VarParam::~BlackBull0210VarParam() {}

BlackBull0210::~BlackBull0210() {}
