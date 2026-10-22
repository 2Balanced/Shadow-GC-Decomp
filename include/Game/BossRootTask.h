#ifndef GAME_BOSSROOTTASK_H
#define GAME_BOSSROOTTASK_H

/* The bosses' task classes (class names from RTTI). Each boss (DevilDoom, BlackDoom, the
 * BlackBull, HeavyDog and EggMeca stages, ...) is a BossRootTask that owns a BossVarParam with its
 * state (see Game/BossCtrlUnit.h). Member names and meanings are inferred. */

#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class BossVarParam;
class UnkPlacement_800C9CD0;

/* BossRootTask (class name from RTTI; vtable at 0x8054DEDC, constructor 0x8027E028, destructor
 * 0x8027DFB4): the task of a boss gadget. Tick (0x8027DEB0) calls Init once the boss's data
 * is loaded, then follows the gadget owner until it goes away. */
class BossRootTask : public Task {
  public:
    BossRootTask(Task *parent, UnkPlacement_800C9CD0 *owner, bool flag, BossVarParam *param);
    virtual ~BossRootTask();
    virtual void Tick(float dt);
    /* Sets the boss up (slot 0x10, pure in BossRootTask). */
    virtual void Init() = 0;

    /* "BossRootTask", the task name the constructor stores (Game/BossRootTask.cpp). */
    static const char *s_taskName;

    /* 0x28 */ UnkPlacement_800C9CD0 *m_owner;
    /* 0x2C */ BossVarParam *m_param;
    /* 0x30 */ u8 m_started; /* Init has run */
    /* 0x31 */ bool m_flag;  /* the constructor's flag (meaning unknown) */
    /* 0x34 */ u32 field_0x34;
};
SIZE_ASSERT(BossRootTask, 0x38);

/* BossCtrl (class name from RTTI; vtable at 0x8052BE8C, constructor 0x80179B3C): a task that the
 * boss tasks create under themselves on their BossVarParam. It creates the boss's other tasks
 * (BossMotion, BossCharaColli, BossTarget, BossAfterCtrl, BossRender, BossForceClear) and runs the
 * boss's control units (Game/BossCtrl.cpp). */
class BossCtrl : public Task {
  public:
    BossCtrl(Task *parent, BossVarParam *param);
    virtual ~BossCtrl();
    virtual void Tick(float dt);

#ifdef BOSSCTRL_MEMBERS
    /* The members as Game/BossCtrl.cpp sees them: boost::scoped_ptr is a template the other
     * units must not see (it would renumber their @N locals). */
    class Impl;

    /* 0x28 */ boost::scoped_ptr<Impl> m_impl;
#else
    /* 0x28 */ void *field_0x28;
#endif
    /* 0x2C */ u32 field_0x2C;
};
SIZE_ASSERT(BossCtrl, 0x30);

/* BossCharInfo (class name from RTTI; vtable at 0x805473FC, constructor 0x80233158): the
 * character info of a boss. BossVarParam keeps it in a shared_ptr at 0x1C. */
class BossCharInfo : public CharInfo {
  public:
    /* type is the character type, passed on to CharInfo's constructor. */
    BossCharInfo(int type);
    virtual ~BossCharInfo();
    /* Gives command to the command unit, or deletes it when there is none (as GadgetCharInfo
     * does). Defined inline in Game/BlackBull0210.cpp, which emits the linked copy. */
    virtual void PushCommand(CharCommand *command);

    /* 0x64 */ boost::shared_ptr<CharCommandUnit> m_commandUnit;
};
SIZE_ASSERT(BossCharInfo, 0x6C);

#endif
