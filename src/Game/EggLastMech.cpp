/* EggLastMech::EggLastMech (class name from RTTI): the boss of the EGG_LASTMECH gadget, an enemy
 * character info (EnemyCharInfo) that an EggLastMech::EggLastMechTask (Game/EggLastMechTask.cpp)
 * creates and runs. It keeps an EnemyRender model, a collider whose attack collision takes the
 * player's hits, a target the player locks on to, an AI state machine (EggLastMechAIStateBase
 * states), and five helper objects defined in other units (one of them a slot machine whose
 * result starts the boss's "fever" states; meanings inferred). Hits lower its health (150): below
 * 75 it changes state, at 0 it ends; the first hit shows a boss hint (EnemyHint). Its motion
 * events fire homing shots at the player. File name inferred from the class. Names marked
 * "(name inferred)" are not original. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). The unit's code reaches its .data through a pooled base
 * address, so the linker keeps that section whole: Game/CharaColliBaseCharInfo.h and
 * Game/TNumEdit.h supply the RTTI base lists it starts with, and the dead weak vtables (exception,
 * bad_weak_ptr, bad_alloc, sp_counted_base and LockonTarget's control block) end it zero-filled. */
/* Keeps the dead copy of bad_alloc's vtable in .data, as retail has it (see <new>). */
#define MSL_NEW_INLINE_BAD_ALLOC
#define BOOST_WEAK_PTR_CONVERTING_ASSIGNMENT

#include <boost/function.hpp>

#include "Game/CharaColliBaseCharInfo.h"
/* After CharaColliBaseCharInfo.h: the order of the casts sets the order of the RTTI base lists in
 * .data. */
#include "Game/TNumEdit.h"
#include "Effect/System/EffectElement.h"
#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/EggLastMech.h"
#include "Game/EnemyCharInfo.h"
#include "Game/EnemyHint.h"
#include "Game/EnemyRender.h"
#include "Game/Gadget.h"
#include "Game/Player.h"
#include "Game/Task.h"
#include "Game/Vibration.h"
#include "Game/WeaponBullet.h"
#include "Game/WeaponLockonTarget.h"
#include "Game/Random.h"
#include "Game/MathTypes.h"
#include "boost/enable_shared_from_this.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"
/* After the shared_ptr headers: retail emits the instances of shared_ptr's function templates
 * before those of boost::bind (CodeWarrior emits them in the order they are declared). */
#include <boost/bind.hpp>

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

/* Target (class name from RTTI): a point characters aim at. The flag word is mapped here (bit 0
 * makes the target active; see Bomb.cpp). */
class Target {
  public:
    /* Makes the target active (name inferred). */
    void TurnOn() {
        m_flags |= 1;
    }
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);

    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ u32 m_flags;
};

namespace Player {
/* PlayerTargetBase (class name from RTTI): the target a player character carries. */
class PlayerTargetBase;
} // namespace Player

/* The target a player character carries (offset as in ArkCannon.cpp; name inferred). */
inline Player::PlayerTargetBase *GetPlayerTarget(Player::PlayerBase *player) {
    return *reinterpret_cast<Player::PlayerTargetBase **>(reinterpret_cast<u8 *>(player) + 0x24C);
}

/* A kind of shot (see ArkCannon.cpp): the function at 0x8 fires one (names inferred). */
struct UnkBulletType_8051DDDC {
    /* 0x00 */ u32 field_0x0;
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ void (*fire)(Weapon::ShotInfo *info);
};
/* EggLastMech's homing shot */
extern UnkBulletType_8051DDDC lbl_8051DDDC;

/* The player's character id (see Helicopter.cpp). */
extern s32 lbl_805EE588;

/* fn_8040CD74 called as returning a - b by value (it stores the result through its first
 * argument; see ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* The save data (see SavePoint.cpp): the record at 0x70C keeps the score (meaning inferred). */
struct UnkObject_80079808 {
    /* 0x000 */ u8 field_0x0[0x70C];
    /* 0x70C */ u8 m_score[1];
};

/* What fn_80178B5C returns (meaning unknown). */
struct UnkRecord_80178B5C {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ u8 field_0x20[1];
};

/* What an attack collision hits with (meaning inferred): kind 0x36 counts as a hit on EggLastMech. */
struct UnkAttackSource {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ u32 kind;
};

/* The parts of AttackCallbackParam EggLastMech reads (layout as in CharaColliAttack.h; the members
 * between are named here, meanings inferred). */
