/* DevilDoomModeCtrlUnit (class name from RTTI): the mode control unit of Devil Doom, the final
 * boss. CreateMode builds one DevilDoomModeCtrlUnit_<MODE> unit (class names from RTTI) for each
 * of Devil Doom's fourteen modes; each one sets up its motion in its constructor, runs in Tick
 * (which asks the boss for the next mode by setting DevilDoomVarParam::m_nextMode) and cleans up
 * in its destructor. The mode units derive from DevilDoomCtrlUnit, whose BossCtrlUnit base is
 * virtual, so CodeWarrior emits this-adjusting thunks for their vtables at the end of the unit.
 * File name inferred from the class. Field and helper names are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first); the mode units'
 * constructors are inline and are emitted after CreateMode, their first user. */
#include <cmath>

#include "Game/BossCtrlUnit.h"
#include "Game/DevilDoomCtrlUnit.h"
#include "Game/EnemyCharInfo.h"
#include "Game/MathConstants.h"
#include "Game/Vibration.h"
#include "Game/MathTypes.h"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

struct _stMtnPlayWork;
struct UnkHintManager_801FB180;
struct UnkObject_80074470;
struct UnkObject_8057D6A0;

/* What fn_80415D40 fills in about a motion (meanings inferred: the frame at 0x4 is the motion's
 * last and the one at 0xC the current one). */
struct UnkMotionFrames {
    /* 0x00 */ float field_0x0;
    /* 0x04 */ float m_end;
    /* 0x08 */ float field_0x8;
    /* 0x0C */ float m_frame;
};

extern "C" {
/* Adds a control unit to a boss (behaviour inferred). */
void fn_803358D0(BossVarParam *owner, BossCtrlUnit *unit);
/* The boss's character pointer's out-of-line operator-> (BossVarParam::m_chara is the px of
 * a smart pointer). */
EnemyCharInfo *fn_801792D0(EnemyCharInfo *const *body);

/* Motion helpers: the current motion id, start a motion, reset, blend time, frames, and the
 * matrix of a node (behaviour inferred; see BAGunShip.cpp for fn_80416F28). */
u16 fn_80415CDC(_stMtnPlayWork *work, int arg1);
void fn_80417568(_stMtnPlayWork *work, u16 motion);
void fn_804165D8(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
void fn_80416148(_stMtnPlayWork *work, int arg1, float arg2);
int fn_80416F28(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
void fn_80415D40(_stMtnPlayWork *work, int arg1, UnkMotionFrames *frames);
Mtx34f *fn_80415E70(_stMtnPlayWork *work, int node);

/* The hint manager's show call (see BDHologram.cpp). */
s32 fn_801FAF00(UnkHintManager_801FB180 *manager, s32 id, s32 branch, int arg3, int arg4);
/* The sound manager and its play call. */
void *fn_80053164(void);
void fn_801D560C(void *manager, int soundId, int arg2, int arg3);
/* Controller rumble (see EggLastMech.cpp and ElecBomb.cpp). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int arg1);
boost::weak_ptr<Vibration::RequestHandle> fn_8034DE74(int pattern, int pad, float duration);
/* Shows a 2D layout's animation (behaviour inferred). */
void fn_8035367C(const char *file, const char *layout, const char *animation);
/* A random number in [0, 1). */
float fn_8040EE90(void);
/* Whether the record is set (behaviour inferred). */
bool fn_802039E8(void *record);
/* Ends the stage (behaviour inferred). */
UnkObject_80074470 *fn_80074470(void);
void fn_80175EEC(UnkObject_80074470 *object, int arg1, int arg2);

/* Vector helpers (see Bomb.cpp and MathTypes.h). */
Vec3f fn_800091A4(const Vec3f *vector, float scale);
Vec3f fn_8040CEE8(const Vec3f *vector);
/* vector * scale (declared with the scale first: the caller evaluates it before the vector). */
Vec3f fn_8040CDA8(float scale, const Vec3f *vector);
float fn_8040CE74(Vec3f *vector);
void fn_8040CE38(Vec3f *vector, float scale);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
/* The translation of matrix, and a vector copy (behaviour from their bodies). */
void fn_804114D0(Vec3f *translation, const Mtx34f *matrix);
void fn_80016350(Vec3f *vector, const Vec3f *value);
Vec3f fn_80016284(const Vec3f &direction);
float fn_80016260(float x, float z);
}

/* The hint manager (see BDHologram.cpp). */
UnkHintManager_801FB180 *GetHintManager();

/* The game state record (class unknown). */
extern u8 lbl_8057D6A0[];

/* fn_8040CD74 called as returning a - b by value (see EggLastMech.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

inline void ConstructVector(Vec3f *vector, const Vec3f &value) {
    fn_800091EC(vector, &value);
}

inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* Whether motion number index of Devil Doom's set is playing (name inferred). */
inline bool IsMotion(DevilDoomVarParam *owner, int index) {
    u16 motion = owner->m_motionBase + index;
    u16 current = fn_80415CDC(owner->m_motion, 0);
    return current == motion;
}

/* WAIT: waits and picks the next attack. */
class DevilDoomModeCtrlUnit_WAIT : public DevilDoomCtrlUnit {
  public:
    DevilDoomModeCtrlUnit_WAIT(DevilDoomVarParam *owner) : BossCtrlUnit(owner), DevilDoomCtrlUnit(owner) {
        if (!IsMotion(m_owner, 4)) {
            fn_804165D8(m_owner->m_motion, 0, -1, -1);
            fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 4);
            fn_80416148(m_owner->m_motion, 0, 30.0f);
            if (m_owner->m_inverted) {
                fn_80416F28(m_owner->m_motion, 0, 7, 0);
            }
        }
    }
    virtual ~DevilDoomModeCtrlUnit_WAIT();
    virtual int Tick(float dt);
};

class DevilDoomModeCtrlUnit_EDIT : public DevilDoomCtrlUnit {
  public:
    DevilDoomModeCtrlUnit_EDIT(DevilDoomVarParam *owner) : BossCtrlUnit(owner), DevilDoomCtrlUnit(owner) {
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 4);
        m_owner->m_fixed = 1;
    }
    virtual ~DevilDoomModeCtrlUnit_EDIT();
    virtual int Tick(float dt);
};

