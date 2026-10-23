/* BkWorm (class name from RTTI; its s_taskName is "BkWorm"): the Black Arms worm, BK_WORM in the
 * stage editor (enemy/BkWormData.one). Its body BkWormBase is an EnemyWalkerCommon
 * (Game/Enemy.h) that comes out of the ground and fires homing shots at the player from the
 * barrels of its weapon (WeaponType NORMAL, WIDE or BIG; two, six or two barrels). File name
 * inferred from the class. Names marked "(name inferred)" are not original. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first).
 *
 * NonMatching: 65 of 67 functions and all data match. vfPara_0x48 (0x80192B68): retail loads
 * m_enemy's base pointer above the store of the link register, as in EnemyRender.cpp and
 * CameraScreenSet.cpp; GC/2.7 does not. CreateLockons (0x80192394): retail gives this r31 and the
 * loop counter r27; GC/2.7 swaps the callee-saved registers under every form tried. */

#include "Effect/System/EffectElement.h"
#include "Game/BkWorm.h"
#include "Game/CharInfo.h"
#include "Game/EnemyRender.h"
#include "Game/Gadget.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/Random.h"
#include "Game/ResourceOneFile.h"
#include "Game/Vibration.h"
#include "Game/WeaponBullet.h"
#include "Game/WeaponLockonTarget.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

/* boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out of line; this unit
 * emits the copy for Weapon::LockonTarget (0x80192328; boost 1.32's definition). */
namespace boost {
template <> inline void shared_ptr<Weapon::LockonTarget>::reset() {
    this_type().swap(*this);
}
} // namespace boost

using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using Effect::System::EffectElement;

/* The cursor a parameter's label callback receives (layout as in Helicopter.cpp). */
class UnkLabelQuery_800CBC20 {
  public:
    /* 0x0 */ const GadgetParamDesc *params;
    /* 0x4 */ s32 index; /* the parameter being shown */
    /* 0x8 */ s32 value; /* its current value */
};

/* The part a hit landed on (layout from vfStat_0x70; class name unknown). */
struct UnkHitPart {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ u32 m_id;
};

/* The camera manager (see Game/CameraManagerUnit.h). */
class UnkCameraManager_80009548;

extern "C" {
/* The enemies' shared helpers (behaviour inferred). */
void fn_8019AA40(Enemy *enemy, u16 sound, int arg);
void fn_8019A954(Enemy *enemy, u16 sound, int arg);
/* Resources and models. */
void *fn_80012684(void *loader, int slot, int kind);
void *fn_80012608(void *loader, int slot, int kind);
boost::shared_ptr<EnemyRender> fn_802CDB08(int slot, const EnemyMotionTable &table, void *model,
                                           void *motion);
/* Sound, rumble and a camera shake (behaviours inferred). */
void *fn_80053164(void);
void fn_801D54F4(void *manager, int sound, const Vec3f *position, int arg3, int arg4);
boost::weak_ptr<Vibration::RequestHandle> fn_8034DE08(int pattern, const Vec3f *position,
                                                      int arg);
UnkCameraManager_80009548 *fn_80009548(void);
void fn_8000D8EC(UnkCameraManager_80009548 *manager, int arg1, int arg2, float strength);
/* Effects at a position with a rotation, and at a matrix (behaviours inferred). */
boost::weak_ptr<EffectElement> fn_801A1F54(const Vec3f *position, const Vec3f *rotation, int id);
boost::weak_ptr<EffectElement> fn_801A1ED8(const Mtx34f *matrix, int id);
boost::weak_ptr<EffectElement> fn_801A1E48(const Mtx34f *matrix, int id);
/* Matrix helpers returning by value: the translation, and vector turned by the matrix. */
void fn_8040FAA0(Vec3f *translation, const Mtx34f *matrix);
Vec3f fn_8040B618(const Mtx34f *matrix, const Vec3f *vector);
Vec3f fn_800496F8(const Mtx34f *matrix);
/* Fires a bullet of type for a shot, and throws a weapon of type from position (see
 * BAGunShip.cpp). */
void fn_80051C34(int type, const Weapon::ShotInfo *info);
void fn_80221BA8(const Vec3f *position, const Vec3f *direction, const Vec3f *velocity, int type,
                 int arg);
/* Whether the hit came from the player (behaviour inferred; see DiablonPartsBody.cpp). */
bool fn_80053538(AttackCallbackParam *param);
/* Whether placement has flag (behaviour inferred). */
bool fn_800C9E80(UnkPlacement_800C9CD0 *placement, u32 flag);
/* The character manager's character by id, and the player's id (see BAGunShip.cpp). */
UnkCharaList_80074194 *fn_80074194(void);
boost::weak_ptr<CharInfo> fn_800A8B74(UnkCharaList_80074194 *manager, int id);
/* The target of a player character's target object (see ArkCannon.cpp). */
boost::weak_ptr<Target> fn_80206574(Player::PlayerTargetBase *target);
}

