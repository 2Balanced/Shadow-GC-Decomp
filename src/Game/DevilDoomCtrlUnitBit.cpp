/* DevilDoomCtrlUnit_BitLaser, DevilDoomCtrlUnit_Bit, DevilDoomCtrlUnit_BitGroupControl and
 * DevilDoomCtrlUnit_BitAttackControl (class names from RTTI): the Devil Doom boss's bits, the eight
 * small pods each of its three rings (the parts at DevilDoomVarParam 0x214) carries. A group control
 * unit makes its ring's bits and turns, scales and places the ring each frame; each bit follows
 * its place in the ring, waits for the attack the boss's bit mode (DevilDoomVarParam 0x628) asks
 * for, charges and fires a laser (a BitLaser) at the player, down, or outward, and can be shot
 * down. The attack control unit runs the bit modes' timing. File name inferred from the classes;
 * names marked "(name inferred)" and all member names are not original. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include <boost/function.hpp>

#include "Effect/System/EffectElement.h"
#include "Game/BossCtrlUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/DevilDoomCtrlUnit.h"
#include "Game/EffectPJSLauncher.h"
#include "Game/EnemyCharInfo.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

namespace std {
float cos(float x);
} // namespace std

using Effect::System::EffectElement;
using Effect::System::EffectLaunchInfo;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

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

/* The game state (struct name unknown; see DevilDoomCtrlUnit.cpp): the frame time at 0x24. */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x24];
    /* 0x024 */ float m_dt;
};
extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The frame time the effects follow (as in DevilDoomCtrlUnit.cpp). */
#define FrameTime() ((u32)&lbl_8057D6A0.m_dt)

/* The save data (see EggLastMech.cpp): the record at 0x70C keeps the score. */
struct UnkObject_80079808 {
    /* 0x000 */ u8 field_0x0[0x70C];
    /* 0x70C */ u8 m_score[1];
};

/* The null owner of colliders, and pi. */
extern boost::shared_ptr<CharInfo> lbl_805EE698;
extern const Vec3f lbl_805E07B8;
extern const float lbl_805FAD38;

extern "C" {
/* Effects (see DevilDoomCtrlUnit.cpp and GoalRing.cpp). */
UnkEffectManager_800A31F0 *fn_800A31F0(void);
void fn_80425E68(EffectElement *effect, const Mtx34f *matrix);
void fn_80425E4C(EffectElement *effect, const Vec3f *position);

/* Motions (see DevilDoomCtrlUnit.cpp). */
Mtx34f *fn_80415E70(_stMtnPlayWork *work, int id);
void fn_80016370(Mtx34f *matrix, const Mtx34f *frame);

/* Sounds (see DevilDoomCtrlUnit.cpp). */
void *fn_80053164(void);
void fn_801D54F4(void *manager, int id, const void *source, int arg3, int arg4);
void fn_801D53C4(void *manager, int id, const Vec3f *position, void *source, int arg4,
                 int arg5, int arg6);

/* The score (see EggLastMech.cpp). */
UnkObject_80079808 *fn_80079808(void);
void fn_8016F0CC(void *score, int arg1, int points);

/* Colliders and targets (see DevilDoomCtrlUnit.cpp and BalloonGenerator.cpp). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7EA8(void *manager, const Vec3f *position, u32 flags,
                                              int arg3, int arg4, const boost::weak_ptr<CharInfo> &owner,
                                              const Vec3f *offset, float radius);
boost::shared_ptr<CharaColliBase> fn_800C799C(void *manager, const Vec3f *position,
                                              const Vec3f *arg2, int arg3, int arg4, int arg5,
                                              int arg6, const boost::weak_ptr<CharInfo> &owner,
                                              const Vec3f *offset, const Vec3f *rotation,
                                              int arg10, float arg11, float arg12);
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager, boost::shared_ptr<CharaColliBase> collider);
boost::shared_ptr<Target> fn_801E72DC(int flags);

/* Math (behaviour inferred). */
float fn_8040EE90(void); /* a random value in [0, 1) */
float fn_8040CF90(const Vec3f *a, const Vec3f *b);
float fn_8040CE74(Vec3f *vector);
void fn_8040CE04(Vec3f *vector, const Vec3f *value); /* -= */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* += */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CEE8(const Vec3f *vector);
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b);
Vec3f fn_80093230(const Vec3f *vector, float dt);
Vec3f fn_8040FB08(const Mtx34f *matrix); /* its translation */
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Mtx34f fn_8040BEF8(const Mtx34f *matrix);
Mtx34f fn_8040EFA4(const Mtx34f *a, const Mtx34f *b, float t);
Mtx34f fn_8001E044(const Mtx34f *a, const Mtx34f *b);
void fn_8040B52C(Mtx34f *matrix, float x, float y, float z);
void fn_8040B790(Mtx34f *matrix, const Vec3f *scale);
void fn_8040FA20(Mtx34f *matrix, const Vec3f *translation);
void fn_80049290(Vec3f *rotation, const Mtx34f *matrix);
}

/* fn_8040CD74 called as returning a - b by value (see ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* Passes a returned vector or matrix on by address (as in DevilDoomCtrlUnit.cpp). */
inline const Vec3f &Ref(const Vec3f &vector) {
    return vector;
}
inline const Mtx34f &RefM(const Mtx34f &matrix) {
    return matrix;
}

/* a - b (name inferred). */
inline Vec3f Sub(const Vec3f &a, const Vec3f &b) {
    return ((VectorSubtractFn)fn_8040CD74)(&a, &b);
}

