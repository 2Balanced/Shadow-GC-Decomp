/* GunBeetle (class name from RTTI; its s_taskName is "GunBeetle"): the G.U.N. Beetle, GUN_BEETLE in
 * the stage editor (enemy/GunBeetleData.one). Its body GunBeetleBase is a FlyerCommonBase
 * (Game/EnemyFlyer.h); it may carry one of three guns or drop bombs (WeaponType), and with
 * ActionType 1 it discharges sparks at intervals. File name inferred from the class. Names marked
 * "(name inferred)" are not original. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */

#include "Game/GunBeetle.h"
#include "Game/CharaColliAttack.h"
#include "Game/EnemyRender.h"
#include "Game/Gadget.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathTypes.h"
#include "Game/ResourceOneFile.h"
#include "Game/ThrowBomb.h"
#include "Effect/System/EffectElement.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

using Effect::System::EffectElement;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;

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

/* The parameters of a bomb throw (layout as in EggLastMech.cpp; the out-of-line constructor is
 * emitted here first; names inferred). */
struct UnkEffectParam_80194C1C {
    UnkEffectParam_80194C1C(const Vec3f &position, const Vec3f &velocity, float scale, bool flag) {
        fn_800091EC(&m_position, &position);
        fn_800091EC(&m_velocity, &velocity);
        m_scale = scale;
        field_0x1C = 4.0f;
        m_flag = flag;
    }

    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ Vec3f m_velocity;
    /* 0x18 */ float m_scale;
    /* 0x1C */ float field_0x1C;
    /* 0x20 */ u8 m_flag;
};
SIZE_ASSERT(UnkEffectParam_80194C1C, 0x24);

/* The cursor a parameter's label callback receives (layout as in Helicopter.cpp). */
class UnkLabelQuery_800CBC20 {
  public:
    /* 0x0 */ const GadgetParamDesc *params;
    /* 0x4 */ s32 index;
    /* 0x8 */ s32 value;
};

extern "C" {
/* The enemies' shared helpers (behaviour inferred): a sound that follows the enemy. */
UnkSoundHandle_801D3DC8 fn_8019A908(Enemy *enemy, u16 sound, int arg);
bool fn_80053538(AttackCallbackParam *param);
/* An effect that follows matrix (behaviour inferred). */
boost::weak_ptr<EffectElement> fn_801A1E48(Mtx34f *matrix, int kind);
/* The weapon's calls (see BkSoldier.cpp). */
void fn_801AA830(EnemyWeapon *weapon);
void fn_801AA868(EnemyWeapon *weapon);
void fn_801AA8A0(EnemyWeapon *weapon, const Vec3f *offset);
void fn_801AA8E4(EnemyWeapon *weapon, float value);
void fn_801AB3F0(EnemyWeapon *weapon);
void fn_801AB710(EnemyWeapon *weapon, const Mtx34f *matrix);
/* Flyer helpers (Game/EnemyFlyer.h; behaviour inferred). */
void fn_80193790(FlyerCommonBase *flyer, GunBeetleParam *params);
/* Resources and models (see BkSoldier.cpp). */
void *fn_80012684(void *loader, int slot, int kind);
void *fn_80012608(void *loader, int slot, int kind);
boost::shared_ptr<EnemyRender> fn_802CDB08(int slot, const EnemyMotionTable &table, void *model,
                                           void *motion);
/* Sound. */
void *fn_80053164(void);
void fn_801D54F4(void *manager, int sound, const Vec3f *position, int arg3, int arg4);
/* Matrix and vector helpers. */
void fn_8040B52C(Mtx34f *matrix, float x, float y, float z);
void fn_8040BAC4(Mtx34f *matrix, const Mtx34f *other);
Vec3f fn_800496F8(const Mtx34f *matrix);
Vec3f fn_8040D0AC(const Vec3f *v, float angle);
}

/* EnemyStatFunc's slot 0x84 (Game/Enemy.h). Calling it by name makes GC/2.7 emit its weak
 * EnemyStatFunc copy right after the caller, earlier than retail has it, so the call goes through
 * this view of the vtable's first slots (not original). */
