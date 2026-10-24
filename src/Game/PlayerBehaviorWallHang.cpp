/* Player::Behavior::WallHang (class name from RTTI): behavior 0xD of Shadow, clinging to a wall
 * (Player::Behavior::InitializeArgsWallHang names the wall's normal and character). The player
 * turns to the wall and holds on; pushing towards the wall climbs onto it (listening to the
 * collisions, which keep the normal up to date) and moves along it. Command 0x104 jumps off
 * (behavior 0x10, Player::Behavior::InitializeArgsWallJump, telling the wall's character), command
 * 0x106 lets go, and losing the wall, the input, the hold time or the ground below makes the player
 * fall (behavior 2). File name inferred from the class (as Game/PlayerBehaviorSuper.cpp); all other
 * names are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). Where the game reloads the player pointer in a call's arguments after
 * another argument's call, the code reads m_player.px (as in Game/PlayerBehaviorGrind.cpp). */
/* Declares Player::Behavior::Behavior's slots with their arguments (see Game/PlayerBehavior.h). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
#include <boost/bind.hpp>
#include <boost/function.hpp>

/* Defines InitializeArgsWallJump's inline functions, whose out-of-line copies this unit emits. */
#define PLAYER_BEHAVIOR_ARGS_INLINE

#include "Game/CharCommand.h"
#include "Game/GeometryLine.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerMotionEvent.h"
#include "Game/PlayerSoundBase.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/Vibration.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Sonicteam::System::Geometry::Line;

namespace SonicteamUSA {
namespace System {
namespace CharaColli {
class CharaColliBase;
} // namespace CharaColli
} // namespace System
} // namespace SonicteamUSA

/* A collision object of the world (see Game/VehicleColliMove.cpp). */
class UnkColliObject_80028CA4;

/* What a mover stands on (as declared in Game/PlayerBehaviorEdgeHang.cpp). */
struct UnkColliMoveContact_803334A4 {
    UnkColliMoveContact_803334A4();
    ~UnkColliMoveContact_803334A4();

    /* Stands on character chara (0x803333A4). */
    void SetCharInfo(const boost::weak_ptr<CharInfo> &chara);

    /* 0x00 */ u8 m_isCollider;
    /* 0x04 */ boost::weak_ptr<CharInfo> m_chara;
    /* 0x0C */ boost::weak_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> m_collider;
    /* 0x14 */ UnkColliObject_80028CA4 *m_object;
};
SIZE_ASSERT(UnkColliMoveContact_803334A4, 0x18);

/* The object at 0x278 of the player (struct name unknown; as in
 * Game/PlayerBehaviorEdgeHang.cpp). */
struct UnkObject_8007FE50 {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ float field_0x20;
    /* 0x24 */ u8 field_0x24;
};

/* The player's members read here (struct name unknown; Player::PlayerBase leaves them
 * unmapped). */
struct UnkPlayerFields_800919A8 {
    /* 0x000 */ u8 field_0x0[0xB8];
    /* 0x0B8 */ Vec3f field_0xB8; /* the input's direction (as in Game/PlayerBehaviorEdgeHang.cpp) */
    /* 0x0C4 */ u8 field_0xC4[0xE8 - 0xC4];
    /* 0x0E8 */ Vec3f m_jumpUp; /* (as in Game/PlayerBehaviorHomingAttack.cpp) */
    /* 0x0F4 */ Vec3f m_up;     /* (as in Game/PlayerBehaviorEdgeHang.cpp) */
    /* 0x100 */ u8 field_0x100[0x1C8 - 0x100];
    /* 0x1C8 */ Vec3f m_groundNormal; /* (as in Game/PlayerBehaviorHomingAttack.cpp) */
    /* 0x1D4 */ u8 field_0x1D4[0x278 - 0x1D4];
    /* 0x278 */ UnkObject_8007FE50 *field_0x278;
};

extern "C" u32 fn_80026588(UnkColliContactNode_800587A0 *contact);

/* A contact of a collision (as in Game/PlayerBehaviorJumpDash.cpp): its object at 0x14. */
struct UnkColliContactNode_800587A0 {
    /* The contact's attributes, the low 28 bits of its flags (name inferred). */
    u32 GetAttributes() {
        return fn_80026588(this) & 0x0FFFFFFF;
    }

    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ void *m_object;
};