class DevilDoomModeCtrlUnit_GET_ATK : public DevilDoomCtrlUnit {
  public:
    DevilDoomModeCtrlUnit_GET_ATK(DevilDoomVarParam *owner) : BossCtrlUnit(owner), DevilDoomCtrlUnit(owner) {
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80416148(m_owner->m_motion, 0, 30.0f);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 0xD);
        if (m_owner->m_eyeOpen[1]) {
            fn_80416F28(m_owner->m_motion, 0, 7, 0);
        }
        m_owner->m_stopped = 1;
        m_owner->m_fixed = 1;
        m_owner->m_eyeClosing = 0;
        m_owner->m_neckUp = 0;
        if (!m_owner->field_0x18E) {
            fn_803358D0(m_owner, new BossCtrlUnit_PlayCameraMotion(m_owner, 0x10, 0.0f, 0.0f));
            m_owner->m_active = 0;
            m_owner->field_0x18E = 1;
        }
        if (m_owner->m_eyeOpen[0]) {
            m_side = 0;
        } else {
            m_side = 1;
        }
        m_count = 0;
        m_owner->m_attribute = 0;
        fn_803358D0(m_owner, new DevilDoomCtrlUnit_DoomsEye(m_owner, m_owner->m_motionBase + 0x19));
    }
    virtual ~DevilDoomModeCtrlUnit_GET_ATK();
    virtual int Tick(float dt);

    /* 0x0C */ s32 m_side;
    /* 0x10 */ s32 m_count;
};

class DevilDoomModeCtrlUnit_INVERSE : public DevilDoomCtrlUnit {
  public:
    DevilDoomModeCtrlUnit_INVERSE(DevilDoomVarParam *owner) : BossCtrlUnit(owner), DevilDoomCtrlUnit(owner) {
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 0xB);
        fn_80416148(m_owner->m_motion, 0, 30.0f);
        if (m_owner->m_inverted) {
            fn_80416F28(m_owner->m_motion, 0, 7, 0);
        }
        m_owner->m_fixed = 1;
        m_owner->m_attribute = 0;
    }
    virtual ~DevilDoomModeCtrlUnit_INVERSE();
    virtual int Tick(float dt);
};

class DevilDoomModeCtrlUnit_CHAOS_CONTROL : public DevilDoomCtrlUnit {
  public:
    DevilDoomModeCtrlUnit_CHAOS_CONTROL(DevilDoomVarParam *owner)
        : BossCtrlUnit(owner), DevilDoomCtrlUnit(owner) {
        m_time = 0.0f;
        if (!IsMotion(m_owner, 4)) {
            fn_804165D8(m_owner->m_motion, 0, -1, -1);
            fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 4);
            fn_80416148(m_owner->m_motion, 0, 30.0f);
            if (m_owner->m_inverted) {
                fn_80416F28(m_owner->m_motion, 0, 7, 0);
            }
        }
        m_owner->m_stopped = 1;
        m_owner->m_fixed = 1;
        m_owner->m_hidden = 1;
        m_owner->field_0x19C = 0;
        m_owner->m_wingsActive = 0;
        if (m_owner->field_0x199) {
            m_fromPsycho = 1;
            m_owner->field_0x199 = 0;
        } else {
            m_fromPsycho = 0;
        }
        fn_801FAF00(GetHintManager(), 0x1AD, 0, 0, 0);
        fn_801D560C(fn_80053164(), 0x8134, 0, 0);
        fn_8034DF48(8, -2);
    }
    virtual ~DevilDoomModeCtrlUnit_CHAOS_CONTROL();
    virtual int Tick(float dt);

    /* 0x0C */ float m_time;
    /* 0x10 */ u8 m_fromPsycho;
};

