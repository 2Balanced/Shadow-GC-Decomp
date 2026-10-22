/* HeavyDog0510ModeCtrlUnit (class name from RTTI): the mode control unit of the Heavy Dog boss's
 * 0510 stage, the fight in the air. CreateMode builds one HeavyDog0510ModeCtrlUnit_<MODE> unit
 * (class names from RTTI) for each of the stage's modes, as in Game/HeavyDog0410ModeCtrlUnit.cpp.
 * This unit also holds the stage's unit base, HeavyDog0510CtrlUnit, whose constructor and
 * destructor are inline and emitted after their first users. File name inferred from the class.
 * Field and helper names are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first).
 *
 * Every function and data object matches, but the unit does not link to retail: retail lays out
 * HeavyDog0510CtrlUnit's vtable after the mode units' (and its destructor thunk before theirs),
 * while GC/2.7 generates it with the first inline constructor it compiles, before them. No
 * placement of the inline constructor and destructor (in the unit, in the header, in the class,
 * a non-inline destructor, an unused out-of-line user) changes that. */
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

/* The game state (as in Game/HeavyDog0410ModeCtrlUnit.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x18];
    /* 0x018 */ float field_0x18;
};
extern UnkGameState_8057D6A0 lbl_8057D6A0;

extern const Vec3f lbl_805E07B8;

/* A bomb the boss drops (defined in Game/HeavyDog0510CtrlUnit.cpp). */
class HeavyDog0510CtrlUnit_Bomb : public HeavyDog0510CtrlUnit {
  public:
    HeavyDog0510CtrlUnit_Bomb(HeavyDog0510VarParam *param, const Vec3f &position,
                              const Vec3f &velocity);
    virtual ~HeavyDog0510CtrlUnit_Bomb();
    virtual int Tick(float dt);

    /* 0x10 */ u8 field_0x10[0x8];
};

