/* ArkCannon (class name from RTTI): the ARKCANNON gadget, a cannon turret (ARKCANNON.BIN) on the
 * ground or in the air ("0:GRAND 1:AIR", sic). It waits until the nearest character comes within
 * "RANGE m" (tenths of the stored range), turns towards it at 45 degrees a second, then fires four
 * homing shots from its muzzles, half a second apart, and rests for five seconds. Attacks wear its
 * health down; once destroyed it rewards the attacker, breaks into its debris and finishes. It
 * keeps a Target at its centre and two more around it for other objects to aim at. File name
 * inferred from the class. Method names marked "(name inferred)" are not original. Definitions are
 * in reverse address order (deferred inlining emits functions last-defined first).
 *
 * Not yet matching (36 of 38 functions and every data section match):
 * - Aim: retail stores the angle straight from the double that fn_8040F8FC returns (stfs f1) and
 *   rounds a second copy with frsp for the comparison; GC/2.7 rounds once and stores the rounded
 *   value, which shifts three float registers. No source shape tried changes that (see the float
 *   conversion notes in docs/research-notes.md).
 * - Fire: every instruction and stack offset matches, but GC/2.7 gives the frame 0x10 more bytes
 *   (0xD0, retail 0xC0) whenever both destructible temporaries (the weak_ptr<Target> passed to
 *   SetTarget and the weak_ptr<CharInfo> passed to the shot's constructor) are present. */
#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/DebugDraw.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/Player.h"
#include "Game/MathConstants.h"
#include "Game/UnkTargetList_80224850.h"
#include "Game/WeaponLockonTarget.h"
#include "Game/FileQueue.h"
#include "boost/bind.hpp"
#include "boost/function.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;

namespace boost {
/* boost 1.32's static_pointer_cast, which the trimmed shared_ptr.hpp leaves out; the game's copy
 * (0x80095388) is the one linked. */
template <class T, class U> shared_ptr<T> static_pointer_cast(shared_ptr<U> const &r);
} // namespace boost

/* Target (class name from RTTI): a point other objects can aim at (see Spring.cpp). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);
};

namespace Player {
/* PlayerTargetBase (class name from RTTI): the target a player character carries. */
class PlayerTargetBase;
} // namespace Player

/* The target a player character carries (offset from this gadget's use; name inferred). */
inline Player::PlayerTargetBase *GetPlayerTarget(Player::PlayerBase *player) {
    return *reinterpret_cast<Player::PlayerTargetBase **>(reinterpret_cast<u8 *>(player) + 0x24C);
}

/* The parameters of a debris spawn (layout as in CaptureCage.cpp). */
struct UnkDebrisParam_8015F0B8 {
    UnkDebrisParam_8015F0B8(const boost::shared_ptr<CharInfo> &info, int index);

    /* 0x00 */ u8 field_0x0[0x34];
    /* 0x34 */ float scale;
    /* 0x38 */ u8 field_0x38[0x40 - 0x38];
    /* 0x40 */ float field_0x40;
    /* 0x44 */ s32 model;
    /* 0x48 */ u8 field_0x48[0x4A - 0x48];
    /* 0x4A */ u8 field_0x4A;
    /* 0x4B */ u8 field_0x4B;
    /* 0x4C */ s32 field_0x4C;
    /* 0x50 */ u8 field_0x50[0x54 - 0x50];
    /* 0x54 */ u8 field_0x54;
    /* 0x55 */ u8 field_0x55[0x5C - 0x55];
};
SIZE_ASSERT(UnkDebrisParam_8015F0B8, 0x5C);

/* The launch parameters of a shot (class name unknown; layout from its constructor at 0x80051D10
 * and destructor at 0x8005C410; names inferred). */
struct UnkBulletParam_80051D10;

extern "C" {
UnkBulletParam_80051D10 *fn_80051D10(UnkBulletParam_80051D10 *param,
                                     const boost::weak_ptr<CharInfo> &owner,
                                     const boost::shared_ptr<Weapon::LockonTarget> &lockon, int arg3,
                                     const Mtx34f *muzzle, const Vec3f *velocity);
UnkBulletParam_80051D10 *fn_8005C410(UnkBulletParam_80051D10 *param, short deleteFlag);
}

