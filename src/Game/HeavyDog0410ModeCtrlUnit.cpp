/* HeavyDog0410ModeCtrlUnit (class name from RTTI): the mode control unit of the Heavy Dog boss's
 * 0410 stage. CreateMode builds one HeavyDog0410ModeCtrlUnit_<MODE> unit (class names from RTTI)
 * for each of the stage's modes; each one sets up its motion in its constructor, runs in Tick
 * (which asks the boss for the next mode by setting BossVarParam::m_nextMode) and cleans up in its
 * destructor. File name inferred from the class (as Game/BlackBull0412ModeCtrlUnit.cpp). Field
 * and helper names are inferred. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first); the mode units' constructors are inline and are emitted
 * after CreateMode, their first user. */
#define BOSSVARPARAM_CHARA_SHARED_PTR
#define HEAVYDOGVARPARAM_FULL
#include "Effect/System/EffectElement.h"
#include "Game/BossCtrlUnit.h"
#include "Game/BossRootTask.h"
#include "Game/CharInfo.h"
#include "Game/EnemyCharInfo.h"
#include "Game/HeavyDogCtrlUnit.h"
#include "Game/HeavyDogCtrlUnits.h"
#include "Game/HeavyDogModeCtrlUnit.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/Vibration.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Effect::System::EffectElement;

struct _stMtnPlayWork;
struct UnkHintManager_801FB180;
struct UnkObject_80074470;

/* What fn_80415D40 fills in about a motion (as in BlackBull0412ModeCtrlUnit.cpp). */
struct UnkMotionFrames {
    /* 0x00 */ float m_start;
    /* 0x04 */ float m_end;
    /* 0x08 */ float field_0x8;
    /* 0x0C */ float m_frame;
};

/* The game state (as in BlackBull0412ModeCtrlUnit.cpp): the time step the stun counts at 0x18
 * (meaning inferred). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x18];
    /* 0x018 */ float field_0x18;
};
extern UnkGameState_8057D6A0 lbl_8057D6A0;

extern const Vec3f lbl_805E07B8;

/* A mine the boss drops (defined in Game/HeavyDog0410CtrlUnit.cpp). */
class HeavyDog0410CtrlUnit_Mine : public HeavyDog0410CtrlUnit, public AttackCallbackContainer {
  public:
    HeavyDog0410CtrlUnit_Mine(HeavyDog0410VarParam *param, const Vec3f &position,
                              const Vec3f &velocity, const Vec3f &target);
    virtual ~HeavyDog0410CtrlUnit_Mine();
    virtual int Tick(float dt);
    virtual void OnDamage(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);

    /* 0x14 */ u8 field_0x14[0x68 - 0x14];
};

extern "C" {
u16 fn_80415CDC(_stMtnPlayWork *work, int arg1);
void fn_80417568(_stMtnPlayWork *work, u16 motion);
int fn_80416F28(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
void fn_80415D40(_stMtnPlayWork *work, int arg1, UnkMotionFrames *frames);
Mtx34f *fn_80415E70(_stMtnPlayWork *work, int node);
void fn_80016370(Mtx34f *matrix, void *frame);

s32 fn_801FAF00(UnkHintManager_801FB180 *manager, s32 id, s32 branch, int arg3, int arg4);
void fn_8035367C(const char *file, const char *layout, const char *animation);
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int arg1);
void fn_800A4210(const boost::shared_ptr<EffectElement> &effect);
UnkObject_80074470 *fn_80074470(void);
void fn_80175EEC(UnkObject_80074470 *object, int arg1, int arg2);
void *fn_80053164(void);
void fn_801D54F4(void *manager, int id, const void *source, int arg3, int arg4);
EnemyCharInfo *fn_801792D0(EnemyCharInfo *const *body);

float fn_80016260(float x, float z); /* the angle of (x, z) */
float fn_8040CE74(Vec3f *vector);
void fn_8040CE38(Vec3f *vector, float scale);
float fn_8040CF90(const Vec3f *a, const Vec3f *b);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
Vec3f fn_8040FB08(const Mtx34f *matrix);
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CEE8(const Vec3f *vector);
}

UnkHintManager_801FB180 *GetHintManager();

typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

inline const Vec3f &Ref(const Vec3f &vector) {
    return vector;
}