class DevilDoomModeCtrlUnit_ATK_FIRE : public DevilDoomCtrlUnit {
  public:
    DevilDoomModeCtrlUnit_ATK_FIRE(DevilDoomVarParam *owner) : BossCtrlUnit(owner), DevilDoomCtrlUnit(owner) {
        m_time = 0.0f;
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 1);
        fn_80416148(m_owner->m_motion, 0, 30.0f);
        if (m_owner->m_inverted) {
            fn_80416F28(m_owner->m_motion, 0, 7, 0);
        }
        m_owner->m_stopped = 1;
        m_owner->m_fixed = 0;
        m_owner->m_neckUp = 0;
        fn_801FAF00(GetHintManager(), 0x1AC, 0, 0, 0);
    }
    virtual ~DevilDoomModeCtrlUnit_ATK_FIRE();
    virtual int Tick(float dt);

    /* 0x0C */ float m_time;
};

class DevilDoomModeCtrlUnit_ATK_PSYCHO_PREPARE : public DevilDoomCtrlUnit {
  public:
    DevilDoomModeCtrlUnit_ATK_PSYCHO_PREPARE(DevilDoomVarParam *owner)
        : BossCtrlUnit(owner), DevilDoomCtrlUnit(owner) {
        m_time = 0.0f;
        if (m_owner->m_distance < m_owner->m_settings.field_0x58) {
            m_owner->m_nextMode = 5;
            m_owner->field_0x199 = 1;
            return;
        }
        m_owner->field_0x1A0 = 1;
        m_owner->field_0x19C = 1;
        m_owner->field_0x1A4 = m_owner->m_settings.field_0x30;
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 5);
        fn_80416148(m_owner->m_motion, 0, 30.0f);
        if (m_owner->m_inverted) {
            fn_80416F28(m_owner->m_motion, 0, 7, 0);
        }
        m_owner->m_fixed = 0;
        m_owner->m_attribute = 0;
    }
    virtual ~DevilDoomModeCtrlUnit_ATK_PSYCHO_PREPARE();
    virtual int Tick(float dt);

    /* 0x0C */ float m_time;
};

class DevilDoomModeCtrlUnit_ATK_PSYCHO_THROW : public DevilDoomCtrlUnit {
  public:
    DevilDoomModeCtrlUnit_ATK_PSYCHO_THROW(DevilDoomVarParam *owner)
        : BossCtrlUnit(owner), DevilDoomCtrlUnit(owner) {
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        if (m_owner->field_0x193) {
            fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 9);
            m_owner->field_0x193 = 0;
        } else {
            fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 8);
            m_owner->field_0x193 = 1;
        }
        fn_80416148(m_owner->m_motion, 0, 30.0f);
        if (m_owner->m_inverted) {
            fn_80416F28(m_owner->m_motion, 0, 7, 0);
        }
        m_owner->field_0x197 = 1;
        m_owner->m_attribute = 0;
        m_delay = m_owner->m_healthRatio * (m_owner->m_settings.field_0x34 - m_owner->m_settings.field_0x38) + m_owner->m_settings.field_0x38;
    }
    virtual ~DevilDoomModeCtrlUnit_ATK_PSYCHO_THROW();
    virtual int Tick(float dt);

    /* 0x0C */ float m_delay;
};

class DevilDoomModeCtrlUnit_ATK_BIT1 : public DevilDoomCtrlUnit {
  public:
    DevilDoomModeCtrlUnit_ATK_BIT1(DevilDoomVarParam *owner) : BossCtrlUnit(owner), DevilDoomCtrlUnit(owner) {
        m_owner->field_0x1A0 = 2;
        m_owner->field_0x19C = 2;
        m_owner->field_0x628 = 1;
        fn_803358D0(m_owner, new DevilDoomCtrlUnit_BitAttackControl(m_owner));
        m_owner->m_attribute = 0;
    }
    virtual ~DevilDoomModeCtrlUnit_ATK_BIT1();
    virtual int Tick(float dt);
};

class DevilDoomModeCtrlUnit_ATK_BIT2 : public DevilDoomCtrlUnit {
  public:
    DevilDoomModeCtrlUnit_ATK_BIT2(DevilDoomVarParam *owner) : BossCtrlUnit(owner), DevilDoomCtrlUnit(owner) {
        m_owner->field_0x1A0 = 2;
        m_owner->field_0x19C = 2;
        m_owner->field_0x628 = 2;
        fn_803358D0(m_owner, new DevilDoomCtrlUnit_BitAttackControl(m_owner));
        m_owner->m_attribute = 0;
        fn_801FAF00(GetHintManager(), 0x1AB, 0, 0, 0);
    }
    virtual ~DevilDoomModeCtrlUnit_ATK_BIT2();
    virtual int Tick(float dt);
};

