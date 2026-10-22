/* BlackDoomModeCtrlUnit (class name from RTTI): the mode control unit of the Black Doom boss.
 * CreateMode builds one BlackDoomModeCtrlUnit_<MODE> unit (class names from RTTI) for each of the
 * boss's modes; each one sets up its motion in its constructor, runs in Tick (which asks the boss
 * for the next mode by setting BossVarParam::m_nextMode) and cleans up in its destructor. The
 * mode units derive from BlackDoomCtrlUnit, whose BossCtrlUnit base is virtual, so CodeWarrior
 * emits this-adjusting thunks for their vtables at the end of the unit. File name inferred from
 * the class (as Game/DevilDoomModeCtrlUnit.cpp). Field and helper names are inferred. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first); the mode
 * units' constructors are inline and are emitted after CreateMode, their first user. */
#include "Effect/System/EffectElement.h"
#include "Game/BlackDoomCtrlUnit.h"
#include "Game/BossCtrlUnit.h"
#include "Game/CharInfo.h"
#include "Game/EffectPJSLauncher.h"
#include "Game/EnemyCharInfo.h"
#include "Game/MathTypes.h"
#include "Game/Vibration.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Effect::System::EffectElement;
using Effect::System::EffectLaunchInfo;

struct _stMtnPlayWork;
struct UnkHintManager_801FB180;
struct UnkObject_80074470;

/* What fn_80415D40 fills in about a motion (as in Game/BlackBull0210ModeCtrlUnit.cpp). */
struct UnkMotionFrames {
    /* 0x00 */ float m_start;
    /* 0x04 */ float m_end;
    /* 0x08 */ float field_0x8;
    /* 0x0C */ float m_frame;
};

/* The effect manager fn_800A31F0 returns (as in Game/DevilDoomCtrlUnit.cpp). */
class UnkEffectManager_800A31F0 {
  public:
    virtual void vf08();
    virtual boost::weak_ptr<EffectElement> LaunchId(s32 id, s32 arg);
    virtual boost::weak_ptr<EffectElement> Launch(boost::shared_ptr<EffectLaunchInfo> info, s32 id);
};

/* The game state (struct name unknown; see AdvMemoryCheck.cpp): the time step at 0x18 (meaning
 * inferred), the frame time at 0x24 and the stage number at 0xA8. */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x18];
    /* 0x018 */ float m_step;
    /* 0x01C */ u8 field_0x1C[0x24 - 0x1C];
    /* 0x024 */ float m_dt;
    /* 0x028 */ u8 field_0x28[0xA8 - 0x28];
    /* 0x0A8 */ s32 m_stage;
    /* 0x0AC */ u8 field_0xAC[0x150 - 0xAC];
};
extern UnkGameState_8057D6A0 lbl_8057D6A0;

#define FrameTime() ((u32)&lbl_8057D6A0.m_dt)

extern "C" {
u16 fn_80415CDC(_stMtnPlayWork *work, int arg1);
void fn_80417568(_stMtnPlayWork *work, u16 motion);
void fn_804165D8(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
void fn_80416148(_stMtnPlayWork *work, int arg1, float arg2);
int fn_80416F28(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
void fn_80415D40(_stMtnPlayWork *work, int arg1, UnkMotionFrames *frames);
void fn_80416E20(_stMtnPlayWork *work, int arg1, float frame);
void fn_80416014(_stMtnPlayWork *work, int arg1, float value);
void fn_80416198(_stMtnPlayWork *work, int arg1);
void fn_80415F98(_stMtnPlayWork *work, int arg1, float arg2, int motion, float time);

s32 fn_801FAF00(UnkHintManager_801FB180 *manager, s32 id, s32 branch, int arg3, int arg4);
void fn_8035367C(const char *file, const char *layout, const char *animation);
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int arg1);
UnkEffectManager_800A31F0 *fn_800A31F0(void);
void fn_800A4210(const boost::shared_ptr<EffectElement> &effect);
UnkObject_80074470 *fn_80074470(void);
void fn_80175EEC(UnkObject_80074470 *object, int arg1, int arg2);
float fn_8040EE90(void);

/* Sounds (see BAGunShip.cpp): play id at a character, a looped one keyed by an owner, and a
 * voice (behaviour inferred). */
void *fn_80053164(void);
void fn_801D54F4(void *manager, int id, const void *source, int arg3, int arg4);
void fn_801D53C4(void *manager, int id, const void *source, const void *owner, int arg4, int arg5,
                 int arg6);
void fn_801D51A0(void *manager, int id, int arg2, int arg3);

Vec3f fn_8040CEE8(const Vec3f *vector);
/* vector * scale (declared with the scale first: the caller evaluates it before the vector). */
Vec3f fn_8040CDA8(float scale, const Vec3f *vector);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
}

UnkHintManager_801FB180 *GetHintManager();

typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

inline void ConstructVector(Vec3f *vector, const Vec3f &value) {
    fn_800091EC(vector, &value);
}

inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* Whether motion number index of the boss's set is playing (name inferred). */
inline bool IsMotion(BlackDoomVarParam *owner, int index) {
    u16 motion = owner->m_motionBase + index;
    u16 current = fn_80415CDC(owner->m_motion, 0);
    return motion == current;
}

/* Plays sound id at the boss (name inferred). */
inline void PlaySound(int id, const void *chara) {
    fn_801D54F4(fn_80053164(), id, chara, 0, 1);
}

/* WAIT: waits, then gets ready to attack. */
class BlackDoomModeCtrlUnit_WAIT : public BlackDoomCtrlUnit {
  public:
    BlackDoomModeCtrlUnit_WAIT(BlackDoomVarParam *owner) : BossCtrlUnit(owner), BlackDoomCtrlUnit(owner) {
        m_time = 0.0f;
        m_owner->m_moveLocked = 0;
        m_owner->m_turnLocked = 0;
        m_owner->m_comboCount = 0;
        m_time = 0.0f;
        m_owner->m_accel = m_owner->m_settings.m_accel;
    }
    virtual ~BlackDoomModeCtrlUnit_WAIT();
    virtual int Tick(float dt);

    /* 0x0C */ float m_time;
};

class BlackDoomModeCtrlUnit_EDIT : public BlackDoomCtrlUnit {
  public:
    BlackDoomModeCtrlUnit_EDIT(BlackDoomVarParam *owner) : BossCtrlUnit(owner), BlackDoomCtrlUnit(owner) {
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 7);
        m_owner->m_moveLocked = 1;
        m_owner->m_turnLocked = 1;
    }
    virtual ~BlackDoomModeCtrlUnit_EDIT();
    virtual int Tick(float dt);
};

