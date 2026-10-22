/* Player::PlayerTargetBase (class name from RTTI): the aim of a player character. It keeps the
 * player's own Target (moved to the player, or to the seat of the vehicle it rides, each frame)
 * and a turn rate eased towards the player's speed. The free functions after the class search
 * the target manager for what the player aims at. File name inferred from the class; other names
 * are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
/* Enables shared_ptr::reset(Y *) (see boost/shared_ptr.hpp). */
#define BOOST_SP_RESET_POINTER
#include "Game/MathConstants.h"
#include "Game/Player.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/TVehicleBase.h"
#include "Game/TargetSearchList.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* Target (class name from RTTI): a point other objects can aim at (members as in
 * BAGunShip.cpp). */
class Target {
  public:
    void TurnOn() {
        m_flags |= 1;
    }
    void TurnOff() {
        m_flags &= ~1;
    }
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);

    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ u32 m_flags;
};

/* MSL's std::cos(float), which the game calls out of line (0x8000F3B0; declared here rather than
 * through <cmath>, as in Coaster.cpp). */
namespace std {
float cos(float x);
}

extern "C" {
/* A bit array: test bit. */
bool fn_8006CD78(u32 *bits, int bit);
/* A new target owned by a character (as in ArkCannon.cpp). */
boost::shared_ptr<Target> fn_801E7128(const boost::weak_ptr<CharInfo> &owner, int flags);
/* Narrowing a target search list: drops the targets of character (behaviour inferred). */
void fn_801E81B0(TargetSearchList *list, CharInfo *character);
/* A direction turned into a character's frame, and the character's up and forward directions
 * turned the same way (behaviour inferred). */
Vec3f fn_80079628(Player::PlayerBase *player, const Vec3f *direction);
Vec3f fn_80079698(Player::PlayerBase *player);
Vec3f fn_800796DC(Player::PlayerBase *player);
/* direction normalized (behaviour inferred). */
Vec3f fn_80016284(const Vec3f &direction);
Vec3f fn_8040CEE8(const Vec3f *vector);
/* The smaller of two values, and value clamped to [low, high] (behaviour inferred). */
const float &fn_800704AC(const float &a, const float &b);
const float &fn_80070460(const float &value, const float &low, const float &high);
/* Which side of the player velocity points to (1 or 2; behaviour inferred). */
int fn_80093C60(const boost::shared_ptr<Player::PlayerBase> *player, const Vec3f *velocity);
}

inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* A length (name inferred). As a call it is not a constant expression, so the value below is
 * stored by the static initializer, as in retail (as in WeaponVacuumBase.cpp). */
inline float Span(float length) {
    return length;
}

/* How far the aim looks for targets, the turn at full speed and the turn per second (names
 * inferred). */
static float s_searchRange = Span(150.0f);
static float s_maxTurn = DegToRad(40.0f);
static float s_turnSpeed = DegToRad(200.0f);
/* The speed at which the turn is full (name inferred). */
static float s_fullTurnSpeed = 1.0f;

namespace Player {

SIZE_ASSERT(PlayerTargetBase, 0x18);

/* The frame time a character's word at 0x60 points at (as in EggLastMechArea.cpp). */
inline float FrameTime(PlayerBase *player) {
    return **reinterpret_cast<float *const *>(player->field_0x60);
}

/* The player's velocity (at 0xB8; name inferred). */
inline Vec3f &Velocity(PlayerBase *player) {
    return *reinterpret_cast<Vec3f *>(reinterpret_cast<u8 *>(player) + 0xB8);
}

PlayerTargetBase::PlayerTargetBase(const boost::shared_ptr<PlayerBase> &player)
    : m_player(player) {
    m_target = fn_801E7128(boost::weak_ptr<CharInfo>(m_player), 0x80000);
    m_turn = 0.0f;
}

PlayerTargetBase::~PlayerTargetBase() {}

void PlayerTargetBase::vf_0x0C() {
    if (m_target) {
        if (!fn_8006CD78(m_player->m_stateBits, 0x45)) {
            Vec3f position;
            if (fn_8006CD78(m_player->m_stateBits, 0x10)) {
                boost::shared_ptr<Vehicle::TVehicleBase> vehicle = m_player->m_vehicle.lock();
                vehicle->GetSeat(&position, 0);
            } else {
                fn_800091D0(&position, &m_player->m_position);
            }
            m_target->fn_800CCE38(&position);
        }
        if (fn_8006CD78(m_player->m_stateBits, 0x27) ||
            fn_8006CD78(m_player->m_stateBits, 0x44)) {
            m_target->TurnOff();
        } else {
            m_target->TurnOn();
        }
    }
    float turn = 0.0f;
    if (fn_8006CD78(m_player->m_stateBits, 0x42) && fn_8006CD78(m_player->m_stateBits, 0x1B)) {
        turn = s_maxTurn * fn_800704AC(fn_8040CF50(&Velocity(m_player.get())), s_fullTurnSpeed);
        switch (fn_80093C60(&m_player, &Velocity(m_player.get()))) {
        case 1:
            turn = -turn;
            break;
        case 2:
            break;
        }
    }
    float step = s_turnSpeed * FrameTime(m_player.get());
    m_turn = fn_80070460(turn, m_turn - step, m_turn + step);
}

int PlayerTargetBase::vf_0x10() {
    return 0;
}

bool PlayerTargetBase::HasTarget() {
    return false;
}

void PlayerTargetBase::vf_0x18() {}

AimPosition PlayerTargetBase::vf_0x1C() {
    return lbl_805E07A0;
}

AimPosition PlayerTargetBase::GetTargetPosition() {
    return lbl_805E07A0;
}

boost::weak_ptr<Target> PlayerTargetBase::GetTarget() {
    return boost::weak_ptr<Target>();
}

boost::weak_ptr<Target> PlayerTargetBase::vf_0x28() {
    return boost::weak_ptr<Target>();
}

Vec3f PlayerTargetBase::ToAimSpace(const Vec3f &vector) {
    return fn_80079628(m_player.get(), &vector);
}

float PlayerTargetBase::GetTurn() {
    return m_turn;
}

} // namespace Player