class DevilDoomModeCtrlUnit_ATK_BIT3 : public DevilDoomCtrlUnit {
  public:
    DevilDoomModeCtrlUnit_ATK_BIT3(DevilDoomVarParam *owner) : BossCtrlUnit(owner), DevilDoomCtrlUnit(owner) {
        m_owner->field_0x1A0 = 2;
        m_owner->field_0x19C = 2;
        m_owner->field_0x628 = 3;
        fn_803358D0(m_owner, new DevilDoomCtrlUnit_BitAttackControl(m_owner));
        m_owner->m_attribute = 0;
    }
    virtual ~DevilDoomModeCtrlUnit_ATK_BIT3();
    virtual int Tick(float dt);
};

class DevilDoomModeCtrlUnit_START : public DevilDoomCtrlUnit {
  public:
    DevilDoomModeCtrlUnit_START(DevilDoomVarParam *owner) : BossCtrlUnit(owner), DevilDoomCtrlUnit(owner) {
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 0xA);
        if (m_owner->m_inverted) {
            fn_80416F28(m_owner->m_motion, 0, 7, 0);
        }
        m_owner->m_stopped = 1;
        m_owner->m_fixed = 1;
        fn_803358D0(m_owner, new BossCtrlUnit_PlayCameraMotion(m_owner, 0x11, 0.0f, 0.0f));
        m_owner->m_active = 0;
        m_owner->field_0x19C = 0;
        m_owner->m_wingsActive = 0;
        fn_801FAF00(GetHintManager(), 0x1A4, 0, 0, 0);
        fn_801D560C(fn_80053164(), 0x8144, 0, 0);
        fn_8035367C("CsdFiles/BChara_DevDoom", "bchara", "open");
    }
    virtual ~DevilDoomModeCtrlUnit_START();
    virtual int Tick(float dt);
};

class DevilDoomModeCtrlUnit_BELOW_HALF : public DevilDoomCtrlUnit {
  public:
    /* NonMatching (98.9%): retail keeps this in r30 and moveCamera in r31; GC/2.7 swaps them. The
     * same code in DEATH's constructor matches. Typing moveCamera as int gives retail's registers
     * but a word compare instead of retail's byte test. Everything else in the unit links. */
    DevilDoomModeCtrlUnit_BELOW_HALF(DevilDoomVarParam *owner) : BossCtrlUnit(owner), DevilDoomCtrlUnit(owner) {
        bool moveCamera = false;
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80416148(m_owner->m_motion, 0, 30.0f);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 0xD);
        if (m_owner->m_eyeOpen[1]) {
            fn_80416F28(m_owner->m_motion, 0, 7, 0);
            moveCamera = true;
        }
        m_owner->m_stopped = 1;
        m_owner->m_fixed = 1;
        m_owner->m_eyeClosing = 0;
        m_owner->m_neckUp = 0;
        if (moveCamera) {
            Vec3f position;
            ConstructVector(&position, m_owner->m_chara->m_position);
            Vec3f rotation = fn_801792D0(&m_owner->m_chara)->m_rotation;
            rotation.y += lbl_805FAD38;
            fn_803358D0(m_owner,
                        new BossCtrlUnit_PlayCameraMotion(m_owner, 0x10, &position, &rotation, 0.0f, 0.0f));
        } else {
            fn_803358D0(m_owner, new BossCtrlUnit_PlayCameraMotion(m_owner, 0x10, 0.0f, 0.0f));
        }
        m_owner->m_active = 0;
        if (m_owner->m_eyeOpen[0]) {
            m_side = 0;
        } else {
            m_side = 1;
        }
        m_count = 0;
        m_owner->m_attribute = 0;
        fn_803358D0(m_owner, new DevilDoomCtrlUnit_DoomsEye(m_owner, m_owner->m_motionBase + 0x19));
        m_owner->field_0x19C = 0;
        fn_801FAF00(GetHintManager(), 0x1A8, 0, 0, 0);
        fn_801D560C(fn_80053164(), 0x8144, 0, 0);
        fn_8034DF48(0xC, -2);
    }
    virtual ~DevilDoomModeCtrlUnit_BELOW_HALF();
    virtual int Tick(float dt);

    /* 0x0C */ s32 m_side;
    /* 0x10 */ s32 m_count;
};

class DevilDoomModeCtrlUnit_DEATH : public DevilDoomCtrlUnit {
  public:
    DevilDoomModeCtrlUnit_DEATH(DevilDoomVarParam *owner) : BossCtrlUnit(owner), DevilDoomCtrlUnit(owner) {
        bool moveCamera = false;
        m_ended = 0;
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 0xE);
        if (m_owner->m_eyeOpen[1]) {
            fn_80416F28(m_owner->m_motion, 0, 7, 0);
            moveCamera = true;
        }
        m_owner->m_stopped = 1;
        m_owner->m_fixed = 1;
        if (moveCamera) {
            Vec3f position;
            ConstructVector(&position, m_owner->m_chara->m_position);
            Vec3f rotation = fn_801792D0(&m_owner->m_chara)->m_rotation;
            rotation.y += lbl_805FAD38;
            fn_803358D0(m_owner,
                        new BossCtrlUnit_PlayCameraMotion(m_owner, 0xF, &position, &rotation, 0.0f, 0.0f));
        } else {
            fn_803358D0(m_owner, new BossCtrlUnit_PlayCameraMotion(m_owner, 0xF, 0.0f, 0.0f));
        }
        m_owner->m_active = 0;
        m_owner->field_0x19C = 0;
        m_owner->m_wingsActive = 0;
        fn_801FAF00(GetHintManager(), 0x1A9, 0, 0, 0);
        fn_801D560C(fn_80053164(), 0x8145, 0, 0);
        fn_8034DF48(7, -2);
    }
    virtual ~DevilDoomModeCtrlUnit_DEATH();
    virtual int Tick(float dt);

    /* 0x0C */ u8 m_ended;
};