/* GET_COMBO: takes hits until the combo ends. */
class BlackDoomModeCtrlUnit_GET_COMBO : public BlackDoomCtrlUnit {
  public:
    BlackDoomModeCtrlUnit_GET_COMBO(BlackDoomVarParam *owner)
        : BossCtrlUnit(owner), BlackDoomCtrlUnit(owner) {
        m_owner->m_moveLocked = 0;
        m_owner->m_accel = 0.0f;
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 7);
        fn_80416148(m_owner->m_motion, 0, 30.0f);
    }
    virtual ~BlackDoomModeCtrlUnit_GET_COMBO();
    virtual int Tick(float dt);
};

/* GET_ATK: reels from a hit. */
class BlackDoomModeCtrlUnit_GET_ATK : public BlackDoomCtrlUnit {
  public:
    BlackDoomModeCtrlUnit_GET_ATK(BlackDoomVarParam *owner) : BossCtrlUnit(owner), BlackDoomCtrlUnit(owner) {
        if (!IsMotion(m_owner, 6)) {
            fn_804165D8(m_owner->m_motion, 2, -1, -1);
            fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 6);
            fn_80416014(m_owner->m_motion, 0, 0.0f);
            fn_80415F98(m_owner->m_motion, 0, 0.0f, 0x1E, 1.0f);
        } else {
            fn_80416E20(m_owner->m_motion, 0, 0.0f);
        }
        m_owner->m_stopped = 1;
        m_owner->m_moveLocked = 0;
        m_owner->m_accel = 0.0f;
        m_owner->m_turnLocked = 0;
        m_owner->m_comboCount++;
        switch (lbl_8057D6A0.m_stage) {
        case 0x262:
            fn_801D51A0(fn_80053164(), 0xC0D, 0, -1);
            break;
        case 0x268:
            fn_801D51A0(fn_80053164(), 0xC83, 0, -1);
            break;
        case 0x269:
            fn_801D51A0(fn_80053164(), 0xC95, 0, -1);
            break;
        }
    }
    virtual ~BlackDoomModeCtrlUnit_GET_ATK();
    virtual int Tick(float dt);

    /* 0x0C */ float m_time;
};

/* ATK_READY: picks the next attack. */
class BlackDoomModeCtrlUnit_ATK_READY : public BlackDoomCtrlUnit {
  public:
    BlackDoomModeCtrlUnit_ATK_READY(BlackDoomVarParam *owner)
        : BossCtrlUnit(owner), BlackDoomCtrlUnit(owner) {
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 7);
        fn_80416148(m_owner->m_motion, 0, 10.0f);
        if (m_owner->m_hidden) {
            fn_801D54F4(fn_80053164(), 0x802F, m_owner->m_chara, 0, 1);
        }
        m_owner->m_hidden = 0;
        m_owner->m_moveLocked = 0;
        m_owner->m_turnLocked = 0;
        m_owner->m_stopped = 1;
        m_owner->m_accel = m_owner->m_settings.m_accel;
        m_owner->m_damage = 0.0f;
        m_state = 0;
    }
    virtual ~BlackDoomModeCtrlUnit_ATK_READY();
    virtual int Tick(float dt);

    /* 0x0C */ float m_time;
    /* 0x10 */ s32 m_state;
};

