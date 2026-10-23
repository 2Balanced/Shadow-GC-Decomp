#ifndef GAME_VEHICLEEFFECT_H
#define GAME_VEHICLEEFFECT_H

#include "Game/Task.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>

namespace Vehicle {

class TVehicleBase;
class EffectImpl;

/* Effect (class name from RTTI; vtable at 0x8055F394; Game/VehicleEffect.cpp): the task that runs
 * a vehicle's damage effect. The vehicle keeps it at its word 0x74 (TVehicleBase::field_0x74).
 * 0x30 bytes. Method names are inferred. */
class Effect : public Task {
  public:
    Effect(Task *parent, const boost::shared_ptr<TVehicleBase> &vehicle);
    virtual ~Effect();
    virtual void Tick(float dt);
    /* Sets the model frame the damage effect follows (0x8034F250). */
    void SetNode(void *frame);
    /* Starts the vehicle kind's burst effect at a model frame (0x8034F1D0). */
    void StartBurst(void *frame);

    /* 0x28 */ boost::scoped_ptr<EffectImpl> m_impl;
    /* 0x2C */ u32 pad_0x2C; /* the class size is a multiple of 8 */
};

} // namespace Vehicle

#endif
