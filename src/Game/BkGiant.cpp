/* BkGiant (namespace and class names from RTTI; its task's s_taskName is "BkGiant"): the Black
 * Arms giant, BK_GIANT in the stage editor (enemy/BkGiantData.one). Its body BkGiant::BkGiant is
 * an EnemyWalkerCommon (Game/Enemy.h) that waits or drops in (AppearType), carries a black sword,
 * a dark hammer or a big barrel (WeaponType) and, with Guard on, blocks hits from the front; its
 * AI is BkGiantAI (Game/BkGiantAI.cpp). File name inferred from the class. Names marked "(name
 * inferred)" are not original. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
/* See EnemyReferer (Game/Enemy.h): keeps EnemyTemplate's destructor out of line, as in EggPawn.cpp. */
#define ENEMY_REFERER_DTOR

#include "Game/BkGiant.h"
#include "Game/CharaColliAttack.h"
#include "Game/EnemyRender.h"
#include "Game/EnemyWeapon.h"
#include "Game/Gadget.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathTypes.h"
#include "Game/Random.h"
#include "Game/ResourceOneFile.h"
#include "Game/Vibration.h"
#include "Effect/System/EffectElement.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using Effect::System::EffectElement;

/* MSL's std::fabs for floats, which the game calls out of line (as in BkSoldier.cpp). */
namespace std {
float fabs(float x);
}

namespace BkGiant {

/* BkGiant::BkGiantTask (class name from RTTI; vtable 0x8054FCE0): the task that runs a giant. */
class BkGiantTask : public EnemyTemplate<BkGiant, BkGiantAI> {
  public:
    BkGiantTask(Task *parent, UnkPlacement_800C9CD0 *placement);
    virtual ~BkGiantTask();

