/* Control units of the Heavy Dog boss's 0410 stage (class names from RTTI): the dust the boss
 * raises, the mines it drops and the boss's movement. File name inferred from the classes; all
 * member names and the names marked "(name inferred)" are not original. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include <boost/function.hpp>

#include "Effect/System/EffectElement.h"
#include "Game/BossCtrlUnit.h"
#include "Game/BossRootTask.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/EffectPJSLauncher.h"
#include "Game/EnemyCharInfo.h"
#include "Game/HeavyDogCtrlUnit.h"
#include "Game/WeaponBullet.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Effect::System::EffectElement;
using Effect::System::EffectLaunchInfo;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

/* The RenderWare types used here, declared locally (layouts as in rw/rwcore.h and rw/rpworld.h,
 * reduced to the members used; as in HintRing.cpp). */
extern "C" {
typedef struct RwRGBA {
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
} RwRGBA;

typedef struct RwObject {
    u8 type;
    u8 subKind;
    u8 flags;
    u8 privateFlags;
    void *parent;
} RwObject;

typedef struct RwFrame {
    /* 0x00 */ RwObject object;
    /* 0x08 */ u8 inDirtyListLink[0x8];
    /* 0x10 */ u8 modelling[0x30]; /* the frame's matrix: its axes, */
    /* 0x40 */ Vec3f position; /* and its position */
} RwFrame;

typedef struct RpClump {
    /* 0x00 */ RwObject object; /* the parent is the clump's frame */
} RpClump;
}

/* Target (class name from RTTI): a point characters aim at (as in ArkCannon.cpp). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);
};

/* The effect manager fn_800A31F0 returns (as in DevilDoomCtrlUnit.cpp); its third virtual
 * function launches effect id from a launch description. */
class UnkEffectManager_800A31F0 {
  public:
    virtual void vf08();
    virtual boost::weak_ptr<EffectElement> LaunchId(s32 id, s32 arg);
    virtual boost::weak_ptr<EffectElement> Launch(boost::shared_ptr<EffectLaunchInfo> info, s32 id);
};

/* The boss's effect launcher (BossVarParam::m_effects). Retail converts it to the launch
 * parameter through a converting constructor of its own (a local copy at 0x8026C784, as every boss
 * unit has), so here it is read as the PJS launcher it holds (element type inferred). */
inline const boost::shared_ptr<Effect::PJS::Launcher> &BossEffects(BossVarParam *param) {
    return *reinterpret_cast<const boost::shared_ptr<Effect::PJS::Launcher> *>(&param->m_effects);
}

/* The game state (struct name unknown; see DevilDoomCtrlUnit.cpp): the frame time at 0x24. */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x24];
    /* 0x024 */ float m_dt;
};
extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The world of each stage area (layout as in DevilDoomCtrlUnit.cpp). */
struct UnkWorldEntry_8051D208 {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ void *m_world;
    /* 0x14 */ u8 field_0x14[0x18 - 0x14];
};
extern UnkWorldEntry_8051D208 lbl_8051D208[];

