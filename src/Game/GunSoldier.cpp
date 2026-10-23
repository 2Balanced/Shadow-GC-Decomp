/* GunSoldier (namespace and class names from RTTI; its task's s_taskName is "GunSoldier"): the
 * G.U.N. soldier, GUN_SOLDIER in the stage editor (enemy/GunSoldierData_<stage>.one). Its body
 * GunSoldier::GunSoldierBase is a SoldierCommonBase (Game/Enemy.h); it may carry a shield and one of
 * six weapons, and appears standing, walking between up to three points, moving at random, at an
 * offset or down (HINSHI) waiting to be helped (AppearType). It shows the player hints when it is
 * near. File name inferred from the class. Names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined first).
 * The unit's code reaches its .data through a pooled base address, so the linker keeps that section
 * whole: Game/TNumEdit.h and Game/CharaColliBaseCharInfo.h supply the RTTI base lists it starts
 * with (as in BkSoldier.cpp). */
/* Keeps the dead copy of bad_alloc's vtable in .data, as retail has it (see <new>). */
#define MSL_NEW_INLINE_BAD_ALLOC

#include <stdio.h>

#include "Game/TNumEdit.h"
/* The .rodata is kept whole, with an unreferenced constant block of the original headers in it (as
 * in BkSoldier.cpp): seven zero words after the TNumEdit names. */
static const float s_zeros[7] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/GunSoldier.h"
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

typedef GunSoldier::GunSoldierBase GunSoldierBase;
typedef GunSoldier::GunSoldierAI GunSoldierAI;
typedef GunSoldier::STemporaryData STemporaryData;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;

/* The stages whose soldier data the task loads (indexed by fn_801A2064; name inferred). */
static const char *s_stageNames[9] = {
    "ark00", "ark50", "citii", "densi", "iseki", "jungl", "kichi", "nazca", "ark00",
};

/* The value EnemyStatusCommon's flag pair 0x90 is set to when a soldier is down (name unknown). */
u8 lbl_805E7E30 = 1;

/* x squared (name inferred). As a call it is not a constant expression, so the value below is stored
 * by the static initializer, as in retail (as in ArkLaser.cpp). */
inline float Square(float x) {
    return x * x;
}

/* The squared distance within which the soldier shows the approach hints (name inferred). */
static float s_hintDistanceSq = Square(300.0f);

/* A status flag set of EnemyStatusCommon's object at 0x8 (defined elsewhere; meaning unknown). */
extern const u32 lbl_805F56D0;

/* The cursor a parameter's label callback receives (layout as in Helicopter.cpp). */
class UnkLabelQuery_800CBC20 {
  public:
    /* 0x0 */ const GadgetParamDesc *params;
    /* 0x4 */ s32 index; /* the parameter being shown */
    /* 0x8 */ s32 value; /* its current value */
};

/* EnemyShield (class name from the RTTI of its shared_ptr control block; code around
 * 0x801A7478): a soldier's shield (names inferred, as in BkSoldier.cpp). */
class EnemyShield {
  public:
    void fn_801A7478();
    void fn_801A74C4();
    bool fn_801A74E8();
    void fn_801A7508(AttackCallbackParam &param);
};

/* EnemyWeapon (class name from RTTI; vtable 0x8053B218 at 0x64; constructor 0x801AB904): the
 * weapon a soldier carries (names inferred, as in BkSoldier.cpp). */
class EnemyWeapon {
  public:
    EnemyWeapon(const boost::weak_ptr<CharInfo> &owner, int type);

    /* 0x00 */ u8 field_0x0[0x64];

    virtual ~EnemyWeapon();
    virtual void vf_0x0C(float dt);
};

/* EnemyTarget (class name from RTTI; see Game/EnemyTarget.cpp). */
class EnemyTarget {
  public:
    void TurnOff();
};

