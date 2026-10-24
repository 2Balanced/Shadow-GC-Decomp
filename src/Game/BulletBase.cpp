/* BulletBase (class name from RTTI: Weapon::Bullet::BulletBase): the base of every bullet. A bullet
 * registers itself with the bullet manager for as long as it exists. This unit also holds the
 * manager's instance (fn_800522A8, which other units call by that name) and the out-of-line
 * functions of BulletPosition that it is the first to use. File name inferred from the class
 * name. Method and member names are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include "Game/BulletManager.h"
#include "Game/WeaponBullet.h"
#include "boost/weak_ptr.hpp"

using Weapon::ShotInfo;
using Weapon::Bullet::BulletBase;
using Weapon::Bullet::BulletManager;
using Weapon::Bullet::BulletPosition;

/* The rotation of a bullet copied as one block, as the original's implicit member-wise
 * assignment does (struct name inferred). */
struct UnkRotationBlock {
    Vec3f value;
};

namespace Weapon {
namespace Bullet {

BulletPosition &BulletPosition::operator=(const BulletPosition &other) {
    fn_800091D0(&m_position, &other.m_position);
    fn_800091D0(&m_direction, &other.m_direction);
    reinterpret_cast<UnkRotationBlock &>(m_rotation) =
        reinterpret_cast<const UnkRotationBlock &>(other.m_rotation);
    return *this;
}

} // namespace Bullet
} // namespace Weapon

/* The bullet manager, created on first use (name unknown; other units call it by its address). */
extern "C" BulletManager *fn_800522A8(void);

namespace Weapon {
namespace Bullet {

BulletBase::BulletBase(const BulletBase &other) {
    fn_800522A8()->Add(this);
    m_state = other.m_state;
    m_side = other.m_side;
    m_attackKind = other.m_attackKind;
    m_owner = other.m_owner;
    m_distance = 0.0f;
}

BulletPosition::BulletPosition() : m_rotation(0.0f, 0.0f, 0.0f) {}

BulletBase::BulletBase(const ShotInfo &info) : m_side(info.m_side), m_owner(info.m_owner) {
    fn_800522A8()->Add(this);
    m_attackKind = m_side == 0 ? 3 : 6;
    m_distance = 0.0f;
}

} // namespace Bullet
} // namespace Weapon

extern "C" BulletManager *fn_800522A8(void) {
    if (BulletManager::s_instance == 0) {
        static BulletManager s_manager;
        BulletManager::s_instance = &s_manager;
    }
    return BulletManager::s_instance;
}

BulletManager *BulletManager::s_instance;

namespace Weapon {
namespace Bullet {

BulletBase::~BulletBase() {
    fn_800522A8()->Remove(this);
}

} // namespace Bullet
} // namespace Weapon