struct UnkBulletParam_80051D10 {
    UnkBulletParam_80051D10() {}
    UnkBulletParam_80051D10(const boost::weak_ptr<CharInfo> &owner,
                            const boost::shared_ptr<Weapon::LockonTarget> &lockon, int arg3,
                            const Mtx34f *muzzle, const Vec3f *velocity) {
        fn_80051D10(this, owner, lockon, arg3, muzzle, velocity);
    }
    ~UnkBulletParam_80051D10() {
        fn_8005C410(this, -1);
    }

    /* The constructor and destructor are the out-of-line functions above, so the members (a
     * weak_ptr<CharInfo> owner, a shared_ptr<Weapon::LockonTarget>, the position, direction and
     * velocity at 0x10, 0x1C and 0x28, and the integer argument at 0x34) are left as bytes. */
    /* 0x00 */ u8 field_0x0[0x4C];
};
SIZE_ASSERT(UnkBulletParam_80051D10, 0x4C);

/* A kind of shot (struct name unknown; the records lie in .data around 0x8051DD14): the function at
 * 0x8 fires one (names inferred). */
struct UnkBulletType_8051DDC8 {
    /* 0x00 */ u32 field_0x0;
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ void (*fire)(UnkBulletParam_80051D10 *param);
    /* 0x0C */ u32 field_0xC;
    /* 0x10 */ float field_0x10;
};
/* the cannon shot */
extern UnkBulletType_8051DDC8 lbl_8051DDC8;

/* fn_8040CD74 called as returning a - b by value (it stores the result through its first
 * argument). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

extern "C" {
/* GadgetModelInfo methods that are not declared in its header yet (see BreakObj.cpp, SetWeapon.cpp
 * and Switch.cpp). */
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int variant);
void fn_80161A24(GadgetModelInfo *model, int arg1);
void fn_80162790(GadgetModelInfo *model);
/* Finds the matrix of node id of a model (behaviour inferred; arg2 unknown). */
bool fn_80169EF0(Mtx34f *matrix, boost::shared_ptr<GadgetModelInfo> model, int arg2, int id);
/* Sets callback as the attack callback of an attack collision (see BreakRoad.cpp). */
void fn_80096B64(CharaColliAttack *attack, int arg1, const AttackCallback &callback);

/* Builds the character info of a gadget type and loads a gadget model (see GadgetUtil.h). */
boost::shared_ptr<GadgetCharInfo> fn_8015CD90(int arg0, int arg1);
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* Creates a Target for owner (behaviour inferred; the flags are unknown). */
boost::shared_ptr<Target> fn_801E7128(const boost::weak_ptr<CharInfo> &owner, int flags);
/* The attacker of an attack (behaviour inferred). */
boost::shared_ptr<CharInfo> fn_8015D570(AttackCallbackParam &param);
/* Gives the attacker the reward for breaking the object (behaviour inferred). */
void fn_8020642C(const boost::shared_ptr<CharInfo> &attacker, int reward);
/* A test on a timer or the health (behaviour unknown; see FootingMovable.cpp). */
bool fn_8000A950(float value);
/* The target of a player character's target object (behaviour inferred). */
boost::weak_ptr<Target> fn_80206574(Player::PlayerTargetBase *target);

/* A 32-bit flag word: fn_8000AAEC constructs one from a value, fn_80014E1C sets bit, fn_8006127C
 * clears bit and fn_80014E34 tests bit (see FireRing.cpp and Elevator.cpp). */
u32 *fn_8000AAEC(u32 *flags, u32 value);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* Debris (see GadgetDebris.h) and the model loader (see BreakObj.cpp). */
void fn_8015D93C(UnkDebrisParam_8015F0B8 *param);
void *fn_800122D4(void);
s32 fn_80012700(void *manager, int arg1, const char *fileName);
/* Tells the owner that the gadget is gone (behaviour inferred). */
void fn_800C9B04(UnkPlacement_800C9CD0 *owner);

/* Vector and matrix helpers (behaviour inferred from their bodies). */
float fn_8040CE74(Vec3f *vector); /* normalises; returns the former length */
/* Splits vector into its parts along and across normal (either output may be null). */
void fn_8040F990(Vec3f *along, Vec3f *across, const Vec3f *vector,
                 const Vec3f *normal);
/* The angle between two unit vectors. */
double fn_8040F8FC(const Vec3f *a, const Vec3f *b);
/* The smaller of two values. */
const float &fn_800704AC(const float &a, const float &b);
/* a x b. */
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b);
/* The rotation by angle about axis. */
Mtx34f fn_80049778(const Vec3f *axis, float angle);
/* a * b. */
Mtx34f fn_8001E044(const Mtx34f *a, const Mtx34f *b);
/* The Euler angles of a rotation matrix. */
void fn_800494F8(Vec3f *rotation, const Mtx34f *matrix);
/* vector turned by angle about the y axis. */
Vec3f fn_8040D044(const Vec3f *vector, float angle);
/* matrix * point. */
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
}

