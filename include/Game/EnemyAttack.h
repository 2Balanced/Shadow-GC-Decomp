#ifndef GAME_ENEMYATTACK_H
#define GAME_ENEMYATTACK_H

#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class CharInfo;

/* EnemyAttack (class name from the RTTI of its bound callback, Game/EnemyAttack.cpp): an attack
 * collider an enemy keeps for one kind of attack at a time (BkChaos keeps one at 0x2A8). Set makes
 * the collider and its attack collision when the kind changes and attaches it to a character info;
 * Update, once per frame, takes it down again once it was not set since the last call. Member
 * names are inferred. */
class EnemyAttack {
  public:
    /* What Set is passed (struct name unknown). */
    struct Desc {
        /* 0x00 */ s32 m_type;   /* the kind of attack; a new kind remakes the collider */
        /* 0x04 */ s32 m_damage; /* passed to the attack collision */
        /* 0x08 */ u32 field_0x8; /* the first two words of the collider's shape */
        /* 0x0C */ u32 field_0xC;
        /* 0x10 */ boost::weak_ptr<CharInfo> m_owner;
    };

    EnemyAttack();
    ~EnemyAttack();

    void Set(const Desc &desc, CharInfo *info); /* 0x802DC764 */
    void Update();                              /* 0x802DC724 */
    void Reset();                               /* 0x802DC6FC */
    void Create(const Desc &desc);              /* 0x802DC324 */
    void Release();                             /* 0x802DC000 */
    void SetCharInfo(CharInfo *info);           /* 0x802DBFC8 */

    /* The attack collision's callbacks. */
    void OnHit(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    void OnHitEnd(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);

    /* 0x00 */ boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> m_collider;
    /* 0x08 */ boost::weak_ptr<SonicteamUSA::System::CharaColli::CharaColliAttack> m_attack;
    /* 0x10 */ s32 m_type; /* -1 when there is no collider */
    /* 0x14 */ u8 m_set;   /* Set was called since the last Update */
};
SIZE_ASSERT(EnemyAttack, 0x18);

#endif
