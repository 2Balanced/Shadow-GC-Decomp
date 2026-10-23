/* BkWingLarge (class name from RTTI; its s_taskName is "BkWingLarge"): the large Black Arms wing,
 * BK_WINGLARGE in the stage editor (enemy/BkWingLargeData.one). Its body BkWingLargeBase is a
 * FlyerCommonBase (Game/EnemyFlyer.h); it flies along a path and fires air cutters
 * (BkWingLargeAirCutter, a FlyingObject). The unit also holds the first copies of the flyers'
 * inline code: the constructors and destructors of EnemyMoveFlyer, EnemyFlyerCommon,
 * FlyerCommonBase and FlyingObject, and their virtual-base thunks. File name inferred from the
 * class. Names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
/* EnemyMoveCommon's vfMove_0x24 is inline here, as in the original: its weak copy pools the
 * constant 0 ahead of vfPara_0x68's 10 in .sdata2 (see Game/Enemy.h). */
#define ENEMY_MOVE_COMMON_INLINE

#include "Game/BkWingLarge.h"
#include "Game/Gadget.h"
#include "Game/GadgetClassDesc.h"
#include "Game/EnemyRender.h"
#include "Game/MathTypes.h"
#include "Game/ResourceOneFile.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

using Effect::System::EffectElement;

/* The cursor a parameter's label callback receives (layout as in Helicopter.cpp). */
class UnkLabelQuery_800CBC20 {
  public:
    /* 0x0 */ const GadgetParamDesc *params;
    /* 0x4 */ s32 index; /* the parameter being shown */
    /* 0x8 */ s32 value; /* its current value */
};

/* A sphere for the collision world's queries (struct name unknown). */
struct UnkSphere_80021C04 {
    /* 0x0 */ u8 field_0x0[0x10];
};

/* The collision world's hit list (struct name unknown; see BlackBullCtrlUnit.cpp). */
struct UnkLineHits_80021FE4;

/* An entry of the attack table the air cutter's attack points at (struct name unknown). */
struct UnkAttackEntry_8001277C {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ u32 m_id; /* zero ends the table */
    /* 0x8 */ u32 m_flags;
    /* 0xC */ u32 field_0xC;
};

extern "C" {
/* The enemies' shared helpers (behaviour inferred). */
void fn_8019AA40(Enemy *enemy, u16 sound, int arg);
UnkSoundHandle_801D3DC8 fn_8019A908(Enemy *enemy, u16 sound, int arg);
/* Reads the flyer's common parameters (behaviour inferred). */
void fn_80193790(FlyerCommonBase *flyer, BkWingLargeParam *params);
/* Resources and models. */
void *fn_80012684(void *loader, int slot, int kind);
void *fn_80012608(void *loader, int slot, int kind);
UnkAttackEntry_8001277C *fn_8001277C(void *loader, s32 slot, int kind);
boost::shared_ptr<EnemyRender> fn_802CDB08(int slot, const EnemyMotionTable &table, void *model,
                                           void *motion);
/* Sound. */
void *fn_80053164(void);
void fn_801D54F4(void *manager, int sound, const Vec3f *position, int arg3, int arg4);
/* Effects at position with rotation (behaviour inferred). */
boost::weak_ptr<EffectElement> fn_801A1E8C(const Vec3f *position, const Vec3f *rotation, int id);
boost::weak_ptr<EffectElement> fn_801A1F54(const Vec3f *position, const Vec3f *rotation, int id);
/* Vector helpers returning by value: v turned about z, x and y, and a + b. */
Vec3f fn_8040D044(const Vec3f *v, float angle);
Vec3f fn_8040D118(const Vec3f *v, float angle);
Vec3f fn_8040D0AC(const Vec3f *v, float angle);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
/* The collision world and a sphere query on it (behaviour inferred). */
void fn_80021C04(UnkSphere_80021C04 *sphere, const Vec3f *center, float radius);
void *fn_80020D40(void);
UnkLineHits_80021FE4 *fn_80021CA4(void *world, const UnkSphere_80021C04 *sphere, int arg2,
                                  int arg3, int arg4, int arg5, int arg6);
void fn_80026FB4(UnkLineHits_80021FE4 *hits, int arg1);
}

/* EnemyStatFunc's slots 0x20 and 0x94 (Game/Enemy.h). Calling them by name makes GC/2.7 emit
 * their weak EnemyStatFunc copies right after the caller, earlier than retail has them, so the
 * calls go through this view of the same vtable (not original; never instantiated). */
