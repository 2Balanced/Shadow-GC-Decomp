/* Player::Behavior::JumpDash (class name from RTTI): behavior 6 of Shadow, the dash in the air.
 * Entering it throws the player forward, keeps pushing it ahead for a while and lets it steer;
 * meeting a wall it can hang on hangs on it (behavior 0xD, Player::Behavior::InitializeArgsWallHang)
 * and meeting anything else falls (behavior 2), as it does when the dash is over; it lands
 * (behavior 3) on the ground. Started from state 0x25 it plays another motion and does not listen
 * to collisions. File name inferred from the class (as Game/PlayerBehaviorSuper.cpp); all other
 * names are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
/* Declares Player::Behavior::Behavior's slots with their arguments (see Game/PlayerBehavior.h). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
#include <boost/bind.hpp>
#include <boost/function.hpp>

/* Defines InitializeArgsWallHang's inline functions, whose out-of-line copies this unit emits. */
#define PLAYER_BEHAVIOR_ARGS_INLINE

#include "Game/CharCommand.h"
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

/* The player's members read here (struct name unknown; Player::PlayerBase leaves them
 * unmapped). */
struct UnkPlayerFields_800882A4 {
    /* 0x000 */ u8 field_0x0[0x64];
    /* 0x064 */ s32 m_kind; /* the character (as in Game/PlayerBehaviorFall.cpp) */
    /* 0x068 */ u8 field_0x68[0xB8 - 0x68];
    /* 0x0B8 */ Vec3f field_0xB8; /* the input's direction (as in Game/PlayerBehaviorEdgeHang.cpp) */
};

extern "C" u32 fn_80026588(UnkColliContactNode_800587A0 *contact);

/* A contact of a collision (see Game/VehicleColliMove.cpp): its object at 0x14. */
struct UnkColliContactNode_800587A0 {
    /* The contact's attributes, the low 28 bits of its flags (name inferred). */
    u32 GetAttributes() {
        return fn_80026588(this) & 0x0FFFFFFF;
    }

    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ void *m_object;
};

/* Attribute masks of the walls the player may hang on and of those it may not (meanings
 * inferred). */
extern u32 lbl_805E4514;
extern u32 lbl_805E4518;

extern "C" {
/* The normal of a contact's object (see Game/BirdBase.cpp). */
const Vec3f *fn_8001E9C4(void *object);
/* The player's flag word and state bits (see Game/PlayerShadowSuper.cpp). */
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
bool fn_8006CD78(u32 *bits, int bit);
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* Called with the player's object at 0x234: stopping and starting its effect (behaviour inferred;
 * see Game/PlayerBehaviorFall.cpp). */
void fn_8007792C(void *object);
void fn_80077950(void *object, int arg);
/* The player's front axis (see Game/PlayerBehaviorGrind.cpp). */
Vec3f fn_800796DC(Player::PlayerBase *player);
/* Player updates run before and after the behavior's own (behaviour unknown; as in FirstMeet). */
void fn_80079720(Player::PlayerBase *player);
void fn_8009728C(boost::shared_ptr<Player::PlayerBase> *player, int arg1);
/* Turning the player by its turn rate (behaviour inferred; as in Super). */
void fn_80093AF8(boost::shared_ptr<Player::PlayerBase> *player, float rate);
/* The trail of the player's effects at 0x23C (see Game/PlayerBehaviorHomingAttack.cpp). */
void fn_800A2D30(void *effects);
void fn_800A2D54(void *effects, float length);
/* Switches a player's behavior; false when the new behavior refuses (behaviour inferred). */
bool fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);

Vec3f fn_800091A4(const Vec3f *vector, float scale); /* vector * scale */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* vector += value */
}

/* The player's members above (a macro, as in Game/PlayerBehaviorSuper.cpp). */
#define FieldsOf(player) reinterpret_cast<UnkPlayerFields_800882A4 *>(player)

/* vector += value (an inline wrapper, which orders the call's arguments as the game does). */
inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

namespace Player {
namespace Behavior {

class JumpDash : public Behavior {
  public:
    JumpDash(const boost::shared_ptr<PlayerBase> &player);
    virtual ~JumpDash();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void Update(float dt);

