/* Player::Behavior::WallJump (class name from RTTI): behavior 0x10 of Shadow, the jump off a wall
 * (Player::Behavior::InitializeArgsWallJump, from Player::Behavior::WallHang). When a wall lies
 * ahead along the jump the player flies at it for up to 4 seconds (with the jump effect), hanging
 * on when it meets a wall it may hang on (behavior 0xD); otherwise it is a short hop. It falls
 * (behavior 2) when the time runs out or it meets anything else, and lands (behavior 3) on the
 * ground. File name inferred from the class (as Game/PlayerBehaviorSuper.cpp); all other names are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). Where the game reloads the player pointer in a call's arguments after
 * another argument's call, the code reads m_player.px (as in Game/PlayerBehaviorGrind.cpp). */
/* Declares Player::Behavior::Behavior's slots with their arguments (see Game/PlayerBehavior.h). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharCommand.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerMotionEvent.h"
#include "Game/PlayerSoundBase.h"
#include "Game/PlayerWeaponBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

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

/* A line segment between two points, built by fn_80020A20 (as in Game/AttackObject.cpp: plain
 * bytes, so that declaring one constructs nothing); the line cast reads it as a
 * Sonicteam::System::Geometry::Line. */
struct UnkSegment_80020A20 {
    /* 0x00 */ u8 field_0x0[0x18];
};

/* Attribute masks of the walls the player may hang on and of those it may not (defined in
 * Game/PlayerBehaviorWallHang.cpp). */
extern u32 lbl_805E4514;
extern u32 lbl_805E4518;

extern "C" {
/* The normal of a contact's object (see Game/BirdBase.cpp), and a hit object's normal (see
 * Game/DiablonPartsBeam.cpp). */
const Vec3f *fn_8001E9C4(void *object);
const Vec3f *fn_800266D4(void *object);
/* Builds segment from start to end (copying the points; behaviour from its body). */
UnkSegment_80020A20 *fn_80020A20(UnkSegment_80020A20 *segment, const Vec3f *start,
                                 const Vec3f *end);
/* The player's state bits (see Game/PlayerShadowSuper.cpp). */
bool fn_8006CD78(u32 *bits, int bit);
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* Player updates run before and after the behavior's own (behaviour unknown; as in FirstMeet). */
void fn_80079720(Player::PlayerBase *player);
void fn_8009728C(boost::shared_ptr<Player::PlayerBase> *player, int arg1);
/* Turning the player by its turn rate (behaviour inferred; as in Super). */
void fn_80093AF8(boost::shared_ptr<Player::PlayerBase> *player, float rate);
/* Casts segment through the world, giving the attributes of what it hits (behaviour inferred; a
 * sibling of fn_800979A0, see Game/PlayerBehaviorEdgeHang.cpp). */
float fn_800979E8(boost::shared_ptr<Player::PlayerBase> *player, int arg1, int arg2, int arg3,
                  u32 *attributes, int arg5, const UnkSegment_80020A20 *segment, int arg7,
                  int arg8, int arg9, int arg10);
/* Stopping and starting the jump effect of the player's effects at 0x23C (as in
 * Game/PlayerBehaviorSliding.cpp). */
void fn_800A2D30(void *effects);
void fn_800A2D54(void *effects, float length);
/* Switches a player's behavior (behaviour inferred). */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);

Vec3f fn_800091A4(const Vec3f *vector, float scale); /* vector * scale */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale); /* vector * scale */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);   /* a + b */
void fn_8040CE38(Vec3f *vector, float scale);        /* vector *= scale */
Vec3f fn_8040CEE8(const Vec3f *vector);              /* normalized */
float fn_8040CF90(const Vec3f *a, const Vec3f *b);   /* dot product */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* vector += value */
}

/* vector += value (an inline wrapper, which orders the call's arguments as the game does). */
inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

namespace Player {
namespace Behavior {

class WallJump : public Behavior {
  public:
    WallJump(const boost::shared_ptr<PlayerBase> &player);
    virtual ~WallJump();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x18(CharCommand *command);
    virtual void Update(float dt);

