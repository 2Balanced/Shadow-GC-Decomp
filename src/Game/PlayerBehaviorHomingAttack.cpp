/* Player::Behavior::HomingAttack (class name from RTTI): behavior 8 of Shadow, the homing attack on
 * a target (Player::Behavior::InitializeArgsHomingAttack). Entering it tells the target's character
 * it is attacked and listens to the player's collisions and attacks; each frame the player turns
 * towards the target, ever faster, and flies on. Meeting the target jumps off it (behavior 9,
 * Player::Behavior::InitializeArgsHomingJump, a hit), meeting another attack target jumps off
 * without one, and bumping into anything else, losing the target or flying for ten seconds falls
 * (behavior 2). File name inferred from the class (as Game/PlayerBehaviorSuper.cpp); all other
 * names are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). Where the game reloads the player pointer in a call's arguments after
 * another argument's call, the code reads m_player.px: an inline get() there is expanded ahead of
 * the calls (as in Game/PlayerBehaviorGrind.cpp). */
/* Declares Player::Behavior::Behavior's slots with their arguments (see Game/PlayerBehavior.h). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <cmath>

/* Defines InitializeArgsHomingJump's inline functions, whose out-of-line copies this unit emits. */
#define PLAYER_BEHAVIOR_ARGS_INLINE

#include "Game/CharCommand.h"
#include "Game/CharaColliAttack.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerCharaParam.h"
#include "Game/PlayerMotionEvent.h"
#include "Game/PlayerSoundBase.h"
#include "Game/PlayerWeaponBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;

/* Target (class name from RTTI; see Game/CharCommand.h): what a homing attack locks on to. Only what
 * this unit uses is declared (names and meanings inferred). */
class Target {
  public:
    /* The point to aim at, as seen by viewer (vtable slot 0x8). */
    virtual Vec3f GetPosition(CharInfo *viewer);

    /* 0x04 */ u8 field_0x4[0x28 - 0x4];
    /* 0x28 */ boost::weak_ptr<CharInfo> m_owner; /* the character the target belongs to */
    /* 0x30 */ u32 m_flags;                       /* bit 0: it can be locked on to */
};

/* The player's members read here (struct name unknown; Player::PlayerBase leaves them
 * unmapped). */
struct UnkPlayerFields_80084EB0 {
    /* 0x000 */ u8 field_0x0[0x64];
    /* 0x064 */ s32 m_kind; /* the character (as in Game/PlayerBehaviorFall.cpp) */
    /* 0x068 */ u8 field_0x68[0xE8 - 0x68];
    /* 0x0E8 */ Vec3f m_jumpUp; /* the direction jumps go (the player falls when it moves against it) */
    /* 0x0F4 */ u8 field_0xF4[0x1C8 - 0xF4];
    /* 0x1C8 */ Vec3f m_groundNormal; /* (going by fn_8009330C, Game/PlayerBehaviorIdle.cpp) */
};

extern "C" {
/* The collider manager (see Game/BlockChange.cpp) and whether a contact is an attack target (see
 * Game/BirdBase.cpp). */
void *fn_80021B08(void);
bool fn_800C3ED4(void *manager, UnkColliContactNode_800587A0 *contact);
/* The player's state bits (see Game/PlayerShadowSuper.cpp). */
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* Called with the player's object at 0x234: clearing and setting its attack callback, and
 * stopping and starting its effect (behaviour inferred; see Game/PlayerBehaviorFall.cpp). */
void fn_800778B8(void *object);
void fn_800778DC(void *object, const AttackCallback &callback);
void fn_8007792C(void *object);
void fn_80077950(void *object, int arg);
/* The player's front axis (see Game/PlayerBehaviorGrind.cpp). */
Vec3f fn_800796DC(Player::PlayerBase *player);
/* Player updates run before and after the behavior's own (behaviour unknown; as in FirstMeet). */
void fn_80079720(Player::PlayerBase *player);
void fn_8009728C(boost::shared_ptr<Player::PlayerBase> *player, int arg1);
/* The player's Euler angles for a frame (behaviour inferred). */
Vec3f fn_80093D34(boost::shared_ptr<Player::PlayerBase> *player, const Vec3f *forward,
                  const Vec3f *up);
/* The attack's trail of the player's effects at 0x23C, stopped and started with a length
 * (behaviour inferred). */
void fn_800A2D30(void *effects);
void fn_800A2D54(void *effects, float length);
/* Switches a player's behavior (behaviour inferred). */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);

Vec3f fn_800091A4(const Vec3f *vector, float scale);      /* vector * scale */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);      /* vector * scale */
Vec3f fn_8040CEE8(const Vec3f *vector);                   /* normalized */
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b);        /* cross product */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);      /* vector += value */
float fn_8040E99C(float angle);                           /* wrapped to a half turn */
Vec3f fn_8040F364(const Vec3f *forward, const Vec3f *up); /* Euler angles */
Vec3f fn_80410098(const Vec3f *vector, const Vec3f *rotation, int order); /* rotated */
}