extern "C" {
void fn_80417568(_stMtnPlayWork *work, u16 motion);
void fn_804165D8(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
void fn_80416148(_stMtnPlayWork *work, int arg1, float arg2);
int fn_80416F28(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
void fn_80415D40(_stMtnPlayWork *work, int arg1, UnkMotionFrames *frames);
Mtx34f *fn_80415E70(_stMtnPlayWork *work, int node);
void fn_80016370(Mtx34f *matrix, void *frame);

s32 fn_801FAF00(UnkHintManager_801FB180 *manager, s32 id, s32 branch, int arg3, int arg4);
void fn_8035367C(const char *file, const char *layout, const char *animation);
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int arg1);
UnkObject_80074470 *fn_80074470(void);
void fn_80175EEC(UnkObject_80074470 *object, int arg1, int arg2);
EnemyCharInfo *fn_801792D0(EnemyCharInfo *const *body);

/* A random number from 0 to 1 (behaviour inferred). */
float fn_8040EE90(void);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
Vec3f fn_8040FB08(const Mtx34f *matrix);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
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

/* The boss's body as its character info (as in Game/HeavyDog0410ModeCtrlUnit.cpp). */
#define Body(owner) fn_801792D0(reinterpret_cast<EnemyCharInfo *const *>(&(owner)->m_charaPtr))

/* Drops a bomb from the boss's hatch, thrown out from the centre with a random spread (name
 * inferred; written out where used). */
#define DropBomb()                                                                                 \
    do {                                                                                           \
        Mtx34f *node = fn_80415E70(m_owner->m_motion, 0x1194);                                     \
        Mtx34f matrix;                                                                             \
        fn_80016370(&matrix, node);                                                                \
        Vec3f position;                                                                            \
        fn_800091EC(&position, &Ref(fn_8040FB08(&matrix)));                                        \
        Vec3f velocity;                                                                            \
        fn_800091EC(&velocity, &Ref(Sub(lbl_805E07A0, m_owner->m_charaPtr.get()->m_position)));    \
        velocity.y = 0.0f;                                                                         \
        float speed = m_owner->m_bombSpeed;                                                        \
        fn_800091D0(&velocity, &Ref(fn_8040CDA8(&Ref(fn_8040CEE8(&velocity)), speed)));           \
        Vec3f spread(m_owner->m_bombSpread * fn_8040EE90(), 0.0f, 0.0f);                           \
        fn_8040C148(&matrix, &lbl_805E0720);                                                       \
        fn_8040B848(&matrix, lbl_805FAD38 * fn_8040EE90());                                        \
        fn_800091D0(&spread, &Ref(fn_8040B67C(&matrix, &spread)));                                 \
        fn_8040CFF8(&velocity, &spread);                                                           \
        fn_803358D0(m_owner, new HeavyDog0510CtrlUnit_Bomb(m_owner, position, velocity));          \
    } while (0)


inline HeavyDog0510CtrlUnit::HeavyDog0510CtrlUnit(HeavyDog0510VarParam *owner)
    : BossCtrlUnit(owner), HeavyDogCtrlUnit(owner), m_owner(owner) {}

inline HeavyDog0510CtrlUnit::~HeavyDog0510CtrlUnit() {}

/* WAIT: waits for the next attack. */
class HeavyDog0510ModeCtrlUnit_WAIT : public HeavyDog0510CtrlUnit {
  public:
    HeavyDog0510ModeCtrlUnit_WAIT(HeavyDog0510VarParam *owner)
        : BossCtrlUnit(owner), HeavyDog0510CtrlUnit(owner) {
        m_owner->m_fixed = 0;
        m_owner->m_turnFixed = 0;
    }
    virtual ~HeavyDog0510ModeCtrlUnit_WAIT();
    virtual int Tick(float dt);
};

class HeavyDog0510ModeCtrlUnit_EDIT : public HeavyDog0510CtrlUnit {
  public:
    HeavyDog0510ModeCtrlUnit_EDIT(HeavyDog0510VarParam *owner)
        : BossCtrlUnit(owner), HeavyDog0510CtrlUnit(owner) {
        m_owner->m_fixed = 1;
    }
    virtual ~HeavyDog0510ModeCtrlUnit_EDIT();
    virtual int Tick(float dt);
};

/* CHASE: follows the player's height, dropping bombs from above. */
class HeavyDog0510ModeCtrlUnit_CHASE : public HeavyDog0510CtrlUnit {
  public:
    HeavyDog0510ModeCtrlUnit_CHASE(HeavyDog0510VarParam *owner)
        : BossCtrlUnit(owner), HeavyDog0510CtrlUnit(owner) {
        m_owner->m_fixed = 0;
        m_owner->m_turnFixed = 0;
        m_time = 0.0f;
        m_count = 0;
    }
    virtual ~HeavyDog0510ModeCtrlUnit_CHASE();
    virtual int Tick(float dt);

    /* 0x10 */ float m_time;
    /* 0x14 */ s32 m_count;
};

/* ESCAPE: turns away from the player, dropping bombs. */
class HeavyDog0510ModeCtrlUnit_ESCAPE : public HeavyDog0510CtrlUnit {
  public:
    HeavyDog0510ModeCtrlUnit_ESCAPE(HeavyDog0510VarParam *owner)
        : BossCtrlUnit(owner), HeavyDog0510CtrlUnit(owner) {
        m_owner->m_fixed = 0;
        m_owner->m_turnFixed = 0;
        m_owner->m_targetAngle = m_owner->m_aimAngle + lbl_805FAD38;
        m_owner->m_damaged = 0;
        m_time = 0.0f;
        m_count = 0;
    }
    virtual ~HeavyDog0510ModeCtrlUnit_ESCAPE();
    virtual int Tick(float dt);

    /* 0x10 */ float m_time;
    /* 0x14 */ s32 m_count;
};

/* GET_ATK: the boss is hit. */
class HeavyDog0510ModeCtrlUnit_GET_ATK : public HeavyDog0510CtrlUnit {
  public:
    HeavyDog0510ModeCtrlUnit_GET_ATK(HeavyDog0510VarParam *owner)
        : BossCtrlUnit(owner), HeavyDog0510CtrlUnit(owner) {
        m_owner->m_stopped = 1;
        m_owner->m_fixed = 1;
        m_owner->field_0x1D0++;
        m_time = 0.0f;
    }
    virtual ~HeavyDog0510ModeCtrlUnit_GET_ATK();
    virtual int Tick(float dt);

    /* 0x10 */ float m_time;
};

/* START: the boss's entrance. */
class HeavyDog0510ModeCtrlUnit_START : public HeavyDog0510CtrlUnit {
  public:
    HeavyDog0510ModeCtrlUnit_START(HeavyDog0510VarParam *owner)
        : BossCtrlUnit(owner), HeavyDog0510CtrlUnit(owner) {
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 3);
        fn_800091D0(&m_owner->m_charaPtr.get()->m_position, &lbl_805E07A0);
        Body(m_owner)->m_rotation = lbl_805E07B8;
        m_owner->m_stopped = 1;
        m_owner->m_fixed = 1;
        m_owner->m_turnFixed = 1;
        fn_803358D0(m_owner, new BossCtrlUnit_PlayCameraMotion(m_owner, 0xA, &lbl_805E07A0,
                                                                &lbl_805E07B8, 0.0f, 0.0f));
        m_owner->m_active = 0;
        fn_801FAF00(GetHintManager(), 0xEC, 0, 0, 0);
        fn_8035367C("CsdFiles/BChara_BFalcon", "bchara", "open");
    }
    virtual ~HeavyDog0510ModeCtrlUnit_START();
    virtual int Tick(float dt);
};

/* BELOW_HALF: the cut-in when the boss's health falls below half. */
class HeavyDog0510ModeCtrlUnit_BELOW_HALF : public HeavyDog0510CtrlUnit {
  public:
    HeavyDog0510ModeCtrlUnit_BELOW_HALF(HeavyDog0510VarParam *owner)
        : BossCtrlUnit(owner), HeavyDog0510CtrlUnit(owner) {
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 4);
        m_owner->m_stopped = 1;
        m_owner->m_fixed = 1;
        m_owner->m_turnFixed = 1;
        fn_803358D0(m_owner, new BossCtrlUnit_PlayCameraMotion(m_owner, 8, 0.0f, 0.0f));
        m_owner->m_active = 0;
        fn_801FAF00(GetHintManager(), 0xEE, 0, 0, 0);
        fn_8034DF48(0xC, -2);
    }
    virtual ~HeavyDog0510ModeCtrlUnit_BELOW_HALF();
    virtual int Tick(float dt);
};

