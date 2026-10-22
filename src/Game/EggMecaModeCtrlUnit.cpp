/* EggMecaModeCtrlUnit (class name from RTTI): the mode control unit of the Egg Breaker boss
 * (EggMeca), shared by its three stages 310, 411 and 511. CreateMode builds one
 * EggMecaModeCtrlUnit_<MODE> unit (class names from RTTI) for each mode; each one sets up its
 * motion in its constructor, runs in Tick (which asks the boss for the next mode by setting
 * BossVarParam::m_nextMode) and cleans up in its destructor. Where the stages differ, the code
 * switches on the stage number. File name inferred from the class (as
 * Game/BlackBull0412ModeCtrlUnit.cpp). Field and helper names are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first); the mode units'
 * constructors are inline and are emitted after CreateMode, their first user.
 *
 * Every function and data object matches, but the unit does not link to retail. ATK_BOMB keeps its
 * bombs in shared_ptrs, so this unit emits the control block sp_counted_base_impl<
 * EggMecaCtrlUnit_Bomb *, checked_deleter<...> >. Retail places that class's vtable (and its RTTI
 * record and name string) between ATK_MOVE's and ATK_BOMB's, where the class is instantiated;
 * GC/2.7 emits the vtables of instantiated class templates as one group ahead of the mode units'
 * vtables, whatever the source does (an explicit instantiation, an explicit specialization or a
 * use of the class at that point were tried). See docs/research-notes.md on the game's compiler
 * being newer than GC/2.7. */
#define BOSSVARPARAM_CHARA_SHARED_PTR
#include "Effect/System/EffectElement.h"
#include "Game/BossCtrlUnit.h"
#include "Game/BossRootTask.h"
#include "Game/CharInfo.h"
#include "Game/EffectPJSLauncher.h"
#include "Game/EggMecaCtrlUnit.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/PooledList.h"
#include "Game/Vibration.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"
#include <cmath>

using Effect::System::EffectElement;
using Effect::System::EffectLaunchInfo;

struct _stMtnPlayWork;
struct UnkHintManager_801FB180;
struct UnkObject_80074470;
class UnkCharaList_80074194;

/* What fn_80415D40 fills in about a motion (as in BlackBull0412ModeCtrlUnit.cpp). */
struct UnkMotionFrames {
    /* 0x00 */ float m_start;
    /* 0x04 */ float m_end;
    /* 0x08 */ float field_0x8;
    /* 0x0C */ float m_frame;
};

/* A model frame's matrix as fn_80016390 copies it (RenderWare's RwMatrix layout). */
struct RwV3d {
    float x;
    float y;
    float z;
};

struct RwMatrix {
    /* 0x00 */ RwV3d right;
    /* 0x0C */ u32 flags;
    /* 0x10 */ RwV3d up;
    /* 0x1C */ u32 pad1;
    /* 0x20 */ RwV3d at;
    /* 0x2C */ u32 pad2;
    /* 0x30 */ RwV3d pos;
    /* 0x3C */ u32 pad3;
};

/* A segment fn_80020A20 builds for the collision queries (as in BlackBullCtrlUnit.cpp). */
struct UnkLine_80020A20 {
    /* 0x00 */ u8 field_0x0[0x18];
};

/* What fn_80021E24 returns: the first thing the segment hits. */
struct UnkLineHit_80021E24 {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ void *m_object;
};

/* The effect manager fn_800A31F0 returns (as in BlackBull0412ModeCtrlUnit.cpp). */
class UnkEffectManager_800A31F0 {
  public:
    virtual void vf08();
    virtual boost::weak_ptr<EffectElement> LaunchId(s32 id, s32 arg);
    virtual boost::weak_ptr<EffectElement> Launch(boost::shared_ptr<EffectLaunchInfo> info, s32 id);
};

/* The game state: the frame times at 0x18 and 0x20, the one effects count with at 0x24, and the
 * stage number at 0xA8 (names inferred). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x18];
    /* 0x018 */ float m_stunDt;
    /* 0x01C */ u32 field_0x1C;
    /* 0x020 */ float m_dt;
    /* 0x024 */ float m_effectDt;
    /* 0x028 */ u8 field_0x28[0xA8 - 0x28];
    /* 0x0A8 */ s32 m_stageId;
};
extern UnkGameState_8057D6A0 lbl_8057D6A0;

#define FrameTime() ((u32)&lbl_8057D6A0.m_effectDt)

/* The stages' numbers. */
enum {
    STAGE_310 = 310,
    STAGE_411 = 411,
    STAGE_511 = 511
};

extern s32 lbl_805EE588;

