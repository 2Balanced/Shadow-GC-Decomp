#ifndef EFFECT_SYSTEM_EFFECTSHIMMER_H
#define EFFECT_SYSTEM_EFFECTSHIMMER_H

/* Effect::System::EffectShimmer (class name from RTTI): the heat-shimmer element. The portable
 * class keeps the wave's parameters, which its command list (amplitude, cycle and wave length
 * commands, in one and two dimensions) moves, and the RenderWare class (Game/EffectManagerPJS.h)
 * draws it. Its members are seen only by Game/EffectShimmer.cpp, which defines
 * EFFECT_ELEMENT_FULL; names other than the class's are inferred. */
#include "Effect/System/EffectElement.h"

namespace Effect {
namespace System {

#ifdef EFFECT_ELEMENT_FULL
class EffectCommandExec;
class UnkEffectPool_8042A344;
#endif

class EffectShimmer : public EffectElement {
  public:
#ifdef EFFECT_ELEMENT_FULL
    EffectShimmer(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,
                  EffectManager *manager);
#endif
    virtual ~EffectShimmer();
    virtual void Update(float dt);

#ifdef EFFECT_ELEMENT_FULL
    /* Turns commands into executors until a wait (0x8042AAB0). */
    void ParseCommands();

    /* 0x0FC */ u32 m_state;  /* bit 0: out of executor memory */
    /* 0x100 */ u32 m_config; /* the description's flags; bit 0: never times out */
    /* 0x104 */ float m_duration;
    /* 0x108 */ s32 m_counts[2]; /* at least 1 each */
    /* 0x110 */ Vec2f field_0x110;
    /* 0x118 */ Vec2f m_amplitude;
    /* 0x120 */ u8 field_0x120[0x124 - 0x120];
    /* 0x124 */ Vec2f m_cycle;
    /* 0x12C */ u8 field_0x12C[0x130 - 0x12C];
    /* 0x130 */ Vec2f m_waveLength;
    /* 0x138 */ u8 field_0x138[0x13C - 0x138];
    /* 0x13C */ u32 field_0x13C;
    /* 0x140 */ u32 m_alpha;  /* 8 bits, from bit 10 */
    /* 0x144 */ const void *m_params;
    /* 0x148 */ const u8 *m_cursor; /* the next command */
    /* 0x14C */ EffectCommandExec *m_head;
    /* 0x150 */ EffectCommandExec *m_tail;
    /* 0x154 */ UnkEffectPool_8042A344 *m_pool;
#endif
};

} // namespace System
} // namespace Effect

#endif