extern "C" {
/* Effects (see DevilDoomCtrlUnit.cpp); fn_800A4210 removes a stopped effect (behaviour
 * inferred). */
UnkEffectManager_800A31F0 *fn_800A31F0(void);
void fn_800A4210(const boost::shared_ptr<EffectElement> &effect);

/* RenderWare clumps and frames (see DevilDoomCtrlUnit.cpp and ElecFan.cpp). */
void *fn_8046256C(RpClump *clump);
void fn_804623EC(void *world, RpClump *clump);
void fn_804622FC(void *world, RpClump *clump);
RpClump *fn_80457090(void *data);
void fn_80457468(RpClump *clump);
void *fn_80169364(RpClump *clump);
int fn_80169350(void *frame);
void fn_8003A840(int area);
RwFrame *fn_8048722C(RwFrame *frame);
void fn_80037B84(RpClump *clump, const RwRGBA *color);

/* The resource loader (see DevilDoomCtrlUnit.cpp). */
void *fn_800122D4(void);
void *fn_8001277C(void *loader, s32 slot, int arg2);

/* Sounds (see DevilDoomCtrlUnit.cpp). */
void *fn_80053164(void);
void fn_801D54F4(void *manager, int id, const void *source, int arg3, int arg4);
void fn_801D53C4(void *manager, int id, const Vec3f *position, void *source, int arg4,
                 int arg5, int arg6);
/* An explosion at position (behaviour inferred; see BAMine.cpp). */
void fn_801DD0BC(const Vec3f *position, int arg1, float arg2, int arg3);

/* Characters, colliders and targets (see DevilDoomCtrlUnit.cpp and ArkCannon.cpp). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7EA8(void *manager, const Vec3f *position, u32 flags,
                                              int arg3, int arg4, const boost::weak_ptr<CharInfo> &owner,
                                              const Vec3f *offset, float radius);
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager, boost::shared_ptr<CharaColliBase> collider);
boost::shared_ptr<Target> fn_801E7128(const boost::weak_ptr<CharInfo> &owner, int flags);
/* The character info of a boss (as in DevilDoomModeCtrlUnit.cpp). */
EnemyCharInfo *fn_801792D0(EnemyCharInfo *const *body);

/* Vector math (behaviour inferred). */
Fn8040CopyVector3 fn_8040CDA8(const Vec3f *vector, float scale);
Fn8040CopyVector3 fn_800091A4(const Vec3f *vector, float scale);
void fn_8040CE04(Vec3f *vector, const Vec3f *value); /* -= */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* += */
}

/* fn_8040CD74 called as returning a - b by value (as in EggLastMech.cpp). */
typedef Fn8040CopyVector3 (*CopyVectorSubtractFn)(const Vec3f *a, const Vec3f *b);

#define Subtract(a, b) ((CopyVectorSubtractFn)fn_8040CD74)(&(a), &(b))

#define Ptr(vector) ((const Vec3f *)&(vector))

/* Puts clump in world (name inferred; as in DevilDoomCtrlUnit.cpp). */
inline void SetClumpWorld(RpClump *clump, void *world) {
    void *current = fn_8046256C(clump);
    if (current != world) {
        if (current != 0) {
            fn_804623EC(current, clump);
        }
        fn_804622FC(world, clump);
    }
}

/* boost 1.32's reset, which the trimmed shared_ptr.hpp declares out of line: this unit holds the
 * first copy of the instance for BossCharInfo. */
template <> inline void boost::shared_ptr<BossCharInfo>::reset() {
    this_type().swap(*this);
}

/* The frame time the effects follow (as in DevilDoomCtrlUnit.cpp). */
#define FrameTime() ((u32)&lbl_8057D6A0.m_dt)

class HeavyDog0410CtrlUnit_Dust : public HeavyDog0410CtrlUnit {
  public:
    HeavyDog0410CtrlUnit_Dust(HeavyDog0410VarParam *param);
    virtual ~HeavyDog0410CtrlUnit_Dust();
    virtual int Tick(float dt);

    /* 0x10 */ boost::weak_ptr<EffectElement> m_effect;
};

/* A mine the boss drops: it flies to its target, waits, blinks and explodes (behaviour
 * inferred). */
class HeavyDog0410CtrlUnit_Mine : public HeavyDog0410CtrlUnit, public AttackCallbackContainer {
  public:
    HeavyDog0410CtrlUnit_Mine(HeavyDog0410VarParam *param, const Vec3f &position,
                              const Vec3f &velocity, const Vec3f &target);
    virtual ~HeavyDog0410CtrlUnit_Mine();
    virtual int Tick(float dt);
    virtual void OnDamage(AttackCallbackParam &param);

    /* 0x14 */ RpClump *m_clumps[2];
    /* 0x1C */ RwFrame *m_frames[2];
    /* 0x24 */ s32 m_state; /* 0 waiting, 1 hit, 2 blinking, 3 exploding, 4 exploded */
    /* 0x28 */ float m_time;
    /* 0x2C */ Vec3f m_position;
    /* 0x38 */ Vec3f m_velocity;
    /* 0x44 */ Vec3f m_target;
    /* 0x50 */ boost::shared_ptr<BossCharInfo> m_chara;
    /* 0x58 */ boost::shared_ptr<Target> m_targetPoint;
    /* 0x60 */ boost::shared_ptr<CharaColliBase> m_collider;
};

class HeavyDog0410CtrlUnit_Move : public HeavyDog0410CtrlUnit {
  public:
    HeavyDog0410CtrlUnit_Move(HeavyDog0410VarParam *param);
    virtual ~HeavyDog0410CtrlUnit_Move();
    virtual int Tick(float dt);
};

