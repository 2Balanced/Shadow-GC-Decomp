/* BlackDoomCtrlUnit_AlterEgo (class name from RTTI; see Game/BlackDoomCtrlUnit.h): the Black Doom
 * boss's copies. Each alter ego owns one of the boss's queued attacks (m_owner->m_attacks[m_index])
 * and steps through its states: waiting for the alter ego before it, appearing behind it (or behind
 * the boss), fading in, following, the disk, meteor and boomerang attacks the boss's modes order,
 * fading out, and waiting to start again. The file name and the member names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Effect/System/EffectElement.h"
#include "Game/BlackDoomCtrlUnit.h"
#include "Game/BossCtrlUnit.h"
#include "Game/CharInfo.h"
#include "Game/EffectPJSLauncher.h"
#include "Game/EnemyCharInfo.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* MSL's std::fabs for floats, which the game calls out of line (as in BlackDoomCtrlUnit.cpp). */
namespace std {
float fabs(float x);
} // namespace std

using Effect::System::EffectElement;
using Effect::System::EffectLaunchInfo;

/* RenderWare's clump (rw/rwcore.h), reduced to what is used here. */
extern "C" {
typedef struct RpClump RpClump;
}

/* An object that keeps a clump in step with a motion (class name unknown; as in
 * BlackDoomCtrlUnit.cpp). */
class UnkClumpUpdater_80015A7C {
  public:
    UnkClumpUpdater_80015A7C(RpClump *clump, _stMtnPlayWork *motion);
    ~UnkClumpUpdater_80015A7C();

    /* 0x00 */ u8 field_0x0[0x1C];
};

/* A clump's colour fade (struct name unknown; as in BlackDoomCtrlUnit.cpp). */
struct UnkClumpFade_80027D5C {
    /* 0x00 */ float m_color[4];
    /* 0x10 */ u8 field_0x10[0x24 - 0x10];
    /* 0x24 */ u8 m_mode;
    /* 0x25 */ u8 field_0x25[0x2C - 0x25];

    /* Sets the alpha (name inferred). */
    void SetAlpha(float alpha) {
        m_color[3] = alpha;
    }
};
SIZE_ASSERT(UnkClumpFade_80027D5C, 0x2C);

/* The boss's tables (see Game/BossDef.h). */
struct UnkBossDef {
    /* 0x00 */ s32 m_model;
    /* 0x04 */ s32 m_motion;
};

/* What fn_80415D40 fills in about a motion (as in BlackBull0210ModeCtrlUnit.cpp). */
struct UnkMotionFrames {
    /* 0x00 */ float field_0x0;
    /* 0x04 */ float m_end;
    /* 0x08 */ float field_0x8;
    /* 0x0C */ float m_frame;
};

/* The effect manager fn_800A31F0 returns (as in BlackDoomCtrlUnit.cpp). */
class UnkEffectManager_800A31F0 {
  public:
    virtual void vf08();
    virtual boost::weak_ptr<EffectElement> LaunchId(s32 id, s32 arg);
    virtual boost::weak_ptr<EffectElement> Launch(boost::shared_ptr<EffectLaunchInfo> info, s32 id);
};

/* The game state (as in BlackDoomCtrlUnit.cpp): the frame time at 0x24. */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x24];
    /* 0x024 */ float m_dt;
};
extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The frame time the effects follow. */
#define FrameTime() ((u32)&lbl_8057D6A0.m_dt)

/* The world of each stage area (as in BlackDoomCtrlUnit.cpp). */
struct UnkWorldEntry_8051D208 {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ void *m_world;
    /* 0x14 */ u8 field_0x14[0x18 - 0x14];
};
extern UnkWorldEntry_8051D208 lbl_8051D208[];

/* The number of alter egos (defined in Game/BlackDoom.cpp). */
namespace BlackDoomData {
extern const s32 kAlterEgoCount;
} // namespace BlackDoomData