extern "C" {
u16 fn_80415CDC(_stMtnPlayWork *work, int arg1);
void fn_80417568(_stMtnPlayWork *work, u16 motion);
void fn_804165D8(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
void fn_80416148(_stMtnPlayWork *work, int arg1, float arg2);
int fn_80416F28(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
void fn_80415D40(_stMtnPlayWork *work, int arg1, UnkMotionFrames *frames);
void *fn_80415E70(_stMtnPlayWork *work, int node);
void fn_80416E20(_stMtnPlayWork *work, int arg1, float frame);
void fn_80416014(_stMtnPlayWork *work, int arg1, float value);
void fn_80416198(_stMtnPlayWork *work, int arg1);
void fn_80415F98(_stMtnPlayWork *work, int arg1, float arg2, int motion, float time);
void fn_80016370(Mtx34f *matrix, void *frame);
void fn_80016390(RwMatrix *matrix, void *frame);
Vec3f fn_800169F8(const RwV3d *vector);

s32 fn_801FAF00(UnkHintManager_801FB180 *manager, s32 id, s32 branch, int arg3, int arg4);
void fn_8035367C(const char *file, const char *layout, const char *animation);
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int arg1);
boost::weak_ptr<Vibration::RequestHandle> fn_8034DE08(int pattern, const Vec3f *position,
                                                      int arg2);
UnkEffectManager_800A31F0 *fn_800A31F0(void);
void fn_800A4210(const boost::shared_ptr<EffectElement> &effect);
void fn_80425E68(EffectElement *effect, const Mtx34f *matrix);
UnkObject_80074470 *fn_80074470(void);
void fn_80175EEC(UnkObject_80074470 *object, int arg1, int arg2);
void *fn_80053164(void);
void fn_801D54F4(void *manager, int id, const void *source, int arg3, int arg4);
void fn_801D53C4(void *manager, int id, const void *source, void *owner, int arg4, int arg5,
                 int arg6);

UnkCharaList_80074194 *fn_80074194(void);
boost::weak_ptr<CharInfo> fn_800A8B74(UnkCharaList_80074194 *manager, int id);
bool fn_800B77A0(const boost::shared_ptr<CharInfo> *chara);

void *fn_80020A20(UnkLine_80020A20 *line, const Vec3f *start, const Vec3f *end);
void *fn_80020D40(void);
UnkLineHit_80021E24 *fn_80021E24(void *world, const UnkLine_80020A20 *line, int arg2, int arg3,
                                 int arg4, int arg5);
const Vec3f *fn_8001E9BC(void *object);
void fn_800265A4(UnkLineHit_80021E24 *hit, int arg1);

float fn_8000FEE4(float value); /* square root */
/* The roots of a t^2 + b t + c (behaviour inferred); false when there are none. */
bool fn_8003B7AC(float *root0, float *root1, float a, float b, float c);
float fn_8040E99C(float angle);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
void fn_8040CE38(Vec3f *vector, float scale);
void fn_804103E8(Vec3f *position, Vec3f *velocity, const Vec3f *acceleration, float drag,
                 float dt);
}

UnkHintManager_801FB180 *GetHintManager();

typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

inline const Vec3f &Ref(const Vec3f &vector) {
    return vector;
}

inline Vec3f Sub(const Vec3f &a, const Vec3f &b) {
    return ((VectorSubtractFn)fn_8040CD74)(&a, &b);
}

/* Stores sin(angle) and cos(angle) scaled by radius through x and z (as in
 * ElecCircuitEntrance.cpp). */
inline void GetCirclePoint(float *x, float *z, float angle, float radius) {
    *x = radius * std::sin(angle);
    *z = radius * std::cos(angle);
}

/* The boss's character. */
#define Chara() (m_owner->m_charaPtr.get())

/* Whether motion number index of the boss's set is playing (name inferred). */
inline bool IsMotion(EggMecaVarParam *owner, int index) {
    u16 motion = owner->m_motionBase + index;
    u16 current = fn_80415CDC(owner->m_motion, 0);
    return motion == current;
}

/* The id of motion number index of the boss's set (name inferred). */
inline u16 MotionId(EggMecaVarParam *owner, int index) {
    return owner->m_motionBase + index;
}

/* WAIT: stands, then picks the next move by the player's distance. */
class EggMecaModeCtrlUnit_WAIT : public EggMecaCtrlUnit {
  public:
    EggMecaModeCtrlUnit_WAIT(EggMecaVarParam *owner) : BossCtrlUnit(owner), EggMecaCtrlUnit(owner) {
        m_time = 0.0f;
        m_owner->m_motionLocked = 1;
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 0xD);
        fn_80416148(m_owner->m_motion, 0, 10.0f);
        m_owner->m_fixed = 0;
        m_owner->m_stopped = 0;
        m_owner->m_moving = 0;
        m_owner->m_comboCount = 0;
        m_time = m_owner->m_settings.m_waitTime;
        m_owner->m_turnSpeed = m_owner->m_settings.m_waitSpeed;
    }
    virtual ~EggMecaModeCtrlUnit_WAIT();
    virtual int Tick(float dt);
    int Tick310();
    int Tick511();
    bool CanShoot();
    bool IsFacing();
    bool IsFacing2();

    /* 0x0C */ float m_time;
};

class EggMecaModeCtrlUnit_EDIT : public EggMecaCtrlUnit {
  public:
    EggMecaModeCtrlUnit_EDIT(EggMecaVarParam *owner) : BossCtrlUnit(owner), EggMecaCtrlUnit(owner) {
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 0xD);
        m_owner->m_motionLocked = 1;
        m_owner->m_fixed = 1;
    }
    virtual ~EggMecaModeCtrlUnit_EDIT();
    virtual int Tick(float dt);
};

/* GET_COMBO: takes a combination of hits. */
class EggMecaModeCtrlUnit_GET_COMBO : public EggMecaCtrlUnit {
  public:
    EggMecaModeCtrlUnit_GET_COMBO(EggMecaVarParam *owner)
        : BossCtrlUnit(owner), EggMecaCtrlUnit(owner) {
        m_owner->m_motionLocked = 1;
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 0xD);
        fn_80416148(m_owner->m_motion, 0, 10.0f);
        m_time = 0.0f;
    }
    virtual ~EggMecaModeCtrlUnit_GET_COMBO();
    virtual int Tick(float dt);

    /* 0x0C */ float m_time;
};

/* GET_ATK: the boss is hit. */
class EggMecaModeCtrlUnit_GET_ATK : public EggMecaCtrlUnit {
  public:
    EggMecaModeCtrlUnit_GET_ATK(EggMecaVarParam *owner)
        : BossCtrlUnit(owner), EggMecaCtrlUnit(owner) {
        if (!IsMotion(m_owner, 5)) {
            fn_804165D8(m_owner->m_motion, 2, -1, -1);
            fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 5);
            fn_80416014(m_owner->m_motion, 0, 0.0f);
            fn_80415F98(m_owner->m_motion, 0, 0.0f, 0x1E, 1.0f);
        } else {
            fn_80416E20(m_owner->m_motion, 0, 0.0f);
        }
        m_owner->m_stopped = 1;
        m_owner->m_motionLocked = 1;
        m_owner->m_fixed = 1;
        m_owner->m_comboCount++;
    }
    virtual ~EggMecaModeCtrlUnit_GET_ATK();
    virtual int Tick(float dt);

    /* 0x0C */ float m_time;
};

