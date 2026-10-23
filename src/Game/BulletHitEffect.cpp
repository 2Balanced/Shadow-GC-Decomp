/* The hit effect of bullets (file name inferred; the game does not name this code): when a bullet's
 * move hit a surface, fn_802B3FE8 starts the effect for the bullet kind at the hit position, facing
 * the surface normal. Kind 1 picks the effect from the surface's material; surfaces flagged 0x20000
 * get none. Called by Game/BulletLaser.cpp and by 0x80055DD8. Names marked "(name inferred)" are
 * not original. */
#include "Effect/System/EffectElement.h"
#include "Game/WeaponBullet.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

using Weapon::Bullet::HitResult;

extern "C" {
/* Starts effect id at position, facing direction, for owner (behaviour inferred; see
 * BulletLaser.cpp). */
boost::weak_ptr<Effect::System::EffectElement> fn_8005E32C(const Vec3f *position,
                                                           const Vec3f *direction, int id,
                                                           const boost::weak_ptr<CharInfo> &owner);
}

/* An empty character (used as "no owner"; see GunBattery.cpp). */
extern boost::shared_ptr<CharInfo> lbl_805EF980;

/* The effect of a kind-1 hit for each surface material (-1: none). */
static const s32 lbl_804FDD38[16] = {
    -1, 0x197, -1, -1, -1, 0x198, -1, -1, -1, 0x199, -1, -1, -1, -1, -1, -1,
};

/* Starts the hit effect of bullet kind kind at hit (name unknown). */
extern "C" void fn_802B3FE8(int kind, const HitResult *hit) {
    if (hit->m_hit && hit->m_struck && hit->field_0x2C != 0 && !(hit->field_0x2C & 0x20000)) {
        int id = -1;
        switch (kind) {
        case 1:
            id = lbl_804FDD38[hit->field_0x30];
            break;
        case 2:
            id = 0x199;
            break;
        case 3:
            id = 0x257;
            break;
        case 4:
            id = 0x25B;
            break;
        case 5:
            id = 0x25C;
            break;
        }
        if (id >= 0) {
            fn_8005E32C((const Vec3f *)&hit->m_position, (const Vec3f *)&hit->m_normal, id,
                        boost::weak_ptr<CharInfo>(lbl_805EF980));
        }
    }
}
