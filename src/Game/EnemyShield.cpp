/* EnemyShield and EnemyShieldTask (class names from RTTI): the shield a soldier or an EggPawn
 * carries. EnemyShield is a handle to its implementation (struct name unknown; names here are
 * inferred), a model hung from a bone of the enemy that takes hits until it breaks off, then falls
 * and spins as it fades; EnemyShieldTask runs it until it is gone. The enemy units create it with
 * fn_801A752C and call its functions by their address names. Names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first).
 *
 * NonMatching: 29 of 30 functions and all data match. In Setup retail loads the enemy's motion slot
 * into r0 and moves the table from r4 to r5 after it; GC/2.7 moves the table to r0 at the entry and
 * loads the slot straight into r4, for every source form tried (a local, an inline wrapper, a
 * pointer to the table). */
#include "Game/CharaColliAttack.h"
#include "Game/Enemy.h"
#include "Game/EnemyRender.h"
#include "Game/Gadget.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/Task.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

using SonicteamUSA::System::CharaColli::AttackCallbackParam;

struct RwFrame;
struct _stMtnPlayWork;
class UnkModel_801A3E9C;

namespace std {
float cos(float x);
}

extern "C" {
/* The enemy model calls (Game/EnemyModel.cpp; names inferred there): load the parts, release
 * them, the two draw modes, alpha, update, attach to and detach from a frame, the body's frame, a
 * bone's frame and the body's placement. */
void fn_801A3CE4(UnkModel_801A3E9C *model, int slot, const EnemyMotionTable *table, _stMtnPlayWork *motion);
void fn_801A3B58(UnkModel_801A3E9C *model);
void fn_801A3AFC(UnkModel_801A3E9C *model);
void fn_801A3AA0(UnkModel_801A3E9C *model);
void fn_801A39E0(UnkModel_801A3E9C *model, float alpha);
void fn_801A38E8(UnkModel_801A3E9C *model);
void fn_801A3814(UnkModel_801A3E9C *model, RwFrame *parent);
void fn_801A37C8(UnkModel_801A3E9C *model);
RwFrame *fn_801A3750(UnkModel_801A3E9C *model);
RwFrame *fn_801A3704(UnkModel_801A3E9C *model, int id);
void fn_801A367C(UnkModel_801A3E9C *model, const Vec3f *position, const Vec3f *rotation);
/* A frame's world matrix, the translation of a matrix and the rotation of one (see
 * Game/EnemyTarget.cpp and Game/EnemyDispCommon.cpp). */
Matrix34 fn_802C8068(RwFrame *frame);
Vec3f fn_8040FB08(const Mtx34f *matrix);
void fn_800494F8(Vec3f *rotation, const Mtx34f *matrix);
/* Moves position by velocity under acceleration over dt (behaviour inferred). */
void fn_804103E8(Vec3f *position, Vec3f *velocity, const Vec3f *acceleration, float scale, float dt);
/* vector turned about y by angle, and an angle brought into range (behaviours inferred). */
Vec3f fn_8040D0AC(const Vec3f *vector, float angle);
float fn_8040E99C(float angle);
}

/* The model part of an enemy (class name unknown; Game/BkLarva.h). */
class UnkModel_801A3E9C {
  public:
    UnkModel_801A3E9C();
    ~UnkModel_801A3E9C();

    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ s32 m_frameCount;
};

