/* BkLarvaGenerator and BkLarva (class names from RTTI; their s_taskNames are "BkLarvaGenerator"
 * and "BkLarva"): the Black Arms larvae, BK_LARVA in the stage editor (enemy/BkLarvaData.one).
 * The generator is a SetEnemyBase (Game/BkLarva.h) that waits until one of the blocks at its
 * position is active, then spawns Num larvae at random points of its area (AppearType NORMAL, or
 * DROP from Offset_Y above); it ends once all of them are gone and remembers on its placement
 * record how many are left. A larva hops about the area, pushed apart from the larvae it touches,
 * until an attack throws it into the air or a vacuum takes it. File name inferred from the classes.
 * Names marked "(name inferred)" and all member names are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). The unit's code reaches
 * its .data and .rodata through pooled base addresses, so the linker keeps those sections whole:
 * Game/CharaColliBaseCharInfo.h and Game/TNumEdit.h supply the RTTI base lists the .data starts
 * with (as in BkSoldier.cpp). */

#include <stdio.h>
#include <string.h>

/* This unit's .rodata is kept whole, with unreferenced constant blocks of the original headers in
 * it (as in BkSoldier.cpp): seven zero words before the CharaColliBase names, and the unit axes
 * and a zero vector after the TNumEdit names (the axes are used below). */
static const float s_zeros[7] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
/* CharaColliBaseCharInfo.h before TNumEdit.h: the order of the casts sets the order of the RTTI
 * base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/TNumEdit.h"
extern "C" {
typedef struct RwV3d {
    float x;
    float y;
    float z;
} RwV3d;
}
static const RwV3d s_xAxis = {1.0f, 0.0f, 0.0f};
static const RwV3d s_yAxis = {0.0f, 1.0f, 0.0f};
static const RwV3d s_zAxis = {0.0f, 0.0f, 1.0f};
static const float s_zeroVector[3] = {0.0f, 0.0f, 0.0f};

#define ENEMY_AREA_PARTS
#include "Game/BkLarva.h"
#include "Effect/System/EffectElement.h"
#include "Game/CharCommand.h"
#include "Game/CharaColliAttack.h"
#include "Game/EnemyCharInfo.h"
#include "Game/EnemyRender.h"
#include "Game/Gadget.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetCollision.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/Random.h"
#include "Game/ResourceOneFile.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <boost/bind.hpp>

using Effect::System::EffectElement;
using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

/* The parts of RenderWare used here (rw/rwcore.h; only the members used are laid out). */
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

struct RwFrame {
    /* 0x00 */ u8 object[0x10];
    /* 0x10 */ RwMatrix modelling;
};

/* The cursor a parameter's label callback receives (layout as in Helicopter.cpp). */
class UnkLabelQuery_800CBC20 {
  public:
    /* 0x0 */ const GadgetParamDesc *params;
    /* 0x4 */ s32 index;
    /* 0x8 */ s32 value;
};

struct _stMtnPlayWork;
class UnkBlockSet_8003AAB0;
class UnkCharaColliObject_8001E9C4;

/* A node of a collider's contact list as Separate reads it (layout unknown beyond the object). */
struct UnkColliContactNode_800587A0 {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ UnkCharaColliObject_8001E9C4 *m_object;
};

/* The object of a contact: its radius at 0x1C (name inferred). */
class UnkCharaColliObject_8001E9C4 {
  public:
    /* 0x00 */ u8 field_0x0[0x1C];
    /* 0x1C */ float m_radius;
};

