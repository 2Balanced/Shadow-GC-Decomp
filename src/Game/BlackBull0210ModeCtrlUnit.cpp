/* BlackBull0210ModeCtrlUnit (class name from RTTI): the mode control unit of the Black Bull
 * boss's 0210 stage. CreateMode builds one BlackBull0210ModeCtrlUnit_<MODE> unit (class names from
 * RTTI) for each of the stage's modes; each one sets up its motion in its constructor, runs in Tick
 * (which asks the boss for the next mode by setting BossVarParam::m_nextMode) and cleans up in its
 * destructor. Between attacks the boss walks round eight points of the arena (s_points), chasing
 * the player or fleeing. The mode units derive from BlackBull0210CtrlUnit, whose BossCtrlUnit base
 * is virtual, so CodeWarrior emits this-adjusting thunks for their vtables at the end of the unit.
 * File name inferred from the class (as Game/DevilDoomModeCtrlUnit.cpp). Field and helper names
 * are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first); the mode units' constructors are inline and are emitted after CreateMode,
 * their first user. */
#include "Effect/System/EffectElement.h"
#include "Game/BlackBull0210CtrlUnit.h"
#include "Game/BlackBullCtrlUnit.h"
#include "Game/BlackBullCtrlUnits.h"
#include "Game/BossCtrlUnit.h"
#include "Game/EnemyCharInfo.h"
#include "Game/MathConstants.h"
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

/* MSL's std::fabs for floats, which the game calls out of line (as in DevilDoomCtrlUnit.cpp). */
namespace std {
float fabs(float x);
} // namespace std

/* What fn_80415D40 fills in about a motion (as in DevilDoomModeCtrlUnit.cpp). */
struct UnkMotionFrames {
    /* 0x00 */ float field_0x0;
    /* 0x04 */ float m_end;
    /* 0x08 */ float field_0x8;
    /* 0x0C */ float m_frame;
};

/* The effect manager fn_800A31F0 returns (as in DevilDoomCtrlUnit.cpp); its third virtual
 * function launches effect id from a launch description. */
class UnkEffectManager_800A31F0 {
  public:
    virtual void vf08();
    virtual boost::weak_ptr<EffectElement> Launch(s32 id, s32 arg);
    virtual boost::weak_ptr<EffectElement> Launch(boost::shared_ptr<EffectLaunchInfo> info, s32 id);
};

extern "C" {
/* Adds a control unit to a boss (behaviour inferred). */
void fn_803358D0(BossVarParam *owner, BossCtrlUnit *unit);

/* Motion helpers (see DevilDoomModeCtrlUnit.cpp and DevilDoomCtrlUnit.cpp; behaviour
 * inferred). */
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
/* Blends in motion over time (behaviour inferred). */
void fn_80415F98(_stMtnPlayWork *work, int arg1, float arg2, int motion, float time);

/* The hint manager's calls (see BDHologram.cpp and EnemyHint.cpp). */
s32 fn_801FAF00(UnkHintManager_801FB180 *manager, s32 id, s32 branch, int arg3, int arg4);
void fn_801FAAA4(UnkHintManager_801FB180 *manager);
void fn_801FAAB4(UnkHintManager_801FB180 *manager);
/* Shows a 2D layout's animation (behaviour inferred). */
void fn_8035367C(const char *file, const char *layout, const char *animation);
/* Controller rumble (see EggLastMech.cpp). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int arg1);
/* Starts and stops the boss's footstep sound for a point of the arena (behaviour inferred). */
void fn_801ED410(int sound);
void fn_801ED5A8(int sound);
/* The effect manager, and the release of an effect (see BKTank.cpp). */
UnkEffectManager_800A31F0 *fn_800A31F0(void);
void fn_800A4210(boost::shared_ptr<EffectElement> *effect);
/* Sets an effect's position (see EffectPJS.cpp). */
void fn_80425E4C(EffectElement *effect, const Vec3f *position);
/* Ends the stage (see DevilDoomModeCtrlUnit.cpp). */
UnkObject_80074470 *fn_80074470(void);
void fn_80175EEC(UnkObject_80074470 *object, int arg1, int arg2);

/* Vector helpers (see DevilDoomModeCtrlUnit.cpp and HeavyDog0410CtrlUnit.cpp). */
float fn_8040CE74(Vec3f *vector);
void fn_8040CE38(Vec3f *vector, float scale);
void fn_8040CE04(Vec3f *vector, const Vec3f *value); /* -= */
void fn_804114D0(Vec3f *translation, const Mtx34f *matrix);
void fn_80016350(Vec3f *vector, const Vec3f *value);
Vec3f fn_80016284(const Vec3f &direction);
}

