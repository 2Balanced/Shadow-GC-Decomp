/* Player::Behavior::LightDash (class name from RTTI): behavior 0x14 of Shadow, the light dash along a
 * line of targets (rings; bit 3 of a target's flags). The player flies from target to target,
 * touching each (UseCommand), as far as its speed carries it in a frame, and faces where it goes;
 * it falls (behavior 2) when the line ends or something blocks the way. Leaving it keeps a share of
 * the speed that grows with the last step's length. File name inferred from the class (as
 * Game/PlayerBehaviorSuper.cpp); all other names are inferred. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). Where the game reloads the player
 * pointer in a call's arguments after another argument's call, the code reads m_player.px (as in
 * Game/PlayerBehaviorGrind.cpp). */
/* Declares Player::Behavior::Behavior's slots with their arguments (see Game/PlayerBehavior.h). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
#include <cmath>

/* Defines CharaColliBase::GetType and IsType4Or5, whose out-of-line copies this unit emits. */
#define CHARACOLLI_BASE_TYPE_INLINE

#include "Game/CharCommand.h"
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/GadgetSound.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerSoundBase.h"
#include "Game/PlayerWeaponBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::CharaColliBaseCharInfo;

/* Target (class name from RTTI; see Game/CharCommand.h): what the light dash follows. Only what this
 * unit uses is declared (names and meanings inferred). */
class Target {
  public:
    /* Whether the target can be locked on to, and whether a light dash follows it (flag bits 0 and
     * 3). Inline functions in the game: the shared_ptr's operator-> before them stays out of
     * line. */
    bool IsActive() const {
        return m_flags & 1;
    }
    bool IsLightDashTarget() const {
        return m_flags & 8;
    }

    /* The point to aim at, as seen from a position (vtable slot 0x8). */
    virtual Vec3f GetPosition(const Vec3f *from);

    /* 0x04 */ u8 field_0x4[0x28 - 0x4];
    /* 0x28 */ boost::weak_ptr<CharInfo> m_owner; /* the character the target belongs to */
    /* 0x30 */ u32 m_flags;
};

/* boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out of line; this unit
 * emits the copy for the target (boost 1.32's definition; as in Game/AdvFolder.cpp). */
namespace boost {
template <> inline void shared_ptr<Target>::reset() {
    this_type().swap(*this);
}
} // namespace boost

/* The player's members read here (struct name unknown; Player::PlayerBase leaves them
 * unmapped). */
struct UnkPlayerFields_80089C20 {
    /* 0x000 */ u8 field_0x0[0xD0];
    /* 0x0D0 */ Vec3f m_prevPosition; /* where the player was before this frame's move (inferred) */
    /* 0x0DC */ u8 field_0xDC[0xE8 - 0xDC];
    /* 0x0E8 */ Vec3f m_jumpUp;       /* (as in Game/PlayerBehaviorHomingAttack.cpp) */
    /* 0x0F4 */ u8 field_0xF4[0x1C8 - 0xF4];
    /* 0x1C8 */ Vec3f m_groundNormal; /* (as in Game/PlayerBehaviorHomingAttack.cpp) */
};

/* A line segment (layout unknown; built by fn_80020A20, as in Game/AttackObject.cpp). */
struct UnkSegment_80020A20 {
    /* 0x00 */ u8 field_0x0[0x18];
};

