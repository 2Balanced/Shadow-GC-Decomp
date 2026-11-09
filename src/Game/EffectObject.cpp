/* Effect::System::EffectObject (class name and base from RTTI): the effect library's model effect,
 * which runs the commands of its description, and the base of all command executors,
 * Effect::System::EffectCommandExec, whose destructor is the unit's first function in the source.
 * The unit also emits the inline destructors of the position, rotation and velocity commands
 * (their only users are here), EffCmdVector3Exec's (0x80421C18), and the executors' and the
 * command pool's operator delete, which the exception tables of the constructor and
 * ParseCommands name. File name from the class. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first).
 *
 * NonMatching: 10 of 11 functions match.
 * - The constructor (78%): retail loads each colour byte of the description after storing the
 *   one before it (and m_size before them), GC/2.7 hoists the four loads above the stores; about
 *   ten spellings of the colour code (local pointers, an inline setter, masks, casts) leave it so.
 *   This is the load-after-store scheduling difference of docs/research-notes.md
 *   (Game/ScatteringRing.cpp).
 * - GC/2.7 emits the two operator deletes right after the functions whose exception tables name
 *   them (ParseCommands, the constructor); retail has them after ~EffectCommandExec. */
#define EFFECT_ELEMENT_POLYMORPHIC
#define EFFECT_ELEMENT_FULL
#define EFFECT_SYSTEM_FULL

#include "Effect/System/EffectCommandExec.h"
#include "Effect/System/EffectDesc.h"
#include "Effect/System/EffectObject.h"
#include "Effect/System/EffectPool.h"

namespace Effect {
namespace System {

/* The part of the description EffectObject reads (layout from the constructor). */
struct UnkObjectDesc_80421D38 {
    /* 0x00 */ u8 field_0x0[0x2C];
    /* 0x2C */ u32 m_config;
    /* 0x30 */ s32 m_commandCount; /* executors that can run at once */
    /* 0x34 */ float m_duration;   /* in frames */
    /* 0x38 */ u8 m_color[4];      /* alpha, red, green, blue */
    /* 0x3C */ u8 field_0x3C[0x40 - 0x3C];
    /* 0x40 */ const u8 *m_commands;
};

EffectCommandExec::~EffectCommandExec() {}

EffectObject::EffectObject(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,
                           EffectManager *manager)
    : EffectElement(desc, info, manager), m_state(0), m_config(0), m_cursor(0), m_head(0),
      m_tail(0), m_commandPool(0) {
    const UnkObjectDesc_80421D38 *params = reinterpret_cast<const UnkObjectDesc_80421D38 *>(desc);
    m_size -= sizeof(EffectElement);
    m_size += sizeof(EffectObject);
    if (!(m_flags & 1)) {
        return;
    }
    m_config = params->m_config;
    m_commandPool = new UnkEffectPool_8042A344(0x30, params->m_commandCount);
    if (m_commandPool == 0 ||
        (params->m_commandCount != 0 && m_commandPool->GetSize() == 0)) {
        m_flags &= ~1;
        return;
    }
    m_size = m_size + m_commandPool->GetSize() + sizeof(UnkEffectPool_8042A344);
    m_color[0] = (params->m_color[1] & 0xFF) << 22;
    m_color[1] = (params->m_color[2] & 0xFF) << 22;
    m_color[2] = (params->m_color[3] & 0xFF) << 22;
    m_color[3] = (params->m_color[0] & 0xFF) << 22;
    m_cursor = params->m_commands;
}

EffectObject::~EffectObject() {
    EffectCommandExec *exec = m_head;
    fn_8042A5D0()->SetPool(m_commandPool);
    while (exec != 0) {
        EffectCommandExec *next = exec->m_next;
        delete exec;
        exec = next;
    }
    if (m_commandPool != 0) {
        delete m_commandPool;
    }
}

void EffectObject::ParseCommands() {
    while (m_cursor != 0) {
        EffectCommandExec *exec;
        u32 size;
        if (m_tail != 0 && dynamic_cast<EffCmdWaitExec *>(m_tail) != 0) {
            return;
        }
        switch (*m_cursor) {
        case 2:
            exec = new EffCmdGlareSWExec(m_cursor, &m_state);
            size = 8;
            break;
        case 3:
            exec = new EffCmd3DPosExec(m_cursor, &m_state, &m_offset);
            size = 0x24;
            break;
        case 4:
            exec = new EffCmd3DRotExec(m_cursor, &m_state, &m_angles);
            size = 0x24;
            break;
        case 6:
            exec = new EffCmd3DSclExec(m_cursor, &m_state, &m_scale);
            size = 0x24;
            break;
        case 8:
            exec = new EffCmd3DVelExec(m_cursor, &m_state, &m_offsetSpeed);
            size = 0x24;
            break;
        case 11:
            exec = new EffCmdMatColorExec(m_cursor, &m_state, &m_color[0]);
            size = 0x14;
            break;
        case 12:
            exec = new EffCmdMatAlphaExec(m_cursor, &m_state, &m_color[3]);
            size = 0x10;
            break;
        case 13:
            exec = new EffCmdWaitExec(m_cursor, &m_state, m_tail);
            size = 8;
            break;
        case 14:
            exec = new EffCmdLoopStartExec(m_cursor, &m_state);
            size = 8;
            m_loopDepth++;
            if (m_skipLoops != 0 || (m_flags & 0x10)) {
                m_skipLoops++;
            }
            break;
        case 15: {
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
                size = 8;
            } else {
                m_loopDepth--;
                size = 4;
            }
            break;
        }
        case 16:
            exec = new EffCmdCallbackExec(m_cursor, &m_state);
            size = 8;
            break;
        case 19:
            exec = new EffCmdRequestStopExec(m_cursor, &m_state, this);
            size = 4;
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
        if (m_cursor[1] != 0 && size != 0) {
            m_cursor += size;
        } else {
            m_cursor = 0;
        }
    }
}

void EffectObject::Update(float dt) {
    if (!(m_flags & 1)) {
        return;
    }
    if (m_flags & 4) {
        return;
    }
    if (m_flags & 0x10) {
        if (!(m_config & 0x40000) || m_head == 0) {
            m_flags &= ~1;
            return;
        }
    }
    const UnkObjectDesc_80421D38 *params = reinterpret_cast<const UnkObjectDesc_80421D38 *>(m_desc);
    if (m_flags & 2) {
        m_time += dt;
    }
    if (!(m_config & 1) && m_time > 0.0001f + params->m_duration) {
        m_flags &= ~1;
        return;
    }
    fn_8042A5D0()->SetPool(m_commandPool);
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
