/* Enemy (class name from RTTI; Game/Enemy.h): the base of every enemy, with the helpers the enemies
 * share (sounds, turning and moving toward a target, distances, random points near home), the
 * enemy type names, and the do-nothing interface objects an enemy's part pointers start with. Most
 * helpers keep their address names, as the enemy units call them as functions of their own. File
 * name inferred from the class. Names marked "(name inferred)" are not original. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */

#include "Game/CharInfo.h"
#include "Game/Enemy.h"
#include "Game/GadgetSound.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/Random.h"
#include "Game/ResourceOneFile.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

/* MSL's std::fabs for floats, which the game calls out of line (as in BkSoldier.cpp). */
namespace std {
float fabs(float x);
}

/* The degrees-to-radians conversion (see BAGunShip.cpp). */
float DegToRad(float degrees);

/* The attack flags the enemies' attack tables get (fn_8019A890; GunSoldier and EggPierrot read
 * the first word; name unknown). Retail has it as an 8-byte object: the second word is zero and
 * unreferenced. */
extern const u32 lbl_805F56D0[2] = {0x202, 0};

/* The enemy types' names, by type (fn_8019A8F4; name inferred). */
static const char *s_typeNames[16] = {
    "GUN_SOLDIER", "GUN_BEETLE", "GUN_BIGFOOT", "GUN_ROBOT",   "BK_GIANT",    "BK_SOLDIER",
    "BK_WINGLARGE", "BK_WINGSMALL", "BK_WORM", "BK_LARVA", "BK_CHAOS", "BK_NINJA",
    "EGG_PAWN",    "EGG_PIERROT", "EGG_SHADOWANDROID", 0,
};

/* The do-nothing parts an enemy's part pointers start with (Game/Enemy.h). */
EnemyParaFunc lbl_805EF6E8;
EnemyStatFunc lbl_805EF6EC;
EnemyDispFunc lbl_80580F78;
EnemyMoveFunc lbl_805EF6F0;
EnemyCtrlFunc lbl_80580F88;
EnemySetFunc lbl_805EF6F4;

/* An entry of a model's attack table (layout as in BkWingLarge.cpp). */
struct UnkAttackEntry_8001277C {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ u32 m_id; /* zero ends the table */
    /* 0x8 */ u32 m_flags;
    /* 0xC */ u32 field_0xC;
};

extern "C" {
/* A model's attack table (see BkWingLarge.cpp). */
UnkAttackEntry_8001277C *fn_8001277C(void *loader, s32 slot, int kind);
/* Sounds: a sound at a character info and one without a position (see GadgetSound.h). */
UnkSoundHandle_801D3DC8 fn_801D3C38(u16 sound, CharInfo *charInfo, void *owner);
UnkSoundHandle_801D3DC8 fn_801D3EF0(u16 sound, void *owner);
void *fn_80053164(void);
void fn_801D54F4(void *manager, int sound, CharInfo *charInfo, int arg3, int arg4);
void fn_801D560C(void *manager, int sound, int arg2, int arg3);
/* The enemy's area radius, and a position of the enemy (weak getters of other units; names
 * unknown). */
float fn_8018346C(Enemy *enemy);
Vec3f *fn_80181F0C(Enemy *enemy);
/* Whether the area contains point (behaviour inferred). */
bool fn_801A6944(UnkEnemyArea_8018A374 *area, const Vec3f *point);
/* Angles: wrapped into [-pi, pi], the angle of (x, z) and a direction's pitch and yaw (behaviour
 * inferred). */
float fn_8040E99C(float angle);
float fn_80016260(float x, float z);
Vec2f fn_8040F760(const Vec3f *direction);
/* Vector helpers (see AdjustCosRot.cpp and BlackBullCtrlUnit.cpp). */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040D0AC(const Vec3f *v, float angle);
Vec3f fn_8040CEE8(const Vec3f *v);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
float fn_8040CE74(Vec3f *v);
void fn_8040CE38(Vec3f *vector, float scale);
void fn_8040CE04(Vec3f *vector, const Vec3f *value);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
float fn_8040CF90(const Vec3f *a, const Vec3f *b);
}

