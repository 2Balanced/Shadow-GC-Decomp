/* Player::Npc::BossSonic::Desire::Move (class name from RTTI): Boss Sonic's desire to move. Every
 * frame it picks where the boss should go (BossSonicWork::m_destination): with the AI's flag bit 0
 * set it keeps away from the player (behind the player, to one side of that spot, or a step away
 * from the target when the boss is in front of the player); otherwise it closes in on the player
 * and the target (behind the player when they face each other and are close, or around the point
 * between them). Of the candidate points it takes the one most ahead of the NPC. While mode 1
 * runs the desire fills over the stage's move time; starting it asks for mode 2. File, function
 * and local names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first).
 *
 * NonMatching (16/18 functions; the data matches):
 * KeepAway and Approach differ in register use. Within one statement GC/2.7 keeps the target's
 * pointer in a saved register between the vector calls (the point beside the target: target +
 * Normalize(npc - target) * 10), where retail reloads it from the stack after each call; splitting
 * the statement fixes the loads but reorders the temporaries' stack slots. Approach also loads the
 * circle speed after the Normalize call where retail keeps it in f31 from before. About 25 source
 * shapes and the optimizer pragmas were tried (see docs/research-notes.md on the game's later
 * 2.7-era compiler). */
#include "Game/BossSonic.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

namespace std {
float cos(float x);
} // namespace std

extern const float lbl_805FAD38; /* pi */

extern "C" {
/* Vector helpers (behaviour from their bodies; see Game/VehicleColliMove.cpp). */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
void fn_8040CE74(Vec3f *vector); /* normalizes */
Vec3f fn_8040CEE8(const Vec3f *vector); /* the normalized vector */
float fn_8040CF50(const Vec3f *vector); /* the length */
float fn_8040CF90(const Vec3f *a, const Vec3f *b); /* the dot product */
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b); /* the cross product */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* += */
Vec3f fn_8040D0AC(const Vec3f *vector, float angle); /* turned about Y */
/* The direction the NPC faces (behaviour inferred). */
Vec3f fn_800796DC(Player::Npc::Npc *npc);
}

/* fn_8040CD74 called as returning a - b by value (see Game/VehicleColliMove.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* A vector copied through the out-of-line copy constructor (fn_800091EC; as in
 * Game/VehicleColliMove.cpp). */
struct UnkCopyVector3_800091EC {
    UnkCopyVector3_800091EC() {}
    UnkCopyVector3_800091EC(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    UnkCopyVector3_800091EC(const Vec3f &v) {
        fn_800091EC(reinterpret_cast<Vec3f *>(this), &v);
    }
    UnkCopyVector3_800091EC(const UnkCopyVector3_800091EC &v) {
        fn_800091EC(reinterpret_cast<Vec3f *>(this), reinterpret_cast<const Vec3f *>(&v));
    }

    float x;
    float y;
    float z;
};

#define AS_VEC(v) (*reinterpret_cast<Vec3f *>(&(v)))

/* a += b (through the vector helper). */
inline void AddVector(Vec3f &a, const Vec3f &b) {
    fn_8040CFF8(&a, &b);
}

/* vector = value through the out-of-line assignment. */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

namespace Player {
namespace Npc {
namespace BossSonic {
namespace Desire {

using boost::shared_ptr;

/* Whether to lies within angle of from's facing, on the ground plane. */
static bool IsFacing(const shared_ptr<CharInfo> &from, const shared_ptr<CharInfo> &to, float angle) {
    Vec3f forward(0.0f, 0.0f, -1.0f);
    fn_800091D0(&forward, &(const Vec3f &)fn_8040D0AC(&forward, from->m_rotation.y));
    UnkCopyVector3_800091EC direction;
    fn_800091EC(&AS_VEC(direction), &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&to->m_position, &from->m_position));
    direction.y = 0.0f;
    fn_8040CE74(&AS_VEC(direction));
    float dot = fn_8040CF90(&forward, &AS_VEC(direction));
    return dot > std::cos(angle);
}

/* The distance between a and b on the ground plane. */
static float Distance(const shared_ptr<CharInfo> &a, const shared_ptr<CharInfo> &b) {
    UnkCopyVector3_800091EC offset;
    fn_800091EC(&AS_VEC(offset), &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&a->m_position, &b->m_position));
    offset.y = 0.0f;
    return fn_8040CF50(&AS_VEC(offset));
}

/* The point at local in chara's frame (turned by its facing), on the ground. */
static UnkCopyVector3_800091EC Offset(const shared_ptr<CharInfo> &chara, const Vec3f &local) {
    UnkCopyVector3_800091EC position;
    fn_800091EC(&AS_VEC(position), &(const Vec3f &)fn_8040D0AC(&local, chara->m_rotation.y));
    AddVector(AS_VEC(position), chara->m_position);
    position.y = 0.0f;
    return position;
}

/* Of count points, the one most ahead of the NPC (or zero). */
static UnkCopyVector3_800091EC Nearest(const shared_ptr<Npc> &npc, const Vec3f *points, int count) {
    UnkCopyVector3_800091EC front;
    fn_800091EC(&AS_VEC(front), &(const Vec3f &)fn_800796DC(npc.get()));
    const Vec3f *nearest = 0;
    float best = -2.0f;
    do {
        float dot = fn_8040CF90(&AS_VEC(front), &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(points, &npc->m_position));
        if (best < dot) {
            best = dot;
            nearest = points;
        }
        points++;
    } while (--count > 0);
    if (nearest != 0) {
        return *nearest;
    }
    return UnkCopyVector3_800091EC(0.0f, 0.0f, 0.0f);
}

class Move : public BossSonicDesireBase {
  public:
    Move(const shared_ptr<BossSonicWork> &work) : BossSonicDesireBase(work, "Move", 0) {}
    virtual ~Move();
    virtual void Tick(float dt);
    virtual void Begin();

