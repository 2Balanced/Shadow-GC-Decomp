#ifndef GAME_GADGETPARTICLE_H
#define GAME_GADGETPARTICLE_H

#include "boost/weak_ptr.hpp"
#include "size_assert.h"

namespace GadgetParticle {
/* GadgetParticleCtrl (class name from the RTTI of its shared_ptr control block): the particle
 * emitter of a gadget model. */
class GadgetParticleCtrl;
} // namespace GadgetParticle

/* The parameter a gadget passes with fn_80169D00 to fn_8015FA78, which calls it for each part of a
 * model so the particle controller is attached to every part (name and purpose inferred). Its
 * constructor and destructor are shared weak instances at 0x800CC6C8 and 0x800CC62C. */
struct UnkParticleAttach_80169D00 {
    /* 0x0 */ boost::weak_ptr<GadgetParticle::GadgetParticleCtrl> ctrl;
};
SIZE_ASSERT(UnkParticleAttach_80169D00, 0x8);

#endif