struct EggLastMechAttackParam {
    /* 0x00 */ boost::weak_ptr<CharaColliAttack> m_attacker;
    /* 0x08 */ u8 field_0x8[0x14 - 0x8];
    /* 0x14 */ UnkAttackSource *m_source;
    /* 0x18 */ Vec3f m_position;
    /* 0x24 */ u8 field_0x24[0x30 - 0x24];
    /* 0x30 */ u8 m_byPlayer;
    /* 0x34 */ Vec3f m_offset;
    /* 0x40 */ Vec3f m_direction;
    /* 0x4C */ u8 field_0x4C[0x50 - 0x4C];
    /* 0x50 */ float m_damage;
    /* 0x54 */ s32 m_result;
};
SIZE_ASSERT(EggLastMechAttackParam, 0x58);

inline EggLastMechAttackParam &Param(AttackCallbackParam &param) {
    return reinterpret_cast<EggLastMechAttackParam &>(param);
}

struct UnkEffectParam_80194C1C;

extern "C" {
/* The collision manager, a collider of a model part for a character info, and the attack
 * collision of a collider (see TurnOver.cpp). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7F7C(void *manager, const boost::weak_ptr<CharInfo> &charInfo,
                                              void *part, void *work, int arg4, int arg5);
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager, boost::shared_ptr<CharaColliBase> collider);
/* Removes a collider from the manager (behaviour inferred). */
void fn_800C78B0(void *manager, boost::shared_ptr<CharaColliBase> *collider);
/* Sets the attack collision's callbacks (see TurnOver.cpp and BreakRoad.cpp). */
void fn_80054DD0(CharaColliAttack *attack, int arg1, int arg2, float arg3, const AttackCallback &onHit,
                 const AttackCallback &onEnd);
void fn_80096B64(CharaColliAttack *attack, int arg1, const AttackCallback &callback);
/* The collision data part index of resource slot (behaviour inferred). */
void *fn_8019A890(int slot, int index);
/* Whether the hit is to be ignored (behaviour inferred). */
bool fn_80053538(AttackCallbackParam *param);

/* Creates a target for owner (see ArkCannon.cpp). */
boost::shared_ptr<Target> fn_801E7128(const boost::weak_ptr<CharInfo> &owner, int flags);

/* The character manager and its character by id, and the player's id (see Helicopter.cpp). */
boost::weak_ptr<CharInfo> fn_800A8B74(UnkCharaList_80074194 *manager, int id);
/* The character a player's character info is riding with (behaviour inferred). */
boost::weak_ptr<CharInfo> fn_800B75DC(const boost::weak_ptr<CharInfo> &player);
/* The target of a player's target (see ArkCannon.cpp). */
boost::weak_ptr<Target> fn_80206574(Player::PlayerTargetBase *target);

/* EnemyRender's methods (behaviour inferred). */
boost::shared_ptr<EnemyRender> fn_802CDB08(int slot, const EnemyMotionTable &table, void *model,
                                           void *motion);
void *fn_802CD420(EnemyRender *render);
Vec3f fn_802CD508(EnemyRender *render, int bone);
Mtx34f *fn_802CD560(EnemyRender *render, int bone);
void *fn_802CD588(EnemyRender *render);
void *fn_802CD5AC(EnemyRender *render);
void fn_802CD638(EnemyRender *render, float scale);
void fn_802CD728(EnemyRender *render, const Vec3f *position, const Vec3f *rotation);
void fn_802CD824(EnemyRender *render, float dt);
void fn_802CD7E8(EnemyRender *render);
Vec3f fn_802CD450(EnemyRender *render);
void fn_802CD270(EnemyRender *render, const EnemyMotionCallback &callback);
/* Motion and model helpers (behaviour inferred). */
void fn_801A46E4(void *motion, int arg1);
void fn_801A4898(void *motion, int id, int arg2, float arg3);
void *fn_801A3634(void *model, int index);
void fn_801A35AC(void *model, void *part, int arg2);
/* The resource loader, and the model and motion files of a resource slot (behaviour inferred). */
void *fn_800122D4(void);
void *fn_80012684(void *loader, int slot, int index);
void *fn_80012608(void *loader, int slot, int index);

/* Vector helpers (see Bomb.cpp and MathTypes.h). */
Vec3f fn_800091A4(const Vec3f *vector, float scale);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
void fn_8040CE38(Vec3f *vector, float scale);
void fn_8040CE60(Vec3f *vector);
/* The translation of matrix (behaviour inferred). */
void fn_8040FAA0(Vec3f *translation, const Mtx34f *matrix);
Vec3f fn_8040B618(const Mtx34f *matrix, const Vec3f *point);
void fn_804103E8(Vec3f *position, Vec3f *velocity, const Vec3f *acceleration,
                 float scale, float dt);
/* The pitch and yaw of direction (see UpDownReel.cpp). */
void fn_8003B9E8(const Vec3f *direction, float *pitch, float *yaw);

/* Sound and effects (see BreakObj.cpp). */
void *fn_80053164(void);
void fn_801D54F4(void *manager, int id, const Vec3f *position, int arg3, int arg4);
void fn_801D560C(void *sound, int id, int arg2, int arg3);
boost::weak_ptr<Effect::System::EffectElement> fn_801D2AFC(int arg0, const Vec3f *position,
                                                           const Vec3f *angles, int id,
                                                           void *arg4);
/* Launches the effect param describes on a task layer (behaviour inferred). */
void fn_80209B58(Task *layer, int arg1, const UnkEffectParam_80194C1C *param);
/* Vibrations (see Vibration.h). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DE08(int pattern, const Vec3f *position,
                                                      int arg2);
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int arg1);
/* The save data and its score (see SavePoint.cpp). */
UnkObject_80079808 *fn_80079808(void);
UnkRecord_80178B5C *fn_80178B5C(UnkObject_80079808 *data, int arg1);
int fn_8016EF84(void *record);
void fn_8016F0CC(void *score, int arg1, int points);
/* The boss gauge (behaviour inferred). */
void fn_800BE2DC(float ratio);
void fn_800BE2EC(int show);
/* Debug: whether the boss is to die at once (behaviour inferred). */
bool fn_802DFA00(void);
/* A shadow at position (behaviour inferred). */
void fn_80356400(const Vec3f &position, const Vec3f &up, float radius);
/* A debug message (behaviour inferred). */
void fn_803A953C(const char *format, ...);
/* The material blink of a model (behaviour inferred). */
void fn_80336888(Task *parent, void *part, float arg2, float arg3, float arg4, float arg5);
}

