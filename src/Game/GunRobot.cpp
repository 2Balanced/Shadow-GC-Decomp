/* GunRobot (class name from RTTI; its task's s_taskName is "GunRobot"): the G.U.N. Robot, GUN_ROBOT
 * in the stage editor (enemy/GunRobotData.one). Its body GunRobot is an EnemyWalker
 * (Game/EnemyWalker.h) that carries one of six guns, appears waiting, at an offset or by warping
 * (AppearType) and waits, walks between up to three points or moves at random (WaitActMoveType).
 * File name inferred from the class. Names marked "(name inferred)" are not original. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first). The unit's
 * code reaches its .data and .rodata through pooled base addresses, so the linker keeps those
 * sections whole: Game/TNumEdit.h and Game/CharaColliBaseCharInfo.h supply the RTTI base lists the
 * .data starts with (as in BkSoldier.cpp). */

#include <stdio.h>
#include <string.h>

#include "Game/TNumEdit.h"
/* The .rodata is kept whole, with unreferenced constant blocks of the original headers in it (as
 * in BkSoldier.cpp): seven zero words after the TNumEdit names and a zero vector after the
 * CharaColliBase names. */
static const float s_zeros[7] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
static const float s_zeroVector[3] = {0.0f, 0.0f, 0.0f};
#include "Game/GunRobot.h"
#include "Game/EnemyRender.h"
#include "Game/Gadget.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathTypes.h"
#include "Game/Random.h"
#include "Game/ResourceOneFile.h"
#include "Game/Vibration.h"
#include "Effect/System/EffectElement.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

using Effect::System::EffectElement;

extern float lbl_805FAD38; /* pi */

/* EnemyWeapon (class name from RTTI; vtable 0x8053B218 at 0x64; constructor 0x801AB904): the
 * weapon an enemy carries (layout as in BkSoldier.cpp; names inferred). */
class EnemyWeapon {
  public:
    EnemyWeapon(const boost::weak_ptr<CharInfo> &owner, int type);

    /* Fires once (name inferred). */
    void Fire();

    /* 0x00 */ u8 field_0x0[0x64];

    virtual ~EnemyWeapon();
    virtual void vf_0x0C(float dt);
};

/* The cursor a parameter's label callback receives (layout as in Helicopter.cpp). */
class UnkLabelQuery_800CBC20 {
  public:
    /* 0x0 */ const GadgetParamDesc *params;
    /* 0x4 */ s32 index;
    /* 0x8 */ s32 value;
};

/* The camera manager (see AdjustCosRot.cpp). */
class UnkCameraManager_80009548;

extern "C" {
/* The enemies' shared helpers (behaviour inferred). */
void fn_8019AA40(Enemy *enemy, u16 sound, int arg);
/* An effect at position (behaviour inferred). */
boost::weak_ptr<EffectElement> fn_801A1E8C(const Vec3f *position, const Vec3f *direction, int kind);
/* The weapon's calls (see BkSoldier.cpp). */
void fn_801AA830(EnemyWeapon *weapon);
void fn_801AA868(EnemyWeapon *weapon);
void fn_801AA8A0(EnemyWeapon *weapon, const Vec3f *offset);
void fn_801AA8E4(EnemyWeapon *weapon, float value);
void fn_801AB3F0(EnemyWeapon *weapon);
void fn_801AB590(EnemyWeapon *weapon, void *part);
void *fn_801A46E4(void *model, int part);
/* Resources and models (see BkSoldier.cpp). */
void *fn_80012684(void *loader, int slot, int kind);
void *fn_80012608(void *loader, int slot, int kind);
boost::shared_ptr<EnemyRender> fn_802CDB08(int slot, const EnemyMotionTable &table, void *model,
                                           void *motion);
void *fn_802CD420(EnemyRender *render);
void fn_80415808(void *motion, int id, void (*callback)(void *), void *data);
/* Camera shake and rumble (see ThrowBomb.cpp and BkSoldier.cpp). */
UnkCameraManager_80009548 *fn_80009548(void);
void fn_8000D8EC(UnkCameraManager_80009548 *manager, int arg1, int arg2, float strength);
boost::weak_ptr<Vibration::RequestHandle> fn_8034DE08(int pattern, const Vec3f *position,
                                                      int arg);
/* Model nodes, matrices and vectors (see BkSoldier.cpp). */
void fn_80016370(Mtx34f *matrix, void *node);
void fn_800163F4(void *node, const Mtx34f *matrix);
Vec3f fn_8040FB08(const Mtx34f *matrix);
void fn_8040FA20(Mtx34f *matrix, const Vec3f *translation);
Mtx34f fn_8040BEF8(const Mtx34f *matrix);
Mtx34f fn_8001E044(const Mtx34f *a, const Mtx34f *b);
void fn_8040BAC4(Mtx34f *matrix, const Mtx34f *other);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Vec2f fn_8040F760(const Vec3f *direction);
float fn_8040F314(float value, float target, float step);
Vec3f fn_8040CEE8(const Vec3f *v);
Vec3f fn_8040D0AC(const Vec3f *v, float angle);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
/* Sets a Vec2f to zero (behaviour inferred). */
void fn_8040E988(Vec2f *vector);
}