extern "C" {
/* Effects (see BlackDoomCtrlUnit.cpp). */
UnkEffectManager_800A31F0 *fn_800A31F0(void);
void fn_800A4210(const boost::shared_ptr<EffectElement> &effect);
void fn_80425E4C(EffectElement *effect, const Vec3f *position);

/* Sounds (see BlackDoomCtrlUnit.cpp). */
void *fn_80053164(void);
void fn_801D54F4(void *manager, int id, const void *source, int arg3, int arg4);
void fn_801D53C4(void *manager, int id, const Vec3f *position, void *source, int arg4, int arg5,
                 int arg6);

/* Clumps, motions and fades (see BlackDoomCtrlUnit.cpp and BossMotion.cpp). */
void *fn_8046256C(RpClump *clump);
void fn_804623EC(void *world, RpClump *clump);
void fn_804622FC(void *world, RpClump *clump);
RpClump *fn_80457090(void *data);
void fn_80457468(RpClump *clump);
/* A singleton of the stage areas (behaviour unknown; its result is unused here). */
void *fn_8003A840(void);
UnkClumpFade_80027D5C *fn_80027D5C(UnkClumpFade_80027D5C *fade);
void fn_80038198(RpClump *clump, UnkClumpFade_80027D5C *fade);
void *fn_800122D4(void);
void *fn_8001277C(void *loader, s32 slot, int arg2);
void *fn_80012684(void *loader, int slot, int index);
_stMtnPlayWork *fn_8041B908(void *data, int arg1);
void fn_8041B884(_stMtnPlayWork *work);
void fn_800159AC(UnkClumpUpdater_80015A7C *updater);
u16 fn_80415CDC(_stMtnPlayWork *work, int arg1);
void fn_80417568(_stMtnPlayWork *work, u16 motion);
void fn_804165D8(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
void fn_80416148(_stMtnPlayWork *work, int arg1, float arg2);
int fn_80416F28(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
void fn_80415D40(_stMtnPlayWork *work, int arg1, UnkMotionFrames *frames);
void fn_80416E20(_stMtnPlayWork *work, int arg1, float frame);
void fn_804173A8(_stMtnPlayWork *work, const Vec3f *position, const Vec3f *rotation, int arg3);
void fn_80416DC8(_stMtnPlayWork *work, int arg1, float dt);
void fn_804160F8(_stMtnPlayWork *work, int arg1, float dt);
void fn_80415EA4(_stMtnPlayWork *work);
int fn_80415854(_stMtnPlayWork *work, int arg1, u16 *type, void **data, int arg4);

/* The boss's units (see Game/BossCallbackContainer.cpp). */
void fn_803358D0(BossVarParam *param, BossCtrlUnit *unit);

/* Math (see BlackDoomCtrlUnit.cpp). */
float fn_8040EE90(void);             /* a random value in [0, 1) */
float fn_80016260(float x, float z); /* the angle of (x, z) */
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
Vec3f fn_8040CEE8(const Vec3f *vector);
}

/* fn_8040CD74 called as returning a - b by value (see ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

inline const Vec3f &Ref(const Vec3f &vector) {
    return vector;
}

inline Vec3f Sub(const Vec3f &a, const Vec3f &b) {
    return ((VectorSubtractFn)fn_8040CD74)(&a, &b);
}

/* Degrees to radians (name inferred). */
inline float ToRadian(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* Puts clump in world (name inferred; as in BlackDoomCtrlUnit.cpp). */
inline void SetClumpWorld(RpClump *clump, void *world) {
    void *current = fn_8046256C(clump);
    if (current != world) {
        if (current != 0) {
            fn_804623EC(current, clump);
        }
        fn_804622FC(world, clump);
    }
}

/* Whether work plays motion (name inferred; as in BlackBull0210CtrlUnit.cpp). */
inline bool IsPlaying(_stMtnPlayWork *work, u16 motion) {
    return motion == fn_80415CDC(work, 0);
}

/* The alter egos' states (names inferred). */
enum {
    ALTER_EGO_WAIT = 0,
    ALTER_EGO_START = 1,
    ALTER_EGO_APPEAR = 2,
    ALTER_EGO_FADE_IN = 3,
    ALTER_EGO_FOLLOW = 4,
    ALTER_EGO_DISK = 5,
    ALTER_EGO_METEOR = 6,
    ALTER_EGO_BOOMERANG = 7,
    ALTER_EGO_FADE_OUT = 8,
    ALTER_EGO_END = 9
};

BlackDoomCtrlUnit_AlterEgo::BlackDoomCtrlUnit_AlterEgo(BlackDoomVarParam *param)
    : BossCtrlUnit(param), BlackDoomCtrlUnit(param) {
    m_kind = 2;
    if (m_owner->field_0x184 >= BlackDoomData::kAlterEgoCount) {
        m_index = -1;
        return;
    }
    m_index = m_owner->field_0x184++;
    m_owner->m_attacks[m_index].m_state = ALTER_EGO_WAIT;
    m_owner->m_attacks[m_index].field_0x4 = 0;
    fn_800091D0(&m_owner->m_attacks[m_index].m_position, &m_owner->m_chara->m_position);
    m_owner->m_attacks[m_index].m_rotation = m_owner->m_chara->m_rotation.y;
    void *data = fn_8001277C(fn_800122D4(), 0x16, m_owner->m_def->m_model);
    m_clump = 0;
    if (data != 0) {
        m_clump = fn_80457090(data);
    }
    m_motion = 0;
    if (m_clump != 0) {
        UnkClumpFade_80027D5C *fade =
            (UnkClumpFade_80027D5C *)operator new(sizeof(UnkClumpFade_80027D5C));
        if (fade != 0) {
            fade = fn_80027D5C(fade);
        }
        m_fade = fade;
        if (m_fade != 0) {
            m_fade->m_mode = 0xC;
            fn_80038198(m_clump, m_fade);
        }
        m_motion = fn_8041B908(fn_80012684(fn_800122D4(), 0x16, m_owner->m_def->m_motion), 1);
    }
    m_updater = 0;
    if (m_motion != 0) {
        m_updater = new UnkClumpUpdater_80015A7C(m_clump, m_motion);
    }
}

BlackDoomCtrlUnit_AlterEgo::~BlackDoomCtrlUnit_AlterEgo() {
    boost::shared_ptr<EffectElement> effect;
    for (int i = 0; i < 5; i++) {
        effect = m_effects[i].lock();
        if (effect) {
            effect->Stop();
            fn_800A4210(effect);
        }
    }
    if (m_updater != 0) {
        delete m_updater;
        m_updater = 0;
    }
    if (m_motion != 0) {
        fn_8041B884(m_motion);
        m_motion = 0;
    }
    if (m_fade != 0) {
        operator delete(m_fade);
        m_fade = 0;
    }
    if (m_clump != 0) {
        void *world = fn_8046256C(m_clump);
        if (world != 0) {
            fn_804623EC(world, m_clump);
        }
        fn_80457468(m_clump);
        m_clump = 0;
    }
}

int BlackDoomCtrlUnit_AlterEgo::Tick(float dt) {
    if (m_index < 0) {
        return 1;
    }
    Vec3f offset;
    float speed;
    switch (m_owner->m_attacks[m_index].m_state) {
    case ALTER_EGO_WAIT:
        if (m_index != 0 && m_owner->m_attacks[m_index - 1].m_state != ALTER_EGO_WAIT) {
            m_owner->m_attacks[m_index].m_state = ALTER_EGO_START;
        }
        break;
    case ALTER_EGO_START:
        if (m_index == 0) {
            fn_800091D0(&m_owner->m_attacks[m_index].m_position, &m_owner->m_chara->m_position);
            m_owner->m_attacks[m_index].m_rotation = m_owner->m_chara->m_rotation.y;
        } else {
            fn_800091D0(&m_owner->m_attacks[m_index].m_position,
                        &m_owner->m_attacks[m_index - 1].m_position);
            m_owner->m_attacks[m_index].m_rotation = m_owner->m_attacks[m_index - 1].m_rotation;
        }
        m_owner->m_attacks[m_index].m_state = ALTER_EGO_APPEAR;
    case ALTER_EGO_APPEAR:
        if (m_index >= m_owner->m_attackStep) {
            m_owner->m_attacks[m_index].m_state = ALTER_EGO_WAIT;
            break;
        }
        if (m_index == 0) {
            fn_800091D0(&offset, &Ref(Sub(m_owner->m_chara->m_position,
                                          m_owner->m_attacks[m_index].m_position)));
        } else {
            fn_800091D0(&offset, &Ref(Sub(m_owner->m_attacks[m_index - 1].m_position,
                                          m_owner->m_attacks[m_index].m_position)));
        }
        if (fn_8040CF50(&offset) < m_owner->m_settings.field_0x74) {
            break;
        }
        m_owner->m_attacks[m_index].m_state = ALTER_EGO_FADE_IN;
        if (fn_8046256C(m_clump) == 0) {
            fn_8003A840();
            SetClumpWorld(m_clump, lbl_8051D208[14].m_world);
        }
        fn_80417568(m_motion, m_owner->m_motionBase + 7);
        m_alpha = 0.0f;
    case ALTER_EGO_FADE_IN:
        m_alpha += dt * m_owner->m_settings.field_0x70 / m_owner->m_settings.field_0x6C;
        if (m_alpha > m_owner->m_settings.field_0x70) {
            m_alpha = m_owner->m_settings.field_0x70;
            m_owner->m_attacks[m_index].m_state = ALTER_EGO_FOLLOW;
            m_owner->m_alterEgos++;
        }
        if (m_fade != 0) {
            m_fade->SetAlpha(m_alpha);
        }
    case ALTER_EGO_FOLLOW:
        if (m_index == 0) {
            fn_800091D0(&offset, &Ref(Sub(m_owner->m_chara->m_position,
                                          m_owner->m_attacks[m_index].m_position)));
            float distance = m_owner->m_settings.field_0x74;
            fn_800091D0(&m_owner->m_attacks[m_index].m_position,
                        &Ref(Sub(m_owner->m_chara->m_position,
                                 fn_8040CDA8(&Ref(fn_8040CEE8(&offset)), distance))));
        } else {
            fn_800091D0(&offset, &Ref(Sub(m_owner->m_attacks[m_index - 1].m_position,
                                          m_owner->m_attacks[m_index].m_position)));
            float distance = m_owner->m_settings.field_0x74;
            fn_800091D0(&m_owner->m_attacks[m_index].m_position,
                        &Ref(Sub(m_owner->m_attacks[m_index - 1].m_position,
                                 fn_8040CDA8(&Ref(fn_8040CEE8(&offset)), distance))));
        }
        Turn(dt);
        UpdateMotion(dt);
        break;
    case ALTER_EGO_DISK: {
        if (!IsPlaying(m_motion, m_owner->m_motionBase + 1)) {
            fn_804165D8(m_motion, 0, -1, -1);
            fn_80417568(m_motion, m_owner->m_motionBase + 1);
            fn_80416148(m_motion, 0, 10.0f);
            boost::shared_ptr<EffectElement> effect;
            for (int i = 0; i < 2; i++) {
                m_effects[i] = fn_800A31F0()->Launch(m_owner->m_effects, 6);
                effect = m_effects[i].lock();
                if (effect) {
                    effect->SetField4C((u32)&m_points[i]);
                    effect->SetField14(FrameTime());
                }
            }
            m_attribute = 0;
            m_shots = 2;
            m_point = 2.0f * fn_8040EE90();
        }
        fn_801D53C4(fn_80053164(), 0x8031, &m_owner->m_attacks[m_index].m_position, this, 3, 0, 3);
        PlaceDiskPoints();
        if (m_attribute != 0) {
            Vec3f direction;
            fn_800091EC(&direction, &Ref(Sub(m_owner->m_testDestination, m_points[m_point])));
            direction.y = 0.0f;
            speed = m_owner->m_settings.m_diskSpeed;
            fn_800091D0(&direction, &Ref(fn_8040CDA8(&Ref(fn_8040CEE8(&direction)), speed)));
            fn_803358D0(m_owner,
                        new BlackDoomCtrlUnit_Disk(m_owner, &m_points[m_point], &direction));
            m_shots--;
            m_attribute = 0;
            m_point = m_point == 0 ? 1 : 0;
            if (m_shots != 0) {
                UnkMotionFrames frames;
                fn_80415D40(m_motion, 0, &frames);
                float rewind = m_owner->m_settings.m_diskRewind;
                float blend = rewind;
                if (rewind > m_owner->m_settings.m_diskBlend) {
                    blend = m_owner->m_settings.m_diskBlend;
                }
                float frame = frames.m_frame - rewind;
                fn_804165D8(m_motion, 0, -1, -1);
                fn_80416148(m_motion, 0, blend);
                fn_80417568(m_motion, m_owner->m_motionBase + 1);
                if (frame > 0.0f) {
                    fn_80416E20(m_motion, 0, frame);
                }
            }
        }
        Turn(dt);
        UpdateMotion(dt);
        if (fn_80416F28(m_motion, 0, 3, 1)) {
            boost::shared_ptr<EffectElement> effect;
            for (int i = 0; i < 2; i++) {
                effect = m_effects[i].lock();
                if (effect) {
                    effect->Stop();
                    fn_800A4210(effect);
                }
            }
            m_owner->m_attacks[m_index].m_state = ALTER_EGO_FADE_OUT;
        }
        break;
    }
    case ALTER_EGO_METEOR: {
        if (!IsPlaying(m_motion, m_owner->m_motionBase + 3)) {
            fn_804165D8(m_motion, 0, -1, -1);
            fn_80417568(m_motion, m_owner->m_motionBase + 3);
            fn_80416148(m_motion, 0, 10.0f);
            m_attribute = 0;
            m_shots = 5;
            m_point = 5.0f * fn_8040EE90();
            m_done = 0;
            m_launched = 0;
            m_timer = 0.0f;
        }
        m_timer -= dt;
        PlaceMeteorPoints();
        if (m_launched) {
            fn_801D53C4(fn_80053164(), 0x8031, &m_owner->m_attacks[m_index].m_position, this, 3, 0,
                        3);
        }
        if (!m_done && m_attribute != 0) {
            if (m_attribute->m_state == 1) {
                int index = m_attribute->m_index;
                boost::shared_ptr<EffectElement> effect;
                m_effects[index] = fn_800A31F0()->Launch(m_owner->m_effects, 6);
                effect = m_effects[index].lock();
                if (effect) {
                    effect->SetField4C((u32)&m_points[index]);
                    effect->SetField14(FrameTime());
                }
                m_launched = 1;
            } else if (m_attribute->m_state == 2) {
                m_done = 1;
            }
            m_attribute = 0;
        }
        if (m_done && m_timer < 0.0f) {
            if (m_shots == 0) {
                boost::shared_ptr<EffectElement> effect;
                for (int i = 0; i < 5; i++) {
                    effect = m_effects[i].lock();
                    if (effect) {
                        effect->Stop();
                        fn_800A4210(effect);
                    }
                }
                m_owner->m_attacks[m_index].m_state = ALTER_EGO_FADE_OUT;
                break;
            }
            Vec3f velocity;
            fn_800091EC(&velocity, &Ref(Sub(m_owner->m_testDestination, m_points[m_point])));
            speed = m_owner->m_settings.m_meteorSpeed;
            fn_800091D0(&velocity, &Ref(fn_8040CDA8(&Ref(fn_8040CEE8(&velocity)), speed)));
            fn_803358D0(m_owner,
                        new BlackDoomCtrlUnit_Meteor(m_owner, &m_points[m_point], &velocity));
            m_shots--;
            m_point = 5.0f * fn_8040EE90();
            m_timer = m_owner->m_settings.m_meteorInterval;
        }
        Turn(dt);
        UpdateMotion(dt);
        break;
    }
    case ALTER_EGO_BOOMERANG: {
        if (!IsPlaying(m_motion, m_owner->m_motionBase + 2)) {
            fn_804165D8(m_motion, 0, -1, -1);
            fn_80417568(m_motion, m_owner->m_motionBase + 2);
            fn_80416148(m_motion, 0, 10.0f);
            m_effects[0] = fn_800A31F0()->Launch(m_owner->m_effects, 0x1E);
            boost::shared_ptr<EffectElement> effect(m_effects[0].lock());
            if (effect) {
                Vec3f position;
                fn_800091EC(&position, &m_owner->m_attacks[m_index].m_position);
                position.y = 5.0f + (100.0f + m_owner->m_home.y);
                fn_80425E4C(effect.get(), &position);
                effect->SetField14(FrameTime());
            }
            fn_801D54F4(fn_80053164(), 0x8037, &m_owner->m_attacks[m_index].m_position, 0, 1);
            m_attribute = 0;
        }
        if (m_attribute != 0) {
            Vec3f position;
            fn_800091EC(&position, &m_owner->m_attacks[m_index].m_position);
            position.y = m_owner->m_home.y - 35.0f;
            Vec3f velocity;
            fn_800091EC(&velocity, &Ref(Sub(m_owner->m_testDestination, position)));
            velocity.y = 0.0f;
            speed = m_owner->m_settings.m_boomerangSpeed;
            fn_800091D0(&velocity, &Ref(fn_8040CDA8(&Ref(fn_8040CEE8(&velocity)), speed)));
            fn_803358D0(m_owner, new BlackDoomCtrlUnit_Boomerang(m_owner, &position, &velocity));
            boost::shared_ptr<EffectElement> effect(m_effects[0].lock());
            if (effect) {
                effect->Stop();
                fn_800A4210(effect);
            }
            m_attribute = 0;
        }
        Turn(dt);
        UpdateMotion(dt);
        if (fn_80416F28(m_motion, 0, 3, 1)) {
            m_owner->m_attacks[m_index].m_state = ALTER_EGO_FADE_OUT;
        }
        break;
    }
    case ALTER_EGO_FADE_OUT:
        m_alpha -= dt * m_owner->m_settings.field_0x70 / m_owner->m_settings.field_0x6C;
        if (m_alpha < 0.0f) {
            m_alpha = 0.0f;
            void *world = fn_8046256C(m_clump);
            if (world != 0) {
                fn_804623EC(world, m_clump);
            }
            m_owner->m_attacks[m_index].m_state = ALTER_EGO_END;
        }
        if (m_fade != 0) {
            m_fade->SetAlpha(m_alpha);
        }
        break;
    case ALTER_EGO_END:
        if (m_index == 0 || m_owner->m_attacks[m_index - 1].m_state == ALTER_EGO_WAIT) {
            m_owner->m_attacks[m_index].m_state = ALTER_EGO_WAIT;
            m_owner->m_alterEgos--;
        }
        break;
    }
    return 0;
}

/* Turns the alter ego towards where the boss aims. */
void BlackDoomCtrlUnit_AlterEgo::Turn(float dt) {
    Vec3f offset;
    fn_800091EC(&offset, &Ref(Sub(m_owner->m_testDestination,
                                  m_owner->m_attacks[m_index].m_position)));
    float diff = fn_80016260(-offset.x, -offset.z) - m_owner->m_attacks[m_index].m_rotation;
    while (std::fabs(diff) > lbl_805FAD38) {
        diff += diff > 0.0f ? -2.0f * lbl_805FAD38 : 2.0f * lbl_805FAD38;
    }
    float speed = ToRadian(m_owner->m_settings.m_turnSpeedMax);
    float step = speed;
    if (std::fabs(diff) < speed * dt) {
        m_owner->m_attacks[m_index].m_rotation += diff;
    } else {
        if (diff < 0.0f) {
            step = -speed;
        }
        m_owner->m_attacks[m_index].m_rotation += step * dt;
    }
}

/* Places the model and advances its motion, keeping the motion's attribute 0x5000. */
void BlackDoomCtrlUnit_AlterEgo::UpdateMotion(float dt) {
    Vec3f position;
    position.x = m_owner->m_attacks[m_index].m_position.x;
    position.y = m_owner->m_attacks[m_index].m_position.y;
    position.z = m_owner->m_attacks[m_index].m_position.z;
    Vec3f rotation;
    rotation.x = 0.0f;
    rotation.y = m_owner->m_attacks[m_index].m_rotation;
    rotation.z = 0.0f;
    fn_804173A8(m_motion, &position, &rotation, 0);
    fn_80416DC8(m_motion, 0, lbl_8057D6A0.m_dt);
    fn_804160F8(m_motion, 0, lbl_8057D6A0.m_dt);
    fn_80415EA4(m_motion);
    if (m_updater != 0) {
        fn_800159AC(m_updater);
    }
    u16 type;
    void *value;
    for (_stMtnPlayWork *work = m_motion; fn_80415854(work, 0, &type, &value, 0) != 0; work = 0) {
        switch (type) {
        case 0x5000:
            m_attribute = (const BlackDoomAttribute *)value;
            break;
        case 0x5001:
            break;
        }
    }
}

/* Places the two points the disks start from, at the alter ego's sides. */
void BlackDoomCtrlUnit_AlterEgo::PlaceDiskPoints() {
    Mtx34f matrix;
    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_owner->m_attacks[m_index].m_position);
    fn_8040B848(&matrix, m_owner->m_attacks[m_index].m_rotation);
    Vec3f points[2];
    Vec3f point0(30.0f, 20.0f, 0.0f);
    fn_800091EC(&points[0], &point0);
    Vec3f point1(-30.0f, 20.0f, 0.0f);
    fn_800091EC(&points[1], &point1);
    for (int i = 0; i < 2; i++) {
        fn_800091D0(&m_points[i], &Ref(fn_8040B67C(&matrix, &points[i])));
    }
}

/* Places the five points the meteors start from, in an arc above the alter ego. */
void BlackDoomCtrlUnit_AlterEgo::PlaceMeteorPoints() {
    Mtx34f matrix;
    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_owner->m_attacks[m_index].m_position);
    fn_8040B848(&matrix, m_owner->m_attacks[m_index].m_rotation);
    Vec3f points[5];
    Vec3f point0(-25.0f, 10.0f, 0.0f);
    fn_800091EC(&points[0], &point0);
    Vec3f point1(-30.0f, 45.0f, 0.0f);
    fn_800091EC(&points[1], &point1);
    Vec3f point2(0.0f, 60.0f, 0.0f);
    fn_800091EC(&points[2], &point2);
    Vec3f point3(30.0f, 45.0f, 0.0f);
    fn_800091EC(&points[3], &point3);
    Vec3f point4(25.0f, 10.0f, 0.0f);
    fn_800091EC(&points[4], &point4);
    for (int i = 0; i < 5; i++) {
        fn_800091D0(&m_points[i], &Ref(fn_8040B67C(&matrix, &points[i])));
    }
}
