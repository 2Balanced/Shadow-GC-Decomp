/* Player::Behavior::LineHang (class name from RTTI): behavior 0xE of Shadow, hanging from a line (a
 * pole or a rope; Player::Behavior::InitializeArgsLineHang). The player rides the line's
 * character (RideCommand, LeaveCommand when it lets go) and is held at the point of the line
 * nearest to its hands: below a level line it swings and turns along the input; on an upright line
 * (mode 1) it climbs with the input and falls (behavior 2) off the top. Commands move the line,
 * pass the input on to the line's character, let go (behavior 0x18) or turn the player over. File
 * name inferred from the class (as Game/PlayerBehaviorSuper.cpp); all other names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). Where the game reloads the player pointer in a call's arguments after another argument's
 * call, the code reads m_player.px (as in Game/PlayerBehaviorGrind.cpp). */
/* Declares Player::Behavior::Behavior's slots with their arguments (see Game/PlayerBehavior.h). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
#include "Game/CharCommand.h"
#include "Game/GadgetCharCommands.h"
#include "Game/GeometryLine.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerSoundBase.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/Vibration.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <cmath>
#include <dolphin/types.h>
#include "size_assert.h"

using Sonicteam::System::Geometry::Line;

/* The player's members read here (struct name unknown; Player::PlayerBase leaves them
 * unmapped). */
struct UnkPlayerFields_8008A838 {
    /* 0x000 */ u8 field_0x0[0x9C];
    /* 0x09C */ float m_hangWait; /* set by letting go; a hang may start again once it is not
                                     positive (meaning inferred) */
    /* 0x0A0 */ u8 field_0xA0[0xB8 - 0xA0];
    /* 0x0B8 */ Vec3f field_0xB8;     /* the input's direction (as in Game/PlayerBehaviorEdgeHang.cpp) */
    /* 0x0C4 */ u8 field_0xC4[0xD0 - 0xC4];
    /* 0x0D0 */ Vec3f m_prevPosition; /* (as in Game/PlayerBehaviorLightDash.cpp) */
    /* 0x0DC */ u8 field_0xDC[0xE8 - 0xDC];
    /* 0x0E8 */ Vec3f m_jumpUp;       /* (as in Game/PlayerBehaviorHomingAttack.cpp) */
    /* 0x0F4 */ Vec3f m_up;
    /* 0x100 */ u8 field_0x100[0x17C - 0x100];
    /* 0x17C */ Vec3f field_0x17C; /* an axis the hands are offset along (meaning unknown) */
    /* 0x188 */ u8 field_0x188[0x1C8 - 0x188];
    /* 0x1C8 */ Vec3f m_groundNormal; /* (as in Game/PlayerBehaviorHomingAttack.cpp) */
};

/* A part of a player character fn_80078CEC returns (as in Game/PlayerBehaviorGrind.cpp). */
struct UnkPlayerPart_80078CEC {
    /* 0x00 */ u8 field_0x0[0xC];
    /* 0x0C */ Vec3f m_position;
};

/* a - b (0x8000AD64; as in Game/PlayerBehaviorEdgeHang.cpp). */
Vec3f operator-(const Vec3f &a, const Vec3f &b);

extern "C" {
/* Whether a value is about zero (the game's out-of-line copy; see
 * Game/PlayerBehaviorGrind.cpp). */
bool fn_8000A950(float value);
/* The player's flag words and state bits (see Game/PlayerShadowSuper.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
bool fn_8006CD78(u32 *bits, int bit);
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* -v (the game's out-of-line unary minus; behaviour from its body). */
Vec3f fn_80016284(const Vec3f *v);
/* The sound manager and playing a sound at a character for an owner (see Game/BAGunShip.cpp). */
void *fn_80053164(void);
void fn_801D53C4(void *manager, int id, CharInfo *character, void *owner, int arg4, int arg5,
                 int arg6);
/* std::min for floats (an out-of-line copy; see Game/BirdBase.cpp). */
const float &fn_800704AC(const float &a, const float &b);
/* The player's part index (see Game/CatchStick.cpp). */
UnkPlayerPart_80078CEC *fn_80078CEC(Player::PlayerBase *player, int index);
/* The player's front axis (see Game/PlayerBehaviorGrind.cpp). */
Vec3f fn_800796DC(Player::PlayerBase *player);
/* Player updates run before and after the behavior's own (behaviour unknown; as in FirstMeet). */
void fn_80079720(Player::PlayerBase *player);
/* Setting the player's up axis (see Game/PlayerBehaviorChaosControl.cpp), turning it towards a
 * direction, the input's direction relative to a direction, and the direction it faces (behaviour
 * inferred; as in Super). */
void fn_8009339C(boost::shared_ptr<Player::PlayerBase> *player, const Vec3f *up);
void fn_80093BA0(boost::shared_ptr<Player::PlayerBase> *player, Vec3f *direction, float rate);
int fn_80093C60(boost::shared_ptr<Player::PlayerBase> *player, const Vec3f *direction);
Vec3f fn_80093FC0(boost::shared_ptr<Player::PlayerBase> *player);
/* Switches a player's behavior (behaviour inferred). */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);
/* The player's motion: the motion playing, and setting its rate (meanings inferred; as in
 * Game/PlayerBehaviorGrind.cpp). */