SIZE_ASSERT(DevilDoomModeCtrlUnit, 0x28);
SIZE_ASSERT(DevilDoomModeCtrlUnit_WAIT, 0x1C);
SIZE_ASSERT(DevilDoomModeCtrlUnit_GET_ATK, 0x24);
SIZE_ASSERT(DevilDoomModeCtrlUnit_CHAOS_CONTROL, 0x24);
SIZE_ASSERT(DevilDoomModeCtrlUnit_ATK_FIRE, 0x20);
SIZE_ASSERT(DevilDoomModeCtrlUnit_DEATH, 0x20);

DevilDoomModeCtrlUnit_DEATH::~DevilDoomModeCtrlUnit_DEATH() {}

int DevilDoomModeCtrlUnit_DEATH::Tick(float dt) {
    if (IsMotion(m_owner, 0xE)) {
        if (fn_80416F28(m_owner->m_motion, 0, 3, 1) != 0) {
            fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 0xF);
            if (m_owner->m_eyeOpen[1]) {
                fn_80416F28(m_owner->m_motion, 0, 7, 0);
            }
        }
    } else if (IsMotion(m_owner, 0xF)) {
        if (fn_80416F28(m_owner->m_motion, 0, 3, 1) != 0) {
            fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 0x10);
            if (m_owner->m_eyeOpen[1]) {
                fn_80416F28(m_owner->m_motion, 0, 7, 0);
            }
        }
    } else if (IsMotion(m_owner, 0x10) && !m_ended) {
        UnkMotionFrames frames;
        fn_80415D40(m_owner->m_motion, 0, &frames);
        if (frames.m_frame > frames.m_end - 60.0f) {
            fn_80175EEC(fn_80074470(), 6, 0);
            m_ended = 1;
        }
    }
    return 0;
}

DevilDoomModeCtrlUnit_BELOW_HALF::~DevilDoomModeCtrlUnit_BELOW_HALF() {
    m_owner->m_stopped = 0;
    m_owner->m_eyeClosing = 1;
    m_owner->m_neckUp = 1;
    m_owner->m_active = 1;
    if (m_owner->m_eyeOpen[0] && !m_owner->m_inverted) {
        m_owner->m_inverted = 1;
    } else if (!m_owner->m_eyeOpen[0] && m_owner->m_inverted) {
        m_owner->m_inverted = 0;
    }
}

int DevilDoomModeCtrlUnit_BELOW_HALF::Tick(float dt) {
    if (m_owner->m_attribute) {
        switch (m_count) {
        case 0:
            m_owner->m_wingsActive = 0;
            m_owner->m_eyeOpen[m_side] = 0;
            break;
        case 1:
            m_owner->m_wingsActive = 1;
            m_owner->m_eyeOpen[1 - m_side] = 1;
            m_owner->m_flipped = !m_owner->m_flipped;
            break;
        }
        m_count++;
        m_owner->m_attribute = 0;
    }
    if (fn_80416F28(m_owner->m_motion, 0, 3, 1) != 0) {
        m_owner->m_nextMode = 2;
    }
    return 0;
}

DevilDoomModeCtrlUnit_START::~DevilDoomModeCtrlUnit_START() {
    m_owner->m_stopped = 0;
    m_owner->m_active = 1;
    m_owner->m_wingsActive = 1;
    for (int i = 0; i < 3; i++) {
        fn_803358D0(m_owner, new DevilDoomCtrlUnit_BitGroupControl(m_owner, i));
    }
}

int DevilDoomModeCtrlUnit_START::Tick(float dt) {
    if (fn_80416F28(m_owner->m_motion, 0, 3, 1) != 0) {
        m_owner->m_nextMode = 2;
    }
    return 0;
}

DevilDoomModeCtrlUnit_ATK_BIT3::~DevilDoomModeCtrlUnit_ATK_BIT3() {}

int DevilDoomModeCtrlUnit_ATK_BIT3::Tick(float dt) {
    m_owner->m_nextMode = 2;
    return 0;
}

DevilDoomModeCtrlUnit_ATK_BIT2::~DevilDoomModeCtrlUnit_ATK_BIT2() {}

int DevilDoomModeCtrlUnit_ATK_BIT2::Tick(float dt) {
    m_owner->m_nextMode = 2;
    return 0;
}