extern "C" {
/* The resource loader (see BkSoldier.cpp) and its state for slot (3 when loaded; behaviour
 * inferred). */
void *fn_800122D4(void);
s32 fn_80012408(void *loader, s32 slot);
void *fn_80012684(void *loader, int slot, int kind);
int fn_80012608(void *loader, int slot, int kind);
void fn_800127F8(void *loader, int slot);
void fn_8001299C(void *loader, int slot, ResourceOneFile *resource);
/* Whether the stage is running (behaviour inferred). */
u8 fn_80177CF8(void);
/* The block sets (see BlockChange.cpp): the blocks at position (it allocates the list and returns
 * its length), and whether block id is active (behaviours inferred). */
extern UnkBlockSet_8003AAB0 *lbl_8051CE88[4];
s32 fn_8003A380(UnkBlockSet_8003AAB0 *blocks, s16 **ids, const Vec3f *position);
bool fn_8003A4FC(UnkBlockSet_8003AAB0 *blocks, s32 id);
/* The placement record: the character info it hands out and a notification, its flags and the
 * end of the enemy (behaviours inferred). */
void fn_800C97F8(UnkPlacement_800C9CD0 *owner, const boost::shared_ptr<CharInfo> &charInfo);
void fn_800C985C(UnkPlacement_800C9CD0 *owner);
void fn_800C9E9C(UnkPlacement_800C9CD0 *owner, u32 mask);
void fn_800C9EB0(UnkPlacement_800C9CD0 *owner, u32 mask);
void fn_800C9B04(UnkPlacement_800C9CD0 *owner);
/* Removes the task's children, and counts them (behaviours inferred). */
void fn_8004EE88(Task *task);
void fn_8004EAEC(Task *task, int *count);
/* The area of the generator: setting it up from the placement, and keeping a character inside
 * it (behaviours inferred). */
void fn_801A69FC(UnkEnemyArea_8018A374 *area, const Vec3f *position, const Vec3f *rotation,
                 const BkLarvaParams *params);
bool fn_801A69A0(UnkEnemyArea_8018A374 *area, CharInfo *charInfo);
/* The motions the larvae share (see BossCtrlUnit.h). */
_stMtnPlayWork *fn_8041B908(void *data, int arg1);
void fn_8041B884(_stMtnPlayWork *work);
void fn_80416E20(_stMtnPlayWork *work, int arg1, float frame);
void fn_80416DC8(_stMtnPlayWork *work, int arg1, float dt);
void fn_80417568(_stMtnPlayWork *work, u16 motion);
void fn_80415EA4(_stMtnPlayWork *work);
Mtx34f *fn_80415E70(_stMtnPlayWork *work, int node);
/* The characters (see BkWorm.cpp). */
boost::weak_ptr<CharInfo> fn_800A8B74(UnkCharaList_80074194 *manager, int id);
/* Matrices and vectors (see BkSoldier.cpp and ThrowBomb.cpp). */
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Mtx34f fn_8040BEF8(const Mtx34f *matrix);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
Vec3f fn_800091A4(const Vec3f *vector, float scale);
Vec3f fn_8040D0AC(const Vec3f *v, float angle);
void fn_8040CE38(Vec3f *v, float scale);
float fn_8040CE74(Vec3f *vector);
void fn_8040CE60(Vec3f *vector);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
float fn_80016260(float x, float z);
void fn_80016390(RwMatrix *out, const Mtx34f *matrix);
Mtx34f fn_800635A4(const RwMatrix *matrix);
/* RenderWare: RwMatrixTranslate, RwMatrixRotate, RwFrameUpdateObjects and
 * RwV3dTransformPoints (behaviours inferred). */
RwMatrix *fn_8047F59C(RwMatrix *matrix, const RwV3d *translation, int combine);
RwMatrix *fn_8047EFC8(RwMatrix *matrix, const RwV3d *axis, float angle, int combine);
RwFrame *fn_8048722C(RwFrame *frame);
void fn_80482458(RwV3d *out, const RwV3d *in, const RwMatrix *matrix);
/* The model (see ThrowBomb.cpp), its clump, and showing and hiding a clump (behaviours
 * inferred). */
void fn_801A3CA4(UnkModel_801A3E9C *model, int id, int arg2, int arg3);
RwFrame *fn_801A3750(UnkModel_801A3E9C *model);
void *fn_801A378C(UnkModel_801A3E9C *model);
void fn_80038FC0(void *clump);
void fn_80038F94(void *clump);
/* The collider manager, a sphere collider and the attack collision of a collider (see
 * ThrowBomb.cpp), and the contacts (see Game/GadgetCollision.h). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7DEC(void *manager, const boost::weak_ptr<CharInfo> &owner,
                                              int flags, int arg3, int arg4, const Vec3f &offset,
                                              float radius);
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager,
                                              boost::shared_ptr<CharaColliBase> collider);
UnkColliContactNode_800587A0 *fn_8001EE04(UnkColliContacts_8001EE04 *contacts);
UnkColliContactNode_800587A0 *fn_8001EDEC(UnkColliContacts_8001EE04 *contacts);
void fn_80027398(UnkColliContactNode_800587A0 **it);
u32 fn_80026588(UnkColliContactNode_800587A0 *node);
const Vec3f *fn_8001E9C4(UnkCharaColliObject_8001E9C4 *object);
/* The target of a character info (see ArkCannon.cpp). */
boost::shared_ptr<Target> fn_801E7128(const boost::weak_ptr<CharInfo> &owner, int flags);
/* Sounds (see Game/GadgetSound.h and ElecBomb.cpp), and whether the looping sound plays. */
void *fn_80053164(void);
void fn_801D54F4(void *manager, int sound, const CharInfo *source, int arg3, int arg4);
UnkSoundHandle_801D3DC8 fn_801D3C38(int soundId, const CharInfo *source, void *owner);
u32 fn_8020D1E8(void);
boost::shared_ptr<UnkSound_802B4254> fn_802B4254(int kind, int arg1);
void fn_802B4510(UnkSound_802B4254 *sound, u8 flag);
bool fn_802B4548(UnkSound_802B4254 *sound);
void fn_802B44C8(UnkSound_802B4254 *sound, const Vec3f *position);
/* Effects at a position with a rotation and at a matrix (see BkWorm.cpp). */
boost::weak_ptr<EffectElement> fn_801A1F54(const Vec3f *position, const Vec3f *rotation, int id);
boost::weak_ptr<EffectElement> fn_801A1ED8(const Mtx34f *matrix, int id);
/* The enemy registry (see Game/Enemy.h) and its kill report (behaviour inferred). */
void fn_801A2150(void *registry, UnkPlacement_800C9CD0 *owner, int arg2, int result);
/* Rewards the attacker, and whether it was the player (behaviours inferred). */
void fn_8020642C(const boost::shared_ptr<CharInfo> &attacker, int reward);
bool fn_8021C958(const boost::shared_ptr<CharInfo> &attacker);
/* The vacuum command's owner, and the vacuum taking the enemy's model (behaviours inferred). */
boost::weak_ptr<CharInfo> fn_801DADFC(VacuumCommand *command);
void fn_8028CFC4(VacuumCommand *command, const boost::shared_ptr<EnemyRender> &render,
                 const Vec3f *position, const Vec3f *rotation, int arg4);
boost::shared_ptr<EnemyRender> fn_802CDB08(int slot, const EnemyMotionTable &table, void *model,
                                           void *motion);
void fn_802CD68C(EnemyRender *render, int arg1, float arg2);
/* The debug print. */
void fn_803A953C(const char *format, ...);
}