/* ATK_DISK: throws two disks. */
class BlackDoomModeCtrlUnit_ATK_DISK : public BlackDoomCtrlUnit {
  public:
    BlackDoomModeCtrlUnit_ATK_DISK(BlackDoomVarParam *owner) : BossCtrlUnit(owner), BlackDoomCtrlUnit(owner) {
        m_owner->m_moveLocked = 0;
        m_owner->m_accel = 0.0f;
        m_owner->m_turnLocked = 0;
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 1);
        fn_80416148(m_owner->m_motion, 0, 10.0f);
        UpdatePositions();
        boost::shared_ptr<EffectElement> effect;
        for (int i = 0; i < 2; i++) {
            m_effects[i] = fn_800A31F0()->Launch(m_owner->m_effects, 6);
            effect = m_effects[i].lock();
            if (effect) {
                effect->SetField4C((u32)&m_positions[i]);
                effect->SetField14(FrameTime());
            }
        }
        m_owner->m_attribute = 0;
        m_count = 2;
        m_index = 2.0f * fn_8040EE90();
        switch (lbl_8057D6A0.m_stage) {
        case 0x262:
            fn_801FAF00(GetHintManager(), 0xFF, 0, 0, 0);
            break;
        case 0x268:
            fn_801FAF00(GetHintManager(), 0x175, 0, 0, 0);
            break;
        case 0x269:
            fn_801FAF00(GetHintManager(), 0x183, 0, 0, 0);
            break;
        }
    }
    virtual ~BlackDoomModeCtrlUnit_ATK_DISK();
    virtual int Tick(float dt);

    /* Places the disks at the boss's hands (name inferred). */
    void UpdatePositions() {
        Mtx34f matrix;
        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_owner->m_chara->m_position);
        fn_8040B848(&matrix, m_owner->m_chara->m_rotation.y);
        Vec3f offsets[2];
        Vec3f offset0(30.0f, 20.0f, 0.0f);
        fn_800091EC(&offsets[0], &offset0);
        Vec3f offset1(-30.0f, 20.0f, 0.0f);
        fn_800091EC(&offsets[1], &offset1);
        for (int i = 0; i < 2; i++) {
            AssignVector(m_positions[i], fn_8040B67C(&matrix, &offsets[i]));
        }
    }

    /* 0x0C */ Vec3f m_positions[2];
    /* 0x24 */ boost::weak_ptr<EffectElement> m_effects[2];
    /* 0x34 */ s32 m_count;
    /* 0x38 */ s32 m_index;
};

/* ATK_METEOR: rains meteors from five places above the boss. */
class BlackDoomModeCtrlUnit_ATK_METEOR : public BlackDoomCtrlUnit {
  public:
    BlackDoomModeCtrlUnit_ATK_METEOR(BlackDoomVarParam *owner)
        : BossCtrlUnit(owner), BlackDoomCtrlUnit(owner) {
        m_owner->m_moveLocked = 0;
        m_owner->m_accel = 0.0f;
        m_owner->m_turnLocked = 0;
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 3);
        fn_80416148(m_owner->m_motion, 0, 10.0f);
        UpdatePositions();
        m_owner->m_attribute = 0;
        m_count = 5;
        m_index = 5.0f * fn_8040EE90();
        m_done = 0;
        m_playing = 0;
        m_time = 0.0f;
        switch (lbl_8057D6A0.m_stage) {
        case 0x262:
            fn_801FAF00(GetHintManager(), 0x100, 0, 0, 0);
            break;
        case 0x268:
            fn_801FAF00(GetHintManager(), 0x176, 0, 0, 0);
            break;
        case 0x269:
            fn_801FAF00(GetHintManager(), 0x184, 0, 0, 0);
            break;
        }
    }
    virtual ~BlackDoomModeCtrlUnit_ATK_METEOR();
    virtual int Tick(float dt);

    /* Places the meteors around the boss (name inferred). */
    void UpdatePositions() {
        Mtx34f matrix;
        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_owner->m_chara->m_position);
        fn_8040B848(&matrix, m_owner->m_chara->m_rotation.y);
        Vec3f offsets[5];
        Vec3f offset0(-25.0f, 10.0f, 0.0f);
        fn_800091EC(&offsets[0], &offset0);
        Vec3f offset1(-30.0f, 45.0f, 0.0f);
        fn_800091EC(&offsets[1], &offset1);
        Vec3f offset2(0.0f, 60.0f, 0.0f);
        fn_800091EC(&offsets[2], &offset2);
        Vec3f offset3(30.0f, 45.0f, 0.0f);
        fn_800091EC(&offsets[3], &offset3);
        Vec3f offset4(25.0f, 10.0f, 0.0f);
        fn_800091EC(&offsets[4], &offset4);
        for (int i = 0; i < 5; i++) {
            AssignVector(m_positions[i], fn_8040B67C(&matrix, &offsets[i]));
        }
    }

    /* 0x0C */ Vec3f m_positions[5];
    /* 0x48 */ boost::weak_ptr<EffectElement> m_effects[5];
    /* 0x70 */ float m_time;
    /* 0x74 */ u8 m_done;
    /* 0x75 */ u8 m_playing;
    /* 0x78 */ s32 m_count;
    /* 0x7C */ s32 m_index;
};