/* degrees in radians (name inferred; as in Game/EnemyArea.cpp). */
inline float Radians(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* Loads model from table (name inferred). */
inline void LoadModel(UnkModel_801A3E9C *model, int slot, const EnemyMotionTable &table) {
    fn_801A3CE4(model, slot, &table, 0);
}

/* The shield itself (struct name unknown; names inferred). */
struct UnkEnemyShield_801A7E1C {
    UnkEnemyShield_801A7E1C(Enemy *enemy);
    ~UnkEnemyShield_801A7E1C() {
        fn_801A3B58(&m_model);
    }

    void Setup(const EnemyMotionTable &table, int bone);
    void Update(float dt);
    void Hit(AttackCallbackParam &param);
    void BreakOff();
    void Release();
    bool Fall(float dt);
    float GetAlpha();
    /* Takes the shield away at once. */
    void Remove() {
        fn_801A3B58(&m_model);
        m_state = 2;
        m_enemy = 0;
    }

    /* 0x00 */ Enemy *m_enemy;       /* the enemy carrying it, until it breaks off */
    /* 0x04 */ UnkModel_801A3E9C m_model;
    /* 0x0C */ s32 m_state;          /* 0 carried, 1 falling, 2 gone */
    /* 0x10 */ float m_health;
    /* 0x14 */ float m_fallTime;     /* left of the fall */
    /* 0x18 */ Vec3f m_position;
    /* 0x24 */ Vec3f m_velocity;
    /* 0x30 */ Vec3f m_rotation;
};

/* EnemyShield (class name from the RTTI of its shared_ptr control block): the handle the enemy
 * holds (method names keep their addresses, as the enemy units call them). */
class EnemyShield {
  public:
    EnemyShield(Enemy *enemy);

    void fn_801A7478();
    void fn_801A74C4();
    bool fn_801A74E8();
    void fn_801A7508(AttackCallbackParam &param);

    /* 0x0 */ boost::scoped_ptr<UnkEnemyShield_801A7E1C> m_impl;
};

/* EnemyShieldTask (class name from RTTI): runs the shield until it is gone. */
class EnemyShieldTask : public Task {
  public:
    EnemyShieldTask(Task *parent, const boost::shared_ptr<EnemyShield> &shield);
    virtual ~EnemyShieldTask();
    virtual void Tick(float dt);

    /* 0x28 */ boost::shared_ptr<EnemyShield> m_shield;
};

UnkEnemyShield_801A7E1C::UnkEnemyShield_801A7E1C(Enemy *enemy)
    : m_enemy(enemy), m_state(0), m_rotation(0.0f, 0.0f, 0.0f) {
    m_health = 4.0f;
    m_fallTime = 1.5f;
    fn_8040CE60(&m_position);
    fn_8040CE60(&m_velocity);
    fn_8040E970(&m_rotation);
}

/* Loads the model and hangs it from bone of the enemy. */
void UnkEnemyShield_801A7E1C::Setup(const EnemyMotionTable &table, int bone) {
    if (m_enemy != 0) {
        LoadModel(&m_model, m_enemy->m_motionBase, table);
        UnkModel_801A3E9C *body = reinterpret_cast<UnkModel_801A3E9C *>(m_enemy->m_disp->vfDisp_0x18());
        if (body != 0) {
            RwFrame *frame = fn_801A3704(body, bone);
            if (frame != 0) {
                fn_801A3814(&m_model, frame);
            }
        }
    }
}

void UnkEnemyShield_801A7E1C::Update(float dt) {
    switch (m_state) {
    case 0:
        break;
    case 1:
        if (*reinterpret_cast<u32 *>(&lbl_8057D6A0[0x90]) & 1) {
            fn_801A3AFC(&m_model);
        } else {
            fn_801A3AA0(&m_model);
        }
        if (Fall(dt)) {
            m_state = 2;
        }
        break;
    case 2:
        return;
    }
    fn_801A39E0(&m_model, GetAlpha());
    fn_801A38E8(&m_model);
}

void UnkEnemyShield_801A7E1C::Hit(AttackCallbackParam &param) {
    float health = m_health;
    if ((m_health = health - param.m_damage) <= 0.0f) {
        m_health = 0.0f;
        BreakOff();
    }
    param.m_damage -= health - m_health;
    if (param.m_damage < 0.0f) {
        param.m_damage = 0.0f;
    }
}

void UnkEnemyShield_801A7E1C::BreakOff() {
    if (m_state < 1) {
        Release();
        m_state = 1;
        m_enemy = 0;
    }
}

/* Takes the model off the enemy where it is, and gives it a push away from it. */
void UnkEnemyShield_801A7E1C::Release() {
    RwFrame *frame = fn_801A3750(&m_model);
    if (frame != 0) {
        Matrix34 matrix = fn_802C8068(frame);
        fn_800091D0(&m_position, &(const Vec3f &)fn_8040FB08(&matrix));
        fn_800494F8(&m_rotation, &matrix);
    }
    if (m_enemy != 0) {
        fn_800091D0(&m_velocity, &(const Vec3f &)fn_8040D0AC(&Vec3f(0.0f, 50.0f, 5.0f), m_enemy->GetRotation()->y));
    }
    fn_801A37C8(&m_model);
}

bool UnkEnemyShield_801A7E1C::Fall(float dt) {
    Vec3f gravity(0.0f, -60.0f, 0.0f);
    fn_804103E8(&m_position, &m_velocity, &gravity, 0.0f, dt);
    m_rotation.y = fn_8040E99C(m_rotation.y + dt * Radians(100.0f));
    m_rotation.z = fn_8040E99C(m_rotation.z + dt * Radians(30.0f));
    fn_801A367C(&m_model, &m_position, &m_rotation);
    if ((m_fallTime -= dt) < 0.0f) {
        m_fallTime = 0.0f;
        return true;
    }
    return false;
}

float UnkEnemyShield_801A7E1C::GetAlpha() {
    if (m_state >= 1) {
        return std::cos(0.5f * ((1.0f - m_fallTime / 1.5f) * lbl_805FAD38));
    }
    if (m_enemy != 0) {
        return m_enemy->field_0xEC;
    }
    return 0.0f;
}

EnemyShieldTask::EnemyShieldTask(Task *parent, const boost::shared_ptr<EnemyShield> &shield)
    : Task(parent), m_shield(shield) {
    m_name = "EnemyShield";
}

EnemyShieldTask::~EnemyShieldTask() {}

void EnemyShieldTask::Tick(float dt) {
    if (m_shield.get() == 0 || m_shield->m_impl->m_state >= 2) {
        m_flags |= 1;
    } else {
        m_shield->m_impl->Update(dt);
    }
}

EnemyShield::EnemyShield(Enemy *enemy) : m_impl(new UnkEnemyShield_801A7E1C(enemy)) {}

/* Creates the shield of enemy from table and hangs it from bone (name inferred). */
extern "C" boost::weak_ptr<EnemyShield> fn_801A752C(Enemy *enemy, const EnemyMotionTable &table, int bone) {
    boost::shared_ptr<EnemyShield> shield = boost::shared_ptr<EnemyShield>(new EnemyShield(enemy));
    if (shield.get() != 0) {
        shield->m_impl->Setup(table, bone);
    }
    new EnemyShieldTask(fn_801EB304(fn_8001C4F8(), 0xE), shield);
    return shield;
}

/* A hit on the shield. */
void EnemyShield::fn_801A7508(AttackCallbackParam &param) {
    m_impl->Hit(param);
}

/* Whether the shield is off the enemy. */
bool EnemyShield::fn_801A74E8() {
    return m_impl->m_state >= 1;
}

/* Breaks the shield off the enemy. */
void EnemyShield::fn_801A74C4() {
    m_impl->BreakOff();
}

/* Takes the shield away. */
void EnemyShield::fn_801A7478() {
    m_impl->Remove();
}