extern s32 lbl_805EE588;
/* The game's physics parameter tables (gravity at 0x114; meanings inferred). */
extern u8 lbl_804CC424[];
extern u8 lbl_804CC604[];

/* Target (class name from RTTI; see EnemyTarget.cpp). */
class Target {
  public:
    void fn_800CCE38(const Vec3f *position);

    void Activate() {
        m_flags |= 1;
    }
    void Deactivate() {
        m_flags &= ~1;
    }

    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ u32 m_flags;
};

/* fn_8040CD74 called as returning a - b by value (see ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* Out-of-line vector assignment and addition taking references (not original; see
 * JumpBoard.cpp). */
inline void AssignVector(Vec3f &dst, const Vec3f &src) {
    fn_800091D0(&dst, &src);
}
inline void AddVector(Vec3f &dst, const Vec3f &value) {
    fn_8040CFF8(&dst, &value);
}

/* Radians to degrees (not original). */
inline float RadToDeg(float radians) {
    return radians * (180.0f / lbl_805FAD38);
}

/* A motion table built in place (not original; as in BkSoldier.cpp). */
struct MotionTable : public EnemyMotionTable {
    MotionTable(const EnemyMotionEntry *e, s32 c) {
        entries = e;
        count = c;
    }
};

/* The vectors an attack passes (offsets from OnAttacked; names inferred). */
#define HIT_POSITION(param) (*reinterpret_cast<Vec3f *>(&(param).field_0x8[0x18 - 0x8]))
#define HIT_ROTATION(param) (*reinterpret_cast<Vec3f *>(&(param).field_0x8[0x24 - 0x8]))
#define HIT_DIRECTION(param) (*reinterpret_cast<Vec3f *>(&(param).field_0x8[0x34 - 0x8]))
/* The enemy character info's push (EnemyCharInfo's field_0x9C). */
#define PUSH(chara) (reinterpret_cast<EnemyCharInfo *>(chara)->field_0x9C)

/* The motions all larvae play, and the number of generators (names inferred). */
static _stMtnPlayWork *s_motions[5];
static s32 s_generators;
static u32 s_lastFrame;

/* Out of line, but inlined into ~BkLarvaGenerator as retail has it: #pragma auto_inline makes
 * GC/2.7 inline it there and emit the one copy here, after Tick (an inline destructor would also
 * get a weak copy right after the generator's constructor, which retail does not have). */
#pragma auto_inline on
SetEnemyBase::~SetEnemyBase() {
    if (m_blocks != 0) {
        fn_80049B34(m_blocks);
    }
    if (m_owner != 0) {
        fn_800C9BE8(m_owner);
        m_owner = 0;
    }
}
#pragma auto_inline reset

/* Out of line as well (retail has it here, where the generator's vtable does not pull it to the
 * front of the unit as it would an inline function). */
void SetEnemyBase::Tick(float dt) {
    if (m_owner != 0) {
        if (fn_800C9A64(m_owner)) {
            m_flags |= 1;
            return;
        }
        if (fn_800C996C(m_owner)) {
            m_flags |= 1;
            return;
        }
    }
    Behave(dt);
}

const char *BkLarva::s_taskName = "BkLarva";
const char *SetEnemyBase::s_taskName = "SetEnemyBase";

/* The motion table of the model a vacuum takes away (in .sdata2). */
static const EnemyMotionEntry lbl_805F5198[] = {{3, -1}};