/* DESTROY: the boss is destroyed and the stage ends. */
class HeavyDog0510ModeCtrlUnit_DESTROY : public HeavyDog0510CtrlUnit {
  public:
    HeavyDog0510ModeCtrlUnit_DESTROY(HeavyDog0510VarParam *owner)
        : BossCtrlUnit(owner), HeavyDog0510CtrlUnit(owner) {
        m_ended = 0;
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 2);
        fn_800091D0(&m_owner->m_charaPtr.get()->m_position, &lbl_805E07A0);
        Body(m_owner)->m_rotation = lbl_805E07B8;
        m_owner->m_stopped = 1;
        m_owner->m_fixed = 1;
        m_owner->m_turnFixed = 1;
        fn_803358D0(m_owner, new BossCtrlUnit_PlayCameraMotion(m_owner, 9, &lbl_805E07A0,
                                                                &lbl_805E07B8, 0.0f, 0.0f));
        m_owner->m_active = 0;
        fn_801FAF00(GetHintManager(), 0xEF, 0, 0, 0);
        fn_803358D0(m_owner, new HeavyDogCtrlUnit_DestroyEffect(m_owner));
        fn_8034DF48(7, -2);
    }
    virtual ~HeavyDog0510ModeCtrlUnit_DESTROY();
    virtual int Tick(float dt);

    /* 0x10 */ u8 m_ended;
};

/* ATK_ROCKET: fires the current launcher. */
class HeavyDog0510ModeCtrlUnit_ATK_ROCKET : public HeavyDog0510CtrlUnit {
  public:
    HeavyDog0510ModeCtrlUnit_ATK_ROCKET(HeavyDog0510VarParam *owner)
        : BossCtrlUnit(owner), HeavyDog0510CtrlUnit(owner) {
        m_owner->m_launchers[m_owner->m_launcherIndex].m_fired = 1;
    }
    virtual ~HeavyDog0510ModeCtrlUnit_ATK_ROCKET();
    virtual int Tick(float dt);
};

