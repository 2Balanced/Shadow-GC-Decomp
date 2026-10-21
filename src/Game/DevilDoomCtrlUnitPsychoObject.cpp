/* DevilDoomCtrlUnit_PsychoObject and DevilDoomCtrlUnit_PsychoObjectControl (class names from
 * RTTI): the objects the Devil Doom boss raises with psychokinesis and throws at the player. The
 * control unit raises them once the boss's psycho mode starts (field_0x19C 1): ring capsules first
 * (more while time remains), then the debris kinds the boss's health level asks for, and deals them
 * at random into two rows (DevilDoomVarParam 0x20C and 0x210) that float around the boss. Each
 * frame it spaces the rows out by the objects' widths, and when the boss asks, it sends the first
 * heavy object (kind 2 or more) at the player (state 2) or has the sent ones fly on (state 3).
 * An object bobs at its place in a row (state 1), turns (state 2), flies at the target (state 3),
 * flies on until its time is up (state 4), falls and fades when its row is let go (state 5), and
 * breaks when hit (state 6). File name inferred from the classes; names marked "(name inferred)"
 * and all member names are not original. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first).
 *
 * The unit is NonMatching: every function, the data and the function order match except
 * DevilDoomCtrlUnit_PsychoObjectControl::Tick (20/21 functions). Its code matches but for the
 * register assignment: retail keeps the object count in r31 and this in r30, GC/2.7 the other way
 * round (and the group bases one register lower). About 25 source shapes were tried. */
#include <boost/function.hpp>

#include "Effect/System/EffectElement.h"
#include "Game/BossCtrlUnit.h"
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/DevilDoomCtrlUnit.h"
#include "Game/EffectPJSLauncher.h"
#include "Game/EnemyCharInfo.h"
#include "Game/GadgetDebris.h"
#include "Game/WeaponBullet.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

namespace std {
float sin(float x);
} // namespace std

using Effect::System::EffectElement;
using Effect::System::EffectLaunchInfo;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

/* The RenderWare types used here, declared locally (layouts as in rw/rwcore.h, reduced to the
 * members used; as in HeavyDog0410CtrlUnit.cpp). */
extern "C" {
typedef struct RwV3d {
    float x;
    float y;
    float z;
} RwV3d;

typedef struct RwMatrix {
    RwV3d right;
    u32 flags;
    RwV3d up;
    u32 pad1;
    RwV3d at;
    u32 pad2;
    RwV3d pos;
    u32 pad3;
} RwMatrix;

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
    /* 0x10 */ RwMatrix modelling;
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

/* The boss's character info, which BossVarParam keeps in a shared_ptr (its px is mapped as
 * m_chara). */
inline const boost::shared_ptr<EnemyCharInfo> &BossCharInfo(BossVarParam *param) {
    return *reinterpret_cast<const boost::shared_ptr<EnemyCharInfo> *>(&param->m_chara);
}

/* The game state (struct name unknown; see DevilDoomCtrlUnit.cpp): the frame time at 0x24. */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x24];
    /* 0x024 */ float m_dt;
};
extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The frame time the effects follow (as in DevilDoomCtrlUnit.cpp). */
#define FrameTime() ((u32)&lbl_8057D6A0.m_dt)

/* The world of each stage area (layout as in DevilDoomCtrlUnit.cpp). */
struct UnkWorldEntry_8051D208 {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ void *m_world;
    /* 0x14 */ u8 field_0x14[0x18 - 0x14];
};
extern UnkWorldEntry_8051D208 lbl_8051D208[];

/* The save data (see EggLastMech.cpp): the record at 0x70C keeps the score. */
struct UnkObject_80079808 {
    /* 0x000 */ u8 field_0x0[0x70C];
    /* 0x70C */ u8 m_score[1];
};

/* A colour fade applied to a clump's materials (class name unknown; constructor 0x80027D5C, size
 * from its allocation): the alpha at 0xC, a mode at 0x24. */
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

class UnkCharaList_80074194;

/* The player's character id (see Helicopter.cpp) and the null owner of colliders. */
extern s32 lbl_805EE588;
extern boost::shared_ptr<CharInfo> lbl_805EE698;
extern const float lbl_805FAD38; /* pi */

