/* BkSoldier (class name from RTTI; its s_taskName is "BkSoldier"): the Black Arms soldier, BK_SOLDIER
 * in the stage editor (enemy/BkSoldierData.one). Its body BkSoldierBase is a SoldierCommonBase
 * (Game/Enemy.h); it may carry a shield and one of eight weapons, ride an Air Saucer, and appears
 * standing, walking between up to three points, moving at random, at an offset or by warping
 * (AppearType). The unit also holds the first copies of the enemy framework's inline code: the
 * constructors and destructors of the bases, the do-nothing slots of the interfaces and the
 * virtual-base thunks. File name inferred from the class. Names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). The unit's code reaches its .data through a pooled base address, so the
 * linker keeps that section whole: Game/TNumEdit.h and Game/CharaColliBaseCharInfo.h supply the
 * RTTI base lists it starts with. */
/* Keeps the dead copy of bad_alloc's vtable in .data, as retail has it (see <new>). */
#define MSL_NEW_INLINE_BAD_ALLOC

#include <stdio.h>

#include "Game/TNumEdit.h"
/* This unit's .rodata is kept whole as well, and retail has unreferenced constant blocks of the
 * original headers in it (as in StretchGrass.cpp and HintRing.cpp): seven zero words after the
 * TNumEdit names, and the unit axes and a zero vector after the CharaColliBase names (their
 * meaning and types are unknown; the bytes are reproduced). */
static const float s_zeros[7] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
extern "C" {
typedef struct RwV3d {
    float x;
    float y;
    float z;
} RwV3d;
}
static const RwV3d s_xAxis = {1.0f, 0.0f, 0.0f};
static const RwV3d s_yAxis = {0.0f, 1.0f, 0.0f};
static const RwV3d s_zAxis = {0.0f, 0.0f, 1.0f};
static const float s_zeroVector[3] = {0.0f, 0.0f, 0.0f};
#include "Game/BkSoldier.h"
#include "Game/CharCommand.h"
#include "Game/EnemyRender.h"
#include "Game/Gadget.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathTypes.h"
#include "Game/Random.h"
#include "Game/ResourceOneFile.h"
#include "Game/TVehicleBase.h"
#include "Game/Vibration.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

/* MSL's std::fabs for floats, which the game calls out of line (as in DevilDoomCtrlUnit.cpp). */
namespace std {
float fabs(float x);
}

extern float lbl_805FAD38; /* pi */

using SonicteamUSA::System::CharaColli::AttackCallbackParam;

/* The initial values of EnemyStatusCommon's flag pairs (Game/Enemy.h). */
#pragma explicit_zero_data on
u8 lbl_805E76E8 = 0;
u8 lbl_805E76E9 = 0;
u8 lbl_805E76EA = 0;
u8 lbl_805E76EB = 0;
u8 lbl_805E76EC = 0;
#pragma explicit_zero_data reset
using SonicteamUSA::System::CharaColli::CharaColliAttack;

/* AccelerateInputCommand (command type 1) is declared in Game/CharCommand.h. */

/* The cursor a parameter's label callback receives (layout as in Helicopter.cpp). */
class UnkLabelQuery_800CBC20 {
  public:
    /* 0x0 */ const GadgetParamDesc *params;
    /* 0x4 */ s32 index; /* the parameter being shown */
    /* 0x8 */ s32 value; /* its current value */
};

/* EnemyShield (class name from the RTTI of its shared_ptr control block; code around
 * 0x801A7478): a soldier's shield (names inferred). */
class EnemyShield {
  public:
    void fn_801A7478();
    void fn_801A74C4();
    bool fn_801A74E8();
    void fn_801A7508(AttackCallbackParam &param);
};

/* EnemyWeapon (class name from RTTI; vtable 0x8053B218 at 0x64; constructor 0x801AB904): the
 * weapon a soldier carries (names inferred). */
class EnemyWeapon {
  public:
    EnemyWeapon(const boost::weak_ptr<CharInfo> &owner, int type);

    /* 0x00 */ u8 field_0x0[0x64];

    virtual ~EnemyWeapon();
    virtual void vf_0x0C(float dt);
};

/* The table of vehicles (class name unknown). */
class UnkVehicleSlot_801B48D4 {
  public:
    /* The vehicle of the slot (inline; weak copy 0x8018DF2C; name inferred). */
    boost::weak_ptr<Vehicle::TVehicleBase> GetVehicle() {
        return m_vehicle;
    }

    /* 0x00 */ u8 field_0x0[0x28];
    /* 0x28 */ boost::shared_ptr<Vehicle::TVehicleBase> m_vehicle;
};