/* ATK_BOOMERANG: throws a boomerang. */
class BlackDoomModeCtrlUnit_ATK_BOOMERANG : public BlackDoomCtrlUnit {
  public:
    BlackDoomModeCtrlUnit_ATK_BOOMERANG(BlackDoomVarParam *owner)
        : BossCtrlUnit(owner), BlackDoomCtrlUnit(owner) {
        m_owner->m_moveLocked = 0;
        m_owner->m_accel = 0.0f;
        m_owner->m_turnLocked = 0;
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 2);
        fn_80416148(m_owner->m_motion, 0, 10.0f);
        m_effect = fn_800A31F0()->Launch(m_owner->m_effects, 0x1E);
        boost::shared_ptr<EffectElement> effect(m_effect.lock());
        if (effect) {
            AssignVector(m_position, m_owner->m_chara->m_position);
            m_position.y = 5.0f + (100.0f + m_owner->m_home.y);
            effect->SetField4C((u32)&m_position);
            effect->SetField14(FrameTime());
        }
        PlaySound(0x8037, m_owner->m_chara);
        m_owner->m_attribute = 0;
        switch (lbl_8057D6A0.m_stage) {
        case 0x262:
            fn_801FAF00(GetHintManager(), 0x101, 0, 0, 0);
            break;
        case 0x268:
            fn_801FAF00(GetHintManager(), 0x177, 0, 0, 0);
            break;
        case 0x269:
            fn_801FAF00(GetHintManager(), 0x185, 0, 0, 0);
            break;
        }
    }
    virtual ~BlackDoomModeCtrlUnit_ATK_BOOMERANG();
    virtual int Tick(float dt);

    /* 0x0C */ boost::weak_ptr<EffectElement> m_effect;
    /* 0x14 */ Vec3f m_position;
};

class BlackDoomModeCtrlUnit_START : public BlackDoomCtrlUnit {
  public:
    BlackDoomModeCtrlUnit_START(BlackDoomVarParam *owner) : BossCtrlUnit(owner), BlackDoomCtrlUnit(owner) {
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 8);
        m_owner->m_stopped = 1;
        m_owner->m_moveLocked = 1;
        m_owner->m_turnLocked = 1;
        fn_803358D0(m_owner, new BossCtrlUnit_PlayCameraMotion(m_owner, 0x10, 0.0f, 0.0f));
        m_owner->m_active = 0;
        switch (lbl_8057D6A0.m_stage) {
        case 0x262:
            fn_801FAF00(GetHintManager(), 0xFB, 0, 0, 0);
            break;
        case 0x268:
            fn_801FAF00(GetHintManager(), 0x171, 0, 0, 0);
            break;
        case 0x269:
            fn_801FAF00(GetHintManager(), 0x17F, 0, 0, 0);
            break;
        }
        fn_8035367C("CsdFiles/BChara_BDoom", "bchara", "open");
    }
    virtual ~BlackDoomModeCtrlUnit_START();
    virtual int Tick(float dt);
};

class BlackDoomModeCtrlUnit_BELOW_HALF : public BlackDoomCtrlUnit {
  public:
    BlackDoomModeCtrlUnit_BELOW_HALF(BlackDoomVarParam *owner)
        : BossCtrlUnit(owner), BlackDoomCtrlUnit(owner) {
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80416148(m_owner->m_motion, 0, 20.0f);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 4);
        m_owner->m_stopped = 1;
        m_owner->m_moveLocked = 1;
        m_owner->m_turnLocked = 1;
        fn_803358D0(m_owner, new BossCtrlUnit_PlayCameraMotion(m_owner, 0xE, 0.0f, 0.0f));
        m_owner->m_active = 0;
        switch (lbl_8057D6A0.m_stage) {
        case 0x262:
            fn_801FAF00(GetHintManager(), 0xFD, 0, 0, 0);
            break;
        case 0x268:
            fn_801FAF00(GetHintManager(), 0x173, 0, 0, 0);
            break;
        case 0x269:
            fn_801FAF00(GetHintManager(), 0x181, 0, 0, 0);
            break;
        }
        fn_8034DF48(0xC, -2);
    }
    virtual ~BlackDoomModeCtrlUnit_BELOW_HALF();
    virtual int Tick(float dt);
};

