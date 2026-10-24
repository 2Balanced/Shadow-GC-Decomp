#ifndef GAME_BULLETMANAGER_H
#define GAME_BULLETMANAGER_H

#include "size_assert.h"
#ifdef BULLET_MANAGER_MEMBERS
#include "boost/scoped_ptr.hpp"
#endif

namespace Weapon {
namespace Bullet {

class BulletBase;

/* The bullets in flight (class name inferred from Weapon::Bullet::BulletManagerTask, which has
 * RTTI; this class has none). One instance lives in a function-local static of fn_800522A8
 * (Game/BulletBase.cpp); every bullet adds itself when it is built and removes itself when it is
 * destroyed. It only holds its list, behind a pointer (Game/BulletManagerTask.cpp, which defines
 * the class's functions). Method names are inferred. */
class BulletManager {
  public:
    BulletManager();
    ~BulletManager();
    /* Adds and removes a bullet (0x80058B80 and 0x80058B5C). */
    void Add(BulletBase *bullet);
    void Remove(BulletBase *bullet);

    /* The instance, set by fn_800522A8 when it first builds it. */
    static BulletManager *s_instance;

#ifdef BULLET_MANAGER_MEMBERS
    /* The members, seen only by Game/BulletManagerTask.cpp: boost::scoped_ptr is a template the
     * other units must not see (see Game/GadgetCollision.h). */
    class Impl;

    /* 0x0 */ boost::scoped_ptr<Impl> m_impl;
#else
    /* 0x0 */ void *m_impl;
#endif
};
SIZE_ASSERT(BulletManager, 0x4);

} // namespace Bullet
} // namespace Weapon

#endif
