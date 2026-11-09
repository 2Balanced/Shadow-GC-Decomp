/* Effect::System::EffectArranger (class name from RTTI): the element that runs its description's
 * command list. Each update it turns commands into executors (Effect/System/EffectCommandExec.h)
 * up to the next wait, runs them, and frees the finished ones; the launch commands (opcodes 0x11
 * and 0x12) run in their own pool, after the element's transform is built, and keep running while
 * the element is paused. File name from the class; member names are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first).
 *
 * NonMatching: 2 of 4 functions match. The constructor (98%) computes the second pool's size in
 * other registers; ParseCommands (99.96%) calls the executor constructors of the previous unit,
 * which have no names there yet. */
#define EFFECT_ELEMENT_POLYMORPHIC
#define EFFECT_ELEMENT_FULL
#define EFFECT_SYSTEM_FULL

#include "Effect/System/EffectArranger.h"
#include "Effect/System/EffectCommandExec.h"
#include "Effect/System/EffectDesc.h"
#include "Effect/System/EffectPool.h"

/* An arranger's description (layout from the constructor and Update). */
struct UnkArrangerDesc_80426C08 {
    /* 0x00 */ u8 field_0x0[0x2C];
    /* 0x2C */ u32 m_config;
    /* 0x30 */ s16 m_commandCount; /* executors that can run at once */
    /* 0x32 */ s16 m_launchCount;  /* launch executors that can run at once */
    /* 0x34 */ float m_duration;
    /* 0x38 */ const u8 *m_commands;
};

/* Whether any launch command (opcode 0x11 or 0x12) is still running. */
inline bool HasLaunchCommands(Effect::System::EffectCommandExec *head) {
    for (Effect::System::EffectCommandExec *exec = head; exec != 0;
         exec = exec->m_next) {
        u32 opcode = exec->m_data[0];
        if (opcode == 0x11 || opcode == 0x12) {
            return true;
        }
    }
    return false;
}

/* Runs the launch commands (opcodes 0x11 and 0x12) in the launch pool, and the others in the
 * command pool, freeing the finished ones. */
inline void RunLaunchCommands(Effect::System::EffectArranger *arranger, float dt) {
    Effect::System::fn_8042A5D0()->SetPool(arranger->m_launchPool);
    Effect::System::EffectCommandExec *exec = arranger->m_head;
    while (exec != 0) {
        u32 opcode = exec->m_data[0];
        if (opcode != 0x11 && opcode != 0x12) {
            exec = exec->m_next;
        } else if (!exec->Exec(dt)) {
            if (exec->m_prev != 0) {
                exec->m_prev->m_next = exec->m_next;
            }
            if (exec->m_next != 0) {
                exec->m_next->m_prev = exec->m_prev;
            }
            if (exec == arranger->m_head) {
                arranger->m_head = exec->m_next;
            }
            if (exec == arranger->m_tail) {
                arranger->m_tail = exec->m_prev;
            }
            Effect::System::EffectCommandExec *next = exec->m_next;
            delete exec;
            exec = next;
        } else {
            exec = exec->m_next;
        }
    }
}

inline void RunOtherCommands(Effect::System::EffectArranger *arranger, float dt) {
    Effect::System::fn_8042A5D0()->SetPool(arranger->m_commandPool);
    Effect::System::EffectCommandExec *exec = arranger->m_head;
    while (exec != 0) {
        u32 opcode = exec->m_data[0];
        if (opcode == 0x11 || opcode == 0x12) {
            exec = exec->m_next;
        } else if (!exec->Exec(dt)) {
            if (exec->m_prev != 0) {
                exec->m_prev->m_next = exec->m_next;
            }
            if (exec->m_next != 0) {
                exec->m_next->m_prev = exec->m_prev;
            }
            if (exec == arranger->m_head) {
                arranger->m_head = exec->m_next;
            }
            if (exec == arranger->m_tail) {
                arranger->m_tail = exec->m_prev;
            }
            Effect::System::EffectCommandExec *next = exec->m_next;
            delete exec;
            exec = next;
        } else {
            exec = exec->m_next;
        }
    }
}

