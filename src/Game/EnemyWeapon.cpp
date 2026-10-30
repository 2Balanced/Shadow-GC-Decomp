/* EnemyWeapon (class name from RTTI; Game/EnemyWeapon.h): the weapon an enemy carries. It makes a
 * Weapon::WeaponBase by type, places it each frame and fires it the way its kind needs: guns,
 * cannons and lasers fire straight ahead, lock-on weapons lock on to the player when it is near
 * and in front. Names marked "(name inferred)" are not original. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#define ENEMY_WEAPON_FADE
#include "Game/Diablon.h"
#include "Game/EnemyWeapon.h"
#include "Game/CharInfo.h"
#include "Game/Player.h"
#include "Game/TGi.h"
#include "Game/WeaponBase.h"
#include "Game/WeaponCannonBase.h"
#include "Game/WeaponLockonBase.h"
#include "Game/WeaponLockonTarget.h"
#include "Game/WeaponStickBase.h"
#include "Game/WeaponVacuumBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

/* boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out of line; this unit
 * emits the copy for Weapon::WeaponBase (boost 1.32's definition). */
namespace boost {
template <> inline void shared_ptr<Weapon::WeaponBase>::reset() {
    this_type().swap(*this);
}
} // namespace boost

using Weapon::LockonTarget;
using Weapon::WeaponBase;
using Weapon::WeaponCannonBase;
using Weapon::WeaponGunBase;
using Weapon::WeaponLaserBase;
using Weapon::WeaponLockonBase;
using Weapon::WeaponStickBase;
using Weapon::WeaponVacuumBase;

class Target;

extern s32 lbl_805EE588;

