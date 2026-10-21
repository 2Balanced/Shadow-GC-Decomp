/* BlackBull0412ModeCtrlUnit (class name from RTTI): the mode control unit of the Black Bull
 * boss's 0412 stage, the airborne fight. CreateMode builds one BlackBull0412ModeCtrlUnit_<MODE>
 * unit (class names from RTTI) for each of the stage's modes; each one sets up its motion in its
 * constructor, runs in Tick (which asks the boss for the next mode by setting
 * BossVarParam::m_nextMode) and cleans up in its destructor. File name inferred from the class (as
 * Game/BlackBull0210ModeCtrlUnit.cpp). Field and helper names are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first); the mode units'
 * constructors are inline and are emitted after CreateMode, their first user. */
#define BLACKBULLVARPARAM_VECTORS
#include "Effect/System/EffectElement.h"
#include "Game/BlackBull0412CtrlUnit.h"
#include "Game/BlackBullCtrlUnit.h"
#include "Game/BlackBullCtrlUnits.h"
#include "Game/BossCtrlUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/EffectPJSLauncher.h"
#include "Game/EnemyCharInfo.h"
#include "Game/MathTypes.h"
#include "Game/Vibration.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Effect::System::EffectElement;
using Effect::System::EffectLaunchInfo;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

struct _stMtnPlayWork;
struct UnkHintManager_801FB180;
struct UnkObject_80074470;

/* What a motion passes to its callbacks (as in DevilDoomCtrlUnit.cpp). */
struct _stMtnCallbackParam {
    /* 0x0 */ void *m_frame;
    /* 0x4 */ u32 field_0x4;
    /* 0x8 */ s32 m_id;
};

/* What fn_80415D40 fills in about a motion (as in BlackBull0210ModeCtrlUnit.cpp). */
struct UnkMotionFrames {
    /* 0x00 */ float m_start;
    /* 0x04 */ float m_end;
    /* 0x08 */ float field_0x8;
    /* 0x0C */ float m_frame;
};

/* The parts of AttackCallbackParam OnHit reads (as in DevilDoomCtrlUnit.cpp). */
struct BlackBullAttackParam {
    /* 0x00 */ boost::weak_ptr<CharaColliAttack> m_attacker;
    /* 0x08 */ u8 field_0x8[0x18 - 0x8];
    /* 0x18 */ Vec3f m_position;
    /* 0x24 */ u8 field_0x24[0x34 - 0x24];
    /* 0x34 */ Vec3f m_offset;
    /* 0x40 */ u8 field_0x40[0x50 - 0x40];
    /* 0x50 */ float m_damage;
    /* 0x54 */ s32 m_result;
};

#define Param(param) (*(BlackBullAttackParam *)&(param))

/* The effect manager fn_800A31F0 returns (as in DevilDoomCtrlUnit.cpp). */
class UnkEffectManager_800A31F0 {
  public:
    virtual void vf08();
    virtual boost::weak_ptr<EffectElement> LaunchId(s32 id, s32 arg);
    virtual boost::weak_ptr<EffectElement> Launch(boost::shared_ptr<EffectLaunchInfo> info, s32 id);
};

/* The boss's effect launcher, read as the PJS launcher it holds (as in HeavyDog0410CtrlUnit.cpp):
 * retail converts it through a local converting constructor. */

/* The game state (as in DevilDoomCtrlUnit.cpp): the frame time at 0x24, and the time step the
 * hit stun counts at 0x28 (meaning inferred). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x24];
    /* 0x024 */ float m_dt;
    /* 0x028 */ float field_0x28;
};
extern UnkGameState_8057D6A0 lbl_8057D6A0;

#define FrameTime() ((u32)&lbl_8057D6A0.m_dt)

/* The null owner of colliders. */
extern boost::shared_ptr<CharInfo> lbl_805EE698;
extern const Vec3f lbl_805E07B8;

