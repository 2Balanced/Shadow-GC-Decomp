/* EggShadowAndroid (namespace and class names from RTTI; its task's s_taskName is
 * "EggShadowAndroid"): the Shadow android, EGG_SHADOWANDROID in the stage editor
 * (enemy/EggShadowAndroidData.one, motions from the player's SHADOW.MTP). Its body
 * EggShadowAndroid::EggShadowAndroid is an EnemyWalkerCommon (Game/Enemy.h) that stands or appears
 * at an offset (AppearType), jumps with jet effects at its feet, leaves a belt behind it while it
 * homes in (EggShadowAndroid::Belt) and fires lock-on missiles from eight launchers; its AI is
 * EggShadowAndroidAI (Game/EggShadowAndroidAI.cpp). File name inferred from the class. Names
 * marked "(name inferred)" are not original. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). NonMatching (85 of 86 functions; every data section
 * matches, and with the unit linked only vfDisp_0x64 differs from retail): vfDisp_0x64's register
 * allocation (see there). */
/* See EnemyReferer (Game/Enemy.h): keeps EnemyTemplate's destructor out of line, as in EggPawn.cpp. */
#define ENEMY_REFERER_DTOR
/* See CharacterFileManager (Game/CharacterFiles.h). */
#define CHARACTER_FILES_GET_HANDLE_EXTERN

#include "Game/EggShadowAndroid.h"
#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/CharacterFiles.h"
#include "Game/EffBelt.h"
#include "Game/EffLocusMultiVtx.h"
#include "Game/EnemyRender.h"
#include "Game/Gadget.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathTypes.h"
#include "Game/MotionSequenceModule.h"
#include "Game/Player.h"
#include "Game/Random.h"
#include "Game/ResourceOneFile.h"
#include "Game/Task.h"
#include "Game/UnkWords_8007A4D4.h"
#include "Game/WeaponBullet.h"
#include "Game/WeaponLockonTarget.h"
#include "Effect/System/EffectElement.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using Effect::System::EffectElement;

namespace EggShadowAndroid {

/* EggShadowAndroid::EggShadowAndroidTask (class name from RTTI; vtable 0x805530AC): the task that
 * runs an android. */
class EggShadowAndroidTask : public EnemyTemplate<EggShadowAndroid, EggShadowAndroidAI> {
  public:
    EggShadowAndroidTask(Task *parent, UnkPlacement_800C9CD0 *placement);
    virtual ~EggShadowAndroidTask();

    /* The stage editor's hooks (names inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *placement);
    static void LoadFiles();
    static void DropFiles();

    static const char *s_taskName;
};
SIZE_ASSERT(EggShadowAndroidTask, 0x3E8);

/* EggShadowAndroid::Belt (class name from RTTI; vtable 0x80553934): the belt the android trails
 * while it homes in, a Test::EffBelt that follows the body (names inferred). */
class Belt : public Task {
  public:
    Belt(Task *parent, EggShadowAndroid *owner, float width);
    virtual ~Belt();
    virtual void Tick(float dt);

    void Finish();