/* MOVE: walks. */
class EggMecaModeCtrlUnit_MOVE : public EggMecaCtrlUnit {
  public:
    EggMecaModeCtrlUnit_MOVE(EggMecaVarParam *owner) : BossCtrlUnit(owner), EggMecaCtrlUnit(owner) {
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 0xF);
        fn_80416148(m_owner->m_motion, 0, 10.0f);
        m_owner->m_motionLocked = 0;
        m_owner->m_fixed = 0;
        Chara()->m_position.y = 0.0f;
    }
    virtual ~EggMecaModeCtrlUnit_MOVE();
    virtual int Tick(float dt);
    void Check();
};

/* DASH: runs around the arena. */
class EggMecaModeCtrlUnit_DASH : public EggMecaCtrlUnit {
  public:
    EggMecaModeCtrlUnit_DASH(EggMecaVarParam *owner) : BossCtrlUnit(owner), EggMecaCtrlUnit(owner) {
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 0x13);
        fn_80416148(m_owner->m_motion, 0, 10.0f);
        m_owner->m_motionLocked = 0;
        m_owner->m_fixed = 0;
        m_owner->m_turnSpeed = m_owner->m_settings.m_dashSpeed;
        Chara()->m_position.y = 0.0f;
    }
    virtual ~EggMecaModeCtrlUnit_DASH();
    virtual int Tick(float dt);
    void Check();
};

/* ATK_HAMMER: swings the hammer. */
class EggMecaModeCtrlUnit_ATK_HAMMER : public EggMecaCtrlUnit {
  public:
    EggMecaModeCtrlUnit_ATK_HAMMER(EggMecaVarParam *owner)
        : BossCtrlUnit(owner), EggMecaCtrlUnit(owner) {
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 1);
        fn_80416148(m_owner->m_motion, 0, 10.0f);
        m_owner->m_motionLocked = 1;
        m_owner->m_fixed = 1;
    }
    virtual ~EggMecaModeCtrlUnit_ATK_HAMMER();
    virtual int Tick(float dt);
};

/* ATK_SHOOT: fires the gun. */
class EggMecaModeCtrlUnit_ATK_SHOOT : public EggMecaCtrlUnit {
  public:
    EggMecaModeCtrlUnit_ATK_SHOOT(EggMecaVarParam *owner)
        : BossCtrlUnit(owner), EggMecaCtrlUnit(owner) {
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 0xA);
        fn_80416148(m_owner->m_motion, 0, 10.0f);
        m_owner->m_motionLocked = 1;
        m_owner->m_fixed = 0;
        m_owner->m_turnSpeed = m_owner->m_settings.m_shootSpeed;
        fn_801D54F4(fn_80053164(), 0x7C36, Chara(), 0, 1);
    }
    virtual ~EggMecaModeCtrlUnit_ATK_SHOOT();
    virtual int Tick(float dt);
};

/* ATK_MOVE: moves for a while. */
class EggMecaModeCtrlUnit_ATK_MOVE : public EggMecaCtrlUnit {
  public:
    EggMecaModeCtrlUnit_ATK_MOVE(EggMecaVarParam *owner)
        : BossCtrlUnit(owner), EggMecaCtrlUnit(owner) {
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 0x12);
        fn_80416148(m_owner->m_motion, 0, 10.0f);
        m_owner->m_motionLocked = 1;
        m_owner->m_fixed = 0;
        m_owner->m_stopped = 1;
        m_owner->m_moving = 1;
        m_time = 0.0f;
        Chara()->m_position.y = 0.0f;
    }
    virtual ~EggMecaModeCtrlUnit_ATK_MOVE();
    virtual int Tick(float dt);

    /* 0x0C */ float m_time;
};

/* ATK_BOMB: throws a ring of bombs. */
class EggMecaModeCtrlUnit_ATK_BOMB : public EggMecaCtrlUnit {
  public:
    EggMecaModeCtrlUnit_ATK_BOMB(EggMecaVarParam *owner)
        : BossCtrlUnit(owner), EggMecaCtrlUnit(owner) {
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 1);
        fn_80416148(m_owner->m_motion, 0, 10.0f);
        m_owner->m_motionLocked = 1;
        m_owner->m_fixed = 0;
        m_owner->m_turnSpeed = m_owner->m_settings.m_waitSpeed;
        if (m_owner->m_settings.m_bombCount > 0) {
            float spread;
            float step = (2.0f * lbl_805FAD38) / m_owner->m_settings.m_bombCount;
            if (m_owner->m_health < 0.5f * m_owner->m_maxHealth) {
                spread = m_owner->m_settings.m_bombSpread;
            } else {
                spread = 0.0f;
            }
            for (int i = 0; i < m_owner->m_settings.m_bombCount; i++) {
                Vec3f position;
                m_owner->m_units->PushBack(boost::shared_ptr<BossCtrlUnit>(new EggMecaCtrlUnit_Bomb(
                    m_owner, *fn_800091EC(&position, &Chara()->m_position),
                    step * i + Chara()->m_rotation.y, spread)));
            }
        }
        fn_801D54F4(fn_80053164(), 0x7C3A, Chara(), 0, 1);
        Chara()->m_position.y = 0.0f;
        fn_801FAF00(GetHintManager(), 0xCB, 0, 0, 0);
    }
    virtual ~EggMecaModeCtrlUnit_ATK_BOMB();
    virtual int Tick(float dt);
};

/* ATK_LASER: fires the laser. */
class EggMecaModeCtrlUnit_ATK_LASER : public EggMecaCtrlUnit {
  public:
    EggMecaModeCtrlUnit_ATK_LASER(EggMecaVarParam *owner)
        : BossCtrlUnit(owner), EggMecaCtrlUnit(owner) {
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 3);
        fn_80416148(m_owner->m_motion, 0, 10.0f);
        m_owner->m_motionLocked = 1;
        m_owner->m_fixed = 1;
        m_owner->m_turnSpeed = m_owner->m_settings.m_laserSpeed;
    }
    virtual ~EggMecaModeCtrlUnit_ATK_LASER();
    virtual int Tick(float dt);
};

