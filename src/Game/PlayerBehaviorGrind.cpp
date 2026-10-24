/* Player::Behavior::Grind (class name from RTTI): behavior 0x11 of Shadow, grinding along a rail
 * (a circuit target, which comes with Player::Behavior::InitializeArgsGrind). Each frame it moves
 * the player along the rail at its speed, turning the player's frame towards the rail's; the speed
 * falls uphill and rises downhill, and firing a weapon slows it. Commands from the motion and the
 * input crouch (event 0xD003), jump off (behavior 5, Player::Behavior::InitializeArgsJump), change
 * to the rail beside (an arc over 0.47 seconds), turn round or attack; an electric rail hurts. At
 * the rail's end it falls (behavior 2). File name inferred from the class (as
 * Game/PlayerBehaviorSuper.cpp); all other names are inferred. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). Where the game reloads the player
 * pointer in a call's arguments after another argument's call, the code reads m_player.px: an
 * inline get() there is expanded ahead of the calls. */
/* Declares Player::Behavior::Behavior's slots with their arguments (see Game/PlayerBehavior.h). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <cmath>

/* Defines Player::WeaponBase::GetWeapon in its class, whose out-of-line copy this unit emits. */
#define PLAYER_WEAPON_BASE_GET_WEAPON_INLINE

#include "Game/CharCommand.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerMotionEvent.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/UnkCircuitTarget_800ABCE8.h"
#include "Game/WeaponBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* A part of a player character fn_80078CEC returns (as in Game/CatchStick.cpp). */
struct UnkPlayerPart_80078CEC {
    /* 0x00 */ u8 field_0x0[0xC];
    /* 0x0C */ Vec3f m_position;
};

/* The player's members read here (struct name unknown; Player::PlayerBase leaves them
 * unmapped). */
struct UnkPlayerFields_80081950 {
    /* 0x000 */ u8 field_0x0[0xB8];
    /* 0x0B8 */ Vec3f field_0xB8; /* the input's direction (as in Game/PlayerBehaviorEdgeHang.cpp) */
    /* 0x0C4 */ Vec3f field_0xC4; /* added to the grind's offset each frame (meaning unknown) */
    /* 0x0D0 */ u8 field_0xD0[0xF4 - 0xD0];
    /* 0x0F4 */ Vec3f m_up;
};

/* a - b (0x8000AD64; as in Game/PlayerBehaviorEdgeHang.cpp). */
Vec3f operator-(const Vec3f &a, const Vec3f &b);

extern "C" {
/* A 32-bit flag word: fn_80014E04 constructs it cleared, fn_80014E10 clears it, fn_80014E1C sets
 * bit, fn_8006127C clears bit and fn_80014E34 tests bit (see Game/BAMine.cpp); fn_80082AE4, defined
 * below, flips bit. */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_80082AE4(u32 *flags, int bit);
/* The player's state bits (see Game/PlayerShadowSuper.cpp). */
bool fn_8006CD78(u32 *bits, int bit);
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* The player's part index (see Game/CatchStick.cpp). */
UnkPlayerPart_80078CEC *fn_80078CEC(Player::PlayerBase *player, int index);
/* A direction in the player's frame turned into the world's, and the player's front and up axes
 * (behaviour inferred). */
Vec3f fn_80079628(Player::PlayerBase *player, const Vec3f *direction);
Vec3f fn_800796DC(Player::PlayerBase *player);
Vec3f fn_80079698(Player::PlayerBase *player);
/* The player's height (the word at 0x274; see Game/PlayerBehaviorFall.cpp). */
float fn_8007E514(Player::PlayerBase *player);
/* A blend of from and to along an arc of height up (see Game/ThrowBomb.cpp). */
Vec3f fn_800930AC(const Vec3f *from, const Vec3f *to, const Vec3f *up, float t);
/* The player's input along a direction (see Game/PlayerBehaviorEdgeHang.cpp). */
int fn_80093C60(boost::shared_ptr<Player::PlayerBase> *player, const Vec3f *direction);
/* Player updates run after the behavior's own (behaviour unknown; as in FirstMeet). */
void fn_8009728C(boost::shared_ptr<Player::PlayerBase> *player, int arg1);
/* Tests bit 3 of the player's word at 0xB4 (meaning unknown; as in AirAttack). */
bool fn_800A9E1C(boost::shared_ptr<Player::PlayerBase> *player);
/* Called on the player when its attack on the rail ends (behaviour unknown). */
void fn_800B7BCC(boost::shared_ptr<Player::PlayerBase> *player);
/* The grind's effect of the player's effects at 0x23C: started on entry, stopped on exit and told
 * each frame whether sparks show (behaviour inferred). */
void fn_800A2C70(void *effects);
void fn_800A2C4C(void *effects);
void fn_800A2C28(void *effects, bool sparks);
/* Switches a player's behavior (behaviour inferred). */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);
/* Calls on the player's motion (meanings unknown; fn_800AA7B8 sets the motion's word at 0x438, as
 * in AirAttack). */