class BlackDoomModeCtrlUnit_DEATH : public BlackDoomCtrlUnit {
  public:
    BlackDoomModeCtrlUnit_DEATH(BlackDoomVarParam *owner) : BossCtrlUnit(owner), BlackDoomCtrlUnit(owner) {
        m_ended = 0;
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 5);
        m_owner->m_chara->m_position.y = 0.0f;
        m_owner->m_stopped = 1;
        m_owner->m_moveLocked = 1;
        m_owner->m_turnLocked = 1;
        fn_803358D0(m_owner, new BossCtrlUnit_PlayCameraMotion(m_owner, 0xF, 0.0f, 0.0f));
        m_owner->m_active = 0;
        switch (lbl_8057D6A0.m_stage) {
        case 0x262:
            fn_801FAF00(GetHintManager(), 0xFE, 0, 0, 0);
            break;
        case 0x268:
            fn_801FAF00(GetHintManager(), 0x174, 0, 0, 0);
            break;
        case 0x269:
            fn_801FAF00(GetHintManager(), 0x182, 0, 0, 0);
            break;
        }
        fn_8034DF48(7, -2);
    }
    virtual ~BlackDoomModeCtrlUnit_DEATH();
    virtual int Tick(float dt);

    /* 0x0C */ boost::weak_ptr<EffectElement> m_effect;
    /* 0x14 */ u8 m_ended;
};

class BlackDoomModeCtrlUnit_MOVETEST : public BlackDoomCtrlUnit {
  public:
    BlackDoomModeCtrlUnit_MOVETEST(BlackDoomVarParam *owner)
        : BossCtrlUnit(owner), BlackDoomCtrlUnit(owner) {
        m_owner->m_moveLocked = 0;
        m_owner->m_turnLocked = 0;
        m_owner->m_accel = m_owner->m_settings.m_accel;
    }
    virtual ~BlackDoomModeCtrlUnit_MOVETEST();
    virtual int Tick(float dt);
};

SIZE_ASSERT(BlackDoomModeCtrlUnit, 0x28);
SIZE_ASSERT(BlackDoomModeCtrlUnit_WAIT, 0x20);
SIZE_ASSERT(BlackDoomModeCtrlUnit_EDIT, 0x1C);
SIZE_ASSERT(BlackDoomModeCtrlUnit_GET_ATK, 0x20);
SIZE_ASSERT(BlackDoomModeCtrlUnit_ATK_READY, 0x24);
SIZE_ASSERT(BlackDoomModeCtrlUnit_ATK_DISK, 0x4C);
SIZE_ASSERT(BlackDoomModeCtrlUnit_ATK_METEOR, 0x90);
SIZE_ASSERT(BlackDoomModeCtrlUnit_ATK_BOOMERANG, 0x30);
SIZE_ASSERT(BlackDoomModeCtrlUnit_DEATH, 0x28);

BlackDoomModeCtrlUnit_MOVETEST::~BlackDoomModeCtrlUnit_MOVETEST() {}

int BlackDoomModeCtrlUnit_MOVETEST::Tick(float dt) {
    m_owner->m_accel = m_owner->m_settings.m_accel;
    if (m_owner->m_alterEgos == 0 && m_owner->m_attacks[0].m_state == 0) {
        m_owner->m_attacks[0].m_state = 1;
        if (m_owner->m_settings.m_attackCount >= 0) {
            m_owner->m_attackStep = m_owner->m_settings.m_attackCount;
        }
    }
    m_owner->m_nextMode = 2;
    return 0;
}

BlackDoomModeCtrlUnit_DEATH::~BlackDoomModeCtrlUnit_DEATH() {
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        effect->Stop();
        fn_800A4210(effect);
    }
}

int BlackDoomModeCtrlUnit_DEATH::Tick(float dt) {
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

BlackDoomModeCtrlUnit_BELOW_HALF::~BlackDoomModeCtrlUnit_BELOW_HALF() {
    m_owner->m_stopped = 0;
    m_owner->m_active = 1;
}

int BlackDoomModeCtrlUnit_BELOW_HALF::Tick(float dt) {
    if (IsMotion(m_owner, 4)) {
        if (fn_80416F28(m_owner->m_motion, 0, 3, 1) != 0) {
            fn_804165D8(m_owner->m_motion, 0, -1, -1);
            fn_80416148(m_owner->m_motion, 0, 20.0f);
            fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 7);
        }
    } else if (IsMotion(m_owner, 7) && fn_80416F28(m_owner->m_motion, 0, 6, 1) != 0) {
        m_owner->m_nextMode = 2;
    }
    return 0;
}

BlackDoomModeCtrlUnit_START::~BlackDoomModeCtrlUnit_START() {
    m_owner->m_stopped = 0;
    m_owner->m_active = 1;
}

int BlackDoomModeCtrlUnit_START::Tick(float dt) {
    if (fn_80416F28(m_owner->m_motion, 0, 3, 1) != 0) {
        m_owner->m_nextMode = 2;
    }
    return 0;
}

