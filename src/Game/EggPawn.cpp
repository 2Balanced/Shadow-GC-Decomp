/* EggPawn (class name from RTTI; its s_taskName is "EggPawn"): the Egg Pawn, EGG_PAWN in the stage
 * editor (enemy/EggPawnData.one). Its body EggPawnBase is an EnemyWalker (Game/EnemyWalker.h); it
 * may carry a shield and an auto rifle, a bazooka or a lance, and walks between up to three points
 * or at random (WaitActMoveType), acting at each point (ActionType). The unit also holds the first
 * copies of the enemy parts' small virtuals (EnemyStatusCommon, EnemyParamCommon, ...), defined
 * last here. File name inferred from the class. Names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). The unit's code reaches its .data and .rodata through pooled base addresses, so the
 * linker keeps those sections whole: Game/CharaColliBaseCharInfo.h and Game/TNumEdit.h supply the
 * RTTI base lists the .data starts with. */
/* See EnemyReferer (Game/Enemy.h) and EnemyBaseAI (Game/EnemyAI.h). */
#define ENEMY_REFERER_DTOR
#define ENEMY_BASE_AI_STATES

#include <stdio.h>
#include <string.h>

/* The ten zero words .rodata starts with (meaning and type unknown; the bytes are reproduced, as
 * in BkSoldier.cpp). */
static const float s_zeros[10] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
/* The order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/TNumEdit.h"
#include "Game/EggPawn.h"
#include "Game/EnemyRender.h"
#include "Game/Gadget.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathTypes.h"
#include "Game/ResourceOneFile.h"
#include "Game/Vibration.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

/* MSL's std::fabs for floats, which the game calls out of line (as in BkSoldier.cpp). */
namespace std {
float fabs(float x);
}

extern float lbl_805FAD38; /* pi */

using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;

/* The cursor a parameter's label callback receives (layout as in Helicopter.cpp). */
class UnkLabelQuery_800CBC20 {
  public:
    /* 0x0 */ const GadgetParamDesc *params;
    /* 0x4 */ s32 index; /* the parameter being shown */
    /* 0x8 */ s32 value; /* its current value */
};

/* EnemyShield (class name from the RTTI of its shared_ptr control block; code around
 * 0x801A7478): a shield (names inferred; as declared in BkSoldier.cpp). */
class EnemyShield {
  public:
    void fn_801A7478();
    void fn_801A74C4();
    bool fn_801A74E8();
    void fn_801A7508(AttackCallbackParam &param);
};

/* EnemyWeapon (class name from RTTI; vtable 0x8053B218 at 0x64; constructor 0x801AB904): the
 * weapon an enemy carries (names inferred; as declared in BkSoldier.cpp). */
class EnemyWeapon {
  public:
    EnemyWeapon(const boost::weak_ptr<CharInfo> &owner, int type);

    /* 0x00 */ u8 field_0x0[0x64];

    virtual ~EnemyWeapon();
    virtual void vf_0x0C(float dt);
};

/* boost/weak_ptr.hpp only declares weak_ptr::reset, which the game has out of line; this unit
 * emits the copy for the shield (boost 1.32's definition; as in Game/BaseDoor.cpp). */
namespace boost {
template <> inline void weak_ptr<EnemyShield>::reset() {
    this_type().swap(*this);
}
} // namespace boost