/* vector = value through the out-of-line assignment (see Game/GadgetUtil.h). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* The flag word at 0x68 (struct name unknown). */
struct ArkCannonFlags {
    ArkCannonFlags(u32 value) {
        fn_8000AAEC(&m_bits, value);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    void Clear(int bit) {
        fn_8006127C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits; /* bit 0 hit this frame, bit 1 destroyed */
};

/* The parameters of the owner record. */
struct ArkCannonParams {
    /* 0x0 */ u32 type; /* 0 ground, 1 air */
    /* 0x4 */ float range;
};

class ArkCannon : public Gadget {
  public:
    ArkCannon(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~ArkCannon();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's placement and parameters and resets the state (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Shows the model of the type and registers OnAttack with its collision (name inferred). */
    void SetupModel();
    /* Moves the model and the targets to the character info's place (name inferred). */
    void UpdateTargets();
    /* Whether the nearest character is within range (name inferred). */
    bool IsEnemyInRange();
    /* Turns towards enemy; true once it points at it (name inferred). */
    bool Aim(boost::shared_ptr<CharInfo> enemy, float dt);
    /* Fires a shot at enemy from muzzle index (name inferred). */
    void Fire(u32 index, boost::shared_ptr<CharInfo> enemy);
    /* Enters state (name inferred). */
    void SetState(int state);
    /* The attack callback of the model's collision (name inferred). */
    void OnAttack(AttackCallbackParam &param);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ ArkCannonParams m_params;
    /* 0x38 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x40 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x48 */ boost::shared_ptr<Target> m_target;
    /* 0x50 */ UnkTargetList_80224850 m_sideTargets;
    /* 0x5C */ boost::weak_ptr<CharInfo> m_enemy;
    /* 0x64 */ s32 m_state; /* 0 waiting, 1 aiming, 2 firing, 3 resting, 4 destroyed */
    /* 0x68 */ ArkCannonFlags m_hitFlags;
    /* 0x6C */ float m_health;
    /* 0x70 */ float m_restTime;
    /* 0x74 */ float m_fireTime;
    /* 0x78 */ u32 m_shotCount;
    /* 0x7C */ u32 pad_0x7C; /* the class size is a multiple of 8 */
};
SIZE_ASSERT(ArkCannon, 0x80);

/* The debris model of each type. */
static const char *lbl_805E9AD8[] = {"A10000ARKCANNON_BRK.DFF", "A10000ARKCANNONAIR_BRK.DFF"};

/* The model node of each muzzle. */
static const int lbl_804F164C[] = {2, 4, 5, 3};

const char *ArkCannon::s_taskName = "ArkCannon";

/* Converts the parameters' metres to world units (name inferred). */
inline float MetresToUnits(float metres) {
    return 10.0f * metres;
}

/* The number of side targets. */
static const int kSideTargetCount = 2;

/* The distance of the side targets from the centre. A constant with a dynamic initializer, so it
 * lives in .sbss and the static initializer (0x802280F8) stores it. */
static const float lbl_805EFE04 = MetresToUnits(0.75f);

ArkCannon::ArkCannon(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner), m_state(0), m_hitFlags(0) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x64, 4);
    ReadPlacement(owner);
    m_target = fn_801E7128(m_chara, 0xA400);
    m_sideTargets.reserve(kSideTargetCount);
    for (u32 i = 0; i < kSideTargetCount; i++) {
        m_sideTargets.push_back(fn_801E7128(m_chara, 0x8800));
    }
    m_model = fn_8015F800(1, "ARKCANNON.BIN");
    SetupModel();
    UpdateTargets();
}

ArkCannon::~ArkCannon() {}

extern "C" void fn_80227DB8(void) {
    fn_8006B474("ARKCANNON.BIN", 1);
}

extern "C" void fn_80227DB4(void) {}

void ArkCannon::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new ArkCannon(fn_801EB304(fn_8001C4F8(), 11), owner);
}