/* Degrees to radians (name inferred). */
inline float ToRadian(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* The parts of AttackCallbackParam OnHit reads (as in DevilDoomCtrlUnit.cpp). */
struct DevilDoomAttackParam {
    /* 0x00 */ boost::weak_ptr<CharaColliAttack> m_attacker;
    /* 0x08 */ u8 field_0x8[0x18 - 0x8];
    /* 0x18 */ Vec3f m_position;
    /* 0x24 */ u8 field_0x24[0x34 - 0x24];
    /* 0x34 */ Vec3f m_offset;
    /* 0x40 */ u8 field_0x40[0x50 - 0x40];
    /* 0x50 */ float m_damage;
    /* 0x54 */ s32 m_result;
};

#define Param(param) (*(DevilDoomAttackParam *)&(param))

/* A matrix assigned through fn_8040C148 (struct name inferred). Retail calls its assignment out of
 * line where the result is assigned on. */
struct UnkMatrix_80325384 : public Mtx34f {
    UnkMatrix_80325384 &operator=(const Mtx34f &matrix) {
        fn_8040C148(this, &matrix);
        return *this;
    }
    UnkMatrix_80325384 &operator=(const UnkMatrix_80325384 &matrix) {
        fn_8040C148(this, &matrix);
        return *this;
    }
};

/* A vector of the ring parts, which keep them as plain floats (name inferred). */
#define V3(array) (reinterpret_cast<Vec3f *>(array))

/* The rings' heights and radii (names inferred). */
static const float s_heights[3] = {-40.0f, 20.0f, 80.0f};
static const float s_radii[3] = {180.0f, 200.0f, 180.0f};
static const float s_closedHeights[3] = {120.0f, 100.0f, 80.0f};

/* A laser a bit fires (behaviour inferred): it grows from its start along its direction, follows
 * the bit while it lives, and fades out at the end. */
class DevilDoomCtrlUnit_BitLaser : public DevilDoomCtrlUnit, public AttackCallbackContainer {
  public:
    DevilDoomCtrlUnit_BitLaser(DevilDoomVarParam *param, const Vec3f *start,
                               const Vec3f *direction, float length);
    DevilDoomCtrlUnit_BitLaser(DevilDoomVarParam *param, const Vec3f *start,
                               const Vec3f *direction, const u8 *dead, float length);
    virtual ~DevilDoomCtrlUnit_BitLaser();
    virtual int Tick(float dt);
    virtual void OnHit(AttackCallbackParam &param);

    void Init();

    /* 0x10 */ u8 m_following; /* follows the bit's start and direction */
    /* 0x11 */ u8 m_ending;
    /* 0x14 */ const u8 *m_dead; /* the bit's flag */
    /* 0x18 */ float m_time;
    /* 0x1C */ float m_maxLength;
    /* 0x20 */ float m_length;
    /* 0x24 */ float m_width;
    /* 0x28 */ Vec3f m_start;
    /* 0x34 */ const Vec3f *m_startSource;
    /* 0x38 */ Vec3f m_direction;
    /* 0x44 */ const Vec3f *m_directionSource;
    /* 0x48 */ Vec3f m_soundPosition;
    /* 0x54 */ UnkMatrix_80325384 m_matrix;
    /* 0x84 */ UnkMatrix_80325384 m_startMatrix;
    /* 0xB4 */ boost::weak_ptr<EffectElement> m_effect;
    /* 0xBC */ boost::weak_ptr<EffectElement> m_startEffect;
    /* 0xC4 */ boost::shared_ptr<CharaColliBase> m_collider;
};
SIZE_ASSERT(DevilDoomCtrlUnit_BitLaser, 0xDC);

class DevilDoomCtrlUnit_Bit : public DevilDoomCtrlUnit, public AttackCallbackContainer {
  public:
    DevilDoomCtrlUnit_Bit(DevilDoomVarParam *param, int group, int index);
    virtual ~DevilDoomCtrlUnit_Bit();
    virtual int Tick(float dt);
    virtual void OnDamage(AttackCallbackParam &param);

    /* Names inferred. */
    bool Appear();
    void StartCharge();
    void EndCharge();
    void Disappear();
    void Vanish();

    /* The bit's place in its ring (name inferred). */
    UnkDevilDoomPartElement *Place() {
        UnkDevilDoomPart *parts = m_owner->m_parts;
        return &parts[m_group].m_places[m_index];
    }

    /* 0x10 */ s32 m_group;
    /* 0x14 */ s32 m_index;
    /* 0x18 */ float m_health;
    /* 0x1C */ float m_time;
    /* 0x20 */ Mtx34f m_matrix;
    /* 0x50 */ Mtx34f m_prevRotation;
    /* 0x80 */ Vec3f m_position;
    /* 0x8C */ Vec3f m_prevPosition;
    /* 0x98 */ Vec3f m_spin;
    /* 0xA4 */ Vec3f m_laserStart;
    /* 0xB0 */ Vec3f m_laserDirection;
    /* 0xBC */ s32 m_state;
    /* 0xC0 */ s32 m_nextState;
    /* 0xC4 */ boost::weak_ptr<EffectElement> m_effect;
    /* 0xCC */ boost::weak_ptr<EffectElement> m_chargeEffect;
    /* 0xD4 */ boost::shared_ptr<CharaColliBase> m_collider;
    /* 0xDC */ boost::shared_ptr<Target> m_target;
    /* 0xE4 */ u8 m_dead;
};
SIZE_ASSERT(DevilDoomCtrlUnit_Bit, 0xF8);

DevilDoomCtrlUnit_BitAttackControl::DevilDoomCtrlUnit_BitAttackControl(DevilDoomVarParam *owner)
    : BossCtrlUnit(owner), DevilDoomCtrlUnit(owner) {
    int g;
    int i;
    UnkDevilDoomPart *group;
    UnkDevilDoomPartElement *places;
    Mtx34f rotation;
    m_kind = 2;
    m_time = 0.0f;
    for (g = 0; g < 3; g++) {
        m_owner->m_parts[g].m_closed = 1;
        for (i = 0; i < 8; i++) {
            m_owner->m_parts[g].m_places[i].m_locked = 1;
        }
    }
    switch (m_owner->field_0x628) {
    case 1:
        break;
    case 2: {
        m_time = m_owner->m_settings.field_0xCC - m_owner->m_settings.field_0x98;
        for (g = 0; g < 3; g++) {
            group = &m_owner->m_parts[g];
            fn_800091D0(V3(group->m_position), &lbl_805E07A0);
            group->m_position[1] = s_closedHeights[g];
            group->m_rotation[2] = 0.0f;
            group->m_rotation[0] = 0.0f;
            fn_800091D0(V3(group->m_scale), &Vec3f(1.0f, 1.0f, 1.0f));
            group->m_blend = 0.0f;
            places = group->m_places;
            fn_8040C148(&rotation, &lbl_805E0720);
            for (i = 0; i < 8; i++) {
                fn_800091D0(V3(places[i].m_position),
                            &Ref(fn_8040B67C(&rotation, &Vec3f(0.0f, 0.0f, s_radii[2]))));
                places[i].m_blend = 0.0f;
                fn_8040B848(&rotation, 2.0f * lbl_805FAD38 / 8.0f);
            }
        }
        break;
    }
    case 3: {
        for (g = 0; g < 3; g++) {
            group = &m_owner->m_parts[g];
            fn_800091D0(V3(group->m_position), &lbl_805E07A0);
            group->m_position[1] = 0.0f;
            group->m_rotation[2] = 0.0f;
            group->m_rotation[0] = 0.0f;
            fn_800091D0(V3(group->m_scale), &Vec3f(1.0f, 1.0f, 1.0f));
            group->m_blend = 0.0f;
            places = group->m_places;
            int turns = (int)(group->m_rotation[1] / (2.0f * lbl_805FAD38 / 8.0f));
            group->m_rotation[1] = 2.0f * lbl_805FAD38 / 8.0f * (turns + g / 3.0f);
            fn_8040C148(&rotation, &lbl_805E0720);
            for (i = 0; i < 8; i++) {
                fn_800091D0(V3(places[i].m_position),
                            &Ref(fn_8040B67C(&rotation, &Vec3f(0.0f, 0.0f, s_radii[1]))));
                places[i].m_blend = 0.0f;
                fn_8040B848(&rotation, 2.0f * lbl_805FAD38 / 8.0f);
            }
        }
        break;
    }
    default:
        m_owner->field_0x19C = 0;
        break;
    }
}

DevilDoomCtrlUnit_BitAttackControl::~DevilDoomCtrlUnit_BitAttackControl() {
    for (int g = 0; g < 3; g++) {
        m_owner->m_parts[g].m_closed = 0;
        for (int i = 0; i < 8; i++) {
            m_owner->m_parts[g].m_places[i].m_locked = 0;
            m_owner->m_parts[g].m_places[i].m_done = 0;
        }
    }
    if (m_owner->field_0x19C == 2) {
        m_owner->field_0x19C = 0;
    }
    m_owner->field_0x628 = 0;
}

int DevilDoomCtrlUnit_BitAttackControl::Tick(float dt) {
    int g;
    if (m_owner->field_0x19C != 2) {
        ResetGroups();
        return false;
    }
    if (!m_owner->m_active) {
        ResetGroups();
        return false;
    }
    m_time += dt;
    switch (m_owner->field_0x628) {
    case 1:
        for (g = 0; g < 3; g++) {
            if (m_owner->m_parts[g].m_closed) {
                m_owner->m_parts[g].m_closed = 0;
            }
        }
        if (m_time > m_owner->m_settings.field_0xB8) {
            ResetGroups();
        }
        break;
    case 2:
        fn_801D53C4(fn_80053164(), 0x8146, 0, this, 2, 0, 3);
        if (m_time > m_owner->m_settings.field_0xCC) {
            for (g = 2; g >= 0; g--) {
                if (m_owner->m_parts[g].m_closed) {
                    m_owner->m_parts[g].m_closed = 0;
                    break;
                }
            }
            if (g >= 0) {
                if (!m_owner->m_parts[g].m_closed) {
                    m_time = 0.0f;
                }
            } else {
                int k;
                bool done = true;
                for (k = 0; k < 3; k++) {
                    for (int i = 0; i < 8; i++) {
                        if (!m_owner->m_parts[k].m_places[i].m_done) {
                            done = false;
                            break;
                        }
                    }
                    if (!done) {
                        break;
                    }
                }
                if (done) {
                    ResetGroups();
                }
            }
        }
        break;
    case 3:
        if (m_time > m_owner->m_settings.field_0x98) {
            for (g = 0; g < 3; g++) {
                if (m_owner->m_parts[g].m_closed) {
                    m_owner->m_parts[g].m_closed = 0;
                }
            }
        }
        if (m_time > m_owner->m_settings.field_0xD4) {
            ResetGroups();
        }
        break;
    case 4:
        if (m_time > m_owner->m_settings.field_0x98) {
            return true;
        }
        break;
    }
    return false;
}

void DevilDoomCtrlUnit_BitAttackControl::ResetGroups() {
    for (int g = 0; g < 3; g++) {
        int i;
        UnkDevilDoomPart *group = &m_owner->m_parts[g];
        fn_800091D0(V3(group->m_position), &lbl_805E07A0);
        group->m_position[1] = s_heights[g];
        group->m_rotation[2] = 0.0f;
        group->m_rotation[0] = 0.0f;
        fn_800091D0(V3(group->m_scale), &Vec3f(1.0f, 1.0f, 1.0f));
        group->m_blend = 0.0f;
        group->m_closed = 0;
        UnkDevilDoomPartElement *places = group->m_places;
        Mtx34f rotation;
        fn_8040C148(&rotation, &lbl_805E0720);
        for (i = 0; i < 8; i++) {
            fn_800091D0(V3(places[i].m_position), &Ref(fn_8040B67C(&rotation, &Vec3f(0.0f, 0.0f, s_radii[g]))));
            places[i].m_blend = 0.0f;
            places[i].m_free = 1;
            fn_8040B848(&rotation, 2.0f * lbl_805FAD38 / 8.0f);
        }
    }
    m_owner->field_0x628 = 4;
    m_time = 0.0f;
}

DevilDoomCtrlUnit_BitGroupControl::DevilDoomCtrlUnit_BitGroupControl(DevilDoomVarParam *owner, int index)
    : BossCtrlUnit(owner), DevilDoomCtrlUnit(owner) {
    m_kind = 2;
    m_group = index;
    m_created = 0;
    UnkDevilDoomPart *group = &m_owner->m_parts[m_group];
    group->m_blend = 1.0f;
    fn_800091D0(V3(group->m_position), fn_800091D0(V3(group->m_rotation), &lbl_805E07A0));
    fn_800091D0(V3(group->m_scale), &Vec3f(1.0f, 1.0f, 1.0f));
    group->m_position[1] = s_heights[m_group];
    group->m_closed = 0;
    UnkDevilDoomPartElement *places = group->m_places;
    Mtx34f rotation;
    fn_8040C148(&rotation, &lbl_805E0720);
    for (int i = 0; i < 8; i++) {
        UnkDevilDoomPartElement *place = &places[i];
        fn_800091D0(V3(place->m_position), &Ref(fn_8040B67C(&rotation, &Vec3f(0.0f, 0.0f, s_radii[m_group]))));
        places[i].m_blend = 0.0f;
        fn_8040B848(&rotation, 2.0f * lbl_805FAD38 / 8.0f);
    }
}

DevilDoomCtrlUnit_BitGroupControl::~DevilDoomCtrlUnit_BitGroupControl() {}

int DevilDoomCtrlUnit_BitGroupControl::Tick(float dt) {
    UnkDevilDoomPart *group = &m_owner->m_parts[m_group];
    m_time += dt;
    if (!m_created) {
        for (int i = 0; i < 8; i++) {
            group->m_places[i].m_blend = 1.0f;
            fn_803358D0(m_owner, new DevilDoomCtrlUnit_Bit(m_owner, m_group, i));
        }
        m_created = 1;
    }
    switch (m_owner->field_0x628) {
    default: {
        float speed = ToRadian(m_owner->m_settings.field_0x8C);
        group->m_rotation[1] += (m_group % 2 != 0 ? -1.0f : 1.0f) * (dt * speed);
        if (group->m_closed) {
            for (int i = 0; i < 8; i++) {
                group->m_places[i].m_locked = 1;
            }
        } else {
            for (int i = 0; i < 8; i++) {
                group->m_places[i].m_locked = 0;
            }
        }
        break;
    }
    case 1: {
        float speed = ToRadian(m_owner->m_settings.field_0xBC);
        group->m_rotation[1] += (m_group % 2 != 0 ? -1.0f : 1.0f) * (dt * speed);
        if (group->m_closed) {
            for (int i = 0; i < 8; i++) {
                group->m_places[i].m_locked = 1;
            }
        } else {
            for (int i = 0; i < 8; i++) {
                group->m_places[i].m_locked = 0;
            }
        }
        break;
    }
    case 2: {
        float speed = ToRadian(m_owner->m_settings.field_0xC4);
        group->m_rotation[1] += (m_group % 2 != 0 ? -1.0f : 1.0f) * (dt * speed);
        if (group->m_closed) {
            m_time = 0.0f;
            for (int i = 0; i < 8; i++) {
                group->m_places[i].m_locked = 1;
            }
        } else {
            for (int i = 0; i < 8; i++) {
                group->m_places[i].m_locked = 0;
            }
        }
        if (m_time > m_owner->m_settings.field_0x9C + m_owner->m_settings.field_0x98) {
            group->m_scale[0] += dt * m_owner->m_settings.field_0xC8;
            group->m_scale[2] += dt * m_owner->m_settings.field_0xC8;
        }
        break;
    }
    case 3:
        group->m_rotation[1] += dt * ToRadian(m_owner->m_settings.field_0xD8);
        if (group->m_closed) {
            m_time = m_owner->m_settings.field_0xDC;
            for (int i = 0; i < 8; i++) {
                group->m_places[i].m_locked = 1;
            }
        } else if (m_time > m_owner->m_settings.field_0xDC) {
            int count = 0;
            for (int i = 0; i < 8; i++) {
                if (group->m_places[i].m_locked) {
                    count++;
                }
            }
            int left = m_owner->m_settings.field_0xE4;
            while (count != 0 && left != 0) {
                int index = (int)(count * fn_8040EE90());
                for (int i = 0; i < 8; i++) {
                    if (group->m_places[i].m_locked) {
                        if (index == 0) {
                            group->m_places[i].m_locked = 0;
                            break;
                        }
                        index--;
                    }
                }
                count--;
                left--;
            }
            m_time = 0.0f;
        }
        break;
    }
    float blend = group->m_blend;
    if (blend < 0.0001) {
        fn_800091D0(&m_prevPosition, &m_position);
        fn_8040C148(&m_prevRotation, &m_rotation);
        fn_800091D0(&m_prevScale, &m_scale);
    }
    blend += dt / m_owner->m_settings.field_0x98;
    if (blend > 1.0f) {
        blend = 1.0f;
    }
    Vec3f position;
    fn_800091EC(&position, V3(group->m_position));
    if (blend < 1.0f) {
        fn_800091D0(&position, &Ref(fn_8040CDD0(&Ref(fn_8040CDA8(&position, blend)),
                                                &Ref(fn_8040CDA8(&m_prevPosition, 1.0f - blend)))));
    }
    fn_800091D0(&m_position, &position);
    Mtx34f rotation;
    fn_8040C148(&rotation, &lbl_805E0720);
    fn_8040B80C(&rotation, group->m_rotation[2]);
    fn_8040B848(&rotation, group->m_rotation[1]);
    fn_8040B884(&rotation, group->m_rotation[0]);
    if (blend < 1.0f) {
        fn_8040C148(&rotation, &RefM(fn_8040EFA4(&m_prevRotation, &rotation, blend)));
    }
    fn_8040C148(&m_rotation, &rotation);
    Vec3f scale;
    fn_800091EC(&scale, V3(group->m_scale));
    if (blend < 1.0f) {
        fn_800091D0(&scale, &Ref(fn_8040CDD0(&Ref(fn_8040CDA8(&scale, blend)),
                                             &Ref(fn_8040CDA8(&m_prevScale, 1.0f - blend)))));
    }
    fn_800091D0(&m_scale, &scale);
    fn_8040C148(&group->m_matrix, &lbl_805E0720);
    fn_8040B59C(&group->m_matrix, &Ref(fn_8040CDD0(&m_owner->m_chara->m_position, &position)));
    fn_8040C148(&group->m_matrix, &RefM(fn_8001E044(&group->m_matrix, &rotation)));
    fn_8040B790(&group->m_matrix, &scale);
    fn_800091D0(&m_worldPosition, &Ref(fn_8040FB08(&group->m_matrix)));
    group->m_blend = blend;
    return false;
}

DevilDoomCtrlUnit_Bit::DevilDoomCtrlUnit_Bit(DevilDoomVarParam *param, int group, int index)
    : BossCtrlUnit(param), DevilDoomCtrlUnit(param) {
    m_kind = 2;
    m_group = group;
    m_index = index;
    m_state = 1;
}

DevilDoomCtrlUnit_Bit::~DevilDoomCtrlUnit_Bit() {
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        effect->Stop();
    }
    effect = m_chargeEffect.lock();
    if (effect) {
        effect->Stop();
    }
    m_dead = 1;
    if (m_collider) {
        m_collider.reset();
    }
    if (m_target) {
        m_target.reset();
    }
}