/* The hint manager (see BDHologram.cpp). */
UnkHintManager_801FB180 *GetHintManager();

/* The game state record (class unknown): the frame time at 0x18 (meaning inferred). */
extern u8 lbl_8057D6A0[];

inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* The eight points the boss walks between, clockwise from the north (names inferred). The
 * original copy-constructs them from temporaries through the vector type's out-of-line copy
 * constructor, fn_800091EC; this wrapper's constructor writes those copies out so that the startup
 * code stays the same (as in Game/Cage.cpp). */
static struct UnkArenaPoints {
    UnkArenaPoints() {
        float point7[3];
        float point6[3];
        float point5[3];
        float point4[3];
        float point3[3];
        float point2[3];
        float point1[3];
        float point0[3];

        point0[0] = 0.0f;
        point0[1] = 0.0f;
        point0[2] = 680.0f;
        fn_800091EC(reinterpret_cast<Vec3f *>(m_points[0]), reinterpret_cast<Vec3f *>(point0));
        point1[0] = 680.0f;
        point1[1] = 0.0f;
        point1[2] = 680.0f;
        fn_800091EC(reinterpret_cast<Vec3f *>(m_points[1]), reinterpret_cast<Vec3f *>(point1));
        point2[0] = 680.0f;
        point2[1] = 0.0f;
        point2[2] = 0.0f;
        fn_800091EC(reinterpret_cast<Vec3f *>(m_points[2]), reinterpret_cast<Vec3f *>(point2));
        point3[0] = 680.0f;
        point3[1] = 0.0f;
        point3[2] = -680.0f;
        fn_800091EC(reinterpret_cast<Vec3f *>(m_points[3]), reinterpret_cast<Vec3f *>(point3));
        point4[0] = 0.0f;
        point4[1] = 0.0f;
        point4[2] = -680.0f;
        fn_800091EC(reinterpret_cast<Vec3f *>(m_points[4]), reinterpret_cast<Vec3f *>(point4));
        point5[0] = -680.0f;
        point5[1] = 0.0f;
        point5[2] = -680.0f;
        fn_800091EC(reinterpret_cast<Vec3f *>(m_points[5]), reinterpret_cast<Vec3f *>(point5));
        point6[0] = -680.0f;
        point6[1] = 0.0f;
        point6[2] = 0.0f;
        fn_800091EC(reinterpret_cast<Vec3f *>(m_points[6]), reinterpret_cast<Vec3f *>(point6));
        point7[0] = -680.0f;
        point7[1] = 0.0f;
        point7[2] = 680.0f;
        fn_800091EC(reinterpret_cast<Vec3f *>(m_points[7]), reinterpret_cast<Vec3f *>(point7));
    }

    /* 0x00 */ float m_points[8][3];
} s_points;

/* Whether motion number index of the boss's set is playing (name inferred). */
inline bool IsMotion(BlackBull0210VarParam *owner, int index) {
    u16 motion = owner->m_motionBase + index;
    u16 current = fn_80415CDC(owner->m_motion, 0);
    return motion == current;
}

/* Radians to degrees (name inferred; as in BlackBull0210CtrlUnit.cpp). */
inline float ToDegree(float radians) {
    return radians * (180.0f / lbl_805FAD38);
}

/* The boss's health as a part of its full health. */
inline float HealthRatio(BlackBull0210VarParam *owner) {
    return owner->m_health / owner->m_maxHealth;
}

/* WAIT: waits at a point, turned to the player, then breathes fire. */
class BlackBull0210ModeCtrlUnit_WAIT : public BlackBull0210CtrlUnit {
  public:
    BlackBull0210ModeCtrlUnit_WAIT(BlackBull0210VarParam *owner)
        : BossCtrlUnit(owner), BlackBull0210CtrlUnit(owner) {
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
        int point;
        if (HealthRatio(m_owner) <= 0.5f) {
            point = (int)((m_owner->m_angle + lbl_805FAD38) / (lbl_805FAD38 / 2.0f)) * 2 + 1;
        } else {
            point = (int)((1.125f * lbl_805FAD38 + m_owner->m_angle) / (lbl_805FAD38 / 4.0f));
        }
        if (point < 0) {
            point += 8;
        } else if (point >= 8) {
            point -= 8;
        }
        m_owner->m_point = point;
        fn_800091D0(&m_owner->m_destination, reinterpret_cast<Vec3f *>(s_points.m_points[point]));
        m_owner->m_destination.y = m_owner->m_position.y;
    }
    virtual ~BlackBull0210ModeCtrlUnit_WAIT();
    virtual int Tick(float dt);