BkLarva::BkLarva(BkLarvaGenerator *generator, s32 index) : Task(generator), m_generator(generator) {
    m_name = s_taskName;
    m_index = index;
    m_motion = index % 5;
    m_timer = fn_8040EE90();
    m_chara = fn_801A0030(0xA2);
    fn_8019FD64(reinterpret_cast<EnemyCharInfo *>(m_chara.get()),
                boost::bind(&BkLarva::OnCommand, this, _1));
    m_target = fn_801E7128(boost::weak_ptr<CharInfo>(m_chara), 0x20C80);
    m_target->Activate();
    if (fn_8020D1E8() & 4) {
        m_loop = fn_802B4254(2, 0);
        if (m_loop.get() != 0) {
            fn_802B4510(m_loop.get(), 1);
        }
    }
    fn_801A3CA4(&m_model, 0x10, 3, 0);
    /* A named node: retail moves the call's result through r0. */
    Mtx34f *node = fn_80415E70(s_motions[m_motion], 10);
    fn_80016390(reinterpret_cast<RwMatrix *>(m_frame), node);
    Place();
    m_collider = fn_800C7DEC(fn_80021B08(), boost::weak_ptr<CharInfo>(m_chara), 0, 5, 0,
                             Vec3f(0.0f, 6.0f, 0.0f), 6.0f);
    m_collider->SetField2C(1);
    boost::weak_ptr<CharaColliAttack> attack(fn_800C4374(fn_80021B08(), m_collider));
    if (boost::shared_ptr<CharaColliAttack> attackPtr = attack.lock()) {
        attackPtr->m_power = 1.0f;
        attackPtr->m_kind = 1;
        *reinterpret_cast<s32 *>(attackPtr->field_0x3C) = 1;
        attackPtr->m_flags = 1;
        attackPtr->field_0x54 = 1;
        attackPtr->m_hitCallback = AttackCallback(boost::bind(&BkLarva::OnHit, this, _1));
        attackPtr->field_0x55 = 1;
        attackPtr->m_attackCallback = AttackCallback(boost::bind(&BkLarva::OnAttacked, this, _1));
    }
    if (m_generator->m_params.AppearType == 1) {
        fn_80038FC0(fn_801A378C(&m_model));
        if (m_loop.get() != 0) {
            fn_802B4510(m_loop.get(), 0);
        }
        m_falling = 1;
        m_state = 0;
    } else {
        m_falling = 0;
        m_state = 2;
    }
}

BkLarva::~BkLarva() {
    if (m_sound.m_id != 0) {
        m_generator->m_soundCount--;
        m_sound.fn_801D455C();
    }
}

void BkLarva::Place() {
    float range = m_generator->m_params.AppearRange;
    float distance = range * fn_8040EE90();
    float height = 0.0f;
    if (m_generator->m_params.AppearType == 1) {
        height = m_generator->m_params.Offset_Y;
    }
    Vec3f offset(distance, height, 0.0f);
    AssignVector(offset, fn_8040D0AC(&offset, fn_8040EC80(0.0f, 2.0f * lbl_805FAD38)));
    AssignVector(offset, fn_8040B67C(&m_generator->m_matrix, &offset));
    /* Bound to a reference: retail calls fn_8040CDD0 before it loads m_chara. */
    const Vec3f &position = fn_8040CDD0(&m_generator->m_position, &offset);
    fn_800091D0(&m_chara->m_position, &position);
    m_chara->m_rotation = m_generator->m_rotation;
    fn_8040CE60(&m_chara->field_0x18);
}

void BkLarva::Move(float dt) {
    Vec3f previous;
    fn_800091EC(&previous, &m_chara->m_position);
    AddVector(m_chara->m_position, fn_800091A4(&m_chara->field_0x18, dt));
    AddVector(m_chara->m_position, PUSH(m_chara.operator->()));
    fn_8040CE60(&PUSH(m_chara.get()));
    if (m_falling) {
        m_chara->field_0x18.y -= *reinterpret_cast<float *>(&lbl_804CC424[0x114]) * dt;
    }
    if (!fn_801A69A0(&m_generator->m_area, m_chara.get())) {
        fn_800091D0(&m_chara->m_position, &previous);
    }
    /* Copied through a plain pointer: Vec3f's inline copy would call the shared_ptr's operator->
     * out of line. */
    UnkEnemyInfo_8017D014 *chara = m_chara.get();
    Vec3f position = chara->m_position;
    Mtx34f *node = fn_80415E70(s_motions[m_motion], 10);
    fn_80016390(reinterpret_cast<RwMatrix *>(m_frame), node);
    RwMatrix *matrix = &fn_801A3750(&m_model)->modelling;
    *matrix = *reinterpret_cast<RwMatrix *>(m_frame);
    Vec3f lift(0.0f, -2.0f, 0.0f);
    fn_8047F59C(matrix, reinterpret_cast<RwV3d *>(&lift), 2);
    fn_8047EFC8(matrix, &s_zAxis, RadToDeg(m_chara->m_rotation.z), 2);
    fn_8047EFC8(matrix, &s_xAxis, RadToDeg(m_chara->m_rotation.x), 2);
    fn_8047EFC8(matrix, &s_yAxis, RadToDeg(m_chara->m_rotation.y), 2);
    fn_8047F59C(matrix, reinterpret_cast<RwV3d *>(&position), 2);
    fn_8048722C(fn_801A3750(&m_model));
    Vec3f top(0.0f, 5.0f, 0.0f);
    fn_80482458(reinterpret_cast<RwV3d *>(&top), reinterpret_cast<RwV3d *>(&top), matrix);
    Vec3f targetPosition = top;
    m_target->fn_800CCE38(&targetPosition);
}