void fn_800AA700(Player::PlayerMotion *motion, int arg);
void fn_800AA7B8(Player::PlayerMotion *motion, float value);
/* The circuit target: where it is (position and the two directions of its frame; any may be 0),
 * whether it has a path, moving it on by a distance (false at the path's end), its kind (3: an
 * electric rail, inferred), a flag, clearing it and a call when the player's angles are set
 * (behaviour inferred; see Game/PlayerBehaviorChaosControl.cpp). */
void fn_800ABAE8(const UnkCircuitTarget_800ABCE8 *target, Vec3f *position, Vec3f *forward,
                 Vec3f *up);
bool fn_8007646C(UnkCircuitTarget_800ABCE8 *target);
bool fn_800ABB14(UnkCircuitTarget_800ABCE8 *target, float distance);
int fn_800ABB48(UnkCircuitTarget_800ABCE8 *target);
void fn_800ABAA4(UnkCircuitTarget_800ABCE8 *target, bool on);
void fn_8007684C(UnkCircuitTarget_800ABCE8 *target);
void fn_800ABB80(UnkCircuitTarget_800ABCE8 *target);
/* Looks for a rail from a position along a direction within a range; 2 or 3 when one is found
 * (behaviour inferred). */
int fn_800ABC0C(UnkCircuitTarget_800ABCE8 *target, boost::shared_ptr<Player::PlayerBase> *player,
                const Vec3f *position, const Vec3f *direction, int arg4, float range);

void fn_8040CE60(Vec3f *vector);                          /* vector = 0 */
float fn_8040CE74(Vec3f *vector);                         /* normalises it */
Vec3f fn_8040CEE8(const Vec3f *vector);                   /* normalized */
float fn_8040CF2C(const Vec3f *v);                        /* |v|^2 */
float fn_8040CF50(const Vec3f *v);                        /* |v| */
float fn_8040CF90(const Vec3f *a, const Vec3f *b);        /* dot product */
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b);        /* cross product */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);      /* vector += value */
Vec3f fn_800091A4(const Vec3f *vector, float scale);      /* vector * scale */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);      /* vector * scale */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);        /* a + b */
Vec3f fn_8040F364(const Vec3f *forward, const Vec3f *up); /* Euler angles */
float fn_8040F8FC(const Vec3f *a, const Vec3f *b);        /* the angle between */
/* Splits vector along normal (see Game/AdjustCosRot.cpp). */
void fn_8040F990(Vec3f *along, Vec3f *across, const Vec3f *vector, const Vec3f *normal);
Mtx34f fn_8040FD8C(const Vec3f *axis, float angle);          /* a rotation about axis */
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point); /* the point transformed */
}

/* Whether a value is about zero: an inline function in the game, which calls std::fabs, another
 * inline function, and so stays out of line. The game links the copy at 0x8000A950; this unit
 * emits its own after Update, which puts its constant ahead of the later functions' ones, as in
 * the game, and is dropped at link. */
extern "C" inline bool fn_8000A950(float value) {
    return std::fabs(value) < 0.0001f;
}

/* The player's members above (a macro, as in Game/PlayerBehaviorSuper.cpp). */
#define FieldsOf(player) reinterpret_cast<UnkPlayerFields_80081950 *>(player)

/* fn_8040CD74 returns lhs - rhs through its first argument, as a function returning a Vec3f does;
 * called this way the result is a temporary, as in the game (see Game/PlayerBehaviorSuper.cpp). */
typedef Vec3f (*VectorSubtract)(const Vec3f *lhs, const Vec3f *rhs);
#define Subtract(lhs, rhs) (reinterpret_cast<VectorSubtract>(fn_8040CD74)(lhs, rhs))