namespace Effect {
namespace System {

EffectArranger::EffectArranger(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,
                               EffectManager *manager)
    : EffectElement(desc, info, manager), m_state(0), m_config(0), m_launchInfo(0), m_cursor(0),
      m_head(0), m_tail(0), m_commandPool(0), m_launchPool(0) {
    m_size -= sizeof(EffectElement);
    m_size += sizeof(EffectArranger);
    if (!(m_flags & 1)) {
        return;
    }
    const UnkArrangerDesc_80426C08 *params =
        reinterpret_cast<const UnkArrangerDesc_80426C08 *>(desc);
    m_config = params->m_config;
    m_commandPool = new UnkEffectPool_8042A344(0x30, params->m_commandCount);
    if (m_commandPool == 0 ||
        (params->m_commandCount != 0 && m_commandPool->GetSize() == 0)) {
        m_flags &= ~1;
        return;
    }
    m_size += m_commandPool->GetSize() + sizeof(UnkEffectPool_8042A344);
    m_launchPool = new UnkEffectPool_8042A344(0x80, params->m_launchCount);
    if (m_launchPool == 0 || (params->m_launchCount != 0 && m_launchPool->GetSize() == 0)) {
        m_flags &= ~1;
        return;
    }
    m_size += m_commandPool->GetSize() + sizeof(UnkEffectPool_8042A344);
    m_cursor = params->m_commands;
}

EffectArranger::~EffectArranger() {
    EffectCommandExec *exec = m_head;
    while (exec != 0) {
        EffectCommandExec *next = exec->m_next;
        if (exec->m_data[0] == 0x11) {
            fn_8042A5D0()->SetPool(m_launchPool);
        } else {
            fn_8042A5D0()->SetPool(m_commandPool);
        }
        delete exec;
        exec = next;
    }
    if (m_commandPool != 0) {
        delete m_commandPool;
    }
    if (m_launchPool != 0) {
        delete m_launchPool;
    }
}

void EffectArranger::ParseCommands() {
    fn_8042A5D0()->SetPool(m_commandPool);
    while (m_cursor != 0) {
        if (m_tail != 0 && dynamic_cast<EffCmdWaitExec *>(m_tail) != 0) {
            return;
        }
        EffectCommandExec *exec = 0;
        u32 length;
        switch (m_cursor[0]) {
        case 3:
            exec = new EffCmd3DPosExec(m_cursor, &m_state, &m_offset);
            length = 0x24;
            break;
        case 4:
            exec = new EffCmd3DRotExec(m_cursor, &m_state, &m_angles);
            length = 0x24;
            break;
        case 6:
            exec = new EffCmd3DSclExec(m_cursor, &m_state, &m_scale);
            length = 0x24;
            break;
        case 8:
            exec = new EffCmd3DVelExec(m_cursor, &m_state, &m_offsetSpeed);
            length = 0x24;
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
        case 0x11:
            if (!(m_flags & 0x10) || !(m_config & 0x40000)) {
                fn_8042A5D0()->SetPool(m_launchPool);
                exec = new EffCmdLaunchExec(m_cursor, &m_state, m_manager, m_launchInfo, this);
                fn_8042A5D0()->SetPool(m_commandPool);
            }
            length = 0x58;
            break;
        case 0x12:
            if (!(m_flags & 0x10) || !(m_config & 0x40000)) {
                fn_8042A5D0()->SetPool(m_launchPool);
                exec = new EffCmdLaunchExec(m_cursor, &m_state, m_manager, m_launchInfo, this);
                fn_8042A5D0()->SetPool(m_commandPool);
            }
            length = 0x5C;
            break;
        case 0x13:
            exec = new EffCmdRequestStopExec(m_cursor, &m_state, this);
            length = 4;
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

void EffectArranger::Update(float dt) {
    if (!(m_flags & 1)) {
        return;
    }
    if (m_flags & 4) {
        RunLaunchCommands(this, dt);
    } else {
        const UnkArrangerDesc_80426C08 *params =
            reinterpret_cast<const UnkArrangerDesc_80426C08 *>(m_desc);
        if (m_flags & 2) {
            m_time += dt;
        }
        if (!(m_config & 1) && m_time > 0.0001f + params->m_duration) {
            m_flags &= ~1;
            return;
        }
        ParseCommands();
        if (!(m_state & 1)) {
            RunOtherCommands(this, dt);
        }
        if (m_state & 1) {
            m_flags &= ~1;
            return;
        }
        UpdateTransform(dt);
        RunLaunchCommands(this, dt);
        if (m_flags & 0x10) {
            EffectCommandExec *head = m_head;
            if (!HasLaunchCommands(head)) {
                if (!(m_config & 0x40000) || head == 0) {
                    m_flags &= ~1;
                }
            }
        }
    }
}

} // namespace System
} // namespace Effect