int fn_800AA7B0(Player::PlayerMotion *motion);
void fn_800AA7B8(Player::PlayerMotion *motion, float value);
/* Starts vibration pattern on the controller of player (see Game/BirdBase.cpp). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int player);

Vec3f fn_800091A4(const Vec3f *vector, float scale); /* vector * scale */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale); /* vector * scale */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);   /* a + b */
void fn_8040CE60(Vec3f *vector);                     /* vector = 0 */
Vec3f fn_8040CEE8(const Vec3f *vector);              /* normalized */
float fn_8040CF2C(const Vec3f *v);                   /* |v|^2 */
float fn_8040CF90(const Vec3f *a, const Vec3f *b);   /* dot product */
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b);   /* cross product */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* vector += value */
float fn_8040D180(const Vec3f *a, const Vec3f *b);   /* the squared distance */
/* Whether point lies across the line's span, and the line's point nearest to point (behaviour
 * inferred). */
bool fn_8040FB88(const Line *line, const Vec3f *point);
Vec3f fn_8040FC3C(const Line *line, const Vec3f *point);
}

/* The player's members above (a macro, as in Game/PlayerBehaviorSuper.cpp). */
#define FieldsOf(player) reinterpret_cast<UnkPlayerFields_8008A838 *>(player)

/* fn_8040CD74 returns lhs - rhs through its first argument, as a function returning a Vec3f does;
 * called this way the result is a temporary, as in the game (see Game/PlayerBehaviorSuper.cpp). */
typedef Vec3f (*VectorSubtract)(const Vec3f *lhs, const Vec3f *rhs);
#define Subtract(lhs, rhs) (reinterpret_cast<VectorSubtract>(fn_8040CD74)(lhs, rhs))

/* a + b (an inline wrapper; the player's operator-> in its arguments stays out of line). */
inline Vec3f Add(const Vec3f &a, const Vec3f &b) {
    return fn_8040CDD0(&a, &b);
}

/* v . the player's ground normal (an inline helper: the game loads the player before working out
 * v; name inferred). */
inline float DotGroundNormal(const Vec3f &v, Player::PlayerBase *player) {
    return fn_8040CF90(&v, &FieldsOf(player)->m_groundNormal);
}

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

/* The hang's flag word (struct name unknown): bit 0 the hands are at the line's top end, bit 1
 * the climb may go over the top, bit 2 a pull up asked for (command 0x106). */
