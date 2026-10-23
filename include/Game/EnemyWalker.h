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

/* The values of point index of a walking enemy's parameters Param: Pos0 (index 0, the starting
 * point), Pos1 and Pos2. A template over the parameter struct, so each enemy's unit has its own
 * copies, which retail calls out of line (as EggPawn's; names inferred). */
template <class Param>
struct EnemyWalkerPoints {
    static float GetActionTime(const Param &param, int index);
    static int GetActionType(const Param &param, int index);
    static float GetMoveSpeedRatio(const Param &param, int index);
};

template <class Param>
float EnemyWalkerPoints<Param>::GetActionTime(const Param &param, int index) {
    if (index == 1) {
        return param.Pos1_ActionTime;
    }
    if (index == 2) {
        return param.Pos2_ActionTime;
    }
    return param.Pos0_ActionTime;
}

template <class Param>
int EnemyWalkerPoints<Param>::GetActionType(const Param &param, int index) {
    if (index == 1) {
        return param.Pos1_ActionType;
    }
    if (index == 2) {
        return param.Pos2_ActionType;
    }
    return param.Pos0_ActionType;
}

template <class Param>
float EnemyWalkerPoints<Param>::GetMoveSpeedRatio(const Param &param, int index) {
    if (index == 1) {
        return param.Pos1_MoveSpeedRatio;
    }
    if (index == 2) {
        return param.Pos2_MoveSpeedRatio;
    }
    return param.Pos0_MoveSpeedRatio;
}

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