/* The target the player aims at along direction: the nearest in a 45-degree cone about it, else
 * the nearest in range; with no direction, the nearest in front of the player (behaviour
 * inferred; called by 0x800A02FC). */
extern "C" boost::weak_ptr<Target> fn_80206814(Player::PlayerTargetBase *aim,
                                                const Vec3f *direction) {
    boost::shared_ptr<TargetSearchList> list;
    list.reset(fn_801E7930(fn_80060058()));
    fn_801E80C0(list.get(), 0x400);
    fn_801E7E48(list.get(), aim->m_target);
    if (!fn_8006CD78(aim->m_player->m_stateBits, 0x25)) {
        fn_801E7FB4(list.get(), &aim->m_player->m_position, 0.0f,
                    &(const Vec3f &)fn_80016284(fn_80079698(aim->m_player.get())));
    }
    if (fn_8040CF2C(direction) > 0.09f) {
        fn_801E7FB4(list.get(), &aim->m_player->m_position, 0.0f, &(const Vec3f &)fn_8040CEE8(direction));
        fn_801E8218(list.get(), &aim->m_player->m_position, s_searchRange);
        fn_801E81B0(list.get(), aim->m_player.get());
        boost::weak_ptr<Target> nearest = fn_801E848C(list.get(), &aim->m_player->m_position);
        float cosAngle = std::cos(DegToRad(45.0f));
        fn_801E7FB4(list.get(), &aim->m_player->m_position, cosAngle,
                    &(const Vec3f &)fn_8040CEE8(direction));
        boost::weak_ptr<Target> target = fn_801E848C(list.get(), &aim->m_player->m_position);
        if (target.expired()) {
            target = nearest;
        }
        return target;
    } else {
        float cosAngle = std::cos(DegToRad(90.0f));
        fn_801E7FB4(list.get(), &aim->m_player->m_position, cosAngle,
                    &(const Vec3f &)fn_800796DC(aim->m_player.get()));
        fn_801E8218(list.get(), &aim->m_player->m_position, s_searchRange);
        fn_801E81B0(list.get(), aim->m_player.get());
        boost::weak_ptr<Target> target = fn_801E848C(list.get(), &aim->m_player->m_position);
        return target;
    }
}

/* The target nearest to position within a cone about direction, among those with flags, other
 * than the player's own (behaviour inferred; called by 0x80089ADC). */
extern "C" boost::weak_ptr<Target> fn_802065D8(Player::PlayerTargetBase *aim,
                                                const Vec3f *position, const Vec3f *direction,
                                                int flags, float cosAngle) {
    boost::shared_ptr<TargetSearchList> list;
    list.reset(fn_801E7930(fn_80060058()));
    fn_801E80C0(list.get(), flags);
    fn_801E7E48(list.get(), aim->m_target);
    fn_801E7FB4(list.get(), position, cosAngle, direction);
    fn_801E81B0(list.get(), aim->m_player.get());
    boost::weak_ptr<Target> target = fn_801E848C(list.get(), position);
    return target;
}

/* The player's own target (as ArkCannon.cpp calls it). */
extern "C" boost::weak_ptr<Target> fn_80206574(Player::PlayerTargetBase *aim) {
    return aim->m_target;
}