inline void EnemyWeapon::Fire() {
    field_0x0[0] = 0;
    fn_801AB3F0(this);
}

/* What the model's update callback receives (as in BkSoldier.cpp). */
struct UnkModelCallbackArg {
    /* 0x0 */ void *m_node;
    /* 0x4 */ GunRobot *m_owner;
};

/* A motion table (not original). Its copy constructor makes vfDisp_0x64's static tables
 * initialize at run time, with their constructor out of line, as retail does. */
struct MotionTable : public EnemyMotionTable {
    MotionTable(const EnemyMotionEntry *e, s32 c) {
        entries = e;
        count = c;
    }
    MotionTable(const MotionTable &other) {
        entries = other.entries;
        count = other.count;
    }
};

/* Three floats copied as a plain struct (as in BkSoldier.cpp). */
struct Vec3fWords {
    float x;
    float y;
    float z;
};

/* The values of a walker's point index (0 to 2), and the labels of its waiting parameters (names
 * inferred). Retail emits these after the unit's other functions, as template instances; the
 * declaration order sets their order. */
template <class T>
const char *GetWaitLabel(UnkLabelQuery_800CBC20 *query, T *actionTypes);
template <class T>
float GetMoveSpeedRatio(const T *params, int index);
template <class T>
int GetActionType(const T *params, int index);
template <class T>
float GetActionTime(const T *params, int index);