extern "C" {
/* Effects (see DevilDoomCtrlUnit.cpp and GoalRing.cpp). */
UnkEffectManager_800A31F0 *fn_800A31F0(void);
void fn_800A4210(boost::shared_ptr<EffectElement> *effect);
void fn_80425E4C(EffectElement *effect, const Vec3f *position);
boost::weak_ptr<EffectElement> fn_801D2B38(int arg0, const Vec3f *position,
                                           const Vec3f *rotation, int number, const float *time);

/* RenderWare clumps and frames (see DevilDoomCtrlUnit.cpp). */
void *fn_8046256C(RpClump *clump);
void fn_804623EC(void *world, RpClump *clump);
void fn_804622FC(void *world, RpClump *clump);
RpClump *fn_80457090(void *data);
void fn_80457468(RpClump *clump);
void *fn_80169364(RpClump *clump);
int fn_80169350(void *frame);
void fn_8003A840(int area);
void fn_8047EBA0(RwMatrix *matrix);
RwFrame *fn_8048722C(RwFrame *frame);

/* The clump fade (behaviour inferred). */
UnkClumpFade_80027D5C *fn_80027D5C(UnkClumpFade_80027D5C *fade);
void fn_80038198(RpClump *clump, UnkClumpFade_80027D5C *fade);

/* The resource loader (see DevilDoomCtrlUnit.cpp). */
void *fn_800122D4(void);
void *fn_8001277C(void *loader, s32 slot, int arg2);

/* Sounds (see DevilDoomCtrlUnit.cpp). */
void *fn_80053164(void);
void fn_801D54F4(void *manager, u16 id, const void *source, int arg3, int arg4);
void fn_801D53C4(void *manager, int id, const Vec3f *position, void *source, int arg4,
                 int arg5, int arg6);

/* The score (see EggLastMech.cpp), the player and the item command (Player::GetItemCommand's
 * constructor, 0x8015F760). */
UnkObject_80079808 *fn_80079808(void);
void fn_8016F0CC(void *score, int arg1, int points);
UnkCharaList_80074194 *fn_80074194(void);
boost::weak_ptr<CharInfo> fn_800A8B74(UnkCharaList_80074194 *manager, int id);
CharCommand *fn_8015F760(void *command, int arg0, int item, u8 arg2, int arg3);

/* Colliders and targets (see DevilDoomCtrlUnit.cpp and BalloonGenerator.cpp). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C86A8(void *manager, void *data, const Vec3f *position,
                                              const Vec3f *rotation, int arg4, int arg5, int arg6,
                                              const boost::weak_ptr<CharInfo> &owner);
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager, boost::shared_ptr<CharaColliBase> collider);
boost::weak_ptr<CharaColliAttack> fn_800C42D0(void *manager, boost::shared_ptr<CharaColliBase> collider);
boost::shared_ptr<Target> fn_801E72DC(int flags);

/* Math (behaviour inferred). */
float fn_8040EE90(void); /* a random value in [0, 1) */
float fn_8040CF90(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Vec3f fn_800091A4(const Vec3f *vector, float scale);
void fn_8040CE04(Vec3f *vector, const Vec3f *value); /* -= */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* += */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
Vec3f fn_8040CEE8(const Vec3f *vector);
Vec3f fn_80093230(const Vec3f *vector, float dt);
}

/* fn_8040CD74 called as returning a - b by value (see ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* Passes a returned vector on by address (as in DevilDoomCtrlUnit.cpp). */
inline const Vec3f &Ref(const Vec3f &vector) {
    return vector;
}

/* A copied vector passed on by address (as in HeavyDog0410CtrlUnit.cpp). */
#define Ptr(vector) ((Vec3f *)&(vector))

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

/* The place of a psycho object in its row (struct name inferred): the rows are doubly linked
 * lists of these. */
struct PsychoObjectNode {
    /* 0x00 */ float m_angle; /* around the boss */
    /* 0x04 */ s32 m_type;    /* the index in s_objects */
    /* 0x08 */ s32 m_state;
    /* 0x0C */ PsychoObjectNode *m_next;
    /* 0x10 */ PsychoObjectNode *m_prev;
};

/* The two rows the boss keeps, DevilDoomVarParam 0x20C and 0x210 (mapped there as two words; struct
 * and member names inferred). PsychoRow is a macro: an inline function here would keep Unlink,
 * which uses it, from being inlined. */
struct DevilDoomPsychoRows {
    /* 0x000 */ u8 field_0x0[0x20C];
    /* 0x20C */ PsychoObjectNode *m_rows[2];
};
#define PsychoRow(param, index) (reinterpret_cast<DevilDoomPsychoRows *>(param)->m_rows[index])

/* Appends node to the list at head (name inferred). */
inline void PushBack(PsychoObjectNode **head, PsychoObjectNode *node) {
    if (*head == 0) {
        *head = node;
        node->m_prev = 0;
        node->m_next = 0;
    } else {
        PsychoObjectNode *tail;
        for (tail = *head; tail->m_next != 0; tail = tail->m_next) {
        }
        tail->m_next = node;
        node->m_prev = tail;
        node->m_next = 0;
    }
}

/* A kind of psycho object (struct name inferred): kind 0 is a ring capsule, 1 light debris,
 * 2 and 3 heavy debris; the models, collision and debris are resource slot 0x16 entries. */
struct PsychoObjectInfo {
    /* 0x00 */ s32 m_kind;
    /* 0x04 */ s32 m_models[2];
    /* 0x0C */ s32 m_debris;
    /* 0x10 */ s32 m_collision;
    /* 0x14 */ s32 field_0x14;
    /* 0x18 */ float m_health;
    /* 0x1C */ s32 m_score;
    /* 0x20 */ s32 m_item;
    /* 0x24 */ s32 field_0x24;
    /* 0x28 */ s32 m_effect;
    /* 0x2C */ s32 m_sound;
};

/* The types each group of objects is drawn from: the first type, then the number of types of
 * each of the three debris groups (name inferred). */
static const s32 s_groupTypes[4] = {1, 8, 3, 2};

/* The number of objects of each group per health level (name inferred). */
static s32 s_groupCounts[4][4] = {
    {0, 5, 10, 20},
    {0, 5, 10, 10},
    {0, 5, 15, 5},
    {0, 5, 10, 5},
};

/* The width each kind takes in a row, in degrees (name inferred). */
static float s_widths[4] = {5.0f, 5.0f, 10.0f, 40.0f};

static PsychoObjectInfo s_objects[14] = {
    {0, {0x16, 0x18}, -1, 0x15, 0x17, 1.0f, 0, 0x14, -1, -1, -1},
    {1, {0x32, -1}, 0x33, 0x31, -1, 1.0f, 500, 0, -1, 0x47, -1},
    {1, {0x35, -1}, 0x36, 0x34, -1, 1.0f, 500, 0, -1, 0x47, -1},
    {1, {0x38, -1}, 0x39, 0x37, -1, 1.0f, 500, 0, -1, 0x47, -1},
    {1, {0x1A, -1}, 0x1B, 0x19, -1, 1.0f, 500, 0, -1, 0x17, 0x813A},
    {1, {0x23, -1}, 0x24, 0x22, -1, 1.0f, 500, 0, -1, 0x44, 0x813B},
    {1, {0x29, -1}, 0x2A, 0x28, -1, 1.0f, 500, 0, -1, 0x3B, 0x8138},
    {1, {0x3B, -1}, 0x3C, 0x3A, -1, 1.0f, 500, 0, -1, 0x4A, 0x813C},
    {1, {0x3E, -1}, 0x3F, 0x3D, -1, 1.0f, 500, 0, -1, 0x1A, 0x813B},
    {2, {0x20, -1}, 0x21, 0x1F, -1, 6.0f, 1000, 0, -1, 0x41, 0x8138},
    {2, {0x26, -1}, 0x27, 0x25, -1, 6.0f, 1000, 0, -1, 0x2C, 0x8139},
    {2, {0x2C, -1}, 0x2D, 0x2B, -1, 6.0f, 1000, 0, -1, 0x3E, 0x8138},
    {3, {0x1D, -1}, 0x1E, 0x1C, -1, 12.0f, 2000, 0, -1, 0x2F, 0x8138},
    {3, {0x2F, -1}, 0x30, 0x2E, -1, 12.0f, 2000, 0, -1, 0x33, 0x8138},
};

class DevilDoomCtrlUnit_PsychoObject : public DevilDoomCtrlUnit, public AttackCallbackContainer {
  public:
    DevilDoomCtrlUnit_PsychoObject(DevilDoomVarParam *param, PsychoObjectNode **list, int type);
    virtual ~DevilDoomCtrlUnit_PsychoObject();
    virtual int Tick(float dt);
    virtual void OnHit(AttackCallbackParam &param);
    virtual void OnDamage(AttackCallbackParam &param);

    /* Names inferred. */
    void Fall(float dt);
    void Float(float dt);
    Fn8040CopyVector3 GetHome();

    /* Takes the object out of its row (name inferred). */
    void Unlink() {
        if (m_node.m_prev != 0) {
            m_node.m_prev->m_next = m_node.m_next;
        }
        if (m_node.m_next != 0) {
            m_node.m_next->m_prev = m_node.m_prev;
        }
        if (PsychoRow(m_owner, 0) == &m_node) {
            PsychoRow(m_owner, 0) = m_node.m_next;
        }
        if (PsychoRow(m_owner, 1) == &m_node) {
            PsychoRow(m_owner, 1) = m_node.m_next;
        }
        m_node.m_prev = 0;
        m_node.m_next = 0;
    }

    /* The random value between low and high (name inferred). */
    static float Random(const float &low, const float &high) {
        return low + (high - low) * fn_8040EE90();
    }

    /* 0x10 */ PsychoObjectNode m_node;
    /* 0x24 */ u32 field_0x24;
    /* 0x28 */ float m_health;
    /* 0x2C */ float m_height;    /* above the boss */
    /* 0x30 */ float m_distance;  /* from the boss */
    /* 0x34 */ float m_time;
    /* 0x38 */ float m_phase;     /* of the bobbing */
    /* 0x3C */ float m_amplitude; /* of the bobbing */
    /* 0x40 */ float m_period;    /* of the bobbing */
    /* 0x44 */ float m_holdTime;  /* before a let-go capsule falls */
    /* 0x48 */ Vec3f m_position;
    /* 0x54 */ Vec3f m_rotation;
    /* 0x60 */ Vec3f m_velocity;
    /* 0x6C */ Vec3f m_spin;
    /* 0x78 */ Vec3f m_spinAccel;
    /* 0x84 */ u8 m_passed; /* has passed the player */
    /* 0x88 */ RpClump *m_clumps[2];
    /* 0x90 */ RwFrame *m_frames[2];
    /* 0x98 */ boost::weak_ptr<EffectElement> m_effect;
    /* 0xA0 */ float m_alpha;
    /* 0xA4 */ float m_alphaSpeed;
    /* 0xA8 */ UnkClumpFade_80027D5C *m_fade;
    /* 0xAC */ boost::shared_ptr<CharaColliBase> m_collider;
    /* 0xB4 */ boost::shared_ptr<Target> m_target;
};
SIZE_ASSERT(DevilDoomCtrlUnit_PsychoObject, 0xCC);

DevilDoomCtrlUnit_PsychoObjectControl::DevilDoomCtrlUnit_PsychoObjectControl(DevilDoomVarParam *owner)
    : BossCtrlUnit(owner), DevilDoomCtrlUnit(owner) {
    m_kind = 2;
    m_owner->field_0x196 = 0;
    m_owner->field_0x198 = 0;
    m_owner->field_0x197 = 0;
    field_0xC[0] = 0;
}

DevilDoomCtrlUnit_PsychoObjectControl::~DevilDoomCtrlUnit_PsychoObjectControl() {
    for (int i = 0; i < 2; i++) {
        PsychoObjectNode *head = PsychoRow(m_owner, i);
        if (head != 0) {
            for (PsychoObjectNode *node = head; node != 0; node = node->m_next) {
                node->m_state = 5;
            }
        }
    }
}

int DevilDoomCtrlUnit_PsychoObjectControl::Tick(float dt) {
    if (m_owner->field_0x19C != 1) {
        return true;
    }
    fn_801D53C4(fn_80053164(), 0x8136, 0, this, 2, 0, 3);
    if (!field_0xC[0]) {
        PsychoObjectNode *list = 0;
        int total = 0;
        if (m_owner->m_timeRatio > 0.0f) {
            int count = (int)(m_owner->m_settings.field_0x3C * m_owner->m_timeRatio) + 1;
            total = count;
            for (; count != 0; count--) {
                fn_803358D0(m_owner, new DevilDoomCtrlUnit_PsychoObject(m_owner, &list, 0));
            }
        }
        int first = s_groupTypes[0];
        int count = s_groupCounts[m_owner->m_healthLevel][1];
        total += count;
        for (; count != 0; count--) {
            fn_803358D0(m_owner, new DevilDoomCtrlUnit_PsychoObject(
                                     m_owner, &list, first + (int)(s_groupTypes[1] * fn_8040EE90())));
        }
        first += s_groupTypes[1];
        count = s_groupCounts[m_owner->m_healthLevel][2];
        total += count;
        for (; count != 0; count--) {
            fn_803358D0(m_owner, new DevilDoomCtrlUnit_PsychoObject(
                                     m_owner, &list, first + (int)(s_groupTypes[2] * fn_8040EE90())));
        }
        first += s_groupTypes[2];
        count = s_groupCounts[m_owner->m_healthLevel][3];
        total += count;
        for (; count != 0; count--) {
            fn_803358D0(m_owner, new DevilDoomCtrlUnit_PsychoObject(
                                     m_owner, &list, first + (int)(s_groupTypes[3] * fn_8040EE90())));
        }
        int left = total;
        int row = 0;
        for (; left != 0; left--) {
            int index = (int)(left * fn_8040EE90());
            PsychoObjectNode *node = list;
            for (int i = 0; node != 0; node = node->m_next, i++) {
                if (i == index) {
                    break;
                }
            }
            if (node->m_next != 0) {
                node->m_next->m_prev = node->m_prev;
            }
            if (node->m_prev != 0) {
                node->m_prev->m_next = node->m_next;
            }
            if (node == list) {
                list = node->m_next;
            }
            PushBack(&PsychoRow(m_owner, row), node);
            row = 1 - row;
        }
        boost::weak_ptr<EffectElement> effect = fn_800A31F0()->Launch(m_owner->m_effects, 0x34);
        boost::shared_ptr<EffectElement> element = effect.lock();
        if (element) {
            Vec3f position;
            fn_800091EC(&position, &m_owner->m_chara->m_position);
            position.y = 0.5f * (m_owner->m_settings.field_0x44 + m_owner->m_settings.field_0x40);
            fn_80425E4C(element.get(), &position);
            element->SetField14(FrameTime());
        }
        field_0xC[0] = 1;
    }
    if (PsychoRow(m_owner, 0) == 0 && PsychoRow(m_owner, 1) == 0) {
        return true;
    }
    UpdateAngles();
    if (!m_owner->field_0x196) {
        return true;
    }
    int sent = 0;
    if (m_owner->field_0x197) {
        PsychoObjectNode *nodes[2];
        nodes[0] = PsychoRow(m_owner, 0);
        nodes[1] = PsychoRow(m_owner, 1);
        while (nodes[0] != 0 || nodes[1] != 0) {
            for (int i = 0; i < 2; i++) {
                if (nodes[i] != 0) {
                    if (s_objects[nodes[i]->m_type].m_kind >= 2) {
                        sent++;
                        nodes[i]->m_state = 2;
                        if (sent >= 1) {
                            break;
                        }
                    }
                    nodes[i] = nodes[i]->m_next;
                }
            }
            if (sent >= 1) {
                break;
            }
        }
        m_owner->field_0x197 = 0;
    }
    if (m_owner->field_0x198) {
        for (int i = 0; i < 2; i++) {
            for (PsychoObjectNode *node = PsychoRow(m_owner, i); node != 0; node = node->m_next) {
                if (node->m_state == 2) {
                    node->m_state = 3;
                }
            }
        }
        m_owner->field_0x198 = 0;
    }
    return false;
}

void DevilDoomCtrlUnit_PsychoObjectControl::UpdateAngles() {
    PsychoObjectNode *prev;
    PsychoObjectNode *node;
    m_owner->field_0x196 = 0;
    for (int i = 0; i < 2; i++) {
        prev = 0;
        node = PsychoRow(m_owner, i);
        float sign = i != 0 ? -1.0f : 1.0f;
        while (node != 0) {
            float step;
            if (prev == 0) {
                node->m_angle = 0.0f;
                step = 0.5f * ToRadian(s_widths[s_objects[node->m_type].m_kind]);
            } else {
                node->m_angle = prev->m_angle;
                float width = ToRadian(s_widths[s_objects[node->m_type].m_kind]);
                float prevWidth = ToRadian(s_widths[s_objects[prev->m_type].m_kind]);
                step = width + prevWidth;
                step *= 0.5f;
            }
            node->m_angle += sign * step;
            if (s_objects[node->m_type].m_kind >= 2) {
                m_owner->field_0x196 = 1;
            }
            prev = node;
            node = node->m_next;
        }
    }
}

DevilDoomCtrlUnit_PsychoObject::DevilDoomCtrlUnit_PsychoObject(DevilDoomVarParam *param,
                                                               PsychoObjectNode **list, int type)
    : BossCtrlUnit(param), DevilDoomCtrlUnit(param), m_rotation(0.0f, 0.0f, 0.0f) {
    m_kind = 2;
    m_node.m_type = type;
    m_node.m_state = 0;
    m_passed = 0;
    m_height = Random(m_owner->m_settings.field_0x4C, m_owner->m_settings.field_0x50);
    m_distance = Random(m_owner->m_settings.field_0x54, m_owner->m_settings.field_0x58);
    m_health = s_objects[m_node.m_type].m_health;
    m_rotation.x = fn_8040EE90() * ToRadian(360.0f);
    m_rotation.y = fn_8040EE90() * ToRadian(360.0f);
    m_rotation.z = fn_8040EE90() * ToRadian(360.0f);
    m_spin.x = fn_8040EE90() * ToRadian(360.0f);
    m_spin.y = fn_8040EE90() * ToRadian(360.0f);
    m_spin.z = fn_8040EE90() * ToRadian(360.0f);
    fn_800091D0(&m_spin, &Ref(fn_8040CDA8(&Ref(fn_8040CEE8(&m_spin)), ToRadian(m_owner->m_settings.field_0x5C))));
    fn_800091D0(&m_spinAccel, &lbl_805E07A0);
    m_period = m_owner->m_settings.field_0x60;
    m_amplitude = m_owner->m_settings.field_0x64;
    m_time = 0.0f;
    m_phase = m_period * fn_8040EE90();
    m_holdTime = 0.0f;
    if (s_objects[m_node.m_type].m_kind == 0) {
        m_holdTime = m_owner->m_settings.field_0x80;
    }
    if (*list == 0) {
        *list = &m_node;
        m_node.m_prev = 0;
        m_node.m_next = 0;
    } else {
        PsychoObjectNode *tail = *list;
        PsychoObjectNode *next;
        while ((next = tail->m_next) != 0) {
            tail = next;
        }
        tail->m_next = &m_node;
        m_node.m_prev = tail;
        m_node.m_next = 0;
    }
    UnkClumpFade_80027D5C *fade = (UnkClumpFade_80027D5C *)operator new(sizeof(UnkClumpFade_80027D5C));
    if (fade != 0) {
        fade = fn_80027D5C(fade);
    }
    m_fade = fade;
    if (m_fade != 0) {
        m_fade->m_mode = 4;
        m_fade->SetAlpha(m_alpha);
    }
    m_alpha = 0.0f;
    m_alphaSpeed = 1.0f;
    for (int i = 0; i < 2; i++) {
        m_clumps[i] = 0;
        m_frames[i] = 0;
        if (s_objects[m_node.m_type].m_models[i] == -1) {
            break;
        }
        void *data = fn_8001277C(fn_800122D4(), 0x16, s_objects[m_node.m_type].m_models[i]);
        if (data != 0) {
            m_clumps[i] = fn_80457090(data);
        }
        if (m_clumps[i] != 0) {
            int area = fn_80169350(fn_80169364(m_clumps[i]));
            fn_8003A840(area);
            SetClumpWorld(m_clumps[i], lbl_8051D208[area].m_world);
            m_frames[i] = (RwFrame *)m_clumps[i]->object.parent;
            if (m_fade != 0) {
                fn_80038198(m_clumps[i], m_fade);
            }
        }
    }
    m_target = fn_801E72DC(0x800);
}

DevilDoomCtrlUnit_PsychoObject::~DevilDoomCtrlUnit_PsychoObject() {
    Unlink();
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
    if (m_fade != 0) {
        operator delete(m_fade);
        m_fade = 0;
    }
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        effect->SetField4C(0);
        effect->Stop();
        fn_800A4210(&effect);
    }
    if (m_collider) {
        m_collider.reset();
    }
    if (m_target) {
        m_target.reset();
    }
}

