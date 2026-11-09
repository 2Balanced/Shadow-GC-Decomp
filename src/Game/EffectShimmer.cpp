/* Effect::System::EffectShimmer (class name from RTTI): the portable part of the heat-shimmer
 * element. It copies the wave's parameters from its description and runs a command list whose
 * amplitude, cycle and wave length commands move them (the six command classes are this unit's;
 * their constructors are inline and their destructors and vtables come out here). File name from
 * the class; member names are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first).
 *
 * NonMatching: 9 of 10 functions match. The constructor (94%) has the description loads of
 * Game/EffectGlobalParam.cpp's scheduled differently. The unit also emits out-of-line copies of the
 * executors' and the pool's inline operator delete for its exception tables, which retail does not
 * have here. */
#define EFFECT_ELEMENT_POLYMORPHIC
#define EFFECT_ELEMENT_FULL
#define EFFECT_SYSTEM_FULL

#include "Effect/System/EffectCommandExec.h"
#include "Effect/System/EffectDesc.h"
#include "Effect/System/EffectPool.h"
#include "Effect/System/EffectShimmer.h"

/* A shimmer element's description (layout from the constructor). */
struct UnkShimmerDesc_8042AF58 {
    /* 0x00 */ u8 field_0x0[0x2C];
    /* 0x2C */ u32 m_config;
    /* 0x30 */ u8 m_alpha;
    /* 0x31 */ u8 field_0x31;
    /* 0x32 */ s16 m_commandCount;
    /* 0x34 */ float m_duration;
    /* 0x38 */ const void *m_params;
    /* 0x3C */ const u8 *m_commands;
};

/* The wave parameters a description points to, in its two layouts (from the constructor). */
struct UnkShimmerParams0_8042AF58 {
    /* 0x00 */ s32 m_type; /* 0 */
    /* 0x04 */ s32 m_counts[2];
    /* 0x0C */ Vec2f field_0xC;
    /* 0x14 */ Vec2f m_amplitude;
    /* 0x1C */ Vec2f m_cycle;
    /* 0x24 */ Vec2f m_waveLength;
};

struct UnkShimmerParams1_8042AF58 {
    /* 0x00 */ s32 m_type; /* 1 */
    /* 0x04 */ s32 m_counts[2];
    /* 0x0C */ u8 field_0xC;
    /* 0x10 */ float field_0x10;
    /* 0x14 */ float field_0x14;
    /* 0x18 */ float m_amplitude;
    /* 0x1C */ float m_cycle;
    /* 0x20 */ float m_waveLength;
};