int DevilDoomCtrlUnit_Bit::Tick(float dt) {
    if (m_owner->m_health <= 0.0f) {
        return true;
    }
    UnkDevilDoomPartElement *place = &m_owner->m_parts[m_group].m_places[m_index];
    m_time += dt;
    switch (m_state) {
    case 1:
        if (Appear()) {
            m_state = 2;
        }
        break;
    case 2:
        m_nextState = 0;
        place->m_free = 0;
        if (place->m_locked) {
            break;
        }
        if (!m_owner->m_active) {
            break;
        }
        switch (m_owner->field_0x628) {
        case 1: {
            if (m_time < m_owner->m_settings.field_0x94) {
                break;
            }
            Vec3f toBit;
            fn_800091EC(&toBit, &Ref(Sub(fn_8040FB08(&m_matrix), m_owner->m_chara->m_position)));
            Vec3f toTarget;
            fn_800091EC(&toTarget, &Ref(((VectorSubtractFn)fn_8040CD74)(&m_owner->m_target,
                                                                       &m_owner->m_chara->m_position)));
            float limit = std::cos(ToRadian(45.0f));
            toTarget.y = 0.0f;
            toBit.y = 0.0f;
            if (fn_8040CF90(&Ref(fn_8040CEE8(&toBit)), &Ref(fn_8040CEE8(&toTarget))) > limit) {
                StartCharge();
                m_state = 3;
                m_nextState = 5;
                m_time = 0.0f;
                fn_800091D0(&m_laserDirection, &m_owner->m_target);
            }
            break;
        }
        case 2:
            StartCharge();
            m_state = 3;
            m_nextState = 5;
            m_time = 0.0f;
            break;
        case 3: {
            Vec3f toBit;
            fn_800091EC(&toBit, &Ref(Sub(fn_8040FB08(&m_matrix), m_owner->m_chara->m_position)));
            Vec3f toTarget;
            fn_800091EC(&toTarget, &Ref(((VectorSubtractFn)fn_8040CD74)(&m_owner->m_target,
                                                                       &m_owner->m_chara->m_position)));
            toTarget.y = 0.0f;
            toBit.y = 0.0f;
            if (fn_8040CF90(&Ref(fn_8040CEE8(&toBit)), &Ref(fn_8040CEE8(&toTarget))) > 0.0f) {
                StartCharge();
                m_state = 3;
                m_nextState = 5;
                m_time = 0.0f;
            } else {
                place->m_locked = 1;
            }
            break;
        }
        default: {
            if (m_time < m_owner->m_settings.field_0x94) {
                break;
            }
            Vec3f offset;
            fn_800091EC(&offset, &Ref(((VectorSubtractFn)fn_8040CD74)(&m_owner->m_target,
                                                                     &Ref(fn_8040FB08(&m_matrix)))));
            if (fn_8040CF50(&offset) < m_owner->m_settings.field_0xAC) {
                StartCharge();
                m_state = 3;
                m_nextState = 5;
                m_time = 0.0f;
                fn_800091D0(&m_laserDirection, &m_owner->m_target);
            }
            break;
        }
        }
        break;
    case 3:
        if (place->m_free || !m_owner->m_active) {
            m_state = m_nextState;
            break;
        }
        if (m_time < m_owner->m_settings.field_0x9C) {
            break;
        }
        switch (m_owner->field_0x628) {
        case 1:
        default:
            fn_800091D0(&m_laserStart, &Ref(fn_8040FB08(&m_matrix)));
            fn_8040CE04(&m_laserDirection, &m_laserStart);
            fn_803358D0(m_owner, new DevilDoomCtrlUnit_BitLaser(m_owner, &m_laserStart, &m_laserDirection,
                                                                m_owner->m_settings.field_0xC0));
            m_state = 5;
            break;
        case 2:
            fn_800091D0(&m_laserStart, &Ref(fn_8040FB08(&m_matrix)));
            fn_800091D0(&m_laserDirection, &Vec3f(0.0f, -1.0f, 0.0f));
            if (m_owner->m_parts[m_group].m_position[1] < 0.0f) {
                m_laserDirection.y = 1.0f;
            }
            m_dead = 0;
            fn_803358D0(m_owner, new DevilDoomCtrlUnit_BitLaser(m_owner, &m_laserStart, &m_laserDirection,
                                                                &m_dead, m_owner->m_settings.field_0xD0));
            m_state = 4;
            m_nextState = 8;
            m_time = 0.0f;
            break;
        case 3:
            fn_800091D0(&m_laserStart, &Ref(fn_8040FB08(&m_matrix)));
            fn_800091D0(&m_laserDirection, &Ref(((VectorSubtractFn)fn_8040CD74)(&m_laserStart,
                                                                               &m_owner->m_chara->m_position)));
            fn_803358D0(m_owner, new DevilDoomCtrlUnit_BitLaser(m_owner, &m_laserStart, &m_laserDirection,
                                                                m_owner->m_settings.field_0xE0));
            place->m_locked = 1;
            m_state = 5;
            break;
        }
        break;
    case 4:
        if (place->m_free || !m_owner->m_active) {
            m_state = m_nextState;
            break;
        }
        switch (m_owner->field_0x628) {
        case 2: {
            Vec3f toBit;
            fn_800091EC(&toBit, &Ref(Sub(fn_8040FB08(&m_matrix), m_owner->m_chara->m_position)));
            Vec3f toTarget;
            fn_800091EC(&toTarget, &Ref(((VectorSubtractFn)fn_8040CD74)(&m_owner->m_target,
                                                                       &m_owner->m_chara->m_position)));
            toTarget.y = 0.0f;
            toBit.y = 0.0f;
            float radius = 100.0f + fn_8040CF50(&toTarget);
            if (fn_8040CF50(&toBit) > radius) {
                m_state = m_nextState;
            }
            break;
        }
        }
        break;
    case 5:
        EndCharge();
        m_state = 2;
        m_time = 0.0f;
        break;
    case 6:
        Disappear();
        m_state = 10;
        m_time = 0.0f;
        break;
    case 7:
        Vanish();
        m_state = 10;
        m_time = m_owner->m_settings.field_0x90;
        break;
    case 8:
        Vanish();
        m_state = 9;
        m_time = m_owner->m_settings.field_0x90;
        place->m_done = 1;
        break;
    case 9:
        if (!place->m_done) {
            m_state = 1;
        }
        break;
    case 10:
        if (m_owner->field_0x628 == 2) {
            place->m_done = 1;
            m_state = 9;
        } else if (m_time > m_owner->m_settings.field_0x90) {
            m_state = 1;
        }
        break;
    }
    float blend = place->m_blend;
    if (blend < 0.0001) {
        fn_800091D0(&m_prevPosition, &m_position);
        fn_8040C148(&m_prevRotation, &m_matrix);
        fn_8040FA20(&m_prevRotation, &lbl_805E07A0);
    }
    blend += dt / m_owner->m_settings.field_0x98;
    if (blend > 1.0f) {
        blend = 1.0f;
    }
    Vec3f position;
    fn_800091EC(&position, V3(place->m_position));
    if (blend < 1.0f) {
        fn_800091D0(&position, &Ref(fn_8040CDD0(&Ref(fn_8040CDA8(&position, blend)),
                                                &Ref(fn_8040CDA8(&m_prevPosition, 1.0f - blend)))));
    }
    fn_800091D0(&m_position, &position);
    fn_8040CFF8(V3(place->m_rotation), &Ref(fn_80093230(&m_spin, dt)));
    Mtx34f rotation;
    fn_8040C148(&rotation, &lbl_805E0720);
    fn_8040B80C(&rotation, place->m_rotation[2]);
    fn_8040B848(&rotation, place->m_rotation[1]);
    fn_8040B884(&rotation, place->m_rotation[0]);
    if (blend < 1.0f) {
        fn_8040C148(&rotation, &RefM(fn_8040EFA4(&m_prevRotation, &rotation, blend)));
    }
    fn_8040C148(&m_matrix, &lbl_805E0720);
    fn_8040B59C(&m_matrix, &Ref(fn_8040B67C(&m_owner->m_parts[m_group].m_matrix, &position)));
    fn_8040C148(&m_matrix, &RefM(fn_8001E044(&m_matrix, &rotation)));
    place->m_blend = blend;
    fn_800091D0(&m_laserStart, &Ref(fn_8040FB08(&m_matrix)));
    if (CharaColliBase *collider = m_collider.get()) {
        /* Slot 0x3C reads a position from the start of what it is given. */
        collider->SetCharInfo((CharInfo *)&Ref(fn_8040FB08(&m_matrix)));
    }
    if (Target *target = m_target.get()) {
        target->fn_800CCE38(&Ref(fn_8040FB08(&m_matrix)));
    }
    return false;
}