/* fn_800091EC copies a vector into its first argument, as the copy of a returned Vec3f does;
 * called this way it builds a function's result (name invented). */
typedef Vec3f (*VectorCopy)(const Vec3f *from);
#define CopyVector(from) (reinterpret_cast<VectorCopy>(fn_800091EC)(from))

/* The player's height (the word at 0x274; an inline accessor, whose out-of-line copy is
 * fn_8007E514). */
inline float Height(Player::PlayerBase *player) {
    return player->field_0x274;
}

/* vector * scale (an inline wrapper, which orders the call as the game does). */
inline Vec3f Scale(const Vec3f &vector, float scale) {
    return fn_8040CDA8(&vector, scale);
}

/* A blend of from and to along an arc of height up (an inline wrapper of fn_800930AC, which orders
 * the call's arguments as the game does). */
inline Vec3f Arc(const Vec3f &from, const Vec3f &to, const Vec3f &up, float t) {
    return fn_800930AC(&from, &to, &up, t);
}

/* Degrees to radians (name inferred; as in Game/BalloonGenerator.cpp). */
inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* vector = value, through the game's out-of-line copy (see Game/JumpBoard.cpp). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* vector += value (an inline wrapper, which orders the call's arguments as the game does). */
inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

/* The flag word's flip (the game's out-of-line copy, first emitted here; name inferred). */
void fn_80082AE4(u32 *flags, int bit) {
    *flags ^= 1 << bit;
}

/* from turned towards to by at most maxAngle (half of it, as the rotation is applied; name
 * inferred). */
Vec3f TurnTowards(const Vec3f *from, const Vec3f *to, float maxAngle) {
    Vec3f axis;
    fn_800091D0(&axis, &(const Vec3f &)fn_8040CFB8(from, to));
    if (axis.x * axis.x + axis.y * axis.y + axis.z * axis.z < 0.0001f) {
        return CopyVector(to);
    }
    float angle = fn_8040F8FC(from, to);
    if (angle > maxAngle) {
        angle = maxAngle;
    }
    Mtx34f rotation;
    fn_8040C148(&rotation, &(const Mtx34f &)fn_8040FD8C(&(const Vec3f &)fn_8040CEE8(&axis),
                                                          0.5f * angle));
    return fn_8040B67C(&rotation, from);
}

namespace Player {
namespace Behavior {

/* InitializeArgsGrind (class name from RTTI): switches to the grind (behavior 0x11) on a rail.
 * Its vtable and destructor are not in this unit. */
class InitializeArgsGrind : public InitializeArgs {
  public:
    virtual ~InitializeArgsGrind();