extern "C" {
u16 fn_80415CDC(_stMtnPlayWork *work, int arg1);
void fn_80417568(_stMtnPlayWork *work, u16 motion);
void fn_804165D8(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
void fn_80416148(_stMtnPlayWork *work, int arg1, float arg2);
int fn_80416F28(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
void fn_80415D40(_stMtnPlayWork *work, int arg1, UnkMotionFrames *frames);
Mtx34f *fn_80415E70(_stMtnPlayWork *work, int node);
void fn_80416E20(_stMtnPlayWork *work, int arg1, float frame);
void fn_80416014(_stMtnPlayWork *work, int arg1, float value);
void fn_80416198(_stMtnPlayWork *work, int arg1);
void fn_80415F98(_stMtnPlayWork *work, int arg1, float arg2, int motion, float time);
void fn_80415808(_stMtnPlayWork *work, int id, void (*callback)(void *, void *), void *data);
void fn_80335844(void *, void *);
void fn_80016370(Mtx34f *matrix, void *frame);

s32 fn_801FAF00(UnkHintManager_801FB180 *manager, s32 id, s32 branch, int arg3, int arg4);
void fn_801FAAA4(UnkHintManager_801FB180 *manager);
void fn_801FAAB4(UnkHintManager_801FB180 *manager);
void fn_8035367C(const char *file, const char *layout, const char *animation);
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int arg1);
UnkEffectManager_800A31F0 *fn_800A31F0(void);
void fn_800A4210(const boost::shared_ptr<EffectElement> &effect);
void fn_80425E4C(EffectElement *effect, const Vec3f *position);
UnkObject_80074470 *fn_80074470(void);
void fn_80175EEC(UnkObject_80074470 *object, int arg1, int arg2);
void *fn_80053164(void);
void fn_801D54F4(void *manager, int id, const void *source, int arg3, int arg4);

void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7EA8(void *manager, const Vec3f *position, u32 flags,
                                              int arg3, int arg4,
                                              const boost::weak_ptr<CharInfo> &owner,
                                              const Vec3f *offset, float radius);
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager,
                                              boost::shared_ptr<CharaColliBase> collider);

float fn_8040CE74(Vec3f *vector);
void fn_8040CE38(Vec3f *vector, float scale);
void fn_804114D0(Vec3f *translation, const Mtx34f *matrix);
void fn_80016350(Vec3f *vector, const Vec3f *value);
Vec3f fn_80016284(const Vec3f &direction);
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040FB08(const Mtx34f *matrix);
}

UnkHintManager_801FB180 *GetHintManager();

typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

inline const Vec3f &Ref(const Vec3f &vector) {
    return vector;
}

inline Vec3f Sub(const Vec3f &a, const Vec3f &b) {
    return ((VectorSubtractFn)fn_8040CD74)(&a, &b);
}

inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* Whether motion number index of the boss's set is playing (name inferred). */
inline bool IsMotion(BlackBull0412VarParam *owner, int index) {
    u16 motion = owner->m_motionBase + index;
    u16 current = fn_80415CDC(owner->m_motion, 0);
    return motion == current;
}

/* The boss's health as a part of its full health. */
inline float HealthRatio(BlackBull0412VarParam *owner) {
    return owner->m_health / owner->m_maxHealth;
}

/* WAIT: hovers, then picks an attack. */
class BlackBull0412ModeCtrlUnit_WAIT : public BlackBull0412CtrlUnit {
  public:
    BlackBull0412ModeCtrlUnit_WAIT(BlackBull0412VarParam *owner)
        : BossCtrlUnit(owner), BlackBull0412CtrlUnit(owner) {
        m_time = 0.0f;
        m_owner->m_motionLocked = 0;
        m_owner->m_motionLocked2 = 0;
        m_owner->m_moving = 0;
        m_owner->m_stopped = 0;
        m_owner->m_comboCount = 0;
        if (!m_owner->m_arrived) {
            m_time = m_owner->m_waitTime;
        }
        m_owner->m_arrived = 0;
        fn_800091D0(&m_owner->m_destination, &m_owner->m_position);
    }
    virtual ~BlackBull0412ModeCtrlUnit_WAIT();
    virtual int Tick(float dt);

    /* 0x10 */ float m_time;
};

class BlackBull0412ModeCtrlUnit_EDIT : public BlackBull0412CtrlUnit {
  public:
    BlackBull0412ModeCtrlUnit_EDIT(BlackBull0412VarParam *owner)
        : BossCtrlUnit(owner), BlackBull0412CtrlUnit(owner) {
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 5);
        m_owner->m_motionLocked = 1;
        m_owner->m_motionLocked2 = 1;
    }
    virtual ~BlackBull0412ModeCtrlUnit_EDIT();
    virtual int Tick(float dt);
};

/* ESCAPE: rises back to its height after a fall, then flies to its destination. */
class BlackBull0412ModeCtrlUnit_ESCAPE : public BlackBull0412CtrlUnit {
  public:
    BlackBull0412ModeCtrlUnit_ESCAPE(BlackBull0412VarParam *owner)
        : BossCtrlUnit(owner), BlackBull0412CtrlUnit(owner) {
        m_owner->m_motionLocked = 1;
        m_owner->m_stopped = 1;
        m_owner->m_motionLocked2 = 0;
        m_owner->field_0x171 = 0;
    }
    virtual ~BlackBull0412ModeCtrlUnit_ESCAPE();
    virtual int Tick(float dt);