class UnkStatusView {
  public:
    virtual void vfStat_0x08() = 0;
    virtual void vfStat_0x0C() = 0;
    virtual void vfStat_0x10() = 0;
    virtual void vfStat_0x14() = 0;
    virtual void vfStat_0x18() = 0;
    virtual void vfStat_0x1C() = 0;
    virtual void vfStat_0x20() = 0;
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
    virtual void vfStat_0x84(int arg) = 0;
};

inline void CallStatusSlot84(EnemyStatFunc *status, int arg) {
    reinterpret_cast<UnkStatusView *>(status)->vfStat_0x84(arg);
}

inline void EnemyWeapon::Fire() {
    field_0x0[0] = 0;
    fn_801AB3F0(this);
}

/* Vector assignment through the out-of-line operator (as AssignVector in Game/GadgetUtil.h). */
inline void SetVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* A motion table built in place (as in BkSoldier.cpp). */
struct MotionTable : public EnemyMotionTable {
    MotionTable(const EnemyMotionEntry *e, s32 c) {
        entries = e;
        count = c;
    }
};

/* Three floats copied as a plain struct (as in BkSoldier.cpp). */
struct Vec3fWords {
    float x;
    float y;
    float z;
};

/* Where each WeaponType's gun sits on the model part, and (z) the seconds between shots. */
static const Vec3fWords lbl_804C9C70[5] = {
    {0.0f, -3.0f, 0.15f}, {0.6f, -2.3f, 1.5f}, {0.0f, 0.0f, 5.0f}, {0.0f, 0.0f, 0.15f},
    {0.0f, 0.0f, 0.0f},
};
/* The EnemyWeapon type of each WeaponType (0: bombs, or none). */
static const s32 lbl_804C9CAC[5] = {3, 0x13, 0, 2, 0};

/* The motion table (steps in .rodata and .sdata2). */
static const MotionSequenceStep lbl_804C9CC0[] = {{2, -1, -1, 30, 0}, {2, -1, -1, 0, 1}};
static const MotionSequenceStep lbl_805F5460[] = {{2, -1, -1, 90, 0}};
static const MotionSequenceStep lbl_805F5468[] = {{1, -1, -1, 0, 0}};
static const MotionSequence lbl_804C9CD0[] = {
    {lbl_804C9CC0, 2, -3},
    {lbl_805F5460, 1, -3},
    {lbl_805F5468, 1, -3},
};
static const MotionSequenceTable lbl_805F5470 = {lbl_804C9CD0, 3};

/* The model's motion table of each BodyType. */
static const EnemyMotionEntry lbl_804C9CE8[] = {{3, -1}, {4, 2000}, {5, -1}, {6, 2000}};

/* The Beetle's gun (class name unknown): an EnemyWeapon, or for WeaponType 2 a bomb dropper,
 * placed on model part 3000 (names inferred). */
class GunBeetleGun {
  public:
    GunBeetleGun(const boost::weak_ptr<CharInfo> &owner, int type) {
        m_offset = &lbl_804C9C70[type];
        m_state = 0;
        if (lbl_804C9CAC[type] != 0) {
            m_weapon = new EnemyWeapon(owner, lbl_804C9CAC[type]);
        } else {
            m_weapon = 0;
        }
        m_matrix = 0;
        m_firing = 0;
        m_timer = 0.0f;
        m_bombScale = 0.0f;
    }
    ~GunBeetleGun() {
        if (m_weapon != 0) {
            delete m_weapon;
            m_weapon = 0;
        }
    }

    static GunBeetleGun *Create(const boost::weak_ptr<CharInfo> &owner, int type);
    void Update(float dt);

    void SetValue(float value) {
        if (m_weapon != 0) {
            fn_801AA8E4(m_weapon, value);
        }
    }
    void Hide() {
        if (m_weapon != 0) {
            fn_801AA830(m_weapon);
        }
    }
    void Show() {
        if (m_weapon != 0) {
            fn_801AA868(m_weapon);
        }
    }
    void Drop(const Vec3f &offset) {
        if (m_weapon != 0) {
            fn_801AA8A0(m_weapon, &offset);
        }
    }

