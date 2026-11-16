#ifndef GAME_ENEMYWALKER_H
#define GAME_ENEMYWALKER_H

/* EnemyWalker (class name from RTTI): the body of the walking enemies that move between up to
 * three points (EggPawn, Game/EggPawn.h; GunRobot, Game/GunRobot.h). EnemyWalker extends
 * EnemyWalkerCommon (Game/Enemy.h) with nine pure virtual functions; its code is in the Enemy
 * units (constructor 0x801AA774, destructor 0x801AA6F4). Their AI is EnemyWalkerAI
 * (Game/EnemyWalkerAI.h). Slot names follow Game/Enemy.h; their meanings are inferred from
 * GunRobot's and EggPawnBase's implementations, whose parameters are the stage editor's
 * Pos0/Pos1/Pos2 records. */

#include "Game/Enemy.h"
#include "Game/WeaponBullet.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* EnemyWalker (class name from RTTI; vtable 0x8053AFBC). */
class EnemyWalker : public EnemyWalkerCommon {
  public:
    EnemyWalker(int type);
    virtual ~EnemyWalker();
    /* The move speed ratio, action type and action time at point index (0 to 2). */
    virtual float vf_0x238(int index) = 0;
    virtual int vf_0x23C(int index) = 0;
    virtual float vf_0x240(int index) = 0;
    /* Point index's position (3: a random point near home). */
    virtual Fn8040CopyVector3 vf_0x244(int index) = 0;
    /* The range of the random moves. */
    virtual float vf_0x248() = 0;
    /* The motion to wait with at point index. */
    virtual int vf_0x24C(int index) = 0;
    /* How the enemy moves between the points (WaitActMoveType). */
    virtual int vf_0x250() = 0;
    virtual int vf_0x254() = 0;
    virtual int vfStat_0xB8() = 0;
};

#endif