class EnemyStatFuncArgs {
  public:
    virtual void vfStat_0x08() = 0;
    virtual void vfStat_0x0C() = 0;
    virtual void vfStat_0x10() = 0;
    virtual void vfStat_0x14() = 0;
    virtual void vfStat_0x18() = 0;
    virtual void vfStat_0x1C() = 0;
    virtual int vfStat_0x20(int motion, bool restart) = 0;
    virtual void vfStat_0x24() = 0;
    virtual void vfStat_0x28() = 0;
    virtual void vfStat_0x2C() = 0;
    virtual void vfStat_0x30() = 0;
    virtual void vfStat_0x34() = 0;
    virtual void vfStat_0x38() = 0;
    virtual void vfStat_0x3C() = 0;
    virtual void vfStat_0x40() = 0;
    virtual void vfStat_0x44() = 0;
    virtual void vfStat_0x48() = 0;
    virtual void vfStat_0x4C() = 0;
    virtual void vfStat_0x50() = 0;
    virtual void vfStat_0x54() = 0;
    virtual void vfStat_0x58() = 0;
    virtual void vfStat_0x5C() = 0;
    virtual void vfStat_0x60() = 0;
    virtual void vfStat_0x64() = 0;
    virtual void vfStat_0x68() = 0;
    virtual void vfStat_0x6C() = 0;
    virtual void vfStat_0x70() = 0;
    virtual void vfStat_0x74() = 0;
    virtual void vfStat_0x78() = 0;
    virtual void vfStat_0x7C() = 0;
    virtual void vfStat_0x80() = 0;
    virtual void vfStat_0x84() = 0;
    virtual void vfStat_0x88() = 0;
    virtual void vfStat_0x8C() = 0;
    virtual void vfStat_0x90() = 0;
    virtual void vfStat_0x94(int flags) = 0;
};

/* The vector type's out-of-line assignment (see Game/GadgetUtil.h). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* A motion table built in place (not original; as in BkSoldier.cpp). */
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
static const MotionSequenceStep lbl_805F52F8[] = {{1, -1, -1, 20, 1}};
static const MotionSequenceStep lbl_805F5300[] = {{3, -1, -1, 15, 0}};
static const MotionSequenceStep lbl_805F5308[] = {{2, -1, -1, 10, 0}};
static const MotionSequenceStep lbl_804C9100[] = {{1, -1, -1, 15, 0}, {1, -1, -1, 0, 1}};
static const MotionSequenceStep lbl_804C9110[] = {
    {2, -1, -1, 10, 0}, {4, -1, -1, 10, 2}, {5, -1, -1, 10, 0}};
static const MotionSequence lbl_804C9128[] = {
    {lbl_804C9100, 2, -3}, {lbl_805F5300, 1, -3}, {lbl_805F5308, 1, -3}, {lbl_805F52F8, 1, -3},
    {lbl_804C9110, 3, -3},
};
static const MotionSequenceTable lbl_805F5310 = {lbl_804C9128, 5};

/* The model's motion tables: the second for the variant Type bits 4-7 select. */
static const EnemyMotionEntry lbl_805F5318[] = {{4, -1}, {3, -1}};

/* How the air cutter flies. */
static FlyingObjectMotion lbl_80532428 = {80.0f, 0, 0, 0, 1, -1};

/* The unit's .bss is kept whole (the static initializer reaches it through one base address), and
 * retail has 0xD8 and 0x120 unreferenced bytes around the air cutter's statics (their objects and
 * types are unknown; the space is reproduced). */
static u8 s_unknownBss0[0xD8];

/* The air cutter's attack: placed at the wing when one is fired. */
static Vec3f s_cutterPosition(0.0f, 0.0f, 0.0f);
static UnkRotation s_cutterRotation(0.0f, 0.0f, 0.0f);
static AttackObjectParam s_cutterParam(5.0f, 0, 2, 6, 1.0f, 4, 0x40, 6,
                                       *fn_800091EC(&(Vec3f &)Vec3f(), &s_cutterPosition),
                                       s_cutterRotation);
static u8 s_unknownBss1[0x120];

BkWingLargeAirCutter *BkWingLargeAirCutter::Create(const Vec3f *position, const Vec3f *rotation) {
    fn_800091D0(&s_cutterParam.m_position, position);
    s_cutterParam.m_rotation.x = rotation->x;
    s_cutterParam.m_rotation.y = rotation->y;
    s_cutterParam.m_rotation.z = rotation->z;
    s_cutterParam.field_0x4 = reinterpret_cast<u32>(fn_8001277C(fn_800122D4(), 0xD, 7));
    for (UnkAttackEntry_8001277C *entry =
             reinterpret_cast<UnkAttackEntry_8001277C *>(s_cutterParam.field_0x4);
         entry->m_id != 0; entry++) {
        entry->m_flags = 0x100000;
    }
    return new BkWingLargeAirCutter(&lbl_80532428, &s_cutterParam);
}