/* ATK_JUMP: jumps at the player and lands with a shock wave. */
class EggMecaModeCtrlUnit_ATK_JUMP : public EggMecaCtrlUnit {
  public:
    EggMecaModeCtrlUnit_ATK_JUMP(EggMecaVarParam *owner)
        : BossCtrlUnit(owner), EggMecaCtrlUnit(owner) {
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 7);
        fn_80416148(m_owner->m_motion, 0, 10.0f);
        m_owner->m_motionLocked = 1;
        m_owner->m_fixed = 1;
        switch (lbl_8057D6A0.m_stageId) {
        case STAGE_310:
            fn_801FAF00(GetHintManager(), 0xBF, 0, 0, 0);
            break;
        case STAGE_411:
            fn_801FAF00(GetHintManager(), 0xD7, 0, 0, 0);
        case STAGE_511:
            fn_801FAF00(GetHintManager(), 0xD7, 0, 0, 0);
            break;
        }
        fn_801D54F4(fn_80053164(), 0x7C33, Chara(), 0, 1);
    }
    virtual ~EggMecaModeCtrlUnit_ATK_JUMP();
    virtual int Tick(float dt);
    bool CalcJump(float distance, float height, float angle, float gravity, Vec3f *velocity);
};

/* START: the boss's entrance. */
class EggMecaModeCtrlUnit_START : public EggMecaCtrlUnit {
  public:
    EggMecaModeCtrlUnit_START(EggMecaVarParam *owner)
        : BossCtrlUnit(owner), EggMecaCtrlUnit(owner) {
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 0xE);
        m_owner->m_stopped = 1;
        m_owner->m_motionLocked = 1;
        m_owner->m_fixed = 1;
        fn_803358D0(m_owner, new BossCtrlUnit_PlayCameraMotion(m_owner, 0x19, 0.5f, 0.5f));
        m_owner->m_active = 0;
        m_owner->m_visible = 1;
        fn_8035367C("CsdFiles/BChara_EBreaker", "bchara", "open");
    }
    virtual ~EggMecaModeCtrlUnit_START();
    virtual int Tick(float dt);
};

/* BELOW_HALF: the cut-in when the boss's health falls below half. */
class EggMecaModeCtrlUnit_BELOW_HALF : public EggMecaCtrlUnit {
  public:
    EggMecaModeCtrlUnit_BELOW_HALF(EggMecaVarParam *owner)
        : BossCtrlUnit(owner), EggMecaCtrlUnit(owner) {
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80416148(m_owner->m_motion, 0, 20.0f);
        fn_80417568(m_owner->m_motion,
                    m_owner->m_motionBase + m_owner->m_settings.m_belowHalfMotion);
        m_owner->m_stopped = 1;
        m_owner->m_motionLocked = 1;
        m_owner->m_fixed = 1;
        fn_803358D0(m_owner, new BossCtrlUnit_PlayCameraMotion(
                                 m_owner, m_owner->m_settings.m_belowHalfCamera, 0.0f, 0.0f));
        m_owner->m_active = 0;
        m_owner->m_visible = 1;
        m_owner->m_dead = 1;
        switch (lbl_8057D6A0.m_stageId) {
        case STAGE_310:
            fn_801FAF00(GetHintManager(), 0xBD, 0, 0, 0);
            break;
        case STAGE_411:
            fn_801FAF00(GetHintManager(), 0xC9, 0, 0, 0);
            break;
        case STAGE_511:
            fn_801FAF00(GetHintManager(), 0xD5, 0, 0, 0);
            break;
        }
    }
    virtual ~EggMecaModeCtrlUnit_BELOW_HALF();
    virtual int Tick(float dt);
};

/* DEATH: the boss falls and the stage ends. */
class EggMecaModeCtrlUnit_DEATH : public EggMecaCtrlUnit {
  public:
    EggMecaModeCtrlUnit_DEATH(EggMecaVarParam *owner)
        : BossCtrlUnit(owner), EggMecaCtrlUnit(owner) {
        m_ended = 0;
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80416148(m_owner->m_motion, 0, 10.0f);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 6);
        m_owner->m_stopped = 1;
        m_owner->m_motionLocked = 1;
        m_owner->m_fixed = 1;
        fn_803358D0(m_owner, new BossCtrlUnit_PlayCameraMotion(m_owner, 0x18, 1.0f, 1.0f));
        m_owner->m_active = 0;
        m_owner->m_visible = 1;
        m_owner->m_dead = 1;
        switch (lbl_8057D6A0.m_stageId) {
        case STAGE_310:
            fn_801FAF00(GetHintManager(), 0xBE, 0, 0, 0);
            break;
        case STAGE_411:
            fn_801FAF00(GetHintManager(), 0xCA, 0, 0, 0);
            break;
        case STAGE_511:
            fn_801FAF00(GetHintManager(), 0xD6, 0, 0, 0);
            break;
        }
        m_effect = fn_800A31F0()->Launch(m_owner->m_effects, 0x228);
        boost::shared_ptr<EffectElement> effect = m_effect.lock();
        if (effect) {
            void *frame = fn_80415E70(m_owner->m_motion, 0x28);
            Mtx34f matrix;
            fn_80016370(&matrix, frame);
            fn_80425E68(effect.get(), &matrix);
            effect->SetField14(FrameTime());
        }
    }
    virtual ~EggMecaModeCtrlUnit_DEATH();
    virtual int Tick(float dt);

    /* 0x0C */ boost::weak_ptr<EffectElement> m_effect;
    /* 0x14 */ u8 m_ended;
};

SIZE_ASSERT(EggMecaModeCtrlUnit_WAIT, 0x20);
SIZE_ASSERT(EggMecaModeCtrlUnit_EDIT, 0x1C);
SIZE_ASSERT(EggMecaModeCtrlUnit_GET_COMBO, 0x20);
SIZE_ASSERT(EggMecaModeCtrlUnit_GET_ATK, 0x20);
SIZE_ASSERT(EggMecaModeCtrlUnit_ATK_MOVE, 0x20);
SIZE_ASSERT(EggMecaModeCtrlUnit_DEATH, 0x28);

EggMecaModeCtrlUnit_DEATH::~EggMecaModeCtrlUnit_DEATH() {
    boost::shared_ptr<EffectElement> effect = m_effect.lock();
    if (effect) {
        effect->Stop();
        fn_800A4210(effect);
    }
}