DevilDoomModeCtrlUnit_ATK_BIT1::~DevilDoomModeCtrlUnit_ATK_BIT1() {}

int DevilDoomModeCtrlUnit_ATK_BIT1::Tick(float dt) {
    m_owner->m_nextMode = 2;
    return 0;
}

DevilDoomModeCtrlUnit_ATK_PSYCHO_THROW::~DevilDoomModeCtrlUnit_ATK_PSYCHO_THROW() {
    if (m_delay > 0.0f) {
        m_owner->field_0x1A8 = m_delay;
    }
}

int DevilDoomModeCtrlUnit_ATK_PSYCHO_THROW::Tick(float dt) {
    if (m_owner->m_attribute) {
        m_owner->field_0x198 = 1;
        m_owner->m_attribute = 0;
    }
    if (m_owner->field_0x1A4 > 0.0f && m_owner->field_0x196 && m_delay < 0.0f) {
        UnkMotionFrames frames;
        fn_80415D40(m_owner->m_motion, 0, &frames);
        if (frames.m_frame > 60.0f * m_delay + frames.m_end) {
            m_owner->m_nextMode = 8;
            return 0;
        }
    }
    if (fn_80416F28(m_owner->m_motion, 0, 3, 1) != 0) {
        m_owner->m_nextMode = 2;
    }
    return 0;
}

DevilDoomModeCtrlUnit_ATK_PSYCHO_PREPARE::~DevilDoomModeCtrlUnit_ATK_PSYCHO_PREPARE() {}

int DevilDoomModeCtrlUnit_ATK_PSYCHO_PREPARE::Tick(float dt) {
    if (IsMotion(m_owner, 5)) {
        if (m_owner->m_attribute) {
            fn_803358D0(m_owner, new DevilDoomCtrlUnit_PsychoObjectControl(m_owner));
            m_owner->m_attribute = 0;
            fn_801FAF00(GetHintManager(), 0x1AA, 0, 0, 0);
            fn_8034DE74(8, -2, 3.0f);
        }
        if (fn_80416F28(m_owner->m_motion, 0, 3, 1) != 0) {
            fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 6);
            if (m_owner->m_inverted) {
                fn_80416F28(m_owner->m_motion, 0, 7, 0);
            }
            m_time = 0.0f;
        }
    } else if (IsMotion(m_owner, 6)) {
        m_time += dt;
        if (m_time > m_owner->m_settings.field_0x2C) {
            m_owner->m_nextMode = 8;
        }
    } else if (IsMotion(m_owner, 7)) {
        if (fn_80416F28(m_owner->m_motion, 0, 3, 1) != 0) {
            m_owner->m_nextMode = 8;
        }
    }
    return 0;
}

DevilDoomModeCtrlUnit_ATK_FIRE::~DevilDoomModeCtrlUnit_ATK_FIRE() {
    m_owner->m_stopped = 0;
    m_owner->m_neckUp = 1;
    m_owner->field_0x630 = m_owner->m_settings.field_0xF0;
}

int DevilDoomModeCtrlUnit_ATK_FIRE::Tick(float dt) {
    if (IsMotion(m_owner, 1)) {
        if (fn_80416F28(m_owner->m_motion, 0, 3, 1) != 0) {
            fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 2);
            if (m_owner->m_inverted) {
                fn_80416F28(m_owner->m_motion, 0, 7, 0);
            }
            m_time = 0.0f;
            m_owner->m_fixed = 1;
            Mtx34f *mouth;
            if (m_owner->m_inverted) {
                mouth = fn_80415E70(m_owner->m_motion, 0x3FA);
            } else {
                mouth = fn_80415E70(m_owner->m_motion, 0x404);
            }
            Vec3f translation;
            fn_804114D0(&translation, mouth);
            Vec3f position;
            fn_80016350(&position, &translation);
            position.y = 0.0f;
            Vec3f velocity;
            ConstructVector(&velocity, ((VectorSubtractFn)fn_8040CD74)(&position, &m_owner->m_chara->m_position));
            velocity.y = 0.0f;
            fn_8040CE74(&velocity);
            fn_8040CFF8(&position, &(const Vec3f &)fn_800091A4(&velocity, 30.0f));
            fn_8040CE38(&velocity, m_owner->m_settings.field_0xEC);
            fn_803358D0(m_owner, new DevilDoomCtrlUnit_FireBreath(m_owner, &position, &velocity));
        }
    } else if (IsMotion(m_owner, 2)) {
        m_time += dt;
        if (m_time > 800.0f / m_owner->m_settings.field_0xEC) {
            fn_804165D8(m_owner->m_motion, 0, -1, -1);
            fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 3);
            fn_80416148(m_owner->m_motion, 0, 30.0f);
            if (m_owner->m_inverted) {
                fn_80416F28(m_owner->m_motion, 0, 7, 0);
            }
        }
    } else if (IsMotion(m_owner, 3)) {
        if (fn_80416F28(m_owner->m_motion, 0, 3, 1) != 0) {
            m_owner->m_nextMode = 2;
        }
    }
    return 0;
}