void BkLarva::Tick(float dt) {
    switch (m_state) {
    case 0:
        if (!m_generator->m_playerInArea) {
            return;
        }
        if (m_timer < 0.0f) {
            fn_80038F94(fn_801A378C(&m_model));
            if (m_loop.get() != 0) {
                fn_802B4510(m_loop.get(), 1);
            }
            m_state = 1;
        }
        break;
    case 1:
        Separate();
        if (m_chara->m_position.y < m_generator->m_position.y) {
            m_chara->m_position.y = m_generator->m_position.y;
            m_chara->field_0x18.y = 0.0f;
            m_timer = fn_8040EC80(1.0f, 2.0f);
            m_falling = 0;
            m_state = 2;
        }
        break;
    case 2:
        Separate();
        if (m_timer < 0.0f) {
            m_timer = fn_8040EC80(1.0f, 2.0f);
            if (m_generator->m_playerInArea) {
                boost::weak_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588);
                boost::shared_ptr<CharInfo> playerPtr = player.lock();
                Vec3f direction(0.0f, 0.0f, 0.0f);
                if (playerPtr.get() != 0) {
                    fn_800091D0(&direction, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                                &playerPtr->m_position, &m_chara->m_position));
                }
                AssignVector(direction, fn_8040B67C(&m_generator->m_inverse, &direction));
                direction.y = 0.0f;
                fn_8040CE74(&direction);
                float minSpeed = m_generator->GetBaseSpeed();
                float speed = fn_8040EC80(minSpeed, m_generator->GetMaxSpeed());
                AssignVector(direction, fn_8040CDA8(&direction, speed));
                AssignVector(direction,
                             fn_8040D0AC(&direction, fn_8040EC80(-10.0f * (lbl_805FAD38 / 180.0f),
                                                                 10.0f * (lbl_805FAD38 / 180.0f))));
                const Vec3f &velocity = fn_8040B67C(&m_generator->m_matrix, &direction);
                fn_800091D0(&m_chara->field_0x18, &velocity);
            } else {
                Vec3f velocity(fn_8040EC80(m_generator->GetMinSpeed(), m_generator->GetMaxSpeed()),
                               0.0f, 0.0f);
                AssignVector(velocity,
                             fn_8040D0AC(&velocity, lbl_805FAD38 * (2.0f * fn_8040EE90())));
                const Vec3f &result = fn_8040B67C(&m_generator->m_matrix, &velocity);
                fn_800091D0(&m_chara->field_0x18, &result);
            }
            if (m_generator->m_soundCount < 3) {
                m_generator->m_soundCount++;
                m_sound = fn_801D3C38(0x6024, m_chara.get(), this);
            }
            m_state = 3;
        }
        break;
    case 3:
        Separate();
        if (m_timer < 0.0f) {
            m_timer = fn_8040EC80(0.5f, 1.0f);
            fn_8040CE60(&m_chara->field_0x18);
            if (m_sound.m_id != 0) {
                m_generator->m_soundCount--;
                m_sound.fn_801D455C();
            }
            m_state = 2;
        }
        break;
    case 4:
        m_timer = 0.5f;
        m_falling = 1;
        m_target->Deactivate();
        m_collider->TurnOff(-1);
        if (m_loop.get() != 0) {
            fn_802B4510(m_loop.get(), 0);
        }
        m_spinX = lbl_805FAD38 * (2.0f * (1.0f + fn_8040EE90()));
        m_spinZ = lbl_805FAD38 * (0.2f * (fn_8040EE90() - 0.5f));
        m_generator->m_remaining--;
        m_state = 5;
        break;
    case 5:
        if (m_timer < 0.0f) {
            RwMatrix *frame = &fn_801A3750(&m_model)->modelling;
            if (frame != 0) {
                Mtx34f matrix;
                fn_8040C148(&matrix, &(const Mtx34f &)fn_800635A4(frame));
                fn_801D54F4(fn_80053164(), 0x6009, m_chara.get(), 0, 1);
                fn_801A1ED8(&matrix, 0x78);
            }
            m_flags |= 1;
            return;
        }
        m_chara->m_rotation.x += m_spinX * dt;
        m_chara->m_rotation.z += m_spinZ * dt;
        if (*reinterpret_cast<u32 *>(&lbl_8057D6A0[0x90]) & 1) {
            fn_80038FC0(fn_801A378C(&m_model));
        } else {
            fn_80038F94(fn_801A378C(&m_model));
        }
        break;
    }
    Move(dt);
    if (m_loop.get() != 0 && fn_802B4548(m_loop.get())) {
        const Vec3f &position =
            fn_8040CDD0(&m_chara->m_position, &(const Vec3f &)Vec3f(0.0f, 10.0f, 0.0f));
        fn_802B44C8(m_loop.get(), &position);
    }
    m_timer -= dt;
}