BkWingLargeAirCutter::BkWingLargeAirCutter(const FlyingObjectMotion *motion,
                                           const AttackObjectParam *param)
    : FlyingObject(motion, param), Task(fn_801EB304(fn_8001C4F8(), 0xE)) {
    m_soundTimer = 0.0f;
    boost::shared_ptr<EffectElement> effect = m_effect.lock();
    if (effect.get() == 0) {
        m_effectRotation.x = m_rotation.x;
        m_effectRotation.y = m_rotation.y;
        m_effectRotation.z = m_rotation.z;
        m_effect = fn_801A1E8C(&m_position, &m_effectRotation, 0x7B);
    }
}

BkWingLargeAirCutter::~BkWingLargeAirCutter() {
    boost::shared_ptr<EffectElement> effect = m_effect.lock();
    if (effect.get() != 0) {
        effect->Retire();
        fn_801A1F54(&m_position, &m_effectRotation, 0x7F);
    }
}

void BkWingLargeAirCutter::Tick(float dt) {
    FlyingObject::vf_0x38(dt);
    m_effectRotation.x = m_rotation.x;
    m_effectRotation.y = m_rotation.y;
    m_effectRotation.z = m_rotation.z;
    if (vf_0x10()) {
        m_flags |= 1;
        return;
    }
    if ((m_soundTimer -= dt) < 0.0f) {
        m_soundTimer = 1.0f;
        fn_801D54F4(fn_80053164(), 0x6044, &m_position, 0, 1);
    }
}

bool BkWingLargeAirCutter::vf_0x20() {
    bool hit = false;
    Vec3f offset(0.0f, 0.0f, -35.0f);
    UnkSphere_80021C04 sphere;
    AssignVector(offset, fn_8040D044(&offset, m_rotation.z));
    AssignVector(offset, fn_8040D118(&offset, m_rotation.x));
    AssignVector(offset, fn_8040D0AC(&offset, m_rotation.y));
    fn_80021C04(&sphere, &(const Vec3f &)fn_8040CDD0(&m_position, &offset), 5.0f);
    UnkLineHits_80021FE4 *hits = fn_80021CA4(fn_80020D40(), &sphere, 1, 6, 0, 0, 0);
    if (hits != 0) {
        hit = true;
        vf_0x28();
        fn_80026FB4(hits, 1);
    }
    return hit;
}

BkWingLargeBase::BkWingLargeBase() : FlyerCommonBase(6) {
    m_enemy = this;
    vfPara_0x40(reinterpret_cast<int>(&lbl_805F5310));
    m_status->vfStat_0x80(false);
}

BkWingLargeBase::~BkWingLargeBase() {
    if (m_sound.m_id != 0) {
        m_sound.fn_801D455C();
    }
}

int BkWingLargeBase::vfStat_0xC4() {
    return (m_params.Type & 0xF) != 0;
}

int BkWingLargeBase::vfStat_0xB8() {
    return 0x5B;
}

void BkWingLargeBase::vfDisp_0x64() {
    void *loader = fn_800122D4();
    int slot = m_motionBase;
    void *model = fn_80012684(loader, slot, 5);
    void *motion = fn_80012608(loader, slot, 8);
    m_render = fn_802CDB08(
        slot, MotionTable(lbl_805F5318 + (((m_params.Type >> 4) & 0xF) ? 1 : 0), 1), model, motion);
}

void BkWingLargeBase::vfSet_0x1C(UnkPlacement_800C9CD0 *placement) {
    vfSet_0x20(placement);
    /* Copied as a plain struct (word loads and stores), unlike Vec3f's own copy. */
    Vec3fWords rotation = *reinterpret_cast<Vec3fWords *>(GetRotation());
    vfDisp_0x38(GetPosition(), reinterpret_cast<Vec3f *>(&rotation));
    m_ctrl->vfCtrl_0x20();
}

void BkWingLargeBase::vfSet_0x20(UnkPlacement_800C9CD0 *placement) {
    TEnemySetBase::vfSet_0x20(placement);
    fn_800C9DBC(placement, &m_params, sizeof(m_params));
    fn_80193790(this, &m_params);
}