struct LineHangFlags {
    LineHangFlags() {
        fn_80014E04(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    void Clear(int bit) {
        fn_8006127C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }
    void Reset() {
        fn_80014E10(&m_bits);
    }

    u32 m_bits;
};

class LineHang : public Behavior {
  public:
    LineHang(const boost::shared_ptr<PlayerBase> &player);
    virtual ~LineHang();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x18(CharCommand *command);
    virtual void Update(float dt);
    virtual void vf_0x20();

    /* The factory record's functions (names inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);
    static void Release();

    void UpdateLevel(float dt);
    void UpdateUpright(float dt);
    void TurnOver();
    void TurnBack();
    int Classify(const boost::weak_ptr<Line> &line);
    void UpdateLine();
    Vec3f HandOffset();
    void MoveToLine(float dt);

    static const char *s_name;

    /* 0x1C */ s32 m_mode;        /* 0 a level line, 1 an upright one */
    /* 0x20 */ Vec3f m_point;     /* the line's point nearest to the hands */
    /* 0x2C */ Vec3f m_facing;    /* where the player turns to */
    /* 0x38 */ Vec3f m_up;        /* the player's up axis while turned over */
    /* 0x44 */ boost::weak_ptr<CharInfo> m_owner; /* the line's character */
    /* 0x4C */ boost::weak_ptr<Line> m_line;
    /* 0x54 */ LineHangFlags m_flags;
    /* 0x58 */ float m_moveTime; /* left of the move onto the line */
};
SIZE_ASSERT(LineHang, 0x5C);

const char *LineHang::s_name = "LineHang";

/* The largest share of the move onto the line one frame makes (name inferred). */
static float s_maxStep = 1.0f;

/* How far the hands are from the player along the axis at 0x17C and ahead, below a level line and
 * beside an upright one (names inferred). The game sets them in __sinit rather than in .sdata, as
 * an initializer that is not a constant expression does; the inline call reproduces that (as in
 * Game/PlayerBehaviorFall.cpp). */
inline float DynamicFloat(float value) {
    return value;
}
static float s_levelUp = DynamicFloat(6.0f);
static float s_levelAhead = DynamicFloat(0.5f);
static float s_uprightUp = DynamicFloat(4.0f);
static float s_uprightAhead = DynamicFloat(0.0f);

LineHang::LineHang(const boost::shared_ptr<PlayerBase> &player) : Behavior(player, s_name, 0xE) {}

LineHang::~LineHang() {}

bool LineHang::vf_0x0C(int prevId, const InitializeArgs &args) {
    if (FieldsOf(m_player.get())->m_hangWait > 0.0f) {
        return false;
    }
    if (fn_8006CD78(m_player.get()->m_stateBits, 0x1E)) {
        return false;
    }
    const InitializeArgsLineHang &hang = static_cast<const InitializeArgsLineHang &>(args);
    boost::shared_ptr<Line> line = hang.m_line.lock();
    if (!fn_8040FB88(line.get(), &m_player.get()->m_position)) {
        return false;
    }
    int mode = Classify(hang.m_line);
    Vec3f point;
    fn_800091EC(&point, &(const Vec3f &)fn_8040FC3C(line.get(), &m_player.get()->m_position));
    if (mode == 0) {
        if (fn_8040CF90(&(const Vec3f &)Subtract(&point, &m_player.get()->m_position),
                        &FieldsOf(m_player.get())->m_jumpUp) > 0.0f) {
            return true;
        }
    } else {
        if (fn_8006CD78(m_player.get()->m_stateBits, 2)) {
            return false;
        }
        Vec3f front;
        fn_800091EC(&front, &(const Vec3f &)fn_800796DC(m_player.get()));
        if (fn_8040CF90(&(const Vec3f &)Subtract(
                            &point, &(const Vec3f &)fn_8040CDD0(
                                        &m_player.px->m_position,
                                        &(const Vec3f &)fn_8040CDA8(&front, -1.0f))),
                        &front) < 0.0f) {
            UnkPlayerPart_80078CEC *hands = fn_80078CEC(m_player.get(), 1);
            if (fn_8040CF90(&(const Vec3f &)Subtract(
                                &point, &(const Vec3f &)fn_8040CDD0(
                                            &hands->m_position,
                                            &(const Vec3f &)fn_8040CDA8(&front, -1.0f))),
                            &front) < 0.0f) {
                return false;
            }
        }
        return true;
    }
    return false;
}

void LineHang::Enter(int prevId, const InitializeArgs &args) {
    const InitializeArgsLineHang &hang = static_cast<const InitializeArgsLineHang &>(args);
    m_flags.Reset();
    m_owner = hang.m_owner;
    m_line = hang.m_line;
    boost::shared_ptr<Line> line = m_line.lock();
    fn_80076CA8(m_player.get()->m_stateBits, 0x34);
    fn_80076CA8(m_player.get()->m_stateBits, 0xB);
    fn_80076CA8(m_player.get()->m_stateBits, 0xC);
    fn_80076CC8(m_player.get()->m_stateBits, 2);
    fn_80076CC8(m_player.get()->m_stateBits, 0xF);
    fn_80014E1C(&m_player.get()->m_flags, 1);
    fn_80014E1C(&m_player.get()->m_flags, 7);
    m_mode = Classify(m_line);
    if (m_mode == 0) {
        m_player.get()->m_motion->vf_0x10(0xE, 2.0f);
    } else {
        m_player.get()->m_motion->vf_0x10(0xF, 2.0f);
        fn_80076CA8(m_player.get()->m_stateBits, 0x3A);
    }
    fn_8040CE60(&m_player.get()->field_0x18);
    if (prevId == 0x18) {
        TurnOver();
    }
    fn_800091D0(&m_up, &FieldsOf(m_player.get())->m_up);
    if (!m_owner.expired()) {
        boost::shared_ptr<CharInfo> owner = m_owner.lock();
        owner->PushCommand(
            new RideCommand(boost::weak_ptr<CharInfo>(m_player), m_player.get()->m_position));
    }
    m_moveTime = 0.1f;
    UpdateLine();
    m_player.get()->m_sound->StopVoice(0x14);
    fn_8034DF48(10, m_player.get()->field_0x54);
}

void LineHang::Leave(int nextId) {
    fn_8006127C(&m_player.get()->m_flags, 1);
    fn_8006127C(&m_player.get()->m_flags, 7);
    fn_80076CC8(m_player.get()->m_stateBits, 0x34);
    fn_80076CC8(m_player.get()->m_stateBits, 0xB);
    fn_80076CC8(m_player.get()->m_stateBits, 0xC);
    fn_80076CC8(m_player.get()->m_stateBits, 0x3A);
    fn_80076CC8(m_player.get()->m_stateBits, 0x39);
    if (!m_owner.expired()) {
        boost::shared_ptr<CharInfo> owner = m_owner.lock();
        owner->PushCommand(
            new LeaveCommand(boost::weak_ptr<CharInfo>(m_player), m_player.get()->m_position));
    }
    FieldsOf(m_player.get())->m_hangWait = 0.5f;
}

void LineHang::vf_0x18(CharCommand *command) {
    switch (command->TypeId()) {
    case 1:
        if (!m_owner.expired()) {
            boost::shared_ptr<CharInfo> owner = m_owner.lock();
            if (owner.get() != 0) {
                owner->PushCommand(new AccelerateInputCommand(
                    static_cast<AccelerateInputCommand *>(command)->m_direction, 4));
            }
        }
        break;
    case 6: {
        AccelerateCommand *accelerate = static_cast<AccelerateCommand *>(command);
        if (accelerate->m_velocity.x * accelerate->m_velocity.x +
                accelerate->m_velocity.y * accelerate->m_velocity.y +
                accelerate->m_velocity.z * accelerate->m_velocity.z <
            0.0001f) {
            fn_800091D0(&m_up, &FieldsOf(m_player.get())->m_jumpUp);
        } else {
            fn_800091D0(&m_up, &(const Vec3f &)fn_80016284(
                                   &(const Vec3f &)fn_8040CEE8(&accelerate->m_velocity)));
        }
        command->Flags() |= 2;
        break;
    }
    case 0x104:
        if (fn_8006CD78(m_player.get()->m_stateBits, 0xF)) {
            fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(0x18));
            command->Flags() |= 2;
        }
        break;
    case 0x106:
        switch (m_mode) {
        case 1:
            m_flags.Set(2);
            break;
        default:
            if (fn_8006CD78(m_player.get()->m_stateBits, 0xF)) {
                fn_800A7F14(m_player.get()->m_behaviors,
                            &(const InitializeArgs &)InitializeArgs(0x18));
            } else {
                fn_800A7F14(m_player.get()->m_behaviors,
                            &(const InitializeArgs &)InitializeArgs(2));
            }
            break;
        }
        command->Flags() |= 2;
        break;
    case 0x107:
        m_flags.Clear(2);
        break;
    case 0x111: {
        TouchLineCommand *touch = static_cast<TouchLineCommand *>(command);
        if (m_owner.lock() == touch->m_target.lock()) {
            m_line = touch->m_line;
        }
        command->Flags() |= 2;
        break;
    }
    case 0x122:
        if (static_cast<StreamCommand *>(command)->m_on) {
            TurnOver();
        } else {
            TurnBack();
        }
        command->Flags() |= 2;
        break;
    }
}

void LineHang::Update(float dt) {
    UpdateLine();
    switch (m_mode) {
    case 0:
        UpdateLevel(dt);
        break;
    case 1:
        UpdateUpright(dt);
        break;
    }
    if (!fn_8000A950(dt)) {
        fn_800091D0(&m_player.px->field_0x18,
                    &(const Vec3f &)fn_8040CDA8(
                        &(const Vec3f &)Subtract(&m_player.get()->m_position,
                                                 &FieldsOf(m_player.get())->m_prevPosition),
                        1.0f / dt));
    }
}

/* Hanging from a level line: the swing motion (another one turned over, state bit 0xF) and its
 * rate from the input (name inferred). */
void LineHang::UpdateLevel(float dt) {
    if (fn_8006CD78(m_player.get()->m_stateBits, 0xF)) {
        fn_800091D0(&FieldsOf(m_player.get())->m_up, &m_up);
        if (fn_800AA7B0(m_player.get()->m_motion) != 0x28) {
            m_player.get()->m_motion->vf_0x10(0x28, 10.0f);
        }
    } else if (fn_800AA7B0(m_player.get()->m_motion) != 0xE) {
        m_player.get()->m_motion->vf_0x10(0xE, 10.0f);
    }
    fn_8009339C(&m_player, &FieldsOf(m_player.get())->m_up);
    MoveToLine(dt);
    if (!fn_8006CD78(m_player.get()->m_stateBits, 0xF) &&
        fn_8006CD78(m_player.get()->m_stateBits, 0x1B) &&
        fn_8040CF2C(&FieldsOf(m_player.get())->field_0xB8) > 0.25f) {
        switch (fn_80093C60(&m_player, &FieldsOf(m_player.get())->field_0xB8)) {
        case 1:
            fn_800AA7B8(m_player.get()->m_motion, 1.0f);
            break;
        case 2:
            fn_800AA7B8(m_player.get()->m_motion, -1.0f);
            break;
        }
    }
}

/* Climbing an upright line: up and down with the input, over the top when it can, turning about
 * the line with the input's side (name inferred). */
void LineHang::UpdateUpright(float dt) {
    MoveToLine(dt);
    if ((fn_8006CD78(m_player.get()->m_stateBits, 0x1B) &&
         fn_8040CF2C(&FieldsOf(m_player.get())->field_0xB8) > 0.25f) ||
        m_flags.Test(2)) {
        Vec3f facing;
        fn_800091EC(&facing, &(const Vec3f &)fn_80093FC0(&m_player));
        fn_8040CF90(&FieldsOf(m_player.get())->field_0xB8, &facing);
        if (FieldsOf(m_player.get())->field_0xB8.z < -0.5f) {
            fn_800AA7B8(m_player.get()->m_motion, 1.0f);
        } else if (FieldsOf(m_player.get())->field_0xB8.z > 0.5f || m_flags.Test(2)) {
            if (m_flags.Test(0) && m_flags.Test(1)) {
                fn_800A7F14(m_player.get()->m_behaviors,
                            &(const InitializeArgs &)InitializeArgs(2));
            } else {
                fn_800AA7B8(m_player.get()->m_motion, -1.0f);
                m_flags.Clear(1);
                if (!m_flags.Test(0)) {
                    fn_801D53C4(fn_80053164(), 0x3007, 0, this, 2, 0, 5);
                }
            }
        }
        if (std::fabs(FieldsOf(m_player.get())->field_0xB8.x) > 0.5f) {
            m_player.get()->m_rotation.y -= 4.5f * dt * FieldsOf(m_player.get())->field_0xB8.x;
        }
    } else if (m_flags.Test(0)) {
        m_flags.Set(1);
    }
}

/* Turns the player over (state bits 0x39 and 0xF) and back (names inferred). */
void LineHang::TurnOver() {
    fn_80076CA8(m_player.get()->m_stateBits, 0x39);
    fn_80076CA8(m_player.get()->m_stateBits, 0xF);
}

void LineHang::TurnBack() {
    fn_80076CC8(m_player.get()->m_stateBits, 0x39);
    fn_80076CC8(m_player.get()->m_stateBits, 0xF);
}

/* 1 when the line stands upright (along the player's jump direction), 0 when it is level (name
 * inferred). */
int LineHang::Classify(const boost::weak_ptr<Line> &line) {
    boost::shared_ptr<Line> locked = line.lock();
    if (locked.get() != 0) {
        Vec3f direction;
        fn_800091EC(&direction, &(const Vec3f &)fn_8040CEE8(&(const Vec3f &)Subtract(
                                    &locked.get()->m_start, &locked.get()->m_end)));
        if (std::fabs(fn_8040CF90(&direction, &FieldsOf(m_player.get())->m_jumpUp)) > 0.5f) {
            return 1;
        }
    }
    return 0;
}

/* The line's point nearest to the hands, whether the hands are at its top end, and where the
 * player turns to (name inferred). */
void LineHang::UpdateLine() {
    boost::shared_ptr<Line> line = m_line.lock();
    if (line.get() != 0) {
        Vec3f hands;
        fn_800091EC(&hands, &(const Vec3f &)fn_8040CDD0(&m_player.px->m_position,
                                                        &(const Vec3f &)HandOffset()));
        fn_800091D0(&m_point, &(const Vec3f &)fn_8040FC3C(line.get(), &hands));
        Vec3f top;
        if (DotGroundNormal(Subtract(&line.get()->m_start, &line.get()->m_end), m_player.px) >=
            0.0f) {
            fn_800091D0(&top, &line.get()->m_end);
        } else {
            fn_800091D0(&top, &line.get()->m_start);
        }
        if (fn_8040D180(&m_point, &top) < 0.01f) {
            m_flags.Set(0);
        } else {
            m_flags.Clear(0);
        }
        if (m_mode == 0) {
            Vec3f along;
            fn_800091EC(&along, &(const Vec3f &)Subtract(&line.get()->m_end, &line.get()->m_start));
            Vec3f facing;
            fn_800091EC(&facing, &(const Vec3f &)fn_8040CEE8(&(const Vec3f &)fn_8040CFB8(
                                     &along, &FieldsOf(m_player.get())->m_up)));
            if (fn_8040CF90(&facing, &(const Vec3f &)fn_800796DC(m_player.get())) < 0.0f) {
                fn_800091D0(&facing, &(const Vec3f &)fn_80016284(&facing));
            }
            AssignVector(m_facing, facing);
        } else {
            fn_800091D0(&m_facing, &(const Vec3f &)fn_800796DC(m_player.get()));
        }
    }
}

/* Where the hands are from the player (name inferred). */
Vec3f LineHang::HandOffset() {
    if (m_mode == 0) {
        return Add(fn_800091A4(&FieldsOf(m_player.operator->())->field_0x17C, s_levelUp),
                   fn_800091A4(&(const Vec3f &)fn_800796DC(m_player.operator->()), s_levelAhead));
    } else {
        return Add(fn_800091A4(&FieldsOf(m_player.operator->())->field_0x17C, s_uprightUp),
                   fn_800091A4(&(const Vec3f &)fn_800796DC(m_player.operator->()), s_uprightAhead));
    }
}

/* Moves the player so that its hands reach the line's point, at once or over m_moveTime, and turns
 * it (name inferred). */
void LineHang::MoveToLine(float dt) {
    Vec3f goal;
    AssignVector(goal, Subtract(&m_point, &(const Vec3f &)HandOffset()));
    if (m_moveTime <= 0.0f) {
        m_moveTime = 0.0f;
        fn_800091D0(&m_player.get()->m_position, &goal);
    } else {
        float share = dt / m_moveTime;
        share = fn_800704AC(share, s_maxStep);
        AddVector(m_player->m_position, fn_800091A4(&(const Vec3f &)(goal - m_player->m_position), share));
        m_moveTime -= dt;
    }
    fn_80093BA0(&m_player, &m_facing, 6.5f * dt);
}

void LineHang::vf_0x20() {}

Behavior *LineHang::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new LineHang(player);
}

void LineHang::Release() {}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds LineHang by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804B0944 = {
    Player::Behavior::LineHang::Create, Player::Behavior::LineHang::Release, 0};