/* An entry of the larvae a larva touches: how hard it pushes and from where (struct name
 * inferred). */
struct UnkLarvaContact {
    /* 0x0 */ float m_weight;
    /* 0x4 */ Vec3f m_offset;
};

void BkLarva::Separate() {
    CharaColliBase *collider = m_collider.get();
    if (collider != 0) {
        UnkColliContacts_8001EE04 *contacts =
            reinterpret_cast<UnkColliContacts_8001EE04 *>(reinterpret_cast<u8 *>(collider) + 0xC);
        if (contacts != 0 && contacts->m_count > 0) {
            UnkColliContactNode_800587A0 *it = fn_8001EE04(contacts);
            static UnkLarvaContact s_contacts[16];
            /* Retail has the guard of an initialized static but no initial store: the store below
             * makes it dead. */
            static s32 s_count = 0;
            s_count = 0;
            float radius;
            float total = 0.0f;
            for (; it != fn_8001EDEC(contacts); fn_80027398(&it)) {
                UnkCharaColliObject_8001E9C4 *object = it->m_object;
                if (object != 0 && fn_80026588(it) != 0 && !(fn_80026588(it) & 0x140000)) {
                    Vec3f offset;
                    fn_800091EC(&offset, fn_8001E9C4(object));
                    AssignVector(offset, fn_8040B67C(&m_generator->m_inverse, &offset));
                    offset.y = 0.0f;
                    radius = object->m_radius;
                    fn_8040CE38(&offset, -radius);
                    AssignVector(offset, fn_8040B67C(&m_generator->m_matrix, &offset));
                    total += radius;
                    s_contacts[s_count].m_weight = radius;
                    fn_800091D0(&s_contacts[s_count].m_offset, &offset);
                    if (++s_count >= 16) {
                        s_count = 15;
                    }
                }
            }
            if (s_count != 0) {
                for (int i = 0; i < s_count; i++) {
                    float weight = s_contacts[i].m_weight / total;
                    AddVector(PUSH(m_chara.operator->()),
                              fn_800091A4(&(const Vec3f &)fn_800091A4(&s_contacts[i].m_offset,
                                                                      weight),
                                          0.5f));
                }
            }
        }
    }
}

void BkLarva::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0x14: {
        VacuumCommand *vacuum = static_cast<VacuumCommand *>(&command);
        void *loader = fn_800122D4();
        void *model = fn_80012684(loader, 0x10, 4);
        void *motion = reinterpret_cast<void *>(fn_80012608(loader, 0x10, 5));
        boost::shared_ptr<EnemyRender> render =
            fn_802CDB08(0x10, MotionTable(lbl_805F5198, 1), model, motion);
        if (render.get() != 0) {
            fn_802CD68C(render.get(), 1, 30.0f);
            fn_8028CFC4(vacuum, render, &m_chara->m_position, &m_chara->m_rotation, 1);
        }
        int result = 1;
        if (boost::shared_ptr<CharInfo> attacker = fn_801DADFC(vacuum).lock()) {
            fn_8020642C(attacker, 0x5F);
            if (fn_8021C958(attacker)) {
                result |= 4;
            }
        }
        fn_801A2150(fn_80119178(), m_generator->m_owner, 2, result);
        m_generator->m_remaining--;
        m_flags |= 1;
        break;
    }
    }
    command.Flags() |= 2;
    if (!(command.Flags() & 2)) {
        command.Reject(boost::weak_ptr<CharInfo>(m_chara));
    }
    if (!(command.Flags() & 2)) {
        fn_803A953C("Command isn't Received ! : (%d)%s\n",
                    *reinterpret_cast<const u16 *>(command.field_0x4), command.GetName());
    }
}

void BkLarva::OnHit(AttackCallbackParam &param) {
    boost::shared_ptr<CharInfo> attacker = param.m_attacker.lock()->m_owner.lock();
    fn_800091D0(&HIT_DIRECTION(param), &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                           &HIT_POSITION(param), &attacker->m_position));
}

