/* BkWingSmall (class name from RTTI; its s_taskName is "BkWingSmall"): the small Black Arms wing,
 * BK_WINGSMALL in the stage editor (enemy/BkWingSmallData.one). Its body BkWingSmallBase is a
 * FlyerCommonBase (Game/EnemyFlyer.h) that flies along a path or floats in place. File name
 * inferred from the class. Names marked "(name inferred)" are not original. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */

/* EnemyMoveCommon::vfMove_0x24 inline, as in BkWingLarge.cpp: its constant 0 comes first in
 * .sdata2, as in retail. */
#define ENEMY_MOVE_COMMON_INLINE

#include "Game/BkWingSmall.h"
#include "Game/EnemyRender.h"
#include "Game/Gadget.h"
#include "Game/GadgetClassDesc.h"
#include "Game/Random.h"
#include "Game/ResourceOneFile.h"
#include "boost/shared_ptr.hpp"

/* The cursor a parameter's label callback receives (layout as in Helicopter.cpp). */
class UnkLabelQuery_800CBC20 {
  public:
    /* 0x0 */ const GadgetParamDesc *params;
    /* 0x4 */ s32 index; /* the parameter being shown */
    /* 0x8 */ s32 value; /* its current value */
};

extern "C" {
/* The enemies' shared helpers (behaviour inferred). */
void fn_8019AA40(Enemy *enemy, u16 sound, int arg);
void fn_8019A9DC(Enemy *enemy, u16 sound, int arg);
void fn_8019A954(Enemy *enemy, u16 sound, int arg);
/* Sets a flyer up from its parameters (FlyerCommonBase's unit; behaviour inferred). */
void fn_80193790(FlyerCommonBase *flyer, FlyerParam *params);
/* Resources and models. */
void *fn_80012684(void *loader, int slot, int kind);
void *fn_80012608(void *loader, int slot, int kind);
boost::shared_ptr<EnemyRender> fn_802CDB08(int slot, const EnemyMotionTable &table, void *model,
                                           void *motion);
}

/* A motion table built in place (not original; it gives retail's stack layout, as in
 * BkSoldier.cpp). */
struct MotionTable : public EnemyMotionTable {
    MotionTable(const EnemyMotionEntry *e, s32 c) {
        entries = e;
        count = c;
    }
};

/* Three floats copied as a plain struct (not original). */
struct Vec3fWords {
    float x;
    float y;
    float z;
};

/* The motion table (steps in .rodata and .sdata2). */
static const MotionSequenceStep lbl_804C9480[] = {{1, -1, -1, 30, 0}, {1, -1, -1, 0, 1}};
static const MotionSequenceStep lbl_805F5368[] = {{3, -1, -1, 30, 0}};
static const MotionSequenceStep lbl_805F5370[] = {{4, -1, -1, 0, 1}};
static const MotionSequenceStep lbl_805F5378[] = {{2, -1, -1, 0, 0}};
static const MotionSequence lbl_804C9490[] = {
    {lbl_804C9480, 2, -3}, {lbl_805F5368, 1, -3}, {lbl_805F5370, 1, -3}, {lbl_805F5378, 1, -3},
};
static const MotionSequenceTable lbl_805F5380 = {lbl_804C9490, 4};

/* The model's motion table. */
static const EnemyMotionEntry lbl_805F5388[] = {{3, -1}};

BkWingSmallBase::BkWingSmallBase() : FlyerCommonBase(7) {
    m_enemy = this;
    vfPara_0x40(reinterpret_cast<int>(&lbl_805F5380));
    m_status->vfStat_0x80(false);
    field_0x348 = fn_8040EC80(8.0f, 12.0f);
}

BkWingSmallBase::~BkWingSmallBase() {}

int BkWingSmallBase::vfStat_0xB8() {
    return 0x5A;
}

void BkWingSmallBase::vfDisp_0x64() {
    void *loader = fn_800122D4();
    int slot = m_motionBase;
    void *model = fn_80012684(loader, slot, 4);
    void *motion = fn_80012608(loader, slot, 6);
    m_render = fn_802CDB08(slot, MotionTable(lbl_805F5388, 1), model, motion);
}

void BkWingSmallBase::vfDisp_0x10(float dt) {
    EnemyDispCommon::vfDisp_0x10(dt);
    if (vfStat_0x58() && !vfStat_0x6C()) {
        fn_8019A954(this, 0x6041, 1);
    }
    void *key = m_enemy->m_ctrl->vfCtrl_0x10();
    if (m_enemy->field_0xA8.Contains(key)) {
        if ((field_0x348 -= dt) < 0.0f) {
            field_0x348 = fn_8040EC80(8.0f, 12.0f);
            fn_8019A954(this, 0x603E, 1);
        }
    }
}