/* The motion table (steps in .rodata and .sdata2). */
static const MotionSequenceStep lbl_805F54A0[] = {{4, -1, -1, 30, 1}};
static const MotionSequenceStep lbl_804CA0DC[] = {{5, -1, -1, 30, 2}, {6, -1, -1, 10, 0}};
static const MotionSequenceStep lbl_805F54A8[] = {{1, -1, -1, 30, 1}};
static const MotionSequenceStep lbl_805F54B0[] = {{2, -1, -1, 30, 1}};
static const MotionSequenceStep lbl_804CA0EC[] = {
    {9, -1, -1, 10, 0}, {7, -1, -1, 0, 0}, {7, -1, -1, 0, 0}, {7, -1, -1, 0, 0}, {7, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_804CA114[] = {{10, -1, -1, 10, 0}, {8, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_805F54B8[] = {{11, -1, -1, 30, 1}};
static const MotionSequenceStep lbl_804CA124[] = {{3, -1, 14, 0, 0}, {3, 14, 15, 0, 2}, {3, 15, -1, 0, 0}};
static const MotionSequence lbl_804CA13C[] = {
    {lbl_805F54A0, 1, -3}, {lbl_804CA0DC, 2, -3}, {lbl_805F54A8, 1, -3}, {lbl_805F54B0, 1, -3},
    {lbl_804CA0EC, 5, -3}, {lbl_804CA114, 2, -3}, {lbl_805F54B8, 1, -3}, {lbl_804CA124, 3, -1},
};
static const MotionSequenceTable lbl_805F54C0 = {lbl_804CA13C, 8};

/* The weapon types the WeaponType parameter selects. */
static const s32 lbl_804CA17C[] = {3, 4, 0xD, 0x13, 0x14, 0x19};

/* The model motion tables of the two BodyTypes. */
static const EnemyMotionEntry lbl_804CA194[] = {{3, -1}, {4, 30}, {5, 10}};
static const EnemyMotionEntry lbl_804CA1A0[] = {{6, -1}, {7, 30}, {8, 10}};

const char *GunRobotTask::s_taskName = "GunRobot";

template <class T>
const char *GetWaitLabel(UnkLabelQuery_800CBC20 *query, T *actionTypes) {
    static const char *s_moveTypes[4] = {
        "WaitActMoveType : STAND",
        "WaitActMoveType : LINEAR",
        "WaitActMoveType : TRIANGLE",
        "WaitActMoveType : RANDOM",
    };
    static int s_moveType = 0;
    static char s_buffer[0x80];

    switch (query->index) {
    case 13:
        s_moveType = query->value;
        return s_moveTypes[query->value];
    case 14:
        if (s_moveType == 3) {
            return "Rand_ActionTime";
        }
        if (s_moveType == 1 || s_moveType == 2) {
            return query->params[query->index].label;
        }
        return "Stnd_ActionTime";
    case 15:
        if (s_moveType == 3) {
            sprintf(s_buffer, "Rand_%s", actionTypes[query->value]);
        } else if (s_moveType == 1 || s_moveType == 2) {
            sprintf(s_buffer, "Pos0_%s", actionTypes[query->value]);
        } else {
            sprintf(s_buffer, "Stnd_%s", actionTypes[query->value]);
        }
        return s_buffer;
    case 16:
        if (s_moveType == 3) {
            return "Rand_MoveSpeedRatio";
        }
        if (s_moveType == 1 || s_moveType == 2) {
            return "Pos0_MoveSpeedRatio";
        }
        break;
    case 17:
    case 18:
        if (s_moveType == 1 || s_moveType == 2) {
            return query->params[query->index].label;
        }
        break;
    case 19:
        if (s_moveType == 3) {
            return "RandomMoveRange";
        }
        if (s_moveType == 1 || s_moveType == 2) {
            return query->params[query->index].label;
        }
        break;
    case 20:
        if (s_moveType == 1 || s_moveType == 2) {
            sprintf(s_buffer, "Pos1_%s", actionTypes[query->value]);
            return s_buffer;
        }
        break;
    case 21:
        if (s_moveType == 1 || s_moveType == 2) {
            return query->params[query->index].label;
        }
        break;
    case 22:
    case 23:
    case 24:
    case 26:
        if (s_moveType == 2) {
            return query->params[query->index].label;
        }
        break;
    case 25:
        if (s_moveType == 2) {
            sprintf(s_buffer, "Pos2_%s", actionTypes[query->value]);
            return s_buffer;
        }
        break;
    }
    return "";
}

template <class T>
float GetActionTime(const T *params, int index) {
    if (index == 1) {
        return params->Pos1_ActionTime;
    }
    if (index == 2) {
        return params->Pos2_ActionTime;
    }
    return params->Pos0_ActionTime;
}

template <class T>
int GetActionType(const T *params, int index) {
    if (index == 1) {
        return params->Pos1_ActionType;
    }
    if (index == 2) {
        return params->Pos2_ActionType;
    }
    return params->Pos0_ActionType;
}

template <class T>
float GetMoveSpeedRatio(const T *params, int index) {
    if (index == 1) {
        return params->Pos1_MoveSpeedRatio;
    }
    if (index == 2) {
        return params->Pos2_MoveSpeedRatio;
    }
    return params->Pos0_MoveSpeedRatio;
}

/* EnemyCtrlFunc's default slot 0x1C (Game/Enemy.h): retail has its out-of-line copy here, where
 * the call in OnModelUpdate first needs it. It returns the vector at the start of the object,
 * copied through fn_800091EC, which Vec3f's inline copy does not do; hence this definition by
 * its linker name (not original). */
extern "C" Fn8040CopyVector3 vfCtrl_0x1C__13EnemyCtrlFuncFf(const EnemyCtrlFunc *ctrl) {
    return *reinterpret_cast<const Fn8040CopyVector3 *>(ctrl);
}

/* The model's update callback: turns the Robot's upper body toward its target, at most 30
 * degrees up or down and 10 to the side (behaviour inferred). */
void GunRobot::OnModelUpdate(void *data) {
    UnkModelCallbackArg *arg = static_cast<UnkModelCallbackArg *>(data);
    GunRobot *self = arg->m_owner;
    Mtx34f matrix;
    fn_80016370(&matrix, arg->m_node);
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)fn_8040FB08(&matrix));
    Vec2f angles(0.0f, 0.0f);
    Matrix34 rotation = MakeRotation(self->GetRotation(), 0);
    if (self->fn_8019531C()) {
        Vec3f target;
        fn_800091EC(&target,
                    &(const Vec3f &)self->m_ctrl->vfCtrl_0x1C(-1.0f));
        fn_8040FA20(&rotation, &position);
        Vec3f local;
        fn_800091EC(&local, &(const Vec3f &)fn_8040B67C(&(const Mtx34f &)fn_8040BEF8(&rotation),
                                                        &target));
        Vec3f direction;
        fn_800091EC(&direction, &(const Vec3f &)fn_8040CEE8(&local));
        angles = fn_8040F760(&direction);
        float degree = lbl_805FAD38 / 180.0f;
        if (angles.x > 20.0f * degree) {
            angles.x = 30.0f * degree;
        } else if (angles.x < -(30.0f * degree)) {
            angles.x = -(30.0f * degree);
        }
        if (angles.y > 10.0f * degree) {
            angles.y = 10.0f * degree;
        } else if (angles.y < -(10.0f * degree)) {
            angles.y = -(10.0f * degree);
        }
    }
    self->m_aim.x = fn_8040F314(self->m_aim.x, angles.x, 0.2f * self->GetFrameTime2());
    self->m_aim.y = fn_8040F314(self->m_aim.y, angles.y, 0.2f * self->GetFrameTime2());
    Mtx34f turn;
    fn_8040B6F8(&turn);
    fn_8040B848(&turn, self->m_aim.y + self->GetRotation()->y);
    fn_8040B884(&turn, self->m_aim.x);
    fn_8040BAC4(&turn, &(const Mtx34f &)fn_8040BEF8(&rotation));
    fn_8040C148(&matrix, &(const Mtx34f &)fn_8001E044(&turn, &matrix));
    fn_8040FA20(&matrix, &position);
    fn_800163F4(arg->m_node, &matrix);
}

GunRobot::GunRobot() : EnemyWalker(3), m_weapon(0), m_aim(0.0f, 0.0f) {
    m_enemy = this;
    vfPara_0x40(reinterpret_cast<int>(&lbl_805F54C0));
    field_0x2E0 = 0.0f;
    field_0x2E4 = 0;
    field_0x2E5 = 0;
    fn_8040E988(&m_aim);
}

GunRobot::~GunRobot() {
    if (m_weapon != 0) {
        delete m_weapon;
        m_weapon = 0;
    }
    if (!m_effect.expired()) {
        m_effect.lock()->Stop();
    }
}

int GunRobot::vfStat_0xB8() {
    if (m_params.BodyType == 0) {
        return 0x53;
    }
    if (m_params.BodyType == 1) {
        return 0x54;
    }
    return 0x27;
}

void GunRobot::vfSet_0x20(UnkPlacement_800C9CD0 *placement) {
    TEnemySetBase::vfSet_0x20(placement);
    fn_800C9DBC(placement, &m_params, sizeof(m_params));
    if (m_params.AppearType != 0) {
        GetPosition()->x += m_params.OffsetPos_X;
        GetPosition()->y += m_params.OffsetPos_Y;
        GetPosition()->z += m_params.OffsetPos_Z;
    }
    if (m_params.AppearType == 2) {
        field_0x2E0 = 1.0f;
    }
}

void GunRobot::vfSet_0x1C(UnkPlacement_800C9CD0 *placement) {
    int bodyType = m_params.BodyType;
    int weaponType = m_params.WeaponType;
    vfSet_0x20(placement);
    /* Copied as a plain struct (word loads and stores), unlike Vec3f's own copy. */
    Vec3fWords rotation = *reinterpret_cast<Vec3fWords *>(GetRotation());
    vfDisp_0x38(GetPosition(), reinterpret_cast<Vec3f *>(&rotation));
    m_ctrl->vfCtrl_0x20();
    if (bodyType != m_params.BodyType) {
        vfDisp_0x64();
    }
    if (weaponType != m_params.WeaponType) {
        if (m_weapon != 0) {
            delete m_weapon;
            m_weapon = 0;
        }
        m_weapon = new EnemyWeapon(fn_8017D014(this), lbl_804CA17C[m_params.WeaponType]);
    }
}

/* Point index's position: the home position, offset by Pos1 or Pos2, or (3) a random point within
 * Pos1_ActionTime (the random move range). The sums go through locals, as retail schedules them. */
Fn8040CopyVector3 GunRobot::vf_0x244(int index) {
    Fn8040CopyVector3 position(*reinterpret_cast<Fn8040CopyVector3 *>(&field_0x48));
    float x;
    float z;
    switch (index) {
    case 0:
        break;
    case 1:
        x = position.x + m_params.Pos1_X;
        z = position.z + m_params.Pos1_Z;
        position.z = z;
        position.x = x;
        break;
    case 2:
        x = position.x + m_params.Pos2_X;
        z = position.z + m_params.Pos2_Z;
        position.z = z;
        position.x = x;
        break;
    case 3: {
        float distance = fn_8040EC80(-m_params.Pos1_ActionTime, m_params.Pos1_ActionTime);
        fn_8040CFF8(reinterpret_cast<Vec3f *>(&position),
                    &(const Vec3f &)fn_8040D0AC(fn_80008E1C((Vec3f *)&(const Vec3f &)Vec3f(), distance,
                                                            0.0f, 0.0f),
                                                fn_8040EC80(0.0f, lbl_805FAD38)));
        break;
    }
    }
    return position;
}

void GunRobot::vfStat_0xA0() {
    EnemyStatusCommon::vfStat_0xA0();
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

void GunRobot::vfDisp_0x64() {
    static MotionTable s_tables[2] = {MotionTable(lbl_804CA194, 3), MotionTable(lbl_804CA1A0, 3)};
    void *loader = fn_800122D4();
    int slot = m_motionBase;
    void *model = fn_80012684(loader, slot, 10);
    void *motion = fn_80012608(loader, slot, 0xC);
    m_render = fn_802CDB08(slot, s_tables[m_params.BodyType], model, motion);
    void *data = fn_802CD420(m_render.get());
    if (data != 0) {
        fn_80415808(data, 0x19, OnModelUpdate, this);
    }
}

void GunRobot::vfDisp_0x08() {
    EnemyDispCommon::vfDisp_0x08();
    if (m_weapon != 0) {
        delete m_weapon;
        m_weapon = 0;
    }
    m_weapon = new EnemyWeapon(fn_8017D014(this), lbl_804CA17C[GetWeaponType()]);
}

void GunRobot::vfDisp_0x10(float dt) {
    EnemyWalkerCommon::vfDisp_0x10(dt);
    if (m_weapon != 0) {
        void *part = fn_801A46E4(m_enemy->m_disp->vfDisp_0x1C(), 0xD49);
        fn_801AB590(m_weapon, part);
        fn_801AA8E4(m_weapon, field_0xEC);
        m_weapon->vf_0x0C(dt);
    }
    if (!field_0x2E5) {
        if (!m_effect.expired()) {
            m_effect.lock()->Stop();
        }
    } else if (m_effect.expired()) {
        m_effect = fn_801A1E8C(GetPosition(), &lbl_805E07A0, 0x88);
        fn_8019AA40(this, 0x601E, 1);
    }
    field_0x2E5 = 0;
}

void GunRobot::vfDisp_0x4C() {
    EnemyDispCommon::vfDisp_0x4C();
}

void GunRobot::vfDisp_0x40(EnemyMotionEventArg &arg) {
    EnemyDispCommon::vfDisp_0x40(arg);
    if (m_weapon != 0 && arg.m_type == 0x5000 && arg.m_data->kind == 2) {
        m_weapon->Fire();
    }
}

/* EnemyMoveFunc's default (Game/Enemy.h): retail has its out-of-line copy here, where the call in
 * vfDisp_0x44 first needs it. */
bool EnemyMoveFunc::vfMove_0x18() {
    return 0;
}

bool GunRobot::vfDisp_0x44(int *command) {
    if (EnemyDispCommon::vfDisp_0x44(command)) {
        return true;
    }
    switch (*command) {
    case 14:
        fn_8000D8EC(fn_80009548(), 0, 0, 0.5f);
        fn_8034DE08(8, GetPosition(), -2);
        fn_8019AA40(this, 0x601C, 1);
        return true;
    case 13:
        fn_8019AA40(this, 0x601D, 1);
        return true;
    case 25:
        if (m_enemy->m_move->vfMove_0x18()) {
            field_0x2E5 = 1;
        }
        return true;
    }
    return false;
}

float GunRobot::vfPara_0x6C() {
    if (m_params.BodyType == 0) {
        return 8.0f;
    }
    return 16.0f;
}

int GunRobot::vfPara_0x48() {
    switch (m_params.WeaponType) {
    case 0:
    case 1:
    case 5:
        return 2;
    case 2:
    case 3:
    case 4:
        return 3;
    }
    return 2;
}

int GunRobot::vfPara_0x4C() {
    return 0;
}

int GunRobot::vfPara_0x50() {
    return 1;
}

int GunRobot::vfPara_0x54() {
    switch (m_params.WeaponType) {
    case 0:
    case 1:
    case 5:
        return 2;
    case 2:
    case 3:
    case 4:
        return 3;
    }
    return 2;
}

int GunRobot::vfPara_0x58() {
    switch (m_params.WeaponType) {
    case 0:
    case 1:
    case 5:
        return 4;
    case 2:
    case 3:
    case 4:
        return 5;
    }
    return 2;
}

int GunRobot::vfPara_0x5C() {
    return 7;
}

int GunRobot::vfPara_0x60() {
    return 7;
}

int GunRobot::vf_0x254() {
    return 0;
}

void GunRobot::vfStat_0x38() {
    if (m_params.AppearType != 0 || m_params.AppearType == 2) {
        void *key = m_ctrl->vfCtrl_0x10();
        if (!m_enemy->field_0xA8.Contains(key)) {
            if (m_weapon != 0) {
                fn_801AA830(m_weapon);
            }
            m_enemy->m_status->vfStat_0x30(false);
            m_enemy->m_status->vfStat_0x3C(true);
            return;
        }
        if ((field_0x2E0 -= GetFrameTime()) >= 0.0f) {
            if (!field_0x2E4) {
                field_0x2E4 = 1;
                vfDisp_0x50();
            }
            if (m_weapon != 0) {
                fn_801AA830(m_weapon);
            }
            m_enemy->m_status->vfStat_0x30(false);
            m_enemy->m_status->vfStat_0x3C(true);
            return;
        }
    }
    if (m_weapon != 0) {
        fn_801AA868(m_weapon);
    }
    m_status->vfStat_0x3C(false);
    m_status->vfStat_0x30(true);
}

/* The AppearType (called by GunRobotAI; name inferred). */
int GunRobot::GetAppearType() {
    return m_params.AppearType;
}

float GunRobot::vf_0x238(int index) {
    return GetMoveSpeedRatio(&m_params, index);
}

int GunRobot::vf_0x23C(int index) {
    return GetActionType(&m_params, index);
}

float GunRobot::vf_0x240(int index) {
    return GetActionTime(&m_params, index);
}

float GunRobot::vf_0x248() {
    return m_params.Pos1_ActionTime;
}

int GunRobot::vf_0x24C(int index) {
    switch (vf_0x23C(index)) {
    case 0:
        return vfPara_0x48();
    case 1:
        return vfPara_0x58();
    case 2:
        return vfPara_0x48();
    }
    return vfPara_0x48();
}

int GunRobot::vf_0x250() {
    return m_params.WaitActMoveType;
}

bool GunRobot::fn_8019531C() {
    if (vfStat_0x24() == vfPara_0x58() && field_0xA8.Contains(m_ctrl->vfCtrl_0x10())) {
        return true;
    }
    return false;
}

GunRobotTask::GunRobotTask(Task *parent, UnkPlacement_800C9CD0 *placement)
    : EnemyTemplate<GunRobot, GunRobotAI>(parent, placement) {
    m_enemy = this;
    m_name = s_taskName;
}

GunRobotTask::~GunRobotTask() {}

void GunRobotTask::LoadFiles() {
    void *loader = fn_800122D4();
    fn_8001299C(loader, 10, new ResourceOneFile("enemy/GunRobotData.one", false));
}

void GunRobotTask::DropFiles() {
    fn_800127F8(fn_800122D4(), 10);
}

void GunRobotTask::Instantiate(UnkPlacement_800C9CD0 *placement) {
    new GunRobotTask(fn_801EB304(fn_8001C4F8(), 0xE), placement);
}

extern "C" const char *fn_80194E44(UnkLabelQuery_800CBC20 *query) {
    static const char *s_actionTypes[3] = {
        "ActionType : NONE", "ActionType : ATTACK", "ActionType : HIDE"};
    static const char *s_bodyTypes[2] = {"BodyType : TYPE_A", "BodyType : TYPE_B"};
    static const char *s_appearTypes[4] = {
        "AppearType : WAIT_ACT", "AppearType : OFFSET", "AppearType : WARP", "AppearType : XXX"};
    static const char *s_weaponTypes[6] = {
        "Weapon : AUTORIFLE", "Weapon : AIRCRAFTRIFLE", "Weapon : BAZOOKA",
        "Weapon : ROCKET4",   "Weapon : ROCKET8",       "Weapon : LASERRIFLE",
    };
    static char s_buffer[0x80];
    static int s_appearType = 0;

    strcpy(s_buffer, GetWaitLabel(query, s_actionTypes));
    switch (query->index) {
    case 7:
        s_appearType = query->value;
        return s_appearTypes[query->value];
    case 8:
        return s_weaponTypes[query->value];
    case 9:
        return s_bodyTypes[query->value];
    case 10:
    case 11:
    case 12:
        if (s_appearType != 0) {
            return query->params[query->index].label;
        }
        break;
    }
    return s_buffer;
}

static GadgetParamDesc lbl_80536F44[] = {
    {4, "MoveRange", 0, 0, 0, 200.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchRange", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchAngle", 0, 0, 0, 0.0f, -360.0f, 360.0f, fn_800CBC20, 0},
    {4, "SearchWidth", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchHeight", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchHeightOffset", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "MoveSpeedRatio", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {1, "AppearType", 0, 0, 3, 0.0f, 0.0f, 0.0f, fn_80194E44, 0},
    {1, "WeaponType", 0, 0, 5, 0.0f, 0.0f, 0.0f, fn_80194E44, 0},
    {1, "BodyType", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_80194E44, 0},
    {4, "OffsetPos_X", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_80194E44, 0},
    {4, "OffsetPos_Y", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_80194E44, 0},
    {4, "OffsetPos_Z", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_80194E44, 0},
    {1, "WaitActMoveType", 0, 0, 3, 0.0f, 0.0f, 0.0f, fn_80194E44, 0},
    {4, "Pos0_ActionTime", 0, 0, 0, 3.0f, 0.0f, 65536.0f, fn_80194E44, 0},
    {1, "Pos0_ActionType", 0, 0, 2, 0.0f, 0.0f, 0.0f, fn_80194E44, 0},
    {4, "Pos0_MoveSpeedRatio", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_80194E44, 0},
    {4, "Pos1_X", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_80194E44, 0},
    {4, "Pos1_Z", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_80194E44, 0},
    {4, "Pos1_ActionTime", 0, 0, 0, 3.0f, 0.0f, 65536.0f, fn_80194E44, 0},
    {1, "Pos1_ActionType", 0, 0, 2, 0.0f, 0.0f, 0.0f, fn_80194E44, 0},
    {4, "Pos1_MoveSpeedRatio", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_80194E44, 0},
    {4, "Pos2_X", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_80194E44, 0},
    {4, "Pos2_Z", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_80194E44, 0},
    {4, "Pos2_ActionTime", 0, 0, 0, 3.0f, 0.0f, 65536.0f, fn_80194E44, 0},
    {1, "Pos2_ActionType", 0, 0, 2, 0.0f, 0.0f, 0.0f, fn_80194E44, 0},
    {4, "Pos2_MoveSpeedRatio", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_80194E44, 0},
    {0},
};

GadgetClassDesc lbl_805373A4 = {
    "GUN_ROBOT", GunRobotTask::LoadFiles, GunRobotTask::DropFiles,
    reinterpret_cast<void (*)(UnkPlacement_800C9CD0 *)>(GunRobotTask::Instantiate), 0, 0, 0x68, 2,
    0, 5, lbl_80536F44,
};

/* Always false for the Robot (meaning unknown); retail has it ahead of the label callback. */
bool GunRobot::vfStat_0xC8() {
    return 0;
}

/* Not called anywhere: the linker strips it. Retail pools 0 and 0.5 ahead of the other float
 * constants in .sdata2; this function, emitted first, makes the compiler pool them there (see
 * docs/research-notes.md; the name is a placeholder). */
static float UnkConstantUser(float x) {
    x *= 0.0f;
    x *= 0.5f;
    return x;
}

/* Not called anywhere: the linker strips it. Retail's .data, which the linker keeps whole, has the
 * zero-filled copy of sp_counted_base's vtable after EnemyTemplate's; this function, defined
 * last, makes the compiler place it there (as in BkSoldier.cpp; the name is a placeholder). */
static void UnkCountedBaseUser(boost::detail::sp_counted_base *base) {
    base->boost::detail::sp_counted_base::~sp_counted_base();
}