extern s32 lbl_805EE588;

/* Degrees to radians: an out-of-line function of the game (0x8000AB90). */
float DegToRad(float degrees);

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

/* A weapon of the worm: its barrels (model parts), and the weapon type of its shots (struct and
 * names inferred). */
struct WormWeapon {
    const s32 *parts;
    s32 count;
    s32 type;
};

static const s32 lbl_805F53B0[] = {0xBCE, 0xBCC};
static const s32 lbl_804C97A8[] = {0xBCE, 0xBCC, 0xBCD, 0xBD8, 0xBD6, 0xBD7};
static const s32 lbl_805F53B8[] = {0xBCE, 0xBCD};
static const WormWeapon lbl_804C97C0[] = {
    {lbl_805F53B0, 2, 0x15},
    {lbl_804C97A8, 6, 0x16},
    {lbl_805F53B8, 2, 0x17},
};

/* The motion table (steps in .sdata2). */
static const MotionSequenceStep lbl_805F53C0[] = {{4, -1, -1, 30, 1}};
static const MotionSequenceStep lbl_805F53C8[] = {{3, 0xA0, 0xA0, 0, 0}};
static const MotionSequenceStep lbl_805F53D0[] = {{1, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_805F53D8[] = {{5, -1, -1, 30, 0}};
static const MotionSequenceStep lbl_805F53E0[] = {{2, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_805F53E8[] = {{3, -1, -1, 30, 0}};
static const MotionSequence lbl_804C97E4[] = {
    {lbl_805F53C0, 1, -3}, {lbl_805F53D0, 1, -3}, {lbl_805F53C8, 1, -1},
    {lbl_805F53D8, 1, -3}, {lbl_805F53E0, 1, -3}, {lbl_805F53E8, 1, -1},
};
static const MotionSequenceTable lbl_805F53F0 = {lbl_804C97E4, 6};

/* The model's motion tables and model kinds, by weapon. */
static const EnemyMotionEntry lbl_804C9820[] = {{3, -1}, {4, -1}, {5, -1}};
static const s32 lbl_804C982C[] = {6, 7, 8};

/* The rotation the worm's effects play with. */
static Vec3f lbl_80580D98(0.0f, DegToRad(90.0f), 0.0f);

BkWormBase::BkWormBase() : EnemyWalkerCommon(8) {
    m_enemy = this;
    vfStat_0x80(false);
    EnemyMoveCommon::field_0x8[0x5C - 0x8] = 0;
    field_0x2B0 = fn_8040EC80(8.0f, 12.0f);
}

BkWormBase::~BkWormBase() {
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        effect->SetMatrix(0);
        effect->Stop();
    }
    for (int i = 0; i < 2; i++) {
        StopEffect(i);
    }
    for (int i = 0; i < 6; i++) {
        m_lockons[i].reset();
    }
}

int BkWormBase::vfStat_0xB8() {
    return 0x5C;
}

void BkWormBase::vfStat_0x38() {
    void *key = m_enemy->GetPosition();
    if (m_enemy->field_0xA8.Contains(key)) {
        vfStat_0x30(true);
    }
}

void BkWormBase::vfStat_0x70(AttackCallbackParam &param) {
    UnkHitPart *part = *reinterpret_cast<UnkHitPart **>(&param.field_0x8[0x14 - 0x8]);
    if (part != 0 && part->m_id == 0xBBF) {
        EnemyStatusCommon::vfStat_0x70(param);
        return;
    }
    if (!fn_80053538(&param)) {
        u32 flags = param.m_attacker.lock()->m_flags & 0x2B0;
        if (flags != 0) {
            EnemyStatusCommon::vfStat_0x70(param);
            return;
        }
    }
    fn_8019AA40(this, 0x6025, 1);
    param.m_result = 5;
}

void BkWormBase::vfDisp_0x64() {
    void *loader = fn_800122D4();
    int slot = m_motionBase;
    void *model = fn_80012684(loader, slot, lbl_804C982C[m_params.WeaponType]);
    void *motion = fn_80012608(loader, slot, 10);
    m_render =
        fn_802CDB08(slot, MotionTable(&lbl_804C9820[m_params.WeaponType], 1), model, motion);
}

void BkWormBase::vfStat_0xA0() {
    EnemyStatusCommon::vfStat_0xA0();
    int weapon;
    Mtx34f *matrix = GetPartMatrix(0xBBF);
    weapon = m_params.WeaponType;
    fn_80221BA8(&(const Vec3f &)fn_800496F8(matrix), GetRotation(),
                &(const Vec3f &)Vec3f(0.0f, 40.0f, 0.0f), lbl_804C97C0[weapon].type, -1);
}

void BkWormBase::vfDisp_0x10(float dt) {
    EnemyWalkerCommon::vfDisp_0x10(dt);
    if (!vfStat_0x58() && vfStat_0x4C() && !vfStat_0x60()) {
        fn_801D54F4(fn_80053164(), 0x602A, m_enemy->GetPosition(), 0, 1);
    } else {
        void *key = m_enemy->m_ctrl->vfCtrl_0x10();
        if (m_enemy->field_0xA8.Contains(key)) {
            if ((field_0x2B0 -= dt) < 0.0f) {
                field_0x2B0 = fn_8040EC80(8.0f, 12.0f);
                fn_8019A954(this, 0x6029, 1);
            }
        }
    }
    if (m_enemy->m_status->vfStat_0x40()) {
        boost::shared_ptr<EffectElement> effect(m_effect.lock());
        if (effect) {
            effect->Stop();
        }
    } else if (m_effect.expired()) {
        Mtx34f *matrix = GetPartMatrix(0xBBF);
        m_effect = fn_801A1E48(matrix, 0x6D);
    }
}

int BkWormBase::vfPara_0x48() {
    return m_enemy->m_status->vfStat_0x34() ? 0 : 1;
}

int BkWormBase::vfPara_0x5C() {
    return 4;
}

int EnemyParaFunc::vfPara_0x3C() {
    return 0;
}

void BkWormBase::vfDisp_0x4C() {
    Mtx34f *matrix = GetPartMatrix(m_enemy->m_para->vfPara_0x3C());
    if (matrix != 0) {
        fn_801D54F4(fn_80053164(), 0x6009, m_enemy->GetPosition(), 0, 1);
        fn_801D54F4(fn_80053164(), 0x602B, m_enemy->GetPosition(), 0, 1);
        fn_801A1ED8(matrix, 0x6E);
    }
}

bool BkWormBase::vfDisp_0x44(int *command) {
    if (EnemyDispCommon::vfDisp_0x44(command)) {
        return true;
    }
    switch (*command) {
    case 5:
        fn_8000D8EC(fn_80009548(), 0, 0, 0.5f);
        fn_8034DE08(8, GetPosition(), -2);
        fn_8019AA40(this, 0x6023, 1);
        if (*reinterpret_cast<s32 *>(&lbl_8057D6A0[0xA8]) == 0x259) {
            fn_801A1F54(m_enemy->GetPosition(), &lbl_80580D98, 0x293);
        } else {
            fn_801A1F54(m_enemy->GetPosition(), &lbl_80580D98, 0x85);
        }
        break;
    case 19:
        fn_8019AA40(this, 0x6027, 1);
        break;
    case 20:
        fn_8019AA40(this, 0x6026, 1);
        CreateLockons();
        break;
    case 9:
        FireShots();
        break;
    case 10:
        fn_8000D8EC(fn_80009548(), 0, 0, 0.5f);
        fn_8034DE08(8, GetPosition(), -2);
        StopEffect(1);
        fn_801D54F4(fn_80053164(), 0x6023, &m_enemy->field_0x6C, 0, 1);
        if (*reinterpret_cast<s32 *>(&lbl_8057D6A0[0xA8]) == 0x259) {
            fn_801A1F54(&m_enemy->field_0x6C, &lbl_80580D98, 0x293);
        } else {
            fn_801A1F54(&m_enemy->field_0x6C, &lbl_80580D98, 0x85);
        }
        break;
    }
    return true;
}

void BkWormBase::StartEffect(const Vec3f *position, int index) {
    StopEffect(index);
    m_effects[index] = fn_801A1F54(position, &lbl_80580D98, 0x83);
}

void BkWormBase::StopEffect(int index) {
    boost::shared_ptr<EffectElement> effect(m_effects[index].lock());
    if (effect) {
        effect->Stop();
    }
}

void BkWormBase::Shoot(Mtx34f *matrix, boost::shared_ptr<Weapon::LockonTarget> &target, int type) {
    if (matrix != 0) {
        Vec3f position;
        fn_8040FAA0(&position, matrix);
        Vec3f direction(0.0f, 0.0f, -1.0f);
        fn_800091D0(&direction, &(const Vec3f &)fn_8040B618(matrix, &direction));
        Weapon::ShotInfo shot(fn_8017D014(m_enemy), target, true, position, direction,
                              lbl_805E07A0);
        fn_80051C34(type, &shot);
    }
}

void BkWormBase::CreateLockons() {
    int i;
    const WormWeapon &weapon = lbl_804C97C0[m_params.WeaponType];
    for (i = 0; i < weapon.count; i++) {
        m_lockons[i] = boost::shared_ptr<Weapon::LockonTarget>(new Weapon::LockonTarget());
        boost::weak_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588);
        boost::shared_ptr<CharInfo> chara = player.lock();
        if (chara) {
            boost::shared_ptr<Player::PlayerBase> base =
                boost::dynamic_pointer_cast<Player::PlayerBase>(chara);
            if (base.get() != 0 && base->m_target != 0) {
                m_lockons[i]->SetTarget(fn_80206574(base->m_target));
                m_lockons[i]->fn_80060F70();
            }
        }
    }
}

void BkWormBase::FireShots() {
    const WormWeapon &weapon = lbl_804C97C0[m_params.WeaponType];
    for (int i = 0; i < weapon.count; i++) {
        Mtx34f *matrix = GetPartMatrix(weapon.parts[i]);
        if (matrix != 0) {
            Shoot(matrix, m_lockons[i], weapon.type);
            m_lockons[i].reset();
            if (i & 1) {
                fn_8019AA40(this, 0x6028, 1);
            }
        }
    }
}

bool BkWorm::vfMove_0x10() {
    UnkPlacement_800C9CD0 *placement = reinterpret_cast<UnkPlacement_800C9CD0 *>(TEnemySetTask::field_0x2C);
    if (placement != 0 && fn_800C9E80(placement, 0x400000)) {
        return false;
    }
    return *reinterpret_cast<bool *>(&this->EnemyMoveCommon::field_0x8[0x5F - 0x8]);
}

extern "C" const char *fn_801921E8(UnkLabelQuery_800CBC20 *query) {
    static const char *s_weaponTypes[3] = {
        "WeaponType : NORMAL",
        "WeaponType : WIDE",
        "WeaponType : BIG",
    };

    switch (query->index) {
    case 7:
        return s_weaponTypes[query->value];
    }
    return "";
}

/* Not called anywhere: the linker strips it. Retail pools the float constants 0 and 0.5 ahead of
 * the label function's string in .sdata2, as in BkSoldier.cpp; this function, defined after the
 * label function, makes the compiler pool them there (see docs/research-notes.md; the original
 * reason is unknown, and the name is a placeholder). */
static float UnkConstantUser(float x) {
    if (x < 0.5f) {
        return 0.0f;
    }
    return x;
}

BkWorm::BkWorm(Task *parent, UnkPlacement_800C9CD0 *placement)
    : EnemyTemplate<BkWormBase, BkWormAI>(parent, placement) {
    m_enemy = this;
    m_name = s_taskName;
    vfPara_0x40(reinterpret_cast<int>(&lbl_805F53F0));
}

BkWorm::~BkWorm() {}

void BkWorm::vfSet_0x1C(UnkPlacement_800C9CD0 *placement) {
    int weapon = m_params.WeaponType;
    vfSet_0x20(placement);
    /* Copied as a plain struct (word loads and stores), unlike Vec3f's own copy. */
    Vec3fWords rotation = *reinterpret_cast<Vec3fWords *>(m_enemy->GetRotation());
    vfDisp_0x38(m_enemy->GetPosition(), reinterpret_cast<Vec3f *>(&rotation));
    m_ctrl->vfCtrl_0x20();
    if (weapon != m_params.WeaponType) {
        vfDisp_0x64();
    }
}

void BkWorm::vfSet_0x20(UnkPlacement_800C9CD0 *placement) {
    TEnemySetBase::vfSet_0x20(placement);
    fn_800C9DBC(placement, &m_params, sizeof(m_params));
}

void BkWorm::LoadFiles() {
    void *loader = fn_800122D4();
    fn_8001299C(loader, 0xF, new ResourceOneFile("enemy/BkWormData.one", false));
}

void BkWorm::DropFiles() {
    fn_800127F8(fn_800122D4(), 0xF);
}

void BkWorm::Instantiate(UnkPlacement_800C9CD0 *placement) {
    new BkWorm(fn_801EB304(fn_8001C4F8(), 0xE), placement);
}

static GadgetParamDesc lbl_8053492C[] = {
    {4, "MoveRange", 0, 0, 0, 200.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchRange", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchAngle", 0, 0, 0, 0.0f, -360.0f, 360.0f, fn_800CBC20, 0},
    {4, "SearchWidth", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchHeight", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchHeightOffset", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "MoveSpeedRatio", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {1, "WeaponType", 0, 0, 2, 0.0f, 0.0f, 0.0f, fn_801921E8, 0},
    {1, "AttackTimes", 1, 1, 100, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {4, "IntervalSec", 0, 0, 0, 3.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80534AE4 = {
    "BK_WORM", BkWorm::LoadFiles, BkWorm::DropFiles,
    reinterpret_cast<void (*)(UnkPlacement_800C9CD0 *)>(BkWorm::Instantiate), 0, 0, 0x90, 2, 0, 5,
    lbl_8053492C,
};

const char *BkWorm::s_taskName = "BkWorm";

void BkWormBase::vfDisp_0x5C() {}

void BkWormBase::vfDisp_0x54() {}