namespace Effect {
namespace System {

class EffCmdWaveLengthExec : public EffCmdSingleExec {
  public:
    EffCmdWaveLengthExec(const u8 *data, u32 *state, float *target)
        : EffCmdSingleExec(data, state, target) {}
};

class EffCmdCycleExec : public EffCmdSingleExec {
  public:
    EffCmdCycleExec(const u8 *data, u32 *state, float *target)
        : EffCmdSingleExec(data, state, target) {}
};

class EffCmdAmplitudeExec : public EffCmdSingleExec {
  public:
    EffCmdAmplitudeExec(const u8 *data, u32 *state, float *target)
        : EffCmdSingleExec(data, state, target) {}
};

class EffCmd2DWaveLengthExec : public EffCmdVector2Exec {
  public:
    EffCmd2DWaveLengthExec(const u8 *data, u32 *state, Vec2f *target)
        : EffCmdVector2Exec(data, state, target) {}
};

class EffCmd2DCycleExec : public EffCmdVector2Exec {
  public:
    EffCmd2DCycleExec(const u8 *data, u32 *state, Vec2f *target)
        : EffCmdVector2Exec(data, state, target) {}
};

class EffCmd2DAmplitudeExec : public EffCmdVector2Exec {
  public:
    EffCmd2DAmplitudeExec(const u8 *data, u32 *state, Vec2f *target)
        : EffCmdVector2Exec(data, state, target) {}
};

EffectShimmer::EffectShimmer(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,
                             EffectManager *manager)
    : EffectElement(desc, info, manager), m_state(0), m_config(0), m_cursor(0), m_head(0),
      m_tail(0), m_pool(0) {
    m_size -= sizeof(EffectElement);
    m_size += sizeof(EffectShimmer);
    if (!(m_flags & 1)) {
        return;
    }
    const UnkShimmerDesc_8042AF58 *params = reinterpret_cast<const UnkShimmerDesc_8042AF58 *>(desc);
    m_params = params->m_params;
    m_config = params->m_config;
    m_duration = params->m_duration;
    m_pool = new UnkEffectPool_8042A344(0x30, params->m_commandCount);
    if (m_pool == 0 || (params->m_commandCount != 0 && m_pool->GetSize() == 0)) {
        m_flags &= ~1;
        return;
    }
    m_size += m_pool->GetSize() + sizeof(UnkEffectPool_8042A344);
    m_cursor = params->m_commands;
    const UnkShimmerParams0_8042AF58 *wave =
        static_cast<const UnkShimmerParams0_8042AF58 *>(params->m_params);
    switch (wave->m_type) {
    case 0:
        m_counts[0] = wave->m_counts[0];
        m_counts[1] = wave->m_counts[1];
        field_0x110.x = wave->field_0xC.x;
        field_0x110.y = wave->field_0xC.y;
        m_amplitude.x = wave->m_amplitude.x;
        m_amplitude.y = wave->m_amplitude.y;
        m_cycle.x = wave->m_cycle.x;
        m_cycle.y = wave->m_cycle.y;
        m_waveLength.x = wave->m_waveLength.x;
        m_waveLength.y = wave->m_waveLength.y;
        break;
    case 1: {
        const UnkShimmerParams1_8042AF58 *wave1 =
            reinterpret_cast<const UnkShimmerParams1_8042AF58 *>(wave);
        m_counts[0] = wave1->m_counts[0];
        m_counts[1] = wave1->m_counts[1];
        field_0x110.x = wave1->field_0x10;
        field_0x110.y = wave1->field_0x14;
        m_amplitude.x = wave1->m_amplitude;
        m_cycle.x = wave1->m_cycle;
        m_waveLength.x = wave1->m_waveLength;
        field_0x13C = wave1->field_0xC;
        break;
    }
    }
    m_alpha = (params->m_alpha & 0xFF) << 10;
    for (int i = 0; i < 2; i++) {
        if (m_counts[i] < 1) {
            m_counts[i] = 1;
        }
    }
}

EffectShimmer::~EffectShimmer() {
    EffectCommandExec *exec = m_head;
    fn_8042A5D0()->SetPool(m_pool);
    while (exec != 0) {
        EffectCommandExec *next = exec->m_next;
        delete exec;
        exec = next;
    }
    if (m_pool != 0) {
        delete m_pool;
    }
}

void EffectShimmer::ParseCommands() {
    while (m_cursor != 0) {
        if (m_tail != 0 && dynamic_cast<EffCmdWaitExec *>(m_tail) != 0) {
            return;
        }
        EffectCommandExec *exec;
        u32 length;
        switch (m_cursor[0]) {
        case 0xC:
            exec = new EffCmdMatAlphaExec(m_cursor, &m_state, &m_alpha);
            length = 0x10;
            break;
        case 0x1C:
            exec = new EffCmd2DAmplitudeExec(m_cursor, &m_state, &m_amplitude);
            length = 0x1C;
            break;
        case 0x1D:
            exec = new EffCmd2DCycleExec(m_cursor, &m_state, &m_cycle);
            length = 0x1C;
            break;
        case 0x1E:
            exec = new EffCmd2DWaveLengthExec(m_cursor, &m_state, &m_waveLength);
            length = 0x1C;
            break;
        case 0x1F:
            exec = new EffCmdAmplitudeExec(m_cursor, &m_state, &m_amplitude.x);
            length = 0x14;
            break;
        case 0x20:
            exec = new EffCmdCycleExec(m_cursor, &m_state, &m_cycle.x);
            length = 0x14;
            break;
        case 0x21:
            exec = new EffCmdWaveLengthExec(m_cursor, &m_state, &m_waveLength.x);
            length = 0x14;
            break;
        case 0xD:
            exec = new EffCmdWaitExec(m_cursor, &m_state, m_tail);
            length = 8;
            break;
        case 0xE:
            exec = new EffCmdLoopStartExec(m_cursor, &m_state);
            length = 8;
            m_loopDepth++;
            if (m_skipLoops != 0 || (m_flags & 0x10)) {
                m_skipLoops++;
            }
            break;
        case 0xF: {
            const u8 *jump = 0;
            if (m_skipLoops != 0 || (m_flags & 0x10)) {
                exec = new EffCmdLoopEndExec(m_cursor, &m_state, m_tail, 0);
                if (m_skipLoops != 0) {
                    m_skipLoops--;
                }
            } else {
                exec = new EffCmdLoopEndExec(m_cursor, &m_state, m_tail, &jump);
            }
            if (jump != 0) {
                m_cursor = jump;
                length = 8;
            } else {
                m_loopDepth--;
                length = 4;
            }
            break;
        }
        case 0x10:
            exec = new EffCmdCallbackExec(m_cursor, &m_state);
            length = 8;
            break;
        default:
            return;
        }
        if (exec != 0) {
            if (m_tail != 0) {
                m_tail->m_next = exec;
                exec->m_prev = m_tail;
                m_tail = exec;
            } else {
                m_tail = exec;
                m_head = exec;
            }
        } else {
            m_state |= 1;
            return;
        }
        if (m_cursor[1] != 0 && length != 0) {
            m_cursor += length;
        } else {
            m_cursor = 0;
        }
    }
}

void EffectShimmer::Update(float dt) {
    if (!(m_flags & 1)) {
        return;
    }
    if (m_flags & 4) {
        return;
    }
    if ((m_flags & 0x10) && (!(m_config & 0x40000) || m_head == 0)) {
        m_flags &= ~1;
        return;
    }
    if (m_flags & 2) {
        m_time += dt;
    }
    if (!(m_config & 1) && m_time > 0.0001f + m_duration) {
        m_flags &= ~1;
        return;
    }
    fn_8042A5D0()->SetPool(m_pool);
    ParseCommands();
    if (!(m_state & 1)) {
        EffectCommandExec *next;
        EffectCommandExec *exec = m_head;
        while (exec != 0) {
            if (!exec->Exec(dt)) {
                if (exec->m_prev != 0) {
                    exec->m_prev->m_next = exec->m_next;
                }
                if (exec->m_next != 0) {
                    exec->m_next->m_prev = exec->m_prev;
                }
                if (exec == m_head) {
                    m_head = exec->m_next;
                }
                if (exec == m_tail) {
                    m_tail = exec->m_prev;
                }
                next = exec->m_next;
                delete exec;
                exec = next;
            } else {
                exec = exec->m_next;
            }
        }
    }
    if (m_state & 1) {
        m_flags &= ~1;
        return;
    }
    UpdateTransform(dt);
}

} // namespace System
} // namespace Effect
