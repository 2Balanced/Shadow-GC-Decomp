/* GunBigfoot (namespace and class names from RTTI; its task's s_taskName is "GunBigfoot"): the
 * G.U.N. Bigfoot walker, GUN_BIGFOOT in the stage editor (enemy/GunBigfootData.one). Its body
 * GunBigfoot::GunBigfoot is an EnemyWalkerCommon (Game/Enemy.h) that waits crouching, hovering,
 * dropping in or ready to act (AppearType) and fires a vulcan or homing missiles (WeaponType); its
 * AI is GunBigfootAI (Game/GunBigfootAI.cpp). File name inferred from the class. Names marked
 * "(name inferred)" are not original. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
/* See EnemyReferer (Game/Enemy.h): keeps EnemyTemplate's destructor out of line, as in EggPawn.cpp. */
#define ENEMY_REFERER_DTOR

#include "Game/GunBigfoot.h"
#include "Game/AngleUtil.h"
#include "Game/CharInfo.h"
#include "Game/EnemyRender.h"
#include "Game/EnemyWeapon.h"
#include "Game/Gadget.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GunBigfootAI.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/ResourceOneFile.h"
#include "Game/Vibration.h"
#include "Game/WeaponBullet.h"
#include "Game/WeaponLockonTarget.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

using SonicteamUSA::System::CharaColli::AttackCallbackParam;

namespace GunBigfoot {

/* GunBigfoot::GunBigfootTask (class name from RTTI; vtable 0x8054ED58): the task that runs a
 * walker. */
class GunBigfootTask : public EnemyTemplate<GunBigfoot, GunBigfootAI> {
  public:
    GunBigfootTask(Task *parent, UnkPlacement_800C9CD0 *placement);
    virtual ~GunBigfootTask();