    /* 0x10 */ float m_time;
};

class BlackBull0210ModeCtrlUnit_EDIT : public BlackBull0210CtrlUnit {
  public:
    BlackBull0210ModeCtrlUnit_EDIT(BlackBull0210VarParam *owner)
        : BossCtrlUnit(owner), BlackBull0210CtrlUnit(owner) {
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 5);
        m_owner->m_motionLocked = 1;
        m_owner->m_motionLocked2 = 1;
    }
    virtual ~BlackBull0210ModeCtrlUnit_EDIT();
    virtual int Tick(float dt);
};

/* CHASE: walks to the next point towards the player. */
class BlackBull0210ModeCtrlUnit_CHASE : public BlackBull0210CtrlUnit {
  public:
    BlackBull0210ModeCtrlUnit_CHASE(BlackBull0210VarParam *owner)
        : BossCtrlUnit(owner), BlackBull0210CtrlUnit(owner) {
        m_owner->m_motionLocked = 0;
        m_owner->m_motionLocked2 = 0;
        m_owner->m_moving = 1;
        float diff = m_owner->m_targetAngle - m_owner->m_angle;
        while (std::fabs(diff) > lbl_805FAD38) {
            diff += diff > 0.0f ? -2.0f * lbl_805FAD38 : 2.0f * lbl_805FAD38;
        }
        int point;
        if (m_owner->m_point != -1) {
            point = m_owner->m_point;
        } else {
            point = (int)((1.125f * lbl_805FAD38 + m_owner->m_angle) / (lbl_805FAD38 / 4.0f));
            if (point < 0) {
                point += 8;
            } else if (point >= 8) {
                point -= 8;
            }
        }
        if (HealthRatio(m_owner) <= 0.5f) {
            int near = (int)((m_owner->m_angle + lbl_805FAD38) / (lbl_805FAD38 / 2.0f)) * 2 + 1;
            if (near < 0) {
                near += 8;
            } else if (near >= 8) {
                near -= 8;
            }
            if (point != near) {
                point += diff > 0.0f ? 1 : -1;
            } else {
                point += diff > 0.0f ? 2 : -2;
            }
        } else {
            point += diff > 0.0f ? 1 : -1;
        }
        if (point < 0) {
            point += 8;
        } else if (point >= 8) {
            point -= 8;
        }
        m_owner->m_point = point;
        fn_800091D0(&m_owner->m_destination, reinterpret_cast<Vec3f *>(s_points.m_points[point]));
        m_owner->m_destination.y = m_owner->m_position.y;
        if (m_owner->m_sound != -1) {
            fn_801ED410(m_owner->m_sound);
            m_owner->m_sound = -1;
        }
    }
    virtual ~BlackBull0210ModeCtrlUnit_CHASE();
    virtual int Tick(float dt);
};

/* ESCAPE: walks to the next point away from the player. */
class BlackBull0210ModeCtrlUnit_ESCAPE : public BlackBull0210CtrlUnit {
  public:
    BlackBull0210ModeCtrlUnit_ESCAPE(BlackBull0210VarParam *owner)
        : BossCtrlUnit(owner), BlackBull0210CtrlUnit(owner) {
        m_owner->m_motionLocked = 0;
        m_owner->m_motionLocked2 = 0;
        m_owner->m_moving = 1;
        m_owner->field_0x171 = 0;
        float diff = m_owner->m_targetAngle - m_owner->m_angle;
        while (std::fabs(diff) > lbl_805FAD38) {
            diff += diff > 0.0f ? -2.0f * lbl_805FAD38 : 2.0f * lbl_805FAD38;
        }
        int point;
        if (m_owner->m_point != -1) {
            point = m_owner->m_point;
        } else {
            point = (int)((1.125f * lbl_805FAD38 + m_owner->m_angle) / (lbl_805FAD38 / 4.0f));
            if (point < 0) {
                point += 8;
            } else if (point >= 8) {
                point -= 8;
            }
        }
        if (HealthRatio(m_owner) <= 0.5f) {
            int near = (int)((m_owner->m_angle + lbl_805FAD38) / (lbl_805FAD38 / 2.0f)) * 2 + 1;
            if (near < 0) {
                near += 8;
            } else if (near >= 8) {
                near -= 8;
            }
            if (point != near) {
                point += diff > 0.0f ? -1 : 1;
            } else {
                point += diff > 0.0f ? -2 : 2;
            }
        } else {
            point += diff > 0.0f ? -1 : 1;
        }
        if (point < 0) {
            point += 8;
        } else if (point >= 8) {
            point -= 8;
        }
        m_owner->m_point = point;
        fn_800091D0(&m_owner->m_destination, reinterpret_cast<Vec3f *>(s_points.m_points[point]));
        m_owner->m_destination.y = m_owner->m_position.y;
        if (m_owner->m_sound != -1) {
            fn_801ED410(m_owner->m_sound);
            m_owner->m_sound = -1;
        }
    }
    virtual ~BlackBull0210ModeCtrlUnit_ESCAPE();
    virtual int Tick(float dt);
};