/* fn_8040CD74 as a function that returns its result (see Game/EnemyRender.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *lhs, const Vec3f *rhs);

/* Turns current toward target by at most step (Game/AngleUtil.h; the linked copy is UpDownReel's,
 * declared here so that pi stays const, see Game/MathConstants.h). */
float TurnAngleToward(float target, float current, float step);

/* The vector type's out-of-line copy and assignment, called with a temporary the way the
 * original's operators are (names inferred; see Game/GadgetUtil.h). */
inline void ConstructVector(Vec3f *vector, const Vec3f &value) {
    fn_800091EC(vector, &value);
}

inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* degrees in radians (name inferred). */
inline float Radians(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* The distance from the enemy to point on the ground plane (name inferred). */
extern "C" float fn_8019B298(Enemy *enemy, const Vec3f *point) {
    Vec3f offset;
    ConstructVector(&offset, *enemy->GetPosition());
    fn_8040CE04(&offset, point);
    offset.y = 0.0f;
    return fn_8040CF50(&offset);
}

/* The distance from the enemy to point (name inferred). */
extern "C" float fn_8019B24C(Enemy *enemy, const Vec3f *point) {
    Vec3f offset;
    ConstructVector(&offset, *enemy->GetPosition());
    fn_8040CE04(&offset, point);
    return fn_8040CF50(&offset);
}

/* The squared distance from the enemy to point (name inferred). */
extern "C" float fn_8019B200(Enemy *enemy, const Vec3f *point) {
    Vec3f offset;
    ConstructVector(&offset, *enemy->GetPosition());
    fn_8040CE04(&offset, point);
    return fn_8040CF2C(&offset);
}

/* The yaw of direction (name inferred). */
extern "C" float fn_8019B1D8(Enemy *enemy, const Vec3f *direction) {
    return fn_80016260(direction->x, direction->z);
}

/* How far the enemy has to turn to face direction (name inferred). */
extern "C" float fn_8019B184(Enemy *enemy, const Vec3f *direction) {
    float yaw = fn_8019B1D8(enemy, direction);
    return fn_8040E99C(enemy->GetRotation()->y - yaw);
}

/* Sets the enemy's facing (field_0x84) and its forward vector (field_0x78) from rotation (name
 * inferred). */
extern "C" void fn_8019B0F8(Enemy *enemy, const Vec3f *rotation) {
    Vec3f forward(0.0f, 0.0f, -1.0f);
    Mtx34f matrix;
    fn_8001E070(&matrix, rotation, 2);
    AssignVector(forward, fn_8040B67C(&matrix, &forward));
    fn_800091D0(&enemy->field_0x78, &forward);
    enemy->field_0x84 = *rotation;
}

/* Turns the enemy's facing (field_0x84) toward direction (name inferred). */
extern "C" void fn_8019B0B0(Enemy *enemy, const Vec3f *direction) {
    Vec2f angles = fn_8040F760(direction);
    enemy->field_0x84.x = angles.x;
    enemy->field_0x84.y = angles.y;
    enemy->field_0x84.z = 0.0f;
}

/* A random point within range of the enemy's home, at most its area radius away (name inferred). */
extern "C" void fn_8019B000(Enemy *enemy, Vec3f *point, float range) {
    float radius = fn_8018346C(enemy);
    if (0.0f < range && range < radius) {
        radius = range;
    }
    Vec3f offset(0.0f, 0.0f, fn_8040EC80(0.0f, radius));
    fn_800091D0(point, &enemy->field_0x48);
    fn_8040CFF8(point, &(const Vec3f &)fn_8040D0AC(&offset, fn_8040EC80(0.0f, DegToRad(360.0f))));
}

/* The character in reach of the enemy (field_0x40, the nearest character); true when it is in
 * the enemy's area (behaviour inferred). */
inline bool InArea(UnkEnemyArea_8018A374 *area, void *key) {
    return area->Contains(key);
}

extern "C" bool fn_8019AF04(Enemy *enemy) {
    Vec3f *position = enemy->GetPosition();
    boost::weak_ptr<CharInfo> target = fn_800A8A7C(fn_80074194(), position);
    boost::shared_ptr<CharInfo> chara(target.lock());
    if (chara.get() != 0 && InArea(&enemy->field_0xA8, chara.operator->())) {
        enemy->field_0x40 = target;
        return true;
    }
    enemy->field_0x40 = target;
    return false;
}

/* The distance from the enemy to its target point on the ground plane (name inferred). */
extern "C" float fn_8019AEE0(Enemy *enemy) {
    return fn_8019B298(enemy, &enemy->field_0x6C);
}

/* Moves the enemy toward its target point (field_0x6C), once it faces it; true once it is there
 * (behaviour inferred). */
bool Enemy::vf_0x0C() {
    if (fn_8019AEE0(this) < 10.0f) {
        return true;
    }
    Vec3f direction;
    ConstructVector(&direction, ((VectorSubtractFn)fn_8040CD74)(&field_0x6C, GetPosition()));
    direction.y = 0.0f;
    fn_8019B0B0(this, &direction);
    if (vf_0x10()) {
        float step = m_para->vfPara_0x20() * GetFrameTime();
        float length = fn_8040CF50(&direction);
        if (step > length) {
            step = length;
        }
        fn_8040CE74(&direction);
        fn_8040CE38(&direction, step);
        fn_8040CFF8(fn_80181F0C(this), &direction);
        m_disp->vfDisp_0x3C(true);
    } else if (direction.x * direction.x + direction.y * direction.y +
                   direction.z * direction.z <
               0.0001f) {
        m_disp->vfDisp_0x3C(false);
    } else {
        float cosine = fn_8040CF90(&(const Vec3f &)fn_8040CEE8(&direction),
                                   &(const Vec3f &)fn_8040D0AC(&(const Vec3f &)Vec3f(0.0f, 0.0f, -1.0f),
                                                               GetRotation()->y));
        if (cosine < 0.0f) {
            cosine = 0.2f;
        }
        m_disp->vfDisp_0x30(cosine);
    }
    return false;
}

float EnemyParaFunc::vfPara_0x34() {
    return 0.0f;
}

/* Turns the enemy toward its facing (field_0x84.y) by its turn speed; true once it faces it
 * (behaviour inferred). */
bool Enemy::vf_0x10() {
    float step = m_para->vfPara_0x34() * GetFrameTime();
    float half = std::fabs(0.5f * fn_8040E99C(field_0x84.y - GetRotation()->y));
    if (step < half) {
        half = step;
    }
    step = Radians(1.0f);
    if (half > step) {
        step = half;
    }
    GetRotation()->y = TurnAngleToward(field_0x84.y, GetRotation()->y, step);
    if (std::fabs(field_0x84.y - GetRotation()->y) < Radians(1.0f)) {
        return true;
    }
    return false;
}

/* A point dist ahead of the enemy toward position, kept in the enemy's area (behaviour
 * inferred). */
extern "C" Fn8040CopyVector3 fn_8019AADC(Enemy *enemy, const Vec3f *position, float dist) {
    Vec3f point(0.0f, 0.0f, fn_8040EC80(0.5f * dist, dist));
    AssignVector(point, fn_8040CDD0(position, &(const Vec3f &)fn_8040D0AC(
                                                  &point, fn_8040EC80(0.0f, 360.0f * (lbl_805FAD38 / 180.0f)))));
    if (!fn_801A6944(&enemy->field_0xA8, &point)) {
        Vec3f offset;
        ConstructVector(&offset, ((VectorSubtractFn)fn_8040CD74)(&point, &enemy->field_0x48));
        offset.y = 0.0f;
        fn_8040CE74(&offset);
        fn_8040CE38(&offset, fn_8018346C(enemy) - 1.0f);
        point.x = offset.x + enemy->field_0x48.x;
        point.z = offset.z + enemy->field_0x48.z;
    }
    return *reinterpret_cast<Fn8040CopyVector3 *>(&point);
}

/* Sets field_0xEC (name inferred). */
extern "C" void fn_8019AAD4(Enemy *enemy, float value) {
    enemy->field_0xEC = value;
}

/* Sets field_0xF0, and bit of the flags at field_0xF4 (all of them cleared for a negative bit;
 * names inferred). */
extern "C" void fn_8019AAA4(Enemy *enemy, int bit, float value) {
    enemy->field_0xF0 = value;
    if (bit >= 0) {
        enemy->field_0xF4 |= 1 << bit;
    } else {
        enemy->field_0xF4 = 0;
    }
}

/* Plays sound at the enemy, or without a position (names inferred; two copies of one body). */
extern "C" void fn_8019AA40(Enemy *enemy, u16 sound, bool positional) {
    if (positional) {
        fn_801D54F4(fn_80053164(), sound, enemy->m_chara.get(), 0, 1);
    } else {
        fn_801D560C(fn_80053164(), sound, 0, 0);
    }
}

extern "C" void fn_8019A9DC(Enemy *enemy, u16 sound, bool positional) {
    if (positional) {
        fn_801D54F4(fn_80053164(), sound, enemy->m_chara.get(), 0, 1);
    } else {
        fn_801D560C(fn_80053164(), sound, 0, 0);
    }
}

/* Plays sound as fn_8019AA40, at most once a frame (name inferred). */
extern "C" void fn_8019A954(Enemy *enemy, u16 sound, bool positional) {
    if (enemy->field_0xFC != *reinterpret_cast<u32 *>(&lbl_8057D6A0[0x90]) - enemy->field_0x2C) {
        enemy->field_0xFC = *reinterpret_cast<u32 *>(&lbl_8057D6A0[0x90]) - enemy->field_0x2C;
        if (positional) {
            fn_801D54F4(fn_80053164(), sound, enemy->m_chara.get(), 0, 1);
        } else {
            fn_801D560C(fn_80053164(), sound, 0, 0);
        }
    }
}

/* Starts sound at the enemy, or without a position, and returns it (name inferred). */
extern "C" UnkSoundHandle_801D3DC8 fn_8019A908(Enemy *enemy, u16 sound, bool positional) {
    if (positional) {
        return fn_801D3C38(sound, enemy->m_chara.get(), enemy);
    }
    return fn_801D3EF0(sound, enemy);
}

/* The name of enemy type type (Game/Enemy.h declares it returning an int). */
extern "C" int fn_8019A8F4(int type) {
    return reinterpret_cast<int>(s_typeNames[type]);
}

/* A model's attack table, with the enemies' attack flags set on every entry (name inferred). */
extern "C" void *fn_8019A890(s32 slot, int kind) {
    UnkAttackEntry_8001277C *table = fn_8001277C(fn_800122D4(), slot, kind);
    if (table != 0) {
        for (UnkAttackEntry_8001277C *entry = table; entry->m_id != 0; entry++) {
            entry->m_flags = 0x202;
        }
    }
    return table;
}

/* The interfaces' defaults (Game/Enemy.h): inline in the original, first emitted here, after the
 * static initializer. */
inline bool EnemySetFunc::vfSet_0x0C() {
    return 0;
}
inline void EnemySetFunc::vfSet_0x14() {}
inline void EnemySetFunc::vfSet_0x1C(UnkPlacement_800C9CD0 *placement) {}
inline void EnemySetFunc::vfSet_0x20(UnkPlacement_800C9CD0 *placement) {}
inline void EnemySetFunc::vfSet_0x24() {}
inline bool EnemySetFunc::vfSet_0x28() {
    return 0;
}

inline void EnemyDispFunc::vfDisp_0x08() {}
inline void EnemyDispFunc::vfDisp_0x0C() {}
inline void EnemyDispFunc::vfDisp_0x10(float dt) {}
inline int EnemyDispFunc::vfDisp_0x14() {
    return 0;
}
inline int EnemyDispFunc::vfDisp_0x18() {
    return 0;
}
inline Fn8040CopyVector3 EnemyDispFunc::vfDisp_0x28() {
    return *reinterpret_cast<Fn8040CopyVector3 *>(&field_0x0);
}
inline Fn8040CopyVector3 EnemyDispFunc::vfDisp_0x2C() {
    return *reinterpret_cast<Fn8040CopyVector3 *>(&field_0x0);
}
inline float EnemyDispFunc::vfDisp_0x34() {
    return 1.0f;
}
inline void EnemyDispFunc::vfDisp_0x38(const Vec3f *position, const Vec3f *rotation) {}
inline void EnemyDispFunc::vfDisp_0x40(EnemyMotionEventArg &arg) {}
inline bool EnemyDispFunc::vfDisp_0x44(int *command) {
    return 0;
}
inline void EnemyDispFunc::vfDisp_0x48() {}
inline void EnemyDispFunc::vfDisp_0x54() {}
inline void EnemyDispFunc::vfDisp_0x58(bool value) {}

inline void EnemyCtrlFunc::vfCtrl_0x08() {}
inline void EnemyCtrlFunc::vfCtrl_0x0C() {}
inline void *EnemyCtrlFunc::vfCtrl_0x14() {
    return &field_0xC;
}
inline float EnemyCtrlFunc::vfCtrl_0x18() {
    return 0.0f;
}

/* Slot 0x24 is EnemyMoveFunc's too (0x8017B5A0; EnemyMoveCommon's vtable has the same function,
 * which Game/Enemy.h declares as EnemyMoveCommon's override): symbols.txt gives that address both
 * names. */
inline void EnemyMoveFunc::vfMove_0x08() {}
inline void EnemyMoveFunc::vfMove_0x0C(float dt) {}
inline bool EnemyMoveFunc::vfMove_0x14() {
    return 0;
}
inline int EnemyMoveFunc::vfMove_0x1C() {
    return 0;
}
inline int EnemyMoveFunc::vfMove_0x20() {
    return 0;
}
inline float EnemyMoveFunc::vfMove_0x28() {
    return 0.0f;
}
inline void EnemyMoveFunc::vfMove_0x34() {}

inline int EnemyParaFunc::vfPara_0x08() {
    return -1;
}
inline int EnemyParaFunc::vfPara_0x0C() {
    return 0;
}
inline int EnemyParaFunc::vfPara_0x10() {
    return 0;
}
inline int EnemyParaFunc::vfPara_0x14() {
    return 0;
}
inline int EnemyParaFunc::vfPara_0x18() {
    return 0;
}
inline int EnemyParaFunc::vfPara_0x1C() {
    return 0;
}
inline float EnemyParaFunc::vfPara_0x24() {
    return 0.0f;
}
inline float EnemyParaFunc::vfPara_0x28() {
    return 0.0f;
}
inline float EnemyParaFunc::vfPara_0x2C() {
    return 0.0f;
}
inline float EnemyParaFunc::vfPara_0x30() {
    return 0.0f;
}
inline int EnemyParaFunc::vfPara_0x38() {
    return 0;
}
inline void EnemyParaFunc::vfPara_0x40(int value) {}
inline int EnemyParaFunc::vfPara_0x44() {
    return 0;
}
inline int EnemyParaFunc::vfPara_0x50() {
    return -1;
}
inline int EnemyParaFunc::vfPara_0x5C() {
    return -1;
}
inline int EnemyParaFunc::vfPara_0x64() {
    return -1;
}
inline float EnemyParaFunc::vfPara_0x68() {
    return 0.0f;
}
inline float EnemyParaFunc::vfPara_0x6C() {
    return 0.0f;
}
inline int EnemyParaFunc::vfPara_0x70() {
    return 0;
}
inline int EnemyParaFunc::vfPara_0x74() {
    return 0;
}