inline Vec3f Sub(const Vec3f &a, const Vec3f &b) {
    return ((VectorSubtractFn)fn_8040CD74)(&a, &b);
}

/* The boss's body as its character info (as in Game/HeavyDog0410CtrlUnit.cpp). */
#define Body(owner) fn_801792D0(reinterpret_cast<EnemyCharInfo *const *>(&(owner)->m_charaPtr))

/* Drops a mine from the boss's hatch toward the player (name inferred; written out where
 * used). */
#define DropMine()                                                                                 \
    do {                                                                                           \
        Mtx34f *node = fn_80415E70(m_owner->m_motion, 0x1194);                                     \
        Mtx34f matrix;                                                                             \
        fn_80016370(&matrix, node);                                                                \
        Vec3f position;                                                                            \
        fn_800091EC(&position, &Ref(fn_8040FB08(&matrix)));                                        \
        fn_80016370(&matrix, fn_80415E70(m_owner->m_motion, 0xFAA));                               \
        Vec3f body;                                                                                \
        fn_800091EC(&body, &Ref(fn_8040FB08(&matrix)));                                            \
        Vec3f target;                                                                              \
        fn_800091EC(&target, &Ref(Sub(m_owner->m_playerPosition, body)));                          \
        float reach = m_owner->field_0x1DC; \
        fn_800091D0(&target, \
                    &Ref(fn_8040CDD0(&Ref(fn_8040CDA8(&Ref(fn_8040CEE8(&target)), reach)), &body))); \
        Vec3f velocity;                                                                            \
        fn_800091EC(&velocity, &Ref(Sub(m_owner->m_playerPosition, body)));                        \
        velocity.y = 0.0f;                                                                         \
        float speed = m_owner->field_0x1E0; \
        fn_800091D0(&velocity, &Ref(fn_8040CDA8(&Ref(fn_8040CEE8(&velocity)), speed))); \
        velocity.y = m_owner->field_0x1E4;                                                         \
        fn_8040CFF8(&velocity, &m_owner->m_velocity);                                              \
        fn_803358D0(m_owner,                                                                       \
                    new HeavyDog0410CtrlUnit_Mine(m_owner, position, velocity, target));           \
    } while (0)

/* WAIT: waits for the next attack. */
class HeavyDog0410ModeCtrlUnit_WAIT : public HeavyDog0410CtrlUnit {
  public:
    HeavyDog0410ModeCtrlUnit_WAIT(HeavyDog0410VarParam *owner)
        : BossCtrlUnit(owner), HeavyDog0410CtrlUnit(owner) {
        m_owner->m_fixed = 0;
        m_owner->m_turnFixed = 0;
    }
    virtual ~HeavyDog0410ModeCtrlUnit_WAIT();
    virtual int Tick(float dt);
};

class HeavyDog0410ModeCtrlUnit_EDIT : public HeavyDog0410CtrlUnit {
  public:
    HeavyDog0410ModeCtrlUnit_EDIT(HeavyDog0410VarParam *owner)
        : BossCtrlUnit(owner), HeavyDog0410CtrlUnit(owner) {
        m_owner->m_fixed = 1;
        m_owner->m_turnFixed = 1;
    }
    virtual ~HeavyDog0410ModeCtrlUnit_EDIT();
    virtual int Tick(float dt);
};

/* CHASE: drives around the player. */
class HeavyDog0410ModeCtrlUnit_CHASE : public HeavyDog0410CtrlUnit {
  public:
    HeavyDog0410ModeCtrlUnit_CHASE(HeavyDog0410VarParam *owner)
        : BossCtrlUnit(owner), HeavyDog0410CtrlUnit(owner) {
        m_owner->m_fixed = 0;
        m_owner->m_turnFixed = 0;
        m_time = 0.0f;
    }
    virtual ~HeavyDog0410ModeCtrlUnit_CHASE();
    virtual int Tick(float dt);

    /* 0x10 */ float m_time;
};

/* ESCAPE: flees to the corner away from the player, dropping mines. */
class HeavyDog0410ModeCtrlUnit_ESCAPE : public HeavyDog0410CtrlUnit {
  public:
    HeavyDog0410ModeCtrlUnit_ESCAPE(HeavyDog0410VarParam *owner)
        : BossCtrlUnit(owner), HeavyDog0410CtrlUnit(owner) {
        m_owner->m_fixed = 0;
        m_owner->m_turnFixed = 0;
    }
    virtual ~HeavyDog0410ModeCtrlUnit_ESCAPE();
    virtual int Tick(float dt);
};

