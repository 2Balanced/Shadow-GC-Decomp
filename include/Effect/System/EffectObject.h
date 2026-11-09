#ifndef EFFECT_SYSTEM_EFFECTOBJECT_H
#define EFFECT_SYSTEM_EFFECTOBJECT_H

/* Effect::System::EffectObject (class name and base from RTTI): the portable part of the model
 * effect. It runs the commands of its description (Effect/System/EffectCommandExec.h) on the
 * element's offset, angles, scale, offset speed and colour, in a command pool of its own
 * (Game/EffectObject.cpp, which defines EFFECT_ELEMENT_FULL). Drawing is left to the RenderWare
 * class (Effect::RW3::EffectObjectRW3, Game/EffectManagerPJS.h). Names other than the class's
 * are inferred. */
#include "Effect/System/EffectElement.h"

namespace Effect {
namespace System {

class EffectCommandExec;
class UnkEffectPool_8042A344;

class EffectObject : public EffectElement {
  public:
#ifdef EFFECT_ELEMENT_FULL
    /* 0x80421D38. */
    EffectObject(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,
                 EffectManager *manager);
#endif
    virtual ~EffectObject();
    virtual void Update(float dt);

#ifdef EFFECT_ELEMENT_FULL
    /* Turns commands into executors until a wait (0x80421834). */
    void ParseCommands();
#else
    /* 0x054 */ u8 field_0x54[0xFC - 0x54];
#endif
    /* 0x0FC */ u32 m_state;  /* bit 0: out of executor memory, or a command ended the effect */
    /* 0x100 */ u32 m_config; /* the description's flags; bit 0: never times out, bit 18: keeps
                               * running after a stop while executors are left */
    /* 0x104 */ u32 m_color[4]; /* red, green, blue, alpha: 8 bits each, from bit 22 */
    /* 0x114 */ const u8 *m_cursor; /* the next command */
    /* 0x118 */ EffectCommandExec *m_head;
    /* 0x11C */ EffectCommandExec *m_tail;
    /* 0x120 */ UnkEffectPool_8042A344 *m_commandPool;
};

} // namespace System
} // namespace Effect

#endif