    /* The stage editor's hooks (names inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *placement);
    static void LoadFiles();
    static void DropFiles();

    static const char *s_taskName;
};
SIZE_ASSERT(BkGiantTask, 0x360);

} // namespace BkGiant

/* The cursor a parameter's label callback receives (layout as in Helicopter.cpp). */
class UnkLabelQuery_800CBC20 {
  public:
    /* 0x0 */ const GadgetParamDesc *params;
    /* 0x4 */ s32 index; /* the parameter being shown */
    /* 0x8 */ s32 value; /* its current value */
};

/* The camera manager (see Game/CameraManagerUnit.h). */
class UnkCameraManager_80009548;

extern "C" {
/* The enemies' shared helpers (behaviour inferred). */
void fn_8019AA40(Enemy *enemy, u16 sound, int arg);
void fn_8019A954(Enemy *enemy, u16 sound, int arg);
void fn_8017CA2C(Enemy *enemy);
/* The angle from enemy to position, and the yaw that faces it (see BkSoldier.cpp and
 * BkWormAI.cpp). */
float fn_8019B184(Enemy *enemy, const Vec3f *position);
float fn_8019B1D8(Enemy *enemy, const Vec3f *position);
/* Resources and models (see BkSoldier.cpp). */
void *fn_80012684(void *loader, int slot, int kind);
void *fn_80012608(void *loader, int slot, int kind);
boost::shared_ptr<EnemyRender> fn_802CDB08(int slot, const EnemyMotionTable &table, void *model,
                                           void *motion);
void *fn_801A46E4(void *model, int part);
/* An effect at a matrix (see BkWorm.cpp). */
boost::weak_ptr<EffectElement> fn_801A1ED8(const Mtx34f *matrix, int id);
/* Rumble and a camera shake (see BkWorm.cpp). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DE08(int pattern, const Vec3f *position,
                                                      int arg);
UnkCameraManager_80009548 *fn_80009548(void);
void fn_8000D8EC(UnkCameraManager_80009548 *manager, int arg1, int arg2, float strength);
/* Vector routines (see GunRobot.cpp). */
Vec3f fn_8040D0AC(const Vec3f *v, float angle);
}

extern float lbl_805FAD38; /* pi */

/* A motion table built in place (not original; as in BkWorm.cpp). */
struct MotionTable : public EnemyMotionTable {
    MotionTable(const EnemyMotionEntry *e, s32 c) {
        entries = e;
        count = c;
    }
};

/* Fires the weapon once (name inferred; as GunRobot.cpp's EnemyWeapon::Fire). */
inline void Fire(EnemyWeapon *weapon) {
    weapon->field_0x0 = 0;
    fn_801AB3F0(weapon);
}

/* Three floats copied as a plain struct (not original). */
struct Vec3fWords {
    float x;
    float y;
    float z;
};

/* The motion table (steps in .sdata2 and .rodata). */
static const MotionSequenceStep lbl_805F8358[] = {{7, -1, -1, 20, 1}};
static const MotionSequenceStep lbl_805F8360[] = {{1, -1, -1, 10, 1}};
static const MotionSequenceStep lbl_804F89E0[] = {{5, -1, -1, 30, 2}, {6, -1, -1, 10, 0}};
static const MotionSequenceStep lbl_804F89F0[] = {{10, -1, -1, 10, 0}, {10, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_805F8368[] = {{3, -1, -1, 20, 0}};
static const MotionSequenceStep lbl_805F8370[] = {{2, -1, -1, 20, 0}};
static const MotionSequenceStep lbl_804F8A00[] = {{12, -1, -1, 20, 0}, {11, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_805F8378[] = {{4, -1, -1, 10, 0}};
static const MotionSequenceStep lbl_805F8380[] = {{8, -1, -1, 30, 0}};
static const MotionSequenceStep lbl_805F8388[] = {{9, -1, -1, 30, 0}};
static const MotionSequence lbl_804F8A10[] = {
    {lbl_805F8358, 1, -3}, {lbl_805F8360, 1, -3}, {lbl_804F89E0, 2, -3}, {lbl_804F89F0, 2, -3},
    {lbl_805F8368, 1, -3}, {lbl_805F8370, 1, -3}, {lbl_804F8A00, 2, -3}, {lbl_805F8378, 1, -3},
    {lbl_805F8380, 1, -3}, {lbl_805F8388, 1, -3},
};
static const MotionSequenceTable lbl_805F8390 = {lbl_804F8A10, 10};

/* The model's motion table. */
static const EnemyMotionEntry lbl_805F8398[] = {{3, -1}};

/* The weapon types the WeaponType parameter selects. */
static const s32 lbl_804F8A60[] = {0x1F, 0x20, 0x10};

namespace BkGiant {

const char *BkGiantTask::s_taskName = "BkGiant";

BkGiant::BkGiant() : EnemyWalkerCommon(4), m_weapon(0), m_voiceTime(fn_8040EC80(8.0f, 12.0f)) {
    m_enemy = this;
    vfPara_0x40(reinterpret_cast<int>(&lbl_805F8390));
    m_hits = 0;
    m_hitTime = 0.0f;
}

BkGiant::~BkGiant() {
    if (m_weapon != 0) {
        delete m_weapon;
        m_weapon = 0;
    }
}

void BkGiant::vfSet_0x1C(UnkPlacement_800C9CD0 *placement) {
    vfSet_0x20(placement);
    /* Copied as a plain struct (word loads and stores), unlike Vec3f's own copy. */
    Vec3fWords rotation = *reinterpret_cast<Vec3fWords *>(GetRotation());
    vfDisp_0x38(GetPosition(), reinterpret_cast<Vec3f *>(&rotation));
    m_ctrl->vfCtrl_0x20();
}

void BkGiant::vfSet_0x20(UnkPlacement_800C9CD0 *placement) {
    TEnemySetBase::vfSet_0x20(placement);
    fn_800C9DBC(placement, &m_params, sizeof(m_params));
    if (m_params.AppearType == 1) {
        GetPosition()->y += m_params.OffsetPos_Y;
    }
}

void BkGiant::vfStat_0x38() {
    if (m_params.AppearType == 1 && !field_0xA8.Contains(m_ctrl->vfCtrl_0x10())) {
        m_status->vfStat_0x3C(true);
        m_status->vfStat_0x30(false);
        if (m_weapon != 0) {
            fn_801AA830(m_weapon);
        }
    } else {
        if (m_weapon != 0) {
            fn_801AA868(m_weapon);
        }
        m_status->vfStat_0x3C(false);
        m_status->vfStat_0x30(true);
    }
}

void BkGiant::vfDisp_0x64() {
    void *loader = fn_800122D4();
    int slot = m_motionBase;
    void *model = fn_80012684(loader, slot, 4);
    void *motion = fn_80012608(loader, slot, 6);
    m_render = fn_802CDB08(slot, MotionTable(lbl_805F8398, 1), model, motion);
}

void BkGiant::vfDisp_0x08() {
    EnemyDispCommon::vfDisp_0x08();
    if (m_weapon == 0) {
        m_weapon = new EnemyWeapon(fn_8017D014(this), lbl_804F8A60[GetWeaponType()]);
    }
}

void BkGiant::vfStat_0xA0() {
    EnemyStatusCommon::vfStat_0xA0();
    fn_8019A954(this, 0x6035, 1);
    if (m_weapon != 0) {
        float y = GetRotation()->y;
        if (m_enemy->m_status->vfStat_0x74() == 1) {
            y = -y;
        }
        Vec3f offset;
        fn_800091EC(&offset, &(const Vec3f &)fn_8040D0AC(&(const Vec3f &)Vec3f(0.0f, 20.0f, 10.0f), y));
        fn_801AA8A0(m_weapon, &offset);
        delete m_weapon;
        m_weapon = 0;
    }
}

void BkGiant::vfDisp_0x10(float dt) {
    EnemyWalkerCommon::vfDisp_0x10(dt);
    if ((m_hitTime += dt) > 10.0f) {
        m_hits = 0;
    }
    if (vfStat_0x4C() && !vfStat_0x60()) {
        fn_8019A954(this, 0x6034, 1);
    }
    if ((m_voiceTime -= dt) < 0.0f) {
        m_voiceTime = fn_8040EC80(8.0f, 12.0f);
        if (field_0xA8.Contains(m_ctrl->vfCtrl_0x10())) {
            fn_8019AA40(this, 0x6033, 1);
        } else {
            fn_8019AA40(this, 0x6032, 1);
        }
    }
    if (m_weapon != 0) {
        Mtx34f *part = static_cast<Mtx34f *>(fn_801A46E4(m_disp->vfDisp_0x1C(), 0x1C));
        fn_801AB590(m_weapon, part);
        fn_801AA8E4(m_weapon, field_0xEC);
        m_weapon->Tick(dt);
    }
}

void BkGiant::vfDisp_0x40(EnemyMotionEventArg &arg) {
    EnemyDispCommon::vfDisp_0x40(arg);
    if (m_weapon != 0 && arg.m_type == 0x5000 && arg.m_data->kind == 2) {
        Fire(m_weapon);
    }
}

bool BkGiant::vfDisp_0x44(int *command) {
    if (EnemyDispCommon::vfDisp_0x44(command)) {
        return true;
    }
    switch (*command) {
    case 21:
        fn_8000D8EC(fn_80009548(), 0, 0, 0.5f);
        fn_8034DE08(8, GetPosition(), -2);
        fn_8019AA40(this, 0x602C, 1);
        break;
    case 22:
        fn_8019AA40(this, 0x602D, 1);
        fn_8017CA2C(this);
        break;
    case 23:
        fn_8019AA40(this, 0x6030, 1);
        break;
    case 24:
        fn_8019AA40(this, 0x6031, 1);
        break;
    }
    return true;
}

float BkGiant::vfPara_0x6C() {
    if (m_params.WeaponType >= 1) {
        return 32.0f;
    }
    return 24.0f;
}

int BkGiant::vfPara_0x48() {
    return vfPara_0x54();
}

int BkGiant::vfPara_0x4C() {
    return 1;
}

int BkGiant::vfPara_0x50() {
    return 2;
}

int BkGiant::vfPara_0x54() {
    return 0;
}

int BkGiant::vfPara_0x58() {
    switch (m_params.WeaponType) {
    case 0:
        return 4;
    case 1:
        return 5;
    case 2:
        return 6;
    }
    return vfPara_0x48();
}

int BkGiant::vfPara_0x5C() {
    return 8;
}

int BkGiant::vfPara_0x60() {
    return 9;
}

int BkGiant::vfPara_0x64() {
    return 3;
}

void BkGiant::vfStat_0x70(AttackCallbackParam &param) {
    boost::shared_ptr<CharaColliAttack> attacker(param.m_attacker);
    if (attacker.get() != 0) {
        if (m_params.Guard != 0 && (attacker->m_flags & 0x40)) {
            if (std::fabs(fn_8019B184(m_enemy, reinterpret_cast<Vec3f *>(&param.field_0x8[0x34 - 0x8]))) <
                0.75f * lbl_805FAD38) {
                GetRotation()->y = fn_8019B1D8(this, reinterpret_cast<Vec3f *>(&param.field_0x8[0x34 - 0x8]));
                vfStat_0xA8()->Set(5);
                fn_8019AA40(this, 0x602F, 1);
                param.m_result = 5;
                return;
            }
        }
        if (attacker->m_flags & 8) {
            m_hitTime = 0.0f;
            m_hits++;
        }
    }
    EnemyStatusCommon::vfStat_0x70(param);
}

int BkGiant::vfStat_0xB8() {
    int kind = 0x5D;
    if (m_params.WeaponType >= 1) {
        kind = 0x5E;
    }
    return kind;
}

void BkGiant::vfDisp_0x4C() {
    Mtx34f *matrix = GetPartMatrix(m_enemy->m_para->vfPara_0x3C());
    if (matrix != 0) {
        fn_8019AA40(this, 0x600A, 1);
        fn_801A1ED8(matrix, 0x23D);
    }
}

void BkGiant::vfDisp_0x50() {
    EnemyDispCommon::vfDisp_0x50();
    fn_8019AA40(this, 0x602E, 1);
}

BkGiantTask::BkGiantTask(Task *parent, UnkPlacement_800C9CD0 *placement)
    : EnemyTemplate<BkGiant, BkGiantAI>(parent, placement) {
    m_enemy = this;
    m_name = s_taskName;
}

BkGiantTask::~BkGiantTask() {}

} // namespace BkGiant

extern "C" const char *fn_80288618(UnkLabelQuery_800CBC20 *query) {
    static const char *s_weaponTypes[3] = {
        "Weapon : BLACK_SWORD", "Weapon : DARK_HAMMER", "Weapon : BIG_BARREL"};
    static const char *s_appearTypes[2] = {"AppearType : WAIT", "AppearType : DROP"};
    static int s_appearType = 0;

    switch (query->index) {
    case 7:
        s_appearType = query->value;
        return s_appearTypes[query->value];
    case 8:
        return s_weaponTypes[query->value];
    case 9:
        if (s_appearType == 1) {
            return query->params[query->index].label;
        }
        break;
    case 10:
        if (query->value == 0) {
            return "Guard : OFF";
        }
        return query->params[query->index].label;
    }
    return "";
}

namespace BkGiant {

void BkGiantTask::LoadFiles() {
    void *loader = fn_800122D4();
    fn_8001299C(loader, 0xB, new ResourceOneFile("enemy/BkGiantData.one", false));
}

void BkGiantTask::DropFiles() {
    fn_800127F8(fn_800122D4(), 0xB);
}

void BkGiantTask::Instantiate(UnkPlacement_800C9CD0 *placement) {
    new BkGiantTask(fn_801EB304(fn_8001C4F8(), 0xE), placement);
}

} // namespace BkGiant

static GadgetParamDesc lbl_8054F604[] = {
    {4, "MoveRange", 0, 0, 0, 200.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchRange", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchAngle", 0, 0, 0, 0.0f, -360.0f, 360.0f, fn_800CBC20, 0},
    {4, "SearchWidth", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchHeight", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchHeightOffset", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "MoveSpeedRatio", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {1, "AppearType", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_80288618, 0},
    {1, "WeaponType", 0, 0, 2, 0.0f, 0.0f, 0.0f, fn_80288618, 0},
    {4, "OffsetPos_Y", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_80288618, 0},
    {1, "Gurad : ON", 1, 0, 1, 0.0f, 0.0f, 0.0f, fn_80288618, 0},
    {0},
};

GadgetClassDesc lbl_8054F7E4 = {
    "BK_GIANT", BkGiant::BkGiantTask::LoadFiles, BkGiant::BkGiantTask::DropFiles,
    reinterpret_cast<void (*)(UnkPlacement_800C9CD0 *)>(BkGiant::BkGiantTask::Instantiate), 0, 0,
    0x8C, 2, 0, 5, lbl_8054F604,
};

/* Always false for the giant (meaning unknown); retail has it ahead of the stage editor's hooks. */
bool BkGiant::BkGiant::vfStat_0xC8() {
    return 0;
}

/* Not called anywhere: the linker strips it. Retail pools 0 and 0.5 ahead of the other float
 * constants in .sdata2; this function, emitted first, makes the compiler pool them there (as in
 * GunRobot.cpp; the name is a placeholder). */
static float UnkConstantUser(float x) {
    x *= 0.0f;
    x *= 0.5f;
    return x;
}