BlackDoomModeCtrlUnit_ATK_BOOMERANG::~BlackDoomModeCtrlUnit_ATK_BOOMERANG() {
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        effect->Stop();
        fn_800A4210(effect);
    }
    m_owner->m_time = 0.0f;
}

int BlackDoomModeCtrlUnit_ATK_BOOMERANG::Tick(float dt) {
    AssignVector(m_position, m_owner->m_chara->m_position);
    m_position.y = 5.0f + (100.0f + m_owner->m_home.y);
    if (m_owner->m_attribute) {
        Vec3f start;
        ConstructVector(&start, m_owner->m_chara->m_position);
        start.y = m_owner->m_home.y - 35.0f;
        Vec3f velocity;
        ConstructVector(&velocity, ((VectorSubtractFn)fn_8040CD74)(&m_owner->m_testDestination, &start));
        velocity.y = 0.0f;
        float speed = m_owner->m_settings.m_boomerangSpeed;
        AssignVector(velocity, fn_8040CDA8(speed, &(const Vec3f &)fn_8040CEE8(&velocity)));
        fn_803358D0(m_owner, new BlackDoomCtrlUnit_Boomerang(m_owner, &start, &velocity));
        boost::shared_ptr<EffectElement> effect(m_effect.lock());
        if (effect) {
            effect->Stop();
            fn_800A4210(effect);
        }
        m_owner->m_attribute = 0;
        m_owner->m_stopped = 0;
    }
    if (fn_80416F28(m_owner->m_motion, 0, 3, 1) != 0) {
        m_owner->m_nextMode = 3;
    }
    return 0;
}

BlackDoomModeCtrlUnit_ATK_METEOR::~BlackDoomModeCtrlUnit_ATK_METEOR() {
    boost::shared_ptr<EffectElement> effect;
    for (int i = 0; i < 5; i++) {
        effect = m_effects[i].lock();
        if (effect) {
            effect->Stop();
            fn_800A4210(effect);
        }
    }
    m_owner->m_time = 0.0f;
}

int BlackDoomModeCtrlUnit_ATK_METEOR::Tick(float dt) {
    m_time -= dt;
    UpdatePositions();
    if (m_playing) {
        fn_801D53C4(fn_80053164(), 0x8031, m_owner->m_chara, this, 3, 0, 3);
    }
    if (!m_done && m_owner->m_attribute) {
        if (m_owner->m_attribute->m_state == 1) {
            int index = m_owner->m_attribute->m_index;
            boost::shared_ptr<EffectElement> effect;
            m_effects[index] = fn_800A31F0()->Launch(m_owner->m_effects, 6);
            effect = m_effects[index].lock();
            if (effect) {
                effect->SetField4C((u32)&m_positions[index]);
                effect->SetField14(FrameTime());
            }
            m_playing = 1;
        } else if (m_owner->m_attribute->m_state == 2) {
            m_done = 1;
            m_owner->m_stopped = 0;
        }
        m_owner->m_attribute = 0;
    }
    if (m_done && m_time < 0.0f) {
        if (m_count == 0) {
            m_owner->m_nextMode = 3;
            return 0;
        }
        Vec3f velocity;
        ConstructVector(&velocity,
                        ((VectorSubtractFn)fn_8040CD74)(&m_owner->m_testDestination, &m_positions[m_index]));
        float speed = m_owner->m_settings.m_meteorSpeed;
        AssignVector(velocity, fn_8040CDA8(speed, &(const Vec3f &)fn_8040CEE8(&velocity)));
        fn_803358D0(m_owner, new BlackDoomCtrlUnit_Meteor(m_owner, &m_positions[m_index], &velocity));
        m_count--;
        m_index = 5.0f * fn_8040EE90();
        m_time = m_owner->m_settings.m_meteorInterval;
    }
    fn_80416F28(m_owner->m_motion, 0, 3, 1);
    return 0;
}

BlackDoomModeCtrlUnit_ATK_DISK::~BlackDoomModeCtrlUnit_ATK_DISK() {
    boost::shared_ptr<EffectElement> effect;
    for (int i = 0; i < 2; i++) {
        effect = m_effects[i].lock();
        if (effect) {
            effect->Stop();
            fn_800A4210(effect);
        }
    }
    m_owner->m_time = 0.0f;
}