/* GET_ATK: the boss is hit. */
class HeavyDog0410ModeCtrlUnit_GET_ATK : public HeavyDog0410CtrlUnit {
  public:
    HeavyDog0410ModeCtrlUnit_GET_ATK(HeavyDog0410VarParam *owner)
        : BossCtrlUnit(owner), HeavyDog0410CtrlUnit(owner) {
        m_owner->m_stopped = 1;
        m_owner->m_fixed = 1;
        m_owner->field_0x1D0++;
        m_time = 0.0f;
    }
    virtual ~HeavyDog0410ModeCtrlUnit_GET_ATK();
    virtual int Tick(float dt);

    /* 0x10 */ float m_time;
};

/* START: the boss's entrance. */
class HeavyDog0410ModeCtrlUnit_START : public HeavyDog0410CtrlUnit {
  public:
    HeavyDog0410ModeCtrlUnit_START(HeavyDog0410VarParam *owner)
        : BossCtrlUnit(owner), HeavyDog0410CtrlUnit(owner) {
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 4);
        fn_800091D0(&m_owner->m_charaPtr.get()->m_position, &lbl_805E07A0);
        Body(m_owner)->m_rotation = lbl_805E07B8;
        m_owner->m_stopped = 1;
        m_owner->m_fixed = 1;
        m_owner->m_turnFixed = 1;
        fn_803358D0(m_owner, new BossCtrlUnit_PlayCameraMotion(m_owner, 0xE, &lbl_805E07A0,
                                                                &lbl_805E07B8, 0.0f, 0.0f));
        m_owner->m_active = 0;
        fn_801FAF00(GetHintManager(), 0xDF, 0, 0, 0);
        fn_8035367C("CsdFiles/BChara_HDog", "bchara", "open");
    }
    virtual ~HeavyDog0410ModeCtrlUnit_START();
    virtual int Tick(float dt);
};

/* BELOW_HALF: the cut-in when the boss's health falls below half. */
class HeavyDog0410ModeCtrlUnit_BELOW_HALF : public HeavyDog0410CtrlUnit {
  public:
    HeavyDog0410ModeCtrlUnit_BELOW_HALF(HeavyDog0410VarParam *owner)
        : BossCtrlUnit(owner), HeavyDog0410CtrlUnit(owner) {
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 2);
        m_owner->m_stopped = 1;
        m_owner->m_fixed = 1;
        m_owner->m_turnFixed = 1;
        fn_803358D0(m_owner, new BossCtrlUnit_PlayCameraMotion(m_owner, 0xC, 0.0f, 0.0f));
        m_owner->m_active = 0;
        fn_801FAF00(GetHintManager(), 0xE1, 0, 0, 0);
        fn_8034DF48(0xC, -2);
    }
    virtual ~HeavyDog0410ModeCtrlUnit_BELOW_HALF();
    virtual int Tick(float dt);
};

/* DESTROY: the boss is destroyed and the stage ends. */
class HeavyDog0410ModeCtrlUnit_DESTROY : public HeavyDog0410CtrlUnit {
  public:
    HeavyDog0410ModeCtrlUnit_DESTROY(HeavyDog0410VarParam *owner)
        : BossCtrlUnit(owner), HeavyDog0410CtrlUnit(owner) {
        m_ended = 0;
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 3);
        fn_800091D0(&m_owner->m_charaPtr.get()->m_position, &lbl_805E07A0);
        Body(m_owner)->m_rotation = lbl_805E07B8;
        m_owner->m_stopped = 1;
        m_owner->m_fixed = 1;
        m_owner->m_turnFixed = 1;
        fn_803358D0(m_owner, new BossCtrlUnit_PlayCameraMotion(
                                 m_owner, 0xD, &m_owner->m_charaPtr.get()->m_position,
                                 &m_owner->m_charaPtr.get()->m_rotation, 0.0f, 0.0f));
        m_owner->m_active = 0;
        fn_801FAF00(GetHintManager(), 0xE2, 0, 0, 0);
        fn_803358D0(m_owner, new HeavyDogCtrlUnit_DestroyEffect(m_owner));
        fn_8034DF48(7, -2);
    }
    virtual ~HeavyDog0410ModeCtrlUnit_DESTROY();
    virtual int Tick(float dt);

    /* 0x10 */ boost::weak_ptr<EffectElement> m_effect;
    /* 0x18 */ u8 m_ended;
};