    /* 0x10 */ float m_speed;
};

/* GET_COMBO: takes a combination of hits. */
class BlackBull0412ModeCtrlUnit_GET_COMBO : public BlackBull0412CtrlUnit {
  public:
    BlackBull0412ModeCtrlUnit_GET_COMBO(BlackBull0412VarParam *owner)
        : BossCtrlUnit(owner), BlackBull0412CtrlUnit(owner) {
        m_owner->m_motionLocked = 1;
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 5);
        fn_80416148(m_owner->m_motion, 0, 10.0f);
    }
    virtual ~BlackBull0412ModeCtrlUnit_GET_COMBO();
    virtual int Tick(float dt);
};

/* GET_ATK: the boss is hit. */
class BlackBull0412ModeCtrlUnit_GET_ATK : public BlackBull0412CtrlUnit {
  public:
    BlackBull0412ModeCtrlUnit_GET_ATK(BlackBull0412VarParam *owner)
        : BossCtrlUnit(owner), BlackBull0412CtrlUnit(owner) {
        m_time = 0.0f;
        if (!IsMotion(m_owner, 6)) {
            fn_804165D8(m_owner->m_motion, 2, -1, -1);
            fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 6);
            fn_80416014(m_owner->m_motion, 0, 0.0f);
            fn_80415F98(m_owner->m_motion, 0, 0.0f, 0x1392, 1.0f);
        } else {
            fn_80416E20(m_owner->m_motion, 0, 0.0f);
        }
        m_owner->m_stopped = 1;
        m_owner->m_motionLocked = 1;
        m_owner->m_comboCount++;
    }
    virtual ~BlackBull0412ModeCtrlUnit_GET_ATK();
    virtual int Tick(float dt);

    /* 0x10 */ float m_time;
};

/* BELOW_HALF: the cut-in when the boss's health falls below half. */
class BlackBull0412ModeCtrlUnit_BELOW_HALF : public BlackBull0412CtrlUnit {
  public:
    BlackBull0412ModeCtrlUnit_BELOW_HALF(BlackBull0412VarParam *owner)
        : BossCtrlUnit(owner), BlackBull0412CtrlUnit(owner) {
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80416148(m_owner->m_motion, 0, 20.0f);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 1);
        m_owner->m_stopped = 1;
        m_owner->field_0x171 = 0;
        m_owner->m_motionLocked = 1;
        m_owner->m_motionLocked2 = 1;
        fn_803358D0(m_owner, new BossCtrlUnit_PlayCameraMotion(m_owner, 7, 0.0f, 0.0f));
        m_owner->m_active = 0;
        fn_8034DF48(0xC, -2);
    }
    virtual ~BlackBull0412ModeCtrlUnit_BELOW_HALF();
    virtual int Tick(float dt);
};

/* DEATH: the boss falls and the stage ends. */
class BlackBull0412ModeCtrlUnit_DEATH : public BlackBull0412CtrlUnit {
  public:
    BlackBull0412ModeCtrlUnit_DEATH(BlackBull0412VarParam *owner)
        : BossCtrlUnit(owner), BlackBull0412CtrlUnit(owner) {
        m_ended = 0;
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 2);
        m_owner->m_chara->m_position.y = -60.0f;
        m_owner->m_stopped = 1;
        m_owner->field_0x170 = 0;
        m_owner->field_0x171 = 0;
        m_owner->m_motionLocked = 1;
        m_owner->m_motionLocked2 = 1;
        fn_803358D0(m_owner, new BossCtrlUnit_PlayCameraMotion(m_owner, 9, 0.0f, 0.0f));
        m_owner->m_active = 0;
        m_owner->m_attribute = 0;
        fn_801FAF00(GetHintManager(), 0xB3, 2, 0, 0);
        fn_8034DF48(7, -2);
    }
    virtual ~BlackBull0412ModeCtrlUnit_DEATH();
    virtual int Tick(float dt);

    /* 0x10 */ boost::weak_ptr<EffectElement> m_effect;
    /* 0x18 */ u8 m_ended;
};