/* The parameters of an effect fn_80209B58 launches (struct name unknown; layout from its
 * constructor, 0x80194C1C; names inferred). */
struct UnkEffectParam_80194C1C {
    UnkEffectParam_80194C1C(const Vec3f &position, const Vec3f &velocity, float scale,
                            bool flag);

    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ Vec3f m_velocity;
    /* 0x18 */ float m_scale;
    /* 0x1C */ float field_0x1C;
    /* 0x20 */ u8 m_flag;
};
SIZE_ASSERT(UnkEffectParam_80194C1C, 0x24);

/* fn_8040CD74 called as returning a - b by value, as a vector copied through the out-of-line
 * copy constructor (name inferred). */
typedef Fn8040CopyVector3 (*CopyVectorSubtractFn)(const Vec3f *a, const Vec3f *b);

inline Fn8040CopyVector3 Subtract(const Vec3f &a, const Vec3f &b) {
    return ((CopyVectorSubtractFn)fn_8040CD74)(&a, &b);
}

/* True when the vector is shorter than 0.01 (as in Helicopter.cpp; name inferred). */
inline bool IsZero(const Vec3f &vector) {
    return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z < 0.0001f;
}

namespace EggLastMech {

extern "C" {
/* The AI states (getters of the EggLastMech::AIState singletons; class names from the RTTI of
 * the objects they return, matched by address order and use; behaviour inferred). */
EggLastMechAIStateBase *fn_8031017C(void); /* End */
EggLastMechAIStateBase *fn_80310314(void);
EggLastMechAIStateBase *fn_803104C0(void);
EggLastMechAIStateBase *fn_8031062C(void);
EggLastMechAIStateBase *fn_803107FC(void);
EggLastMechAIStateBase *fn_80310C9C(void);
EggLastMechAIStateBase *fn_80310EA0(void);
EggLastMechAIStateBase *fn_80311060(void);
EggLastMechAIStateBase *fn_80311AE4(void);
EggLastMechAIStateBase *fn_80311C5C(void);
EggLastMechAIStateBase *fn_80312274(void);
EggLastMechAIStateBase *fn_803123E8(void);
EggLastMechAIStateBase *fn_803124BC(void); /* Nop */

/* The slot machine (behaviour inferred). */
void fn_80313C10(UnkSlot_803142C0 *slot);
void fn_80313CC0(UnkSlot_803142C0 *slot);
int fn_80313F48(UnkSlot_803142C0 *slot);
void fn_80314004(UnkSlot_803142C0 *slot, int reel, int arg2, int arg3);
/* The part at 0xF8 (behaviour inferred). */
void fn_80334A70(UnkPart_80334C78 *part);
/* The sound player (behaviour inferred). */
void fn_80336B58(UnkSound_80336F48 *sound, int id);
void fn_80336D38(UnkSound_80336F48 *sound, int arg1);
void fn_80336DF4(UnkSound_80336F48 *sound, int arg1);
}

/* The event bit each slot result needs, or -1 (values from .rodata). */
static const s32 s_resultEvents[8] = {6, 6, 4, 4, 3, 5, 7, -1};

/* The hint shown for each slot result (values from .rodata). */
static const s32 s_resultHints[8] = {4, 5, 6, 7, 8, 9, 10, -1};

/* Shows the hint of slot result (name inferred). */
static void ShowResultHint(int result) {
    EnemyHint::ShowBossHint(s_resultHints[result]);
}

boost::shared_ptr<EggLastMech> EggLastMech::Instantiate(Task *task) {
    boost::shared_ptr<EggLastMech> mech = boost::shared_ptr<EggLastMech>(new EggLastMech(task));
    return mech;
}

EggLastMech::EggLastMech(Task *task) : EnemyCharInfo(0xAD), m_ai(0), m_slot(0), m_partF0(0),
                                       m_partF4(0), m_partF8(0), m_sound(0), m_task(task) {
    field_0x118 = 0.0f;
    field_0x11C = 0.0f;
    field_0xAC = 700.0f;
    field_0xB4 = 2.0f;
    m_health = 150.0f;
    field_0x10C = 0;
    m_player = fn_800A8B74(fn_80074194(), lbl_805EE588);
    m_hintShown = 0;
    field_0x105 = 0;
    field_0x114 = 0.0f;
}

EggLastMech::~EggLastMech() {
    m_task = 0;
    if (m_sound != 0) {
        delete m_sound;
        m_sound = 0;
    }
    if (m_partF8 != 0) {
        delete m_partF8;
        m_partF8 = 0;
    }
    if (m_partF0 != 0) {
        delete m_partF0;
        m_partF0 = 0;
    }
    if (m_slot != 0) {
        delete m_slot;
        m_slot = 0;
    }
    if (m_partF4 != 0) {
        delete m_partF4;
        m_partF4 = 0;
    }
    if (m_ai != 0) {
        delete m_ai;
        m_ai = 0;
    }
    if (m_collider.get() != 0) {
        fn_800C78B0(fn_80021B08(), &m_collider);
    }
    fn_800BE2EC(0);
}

void EggLastMech::Init(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_position);
    fn_800C9ED4(owner, &m_rotation);
    SetupCommand();
    SetupRender();
    SetupCollider();
    m_target = fn_801E7128(GetWeakPtr(), 0x800);
    m_target->TurnOn();
    m_ai = new EggLastMechAI(this);
    m_ai->m_state = fn_803124BC();
    m_ai->m_prevState = fn_803124BC();
    m_ai->ChangeState(fn_80311C5C(), 0);
    m_partF4 = new EggLastMechCamera(this);
    m_slot = new UnkSlot_803142C0(this);
    m_partF0 = new UnkPart_8032D6FC(this);
    m_partF8 = new UnkPart_80334C78(this);
    m_sound = new UnkSound_80336F48(this);
    fn_800BE2EC(1);
}

