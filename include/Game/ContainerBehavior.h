#ifndef GAME_CONTAINERBEHAVIOR_H
#define GAME_CONTAINERBEHAVIOR_H

#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetUtil.h"
#include "Game/TBlockManage.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The handle fn_8015B8C4 wraps a collider in (class name unknown; Game/ContainerBehavior.cpp maps
 * it). */
class GadgetCharCollision;

/* The placement and fall of a ContainerBehavior (struct name unknown; its constructor is out of
 * line at 0x800F9570). Member names inferred. */
struct ContainerBehaviorMotion {
    ContainerBehaviorMotion()
        : m_position(0.0f, 0.0f, 0.0f), m_rotation(0.0f, 0.0f, 0.0f), m_fallSpeed(0.0f),
          m_groundY(0.0f) {}

    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ Vec3f m_rotation;
    /* 0x18 */ float m_fallSpeed; /* added to the height every update */
    /* 0x1C */ float m_groundY;   /* the height of the ground below */
};

/* ContainerBehavior (class name from RTTI; vtable at 0x80525560): drops a pickup object (a
 * container or a bomb) to the ground below it and keeps a box collider of its size on it (meaning
 * inferred). Its own unit is Game/ContainerBehavior.cpp; the BOMB unit emits the first copies of
 * its inline helpers and of its shared_ptr instances. Member names inferred. */
class ContainerBehavior {
  public:
    ContainerBehavior(GadgetCharInfo *charInfo, boost::shared_ptr<GadgetModelInfo> model);
    virtual ~ContainerBehavior();

    /* 0x04 */ boost::weak_ptr<GadgetModelInfo> m_model;
    /* 0x0C */ boost::shared_ptr<TBlockManage::forBlockCheck> m_block;
    /* 0x14 */ boost::shared_ptr<GadgetCharCollision> m_handle;
    /* 0x1C */ ContainerBehaviorMotion m_motion;
    /* 0x3C */ u32 m_flags;  /* bit 0: landed */
    /* 0x40 */ bool m_falling; /* still looking for the ground */
};
SIZE_ASSERT(ContainerBehavior, 0x44);

extern "C" {
/* Starts the behavior at position and rotation, and advances it (behaviours inferred). */
void fn_800F8FC4(ContainerBehavior *behavior, const Vec3f *position,
                 const Vec3f *rotation);
void fn_800F9000(ContainerBehavior *behavior);

/* Moves the behavior to position and rotation, and copies its position out: inline helpers of the
 * original whose weak copies (0x800EF474, 0x800EF448) other units call by these names (behaviour
 * from their bodies). */
inline void fn_800EF474(ContainerBehavior *behavior, const Vec3f *position,
                        const Vec3f *rotation) {
    AssignVector(behavior->m_motion.m_position, *position);
    behavior->m_motion.m_rotation.x = rotation->x;
    behavior->m_motion.m_rotation.y = rotation->y;
    behavior->m_motion.m_rotation.z = rotation->z;
}

inline void fn_800EF448(ContainerBehavior *behavior, Vec3f *position) {
    AssignVector(*position, behavior->m_motion.m_position);
}
}

#endif