/* ATK_ROCKET: fires the current launcher. */
class HeavyDog0410ModeCtrlUnit_ATK_ROCKET : public HeavyDog0410CtrlUnit {
  public:
    HeavyDog0410ModeCtrlUnit_ATK_ROCKET(HeavyDog0410VarParam *owner)
        : BossCtrlUnit(owner), HeavyDog0410CtrlUnit(owner) {
        m_owner->m_launchers[m_owner->m_launcherIndex].m_fired = 1;
    }
    virtual ~HeavyDog0410ModeCtrlUnit_ATK_ROCKET();
    virtual int Tick(float dt);
};

/* ATK_BARRIER: stops and puts up the barrier. */
class HeavyDog0410ModeCtrlUnit_ATK_BARRIER : public HeavyDog0410CtrlUnit {
  public:
    HeavyDog0410ModeCtrlUnit_ATK_BARRIER(HeavyDog0410VarParam *owner)
        : BossCtrlUnit(owner), HeavyDog0410CtrlUnit(owner) {
        m_owner->m_fixed = 0;
        m_owner->m_turnFixed = 1;
        m_time = 0.0f;
        fn_801FAF00(GetHintManager(), 0xE5, 0, 0, 0);
        m_owner->m_energy = 1.0f + m_owner->m_energyFull;
        m_state = 0;
    }
    virtual ~HeavyDog0410ModeCtrlUnit_ATK_BARRIER();
    virtual int Tick(float dt);

    /* 0x10 */ s32 m_state;
    /* 0x14 */ float m_time;
};

/* ATK_MINE: drops a mine. */
class HeavyDog0410ModeCtrlUnit_ATK_MINE : public HeavyDog0410CtrlUnit {
  public:
    HeavyDog0410ModeCtrlUnit_ATK_MINE(HeavyDog0410VarParam *owner)
        : BossCtrlUnit(owner), HeavyDog0410CtrlUnit(owner) {
        DropMine();
    }
    virtual ~HeavyDog0410ModeCtrlUnit_ATK_MINE();
    virtual int Tick(float dt);
};

class HeavyDog0410ModeCtrlUnit_MOVETEST : public HeavyDog0410CtrlUnit {
  public:
    HeavyDog0410ModeCtrlUnit_MOVETEST(HeavyDog0410VarParam *owner)
        : BossCtrlUnit(owner), HeavyDog0410CtrlUnit(owner) {}
    virtual ~HeavyDog0410ModeCtrlUnit_MOVETEST();
    virtual int Tick(float dt);
};

SIZE_ASSERT(HeavyDog0410ModeCtrlUnit_WAIT, 0x20);
SIZE_ASSERT(HeavyDog0410ModeCtrlUnit_CHASE, 0x24);
SIZE_ASSERT(HeavyDog0410ModeCtrlUnit_GET_ATK, 0x24);
SIZE_ASSERT(HeavyDog0410ModeCtrlUnit_DESTROY, 0x2C);
SIZE_ASSERT(HeavyDog0410ModeCtrlUnit_ATK_BARRIER, 0x28);

HeavyDog0410ModeCtrlUnit_MOVETEST::~HeavyDog0410ModeCtrlUnit_MOVETEST() {}

int HeavyDog0410ModeCtrlUnit_MOVETEST::Tick(float dt) {
    fn_800091D0(&m_owner->m_targetPosition, &m_owner->m_aim);
    m_owner->m_targetPosition.y = 0.0f;
    m_owner->m_nextMode = 2;
    return 0;
}

HeavyDog0410ModeCtrlUnit_ATK_MINE::~HeavyDog0410ModeCtrlUnit_ATK_MINE() {}

int HeavyDog0410ModeCtrlUnit_ATK_MINE::Tick(float dt) {
    m_owner->m_nextMode = 2;
    return 0;
}

HeavyDog0410ModeCtrlUnit_ATK_BARRIER::~HeavyDog0410ModeCtrlUnit_ATK_BARRIER() {
    m_owner->m_energy = 0.0f;
}