extern "C" {
/* The player character of number id (see Game/AudioChannel.cpp). */
boost::weak_ptr<CharInfo> fn_800A8B74(UnkCharaList_80074194 *list, int id);
/* The target of a player (see Game/ArkCannon.cpp). */
boost::weak_ptr<Target> fn_80206574(Player::PlayerTargetBase *target);
/* Makes the weapon of type for owner. */
boost::shared_ptr<WeaponBase> fn_8005BBF4(const boost::weak_ptr<CharInfo> &owner, int type);
/* Starts an effect at position facing direction (see Game/BkWorm.cpp). */
void fn_80221BA8(const Vec3f *position, const Vec3f *direction, const Vec3f *velocity, int type,
                 int arg);
/* Vector and matrix helpers. */
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Vec3f fn_8040B618(const Mtx34f *matrix, const Vec3f *vector);
float fn_8040CE74(Vec3f *v);
float fn_8040CF90(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040F364(const Vec3f *forward, const Vec3f *up);
void fn_8040FAA0(Vec3f *translation, const Mtx34f *matrix);
void fn_800494F8(Vec3f *rotation, const Mtx34f *matrix);

void fn_801AB48C(EnemyWeapon *weapon, float dt);
void fn_801AB310(EnemyWeapon *weapon);
void fn_801AB244(EnemyWeapon *weapon);
void fn_801AADC0(EnemyWeapon *weapon);
void fn_801AAC90(EnemyWeapon *weapon);
}

/* fn_8040CD74 called as returning a - b by value (see Game/ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* A value (name inferred). As a call it is not a constant expression, so a static initialized by it
 * is set on first use, as in retail. */
inline float Value(float value) {
    return value;
}

EnemyWeapon::EnemyWeapon(const boost::weak_ptr<CharInfo> &owner, int type)
    : field_0x0(0), m_owner(owner), m_weapon(), m_rotation(0.0f, 0.0f, 0.0f), field_0x30(0),
      field_0x34(0.0f) {
    fn_801AB754(this, type);
    m_lastFrame = 0;
    m_fade.m_mode = 8;
}

EnemyWeapon::~EnemyWeapon() {
    if (m_weapon) {
        m_weapon.reset();
    }
}

void fn_801AB754(EnemyWeapon *weapon, int type) {
    if (weapon->m_weapon) {
        weapon->m_weapon.reset();
    }
    weapon->m_weapon = fn_8005BBF4(weapon->m_owner, type);
    if (weapon->m_weapon && weapon->m_weapon->m_model) {
        weapon->m_weapon->m_model->fn_8006310C(&weapon->m_fade);
    }
}

void fn_801AB710(EnemyWeapon *weapon, const Mtx34f *part) {
    if (part) {
        fn_8040FAA0(&weapon->m_position, part);
        fn_800494F8(&weapon->m_rotation, part);
    }
}

void fn_801AB650(EnemyWeapon *weapon, const Mtx34f *part) {
    if (part) {
        fn_8040FAA0(&weapon->m_position, part);
        Vec3f forward(0.0f, 0.0f, -1.0f);
        fn_800091D0(&forward, &(const Vec3f &)fn_8040B618(part, &forward));
        Vec3f up(0.0f, -1.0f, 0.0f);
        fn_800091D0(&up, &(const Vec3f &)fn_8040B618(part, &up));
        weapon->m_rotation = fn_8040F364(&forward, &up);
    }
}

void fn_801AB590(EnemyWeapon *weapon, const Mtx34f *part) {
    if (part) {
        fn_8040FAA0(&weapon->m_position, part);
        Vec3f forward(0.0f, 0.0f, -1.0f);
        fn_800091D0(&forward, &(const Vec3f &)fn_8040B618(part, &forward));
        Vec3f up(0.0f, 1.0f, 0.0f);
        fn_800091D0(&up, &(const Vec3f &)fn_8040B618(part, &up));
        weapon->m_rotation = fn_8040F364(&forward, &up);
    }
}

void EnemyWeapon::Tick(float dt) {
    if (m_weapon) {
        switch (m_weapon->Kind()) {
        case 4:
            fn_801AB48C(this, dt);
            break;
        default:
            m_weapon->SetTransform(&m_position, &m_rotation);
            m_weapon->Tick(dt);
            break;
        }
    }
}

/* Runs a stick weapon (name inferred). */
void fn_801AB48C(EnemyWeapon *weapon, float dt) {
    weapon->m_weapon->SetTransform(&weapon->m_position, &weapon->m_rotation);
    weapon->m_weapon->Tick(dt);
}

void fn_801AB3F0(EnemyWeapon *weapon) {
    if (weapon->m_weapon) {
        switch (weapon->m_weapon->Kind()) {
        case 1:
            fn_801AB310(weapon);
            break;
        case 2:
            fn_801AB244(weapon);
            break;
        case 3:
            fn_801AADC0(weapon);
            break;
        case 6:
            fn_801AAC90(weapon);
            break;
        }
    }
}

/* Fires a gun (name inferred). */
void fn_801AB310(EnemyWeapon *weapon) {
    if (weapon->m_lastFrame + 1 < lbl_8057D6A0.m_frame) {
        weapon->m_lastFrame = lbl_8057D6A0.m_frame;
        boost::shared_ptr<WeaponGunBase> gun = boost::dynamic_pointer_cast<WeaponGunBase>(weapon->m_weapon);
        if (gun) {
            Vec3f aim(0.0f, 0.0f, 0.0f);
            gun->Fire(&aim);
        }
    }
}

/* Fires a cannon (name inferred). */
void fn_801AB244(EnemyWeapon *weapon) {
    if (weapon->m_lastFrame + 1 < lbl_8057D6A0.m_frame) {
        weapon->m_lastFrame = lbl_8057D6A0.m_frame;
        boost::shared_ptr<WeaponCannonBase> cannon =
            boost::dynamic_pointer_cast<WeaponCannonBase>(weapon->m_weapon);
        if (cannon) {
            cannon->ReloadTime();
        }
    }
}

/* Fires a lock-on weapon: at the player when it is near and in front, else without a target (name
 * inferred). */
void fn_801AADC0(EnemyWeapon *weapon) {
    if (weapon->m_lastFrame + 1 < lbl_8057D6A0.m_frame) {
        weapon->m_lastFrame = lbl_8057D6A0.m_frame;
        boost::shared_ptr<WeaponLockonBase> lockon =
            boost::dynamic_pointer_cast<WeaponLockonBase>(weapon->m_weapon);
        if (lockon) {
            boost::weak_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588);
            if (!player.expired()) {
                boost::shared_ptr<CharInfo> chara = player.lock();
                Mtx34f matrix;
                fn_8040B6F8(&matrix);
                fn_8040B848(&matrix, weapon->m_rotation.y);
                fn_8040B884(&matrix, weapon->m_rotation.x);
                fn_8040B80C(&matrix, weapon->m_rotation.z);
                static const float s_rangeSq = Value(90000.0f);
                Vec3f front;
                fn_800091EC(&front, &(const Vec3f &)fn_8040B67C(&matrix, &Vec3f(0.0f, 0.0f, 1.0f)));
                Vec3f to;
                fn_800091EC(&to, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&chara->m_position,
                                                                        &weapon->m_position));
                float distance = fn_8040CF2C(&to);
                fn_8040CE74(&to);
                if (distance < s_rangeSq && fn_8040CF90(&to, &front) > 0.86603f) {
                    boost::shared_ptr<LockonTarget> target = boost::shared_ptr<LockonTarget>(new LockonTarget());
                    if (target) {
                        boost::shared_ptr<Player::PlayerBase> base =
                            boost::dynamic_pointer_cast<Player::PlayerBase>(chara);
                        if (base && base->m_target) {
                            target->SetTarget(fn_80206574(base->m_target));
                            target->fn_80060F70();
                        }
                    }
                    lockon->vf_0x30();
                    lockon->vf_0x34(target, 1);
                } else {
                    lockon->vf_0x34(boost::shared_ptr<LockonTarget>(), 1);
                }
            }
        }
    }
}

