/* Player::Npc::BossSonic::Desire::Attack (class name from RTTI): Boss Sonic's desire to attack.
 * While the NPC may attack the target, the target is in front of it (within the stage's attack
 * angle) and in reach, the desire fills over the stage's attack time; starting it asks for
 * mode 3. File and function names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
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
Vec3f fn_8040CEE8(const Vec3f *vector); /* the normalized vector */
float fn_8040CF50(const Vec3f *vector); /* the length */
float fn_8040CF90(const Vec3f *a, const Vec3f *b); /* the dot product */
/* The direction the NPC faces (see Game/BossSonicDesireMove.cpp). */
Vec3f fn_800796DC(Player::Npc::Npc *npc);
/* Whether the character may attack now (behaviour inferred). */
bool fn_800B7AD0(const boost::shared_ptr<CharInfo> &chara);
}

/* Degrees to radians (name inferred). */
inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* fn_8040CD74 called as returning a - b by value (see Game/VehicleColliMove.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

namespace Player {
namespace Npc {
namespace BossSonic {
namespace Desire {

using boost::shared_ptr;

class Attack : public BossSonicDesireBase {
  public:
    Attack(const shared_ptr<BossSonicWork> &work) : BossSonicDesireBase(work, "Attack", 1) {}
    virtual ~Attack();
    virtual void Tick(float dt);
    virtual void Begin();

    bool CanAttack();

    static shared_ptr<BossSonicDesireBase> Instantiate(const shared_ptr<BossSonicWork> &work);
};
SIZE_ASSERT(Attack, 0x1C);

Attack::~Attack() {}

void Attack::Tick(float dt) {
    if (CanAttack()) {
        float time = m_work->m_params->m_attackTime;
        m_desire += time <= 0.0f ? 1.0f : dt / time;
    }
}

void Attack::Begin() {
    BossSonicDesireBase::Begin();
    m_work->m_mode.Request(3);
    m_desire = 0.0f;
}

bool Attack::CanAttack() {
    if (m_work->m_target.expired()) {
        return false;
    }
    shared_ptr<Npc> npc = m_work->m_npc.lock();
    shared_ptr<CharInfo> target = m_work->m_target.lock();
    bool ready = fn_800B7AD0(shared_ptr<CharInfo>(npc));
    if (!ready) {
        return false;
    }
    Vec3f front;
    Vec3f offset;
    fn_800091EC(&front, &(const Vec3f &)fn_800796DC(npc.get()));
    fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&target->m_position, &npc->m_position));
    float dot = fn_8040CF90(&front, &(const Vec3f &)fn_8040CEE8(&offset));
    if (dot < std::cos(DegToRad(m_work->m_params->m_attackAngle))) {
        return false;
    }
    if (fn_8040CF50(&offset) > m_work->m_params->m_attackRange) {
        return false;
    }
    return true;
}

shared_ptr<BossSonicDesireBase> Attack::Instantiate(const shared_ptr<BossSonicWork> &work) {
    return shared_ptr<BossSonicDesireBase>(new Attack(work));
}

} // namespace Desire
} // namespace BossSonic
} // namespace Npc
} // namespace Player