    /* The stage editor's hooks (names inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *placement);
    static void LoadFiles();
    static void DropFiles();

    static const char *s_taskName;
};
SIZE_ASSERT(GunBigfootTask, 0x390);

} // namespace GunBigfoot

/* The cursor a parameter's label callback receives (layout as in Helicopter.cpp). */
class UnkLabelQuery_800CBC20 {
  public:
    /* 0x0 */ const GadgetParamDesc *params;
    /* 0x4 */ s32 index; /* the parameter being shown */
    /* 0x8 */ s32 value; /* its current value */
};

/* The camera manager (see Game/CameraManagerUnit.h). */
class UnkCameraManager_80009548;

/* A bullet type's record (layout as in BulletCannon.cpp; struct name unknown). */
struct UnkBulletType_80546018 {
    /* 0x00 */ void (*initialize)(void);
    /* 0x04 */ void (*finalize)(void);
    /* 0x08 */ void (*create)(const Weapon::ShotInfo &info);
    /* 0x0C */ u32 field_0xC;
    /* 0x10 */ float m_reach;
};

/* The homing missile's record (Game/BulletHoming.cpp). */
extern UnkBulletType_80546018 lbl_8051DDB4;

extern "C" {
/* The enemies' shared helpers (behaviour inferred). */
void fn_8019AA40(Enemy *enemy, u16 sound, int arg);
UnkSoundHandle_801D3DC8 fn_8019A908(Enemy *enemy, u16 sound, int arg);
void fn_8017CA2C(Enemy *enemy);
/* Resources and models (see BkSoldier.cpp). */
void *fn_80012684(void *loader, int slot, int kind);
void *fn_80012608(void *loader, int slot, int kind);
boost::shared_ptr<EnemyRender> fn_802CDB08(int slot, const EnemyMotionTable &table, void *model,
                                           void *motion);
void *fn_802CD420(EnemyRender *render);
void fn_80415808(void *motion, int id, void (*callback)(void *), void *data);
/* Sounds, rumble and a camera shake (behaviours inferred). */
void fn_801D54F4(void *manager, int sound, const Vec3f *position, int arg3, int arg4);
void fn_801D560C(void *manager, int sound, int arg2, int arg3);
boost::weak_ptr<Vibration::RequestHandle> fn_8034DE08(int pattern, const Vec3f *position,
                                                      int arg);
UnkCameraManager_80009548 *fn_80009548(void);
void fn_8000D8EC(UnkCameraManager_80009548 *manager, int arg1, int arg2, float strength);
/* Fires a bullet of type for a shot, and throws a weapon of type from position (see
 * BAGunShip.cpp). */
void fn_80051C34(int type, const Weapon::ShotInfo *info);
void fn_80221BA8(const Vec3f *position, const Vec3f *direction, const Vec3f *velocity, int type,
                 int arg);
/* The target of a player character's target object (see ArkCannon.cpp). */
boost::weak_ptr<Target> fn_80206574(Player::PlayerTargetBase *target);
/* Model nodes, matrices and vectors (see GunRobot.cpp and BkWorm.cpp). */
void fn_80016370(Mtx34f *matrix, void *node);
void fn_800163F4(void *node, const Mtx34f *matrix);
void fn_8040FAA0(Vec3f *translation, const Mtx34f *matrix);
Vec3f fn_8040B618(const Mtx34f *matrix, const Vec3f *vector);
Mtx34f fn_8040BEF8(const Mtx34f *matrix);
void fn_8040BAC4(Mtx34f *matrix, const Mtx34f *other);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Vec2f fn_8040F760(const Vec3f *direction);
Vec3f fn_8040CEE8(const Vec3f *v);
/* out = a + b (as a function that returns its result, it is fn_8040CDD0 of AdjustCosRot.cpp). */
void fn_8040CDD0(Vec3f *out, const Vec3f *a, const Vec3f *b);
}

extern float lbl_805FAD38; /* pi */

/* What the model's update callback receives (as in BkSoldier.cpp). */
struct UnkModelCallbackArg {
    /* 0x0 */ void *m_node;
    /* 0x4 */ GunBigfoot::GunBigfoot *m_owner;
};

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

/* The motion table (steps in .sdata2 and .rodata). */
static const MotionSequenceStep lbl_805F82B0[] = {{9, -1, -1, 30, 1}};
static const MotionSequenceStep lbl_805F82B8[] = {{1, -1, 1, 0, 1}};
static const MotionSequenceStep lbl_805F82C0[] = {{7, -1, -1, 30, 1}};
static const MotionSequenceStep lbl_804F85B0[] = {{3, -1, -1, 10, 2}, {2, -1, -1, 20, 0}};
static const MotionSequenceStep lbl_805F82C8[] = {{4, -1, -1, 10, 1}};
static const MotionSequenceStep lbl_805F82D0[] = {{1, -1, -1, 60, 0}};
static const MotionSequenceStep lbl_805F82D8[] = {{1, -1, -1, 30, 0}};
static const MotionSequenceStep lbl_804F85C0[] = {{5, -1, -1, 30, 0}, {5, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_805F82E0[] = {{6, -1, -1, 30, 0}};
static const MotionSequenceStep lbl_805F82E8[] = {{8, -1, -1, 10, 0}};
static const MotionSequenceStep lbl_805F82F0[] = {{4, -1, -1, 10, 0}};
static const MotionSequenceStep lbl_805F82F8[] = {{8, -1, -1, 10, 0}};
static const MotionSequence lbl_804F85D0[] = {
    {lbl_805F82B0, 1, -3}, {lbl_805F82B8, 1, -3}, {lbl_805F82C0, 1, -3}, {lbl_804F85B0, 2, -3},
    {lbl_805F82C8, 1, -3}, {lbl_805F82D0, 1, -3}, {lbl_805F82D8, 1, -3}, {lbl_804F85C0, 2, -3},
    {lbl_805F82E0, 1, -3}, {lbl_805F82E8, 1, -3}, {lbl_805F82F0, 1, -3}, {lbl_805F82F8, 1, -3},
};
static const MotionSequenceTable lbl_805F8300 = {lbl_804F85D0, 12};

/* The bullet and thrown-weapon types of the two WeaponTypes. */
static const s32 lbl_805F8308[] = {5, 0x14};

/* The model motion tables of the two WeaponTypes. */
static const EnemyMotionEntry lbl_804F8630[2][2] = {{{3, -1}, {5, 0xC81}}, {{4, -1}, {5, 0xC81}}};

namespace GunBigfoot {

const char *GunBigfootTask::s_taskName = "GunBigfoot";

/* The model kinds of the two WeaponTypes in the walker's resource file (names inferred). */
enum {
    kModelVulcan = 7,
    kModelMissile = 8,
};

/* Degrees to radians (name inferred). Inline: retail multiplies even the zero limit. */
inline float ToRadians(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* The model's update callback: turns the upper body toward the aim point, at most 45 degrees to
 * the side (only while hovering) and 40 degrees down (behaviour inferred). */
void GunBigfoot::OnModelUpdate(void *data) {
    UnkModelCallbackArg *arg = static_cast<UnkModelCallbackArg *>(data);
    float pitch = 0.0f;
    float yaw = pitch;
    Mtx34f matrix;
    fn_80016370(&matrix, arg->m_node);
    GunBigfoot *self = arg->m_owner;
    if (self != 0 && self->m_aiming) {
        Vec3f local;
        fn_800091EC(&local, &(const Vec3f &)fn_8040B67C(&(const Mtx34f &)fn_8040BEF8(&matrix),
                                                        &(const Vec3f &)self->GetAimPoint()));
        Vec3f direction;
        fn_800091EC(&direction, &(const Vec3f &)fn_8040CEE8(&local));
        Vec2f angles = fn_8040F760(&direction);
        pitch = angles.x;
        yaw = angles.y;
        if (!self->m_hovering) {
            yaw = 0.0f;
        }
    }
    if (yaw > ToRadians(45.0f)) {
        yaw = ToRadians(45.0f);
    } else if (yaw < -ToRadians(45.0f)) {
        yaw = -ToRadians(45.0f);
    }
    if (pitch > ToRadians(0.0f)) {
        pitch = ToRadians(0.0f);
    } else if (pitch < ToRadians(-40.0f)) {
        pitch = ToRadians(-40.0f);
    }
    float step = self->GetFrameTime();
    self->m_aimAngles.y = TurnAngleToward(yaw, self->m_aimAngles.y, step);
    self->m_aimAngles.x = TurnAngleToward(pitch, self->m_aimAngles.x, step);
    self->m_aimAngles.z = TurnAngleToward(0.0f, self->m_aimAngles.z, step);
    Mtx34f turn;
    fn_8040B6F8(&turn);
    fn_8001E070(&turn, &self->m_aimAngles, 1);
    fn_8040BAC4(&matrix, &turn);
    fn_800163F4(arg->m_node, &matrix);
}

GunBigfoot::GunBigfoot()
    : EnemyWalkerCommon(2), m_weapon(0), m_aimAngles(0.0f, 0.0f, 0.0f) {
    m_enemy = this;
    vfPara_0x40(reinterpret_cast<int>(&lbl_805F8300));
    m_tube = 0;
    m_hovering = 0;
    m_jetOn = 0;
    m_aiming = 0;
    fn_8040E970(&m_aimAngles);
    fn_8040CE60(&m_aimPoint);
}

GunBigfoot::~GunBigfoot() {
    if (m_weapon != 0) {
        delete m_weapon;
        m_weapon = 0;
    }
    m_jetSound.fn_801D455C();
    if (m_render.get() != 0) {
        void *data = fn_802CD420(m_render.get());
        if (data != 0) {
            fn_80415808(data, 0xC9E, 0, 0);
        }
    }
}

void GunBigfoot::vfSet_0x1C(UnkPlacement_800C9CD0 *placement) {
    vfSet_0x20(placement);
    /* Copied as a plain struct (word loads and stores), unlike Vec3f's own copy. */
    Vec3fWords rotation = *reinterpret_cast<Vec3fWords *>(GetRotation());
    vfDisp_0x38(GetPosition(), reinterpret_cast<Vec3f *>(&rotation));
    m_ctrl->vfCtrl_0x20();
}

bool GunBigfoot::vfStat_0x54() {
    if (m_hovering) {
        return 0;
    }
    return EnemyStatusCommon::vfStat_0x54();
}

bool GunBigfoot::vfStat_0x68() {
    if (m_hovering) {
        return 0;
    }
    return EnemyStatusCommon::vfStat_0x68();
}

float GunBigfoot::vfPara_0x68() {
    if (m_hovering) {
        return 0.0f;
    }
    return EnemyParamCommon::vfPara_0x68();
}

void GunBigfoot::vfSet_0x20(UnkPlacement_800C9CD0 *placement) {
    TEnemySetBase::vfSet_0x20(placement);
    fn_800C9DBC(placement, &m_params, sizeof(m_params));
    switch (m_params.AppearType) {
    case 2:
        GetPosition()->y += m_params.OffsetPos_Y;
        break;
    case 1:
    case 4:
        m_hovering = 1;
        break;
    }
}

void GunBigfoot::vfStat_0x38() {
    if (m_params.AppearType == 2 && !field_0xA8.Contains(m_ctrl->vfCtrl_0x10())) {
        m_status->vfStat_0x3C(true);
        m_status->vfStat_0x30(false);
    } else {
        m_status->vfStat_0x3C(false);
        m_status->vfStat_0x30(true);
    }
}

void GunBigfoot::vfStat_0xA0() {
    EnemyStatusCommon::vfStat_0xA0();
    Vec3f velocity(0.0f, 40.0f, 0.0f);
    int type = m_params.WeaponType;
    Vec3f position;
    fn_8040CDD0(&position, GetPosition(), &(const Vec3f &)Vec3f(0.0f, 30.0f, 0.0f));
    fn_80221BA8(&position, GetRotation(), &velocity, lbl_805F8308[type], -1);
}

void GunBigfoot::vfDisp_0x64() {
    void *loader = fn_800122D4();
    int slot = m_motionBase;
    void *model =
        fn_80012684(loader, slot, m_params.WeaponType != 0 ? kModelMissile : kModelVulcan);
    void *motion = fn_80012608(loader, slot, 10);
    m_render = fn_802CDB08(slot, MotionTable(lbl_804F8630[m_params.WeaponType], 2), model, motion);
    if (m_render.get() != 0) {
        void *data = fn_802CD420(m_render.get());
        if (data != 0) {
            fn_80415808(data, 0xC9E, OnModelUpdate, this);
        }
    }
}

void GunBigfoot::vfDisp_0x08() {
    EnemyDispCommon::vfDisp_0x08();
}

void GunBigfoot::vfDisp_0x10(float dt) {
    EnemyWalkerCommon::vfDisp_0x10(dt);
    if (m_jetOn) {
        if (m_jetSound.m_id == 0) {
            m_jetSound = fn_8019A908(this, 0x6022, 1);
        }
    } else {
        m_jetSound.fn_801D455C();
    }
    m_jetOn = 0;
}

void GunBigfoot::vfDisp_0x40(EnemyMotionEventArg &arg) {
    EnemyDispCommon::vfDisp_0x40(arg);
    if (arg.m_type == 0x5000 && arg.m_data->kind == 2 && arg.m_data->value == m_params.WeaponType) {
        Fire();
    }
}

void GunBigfoot::Fire() {
    switch (m_params.WeaponType) {
    case 0:
        FireVulcan();
        break;
    case 1:
        FireMissile();
        break;
    }
}

void GunBigfoot::FireVulcan() {
    Mtx34f *matrix = GetPartMatrix(0xC9F);
    if (matrix != 0) {
        Vec3f position;
        fn_8040FAA0(&position, matrix);
        Vec3f direction(0.0f, 0.0f, -1.0f);
        fn_800091D0(&direction, &(const Vec3f &)fn_8040B618(matrix, &direction));
        Weapon::ShotInfo shot(fn_8017D014(m_enemy), true, position, direction, lbl_805E07A0);
        fn_80051C34(lbl_805F8308[m_params.WeaponType], &shot);
        fn_801D54F4(fn_80053164(), 0x200C, &position, 0, 1);
    }
}

void GunBigfoot::FireMissile() {
    Mtx34f *matrix = GetPartMatrix(m_tube + 0xCD0);
    if (matrix != 0) {
        Vec3f position;
        fn_8040FAA0(&position, matrix);
        Vec3f direction(0.0f, 0.0f, -1.0f);
        fn_800091D0(&direction, &(const Vec3f &)fn_8040B618(matrix, &direction));
        boost::shared_ptr<Weapon::LockonTarget> target =
            boost::shared_ptr<Weapon::LockonTarget>(new Weapon::LockonTarget());
        if (target) {
            boost::weak_ptr<CharInfo> player = fn_800A8A7C(fn_80074194(), GetPosition());
            boost::shared_ptr<CharInfo> chara = player.lock();
            if (chara) {
                boost::shared_ptr<Player::PlayerBase> base =
                    boost::dynamic_pointer_cast<Player::PlayerBase>(chara);
                if (base.get() != 0 && base->m_target != 0) {
                    target->SetTarget(fn_80206574(base->m_target));
                    target->fn_80060F70();
                    fn_801D560C(fn_80053164(), 0x201D, 0, 0);
                }
            }
            Weapon::ShotInfo shot(fn_8017D014(m_enemy), target, true, position, direction,
                                  lbl_805E07A0);
            lbl_8051DDB4.create(shot);
        }
        if (++m_tube >= 8) {
            m_tube = 0;
        }
    }
}

void GunBigfoot::ResetTube() {
    m_tube &= 4;
}

bool GunBigfoot::vfDisp_0x44(int *command) {
    if (EnemyDispCommon::vfDisp_0x44(command)) {
        return true;
    }
    switch (*command) {
    case 18:
        fn_8000D8EC(fn_80009548(), 0, 0, 0.5f);
        fn_8034DE08(8, GetPosition(), -2);
        fn_8019AA40(this, 0x601F, 1);
        return true;
    case 17:
        fn_8019AA40(this, 0x6020, 1);
        fn_8017CA2C(this);
        return true;
    case 16:
        fn_8019AA40(this, 0x6021, 1);
        return true;
    case 26:
        m_jetOn = 1;
        return true;
    }
    return false;
}

float GunBigfoot::vfPara_0x6C() {
    if (m_params.WeaponType <= 0) {
        return 18.0f;
    }
    return 30.0f;
}

int GunBigfoot::vfPara_0x48() {
    return m_hovering ? 4 : 0;
}

int GunBigfoot::vfPara_0x4C() {
    return 2;
}

int GunBigfoot::vfPara_0x50() {
    return 3;
}

int GunBigfoot::vfPara_0x54() {
    return vfStat_0x24() == 4 ? 5 : 0;
}

int GunBigfoot::vfPara_0x58() {
    return 7;
}

int GunBigfoot::vfPara_0x5C() {
    return m_hovering ? 10 : 9;
}

int GunBigfoot::vfPara_0x60() {
    return m_hovering ? 10 : 11;
}

void GunBigfoot::vfStat_0x70(AttackCallbackParam &param) {
    EnemyStatusCommon::vfStat_0x70(param);
}

int GunBigfoot::vfStat_0xB8() {
    int kind = 0x56;
    if (m_params.WeaponType <= 0) {
        kind = 0x55;
    }
    return kind;
}

GunBigfootTask::GunBigfootTask(Task *parent, UnkPlacement_800C9CD0 *placement)
    : EnemyTemplate<GunBigfoot, GunBigfootAI>(parent, placement) {
    m_enemy = this;
    m_name = s_taskName;
}

GunBigfootTask::~GunBigfootTask() {}

} // namespace GunBigfoot

extern "C" const char *fn_80287014(UnkLabelQuery_800CBC20 *query) {
    static const char *s_weaponTypes[2] = {"Weapon : VULCAN", "Weapon : MISSILE"};
    static const char *s_appearTypes[5] = {
        "AppearType : WAIT_CROUCH",   "AppearType : HOVERING", "AppearType : DROP",
        "AppearType : ACT_ATTACKING", "AppearType : ZUTTO_HOVERING",
    };
    static int s_appearType = 0;

    switch (query->index) {
    case 7:
        s_appearType = query->value;
        return s_appearTypes[query->value];
    case 8:
        return s_weaponTypes[query->value];
    case 9:
        if (s_appearType == 2) {
            return query->params[query->index].label;
        }
        if (s_appearType == 1 || s_appearType == 4) {
            return "MoveWidth";
        }
        break;
    }
    return "";
}

namespace GunBigfoot {

void GunBigfootTask::LoadFiles() {
    void *loader = fn_800122D4();
    fn_8001299C(loader, 9, new ResourceOneFile("enemy/GunBigfootData.one", false));
}

void GunBigfootTask::DropFiles() {
    fn_800127F8(fn_800122D4(), 9);
}

void GunBigfootTask::Instantiate(UnkPlacement_800C9CD0 *placement) {
    new GunBigfootTask(fn_801EB304(fn_8001C4F8(), 0xE), placement);
}

} // namespace GunBigfoot

static GadgetParamDesc lbl_8054E6A4[] = {
    {4, "MoveRange", 0, 0, 0, 200.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchRange", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchAngle", 0, 0, 0, 0.0f, -360.0f, 360.0f, fn_800CBC20, 0},
    {4, "SearchWidth", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchHeight", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchHeightOffset", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "MoveSpeedRatio", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {1, "AppearType", 0, 0, 4, 0.0f, 0.0f, 0.0f, fn_80287014, 0},
    {1, "WeaponType", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_80287014, 0},
    {4, "OffsetPos_Y", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_80287014, 0},
    {0},
};

GadgetClassDesc lbl_8054E85C = {
    "GUN_BIGFOOT", GunBigfoot::GunBigfootTask::LoadFiles, GunBigfoot::GunBigfootTask::DropFiles,
    reinterpret_cast<void (*)(UnkPlacement_800C9CD0 *)>(GunBigfoot::GunBigfootTask::Instantiate),
    0, 0, 0x66, 2, 0, 5, lbl_8054E6A4,
};

/* Always false for the walker (meaning unknown); retail has it ahead of the stage editor's hooks. */
bool GunBigfoot::GunBigfoot::vfStat_0xC8() {
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