void EggLastMech::OnSlotStop() {
    int result = m_slot->m_result;
    if (result >= 0) {
        ShowResultHint(result);
        int event = s_resultEvents[result];
        if (event == -1 || m_events.Test(event)) {
            StartFever(result);
        } else {
            ChangeState(fn_803123E8(), event);
        }
    }
}

void EggLastMech::CheckDeath() {
    if (!m_events.Test(2) && m_health <= 0.0f) {
        m_flags.Set(0);
        fn_80313C10(m_slot);
        if (m_sound != 0) {
            fn_80336DF4(m_sound, 5);
        }
        ChangeState(fn_8031017C(), 0);
    }
}

void EggLastMech::Tick(float dt) {
    if (fn_802DFA00()) {
        m_health = 0.0f;
    }
    field_0x118 -= fn_8019FFE0(this);
    if ((field_0x114 -= dt) < 0.0f) {
        field_0x114 = 0.0f;
    }
    if (m_partF8 != 0) {
        fn_80334A70(m_partF8);
    }
    if (!m_hintShown && m_health < 150.0f) {
        m_hintShown = 1;
        EnemyHint::ShowBossHint(1);
    }
    CheckDeath();
    EggLastMechAI *ai = m_ai;
    if (ai != 0) {
        ai->Tick();
    }
    fn_800091D0(&m_prevPosition, &m_position);
    float scale;
    if (IsZero(field_0x24)) {
        scale = 8.0f;
    } else {
        scale = 1.0f;
    }
    fn_804103E8(&m_position, &field_0x18, &field_0x24, scale, fn_8019FFE0(this));
    fn_8040CE60(&field_0x24);
    fn_802CD728(m_render.get(), &m_position, &m_rotation);
    fn_802CD824(m_render.get(), dt);
    fn_800091D0(&m_position, &(const Vec3f &)fn_802CD450(m_render.get()));
    fn_802CD7E8(m_render.get());
    void *part = fn_801A3634(fn_802CD5AC(m_render.get()), 0);
    fn_801A35AC(fn_802CD5AC(m_render.get()), part, -1);
    fn_80313CC0(m_slot);
    Target *target = m_target.get();
    target->fn_800CCE38(&(const Vec3f &)fn_802CD508(m_render.get(), 0x36));
    fn_800BE2DC(m_health / 150.0f);
    field_0x105 = 0;
    Vec3f position;
    fn_800091EC(&position, &m_position);
    position.y = 0.1f;
    fn_80356400(position, Vec3f(0.0f, 1.0f, 0.0f), 25.0f);
}