    /* 0x8 */ UnkCircuitTarget_800ABCE8 m_rail;
};

/* The behavior's flag word (struct name unknown): bit 0 the facing on the rail, bit 1 crouching
 * (motion event 0xD003), bits 2-5 the jump, rail change, turn and attack commands, bits 6-8 the
 * attack's motion events. */
struct GrindFlags {
    GrindFlags() {
        fn_80014E04(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    void Clear(int bit) {
        fn_8006127C(&m_bits, bit);
    }
    void Flip(int bit) {
        fn_80082AE4(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }
    void Reset() {
        fn_80014E10(&m_bits);
    }

    u32 m_bits;
};

class Grind : public Behavior {
  public:
    Grind(const boost::shared_ptr<PlayerBase> &player);
    virtual ~Grind();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x18(CharCommand *command);
    virtual void Update(float dt);
    virtual void vf_0x20();

    /* The factory record's functions (names inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);
    static void Release();

    bool ChangeRail();
    UnkCircuitTarget_800ABCE8 FindRail(int *side);
    void UpdateSpeed(float dt);
    void UpdateChange(float dt);
    void Move(float dt);
    void OnMotion(MotionEventArgs &args);

    static const char *s_name;

    /* 0x1C */ Vec3f m_offset;
    /* 0x28 */ Vec3f m_up;
    /* 0x34 */ Vec3f m_forward;
    /* 0x40 */ Vec3f m_railPosition; /* where the rail was last frame, while changing rails */
    /* 0x4C */ Vec3f m_position;     /* where the change of rails started */
    /* 0x58 */ UnkCircuitTarget_800ABCE8 m_rail;
    /* 0x60 */ float m_changeTime;
    /* 0x64 */ float m_speed;
    /* 0x68 */ GrindFlags m_flags;
    /* 0x6C */ s32 m_state; /* 0 start, 1 grinding, 2 crouching, 3 changing rails, 4 turning,
                             * 5 attacking */
};
SIZE_ASSERT(Grind, 0x70);

const char *Grind::s_name = "Grind";

/* How far a rail may be to start grinding it, how far aside the next rail is looked for, and how
 * far ahead (names inferred). The game sets them in __sinit rather than in .sdata, as an
 * initializer that is not a constant expression does; the inline call reproduces that (as in
 * Game/PlayerBehaviorSuper.cpp). */
inline float DynamicFloat(float value) {
    return value;
}
static float s_reach = DynamicFloat(9.0f);
static float s_sideReach = DynamicFloat(30.0f);
static float s_aheadReach = DynamicFloat(10.0f);

Grind::Grind(const boost::shared_ptr<PlayerBase> &player) : Behavior(player, s_name, 0x11) {}

Grind::~Grind() {}

bool Grind::vf_0x0C(int prevId, const InitializeArgs &args) {
    const InitializeArgsGrind &grind = dynamic_cast<const InitializeArgsGrind &>(args);
    Vec3f position;
    Vec3f forward;
    Vec3f up;
    fn_800ABAE8(&grind.m_rail, &position, &forward, &up);
    Vec3f toRail;
    fn_800091EC(&toRail, &(const Vec3f &)Subtract(&position, &m_player.get()->m_position));
    if (fn_8040CF90(&up, &toRail) >= 0.0f) {
        Vec3f toPart;
        fn_800091EC(&toPart, &(const Vec3f &)Subtract(&position,
                                                      &fn_80078CEC(m_player.get(), 1)->m_position));
        if (fn_8040CF90(&up, &toPart) >= 0.0f) {
            return false;
        }
    }
    if (fn_8040CF90(&m_player.get()->field_0x18, &up) > 0.01f) {
        return false;
    }
    return !(fn_8040CF2C(&toRail) > s_reach * s_reach);
}

void Grind::Enter(int prevId, const InitializeArgs &args) {
    m_rail = static_cast<const InitializeArgsGrind &>(args).m_rail;
    fn_800ABAA4(&m_rail, 1);
    m_speed = fn_8040CF50(&m_player.get()->field_0x18);
    m_flags.Reset();
    m_flags.Set(0);
    fn_8040CE60(&m_offset);
    fn_800091D0(&m_up, &(const Vec3f &)fn_80079698(m_player.get()));
    fn_800ABAE8(&m_rail, 0, &m_forward, 0);
    fn_800091D0(&m_player.px->field_0x18, &(const Vec3f &)fn_8040CDA8(&m_forward, m_speed));
    fn_80076CA8(m_player.get()->m_stateBits, 8);
    fn_80076CA8(m_player.get()->m_stateBits, 0x31);
    fn_80014E1C(&m_player.get()->m_flags, 1);
    fn_80014E1C(&m_player.get()->m_flags, 6);
    fn_80014E1C(&m_player.get()->m_flags, 7);
    fn_80076CA8(m_player.get()->m_stateBits, 0x38);
    fn_80076CA8(m_player.get()->m_stateBits, 0x40);
    fn_80076CA8(m_player.get()->m_stateBits, 0x42);
    m_player.get()->m_motion->vf_0x10(0x15, 10.0f);
    fn_800AA7B8(m_player.get()->m_motion, 1.0f);
    PlayerMotionEventCallback(m_player.get()) =
        MotionEventCallback(boost::bind(&Grind::OnMotion, this, _1));
    m_state = 0;
    fn_800AA700(m_player.get()->m_motion, 1);
    fn_800A2C70(reinterpret_cast<void *>(m_player.get()->field_0x23C));
}

void Grind::Leave(int nextId) {
    fn_80076CC8(m_player.get()->m_stateBits, 8);
    fn_8006127C(&m_player.get()->m_flags, 1);
    fn_8006127C(&m_player.get()->m_flags, 6);
    fn_8006127C(&m_player.get()->m_flags, 7);
    fn_80076CC8(m_player.get()->m_stateBits, 0x31);
    fn_80076CC8(m_player.get()->m_stateBits, 0x38);
    fn_80076CC8(m_player.get()->m_stateBits, 0x40);
    fn_80076CC8(m_player.get()->m_stateBits, 0x42);
    fn_80076CA8(m_player.get()->m_stateBits, 0x32);
    fn_8007684C(&m_rail);
    m_player.get()->m_rotation.x = DegToRad(0.0f);
    m_player.get()->m_rotation.z = DegToRad(0.0f);
    PlayerMotionEventCallback(m_player.get()).Clear();
    fn_800AA700(m_player.get()->m_motion, 0);
    fn_800A2C4C(reinterpret_cast<void *>(m_player.get()->field_0x23C));
}

void Grind::vf_0x18(CharCommand *command) {
    switch (command->TypeId()) {
    case 0x102:
        m_flags.Set(5);
        command->Flags() |= 2;
        break;
    case 0x104:
        m_flags.Set(2);
        command->Flags() |= 2;
        break;
    case 0x106:
        if (!fn_8006CD78(m_player.get()->m_stateBits, 0x1E)) {
            m_flags.Set(3);
        }
        command->Flags() |= 2;
        break;
    case 0x11E:
        m_flags.Set(4);
        command->Flags() |= 2;
        break;
    case 2:
    case 0x121:
        command->Flags() |= 2;
        break;
    case 3:
        if (fn_8007646C(&m_rail)) {
            m_player->m_rotation = static_cast<SetAngleCommand *>(command)->m_angles;
            fn_800ABB80(&m_rail);
            fn_800ABAE8(&m_rail, 0, &m_forward, 0);
        }
        command->Flags() |= 2;
        break;
    case 4:
        m_speed = 0.0f;
        command->Flags() |= 2;
        break;
    case 5:
        m_speed = fn_8040CF50(&static_cast<SetVelocityCommand *>(command)->m_velocity);
        command->Flags() |= 2;
        break;
    }
}

void Grind::Update(float dt) {
    if (!fn_8007646C(&m_rail) || !fn_800ABB14(&m_rail, dt * m_speed)) {
        fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(2));
        fn_8009728C(&m_player, 0);
        return;
    }
    bool sparks = false;
    switch (m_state) {
    case 0:
        m_player.get()->m_motion->vf_0x10(0x15, 10.0f);
        m_state = 1;
    case 1:
        if (fn_800ABB48(&m_rail) == 3) {
            m_player.get()->PushCommand(new DamageCommand(4, 1.0f));
        }
        sparks = true;
        if (m_flags.Test(1)) {
            m_state = 2;
            fn_800ABAE8(&m_rail, 0, &m_forward, &m_up);
        } else if (m_flags.Test(2)) {
            if (ChangeRail()) {
                break;
            }
            Vec3f direction;
            if (fn_8007646C(&m_rail)) {
                fn_800ABAE8(&m_rail, 0, 0, &direction);
            } else {
                fn_800091D0(&direction, &(const Vec3f &)fn_80079698(m_player.get()));
            }
            fn_8040F990(&m_player.get()->field_0x18, 0, &m_player.get()->field_0x18, &m_forward);
            InitializeArgsJump args(direction);
            fn_800A7F14(m_player.get()->m_behaviors, &args);
            return;
        } else if (m_flags.Test(3)) {
            if (ChangeRail()) {
                break;
            }
            m_flags.Flip(0);
            m_speed += 50.0f;
        } else if (m_flags.Test(5)) {
            m_flags.Clear(5);
            if (fn_8006CD78(m_player.get()->m_stateBits, 0x1F)) {
                switch (m_player.get()->m_weapon->GetWeaponKind()) {
                case 5:
                    m_player.get()->m_motion->vf_0x10(0x20, 2.0f);
                    m_flags.Clear(6);
                    m_flags.Set(8);
                    m_state = 5;
                    break;
                }
            }
        } else if (m_flags.Test(4)) {
            m_flags.Clear(4);
            m_player.get()->m_motion->vf_0x10(0x17, 0.0f);
            fn_80076CC8(m_player.get()->m_stateBits, 0x3D);
            m_state = 4;
            break;
        }
        Move(dt);
        UpdateSpeed(dt);
        if (m_flags.Test(0)) {
            fn_800AA7B8(m_player.get()->m_motion, 1.0f);
        } else {
            fn_800AA7B8(m_player.get()->m_motion, -1.0f);
        }
        break;
    case 2:
        Move(dt);
        if (!m_flags.Test(1)) {
            m_state = 0;
            fn_8040CE60(&m_offset);
        }
        break;
    case 3:
        UpdateChange(dt);
        break;
    case 4:
        Move(dt);
        if (m_flags.Test(7)) {
            fn_800B7BCC(&m_player);
            m_flags.Clear(7);
        }
        if (fn_800A9E1C(&m_player)) {
            fn_800B7BCC(&m_player);
            m_state = 0;
        }
        break;
    case 5:
        sparks = true;
        if (fn_800A9E1C(&m_player)) {
            m_state = 0;
        }
        if (m_flags.Test(8) && !fn_8006CD78(m_player.get()->m_stateBits, 0x1F)) {
            m_flags.Clear(8);
            m_state = 0;
        } else if (m_flags.Test(6)) {
            if (!m_flags.Test(5)) {
                m_state = 0;
            }
            m_flags.Clear(5);
            m_flags.Clear(6);
        }
        Move(dt);
        break;
    }
    fn_8009728C(&m_player, 0);
    fn_80076CC8(m_player.get()->m_stateBits, 2);
    if (!fn_8000A950(dt)) {
        fn_800091D0(&m_player.px->field_0x18, &(const Vec3f &)fn_8040CDA8(&m_forward, m_speed));
    }
    m_flags.Clear(1);
    m_flags.Clear(2);
    m_flags.Clear(3);
    fn_800A2C28(reinterpret_cast<void *>(m_player.get()->field_0x23C), sparks);
}

/* The rail beside the player on the side the input points to, and that side (1 or 2); an empty
 * target when there is none. */
UnkCircuitTarget_800ABCE8 Grind::FindRail(int *side) {
    if (fn_8006CD78(m_player.get()->m_stateBits, 0x1B)) {
        Vec3f offset;
        switch (fn_80093C60(&m_player, &FieldsOf(m_player.get())->field_0xB8)) {
        case 1:
            fn_800091D0(&offset, &(const Vec3f &)Vec3f(1.0f, 0.0f, 0.0f));
            if (side != 0) {
                *side = 1;
            }
            break;
        case 2:
            fn_800091D0(&offset, &(const Vec3f &)Vec3f(-1.0f, 0.0f, 0.0f));
            if (side != 0) {
                *side = 2;
            }
            break;
        default:
            return UnkCircuitTarget_800ABCE8();
        }
        Vec3f from;
        fn_800091EC(&from, &(const Vec3f &)fn_8040CDD0(
                               &m_player.px->m_position,
                               &(const Vec3f &)fn_80079628(
                                   m_player.px,
                                   &(const Vec3f &)fn_8040CDD0(
                                       &(const Vec3f &)fn_8040CDA8(
                                           &(const Vec3f &)Vec3f(0.0f, -1.0f, 0.0f),
                                           Height(m_player.px)),
                                       &(const Vec3f &)Scale(offset, s_sideReach)))));
        UnkCircuitTarget_800ABCE8 rail;
        int found = fn_800ABC0C(&rail, &m_player, &from,
                                &(const Vec3f &)fn_800796DC(m_player.get()), 0, s_aheadReach);
        if (found == 2 || found == 3) {
            return rail;
        }
    }
    return UnkCircuitTarget_800ABCE8();
}

void Grind::OnMotion(MotionEventArgs &args) {
    switch (args.m_event) {
    case 0xD003:
        m_flags.Set(1);
        break;
    case 0x5000:
        switch (*args.m_value) {
        case 1:
            m_flags.Set(6);
            break;
        case 2:
            break;
        case 3:
            m_flags.Set(7);
            break;
        }
        break;
    }
}

/* Moves the player along the rail, turning its frame towards the rail's. */
void Grind::Move(float dt) {
    Vec3f position;
    Vec3f forward;
    Vec3f up;
    fn_800ABAE8(&m_rail, &position, &forward, &up);
    AddVector(m_offset, FieldsOf(m_player.operator->())->field_0xC4);
    if (m_state != 2) {
        float angle = dt * DegToRad(360.0f);
        AssignVector(m_up, TurnTowards(&m_up, &up, angle));
        AssignVector(m_forward, TurnTowards(&m_forward, &forward, angle));
        Vec3f side;
        fn_800091EC(&side, &(const Vec3f &)fn_8040CFB8(&m_forward, &m_up));
        fn_8040CE74(&m_up);
        AssignVector(m_forward, fn_8040CEE8(&(const Vec3f &)fn_8040CFB8(&m_up, &side)));
    }
    AddVector(position, fn_800091A4(&m_up, fn_8007E514(m_player.operator->())));
    fn_800091D0(&m_player.px->m_position, &(const Vec3f &)fn_8040CDD0(&position, &m_offset));
    m_player->m_rotation = fn_8040F364(&m_forward, &m_up);
}

/* Moves the player over to the rail it is changing to, along an arc. */
void Grind::UpdateChange(float dt) {
    Vec3f position;
    Vec3f up;
    fn_800ABAE8(&m_rail, &position, &m_forward, &up);
    fn_8040CFF8(&m_position, &(const Vec3f &)(position - m_railPosition));
    fn_800091D0(&m_railPosition, &position);
    float time = m_changeTime + dt;
    if (time > 0.47f) {
        m_player.get()->m_motion->vf_0x10(0x15, 10.0f);
        if (m_flags.Test(0)) {
            fn_800AA7B8(m_player.get()->m_motion, 1.0f);
        } else {
            fn_800AA7B8(m_player.get()->m_motion, -1.0f);
        }
        m_state = 0;
    } else {
        m_changeTime = time;
        fn_800091D0(&position,
                    &(const Vec3f &)Arc(m_position, position,
                                        fn_8040CDA8(&FieldsOf(m_player.px)->m_up, -80.0f),
                                        m_changeTime / 0.47f));
    }
    fn_800091D0(&m_player.px->m_position,
                &(const Vec3f &)fn_8040CDD0(
                    &position, &(const Vec3f &)fn_8040CDA8(&m_up, m_player.px->field_0x274)));
    m_player->m_rotation = fn_8040F364(&m_forward, &m_up);
}

/* The speed: slower uphill and faster downhill, down to a floor; firing a weapon slows it. */
void Grind::UpdateSpeed(float dt) {
    if (fn_8006CD78(m_player.get()->m_stateBits, 0x1F) && m_player.get()->m_weapon->IsFiring()) {
        boost::shared_ptr<Weapon::WeaponBase> weapon = m_player.get()->m_weapon->GetWeapon();
        float recoil = 0.0f;
        if (weapon.get() != 0) {
            recoil = weapon->vf_0x28();
        }
        m_speed = m_speed * (1.0f - recoil);
    }
    if (!fn_8006CD78(m_player.get()->m_stateBits, 0x2E)) {
        float slope = fn_8040CF90(&FieldsOf(m_player.get())->m_up, &m_forward);
        if (slope >= 0.0f) {
            m_speed -= dt * (100.0f * slope);
        } else {
            m_speed -= dt * (40.0f * slope);
        }
        if (m_speed > 360.0f) {
            m_speed -= 100.0f * dt;
            m_speed = m_speed > 360.0f ? m_speed : 360.0f;
        }
        m_speed = m_speed > 10.0f ? m_speed : 10.0f;
    }
}

/* Changes to the rail beside the player if there is one. */
bool Grind::ChangeRail() {
    int side;
    UnkCircuitTarget_800ABCE8 rail = FindRail(&side);
    if (fn_8007646C(&rail)) {
        fn_800ABAE8(&m_rail, &m_position, 0, 0);
        m_rail = rail;
        fn_800ABAE8(&m_rail, &m_railPosition, 0, 0);
        m_state = 3;
        m_changeTime = 0.0f;
        m_player.get()->m_motion->vf_0x10(0x16, 2.0f);
        float facing;
        if (side == 1) {
            facing = 1.0f;
        } else {
            facing = -1.0f;
        }
        m_flags.Set(0);
        fn_800AA7B8(m_player.get()->m_motion, facing);
        return true;
    }
    return false;
}

void Grind::vf_0x20() {}

Behavior *Grind::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new Grind(player);
}

void Grind::Release() {}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds Grind by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804AFF64 = {
    Player::Behavior::Grind::Create, Player::Behavior::Grind::Release, 0};