    /* 0x00 */ s32 m_state;
    /* 0x04 */ EnemyWeapon *m_weapon;
    /* 0x08 */ const Vec3fWords *m_offset;
    /* 0x0C */ Mtx34f *m_matrix;
    /* 0x10 */ u8 m_firing;
    /* 0x14 */ float m_timer;
    /* 0x18 */ float m_bombScale;
};
SIZE_ASSERT(GunBeetleGun, 0x1C);

GunBeetleGun *GunBeetleGun::Create(const boost::weak_ptr<CharInfo> &owner, int type) {
    return new GunBeetleGun(owner, type);
}

void GunBeetleGun::Update(float dt) {
    static Matrix34 s_matrix;
    if (m_matrix != 0) {
        fn_8040B6F8(&s_matrix);
        fn_8040B52C(&s_matrix, m_offset->x, m_offset->y, 0.0f);
        fn_8040BAC4(&s_matrix, m_matrix);
        if (m_weapon != 0) {
            fn_801AB710(m_weapon, &s_matrix);
            m_weapon->vf_0x0C(dt);
        }
    }
    switch (m_state) {
    case 0:
        if (m_firing) {
            m_timer = 0.5f;
            m_state = 1;
        }
        break;
    case 1:
        if (!m_firing) {
            m_state = 0;
        }
        if ((m_timer -= dt) < 0.0f) {
            m_timer = 0.0f;
            m_state = 2;
        }
        break;
    case 2:
        if (!m_firing) {
            m_state = 0;
        }
        if ((m_timer -= dt) < 0.0f) {
            m_timer = m_offset->z;
            if (m_weapon != 0) {
                m_weapon->Fire();
            } else {
                UnkEffectParam_80194C1C param(fn_800496F8(&s_matrix), lbl_805E07A0, m_bombScale, false);
                SetVector(param.m_position, fn_800496F8(&s_matrix));
                fn_80209B58(fn_801EB304(fn_8001C4F8(), 0xE), 0, param);
                fn_801D54F4(fn_80053164(), 0x6037, &param.m_position, 0, 1);
            }
        }
        break;
    }
}

GunBeetleBase::GunBeetleBase() : FlyerCommonBase(1), m_gun(0) {
    m_enemy = this;
    vfPara_0x40(reinterpret_cast<int>(&lbl_805F5470));
    m_status->vfStat_0x80(false);
    m_sparkState = 0;
    m_sparkTimer = 0.0f;
}

GunBeetleBase::~GunBeetleBase() {
    if (m_gun != 0) {
        delete m_gun;
        m_gun = 0;
    }
    boost::shared_ptr<EffectElement> spark(m_spark.lock());
    if (spark) {
        spark->Retire();
    }
    if (m_sound.m_id != 0) {
        m_sound.fn_801D455C();
    }
    if (m_sparkSound.m_id != 0) {
        m_sparkSound.fn_801D455C();
    }
}

void GunBeetleBase::vfStat_0x70(AttackCallbackParam &param) {
    if (m_sparkState == 2 && !fn_80053538(&param) && !(param.m_attacker.lock()->m_flags & 0x10)) {
        return;
    }
    EnemyStatusCommon::vfStat_0x70(param);
}

int GunBeetleBase::vfStat_0xB8() {
    int result = 0x52;
    if (m_params.BodyType == 0) {
        result = 0x51;
    }
    return result;
}

void GunBeetleBase::vfStat_0xA0() {
    EnemyStatusCommon::vfStat_0xA0();
    if (m_gun != 0) {
        if (lbl_804C9CAC[m_params.WeaponType] != 0) {
            Vec3f offset;
            fn_800091EC(&offset, &(const Vec3f &)fn_8040D0AC(&(const Vec3f &)Vec3f(0.0f, 0.0f, 5.0f),
                                                            GetRotation()->y));
            m_gun->Drop(offset);
        }
        delete m_gun;
        m_gun = 0;
    }
}

void GunBeetleBase::vfDisp_0x64() {
    void *loader = fn_800122D4();
    int slot = m_motionBase;
    void *model = fn_80012684(loader, slot, 9);
    void *motion = fn_80012608(loader, slot, 0xC);
    m_render = fn_802CDB08(slot, MotionTable(&lbl_804C9CE8[m_params.BodyType * 2], 2), model, motion);
}