int DevilDoomCtrlUnit_PsychoObject::Tick(float dt) {
    int i;
    m_time += dt;
    m_alpha += dt * m_alphaSpeed;
    if (m_alpha > 1.0f) {
        m_alpha = 1.0f;
    } else if (m_alpha < 0.0f) {
        m_alpha = 0.0f;
    }
    switch (m_node.m_state) {
    case 0: {
        fn_800091D0(&m_position, Ptr(GetHome()));
        m_position.y = Random(m_owner->m_settings.field_0x40, m_owner->m_settings.field_0x44);
        void *data = fn_8001277C(fn_800122D4(), 0x16, s_objects[m_node.m_type].m_collision);
        if (data != 0) {
            void *manager = fn_80021B08();
            m_collider = fn_800C86A8(manager, data, &m_position, &m_rotation, 0, 4, 0, lbl_805EE698);
            if (m_collider) {
                m_collider->SetField2C(1);
                boost::weak_ptr<CharaColliAttack> attack = fn_800C4374(manager, m_collider);
                boost::shared_ptr<CharaColliAttack> collision(attack.lock());
                if (collision) {
                    collision->m_power = 1.0f;
                    collision->m_kind = 4;
                    collision->m_flags = 0x40;
                    collision->field_0x54 = 0;
                    collision->m_hitCallback = GetHitCallback();
                    *reinterpret_cast<u32 *>(collision->field_0x3C) = 4;
                    collision->field_0x55 = 1;
                    collision->m_attackCallback = GetDamageCallback();
                }
            }
        }
        fn_800091D0(&m_velocity, &lbl_805E07A0);
        m_velocity.y = m_owner->m_settings.field_0x48 * fn_8040EE90();
        m_node.m_state = 1;
        break;
    }
    case 1:
        Float(dt);
        break;
    case 2:
        if (fn_8040CF50(&m_spinAccel) < 0.0001) {
            fn_800091D0(&m_spinAccel,
                        &Ref(fn_8040CDA8(&Ref(fn_8040CEE8(&m_spin)), ToRadian(m_owner->m_settings.field_0x70))));
        }
        fn_8040CFF8(&m_spin, &Ref(fn_80093230(&m_spinAccel, dt)));
        fn_800091D0(&m_velocity, &lbl_805E07A0);
        break;
    case 3: {
        fn_800091D0(&m_velocity, &Ref(((VectorSubtractFn)fn_8040CD74)(&m_owner->m_target, &m_position)));
        float speed = m_owner->m_settings.field_0x74;
        fn_800091D0(&m_velocity, &Ref(fn_8040CDA8(&Ref(fn_8040CEE8(&m_velocity)), speed)));
        boost::shared_ptr<CharaColliAttack> collision = fn_800C42D0(fn_80021B08(), m_collider).lock();
        if (collision) {
            collision->field_0x54 = 1;
        }
    }
        Unlink();
        m_node.m_state = 4;
        m_time = 0.0f;
        break;
    case 4:
        if (!m_passed) {
            Vec3f offset;
            fn_800091EC(&offset,
                        &Ref(((VectorSubtractFn)fn_8040CD74)(&m_owner->m_playerPosition, &m_position)));
            if (fn_8040CF90(&offset, &m_velocity) < 0.0f) {
                float range;
                if (s_objects[m_node.m_type].m_kind == 3) {
                    range = 250.0f;
                } else {
                    range = 100.0f;
                }
                if (fn_8040CF50(&offset) < range) {
                    fn_801D54F4(fn_80053164(), 0x8137, &m_position, 0, 1);
                }
                m_passed = 1;
            }
        }
        if (m_time > m_owner->m_settings.field_0x78) {
            return true;
        }
        break;
    case 5:
        m_holdTime -= dt;
        if (m_holdTime > 0.0f) {
            Float(dt);
            break;
        }
        Unlink();
        Fall(dt);
        if (m_position.y < m_owner->m_settings.field_0x44) {
            m_alphaSpeed = -1.0f;
        }
        if (m_alpha <= 0.0f) {
            return true;
        }
        break;
    case 6: {
        if (s_objects[m_node.m_type].m_debris != -1) {
            void *data = fn_8001277C(fn_800122D4(), 0x16, s_objects[m_node.m_type].m_debris);
            if (data != 0) {
                UnkDebrisParam_8015F0B8 debris(BossCharInfo(m_owner), 0);
                fn_800091D0(reinterpret_cast<Vec3f *>(&debris.field_0x0[0x4]), &m_position);
                reinterpret_cast<Vec3f *>(&debris.field_0x0[0x10])->x = m_rotation.x;
                reinterpret_cast<Vec3f *>(&debris.field_0x0[0x10])->y = m_rotation.y;
                reinterpret_cast<Vec3f *>(&debris.field_0x0[0x10])->z = m_rotation.z;
                debris.scale = 1.3f;
                debris.model = (s32)data;
                debris.field_0x4C = 0x96;
                fn_8015D93C(&debris);
            }
        }
        if (s_objects[m_node.m_type].m_kind == 0) {
            fn_801D2B38(0, &m_position, &m_rotation, 0xA9, &lbl_8057D6A0.m_dt);
            fn_801D54F4(fn_80053164(), 0x813D, &m_position, 0, 1);
        } else if (s_objects[m_node.m_type].m_effect != -1) {
            boost::weak_ptr<EffectElement> effect =
                fn_800A31F0()->Launch(m_owner->m_effects, s_objects[m_node.m_type].m_effect);
            boost::shared_ptr<EffectElement> element = effect.lock();
            if (element) {
                fn_80425E4C(element.get(), &m_position);
                element->SetField14(FrameTime());
            }
            if (s_objects[m_node.m_type].m_sound != -1) {
                fn_801D54F4(fn_80053164(), s_objects[m_node.m_type].m_sound, &m_position, 0, 1);
            }
        }
        for (i = 0; i < 2; i++) {
            if (m_clumps[i] != 0) {
                void *world = fn_8046256C(m_clumps[i]);
                if (world != 0) {
                    fn_804623EC(world, m_clumps[i]);
                }
            }
        }
        return true;
    }
    }
    fn_8040CFF8(&m_position, &Ref(fn_80093230(&m_velocity, dt)));
    m_rotation.x += dt * m_spin.x;
    m_rotation.y += dt * m_spin.y;
    m_rotation.z += dt * m_spin.z;
    Mtx34f matrix;
    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_position);
    fn_8040B80C(&matrix, m_rotation.z);
    fn_8040B848(&matrix, m_rotation.y);
    fn_8040B884(&matrix, m_rotation.x);
    for (i = 0; i < 2; i++) {
        if (m_frames[i] != 0) {
            RwMatrix *modelling = &m_frames[i]->modelling;
            modelling->right.x = matrix.values[0][0];
            modelling->right.y = matrix.values[1][0];
            modelling->right.z = matrix.values[2][0];
            modelling->up.x = matrix.values[0][1];
            modelling->up.y = matrix.values[1][1];
            modelling->up.z = matrix.values[2][1];
            modelling->at.x = matrix.values[0][2];
            modelling->at.y = matrix.values[1][2];
            modelling->at.z = matrix.values[2][2];
            modelling->pos.x = matrix.values[0][3];
            modelling->pos.y = matrix.values[1][3];
            modelling->pos.z = matrix.values[2][3];
            modelling->pad3 = 0;
            modelling->pad2 = 0;
            modelling->pad1 = 0;
            modelling->flags = 0;
            fn_8047EBA0(modelling);
            fn_8048722C(m_frames[i]);
        }
    }
    if (m_fade != 0) {
        m_fade->m_color[3] = m_alpha;
    }
    if (m_collider) {
        /* Slot 0x3C reads a position from the start of what it is given. */
        m_collider.px->SetCharInfo((CharInfo *)&m_position);
        m_collider.px->SetRotation(&m_rotation);
    }
    if (m_target) {
        m_target->fn_800CCE38(&m_position);
    }
    return false;
}