extern "C" {
/* The normal of a contact's object (see Game/BirdBase.cpp). */
const Vec3f *fn_8001E9C4(void *object);
/* -v (the game's out-of-line unary minus; behaviour from its body). */
Vec3f fn_80016284(const Vec3f *v);
/* The player's flag word and state bits (see Game/PlayerShadowSuper.cpp). */
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_8006CD78(u32 *bits, int bit);
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* The player's collision move: its contact, clearing it and setting it (as in
 * Game/PlayerBehaviorEdgeHang.cpp; behaviour inferred). */
UnkColliMoveContact_803334A4 *fn_80078DA0(Player::PlayerBase *player);
void fn_80078DD4(Player::PlayerBase *player);
void fn_80078E00(Player::PlayerBase *player, const UnkColliMoveContact_803334A4 *contact);
/* The player's front axis (see Game/PlayerBehaviorGrind.cpp). */
Vec3f fn_800796DC(Player::PlayerBase *player);
/* Player updates run before and after the behavior's own (behaviour unknown; as in FirstMeet). */
void fn_80079720(Player::PlayerBase *player);
void fn_8009728C(boost::shared_ptr<Player::PlayerBase> *player, int arg1);
/* Casts a line through the world; negative when nothing is hit (as in
 * Game/PlayerBehaviorEdgeHang.cpp). */
float fn_800979A0(boost::shared_ptr<Player::PlayerBase> *player, const Line *line, int arg2,
                  int arg3, int arg4, int arg5);
/* Switches a player's behavior (behaviour inferred). */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);
/* Rumbles player's controller with pattern (see Game/PlayerBehaviorHomingJump.cpp). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int player);

/* The rotation by angle about axis (see Game/ArkCannon.cpp). */
Mtx34f fn_80049778(const Vec3f *axis, float angle);
Vec3f fn_800091A4(const Vec3f *vector, float scale); /* vector * scale */
/* Matrix routines: the point transformed, and a copy (as in Game/PlayerBehaviorFirstMeet.cpp). */
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
void fn_8040C148(Mtx34f *matrix, const Mtx34f *value);
Vec3f fn_8040CDA8(const Vec3f *vector, float scale); /* vector * scale */
void fn_8040CE60(Vec3f *vector);                     /* vector = 0 */
void fn_8040CE74(Vec3f *vector);                     /* normalises it */
Vec3f fn_8040CEE8(const Vec3f *vector);              /* normalized */
float fn_8040CF2C(const Vec3f *v);                   /* |v|^2 */
float fn_8040CF90(const Vec3f *a, const Vec3f *b);   /* dot product */
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b);   /* cross product */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* vector += value */
Vec3f fn_8040F364(const Vec3f *forward, const Vec3f *up); /* Euler angles */
}

/* The player's members above (a macro, as in Game/PlayerBehaviorSuper.cpp). */
#define FieldsOf(player) reinterpret_cast<UnkPlayerFields_800919A8 *>(player)

/* fn_8040CD74 returns lhs - rhs through its first argument, as a function returning a Vec3f does;
 * called this way the result is a temporary, as in the game (see Game/PlayerBehaviorSuper.cpp). */
typedef Vec3f (*VectorSubtract)(const Vec3f *lhs, const Vec3f *rhs);
#define Subtract(lhs, rhs) (reinterpret_cast<VectorSubtract>(fn_8040CD74)(lhs, rhs))