int EggMecaModeCtrlUnit_DEATH::Tick(float dt) {
    if (m_owner->m_attribute) {
        fn_8034DF48(8, -2);
        m_owner->m_attribute = 0;
    }
    if (!m_ended && fn_80416F28(m_owner->m_motion, 0, 3, 1)) {
        fn_80175EEC(fn_80074470(), 6, 0);
        m_ended = 1;
    }
    return 0;
}

EggMecaModeCtrlUnit_BELOW_HALF::~EggMecaModeCtrlUnit_BELOW_HALF() {
    m_owner->m_stopped = 0;
    m_owner->m_active = 1;
    m_owner->m_visible = 0;
    m_owner->m_dead = 0;
}

int EggMecaModeCtrlUnit_BELOW_HALF::Tick(float dt) {
    if (IsMotion(m_owner, m_owner->m_settings.m_belowHalfMotion)) {
        if (fn_80416F28(m_owner->m_motion, 0, 3, 1)) {
            fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 0xD);
            void *frame = fn_80415E70(m_owner->m_motion, 0xD48);
            RwMatrix matrix;
            fn_80016390(&matrix, frame);
            Chara()->m_position.x = matrix.pos.x;
            Chara()->m_position.y = 0.0f;
            Chara()->m_position.z = matrix.pos.z;
        }
    } else if (IsMotion(m_owner, 0xD) && fn_80416F28(m_owner->m_motion, 0, 3, 1)) {
        m_owner->m_nextMode = 2;
    }
    return 0;
}

EggMecaModeCtrlUnit_START::~EggMecaModeCtrlUnit_START() {
    m_owner->m_stopped = 0;
    m_owner->m_active = 1;
    m_owner->m_visible = 0;
}

int EggMecaModeCtrlUnit_START::Tick(float dt) {
    if (fn_80416F28(m_owner->m_motion, 0, 3, 1)) {
        m_owner->m_nextMode = 2;
    }
    return 0;
}

EggMecaModeCtrlUnit_ATK_JUMP::~EggMecaModeCtrlUnit_ATK_JUMP() {}

/* The take-off velocity of a jump that peaks at height and lands distance away in the direction
 * angle; false when there is none. */
bool EggMecaModeCtrlUnit_ATK_JUMP::CalcJump(float distance, float height, float angle,
                                            float gravity, Vec3f *velocity) {
    velocity->y = fn_8000FEE4(2.0f * gravity * height);
    float t0;
    float t1;
    float t;
    if (fn_8003B7AC(&t0, &t1, 0.5f * -gravity, velocity->y, 0.0f)) {
        t = t0 > t1 ? t0 : t1;
    } else {
        return false;
    }
    if (0.0f == t) {
        return false;
    }
    GetCirclePoint(&velocity->x, &velocity->z, angle, -(distance / t));
    return true;
}

int EggMecaModeCtrlUnit_ATK_JUMP::Tick(float dt) {
    u16 motion = fn_80415CDC(m_owner->m_motion, 0);
    if (motion == (u16)(m_owner->m_motionBase + 7)) {
        if (fn_80416F28(m_owner->m_motion, 0, 3, 1)) {
            if (CalcJump(m_owner->m_distance - m_owner->m_settings.m_jumpDistance,
                         m_owner->m_settings.m_jumpHeight, Chara()->m_rotation.y,
                         m_owner->m_settings.m_gravity, &Chara()->field_0x18)) {
                fn_804165D8(m_owner->m_motion, 0, -1, -1);
                fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 8);
                fn_80416148(m_owner->m_motion, 0, 10.0f);
                Vec3f acceleration(0.0f, -m_owner->m_settings.m_gravity, 0.0f);
                fn_800091D0(&Chara()->field_0x24, &acceleration);
                CharInfo *chara = Chara();
                fn_804103E8(&chara->m_position, &chara->field_0x18, &chara->field_0x24, 0.0f,
                            lbl_8057D6A0.m_dt);
            }
        }
    } else if (motion == (u16)(m_owner->m_motionBase + 8)) {
        CharInfo *chara = Chara();
        fn_804103E8(&chara->m_position, &chara->field_0x18, &chara->field_0x24, 0.0f,
                    lbl_8057D6A0.m_dt);
        if (Chara()->m_position.y <= 0.0f) {
            Chara()->m_position.y = 0.0f;
            Chara()->field_0x18.y = 0.0f;
            fn_804165D8(m_owner->m_motion, 0, -1, -1);
            fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 9);
            fn_80416148(m_owner->m_motion, 0, 10.0f);
            m_owner->m_fixed = 0;
            m_owner->m_turnSpeed = m_owner->m_settings.m_landSpeed;
            fn_801D54F4(fn_80053164(), 0x7C34, Chara(), 0, 1);
            fn_8034DE08(8, &Chara()->m_position, -2);
        }
    } else {
        if (m_owner->m_attribute) {
            if (*(s32 *)m_owner->m_attribute == 1) {
                m_owner->m_units->PushBack(boost::shared_ptr<BossCtrlUnit>(
                    new EggMecaCtrlUnit_ShockWave(m_owner, 1)));
                fn_801D54F4(fn_80053164(), 0x7C35, Chara(), 0, 1);
            }
            m_owner->m_attribute = 0;
        }
        if (fn_80416F28(m_owner->m_motion, 0, 3, 1)) {
            m_owner->m_nextMode = 2;
        }
    }
    return 0;
}

EggMecaModeCtrlUnit_ATK_LASER::~EggMecaModeCtrlUnit_ATK_LASER() {}

