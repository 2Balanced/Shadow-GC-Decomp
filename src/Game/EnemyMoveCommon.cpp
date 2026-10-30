/* EnemyMoveCommon (class name from RTTI; Game/Enemy.h): the movement part of an enemy. Each frame
 * it moves the enemy by the frame's movement and its jump or fall, keeps it inside its area, pushes
 * it out of walls (checked every frame, every other frame or every 16th, by how far it moves),
 * finds the floor below with a line test and puts the enemy on it. File name inferred from the
 * class. Names marked "(name inferred)" are not original. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
/* The fields of EnemyMoveCommon (Game/Enemy.h). */
#define ENEMY_MOVE_COMMON_UNIT

#include "Game/DebugDraw.h"
#include "Game/Enemy.h"
#include "Game/GadgetCollision.h"
#include "Game/MathTypes.h"

/* A line between two points for the collision test (see Game/BulletUtil.cpp). */
struct UnkLine_80020A20 {
    u8 field_0x0[0x18];
};

/* A collider the line test meets (see Game/BulletUtil.cpp). */
struct UnkLineObject_8005B5A0 {
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ void *m_collider;
    /* 0x0C */ u32 m_flags;
    /* 0x10 */ void *m_data;
};

/* A node of a contact list: the hit's object at 0x14 (see Game/BkLarva.cpp). */
struct UnkColliContactNode_800587A0 {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ void *m_object;
};