int BlackDoomModeCtrlUnit_ATK_DISK::Tick(float dt) {
    UpdatePositions();
    fn_801D53C4(fn_80053164(), 0x8031, m_owner->m_chara, this, 3, 0, 3);
    if (m_owner->m_attribute) {
        Vec3f velocity;
        ConstructVector(&velocity,
                        ((VectorSubtractFn)fn_8040CD74)(&m_owner->m_testDestination, &m_positions[m_index]));
        velocity.y = 0.0f;
        float speed = m_owner->m_settings.m_diskSpeed;
        AssignVector(velocity, fn_8040CDA8(speed, &(const Vec3f &)fn_8040CEE8(&velocity)));
        fn_803358D0(m_owner, new BlackDoomCtrlUnit_Disk(m_owner, &m_positions[m_index], &velocity));
        m_count--;
        m_owner->m_attribute = 0;
        m_index = m_index ? 0 : 1;
        if (m_count != 0) {
            UnkMotionFrames frames;
            fn_80415D40(m_owner->m_motion, 0, &frames);
            float blend = m_owner->m_settings.m_diskRewind;
            if (blend > m_owner->m_settings.m_diskBlend) {
                blend = m_owner->m_settings.m_diskBlend;
            }
            float frame = frames.m_frame - m_owner->m_settings.m_diskRewind;
            fn_804165D8(m_owner->m_motion, 0, -1, -1);
            fn_80416148(m_owner->m_motion, 0, blend);
            fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 1);
            if (frame > 0.0f) {
                fn_80416E20(m_owner->m_motion, 0, frame);
            }
        }
        m_owner->m_stopped = 0;
    }
    if (fn_80416F28(m_owner->m_motion, 0, 3, 1) != 0) {
        m_owner->m_nextMode = 3;
    }
    return 0;
}

BlackDoomModeCtrlUnit_ATK_READY::~BlackDoomModeCtrlUnit_ATK_READY() {}

int BlackDoomModeCtrlUnit_ATK_READY::Tick(float dt) {
    switch (m_state) {
    case 0:
        if (m_owner->m_alpha < 1.0f) {
            break;
        }
        if (m_owner->m_alterEgos == 0 && m_owner->m_attacks[0].m_state == 0) {
            m_owner->m_attacks[0].m_state = 1;
            if (m_owner->m_settings.m_attackCount >= 0) {
                m_owner->m_attackStep = m_owner->m_settings.m_attackCount;
            } else {
                float health = m_owner->m_health / m_owner->m_maxHealth;
                if (health > 0.75f) {
                    m_owner->m_attackStep = 0;
                    m_owner->m_attackTime = m_owner->m_settings.m_attackTime;
                } else if (health > 0.5f) {
                    m_owner->m_attackStep = 1;
                    m_owner->m_attackTime = m_owner->m_settings.m_attackTime;
                } else if (health > 0.25f) {
                    m_owner->m_attackStep = 2;
                    m_owner->m_attackTime = m_owner->m_settings.m_attackTime;
                } else if (m_owner->m_attackStep > 6) {
                    m_owner->m_attackStep = 6;
                    if (m_owner->m_attackTime < 1.0f) {
                        m_owner->m_attackTime = 1.0f;
                    }
                } else {
                    m_owner->m_attackTime = m_owner->m_settings.m_attackTime;
                }
            }
        }
        if (m_owner->m_attackStep == 0) {
            m_state = 2;
            m_time = m_owner->m_attackTime;
        } else {
            m_state = 1;
        }
        break;
    case 1:
        if (m_owner->m_alterEgos >= m_owner->m_attackStep) {
            m_state = 2;
            m_time = m_owner->m_attackTime;
        }
        break;
    case 2:
        m_time += dt;
        if (m_time < m_owner->m_attackTime) {
            break;
        }
        m_time = 0.0f;
        int count = 0;
        for (int i = 0; i < 3; i++) {
            if (!m_owner->m_attackUsed[i]) {
                count++;
            }
        }
        if (count == 0) {
            count = 3;
            m_owner->m_attackUsed[0] = m_owner->m_attackUsed[1] = m_owner->m_attackUsed[2] = 0;
        }
        int i;
        int pick = count * fn_8040EE90();
        int n;
        int mode = 0;
        for (i = 0, n = 0; i < 3; i++) {
            if (!m_owner->m_attackUsed[i]) {
                if (n == pick) {
                    switch (i) {
                    case 0:
                        mode = 6;
                        break;
                    case 1:
                        mode = 7;
                        break;
                    case 2:
                        mode = 8;
                        break;
                    }
                    m_owner->m_attackUsed[i] = 1;
                    break;
                }
                n++;
            }
        }
        if (m_owner->m_attackStep != 0 && m_owner->m_attacks[0].m_state == 4) {
            for (int i = m_owner->m_attackStep - 1; i >= 0; i--) {
                if (m_owner->m_attacks[i].m_state == 4) {
                    switch (mode) {
                    case 6:
                        m_owner->m_attacks[i].m_state = 5;
                        break;
                    case 7:
                        m_owner->m_attacks[i].m_state = 6;
                        break;
                    case 8:
                        m_owner->m_attacks[i].m_state = 7;
                        break;
                    }
                    break;
                }
            }
        } else {
            m_owner->m_nextMode = mode;
            m_owner->m_attackStep++;
            m_owner->m_attackTime -= 0.2f;
        }
        break;
    }
    return 0;
}