/* Degrees to radians (name inferred; as in Game/PlayerBehaviorGrind.cpp). */
inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* vector += value (an inline wrapper, which orders the call's arguments as the game does). */
inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

/* A direction returned through the game's out-of-line vector copy constructor (fn_800091EC), as
 * AimPosition in Game/PlayerWeaponBase.h (name invented); callers read it as a Vec3f. */
struct WallJumpDirection {
    WallJumpDirection(const Vec3f &direction) {
        fn_800091EC(reinterpret_cast<Vec3f *>(this), &direction);
    }

    float x;
    float y;
    float z;
};

/* fn_8040B67C (the point transformed), called as returning the direction: the game builds the
 * result straight into the return value. */
typedef WallJumpDirection (*TransformDirection)(const Mtx34f *matrix, const Vec3f *point);

/* m = value (an inline wrapper; as in Game/PlayerBehaviorFall.cpp). */
inline void AssignMtx(Mtx34f &m, const Mtx34f &value) {
    fn_8040C148(&m, &value);
}

/* The wall motions (names inferred): holding on and climbing on, plain and mirrored. Enumerators,
 * as the game picks between them with a branch rather than with arithmetic. */
enum {
    MOTION_WALL_HOLD = 0x11,
    MOTION_WALL_HOLD_MIRRORED = 0x12,
    MOTION_WALL_CLIMB = 0x13,
    MOTION_WALL_CLIMB_MIRRORED = 0x14
};

namespace Player {
namespace Behavior {

class WallHang : public Behavior {
  public:
    WallHang(const boost::shared_ptr<PlayerBase> &player);
    virtual ~WallHang();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x18(CharCommand *command);
    virtual void Update(float dt);
    virtual void vf_0x20();

    /* The factory record's functions (names inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);
    static void Release();

    WallJumpDirection JumpDirection();
    void TurnToWall(const Vec3f &normal);
    void OnCollision(CollisionEventArgs &args);
    bool CheckWall();
    void Fall();

    static const char *s_name;

    /* 0x1C */ s32 m_mode;      /* 0 holding on, 1 climbing on, 2 moving along the wall */
    /* 0x20 */ Vec3f m_normal;  /* the wall's normal */
    /* 0x2C */ Vec3f m_input;   /* the input's direction */
    /* 0x38 */ float m_wait;    /* jumps and the input wait until it runs out */
    /* 0x3C */ float m_holdTime; /* the player falls when it runs out */
    /* 0x40 */ s32 m_contacts;  /* the frame's collisions with the wall */
    /* 0x44 */ s32 m_mirrored;  /* picks the mirrored motions */
    /* 0x48 */ boost::weak_ptr<CharInfo> m_wall; /* the wall's character */
};
SIZE_ASSERT(WallHang, 0x50);

} // namespace Behavior
} // namespace Player

const char *Player::Behavior::WallHang::s_name = "WallHang";

/* Attribute masks of the walls the player may hang on and of those it may not (meanings inferred;
 * Game/PlayerBehaviorJumpDash.cpp reads them too; they keep their lbl_ names). */
u32 lbl_805E4514 = 2;
u32 lbl_805E4518 = 0x200;

namespace Player {
namespace Behavior {

/* How far beyond its height the player looks for the wall (name inferred). The game sets it in
 * __sinit rather than in .sbss, as an initializer that is not a constant expression does; the
 * inline call reproduces that (as in Game/PlayerBehaviorFall.cpp). */
inline float DynamicFloat(float value) {
    return value;
}
static float s_wallReach = DynamicFloat(4.0f);

WallHang::WallHang(const boost::shared_ptr<PlayerBase> &player) : Behavior(player, s_name, 0xD) {}

WallHang::~WallHang() {}

bool WallHang::vf_0x0C(int prevId, const InitializeArgs &args) {
    return true;
}

void WallHang::Enter(int prevId, const InitializeArgs &args) {
    const InitializeArgsWallHang *hang = dynamic_cast<const InitializeArgsWallHang *>(&args);
    fn_80076CA8(m_player.get()->m_stateBits, 0xE);
    fn_80076CA8(m_player.get()->m_stateBits, 0x36);
    fn_80014E1C(&m_player.get()->m_flags, 1);
    fn_8040CE60(&m_player.get()->field_0x18);
    TurnToWall(reinterpret_cast<const Vec3f &>(hang->m_normal));
    fn_800091D0(&m_normal, reinterpret_cast<const Vec3f *>(&hang->m_normal));
    m_wait = 0.125f;
    m_holdTime = 2.0f;
    m_mode = 0;
    m_wall = hang->m_wall;
    {
        UnkColliMoveContact_803334A4 contact;
        contact.SetCharInfo(m_wall);
        fn_80078E00(m_player.get(), &contact);
        fn_80076CA8(m_player.get()->m_stateBits, 0x37);
    }
    m_player.get()->m_sound->StopVoice(0x13);
    fn_8034DF48(0xA, m_player.get()->field_0x54);
}

void WallHang::Leave(int nextId) {
    fn_80076CC8(m_player.get()->m_stateBits, 0);
    fn_80076CC8(m_player.get()->m_stateBits, 0xD);
    fn_80076CC8(m_player.get()->m_stateBits, 0xE);
    fn_80076CC8(m_player.get()->m_stateBits, 0x36);
    fn_80076CC8(m_player.get()->m_stateBits, 0x39);
    fn_8006127C(&m_player.get()->m_flags, 1);
    PlayerCollisionEventCallback(m_player.get()).Clear();
    fn_80076CC8(m_player.get()->m_stateBits, 0x37);
    fn_80078DD4(m_player.get());
}

void WallHang::vf_0x18(CharCommand *command) {
    switch (command->TypeId()) {
    case 0x104:
        if (m_wait <= 0.0f) {
            Vec3f direction;
            fn_800091EC(&direction,
                        reinterpret_cast<const Vec3f *>(&(const WallJumpDirection &)JumpDirection()));
            UnkColliMoveContact_803334A4 *contact = fn_80078DA0(m_player.get());
            if (contact != 0) {
                boost::shared_ptr<CharInfo> wall = contact->m_chara.lock();
                if (wall) {
                    wall->PushCommand(
                        new WallJumpCommand(m_player.get()->m_self, m_player.get()->m_position));
                }
            }
            fn_800A7F14(m_player.get()->m_behaviors,
                        &(const InitializeArgs &)InitializeArgsWallJump(m_normal, direction));
        }
        command->Flags() |= 2;
        break;
    case 0x105:
        break;
    case 0x106:
        if (m_wait <= 0.0f) {
            Fall();
        }
        command->Flags() |= 2;
        break;
    }
}

void WallHang::Update(float dt) {
    m_wait -= dt;
    if (m_wait < 0.0f) {
        m_wait = 0.0f;
    }
    m_holdTime -= dt;
    if (m_holdTime < 0.0f) {
        Fall();
        return;
    }
    Vec3f input;
    if (m_wait <= 0.0f) {
        fn_800091D0(&input, &FieldsOf(m_player.get())->field_0xB8);
    } else {
        fn_8040CE60(&input);
    }
    Vec3f front;
    fn_800091D0(&front, &(const Vec3f &)fn_800796DC(m_player.get()));
    bool moving = fn_8040CF2C(&input) > 0.4f * 0.4f;
    fn_800091D0(&m_input, &(const Vec3f &)fn_8040CEE8(&input));
    switch (m_mode) {
    case 0:
        if (moving && fn_8040CF90(&input, &front) > 0.0f) {
            m_mode = 1;
        }
        m_player.get()->m_motion->vf_0x10(
            !m_mirrored ? MOTION_WALL_HOLD : MOTION_WALL_HOLD_MIRRORED, 5.0f);
        if (!CheckWall()) {
            Fall();
        }
        break;
    case 1:
        fn_80076CA8(m_player.get()->m_stateBits, 0);
        fn_80076CA8(m_player.get()->m_stateBits, 0xD);
        fn_80076CC8(m_player.get()->m_stateBits, 0xE);
        fn_80076CA8(m_player.get()->m_stateBits, 0x39);
        fn_800091D0(&FieldsOf(m_player.get())->m_up, &m_normal);
        PlayerCollisionEventCallback(m_player.get()) =
            CollisionEventCallback(boost::bind(&WallHang::OnCollision, this, _1));
        m_mode = 2;
        m_holdTime = 1.0f;
        m_player.get()->m_motion->vf_0x10(
            !m_mirrored ? MOTION_WALL_CLIMB : MOTION_WALL_CLIMB_MIRRORED, 10.0f);
        break;
    case 2:
        AddVector(m_player->field_0x24, fn_800091A4(&front, 500.0f));
        fn_80079720(m_player.get());
        fn_800091D0(&FieldsOf(m_player.get())->m_up, &m_normal);
        if (!moving || fn_8040CF90(&input, &front) <= 0.0f) {
            Fall();
        }
        if (m_contacts == 0) {
            Fall();
        }
        m_contacts = 0;
        break;
    }
    UnkObject_8007FE50 *object = FieldsOf(m_player.get())->field_0x278;
    if (object->field_0x24 && object->field_0x20 < 10.0f) {
        Fall();
    }
    fn_80076CA8(m_player.get()->m_stateBits, 2);
    fn_8009728C(&m_player, 0);
}

void WallHang::vf_0x20() {}

/* Lets go of the wall (name inferred). */
void WallHang::Fall() {
    fn_800091D0(&m_player.px->field_0x18, &(const Vec3f &)fn_8040CDA8(&m_normal, 50.0f));
    fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(2));
    return;
}

/* Whether the wall is still behind the hands (as Player::Behavior::EdgeHang's CheckWall). */
bool WallHang::CheckWall() {
    Line line;
    fn_800091D0(&line.m_start, &m_player.get()->m_position);
    fn_800091D0(&line.m_end,
                &(const Vec3f &)Subtract(&m_player.px->m_position,
                                         &(const Vec3f &)fn_8040CDA8(
                                             &m_normal, s_wallReach + m_player.get()->field_0x274)));
    return !(fn_800979A0(&m_player, &line, 0, 0x1A93, 0x40000, 2) < 0.0f);
}

void WallHang::OnCollision(CollisionEventArgs &args) {
    UnkColliContactNode_800587A0 *contact = args.m_contact;
    u32 attributes = contact->GetAttributes();
    if (attributes & lbl_805E4514) {
        fn_800091D0(&m_normal, fn_8001E9C4(contact->m_object));
        TurnToWall(m_normal);
        m_contacts++;
    }
}

/* Turns the player to face normal's wall, upright along it (name inferred). */
void WallHang::TurnToWall(const Vec3f &normal) {
    Vec3f side;
    fn_800091EC(&side, &(const Vec3f &)fn_8040CFB8(&FieldsOf(m_player.get())->m_jumpUp, &normal));
    fn_8040CE74(&side);
    Vec3f up;
    fn_800091EC(&up, &(const Vec3f &)fn_8040CFB8(&side, &normal));
    fn_8040CE74(&up);
    Vec3f front;
    fn_800091EC(&front, &(const Vec3f &)fn_800796DC(m_player.get()));
    if (fn_8040CF90(&side, &front) <= 0.0f) {
        fn_800091D0(&side, &(const Vec3f &)fn_80016284(&side));
    }
    if (fn_8040CF90(&up, &FieldsOf(m_player.get())->m_jumpUp) <= 0.0f) {
        fn_800091D0(&up, &(const Vec3f &)fn_80016284(&up));
    }
    m_mirrored = fn_8040CF90(&(const Vec3f &)fn_8040CFB8(&side, &up), &normal) < 0.0f ? 0 : 1;
    m_player.px->m_rotation.y = fn_8040F364(&side, &up).y;
    fn_800091D0(&FieldsOf(m_player.get())->m_groundNormal, &up);
}

/* The direction a jump off the wall takes: along the wall's normal, the input's direction, or the
 * normal turned 45 degrees towards the input's side (name inferred). */
WallJumpDirection WallHang::JumpDirection() {
    if (!fn_8006CD78(m_player.get()->m_stateBits, 0x1B)) {
        return m_normal;
    }
    if (fn_8040CF90(&m_normal, &m_input) > 0.70710677f) {
        return m_input;
    }
    Vec3f front;
    fn_800091EC(&front, &(const Vec3f &)fn_800796DC(m_player.get()));
    float angle;
    if (fn_8040CF90(&(const Vec3f &)fn_8040CFB8(&front, &FieldsOf(m_player.get())->m_groundNormal),
                    &m_normal) >= 0.0f) {
        if (fn_8040CF90(&front, &m_input) >= 0.0f) {
            angle = DegToRad(45.0f);
        } else {
            angle = DegToRad(-45.0f);
        }
    } else {
        if (fn_8040CF90(&front, &m_input) >= 0.0f) {
            angle = DegToRad(-45.0f);
        } else {
            angle = DegToRad(45.0f);
        }
    }
    Mtx34f rotation;
    AssignMtx(rotation,
              fn_80049778(&FieldsOf(m_player.operator->())->m_groundNormal, angle));
    return reinterpret_cast<TransformDirection>(fn_8040B67C)(&rotation, &m_normal);
}

Behavior *WallHang::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new WallHang(player);
}

void WallHang::Release() {}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds WallHang by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804B1234 = {
    Player::Behavior::WallHang::Create, Player::Behavior::WallHang::Release, 0};
