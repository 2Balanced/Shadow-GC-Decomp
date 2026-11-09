/* The effect library's commands (class names and bases from RTTI; Effect/System/EffectCommand.h):
 * the commands that stop an effect, launch another one, call back, loop, wait, change the
 * material colour and alpha, a scale, a vector or a float over time, switch the glare and play a
 * texture pattern. Their base, EffectCommandExec, is in Game/EffectObject.cpp. File name from the
 * base class. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first); the inline destructors come out at the end of the unit.
 *
 * NonMatching: 43 of 46 functions match. The constructors of EffCmdMatAlphaExec (98%),
 * EffCmdMatColorExec (98%) and EffCmdPtnAnimExec (99%) add a random whole number to a member
 * right after the random call; retail loads the member into r0 and the converted number into r3,
 * GC/2.7 the other way round (about ten spellings tried: compound and plain adds, a named
 * temporary, an inline helper, casts). The unit's data also carries boost's bad_weak_ptr and
 * std::exception RTTI and vtables from weak_ptr::lock, which retail's unit does not have, and
 * GC/2.7 emits GetSpeed's out-of-line copy after Exec of the launch command where retail has it
 * after EffCmdSingleExec's constructor. */
#define EFFECT_COMMAND_LAUNCH
#define EFFECT_ELEMENT_POLYMORPHIC
#define EFFECT_ELEMENT_FULL
#define EFFECT_SYSTEM_FULL

#include "Effect/System/EffectArranger.h"
#include "Effect/System/EffectCommandExec.h"
#include "Effect/System/EffectManager.h"

#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

extern "C" {
/* Vector helpers (see Game/MathTypes.h). */
Vec3f fn_800091A4(const Vec3f *vector, float scale);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* += */
void fn_8040CE04(Vec3f *vector, const Vec3f *value); /* -= */
void fn_8040CE38(Vec3f *vector, float scale);         /* *= */
void fn_8040B790(Mtx34f *matrix, const Vec3f *scale); /* applies a scale */
/* Makes an element follow a matrix (a member of EffectElement, at 0x80425E68). */
void fn_80425E68(::Effect::System::EffectElement *element, const Mtx34f *matrix);
}