/* ATK_BARRIER: stops and puts up the barrier. */
class HeavyDog0510ModeCtrlUnit_ATK_BARRIER : public HeavyDog0510CtrlUnit {
  public:
    HeavyDog0510ModeCtrlUnit_ATK_BARRIER(HeavyDog0510VarParam *owner)
        : BossCtrlUnit(owner), HeavyDog0510CtrlUnit(owner) {
        m_owner->m_fixed = 0;
        m_owner->m_turnFixed = 1;
        m_time = 0.0f;
        fn_801FAF00(GetHintManager(), 0xF2, 0, 0, 0);
        m_owner->m_energy = 1.0f + m_owner->m_energyFull;
        m_state = 0;
    }
    virtual ~HeavyDog0510ModeCtrlUnit_ATK_BARRIER();
    virtual int Tick(float dt);

    /* 0x10 */ float m_time;
    /* 0x14 */ s32 m_state;
};

/* ATK_MINE: drops a bomb. */
class HeavyDog0510ModeCtrlUnit_ATK_MINE : public HeavyDog0510CtrlUnit {
  public:
    HeavyDog0510ModeCtrlUnit_ATK_MINE(HeavyDog0510VarParam *owner)
        : BossCtrlUnit(owner), HeavyDog0510CtrlUnit(owner) {
        DropBomb();
    }
    virtual ~HeavyDog0510ModeCtrlUnit_ATK_MINE();
    virtual int Tick(float dt);
};

class HeavyDog0510ModeCtrlUnit_MOVETEST : public HeavyDog0510CtrlUnit {
  public:
    HeavyDog0510ModeCtrlUnit_MOVETEST(HeavyDog0510VarParam *owner)
        : BossCtrlUnit(owner), HeavyDog0510CtrlUnit(owner) {}
    virtual ~HeavyDog0510ModeCtrlUnit_MOVETEST();
    virtual int Tick(float dt);
};

SIZE_ASSERT(HeavyDog0510ModeCtrlUnit_WAIT, 0x20);
SIZE_ASSERT(HeavyDog0510ModeCtrlUnit_CHASE, 0x28);
SIZE_ASSERT(HeavyDog0510ModeCtrlUnit_ESCAPE, 0x28);
SIZE_ASSERT(HeavyDog0510ModeCtrlUnit_GET_ATK, 0x24);
SIZE_ASSERT(HeavyDog0510ModeCtrlUnit_DESTROY, 0x24);
SIZE_ASSERT(HeavyDog0510ModeCtrlUnit_ATK_BARRIER, 0x28);

HeavyDog0510ModeCtrlUnit_MOVETEST::~HeavyDog0510ModeCtrlUnit_MOVETEST() {}

int HeavyDog0510ModeCtrlUnit_MOVETEST::Tick(float dt) {
    m_owner->m_targetPosition.y = m_owner->m_aim.y;
    m_owner->m_targetAngle = m_owner->m_aimAngle;
    m_owner->m_nextMode = 2;
    return 0;
}

HeavyDog0510ModeCtrlUnit_ATK_MINE::~HeavyDog0510ModeCtrlUnit_ATK_MINE() {}

int HeavyDog0510ModeCtrlUnit_ATK_MINE::Tick(float dt) {
    m_owner->m_nextMode = 2;
    return 0;
}

HeavyDog0510ModeCtrlUnit_ATK_BARRIER::~HeavyDog0510ModeCtrlUnit_ATK_BARRIER() {
    m_owner->m_energy = 0.0f;
}