int HeavyDog0410ModeCtrlUnit_ATK_BARRIER::Tick(float dt) {
    fn_800091D0(&m_owner->m_targetPosition, &m_owner->m_charaPtr.get()->m_position);
    m_time += dt;
    switch (m_state) {
    case 0:
        if (m_time > 1.5f && fn_8040CF50(&m_owner->m_velocity) < m_owner->m_soundSpeed) {
            Vec3f position;
            fn_800091EC(&position, &m_owner->m_charaPtr.get()->m_position);
            if (m_owner->m_playerPosition.y > 40.0f) {
                position.y = 0.5f * m_owner->m_barrierHeight + 40.0f;
            } else {
                position.y = 0.5f * m_owner->m_barrierHeight;
            }
            fn_803358D0(m_owner, new HeavyDogCtrlUnit_Barrier(m_owner, &position, true));
            m_owner->m_energy = -1.0f;
            m_state = 1;
            m_time = 0.0f;
        }
        break;
    case 1:
        m_owner->m_distance = 0.0f;
        if (m_time > m_owner->m_barrierTime) {
            fn_801FAF00(GetHintManager(), 0xE3, 0, 0, 0);
            m_owner->m_nextMode = 2;
        }
        break;
    }
    return 0;
}

HeavyDog0410ModeCtrlUnit_ATK_ROCKET::~HeavyDog0410ModeCtrlUnit_ATK_ROCKET() {}

int HeavyDog0410ModeCtrlUnit_ATK_ROCKET::Tick(float dt) {
    m_owner->m_nextMode = 2;
    return 0;
}

HeavyDog0410ModeCtrlUnit_DESTROY::~HeavyDog0410ModeCtrlUnit_DESTROY() {
    boost::shared_ptr<EffectElement> effect = m_effect.lock();
    if (effect) {
        effect->Stop();
        fn_800A4210(effect);
    }
}

int HeavyDog0410ModeCtrlUnit_DESTROY::Tick(float dt) {
    if (!m_ended) {
        UnkMotionFrames frames;
        fn_80415D40(m_owner->m_motion, 0, &frames);
        if (frames.m_frame > frames.m_end - 60.0f) {
            fn_80175EEC(fn_80074470(), 6, 0);
            m_ended = 1;
        }
    }
    return 0;
}

HeavyDog0410ModeCtrlUnit_BELOW_HALF::~HeavyDog0410ModeCtrlUnit_BELOW_HALF() {
    fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 1);
    m_owner->m_stopped = 0;
    m_owner->m_active = 1;
    m_owner->m_energyFull *= 0.5f;
    m_owner->m_energy = m_owner->m_energyFull;
}

int HeavyDog0410ModeCtrlUnit_BELOW_HALF::Tick(float dt) {
    if (fn_80416F28(m_owner->m_motion, 0, 3, 1)) {
        m_owner->m_nextMode = 2;
    }
    return 0;
}

HeavyDog0410ModeCtrlUnit_START::~HeavyDog0410ModeCtrlUnit_START() {
    fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 1);
    m_owner->m_stopped = 0;
    m_owner->m_active = 1;
}

int HeavyDog0410ModeCtrlUnit_START::Tick(float dt) {
    if (fn_80416F28(m_owner->m_motion, 0, 3, 1)) {
        m_owner->m_nextMode = 2;
    }
    return 0;
}

HeavyDog0410ModeCtrlUnit_GET_ATK::~HeavyDog0410ModeCtrlUnit_GET_ATK() {
    m_owner->m_stopped = 0;
}

int HeavyDog0410ModeCtrlUnit_GET_ATK::Tick(float dt) {
    if (m_owner->m_stopped) {
        m_time += lbl_8057D6A0.field_0x18;
        if (m_time > m_owner->field_0x98) {
            m_owner->m_nextMode = 2;
        }
    }
    return 0;
}

HeavyDog0410ModeCtrlUnit_ESCAPE::~HeavyDog0410ModeCtrlUnit_ESCAPE() {}