void GunBeetleBase::vfDisp_0x08() {
    EnemyDispCommon::vfDisp_0x08();
    if (m_params.WeaponType != 4) {
        m_gun = GunBeetleGun::Create(fn_8017D014(this), m_params.WeaponType);
        if (m_params.WeaponType == 2) {
            m_gun->m_bombScale = m_params.SparkDischarge;
        }
    }
}

/* EnemyDispFunc's default (Game/Enemy.h): retail has its out-of-line copy here, where the call in
 * vfDisp_0x10 first needs it. */
Mtx34f *EnemyDispFunc::GetPartMatrix(int index) {
    return 0;
}

float GunBeetleBase::GetFieldEC() {
    return field_0xEC;
}

void GunBeetleBase::vfDisp_0x10(float dt) {
    EnemyDispCommon::vfDisp_0x10(dt);
    if (m_gun != 0) {
        m_gun->m_matrix = m_enemy->m_disp->GetPartMatrix(3000);
        m_gun->SetValue(GetFieldEC());
        m_gun->Update(dt);
    }
    if (m_sound.m_id == 0) {
        m_sound = fn_8019A908(this, 0x6036, 1);
    }
    if (m_params.ActionType == 1) {
        switch (m_sparkState) {
        case 0:
            m_sparkTimer = m_params.SparkWait;
            m_sparkState = 1;
        case 1:
            if ((m_sparkTimer -= dt) < 0.0f) {
                m_sparkState = 2;
                m_sparkTimer = m_params.SparkDischarge;
                CallStatusSlot84(m_enemy->m_status, 4);
                Mtx34f *matrix = GetPartMatrix(1000);
                m_spark = fn_801A1E48(matrix, 0x7E);
                if (boost::shared_ptr<EffectElement> spark = m_spark.lock()) {
                    spark->ClearFlag4();
                }
                if (m_sparkSound.m_id == 0) {
                    m_sparkSound = fn_8019A908(this, 0x603A, 1);
                }
            }
            break;
        case 2:
            if ((m_sparkTimer -= dt) < 0.0f) {
                m_sparkState = 1;
                m_sparkTimer = m_params.SparkWait;
                CallStatusSlot84(m_enemy->m_status, 1);
                if (boost::shared_ptr<EffectElement> spark = m_spark.lock()) {
                    spark->SetMatrix(0);
                    spark->Stop();
                }
                m_spark.reset();
                if (m_sparkSound.m_id != 0) {
                    m_sparkSound.fn_801D455C();
                }
            }
            break;
        }
    }
}

void GunBeetleBase::vfSet_0x1C(UnkPlacement_800C9CD0 *placement) {
    int bodyType = m_params.BodyType;
    vfSet_0x20(placement);
    /* Copied as a plain struct (word loads and stores), unlike Vec3f's own copy. */
    Vec3fWords rotation = *reinterpret_cast<Vec3fWords *>(GetRotation());
    vfDisp_0x38(GetPosition(), reinterpret_cast<Vec3f *>(&rotation));
    m_ctrl->vfCtrl_0x20();
    if (m_gun != 0) {
        delete m_gun;
        m_gun = 0;
    }
    if (m_params.WeaponType != 4) {
        m_gun = GunBeetleGun::Create(fn_8017D014(this), m_params.WeaponType);
        if (m_params.WeaponType == 2) {
            m_gun->m_bombScale = m_params.SparkDischarge;
        }
    }
    if (m_params.BodyType != bodyType) {
        vfDisp_0x64();
    }
}

void GunBeetleBase::vfSet_0x20(UnkPlacement_800C9CD0 *placement) {
    TEnemySetBase::vfSet_0x20(placement);
    fn_800C9DBC(placement, &m_params, sizeof(m_params));
    fn_80193790(this, &m_params);
}

void GunBeetleBase::vfStat_0x38() {
    FlyerCommonBase::vfStat_0x38();
    if (!vfStat_0x34()) {
        if (m_gun != 0) {
            m_gun->Hide();
        }
    } else if (m_gun != 0) {
        m_gun->Show();
    }
}

void GunBeetleBase::vf_0x244() {
    if (m_gun != 0) {
        m_gun->m_firing = 1;
    }
}