/* DIZZY: the boss reels after a hard hit. */
class BlackBull0210ModeCtrlUnit_DIZZY : public BlackBull0210CtrlUnit {
  public:
    BlackBull0210ModeCtrlUnit_DIZZY(BlackBull0210VarParam *owner)
        : BossCtrlUnit(owner), BlackBull0210CtrlUnit(owner) {
        m_owner->field_0x171 = 0;
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 0xD);
        fn_80416148(m_owner->m_motion, 0, 10.0f);
        if (!m_owner->m_dizzy) {
            fn_803358D0(m_owner, new BlackBullCtrlUnit_Dizzy(m_owner));
            m_owner->m_dizzy = 1;
            m_owner->m_dizzyEnded = 0;
        }
        m_owner->m_motionLocked = 1;
        m_owner->m_motionLocked2 = 1;
    }
    virtual ~BlackBull0210ModeCtrlUnit_DIZZY();
    virtual int Tick(float dt);
};

/* GET_COMBO: takes a combination of hits. */
class BlackBull0210ModeCtrlUnit_GET_COMBO : public BlackBull0210CtrlUnit {
  public:
    BlackBull0210ModeCtrlUnit_GET_COMBO(BlackBull0210VarParam *owner)
        : BossCtrlUnit(owner), BlackBull0210CtrlUnit(owner) {
        m_owner->m_motionLocked = 1;
        m_owner->m_motionLocked2 = 0;
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 5);
        fn_80416148(m_owner->m_motion, 0, 10.0f);
    }
    virtual ~BlackBull0210ModeCtrlUnit_GET_COMBO();
    virtual int Tick(float dt);
};

/* GET_ATK: the boss is hit. */
class BlackBull0210ModeCtrlUnit_GET_ATK : public BlackBull0210CtrlUnit {
  public:
    BlackBull0210ModeCtrlUnit_GET_ATK(BlackBull0210VarParam *owner)
        : BossCtrlUnit(owner), BlackBull0210CtrlUnit(owner) {
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
        m_owner->m_motionLocked2 = 1;
        m_owner->m_comboCount++;
    }
    virtual ~BlackBull0210ModeCtrlUnit_GET_ATK();
    virtual int Tick(float dt);

    /* 0x10 */ float m_time;
};

/* BELOW_HALF: the cut-in when the boss's health falls below half. */
class BlackBull0210ModeCtrlUnit_BELOW_HALF : public BlackBull0210CtrlUnit {
  public:
    BlackBull0210ModeCtrlUnit_BELOW_HALF(BlackBull0210VarParam *owner)
        : BossCtrlUnit(owner), BlackBull0210CtrlUnit(owner) {
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
    virtual ~BlackBull0210ModeCtrlUnit_BELOW_HALF();
    virtual int Tick(float dt);
};

/* DEATH: the boss falls and the stage ends. */
class BlackBull0210ModeCtrlUnit_DEATH : public BlackBull0210CtrlUnit {
  public:
    BlackBull0210ModeCtrlUnit_DEATH(BlackBull0210VarParam *owner)
        : BossCtrlUnit(owner), BlackBull0210CtrlUnit(owner) {
        m_ended = 0;
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 2);
        m_owner->m_chara->m_position.y = 80.0f;
        m_owner->m_stopped = 1;
        m_owner->field_0x170 = 0;
        m_owner->field_0x171 = 0;
        m_owner->m_motionLocked = 1;
        m_owner->m_motionLocked2 = 1;
        fn_803358D0(m_owner, new BossCtrlUnit_PlayCameraMotion(m_owner, 8, 0.0f, 0.0f));
        m_owner->m_active = 0;
        m_owner->m_attribute = 0;
        fn_801FAF00(GetHintManager(), 0xA7, 2, 0, 0);
        fn_8034DF48(7, -2);
    }
    virtual ~BlackBull0210ModeCtrlUnit_DEATH();
    virtual int Tick(float dt);

    /* 0x10 */ boost::weak_ptr<EffectElement> m_effect;
    /* 0x18 */ u8 m_ended;
};

