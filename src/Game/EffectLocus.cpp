/* Effect::System::EffectLocus (class name and base from RTTI): the platform-independent trail
 * effect. It samples its matrix into a list of points (Effect::System::EffectLocusExecUnit, also
 * named by RTTI) at a fixed rate, interpolating between samples, lets the points age out, and runs
 * the command stream of its description. Effect::RW3::EffectLocusRW3 draws the points. Member
 * names are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#define EFFECT_ELEMENT_POLYMORPHIC
#define EFFECT_ELEMENT_FULL
#define EFFECT_SYSTEM_FULL

#include "Game/EffectManagerPJS.h"
#include "Effect/System/EffectCommandExec.h"
#include "Effect/System/EffectPool.h"
#include "Game/Quaternion.h"

using ::Effect::System::EffectLaunchInfo;
using ::Effect::System::EffectManager;
using ::Effect::System::UnkEffectDesc_801D0AFC;

/* The part of the description a trail reads (layout from its uses). */
struct UnkLocusDesc_8042C3A0 {
    /* 0x00 */ u8 field_0x0[0x2C];
    /* 0x2C */ u32 m_config0;
    /* 0x30 */ float m_pointLife;  /* frames a point lives */
    /* 0x34 */ s32 m_rate;         /* points per frame if positive, frames per point if negative */
    /* 0x38 */ u32 m_config1;
    /* 0x3C */ u8 field_0x3C[0x40 - 0x3C];
    /* 0x40 */ u8 m_alpha;
    /* 0x41 */ u8 m_red;
    /* 0x42 */ u8 m_green;
    /* 0x43 */ u8 m_blue;
    /* 0x44 */ u32 m_config2;
    /* 0x48 */ s16 m_frames;       /* how many frames of points the trail keeps */
    /* 0x4A */ s16 m_commandCount; /* the most commands that run at once */
    /* 0x4C */ float m_life;       /* frames the effect lives */
    /* 0x50 */ const u8 *m_commands;
    /* 0x54 */ const u8 *m_sections;
};

extern "C" {
/* Square root, a rotation's quaternion, the rotation between two quaternions at t, a
 * quaternion's matrix and a matrix scaled by a vector (behaviour inferred). */
float fn_8000FEE4(float x);
Quatf fn_8040C55C(const Mtx34f *matrix);
Quatf fn_8040C3EC(const Quatf *from, const Quatf *to, float t);
Mtx34f fn_8040B228(const Quatf *rotation);
void fn_8040B790(Mtx34f *matrix, const Vec3f *scale);
/* The out-of-line assignment of a quaternion (as in Game/EffBelt.cpp). */
void fn_8007A34C(Quatf *q, const Quatf *value);
}

namespace Effect {
namespace System {

/* One point of the trail. */
class EffectLocusExecUnit {
  public:
    EffectLocusExecUnit();
    virtual ~EffectLocusExecUnit();

    static void *operator new(unsigned long) {
        return fn_8042A5D0()->PoolAlloc();
    }
    static void operator delete(void *p);

