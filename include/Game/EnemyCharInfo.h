#ifndef GAME_ENEMYCHARINFO_H
#define GAME_ENEMYCHARINFO_H

#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* EnemyCharInfo (class name from RTTI; vtable at 0x8053A8B4): the character info of an enemy.
 * Layout from its constructor (0x801A0280) and destructor (0x801A0218); member names other than
 * the command receiver are inferred from EggLastMech, which derives from it. */
class EnemyCharInfo : public CharInfo {
  public:
    /* type is the character type, passed on to CharInfo's constructor. */
    EnemyCharInfo(int type);
    virtual ~EnemyCharInfo();
    /* 0x8019FF24 */
    virtual void PushCommand(CharCommand *command);
    /* 0x8019FD5C (meaning unknown) */
    virtual bool vf_0x2C();

    /* 0x64 */ boost::shared_ptr<CharCommandUnit> m_commandUnit;
    /* 0x6C */ Vec3f m_prevPosition; /* the position before the frame's move (inferred) */
    /* 0x78 */ float field_0x78[(0x9C - 0x78) / 4];
    /* 0x9C */ Vec3f field_0x9C;
    /* 0xA8 */ float field_0xA8;
    /* 0xAC */ float field_0xAC; /* 700 for EggLastMech */
    /* 0xB0 */ float field_0xB0;
    /* 0xB4 */ float field_0xB4; /* 2 for EggLastMech */
    /* 0xB8 */ float field_0xB8;
};
SIZE_ASSERT(EnemyCharInfo, 0xBC);

extern "C" {
/* The enemy's frame time (behaviour inferred: EggLastMech counts timers down by it). */
float fn_8019FFE0(EnemyCharInfo *info);
/* Sets the callback that receives the commands pushed on the enemy (behaviour inferred). */
void fn_8019FD64(EnemyCharInfo *info, const CharCommandCallback &callback);
}

#endif