void GunBeetleBase::vf_0x248() {
    if (m_gun != 0) {
        m_gun->m_firing = 0;
    }
}

float GunBeetleBase::vfPara_0x6C() {
    if (m_params.BodyType == 0) {
        return 2.0f;
    }
    return 4.0f;
}

int GunBeetleBase::vfPara_0x48() {
    return 0;
}

int GunBeetleBase::vfPara_0x4C() {
    return vfPara_0x48();
}

int GunBeetleBase::vfPara_0x50() {
    return vfPara_0x48();
}

int GunBeetleBase::vfPara_0x54() {
    return 1;
}

int GunBeetleBase::vfPara_0x58() {
    return vfPara_0x48();
}

int GunBeetleBase::vfPara_0x5C() {
    return 2;
}

int GunBeetleBase::vfPara_0x60() {
    return 2;
}

extern "C" const char *fn_80193DF8(UnkLabelQuery_800CBC20 *query) {
    return "";
}

GunBeetle::GunBeetle(Task *parent, UnkPlacement_800C9CD0 *placement)
    : EnemyTemplate<GunBeetleBase, GunBeetleAI>(parent, placement) {
    m_enemy = this;
    m_name = s_taskName;
}

GunBeetle::~GunBeetle() {}

void GunBeetle::LoadFiles() {
    void *loader = fn_800122D4();
    fn_8001299C(loader, 8, new ResourceOneFile("enemy/GunBeetleData.one", false));
}

void GunBeetle::DropFiles() {
    fn_800127F8(fn_800122D4(), 8);
}

void GunBeetle::Instantiate(UnkPlacement_800C9CD0 *placement) {
    new GunBeetle(fn_801EB304(fn_8001C4F8(), 0xE), placement);
}

static GadgetParamDesc lbl_80535D58[] = {
    {4, "MoveRange", 0, 0, 0, 200.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchRange", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchAngle", 0, 0, 0, 0.0f, -360.0f, 360.0f, fn_800CBC20, 0},
    {4, "SearchWidth", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchHeight", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchHeightOffset", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "MoveSpeedRatio", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {1, "AppearType", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_80193DF8, 0},
    {1, "ActionType", 0, 0, 2, 0.0f, 0.0f, 0.0f, fn_80193DF8, 0},
    {1, "", 0, 0, 8, 0.0f, 0.0f, 0.0f, fn_80193DF8, 0},
    {4, "FloatWidth", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_80193DF8, 0},
    {4, "AttackStart", 0, 0, 0, 0.4f, 0.0f, 1.0f, fn_80193DF8, 0},
    {4, "AttackEnd", 0, 0, 0, 0.6f, 0.0f, 1.0f, fn_80193DF8, 0},
    {1, "PathMirror", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "BodyType", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "WeaponType", 0, 0, 4, 0.0f, 0.0f, 0.0f, fn_80193DF8, 0},
    {4, "SparkDischarge", 0, 0, 0, 2.0f, 0.0f, 65536.0f, fn_80193DF8, 0},
    {4, "SparkWait", 0, 0, 0, 2.0f, 0.0f, 65536.0f, fn_80193DF8, 0},
    {0},
};

GadgetClassDesc lbl_80536050 = {
    "GUN_BEETLE", GunBeetle::LoadFiles, GunBeetle::DropFiles,
    reinterpret_cast<void (*)(UnkPlacement_800C9CD0 *)>(GunBeetle::Instantiate), 0, 0, 0x65, 2, 0,
    5, lbl_80535D58,
};

const char *GunBeetle::s_taskName = "GunBeetle";

/* Whether ActionType is 2 (name inferred); retail has it ahead of Instantiate. */
bool GunBeetleBase::vf_0x23C() {
    return m_params.ActionType == 2;
}

/* Not called anywhere: the linker strips it. Retail pools the unit's float constants in the order
 * below; this function, defined last and so emitted first, makes the compiler pool them that way
 * (see docs/research-notes.md; the original reason is unknown, and the name is a placeholder). */
static float UnkConstantUser(float x) {
    x *= 0.0f;
    x *= 0.5f;
    x *= 2.0f;
    x *= 4.0f;
    x *= 5.0f;
    return x;
}