bool BkWingLargeBase::vfDisp_0x44(int *command) {
    if (EnemyDispCommon::vfDisp_0x44(command)) {
        return true;
    }
    switch (*command) {
    case 8:
        BkWingLargeAirCutter::Create(GetPosition(), GetRotation());
        break;
    case 15:
        fn_8019AA40(this, 0x2056, 1);
        break;
    }
    return true;
}

int BkWingLargeBase::vfPara_0x48() {
    return 0;
}

int BkWingLargeBase::vfPara_0x4C() {
    return vfPara_0x48();
}

int BkWingLargeBase::vfPara_0x50() {
    return vfPara_0x48();
}

int BkWingLargeBase::vfPara_0x54() {
    return vfPara_0x48();
}

int BkWingLargeBase::vfPara_0x58() {
    return 1;
}

int BkWingLargeBase::vfPara_0x5C() {
    return 2;
}

int BkWingLargeBase::vfPara_0x60() {
    return (m_params.Type & 0xF) ? 4 : 2;
}

void BkWingLargeBase::vf_0x244() {
    reinterpret_cast<EnemyStatFuncArgs *>(m_status)->vfStat_0x20(vfPara_0x58(), true);
}

void BkWingLargeBase::vf_0x248() {
    reinterpret_cast<EnemyStatFuncArgs *>(m_status)->vfStat_0x20(vfPara_0x48(), true);
}

void BkWingLargeBase::vfDisp_0x10(float dt) {
    EnemyDispCommon::vfDisp_0x10(dt);
    if (field_0x30C) {
        if (m_sound.m_id == 0) {
            m_sound = fn_8019A908(this, 0x6043, 1);
        }
    } else {
        m_sound.fn_801D455C();
    }
}

void BkWingLargeBase::vfDisp_0x50() {
    EnemyDispCommon::vfDisp_0x50();
    fn_8019AA40(this, 0x6042, 1);
}

extern "C" const char *fn_80190068(UnkLabelQuery_800CBC20 *query) {
    return "";
}

BkWingLarge::BkWingLarge(Task *parent, UnkPlacement_800C9CD0 *placement)
    : EnemyTemplate<BkWingLargeBase, BkWingLargeAI>(parent, placement) {
    m_enemy = this;
    m_name = s_taskName;
}

BkWingLarge::~BkWingLarge() {
    /* vfStat_0xC4 returns a bool in the game (its result is tested as a byte); Game/Enemy.h
     * declares it as int. */
    if (vfStat_0x58() && (u8)vfStat_0xC4()) {
        reinterpret_cast<EnemyStatFuncArgs &>(static_cast<EnemyStatFunc &>(*this)).vfStat_0x94(1);
    }
}

void BkWingLarge::LoadFiles() {
    void *loader = fn_800122D4();
    fn_8001299C(loader, 0xD, new ResourceOneFile("enemy/BkWingLargeData.one", false));
}

void BkWingLarge::DropFiles() {
    fn_800127F8(fn_800122D4(), 0xD);
}

void BkWingLarge::Instantiate(UnkPlacement_800C9CD0 *placement) {
    new BkWingLarge(fn_801EB304(fn_8001C4F8(), 0xE), placement);
}

static GadgetParamDesc lbl_80532440[] = {
    {4, "MoveRange", 0, 0, 0, 200.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchRange", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchAngle", 0, 0, 0, 0.0f, -360.0f, 360.0f, fn_800CBC20, 0},
    {4, "SearchWidth", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchHeight", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchHeightOffset", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "MoveSpeedRatio", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {1, "AppearType", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_80190068, 0},
    {1, "ActionType", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_80190068, 0},
    {1, "", 0, 0, 8, 0.0f, 0.0f, 0.0f, fn_80190068, 0},
    {4, "FloatWidth", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_80190068, 0},
    {4, "AttackStart", 0, 0, 0, 0.4f, 0.0f, 1.0f, fn_80190068, 0},
    {4, "AttackEnd", 0, 0, 0, 0.6f, 0.0f, 1.0f, fn_80190068, 0},
    {1, "PathMirror", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {3, "Type", 0, 0, 17, 0.0f, 0.0f, 0.0f, fn_80190068, 0},
    {0},
};

GadgetClassDesc lbl_805326C0 = {
    "BK_WINGLARGE", BkWingLarge::LoadFiles, BkWingLarge::DropFiles,
    reinterpret_cast<void (*)(UnkPlacement_800C9CD0 *)>(BkWingLarge::Instantiate), 0, 0, 0x8E, 2, 0,
    5, lbl_80532440,
};

const char *BkWingLarge::s_taskName = "BkWingLarge";