void EggLastMechAI::ChangeState(EggLastMechAIStateBase *state, int arg) {
    if (m_lock == 0 || m_busy != 0) {
        if (m_state != 0) {
            m_prevState = m_state;
            m_state->Exit(m_mech);
            m_state = state;
            m_state->Enter(m_mech, arg);
        }
    }
}

void EggLastMech::ChangeState(EggLastMechAIStateBase *state, int arg) {
    m_ai->ChangeState(state, arg);
}

void EggLastMech::LockState() {
    m_ai->m_lock++;
}

void EggLastMech::UnlockState() {
    m_ai->m_lock--;
}

void EggLastMech::StartFever(int result) {
    EggLastMechAIStateBase *state = 0;
    switch (result) {
    case 0:
    case 1:
        state = fn_80310EA0();
        break;
    case 2:
    case 3:
        state = fn_80310C9C();
        break;
    case 4:
        state = fn_803107FC();
        break;
    case 5:
        state = fn_8031062C();
        break;
    case 6:
        state = fn_803104C0();
        break;
    case 7:
        state = fn_80310314();
        break;
    }
    if (state != 0) {
        ChangeState(fn_80311060(), (int)state);
    } else {
        ChangeState(fn_80311AE4(), 0);
    }
}

bool EggLastMech::IsReady() {
    if (m_ai->IsState(4) && field_0x10C == 1) {
        return true;
    }
    return false;
}

void EggLastMech::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0x17:
        command.Flags() |= 2;
        break;
    }
    if (!(command.Flags() & 2)) {
        command.Reject(GetSharedPtr());
    }
    if (!(command.Flags() & 2)) {
        /* The type is read directly, not through TypeId: retail loads it after the GetName
         * call. */
        fn_803A953C("EggLastMech Command isn't Received ! : (%d)%s\n",
                    *reinterpret_cast<const u16 *>(command.field_0x4), command.GetName());
    }
}

void EggLastMech::SetupCommand() {
    fn_8019FD64(this, boost::bind(&EggLastMech::OnCommand, this, _1));
}

/* EggLastMech's motions (values from .rodata). */
static const EnemyMotionEntry s_motions[8] = {
    {0x9, -1},     {0xA, 0x1389}, {0xB, 0x1389}, {0xE, 0x1389},
    {0xC, 0x1389}, {0x4, 0x1389}, {0x6, 0x1389}, {0x8, 0x1389},
};