extern "C" {
/* The enemies' shared helpers (behaviour inferred). */
void fn_8019AA40(Enemy *enemy, u16 sound, int arg);
void fn_8019A9DC(Enemy *enemy, u16 sound, int arg);
float fn_8019B184(Enemy *enemy, const Vec3f *direction);
/* A random point at most range from the enemy's home (behaviour inferred). */
void fn_8019B000(const Enemy *enemy, Vec3f *point, float range);
boost::weak_ptr<EnemyShield> fn_801A752C(Enemy *enemy, const EnemyMotionTable &table, int arg);
/* Whether the game runs at its hard difficulty (behaviour inferred). */
bool fn_801A2030(void);
/* The weapon's state calls. */
void fn_801AA830(EnemyWeapon *weapon);
void fn_801AA868(EnemyWeapon *weapon);
void fn_801AA8A0(EnemyWeapon *weapon, const Vec3f *offset);
void fn_801AA8E4(EnemyWeapon *weapon, float value);
void fn_801AB650(EnemyWeapon *weapon, void *part);
void fn_801AB3F0(EnemyWeapon *weapon);
void *fn_801A46E4(void *model, int part);
/* Rumble. */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DE08(int pattern, const Vec3f *position,
                                                      int arg);
/* Resources and models. */
void *fn_80012684(void *loader, int slot, int kind);
void *fn_80012608(void *loader, int slot, int kind);
boost::shared_ptr<EnemyRender> fn_802CDB08(int slot, const EnemyMotionTable &table, void *model,
                                           void *motion);
/* v turned about y by angle, by value. */
Vec3f fn_8040D0AC(const Vec3f *v, float angle);
/* The parts of EnemyStatusCommon's status object (behaviour inferred). */
void fn_801A9784(UnkEnemyStatus_801AA430 *status, int kind);
void fn_801A96C4(UnkEnemyStatus_801AA430 *status, u32 value);
void fn_801A95F8(UnkEnemyStatus_801AA430 *status, bool arg0, bool arg1);
void fn_801A9540(UnkEnemyStatus_801AA430 *status, int arg);
void fn_801A9508(UnkEnemyStatus_801AA430 *status, int arg);
}

/* Small functions the header declares; they are inline (weak copies, first emitted here). */
inline float Enemy::GetFrameTime() {
    return (*reinterpret_cast<float **>(m_chara->field_0x60))[0];
}

inline Vec3f *Enemy::GetRotation() {
    return &m_chara->m_rotation;
}

inline Vec3f *Enemy::GetPosition() {
    return &m_chara->m_position;
}

/* The enemy's character info (Game/Enemy.h; inline, first emitted here). */
extern "C" inline boost::weak_ptr<UnkEnemyInfo_8017D014> fn_8017D014(Enemy *enemy) {
    return enemy->m_charaRef;
}

inline void *EnemyCtrlFunc::vfCtrl_0x10() {}

inline int EnemySetFunc::vfSet_0x2C() {
    return 0;
}

inline void EnemyCtrlFunc::vfCtrl_0x20() {}

inline int EnemyMoveFunc::vfMove_0x2C() {
    return 0;
}

inline void *EnemyMoveFunc::vfMove_0x30() {
    return 0;
}

inline void *EnemyDispFunc::vfDisp_0x1C() {
    return 0;
}

/* A byte the game keeps as a bool (not original: the headers declare these fields u8). */
inline bool AsBool(const u8 &value) {
    return *reinterpret_cast<const bool *>(&value);
}

/* ------------------------------------------------------------------------------------------- */
/* The first copies of the parts' small virtuals (Game/Enemy.h, Game/EnemyAI.h). */

inline int EnemyParamCommon::vfPara_0x44() {
    return field_0x8;
}

inline void EnemyParamCommon::vfPara_0x40(int value) {
    field_0x8 = value;
}

inline float EnemyStatusCommon::vfStat_0xCC() {
    return 0.5f * field_0x78;
}

inline int EnemyStatusCommon::vfStat_0xC4() {
    return 0;
}

inline void EnemyStatusCommon::vfStat_0xB4(float dt) {
    if ((field_0x7C += dt) > vfStat_0xB0()) {
        field_0x7C = vfStat_0xB0();
    }
}

inline float EnemyStatusCommon::vfStat_0xB0() {
    return field_0x78;
}

inline float EnemyStatusCommon::vfStat_0xAC() {
    return field_0x7C;
}

inline EnemyFlags *EnemyStatusCommon::vfStat_0xA8() {
    return &m_flags;
}

inline void EnemyStatusCommon::vfStat_0x8C(bool value) {
    if (value) {
        fn_801A9540(&field_0x8, -1);
    } else {
        fn_801A9508(&field_0x8, -1);
    }
}

inline void EnemyStatusCommon::vfStat_0x7C(bool arg0, bool arg1) {
    fn_801A95F8(&field_0x8, arg0, arg1);
}

inline void EnemyStatusCommon::vfStat_0x88(u32 value) {
    fn_801A96C4(&field_0x8, value);
}

inline void EnemyStatusCommon::vfStat_0x84(int kind) {
    fn_801A9784(&field_0x8, kind);
}

