/* Vehicle::Effect (class name from RTTI; its task name is "VehicleEffect"): the task that runs a
 * vehicle's damage effect. Each frame it compares the vehicle's health with its maximum and, when
 * the damage level changes, replaces the effect at the vehicle's node with the one the vehicle's
 * kind has for that level. The vehicle keeps the task at its word 0x74. The file name comes from
 * the RTTI name; function and member names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include "Effect/System/EffectElement.h"
#include "Game/MathTypes.h"
#include "Game/TVehicleBase.h"
#include "Game/Task.h"
#include "Game/VehicleEffect.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Effect::System::EffectElement;

/* What CharInfo's word at 0x60 points at (struct name unknown; see Game/DiablonEffect.cpp). */
struct UnkCharTime {
    /* 0x0 */ float m_frameTime;
    /* 0x4 */ u32 field_0x4;
};

extern "C" {
/* A 32-bit flag word: fn_80014E04 clears it, fn_80014E1C sets bit and fn_80014E34 tests bit (see
 * Game/GadgetModelInfo.h). */
void fn_80014E04(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
/* The vehicle's kind, health and maximum health (the first words of its object at 0x98; names
 * inferred). */
int fn_801B6938(Vehicle::TVehicleBase *vehicle);
float fn_801B64F8(Vehicle::TVehicleBase *vehicle);
float fn_801B6564(Vehicle::TVehicleBase *vehicle);
/* Starts effect id at a position, and at a node (see Game/GadgetCommonData.cpp and
 * Game/ClumpFrameEffects.cpp). */
boost::weak_ptr<EffectElement> fn_801D2B74(int arg0, void *position, int id, u32 *arg3);
boost::weak_ptr<EffectElement> fn_801D2C28(int arg0, void *node, int id, u32 *arg3);
}

/* The effects of a vehicle kind (struct name inferred): one per damage level (-1 for none), and
 * the burst StartBurst starts. */
struct UnkVehicleEffectIds {
    s32 m_levels[3];
    s32 m_burst;
};

static const UnkVehicleEffectIds s_effectIds[14] = {
    {{-1, -1, -1}, -1},
    {{-1, 0x230, 0x237}, 0x72},
    {{-1, 0x230, 0x237}, 0x72},
    {{-1, 0x230, 0x237}, 0x72},
    {{-1, 0x2F3, 0x2F6}, 0x72},
    {{-1, 0xD2, 0xD4}, 0x72},
    {{-1, 0xD2, 0xD4}, 0x72},
    {{-1, 0x2EA, 0x2EC}, 0x72},
    {{-1, 0x2E2, 0x2E5}, 0x78},
    {{-1, 0x2E2, 0x2E5}, 0x78},
    {{-1, -1, -1}, -1},
    {{-1, -1, -1}, -1},
    {{-1, -1, -1}, -1},
    {{-1, -1, -1}, -1},
};

namespace Vehicle {

/* The flag word (struct name unknown; bit 0: the effect node is set). */
struct EffectFlags {
    EffectFlags() {
        fn_80014E04(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits;
};

/* The task's state (class name unknown). */
class EffectImpl {
  public:
    EffectImpl(const boost::shared_ptr<TVehicleBase> &vehicle);
    ~EffectImpl();
    void Tick(float dt);
    /* Replaces the effect when the damage level changed. */
    void Update();

    /* 0x00 */ boost::shared_ptr<TVehicleBase> m_vehicle;
    /* 0x08 */ EffectFlags m_flags;
    /* 0x0C */ s32 m_level; /* 0 to 2 */
    /* 0x10 */ void *m_node; /* a model frame */
    /* 0x14 */ boost::weak_ptr<EffectElement> m_effect;
};

/* Deletes the state (name inferred; see checked_delete below). */
inline void DeleteEffectImpl(EffectImpl *impl) {
    delete impl;
}

} // namespace Vehicle

/* Retail keeps boost::checked_delete<EffectImpl> out of line and the destructor at its own place;
 * this specialization gives that order under GC/2.7 (see docs/research-notes.md and
 * Game/EnergyCore.cpp). */
namespace boost {
template <> inline void checked_delete<Vehicle::EffectImpl>(Vehicle::EffectImpl *x) {
    Vehicle::DeleteEffectImpl(x);
}
} // namespace boost

SIZE_ASSERT(Vehicle::EffectImpl, 0x1C);
SIZE_ASSERT(Vehicle::Effect, 0x30);

namespace Vehicle {

EffectImpl::EffectImpl(const boost::shared_ptr<TVehicleBase> &vehicle) : m_vehicle(vehicle) {
    m_level = 0;
    m_node = 0;
}

EffectImpl::~EffectImpl() {
    if (!m_effect.expired()) {
        m_effect.lock()->Stop();
    }
}

void EffectImpl::Tick(float dt) {
    Update();
}

void EffectImpl::Update() {
    if (m_flags.Test(0)) {
        TVehicleBase *vehicle = m_vehicle.get();
        float max = fn_801B6564(vehicle);
        float ratio = fn_801B64F8(vehicle) / max;
        int level = 0;
        if (ratio < 0.55f) {
            level = 1;
            if (ratio < 0.3f) {
                level = 2;
            }
        }
        if (m_level != level) {
            m_level = level;
            if (!m_effect.expired()) {
                m_effect.lock()->Stop();
                m_effect.reset();
            }
            int id = s_effectIds[fn_801B6938(m_vehicle.get())].m_levels[m_level];
            if (id >= 0) {
                m_effect = fn_801D2C28(0, m_node, id,
                                       &reinterpret_cast<UnkCharTime *&>(m_vehicle->field_0x60)->field_0x4);
            }
        }
    }
}

Effect::Effect(Task *parent, const boost::shared_ptr<TVehicleBase> &vehicle)
    : Task(parent), m_impl(new EffectImpl(vehicle)) {
    m_name = "VehicleEffect";
    m_impl->m_vehicle->field_0x74 = (u32)this;
}

Effect::~Effect() {
    m_impl->m_vehicle->field_0x74 = 0;
}

void Effect::Tick(float dt) {
    m_impl->Tick(dt);
}

void Effect::SetNode(void *frame) {
    m_impl->m_node = frame;
    /* A direct call: through the inline Set, scoped_ptr's operator-> would be called out of line. */
    fn_80014E1C(&m_impl->m_flags.m_bits, 0);
}

void Effect::StartBurst(void *frame) {
    int id = s_effectIds[fn_801B6938(m_impl->m_vehicle.get())].m_burst;
    if (id >= 0) {
        fn_801D2B74(0, frame, id, &reinterpret_cast<UnkCharTime *&>(m_impl->m_vehicle->field_0x60)->field_0x4);
    }
}

} // namespace Vehicle