extern "C" {
/* The collision world, a line between two points, the contacts of what a line or a sphere meets,
 * and their list (see Game/BulletUtil.cpp and Game/BkWingLarge.cpp). */
void *fn_80020D40(void);
void *fn_80020A20(UnkLine_80020A20 *line, const Vec3f *start, const Vec3f *end);
UnkColliContacts_8001EE04 *fn_80021FE4(void *world, const UnkLine_80020A20 *line, int arg2, int kind,
                                       bool (*filter)(const UnkLineObject_8005B5A0 *object), void *data);
UnkColliContacts_8001EE04 *fn_80021CA4(void *world, const UnkSphere_8004E6CC *sphere, int arg2,
                                       int arg3, int arg4, int arg5, int arg6);
void fn_80026FB4(UnkColliContacts_8001EE04 *contacts, int arg1);
void fn_8001E9F8(UnkColliContacts_8001EE04 *contacts);
UnkColliContactNode_800587A0 *fn_8001EE04(UnkColliContacts_8001EE04 *contacts);
UnkColliContactNode_800587A0 *fn_8001EDEC(UnkColliContacts_8001EE04 *contacts);
void fn_80027398(UnkColliContactNode_800587A0 **it);
u32 fn_80026588(UnkColliContactNode_800587A0 *node);
/* A hit object's point, normal and kind, and the push out of it for a sphere (behaviour
 * inferred). */
const Vec3f *fn_8001E9BC(void *object);
const Vec3f *fn_8001E9C4(void *object);
int fn_8001E9F0(void *object);
Vec3f fn_8001E858(void *world, void *object, const UnkSphere_8004E6CC *sphere, float radius);
/* The movement of the frame and the area radius of an enemy (weak getters of other units; see
 * Game/Enemy.cpp). */
Vec3f *fn_80181F0C(Enemy *enemy);
float fn_8018346C(Enemy *enemy);
/* The area: keeps it about a point, and whether it holds a point (Game/Enemy.cpp; behaviour
 * inferred). */
void fn_801A687C(UnkEnemyArea_8018A374 *area, const Vec3f *point);
bool fn_801A6944(UnkEnemyArea_8018A374 *area, const Vec3f *point);
/* Whether value is nearly zero (see Game/CameraModeGeneral.cpp). */
bool fn_8000B044(float value);
/* Whether bit is set in flags (see Game/EggLastMech.h). */
bool fn_80014E34(u32 *flags, int bit);
/* Vector helpers (see Game/Enemy.cpp and Game/EnemyDispCommon.cpp). */
Vec3f fn_8040CDA8(const Vec3f *v, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040D0AC(const Vec3f *v, float angle);
float fn_8040CE74(Vec3f *v);
void fn_8040CE38(Vec3f *vector, float scale);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
Vec3f fn_8040FB08(const Mtx34f *matrix);
}

/* fn_8040CD74 as a function that returns its result (see Game/Enemy.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *lhs, const Vec3f *rhs);

/* The vector type's out-of-line copy and assignment, called with a temporary the way the
 * original's operators are (names inferred; see Game/Enemy.cpp). */
inline void ConstructVector(Vec3f *vector, const Vec3f &value) {
    fn_800091EC(vector, &value);
}

inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* The parameter slots 0x1C and 0x70 return the enemy's body and jump records, read here as floats:
 * the body sphere's radius, the floor offset and the sphere's height, and the jump's speed and height
 * (see Game/EnemyParamCommon.cpp). */

/* The larger of a and b (name inferred). */
inline float Max(float a, float b) {
    return a > b ? a : b;
}

/* The walls' initial touching flag (Game/Enemy.h). */
#pragma explicit_zero_data on
u8 lbl_805E81D0 = 0;
#pragma explicit_zero_data off

/* Whether the line test counts a collider (name inferred). */
extern "C" bool fn_801A61D0(const UnkLineObject_8005B5A0 *object) {
    return (object->m_flags & 0x1AB3) != 0;
}

/* The floor below from: its height, normal and attributes, 0 if there is none (name inferred). */
extern "C" u32 fn_801A609C(const Vec3f *from, float *height, Vec3f *normal, int arg3, int kind) {
    Vec3f to(from->x, from->y - 10000.0f, from->z);
    UnkLine_80020A20 line;
    fn_80020A20(&line, from, &to);
    void *object;
    UnkColliContacts_8001EE04 *contacts = fn_80021FE4(fn_80020D40(), &line, arg3, kind, fn_801A61D0, 0);
    u32 result = 0;
    if (contacts != 0 && contacts->m_count > 0) {
        for (UnkColliContactNode_800587A0 *it = fn_8001EE04(contacts); it != fn_8001EDEC(contacts);
             fn_80027398(&it)) {
            object = it->m_object;
            if (object != 0 && fn_8001E9F0(object) != 3) {
                if (height != 0) {
                    *height = fn_8001E9BC(object)->y;
                }
                if (normal != 0) {
                    fn_800091D0(normal, fn_8001E9C4(object));
                }
                result = fn_80026588(it);
                break;
            }
        }
        fn_80026FB4(contacts, 1);
    }
    return result;
}

void UnkEnemyGround_801A5EA8::Update(const Vec3f *position) {
    void *object;
    fn_8040CE60(&m_push);
    bool touching = false;
    Vec3f offset;
    ConstructVector(&offset, ((VectorSubtractFn)fn_8040CD74)(
                                 position, &(const Vec3f &)m_enemy->m_disp->vfDisp_0x2C()));
    Mtx34f *matrix = m_enemy->m_disp->GetPartMatrix(m_enemy->m_para->vfPara_0x3C());
    if (matrix != 0) {
        AssignVector(m_sphere.m_center, fn_8040CDD0(&(const Vec3f &)fn_8040FB08(matrix), &offset));
    } else {
        AssignVector(m_sphere.m_center, *m_enemy->GetPosition());
        m_sphere.m_center.y += ((const float *)m_enemy->m_para->vfPara_0x1C())[3];
    }
    void *world = fn_80020D40();
    UnkColliContacts_8001EE04 *contacts = fn_80021CA4(world, &m_sphere, 1, 0, 0, 0, 0);
    if (contacts != 0) {
        fn_8001E9F8(contacts);
        for (UnkColliContactNode_800587A0 *it = fn_8001EE04(contacts); it != fn_8001EDEC(contacts);
             fn_80027398(&it)) {
            object = it->m_object;
            if (object != 0) {
                Vec3f normal;
                fn_800091EC(&normal, fn_8001E9C4(object));
                if (normal.y < 0.5f) {
                    Vec3f push;
                    ConstructVector(&push, fn_8001E858(world, object, &m_sphere, m_sphere.m_radius));
                    push.y = 0.0f;
                    fn_8040CFF8(&m_push, &push);
                    touching = true;
                }
            }
        }
        fn_80026FB4(contacts, 1);
    }
    m_touching.m_previous = m_touching.m_current;
    m_touching.m_current = touching;
}

inline void UnkEnemyGround_801A5EA8::Init(Enemy *enemy) {
    m_enemy = enemy;
    AssignVector(m_sphere.m_center, *m_enemy->GetPosition());
    m_sphere.m_radius = *(const float *)m_enemy->m_para->vfPara_0x1C();
}

EnemyMoveCommon::EnemyMoveCommon() {
    m_ground.Init(m_enemy);
    AssignVector(m_position, *m_enemy->GetPosition());
    m_height = 0.0f;
    m_floorY = -999999.0f;
    m_floorKind = 0xF;
    Vec3f up;
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    AssignVector(m_floorNormal, up);
    m_groundCheck = true;
    m_jumping = false;
    m_jumpStart = false;
    m_onWater = false;
    m_overWater = false;
}

void EnemyMoveCommon::vfMove_0x08() {
    AssignVector(m_position, *m_enemy->GetPosition());
    vfMove_0x0C(0.0f);
}

/* An empty inline call that keeps its caller out of line, as retail has it (name inferred). */
inline void UnkEnemyNothing() {}

inline Vec3f *UnkEnemyGround_801A5EA8::GetPush() {
    UnkEnemyNothing();
    return &m_push;
}

/* The jump or fall of the frame (name inferred). */
extern "C" Fn8040CopyVector3 fn_801A54E8(EnemyMoveCommon *move, float dt);

void EnemyMoveCommon::vfMove_0x0C(float dt) {
    if ((u8)vfMove_0x20()) {
        Vec3f *position = m_enemy->GetPosition();
        fn_801A687C(&m_enemy->field_0xA8, position);
    }
    if (m_jumping) {
        fn_8040CFF8(fn_80181F0C(m_enemy), &(const Vec3f &)fn_801A54E8(this, dt));
    }
    Vec3f move;
    ConstructVector(&move, fn_8040CDD0(fn_80181F0C(m_enemy), &(const Vec3f &)m_enemy->m_disp->vfDisp_0x28()));
    AssignVector(m_position, fn_8040CDD0(m_enemy->GetPosition(), &move));
    if (!fn_801A6944(&m_enemy->field_0xA8, &m_position)) {
        Vec3f out;
        ConstructVector(&out, ((VectorSubtractFn)fn_8040CD74)(&m_position, &m_enemy->field_0x48));
        out.y = 0.0f;
        fn_8040CE74(&out);
        fn_8040CE38(&out, fn_8018346C(m_enemy));
        m_position.x = out.x + m_enemy->field_0x48.x;
        m_position.z = out.z + m_enemy->field_0x48.z;
        move.x = m_position.x - m_enemy->GetPosition()->x;
        move.z = m_position.z - m_enemy->GetPosition()->z;
    }
    float distance = fn_8040CF2C(&move);
    if (m_groundCheck) {
        int rate;
        if ((u8)m_ground.IsTouching() || !vfMove_0x18() || distance > 0.5f) {
            rate = 2;
        } else if (distance > 0.1f) {
            rate = 1;
        } else {
            rate = 0;
        }
        u32 frame = *reinterpret_cast<u32 *>(&lbl_8057D6A0[0x90]) + m_enemy->m_handle;
        if (rate == 2 || (rate == 1 && (frame & 1)) || (rate == 0 && !(frame & 0xF))) {
            m_ground.Update(&m_position);
            fn_8040CFF8(&move, m_ground.GetPush());
        }
    }
    move.y -= dt * m_enemy->m_para->vfPara_0x68();
    Vec3f probe;
    ConstructVector(&probe, fn_8040CDD0(m_enemy->GetPosition(), &move));
    probe.y += ((const float *)m_enemy->m_para->vfPara_0x1C())[2];
    Vec3f to;
    fn_800091EC(&to, &probe);
    probe.y += 10.0f;
    m_floorY = -999999.0f;
    u32 floor = fn_801A609C(&probe, &m_floorY, &m_floorNormal, 3, 0xC);
    if (floor != 0) {
        to.y = Max(to.y, m_floorY);
    } else if (!(u8)vfMove_0x20()) {
        to.x = m_enemy->GetPosition()->x;
        to.z = m_enemy->GetPosition()->z;
        fn_800091D0(&probe, &to);
        m_floorY = -999999.0f;
        floor = fn_801A609C(&probe, &m_floorY, &m_floorNormal, 3, 0xC);
        if (floor != 0) {
            to.y = Max(to.y, m_floorY);
        }
    }
    m_height = to.y - m_floorY;
    to.y -= ((const float *)m_enemy->m_para->vfPara_0x1C())[2];
    if (floor & 0x20) {
        m_overWater = true;
        if (m_height < 5.0f) {
            m_onWater = true;
        } else {
            m_onWater = false;
        }
    } else {
        m_overWater = false;
        m_onWater = false;
    }
    if (floor != 0) {
        m_floorKind = floor >> 28;
    } else {
        m_floorKind = 0xF;
    }
    fn_800091D0(&m_position, &to);
    if (!fn_80014E34(&m_enemy->m_status->vfStat_0xA8()->m_bits, 0xA)) {
        fn_800091D0(m_enemy->GetPosition(), &m_position);
    }
    m_enemy->m_disp->vfDisp_0x3C(true);
}

bool EnemyMoveCommon::vfMove_0x18() {
    return m_height < 5.0f;
}

inline int UnkEnemyGround_801A5EA8::IsTouching() {
    UnkEnemyNothing();
    return m_touching.m_current;
}

int EnemyMoveCommon::vfMove_0x1C() {
    return m_ground.IsTouching();
}

/* Starts a jump (name inferred). */
void EnemyMoveCommon::vfMove_0x34() {
    m_jumping = true;
    m_jumpStart = true;
    float forward = m_enemy->m_status->vfStat_0x74() == 0 ? 1.0f : -1.0f;
    Vec3f direction;
    direction.x = 0.0f;
    direction.y = 0.0f;
    direction.z = forward;
    AssignVector(m_velocity, direction);
    AssignVector(m_velocity, fn_8040D0AC(&m_velocity, m_enemy->GetRotation()->y));
    fn_8040CE38(&m_velocity, *(const float *)m_enemy->m_para->vfPara_0x70());
    m_velocity.y = m_enemy->m_para->vfPara_0x68() * ((const float *)m_enemy->m_para->vfPara_0x70())[1];
}

extern "C" Fn8040CopyVector3 fn_801A54E8(EnemyMoveCommon *move, float dt) {
    Vec3f step;
    AssignVector(step, fn_8040CDA8(&move->m_velocity, dt));
    if ((move->m_velocity.y -= dt * move->m_enemy->m_para->vfPara_0x68()) < 0.0f) {
        move->m_velocity.y = 0.0f;
    }
    if (!move->m_jumpStart && move->m_height < 0.1f) {
        if ((move->m_velocity.x *= 0.001f) < 0.0001f) {
            move->m_velocity.x = 0.0f;
        }
        if ((move->m_velocity.z *= 0.001f) < 0.0001f) {
            move->m_velocity.z = 0.0f;
        }
    }
    if (fn_8000B044(fn_8040CF2C(&move->m_velocity))) {
        move->m_jumping = false;
    }
    move->m_jumpStart = false;
    return Fn8040CopyVector3(step);
}