void DevilDoomCtrlUnit_Bit::OnDamage(AttackCallbackParam &param) {
    param.m_result = 1;
    m_health -= param.m_damage;
    if (m_health <= 0.0f) {
        fn_8016F0CC(fn_80079808()->m_score, 0, m_owner->m_settings.field_0x88);
        m_state = 6;
    }
}

bool DevilDoomCtrlUnit_Bit::Appear() {
    if (m_owner->field_0x62C > 0.0f) {
        return false;
    }
    m_owner->field_0x62C = m_owner->m_settings.field_0xB0;
    m_health = m_owner->m_settings.field_0x84;
    fn_8040C148(&m_matrix, &lbl_805E0720);
    m_effect = fn_800A31F0()->Launch(m_owner->m_effects, 6);
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        effect->SetMatrix(&m_matrix);
        effect->SetField14(FrameTime());
    }
    void *manager = fn_80021B08();
    m_collider = fn_800C7EA8(manager, &Ref(fn_8040FB08(&m_matrix)), 0x80, 4, 0, lbl_805EE698, &lbl_805E07A0,
                             15.0f);
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
    m_target = fn_801E72DC(0x800);
    Place()->m_blend = 0.0f;
    Mtx34f *frame;
    Vec3f position;
    float nearest;
    int best = -1;
    Vec3f nearestPosition;
    for (int i = 0; i < 3; i++) {
        frame = fn_80415E70(m_owner->m_motion, 0x42F + i);
        position.x = frame->values[0][3];
        position.y = frame->values[1][3];
        position.z = frame->values[2][3];
        float distance = fn_8040CF50(&Ref(((VectorSubtractFn)fn_8040CD74)(&position, &Ref(fn_8040FB08(&m_matrix)))));
        if (best < 0 || distance < nearest) {
            nearest = distance;
            best = i;
            fn_800091D0(&nearestPosition, &position);
        }
        effect = fn_800A31F0()->Launch(m_owner->m_effects, 0x23).lock();
        if (effect) {
            Mtx34f matrix;
            fn_80016370(&matrix, frame);
            fn_80425E68(effect.get(), &matrix);
            effect->SetField14(FrameTime());
        }
    }
    fn_800091D0(&m_position, &Ref(fn_8040B67C(&RefM(fn_8040BEF8(&m_owner->m_parts[m_group].m_matrix)),
                                              &nearestPosition)));
    fn_8040B80C(&m_matrix, Place()->m_rotation[2]);
    fn_8040B848(&m_matrix, Place()->m_rotation[1]);
    fn_8040B884(&m_matrix, Place()->m_rotation[0]);
    Place()->m_rotation[0] = ToRadian(180.0f - 360.0f * fn_8040EE90());
    Place()->m_rotation[1] = ToRadian(180.0f - 360.0f * fn_8040EE90());
    Place()->m_rotation[2] = ToRadian(180.0f - 360.0f * fn_8040EE90());
    m_spin.x = 1.0f - 2.0f * fn_8040EE90();
    m_spin.y = 1.0f - 2.0f * fn_8040EE90();
    m_spin.z = 1.0f - 2.0f * fn_8040EE90();
    fn_800091D0(&m_spin, &Ref(fn_8040CDA8(&Ref(fn_8040CEE8(&m_spin)), ToRadian(m_owner->m_settings.field_0xB4))));
    Place()->m_alive = 1;
    return true;
}