void EggLastMech::SetupRender() {
    void *model = fn_80012684(fn_800122D4(), 0x16, 0xF);
    void *motion = fn_80012608(fn_800122D4(), 0x16, 0x19);
    EnemyMotionTable table;
    table.entries = s_motions;
    table.count = 8;
    m_render = fn_802CDB08(0x16, table, model, motion);
    if (m_render.get() != 0) {
        fn_802CD638(m_render.get(), 1.0f);
        fn_802CD728(m_render.get(), &m_position, &m_rotation);
        fn_801A4898(fn_802CD588(m_render.get()), 0xC, 0, 0.0f);
        fn_802CD824(m_render.get(), 0.0f);
        fn_802CD7E8(m_render.get());
        fn_802CD270(m_render.get(), boost::bind(&EggLastMech::OnMotionEvent, this, _1));
    }
}

void EggLastMech::SetupCollider() {
    void *part = fn_8019A890(0x16, 0x11);
    if (m_render.get() != 0 && part != 0) {
        m_collider = fn_800C7F7C(fn_80021B08(), boost::weak_ptr<CharInfo>(GetWeakPtr()), part,
                                 fn_802CD420(m_render.get()), 5, 0);
        m_collider->SetField2C(1);
        boost::weak_ptr<CharaColliAttack> attack = fn_800C4374(fn_80021B08(), m_collider);
        if (boost::shared_ptr<CharaColliAttack> attackPtr = attack.lock()) {
            fn_80054DD0(attackPtr.get(), 1, 1, 1.0f, boost::bind(&EggLastMech::OnAttackHit, this, _1),
                        boost::bind(&EggLastMech::OnAttackEnd, this, _1));
            fn_80096B64(attackPtr.get(), 1, boost::bind(&EggLastMech::OnAttacked, this, _1));
        }
    }
}

void EggLastMech::OnAttackHit(AttackCallbackParam &param) {
    boost::shared_ptr<CharInfo> owner = param.m_attacker.lock()->m_owner.lock();
    if (owner.get() != 0) {
        if (field_0x114 > 0.0f && owner->field_0x5C == 1) {
            Param(param).m_byPlayer = 1;
        }
        fn_800091D0(&Param(param).m_offset,
                    &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&Param(param).m_position,
                                                                            &owner->m_position));
        return;
    }
    fn_8040CE60(&Param(param).m_offset);
}

void EggLastMech::OnAttackEnd(AttackCallbackParam &param) {}

void EggLastMech::OnAttacked(AttackCallbackParam &param) {
    UnkAttackSource *source = Param(param).m_source;
    fn_8040CE38(&field_0x18, 0.5f);
    if (!fn_80053538(&param) && (param.m_attacker.lock()->m_flags & 0xA10)) {
        param.m_result = 1;
        if ((u32)fn_8040EA90(2) != 0) {
            fn_80336B58(m_sound, 0x6002);
        } else {
            fn_80336B58(m_sound, 0x6003);
        }
    } else if (source != 0 && source->kind == 0x36) {
        param.m_result = 1;
        fn_80336B58(m_sound, 0x7F36);
    }
    if (param.m_result == 1) {
        OnDamage(param);
        Vec3f angles;
        fn_800091EC(&angles, &lbl_805E07A0);
        fn_8003B9E8(&Param(param).m_direction, &angles.x, &angles.y);
        fn_801D2AFC(0, &Param(param).m_position, &angles, 0x225,
                    *reinterpret_cast<u8 **>(&field_0x60) + 4);
    } else {
        param.m_result = 5;
    }
}

void EggLastMech::OnDamage(AttackCallbackParam &param) {
    float health = m_health;
    if ((m_health -= param.m_damage) < 0.0f) {
        m_health = 0.0f;
    }
    UnkObject_80079808 *data = fn_80079808();
    if (data != 0) {
        fn_8016F0CC(data->m_score, 0, (int)(100.0f * (health - m_health)));
    }
    fn_8040CE38(&field_0x18, 0.5f);
    if (!m_events.Test(1) && m_health < 75.0f) {
        fn_8034DE08(0xC, &m_position, -2);
        ChangeState(fn_80312274(), 1);
    } else {
        fn_80336888(m_task, fn_801A3634(fn_802CD5AC(m_render.get()), 0), 1.0f, 0.0f, 0.0f, 1.0f);
    }
}

