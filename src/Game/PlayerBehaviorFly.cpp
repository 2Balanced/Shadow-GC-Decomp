/* Player::Behavior::Fly (class name from RTTI): behavior 0 of the flying NPCs
 * (Game/FlyTypeBehaviorFactory.cpp). Each frame it applies the player's acceleration, turns the
 * player towards its direction of flight (or, while it hovers, towards the player the NPC
 * follows) and sets the motion's speed from the player's speed. While it runs it holds the
 * player's state bit 0x38 (meaning unknown). File name inferred from the class; function and
 * member names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
#include "Game/MathTypes.h"
#include "Game/Npc.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerWeaponBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" {
/* Sets and clears a bit of the player's state bits (see Game/Player.h). */
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* Player updates run before the behavior's own (behaviour unknown; see
 * Game/PlayerBehaviorFirstMeet.cpp). */
void fn_80079720(Player::PlayerBase *player);
void fn_8009728C(boost::shared_ptr<Player::PlayerBase> *player, int arg1);
/* Sets the motion's speed (behaviour inferred). */
void fn_800AA7B8(Player::PlayerMotion *motion, float value);
/* The player the NPC follows (see Game/NpcControlRestartFollow.cpp). */
boost::weak_ptr<Player::PlayerBase> fn_8007363C(Player::Npc::Npc *npc);

/* Vector routines (see Game/MathTypes.h). */
Vec3f fn_800091A4(const Vec3f *vector, float scale); /* vector * scale */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* vector += value */
Vec3f fn_8040CEE8(const Vec3f *v);                    /* normalized */
void fn_8040F990(Vec3f *along, Vec3f *across, const Vec3f *vector, const Vec3f *normal);
}

/* Euler angles of a direction, and a turn between two sets of them. The game declares them inline
 * with their bodies elsewhere: called out of line, they keep the player's operator-> in their
 * arguments out of line too. */
extern "C" inline Vec3f fn_8040F364(const Vec3f *forward, const Vec3f *up);
extern "C" inline Vec3f fn_8040F16C(const Vec3f *a, const Vec3f *b, int order, float t);

/* fn_8040CD74 with its result returned by value (as in Game/BulletHoming.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *lhs, const Vec3f *rhs);

namespace Player {
namespace Behavior {

/* The player's acceleration (at 0xB8) and up direction (at 0xE8), inside PlayerBase's unnamed
 * bytes (struct name unknown; names inferred). */
struct UnkPlayerFly_801F2CB8 {
    /* 0x00 */ u8 field_0x0[0xB8];
    /* 0xB8 */ Vec3f m_acceleration;
    /* 0xC4 */ u8 field_0xC4[0xE8 - 0xC4];
    /* 0xE8 */ Vec3f m_up;
};

/* vector += value (an inline wrapper, name inferred: the player's operator-> in its arguments stays
 * out of line, as in the game). */
inline void Add(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

class Fly : public Behavior {
  public:
    Fly(const boost::shared_ptr<PlayerBase> &player);
    virtual ~Fly();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x18(CharCommand *command);
    virtual void Update(float dt);

    /* The factory record's functions (names inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);
    static void Release();

    static const char *s_name;

    /* 0x1C */ boost::shared_ptr<Npc::Npc> m_npc;
};
SIZE_ASSERT(Fly, 0x24);

const char *Fly::s_name = "Fly";

Fly::Fly(const boost::shared_ptr<PlayerBase> &player) : Behavior(player, s_name, 0) {
    m_npc = boost::dynamic_pointer_cast<Npc::Npc>(m_player);
}

Fly::~Fly() {}

bool Fly::vf_0x0C(int prevId, const InitializeArgs &args) {
    return true;
}

void Fly::Enter(int prevId, const InitializeArgs &args) {
    fn_80076CA8(m_player->m_stateBits, 0x38);
    m_player->m_motion->vf_0x10(0, 5.0f);
}

void Fly::Leave(int nextId) {
    fn_80076CC8(m_player->m_stateBits, 0x38);
}

void Fly::vf_0x18(CharCommand *command) {}

void Fly::Update(float dt) {
    Add(m_player->field_0x24,
        fn_800091A4(&reinterpret_cast<UnkPlayerFly_801F2CB8 *>(m_player.operator->())->m_acceleration, 1200.0f));
    fn_80076CA8(m_player->m_stateBits, 2);
    fn_80079720(m_player.get());
    fn_80076CC8(m_player->m_stateBits, 2);
    fn_8009728C(&m_player, 0);
    Vec3f target = m_player->m_rotation;
    if (fn_8040CF2C(&m_player->field_0x18) >= 40000.0f) {
        target = fn_8040F364(&m_player->field_0x18, &reinterpret_cast<UnkPlayerFly_801F2CB8 *>(m_player.operator->())->m_up);
    } else if (m_npc) {
        boost::shared_ptr<PlayerBase> leader = fn_8007363C(m_npc.get()).lock();
        if (leader) {
            Vec3f direction;
            fn_800091EC(&direction, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&leader.get()->m_position,
                                                                                    &m_npc.get()->m_position));
            fn_8040F990(0, &direction, &direction, &reinterpret_cast<UnkPlayerFly_801F2CB8 *>(leader.get())->m_up);
            if (fn_8040CF2C(&direction) > 25.0f) {
                Vec3f up;
                fn_800091EC(&up, &reinterpret_cast<UnkPlayerFly_801F2CB8 *>(leader.get())->m_up);
                target = fn_8040F364(&(const Vec3f &)fn_8040CEE8(&direction), &up);
            }
        }
    }
    m_player->m_rotation = fn_8040F16C(&m_player->m_rotation, &target, 2, 4.0f * dt);
    float speed = fn_8040CF50(&m_player->field_0x18);
    float rate;
    if (speed < 20.0f) {
        rate = speed / 20.0f;
    } else if (speed < 40.0f) {
        rate = (speed - 20.0f) / 20.0f;
        rate += 1.0f;
    } else if (speed < 100.0f) {
        rate = (speed - 40.0f) / 60.0f;
        rate += 2.0f;
    } else {
        rate = 3.0f;
    }
    fn_800AA7B8(m_player->m_motion, rate);}

Behavior *Fly::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new Fly(player);
}

void Fly::Release() {}

} // namespace Behavior
} // namespace Player

/* The factory record (Game/FlyTypeBehaviorFactory.cpp). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804D30A8 = {Player::Behavior::Fly::Create,
                                                                       Player::Behavior::Fly::Release, 0};