void DevilDoomCtrlUnit_Bit::StartCharge() {
    m_chargeEffect = fn_800A31F0()->Launch(m_owner->m_effects, 0xA);
    boost::shared_ptr<EffectElement> effect(m_chargeEffect.lock());
    if (effect) {
        effect->SetMatrix(&m_matrix);
        effect->SetField14(FrameTime());
    }
}

void DevilDoomCtrlUnit_Bit::EndCharge() {
    boost::shared_ptr<EffectElement> effect(m_chargeEffect.lock());
    if (effect) {
        effect->Kill();
    }
    m_dead = 1;
}

void DevilDoomCtrlUnit_Bit::Disappear() {
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        effect->Retire();
    }
    effect = m_chargeEffect.lock();
    if (effect) {
        effect->Retire();
    }
    m_dead = 1;
    if (m_collider) {
        m_collider.reset();
    }
    if (m_target) {
        m_target.reset();
    }
    Place()->m_alive = 0;
    effect = fn_800A31F0()->Launch(m_owner->m_effects, 0).lock();
    if (EffectElement *element = effect.get()) {
        fn_80425E4C(element, &Ref(fn_8040FB08(&m_matrix)));
        effect->SetField14(FrameTime());
    }
    fn_801D54F4(fn_80053164(), 0x8132, &m_laserStart, 0, 1);
}