int EggMecaModeCtrlUnit_ATK_LASER::Tick(float dt) {
    u16 motion = fn_80415CDC(m_owner->m_motion, 0);
    if (motion == MotionId(m_owner, 3)) {
        if (fn_80416F28(m_owner->m_motion, 0, 3, 1)) {
            fn_804165D8(m_owner->m_motion, 0, -1, -1);
            fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 4);
            fn_80416148(m_owner->m_motion, 0, 10.0f);
            m_owner->m_turnSpeed = m_owner->m_settings.m_laserFireSpeed;
            m_owner->m_laserRequest[1] = 1;
            m_owner->m_laserRequest[0] = 1;
            fn_801D54F4(fn_80053164(), 0x7C3C, Chara(), 0, 1);
        }
    } else if (m_owner->m_laserState[0] == 2) {
        fn_801D53C4(fn_80053164(), 0x7C3D, Chara(), this, 3, 0, 3);
    } else if (m_owner->m_laserState[0] == 0 && m_owner->m_laserState[1] == 0) {
        m_owner->m_nextMode = 2;
    }
    return 0;
}

EggMecaModeCtrlUnit_ATK_BOMB::~EggMecaModeCtrlUnit_ATK_BOMB() {}

int EggMecaModeCtrlUnit_ATK_BOMB::Tick(float dt) {
    u16 motion = fn_80415CDC(m_owner->m_motion, 0);
    if (motion == MotionId(m_owner, 1)) {
        if (fn_80416F28(m_owner->m_motion, 0, 3, 1)) {
            fn_804165D8(m_owner->m_motion, 0, -1, -1);
            fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 2);
            fn_80416148(m_owner->m_motion, 0, 10.0f);
            fn_801D54F4(fn_80053164(), 0x7C3B, Chara(), 0, 1);
        }
    } else if (fn_80416F28(m_owner->m_motion, 0, 3, 1)) {
        m_owner->m_nextMode = 2;
    }
    return 0;
}

typedef boost::detail::sp_counted_base_impl<EggMecaCtrlUnit_Bomb *,
                                            boost::checked_deleter<EggMecaCtrlUnit_Bomb> >
    BombCount;

/* Not called anywhere: the linker strips it. Retail emits the bombs' control block destructor
 * here, between ATK_MOVE's destructor and ATK_BOMB's Tick, and calling it from an out-of-line
 * function at this point of the file makes GC/2.7 emit it in the same place (name inferred). */
void DestroyBombCount(BombCount *count) {
    count->~BombCount();
}

EggMecaModeCtrlUnit_ATK_MOVE::~EggMecaModeCtrlUnit_ATK_MOVE() {}

int EggMecaModeCtrlUnit_ATK_MOVE::Tick(float dt) {
    m_time += lbl_8057D6A0.m_dt;
    if (m_time >= m_owner->m_settings.m_moveTime) {
        m_owner->m_nextMode = 2;
    }
    return 0;
}

EggMecaModeCtrlUnit_ATK_SHOOT::~EggMecaModeCtrlUnit_ATK_SHOOT() {}

int EggMecaModeCtrlUnit_ATK_SHOOT::Tick(float dt) {
    u16 motion = fn_80415CDC(m_owner->m_motion, 0);
    if (motion == (u16)(m_owner->m_motionBase + 0xA)) {
        if (fn_80416F28(m_owner->m_motion, 0, 3, 1)) {
            fn_804165D8(m_owner->m_motion, 0, -1, -1);
            fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 0xB);
            fn_80416148(m_owner->m_motion, 0, 10.0f);
            m_owner->m_ironBallRequest = 1;
            fn_801D54F4(fn_80053164(), 0x7C37, Chara(), 0, 1);
            m_owner->m_fixed = 1;
            switch (lbl_8057D6A0.m_stageId) {
            case STAGE_310:
                fn_801FAF00(GetHintManager(), 0xC0, 0, 0, 0);
                break;
            case STAGE_411:
                fn_801FAF00(GetHintManager(), 0xCC, 0, 0, 0);
                break;
            case STAGE_511:
                break;
            }
        }
    } else if (motion == (u16)(m_owner->m_motionBase + 0xB)) {
        if (fn_80416F28(m_owner->m_motion, 0, 3, 1)) {
            fn_804165D8(m_owner->m_motion, 0, -1, -1);
            fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 0xC);
            fn_80416148(m_owner->m_motion, 0, 10.0f);
            m_owner->m_fixed = 0;
            m_owner->m_turnSpeed = m_owner->m_settings.m_waitSpeed;
            fn_801D54F4(fn_80053164(), 0x7C38, Chara(), 0, 1);
        }
    } else if (fn_80416F28(m_owner->m_motion, 0, 3, 1)) {
        m_owner->m_nextMode = 2;
    }
    return 0;
}

EggMecaModeCtrlUnit_ATK_HAMMER::~EggMecaModeCtrlUnit_ATK_HAMMER() {}

int EggMecaModeCtrlUnit_ATK_HAMMER::Tick(float dt) {
    if (fn_80416F28(m_owner->m_motion, 0, 3, 1)) {
        u16 motion = fn_80415CDC(m_owner->m_motion, 0);
        if (motion == (u16)(m_owner->m_motionBase + 1)) {
            fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 2);
        } else if (motion == (u16)(m_owner->m_motionBase + 2)) {
            m_owner->m_nextMode = 2;
        }
    }
    return 0;
}

EggMecaModeCtrlUnit_DASH::~EggMecaModeCtrlUnit_DASH() {}

/* Ends the dash near the player (mode 0xD, the jump) or far from the destination. */
void EggMecaModeCtrlUnit_DASH::Check() {
    float range = m_owner->m_settings.m_dashRadius + m_owner->m_settings.m_jumpDistance;
    float angle = std::fabs(fn_8040E99C(m_owner->m_targetAngle - Chara()->m_rotation.y));
    Vec3f offset;
    fn_8040CD74(&offset, &m_owner->m_target, &Chara()->m_position);
    if (fn_8040CF2C(&offset) <= range * range &&
        angle <= 0.5f * m_owner->m_settings.m_dashAngle * (lbl_805FAD38 / 180.0f)) {
        m_owner->m_nextMode = 0xD;
    } else {
        float limit = m_owner->m_settings.m_nearDistance - range;
        Vec3f away;
        fn_8040CD74(&away, &m_owner->m_destination, &Chara()->m_position);
        if (fn_8040CF2C(&away) >= limit * limit) {
            m_owner->m_nextMode = 0xD;
        }
    }
}

