#ifndef GAME_ENEMYWEAPON_H
#define GAME_ENEMYWEAPON_H

/* EnemyWeapon (class name from RTTI; vtable 0x8053B218 at 0x64; Game/EnemyWeapon.cpp): the weapon
 * an enemy carries, a Weapon::WeaponBase made by type and driven by the enemy (the soldiers, the
 * EggPawns, the GunRobots and the GunBeetles). The enemy units call its functions by their address
 * names; the names and meanings given here are inferred. */

#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class CharInfo;
namespace Weapon {
class WeaponBase;
}

/* A clump's colour fade (Game/Diablon.h); EnemyWeapon keeps its own and hands it to the weapon's
 * model. */
struct UnkClumpFade_80027D5C;

class EnemyWeapon {
  public:
    EnemyWeapon(const boost::weak_ptr<CharInfo> &owner, int type);

    /* 0x00 */ u8 field_0x0;
    /* 0x04 */ boost::weak_ptr<CharInfo> m_owner;
    /* 0x0C */ boost::shared_ptr<Weapon::WeaponBase> m_weapon;
    /* 0x14 */ Vec3f m_position;
    /* 0x20 */ Vec3f m_rotation;
    /* 0x2C */ u32 m_lastFrame; /* the frame the weapon last fired in */
    /* 0x30 */ s32 field_0x30;
    /* 0x34 */ float field_0x34;
#ifdef ENEMY_WEAPON_FADE
    /* 0x38 */ UnkClumpFade_80027D5C m_fade;
#else
    /* 0x38 */ u8 m_fade[0x64 - 0x38];
#endif

    virtual ~EnemyWeapon();
    /* Places the weapon and runs it (name inferred). */
    virtual void Tick(float dt);
};
SIZE_ASSERT(EnemyWeapon, 0x68);

extern "C" {
/* Hide and show the weapon. */
void fn_801AA830(EnemyWeapon *weapon);
void fn_801AA868(EnemyWeapon *weapon);
/* Starts a muzzle effect at the weapon with velocity (behaviour inferred). */
void fn_801AA8A0(EnemyWeapon *weapon, const Vec3f *velocity);
/* Sets the alpha of the weapon's fade. */
void fn_801AA8E4(EnemyWeapon *weapon, float alpha);
/* A vacuum weapon's start and stop. */
void fn_801AA8EC(EnemyWeapon *weapon);
void fn_801AA9E8(EnemyWeapon *weapon);
/* A stick weapon's state test, swing release and swing start. */
bool fn_801AAA54(EnemyWeapon *weapon);
void fn_801AAB68(EnemyWeapon *weapon);
void fn_801AABD4(EnemyWeapon *weapon);
/* Whether the weapon is a stick (kind 4) or a vacuum (kind 5). */
bool fn_801AAC40(EnemyWeapon *weapon);
bool fn_801AAC68(EnemyWeapon *weapon);
/* Fires the weapon, at most once a frame. */
void fn_801AB3F0(EnemyWeapon *weapon);
/* Places the weapon at a model part's matrix: two ways of reading the part's axes, and its
 * rotation. */
void fn_801AB590(EnemyWeapon *weapon, const Mtx34f *part);
void fn_801AB650(EnemyWeapon *weapon, const Mtx34f *part);
void fn_801AB710(EnemyWeapon *weapon, const Mtx34f *part);
/* Makes the weapon of type. */
void fn_801AB754(EnemyWeapon *weapon, int type);
}

#endif