void DevilDoomCtrlUnit_PsychoObject::OnHit(AttackCallbackParam &param) {
    boost::weak_ptr<CharaColliAttack> attack(Param(param).m_attacker);
    fn_800091D0(&Param(param).m_offset, &Ref(((VectorSubtractFn)fn_8040CD74)(
                                            &Param(param).m_position, &m_owner->m_chara->m_position)));
    Param(param).m_damage = attack.lock()->m_power;
    m_node.m_state = 6;
}

void DevilDoomCtrlUnit_PsychoObject::OnDamage(AttackCallbackParam &param) {
    param.m_result = 1;
    m_health -= param.m_damage;
    if (m_health <= 0.0f) {
        fn_8016F0CC(fn_80079808()->m_score, 1, s_objects[m_node.m_type].m_score);
        boost::shared_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588).lock();
        if (player) {
            CharCommand *command = (CharCommand *)CharCommand::operator new(0x20);
            if (command != 0) {
                command = fn_8015F760(command, 0, s_objects[m_node.m_type].m_item, 1, 4);
            }
            player->PushCommand(command);
        }
        m_node.m_state = 6;
    }
}

Fn8040CopyVector3 DevilDoomCtrlUnit_PsychoObject::GetHome() {
    Mtx34f matrix;
    Vec3f offset(0.0f, 0.0f, -m_distance);
    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_owner->m_chara->m_position);
    fn_8040B848(&matrix, m_owner->m_angle + m_node.m_angle);
    fn_800091D0(&offset, &Ref(fn_8040B67C(&matrix, &offset)));
    offset.y += m_height;
    offset.y += m_amplitude * std::sin(2.0f * (lbl_805FAD38 * ((m_time + m_phase) / m_period)));
    return *(Fn8040CopyVector3 *)&offset;
}

void DevilDoomCtrlUnit_PsychoObject::Float(float dt) {
    Vec3f offset;
    fn_800091EC(&offset, &Ref(((VectorSubtractFn)fn_8040CD74)(Ptr(GetHome()), &m_position)));
    Vec3f acceleration;
    fn_800091EC(&acceleration, &Ref(fn_8040CDA8(&offset, m_owner->m_settings.field_0x68)));
    fn_8040CE04(&acceleration, &Ref(fn_800091A4(&m_velocity, m_owner->m_settings.field_0x6C)));
    fn_8040CFF8(&m_velocity, &Ref(fn_800091A4(&acceleration, dt)));
}

void DevilDoomCtrlUnit_PsychoObject::Fall(float dt) {
    Vec3f acceleration;
    fn_800091EC(&acceleration, &Vec3f(0.0f, m_owner->m_settings.field_0x7C, 0.0f));
    fn_8040CE04(&acceleration, &Ref(fn_800091A4(&m_velocity, m_owner->m_settings.field_0x6C)));
    fn_8040CFF8(&m_velocity, &Ref(fn_800091A4(&acceleration, dt)));
}