HeavyDog0410CtrlUnit_Move::HeavyDog0410CtrlUnit_Move(HeavyDog0410VarParam *param)
    : BossCtrlUnit(param), HeavyDog0410CtrlUnit(param) {
    m_kind = 2;
}

HeavyDog0410CtrlUnit_Move::~HeavyDog0410CtrlUnit_Move() {}

int HeavyDog0410CtrlUnit_Move::Tick(float dt) {
    if (!m_owner->m_fixed) {
        Fn8040CopyVector3 offset = Subtract(m_owner->m_targetPosition, m_owner->m_chara->m_position);
        fn_800091D0(&m_owner->m_acceleration, Ptr(fn_8040CDA8(Ptr(offset), m_owner->m_spring)));
        fn_8040CE04(&m_owner->m_acceleration, Ptr(fn_800091A4(&m_owner->m_velocity, m_owner->m_damping)));
        fn_8040CFF8(&m_owner->m_velocity, Ptr(fn_800091A4(&m_owner->m_acceleration, dt)));
        fn_8040CFF8(&fn_801792D0(&m_owner->m_chara)->m_position,
                    Ptr(fn_800091A4(&m_owner->m_velocity, dt)));
        m_owner->m_distance += fn_8040CF50(Ptr(fn_8040CDA8(&m_owner->m_velocity, dt)));
    } else {
        fn_800091D0(&m_owner->m_velocity, &lbl_805E07A0);
        fn_800091D0(&m_owner->m_acceleration, &lbl_805E07A0);
    }
    m_owner->m_targetDistance =
        fn_8040CF50(Ptr(Subtract(m_owner->m_targetPosition, m_owner->m_chara->m_position)));
    return false;
}

HeavyDog0410CtrlUnit_Mine::HeavyDog0410CtrlUnit_Mine(HeavyDog0410VarParam *param,
                                                     const Vec3f &position,
                                                     const Vec3f &velocity,
                                                     const Vec3f &target)
    : BossCtrlUnit(param), HeavyDog0410CtrlUnit(param) {
    fn_800091D0(&m_position, &position);
    fn_800091D0(&m_target, &target);
    fn_800091D0(&m_velocity, &velocity);
    m_state = 0;
    m_time = 0.0f;

    void *data = fn_8001277C(fn_800122D4(), 0x16, 8);
    if (data != 0) {
        m_clumps[0] = fn_80457090(data);
    }
    data = fn_8001277C(fn_800122D4(), 0x16, 9);
    if (data != 0) {
        m_clumps[1] = fn_80457090(data);
    }
    for (int i = 0; i < 2; i++) {
        if (m_clumps[i] != 0) {
            int area = fn_80169350(fn_80169364(m_clumps[i]));
            fn_8003A840(area);
            SetClumpWorld(m_clumps[i], lbl_8051D208[area].m_world);
            m_frames[i] = (RwFrame *)m_clumps[i]->object.parent;
            if (m_frames[i] != 0) {
                m_frames[i]->position = m_position;
                fn_8048722C(m_frames[i]);
            }
        }
    }

    m_chara = boost::shared_ptr<BossCharInfo>(new BossCharInfo(0xA9));
    void *manager = fn_80021B08();
    m_collider = fn_800C7EA8(manager, &m_position, 0x80, 4, 0, m_chara, &lbl_805E07A0, 0.25f);
    if (m_collider) {
        m_collider->SetField2C(1);
        boost::weak_ptr<CharaColliAttack> attack = fn_800C4374(manager, m_collider);
        boost::shared_ptr<CharaColliAttack> collision(attack.lock());
        if (collision) {
            collision->field_0x54 = 0;
            *reinterpret_cast<u32 *>(collision->field_0x3C) = 1;
            collision->field_0x55 = 1;
            collision->m_attackCallback = GetDamageCallback();
        }
    }
    m_targetPoint = fn_801E7128(m_chara, 0x2C00);
}