void ArkCannon::Behave(float dt) {
    if (m_hitFlags.Test(1)) {
        fn_80161A24(m_model.get(), m_params.type);
        fn_80162790(m_model.get());
        if (m_params.type == 0) {
            UnkDebrisParam_8015F0B8 debris(m_chara, 0);

            debris.model = fn_80012700(fn_800122D4(), 1, lbl_805E9AD8[m_params.type]);
            debris.field_0x40 = m_chara->m_position.y;
            debris.scale = 1.0f;
            debris.field_0x4A = 1;
            debris.field_0x54 = 1;
            fn_8015D93C(&debris);
        } else if (m_params.type == 1) {
            for (u32 i = 0; i <= m_params.type; i++) {
                UnkDebrisParam_8015F0B8 debris(m_chara, 0);

                debris.model = fn_80012700(fn_800122D4(), 1, lbl_805E9AD8[i]);
                debris.field_0x40 = m_chara->m_position.y;
                debris.scale = 1.0f;
                debris.field_0x4A = 0;
                debris.field_0x54 = 1;
                fn_8015D93C(&debris);
            }
        }
        m_state = 4;
        m_hitFlags.Clear(1);
        fn_800C9B04(m_owner);
        m_flags |= 1;
        return;
    }

    switch (m_state) {
    case 0:
        if (IsEnemyInRange()) {
            SetState(1);
        }
        break;
    case 1: {
        boost::shared_ptr<CharInfo> enemy = m_enemy.lock();
        if (enemy.get() != 0) {
            if (Aim(enemy, dt)) {
                SetState(2);
            }
        } else {
            SetState(3);
        }
        break;
    }
    case 2: {
        boost::shared_ptr<CharInfo> enemy = m_enemy.lock();
        if (enemy.get() != 0) {
            if (m_fireTime < 0.0f || fn_8000A950(m_fireTime)) {
                Fire(m_shotCount, enemy);
                m_shotCount++;
                if (m_shotCount >= 4) {
                    SetState(3);
                } else {
                    m_fireTime = 0.5f;
                }
            } else {
                m_fireTime -= dt;
            }
        } else {
            SetState(3);
        }
        break;
    }
    case 3:
        if (IsEnemyInRange()) {
            if (m_restTime < 0.0f || fn_8000A950(m_restTime)) {
                SetState(1);
            } else {
                m_restTime -= dt;
            }
        } else {
            SetState(0);
        }
        break;
    }
    m_model->fn_80162FB0(dt);
}

void ArkCannon::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ArkCannonParams params = m_params;

    ReadPlacement(owner);
    if (m_params.type != params.type) {
        m_model->fn_80162804();
        m_model->fn_80161880();
        SetupModel();
    }
    UpdateTargets();
    m_model->fn_80162EF4();
    m_model->fn_8016108C();
    fn_802E0E84(&m_chara->m_position, &lbl_805F06D8, m_params.range);
}

void ArkCannon::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_params.range = MetresToUnits(m_params.range);
    m_health = 16.0f;
    m_restTime = 0.0f;
    m_fireTime = 0.0f;
    m_shotCount = 0;
}

void ArkCannon::SetupModel() {
    m_model->fn_80162A68(m_params.type, 0);
    fn_80161FC8(m_model.get(), m_params.type, m_chara);
    boost::shared_ptr<CharaColliAttack> collision = fn_80161428(m_model.get(), m_params.type).lock();
    if (collision.get() != 0) {
        fn_80096B64(collision.get(), 1, boost::bind(&ArkCannon::OnAttack, this, _1));
    }
}

void ArkCannon::UpdateTargets() {
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_chara->m_position);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    fn_8040B884(&matrix, m_chara->m_rotation.x);
    fn_8040B80C(&matrix, m_chara->m_rotation.z);
    m_model->fn_801633DC(&matrix);
    m_target->fn_800CCE38(&m_chara->m_position);

    boost::shared_ptr<Target> *target = m_sideTargets.begin();
    u32 i = 0;
    float step = 360.0f * (lbl_805FAD38 / 180.0f) / kSideTargetCount;
    for (; target != m_sideTargets.end(); i++, target++) {
        Vec3f position(lbl_805EFE04, 0.0f, 0.0f);

        AssignVector(position, fn_8040D044(&position, step * i));
        AssignVector(position, fn_8040B67C(&matrix, &position));
        (*target)->fn_800CCE38(&position);
    }
}

bool ArkCannon::IsEnemyInRange() {
    boost::shared_ptr<CharInfo> enemy = fn_800A8A7C(fn_80074194(), &m_chara->m_position).lock();
    if (enemy.get() != 0) {
        Vec3f offset;

        fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                 &enemy->m_position, &m_chara->m_position));
        if (fn_8040CF50(&offset) < m_params.range) {
            return true;
        }
    }
    return false;
}