inline void EnemyStatusCommon::vfStat_0x80(bool value) {
    field_0x77 = value;
}

inline int EnemyStatusCommon::vfStat_0x74() {
    return field_0x84;
}

inline bool EnemyStatusCommon::vfStat_0x6C() {
    return AsBool(field_0x90.m_previous);
}

inline bool EnemyStatusCommon::vfStat_0x68() {
    return AsBool(field_0x8E.m_previous);
}

inline bool EnemyStatusCommon::vfStat_0x64() {
    return AsBool(field_0x8C.m_previous);
}

inline bool EnemyStatusCommon::vfStat_0x60() {
    return AsBool(field_0x8A.m_previous);
}

inline bool EnemyStatusCommon::vfStat_0x5C() {
    return AsBool(field_0x88.m_previous);
}

inline bool EnemyStatusCommon::vfStat_0x58() {
    return AsBool(field_0x90.m_current);
}

inline bool EnemyStatusCommon::vfStat_0x54() {
    return AsBool(field_0x8E.m_current);
}

inline bool EnemyStatusCommon::vfStat_0x50() {
    return AsBool(field_0x8C.m_current);
}

inline bool EnemyStatusCommon::vfStat_0x4C() {
    return AsBool(field_0x8A.m_current);
}

inline bool EnemyStatusCommon::vfStat_0x48() {
    return AsBool(field_0x88.m_current);
}

inline bool EnemyStatusCommon::vfStat_0x40() {
    return AsBool(field_0x6D);
}

inline bool EnemyStatusCommon::vfStat_0x34() {
    return AsBool(field_0x6C);
}

inline void EnemyStatusCommon::vfStat_0x3C(bool value) {
    field_0x6D = value;
}

inline void EnemyStatusCommon::vfStat_0x30(bool value) {
    field_0x6C = value;
}

inline bool EnemyStatusCommon::vfStat_0x28() {
    return AsBool(m_motion.m_ended);
}

inline bool EnemyStatusCommon::vfStat_0x2C() {
    return m_motion.m_finished;
}

inline int EnemyStatusCommon::vfStat_0x24() {
    return m_motion.m_index;
}

/* NonMatching: retail tests restart before clearing field_0x4 and reuses restart's register for
 * the zero (3 instructions differ; no source order tried reproduces it). */
inline int EnemyStatusCommon::vfStat_0x20(int motion, bool restart) {
    field_0x70 = motion;
    if (motion < 0) {
        m_motion.Reset();
    } else {
        m_motion.field_0x4 = 0;
        if (restart) {
            m_motion.Start(motion);
        } else if (motion != m_motion.m_index) {
            m_motion.Start(motion);
        } else {
            return 1;
        }
    }
}

inline void EnemyDispCommon::vfDisp_0x58(bool value) {
    field_0x1C[0x22 - 0x1C] = value;
}

inline boost::shared_ptr<EnemyRender> EnemyDispCommon::vfDisp_0x20() {
    return m_render;
}

inline int EnemyDispCommon::vfDisp_0x14() {
    return 0;
}

inline float EnemyMoveCommon::vfMove_0x24() {
    return 0.0f;
}

inline void *EnemyMoveCommon::vfMove_0x30() {
    return &field_0x8[0x40 - 0x8];
}

inline int EnemyMoveCommon::vfMove_0x2C() {
    return *reinterpret_cast<s32 *>(&field_0x8[0x4C - 0x8]);
}

inline float EnemyMoveCommon::vfMove_0x28() {
    return *reinterpret_cast<float *>(&field_0x8[0x3C - 0x8]);
}

inline bool EnemyMoveCommon::vfMove_0x14() {
    return AsBool(field_0x8[0x60 - 0x8]);
}

inline bool EnemyMoveCommon::vfMove_0x10() {
    return AsBool(field_0x8[0x5F - 0x8]);
}

inline bool TEnemySetBase::vfSet_0x28() {
    return *reinterpret_cast<s32 *>(&field_0x8[0x8 - 0x8]) >= 3;
}

inline void TEnemySetBase::vfSet_0x10() {
    *reinterpret_cast<bool *>(&field_0x8[0x14 - 0x8]) = true;
}