extern "C" {
/* The enemies' shared helpers (behaviour inferred). */
void fn_8019AA40(Enemy *enemy, u16 sound, int arg);
void fn_8019A954(Enemy *enemy, u16 sound, int arg);
float fn_8019B184(Enemy *enemy, const Vec3f *direction);
/* The squared distance from the enemy to target (behaviour inferred). */
float fn_8019B200(Enemy *enemy, void *target);
void fn_801989D4(SoldierCommonBase *soldier, SoldierParam *params, Vec3f *position);
void fn_8017CA2C(Enemy *enemy);
boost::weak_ptr<EnemyShield> fn_801A752C(Enemy *enemy, const EnemyMotionTable &table, int arg);
/* The status object's calls (behaviour unknown). */
void fn_801A9578(UnkEnemyStatus_801AA430 *status, u32 flags);
void fn_801A9844(UnkEnemyStatus_801AA430 *status, int arg1, int arg2);
/* The weapon's calls. */
void fn_801AA830(EnemyWeapon *weapon);
void fn_801AA868(EnemyWeapon *weapon);
void fn_801AA8A0(EnemyWeapon *weapon, const Vec3f *offset);
void fn_801AA8E4(EnemyWeapon *weapon, float value);
void fn_801AB650(EnemyWeapon *weapon, void *part);
void fn_801AB3F0(EnemyWeapon *weapon);
void *fn_801A46E4(void *model, int part);
/* Sets flag 0x8 of the owner's record (behaviour inferred; see Cream.cpp). */
void fn_800C985C(UnkPlacement_800C9CD0 *owner);
/* Rumble. */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DE08(int pattern, const Vec3f *position,
                                                      int arg);
/* Resources and models. */
int fn_801A2064(void);
void *fn_80012684(void *loader, int slot, int kind);
void *fn_80012608(void *loader, int slot, int kind);
boost::shared_ptr<EnemyRender> fn_802CDB08(int slot, const EnemyMotionTable &table, void *model,
                                           void *motion);
/* v turned about y by angle. */
Vec3f fn_8040D0AC(const Vec3f *v, float angle);
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
static const MotionSequenceStep lbl_805F55A0[] = {{1, -1, -1, 30, 1}};
static const MotionSequenceStep lbl_805F55A8[] = {{2, -1, -1, 30, 1}};
static const MotionSequenceStep lbl_805F55B0[] = {{3, -1, -1, 30, 1}};
static const MotionSequenceStep lbl_804CA828[] = {{12, -1, -1, 30, 0}, {12, -1, -1, 0, 1}};
static const MotionSequenceStep lbl_804CA838[] = {{13, -1, -1, 30, 0}, {13, -1, -1, 0, 1}};
static const MotionSequenceStep lbl_804CA848[] = {{14, -1, -1, 30, 0}, {14, -1, -1, 0, 1}};
static const MotionSequenceStep lbl_804CA858[] = {{28, -1, -1, 30, 0}, {28, -1, -1, 0, 1}};
static const MotionSequenceStep lbl_804CA868[] = {
    {4, -1, 14, 0, 0}, {4, 14, 15, 0, 2}, {4, 15, -1, 0, 0}, {31, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_804CA888[] = {
    {5, -1, 14, 0, 0}, {5, 14, 15, 0, 2}, {5, 15, -1, 0, 0}, {32, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_804CA8A8[] = {{6, -1, -1, 30, 0}, {6, -1, -1, 0, 1}};
static const MotionSequenceStep lbl_804CA8B8[] = {{7, -1, -1, 30, 0}, {7, -1, -1, 0, 1}};
static const MotionSequenceStep lbl_804CA8C8[] = {
    {4, -1, 14, 0, 0}, {4, 14, 15, 0, 2}, {4, 15, -1, 0, 0}};
static const MotionSequenceStep lbl_804CA8E0[] = {
    {5, -1, 14, 0, 0}, {5, 14, 15, 0, 2}, {5, 15, -1, 0, 0}};
static const MotionSequenceStep lbl_805F55B8[] = {{11, -1, -1, 20, 0}};
static const MotionSequenceStep lbl_805F55C0[] = {{49, -1, -1, 10, 1}};
static const MotionSequenceStep lbl_804CA8F8[] = {{8, -1, -1, 30, 2}, {10, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_805F55C8[] = {{1, -1, -1, 30, 0}};
static const MotionSequenceStep lbl_805F55D0[] = {{2, -1, -1, 30, 0}};
static const MotionSequenceStep lbl_805F55D8[] = {{3, -1, -1, 30, 0}};
static const MotionSequenceStep lbl_805F55E0[] = {{29, -1, -1, 30, 0}};
static const MotionSequenceStep lbl_805F55E8[] = {{30, -1, -1, 30, 0}};
static const MotionSequenceStep lbl_805F55F0[] = {{26, -1, -1, 30, 0}};
static const MotionSequenceStep lbl_804CA908[] = {
    {37, -1, -1, 10, 0}, {37, 10, -1, 30, 0}, {16, -1, -1, 0, 0}, {1, -1, -1, 30, 0}};
static const MotionSequenceStep lbl_804CA928[] = {
    {38, -1, -1, 10, 0}, {38, 10, -1, 30, 0}, {17, -1, -1, 0, 0}, {2, -1, -1, 30, 0}};
static const MotionSequenceStep lbl_804CA948[] = {
    {39, -1, -1, 10, 0}, {39, 10, -1, 30, 0}, {18, -1, -1, 0, 0}, {3, -1, -1, 30, 0}};
static const MotionSequenceStep lbl_804CA968[] = {
    {40, -1, -1, 10, 0}, {40, 10, -1, 30, 0}, {19, -1, -1, 0, 0},
    {19, -1, -1, 0, 0},  {19, -1, -1, 0, 0},  {19, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_804CA998[] = {
    {41, -1, -1, 10, 0}, {41, 10, -1, 30, 0}, {20, -1, -1, 0, 0},
    {20, -1, -1, 0, 0},  {20, -1, -1, 0, 0},  {20, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_804CA9C8[] = {
    {42, -1, -1, 10, 0}, {42, 10, -1, 30, 0}, {21, -1, -1, 0, 0},
    {21, -1, -1, 0, 0},  {21, -1, -1, 0, 0},  {21, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_804CA9F8[] = {
    {43, -1, -1, 10, 0}, {43, 10, -1, 30, 0}, {22, -1, -1, 0, 0}, {22, -1, -1, 0, 0},
    {22, -1, -1, 0, 0},  {22, -1, -1, 0, 0},  {22, -1, -1, 0, 0}, {22, -1, -1, 0, 0},
    {22, -1, -1, 0, 0},  {22, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_804CAA48[] = {
    {44, -1, -1, 10, 0}, {44, 10, -1, 30, 0}, {23, -1, -1, 0, 0}, {23, -1, -1, 0, 0},
    {23, -1, -1, 0, 0},  {23, -1, -1, 0, 0},  {23, -1, -1, 0, 0}, {23, -1, -1, 0, 0},
    {23, -1, -1, 0, 0},  {23, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_804CAA98[] = {
    {46, -1, -1, 10, 0}, {46, 10, -1, 30, 0}, {24, -1, -1, 0, 0}, {24, -1, -1, 0, 0},
    {24, -1, -1, 0, 0},  {24, -1, -1, 0, 0},  {24, -1, -1, 0, 0}, {24, -1, -1, 0, 0},
    {24, -1, -1, 0, 0},  {24, -1, -1, 0, 0},  {24, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_804CAAF0[] = {
    {33, -1, -1, 10, 0}, {33, 10, -1, 30, 0}, {15, -1, -1, 0, 0}, {1, -1, -1, 30, 0}};
static const MotionSequenceStep lbl_804CAB10[] = {
    {45, -1, -1, 10, 0}, {45, 10, -1, 30, 0}, {25, -1, -1, 0, 0}, {1, -1, -1, 30, 0}};
static const MotionSequenceStep lbl_804CAB30[] = {{47, -1, -1, 10, 0}, {47, -1, -1, 0, 0}};
static const MotionSequenceStep lbl_805F55F8[] = {{9, -1, -1, 30, 0}};
static const MotionSequence lbl_804CAB40[] = {
    {lbl_805F55A0, 1, -3},  {lbl_805F55A8, 1, -3}, {lbl_805F55B0, 1, -3}, {lbl_804CA828, 2, -3},
    {lbl_804CA838, 2, -3},  {lbl_804CA848, 2, -3}, {lbl_804CA858, 2, -3}, {lbl_804CA868, 4, -3},
    {lbl_804CA888, 4, -3},  {lbl_804CA8A8, 2, -3}, {lbl_804CA8B8, 2, -3}, {lbl_804CA8C8, 3, -3},
    {lbl_804CA8E0, 3, -3},  {lbl_805F55B8, 1, -3}, {lbl_805F55C0, 1, -3}, {lbl_804CA8F8, 2, -3},
    {lbl_805F55C8, 1, -3},  {lbl_805F55D0, 1, -3}, {lbl_805F55D8, 1, -3}, {lbl_805F55E0, 1, -3},
    {lbl_805F55E8, 1, -3},  {lbl_805F55F0, 1, -3}, {lbl_804CA908, 4, -3}, {lbl_804CA928, 4, -3},
    {lbl_804CA948, 4, -3},  {lbl_804CA968, 6, -3}, {lbl_804CA998, 6, -3}, {lbl_804CA9C8, 6, -3},
    {lbl_804CA9F8, 10, -3}, {lbl_804CAA48, 10, -3}, {lbl_804CAA98, 11, -3}, {lbl_804CAAF0, 4, -3},
    {lbl_804CAB10, 4, -3},  {lbl_804CAB30, 2, -3}, {lbl_805F55F8, 1, -3},
};
static const MotionSequenceTable lbl_805F5600 = {lbl_804CAB40, 35};

/* The weapon types the WeaponType parameter selects. */
static const s32 lbl_804CAC58[] = {0, 0x1E, 1, 2, 3, 0xC, 0x12};

GunSoldierBase::GunSoldierBase()
    : SoldierCommonBase(0), m_weapon(0), m_hintShown(0), m_hintApproach(0), m_contacted(0),
      field_0x357(0), field_0x358(0) {
    m_enemy = this;
    vfPara_0x40(reinterpret_cast<int>(&lbl_805F5600));
    field_0x344 = 0;
}

GunSoldierBase::~GunSoldierBase() {
    if (!m_shield.expired()) {
        m_shield.lock()->fn_801A7478();
        m_shield.reset();
    }
    if (m_weapon != 0) {
        delete m_weapon;
        m_weapon = 0;
    }
}

void GunSoldierBase::vfSet_0x1C(UnkPlacement_800C9CD0 *placement) {
    vfSet_0x20(placement);
    /* Copied as a plain struct (word loads and stores), unlike Vec3f's own copy. */
    Vec3fWords rotation = *reinterpret_cast<Vec3fWords *>(GetRotation());
    vfDisp_0x38(GetPosition(), reinterpret_cast<Vec3f *>(&rotation));
    m_ctrl->vfCtrl_0x20();
}

void GunSoldierBase::vfSet_0x20(UnkPlacement_800C9CD0 *placement) {
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
    if (m_params.WeaponType > 6) {
        m_params.WeaponType = 6;
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
    if (m_params.Pos0_WaitType > 3) {
        m_params.Pos0_WaitType = 3;
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
    if (m_params.Pos1_WaitType > 3) {
        m_params.Pos1_WaitType = 3;
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
    if (m_params.Pos2_WaitType > 3) {
        m_params.Pos2_WaitType = 3;
    }
    fn_801989D4(this, &m_params, &field_0x48);
    if (placement->m_adapterState != 0) {
        STemporaryData *data = static_cast<STemporaryData *>(placement->m_adapterState);
        if (data != 0) {
            if (data->m_state == 0) {
                if (m_appearType == 5) {
                    m_appearType = 0;
                }
            } else if (data->m_state == 1) {
                if (m_appearType != 5) {
                    m_appearType = 5;
                }
            } else if (data->m_state == 2) {
                if (m_appearType == 5) {
                    m_appearType = 0;
                }
                m_contacted = 1;
            }
        }
    } else {
        STemporaryData *data = new STemporaryData();
        if (data != 0) {
            if (m_appearType == 5) {
                data->m_state = 1;
            }
            fn_800C9930(placement, data);
        }
    }
    if (m_appearType == 5) {
        field_0x357 = field_0x270 != 0;
        field_0x270 = 0;
        m_weaponType = 0;
        m_params.WeaponType = 0;
    }
    if (m_appearType == 4) {
        GetPosition()->x += m_params.Pos1_X;
        GetPosition()->y += m_params.Pos1_WaitSec;
        GetPosition()->z += m_params.Pos1_Z;
    }
}

void GunSoldierBase::vfStat_0x38() {
    if (m_appearType == 4 && !field_0xA8.Contains(m_ctrl->vfCtrl_0x10())) {
        if (m_weapon != 0) {
            fn_801AA830(m_weapon);
        }
        m_status->vfStat_0x3C(true);
        m_status->vfStat_0x30(false);
    } else {
        if (m_weapon != 0) {
            fn_801AA868(m_weapon);
        }
        m_status->vfStat_0x3C(false);
        m_status->vfStat_0x30(true);
    }
}

void GunSoldierBase::vfDisp_0x64() {
    EnemyMotionEntry entries[2] = {{4, -1}, {3, 30}};
    void *loader = fn_800122D4();
    int slot = m_motionBase;
    void *model = fn_80012684(loader, slot, 7);
    void *motion = fn_80012608(loader, slot, 8);
    m_render = fn_802CDB08(slot, MotionTable(entries, 2), model, motion);
}

void GunSoldierBase::vfStat_0x08() {
    EnemyStatusCommon::vfStat_0x08();
    if (m_appearType == 5) {
        field_0x7C = 0.0f;
        EnemyStatusCommon::field_0x90.Set(lbl_805E7E30);
        m_target->TurnOff();
        vfDisp_0x58(false);
    }
}

void GunSoldierBase::vfStat_0x10() {
    EnemyStatusCommon::vfStat_0x10();
    vf_0x29C();
}

int GunSoldierBase::vfStat_0xB8() {
    int result = 0x4F;
    if (m_params.HaveShield != 0) {
        result = 0x50;
    }
    return result;
}

void GunSoldierBase::vfStat_0xA0() {
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
        m_weaponType = 0;
    }
    if (m_appearType != 5) {
        m_appearType = 5;
        fn_8019A954(this, 0x600E, 1);
    }
    vfDisp_0x58(false);
}

void GunSoldierBase::vfStat_0xA4() {
    m_contacted = 1;
    m_appearType = 0;
    vfDisp_0x58(true);
}

void GunSoldierBase::vfStat_0x0C() {
    EnemyStatusCommon::vfStat_0x0C();
    if (m_appearType == 5) {
        fn_801A9578(&this->EnemyStatusCommon::field_0x8, 0x200201);
        fn_801A9844(&this->EnemyStatusCommon::field_0x8, 0, 1);
    }
}

void GunSoldierBase::vfDisp_0x08() {
    EnemyDispCommon::vfDisp_0x08();
    if (m_shield.expired() && field_0x270) {
        m_shield = CreateShield();
    }
    if (m_weapon == 0 && m_weaponType != 0) {
        m_weapon = new EnemyWeapon(fn_8017D014(this), lbl_804CAC58[GetWeaponType()]);
    }
}

void GunSoldierBase::vfDisp_0x10(float dt) {
    EnemyWalkerCommon::vfDisp_0x10(dt);
    if (field_0xA8.Contains(m_ctrl->vfCtrl_0x10())) {
        if (!m_hintShown) {
            if (m_appearType == 5) {
                m_hint.Show(3);
            } else if (m_contacted) {
                m_hint.Show(4);
            } else {
                m_hint.Show(2);
            }
            m_hintShown = 1;
        }
    } else {
        if (!m_hintApproach && fn_8019B200(this, m_ctrl->vfCtrl_0x10()) < s_hintDistanceSq &&
            m_appearType != 5) {
            int type = m_points[field_0x280].field_0x14;
            if (type == 0) {
                m_hintApproach = 1;
                m_hint.Show(0);
            } else if (type == 1) {
                m_hintApproach = 1;
                m_hint.Show(1);
            }
        }
        m_hintShown = 0;
    }
    if (m_weapon != 0) {
        void *part = fn_801A46E4(m_disp->vfDisp_0x1C(), 0x18);
        fn_801AB650(m_weapon, part);
        fn_801AA8E4(m_weapon, field_0xEC);
        m_weapon->vf_0x0C(dt);
    }
    if (vfStat_0x4C() && !vfStat_0x60()) {
        fn_8019A954(this, 0x600D, 1);
    }
}

void GunSoldierBase::vfDisp_0x40(EnemyMotionEventArg &arg) {
    EnemyDispCommon::vfDisp_0x40(arg);
    EnemyWeapon *weapon = m_weapon;
    if (weapon != 0 && arg.m_type == 0x5000 && arg.m_data->kind == 2) {
        weapon->field_0x0[0] = field_0x358;
        fn_801AB3F0(weapon);
        field_0x358 = 0;
    }
}

bool GunSoldierBase::vfDisp_0x44(int *command) {
    if (EnemyDispCommon::vfDisp_0x44(command)) {
        return true;
    }
    switch (*command) {
    case 0:
        if (m_appearType == 5) {
            fn_8019AA40(this, 0x6006, 1);
        }
        break;
    case 1:
        fn_8019AA40(this, 0x600B, 1);
        fn_8017CA2C(this);
        break;
    case 2:
        fn_8034DE08(10, GetPosition(), -2);
        fn_8019AA40(this, 0x600C, 1);
        break;
    }
    return true;
}

float GunSoldierBase::vfPara_0x6C() {
    return EnemyParamCommon::vfPara_0x6C();
}

int GunSoldierBase::vfPara_0x48() {
    return vfPara_0x54();
}

int GunSoldierBase::vfPara_0x4C() {
    int result = 3;
    switch (m_weaponType) {
    case 0:
    case 1:
    case 2:
        break;
    case 3:
    case 4:
    case 5:
    case 6:
        if (field_0x270) {
            result = 5;
        } else {
            result = 4;
        }
        break;
    }
    return result;
}

int GunSoldierBase::vfPara_0x50() {
    return 15;
}

int GunSoldierBase::vfPara_0x54() {
    int result = 0;
    switch (m_weaponType) {
    case 0:
    case 1:
    case 2:
        break;
    case 3:
    case 4:
    case 5:
    case 6:
        if (field_0x270) {
            result = 2;
        } else {
            result = 1;
        }
        break;
    }
    return result;
}

int GunSoldierBase::vfPara_0x58() {
    int result = -1;
    switch (m_weaponType) {
    case 0:
        if (field_0x344 == 0) {
            result = 0x13;
        } else {
            result = 0x14;
        }
        break;
    case 1:
        result = 0x15;
        break;
    case 2:
        if (field_0x270) {
            result = 0x18;
        } else {
            if (field_0x344 == 0) {
                result = 0x16;
            } else {
                result = 0x17;
            }
        }
        break;
    case 3:
        if (field_0x270) {
            result = 0x1B;
        } else {
            if (field_0x344 == 0) {
                result = 0x19;
            } else {
                result = 0x1A;
            }
        }
        break;
    case 4:
        if (field_0x270) {
            result = 0x1E;
        } else {
            if (field_0x344 == 0) {
                result = 0x1C;
            } else {
                result = 0x1D;
            }
        }
        break;
    case 5:
        result = 0x1F;
        break;
    case 6:
        result = 0x20;
        break;
    }
    return result;
}

int GunSoldierBase::vfPara_0x5C() {
    return field_0x357 ? 7 : 8;
}

int GunSoldierBase::vfPara_0x60() {
    return field_0x357 ? 11 : 12;
}

int GunSoldierBase::vfPara_0x64() {
    return 0x21;
}

int GunSoldierBase::vf_0x238(int index) {
    int result = 0;
    switch (m_points[index].field_0x14) {
    case 0:
        result = 6;
        break;
    case 1:
        result = m_para->vfPara_0x58();
        break;
    case 2:
        result = 0x22;
        break;
    case 3:
        result = vfPara_0x48();
        break;
    }
    return result;
}

int GunSoldierBase::vf_0x23C() {
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

void GunSoldierBase::vfStat_0x70(AttackCallbackParam &param) {
    if (m_appearType == 5) {
        return;
    }
    m_contacted = 0;
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
        EnemyStatusCommon::vfStat_0x70(param);
    } else {
        EnemyStatusCommon::vfStat_0x70(param);
    }
    if (m_status->vfStat_0x74() == 0) {
        field_0x357 = 1;
    } else {
        field_0x357 = 0;
    }
}

/* The shield's motion table. */
static const EnemyMotionEntry lbl_805F5610[] = {{2, -1}};

boost::weak_ptr<EnemyShield> GunSoldierBase::CreateShield() {
    boost::weak_ptr<EnemyShield> shield = fn_801A752C(this, MotionTable(lbl_805F5610, 1), 0x1C);
    return shield;
}

void GunSoldierBase::fn_801972A8() {
    fn_801A9578(&this->EnemyStatusCommon::field_0x8, 0x200201);
}

void GunSoldierBase::fn_80197280() {
    fn_801A9578(&this->EnemyStatusCommon::field_0x8, lbl_805F56D0);
}

void GunSoldier::GunSoldier::vf_0x29C() {
    UnkPlacement_800C9CD0 *placement =
        reinterpret_cast<UnkPlacement_800C9CD0 *>(TEnemySetTask::field_0x2C);
    if (placement != 0 && placement->m_adapterState != 0) {
        STemporaryData *data = static_cast<STemporaryData *>(placement->m_adapterState);
        if (data != 0) {
            if (m_appearType == 5) {
                data->m_state = 1;
            } else if (m_contacted) {
                data->m_state = 2;
            } else {
                data->m_state = 0;
            }
        }
    }
}

void GunSoldier::GunSoldier::vf_0x2A0() {
    UnkPlacement_800C9CD0 *placement =
        reinterpret_cast<UnkPlacement_800C9CD0 *>(TEnemySetTask::field_0x2C);
    if (placement != 0) {
        fn_800C985C(placement);
    }
}

extern "C" const char *fn_80196EA8(UnkLabelQuery_800CBC20 *query) {
    static const char *s_weaponTypes[7] = {
        "Weapon : NONE",  "Weapon : KNIFE",   "Weapon : GUN",     "Weapon : MACHINEGUN",
        "Weapon : RIFLE", "Weapon : GRENADE", "Weapon : MISSILE",
    };
    static const char *s_appearTypes[6] = {
        "AppearType : STAND",       "AppearType : LINEAR_MOVE", "AppearType : TRIANGLE_MOVE",
        "AppearType : RANDOM_MOVE", "AppearType : OFFSETPOS",   "AppearType : HINSHI",
    };
    static const char *s_waitTypes[4] = {
        "WaitType : RADIO_CONTACT", "WaitType : ATTACK", "WaitType : HIDE", "WaitType : KAMAE"};
    static char s_buffer[0x80];
    static int s_appearType = 0;

    switch (query->index) {
    case 7:
        if (s_appearType != 5) {
            return query->params[query->index].label;
        }
        sprintf(s_buffer, "MotDir : %s", query->value != 0 ? "Lower" : "Upper");
        return s_buffer;
    case 8:
        if (s_appearType != 5) {
            return s_weaponTypes[query->value];
        }
        break;
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
            return "Rand_MoveSpeedRatio";
        }
        if (s_appearType == 1 || s_appearType == 2) {
            return "Pos0_MoveSpeedRatio";
        }
        break;
    case 13:
    case 14:
        if (s_appearType == 1 || s_appearType == 2 || s_appearType == 4) {
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
        if (s_appearType == 4) {
            return "Pos1_Y";
        }
        if (s_appearType == 1 || s_appearType == 2) {
            return query->params[query->index].label;
        }
        break;
    case 17:
        if (s_appearType == 1 || s_appearType == 2) {
            return query->params[query->index].label;
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
            return query->params[query->index].label;
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
 * the compiler pool them there (see docs/research-notes.md; the name is a placeholder). */
static float UnkConstantUser(float x) {
    x *= 0.0f;
    x *= 0.5f;
    return x;
}

static GadgetParamDesc lbl_805383A0[] = {
    {4, "MoveRange", 0, 0, 0, 200.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchRange", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchAngle", 0, 0, 0, 0.0f, -360.0f, 360.0f, fn_800CBC20, 0},
    {4, "SearchWidth", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchHeight", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchHeightOffset", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "MoveSpeedRatio", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {1, "HaveShield", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_80196EA8, 0},
    {1, "WeaponType", 0, 0, 6, 0.0f, 0.0f, 0.0f, fn_80196EA8, 0},
    {1, "AppearType", 0, 0, 5, 0.0f, 0.0f, 0.0f, fn_80196EA8, 0},
    {1, "Pos0_WaitType", 0, 0, 3, 0.0f, 0.0f, 0.0f, fn_80196EA8, 0},
    {4, "Pos0_WaitSec", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_80196EA8, 0},
    {4, "Pos0_MoveSpeedRatio", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_80196EA8, 0},
    {4, "Pos1_X", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_80196EA8, 0},
    {4, "Pos1_Z", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_80196EA8, 0},
    {1, "Pos1_WaitType", 0, 0, 3, 0.0f, 0.0f, 0.0f, fn_80196EA8, 0},
    {4, "Pos1_WaitSec", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_80196EA8, 0},
    {4, "Pos1_MoveSpeedRatio", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_80196EA8, 0},
    {4, "Pos2_X", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_80196EA8, 0},
    {4, "Pos2_Z", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_80196EA8, 0},
    {1, "Pos2_WaitType", 0, 0, 3, 0.0f, 0.0f, 0.0f, fn_80196EA8, 0},
    {4, "Pos2_WaitSec", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_80196EA8, 0},
    {4, "Pos2_MoveSpeedRatio", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_80196EA8, 0},
    {0},
};

const char *GunSoldier::GunSoldier::s_taskName = "GunSoldier";

GadgetClassDesc lbl_80538760 = {
    "GUN_SOLDIER", GunSoldier::GunSoldier::LoadFiles, GunSoldier::GunSoldier::DropFiles,
    reinterpret_cast<void (*)(UnkPlacement_800C9CD0 *)>(GunSoldier::GunSoldier::Instantiate), 0, 0,
    0x64, 2, 0, 5, lbl_805383A0,
};

GunSoldier::GunSoldier::GunSoldier(Task *parent, UnkPlacement_800C9CD0 *placement)
    : EnemyTemplate<GunSoldierBase, GunSoldierAI>(parent, placement) {
    m_enemy = this;
    m_name = s_taskName;
    vf_0x2A0();
}

GunSoldier::GunSoldier::~GunSoldier() {
    vf_0x29C();
}

void GunSoldier::GunSoldier::LoadFiles() {
    char path[0x20];
    sprintf(path, "enemy/GunSoldierData_%s.one", s_stageNames[fn_801A2064()]);
    void *loader = fn_800122D4();
    fn_8001299C(loader, 7, new ResourceOneFile(path, false));
}

void GunSoldier::GunSoldier::DropFiles() {
    fn_800127F8(fn_800122D4(), 7);
}

void GunSoldier::GunSoldier::Instantiate(UnkPlacement_800C9CD0 *placement) {
    new GunSoldier(fn_801EB304(fn_8001C4F8(), 0xE), placement);
}

/* Not called anywhere: the linker strips it. Retail's .data, which the linker keeps whole, has the
 * zero-filled copy of sp_counted_base's vtable after EnemyTemplate's; this function, defined
 * last, makes the compiler place it there (as in BkSoldier.cpp; the name is a placeholder). */
static void UnkCountedBaseUser(boost::detail::sp_counted_base *base) {
    base->boost::detail::sp_counted_base::~sp_counted_base();
}