bool ArkCannon::Aim(boost::shared_ptr<CharInfo> enemy, float dt) {
    Vec3f up(0.0f, 1.0f, 0.0f);
    Vec3f front(0.0f, 0.0f, -1.0f);
    bool aimed = false;
    Vec3f direction;

    fn_800091EC(&direction, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                &enemy->m_position, &m_chara->m_position));
    fn_8040CE74(&direction);

    Vec3f ownerRotation(0.0f, 0.0f, 0.0f);
    fn_800C9ED4(m_owner, &ownerRotation);
    Mtx34f ownerMatrix;
    fn_8040C148(&ownerMatrix, &lbl_805E0720);
    fn_8040B848(&ownerMatrix, ownerRotation.y);
    fn_8040B884(&ownerMatrix, ownerRotation.x);
    fn_8040B80C(&ownerMatrix, ownerRotation.z);
    AssignVector(up, fn_8040B67C(&ownerMatrix, &up));

    Mtx34f matrix;
    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    fn_8040B884(&matrix, m_chara->m_rotation.x);
    fn_8040B80C(&matrix, m_chara->m_rotation.z);
    AssignVector(front, fn_8040B67C(&matrix, &front));

    fn_8040F990(0, &front, &front, &up);
    fn_8040F990(0, &direction, &direction, &up);
    fn_8040CE74(&front);
    fn_8040CE74(&direction);
    float angle = fn_8040F8FC(&front, &direction);
    float step = 45.0f * dt * (lbl_805FAD38 / 180.0f);
    if (angle < step) {
        aimed = true;
    }
    /* The cast keeps the store of step after the branch, as in retail. */
    angle = fn_800704AC(angle, (float)step);

    Vec3f axis;
    fn_800091EC(&axis, &(const Vec3f &)fn_8040CFB8(&front, &direction));
    fn_8040CE74(&axis);
    Mtx34f rotation;
    fn_8040C148(&rotation, &(const Mtx34f &)fn_80049778(&axis, angle));
    fn_8040C148(&matrix, &(const Mtx34f &)fn_8001E044(&rotation, &matrix));
    fn_800494F8(&m_chara->m_rotation, &matrix);
    UpdateTargets();
    return aimed;
}

void ArkCannon::Fire(u32 index, boost::shared_ptr<CharInfo> enemy) {
    Mtx34f muzzle;

    if (fn_80169EF0(&muzzle, m_model, 0, lbl_804F164C[index])) {
        boost::shared_ptr<Weapon::LockonTarget> lockon(new Weapon::LockonTarget());
        boost::shared_ptr<Player::PlayerBase> player =
            boost::static_pointer_cast<Player::PlayerBase>(enemy);
        if (GetPlayerTarget(player.get()) != 0) {
            lockon->SetTarget(fn_80206574(GetPlayerTarget(player.get())));
            lockon->fn_80060F70();
            UnkBulletParam_80051D10 param(m_chara, lockon, 1, &muzzle, &lbl_805E07A0);
            UnkBulletType_8051DDC8 *type = &lbl_8051DDC8;
            type->fire(&param);
        }
    }
}

void ArkCannon::SetState(int state) {
    switch (state) {
    case 1:
        m_enemy = fn_800A8A7C(fn_80074194(), &m_chara->m_position);
        break;
    case 2:
        m_fireTime = 0.0f;
        m_shotCount = 0;
        break;
    case 3:
        m_restTime = 5.0f;
        m_enemy.reset();
        break;
    }
    m_state = state;
}

void ArkCannon::OnAttack(AttackCallbackParam &param) {
    param.m_result = 1;
    if (m_state != 4) {
        m_health -= param.m_damage;
        m_hitFlags.Set(0);
        if (m_health < 0.0f || fn_8000A950(m_health)) {
            param.m_result = 3;
            boost::shared_ptr<CharInfo> attacker = fn_8015D570(param);
            if (attacker.get() != 0) {
                fn_8020642C(attacker, 0x4B);
            }
            m_hitFlags.Set(1);
        }
    }
}

static GadgetParamDesc lbl_80546624[] = {
    {2, "0:GRAND 1:AIR", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {4, "RANGE m", 0, 0, 0, 50.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_8054669C = {
    "ARKCANNON", fn_80227DB8, fn_80227DB4, ArkCannon::Instantiate, 0, 0, 0x138E, 1, 0, 10, lbl_80546624,
};