/* ATK_BREATH: breathes a fire ball along its head. */
class BlackBull0210ModeCtrlUnit_ATK_BREATH : public BlackBull0210CtrlUnit {
  public:
    BlackBull0210ModeCtrlUnit_ATK_BREATH(BlackBull0210VarParam *owner)
        : BossCtrlUnit(owner), BlackBull0210CtrlUnit(owner) {
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80416148(m_owner->m_motion, 0, 10.0f);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 3);
        m_owner->m_motionLocked = 1;
        m_owner->m_motionLocked2 = 1;
        m_owner->m_attribute = 0;
    }
    virtual ~BlackBull0210ModeCtrlUnit_ATK_BREATH();
    virtual int Tick(float dt);
};

/* ATK_WAVERING: shoots fire balls at the player, m_waveCount times. */
class BlackBull0210ModeCtrlUnit_ATK_WAVERING : public BlackBull0210CtrlUnit {
  public:
    BlackBull0210ModeCtrlUnit_ATK_WAVERING(BlackBull0210VarParam *owner)
        : BossCtrlUnit(owner), BlackBull0210CtrlUnit(owner) {
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80416148(m_owner->m_motion, 0, 10.0f);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 4);
        m_owner->field_0x170 = 0;
        m_owner->field_0x171 = 0;
        m_owner->m_motionLocked = 1;
        m_owner->m_motionLocked2 = 0;
        m_count = 0;
        m_owner->m_attribute = 0;
    }
    virtual ~BlackBull0210ModeCtrlUnit_ATK_WAVERING();
    virtual int Tick(float dt);

    /* 0x10 */ s32 m_count;
};

/* ATK_CLOSERANGE: drops a fire ball straight down. */
class BlackBull0210ModeCtrlUnit_ATK_CLOSERANGE : public BlackBull0210CtrlUnit {
  public:
    BlackBull0210ModeCtrlUnit_ATK_CLOSERANGE(BlackBull0210VarParam *owner)
        : BossCtrlUnit(owner), BlackBull0210CtrlUnit(owner) {
        fn_804165D8(m_owner->m_motion, 0, -1, -1);
        fn_80416148(m_owner->m_motion, 0, 10.0f);
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 4);
        m_owner->field_0x170 = 0;
        m_owner->field_0x171 = 0;
        m_owner->m_motionLocked = 1;
        m_owner->m_motionLocked2 = 0;
        m_owner->m_attribute = 0;
    }
    virtual ~BlackBull0210ModeCtrlUnit_ATK_CLOSERANGE();
    virtual int Tick(float dt);
};

/* START: the boss's entrance. */
class BlackBull0210ModeCtrlUnit_START : public BlackBull0210CtrlUnit {
  public:
    BlackBull0210ModeCtrlUnit_START(BlackBull0210VarParam *owner)
        : BossCtrlUnit(owner), BlackBull0210CtrlUnit(owner) {
        fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 0xA);
        m_owner->m_stopped = 1;
        m_owner->field_0x171 = 0;
        m_owner->m_motionLocked = 1;
        m_owner->m_motionLocked2 = 1;
        fn_803358D0(m_owner, new BossCtrlUnit_PlayCameraMotion(m_owner, 6, 0.0f, 0.0f));
        m_owner->m_active = 0;
        fn_801FAF00(GetHintManager(), 0xA4, 0, 0, 0);
        fn_8035367C("CsdFiles/BChara_BBull", "bchara", "open");
        m_owner->m_attribute = 0;
    }
    virtual ~BlackBull0210ModeCtrlUnit_START();
    virtual int Tick(float dt);
};

/* MOVETEST: walks to a test destination (a debug mode). */
class BlackBull0210ModeCtrlUnit_MOVETEST : public BlackBull0210CtrlUnit {
  public:
    BlackBull0210ModeCtrlUnit_MOVETEST(BlackBull0210VarParam *owner)
        : BossCtrlUnit(owner), BlackBull0210CtrlUnit(owner) {}
    virtual ~BlackBull0210ModeCtrlUnit_MOVETEST();
    virtual int Tick(float dt);
};

SIZE_ASSERT(BlackBull0210ModeCtrlUnit, 0x2C);
SIZE_ASSERT(BlackBull0210ModeCtrlUnit_WAIT, 0x24);
SIZE_ASSERT(BlackBull0210ModeCtrlUnit_EDIT, 0x20);
SIZE_ASSERT(BlackBull0210ModeCtrlUnit_GET_ATK, 0x24);
SIZE_ASSERT(BlackBull0210ModeCtrlUnit_DEATH, 0x2C);
SIZE_ASSERT(BlackBull0210ModeCtrlUnit_ATK_WAVERING, 0x24);

BlackBull0210ModeCtrlUnit_MOVETEST::~BlackBull0210ModeCtrlUnit_MOVETEST() {}