int HeavyDog0410ModeCtrlUnit_ESCAPE::Tick(float dt) {
    /* Retail copies the corners from temporaries through fn_800091EC; named temporaries
     * declared after the other locals take their stack slots. */
    Vec3f corners[4];
    Mtx34f matrix;
    Vec3f corner;
    Vec3f offset;
    Vec3f direction;
    Vec3f heading;
    Vec3f position;
    Vec3f body;
    Vec3f target;
    Vec3f velocity;
    Vec3f corner0(300.0f, 0.0f, 300.0f);
    fn_800091EC(&corners[0], &corner0);
    Vec3f corner1(300.0f, 0.0f, -300.0f);
    fn_800091EC(&corners[1], &corner1);
    Vec3f corner2(-300.0f, 0.0f, -300.0f);
    fn_800091EC(&corners[2], &corner2);
    Vec3f corner3(-300.0f, 0.0f, 300.0f);
    fn_800091EC(&corners[3], &corner3);
    int i;
    int farthest = -1;
    int nearest = -1;
    float value; /* the best direction found; then the mine's throw scales */
    float nearestDistance = 10000.0f;
    for (i = 0; i < 4; i++) {
        fn_800091D0(&offset, &Ref(Sub(corners[i], m_owner->m_playerPosition)));
        offset.y = 0.0f;
        float distance = fn_8040CF50(&offset);
        if (distance < nearestDistance) {
            nearest = i;
            nearestDistance = distance;
        }
    }
    value = -1.0f;
    for (i = 0; i < 4; i++) {
        if (i != nearest) {
            fn_800091D0(&offset, &Ref(Sub(m_owner->m_charaPtr.get()->m_position,
                                          m_owner->m_playerPosition)));
            offset.y = 0.0f;
            fn_8040CE74(&offset);
            fn_800091D0(&direction, &Ref(Sub(corners[i], m_owner->m_playerPosition)));
            direction.y = 0.0f;
            fn_8040CE74(&direction);
            float dot = fn_8040CF90(&offset, &direction);
            if (dot > value) {
                farthest = i;
                value = dot;
            }
        }
    }
    if (farthest != -1) {
        fn_800091D0(&corner, &corners[farthest]);
        fn_800091D0(&offset, &Ref(Sub(corner, m_owner->m_charaPtr.get()->m_position)));
        offset.y = 0.0f;
        float angle = fn_80016260(-offset.x, -offset.z);
        fn_800091EC(&heading, &lbl_805E07A0);
        if (angle > 0.5f * lbl_805FAD38) {
            heading.x = -1.0f;
            heading.z = 1.0f;
        } else if (angle > 0.0f) {
            heading.x = -1.0f;
            heading.z = -1.0f;
        } else if (angle > 0.5f * -lbl_805FAD38) {
            heading.x = 1.0f;
            heading.z = -1.0f;
        } else {
            heading.x = 1.0f;
            heading.z = 1.0f;
        }
        fn_8040CE38(&heading, m_owner->field_0xE0);
        fn_800091D0(&m_owner->m_targetPosition,
                    &Ref(fn_8040CDD0(&m_owner->m_charaPtr.get()->m_position, &heading)));
    }
    if (m_owner->m_energy >= m_owner->m_energyFull) {
        m_owner->m_nextMode = 2;
        fn_801FAF00(GetHintManager(), 0xE4, 0, 0, 0);
    }
    if (m_owner->m_aimDistance < m_owner->m_barrierRadius) {
        if (m_owner->m_distance > m_owner->field_0x1D8) {
            Mtx34f *node = fn_80415E70(m_owner->m_motion, 0x1194);
            fn_80016370(&matrix, node);
            fn_800091EC(&position, &Ref(fn_8040FB08(&matrix)));
            fn_80016370(&matrix, fn_80415E70(m_owner->m_motion, 0xFAA));
            fn_800091EC(&body, &Ref(fn_8040FB08(&matrix)));
            fn_800091EC(&target, &Ref(Sub(m_owner->m_playerPosition, body)));
            value = m_owner->field_0x1DC;
            fn_800091D0(&target,
                        &Ref(fn_8040CDD0(&Ref(fn_8040CDA8(&Ref(fn_8040CEE8(&target)), value)), &body)));
            fn_800091EC(&velocity, &Ref(Sub(m_owner->m_playerPosition, body)));
            velocity.y = 0.0f;
            value = m_owner->field_0x1E0;
            fn_800091D0(&velocity, &Ref(fn_8040CDA8(&Ref(fn_8040CEE8(&velocity)), value)));
            velocity.y = m_owner->field_0x1E4;
            fn_8040CFF8(&velocity, &m_owner->m_velocity);
            fn_803358D0(m_owner, new HeavyDog0410CtrlUnit_Mine(m_owner, position, velocity, target));
            m_owner->m_distance = 0.0f;
        }
    } else {
        m_owner->m_launchers[m_owner->m_launcherIndex].m_fired = 1;
    }
    return 0;
}