extern "C" {
/* The enemies' shared helpers (behaviour inferred). */
void fn_8019AA40(Enemy *enemy, u16 sound, int arg);
void fn_8019A954(Enemy *enemy, u16 sound, int arg);
float fn_8019B184(Enemy *enemy, const Vec3f *direction);
void fn_801989D4(SoldierCommonBase *soldier, SoldierParam *params, Vec3f *position);
void fn_8017CA2C(Enemy *enemy);
boost::weak_ptr<EnemyShield> fn_801A752C(Enemy *enemy, const EnemyMotionTable &table, int arg);
/* The weapon's state calls. */
bool fn_801AAC68(EnemyWeapon *weapon);
bool fn_801AAC40(EnemyWeapon *weapon);
void fn_801AAB68(EnemyWeapon *weapon);
void fn_801AABD4(EnemyWeapon *weapon);
void fn_801AA8EC(EnemyWeapon *weapon);
void fn_801AA9E8(EnemyWeapon *weapon);
bool fn_801AAA54(EnemyWeapon *weapon);
void fn_801AA830(EnemyWeapon *weapon);
void fn_801AA868(EnemyWeapon *weapon);
void fn_801AA8A0(EnemyWeapon *weapon, const Vec3f *offset);
void fn_801AA8E4(EnemyWeapon *weapon, float value);
void fn_801AB650(EnemyWeapon *weapon, void *part);
void fn_801AB3F0(EnemyWeapon *weapon);
void *fn_801A46E4(void *model, int part);
/* The vehicles. */
bool fn_801B4B5C(int kind);
UnkVehicleSlot_801B48D4 *fn_801B48D4(int kind);
void fn_801B5D1C(UnkVehicleSlot_801B48D4 *slot);
void fn_801B5D24(UnkVehicleSlot_801B48D4 *slot, const Vec3f *position, const Vec3f *rotation);
void fn_801B67CC(Vehicle::TVehicleBase *vehicle, const boost::weak_ptr<CharInfo> &rider);
void fn_801B6718(Vehicle::TVehicleBase *vehicle, const boost::weak_ptr<CharInfo> &rider);
void fn_801B6944(Vehicle::TVehicleBase *vehicle);
void fn_800C9938(int arg);
/* Sound and rumble. */
void *fn_80053164(void);
void fn_801D54F4(void *manager, int sound, const Vec3f *position, int arg3, int arg4);
boost::weak_ptr<Vibration::RequestHandle> fn_8034DE08(int pattern, const Vec3f *position,
                                                      int arg);
/* Resources and models. */
void *fn_80012684(void *loader, int slot, int kind);
void *fn_80012608(void *loader, int slot, int kind);
boost::shared_ptr<EnemyRender> fn_802CDB08(int slot, const EnemyMotionTable &table, void *model,
                                           void *motion);
void *fn_802CD420(EnemyRender *render);
void fn_80415808(void *motion, int id, void (*callback)(void *), void *data);
/* Vector helpers returning by value: v normalized, and v turned about y by angle. */
Vec3f fn_8040CEE8(const Vec3f *v);
Vec3f fn_8040D0AC(const Vec3f *v, float angle);
}

/* fn_8040CD74 as a function that returns its result (see Game/EnemyRender.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *lhs, const Vec3f *rhs);

/* A motion table built in place (not original; it gives retail's stack layout, as in
 * EggLastMechSlotSwitch.cpp). */
struct MotionTable : public EnemyMotionTable {
    MotionTable(const EnemyMotionEntry *e, s32 c) {
        entries = e;
        count = c;
    }
};

/* What the model's update callback receives (layout from its body; names inferred). */
struct UnkModelCallbackArg {
    /* 0x0 */ void *m_node;
    /* 0x4 */ BkSoldierBase *m_owner;
};

extern "C" {
/* A model node's matrix, and setting it (see BKTank.cpp). */
void fn_80016370(Mtx34f *matrix, void *node);
void fn_800163F4(void *node, const Mtx34f *matrix);
/* Matrix and vector helpers returning by value (see BirdBase.cpp). */
Vec3f fn_8040FB08(const Mtx34f *matrix); /* translation */
void fn_8040FA20(Mtx34f *matrix, const Vec3f *translation);
Mtx34f fn_8040BEF8(const Mtx34f *matrix);              /* inverse */
Mtx34f fn_8001E044(const Mtx34f *a, const Mtx34f *b); /* product */
void fn_8040BAC4(Mtx34f *matrix, const Mtx34f *other);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
/* The pitch and yaw of a direction (behaviour inferred). */
Vec2f fn_8040F760(const Vec3f *direction);
/* value moved toward target by at most step (behaviour inferred). */
float fn_8040F314(float value, float target, float step);
}

/* Three floats copied as a plain struct (not original). */
struct Vec3fWords {
    float x;
    float y;
    float z;
};