void DevilDoomCtrlUnit_Bit::Vanish() {
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        effect->Stop();
    }
    effect = m_chargeEffect.lock();
    if (effect) {
        effect->Stop();
    }
    m_dead = 1;
    if (m_collider) {
        m_collider.reset();
    }
    if (m_target) {
        m_target.reset();
    }
    Place()->m_alive = 0;
}

DevilDoomCtrlUnit_BitLaser::DevilDoomCtrlUnit_BitLaser(DevilDoomVarParam *param, const Vec3f *start,
                                                       const Vec3f *direction, const u8 *dead,
                                                       float length)
    : BossCtrlUnit(param), DevilDoomCtrlUnit(param) {
    m_startSource = start;
    m_directionSource = direction;
    m_dead = dead;
    fn_800091D0(&m_start, m_startSource);
    fn_800091D0(&m_direction, m_directionSource);
    m_maxLength = length;
    m_following = 1;
    Init();
}

DevilDoomCtrlUnit_BitLaser::DevilDoomCtrlUnit_BitLaser(DevilDoomVarParam *param, const Vec3f *start,
                                                       const Vec3f *direction, float length)
    : BossCtrlUnit(param), DevilDoomCtrlUnit(param) {
    m_startSource = 0;
    m_directionSource = 0;
    m_dead = 0;
    fn_800091D0(&m_start, start);
    fn_800091D0(&m_direction, direction);
    m_maxLength = length;
    m_following = 0;
    Init();
}