HeavyDog0410ModeCtrlUnit_CHASE::~HeavyDog0410ModeCtrlUnit_CHASE() {}

int HeavyDog0410ModeCtrlUnit_CHASE::Tick(float dt) {
    m_time += dt;
    Vec3f direction;
    fn_800091EC(&direction, &lbl_805E07A0);
    if (m_owner->m_aimDirection > 0.5f * lbl_805FAD38) {
        direction.x = -1.0f;
        direction.z = 1.0f;
    } else if (m_owner->m_aimDirection > 0.0f) {
        direction.x = -1.0f;
        direction.z = -1.0f;
    } else if (m_owner->m_aimDirection > 0.5f * -lbl_805FAD38) {
        direction.x = 1.0f;
        direction.z = -1.0f;
    } else {
        direction.x = 1.0f;
        direction.z = 1.0f;
    }
    fn_8040CE38(&direction, m_owner->field_0xDC);
    fn_800091D0(&m_owner->m_targetPosition,
                &Ref(fn_8040CDD0(&m_owner->m_charaPtr.get()->m_position, &direction)));
    if (m_time > 1.5f && m_owner->m_aimDistance < m_owner->m_barrierRadius - 30.0f) {
        m_owner->m_nextMode = 10;
    }
    m_owner->m_launchers[m_owner->m_launcherIndex].m_fired = 1;
    return 0;
}

HeavyDog0410ModeCtrlUnit_EDIT::~HeavyDog0410ModeCtrlUnit_EDIT() {
    m_owner->m_fixed = 0;
    m_owner->m_turnFixed = 0;
}

int HeavyDog0410ModeCtrlUnit_EDIT::Tick(float dt) {
    return 0;
}

HeavyDog0410ModeCtrlUnit_WAIT::~HeavyDog0410ModeCtrlUnit_WAIT() {}

int HeavyDog0410ModeCtrlUnit_WAIT::Tick(float dt) {
    if (m_owner->m_energy >= m_owner->m_energyFull) {
        m_owner->m_nextMode = 3;
    } else {
        m_owner->m_nextMode = 4;
    }
    fn_801D54F4(fn_80053164(), 0x7D3F, m_owner->m_charaPtr.get(), 0, 1);
    return 0;
}

BossCtrlUnit *HeavyDog0410ModeCtrlUnit::CreateMode(int mode) {
    switch (mode) {
    case 0:
    case 2:
    default:
        return new HeavyDog0410ModeCtrlUnit_WAIT(m_owner);
    case 1:
        return new HeavyDog0410ModeCtrlUnit_EDIT(m_owner);
    case 3:
        return new HeavyDog0410ModeCtrlUnit_CHASE(m_owner);
    case 4:
        return new HeavyDog0410ModeCtrlUnit_ESCAPE(m_owner);
    case 5:
        return new HeavyDog0410ModeCtrlUnit_GET_ATK(m_owner);
    case 6:
        return new HeavyDog0410ModeCtrlUnit_START(m_owner);
    case 7:
        return new HeavyDog0410ModeCtrlUnit_BELOW_HALF(m_owner);
    case 8:
        return new HeavyDog0410ModeCtrlUnit_DESTROY(m_owner);
    case 9:
        return new HeavyDog0410ModeCtrlUnit_ATK_ROCKET(m_owner);
    case 10:
        return new HeavyDog0410ModeCtrlUnit_ATK_BARRIER(m_owner);
    case 11:
        return new HeavyDog0410ModeCtrlUnit_ATK_MINE(m_owner);
    case 12:
        return new HeavyDog0410ModeCtrlUnit_MOVETEST(m_owner);
    }
}

HeavyDog0410ModeCtrlUnit::~HeavyDog0410ModeCtrlUnit() {}

int HeavyDog0410ModeCtrlUnit::Tick(float dt) {
    return BossModeCtrlUnit::Tick(dt);
}