/* ATK_BREATH: breathes a fire ball along its head. */
class BlackBull0412ModeCtrlUnit_ATK_BREATH : public BlackBull0412CtrlUnit {
  public:
    BlackBull0412ModeCtrlUnit_ATK_BREATH(BlackBull0412VarParam *owner)
        : BossCtrlUnit(owner), BlackBull0412CtrlUnit(owner) {
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80416148(m_owner->m_motion, 0, 10.0f);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 3);
        m_owner->m_motionLocked = 1;
        m_owner->m_motionLocked2 = 1;
        m_owner->m_attribute = 0;
    }
    virtual ~BlackBull0412ModeCtrlUnit_ATK_BREATH();
    virtual int Tick(float dt);
};

/* ATK_WAVERING: rises while dropping rings of fire, m_waveCount times. */
class BlackBull0412ModeCtrlUnit_ATK_WAVERING : public BlackBull0412CtrlUnit {
  public:
    BlackBull0412ModeCtrlUnit_ATK_WAVERING(BlackBull0412VarParam *owner)
        : BossCtrlUnit(owner), BlackBull0412CtrlUnit(owner) {
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80416148(m_owner->m_motion, 0, 10.0f);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 4);
        m_owner->field_0x170 = 0;
        m_owner->field_0x171 = 0;
        m_owner->m_motionLocked = 1;
        m_owner->m_motionLocked2 = 0;
        m_count = 0;
        m_rise = (m_owner->m_settings.field_0x4 - m_owner->m_chara->m_position.y) / 1.25f;
        m_owner->m_attribute = 0;
    }
    virtual ~BlackBull0412ModeCtrlUnit_ATK_WAVERING();
    virtual int Tick(float dt);

    /* 0x10 */ s32 m_count;
    /* 0x14 */ float m_rise; /* how fast the boss climbs */
};

/* ATK_CLOSERANGE: drops a fire ball straight down. */
class BlackBull0412ModeCtrlUnit_ATK_CLOSERANGE : public BlackBull0412CtrlUnit {
  public:
    BlackBull0412ModeCtrlUnit_ATK_CLOSERANGE(BlackBull0412VarParam *owner)
        : BossCtrlUnit(owner), BlackBull0412CtrlUnit(owner) {
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80416148(m_owner->m_motion, 0, 10.0f);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 4);
        m_owner->field_0x170 = 0;
        m_owner->field_0x171 = 0;
        m_owner->m_motionLocked = 1;
        m_owner->m_motionLocked2 = 0;
        m_owner->m_attribute = 0;
    }
    virtual ~BlackBull0412ModeCtrlUnit_ATK_CLOSERANGE();
    virtual int Tick(float dt);
};

/* ATK_CHARGE: dives at the player, its horns hitting through two colliders. */
class BlackBull0412ModeCtrlUnit_ATK_CHARGE : public BlackBull0412CtrlUnit,
                                             public AttackCallbackContainer,
                                             public MotionCallbackContainer {
  public:
    BlackBull0412ModeCtrlUnit_ATK_CHARGE(BlackBull0412VarParam *owner)
        : BossCtrlUnit(owner), BlackBull0412CtrlUnit(owner) {
        m_owner->m_motionLocked = 1;
        m_owner->m_motionLocked2 = 0;
        m_owner->field_0x122 = 1;
        m_state = 0;
        fn_800091D0(&m_start, &m_owner->m_chara->m_position);
        m_owner->m_testDestination.y = -20.0f;
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80416148(m_owner->m_motion, 0, 10.0f);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 0xC);
        m_side = 0;
    }
    virtual ~BlackBull0412ModeCtrlUnit_ATK_CHARGE();
    virtual int Tick(float dt);
    virtual void OnHit(AttackCallbackParam &param);
    virtual void OnMotion(_stMtnCallbackParam &param);

    /* 0x18 */ s32 m_state;
    /* 0x1C */ Vec3f m_start;
    /* 0x28 */ Vec3f field_0x28;
    /* 0x34 */ s32 m_side; /* the horn whose collider moves next */
    /* 0x38 */ boost::shared_ptr<CharaColliBase> m_colliders[2];
    /* 0x48 */ UnkMotionCallback_802414D4 m_callback;
};