void BkLarva::OnAttacked(AttackCallbackParam &param) {
    param.m_result = 1;
    Vec3f velocity(0.0f, 0.0f, 0.0f);
    if (m_generator->m_up.y > 0.8f) {
        fn_800091D0(&velocity, &HIT_DIRECTION(param));
        velocity.y = 0.0f;
        fn_8040CE38(&velocity, *reinterpret_cast<float *>(&lbl_804CC604[0x6C]));
        velocity.y = 0.5f * (*reinterpret_cast<float *>(&lbl_804CC424[0x114]) *
                             *reinterpret_cast<float *>(&lbl_804CC604[0x70]));
    } else if (m_generator->m_up.y >= -0.001f) {
        fn_800091D0(&velocity, &m_generator->m_up);
        fn_8040CE38(&velocity, 20.0f);
    }
    fn_800091D0(&m_chara->field_0x18, &velocity);
    m_chara->m_rotation.y = fn_80016260(HIT_DIRECTION(param).x, HIT_DIRECTION(param).z);
    if ((int)(2.0f * fn_8040EE90()) != 0) {
        fn_801D54F4(fn_80053164(), 0x6004, m_chara.get(), 0, 1);
    } else {
        fn_801D54F4(fn_80053164(), 0x6005, m_chara.get(), 0, 1);
    }
    fn_801A1F54(&HIT_POSITION(param), &HIT_ROTATION(param), 0x6C);
    int result = 1;
    boost::shared_ptr<CharaColliAttack> attacker = param.m_attacker.lock();
    if (attacker.get() != 0) {
        boost::shared_ptr<CharInfo> owner = attacker->m_owner.lock();
        if (owner.get() != 0) {
            fn_8020642C(owner, 0x5F);
            if (fn_8021C958(owner)) {
                result |= 4;
            }
        }
    }
    fn_801A2150(fn_80119178(), m_generator->m_owner, 2, result);
    m_state = 4;
}

const char *BkLarvaGenerator::s_taskName = "BkLarvaGenerator";

BkLarvaGenerator::BkLarvaGenerator(Task *parent, UnkPlacement_800C9CD0 *owner)
    : SetEnemyBase(parent, owner), m_rotation(0.0f, 0.0f, 0.0f), field_0x13C(20.0f), m_spawned(0),
      m_soundCount(0) {
    m_name = s_taskName;
    m_chara = fn_801A0030(0xA2);
    fn_800C97F8(m_owner, boost::shared_ptr<CharInfo>(m_chara));
    fn_800C985C(m_owner);
    fn_8019FD64(reinterpret_cast<EnemyCharInfo *>(m_chara.get()),
                boost::bind(&BkLarvaGenerator::OnCommand, this, _1));
    m_state = 0;
    m_playerInArea = 0;
    if (s_generators == 0) {
        void *loader = fn_800122D4();
        void *data = fn_80012684(loader, 0x10, 4);
        u16 motion = fn_80012608(loader, 0x10, 5);
        for (int i = 0; i < 5; i++) {
            _stMtnPlayWork *work = fn_8041B908(data, 1);
            if (work != 0) {
                fn_80416E20(work, 0, 12.0f * i);
                fn_80416DC8(work, 0, 1.0f + (0.2f * fn_8040EE90() - 0.1f));
                fn_80417568(work, motion);
                s_motions[i] = work;
            }
        }
    }
    s_generators++;
    ReadPlacement(m_owner);
}

BkLarvaGenerator::~BkLarvaGenerator() {
    SaveState();
    if (m_owner != 0 && m_remaining <= 0) {
        fn_800C9E9C(m_owner, 0x200);
        fn_800C9EB0(m_owner, 0x2000000);
        fn_800C9B04(m_owner);
    }
    fn_8004EE88(this);
    if (--s_generators == 0) {
        for (int i = 4; i >= 0; i--) {
            fn_8041B884(s_motions[i]);
        }
    }
}

void BkLarvaGenerator::SaveState() {
    if (m_owner != 0) {
        if (m_owner->m_adapterState == 0) {
            SBkLarvaStateData *state = new SBkLarvaStateData(m_remaining);
            if (state != 0) {
                fn_800C9930(m_owner, state);
            }
        } else {
            SBkLarvaStateData *state = dynamic_cast<SBkLarvaStateData *>(m_owner->m_adapterState);
            if (state != 0) {
                state->m_remaining = m_remaining;
            }
        }
    }
}