/* The player's members above (a macro, as in Game/PlayerBehaviorSuper.cpp). */
#define FieldsOf(player) reinterpret_cast<UnkPlayerFields_80084EB0 *>(player)

/* The attack collision an attack met (the weak pointer at 0x8 of the parameters; a macro, as
 * FieldsOf). */
#define ContactOf(param) (*reinterpret_cast<boost::weak_ptr<CharaColliAttack> *>((param).field_0x8))

/* fn_8040CD74 returns lhs - rhs through its first argument, as a function returning a Vec3f does;
 * called this way the result is a temporary, as in the game (see Game/PlayerBehaviorSuper.cpp). */
typedef Vec3f (*VectorSubtract)(const Vec3f *lhs, const Vec3f *rhs);
#define Subtract(lhs, rhs) (reinterpret_cast<VectorSubtract>(fn_8040CD74)(lhs, rhs))

/* vector = value, through the game's out-of-line copy (see Game/JumpBoard.cpp). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* vector += value (an inline wrapper, which orders the call's arguments as the game does). */
inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

namespace Player {
namespace Behavior {

class HomingAttack : public Behavior {
  public:
    HomingAttack(const boost::shared_ptr<PlayerBase> &player);
    virtual ~HomingAttack();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void Update(float dt);

    /* The factory record's functions (names inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);
    static void Release();

    void OnAttack(AttackCallbackParam &param);
    void AnnounceAttack();
    void Steer(float dt);
    void OnCollision(CollisionEventArgs &args);

    static const char *s_name;

    /* 0x1C */ s32 m_state; /* 0 flying, 1 fall, 2 jump off, 3 jump off the target */
    /* 0x20 */ boost::weak_ptr<Target> m_target;
    /* 0x28 */ float m_turnRate; /* radians per second */
    /* 0x2C */ float m_time;     /* since the attack started */
};
SIZE_ASSERT(HomingAttack, 0x30);

const char *HomingAttack::s_name = "HomingAttack";

HomingAttack::HomingAttack(const boost::shared_ptr<PlayerBase> &player)
    : Behavior(player, s_name, 8) {}

HomingAttack::~HomingAttack() {}

/* NonMatching (96%): retail keeps the table's address in r3 and the player in r4; GC/2.7 hoists the
 * address above the player's load and swaps the two (as Player::Behavior::Ground::SetSpeedRange;
 * the forms tried are listed there). */
bool HomingAttack::vf_0x0C(int prevId, const InitializeArgs &args) {
    return lbl_804F1834[FieldsOf(m_player.get())->m_kind].m_canHomingAttack;
}

void HomingAttack::Enter(int prevId, const InitializeArgs &args) {
    m_target = dynamic_cast<const InitializeArgsHomingAttack *>(&args)->m_target;
    AnnounceAttack();
    m_turnRate = 8.0f;
    fn_80076CA8(m_player.get()->m_stateBits, 6);
    fn_80076CA8(m_player.get()->m_stateBits, 0x38);
    m_player.get()->m_motion->vf_0x10(0xC, 0.0f);
    PlayerCollisionEventCallback(m_player.get()) =
        CollisionEventCallback(boost::bind(&HomingAttack::OnCollision, this, _1));
    m_player.get()->m_sound->StopVoice(5);
    fn_800A2D54(reinterpret_cast<void *>(m_player.get()->field_0x23C), 5.0f);
    fn_80076CA8(m_player.get()->m_stateBits, 0x31);
    fn_80076CA8(m_player.get()->m_stateBits, 0x35);
    fn_80077950(reinterpret_cast<void *>(m_player.get()->field_0x234), 7);
    fn_800778DC(reinterpret_cast<void *>(m_player.get()->field_0x234),
                AttackCallback(boost::bind(&HomingAttack::OnAttack, this, _1)));
    m_state = 0;
    m_time = 0.0f;
}

void HomingAttack::Leave(int nextId) {
    fn_80076CC8(m_player.get()->m_stateBits, 6);
    fn_80076CC8(m_player.get()->m_stateBits, 0x31);
    fn_80076CC8(m_player.get()->m_stateBits, 0x38);
    PlayerCollisionEventCallback(m_player.get()).Clear();
    fn_800A2D30(reinterpret_cast<void *>(m_player.get()->field_0x23C));
    if (nextId != 9) {
        fn_80076CC8(m_player.get()->m_stateBits, 0x35);
        fn_8007792C(reinterpret_cast<void *>(m_player.get()->field_0x234));
    }
    fn_800778B8(reinterpret_cast<void *>(m_player.get()->field_0x234));
}

void HomingAttack::Update(float dt) {
    switch (m_state) {
    case 1:
        fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(2));
        return;
    case 2:
        fn_800A7F14(m_player.get()->m_behaviors,
                    &(const InitializeArgs &)InitializeArgsHomingJump(
                        FieldsOf(m_player.get())->m_jumpUp, false));
        return;
    case 3:
        fn_800A7F14(m_player.get()->m_behaviors,
                    &(const InitializeArgs &)InitializeArgsHomingJump(
                        FieldsOf(m_player.get())->m_jumpUp, true));
        return;
    case 0:
    default:
        m_turnRate += 10.0f * dt;
        if (m_target.expired() || !(m_target.lock()->m_flags & 1)) {
            fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(2));
            return;
        }
        m_time += dt;
        if (m_time > 10.0f) {
            fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(2));
            return;
        }
        Steer(dt);
        Vec3f front;
        fn_800091EC(&front, &(const Vec3f &)fn_800796DC(m_player.get()));
        AddVector(m_player->field_0x24, fn_800091A4(&front, 1000.0f));
        fn_800091D0(&m_player.px->field_0x18,
                    &(const Vec3f &)fn_8040CDA8(&front, fn_8040CF50(&m_player.get()->field_0x18)));
        fn_80079720(m_player.get());
        fn_8009728C(&m_player, 0);
        break;
    }
}

