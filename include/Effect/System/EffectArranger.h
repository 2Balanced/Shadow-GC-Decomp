#ifndef EFFECT_SYSTEM_EFFECTARRANGER_H
#define EFFECT_SYSTEM_EFFECTARRANGER_H

/* Effect::System::EffectArranger (class name from RTTI): the element that runs a list of
 * commands (Effect/System/EffectCommandExec.h) on its own transform, which launch child effects,
 * move, turn and scale them, wait and loop. Its members are seen only by
 * Game/EffectArranger.cpp, which defines EFFECT_ELEMENT_FULL; names other than the class's are
 * inferred. */
#include "Effect/System/EffectElement.h"

namespace Effect {
namespace System {

#ifdef EFFECT_ELEMENT_FULL
class EffectCommandExec;
class UnkEffectPool_8042A344;
#endif

class EffectArranger : public EffectElement {
  public:
#ifdef EFFECT_ELEMENT_FULL
    EffectArranger(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,
                   EffectManager *manager);
#endif
    virtual ~EffectArranger();
    virtual void Update(float dt);
    /* Draws nothing (0x800273E4). */
    virtual void Draw() {}

#ifdef EFFECT_ELEMENT_FULL
    /* Turns commands into executors until a wait (0x80426700). */
    void ParseCommands();

    /* 0x0FC */ u32 m_state;  /* bit 0: out of executor memory */
    /* 0x100 */ u32 m_config; /* the description's flags; bit 0: never times out */
    /* 0x104 */ EffectLaunchInfo *m_launchInfo; /* what child effects are launched with */
    /* 0x108 */ const u8 *m_cursor; /* the next command */
    /* 0x10C */ EffectCommandExec *m_head;
    /* 0x110 */ EffectCommandExec *m_tail;
    /* 0x114 */ UnkEffectPool_8042A344 *m_commandPool;
    /* 0x118 */ UnkEffectPool_8042A344 *m_launchPool; /* for the launch commands */
#endif
};

} // namespace System
} // namespace Effect

#endif
