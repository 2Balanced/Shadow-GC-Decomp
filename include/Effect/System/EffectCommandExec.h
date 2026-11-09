#ifndef EFFECT_SYSTEM_EFFECTCOMMANDEXEC_H
#define EFFECT_SYSTEM_EFFECTCOMMANDEXEC_H

/* The effect library's command executors (class names and bases from RTTI): an element reads its
 * description's command list and makes one executor per command, which then runs once per update
 * until Exec returns false. Executors live in the effect system's current pool. The classes are
 * implemented in the unit before Game/EffectManager.cpp; only what the element units use is
 * declared here (the constructors' parameter types and all member names are inferred). */
#include "Effect/System/EffectSystem.h"
#include "Game/MathTypes.h"
#include <dolphin/types.h>

namespace Effect {
namespace System {

class EffectElement;
class EffectLaunchInfo;
class EffectManager;

class EffectCommandExec {
  public:
    EffectCommandExec(const u8 *data) : m_data(data), m_prev(0), m_next(0), field_0x10(0) {}
    /* 0x80421E98. */
    virtual ~EffectCommandExec();
    /* Runs the command for dt; false once it is done. */
    virtual bool Exec(float dt) = 0;

    /* The executors live in the current pool (EffectSystem::SetPool). */
    static void *operator new(unsigned long size) {
        return fn_8042A5D0()->PoolAlloc();
    }
    static void operator delete(void *p) {
        fn_8042A5D0()->PoolFree(p);
    }

    /* 0x00 vtable */
    /* 0x04 */ const u8 *m_data; /* the command; its first byte is the opcode */
    /* 0x08 */ EffectCommandExec *m_prev;
    /* 0x0C */ EffectCommandExec *m_next;
    /* 0x10 */ u8 field_0x10;
};

/* Moves a vector of the element (0x804245CC). */
class EffCmdVector3Exec : public EffectCommandExec {
  public:
    EffCmdVector3Exec(const u8 *data, u32 *state, Vec3f *target);
    virtual ~EffCmdVector3Exec();
    virtual bool Exec(float dt);
};

class EffCmd3DPosExec : public EffCmdVector3Exec {
  public:
    EffCmd3DPosExec(const u8 *data, u32 *state, Vec3f *target)
        : EffCmdVector3Exec(data, state, target) {}
    virtual ~EffCmd3DPosExec();
};

class EffCmd3DRotExec : public EffCmdVector3Exec {
  public:
    EffCmd3DRotExec(const u8 *data, u32 *state, Vec3f *target)
        : EffCmdVector3Exec(data, state, target) {}
    virtual ~EffCmd3DRotExec();
};

class EffCmd3DVelExec : public EffCmdVector3Exec {
  public:
    EffCmd3DVelExec(const u8 *data, u32 *state, Vec3f *target)
        : EffCmdVector3Exec(data, state, target) {}
    virtual ~EffCmd3DVelExec();
};

/* 0x80423CF8. */
class EffCmd3DSclExec : public EffectCommandExec {
  public:
    EffCmd3DSclExec(const u8 *data, u32 *state, Vec3f *target);
    virtual ~EffCmd3DSclExec();
    virtual bool Exec(float dt);
};

/* Holds the command list back until its time has passed (0x80422E34). */
class EffCmdWaitExec : public EffectCommandExec {
  public:
    EffCmdWaitExec(const u8 *data, u32 *state, EffectCommandExec *previous);
    virtual ~EffCmdWaitExec();
    virtual bool Exec(float dt);
};

/* 0x80422D30. */
class EffCmdLoopStartExec : public EffectCommandExec {
  public:
    EffCmdLoopStartExec(const u8 *data, u32 *state);
    virtual ~EffCmdLoopStartExec();
    virtual bool Exec(float dt);
};

/* Sets *jump to the command to continue at, if it loops back (0x80422BA8). */
class EffCmdLoopEndExec : public EffectCommandExec {
  public:
    EffCmdLoopEndExec(const u8 *data, u32 *state, EffectCommandExec *previous,
                      const u8 **jump);
    virtual ~EffCmdLoopEndExec();
    virtual bool Exec(float dt);
};

/* 0x80422B40. */
class EffCmdCallbackExec : public EffectCommandExec {
  public:
    EffCmdCallbackExec(const u8 *data, u32 *state);
    virtual ~EffCmdCallbackExec();
    virtual bool Exec(float dt);
};

/* Launches a child effect (0x80422750). */
class EffCmdLaunchExec : public EffectCommandExec {
  public:
    EffCmdLaunchExec(const u8 *data, u32 *state, EffectManager *manager, EffectLaunchInfo *info,
                     EffectElement *parent);
    virtual ~EffCmdLaunchExec();
    virtual bool Exec(float dt);
};

/* 0x80422254. */
class EffCmdRequestStopExec : public EffectCommandExec {
  public:
    EffCmdRequestStopExec(const u8 *data, u32 *state, EffectElement *element);
    virtual ~EffCmdRequestStopExec();
    virtual bool Exec(float dt);
};

/* Moves one float of the element (0x80424B80). */
class EffCmdSingleExec : public EffectCommandExec {
  public:
    EffCmdSingleExec(const u8 *data, u32 *state, float *target);
    virtual ~EffCmdSingleExec();
    virtual bool Exec(float dt);
};

/* Moves two floats of the element (0x804248D4). */
class EffCmdVector2Exec : public EffectCommandExec {
  public:
    EffCmdVector2Exec(const u8 *data, u32 *state, Vec2f *target);
    virtual ~EffCmdVector2Exec();
    virtual bool Exec(float dt);
};

/* Fades a packed alpha of the element (0x80423000). */
class EffCmdMatAlphaExec : public EffectCommandExec {
  public:
    EffCmdMatAlphaExec(const u8 *data, u32 *state, u32 *target);
    virtual ~EffCmdMatAlphaExec();
    virtual bool Exec(float dt);
};

} // namespace System
} // namespace Effect

#endif