/* START: the boss's entrance. */
class BlackBull0412ModeCtrlUnit_START : public BlackBull0412CtrlUnit {
  public:
    BlackBull0412ModeCtrlUnit_START(BlackBull0412VarParam *owner)
        : BossCtrlUnit(owner), BlackBull0412CtrlUnit(owner) {
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 0xA);
        m_owner->m_stopped = 1;
        m_owner->field_0x171 = 0;
        m_owner->m_motionLocked = 1;
        m_owner->m_motionLocked2 = 1;
        fn_803358D0(m_owner, new BossCtrlUnit_PlayCameraMotion(m_owner, 8, &lbl_805E07A0,
                                                                &lbl_805E07B8, 0.0f, 0.0f));
        m_owner->m_active = 0;
        fn_801FAF00(GetHintManager(), 0xB0, 0, 0, 0);
        fn_8035367C("CsdFiles/BChara_BBull", "bchara", "open");
        m_owner->m_attribute = 0;
    }
    virtual ~BlackBull0412ModeCtrlUnit_START();
    virtual int Tick(float dt);
};

SIZE_ASSERT(BlackBull0412ModeCtrlUnit_WAIT, 0x24);
SIZE_ASSERT(BlackBull0412ModeCtrlUnit_EDIT, 0x20);
SIZE_ASSERT(BlackBull0412ModeCtrlUnit_ESCAPE, 0x24);
SIZE_ASSERT(BlackBull0412ModeCtrlUnit_GET_ATK, 0x24);
SIZE_ASSERT(BlackBull0412ModeCtrlUnit_DEATH, 0x2C);
SIZE_ASSERT(BlackBull0412ModeCtrlUnit_ATK_WAVERING, 0x28);
SIZE_ASSERT(BlackBull0412ModeCtrlUnit_ATK_CHARGE, 0x64);

BlackBull0412ModeCtrlUnit_START::~BlackBull0412ModeCtrlUnit_START() {
    m_owner->m_stopped = 0;
    m_owner->field_0x171 = 1;
    m_owner->m_motionLocked = 0;
    m_owner->m_motionLocked2 = 0;
    m_owner->m_active = 1;
}

int BlackBull0412ModeCtrlUnit_START::Tick(float dt) {
    if (m_owner->m_attribute) {
        fn_8034DF48(0xC, -2);
        m_owner->m_attribute = 0;
    }
    if (fn_80416F28(m_owner->m_motion, 0, 3, 1)) {
        m_owner->m_nextMode = 2;
        return 1;
    }
    return 0;
}

void BlackBull0412ModeCtrlUnit_ATK_CHARGE::OnMotion(_stMtnCallbackParam &param) {
    Mtx34f matrix;
    fn_80016370(&matrix, param.m_frame);
    if (m_side == 0) {
        fn_8040B59C(&matrix, reinterpret_cast<Vec3f *>(&m_owner->m_settings.field_0x10));
    } else {
        fn_8040B59C(&matrix, reinterpret_cast<Vec3f *>(&m_owner->m_settings.field_0x1C));
    }
    Vec3f position;
    fn_800091EC(&position, &Ref(fn_8040FB08(&matrix)));
    if (CharaColliBase *collider = m_colliders[m_side].get()) {
        collider->SetCharInfo((CharInfo *)&position);
    }
    m_side = 1 - m_side;
}

void BlackBull0412ModeCtrlUnit_ATK_CHARGE::OnHit(AttackCallbackParam &param) {
    boost::weak_ptr<CharaColliAttack> attack(Param(param).m_attacker);
    fn_800091D0(&Param(param).m_offset,
                &Ref(Sub(Param(param).m_position, m_owner->m_chara->m_position)));
    Param(param).m_damage = attack.lock()->m_power;
    fn_801D54F4(fn_80053164(), 0x7B41, &m_owner->m_target, 0, 1);
}

BlackBull0412ModeCtrlUnit_ATK_CHARGE::~BlackBull0412ModeCtrlUnit_ATK_CHARGE() {
    m_owner->field_0x122 = 0;
    for (int i = 0; i < 2; i++) {
        if (m_colliders[i]) {
            m_colliders[i].reset();
        }
    }
    fn_80415808(m_owner->m_motion, 0x139D, 0, 0);
    fn_80415808(m_owner->m_motion, 0x139F, 0, 0);
}

