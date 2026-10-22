/* Boss::Diablon::Desire::Beam (class name from RTTI): the Diablon boss's desire to fire the beam.
 * While field 0xD0 is 1-4, the boss faces the player and the player is in reach, the desire grows;
 * otherwise a full desire falls back just below full, and with field 0xD0 at 0 it is cleared.
 * Starting it switches the boss to the beam behavior (Behavior::Beam). File name inferred from the
 * class. Names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include "Game/BossDesire.h"
#include "Game/CharInfo.h"
#include "Game/Diablon.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

namespace std {
float cos(float x);
}

extern const float lbl_805FAD38; /* pi */

namespace Boss {
namespace Diablon {
namespace Desire {

class Beam : public DiablonDesireBase {
  public:
    Beam(const boost::shared_ptr<Diablon> &owner) : DiablonDesireBase(owner, "Beam", 2) {}
    virtual ~Beam();
    virtual void Tick(float dt);
    virtual void Begin();

    /* Whether the boss faces the player, and whether the player is in the beam's reach (names
     * inferred). */
    bool IsFacingTarget();
    bool IsTargetInRange();

    static boost::shared_ptr<DiablonDesireBase> Instantiate(const boost::shared_ptr<Diablon> &owner);
};
SIZE_ASSERT(Beam, 0x1C);

Beam::~Beam() {}

void Beam::Tick(float dt) {
    if (m_owner->field_0xD0 <= 0) {
        m_desire = 0.0f;
    } else if (m_owner->field_0xD0 >= 5 || !IsFacingTarget()) {
        if (m_desire >= 1.0f) {
            m_desire = 0.98f;
        }
    } else if (IsTargetInRange()) {
        m_desire += dt / (m_owner->m_flags.Test(0) ? m_owner->m_params->m_beamDesireTime
                                                            : m_owner->m_params->m_beamDesireTimeStill);
    }
}

void Beam::Begin() {
    DiablonDesireBase::Begin();
    fn_802D9ED8(m_owner->m_executer, Behavior::InitializeArgs(4));
    m_desire = 0.0f;
}

bool Beam::IsFacingTarget() {
    DiablonParams *params = m_owner->m_params;
    float facing = fn_802D9570(m_owner.get());
    return !(facing < std::cos(params->m_beamFacingAngle * (lbl_805FAD38 / 180.0f)));
}

bool Beam::IsTargetInRange() {
    boost::shared_ptr<CharInfo> target = m_owner->m_target.lock();
    DiablonParams *params = m_owner->m_params;
    if (target.get() == 0) {
        return false;
    }
    float distance = fn_802D94B8(m_owner.get());
    if (params->m_beamMaxDistance < distance) {
        return false;
    }
    if (m_owner->m_flags.Test(0)) {
        if (params->m_beamMinDistance > distance) {
            return false;
        }
    } else if (params->m_beamMinTargetHeight > target->m_position.y) {
        return false;
    }
    return true;
}

boost::shared_ptr<DiablonDesireBase> Beam::Instantiate(const boost::shared_ptr<Diablon> &owner) {
    return boost::shared_ptr<DiablonDesireBase>(new Beam(owner));
}

} // namespace Desire
} // namespace Diablon
} // namespace Boss
