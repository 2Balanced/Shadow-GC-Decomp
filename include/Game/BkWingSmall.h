#ifndef GAME_BKWINGSMALL_H
#define GAME_BKWINGSMALL_H

/* BkWingSmall (class name from RTTI): the small Black Arms wing, BK_WINGSMALL in the stage editor
 * (Game/BkWingSmall.cpp). Its body is BkWingSmallBase, a FlyerCommonBase (Game/EnemyFlyer.h), and
 * its AI BkWingSmallAI; EnemyTemplate<BkWingSmallBase, BkWingSmallAI> (Game/Enemy.h) puts them
 * together. Member names are inferred unless they repeat a parameter name of the stage editor. */

#include "Game/EnemyFlyer.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* The small wing's parameters (struct name unknown; its constructor is inline, weak copy
 * 0x80191BD0). */
struct BkWingSmallParam : public FlyerParam {
    BkWingSmallParam() {
        Reset();
    }
};
SIZE_ASSERT(BkWingSmallParam, 0x38);

/* BkWingSmallBase (class name from RTTI; vtable 0x80534694): the small wing's body. */
class BkWingSmallBase : public FlyerCommonBase {
  public:
    BkWingSmallBase();
    virtual ~BkWingSmallBase();
    /* Whether the wing appears on its path, and whether it attacks on it (behaviour inferred). */
    virtual bool vf_0x238() {
        return m_params.AppearType == 1;
    }
    virtual bool vf_0x23C();
    virtual void vf_0x244();
    virtual void vf_0x248();
    virtual bool vfDisp_0x44(int *command);
    virtual void vfSet_0x1C(UnkPlacement_800C9CD0 *placement);
    virtual void vfSet_0x20(UnkPlacement_800C9CD0 *placement);
    virtual int vfStat_0xB8();
    virtual void vfDisp_0x64();
    virtual void vfDisp_0x10(float dt);
    virtual void vfDisp_0x50();
    virtual void vfDisp_0x48();
    virtual int vfPara_0x48();
    virtual int vfPara_0x4C();
    virtual int vfPara_0x50();
    virtual int vfPara_0x54();
    virtual int vfPara_0x58();
    virtual int vfPara_0x5C();
    virtual int vfPara_0x60();

    /* The flying speed (the AI scales its steps by it; name inferred). */
    float GetSpeed();

    /* 0x310 */ BkWingSmallParam m_params;
    /* 0x348 */ float field_0x348;
};

/* BkWingSmallAI (class name from RTTI; vtable 0x8052E68C; constructor 0x8018235C): the small
 * wing's AI. */
class BkWingSmallAI : public FlyerCommonAI {
  public:
    BkWingSmallAI();
    virtual ~BkWingSmallAI() {} /* weak, first at 0x8018192C */
    virtual void vfAI_0x3C();   /* 0x8018221C */
    virtual void vfAI_0x44();   /* 0x801821E4 */

    /* 0x8C */ BkWingSmallBase *m_wing; /* the body (name inferred) */
    /* 0x90 */ Vec3f m_direction; /* from the wing to its target point (name inferred) */
};

/* BkWingSmall (class name from RTTI; vtable 0x80534094). */
class BkWingSmall : public EnemyTemplate<BkWingSmallBase, BkWingSmallAI> {
  public:
    BkWingSmall(Task *parent, UnkPlacement_800C9CD0 *placement);
    virtual ~BkWingSmall();

    /* The stage editor's hooks (names inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *placement);
    static void LoadFiles();
    static void DropFiles();

    static const char *s_taskName;
};
SIZE_ASSERT(BkWingSmall, 0x420);

#endif