namespace Effect {
namespace System {

/* The effect system's eighth virtual function returns a random number from 0 to 1. The shared
 * Effect/System/EffectSystem.h calls that slot Flush and gives it an argument (EffectSystemUSA's
 * version only calls the game's random number function, fn_8040EE90); this view of the vtable
 * calls it the way the commands do. */
class UnkEffectRandom_8042A5D0 {
  public:
    virtual void *vf08(void *heap, u32 size) = 0;
    virtual void *vf0C() = 0;
    virtual void *vf10(u32 size) = 0;
    virtual void vf14(void *memory) = 0;
    virtual void vf18(void *memory, char value, u32 size) = 0;
    virtual void vf1C(void *to, const void *from, u32 size) = 0;
    virtual float Random() = 0;
};

inline float EffectRandom() {
    return reinterpret_cast<UnkEffectRandom_8042A5D0 *>(fn_8042A5D0())->Random();
}

/* A finished command can end the whole effect (every command class has its m_state). */
template <class T> inline void EndCommand(T *exec) {
    if (exec->m_data[2] != 0) {
        *exec->m_state |= 1;
    }
}

/* Colour channels are kept from bit 22; a channel is at most 255 and goes to 0 below 1. */
inline void ClampColor(int &value) {
    if (value > 0x3FC00000) {
        value = 0x3FC00000;
    } else if (value < 0x3FFFFF) {
        value = 0;
    }
}

/* The launch command's parent is an arranger (Game/EffectArranger.cpp); bit 11 of its settings
 * makes the children follow its matrix every update. */
inline u32 ParentConfig(const EffectElement *parent) {
    return static_cast<const EffectArranger *>(parent)->m_config;
}

EffCmdSingleExec::EffCmdSingleExec(const u8 *data, u32 *state,
                                   float *target)
    : EffectCommandExec(data), m_state(state), m_target(target), m_elapsed(0.0f) {
    const UnkEffectSingleCommand_80424B80 *c =
        static_cast<const UnkEffectSingleCommand_80424B80 *>(reinterpret_cast<const UnkEffectCommand_80421834 *>(m_data));
    m_delta = c->m_base + c->m_range * EffectRandom();
    m_duration = c->m_frames;
    if (c->m_framesRange != 0) {
        m_duration += c->m_framesRange * EffectRandom();
    }
    if (m_duration < 0.0001f) {
        if (c->m_relative) {
            *m_target += m_delta;
        } else {
            *m_target = m_delta;
        }
        m_target = 0;
    } else {
        if (!c->m_relative) {
            m_delta -= *target;
        }
        m_delta = m_delta / m_duration;
        m_duration += 0.0001f;
    }
}

bool EffCmdSingleExec::Exec(float dt) {
    if (m_target == 0) {
        EndCommand(this);
        return false;
    }
    if (!field_0x10) {
        field_0x10 = 1;
        return true;
    }
    m_elapsed += dt;
    if (m_elapsed > m_duration) {
        EndCommand(this);
        return false;
    }
    *m_target += m_delta * dt;
    return true;
}

EffCmdVector2Exec::EffCmdVector2Exec(const u8 *data, u32 *state, Vec2f *target)
    : EffectCommandExec(data), m_state(state), m_target(reinterpret_cast<Vec3f *>(target)), m_elapsed(0.0f) {
    const UnkEffectVector2Command_804248D4 *c =
        static_cast<const UnkEffectVector2Command_804248D4 *>(reinterpret_cast<const UnkEffectCommand_80421834 *>(m_data));
    if (c->m_sharedRandom) {
        float random = EffectRandom();
        m_delta.x = c->m_base.x + c->m_range.x * random;
        m_delta.y = c->m_base.y + c->m_range.y * random;
    } else {
        m_delta.x = c->m_base.x + c->m_range.x * EffectRandom();
        m_delta.y = c->m_base.y + c->m_range.y * EffectRandom();
    }
    m_duration = c->m_frames;
    if (c->m_framesRange != 0) {
        m_duration += c->m_framesRange * EffectRandom();
    }
    if (m_duration < 0.0001f) {
        if (c->m_relative) {
            fn_8040CFF8(m_target, &m_delta);
        } else {
            fn_800091D0(m_target, &m_delta);
        }
        m_target = 0;
    } else {
        if (!c->m_relative) {
            fn_8040CE04(&m_delta, m_target);
        }
        fn_8040CE38(&m_delta, 1.0f / m_duration);
        m_duration += 0.0001f;
    }
}

bool EffCmdVector2Exec::Exec(float dt) {
    if (m_target == 0) {
        EndCommand(this);
        return false;
    }
    if (!field_0x10) {
        field_0x10 = 1;
        return true;
    }
    m_elapsed += dt;
    if (m_elapsed > m_duration) {
        EndCommand(this);
        return false;
    }
    fn_8040CFF8(m_target, &(const Vec3f &)fn_800091A4(&m_delta, dt));
    return true;
}

EffCmdVector3Exec::EffCmdVector3Exec(const u8 *data, u32 *state,
                                     Vec3f *target)
    : EffectCommandExec(data), m_state(state), m_target(target), m_elapsed(0.0f) {
    const UnkEffectVector3Command_804245CC *c =
        static_cast<const UnkEffectVector3Command_804245CC *>(reinterpret_cast<const UnkEffectCommand_80421834 *>(m_data));
    if (c->m_sharedRandom) {
        float random = EffectRandom();
        m_delta.x = c->m_base.x + c->m_range.x * random;
        m_delta.y = c->m_base.y + c->m_range.y * random;
        m_delta.z = c->m_base.z + c->m_range.z * random;
    } else {
        m_delta.x = c->m_base.x + c->m_range.x * EffectRandom();
        m_delta.y = c->m_base.y + c->m_range.y * EffectRandom();
        m_delta.z = c->m_base.z + c->m_range.z * EffectRandom();
    }
    m_duration = c->m_frames;
    if (c->m_framesRange != 0) {
        m_duration += c->m_framesRange * EffectRandom();
    }
    if (m_duration < 0.0001f) {
        if (c->m_relative) {
            fn_8040CFF8(m_target, &m_delta);
        } else {
            fn_800091D0(m_target, &m_delta);
        }
        m_target = 0;
    } else {
        if (!c->m_relative) {
            fn_8040CE04(&m_delta, m_target);
        }
        fn_8040CE38(&m_delta, 1.0f / m_duration);
        m_duration += 0.0001f;
    }
}

bool EffCmdVector3Exec::Exec(float dt) {
    if (m_target == 0) {
        EndCommand(this);
        return false;
    }
    if (!field_0x10) {
        field_0x10 = 1;
        return true;
    }
    m_elapsed += dt;
    if (m_elapsed > m_duration) {
        EndCommand(this);
        return false;
    }
    fn_8040CFF8(m_target, &(const Vec3f &)fn_800091A4(&m_delta, dt));
    return true;
}

EffCmdPtnAnimExec::EffCmdPtnAnimExec(const u8 *data, u32 *state,
                                     Vec2f *uvMin, Vec2f *uvMax)
    : EffectCommandExec(data), m_state(state), m_uvMin(uvMin), m_uvMax(uvMax), m_cell(0),
      m_elapsed(0.0f) {
    const UnkEffectPatternCommand_8042425C *c =
        static_cast<const UnkEffectPatternCommand_8042425C *>(reinterpret_cast<const UnkEffectCommand_80421834 *>(m_data));
    switch (c->m_grid) {
    case 0:
        m_grid = 2;
        break;
    case 1:
        m_grid = 4;
        break;
    case 2:
        m_grid = 8;
        break;
    default:
        m_grid = 0;
        break;
    }
    m_cell = m_firstCell = c->m_firstCell;
    if (c->m_firstCellRange != 0) {
        m_cell += (int)(c->m_firstCellRange * EffectRandom());
    }
    m_lastCell = c->m_lastCell;
    m_frames = c->m_frames;
    if (c->m_framesRange != 0) {
        m_frames += c->m_framesRange * EffectRandom();
    }
    SetCell();
    if (c->m_frames == 0) {
        m_grid = 0;
    }
}

bool EffCmdPtnAnimExec::Exec(float dt) {
    if (m_grid == 0) {
        EndCommand(this);
        return false;
    }
    if (!field_0x10) {
        field_0x10 = 1;
        return true;
    }
    m_elapsed += dt;
    float limit = 0.0001f + m_frames;
    if (m_elapsed < limit) {
        return true;
    }
    if (static_cast<const UnkEffectPatternCommand_8042425C *>(reinterpret_cast<const UnkEffectCommand_80421834 *>(m_data))->m_loop) {
        do {
            m_elapsed -= m_frames;
            if (m_cell++ == m_lastCell) {
                m_cell = m_firstCell;
            }
        } while (m_elapsed > limit);
    } else {
        do {
            m_elapsed -= m_frames;
            if (m_cell++ == m_lastCell) {
                break;
            }
        } while (m_elapsed > limit);
        if (m_cell > m_lastCell) {
            EndCommand(this);
            return false;
        }
    }
    SetCell();
    return true;
}

EffCmdGlareSWExec::EffCmdGlareSWExec(const u8 *data, u32 *state)
    : EffectCommandExec(data), m_state(state) {
    const UnkEffectSwitchCommand_80423FC0 *c =
        static_cast<const UnkEffectSwitchCommand_80423FC0 *>(reinterpret_cast<const UnkEffectCommand_80421834 *>(m_data));
    *m_state |= 2;
    if (c->m_on) {
        *m_state |= 4;
    } else {
        *m_state &= ~4;
    }
}

bool EffCmdGlareSWExec::Exec(float dt) {
    EndCommand(this);
    return false;
}

EffCmd3DSclExec::EffCmd3DSclExec(const u8 *data, u32 *state, Vec3f *target)
    : EffectCommandExec(data), m_state(state), m_target(target), m_elapsed(0.0f) {
    const UnkEffectVector3Command_804245CC *c =
        static_cast<const UnkEffectVector3Command_804245CC *>(reinterpret_cast<const UnkEffectCommand_80421834 *>(m_data));
    if (c->m_sharedRandom) {
        float random = EffectRandom();
        m_delta.x = c->m_base.x + c->m_range.x * random;
        m_delta.y = c->m_base.y + c->m_range.y * random;
        m_delta.z = c->m_base.z + c->m_range.z * random;
    } else {
        m_delta.x = c->m_base.x + c->m_range.x * EffectRandom();
        m_delta.y = c->m_base.y + c->m_range.y * EffectRandom();
        m_delta.z = c->m_base.z + c->m_range.z * EffectRandom();
    }
    m_duration = c->m_frames;
    if (c->m_framesRange != 0) {
        m_duration += c->m_framesRange * EffectRandom();
    }
    if (m_duration < 0.0001f) {
        if (c->m_relative) {
            m_target->x *= m_delta.x;
            m_target->y *= m_delta.y;
            m_target->z *= m_delta.z;
        } else {
            fn_800091D0(m_target, &m_delta);
        }
        m_target = 0;
    } else {
        if (c->m_relative) {
            m_delta.x *= m_target->x;
            m_delta.y *= m_target->y;
            m_delta.z *= m_target->z;
        }
        fn_8040CE04(&m_delta, target);
        fn_8040CE38(&m_delta, 1.0f / m_duration);
        m_duration += 0.0001f;
    }
}

bool EffCmd3DSclExec::Exec(float dt) {
    if (m_target == 0) {
        EndCommand(this);
        return false;
    }
    if (!field_0x10) {
        field_0x10 = 1;
        return true;
    }
    m_elapsed += dt;
    if (m_elapsed > m_duration) {
        EndCommand(this);
        return false;
    }
    fn_8040CFF8(m_target, &(const Vec3f &)fn_800091A4(&m_delta, dt));
    return true;
}

EffCmd2DSclExec::EffCmd2DSclExec(const u8 *data, u32 *state, Vec2f *target)
    : EffectCommandExec(data), m_state(state), m_target(reinterpret_cast<Vec3f *>(target)), m_elapsed(0.0f) {
    const UnkEffectVector2Command_804248D4 *c =
        static_cast<const UnkEffectVector2Command_804248D4 *>(reinterpret_cast<const UnkEffectCommand_80421834 *>(m_data));
    if (c->m_sharedRandom) {
        float random = EffectRandom();
        m_delta.x = c->m_base.x + c->m_range.x * random;
        m_delta.y = c->m_base.y + c->m_range.y * random;
    } else {
        m_delta.x = c->m_base.x + c->m_range.x * EffectRandom();
        m_delta.y = c->m_base.y + c->m_range.y * EffectRandom();
    }
    m_duration = c->m_frames;
    if (c->m_framesRange != 0) {
        m_duration += c->m_framesRange * EffectRandom();
    }
    if (m_duration < 0.0001f) {
        if (c->m_relative) {
            m_target->x *= m_delta.x;
            m_target->y *= m_delta.y;
        } else {
            fn_800091D0(m_target, &m_delta);
        }
        m_target = 0;
    } else {
        if (c->m_relative) {
            m_delta.x *= m_target->x;
            m_delta.y *= m_target->y;
        }
        fn_8040CE04(&m_delta, m_target);
        fn_8040CE38(&m_delta, 1.0f / m_duration);
        m_duration += 0.0001f;
    }
}

bool EffCmd2DSclExec::Exec(float dt) {
    if (m_target == 0) {
        EndCommand(this);
        return false;
    }
    if (!field_0x10) {
        field_0x10 = 1;
        return true;
    }
    m_elapsed += dt;
    if (m_elapsed > m_duration) {
        EndCommand(this);
        return false;
    }
    fn_8040CFF8(m_target, &(const Vec3f &)fn_800091A4(&m_delta, dt));
    return true;
}

EffCmdMatColorExec::EffCmdMatColorExec(const u8 *data, u32 *state, u32 *target)
    : EffectCommandExec(data), m_state(state), m_target(reinterpret_cast<int *>(target)), m_elapsed(0.0f) {
    const UnkEffectColorCommand_8042341C *c =
        static_cast<const UnkEffectColorCommand_8042341C *>(reinterpret_cast<const UnkEffectCommand_80421834 *>(m_data));
    if (c->m_sharedRandom) {
        float random = EffectRandom();
        m_delta[0] = c->m_base[0];
        if (c->m_range[0] != 0) {
            m_delta[0] += (int)(c->m_range[0] * random);
        }
        m_delta[1] = c->m_base[1];
        if (c->m_range[1] != 0) {
            m_delta[1] += (int)(c->m_range[1] * random);
        }
        m_delta[2] = c->m_base[2];
        if (c->m_range[2] != 0) {
            m_delta[2] += (int)(c->m_range[2] * random);
        }
    } else {
        m_delta[0] = c->m_base[0];
        if (c->m_range[0] != 0) {
            m_delta[0] += (int)(c->m_range[0] * EffectRandom());
        }
        m_delta[1] = c->m_base[1];
        if (c->m_range[1] != 0) {
            m_delta[1] += (int)(c->m_range[1] * EffectRandom());
        }
        m_delta[2] = c->m_base[2];
        if (c->m_range[2] != 0) {
            m_delta[2] += (int)(c->m_range[2] * EffectRandom());
        }
    }
    m_duration = c->m_frames;
    if (c->m_framesRange != 0) {
        m_duration += c->m_framesRange * EffectRandom();
    }
    if (c->m_relative) {
        for (int i = 0; i < 3; i++) {
            m_delta[i] <<= 22;
            m_delta[i] += m_target[i];
            ClampColor(m_delta[i]);
            m_delta[i] -= m_target[i];
        }
    } else {
        for (int i = 0; i < 3; i++) {
            m_delta[i] <<= 22;
            ClampColor(m_delta[i]);
        }
    }
    if (m_duration < 0.0001f) {
        for (int i = 0; i < 3; i++) {
            m_target[i] = m_delta[i];
        }
        m_target = 0;
    } else {
        if (!c->m_relative) {
            for (int i = 0; i < 3; i++) {
                m_delta[i] -= m_target[i];
            }
        }
        float rate = 1.0f / m_duration;
        for (int i = 0; i < 3; i++) {
            m_delta[i] = (int)(m_delta[i] * rate);
        }
        m_duration += 0.0001f;
    }
}

bool EffCmdMatColorExec::Exec(float dt) {
    if (m_target == 0) {
        EndCommand(this);
        return false;
    }
    if (!field_0x10) {
        field_0x10 = 1;
        return true;
    }
    m_elapsed += dt;
    if (m_elapsed > m_duration) {
        EndCommand(this);
        return false;
    }
    for (int i = 0; i < 3; i++) {
        if (m_delta[i] != 0) {
            m_target[i] += (int)(m_delta[i] * dt);
            if (m_target[i] > 0x3FC00000) {
                m_target[i] = 0x3FC00000;
                m_delta[i] = 0;
            } else if (m_target[i] < 0x3FFFFF) {
                m_target[i] = 0;
                m_delta[i] = 0;
            }
        }
    }
    return true;
}

EffCmdMatAlphaExec::EffCmdMatAlphaExec(const u8 *data, u32 *state, u32 *target)
    : EffectCommandExec(data), m_state(state), m_target(reinterpret_cast<int *>(target)), m_elapsed(0.0f) {
    const UnkEffectAlphaCommand_80423000 *c =
        static_cast<const UnkEffectAlphaCommand_80423000 *>(reinterpret_cast<const UnkEffectCommand_80421834 *>(m_data));
    m_delta = c->m_base;
    m_delta += (int)(c->m_range * EffectRandom());
    if (c->m_range != 0) {
        m_delta += (int)(c->m_range * EffectRandom());
    }
    m_duration = c->m_frames;
    if (c->m_framesRange != 0) {
        m_duration += c->m_framesRange * EffectRandom();
    }
    m_delta <<= 22;
    if (c->m_relative) {
        m_delta += *m_target;
        ClampColor(m_delta);
        m_delta -= *m_target;
    } else {
        ClampColor(m_delta);
    }
    if (m_duration < 0.0001f) {
        *m_target = m_delta;
        m_target = 0;
    } else {
        if (!c->m_relative) {
            m_delta -= *m_target;
        }
        m_delta = (int)(m_delta / m_duration);
        m_duration += 0.0001f;
    }
}

bool EffCmdMatAlphaExec::Exec(float dt) {
    if (m_target == 0) {
        EndCommand(this);
        return false;
    }
    if (!field_0x10) {
        field_0x10 = 1;
        return true;
    }
    m_elapsed += dt;
    if (m_elapsed > m_duration) {
        EndCommand(this);
        return false;
    }
    if (m_delta != 0) {
        *m_target += (int)(m_delta * dt);
        if (*m_target > 0x3FC00000) {
            *m_target = 0x3FC00000;
            m_delta = 0;
        } else if (*m_target < 0x3FFFFF) {
            m_target = 0;
            m_delta = 0;
        }
    }
    return true;
}

EffCmdWaitExec::EffCmdWaitExec(const u8 *data, u32 *state, EffectCommandExec *previous)
    : EffectCommandExec(data), m_state(state), m_elapsed(0.0f) {
    const UnkEffectCountCommand_80422D30 *c =
        static_cast<const UnkEffectCountCommand_80422D30 *>(reinterpret_cast<const UnkEffectCommand_80421834 *>(m_data));
    m_duration = c->m_count;
    if (c->m_count != 0) {
        m_duration += 0.0001f;
    }
    m_previous = previous;
}

bool EffCmdWaitExec::Exec(float dt) {
    if (m_duration < 0.0001f) {
        if (m_previous == 0 || m_previous != m_prev) {
            EndCommand(this);
            return false;
        }
    } else {
        if (!field_0x10) {
            field_0x10 = 1;
            return true;
        }
        m_elapsed += dt;
        if (m_elapsed > m_duration) {
            EndCommand(this);
            return false;
        }
    }
    return true;
}

EffCmdLoopStartExec::EffCmdLoopStartExec(const u8 *data, u32 *state)
    : EffectCommandExec(data), m_state(state) {
    m_count = m_remaining = static_cast<const UnkEffectCountCommand_80422D30 *>(reinterpret_cast<const UnkEffectCommand_80421834 *>(m_data))->m_count;
    m_done = false;
}

bool EffCmdLoopStartExec::Exec(float dt) {
    if (m_done) {
        EndCommand(this);
        return false;
    }
    return true;
}

EffCmdLoopEndExec::EffCmdLoopEndExec(const u8 *data, u32 *state,
                                     EffectCommandExec *previous, const u8 **jump)
    : EffectCommandExec(data), m_state(state), m_closed(false) {
    EffectCommandExec *exec = previous;
    while (exec != 0) {
        if (dynamic_cast<EffCmdLoopStartExec *>(exec) != 0) {
            EffCmdLoopStartExec *start = dynamic_cast<EffCmdLoopStartExec *>(exec);
            if (start->m_done) {
                exec = exec->m_prev;
            } else {
                if (jump == 0) {
                    start->m_done = true;
                    exec = 0;
                } else if (start->m_count != 0) {
                    if (start->m_remaining == 0) {
                        start->m_done = true;
                        exec = 0;
                    } else {
                        start->m_remaining--;
                        *jump = start->m_data;
                    }
                } else {
                    *jump = start->m_data;
                }
                break;
            }
        } else {
            exec = exec->m_prev;
        }
    }
    if (exec == 0) {
        m_closed = true;
    }
}

bool EffCmdLoopEndExec::Exec(float dt) {
    if (m_closed) {
        EndCommand(this);
    }
    return false;
}

EffCmdCallbackExec::EffCmdCallbackExec(const u8 *data, u32 *state)
    : EffectCommandExec(data), m_state(state) {}

bool EffCmdCallbackExec::Exec(float dt) {
    EndCommand(this);
    return false;
}

EffCmdLaunchExec::EffCmdLaunchExec(const u8 *data, u32 *state,
                                   EffectManager *manager, EffectLaunchInfo *info,
                                   EffectElement *parent)
    : EffectCommandExec(data), m_state(state), m_elapsed(0.0f), m_child(), m_parent(parent) {
    const UnkEffectLaunchCommand_80422750 *c =
        static_cast<const UnkEffectLaunchCommand_80422750 *>(reinterpret_cast<const UnkEffectCommand_80421834 *>(m_data));
    if (c->m_delay != 0) {
        m_delay = c->m_delay;
        if (c->m_delayRange != 0) {
            m_delay += c->m_delayRange * EffectRandom();
        }
    } else {
        m_delay = 0.0f;
    }
    if (c->m_sharedRandom) {
        float random = EffectRandom();
        m_offset.x = c->m_positionBase.x + c->m_positionRange.x * random;
        m_offset.y = c->m_positionBase.y + c->m_positionRange.y * random;
        m_offset.z = c->m_positionBase.z + c->m_positionRange.z * random;
        random = EffectRandom();
        m_angles.x = c->m_rotationBase.x + c->m_rotationRange.x * random;
        m_angles.y = c->m_rotationBase.y + c->m_rotationRange.y * random;
        m_angles.z = c->m_rotationBase.z + c->m_rotationRange.z * random;
        random = EffectRandom();
        m_scale.x = c->m_scaleBase.x + c->m_scaleRange.x * random;
        m_scale.y = c->m_scaleBase.y + c->m_scaleRange.y * random;
        m_scale.z = c->m_scaleBase.z + c->m_scaleRange.z * random;
    } else {
        m_offset.x = c->m_positionBase.x + c->m_positionRange.x * EffectRandom();
        m_offset.y = c->m_positionBase.y + c->m_positionRange.y * EffectRandom();
        m_offset.z = c->m_positionBase.z + c->m_positionRange.z * EffectRandom();
        m_angles.x = c->m_rotationBase.x + c->m_rotationRange.x * EffectRandom();
        m_angles.y = c->m_rotationBase.y + c->m_rotationRange.y * EffectRandom();
        m_angles.z = c->m_rotationBase.z + c->m_rotationRange.z * EffectRandom();
        m_scale.x = c->m_scaleBase.x + c->m_scaleRange.x * EffectRandom();
        m_scale.y = c->m_scaleBase.y + c->m_scaleRange.y * EffectRandom();
        m_scale.z = c->m_scaleBase.z + c->m_scaleRange.z * EffectRandom();
    }
    m_child = manager->Launch(info, c->m_id);
    boost::shared_ptr<EffectElement> element(m_child.lock());
    if (element) {
        element->SetSpeed(m_parent->GetSpeed());
    }
}

EffCmdLaunchExec::~EffCmdLaunchExec() {
    boost::shared_ptr<EffectElement> element(m_child.lock());
    if (element) {
        element->Retire();
    }
}

bool EffCmdLaunchExec::Exec(float dt) {
    boost::shared_ptr<EffectElement> element(m_child.lock());
    if (!element || !element->IsActive()) {
        EndCommand(this);
        return false;
    }
    EffectElement *parent = m_parent;
    if (parent->IsStopped() && !element->IsStopped()) {
        element->Stop();
    }
    if (parent->IsHidden() && !element->IsHidden()) {
        element->Hide();
    } else if (!parent->IsHidden() && element->IsHidden()) {
        element->Show();
    }
    if (parent->IsPaused() && !element->IsPaused()) {
        element->Pause();
    } else if (!parent->IsPaused() && element->IsPaused()) {
        element->ClearFlag4();
    }
    element->SetSpeed(m_parent->GetSpeed());
    if (!field_0x10) {
        field_0x10 = 1;
        fn_8040C148(&m_matrix, &parent->m_worldMatrix);
        fn_8040B59C(&m_matrix, &m_offset);
        fn_8040B80C(&m_matrix, m_angles.z);
        fn_8040B848(&m_matrix, m_angles.y);
        fn_8040B884(&m_matrix, m_angles.x);
        fn_8040B790(&m_matrix, &m_scale);
        if (ParentConfig(parent) & 0x800) {
            element->SetMatrix(&m_matrix);
        } else {
            fn_80425E68(element.get(), &m_matrix);
        }
        return true;
    }
    if (element->IsPaused()) {
        return true;
    }
    m_elapsed += dt;
    if (m_delay > 0.0001f && m_elapsed > m_delay) {
        const UnkEffectLaunchCommand_80422750 *c =
            static_cast<const UnkEffectLaunchCommand_80422750 *>(reinterpret_cast<const UnkEffectCommand_80421834 *>(m_data));
        if (c->m_opcode == 0x12 && c->m_stopOnEnd != 0 && !element->IsStopped()) {
            element->Stop();
        }
        if (!element->IsStopped()) {
            EndCommand(this);
            return false;
        }
    }
    if (ParentConfig(parent) & 0x800) {
        fn_8040C148(&m_matrix, &parent->m_worldMatrix);
        fn_8040B59C(&m_matrix, &m_offset);
        fn_8040B80C(&m_matrix, m_angles.z);
        fn_8040B848(&m_matrix, m_angles.y);
        fn_8040B884(&m_matrix, m_angles.x);
        fn_8040B790(&m_matrix, &m_scale);
    }
    return true;
}

EffCmdRequestStopExec::EffCmdRequestStopExec(const u8 *data, u32 *state,
                                             EffectElement *element)
    : EffectCommandExec(data), m_state(state) {
    if (element != 0) {
        element->Stop();
    }
}

bool EffCmdRequestStopExec::Exec(float dt) {
    EndCommand(this);
    return false;
}

int EffectCommandExec::CountCommands(const u8 *data) {
    int count = 0;
    while (data != 0) {
        u32 size = 0;
        switch (*data) {
        case 1:
            size = 0x10;
            break;
        case 3:
            size = 0x24;
            break;
        case 4:
            size = 0x24;
            break;
        case 5:
            size = 0x14;
            break;
        case 6:
            size = 0x24;
            break;
        case 7:
            size = 0x1C;
            break;
        case 8:
            size = 0x24;
            break;
        case 9:
            size = 0x14;
            break;
        case 10:
            size = 0x14;
            break;
        case 11:
            size = 0x14;
            break;
        case 12:
            size = 0x10;
            break;
        case 13:
            size = 8;
            break;
        case 14:
            size = 8;
            break;
        case 15:
            size = 4;
            break;
        case 16:
            size = 8;
            break;
        case 17:
            size = 0x58;
            break;
        case 18:
            size = 0x5C;
            break;
        case 19:
            size = 4;
            break;
        case 20:
            size = 0x14;
            break;
        case 21:
            size = 0x20;
            break;
        case 22:
            size = 0x14;
            break;
        case 23:
            size = 0x10;
            break;
        case 24:
            size = 0x14;
            break;
        case 25:
            size = 0x10;
            break;
        case 26:
            size = 0x14;
            break;
        case 27:
            size = 0x14;
            break;
        case 28:
            size = 0x1C;
            break;
        case 29:
            size = 0x1C;
            break;
        case 30:
            size = 0x1C;
            break;
        case 31:
            size = 0x14;
            break;
        case 32:
            size = 0x14;
            break;
        case 33:
            size = 0x14;
            break;
        }
        if (data[1] != 0 && size != 0) {
            data += size;
        } else {
            data = 0;
        }
        count++;
    }
    return count;
}

EffCmdRequestStopExec::~EffCmdRequestStopExec() {}

} // namespace System
} // namespace Effect