int BlackBull0210ModeCtrlUnit_MOVETEST::Tick(float dt) {
    AssignVector(m_owner->m_destination, m_owner->m_testDestination);
    m_owner->m_destination.y = m_owner->m_position.y;
    m_owner->m_nextMode = 2;
    return 0;
}

BlackBull0210ModeCtrlUnit_START::~BlackBull0210ModeCtrlUnit_START() {
    m_owner->m_stopped = 0;
    m_owner->field_0x171 = 1;
    m_owner->m_active = 1;
}

int BlackBull0210ModeCtrlUnit_START::Tick(float dt) {
    if (m_owner->m_attribute) {
        fn_8034DF48(0xC, -2);
        m_owner->m_attribute = 0;
    }
    if (fn_80416F28(m_owner->m_motion, 0, 3, 1)) {
        m_owner->m_nextMode = 2;
        m_owner->m_arrived = 1;
    }
    return 0;
}

BlackBull0210ModeCtrlUnit_ATK_CLOSERANGE::~BlackBull0210ModeCtrlUnit_ATK_CLOSERANGE() {
    m_owner->field_0x170 = 1;
    m_owner->field_0x171 = 1;
}

int BlackBull0210ModeCtrlUnit_ATK_CLOSERANGE::Tick(float dt) {
    if (m_owner->m_attribute) {
        Vec3f translation;
        fn_804114D0(&translation, fn_80415E70(m_owner->m_motion, 0x13A6));
        Vec3f position;
        fn_80016350(&position, &translation);
        Vec3f velocity(0.0f, -1.0f, 0.0f);
        fn_8040CE38(&velocity, m_owner->m_fireSpeed);
        fn_803358D0(m_owner, new BlackBullCtrlUnit_FireBall(m_owner, &position, &velocity, 1));
        m_owner->m_attribute = 0;
    }
    if (fn_80416F28(m_owner->m_motion, 0, 3, 1)) {
        m_owner->m_nextMode = 2;
    }
    return 0;
}

BlackBull0210ModeCtrlUnit_ATK_WAVERING::~BlackBull0210ModeCtrlUnit_ATK_WAVERING() {
    m_owner->field_0x170 = 1;
    m_owner->field_0x171 = 1;
}