DevilDoomModeCtrlUnit_CHAOS_CONTROL::~DevilDoomModeCtrlUnit_CHAOS_CONTROL() {
    m_owner->m_stopped = 0;
    m_owner->m_hidden = 0;
    m_owner->m_wingsActive = 1;
    fn_801FAF00(GetHintManager(), 0x1AE, 0, 0, 0);
}

int DevilDoomModeCtrlUnit_CHAOS_CONTROL::Tick(float dt) {
    float before = m_time;
    m_time += dt;
    if (m_time > m_owner->m_settings.field_0x20) {
        Vec3f offset;
        ConstructVector(&offset, m_owner->m_target);
        offset.y = 0.0f;
        if (fn_8040CF50(&offset) > 0.0001) {
            float speed = m_owner->m_settings.field_0x4;
            AssignVector(offset, fn_8040CDA8(speed, &(const Vec3f &)fn_8040CEE8(&offset)));
            if (m_owner->m_distance < m_owner->m_settings.field_0x4) {
                AssignVector(offset, fn_80016284(offset));
            }
            AssignVector(m_owner->m_chara->m_position, offset);
        }
        Vec3f direction;
        ConstructVector(&direction,
                        ((VectorSubtractFn)fn_8040CD74)(&m_owner->m_target, &m_owner->m_chara->m_position));
        m_owner->m_targetAngle = fn_80016260(-direction.x, -direction.z);
        m_owner->m_angle = m_owner->m_targetAngle;
        if (m_owner->m_inverted) {
            m_owner->m_chara->m_rotation.y = m_owner->m_angle - 0.5f * lbl_805FAD38;
        } else {
            m_owner->m_chara->m_rotation.y = m_owner->m_angle + 0.5f * lbl_805FAD38;
        }
        m_owner->m_angleDiff = 0.0f;
        direction.y = 0.0f;
        m_owner->m_distance = fn_8040CF50(&direction);
        if (m_fromPsycho) {
            m_owner->m_nextMode = 7;
        } else {
            m_owner->m_nextMode = 2;
        }
        if (before <= m_owner->m_settings.field_0x20) {
            fn_801D560C(fn_80053164(), 0x8135, 0, 0);
        }
    }
    return 0;
}

DevilDoomModeCtrlUnit_INVERSE::~DevilDoomModeCtrlUnit_INVERSE() {}

int DevilDoomModeCtrlUnit_INVERSE::Tick(float dt) {
    if (m_owner->m_attribute) {
        m_owner->m_inverted = !m_owner->m_inverted;
        m_owner->m_angle += lbl_805FAD38;
        while (std::fabs(m_owner->m_angle) > lbl_805FAD38) {
            m_owner->m_angle += m_owner->m_angle > 0.0f ? -2.0f * lbl_805FAD38 : 2.0f * lbl_805FAD38;
        }
        m_owner->m_angleDiff = m_owner->m_targetAngle - m_owner->m_angle;
        while (std::fabs(m_owner->m_angleDiff) > lbl_805FAD38) {
            m_owner->m_angleDiff += m_owner->m_angleDiff > 0.0f ? -2.0f * lbl_805FAD38 : 2.0f * lbl_805FAD38;
        }
        m_owner->m_attribute = 0;
    }
    if (fn_80416F28(m_owner->m_motion, 0, 3, 1) != 0) {
        m_owner->m_nextMode = 2;
    }
    return 0;
}

DevilDoomModeCtrlUnit_GET_ATK::~DevilDoomModeCtrlUnit_GET_ATK() {
    m_owner->m_stopped = 0;
    m_owner->m_eyeClosing = 1;
    m_owner->m_neckUp = 1;
    if (!m_owner->m_active) {
        m_owner->m_active = 1;
    }
    if (m_owner->m_eyeOpen[m_side]) {
        m_owner->m_eyeOpen[m_side] = 0;
    }
    if (!m_owner->m_eyeOpen[1 - m_side]) {
        m_owner->m_eyeOpen[1 - m_side] = 1;
        m_owner->m_flipped = !m_owner->m_flipped;
        m_owner->m_wingsActive = 1;
    }
    if (m_owner->m_eyeOpen[0] && !m_owner->m_inverted) {
        m_owner->m_inverted = 1;
    } else if (!m_owner->m_eyeOpen[0] && m_owner->m_inverted) {
        m_owner->m_inverted = 0;
    }
}

int DevilDoomModeCtrlUnit_GET_ATK::Tick(float dt) {
    if (m_owner->m_attribute) {
        switch (m_count) {
        case 0:
            m_owner->m_wingsActive = 0;
            m_owner->m_eyeOpen[m_side] = 0;
            break;
        case 1:
            m_owner->m_wingsActive = 1;
            m_owner->m_eyeOpen[1 - m_side] = 1;
            m_owner->m_flipped = !m_owner->m_flipped;
            break;
        }
        m_count++;
        m_owner->m_attribute = 0;
    }
    if (fn_80416F28(m_owner->m_motion, 0, 3, 1) != 0) {
        m_owner->m_nextMode = 2;
    }
    return 0;
}