int HeavyDog0510ModeCtrlUnit_ATK_BARRIER::Tick(float dt) {
    m_owner->m_targetPosition.y = m_owner->m_charaPtr.get()->m_position.y;
    m_owner->m_targetAngle = m_owner->m_angle;
    m_time += dt;
    switch (m_state) {
    case 0:
        if (m_time > 1.5f && fn_8040CF50(&m_owner->m_velocity) < m_owner->m_soundSpeed) {
            Vec3f position;
            fn_800091EC(&position, &m_owner->m_charaPtr.get()->m_position);
            position.y = 0.5f * m_owner->m_barrierHeight + position.y;
            fn_803358D0(m_owner, new HeavyDogCtrlUnit_Barrier(m_owner, &position, false));
            m_owner->m_energy = -1.0f;
            m_state = 1;
            m_time = 0.0f;
        }
        break;
    case 1:
        if (m_time > m_owner->m_barrierTime) {
            fn_801FAF00(GetHintManager(), 0xF0, 0, 0, 0);
            m_owner->m_nextMode = 2;
        }
        break;
    }
    return 0;
}

HeavyDog0510ModeCtrlUnit_ATK_ROCKET::~HeavyDog0510ModeCtrlUnit_ATK_ROCKET() {}

int HeavyDog0510ModeCtrlUnit_ATK_ROCKET::Tick(float dt) {
    m_owner->m_nextMode = 2;
    return 0;
}

HeavyDog0510ModeCtrlUnit_DESTROY::~HeavyDog0510ModeCtrlUnit_DESTROY() {}

int HeavyDog0510ModeCtrlUnit_DESTROY::Tick(float dt) {
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

HeavyDog0510ModeCtrlUnit_BELOW_HALF::~HeavyDog0510ModeCtrlUnit_BELOW_HALF() {
    fn_804165D8(m_owner->m_motion, 0, -1, -1);
    fn_80416148(m_owner->m_motion, 0, 40.0f);
    fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 1);
    m_owner->m_stopped = 0;
    m_owner->m_active = 1;
}

int HeavyDog0510ModeCtrlUnit_BELOW_HALF::Tick(float dt) {
    if (fn_80416F28(m_owner->m_motion, 0, 3, 1)) {
        m_owner->m_nextMode = 2;
    }
    return 0;
}

HeavyDog0510ModeCtrlUnit_START::~HeavyDog0510ModeCtrlUnit_START() {
    fn_804165D8(m_owner->m_motion, 0, -1, -1);
    fn_80416148(m_owner->m_motion, 0, 20.0f);
    fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 1);
    m_owner->m_stopped = 0;
    m_owner->m_active = 1;
}

int HeavyDog0510ModeCtrlUnit_START::Tick(float dt) {
    if (fn_80416F28(m_owner->m_motion, 0, 3, 1)) {
        m_owner->m_nextMode = 2;
    }
    return 0;
}

HeavyDog0510ModeCtrlUnit_GET_ATK::~HeavyDog0510ModeCtrlUnit_GET_ATK() {
    m_owner->m_stopped = 0;
}

int HeavyDog0510ModeCtrlUnit_GET_ATK::Tick(float dt) {
    if (m_owner->m_stopped) {
        m_time += lbl_8057D6A0.field_0x18;
        if (m_time > m_owner->field_0x98) {
            m_owner->m_nextMode = 2;
        }
    }
    return 0;
}

HeavyDog0510ModeCtrlUnit_ESCAPE::~HeavyDog0510ModeCtrlUnit_ESCAPE() {}

int HeavyDog0510ModeCtrlUnit_ESCAPE::Tick(float dt) {
    m_time += dt;
    m_owner->m_targetPosition.y = m_owner->m_aim.y;
    if (m_owner->m_targetPosition.y < 0.0f) {
        m_owner->m_targetPosition.y = 0.0f;
    } else if (m_owner->m_targetPosition.y > 180.0f) {
        m_owner->m_targetPosition.y = 180.0f;
    }
    if (m_owner->m_damaged) {
        m_owner->m_targetAngle = m_owner->m_aimAngle + lbl_805FAD38;
        m_owner->m_damaged = 0;
    }
    if (m_owner->m_energy >= m_owner->m_energyFull) {
        m_owner->m_nextMode = 2;
        fn_801FAF00(GetHintManager(), 0xF1, 0, 0, 0);
    }
    if (m_owner->m_aim.y < -52.0f) {
        if (m_time > m_owner->m_bombInterval) {
            DropBomb();
            if (++m_count >= m_owner->m_bombCount) {
                m_count = 0;
                m_time = 0.0f;
            } else {
                m_time = m_owner->m_bombInterval - 0.1f;
            }
        }
    } else {
        m_owner->m_launchers[m_owner->m_launcherIndex].m_fired = 1;
    }
    return 0;
}