int BlackBull0412ModeCtrlUnit_ATK_CHARGE::Tick(float dt) {
    switch (m_state) {
    case 0: {
        UnkMotionFrames frames;
        fn_80415D40(m_owner->m_motion, 0, &frames);
        float t = (frames.m_frame - frames.m_start) / (frames.m_end - frames.m_start);
        if (m_owner->field_0x168) {
            fn_800091D0(&m_owner->m_testDestination, &m_owner->m_target);
        }
        Vec3f offset;
        fn_800091D0(&offset, &Ref(Sub(m_owner->m_testDestination, m_start)));
        float length = fn_8040CF50(&offset);
        float reach = length - 95.0f;
        if (reach < 0.0f) {
            reach = 0.0f;
        }
        if (length < 0.0001) {
            fn_800091D0(&offset, &lbl_805E07A0);
        } else {
            fn_8040CE38(&offset, reach / length);
        }
        fn_800091D0(&m_owner->m_chara->m_position,
                    &Ref(fn_8040CDD0(&Ref(fn_8040CDA8(&offset, t)), &m_start)));
        if (fn_80416F28(m_owner->m_motion, 0, 3, 1)) {
            fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 0xD);
            m_state = 1;
            void *manager = fn_80021B08();
            for (int i = 0; i < 2; i++) {
                m_colliders[i] =
                    fn_800C7EA8(manager, &m_owner->m_chara->m_position, 0, 4, 0, lbl_805EE698,
                                &lbl_805E07A0, m_owner->m_settings.field_0x28);
                if (m_colliders[i]) {
                    m_colliders[i]->SetField2C(1);
                    boost::weak_ptr<CharaColliAttack> attack =
                        fn_800C4374(manager, m_colliders[i]);
                    boost::shared_ptr<CharaColliAttack> collision(attack.lock());
                    if (collision) {
                        collision->m_power = 1.0f;
                        collision->m_kind = 4;
                        collision->m_flags = 0x40;
                        collision->field_0x54 = 1;
                        collision->m_hitCallback = GetHitCallback();
                        collision->field_0x55 = 0;
                    }
                }
            }
            m_callback = GetCallback();
            fn_80415808(m_owner->m_motion, 0x139D, fn_80335844, &m_callback);
            fn_80415808(m_owner->m_motion, 0x139F, fn_80335844, &m_callback);
        }
        break;
    }
    case 1:
        if (fn_80416F28(m_owner->m_motion, 0, 3, 1)) {
            fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 0xE);
            m_state = 2;
        }
        break;
    case 2:
        if (fn_80416F28(m_owner->m_motion, 0, 3, 1)) {
            m_owner->m_nextMode = 2;
        }
        break;
    }
    return 0;
}

BlackBull0412ModeCtrlUnit_ATK_CLOSERANGE::~BlackBull0412ModeCtrlUnit_ATK_CLOSERANGE() {
    m_owner->field_0x170 = 1;
    m_owner->field_0x171 = 1;
}

int BlackBull0412ModeCtrlUnit_ATK_CLOSERANGE::Tick(float dt) {
    if (m_owner->m_attribute) {
        Vec3f translation;
        fn_804114D0(&translation, fn_80415E70(m_owner->m_motion, 0x13A6));
        Vec3f position;
        fn_80016350(&position, &translation);
        Vec3f velocity(0.0f, -1.0f, 0.0f);
        fn_8040CE38(&velocity, m_owner->m_fireSpeed);
        fn_803358D0(m_owner, new BlackBullCtrlUnit_FireBall(m_owner, &position, &velocity, 0));
        m_owner->m_attribute = 0;
    }
    if (fn_80416F28(m_owner->m_motion, 0, 3, 1)) {
        m_owner->m_nextMode = 2;
    }
    return 0;
}

BlackBull0412ModeCtrlUnit_ATK_WAVERING::~BlackBull0412ModeCtrlUnit_ATK_WAVERING() {
    m_owner->field_0x170 = 1;
    m_owner->field_0x171 = 1;
}

int BlackBull0412ModeCtrlUnit_ATK_WAVERING::Tick(float dt) {
    m_owner->m_chara->m_position.y += m_rise * dt;
    if (m_owner->m_attribute) {
        Vec3f translation;
        fn_804114D0(&translation, fn_80415E70(m_owner->m_motion, 0x13A6));
        Vec3f position;
        fn_80016350(&position, &translation);
        fn_803358D0(m_owner, new BlackBullCtrlUnit_WaveRingFire(m_owner, &position, 0));
        m_count++;
        m_owner->m_attribute = 0;
        m_rise = -m_owner->m_settings.field_0x0 / (m_owner->m_waveBlend / 60.0f);
        if (m_count < m_owner->m_waveCount) {
            UnkMotionFrames frames;
            fn_80415D40(m_owner->m_motion, 0, &frames);
            fn_804165D8(m_owner->m_motion, 0, -1, -1);
            fn_80416148(m_owner->m_motion, 0, m_owner->m_waveBlend);
            fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 4);
            fn_80416E20(m_owner->m_motion, 0, frames.m_frame - m_owner->m_waveBlend);
            return 0;
        }
    }
    if (fn_80416F28(m_owner->m_motion, 0, 3, 1)) {
        m_owner->m_nextMode = 2;
    }
    return 0;
}