HeavyDog0410CtrlUnit_Mine::~HeavyDog0410CtrlUnit_Mine() {
    for (int i = 0; i < 2; i++) {
        if (m_clumps[i] != 0) {
            void *world = fn_8046256C(m_clumps[i]);
            if (world != 0) {
                fn_804623EC(world, m_clumps[i]);
            }
            fn_80457468(m_clumps[i]);
            m_clumps[i] = 0;
            m_frames[i] = 0;
        }
    }
    m_targetPoint.reset();
    m_collider.reset();
    m_chara.reset();
}

int HeavyDog0410CtrlUnit_Mine::Tick(float dt) {
    bool exploded = false;
    m_time += dt;
    RwRGBA color = {0xFF, 0xFF, 0xFF, 0xFF};
    switch (m_state) {
    case 0:
        if (m_time > m_owner->m_mineWaitTime) {
            m_state = 2;
            m_time = 0.0f;
        }
        break;
    case 1:
        if (m_targetPoint) {
            m_targetPoint.reset();
        }
    case 2:
        if (m_time > m_owner->m_mineBlinkTime) {
            m_state = 3;
            m_time = 0.0f;
        } else if ((int)(m_time / 0.1f) % 2 == 0) {
            if ((int)((m_time - dt) / 0.1f) % 2 != 0) {
                fn_801D54F4(fn_80053164(), 0x7D38, &m_position, 0, 1);
            }
            color.alpha = 0;
        }
        break;
    case 3:
        fn_801DD0BC(&m_position, 0, 1.0f, 1);
        if (m_targetPoint) {
            m_targetPoint.reset();
        }
        if (m_collider) {
            m_collider.reset();
        }
        m_state = 4;
        break;
    case 4:
        color.alpha = 0;
        if (m_time > m_owner->m_mineExplodeTime) {
            return true;
        }
        exploded = true;
        break;
    }
    int i;
    for (i = 0; i < 2; i++) {
        if (m_clumps[i] != 0) {
            fn_80037B84(m_clumps[i], &color);
        }
    }
    if (!exploded) {
        Fn8040CopyVector3 offset = Subtract(m_target, m_position);
        Fn8040CopyVector3 acceleration = fn_8040CDA8(Ptr(offset), m_owner->m_mineSpring);
        fn_8040CE04((Vec3f *)&acceleration, Ptr(fn_800091A4(&m_velocity, m_owner->m_mineDamping)));
        fn_8040CFF8(&m_velocity, Ptr(fn_800091A4(Ptr(acceleration), dt)));
        fn_8040CFF8(&m_position, Ptr(fn_800091A4(&m_velocity, dt)));
    }
    for (i = 0; i < 2; i++) {
        if (m_frames[i] != 0) {
            m_frames[i]->position = m_position;
            fn_8048722C(m_frames[i]);
        }
    }
    if (m_targetPoint) {
        m_targetPoint->fn_800CCE38(&m_position);
    }
    if (m_collider) {
        /* Slot 0x3C, called with the mine's position (a character info starts with its
         * position). */
        m_collider->SetCharInfo((CharInfo *)&m_position);
    }
    return false;
}

void HeavyDog0410CtrlUnit_Mine::OnDamage(AttackCallbackParam &param) {
    param.m_result = 1;
    boost::shared_ptr<CharaColliAttack> attacker(param.m_attacker.lock());
    if (attacker->m_flags & 8) {
        if (m_state == 0) {
            m_state = 1;
        } else {
            m_state = 3;
        }
    } else if ((attacker->m_flags & 7) == 0) {
        m_state = 3;
    }
    m_time = 0.0f;
}

HeavyDog0410CtrlUnit_Dust::HeavyDog0410CtrlUnit_Dust(HeavyDog0410VarParam *param)
    : BossCtrlUnit(param), HeavyDog0410CtrlUnit(param) {
    m_kind = 3;
    m_effect = fn_800A31F0()->Launch(BossEffects(m_owner), 0x16);
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        effect->SetField4C((u32)fn_801792D0(&m_owner->m_chara));
        effect->SetField14(FrameTime());
    }
}

HeavyDog0410CtrlUnit_Dust::~HeavyDog0410CtrlUnit_Dust() {
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        effect->Stop();
        fn_800A4210(effect);
    }
}

int HeavyDog0410CtrlUnit_Dust::Tick(float dt) {
    fn_801D53C4(fn_80053164(), 0x7D3C, &m_owner->m_chara->m_position, this, 3, 0, 3);
    return false;
}