BlackDoomModeCtrlUnit_GET_ATK::~BlackDoomModeCtrlUnit_GET_ATK() {
    m_owner->m_stopped = 0;
}

int BlackDoomModeCtrlUnit_GET_ATK::Tick(float dt) {
    if (m_owner->m_stopped) {
        m_time += lbl_8057D6A0.m_step;
        if (m_time > m_owner->m_settings.m_stunTime &&
            m_owner->m_damage <= m_owner->m_settings.m_stunDamage &&
            m_owner->m_time <= m_owner->m_settings.m_comboTime) {
            m_owner->m_stopped = 0;
        }
    }
    if (fn_80416F28(m_owner->m_motion, 0, 3, 1) != 0) {
        fn_80416198(m_owner->m_motion, 0);
        m_owner->m_nextMode = 3;
    } else {
        UnkMotionFrames frames;
        fn_80415D40(m_owner->m_motion, 0, &frames);
        if (frames.m_frame > frames.m_end - 4.0f) {
            fn_80415F98(m_owner->m_motion, 0, 0.0f, 0x1392, (frames.m_end - frames.m_frame) / 4.0f);
        }
    }
    return 0;
}

BlackDoomModeCtrlUnit_GET_COMBO::~BlackDoomModeCtrlUnit_GET_COMBO() {}

int BlackDoomModeCtrlUnit_GET_COMBO::Tick(float dt) {
    if (m_owner->m_damage > m_owner->m_settings.m_stunDamage) {
        m_owner->m_stopped = 1;
        if (!m_owner->m_hidden) {
            fn_801D54F4(fn_80053164(), 0x802E, m_owner->m_chara, 0, 1);
        }
        m_owner->m_hidden = 1;
        m_owner->m_nextMode = 2;
        return 0;
    }
    if (m_owner->m_comboCount >= m_owner->m_settings.m_comboLimit) {
        m_owner->m_stopped = 1;
        if (!m_owner->m_hidden) {
            fn_801D54F4(fn_80053164(), 0x802E, m_owner->m_chara, 0, 1);
        }
        m_owner->m_hidden = 1;
        m_owner->m_nextMode = 2;
        return 0;
    }
    if (m_owner->m_time > m_owner->m_settings.m_comboTime) {
        m_owner->m_stopped = 1;
        if (!m_owner->m_hidden) {
            fn_801D54F4(fn_80053164(), 0x802E, m_owner->m_chara, 0, 1);
        }
        m_owner->m_hidden = 1;
        m_owner->m_nextMode = 2;
    }
    return 0;
}

BlackDoomModeCtrlUnit_EDIT::~BlackDoomModeCtrlUnit_EDIT() {}

int BlackDoomModeCtrlUnit_EDIT::Tick(float dt) {
    return 0;
}

BlackDoomModeCtrlUnit_WAIT::~BlackDoomModeCtrlUnit_WAIT() {}

int BlackDoomModeCtrlUnit_WAIT::Tick(float dt) {
    m_time += dt;
    float wait = m_owner->m_settings.m_waitTime;
    wait *= m_owner->m_damageScale;
    wait *= m_owner->m_healthScale;
    if (m_time < wait) {
        return 0;
    }
    if (m_owner->m_alterEgos == 0) {
        m_owner->m_nextMode = 5;
    }
    return 0;
}

BossCtrlUnit *BlackDoomModeCtrlUnit::CreateMode(int mode) {
    switch (mode) {
    case 0:
    default:
        return new BlackDoomModeCtrlUnit_WAIT(m_owner);
    case 1:
        return new BlackDoomModeCtrlUnit_EDIT(m_owner);
    case 3:
        return new BlackDoomModeCtrlUnit_GET_COMBO(m_owner);
    case 4:
        return new BlackDoomModeCtrlUnit_GET_ATK(m_owner);
    case 5:
        return new BlackDoomModeCtrlUnit_ATK_READY(m_owner);
    case 6:
        return new BlackDoomModeCtrlUnit_ATK_DISK(m_owner);
    case 7:
        return new BlackDoomModeCtrlUnit_ATK_METEOR(m_owner);
    case 8:
        return new BlackDoomModeCtrlUnit_ATK_BOOMERANG(m_owner);
    case 9:
        return new BlackDoomModeCtrlUnit_START(m_owner);
    case 10:
        return new BlackDoomModeCtrlUnit_BELOW_HALF(m_owner);
    case 11:
        return new BlackDoomModeCtrlUnit_DEATH(m_owner);
    case 12:
        return new BlackDoomModeCtrlUnit_MOVETEST(m_owner);
    }
}

BlackDoomModeCtrlUnit::~BlackDoomModeCtrlUnit() {}

int BlackDoomModeCtrlUnit::Tick(float dt) {
    return BossModeCtrlUnit::Tick(dt);
}