void EggLastMech::OnMotionEvent(EnemyMotionEventArg &arg) {
    switch (arg.m_type) {
    case 0x5000: {
        EnemyMotionEventArg::Data *data = arg.m_data;
        if (data != 0) {
            switch (data->kind) {
            case 1: {
                UnkRecord_80178B5C *record = fn_80178B5C(fn_80079808(), 0);
                if (record != 0 && fn_8016EF84(record->field_0x20) == 0) {
                    fn_80314004(m_slot, fn_80313F48(m_slot), 0, 2);
                } else {
                    fn_80314004(m_slot, fn_80313F48(m_slot), 1, 2);
                }
                break;
            }
            case 2:
                Fire(m_slot->m_line.m_shotType, data->value);
                break;
            }
        }
        break;
    }
    case 0x5003:
    case 0xD004: {
        EnemyMotionEventArg::Data *data = arg.m_data;
        if (data != 0) {
            switch (data->kind) {
            case 0x24:
                fn_80336B58(m_sound, 0x7F2E);
                break;
            case 0x21:
                fn_8034DF48(7, -2);
                break;
            }
        }
        break;
    }
    }
}

void EggLastMech::StopSound(int arg) {
    if (m_sound != 0) {
        fn_80336DF4(m_sound, arg);
    }
}

void EggLastMech::PlaySound(int arg) {
    if (m_sound != 0) {
        fn_80336D38(m_sound, arg);
    }
}

void *EggLastMech::GetMotion() {
    return fn_802CD588(m_render.get());
}

void EggLastMech::SetMotion(int arg) {
    fn_801A46E4(GetMotion(), arg);
}

Fn8040CopyVector3 EggLastMech::GetPlayerOffset() {
    if (boost::shared_ptr<CharInfo> player = GetPlayer().lock()) {
        return Subtract(player->m_position, m_position);
    }
    return reinterpret_cast<const Fn8040CopyVector3 &>(lbl_805E07A0);
}

/* The bones the shots leave from (values from .rodata). */
static const s32 s_shotBones[6] = {0x13BB, 0x13C5, 0x13BC, 0x13C6, 0x13BD, 0x13C7};

void EggLastMech::Fire(int type, int index) {
    boost::weak_ptr<CharInfo> owner;
    boost::weak_ptr<Target> target;
    if (type == 1) {
        owner = GetWeakPtr();
        boost::shared_ptr<CharInfo> player = GetPlayer().lock();
        boost::shared_ptr<Player::PlayerBase> base =
            boost::dynamic_pointer_cast<Player::PlayerBase>(player);
        if (base.get() != 0 && GetPlayerTarget(base.get()) != 0) {
            target = fn_80206574(GetPlayerTarget(base.get()));
        }
    } else {
        owner = fn_800B75DC(GetPlayer());
        target = m_target;
    }
    if (!owner.expired() && !target.expired()) {
        boost::shared_ptr<Weapon::LockonTarget> lockon =
            boost::shared_ptr<Weapon::LockonTarget>(new Weapon::LockonTarget());
        if (lockon.get() != 0) {
            lockon->SetTarget(target);
            lockon->fn_80060F70();
            Mtx34f *bone = fn_802CD560(m_render.get(), s_shotBones[index]);
            Vec3f position;
            fn_8040FAA0(&position, bone);
            Vec3f direction(0.0f, 0.0f, 1.0f);
            fn_800091D0(&direction, &(const Vec3f &)fn_8040B618(bone, &direction));
            Weapon::ShotInfo info(owner, lockon, type == 1, position, direction, lbl_805E07A0);
            lbl_8051DDDC.fire(&info);
            fn_80336B58(m_sound, 0x201E);
            fn_801D560C(fn_80053164(), 0x201D, 0, 0);
        }
    }
}

void EggLastMech::SpawnEffect(int arg) {
    Vec3f offset(0.0f, 100.0f, 0.0f);
    if (arg == 1) {
        fn_8040CFF8(&offset, &(const Vec3f &)fn_800091A4(&field_0x18, 0.5f));
    }
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)fn_802CD508(m_render.get(), 0x36));
    UnkEffectParam_80194C1C param(position, offset, 10.0f, true);
    param.field_0x1C = 20.0f;
    fn_80209B58(fn_801EB304(fn_8001C4F8(), 0xE), 2, &param);
    fn_801D54F4(fn_80053164(), 0x6037, &position, 0, 1);
}

bool EggLastMech::IsStopped() {
    /* The comparison with 0 is retail's: it converts the && result once more. */
    return (IsZero(field_0x18) && IsZero(field_0x24)) != 0;
}

} // namespace EggLastMech