DevilDoomCtrlUnit_BitLaser::~DevilDoomCtrlUnit_BitLaser() {
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        effect->Retire();
    }
    effect = m_startEffect.lock();
    if (effect) {
        effect->Stop();
    }
    if (m_collider) {
        m_collider.reset();
    }
}

void DevilDoomCtrlUnit_BitLaser::Init() {
    m_kind = 2;
    m_length = 0.0f;
    m_width = 0.0f;
    m_time = 0.0f;
    m_ending = 0;
    m_matrix = m_startMatrix = lbl_805E0720;
    m_effect = fn_800A31F0()->Launch(m_owner->m_effects, 0x1B);
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        effect->SetMatrix(&m_matrix);
        effect->SetField14(FrameTime());
    }
    m_startEffect = fn_800A31F0()->Launch(m_owner->m_effects, 0x1D);
    effect = m_startEffect.lock();
    if (effect) {
        effect->SetMatrix(&m_startMatrix);
        effect->SetField14(FrameTime());
    }
    void *manager = fn_80021B08();
    m_collider = fn_800C799C(manager, &m_start, &lbl_805E07B8, 0, 0, 4, 0, lbl_805EE698, &lbl_805E07A0,
                             &lbl_805E07B8, 0, 0.5f * m_length, m_width);
    if (m_collider) {
        m_collider->SetField2C(1);
        boost::weak_ptr<CharaColliAttack> attack = fn_800C4374(manager, m_collider);
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

int DevilDoomCtrlUnit_BitLaser::Tick(float dt) {
    m_time += dt;
    if (m_dead != 0 && *m_dead) {
        m_ending = 1;
        m_dead = 0;
        m_directionSource = 0;
        m_startSource = 0;
    }
    if (m_startSource != 0) {
        fn_800091D0(&m_start, m_startSource);
    }
    if (m_directionSource != 0) {
        fn_800091D0(&m_direction, m_directionSource);
    }
    if (m_ending) {
        m_width -= dt / m_owner->m_settings.field_0xA4;
        if (m_width < 0.0f) {
            m_width = 0.0f;
            return true;
        }
    } else {
        m_width += dt / m_owner->m_settings.field_0xA4;
        if (m_width > 1.0f) {
            m_width = 1.0f;
        }
    }
    m_length += dt * m_owner->m_settings.field_0xA0;
    Vec3f up(0.0f, 1.0f, 0.0f);
    Vec3f forward;
    fn_800091EC(&forward, &Ref(fn_8040CEE8(&m_direction)));
    Vec3f side;
    fn_800091EC(&side, &Ref(fn_8040CFB8(&up, &forward)));
    if (fn_8040CF50(&side) < 0.0001) {
        fn_800091D0(&up, &Vec3f(1.0f, 0.0f, 0.0f));
        fn_800091D0(&side, &Ref(fn_8040CEE8(&Ref(fn_8040CFB8(&up, &forward)))));
    } else {
        fn_8040CE74(&side);
    }
    fn_800091D0(&up, &Ref(fn_8040CFB8(&forward, &side)));
    m_startMatrix = lbl_805E0720;
    fn_8040B59C(&m_startMatrix, &m_start);
    m_startMatrix.values[0][0] = side.x;
    m_startMatrix.values[1][0] = side.y;
    m_startMatrix.values[2][0] = side.z;
    m_startMatrix.values[0][1] = up.x;
    m_startMatrix.values[1][1] = up.y;
    m_startMatrix.values[2][1] = up.z;
    m_startMatrix.values[0][2] = forward.x;
    m_startMatrix.values[1][2] = forward.y;
    m_startMatrix.values[2][2] = forward.z;
    m_matrix = m_startMatrix;
    float length;
    if (m_following) {
        if (m_length > m_maxLength) {
            m_length = m_maxLength;
        }
        length = m_length;
    } else if (m_length > m_maxLength) {
        fn_8040B52C(&m_matrix, 0.0f, 0.0f, m_length - m_maxLength);
        length = m_maxLength;
        boost::shared_ptr<EffectElement> effect(m_startEffect.lock());
        if (effect && m_time > 0.5f) {
            effect->Stop();
        }
        float over = m_length - m_maxLength;
        Vec3f target;
        fn_800091EC(&target, &Ref(fn_8040B67C(&RefM(fn_8040BEF8(&m_startMatrix)), &m_owner->m_target)));
        if (over > m_owner->m_settings.field_0xA8 && over > target.z) {
            m_ending = 1;
        }
    } else {
        length = m_length;
    }
    fn_8040B720(&m_matrix, m_width, m_width, length / 100.0f);
    Vec3f center(0.0f, 0.0f, 50.0f);
    fn_800091D0(&center, &Ref(fn_8040B67C(&m_matrix, &center)));
    Vec3f rotation(0.0f, 0.0f, 0.0f);
    fn_80049290(&rotation, &m_startMatrix);
    m_collider.px->SetCharInfo((CharInfo *)&center);
    m_collider.px->SetRotation(&rotation);
    m_collider.px->SetSize(m_width, 0.0f, 0.5f * length, -2);
    if (m_startSource != 0) {
        fn_800091D0(&m_soundPosition, &center);
        m_soundPosition.y = 0.0f;
    } else {
        fn_800091D0(&m_soundPosition, &center);
    }
    fn_801D53C4(fn_80053164(), 0x8133, &m_soundPosition, this, 3, 0, 3);
    return false;
}

void DevilDoomCtrlUnit_BitLaser::OnHit(AttackCallbackParam &param) {
    boost::weak_ptr<CharaColliAttack> attack(Param(param).m_attacker);
    boost::shared_ptr<CharaColliAttack> collision(attack.lock());
    fn_800091D0(&Param(param).m_offset, &Ref(((VectorSubtractFn)fn_8040CD74)(
                                            &Param(param).m_position, &m_owner->m_chara->m_position)));
    Param(param).m_damage = attack.lock()->m_power;
}