HeavyDog0510ModeCtrlUnit_CHASE::~HeavyDog0510ModeCtrlUnit_CHASE() {}

int HeavyDog0510ModeCtrlUnit_CHASE::Tick(float dt) {
    m_time += dt;
    m_owner->m_targetPosition.y = m_owner->m_aim.y;
    if (m_owner->m_targetPosition.y < 0.0f) {
        m_owner->m_targetPosition.y = 0.0f;
    } else if (m_owner->m_targetPosition.y > 200.0f) {
        m_owner->m_targetPosition.y = 200.0f;
    }
    if (fn_8040CF50(&m_owner->m_aim) < 10.0f) {
        m_owner->m_targetAngle = m_owner->m_aimAngle + lbl_805FAD38;
    } else {
        m_owner->m_targetAngle = m_owner->m_aimAngle;
    }
    if (m_owner->m_aimDistance < m_owner->m_barrierRadius - 30.0f &&
        m_owner->m_aimHeight < m_owner->m_turnDistance) {
        m_owner->m_nextMode = 10;
    }
    if (m_owner->m_aim.y < -52.0f) {
        if (m_time > m_owner->m_bombInterval) {
            DropBomb();
            if (++m_count >= m_owner->m_bombCount) {
                m_count = 0;
                m_time = 0.0f;
            } else {
                m_time = m_owner->m_bombInterval - 0.1f;
            }
        }
    } else {
        m_owner->m_launchers[m_owner->m_launcherIndex].m_fired = 1;
    }
    return 0;
}

HeavyDog0510ModeCtrlUnit_EDIT::~HeavyDog0510ModeCtrlUnit_EDIT() {
    m_owner->m_fixed = 0;
}

int HeavyDog0510ModeCtrlUnit_EDIT::Tick(float dt) {
    return 0;
}

HeavyDog0510ModeCtrlUnit_WAIT::~HeavyDog0510ModeCtrlUnit_WAIT() {}

int HeavyDog0510ModeCtrlUnit_WAIT::Tick(float dt) {
    if (m_owner->m_energy >= m_owner->m_energyFull) {
        m_owner->m_nextMode = 3;
    } else {
        m_owner->m_nextMode = 4;
    }
    return 0;
}

BossCtrlUnit *HeavyDog0510ModeCtrlUnit::CreateMode(int mode) {
    switch (mode) {
    case 0:
    case 2:
    default:
        return new HeavyDog0510ModeCtrlUnit_WAIT(m_owner);
    case 1:
        return new HeavyDog0510ModeCtrlUnit_EDIT(m_owner);
    case 3:
        return new HeavyDog0510ModeCtrlUnit_CHASE(m_owner);
    case 4:
        return new HeavyDog0510ModeCtrlUnit_ESCAPE(m_owner);
    case 5:
        return new HeavyDog0510ModeCtrlUnit_GET_ATK(m_owner);
    case 6:
        return new HeavyDog0510ModeCtrlUnit_START(m_owner);
    case 7:
        return new HeavyDog0510ModeCtrlUnit_BELOW_HALF(m_owner);
    case 8:
        return new HeavyDog0510ModeCtrlUnit_DESTROY(m_owner);
    case 9:
        return new HeavyDog0510ModeCtrlUnit_ATK_ROCKET(m_owner);
    case 10:
        return new HeavyDog0510ModeCtrlUnit_ATK_BARRIER(m_owner);
    case 11:
        return new HeavyDog0510ModeCtrlUnit_ATK_MINE(m_owner);
    case 12:
        return new HeavyDog0510ModeCtrlUnit_MOVETEST(m_owner);
    }
}

HeavyDog0510ModeCtrlUnit::~HeavyDog0510ModeCtrlUnit() {}

int HeavyDog0510ModeCtrlUnit::Tick(float dt) {
    return BossModeCtrlUnit::Tick(dt);
}