void BkWingSmallBase::vfDisp_0x50() {
    EnemyDispCommon::vfDisp_0x50();
    fn_8019AA40(this, 0x603B, 1);
}

void BkWingSmallBase::vfDisp_0x48() {
    EnemyDispCommon::vfDisp_0x48();
    fn_8019AA40(this, 0x6040, 1);
}

void BkWingSmallBase::vfSet_0x1C(UnkPlacement_800C9CD0 *placement) {
    vfSet_0x20(placement);
    /* Copied as a plain struct (word loads and stores), unlike Vec3f's own copy. */
    Vec3fWords rotation = *reinterpret_cast<Vec3fWords *>(GetRotation());
    vfDisp_0x38(GetPosition(), reinterpret_cast<Vec3f *>(&rotation));
    m_ctrl->vfCtrl_0x20();
}

void BkWingSmallBase::vfSet_0x20(UnkPlacement_800C9CD0 *placement) {
    TEnemySetBase::vfSet_0x20(placement);
    fn_800C9DBC(placement, &m_params, sizeof(m_params));
    fn_80193790(this, &m_params);
}

bool BkWingSmallBase::vfDisp_0x44(int *command) {
    if (EnemyDispCommon::vfDisp_0x44(command)) {
        return true;
    }
    switch (*command) {
    case 6:
        fn_8019A9DC(this, 0x603C, 1);
        break;
    }
    return true;
}

int BkWingSmallBase::vfPara_0x48() {
    return 0;
}

int BkWingSmallBase::vfPara_0x4C() {
    return vfPara_0x48();
}

int BkWingSmallBase::vfPara_0x50() {
    return vfPara_0x48();
}

int BkWingSmallBase::vfPara_0x54() {
    return 1;
}

int BkWingSmallBase::vfPara_0x58() {
    return 2;
}

int BkWingSmallBase::vfPara_0x5C() {
    return 3;
}

int BkWingSmallBase::vfPara_0x60() {
    return 3;
}

void BkWingSmallBase::vf_0x244() {}

void BkWingSmallBase::vf_0x248() {}

float BkWingSmallBase::GetSpeed() {
    return 120.0f;
}

/* The label callback of the parameters the stage editor hides. */
extern "C" const char *fn_80191664(UnkLabelQuery_800CBC20 *query) {
    return "";
}

BkWingSmall::BkWingSmall(Task *parent, UnkPlacement_800C9CD0 *placement)
    : EnemyTemplate<BkWingSmallBase, BkWingSmallAI>(parent, placement) {
    m_enemy = this;
    m_name = s_taskName;
}

BkWingSmall::~BkWingSmall() {}

void BkWingSmall::LoadFiles() {
    void *loader = fn_800122D4();
    fn_8001299C(loader, 0xE, new ResourceOneFile("enemy/BkWingSmallData.one", false));
}

void BkWingSmall::DropFiles() {
    fn_800127F8(fn_800122D4(), 0xE);
}

void BkWingSmall::Instantiate(UnkPlacement_800C9CD0 *placement) {
    new BkWingSmall(fn_801EB304(fn_8001C4F8(), 0xE), placement);
}

static GadgetParamDesc lbl_80533840[] = {
    {4, "MoveRange", 0, 0, 0, 200.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchRange", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchAngle", 0, 0, 0, 0.0f, -360.0f, 360.0f, fn_800CBC20, 0},
    {4, "SearchWidth", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchHeight", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchHeightOffset", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "MoveSpeedRatio", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {1, "AppearType", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_80191664, 0},
    {1, "ActionType", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_80191664, 0},
    {1, "", 0, 0, 8, 0.0f, 0.0f, 0.0f, fn_80191664, 0},
    {4, "FloatWidth", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_80191664, 0},
    {4, "AttackStart", 0, 0, 0, 0.4f, 0.0f, 1.0f, fn_80191664, 0},
    {4, "AttackEnd", 0, 0, 0, 0.6f, 0.0f, 1.0f, fn_80191664, 0},
    {1, "PathMirror", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80533A98 = {
    "BK_WINGSMALL", BkWingSmall::LoadFiles, BkWingSmall::DropFiles,
    reinterpret_cast<void (*)(UnkPlacement_800C9CD0 *)>(BkWingSmall::Instantiate), 0, 0, 0x8F, 2, 0,
    5, lbl_80533840,
};

const char *BkWingSmall::s_taskName = "BkWingSmall";

bool BkWingSmallBase::vf_0x23C() {
    return m_params.ActionType == 1;
}
