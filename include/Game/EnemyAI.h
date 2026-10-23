#ifndef GAME_ENEMYAI_H
#define GAME_ENEMYAI_H

/* The enemies' AI (class names from RTTI): EnemyBaseAI implements EnemyCtrlFunc (see
 * Game/Enemy.h) and runs a state machine of TEnemyAIState objects (EnemyAIState_<AI>_<State>,
 * code from 0x8017EDDC). SoldierCommonAI is the soldiers' AI, BkSoldierAI the Black Arms
 * soldier's. EnemyBaseAI's code is at 0x8019CB4C..0x8019EB08. Slot names follow Game/Enemy.h. */

#include "Game/Enemy.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* EnemyBaseAI (class name from RTTI; vtable 0x8053A5D0). Its data starts after the pointer to
 * the EnemyReferer at 0x1C. */
class EnemyBaseAI : public virtual EnemyReferer, public EnemyCtrlFunc {
  public:
    EnemyBaseAI();

    virtual void vfCtrl_0x08();  /* 0x8019EB08 */
    virtual void vfCtrl_0x0C();  /* 0x8019E710 */
    virtual void *vfCtrl_0x10(); /* 0x8019DBC0 */
    virtual void *vfCtrl_0x14(); /* 0x8019DAF0 */
    virtual float vfCtrl_0x18(); /* 0x8019DA0C */
    virtual void vfCtrl_0x1C();  /* 0x8019D660 */
    virtual void vfCtrl_0x20();  /* 0x8017B50C */
    virtual ~EnemyBaseAI() {}    /* weak, first at 0x80183614 */
    virtual void vfAI_0x28();    /* 0x8019DC9C */
    virtual void vfAI_0x2C();    /* 0x8019E798 */
    virtual void vfAI_0x30();    /* 0x8019E794 */
    virtual void vfAI_0x34();    /* 0x8019D504 */
    virtual void vfAI_0x38();    /* 0x8019D4D8 */
    virtual void vfAI_0x3C();    /* 0x8019D428 */
    virtual void vfAI_0x40();    /* 0x8019D378 */
    virtual void vfAI_0x44();    /* 0x8019D278 */
    virtual void vfAI_0x48();    /* 0x8019D1C8 */
    virtual void vfAI_0x4C();    /* 0x8019D0C8 */
    virtual void vfAI_0x50();    /* 0x8019CF6C */
    virtual void vfAI_0x54();    /* 0x8019CE10 */
    virtual void vfAI_0x58();    /* 0x8019CCB4 */
    virtual void vfAI_0x5C();    /* 0x8019CCB0 */
    virtual void vfAI_0x60();    /* 0x8019CB4C */

    /* 0x20 */ u8 field_0x20[0x74 - 0x20];
};

/* SoldierCommonAI (class name from RTTI; vtable 0x8052DC38): the soldiers' AI. */
class SoldierCommonAI : public EnemyBaseAI {
  public:
    SoldierCommonAI();
    virtual ~SoldierCommonAI() {} /* weak, first at 0x8017EE3C */

    /* 0x74 */ u8 field_0x74[0x7C - 0x74];
};

/* BkSoldierAI (class name from RTTI; vtable 0x8052DBD4; constructor 0x80180698): the Black Arms
 * soldier's AI. */
class BkSoldierAI : public SoldierCommonAI {
  public:
    BkSoldierAI();
    virtual ~BkSoldierAI() {} /* weak, first at 0x8017EDE4 */
    virtual void vfAI_0x3C(); /* 0x801803BC */
    virtual void vfAI_0x44(); /* 0x80180384 */

    /* 0x7C */ u8 field_0x7C[0x84 - 0x7C];
};

#endif