/* Fires a laser (name inferred). */
void fn_801AAC90(EnemyWeapon *weapon) {
    if (weapon->m_lastFrame + 1 < lbl_8057D6A0.m_frame) {
        weapon->m_lastFrame = lbl_8057D6A0.m_frame;
        boost::shared_ptr<WeaponLaserBase> laser = boost::dynamic_pointer_cast<WeaponLaserBase>(weapon->m_weapon);
        if (laser) {
            Vec3f aim(0.0f, 0.0f, 0.0f);
            laser->Fire(&aim);
        }
    }
}

bool fn_801AAC68(EnemyWeapon *weapon) {
    if (weapon->m_weapon && weapon->m_weapon->Kind() == 5) {
        return true;
    }
    return false;
}

bool fn_801AAC40(EnemyWeapon *weapon) {
    if (weapon->m_weapon && weapon->m_weapon->Kind() == 4) {
        return true;
    }
    return false;
}

void fn_801AABD4(EnemyWeapon *weapon) {
    boost::shared_ptr<WeaponStickBase> stick = boost::dynamic_pointer_cast<WeaponStickBase>(weapon->m_weapon);
    if (stick) {
        stick->SwingHeld();
    }
}

void fn_801AAB68(EnemyWeapon *weapon) {
    boost::shared_ptr<WeaponStickBase> stick = boost::dynamic_pointer_cast<WeaponStickBase>(weapon->m_weapon);
    if (stick) {
        stick->SwingReleased();
    }
}

bool fn_801AAA54(EnemyWeapon *weapon) {
    boost::shared_ptr<WeaponStickBase> stick = boost::dynamic_pointer_cast<WeaponStickBase>(weapon->m_weapon);
    if (stick) {
        return stick->fn_80067918();
    }
    return false;
}

void fn_801AA9E8(EnemyWeapon *weapon) {
    boost::shared_ptr<WeaponVacuumBase> vacuum =
        boost::dynamic_pointer_cast<WeaponVacuumBase>(weapon->m_weapon);
    if (vacuum) {
        vacuum->StartVacuum();
    }
}

void fn_801AA8EC(EnemyWeapon *weapon) {
    boost::shared_ptr<WeaponVacuumBase> vacuum =
        boost::dynamic_pointer_cast<WeaponVacuumBase>(weapon->m_weapon);
    if (vacuum) {
        vacuum->StopVacuum();
    }
}

void fn_801AA8E4(EnemyWeapon *weapon, float alpha) {
    weapon->m_fade.m_color[3] = alpha;
}

void fn_801AA8A0(EnemyWeapon *weapon, const Vec3f *velocity) {
    if (weapon->m_weapon) {
        fn_80221BA8(&weapon->m_position, &weapon->m_rotation, velocity, weapon->m_weapon->TypeId(), -1);
    }
}

void fn_801AA868(EnemyWeapon *weapon) {
    if (weapon->m_weapon) {
        weapon->m_weapon->Show();
    }
}

void fn_801AA830(EnemyWeapon *weapon) {
    if (weapon->m_weapon) {
        weapon->m_weapon->Hide();
    }
}