void BkLarvaGenerator::Behave(float dt) {
    s32 index; /* declared here, it gets retail's register */
    SaveState();
    switch (m_state) {
    case 0:
        if (fn_80012408(fn_800122D4(), 0x10) == 3 && fn_80177CF8()) {
            Vec3f position;
            fn_800C9F14(m_owner, &position);
            m_blockCount = fn_8003A380(lbl_8051CE88[0], &m_blocks, &position);
            m_state = 1;
        }
        break;
    case 1: {
        bool on = false;
        for (int i = 0; i < m_blockCount; i++) {
            if (fn_8003A4FC(lbl_8051CE88[0], m_blocks[i])) {
                on = true;
                break;
            }
        }
        if (!on) {
            break;
        }
        if (m_blocks != 0) {
            fn_80049B34(m_blocks);
            m_blocks = 0;
        }
        m_state = 2;
    }
    case 2: {
        index = m_spawned;
        m_spawned++;
        new BkLarva(this, index);
        if (m_spawned >= m_params.Num) {
            m_state = 3;
        }
        break;
    }
    case 3: {
        if (!(*reinterpret_cast<u32 *>(&lbl_8057D6A0[0x90]) & 7)) {
            boost::weak_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588);
            boost::shared_ptr<CharInfo> playerPtr = player.lock();
            if (playerPtr.get() != 0) {
                m_playerInArea = m_area.Contains(playerPtr.get());
            }
        }
        if (s_lastFrame != *reinterpret_cast<u32 *>(&lbl_8057D6A0[0x90])) {
            s_lastFrame = *reinterpret_cast<u32 *>(&lbl_8057D6A0[0x90]);
            float step = 60.0f * dt + 1e-6f;
            for (int i = 0; i < 5; i++) {
                fn_80416DC8(s_motions[i], 0, step);
                fn_80415EA4(s_motions[i]);
            }
        }
        int count = 0;
        fn_8004EAEC(this, &count);
        if (count == 0) {
            m_flags |= 1;
        }
        break;
    }
    }
}

void BkLarvaGenerator::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_8004EE88(this);
    ReadPlacement(owner);
    m_spawned = 0;
    m_state = 0;
}

void BkLarvaGenerator::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_position);
    fn_800C9ED4(owner, &m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    fn_801A69FC(&m_area, &m_position, &m_rotation, &m_params);
    fn_800091D0(&m_chara->m_position, &m_position);
    m_chara->m_rotation = m_rotation;
    if (owner->m_adapterState != 0) {
        SBkLarvaStateData *state = dynamic_cast<SBkLarvaStateData *>(owner->m_adapterState);
        if (state != 0) {
            m_params.Num = state->m_remaining;
        }
    }
    m_remaining = m_params.Num;
    fn_8040B6F8(&m_matrix);
    fn_8040B80C(&m_matrix, m_rotation.z);
    fn_8040B884(&m_matrix, m_rotation.x);
    fn_8040B848(&m_matrix, m_rotation.y);
    fn_800091D0(&m_up,
                &(const Vec3f &)fn_8040B67C(&m_matrix, &(const Vec3f &)Vec3f(0.0f, 1.0f, 0.0f)));
    fn_8040C148(&m_inverse, &(const Mtx34f &)fn_8040BEF8(&m_matrix));
}

void BkLarvaGenerator::OnCommand(CharCommand &command) {
    if (command.TypeId() == 0x19) {
        command.Flags() |= 2;
    }
}

float BkLarvaGenerator::GetMinSpeed() {
    return GetBaseSpeed();
}

extern "C" const char *fn_8018958C(UnkLabelQuery_800CBC20 *query) {
    static const char *s_appearTypes[2] = {"AppearType : NORMAL", "AppearType : DROP"};
    static int s_appearType = 0;

    switch (query->index) {
    case 9:
        s_appearType = query->value;
        return s_appearTypes[query->value];
    case 10:
        if (s_appearType == 1) {
            return "Offset_Y";
        }
        break;
    }
    return "";
}

extern "C" void fn_8018952C(void) {
    void *loader = fn_800122D4();
    fn_8001299C(loader, 0x10, new ResourceOneFile("enemy/BkLarvaData.one", false));
}

extern "C" void fn_80189504(void) {
    fn_800127F8(fn_800122D4(), 0x10);
}

void BkLarvaGenerator::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new BkLarvaGenerator(fn_801EB304(fn_8001C4F8(), 0xE), owner);
}

static GadgetParamDesc lbl_8053063C[] = {
    {4, "MoveRange", 0, 0, 0, 200.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchRange", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchAngle", 0, 0, 0, 0.0f, -360.0f, 360.0f, fn_800CBC20, 0},
    {4, "SearchWidth", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchHeight", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "SearchHeightOffset", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "MoveSpeedRatio", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {1, "Num", 10, 1, 100, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {4, "AppearRange", 0, 0, 0, 30.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {1, "AppearType", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_8018958C, 0},
    {4, "Offset_Y", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_8018958C, 0},
    {0},
};

GadgetClassDesc lbl_8053081C = {
    "BK_LARVA", fn_8018952C, fn_80189504,
    reinterpret_cast<void (*)(UnkPlacement_800C9CD0 *)>(BkLarvaGenerator::Instantiate), 0, 0, 0x91,
    2, 0, 5, lbl_8053063C,
};