    /* 0x28 */ Test::EffBelt *m_belt;
    /* 0x2C */ EggShadowAndroid *m_owner;
};
SIZE_ASSERT(Belt, 0x30);

} // namespace EggShadowAndroid

/* The cursor a parameter's label callback receives (layout as in Helicopter.cpp). */
class UnkLabelQuery_800CBC20 {
  public:
    /* 0x0 */ const GadgetParamDesc *params;
    /* 0x4 */ s32 index; /* the parameter being shown */
    /* 0x8 */ s32 value; /* its current value */
};

/* A bullet type's record (layout as in BulletCannon.cpp; struct name unknown). */
struct UnkBulletType_80546018 {
    /* 0x00 */ void (*initialize)(void);
    /* 0x04 */ void (*finalize)(void);
    /* 0x08 */ void (*create)(const Weapon::ShotInfo &info);
    /* 0x0C */ u32 field_0xC;
    /* 0x10 */ float m_reach;
};

/* The missile's record. */
extern UnkBulletType_80546018 lbl_8051DDA0;

extern "C" {
/* The enemies' shared helpers (behaviour inferred). */
void fn_8019AA40(Enemy *enemy, u16 sound, int arg);
bool fn_801A2030(void);
/* Resources and models (see BkSoldier.cpp). */
void *fn_80012684(void *loader, int slot, int kind);
int fn_80012608(void *loader, int slot, int kind);
void *fn_8001258C(void *loader, int handle, const char *name);
boost::shared_ptr<EnemyRender> fn_802CDB08(int slot, const EnemyMotionTable &table, void *model,
                                           void *motion);
/* An effect at a matrix, timed by time (see ClumpFrameEffects.cpp). */
boost::weak_ptr<EffectElement> fn_801D2C28(int kind, const Mtx34f *matrix, int id,
                                           const float *time);
/* The target of a player character's target object (see ArkCannon.cpp). */
boost::weak_ptr<Target> fn_80206574(Player::PlayerTargetBase *target);
/* Whether a task is still alive (see PlayerEffect.cpp). */
bool fn_8004EAC0(Task *task);
/* Moves a character by its velocity and lands it (behaviour inferred). */
void fn_80093728(Vec3f *position, Vec3f *velocity, Vec3f *acceleration, const Vec3f *up,
                 UnkWords_8007A4D4 *flags, float dt);
void fn_80076CA8(UnkWords_8007A4D4 *bits, int bit);
/* The character info's vector at 0x18 (inline; first emitted by another unit). */
Vec3f *fn_8028F324(Enemy *enemy);
/* Vector and matrix routines (see GunBigfoot.cpp and BkSoldier.cpp). */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
void fn_8040FAA0(Vec3f *translation, const Mtx34f *matrix);
Vec3f fn_8040B618(const Mtx34f *matrix, const Vec3f *vector);
Vec3f fn_8040FB08(const Mtx34f *matrix);
Vec2f fn_8040F760(const Vec3f *direction);
Vec3f fn_8040CEE8(const Vec3f *v);
Vec3f fn_8040CDA8(const Vec3f *v, float scale);
}

/* A motion table built in place (not original; as in BkWorm.cpp). */
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

/* The motion table (steps in .sdata2, .sdata and .rodata). The two steps in .sdata take the
 * motions the constructor finds in the resource file. */
static const MotionSequenceStep lbl_805F8628[] = {{0x67, -1, -1, 10, 0}};
static const MotionSequenceStep lbl_805F8630[] = {{0x6A, -1, -1, 10, 0}};
static const MotionSequenceStep lbl_805F8638[] = {{0x68, -1, -1, 10, 0}};
static const MotionSequenceStep lbl_805F8640[] = {{0x88, -1, -1, 5, 1}};
static const MotionSequenceStep lbl_804FB9C0[] = {{0x6C, -1, -1, 20, 2}, {0x6D, -1, -1, 10, 0}};
static const MotionSequenceStep lbl_804FB9D0[] = {{0x6E, -1, -1, 10, 0}, {0x6F, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_805F8648[] = {{0x71, -1, -1, 0, 1}};
static const MotionSequenceStep lbl_805F8650[] = {{0x7F, -1, -1, 10, 0}};
static const MotionSequenceStep lbl_805F8658[] = {{0x80, -1, -1, 10, 0}};
static const MotionSequenceStep lbl_805F8660[] = {{0x81, -1, -1, 10, 0}};
static MotionSequenceStep lbl_805EB588[] = {{3, -1, -1, 10, 0}};
static const MotionSequenceStep lbl_804FB9E0[] = {
    {0x43, -1, -1, 0, 0}, {0x44, -1, -1, 0, 2}, {0x45, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_804FB9F8[] = {
    {0x46, -1, -1, 0, 0}, {0x47, -1, -1, 0, 2}, {0x48, -1, -1, 0, 0}};
static MotionSequenceStep lbl_805EB590[] = {{2, -1, -1, 10, 0}};
static const MotionSequenceStep lbl_805F8668[] = {{0x82, -1, -1, 0, 0}};
static const MotionSequence lbl_804FBA10[] = {
    {lbl_805F8628, 1, -3}, {lbl_805F8630, 1, -3}, {lbl_805F8638, 1, -3}, {lbl_805F8640, 1, -3},
    {lbl_804FB9C0, 2, -3}, {lbl_804FB9D0, 2, -3}, {lbl_805F8648, 1, -3}, {lbl_805F8650, 1, -3},
    {lbl_805F8658, 1, -3}, {lbl_805F8660, 1, -3}, {lbl_805EB588, 1, -3}, {lbl_804FB9E0, 3, -3},
    {lbl_804FB9F8, 3, -3}, {lbl_805EB590, 1, -3}, {lbl_805F8668, 1, -3},
};
static MotionSequenceTable lbl_805EB598 = {lbl_804FBA10, 15};

/* The model's motion table. */
static const EnemyMotionEntry lbl_805F8670[] = {{3, -1}};

/* The belt's colour (name inferred). */
static UnkColor_8040D2EC s_beltColor = UnkColor_8040D2EC(0x7F, 0x40, 0);

namespace EggShadowAndroid {

const char *EggShadowAndroidTask::s_taskName = "EggShadowAndroid";

Belt::Belt(Task *parent, EggShadowAndroid *owner, float width) : Task(parent), m_owner(owner) {
    m_name = "Android Belt";
    m_belt = new Test::EffBelt(0x3C, s_beltColor, width);
    if (m_owner != 0) {
        m_owner->StartBoostEffect();
    }
}

Belt::~Belt() {
    if (m_belt != 0) {
        m_belt->m_flags |= 1;
    }
    if (m_owner != 0) {
        m_owner->StopBoostEffect();
    }
}

void Belt::Tick(float dt) {
    if (m_belt != 0 && !fn_8004EAC0(m_belt)) {
        m_belt = 0;
    }
    if (m_belt == 0) {
        m_flags |= 1;
    } else if (m_belt != 0) {
        m_belt->AddPosition(fn_8040FB08(m_owner->GetPartMatrix(1)));
    }
}

void Belt::Finish() {
    if (m_belt != 0) {
        m_belt->Finish();
        m_belt = 0;
    }
    if (m_owner != 0) {
        m_owner->StopBoostEffect();
    }
}

EggShadowAndroid::EggShadowAndroid() : EnemyWalkerCommon(0xE), m_belt(0), m_hit(0), m_noGround(0) {
    m_enemy = this;
    m_tube = 0;
    m_attackWait = fn_8040EA90(5) + 2;
    int motion = fn_80012608(fn_800122D4(), 0x15, 7);
    lbl_805EB588[0].motion = motion + 2;
    lbl_805EB590[0].motion = motion + 1;
    vfPara_0x40(reinterpret_cast<int>(&lbl_805EB598));
    fn_8040B6F8(&m_jetMatrix);
    fn_8040B6F8(&m_boostMatrix);
}

EggShadowAndroid::~EggShadowAndroid() {
    DeleteBelt();
}

void EggShadowAndroid::vfSet_0x1C(UnkPlacement_800C9CD0 *placement) {
    vfSet_0x20(placement);
    /* Copied as a plain struct (word loads and stores), unlike Vec3f's own copy. */
    Vec3fWords rotation = *reinterpret_cast<Vec3fWords *>(GetRotation());
    vfDisp_0x38(GetPosition(), reinterpret_cast<Vec3f *>(&rotation));
    m_ctrl->vfCtrl_0x20();
}

void EggShadowAndroid::vfSet_0x20(UnkPlacement_800C9CD0 *placement) {
    TEnemySetBase::vfSet_0x20(placement);
    fn_800C9DBC(placement, &m_params, sizeof(m_params));
    if (m_params.AppearType == 1) {
        GetPosition()->x += m_params.OffsetPos_X;
        GetPosition()->y += m_params.OffsetPos_Y;
        GetPosition()->z += m_params.OffsetPos_Z;
    }
}

void EggShadowAndroid::vfStat_0x38() {
    if (m_params.AppearType == 1 && !field_0xA8.Contains(m_ctrl->vfCtrl_0x10())) {
        m_status->vfStat_0x3C(true);
        m_status->vfStat_0x30(false);
    } else {
        m_status->vfStat_0x3C(false);
        m_status->vfStat_0x30(true);
    }
}

/* NonMatching: retail gives this r31, the loader r30 and the model r29; GC/2.7 gives this the
 * last register (as in BkGiant.cpp, where retail does too), and no source shape tried (about 20:
 * locals, copies of this, inline helpers, a plain table) changes that. The AI's two turn helpers
 * show the same pattern with a float parameter (Game/EggShadowAndroidAI.cpp). */
void EggShadowAndroid::vfDisp_0x64() {
    void *loader = fn_800122D4();
    void *model = fn_80012684(loader, m_enemy->m_motionBase, 5);
    void *motion = fn_8001258C(loader, fn_80078C88()->GetHandle(0xC), "SHADOW.MTP");
    m_render = fn_802CDB08(m_enemy->m_motionBase, MotionTable(lbl_805F8670, 1), model, motion);
}

void EggShadowAndroid::vfDisp_0x08() {
    EnemyDispCommon::vfDisp_0x08();
}

void EggShadowAndroid::vfDisp_0x10(float dt) {
    EnemyWalkerCommon::vfDisp_0x10(dt);
    if (vfStat_0x24() == 5 || vfStat_0x24() == 6 || vfStat_0x24() == 7 || vfStat_0x24() == 8 ||
        vfStat_0x24() == 9) {
        m_noGround = 1;
    } else {
        m_noGround = 0;
    }
    if (!m_jetEffect.expired()) {
        fn_8040C148(&m_jetMatrix, GetPartMatrix(1));
        fn_8040B59C(&m_jetMatrix, &(const Vec3f &)Vec3f(0.0f, 1.1999999f, -1.5999999f));
    }
    if (!m_boostEffect.expired()) {
        Vec3f direction;
        fn_800091EC(&direction, &(const Vec3f &)fn_8040CEE8(fn_8028F324(this)));
        Vec2f angles = fn_8040F760(&direction);
        float pitch = angles.x;
        float yaw = angles.y;
        fn_8040B6F8(&m_boostMatrix);
        fn_8040B59C(&m_boostMatrix, &(const Vec3f &)fn_8040FB08(GetPartMatrix(1)));
        fn_8040B59C(&m_boostMatrix, &(const Vec3f &)fn_8040CDA8(&direction, 5.0f));
        fn_8040B848(&m_boostMatrix, yaw);
        fn_8040B884(&m_boostMatrix, pitch);
    }
}

void EggShadowAndroid::vfStat_0x1C() {
    EnemyStatusCommon::vfStat_0x1C();
    m_hit = 0;
}

void EggShadowAndroid::vfDisp_0x40(EnemyMotionEventArg &arg) {
    EnemyDispCommon::vfDisp_0x40(arg);
    if (arg.m_type == 0x5000 && arg.m_data->kind == 2) {
        if (arg.m_data->value == 100) {
            Lockon();
            if (++m_tube > 7) {
                m_tube = 0;
            }
        } else if (arg.m_data->value == 101) {
            FireMissile();
        }
    }
}

bool EggShadowAndroid::vfDisp_0x44(int *command) {
    if (EnemyDispCommon::vfDisp_0x44(command)) {
        return true;
    }
    return true;
}

void EggShadowAndroid::Lockon() {
    m_lockon = boost::shared_ptr<Weapon::LockonTarget>(new Weapon::LockonTarget());
    if (m_lockon) {
        boost::weak_ptr<CharInfo> player = fn_800A8A7C(fn_80074194(), GetPosition());
        if (!player.expired()) {
            boost::shared_ptr<CharInfo> chara = player.lock();
            boost::shared_ptr<Player::PlayerBase> base =
                boost::dynamic_pointer_cast<Player::PlayerBase>(chara);
            if (base.get() != 0 && base->m_target != 0) {
                m_lockon->SetTarget(fn_80206574(base->m_target));
                m_lockon->fn_80060F70();
                fn_8019AA40(this, 0x605D, 1);
            }
        }
    }
}

void EggShadowAndroid::ClearLockon() {
    if (m_lockon) {
        m_lockon.reset();
    }
}

void EggShadowAndroid::FireMissile() {
    if (m_lockon) {
        Mtx34f *matrix = GetPartMatrix(m_tube * 1000 + 1000);
        if (matrix != 0) {
            Vec3f position;
            fn_8040FAA0(&position, matrix);
            Vec3f direction(0.0f, 0.0f, -1.0f);
            fn_800091D0(&direction, &(const Vec3f &)fn_8040B618(matrix, &direction));
            Weapon::ShotInfo shot(fn_8017D014(m_enemy), m_lockon, true, position, direction,
                                  lbl_805E07A0);
            lbl_8051DDA0.create(shot);
            fn_8019AA40(this, 0x605E, 1);
        }
        m_lockon.reset();
    }
}

float EggShadowAndroid::vfPara_0x6C() {
    return 8.0f;
}

int EggShadowAndroid::vfPara_0x48() {
    if (--m_attackWait < 0) {
        m_attackWait = fn_8040EA90(5) + 2;
        return -(fn_8040EA90(0xFF) & 1) + 2;
    }
    return 0;
}

int EggShadowAndroid::vfPara_0x4C() {
    return 3;
}

int EggShadowAndroid::vfPara_0x50() {
    return 4;
}

int EggShadowAndroid::vfPara_0x54() {
    return 0;
}

int EggShadowAndroid::vfPara_0x58() {
    return 10;
}

int EggShadowAndroid::vfPara_0x5C() {
    return vfStat_0x74() == 0 ? 11 : 12;
}

int EggShadowAndroid::vfPara_0x60() {
    return 14;
}

float EggShadowAndroid::vfPara_0x68() {
    if (m_noGround) {
        return 0.0f;
    }
    return EnemyParamCommon::vfPara_0x68();
}

void EggShadowAndroid::vfStat_0x70(AttackCallbackParam &param) {
    boost::shared_ptr<CharaColliAttack> attacker(param.m_attacker);
    if (attacker->m_flags & 8) {
        m_hit = 1;
    }
    u32 mask = 0xBF4;
    if (!m_guardFlags.Test(0)) {
        mask |= 8;
    }
    if (attacker->m_flags & mask) {
        EnemyStatusCommon::vfStat_0x70(param);
        return;
    }
    param.m_result = 5;
}

int EggShadowAndroid::vfStat_0xB8() {
    return fn_801A2030() ? 0x6D : 0x67;
}

void EggShadowAndroid::CreateBelt() {
    if (m_belt == 0) {
        m_belt = new Belt(fn_801EB304(fn_8001C4F8(), 0xE), this, 5.0f);
    }
}

void EggShadowAndroid::DeleteBelt() {
    if (m_belt != 0) {
        m_belt->Finish();
        m_belt = 0;
    }
}

void EggShadowAndroid::StartJetEffect() {
    if (m_jetEffect.expired()) {
        m_jetEffect = fn_801D2C28(0, &m_jetMatrix, 0x23,
                                  &(*reinterpret_cast<float **>(
                                      fn_8017D014(this).lock()->field_0x60))[1]);
    }
}

void EggShadowAndroid::StopJetEffect() {
    if (!m_jetEffect.expired()) {
        m_jetEffect.lock()->Kill();
        m_jetEffect.reset();
    }
}

void EggShadowAndroid::StartBoostEffect() {
    if (m_boostEffect.expired()) {
        m_boostEffect = fn_801D2C28(0, &m_boostMatrix, 0x24,
                                    &(*reinterpret_cast<float **>(
                                        fn_8017D014(this).lock()->field_0x60))[1]);
    }
}

void EggShadowAndroid::StopBoostEffect() {
    if (!m_boostEffect.expired()) {
        m_boostEffect.lock()->Kill();
        m_boostEffect.reset();
    }
}

u8 EggShadowAndroid::WasHit() {
    return m_hit;
}

void EggShadowAndroid::Fall() {
    UnkWords_8007A4D4 flags;
    if (m_moveFlags.Test(0)) {
        fn_80076CA8(&flags, 2);
    }
    if (m_moveFlags.Test(1)) {
        fn_80076CA8(&flags, 0x1D);
    }
    Vec3f gravity;
    fn_80008E1C(&gravity, 0.0f, -320.0f, 0.0f);
    fn_8040CFF8(GetAcceleration(), &gravity);
    Vec3f up(0.0f, 1.0f, 0.0f);
    fn_80093728(GetPosition(), fn_8028F324(this), GetAcceleration(), &up, &flags, GetFrameTime());
    fn_8040CE60(GetAcceleration());
}

EggShadowAndroidTask::EggShadowAndroidTask(Task *parent, UnkPlacement_800C9CD0 *placement)
    : EnemyTemplate<EggShadowAndroid, EggShadowAndroidAI>(parent, placement) {
    m_enemy = this;
    m_name = s_taskName;
}

EggShadowAndroidTask::~EggShadowAndroidTask() {}

} // namespace EggShadowAndroid

extern "C" const char *fn_8029C2D4(UnkLabelQuery_800CBC20 *query) {
    static const char *s_appearTypes[2] = {"AppearType : STAND", "AppearType : OFFSET"};
    static int s_appearType = 0;

    switch (query->index) {
    case 7:
        s_appearType = query->value;
        return s_appearTypes[query->value];
    case 8:
    case 9:
    case 10:
        if (s_appearType == 1) {
            return query->params[query->index].label;
        }
        break;
    }
    return "";
}

namespace EggShadowAndroid {

void EggShadowAndroidTask::LoadFiles() {
    void *loader = fn_800122D4();
    fn_8001299C(loader, 0x15, new ResourceOneFile("enemy/EggShadowAndroidData.one", false));
}

void EggShadowAndroidTask::DropFiles() {
    fn_800127F8(fn_800122D4(), 0x15);
}

void EggShadowAndroidTask::Instantiate(UnkPlacement_800C9CD0 *placement) {
    new EggShadowAndroidTask(fn_801EB304(fn_8001C4F8(), 0xE), placement);
}

} // namespace EggShadowAndroid

static GadgetParamDesc lbl_805529D0[] = {
    {4, "MoveRange", 0, 0, 0, 200.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchRange", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchAngle", 0, 0, 0, 0.0f, -360.0f, 360.0f, fn_800CBC20, 0},
    {4, "SearchWidth", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchHeight", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchHeightOffset", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "MoveSpeedRatio", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {1, "AppearType", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_8029C2D4, 0},
    {4, "OffsetPos_X", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_8029C2D4, 0},
    {4, "OffsetPos_Y", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_8029C2D4, 0},
    {4, "OffsetPos_Z", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_8029C2D4, 0},
    {0},
};

GadgetClassDesc lbl_80552BB0 = {
    "EGG_SHADOWANDROID", EggShadowAndroid::EggShadowAndroidTask::LoadFiles,
    EggShadowAndroid::EggShadowAndroidTask::DropFiles,
    reinterpret_cast<void (*)(UnkPlacement_800C9CD0 *)>(
        EggShadowAndroid::EggShadowAndroidTask::Instantiate),
    0, 0, 0x7A, 2, 0, 5, lbl_805529D0,
};

/* Not called anywhere: the linker strips it. Retail pools 0 ahead of the other float constants in
 * .sdata2; this function, emitted first, makes the compiler pool it there (as in BkGiant.cpp; the
 * name is a placeholder). */
static float UnkConstantUser(float x) {
    x *= 0.0f;
    return x;
}