/* The motion table (steps in .rodata and .sdata2). */
static const MotionSequenceStep lbl_805F5210[] = {{1, -1, -1, 30, 1}};
static const MotionSequenceStep lbl_805F5218[] = {{2, -1, -1, 30, 1}};
static const MotionSequenceStep lbl_805F5220[] = {{3, -1, -1, 30, 1}};
static const MotionSequenceStep lbl_805F5228[] = {{8, -1, -1, 10, 1}};
static const MotionSequenceStep lbl_805F5230[] = {{9, -1, -1, 10, 1}};
static const MotionSequenceStep lbl_805F5238[] = {{10, -1, -1, 10, 1}};
static const MotionSequenceStep lbl_805F5240[] = {{1, -1, -1, 30, 0}};
static const MotionSequenceStep lbl_804C88E8[] = {
    {4, -1, 14, 0, 0}, {4, 14, 15, 0, 2}, {4, 15, -1, 0, 0}, {14, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_804C8908[] = {
    {5, -1, 14, 0, 0}, {5, 14, 15, 0, 2}, {5, 15, -1, 0, 0}, {15, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_804C8928[] = {{6, -1, -1, 30, 2}, {7, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_804C8938[] = {{17, -1, -1, 30, 0}, {11, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_804C8948[] = {{18, -1, -1, 30, 0}, {12, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_804C8958[] = {{20, -1, -1, 10, 0}, {16, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_804C8968[] = {
    {19, 10, -1, 20, 0}, {19, 10, -1, 30, 0}, {13, -1, -1, 0, 0}, {-1, -1, -1, 20, 0}};
static const MotionSequenceStep lbl_804C8988[] = {{19, -1, -1, 10, 0}, {19, 10, -1, 0, 1}};
static const MotionSequenceStep lbl_804C8998[] = {{21, -1, -1, 10, 0}, {21, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_804C89A8[] = {
    {4, -1, 14, 0, 0}, {4, 14, 15, 0, 2}, {4, 15, -1, 0, 0}};
static const MotionSequenceStep lbl_804C89C0[] = {
    {5, -1, 14, 0, 0}, {5, 14, 15, 0, 2}, {5, 15, -1, 0, 0}};
static const MotionSequence lbl_804C89D8[] = {
    {lbl_805F5210, 1, -3}, {lbl_805F5218, 1, -3}, {lbl_805F5220, 1, -3}, {lbl_805F5228, 1, -3},
    {lbl_805F5230, 1, -3}, {lbl_805F5238, 1, -3}, {lbl_804C88E8, 4, -3}, {lbl_804C8908, 4, -3},
    {lbl_804C8928, 2, -3}, {lbl_804C8938, 2, -3}, {lbl_804C8948, 2, -3}, {lbl_804C8958, 2, -3},
    {lbl_804C8968, 4, -3}, {lbl_804C8988, 2, -3}, {lbl_804C8998, 2, -3}, {lbl_805F5240, 1, -3},
    {lbl_804C89A8, 3, -3}, {lbl_804C89C0, 3, -3},
};
static const MotionSequenceTable lbl_805F5248 = {lbl_804C89D8, 18};

/* The weapon types the WeaponType parameter selects. */
static const s32 lbl_804C8A68[] = {0, 0x1F, 8, 9, 0xF, 0x1A, 0x18, 0xB, 0xA};

/* The model and shield motion tables. */
static const EnemyMotionEntry lbl_805F5250[] = {{5, -1}};
static const EnemyMotionEntry lbl_805F5254[] = {{3, -1}, {4, 0x2345}};

/* The model's update callback: turns the soldier's upper body toward its target, at most 30
 * degrees up or down and 10 to the side (behaviour inferred). */
void BkSoldierBase::OnModelUpdate(void *data) {
    UnkModelCallbackArg *arg = static_cast<UnkModelCallbackArg *>(data);
    BkSoldierBase *self = arg->m_owner;
    Mtx34f matrix;
    fn_80016370(&matrix, arg->m_node);
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)fn_8040FB08(&matrix));
    Vec2f angles(0.0f, 0.0f);
    Matrix34 rotation = MakeRotation(self->GetRotation(), 0);
    if (self->field_0x359 && self->fn_8018D120()) {
        fn_8040FA20(&rotation, &position);
        Vec3f local;
        fn_800091EC(&local, &(const Vec3f &)fn_8040B67C(&(const Mtx34f &)fn_8040BEF8(&rotation),
                                                        &self->Enemy::field_0x6C));
        Vec3f direction;
        fn_800091EC(&direction, &(const Vec3f &)fn_8040CEE8(&local));
        angles = fn_8040F760(&direction);
        float limit = 30.0f * (lbl_805FAD38 / 180.0f);
        if (angles.x > limit) {
            angles.x = limit;
        } else if (angles.x < -limit) {
            angles.x = -limit;
        }
        limit = 10.0f * (lbl_805FAD38 / 180.0f);
        if (angles.y > limit) {
            angles.y = limit;
        } else if (angles.y < -limit) {
            angles.y = -limit;
        }
    }
    self->field_0x370.x = fn_8040F314(self->field_0x370.x, angles.x, 0.1f * self->GetFrameTime2());
    self->field_0x370.y = fn_8040F314(self->field_0x370.y, angles.y, 0.1f * self->GetFrameTime2());
    Mtx34f turn;
    fn_8040B6F8(&turn);
    fn_8040B848(&turn, self->field_0x370.y + self->GetRotation()->y);
    fn_8040B884(&turn, self->field_0x370.x);
    fn_8040BAC4(&turn, &(const Mtx34f &)fn_8040BEF8(&rotation));
    fn_8040C148(&matrix, &(const Mtx34f &)fn_8001E044(&turn, &matrix));
    fn_8040FA20(&matrix, &position);
    fn_800163F4(arg->m_node, &matrix);
}

BkSoldierBase::BkSoldierBase()
    : SoldierCommonBase(5), m_weapon(0), field_0x35C(fn_8040EC80(8.0f, 12.0f)),
      field_0x370(0.0f, 0.0f, 0.0f) {
    m_enemy = this;
    vfPara_0x40(reinterpret_cast<int>(&lbl_805F5248));
    field_0x348 = 0;
    field_0x360 = 0.0f;
    field_0x364 = 0;
    field_0x365 = 0;
    fn_8040E970(&field_0x370);
}

BkSoldierBase::~BkSoldierBase() {
    if (!m_enemy->m_set->vfSet_0x08()) {
        ReleaseVehicle();
    }
    if (!m_shield.expired()) {
        m_shield.lock()->fn_801A7478();
        m_shield.reset();
    }
    if (m_weapon != 0) {
        delete m_weapon;
        m_weapon = 0;
    }
    if (m_render.get() != 0) {
        void *motion = fn_802CD420(m_render.get());
        if (motion != 0) {
            fn_80415808(motion, 0x15, 0, 0);
        }
    }
}

int BkSoldierBase::vfStat_0xB8() {
    int result = 0x58;
    if (m_params.HaveShield != 0) {
        result = 0x59;
    }
    return result;
}

void BkSoldierBase::vfSet_0x1C(UnkPlacement_800C9CD0 *placement) {
    vfSet_0x20(placement);
    /* Copied as a plain struct (word loads and stores), unlike Vec3f's own copy. */
    Vec3fWords rotation = *reinterpret_cast<Vec3fWords *>(GetRotation());
    vfDisp_0x38(GetPosition(), reinterpret_cast<Vec3f *>(&rotation));
    m_ctrl->vfCtrl_0x20();
}

void BkSoldierBase::vfSet_0x20(UnkPlacement_800C9CD0 *placement) {
    TEnemySetBase::vfSet_0x20(placement);
    fn_800C9DBC(placement, &m_params, sizeof(m_params));
    if (m_params.HaveShield < 0) {
        m_params.HaveShield = 0;
    }
    if (m_params.HaveShield > 1) {
        m_params.HaveShield = 1;
    }
    if (m_params.WeaponType < 0) {
        m_params.WeaponType = 0;
    }
    if (m_params.WeaponType > 8) {
        m_params.WeaponType = 8;
    }
    if (m_params.AppearType < 0) {
        m_params.AppearType = 0;
    }
    if (m_params.AppearType > 5) {
        m_params.AppearType = 5;
    }
    if (m_params.Pos0_WaitSec < 0.0f) {
        m_params.Pos0_WaitSec = 0.0f;
    }
    if (m_params.Pos0_WaitSec > 65536.0f) {
        m_params.Pos0_WaitSec = 65536.0f;
    }
    if (m_params.Pos0_MoveSpeedRatio < 0.0f) {
        m_params.Pos0_MoveSpeedRatio = 0.0f;
    }
    if (m_params.Pos0_MoveSpeedRatio > 65536.0f) {
        m_params.Pos0_MoveSpeedRatio = 65536.0f;
    }
    if (m_params.Pos0_WaitType < 0) {
        m_params.Pos0_WaitType = 0;
    }
    if (m_params.Pos0_WaitType > 2) {
        m_params.Pos0_WaitType = 2;
    }
    if (m_params.Pos1_X < -65536.0f) {
        m_params.Pos1_X = -65536.0f;
    }
    if (m_params.Pos1_X > 65536.0f) {
        m_params.Pos1_X = 65536.0f;
    }
    if (m_params.Pos1_Z < -65536.0f) {
        m_params.Pos1_Z = -65536.0f;
    }
    if (m_params.Pos1_Z > 65536.0f) {
        m_params.Pos1_Z = 65536.0f;
    }
    if (m_params.Pos1_WaitSec < 0.0f) {
        m_params.Pos1_WaitSec = 0.0f;
    }
    if (m_params.Pos1_WaitSec > 65536.0f) {
        m_params.Pos1_WaitSec = 65536.0f;
    }
    if (m_params.Pos1_MoveSpeedRatio < 0.0f) {
        m_params.Pos1_MoveSpeedRatio = 0.0f;
    }
    if (m_params.Pos1_MoveSpeedRatio > 65536.0f) {
        m_params.Pos1_MoveSpeedRatio = 65536.0f;
    }
    if (m_params.Pos1_WaitType < 0) {
        m_params.Pos1_WaitType = 0;
    }
    if (m_params.Pos1_WaitType > 2) {
        m_params.Pos1_WaitType = 2;
    }
    if (m_params.Pos2_X < -65536.0f) {
        m_params.Pos2_X = -65536.0f;
    }
    if (m_params.Pos2_X > 65536.0f) {
        m_params.Pos2_X = 65536.0f;
    }
    if (m_params.Pos2_Z < -65536.0f) {
        m_params.Pos2_Z = -65536.0f;
    }
    if (m_params.Pos2_Z > 65536.0f) {
        m_params.Pos2_Z = 65536.0f;
    }
    if (m_params.Pos2_WaitSec < 0.0f) {
        m_params.Pos2_WaitSec = 0.0f;
    }
    if (m_params.Pos2_WaitSec > 65536.0f) {
        m_params.Pos2_WaitSec = 65536.0f;
    }
    if (m_params.Pos2_MoveSpeedRatio < 0.0f) {
        m_params.Pos2_MoveSpeedRatio = 0.0f;
    }
    if (m_params.Pos2_MoveSpeedRatio > 65536.0f) {
        m_params.Pos2_MoveSpeedRatio = 65536.0f;
    }
    if (m_params.Pos2_WaitType < 0) {
        m_params.Pos2_WaitType = 0;
    }
    if (m_params.Pos2_WaitType > 2) {
        m_params.Pos2_WaitType = 2;
    }
    fn_801989D4(this, &m_params, &m_enemy->field_0x48);
    if (m_appearType == 4 || m_appearType == 5) {
        GetPosition()->x += m_params.Pos1_X;
        GetPosition()->y += m_params.Pos1_WaitSec;
        GetPosition()->z += m_params.Pos1_Z;
    }
    if (m_appearType == 5) {
        field_0x360 = 1.0f;
    }
}

void BkSoldierBase::vfStat_0x38() {
    if (m_appearType == 4 || m_appearType == 5) {
        void *key = m_ctrl->vfCtrl_0x10();
        if (!m_enemy->field_0xA8.Contains(key)) {
            m_enemy->m_status->vfStat_0x30(false);
            m_enemy->m_status->vfStat_0x3C(true);
            if (m_weapon != 0) {
                fn_801AA830(m_weapon);
            }
            return;
        }
        if ((field_0x360 -= GetFrameTime()) >= 0.0f) {
            if (!field_0x364) {
                field_0x364 = 1;
                vfDisp_0x50();
            }
            m_enemy->m_status->vfStat_0x30(false);
            m_enemy->m_status->vfStat_0x3C(true);
            return;
        }
    }
    if (m_weapon != 0) {
        fn_801AA868(m_weapon);
    }
    m_enemy->m_status->vfStat_0x3C(false);
    m_enemy->m_status->vfStat_0x30(true);
}

void BkSoldierBase::vfDisp_0x64() {
    void *loader = fn_800122D4();
    int slot = m_motionBase;
    void *model = fn_80012684(loader, slot, 6);
    void *motion = fn_80012608(loader, slot, 8);
    m_render = fn_802CDB08(slot, MotionTable(lbl_805F5250, 1), model, motion);
    void *data = fn_802CD420(m_render.get());
    if (data != 0) {
        fn_80415808(data, 0x15, OnModelUpdate, this);
    }
}

inline boost::weak_ptr<EnemyShield> BkSoldierBase::CreateShield() {
    boost::weak_ptr<EnemyShield> shield = fn_801A752C(this, MotionTable(lbl_805F5254, 2), 0x1C);
    return shield;
}

void BkSoldierBase::vfDisp_0x08() {
    EnemyDispCommon::vfDisp_0x08();
    if (m_shield.expired() && field_0x270) {
        m_shield = CreateShield();
    }
    if (m_weapon == 0 && m_weaponType != 0) {
        m_weapon = new EnemyWeapon(fn_8017D014(this), lbl_804C8A68[GetWeaponType()]);
    }
    if (m_vehicle.expired() && m_params.RideAirSaucer != 0 && fn_801B4B5C(7)) {
        UnkVehicleSlot_801B48D4 *slot = fn_801B48D4(7);
        if (slot != 0) {
            fn_800C9938(m_enemy->m_set->vfSet_0x2C());
            fn_801B5D1C(slot);
            fn_801B5D24(slot, m_enemy->GetPosition(), m_enemy->GetRotation());
            m_vehicle = slot->GetVehicle();
            boost::shared_ptr<Vehicle::TVehicleBase> vehicle = m_vehicle.lock();
            if (vehicle) {
                fn_801B67CC(vehicle.get(), fn_8017D014(this));
            }
        }
    }
}

void BkSoldierBase::vfDisp_0x10(float dt) {
    EnemyWalkerCommon::vfDisp_0x10(dt);
    if (!m_shield.expired()) {
        if (m_shield.lock()->fn_801A74E8()) {
            m_shield.reset();
            field_0x270 = 0;
        }
    } else if (vfStat_0x4C() && !vfStat_0x60()) {
        fn_8019A954(this, 0x6015, 1);
    }
    if (m_weapon != 0) {
        if (fn_801AAC68(m_weapon)) {
            if (field_0x365) {
                if (!fn_801AAA54(m_weapon) && m_weapon != 0) {
                    if (fn_801AAC68(m_weapon)) {
                        fn_801AABD4(m_weapon);
                    } else if (fn_801AAC40(m_weapon)) {
                        fn_801AA9E8(m_weapon);
                    }
                }
            } else if (fn_801AAA54(m_weapon) && m_weapon != 0) {
                if (fn_801AAC68(m_weapon)) {
                    fn_801AAB68(m_weapon);
                } else if (fn_801AAC40(m_weapon)) {
                    fn_801AA8EC(m_weapon);
                }
            }
        }
        field_0x365 = 0;
        void *part = fn_801A46E4(m_enemy->m_disp->vfDisp_0x1C(), 0x18);
        fn_801AB650(m_weapon, part);
        fn_801AA8E4(m_weapon, field_0xEC);
        m_weapon->vf_0x0C(dt);
    }
    void *key = m_enemy->m_ctrl->vfCtrl_0x10();
    if (m_enemy->field_0xA8.Contains(key)) {
        if ((field_0x35C -= dt) < 0.0f) {
            field_0x35C = fn_8040EC80(8.0f, 12.0f);
            static bool s_toggle = false;
            s_toggle ^= 1;
            fn_8019A954(this, s_toggle ? 0x6013 : 0x6014, 1);
        }
    }
}

void BkSoldierBase::vfDisp_0x40(EnemyMotionEventArg &arg) {
    EnemyDispCommon::vfDisp_0x40(arg);
    EnemyWeapon *weapon = m_weapon;
    if (weapon != 0 && arg.m_type == 0x5000) {
        if (arg.m_data->kind == 2) {
            weapon->field_0x0[0] = 0;
            fn_801AB3F0(weapon);
        }
    } else if (weapon != 0 && arg.m_type == 0xD000 && arg.m_data->value == 0) {
        field_0x365 = 1;
    }
}

bool BkSoldierBase::vfDisp_0x44(int *command) {
    if (EnemyDispCommon::vfDisp_0x44(command)) {
        return true;
    }
    switch (*command) {
    case 3:
        fn_8019AA40(this, 0x6011, 1);
        fn_8017CA2C(this);
        break;
    case 4:
        fn_8034DE08(10, GetPosition(), -2);
        fn_8019AA40(this, 0x6010, 1);
        break;
    case 7:
        fn_8019AA40(this, 0x2005, 1);
        break;
    }
    return true;
}

int BkSoldierBase::vfPara_0x48() {
    return vfPara_0x54();
}

int BkSoldierBase::vfPara_0x4C() {
    if (m_params.RideAirSaucer != 0) {
        return vfPara_0x48();
    }
    switch (m_weaponType) {
    case 0:
    case 1:
        return 3;
    }
    return field_0x270 ? 5 : 4;
}

int BkSoldierBase::vfPara_0x50() {
    return 8;
}

int BkSoldierBase::vfPara_0x54() {
    switch (m_weaponType) {
    case 0:
    case 1:
        return 0;
    }
    return field_0x270 ? 2 : 1;
}

int BkSoldierBase::vfPara_0x58() {
    switch (m_weaponType) {
    case 0:
        return field_0x348 == 0 ? 9 : 10;
    case 1:
        return 11;
    }
    return 12;
}

int BkSoldierBase::vfPara_0x5C() {
    return field_0x358 ? 6 : 7;
}

int BkSoldierBase::vfPara_0x60() {
    return field_0x358 ? 16 : 17;
}

int BkSoldierBase::vfPara_0x64() {
    return 14;
}

int BkSoldierBase::vf_0x238(int index) {
    int result = 0;
    switch (m_points[index].field_0x14) {
    case 0:
        if (m_weaponType != 0) {
            result = m_enemy->m_para->vfPara_0x58();
        } else {
            result = vfPara_0x48();
        }
        break;
    case 1:
        result = 15;
        break;
    case 2:
        result = vfPara_0x48();
        break;
    }
    return result;
}

int BkSoldierBase::vf_0x23C() {
    int result = 0;
    switch (m_appearType) {
    case 0:
        break;
    case 1:
        result = 1;
        break;
    case 2:
        result = 2;
        break;
    case 3:
        result = 3;
        break;
    case 4:
    case 5:
        break;
    }
    return result;
}

void BkSoldierBase::vfStat_0x70(AttackCallbackParam &param) {
    boost::shared_ptr<CharaColliAttack> attacker(param.m_attacker);
    if (!m_shield.expired() && attacker.get() != 0) {
        if (attacker->m_flags & 0x40) {
            if (std::fabs(fn_8019B184(m_enemy,
                                      reinterpret_cast<Vec3f *>(&param.field_0x8[0x34 - 0x8]))) <
                0.5f * lbl_805FAD38) {
                param.m_result = 5;
                vfStat_0xA8()->Set(5);
                fn_8019AA40(this, 0x6076, 1);
                return;
            }
        }
        if (!(attacker->m_flags & 4)) {
            m_shield.lock()->fn_801A7508(param);
            param.m_result = 1;
            if (m_shield.lock()->fn_801A74E8()) {
                fn_8019AA40(this, 0x6077, 1);
                field_0x75 = 1;
                m_shield.reset();
                field_0x270 = 0;
            } else {
                fn_8019AA40(this, 0x6076, 1);
                vfStat_0xA8()->Set(5);
            }
        }
        EnemyStatusCommon::vfStat_0x70(param);
    } else {
        EnemyStatusCommon::vfStat_0x70(param);
    }
    if (m_enemy->m_status->vfStat_0x74() == 0) {
        field_0x358 = 1;
    } else {
        field_0x358 = 0;
    }
}

void BkSoldierBase::vfDisp_0x50() {
    EnemyDispCommon::vfDisp_0x50();
    fn_801D54F4(fn_80053164(), 0x6012, GetPosition(), 0, 1);
}

void BkSoldierBase::fn_8018D3B0() {
    if (m_weapon != 0) {
        if (fn_801AAC68(m_weapon)) {
            fn_801AABD4(m_weapon);
        } else if (fn_801AAC40(m_weapon)) {
            fn_801AA9E8(m_weapon);
        }
    }
}

void BkSoldierBase::fn_8018D34C() {
    if (m_weapon != 0) {
        if (fn_801AAC68(m_weapon)) {
            fn_801AAB68(m_weapon);
        } else if (fn_801AAC40(m_weapon)) {
            fn_801AA8EC(m_weapon);
        }
    }
}

void BkSoldierBase::ReleaseVehicle() {
    if (boost::shared_ptr<Vehicle::TVehicleBase> vehicle = m_vehicle.lock()) {
        fn_801B6944(vehicle.get());
        m_vehicle.reset();
    }
}

void BkSoldierBase::GetOffVehicle() {
    m_params.RideAirSaucer = 0;
    if (boost::shared_ptr<Vehicle::TVehicleBase> vehicle = m_vehicle.lock()) {
        vehicle->OnGetOff();
        fn_801B6718(vehicle.get(), fn_8017D014(this));
        m_vehicle.reset();
        GetRotation()->x = 0.0f;
        GetRotation()->z = 0.0f;
    }
}

bool BkSoldierBase::fn_8018D120() {
    return field_0xA8.Contains(m_ctrl->vfCtrl_0x10());
}

void BkSoldierBase::vfStat_0xA0() {
    EnemyStatusCommon::vfStat_0xA0();
    fn_8019A954(this, 0x6016, 1);
    if (!m_shield.expired()) {
        m_shield.lock()->fn_801A74C4();
        m_shield.reset();
    }
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
        m_params.WeaponType = 0;
    }
}

void BkSoldierBase::vfStat_0xC0() {
    if (!m_vehicle.expired()) {
        GetOffVehicle();
    }
}

void BkSoldierBase::vfMove_0x0C(float dt) {
    if (m_params.RideAirSaucer != 0) {
        if (boost::shared_ptr<Vehicle::TVehicleBase> vehicle = m_vehicle.lock()) {
            Vec3f direction;
            fn_800091EC(&direction,
                        &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&this->Enemy::field_0x6C, GetPosition()));
            vehicle->m_rotation.y = GetRotation()->y;
            vehicle->PushCommand(new AccelerateInputCommand(fn_8040CEE8(&direction), 4));
            vehicle->GetSeat(GetPosition(), GetRotation());
            return;
        }
    }
    EnemyMoveCommon::vfMove_0x0C(dt);
}

extern "C" const char *fn_8018CAF4(UnkLabelQuery_800CBC20 *query) {
    static const char *s_weaponTypes[9] = {
        "Weapon : NONE",       "Weapon : BLACK_SWORD", "Weapon : LIGHT_SHOT",
        "Weapon : FLASH_SHOT", "Weapon : BLACK_BARREL", "Weapon : SPLITTER",
        "Weapon : VACUUM_POD", "Weapon : HEAVY_SHOT",  "Weapon : RING_SHOT",
    };
    static const char *s_appearTypes[6] = {
        "AppearType : STAND",       "AppearType : LINEAR_MOVE", "AppearType : TRIANGLE_MOVE",
        "AppearType : RANDOM_MOVE", "AppearType : OFFSETPOS",   "AppearType : WARP",
    };
    static const char *s_waitTypes[3] = {"WaitType : ATTACK", "WaitType : HIDE", "WaitType : KAMAE"};
    static char s_buffer[0x80];
    static int s_appearType = 0;

    switch (query->index) {
    case 8:
        return s_weaponTypes[query->value];
    case 9:
        s_appearType = query->value;
        return s_appearTypes[query->value];
    case 10:
        if (s_appearType == 3) {
            sprintf(s_buffer, "Rand_%s", s_waitTypes[query->value]);
        } else if (s_appearType == 1 || s_appearType == 2) {
            sprintf(s_buffer, "Pos0_%s", s_waitTypes[query->value]);
        } else {
            sprintf(s_buffer, "Stnd_%s", s_waitTypes[query->value]);
        }
        return s_buffer;
    case 11:
        if (s_appearType == 1 || s_appearType == 2 || s_appearType == 3) {
            return query->params[query->index].label;
        }
        break;
    case 12:
        if (s_appearType == 3) {
            sprintf(s_buffer, "Rand_MoveSpeedRatio %f", query->value);
            return s_buffer;
        }
        if (s_appearType == 1 || s_appearType == 2) {
            sprintf(s_buffer, "Pos0_MoveSpeedRatio %f", query->value);
            return s_buffer;
        }
        break;
    case 13:
    case 14:
        if (s_appearType == 1 || s_appearType == 2 || s_appearType == 4 || s_appearType == 5) {
            return query->params[query->index].label;
        }
        break;
    case 15:
        if (s_appearType == 1 || s_appearType == 2) {
            sprintf(s_buffer, "Pos1_%s", s_waitTypes[query->value]);
            return s_buffer;
        }
        break;
    case 16:
        if (s_appearType == 3) {
            return "RandomMoveRange";
        }
        if (s_appearType == 4 || s_appearType == 5) {
            return "Pos1_Y";
        }
        if (s_appearType == 1 || s_appearType == 2) {
            return query->params[query->index].label;
        }
        break;
    case 17:
        if (s_appearType == 1 || s_appearType == 2) {
            sprintf(s_buffer, "Pos1_MoveSpeedRatio %f", query->value);
            return s_buffer;
        }
        break;
    case 18:
    case 19:
    case 21:
        if (s_appearType == 2) {
            return query->params[query->index].label;
        }
        break;
    case 22:
        if (s_appearType == 2) {
            sprintf(s_buffer, "Pos2_MoveSpeedRatio %f", query->value);
            return s_buffer;
        }
        break;
    case 20:
        if (s_appearType == 2) {
            sprintf(s_buffer, "Pos2_%s", s_waitTypes[query->value]);
            return s_buffer;
        }
        break;
    }
    return "";
}

/* Not called anywhere: the linker strips it. Retail pools the float constants 0 and 0.5 ahead of
 * the label function's strings in .sdata2; this function, defined after the label function, makes
 * the compiler pool them there (see docs/research-notes.md; the original reason is unknown, and
 * the name is a placeholder). */
static float UnkConstantUser(float x) {
    if (x < 0.5f) {
        return 0.0f;
    }
    return x;
}

BkSoldier::BkSoldier(Task *parent, UnkPlacement_800C9CD0 *placement)
    : EnemyTemplate<BkSoldierBase, BkSoldierAI>(parent, placement) {
    m_enemy = this;
    m_name = s_taskName;
}

BkSoldier::~BkSoldier() {}

void BkSoldier::LoadFiles() {
    void *loader = fn_800122D4();
    fn_8001299C(loader, 0xC, new ResourceOneFile("enemy/BkSoldierData.one", false));
}

void BkSoldier::DropFiles() {
    fn_800127F8(fn_800122D4(), 0xC);
}

void BkSoldier::Instantiate(UnkPlacement_800C9CD0 *placement) {
    new BkSoldier(fn_801EB304(fn_8001C4F8(), 0xE), placement);
}

static GadgetParamDesc lbl_80530980[] = {
    {4, "MoveRange", 0, 0, 0, 200.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchRange", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchAngle", 0, 0, 0, 0.0f, -360.0f, 360.0f, fn_800CBC20, 0},
    {4, "SearchWidth", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchHeight", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchHeightOffset", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "MoveSpeedRatio", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {1, "HaveShield", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "WeaponType", 0, 0, 8, 0.0f, 0.0f, 0.0f, fn_8018CAF4, 0},
    {1, "AppearType", 0, 0, 5, 0.0f, 0.0f, 0.0f, fn_8018CAF4, 0},
    {1, "Pos0_WaitType", 0, 0, 2, 0.0f, 0.0f, 0.0f, fn_8018CAF4, 0},
    {4, "Pos0_WaitSec", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_8018CAF4, 0},
    {4, "Pos0_MoveSpeedRatio", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_8018CAF4, 0},
    {4, "Pos1_X", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_8018CAF4, 0},
    {4, "Pos1_Z", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_8018CAF4, 0},
    {1, "Pos1_WaitType", 0, 0, 2, 0.0f, 0.0f, 0.0f, fn_8018CAF4, 0},
    {4, "Pos1_WaitSec", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_8018CAF4, 0},
    {4, "Pos1_MoveSpeedRatio", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_8018CAF4, 0},
    {4, "Pos2_X", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_8018CAF4, 0},
    {4, "Pos2_Z", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_8018CAF4, 0},
    {1, "Pos2_WaitType", 0, 0, 2, 0.0f, 0.0f, 0.0f, fn_8018CAF4, 0},
    {4, "Pos2_WaitSec", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_8018CAF4, 0},
    {4, "Pos2_MoveSpeedRatio", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_8018CAF4, 0},
    {1, "RideAirSaucer", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80530D68 = {
    "BK_SOLDIER", BkSoldier::LoadFiles, BkSoldier::DropFiles,
    reinterpret_cast<void (*)(UnkPlacement_800C9CD0 *)>(BkSoldier::Instantiate), 0, 0, 0x8D, 2, 0,
    5, lbl_80530980,
};

const char *BkSoldier::s_taskName = "BkSoldier";

/* Not called anywhere: the linker strips it. Retail's .data, which the linker keeps whole, has the
 * zero-filled copy of sp_counted_base's vtable after EnemyTemplate's; this function, defined
 * last, makes the compiler create that vtable first and so place it there (as in Spring.cpp; the
 * original reason is unknown, and the name is a placeholder). */
static void UnkCountedBaseUser(boost::detail::sp_counted_base *base) {
    base->boost::detail::sp_counted_base::~sp_counted_base();
}