void HomingAttack::OnAttack(AttackCallbackParam &param) {
    boost::shared_ptr<CharaColliAttack> contact = ContactOf(param).lock();
    if (*reinterpret_cast<s32 *>(contact->field_0x3C) == 2) {
        boost::shared_ptr<Target> target = m_target.lock();
        boost::shared_ptr<CharaColliAttack> attack = ContactOf(param).lock();
        if (target.get() != 0 && attack.get() != 0) {
            boost::shared_ptr<CharInfo> targetOwner = target->m_owner.lock();
            boost::shared_ptr<CharInfo> attackOwner = attack->m_owner.lock();
            if (targetOwner == attackOwner) {
                m_state = 3;
            }
        }
    } else if (param.m_result & 1) {
        m_state = 2;
    } else {
        m_state = 1;
    }
}

/* angle turned towards target by at most step, the short way round (name inferred). */
float TurnAngle(float angle, float target, float step) {
    float diff = fn_8040E99C(target - angle);
    if (std::fabs(diff) <= step) {
        return target;
    }
    angle += diff * std::fabs(step / diff);
    return angle;
}

/* Tells the target's character that the player attacks it (name inferred). */
void HomingAttack::AnnounceAttack() {
    if (!m_target.expired()) {
        boost::shared_ptr<CharInfo> owner = m_target.lock()->m_owner.lock();
        if (owner.get() != 0) {
            owner->PushCommand(new AnnounceAttackCommand(boost::weak_ptr<CharInfo>(m_player), 0, 4));
        }
    }
}

/* Turns the player's frame towards the target by the turn rate (name inferred). */
void HomingAttack::Steer(float dt) {
    Vec3f toTarget;
    fn_800091EC(&toTarget, &(const Vec3f &)fn_8040CEE8(&(const Vec3f &)Subtract(
                               &(const Vec3f &)m_target.lock()->GetPosition(m_player.get()),
                               &m_player.get()->m_position)));
    Vec3f side;
    fn_800091EC(&side, &(const Vec3f &)fn_8040CEE8(&(const Vec3f &)fn_8040CFB8(
                           &toTarget, &FieldsOf(m_player.get())->m_groundNormal)));
    Vec3f up;
    fn_800091EC(&up, &(const Vec3f &)fn_8040CEE8(&(const Vec3f &)fn_8040CFB8(&side, &toTarget)));
    Vec3f to = fn_8040F364(&toTarget, &up);
    Vec3f front;
    fn_800091EC(&front, &(const Vec3f &)fn_800796DC(m_player.get()));
    Vec3f frontSide;
    fn_800091EC(&frontSide, &(const Vec3f &)fn_8040CEE8(&(const Vec3f &)fn_8040CFB8(
                                &front, &FieldsOf(m_player.get())->m_groundNormal)));
    Vec3f frontUp;
    fn_800091EC(&frontUp,
                &(const Vec3f &)fn_8040CEE8(&(const Vec3f &)fn_8040CFB8(&frontSide, &front)));
    Vec3f from = fn_8040F364(&front, &frontUp);
    Vec3f angles(0.0f, 0.0f, 0.0f);
    angles.x = TurnAngle(from.x, to.x, m_turnRate * dt);
    angles.y = TurnAngle(from.y, to.y, m_turnRate * dt);
    angles.z = TurnAngle(from.z, to.z, m_turnRate * dt);
    Vec3f forward(0.0f, 0.0f, -1.0f);
    Vec3f upAxis(0.0f, 1.0f, 0.0f);
    AssignVector(forward, fn_80410098(&forward, &angles, 2));
    AssignVector(upAxis, fn_80410098(&upAxis, &angles, 2));
    m_player->m_rotation = fn_80093D34(&m_player, &forward, &upAxis);
}

void HomingAttack::OnCollision(CollisionEventArgs &args) {
    if (!fn_800C3ED4(fn_80021B08(), args.m_contact) && m_state == 0) {
        m_state = 1;
    }
}

Behavior *HomingAttack::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new HomingAttack(player);
}

void HomingAttack::Release() {}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds HomingAttack by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804B03E0 = {
    Player::Behavior::HomingAttack::Create, Player::Behavior::HomingAttack::Release, 0};