    /* The factory record's functions (names inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);
    static void Release();

    void OnCollision(CollisionEventArgs &args);

    static const char *s_name;

    /* 0x1C */ s32 m_step;
    /* 0x20 */ float m_time;     /* the dash's time left */
    /* 0x24 */ float m_pushTime; /* the push's time left */
};
SIZE_ASSERT(JumpDash, 0x28);

const char *JumpDash::s_name = "JumpDash";

JumpDash::JumpDash(const boost::shared_ptr<PlayerBase> &player) : Behavior(player, s_name, 6) {}

JumpDash::~JumpDash() {}

/* NonMatching (96%): retail keeps the table's address in r3 and the player in r4 (as
 * Player::Behavior::Ground::SetSpeedRange). */
bool JumpDash::vf_0x0C(int prevId, const InitializeArgs &args) {
    if (fn_80014E34(&m_player.get()->m_flags, 2)) {
        return lbl_804F1834[FieldsOf(m_player.get())->m_kind].m_canJumpDash;
    }
    return false;
}

void JumpDash::Enter(int prevId, const InitializeArgs &args) {
    fn_80076CA8(m_player.get()->m_stateBits, 5);
    fn_80076CC8(m_player.get()->m_stateBits, 0x32);
    if (fn_8006CD78(m_player.get()->m_stateBits, 0x25)) {
        fn_80014E1C(&m_player.get()->m_flags, 7);
        m_player.get()->m_motion->vf_0x10(0x2C, 5.0f);
        if (fn_8006CD78(m_player.get()->m_stateBits, 0x1B)) {
            fn_80093AF8(&m_player, 1.0f);
        }
    } else {
        fn_80076CA8(m_player.get()->m_stateBits, 0);
        m_player.get()->m_motion->vf_0x10(6, 5.0f);
        PlayerCollisionEventCallback(m_player.get()) =
            CollisionEventCallback(boost::bind(&JumpDash::OnCollision, this, _1));
    }
    m_step = 0;
    m_player.get()->m_sound->StopVoice(4);
    fn_800A2D54(reinterpret_cast<void *>(m_player.get()->field_0x23C), 5.0f);
    fn_80076CA8(m_player.get()->m_stateBits, 0x35);
    fn_80077950(reinterpret_cast<void *>(m_player.get()->field_0x234), 5);
}

void JumpDash::Leave(int nextId) {
    fn_80076CC8(m_player.get()->m_stateBits, 0);
    fn_80076CC8(m_player.get()->m_stateBits, 5);
    fn_8006127C(&m_player.get()->m_flags, 7);
    PlayerCollisionEventCallback(m_player.get()).Clear();
    fn_800A2D30(reinterpret_cast<void *>(m_player.get()->field_0x23C));
    fn_80076CC8(m_player.get()->m_stateBits, 0x35);
    fn_8007792C(reinterpret_cast<void *>(m_player.get()->field_0x234));
}

void JumpDash::Update(float dt) {
    Vec3f front;
    fn_800091EC(&front, &(const Vec3f &)fn_800796DC(m_player.get()));
    switch (m_step) {
    case 0:
        m_time = 0.4f;
        m_pushTime = 2.0f;
        AddVector(m_player->field_0x18, fn_800091A4(&front, 180.0f));
        m_step++;
    case 1:
        m_pushTime -= dt;
        if (m_pushTime < 0.0f) {
            m_step++;
        } else {
            AddVector(m_player->field_0x24, fn_800091A4(&front, 430.0f));
        }
        break;
    case 2:
        break;
    }
    AddVector(m_player->field_0x24,
              fn_800091A4(&FieldsOf(m_player.operator->())->field_0xB8, 220.0f));
    fn_80079720(m_player.get());
    fn_80093AF8(&m_player, 6.5f * dt);
    fn_8009728C(&m_player, 0);
    m_time -= dt;
    if (m_time <= 0.0f) {
        fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(2));
    }
    if (fn_8006CD78(m_player.get()->m_stateBits, 2)) {
        fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(3));
    }
}

void JumpDash::OnCollision(CollisionEventArgs &args) {
    UnkColliContactNode_800587A0 *contact = args.m_contact;
    u32 attributes = contact->GetAttributes();
    if (!(attributes & lbl_805E4518) && (attributes & lbl_805E4514)) {
        Vec3f normal;
        fn_800091EC(&normal, fn_8001E9C4(contact->m_object));
        if (fn_800A7F14(m_player.get()->m_behaviors,
                        &(const InitializeArgs &)InitializeArgsWallHang(normal, args.m_chara))) {
            return;
        }
    }
    fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(2));
}

Behavior *JumpDash::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new JumpDash(player);
}

void JumpDash::Release() {}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds JumpDash by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804B07A4 = {
    Player::Behavior::JumpDash::Create, Player::Behavior::JumpDash::Release, 0};