BlackBull0412ModeCtrlUnit_ATK_BREATH::~BlackBull0412ModeCtrlUnit_ATK_BREATH() {
    m_owner->m_motionLocked = 0;
    m_owner->m_motionLocked2 = 0;
}

int BlackBull0412ModeCtrlUnit_ATK_BREATH::Tick(float dt) {
    if (m_owner->m_attribute) {
        Mtx34f *matrix = fn_80415E70(m_owner->m_motion, 0x13A6);
        Vec3f translation;
        fn_804114D0(&translation, matrix);
        Vec3f position;
        fn_80016350(&position, &translation);
        Vec3f velocity(matrix->values[0][2], matrix->values[1][2], matrix->values[2][2]);
        AssignVector(velocity, fn_80016284(velocity));
        fn_8040CE74(&velocity);
        fn_8040CE38(&velocity, m_owner->m_fireSpeed);
        fn_803358D0(m_owner, new BlackBullCtrlUnit_FireBall(m_owner, &position, &velocity, 0));
        m_owner->m_attribute = 0;
    }
    if (fn_80416F28(m_owner->m_motion, 0, 3, 1)) {
        m_owner->m_nextMode = 2;
    }
    return 0;
}

BlackBull0412ModeCtrlUnit_DEATH::~BlackBull0412ModeCtrlUnit_DEATH() {
    boost::shared_ptr<EffectElement> effect = m_effect.lock();
    if (effect) {
        effect->Stop();
        fn_800A4210(effect);
    }
}

int BlackBull0412ModeCtrlUnit_DEATH::Tick(float dt) {
    if (m_owner->m_attribute) {
        m_effect = fn_800A31F0()->Launch(m_owner->m_effects, 0);
        boost::shared_ptr<EffectElement> effect = m_effect.lock();
        if (effect) {
            Vec3f position;
            fn_800091EC(&position, &m_owner->m_chara->m_position);
            position.y = 0.0f;
            fn_80425E4C(effect.get(), &position);
            effect->SetField14(FrameTime());
        }
        m_owner->m_attribute = 0;
    }
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

BlackBull0412ModeCtrlUnit_BELOW_HALF::~BlackBull0412ModeCtrlUnit_BELOW_HALF() {
    m_owner->m_stopped = 0;
    m_owner->field_0x171 = 1;
    m_owner->m_active = 1;
    fn_801FAAA4(GetHintManager());
    fn_801FAF00(GetHintManager(), 0xB2, 2, 0, 0);
    fn_801FAAB4(GetHintManager());
}

int BlackBull0412ModeCtrlUnit_BELOW_HALF::Tick(float dt) {
    if (IsMotion(m_owner, 1)) {
        if (fn_80416F28(m_owner->m_motion, 0, 3, 1)) {
            fn_804165D8(m_owner->m_motion, 0, -1, -1);
            fn_80416148(m_owner->m_motion, 0, 20.0f);
            fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 5);
        }
    } else if (IsMotion(m_owner, 5)) {
        if (fn_80416F28(m_owner->m_motion, 0, 6, 1)) {
            m_owner->m_nextMode = 2;
        }
    }
    return 0;
}

BlackBull0412ModeCtrlUnit_GET_ATK::~BlackBull0412ModeCtrlUnit_GET_ATK() {
    m_owner->m_stopped = 0;
    if (!m_owner->m_dizzy && m_owner->m_comboCount >= m_owner->m_comboLimit) {
        if (HealthRatio(m_owner) <= 0.5f) {
            fn_801FAF00(GetHintManager(), 0xB5, 2, 0, 0);
        } else {
            fn_801FAF00(GetHintManager(), 0xB4, 2, 0, 0);
        }
    }
}