DevilDoomModeCtrlUnit_EDIT::~DevilDoomModeCtrlUnit_EDIT() {}

int DevilDoomModeCtrlUnit_EDIT::Tick(float dt) {
    return 0;
}

DevilDoomModeCtrlUnit_WAIT::~DevilDoomModeCtrlUnit_WAIT() {}

int DevilDoomModeCtrlUnit_WAIT::Tick(float dt) {
    float limit = m_owner->m_settings.field_0x10 * (lbl_805FAD38 / 180.0f);
    if (std::fabs(m_owner->m_angleDiff) > limit) {
        m_owner->m_nextMode = 4;
        return 0;
    }
    switch (m_owner->field_0x19C) {
    case 1:
        m_owner->m_fixed = 0;
        if (!m_owner->field_0x196) {
            m_owner->field_0x19C = 0;
        }
        if (m_owner->field_0x1A4 < 0.0f) {
            m_owner->field_0x19C = 0;
        }
        if (m_owner->field_0x1A8 < 0.0f) {
            m_owner->m_nextMode = 8;
        }
        break;
    case 2:
        m_owner->m_fixed = 1;
        if (m_owner->field_0x630 < 0.0f &&
            std::fabs(m_owner->m_angleDiff) < m_owner->m_settings.m_wingAngle * (lbl_805FAD38 / 180.0f) &&
            m_owner->m_distance < m_owner->m_settings.field_0xF4) {
            m_owner->m_nextMode = 6;
            return 0;
        }
        break;
    default:
        if (m_owner->field_0x630 < 0.0f &&
            std::fabs(m_owner->m_angleDiff) < m_owner->m_settings.m_wingAngle * (lbl_805FAD38 / 180.0f) &&
            m_owner->m_distance < m_owner->m_settings.field_0xF4) {
            m_owner->m_nextMode = 6;
            return 0;
        }
        m_owner->field_0x1A4 = 0.0f;
        float health = m_owner->m_health / m_owner->m_maxHealth;
        switch (m_owner->field_0x1A0) {
        case 1:
            if (health > 0.75f) {
                m_owner->m_nextMode = 9;
            } else if (health > 0.5f) {
                m_owner->m_nextMode = 10;
            } else if (health > 0.25f) {
                switch ((int)(2.0f * fn_8040EE90())) {
                case 0:
                    m_owner->m_nextMode = 10;
                    break;
                case 1:
                    m_owner->m_nextMode = 11;
                    break;
                }
            } else {
                switch ((int)(3.0f * fn_8040EE90())) {
                case 0:
                    m_owner->m_nextMode = 9;
                    break;
                case 1:
                    m_owner->m_nextMode = 10;
                    break;
                case 2:
                    m_owner->m_nextMode = 11;
                    break;
                }
            }
            break;
        case 2:
            if (!fn_802039E8(lbl_8057D6A0 + 0xA8)) {
                m_owner->m_nextMode = 7;
            }
            break;
        default:
            if (fn_8040EE90() < 0.5f) {
                m_owner->field_0x1A0 = 1;
            } else {
                m_owner->field_0x1A0 = 2;
            }
            break;
        }
        break;
    }
    return 0;
}

BossCtrlUnit *DevilDoomModeCtrlUnit::CreateMode(int mode) {
    switch (mode) {
    case 0:
    default:
        return new DevilDoomModeCtrlUnit_WAIT(m_owner);
    case 1:
        return new DevilDoomModeCtrlUnit_EDIT(m_owner);
    case 3:
        return new DevilDoomModeCtrlUnit_GET_ATK(m_owner);
    case 4:
        return new DevilDoomModeCtrlUnit_INVERSE(m_owner);
    case 5:
        return new DevilDoomModeCtrlUnit_CHAOS_CONTROL(m_owner);
    case 6:
        return new DevilDoomModeCtrlUnit_ATK_FIRE(m_owner);
    case 7:
        return new DevilDoomModeCtrlUnit_ATK_PSYCHO_PREPARE(m_owner);
    case 8:
        return new DevilDoomModeCtrlUnit_ATK_PSYCHO_THROW(m_owner);
    case 9:
        return new DevilDoomModeCtrlUnit_ATK_BIT1(m_owner);
    case 10:
        return new DevilDoomModeCtrlUnit_ATK_BIT2(m_owner);
    case 11:
        return new DevilDoomModeCtrlUnit_ATK_BIT3(m_owner);
    case 12:
        return new DevilDoomModeCtrlUnit_START(m_owner);
    case 13:
        return new DevilDoomModeCtrlUnit_BELOW_HALF(m_owner);
    case 14:
        return new DevilDoomModeCtrlUnit_DEATH(m_owner);
    }
}

DevilDoomModeCtrlUnit::~DevilDoomModeCtrlUnit() {}

int DevilDoomModeCtrlUnit::Tick(float dt) {
    return BossModeCtrlUnit::Tick(dt);
}