int EggMecaModeCtrlUnit_DASH::Tick(float dt) {
    Vec3f target;
    fn_800091EC(&target, &lbl_805E07A0);
    GetCirclePoint(&target.x, &target.z, Chara()->m_rotation.y,
                   -m_owner->m_settings.m_circleRadius);
    Chara()->field_0x18.y = 0.0f;
    CharInfo *chara = Chara();
    fn_804103E8(&chara->m_position, &chara->field_0x18, &target, m_owner->m_settings.m_circleSpeed,
                lbl_8057D6A0.m_dt);
    Check();
    fn_801D53C4(fn_80053164(), 0x7C2F, Chara(), this, 3, 0, 5);
    return 0;
}

EggMecaModeCtrlUnit_MOVE::~EggMecaModeCtrlUnit_MOVE() {}

/* Picks the next move by the player's distance. */
void EggMecaModeCtrlUnit_MOVE::Check() {
    if (lbl_8057D6A0.m_stageId == STAGE_411 &&
        m_owner->m_settings.m_cameraFreeHeight <= m_owner->m_target.y) {
        if (m_owner->m_distance < m_owner->m_settings.m_nearDistance) {
            m_owner->m_nextMode = 2;
        }
        return;
    }
    if (m_owner->m_distance < m_owner->m_settings.m_nearDistance) {
        m_owner->m_nextMode = 6;
        fn_800091D0(&m_owner->m_destination, &Chara()->m_position);
    } else if (m_owner->m_distance >= m_owner->m_settings.m_farDistance &&
               m_owner->m_hasIronBall) {
        m_owner->m_nextMode = 9;
    }
}

int EggMecaModeCtrlUnit_MOVE::Tick(float dt) {
    Check();
    return 0;
}

EggMecaModeCtrlUnit_GET_ATK::~EggMecaModeCtrlUnit_GET_ATK() {
    m_owner->m_stopped = 0;
}

int EggMecaModeCtrlUnit_GET_ATK::Tick(float dt) {
    if (m_owner->m_stopped) {
        m_time += lbl_8057D6A0.m_stunDt;
        if (m_time > m_owner->m_settings.m_stunTime &&
            m_owner->m_damageTaken <= m_owner->m_settings.m_damageLimit) {
            m_owner->m_stopped = 0;
        }
    }
    if (fn_80416F28(m_owner->m_motion, 0, 3, 1)) {
        fn_80416198(m_owner->m_motion, 0);
        m_owner->m_nextMode = 3;
    } else {
        UnkMotionFrames frames;
        fn_80415D40(m_owner->m_motion, 0, &frames);
        if (frames.m_frame > frames.m_end - 4.0f) {
            fn_80415F98(m_owner->m_motion, 0, 0.0f, 0x1392,
                        (frames.m_end - frames.m_frame) / 4.0f);
        }
    }
    return 0;
}

EggMecaModeCtrlUnit_GET_COMBO::~EggMecaModeCtrlUnit_GET_COMBO() {}

int EggMecaModeCtrlUnit_GET_COMBO::Tick(float dt) {
    if (m_owner->m_damageTaken > m_owner->m_settings.m_damageLimit) {
        m_owner->m_stopped = 1;
        m_owner->m_comboCount = 0;
        m_owner->m_nextMode = 2;
        return 0;
    }
    if (m_owner->m_comboCount >= m_owner->m_settings.m_comboLimit) {
        m_owner->m_stopped = 1;
        m_owner->m_comboCount = 0;
        m_owner->m_nextMode = 2;
        return 0;
    }
    m_time += dt;
    if (m_time > m_owner->m_settings.m_waitTime) {
        m_owner->m_stopped = 1;
        m_owner->m_comboCount = 0;
        m_owner->m_nextMode = 2;
    }
    return 0;
}

EggMecaModeCtrlUnit_EDIT::~EggMecaModeCtrlUnit_EDIT() {}

int EggMecaModeCtrlUnit_EDIT::Tick(float dt) {
    return 0;
}

EggMecaModeCtrlUnit_WAIT::~EggMecaModeCtrlUnit_WAIT() {}

/* The 310 stage's choice. */
int EggMecaModeCtrlUnit_WAIT::Tick310() {
    if (m_owner->m_health >= 0.5f * m_owner->m_maxHealth) {
        if (0.0f == m_time) {
            if (m_owner->m_distance < m_owner->m_settings.m_nearDistance &&
                std::fabs(fn_8040E99C(m_owner->m_targetAngle - Chara()->m_rotation.y)) <
                    0.125f * lbl_805FAD38) {
                m_owner->m_nextMode = 6;
                fn_800091D0(&m_owner->m_destination, &Chara()->m_position);
            } else if (m_owner->m_distance >= m_owner->m_settings.m_farDistance) {
                if (m_owner->m_hasIronBall) {
                    m_owner->m_nextMode = 9;
                }
            } else {
                m_owner->m_nextMode = 5;
            }
        }
    } else if (0.0f == m_time) {
        if (m_owner->m_distance < m_owner->m_settings.m_nearDistance &&
            std::fabs(fn_8040E99C(m_owner->m_targetAngle - Chara()->m_rotation.y)) <
                0.125f * lbl_805FAD38) {
            m_owner->m_nextMode = 6;
            fn_800091D0(&m_owner->m_destination, &Chara()->m_position);
        } else if (m_owner->m_distance >= m_owner->m_settings.m_farDistance) {
            if (m_owner->m_hasIronBall) {
                m_owner->m_nextMode = 9;
            }
        } else {
            m_owner->m_nextMode = 0xA;
        }
    }
    return 0;
}

/* Whether the boss faces the player closely enough to shoot (names inferred). */
bool EggMecaModeCtrlUnit_WAIT::IsFacing2() {
    return std::fabs(fn_8040E99C(m_owner->m_targetAngle - Chara()->m_rotation.y)) <
           0.5f * m_owner->m_settings.m_turnAngle2 * (lbl_805FAD38 / 180.0f);
}

bool EggMecaModeCtrlUnit_WAIT::IsFacing() {
    return std::fabs(fn_8040E99C(m_owner->m_targetAngle - Chara()->m_rotation.y)) <
           0.5f * m_owner->m_settings.m_turnAngle * (lbl_805FAD38 / 180.0f);
}