int BlackBull0412ModeCtrlUnit_GET_ATK::Tick(float dt) {
    if (m_owner->m_stopped) {
        m_time += lbl_8057D6A0.field_0x28;
        if (m_time > m_owner->m_stunTime && m_owner->m_comboCount < m_owner->m_comboLimit) {
            m_owner->m_stopped = 0;
            if (!m_owner->m_dizzy && m_owner->m_comboCount >= m_owner->m_comboLimit) {
                if (HealthRatio(m_owner) <= 0.5f) {
                    fn_801FAF00(GetHintManager(), 0xB5, 2, 0, 0);
                } else {
                    fn_801FAF00(GetHintManager(), 0xB4, 2, 0, 0);
                }
            }
        }
    }
    if (fn_80416F28(m_owner->m_motion, 0, 3, 1)) {
        fn_80416198(m_owner->m_motion, 0);
        m_owner->m_nextMode = 6;
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

BlackBull0412ModeCtrlUnit_GET_COMBO::~BlackBull0412ModeCtrlUnit_GET_COMBO() {}

int BlackBull0412ModeCtrlUnit_GET_COMBO::Tick(float dt) {
    if (m_owner->m_comboCount >= m_owner->m_comboLimit) {
        m_owner->m_comboCount = 0;
        m_owner->m_nextMode = 4;
        return 0;
    }
    if (m_owner->m_hit || m_owner->field_0x168) {
        m_owner->m_comboCount = 0;
        m_owner->m_nextMode = 2;
    }
    return 0;
}

BlackBull0412ModeCtrlUnit_ESCAPE::~BlackBull0412ModeCtrlUnit_ESCAPE() {
    m_owner->field_0x171 = 1;
}

int BlackBull0412ModeCtrlUnit_ESCAPE::Tick(float dt) {
    if (m_owner->m_motionLocked) {
        m_speed += m_owner->m_settings.field_0x8 * dt;
        m_owner->m_chara->m_position.y += m_speed * dt;
        if (m_owner->m_chara->m_position.y >= m_owner->m_settings.field_0xC) {
            m_owner->m_motionLocked = 0;
            m_owner->m_velocity.y = m_speed;
            fn_800091D0(&m_owner->m_destination, &m_owner->m_position);
        }
    } else if (m_owner->m_destinationDistance < m_owner->m_arriveDistance) {
        m_owner->m_nextMode = 2;
        m_owner->m_arrived = 1;
    }
    return 0;
}

BlackBull0412ModeCtrlUnit_EDIT::~BlackBull0412ModeCtrlUnit_EDIT() {}

int BlackBull0412ModeCtrlUnit_EDIT::Tick(float dt) {
    return 0;
}

BlackBull0412ModeCtrlUnit_WAIT::~BlackBull0412ModeCtrlUnit_WAIT() {}

int BlackBull0412ModeCtrlUnit_WAIT::Tick(float dt) {
    m_time -= dt;
    if (m_time < 0.0f) {
        m_time = 0.0f;
    }
    if (m_time > 0.0f) {
        return 0;
    }
    if (m_owner->field_0x168) {
        if (HealthRatio(m_owner) <= 0.5f) {
            m_owner->m_nextMode = 0xC;
        } else {
            m_owner->m_nextMode = 0xB;
        }
    } else if (m_owner->m_hit) {
        if (m_owner->field_0x15C < m_owner->field_0xB0) {
            m_owner->m_nextMode = 0xD;
        } else {
            m_owner->m_nextMode = 0xA;
        }
    }
    return 0;
}

BossCtrlUnit *BlackBull0412ModeCtrlUnit::CreateMode(int mode) {
    switch (mode) {
    case 0:
    case 2:
    case 3:
    case 5:
    default:
        return new BlackBull0412ModeCtrlUnit_WAIT(m_owner);
    case 1:
        return new BlackBull0412ModeCtrlUnit_EDIT(m_owner);
    case 4:
        return new BlackBull0412ModeCtrlUnit_ESCAPE(m_owner);
    case 6:
        return new BlackBull0412ModeCtrlUnit_GET_COMBO(m_owner);
    case 7:
        return new BlackBull0412ModeCtrlUnit_GET_ATK(m_owner);
    case 8:
        return new BlackBull0412ModeCtrlUnit_BELOW_HALF(m_owner);
    case 9:
        return new BlackBull0412ModeCtrlUnit_DEATH(m_owner);
    case 10:
        return new BlackBull0412ModeCtrlUnit_ATK_BREATH(m_owner);
    case 11:
        return new BlackBull0412ModeCtrlUnit_ATK_WAVERING(m_owner);
    case 13:
        return new BlackBull0412ModeCtrlUnit_ATK_CLOSERANGE(m_owner);
    case 12:
        return new BlackBull0412ModeCtrlUnit_ATK_CHARGE(m_owner);
    case 14:
        return new BlackBull0412ModeCtrlUnit_START(m_owner);
    }
}

BlackBull0412ModeCtrlUnit::~BlackBull0412ModeCtrlUnit() {}

int BlackBull0412ModeCtrlUnit::Tick(float dt) {
    return BossModeCtrlUnit::Tick(dt);
}