inline bool TEnemySetBase::vfSet_0x0C() {
    return AsBool(field_0x8[0x15 - 0x8]);
}

inline bool TEnemySetBase::vfSet_0x08() {
    return AsBool(field_0x8[0x14 - 0x8]);
}

inline void EnemyBaseAI::vfCtrl_0x20() {
    vfAI_0x38();
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
    u32 x;
    u32 y;
    u32 z;
};


/* The motion sequences (steps in .sdata2 and .rodata). */
static const MotionSequenceStep lbl_805F4ED8[] = {{8, -1, -1, 30, 1}};
static const MotionSequenceStep lbl_805F4EE0[] = {{9, -1, -1, 30, 1}};
static const MotionSequenceStep lbl_805F4EE8[] = {{11, -1, -1, 30, 1}};
static const MotionSequenceStep lbl_805F4EF0[] = {{10, -1, -1, 30, 1}};
static const MotionSequenceStep lbl_805F4EF8[] = {{23, -1, -1, 10, 1}};
static const MotionSequenceStep lbl_805F4F00[] = {{14, -1, -1, 30, 1}};
static const MotionSequenceStep lbl_805F4F08[] = {{15, -1, -1, 30, 1}};
static const MotionSequenceStep lbl_805F4F10[] = {{17, -1, -1, 30, 1}};
static const MotionSequenceStep lbl_805F4F18[] = {{16, -1, -1, 30, 1}};
static const MotionSequenceStep lbl_805F4F20[] = {{18, -1, -1, 10, 1}};
static const MotionSequenceStep lbl_805F4F28[] = {{19, -1, -1, 10, 1}};
static const MotionSequenceStep lbl_805F4F30[] = {{24, -1, -1, 10, 0}};
static const MotionSequenceStep lbl_805F4F38[] = {{14, -1, -1, 30, 1}};
static const MotionSequenceStep lbl_804C616C[] = {{3, -1, -1, 30, 2}, {13, -1, -1, 10, 0}};
static const MotionSequenceStep lbl_804C617C[] = {{4, 60, 60, 10, 0}, {4, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_804C618C[] = {{5, 60, 60, 10, 0}, {5, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_804C619C[] = {{21, -1, -1, 10, 0}, {21, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_804C61AC[] = {
    {1, -1, 14, 0, 0}, {1, 14, 15, 0, 2}, {1, 15, -1, 0, 0}, {6, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_804C61CC[] = {
    {2, -1, 14, 0, 0}, {2, 14, 15, 0, 2}, {2, 15, -1, 0, 0}, {7, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_804C61EC[] = {{1, -1, 14, 0, 0}, {1, 14, 15, 0, 2}, {1, 15, -1, 0, 0}};
static const MotionSequenceStep lbl_804C6204[] = {{2, -1, 14, 0, 0}, {2, 14, 15, 0, 2}, {2, 15, -1, 0, 0}};
static const MotionSequence lbl_804C621C[] = {
    {lbl_805F4ED8, 1, -3}, {lbl_805F4EE0, 1, -3}, {lbl_805F4EE8, 1, -3}, {lbl_805F4EF0, 1, -3},
    {lbl_805F4EF8, 1, -3}, {lbl_804C616C, 2, -3}, {lbl_805F4F00, 1, -3}, {lbl_805F4F08, 1, -3},
    {lbl_805F4F10, 1, -3}, {lbl_805F4F18, 1, -3}, {lbl_804C617C, 2, -3}, {lbl_804C618C, 2, -3},
    {lbl_805F4F20, 1, -3}, {lbl_805F4F28, 1, -3}, {lbl_805F4F30, 1, -3}, {lbl_804C619C, 2, -3},
    {lbl_805F4F38, 1, -3}, {lbl_804C61AC, 4, -3}, {lbl_804C61CC, 4, -3}, {lbl_804C61EC, 3, -3},
    {lbl_804C6204, 3, -3},
};

/* The weapon types the WeaponType parameter selects. */
static const s32 lbl_804C62C4[] = {0, 7, 0x11, 0x21};

static const MotionSequenceTable lbl_805F4F40 = {lbl_804C621C, 21};

/* The model and shield motion tables. */
static const EnemyMotionEntry lbl_805F4F48[] = {{4, -1}};
static const EnemyMotionEntry lbl_805F4F4C[] = {{3, -1}};

EggPawnBase::EggPawnBase() : EnemyWalker(0xC), m_weapon(0) {
    m_enemy = this;
    vfPara_0x40(reinterpret_cast<int>(&lbl_805F4F40));
    field_0x270 = 0;
    field_0x2EC = 0.0f;
    field_0x2F0 = 0;
    field_0x2F1 = 0;
}

EggPawnBase::~EggPawnBase() {
    if (!m_shield.expired()) {
        m_shield.lock()->fn_801A7478();
        m_shield.reset();
    }
    if (m_weapon != 0) {
        delete m_weapon;
        m_weapon = 0;
    }
}

int EggPawnBase::vfStat_0xB8() {
    if (fn_801A2030()) {
        int result = 0x69;
        if (m_params.HaveShield != 0) {
            result = 0x6A;
        }
        return result;
    }
    int result = 0x63;
    if (m_params.HaveShield != 0) {
        result = 0x64;
    }
    return result;
}

void EggPawnBase::vfSet_0x20(UnkPlacement_800C9CD0 *placement) {
    TEnemySetBase::vfSet_0x20(placement);
    fn_800C9DBC(placement, &m_params, sizeof(m_params));
    if (m_params.AppearType != 0) {
        GetPosition()->x += m_params.OffsetPos_X;
        GetPosition()->y += m_params.OffsetPos_Y;
        GetPosition()->z += m_params.OffsetPos_Z;
    }
    if (m_params.HaveShield != 0) {
        field_0x270 = 1;
    }
    if (m_params.AppearType == 2) {
        field_0x2EC = 1.0f;
    }
}

void EggPawnBase::vfDisp_0x64() {
    void *loader = fn_800122D4();
    int slot = m_motionBase;
    void *model = fn_80012684(loader, slot, 5);
    void *motion = fn_80012608(loader, slot, 7);
    m_render = fn_802CDB08(slot, MotionTable(lbl_805F4F48, 1), model, motion);
}

boost::weak_ptr<EnemyShield> EggPawnBase::CreateShield() {
    boost::weak_ptr<EnemyShield> shield = fn_801A752C(this, MotionTable(lbl_805F4F4C, 1), 0xD4A);
    return shield;
}

void EggPawnBase::vfStat_0xA0() {
    EnemyStatusCommon::vfStat_0xA0();
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

void EggPawnBase::vfDisp_0x08() {
    EnemyDispCommon::vfDisp_0x08();
    if (m_shield.expired() && field_0x270) {
        m_shield = CreateShield();
    }
    if (m_weapon == 0 && m_params.WeaponType != 0) {
        m_weapon = new EnemyWeapon(fn_8017D014(this), lbl_804C62C4[m_params.GetWeaponType()]);
    }
    m_ctrl->vfCtrl_0x20();
}

/* Whether the walker is on the ground (behaviour inferred from fn_803444B0). */
inline u8 IsOnGround(const EnemyWalkerCommon *walker) {
    CollisionEffect *effect = walker->m_collision.get();
    if (effect != 0) {
        return fn_803444B0(effect);
    }
    return 0;
}

/* Plays the effect of the ground the walker stands on (behaviour inferred). */
inline void PlayGroundEffect(EnemyWalkerCommon *walker) {
    CollisionEffect *effect = walker->m_collision.get();
    if (effect != 0) {
        fn_803444E0(effect, walker->GetPosition(),
                    static_cast<const Vec3f *>(walker->m_enemy->m_move->vfMove_0x30()), 0,
                    walker->m_enemy->m_move->vfMove_0x2C());
    }
}

void EggPawnBase::vfDisp_0x10(float dt) {
    EnemyWalkerCommon::vfDisp_0x10(dt);
    if (m_weapon != 0) {
        fn_801AB650(m_weapon, fn_801A46E4(m_enemy->m_disp->vfDisp_0x1C(), 0xD49));
        fn_801AA8E4(m_weapon, field_0xEC);
        m_weapon->vf_0x0C(dt);
    }
    if (field_0x2F1) {
        if (!IsOnGround(this)) {
            fn_8019AA40(this, 0x601A, 1);
        }
        PlayGroundEffect(this);
    }
    field_0x2F1 = 0;
}

/* Plays the landing effect of the ground the enemy stands on (behaviour inferred; also used by
 * BkSoldier). */
extern "C" void fn_8017CA2C(EnemyWalkerCommon *walker) {
    CollisionEffect *effect = walker->m_collision.get();
    if (effect != 0) {
        fn_80344504(effect, walker->GetPosition(),
                    static_cast<const Vec3f *>(walker->m_enemy->m_move->vfMove_0x30()), 0,
                    walker->m_enemy->m_move->vfMove_0x2C());
    }
}

bool EggPawnBase::vfDisp_0x44(int *command) {
    if (EnemyDispCommon::vfDisp_0x44(command)) {
        return true;
    }
    switch (*command) {
    case 12:
        fn_8034DE08(10, GetPosition(), -2);
        fn_8019AA40(this, 0x6017, 1);
        return true;
    case 11:
        fn_8019A9DC(this, 0x6018, 1);
        fn_8017CA2C(this);
        return true;
    case 0x22:
        field_0x2F1 = 1;
        break;
    case 0x23:
        fn_8019AA40(this, 0x601B, 1);
        break;
    }
    return false;
}

void EggPawnBase::vfDisp_0x40(EnemyMotionEventArg &arg) {
    EnemyDispCommon::vfDisp_0x40(arg);
    EnemyWeapon *weapon = m_weapon;
    if (weapon != 0 && arg.m_type == 0x5000 && arg.m_data->kind == 2) {
        weapon->field_0x0[0] = 0;
        fn_801AB3F0(weapon);
    }
}

int EggPawnBase::GetAppearType() {
    return m_params.AppearType;
}

int EggPawnBase::GetWeaponType() {
    return m_params.WeaponType;
}

bool EggPawnBase::HasShield() {
    return AsBool(field_0x270);
}

int EggPawnBase::vfPara_0x48() {
    if (m_params.WeaponType == 1 || m_params.WeaponType == 2) {
        if (field_0x270) {
            return 9;
        }
        return 7;
    }
    return field_0x270 ? 8 : 6;
}

int EggPawnBase::vfPara_0x4C() {
    if (m_params.WeaponType == 1 || m_params.WeaponType == 2) {
        if (field_0x270) {
            return 3;
        }
        return 1;
    }
    return field_0x270 ? 2 : 0;
}

int EggPawnBase::vfPara_0x50() {
    return 5;
}

int EggPawnBase::vfPara_0x54() {
    return vfPara_0x48();
}

int EggPawnBase::vfPara_0x58() {
    if (m_params.WeaponType == 1 || m_params.WeaponType == 2) {
        if (field_0x270) {
            return 11;
        }
        return 10;
    }
    if (m_params.WeaponType == 3) {
        return field_0x270 ? 13 : 12;
    }
    return 6;
}

/* vfPara_0x5C and vfPara_0x60 load m_status ahead of saving the link register, which GC/2.7
 * does only for a const this: EnemyParaFunc's getters are most likely const member functions in
 * the original (a change to Game/Enemy.h and every enemy's overrides). */
int EggPawnBase::vfPara_0x5C() {
    return m_status->vfStat_0x74() == 0 ? 17 : 18;
}

int EggPawnBase::vfPara_0x60() {
    return m_status->vfStat_0x74() == 0 ? 19 : 20;
}

int EggPawnBase::vfPara_0x64() {
    return 15;
}

int EggPawnBase::vf_0x254() {
    return 4;
}

float EggPawnBase::vfPara_0x20() {
    if (vfStat_0x24() == vf_0x254()) {
        return 2.0f * EnemyParamCommon::vfPara_0x20();
    }
    return EnemyParamCommon::vfPara_0x20();
}

/* NonMatching only through its neighbours: retail passes the rotation to vfDisp_0x38 by value (a
 * plain vector the call copies to the stack, as here) and references GetPosition before
 * GetRotation, so their weak copies come out GetRotation first. With Game/Enemy.h's pointer
 * parameter, this copy matches the function but swaps those two copies. */
void EggPawnBase::vfSet_0x1C(UnkPlacement_800C9CD0 *placement) {
    vfSet_0x20(placement);
    /* Copied as a plain struct (word loads and stores), unlike Vec3f's own copy. */
    Vec3fWords rotation = *reinterpret_cast<Vec3fWords *>(GetRotation());
    vfDisp_0x38(GetPosition(), reinterpret_cast<Vec3f *>(&rotation));
    m_ctrl->vfCtrl_0x20();
}

float EggPawnBase::vf_0x238(int index) {
    return m_params.GetMoveSpeedRatio(index);
}

int EggPawnBase::vf_0x23C(int index) {
    return m_params.GetActionType(index);
}

float EggPawnBase::vf_0x240(int index) {
    return m_params.GetActionTime(index);
}

Fn8040CopyVector3 EggPawnBase::vf_0x244(int index) {
    /* The sums go through locals, as retail schedules them (as in GunRobot::vf_0x244). */
    Fn8040CopyVector3 point;
    float x;
    float z;
    switch (index) {
    case 0:
        point = *reinterpret_cast<const Fn8040CopyVector3 *>(&field_0x48);
        break;
    case 1:
        point = *reinterpret_cast<const Fn8040CopyVector3 *>(&field_0x48);
        x = point.x + m_params.Pos1_X;
        z = point.z + m_params.Pos1_Z;
        point.z = z;
        point.x = x;
        break;
    case 2:
        point = *reinterpret_cast<const Fn8040CopyVector3 *>(&field_0x48);
        x = point.x + m_params.Pos2_X;
        z = point.z + m_params.Pos2_Z;
        point.z = z;
        point.x = x;
        break;
    case 3:
        fn_8019B000(this, reinterpret_cast<Vec3f *>(&point), vf_0x248());
        break;
    }
    return point;
}

float EggPawnBase::vf_0x248() {
    return m_params.Pos1_ActionTime;
}

int EggPawnBase::vf_0x24C(int index) {
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

int EggPawnBase::vf_0x250() {
    return m_params.WaitActMoveType;
}

void EggPawnBase::vfStat_0x38() {
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
        if ((field_0x2EC -= GetFrameTime()) >= 0.0f) {
            if (!field_0x2F0) {
                field_0x2F0 = 1;
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

void EggPawnBase::vfStat_0x70(AttackCallbackParam &param) {
    boost::shared_ptr<CharaColliAttack> attacker(param.m_attacker);
    if (!m_shield.expired() && attacker.get() != 0) {
        if (attacker->m_flags & 0x40) {
            if (std::fabs(fn_8019B184(m_enemy,
                                      reinterpret_cast<Vec3f *>(&param.field_0x8[0x34 - 0x8]))) <
                0.5f * lbl_805FAD38) {
                param.m_result = 5;
                vfStat_0xA8()->Set(5);
                fn_8019AA40(this, 0x6074, 1);
                return;
            }
        }
        if (!(attacker->m_flags & 4)) {
            m_shield.lock()->fn_801A7508(param);
            param.m_result = 1;
            if (m_shield.lock()->fn_801A74E8()) {
                fn_8019AA40(this, 0x6075, 1);
                field_0x75 = 1;
                m_shield.reset();
                field_0x270 = 0;
            } else {
                fn_8019AA40(this, 0x6074, 1);
                vfStat_0xA8()->Set(5);
            }
        }
    }
    EnemyStatusCommon::vfStat_0x70(param);
}

EggPawn::EggPawn(Task *parent, UnkPlacement_800C9CD0 *placement)
    : EnemyTemplate<EggPawnBase, EggPawnAI>(parent, placement) {
    m_enemy = this;
    m_name = s_taskName;
}

EggPawn::~EggPawn() {}

void EggPawn::LoadFiles() {
    void *loader = fn_800122D4();
    fn_8001299C(loader, 0x13, new ResourceOneFile("enemy/EggPawnData.one", false));
}

void EggPawn::DropFiles() {
    fn_800127F8(fn_800122D4(), 0x13);
}

void EggPawn::Instantiate(UnkPlacement_800C9CD0 *placement) {
    new EggPawn(fn_801EB304(fn_8001C4F8(), 0xE), placement);
}

/* The labels of the parameters from WaitActMoveType on, for the walking enemies (their units
 * each have a copy; behaviour inferred). */
template <class Walker>
const char *WalkerParamLabel(UnkLabelQuery_800CBC20 *query, const char **actionTypes) {
    static const char *s_moveTypes[4] = {
        "WaitActMoveType : STAND",
        "WaitActMoveType : LINEAR",
        "WaitActMoveType : TRIANGLE",
        "WaitActMoveType : RANDOM",
    };
    static char s_buffer[0x80];
    static int s_moveType = 0;

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

extern "C" const char *fn_8017B8F4(UnkLabelQuery_800CBC20 *query) {
    static const char *s_appearTypes[4] = {
        "AppearType : WAIT_ACT",
        "AppearType : OFFSET",
        "AppearType : WARP",
        "AppearType : DASH",
    };
    static const char *s_weaponTypes[4] = {
        "Weapon : NONE",
        "Weapon : AUTORIFLE",
        "Weapon : BAZOOKA",
        "Weapon : LANCE",
    };
    static const char *s_actionTypes[3] = {
        "ActionType : NONE",
        "ActionType : ATTACK",
        "ActionType : HIDE",
    };
    static char s_buffer[0x80];
    static int s_appearType = 0;

    strcpy(s_buffer, WalkerParamLabel<EggPawnBase>(query, s_actionTypes));
    switch (query->index) {
    case 9:
        s_appearType = query->value;
        return s_appearTypes[query->value];
    case 8:
        return s_weaponTypes[query->value];
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

const char *EggPawn::s_taskName = "EggPawn";

static GadgetParamDesc lbl_8052BF5C[] = {
    {4, "MoveRange", 0, 0, 0, 200.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchRange", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchAngle", 0, 0, 0, 0.0f, -360.0f, 360.0f, fn_800CBC20, 0},
    {4, "SearchWidth", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchHeight", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchHeightOffset", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "MoveSpeedRatio", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {1, "HaveShield", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "WeaponType", 0, 0, 3, 0.0f, 0.0f, 0.0f, fn_8017B8F4, 0},
    {1, "AppearType", 0, 0, 3, 0.0f, 0.0f, 0.0f, fn_8017B8F4, 0},
    {4, "OffsetPos_X", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_8017B8F4, 0},
    {4, "OffsetPos_Z", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_8017B8F4, 0},
    {4, "OffsetPos_Y", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_8017B8F4, 0},
    {1, "WaitActMoveType", 0, 0, 3, 0.0f, 0.0f, 0.0f, fn_8017B8F4, 0},
    {4, "Pos0_ActionTime", 0, 0, 0, 3.0f, 0.0f, 65536.0f, fn_8017B8F4, 0},
    {1, "Pos0_ActionType", 0, 0, 2, 0.0f, 0.0f, 0.0f, fn_8017B8F4, 0},
    {4, "Pos0_MoveSpeedRatio", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_8017B8F4, 0},
    {4, "Pos1_X", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_8017B8F4, 0},
    {4, "Pos1_Z", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_8017B8F4, 0},
    {4, "Pos1_ActionTime", 0, 0, 0, 3.0f, 0.0f, 65536.0f, fn_8017B8F4, 0},
    {1, "Pos1_ActionType", 0, 0, 2, 0.0f, 0.0f, 0.0f, fn_8017B8F4, 0},
    {4, "Pos1_MoveSpeedRatio", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_8017B8F4, 0},
    {4, "Pos2_X", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_8017B8F4, 0},
    {4, "Pos2_Z", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_8017B8F4, 0},
    {4, "Pos2_ActionTime", 0, 0, 0, 3.0f, 0.0f, 65536.0f, fn_8017B8F4, 0},
    {1, "Pos2_ActionType", 0, 0, 2, 0.0f, 0.0f, 0.0f, fn_8017B8F4, 0},
    {4, "Pos2_MoveSpeedRatio", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_8017B8F4, 0},
    {0},
};

GadgetClassDesc lbl_8052C3BC = {
    "EGG_PAWN", EggPawn::LoadFiles, EggPawn::DropFiles,
    reinterpret_cast<void (*)(UnkPlacement_800C9CD0 *)>(EggPawn::Instantiate), 0, 0, 0x79, 2, 0,
    5, lbl_8052BF5C,
};

