#ifndef GAME_COLLISIONEFFECT_H
#define GAME_COLLISIONEFFECT_H

#include "Game/MathTypes.h"
#include "boost/scoped_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The state of a CollisionEffect (class name unknown; defined in Game/GroundEffectTracker.cpp). */
class CollisionEffectImpl;

/* CollisionEffect (class name from the RTTI of its shared_ptr control block,
 * boost::detail::sp_counted_base_impl<CollisionEffect *, boost::checked_deleter<CollisionEffect>>):
 * plays the effect of the ground kind its owner stands on (dust, splashes, ...) and the one-shot
 * effect of a landing. Vehicles (Game/VehicleColliMove.cpp) embed one; enemies hold one in a
 * shared_ptr. Implemented in Game/GroundEffectTracker.cpp; constructor 0x80344608, destructor
 * 0x8034454C. */
class CollisionEffect {
  public:
    CollisionEffect();
    ~CollisionEffect();

    /* 0x0 */ boost::scoped_ptr<CollisionEffectImpl> m_impl;
};
SIZE_ASSERT(CollisionEffect, 0x4);

/* The rest of the class's interface keeps its fn_ names (member names unknown; behaviour
 * inferred). kind indexes the table of ground kinds (0 to 15). */
extern "C" {
/* Whether the owner was on the ground at the last update. */
u8 fn_803444B0(CollisionEffect *effect);
/* Stops the ground effect. */
void fn_803444BC(CollisionEffect *effect);
/* The owner stands on ground of kind at position, normal pointing up from it: plays (or keeps
 * playing) the kind's effect there. */
void fn_803444E0(CollisionEffect *effect, const Vec3f *position, const Vec3f *normal, u32 attributes,
                 int kind);
/* The owner lands on ground of kind: plays the kind's landing effect once. */
void fn_80344504(CollisionEffect *effect, const Vec3f *position, const Vec3f *normal, u32 attributes,
                 int kind);
/* Per frame: stops the effect when there was no update since the last tick, else sets its rate. */
void fn_80344528(CollisionEffect *effect, float dt);
/* Gives ground kind 14 the current stage's own effects, and gives it back its default ones (at
 * the start and end of a stage; Game/StageManager.cpp). */
void fn_80344420(void);
void fn_803443CC(void);
}

#endif