    /* The factory record's functions (names inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);
    static void Release();

    void OnCollision(CollisionEventArgs &args);
    bool HitsWall(const Vec3f *from, const Vec3f *offset);
    bool WallAhead(const Vec3f &normal, const Vec3f &direction);

    static const char *s_name;

    /* 0x1C */ Vec3f m_direction;    /* the jump's direction */
    /* 0x28 */ Vec3f m_lastVelocity; /* the player's velocity at the end of the last frame */
    /* 0x34 */ s32 field_0x34;       /* cleared on entry (meaning unknown) */
    /* 0x38 */ float m_time;         /* left of the jump */
};
SIZE_ASSERT(WallJump, 0x3C);

const char *WallJump::s_name = "WallJump";

WallJump::WallJump(const boost::shared_ptr<PlayerBase> &player) : Behavior(player, s_name, 0x10) {}

WallJump::~WallJump() {}

void WallJump::vf_0x18(CharCommand *command) {}

bool WallJump::vf_0x0C(int prevId, const InitializeArgs &args) {
    return true;
}

void WallJump::Enter(int prevId, const InitializeArgs &args) {
    const InitializeArgsWallJump *jump = dynamic_cast<const InitializeArgsWallJump *>(&args);
    PlayerCollisionEventCallback(m_player.get()) =
        CollisionEventCallback(boost::bind(&WallJump::OnCollision, this, _1));
    m_player.get()->m_motion->vf_0x10(0x10, 10.0f);
    fn_800091D0(&m_direction, reinterpret_cast<const Vec3f *>(&jump->m_direction));
    field_0x34 = 0;
    if (WallAhead(reinterpret_cast<const Vec3f &>(jump->m_normal),
                  reinterpret_cast<const Vec3f &>(jump->m_direction))) {
        m_time = 4.0f;
        fn_800091D0(&m_player.px->field_0x18, &(const Vec3f &)fn_8040CDA8(&m_direction, 200.0f));
        fn_800A2D54(reinterpret_cast<void *>(m_player.get()->field_0x23C), 5.0f);
    } else {
        fn_80076CA8(m_player.get()->m_stateBits, 0);
        m_time = 0.75f;
        fn_800091D0(&m_player.px->field_0x18, &(const Vec3f &)fn_8040CDA8(&m_direction, 60.0f));
    }
    m_player.get()->m_sound->StopVoice(3);
}

void WallJump::Leave(int nextId) {
    fn_80076CC8(m_player.get()->m_stateBits, 0);
    fn_80076CC8(m_player.get()->m_stateBits, 5);
    PlayerCollisionEventCallback(m_player.get()).Clear();
    fn_800A2D30(reinterpret_cast<void *>(m_player.get()->field_0x23C));
}

void WallJump::Update(float dt) {
    m_time -= dt;
    if (m_time < 0.0f) {
        fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(2));
        return;
    }
    if (fn_8006CD78(m_player.get()->m_stateBits, 0)) {
        AddVector(m_player->field_0x24, fn_800091A4(&m_direction, 400.0f));
    } else {
        AddVector(m_player->field_0x24, fn_800091A4(&m_direction, 450.0f));
        if (!HitsWall(&m_player.px->m_position,
                      &(const Vec3f &)fn_8040CDA8(
                          &(const Vec3f &)fn_8040CEE8(&m_player.get()->field_0x18), 113.6f))) {
            m_time = 0.0f;
        }
    }
    fn_80079720(m_player.get());
    fn_80076CC8(m_player.get()->m_stateBits, 0x1B);
    fn_80093AF8(&m_player, 20.0f * dt);
    fn_800091D0(&m_lastVelocity, &m_player.get()->field_0x18);
    fn_8009728C(&m_player, 0);
    if (fn_8006CD78(m_player.get()->m_stateBits, 2)) {
        fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(3));
    }
}

/* Whether a wall lies along direction, within 80 along normal (name inferred). */
bool WallJump::WallAhead(const Vec3f &normal, const Vec3f &direction) {
    Vec3f ray;
    fn_800091EC(&ray, &(const Vec3f &)fn_8040CEE8(&direction));
    float dot = fn_8040CF90(&ray, &normal);
    fn_8040CE38(&ray, 80.0f * (1.0f / dot));
    return HitsWall(&m_player.get()->m_position, &ray);
}

/* Whether the segment from from to from + offset meets a wall the player may hang on (name
 * inferred). */
bool WallJump::HitsWall(const Vec3f *from, const Vec3f *offset) {
    u32 attributes;
    UnkSegment_80020A20 segment;
    fn_80020A20(&segment, from, &(const Vec3f &)fn_8040CDD0(from, offset));
    fn_800979E8(&m_player, 0, 0, 0, &attributes, 0, &segment, 0, 0x1A93, 0x40000, 2);
    return attributes & lbl_805E4514;
}

void WallJump::OnCollision(CollisionEventArgs &args) {
    UnkColliContactNode_800587A0 *contact = args.m_contact;
    void *object = contact->m_object;
    /* The attributes as GetAttributes reads them, written out (as the game colours the
     * registers). */
    u32 attributes = fn_80026588(contact) & 0x0FFFFFFF;
    if (fn_8040CF90(fn_800266D4(object), &m_lastVelocity) > 0.0f) {
        return;
    }
    if (!fn_8006CD78(m_player.get()->m_stateBits, 0) && !(attributes & lbl_805E4518) &&
        (attributes & lbl_805E4514)) {
        Vec3f normal;
        fn_800091EC(&normal, fn_8001E9C4(object));
        fn_800A7F14(m_player.get()->m_behaviors,
                    &(const InitializeArgs &)InitializeArgsWallHang(normal, args.m_chara));
    } else {
        fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(2));
    }
}

Behavior *WallJump::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new WallJump(player);
}

void WallJump::Release() {}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds WallJump by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804B13DC = {
    Player::Behavior::WallJump::Create, Player::Behavior::WallJump::Release, 0};