    void UpdateDestination();
    void Approach();
    void KeepAway();

    static shared_ptr<BossSonicDesireBase> Instantiate(const shared_ptr<BossSonicWork> &work);
};
SIZE_ASSERT(Move, 0x1C);

Move::~Move() {}

void Move::Tick(float dt) {
    UpdateDestination();
    if (m_work->m_mode.m_mode == 1) {
        float time = m_work->m_params->m_moveTime;
        m_desire += time <= 0.0f ? 1.0f : dt / time;
    }
}

void Move::Begin() {
    BossSonicDesireBase::Begin();
    m_work->m_mode.Request(2);
    m_desire = 0.0f;
}

void Move::UpdateDestination() {
    if (m_work->m_flags.Test(0)) {
        KeepAway();
    } else {
        Approach();
    }
}

void Move::Approach() {
    if (!m_work->m_target.expired() && !m_work->m_player.expired()) {
        shared_ptr<Npc> npc = m_work->m_npc.lock();
        shared_ptr<CharInfo> player = m_work->m_player.lock();
        shared_ptr<CharInfo> target = m_work->m_target.lock();
        Vec3f points[3];
        Vec3f center;
        if (IsFacing(player, target, 90.0f * (lbl_805FAD38 / 180.0f)) &&
            Distance(player, target) < m_work->m_params->m_keepDistance) {
            fn_800091D0(&m_work->m_destination, &(const Vec3f &)Offset(player, Vec3f(0.0f, 0.0f, -50.0f)));
        } else {
            int count = 0;
            fn_800091D0(&center, &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)fn_8040CDD0(&player->m_position, &target->m_position), 0.5f));
            fn_800091D0(&points[count++],
                        &(const Vec3f &)fn_8040CDD0(&target->m_position,
                                                    &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)fn_8040CEE8(&(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&npc->m_position, &target->m_position)), 10.0f)));
            UnkCopyVector3_800091EC side;
            fn_800091EC(&AS_VEC(side), &(const Vec3f &)fn_8040CFB8(&(const Vec3f &)fn_8040CEE8(&(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&points[0], &center)), &(const Vec3f &)Vec3f(0.0f, 1.0f, 0.0f)));
            fn_800091D0(&AS_VEC(side), &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)fn_8040CEE8(&AS_VEC(side)), m_work->m_params->m_circleSpeed));
            fn_800091D0(&points[count++], &(const Vec3f &)fn_8040CDD0(&center, &AS_VEC(side)));
            fn_800091D0(&points[count++], &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&center, &AS_VEC(side)));
            BossSonicWork *work = m_work.get();
            fn_800091D0(&work->m_destination, &(const Vec3f &)Nearest(npc, points, count));
        }
    }
}

void Move::KeepAway() {
    if (!m_work->m_target.expired() && !m_work->m_player.expired()) {
        shared_ptr<Npc> npc = m_work->m_npc.lock();
        shared_ptr<CharInfo> player = m_work->m_player.lock();
        shared_ptr<CharInfo> target = m_work->m_target.lock();
        Vec3f points[3];
        Vec3f behind;
        Vec3f side;
        int count = 0;
        fn_800091D0(&behind, &(const Vec3f &)Offset(player, Vec3f(0.0f, 0.0f, -50.0f)));
        fn_800091D0(&side, &(const Vec3f &)Vec3f(0.0f, 0.0f, -1.0f));
        fn_800091D0(&side, &(const Vec3f &)fn_8040D0AC(&side, player->m_rotation.y));
        UnkCopyVector3_800091EC right;
        fn_800091EC(&AS_VEC(right), &(const Vec3f &)fn_8040CFB8(&side, &(const Vec3f &)Vec3f(0.0f, 1.0f, 0.0f)));
        fn_8040CE74(&side);
        fn_800091D0(&points[count++], &(const Vec3f &)fn_8040CDD0(&behind, &(const Vec3f &)fn_8040CDA8(&AS_VEC(right), m_work->m_params->m_sideStep)));
        fn_800091D0(&points[count++], &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&behind, &(const Vec3f &)fn_8040CDA8(&AS_VEC(right), m_work->m_params->m_sideStep)));
        if (fn_8040CF90(&(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&npc->m_position, &player->m_position), &side) > 0.0f) {
            fn_800091D0(&points[count++],
                        &(const Vec3f &)fn_8040CDD0(&target->m_position,
                                                    &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)fn_8040CEE8(&(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&npc->m_position, &target->m_position)), 10.0f)));
        }
        BossSonicWork *work = m_work.get();
        fn_800091D0(&work->m_destination, &(const Vec3f &)Nearest(npc, points, count));
    }
}

shared_ptr<BossSonicDesireBase> Move::Instantiate(const shared_ptr<BossSonicWork> &work) {
    return shared_ptr<BossSonicDesireBase>(new Move(work));
}

} // namespace Desire
} // namespace BossSonic
} // namespace Npc
} // namespace Player