extern "C" {
/* The player's flag word and state bits (see Game/PlayerShadowSuper.cpp). */
bool fn_80014E34(u32 *flags, int bit);
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* Builds a segment from start to end (see Game/GadgetUtil.h). */
void *fn_80020A20(UnkSegment_80020A20 *segment, const Vec3f *start, const Vec3f *end);
/* Called with the player's object at 0x234: stopping and starting its effect (behaviour inferred;
 * see Game/PlayerBehaviorFall.cpp). */
void fn_8007792C(void *object);
void fn_80077950(void *object, int arg);
/* The player's front axis (see Game/PlayerBehaviorGrind.cpp). */
Vec3f fn_800796DC(Player::PlayerBase *player);
/* Player updates run before and after the behavior's own (behaviour unknown; as in FirstMeet). */
void fn_80079720(Player::PlayerBase *player);
void fn_8009728C(boost::shared_ptr<Player::PlayerBase> *player, int arg1);
/* The player's Euler angles for a frame (see Game/PlayerBehaviorHomingAttack.cpp). */
Vec3f fn_80093D34(boost::shared_ptr<Player::PlayerBase> *player, const Vec3f *forward,
                  const Vec3f *up);
/* Sweeps the player's collision along a segment: the share of the way that is clear (at most 0
 * when nothing is in the way) and the collider met (behaviour inferred; the other arguments are
 * unknown). */
float fn_800979E8(boost::shared_ptr<Player::PlayerBase> *player, void *arg1, void *arg2,
                  boost::weak_ptr<CharaColliBaseCharInfo> *hit, void *arg4, void *arg5,
                  UnkSegment_80020A20 *segment, void *arg7, u32 attributes, u32 mask, int arg10);
/* The trail of the player's effects at 0x23C (see Game/PlayerBehaviorHomingAttack.cpp). */
void fn_800A2D30(void *effects);
void fn_800A2D54(void *effects, float length);
/* Switches a player's behavior (behaviour inferred). */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);
/* The target nearest to position within a cone about direction, among those with flags (see
 * Game/PlayerTargetBase.cpp). */
boost::weak_ptr<Target> fn_802065D8(Player::PlayerTargetBase *aim, const Vec3f *position,
                                    const Vec3f *direction, int flags, float cosAngle);

Vec3f fn_800091A4(const Vec3f *vector, float scale); /* vector * scale */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale); /* vector * scale */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);   /* a + b */
void fn_8040CE38(Vec3f *vector, float scale);        /* vector *= scale */
Vec3f fn_8040CEE8(const Vec3f *vector);              /* normalized */
float fn_8040CF50(const Vec3f *v);                   /* |v| */
float fn_8040CF90(const Vec3f *a, const Vec3f *b);   /* dot product */
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b);   /* cross product */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* vector += value */
float fn_8040D1B4(const Vec3f *a, const Vec3f *b);   /* the distance between */
}

/* The player's members above (a macro, as in Game/PlayerBehaviorSuper.cpp). */
#define FieldsOf(player) reinterpret_cast<UnkPlayerFields_80089C20 *>(player)

/* fn_8040CD74 returns lhs - rhs through its first argument, as a function returning a Vec3f does;
 * called this way the result is a temporary, as in the game (see Game/PlayerBehaviorSuper.cpp). */
typedef Vec3f (*VectorSubtract)(const Vec3f *lhs, const Vec3f *rhs);
#define Subtract(lhs, rhs) (reinterpret_cast<VectorSubtract>(fn_8040CD74)(lhs, rhs))

/* Degrees to radians (name inferred; as in Game/PlayerBehaviorGrind.cpp). */
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

/* vector *= scale (the game's out-of-line scaling, fn_8040CE38; as in
 * Game/PlayerBehaviorGround.cpp). */
inline Vec3f &operator*=(Vec3f &vector, float scale) {
    fn_8040CE38(&vector, scale);
    return vector;
}

namespace Player {
namespace Behavior {

class LightDash : public Behavior {
  public:
    LightDash(const boost::shared_ptr<PlayerBase> &player);
    virtual ~LightDash();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x18(CharCommand *command);
    virtual void Update(float dt);
    virtual void vf_0x20();