    /* 0x04 */ Mtx34f m_matrix;
    /* 0x34 */ float m_time; /* the point's age in frames */
    /* 0x38 */ EffectLocusExecUnit *m_prev;
    /* 0x3C */ EffectLocusExecUnit *m_next;
};

EffectLocusExecUnit::EffectLocusExecUnit() {
    fn_8040C148(&m_matrix, &lbl_805E0720);
    m_time = 0.0f;
    m_prev = 0;
    m_next = 0;
}

EffectLocusExecUnit::~EffectLocusExecUnit() {}

inline const UnkLocusDesc_8042C3A0 *LocusDesc(const UnkEffectDesc_801D0AFC *desc) {
    return (const UnkLocusDesc_8042C3A0 *)desc;
}

inline UnkLocusDesc_8042C3A0 *MutableLocusDesc(const UnkEffectDesc_801D0AFC *desc) {
    return (UnkLocusDesc_8042C3A0 *)desc;
}

EffectLocus::EffectLocus(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,
                         EffectManager *manager)
    : EffectElement(desc, info, manager), m_head(0), m_tail(0), m_last(0), field_0x108(0),
      m_config(0), m_pointCount(0), m_elapsed(0.0f), m_command(0), m_commandHead(0),
      m_commandTail(0), m_commandPool(0), m_pointPool(0) {
    m_size -= sizeof(EffectElement);
    m_size += sizeof(EffectLocus);
    if (!(m_flags & 1)) {
        return;
    }
    if (MutableLocusDesc(desc)->m_rate < 0) {
        m_maxPoints = MutableLocusDesc(desc)->m_frames / (1 - MutableLocusDesc(desc)->m_rate) + 1;
        m_interval = 1 - MutableLocusDesc(desc)->m_rate;
    } else {
        m_maxPoints = MutableLocusDesc(desc)->m_frames * (MutableLocusDesc(desc)->m_rate + 1) + 1;
        m_interval = 1.0f / (MutableLocusDesc(desc)->m_rate + 1);
    }
    m_config |= MutableLocusDesc(desc)->m_config2;
    m_config |= MutableLocusDesc(desc)->m_config0;
    m_config |= MutableLocusDesc(desc)->m_config1;
    m_pointPool = new UnkEffectPool_8042A344(sizeof(EffectLocusExecUnit), m_maxPoints);
    if (!m_pointPool || (m_maxPoints != 0 && m_pointPool->GetSize() == 0)) {
        m_flags &= ~1;
        return;
    }
    m_size += m_pointPool->GetSize() + sizeof(UnkEffectPool_8042A344);
    int commandCount = MutableLocusDesc(desc)->m_commandCount;
    m_commandPool = new UnkEffectPool_8042A344(0x30, commandCount);
    if (!m_commandPool || (commandCount != 0 && m_commandPool->GetSize() == 0)) {
        m_flags &= ~1;
        return;
    }
    m_size += m_commandPool->GetSize() + sizeof(UnkEffectPool_8042A344);
    m_command = MutableLocusDesc(desc)->m_commands;
    const u8 *section = MutableLocusDesc(desc)->m_sections;
    m_sectionCount = 0;
    while (section) {
        u32 size;
        if (section[0] == 0x15) {
            m_sectionCount++;
            size = 0x20;
        } else {
            size = 0;
        }
        if (section[1] && size) {
            section += size;
        } else {
            section = 0;
        }
    }
    if (m_config & 0x2000) {
        m_sectionCount++;
    }
    if (m_sectionCount < 2) {
        m_flags &= ~1;
        return;
    }
    m_color[0] = (MutableLocusDesc(desc)->m_red & 0xFF) << 22;
    m_color[1] = (MutableLocusDesc(desc)->m_green & 0xFF) << 22;
    m_color[2] = (MutableLocusDesc(desc)->m_blue & 0xFF) << 22;
    m_color[3] = (MutableLocusDesc(desc)->m_alpha & 0xFF) << 22;
    m_topColor[0] = m_topColor[1] = m_topColor[2] = m_topColor[3] = 0xFF << 22;
    m_lastColor[0] = m_lastColor[1] = m_lastColor[2] = m_lastColor[3] = 0xFF << 22;
}

EffectLocus::~EffectLocus() {
    fn_8042A5D0()->SetPool(m_pointPool);
    EffectLocusExecUnit *next;
    for (EffectLocusExecUnit *point = m_head; point; point = next) {
        next = point->m_next;
        delete point;
    }
    m_head = 0;
    m_tail = 0;
    if (m_last) {
        delete m_last;
        m_last = 0;
    }
    EffectCommandExec *command = m_commandHead;
    fn_8042A5D0()->SetPool(m_commandPool);
    while (command) {
        EffectCommandExec *nextCommand = command->m_next;
        delete command;
        command = nextCommand;
    }
    if (m_pointPool) {
        delete m_pointPool;
    }
    if (m_commandPool) {
        delete m_commandPool;
    }
}

void EffectLocus::StartCommands() {
    while (m_command) {
        if (m_commandTail && dynamic_cast<EffCmdWaitExec *>(m_commandTail)) {
            return;
        }
        EffectCommandExec *exec;
        u32 size;
        switch (m_command[0]) {
        case 2:
            exec = new EffCmdGlareSWExec(m_command, &field_0x108);
            size = 8;
            break;
        case 3:
            exec = new EffCmd3DPosExec(m_command, &field_0x108, &m_offset);
            size = 0x24;
            break;
        case 4:
            exec = new EffCmd3DRotExec(m_command, &field_0x108, &m_angles);
            size = 0x24;
            break;
        case 6:
            exec = new EffCmd3DSclExec(m_command, &field_0x108, &m_scale);
            size = 0x24;
            break;
        case 8:
            exec = new EffCmd3DVelExec(m_command, &field_0x108, &m_offsetSpeed);
            size = 0x24;
            break;
        case 11:
            exec = new EffCmdMatColorExec(m_command, &field_0x108, m_color);
            size = 0x14;
            break;
        case 12:
            exec = new EffCmdMatAlphaExec(m_command, &field_0x108, &m_color[3]);
            size = 0x10;
            break;
        case 22:
            exec = new EffCmdTopColorExec(m_command, &field_0x108, m_topColor);
            size = 0x14;
            break;
        case 23:
            exec = new EffCmdTopAlphaExec(m_command, &field_0x108, &m_topColor[3]);
            size = 0x10;
            break;
        case 24:
            exec = new EffCmdLastColorExec(m_command, &field_0x108, m_lastColor);
            size = 0x14;
            break;
        case 25:
            exec = new EffCmdLastAlphaExec(m_command, &field_0x108, &m_lastColor[3]);
            size = 0x10;
            break;
        case 13:
            exec = new EffCmdWaitExec(m_command, &field_0x108, m_commandTail);
            size = 8;
            break;
        case 14:
            exec = new EffCmdLoopStartExec(m_command, &field_0x108);
            size = 8;
            m_loopDepth++;
            if (m_skipLoops != 0 || (m_flags & 0x10)) {
                m_skipLoops++;
            }
            break;
        case 15: {
            const u8 *jump = 0;
            if (m_skipLoops != 0 || (m_flags & 0x10)) {
                exec = new EffCmdLoopEndExec(m_command, &field_0x108, m_commandTail, 0);
                if (m_skipLoops != 0) {
                    m_skipLoops--;
                }
            } else {
                exec = new EffCmdLoopEndExec(m_command, &field_0x108, m_commandTail, &jump);
            }
            if (jump) {
                m_command = jump;
                size = 8;
            } else {
                m_loopDepth--;
                size = 4;
            }
            break;
        }
        case 16:
            exec = new EffCmdCallbackExec(m_command, &field_0x108);
            size = 8;
            break;
        case 19:
            exec = new EffCmdRequestStopExec(m_command, &field_0x108, this);
            size = 4;
            break;
        default:
            return;
        }
        if (exec) {
            if (m_commandTail) {
                m_commandTail->m_next = exec;
                exec->m_prev = m_commandTail;
                m_commandTail = exec;
            } else {
                m_commandTail = exec;
                m_commandHead = exec;
            }
        } else {
            field_0x108 |= 1;
            return;
        }
        if (m_command[1] && size) {
            m_command += size;
        } else {
            m_command = 0;
        }
    }
}

void EffectLocus::AddPoints(float dt) {
    Mtx34f matrix;
    if (m_config & 0x800) {
        fn_8040C148(&matrix, &lbl_805E0720);
    } else {
        fn_8040C148(&matrix, &m_worldMatrix);
    }
    Quatf fromRotation;
    Quatf rotation;
    fn_8007A34C(&rotation, &(const Quatf &)fn_8040C55C(&matrix));
    float scaleX = fn_8000FEE4(matrix.values[0][0] * matrix.values[0][0] + matrix.values[1][0] * matrix.values[1][0] +
                               matrix.values[2][0] * matrix.values[2][0]);
    float scaleY = fn_8000FEE4(matrix.values[0][1] * matrix.values[0][1] + matrix.values[1][1] * matrix.values[1][1] +
                               matrix.values[2][1] * matrix.values[2][1]);
    float scaleZ = fn_8000FEE4(matrix.values[0][2] * matrix.values[0][2] + matrix.values[1][2] * matrix.values[1][2] +
                               matrix.values[2][2] * matrix.values[2][2]);
    fn_8042A5D0()->SetPool(m_pointPool);
    if (!m_last) {
        m_last = new EffectLocusExecUnit;
        fn_8040C148(&m_last->m_matrix, &matrix);
        EffectLocusExecUnit *point = new EffectLocusExecUnit;
        m_tail = point;
        m_head = point;
        fn_8040C148(&point->m_matrix, &matrix);
        point->m_time = m_elapsed;
        m_elapsed = 0.0f;
        m_pointCount++;
        m_addedCount++;
        return;
    }
    m_elapsed += dt;
    while (m_elapsed > m_interval) {
        EffectLocusExecUnit *point = new EffectLocusExecUnit;
        if (!point) {
            EffectLocusExecUnit *oldest = m_head;
            m_head = oldest->m_next;
            if (m_head) {
                m_head->m_prev = 0;
            }
            delete oldest;
            m_pointCount--;
            m_removedCount++;
            point = new EffectLocusExecUnit;
            if (!point) {
                break;
            }
        }
        Mtx34f *from;
        if (m_tail) {
            from = &m_tail->m_matrix;
        } else {
            from = &m_last->m_matrix;
        }
        m_pointCount++;
        m_addedCount++;
        if (m_tail) {
            m_tail->m_next = point;
            point->m_prev = m_tail;
            m_tail = point;
        } else {
            m_tail = point;
            m_head = point;
        }
        float t = m_interval / m_elapsed;
        fn_8007A34C(&fromRotation, &(const Quatf &)fn_8040C55C(from));
        Vec3f scale;
        scale.x = fn_8000FEE4(from->values[0][0] * from->values[0][0] + from->values[1][0] * from->values[1][0] +
                              from->values[2][0] * from->values[2][0]);
        scale.y = fn_8000FEE4(from->values[0][1] * from->values[0][1] + from->values[1][1] * from->values[1][1] +
                              from->values[2][1] * from->values[2][1]);
        scale.z = fn_8000FEE4(from->values[0][2] * from->values[0][2] + from->values[1][2] * from->values[1][2] +
                              from->values[2][2] * from->values[2][2]);
        fn_8040C148(&point->m_matrix,
                    &(const Mtx34f &)fn_8040B228(
                        &(const Quatf &)fn_8040C3EC(&fromRotation, &rotation, t)));
        float s = 1.0f - t;
        point->m_matrix.values[0][3] = s * from->values[0][3] + t * matrix.values[0][3];
        point->m_matrix.values[1][3] = s * from->values[1][3] + t * matrix.values[1][3];
        point->m_matrix.values[2][3] = s * from->values[2][3] + t * matrix.values[2][3];
        scale.x = s * scale.x + t * scaleX;
        scale.y = s * scale.y + t * scaleY;
        scale.z = s * scale.z + t * scaleZ;
        fn_8040B790(&point->m_matrix, &scale);
        m_elapsed -= m_interval;
        point->m_time = m_elapsed;
    }
    fn_8040C148(&m_last->m_matrix, &matrix);
}

/* Removes point from the trail and frees it. */
inline void EffectLocus_RemovePoint(EffectLocus *locus, EffectLocusExecUnit *point) {
    if (point->m_prev) {
        point->m_prev->m_next = point->m_next;
    }
    if (point->m_next) {
        point->m_next->m_prev = point->m_prev;
    }
    if (point == locus->m_head) {
        locus->m_head = point->m_next;
    }
    if (point == locus->m_tail) {
        locus->m_tail = point->m_prev;
    }
    delete point;
    locus->m_pointCount--;
    locus->m_removedCount++;
}

void EffectLocus::Update(float dt) {
    if (!(m_flags & 1)) {
        return;
    }
    if (m_flags & 4) {
        return;
    }
    m_addedCount = 0;
    m_removedCount = 0;
    const UnkLocusDesc_8042C3A0 *desc = LocusDesc(m_desc);
    if (m_flags & 2) {
        m_time += dt;
    }
    if (!(m_config & 1) && m_time > 0.0001f + desc->m_life) {
        m_flags &= ~1;
        return;
    }
    fn_8042A5D0()->SetPool(m_commandPool);
    StartCommands();
    if (!(field_0x108 & 1)) {
        EffectCommandExec *next;
        EffectCommandExec *command = m_commandHead;
        while (command) {
            if (!command->Exec(dt)) {
                if (command->m_prev) {
                    command->m_prev->m_next = command->m_next;
                }
                if (command->m_next) {
                    command->m_next->m_prev = command->m_prev;
                }
                if (command == m_commandHead) {
                    m_commandHead = command->m_next;
                }
                if (command == m_commandTail) {
                    m_commandTail = command->m_prev;
                }
                next = command->m_next;
                delete command;
                command = next;
            } else {
                command = command->m_next;
            }
        }
    }
    if (field_0x108 & 1) {
        m_flags &= ~1;
        return;
    }
    UpdateTransform(dt);
    if (m_flags & 0x10) {
        if ((m_config & 0x40000) && m_commandHead) {
            AddPoints(dt);
            desc = LocusDesc(m_desc);
            if (!(m_config & 0x100)) {
                float life = 0.0001f + desc->m_pointLife;
                EffectLocusExecUnit *point = m_head;
                while (point) {
                    EffectLocusExecUnit *next = point->m_next;
                    if (m_flags & 2) {
                        point->m_time += dt;
                    }
                    if (point->m_time > life) {
                        EffectLocus_RemovePoint(this, point);
                    }
                    point = next;
                }
            }
            return;
        }
        if ((m_config & 0x100) && m_last && m_head) {
            m_elapsed += dt;
            fn_8042A5D0()->SetPool(m_pointPool);
            while (m_elapsed > m_interval && m_head) {
                EffectLocusExecUnit *oldest = m_head;
                m_head = oldest->m_next;
                if (m_head) {
                    m_head->m_prev = 0;
                }
                delete oldest;
                m_pointCount--;
                m_removedCount++;
                m_elapsed -= m_interval;
            }
            if (!m_head) {
                m_tail = 0;
            }
        }
        desc = LocusDesc(m_desc);
        if (!(m_config & 0x100)) {
            float life = 0.0001f + desc->m_pointLife;
            EffectLocusExecUnit *point = m_head;
            while (point) {
                EffectLocusExecUnit *next = point->m_next;
                if (m_flags & 2) {
                    point->m_time += dt;
                }
                if (point->m_time > life) {
                    EffectLocus_RemovePoint(this, point);
                }
                point = next;
            }
        }
        if (m_pointCount == 0) {
            m_flags &= ~1;
        }
        return;
    }
    AddPoints(dt);
    desc = LocusDesc(m_desc);
    if (!(m_config & 0x100)) {
        float life = 0.0001f + desc->m_pointLife;
        EffectLocusExecUnit *point = m_head;
        while (point) {
            EffectLocusExecUnit *next = point->m_next;
            if (m_flags & 2) {
                point->m_time += dt;
            }
            if (point->m_time > life) {
                EffectLocus_RemovePoint(this, point);
            }
            point = next;
        }
    }
}

inline void EffectLocusExecUnit::operator delete(void *p) {
    fn_8042A5D0()->PoolFree(p);
}

} // namespace System
} // namespace Effect