int BlackBull0210ModeCtrlUnit_ATK_WAVERING::Tick(float dt) {
    if (m_owner->m_attribute) {
        Vec3f translation;
        fn_804114D0(&translation, fn_80415E70(m_owner->m_motion, 0x13A6));
        Vec3f position;
        fn_80016350(&position, &translation);
        Vec3f velocity;
        fn_800091EC(&velocity, &m_owner->m_chara->m_position);
        velocity.y -= 80.0f;
        fn_8040CE04(&velocity, &position);
        fn_8040CE74(&velocity);
        fn_8040CE38(&velocity, m_owner->m_fireSpeed);
        fn_803358D0(m_owner, new BlackBullCtrlUnit_FireBall(m_owner, &position, &velocity, 2));
        m_count++;
        m_owner->m_attribute = 0;
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

BlackBull0210ModeCtrlUnit_ATK_BREATH::~BlackBull0210ModeCtrlUnit_ATK_BREATH() {}

int BlackBull0210ModeCtrlUnit_ATK_BREATH::Tick(float dt) {
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

BlackBull0210ModeCtrlUnit_DEATH::~BlackBull0210ModeCtrlUnit_DEATH() {
    boost::shared_ptr<EffectElement> effect = m_effect.lock();
    if (effect.px != 0) {
        effect->Stop();
        fn_800A4210(&effect);
    }
}

int BlackBull0210ModeCtrlUnit_DEATH::Tick(float dt) {
    if (m_owner->m_attribute) {
        m_effect = fn_800A31F0()->Launch(m_owner->m_effects, 0);
        boost::shared_ptr<EffectElement> effect = m_effect.lock();
        if (effect.px != 0) {
            Vec3f position;
            fn_800091EC(&position, &m_owner->m_chara->m_position);
            position.y = 0.0f;
            fn_80425E4C(effect.px, &position);
            effect->SetField14((u32)(lbl_8057D6A0 + 0x24));
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

BlackBull0210ModeCtrlUnit_BELOW_HALF::~BlackBull0210ModeCtrlUnit_BELOW_HALF() {
    m_owner->m_stopped = 0;
    m_owner->field_0x171 = 1;
    m_owner->m_active = 1;
    fn_801FAAA4(GetHintManager());
    fn_801FAF00(GetHintManager(), 0xA6, 2, 0, 0);
    fn_801FAAB4(GetHintManager());
}

int BlackBull0210ModeCtrlUnit_BELOW_HALF::Tick(float dt) {
    if (IsMotion(m_owner, 1)) {
        if (fn_80416F28(m_owner->m_motion, 0, 3, 1)) {
            fn_804165D8(m_owner->m_motion, 0, -1, -1);
            fn_80416148(m_owner->m_motion, 0, 20.0f);
            fn_80417568(m_owner->m_motion, m_owner->m_motionBase + 5);
        }
    } else if (IsMotion(m_owner, 5)) {
        if (fn_80416F28(m_owner->m_motion, 0, 6, 1)) {
            m_owner->m_nextMode = 4;
        }
    }
    return 0;
}

BlackBull0210ModeCtrlUnit_GET_ATK::~BlackBull0210ModeCtrlUnit_GET_ATK() {
    if (m_owner->m_stopped) {
        if (!m_owner->m_dizzy && m_owner->m_comboCount >= m_owner->m_comboLimit) {
            if (HealthRatio(m_owner) <= 0.5f) {
                fn_801FAF00(GetHintManager(), 0xA9, 2, 0, 0);
            } else {
                fn_801FAF00(GetHintManager(), 0xA8, 2, 0, 0);
            }
        }
        m_owner->m_stopped = 0;
    }
}

int BlackBull0210ModeCtrlUnit_GET_ATK::Tick(float dt) {
    if (m_owner->m_stopped) {
        m_time += *(float *)(lbl_8057D6A0 + 0x18);
        if (m_time > m_owner->m_stunTime && m_owner->m_damage <= m_owner->m_damageLimit) {
            m_owner->m_stopped = 0;
            if (!m_owner->m_dizzy && m_owner->m_comboCount >= m_owner->m_comboLimit) {
                if (HealthRatio(m_owner) <= 0.5f) {
                    fn_801FAF00(GetHintManager(), 0xA9, 2, 0, 0);
                } else {
                    fn_801FAF00(GetHintManager(), 0xA8, 2, 0, 0);
                }
            }
        }
    }
    if (fn_80416F28(m_owner->m_motion, 0, 3, 1)) {
        fn_80416198(m_owner->m_motion, 0);
        if (m_owner->m_comboCount >= m_owner->m_comboLimit) {
            m_owner->m_nextMode = 5;
        } else {
            m_owner->m_nextMode = 6;
        }
    } else {
        UnkMotionFrames frames;
        fn_80415D40(m_owner->m_motion, 0, &frames);
        if (frames.m_frame > frames.m_end - 4.0f) {
            fn_80415F98(m_owner->m_motion, 0, 0.0f, 0x1392, (frames.m_end - frames.m_frame) / 4.0f);
        }
    }
    return 0;
}

BlackBull0210ModeCtrlUnit_GET_COMBO::~BlackBull0210ModeCtrlUnit_GET_COMBO() {}

int BlackBull0210ModeCtrlUnit_GET_COMBO::Tick(float dt) {
    if (m_owner->m_damage > m_owner->m_damageLimit) {
        m_owner->m_stopped = 1;
        m_owner->m_comboCount = 0;
        m_owner->m_nextMode = 4;
        return 0;
    }
    if (m_owner->m_comboCount >= m_owner->m_comboLimit) {
        m_owner->m_stopped = 1;
        m_owner->m_comboCount = 0;
        m_owner->m_nextMode = 4;
        return 0;
    }
    if (m_owner->m_hit) {
        m_owner->m_comboCount = 0;
        m_owner->m_nextMode = 4;
    }
    return 0;
}

BlackBull0210ModeCtrlUnit_DIZZY::~BlackBull0210ModeCtrlUnit_DIZZY() {
    m_owner->field_0x171 = 1;
}

int BlackBull0210ModeCtrlUnit_DIZZY::Tick(float dt) {
    if (m_owner->m_dizzyEnded) {
        m_owner->m_stopped = 1;
        m_owner->m_comboCount = 0;
        m_owner->m_dizzy = 0;
        m_owner->m_nextMode = 4;
    }
    return 0;
}

BlackBull0210ModeCtrlUnit_ESCAPE::~BlackBull0210ModeCtrlUnit_ESCAPE() {
    m_owner->field_0x171 = 1;
}

int BlackBull0210ModeCtrlUnit_ESCAPE::Tick(float dt) {
    m_owner->m_damage = 0.0f;
    m_owner->field_0x178 = 0.0f;
    if (m_owner->m_destinationDistance < m_owner->m_arriveDistance) {
        m_owner->m_nextMode = 2;
        m_owner->m_arrived = 1;
    }
    return 0;
}

BlackBull0210ModeCtrlUnit_CHASE::~BlackBull0210ModeCtrlUnit_CHASE() {}

int BlackBull0210ModeCtrlUnit_CHASE::Tick(float dt) {
    if (m_owner->m_destinationDistance < m_owner->m_arriveDistance) {
        m_owner->m_nextMode = 2;
        m_owner->m_arrived = 1;
    }
    return 0;
}

BlackBull0210ModeCtrlUnit_EDIT::~BlackBull0210ModeCtrlUnit_EDIT() {}

int BlackBull0210ModeCtrlUnit_EDIT::Tick(float dt) {
    return 0;
}

BlackBull0210ModeCtrlUnit_WAIT::~BlackBull0210ModeCtrlUnit_WAIT() {}

int BlackBull0210ModeCtrlUnit_WAIT::Tick(float dt) {
    m_time -= dt;
    if (m_time < 0.0f) {
        m_time = 0.0f;
    }
    float diff = m_owner->m_targetAngle - m_owner->m_angle;
    while (std::fabs(diff) > lbl_805FAD38) {
        diff += diff > 0.0f ? -2.0f * lbl_805FAD38 : 2.0f * lbl_805FAD38;
    }
    if (m_owner->m_damage > m_owner->m_damageLimit || m_owner->field_0x178 > 0.0f) {
        m_owner->m_nextMode = 4;
        return 0;
    }
    if (!m_owner->m_hit) {
        return 0;
    }
    if (m_owner->field_0x15C < m_owner->field_0xB0) {
        m_owner->m_nextMode = 0xD;
        return 0;
    }
    if (HealthRatio(m_owner) <= 0.5f) {
        if (std::fabs(diff) > 0.5f * lbl_805FAD38) {
            m_owner->m_nextMode = 3;
            return 0;
        }
    } else if (std::fabs(diff) > 0.25f * lbl_805FAD38) {
        m_owner->m_nextMode = 3;
        return 0;
    }
    if (ToDegree(std::fabs(m_owner->m_turnSpeed)) > m_owner->m_turnMotionSpeed) {
        return 0;
    }
    int sound = (m_owner->m_point + 1) * 2;
    if (diff < 0.0f) {
        sound--;
    }
    if (m_owner->m_sound != -1 && m_owner->m_sound != sound) {
        fn_801ED410(m_owner->m_sound);
        m_owner->m_sound = -1;
    }
    if (m_owner->m_sound == -1) {
        fn_801ED5A8(sound);
        m_owner->m_sound = sound;
    }
    if (m_time > 0.0f) {
        return 0;
    }
    if (HealthRatio(m_owner) <= 0.5f) {
        m_owner->m_nextMode = 0xB;
    } else {
        m_owner->m_nextMode = 0xA;
    }
    return 0;
}

BossCtrlUnit *BlackBull0210ModeCtrlUnit::CreateMode(int mode) {
    switch (mode) {
    case 0:
    case 2:
    case 12:
    default:
        return new BlackBull0210ModeCtrlUnit_WAIT(m_owner);
    case 1:
        return new BlackBull0210ModeCtrlUnit_EDIT(m_owner);
    case 3:
        return new BlackBull0210ModeCtrlUnit_CHASE(m_owner);
    case 4:
        return new BlackBull0210ModeCtrlUnit_ESCAPE(m_owner);
    case 5:
        return new BlackBull0210ModeCtrlUnit_DIZZY(m_owner);
    case 6:
        return new BlackBull0210ModeCtrlUnit_GET_COMBO(m_owner);
    case 7:
        return new BlackBull0210ModeCtrlUnit_GET_ATK(m_owner);
    case 8:
        return new BlackBull0210ModeCtrlUnit_BELOW_HALF(m_owner);
    case 9:
        return new BlackBull0210ModeCtrlUnit_DEATH(m_owner);
    case 10:
        return new BlackBull0210ModeCtrlUnit_ATK_BREATH(m_owner);
    case 11:
        return new BlackBull0210ModeCtrlUnit_ATK_WAVERING(m_owner);
    case 13:
        return new BlackBull0210ModeCtrlUnit_ATK_CLOSERANGE(m_owner);
    case 14:
        return new BlackBull0210ModeCtrlUnit_START(m_owner);
    case 15:
        return new BlackBull0210ModeCtrlUnit_MOVETEST(m_owner);
    }
}

BlackBull0210ModeCtrlUnit::~BlackBull0210ModeCtrlUnit() {}

int BlackBull0210ModeCtrlUnit::Tick(float dt) {
    return BossModeCtrlUnit::Tick(dt);
}