    /* The factory record's functions (names inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);
    static void Release();

    boost::weak_ptr<Target> FindTarget(const Vec3f &position, const Vec3f &direction);
    bool CheckPath();

    static const char *s_name;

    /* 0x1C */ boost::weak_ptr<Target> m_target;
    /* 0x24 */ Vec3f m_direction; /* from the last target to the next */
    /* 0x30 */ Vec3f m_point;     /* the next target's position */
    /* 0x3C */ float m_distance;  /* the last step's length */
    /* 0x40 */ UnkSoundHandle_801D3DC8 m_voice;
};
SIZE_ASSERT(LightDash, 0x5C);

const char *LightDash::s_name = "LightDash";

/* How far a target may be from the last one (name inferred). The game sets it in __sinit rather
 * than in .sdata, as an initializer that is not a constant expression does; the inline call
 * reproduces that (as in Game/PlayerBehaviorFall.cpp). */
inline float DynamicFloat(float value) {
    return value;
}
static float s_range = DynamicFloat(80.0f);

LightDash::LightDash(const boost::shared_ptr<PlayerBase> &player)
    : Behavior(player, s_name, 0x14) {}

LightDash::~LightDash() {}

bool LightDash::vf_0x0C(int prevId, const InitializeArgs &args) {
    if (!fn_80014E34(&m_player.get()->m_flags, 3)) {
        return false;
    }
    boost::shared_ptr<Target> target = m_player.get()->m_target->vf_0x28().lock();
    if (target.get() != 0 && target->IsLightDashTarget()) {
        return true;
    }
    return false;
}

void LightDash::Enter(int prevId, const InitializeArgs &args) {
    fn_80076CC8(m_player.get()->m_stateBits, 2);
    fn_80076CA8(m_player.get()->m_stateBits, 0x38);
    fn_80076CC8(m_player.get()->m_stateBits, 0x32);
    fn_80076CA8(m_player.get()->m_stateBits, 0x36);
    fn_800091D0(&FieldsOf(m_player.get())->m_groundNormal, &FieldsOf(m_player.get())->m_jumpUp);
    m_player.get()->m_motion->vf_0x10(6, 0.0f);
    fn_800A2D54(reinterpret_cast<void *>(m_player.get()->field_0x23C), 5.0f);
    fn_80076CA8(m_player.get()->m_stateBits, 0x35);
    fn_80077950(reinterpret_cast<void *>(m_player.get()->field_0x234), 5);
    m_target = m_player.get()->m_target->vf_0x28();
    fn_800091D0(&m_point,
                &(const Vec3f &)m_target.lock()->GetPosition(&m_player.get()->m_position));
    fn_800091D0(&m_direction, &(const Vec3f &)fn_8040CEE8(&(const Vec3f &)Subtract(
                                  &m_point, &m_player.get()->m_position)));
    m_distance = 0.0f;
    m_voice = m_player.get()->m_sound->PlayVoice(0xE, reinterpret_cast<s32>(this));
}

void LightDash::Leave(int nextId) {
    fn_80076CC8(m_player.get()->m_stateBits, 0x38);
    fn_80076CC8(m_player.get()->m_stateBits, 0x36);
    fn_800A2D30(reinterpret_cast<void *>(m_player.get()->field_0x23C));
    fn_80076CC8(m_player.get()->m_stateBits, 0x35);
    fn_8007792C(reinterpret_cast<void *>(m_player.get()->field_0x234));
    m_player->field_0x18 *= 0.5f * (m_distance / s_range) + 0.5f;
    m_voice.fn_801D455C();
}

void LightDash::vf_0x18(CharCommand *command) {}

void LightDash::Update(float dt) {
    boost::shared_ptr<Target> target = m_target.lock();
    if (target.get() != 0 && !target->IsActive()) {
        target.reset();
    }
    if (target.get() != 0) {
        fn_800091D0(&m_point, &(const Vec3f &)target->GetPosition(&m_player.get()->m_position));
    }
    Vec3f direction;
    fn_800091EC(&direction, &(const Vec3f &)fn_8040CEE8(&(const Vec3f &)Subtract(
                                &m_point, &m_player.get()->m_position)));
    AddVector(m_player->field_0x24, fn_800091A4(&direction, 5000.0f));
    fn_800091D0(&m_player.px->field_0x18,
                &(const Vec3f &)fn_8040CDA8(&direction, fn_8040CF50(&m_player.get()->field_0x18)));
    if (!CheckPath()) {
        fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(2));
        return;
    }
    fn_80079720(m_player.get());
    Vec3f position;
    fn_800091EC(&position, &FieldsOf(m_player.get())->m_prevPosition);
    float remaining = fn_8040D1B4(&m_player.get()->m_position, &position);
    Vec3f toPoint;
    fn_800091EC(&toPoint, &(const Vec3f &)fn_8040CEE8(&(const Vec3f &)Subtract(
                              &m_point, &m_player.get()->m_position)));
    if (fn_8040CF90(&toPoint, &(const Vec3f &)fn_800796DC(m_player.get())) < 0.0f) {
        do {
            if (target.get() != 0) {
                boost::shared_ptr<CharInfo> owner = target->m_owner.lock();
                if (owner.get() != 0) {
                    owner->PushCommand(new UseCommand(boost::weak_ptr<CharInfo>(m_player),
                                                      boost::weak_ptr<Target>(target)));
                }
            }
            remaining -= fn_8040D1B4(&m_point, &position);
            AssignVector(position, m_point);
            m_target = FindTarget(position, m_direction);
            if (m_target.expired()) {
                AssignVector(toPoint, m_direction);
                fn_800A7F14(m_player.get()->m_behaviors,
                            &(const InitializeArgs &)InitializeArgs(2));
                break;
            }
            target = m_target.lock();
            Vec3f targetPosition;
            fn_800091EC(&targetPosition, &(const Vec3f &)target->GetPosition(&position));
            fn_800091D0(&toPoint, fn_800091D0(&m_direction,
                                              &(const Vec3f &)fn_8040CEE8(&(const Vec3f &)Subtract(
                                                  &targetPosition, &position))));
            m_distance = fn_8040D1B4(&targetPosition, &position);
            AssignVector(m_point, targetPosition);
        } while (m_distance < remaining);
    } else {
        fn_800091D0(&toPoint, &(const Vec3f &)fn_8040CEE8(
                                  &(const Vec3f &)Subtract(&m_point, &position)));
    }
    fn_800091D0(&m_player.px->m_position,
                &(const Vec3f &)fn_8040CDD0(&position,
                                            &(const Vec3f &)fn_8040CDA8(&toPoint, remaining)));
    Vec3f side;
    fn_800091EC(&side, &(const Vec3f &)fn_8040CEE8(&(const Vec3f &)fn_8040CFB8(
                           &toPoint, &FieldsOf(m_player.get())->m_groundNormal)));
    Vec3f up;
    fn_800091EC(&up, &(const Vec3f &)fn_8040CEE8(&(const Vec3f &)fn_8040CFB8(&side, &toPoint)));
    m_player->m_rotation = fn_80093D34(&m_player, &toPoint, &up);
    fn_8009728C(&m_player, 0);
    fn_80076CC8(m_player.get()->m_stateBits, 2);
}

/* The next target of the line after position along direction, if it is near enough (name
 * inferred). */
boost::weak_ptr<Target> LightDash::FindTarget(const Vec3f &position, const Vec3f &direction) {
    float cosAngle = std::cos(DegToRad(45.0f));
    Vec3f point;
    AssignVector(point, fn_8040CDD0(&position, &(const Vec3f &)fn_8040CDA8(&direction, 1.0f)));
    boost::weak_ptr<Target> found =
        fn_802065D8(m_player.get()->m_target, &point, &direction, 8, cosAngle);
    boost::shared_ptr<Target> target = found.lock();
    if (target.get() != 0 && target->IsActive()) {
        Vec3f targetPosition;
        fn_800091EC(&targetPosition, &(const Vec3f &)target->GetPosition(&point));
        if (fn_8040D1B4(&targetPosition, &point) < s_range) {
            return target;
        }
    }
    return boost::weak_ptr<Target>();
}

/* Whether the way to the next target is clear: nothing in it, or only colliders of type 4 or 5
 * (name inferred). */
bool LightDash::CheckPath() {
    boost::weak_ptr<CharaColliBaseCharInfo> hit;
    UnkSegment_80020A20 segment;
    fn_80020A20(&segment, &m_player.get()->m_position, &m_point);
    if (fn_800979E8(&m_player, 0, 0, &hit, 0, 0, &segment, 0, 0x1A93, 0x40000, 2) <= 0.0f) {
        return true;
    }
    if (!hit.expired()) {
        boost::shared_ptr<CharaColliBaseCharInfo> other = hit.lock();
        if (other->IsType4Or5()) {
            return true;
        }
    }
    return false;
}

void LightDash::vf_0x20() {}

Behavior *LightDash::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new LightDash(player);
}

void LightDash::Release() {}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds LightDash by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804B08F4 = {
    Player::Behavior::LightDash::Create, Player::Behavior::LightDash::Release, 0};