/* Whether nothing stands between the boss's gun and the player. */
bool EggMecaModeCtrlUnit_WAIT::CanShoot() {
    void *frame = fn_80415E70(m_owner->m_motion, 0x20);
    RwMatrix matrix;
    fn_80016390(&matrix, frame);
    Vec3f muzzle;
    fn_800091EC(&muzzle, &Ref(fn_800169F8(&matrix.pos)));
    fn_80016390(&matrix, fn_80415E70(m_owner->m_motion, 0x1F));
    fn_8040CFF8(&muzzle, &Ref(fn_800169F8(&matrix.pos)));
    fn_8040CE38(&muzzle, 0.5f);
    UnkLine_80020A20 line;
    fn_80020A20(&line, &muzzle, &m_owner->m_target);
    UnkLineHit_80021E24 *hit = fn_80021E24(fn_80020D40(), &line, 1, 0, 0, 0);
    if (!hit) {
        return true;
    }
    bool result = false;
    if (hit->m_object) {
        Vec3f position;
        fn_800091EC(&position, fn_8001E9BC(hit->m_object));
        result = fn_8040CF2C(fn_800091EC(const_cast<Vec3f *>(&Ref(Vec3f())),
                                         &Ref(Sub(position, m_owner->m_target)))) < 100.0f;
    }
    fn_800265A4(hit, 1);
    return result;
}

/* The 511 stage's choice: the boss reminds the player to use the hint, and shoots when it can. */
int EggMecaModeCtrlUnit_WAIT::Tick511() {
    if (0.0f == m_owner->m_hintTime) {
        if (!IsFacing2()) {
            fn_801FAF00(GetHintManager(), 0xD8, 0, 0, 0);
            m_owner->m_hintTime = m_owner->m_settings.m_hintTime;
        }
    } else {
        m_owner->m_hintTime -= lbl_8057D6A0.m_dt;
        if (m_owner->m_hintTime < 0.0f) {
            m_owner->m_hintTime = 0.0f;
        }
    }
    if (0.0f == m_time && IsFacing()) {
        if (m_owner->m_health >= 0.5f * m_owner->m_maxHealth) {
            if (CanShoot()) {
                m_owner->m_nextMode = 0xC;
            }
        } else if (m_owner->m_distance < m_owner->m_settings.m_nearDistance || !CanShoot()) {
            m_owner->m_nextMode = 6;
        } else {
            m_owner->m_nextMode = 0xC;
        }
    }
    return 0;
}

int EggMecaModeCtrlUnit_WAIT::Tick(float dt) {
    boost::weak_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588);
    boost::shared_ptr<CharInfo> chara = player.lock();
    if (chara && fn_800B77A0(&chara)) {
        return 0;
    }
    m_time -= lbl_8057D6A0.m_dt;
    if (m_time < 0.0f) {
        m_time = 0.0f;
    }
    switch (lbl_8057D6A0.m_stageId) {
    case STAGE_310:
        Tick310();
        break;
    case STAGE_411: {
        EggMecaVarParam *owner = m_owner;
        if (owner->m_settings.m_cameraFreeHeight <= owner->m_target.y) {
            if (owner->m_distance >= owner->m_settings.m_farDistance) {
                owner->m_nextMode = 5;
            } else if (owner->m_mode != 2) {
                owner->m_nextMode = 2;
            }
        } else if (owner->m_health >= 0.5f * owner->m_maxHealth) {
            if (0.0f == m_time) {
                if (owner->m_distance < owner->m_settings.m_nearDistance) {
                    owner->m_nextMode = 0xB;
                } else if (owner->m_distance >= owner->m_settings.m_farDistance) {
                    if (owner->m_hasIronBall) {
                        owner->m_nextMode = 9;
                    }
                } else {
                    owner->m_nextMode = 5;
                }
            }
        } else if (0.0f == m_time) {
            if (owner->m_distance < owner->m_settings.m_nearDistance) {
                owner->m_nextMode = 0xB;
            } else if (owner->m_distance >= owner->m_settings.m_farDistance) {
                if (owner->m_hasIronBall) {
                    owner->m_nextMode = 9;
                }
            } else {
                owner->m_nextMode = 0xD;
            }
        }
        break;
    }
    case STAGE_511:
        Tick511();
        break;
    }
    return 0;
}

BossCtrlUnit *EggMecaModeCtrlUnit::CreateMode(int mode) {
    switch (mode) {
    case 0:
    case 2:
    default:
        return new EggMecaModeCtrlUnit_WAIT(m_owner);
    case 1:
        return new EggMecaModeCtrlUnit_EDIT(m_owner);
    case 3:
        return new EggMecaModeCtrlUnit_GET_COMBO(m_owner);
    case 4:
        return new EggMecaModeCtrlUnit_GET_ATK(m_owner);
    case 5:
        return new EggMecaModeCtrlUnit_MOVE(m_owner);
    case 6:
        return new EggMecaModeCtrlUnit_DASH(m_owner);
    case 8:
        return new EggMecaModeCtrlUnit_ATK_HAMMER(m_owner);
    case 9:
        return new EggMecaModeCtrlUnit_ATK_SHOOT(m_owner);
    case 10:
        return new EggMecaModeCtrlUnit_ATK_MOVE(m_owner);
    case 11:
        return new EggMecaModeCtrlUnit_ATK_BOMB(m_owner);
    case 12:
        return new EggMecaModeCtrlUnit_ATK_LASER(m_owner);
    case 13:
        return new EggMecaModeCtrlUnit_ATK_JUMP(m_owner);
    case 14:
        return new EggMecaModeCtrlUnit_START(m_owner);
    case 15:
        return new EggMecaModeCtrlUnit_BELOW_HALF(m_owner);
    case 16:
        return new EggMecaModeCtrlUnit_DEATH(m_owner);
    }
}

EggMecaModeCtrlUnit::~EggMecaModeCtrlUnit() {}

int EggMecaModeCtrlUnit::Tick(float dt) {
    return BossModeCtrlUnit::Tick(dt);
}
