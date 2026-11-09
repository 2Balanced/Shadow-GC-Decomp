/* Effect::System::EffectRay (class name from RTTI): the portable part of the ray element. It emits
 * rays (Effect::System::EffectRayExecUnit) as Game/EffectParticle.cpp's element emits particles,
 * ages, moves and frees them, and runs both its own command list and each ray's (the width,
 * length, top and last alpha and colour commands are this unit's; their constructors are inline
 * and their destructors and vtables come out here). File name from the class; member names are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first).
 *
 * NonMatching: 13 of 16 functions match. Generate (89%) has the float/int conversion stack-slot
 * difference of docs/research-notes.md and the float-constant base register that
 * Game/EffectParticle.cpp's Generate has; the constructor (90%) loads the description fields in
 * another order, the scheduling difference of Game/EffectShimmer.cpp's and
 * Game/EffectLensFlare.cpp's constructors; Move (99.8%) has the register difference of
 * Game/EffectParticle.cpp's Move. The unit's .data also holds the GenQuantity and Friction
 * command vtables (weak copies; retail links the particle unit's). */
#define EFFECT_ELEMENT_POLYMORPHIC
#define EFFECT_ELEMENT_FULL
#define EFFECT_SYSTEM_FULL

#include "Effect/System/EffectCommandExec.h"
#include "Effect/System/EffectDesc.h"
#include "Effect/System/EffectPool.h"
#include "Effect/System/EffectRay.h"
#include "Game/MathTypes.h"

/* The float overloads of <cmath>, out of line elsewhere in the game (sin__3stdFf, cos__3stdFf). */
namespace std {
float sin(float x);
float cos(float x);
} // namespace std

extern "C" {
Vec3f fn_800091A4(const Vec3f *vector, float scale);             /* vector * scale */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);             /* vector += value */
void fn_8040CE38(Vec3f *vector, float scale);                    /* vector *= scale */
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);     /* matrix * point */
Vec3f fn_8040B618(const Mtx34f *matrix, const Vec3f *direction); /* matrix * direction */
}

namespace Effect {
namespace System {

/* The shape rays are emitted from (as a particle element's; layout from Generate). */
struct UnkRayShape_804298D4 {
    /* 0x00 */ s32 m_type; /* 0: ring, 1: sphere, 2: box, other: the element's position */
    /* 0x04 */ u32 m_flags;
    /* 0x08 */ float m_radius;
    /* 0x0C */ float m_spread;
    /* 0x10 */ float m_angle;
    /* 0x14 */ float m_height;
};

/* A ray element's description (layout from the constructor, Generate and Move). */
struct UnkRayDesc_804298D4 {
    /* 0x00 */ u8 field_0x0[0x2C];
    /* 0x2C */ u32 m_rayFlags;
    /* 0x30 */ float m_speed;
    /* 0x34 */ float m_speedRange; /* added times a random factor */
    /* 0x38 */ float m_lifetime;
    /* 0x3C */ float m_length;
    /* 0x40 */ float m_width;
    /* 0x44 */ float m_friction;
    /* 0x48 */ u32 m_kindFlags;
    /* 0x4C */ u8 field_0x4C[0x50 - 0x4C];
    /* 0x50 */ u8 m_alpha;
    /* 0x51 */ u8 m_red;
    /* 0x52 */ u8 m_green;
    /* 0x53 */ u8 m_blue;
    /* 0x54 */ const u8 *m_rayCommands; /* each ray's command list */
    /* 0x58 */ u32 m_flags;
    /* 0x5C */ s16 m_count;        /* the most rays at once */
    /* 0x5E */ s16 m_commandCount; /* executors that can run at once; 0 to count them */
    /* 0x60 */ float m_duration;
    /* 0x64 */ const u8 *m_commands; /* the element's command list */
    /* 0x68 */ float m_rate;
    /* 0x6C */ const UnkRayShape_804298D4 *m_shape;
};

} // namespace System
} // namespace Effect

/* The effect system's seventh virtual returns a random float, 0 <= result < 1 (EffectSystemUSA's
 * override calls fn_8040EE90, Game/Random.h), while Effect/System/EffectSystem.h declares that
 * slot as Flush(void *). This view of the class's virtuals (not a class of its own; it is never
 * defined or instantiated) lets the unit call it as the game does. */
class UnkEffectSystemView_8042A5D0 {
  public:
    virtual void *Initialize(void *heap, u32 size) = 0;
    virtual void *Finalize() = 0;
    virtual void *Allocate(u32 size) = 0;
    virtual void Free(void *memory) = 0;
    virtual void Fill(void *memory, char value, u32 size) = 0;
    virtual void Copy(void *to, const void *from, u32 size) = 0;
    virtual float Random() = 0;
};

inline float RandomFloat() {
    return reinterpret_cast<UnkEffectSystemView_8042A5D0 *>(Effect::System::fn_8042A5D0())
        ->Random();
}

/* dst = src through the game's out-of-line Vec3f assignment. */
inline void AssignVector(Vec3f &dst, const Vec3f &src) {
    fn_800091D0(&dst, &src);
}

namespace Effect {
namespace System {

class EffCmdWidthExec : public EffCmdSingleExec {
  public:
    EffCmdWidthExec(const u8 *data, u32 *state, float *target)
        : EffCmdSingleExec(data, state, target) {}
    virtual ~EffCmdWidthExec();
};

class EffCmdLengthExec : public EffCmdSingleExec {
  public:
    EffCmdLengthExec(const u8 *data, u32 *state, float *target)
        : EffCmdSingleExec(data, state, target) {}
    virtual ~EffCmdLengthExec();
};

/* The six command classes' destructors are not inline (the top and last ones are declared in
 * Effect/System/EffectCommandExec.h, where Game/EffectLocus.cpp uses them), so their vtables
 * come out here. */
EffCmdTopColorExec::~EffCmdTopColorExec() {}

EffCmdTopAlphaExec::~EffCmdTopAlphaExec() {}

EffCmdLastColorExec::~EffCmdLastColorExec() {}

EffCmdLastAlphaExec::~EffCmdLastAlphaExec() {}

EffCmdLengthExec::~EffCmdLengthExec() {}

EffCmdWidthExec::~EffCmdWidthExec() {}

EffectRayExecUnit::EffectRayExecUnit(const UnkRayDesc_804298D4 *desc,
                                     UnkEffectPool_8042A344 *pool)
    : m_state(0), m_age(0.0f), m_length(desc->m_length), m_width(desc->m_width), m_prev(0),
      m_next(0), m_cursor(0), m_commandHead(0), m_commandTail(0), m_pool(pool) {
    m_color[0] = (desc->m_red & 0xFF) << 22;
    m_color[1] = (desc->m_green & 0xFF) << 22;
    m_color[2] = (desc->m_blue & 0xFF) << 22;
    m_color[3] = (desc->m_alpha & 0xFF) << 22;
    m_topColor[0] = m_topColor[1] = m_topColor[2] = m_topColor[3] = 0xFF << 22;
    m_lastColor[0] = m_lastColor[1] = m_lastColor[2] = m_lastColor[3] = 0xFF << 22;
}

EffectRayExecUnit::~EffectRayExecUnit() {
    EffectCommandExec *exec = m_commandHead;
    EffectCommandExec *next;
    UnkEffectPool_8042A344 *previous = fn_8042A5D0()->SetPool(m_pool);
    while (exec != 0) {
        next = exec->m_next;
        delete exec;
        exec = next;
    }
    fn_8042A5D0()->SetPool(previous);
}

void EffectRayExecUnit::ParseCommands() {
    while (m_cursor != 0) {
        if (m_commandTail != 0 && dynamic_cast<EffCmdWaitExec *>(m_commandTail) != 0) {
            return;
        }
        EffectCommandExec *exec = 0;
        u32 length = 0;
        switch (m_cursor[0]) {
        case 3:
            exec = new EffCmd3DPosExec(m_cursor, &m_state, &m_start);
            length = 0x24;
            break;
        case 8:
            exec = new EffCmd3DVelExec(m_cursor, &m_state, &m_velocity);
            length = 0x24;
            break;
        case 0xB:
            exec = new EffCmdMatColorExec(m_cursor, &m_state, reinterpret_cast<u32 *>(m_color));
            length = 0x14;
            break;
        case 0xC:
            exec = new EffCmdMatAlphaExec(m_cursor, &m_state, reinterpret_cast<u32 *>(&m_color[3]));
            length = 0x10;
            break;
        case 0x16:
            exec = new EffCmdTopColorExec(m_cursor, &m_state, reinterpret_cast<u32 *>(m_topColor));
            length = 0x14;
            break;
        case 0x17:
            exec = new EffCmdTopAlphaExec(m_cursor, &m_state, reinterpret_cast<u32 *>(&m_topColor[3]));
            length = 0x10;
            break;
        case 0x18:
            exec = new EffCmdLastColorExec(m_cursor, &m_state, reinterpret_cast<u32 *>(m_lastColor));
            length = 0x14;
            break;
        case 0x19:
            exec = new EffCmdLastAlphaExec(m_cursor, &m_state, reinterpret_cast<u32 *>(&m_lastColor[3]));
            length = 0x10;
            break;
        case 0xD:
            exec = new EffCmdWaitExec(m_cursor, &m_state, m_commandTail);
            length = 8;
            break;
        case 0xE:
            exec = new EffCmdLoopStartExec(m_cursor, &m_state);
            length = 8;
            break;
        case 0xF: {
            const u8 *jump = 0;
            exec = new EffCmdLoopEndExec(m_cursor, &m_state, m_commandTail, &jump);
            if (jump != 0) {
                m_cursor = jump;
                length = 8;
            } else {
                length = 4;
            }
            break;
        }
        case 0x10:
            exec = new EffCmdCallbackExec(m_cursor, &m_state);
            length = 8;
            break;
        case 0x1A:
            exec = new EffCmdLengthExec(m_cursor, &m_state, &m_length);
            length = 0x14;
            break;
        case 0x1B:
            exec = new EffCmdWidthExec(m_cursor, &m_state, &m_width);
            length = 0x14;
            break;
        }
        if (exec != 0) {
            if (m_commandTail != 0) {
                m_commandTail->m_next = exec;
                exec->m_prev = m_commandTail;
                m_commandTail = exec;
            } else {
                m_commandTail = exec;
                m_commandHead = exec;
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

EffectRay::EffectRay(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,
                     EffectManager *manager)
    : EffectElement(desc, info, manager), m_head(0), m_tail(0), m_request(0), m_config(0),
      m_activeCount(0), m_friction(0.0f), m_cursor(0), m_commandHead(0), m_commandTail(0), m_commandPool(0),
      m_unitPool(0) {
    m_size -= sizeof(EffectElement);
    m_size += sizeof(EffectRay);
    if (!(m_flags & 1)) {
        return;
    }
    const UnkRayDesc_804298D4 *params = reinterpret_cast<const UnkRayDesc_804298D4 *>(desc);
    m_unitCount = params->m_count;
    m_rate = params->m_rate;
    m_config |= params->m_flags;
    m_config |= params->m_rayFlags;
    m_config |= params->m_kindFlags;
    if (params->m_shape != 0) {
        m_config |= params->m_shape->m_flags;
    }
    if (m_config & 0x400) {
        m_friction = params->m_friction;
    }
    m_unitPool = new UnkEffectPool_8042A344(0x80, m_unitCount);
    if (m_unitPool == 0 || (m_unitCount != 0 && m_unitPool->GetSize() == 0)) {
        m_flags &= ~1;
        return;
    }
    m_size += m_unitPool->GetSize() + sizeof(UnkEffectPool_8042A344);
    s32 commandCount = params->m_commandCount;
    if (commandCount == 0) {
        commandCount = m_unitCount * EffectCommandExec::CountCommands(params->m_rayCommands);
        commandCount += EffectCommandExec::CountCommands(params->m_commands);
    }
    m_commandPool = new UnkEffectPool_8042A344(0x30, commandCount);
    if (m_commandPool == 0 || (commandCount != 0 && m_commandPool->GetSize() == 0)) {
        m_flags &= ~1;
        return;
    }
    m_size += m_commandPool->GetSize() + sizeof(UnkEffectPool_8042A344);
    m_cursor = params->m_commands;
}

EffectRay::~EffectRay() {
    fn_8042A5D0()->SetPool(m_unitPool);
    EffectRayExecUnit *ray = m_head;
    while (ray != 0) {
        EffectRayExecUnit *next = ray->m_next;
        delete ray;
        ray = next;
    }
    m_head = 0;
    m_tail = 0;
    EffectCommandExec *exec = m_commandHead;
    fn_8042A5D0()->SetPool(m_commandPool);
    while (exec != 0) {
        EffectCommandExec *next = exec->m_next;
        delete exec;
        exec = next;
    }
    if (m_unitPool != 0) {
        delete m_unitPool;
    }
    if (m_commandPool != 0) {
        delete m_commandPool;
    }
}

void EffectRay::ParseCommands() {
    while (m_cursor != 0) {
        if (m_commandTail != 0 && dynamic_cast<EffCmdWaitExec *>(m_commandTail) != 0) {
            return;
        }
        EffectCommandExec *exec;
        u32 length;
        switch (m_cursor[0]) {
        case 2:
            exec = new EffCmdGlareSWExec(m_cursor, &m_request);
            length = 8;
            break;
        case 3:
            exec = new EffCmd3DPosExec(m_cursor, &m_request, &m_offset);
            length = 0x24;
            break;
        case 4:
            exec = new EffCmd3DRotExec(m_cursor, &m_request, &m_angles);
            length = 0x24;
            break;
        case 6:
            exec = new EffCmd3DSclExec(m_cursor, &m_request, &m_scale);
            length = 0x24;
            break;
        case 8:
            exec = new EffCmd3DVelExec(m_cursor, &m_request, &m_offsetSpeed);
            length = 0x24;
            break;
        case 0xA:
            exec = new EffCmdFrictionExec(m_cursor, &m_request, &m_friction);
            length = 0x14;
            break;
        case 0xD:
            exec = new EffCmdWaitExec(m_cursor, &m_request, m_commandTail);
            length = 8;
            break;
        case 0xE:
            exec = new EffCmdLoopStartExec(m_cursor, &m_request);
            length = 8;
            m_loopDepth++;
            if (m_skipLoops != 0 || (m_flags & 0x10)) {
                m_skipLoops++;
            }
            break;
        case 0xF: {
            const u8 *jump = 0;
            if (m_skipLoops != 0 || (m_flags & 0x10)) {
                exec = new EffCmdLoopEndExec(m_cursor, &m_request, m_commandTail, 0);
                if (m_skipLoops != 0) {
                    m_skipLoops--;
                }
            } else {
                exec = new EffCmdLoopEndExec(m_cursor, &m_request, m_commandTail, &jump);
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
            exec = new EffCmdCallbackExec(m_cursor, &m_request);
            length = 8;
            break;
        case 0x14:
            exec = new EffCmdGenQuantityExec(m_cursor, &m_request, &m_rate);
            length = 0x14;
            break;
        case 0x13:
            exec = new EffCmdRequestStopExec(m_cursor, &m_request, this);
            length = 4;
            break;
        default:
            return;
        }
        if (exec != 0) {
            if (m_commandTail != 0) {
                m_commandTail->m_next = exec;
                exec->m_prev = m_commandTail;
                m_commandTail = exec;
            } else {
                m_commandTail = exec;
                m_commandHead = exec;
            }
        } else {
            m_request |= 1;
            return;
        }
        if (m_cursor[1] != 0 && length != 0) {
            m_cursor += length;
        } else {
            m_cursor = 0;
        }
    }
}

void EffectRay::Generate(float dt) {
    int count = 0;
    float quantity = m_rate * dt;
    const UnkRayDesc_804298D4 *desc = reinterpret_cast<const UnkRayDesc_804298D4 *>(m_desc);
    if (quantity < 1.0f) {
        if (RandomFloat() < quantity) {
            count = 1;
        }
    } else {
        count = (int)quantity;
    }
    if (count <= 0) {
        return;
    }
    for (int i = 0; i < count; i++) {
        if (m_unitCount == m_activeCount) {
            return;
        }
        fn_8042A5D0()->SetPool(m_unitPool);
        EffectRayExecUnit *ray = new EffectRayExecUnit(
            reinterpret_cast<const UnkRayDesc_804298D4 *>(m_desc), m_commandPool);
        if (ray == 0) {
            return;
        }
        m_activeCount++;
        if (m_tail != 0) {
            m_tail->m_next = ray;
            ray->m_prev = m_tail;
            m_tail = ray;
        } else {
            m_tail = ray;
            m_head = ray;
        }
        float velocity = desc->m_speedRange * RandomFloat() + desc->m_speed;
        const UnkRayShape_804298D4 *shape = desc->m_shape;
        switch (shape->m_type) {
        default: {
            AssignVector(ray->m_start, m_lastPosition);
            Vec3f up;
            up.y = velocity;
            AssignVector(ray->m_velocity, up);
            break;
        }
        case 0: {
            float spread = shape->m_spread;
            float angle;
            if ((m_config & 0x40) && count > 1) {
                if (spread > 359.9999f) {
                    angle = spread * ((float)i / (float)count);
                } else {
                    angle = spread * ((float)i / (float)(count - 1));
                }
                angle -= spread * 0.5f;
            } else {
                angle = spread * RandomFloat() - shape->m_spread * 0.5f;
            }
            float radius = 0.0f;
            float tilt = 0.0f;
            if (shape->m_radius > 0.0001f) {
                float r;
                if (m_config & 0x80) {
                    r = shape->m_radius;
                } else {
                    r = shape->m_radius * RandomFloat();
                }
                radius = r;
                tilt = shape->m_angle * (r / shape->m_radius);
            }
            float s = std::sin(angle);
            float c = std::cos(angle);
            float height = 0.0f;
            if (shape->m_height > 0.0001f) {
                height = shape->m_height * RandomFloat();
            }
            ray->m_start.x = s * radius;
            ray->m_start.y = height;
            ray->m_start.z = c * radius;
            float ts = std::sin(tilt);
            float tc = std::cos(tilt);
            ray->m_velocity.x = s * (ts * velocity);
            ray->m_velocity.y = tc * velocity;
            ray->m_velocity.z = c * (ts * velocity);
            break;
        }
        case 1: {
            float spread = shape->m_spread;
            float angle = spread * RandomFloat() - shape->m_spread;
            float radius = 0.0f;
            if (shape->m_radius > 0.0001f) {
                if (m_config & 0x80) {
                    radius = shape->m_radius;
                } else {
                    radius = shape->m_radius * RandomFloat();
                }
            }
            float tilt = shape->m_angle * RandomFloat();
            float ts = std::sin(tilt);
            float tc = std::cos(tilt);
            float x = ts * std::sin(angle);
            float z = ts * std::cos(angle);
            ray->m_start.x = x * radius;
            ray->m_start.y = tc * radius;
            ray->m_start.z = z * radius;
            ray->m_velocity.x = x * velocity;
            ray->m_velocity.y = tc * velocity;
            ray->m_velocity.z = z * velocity;
            if (m_config & 0x20) {
                ray->m_velocity.x = -ray->m_velocity.x;
                ray->m_velocity.y = -ray->m_velocity.y;
                ray->m_velocity.z = -ray->m_velocity.z;
            }
            break;
        }
        case 2: {
            float size = shape->m_radius;
            ray->m_start.x = size * RandomFloat() - 0.5f * size;
            size = shape->m_spread;
            ray->m_start.y = size * RandomFloat() - 0.5f * size;
            size = shape->m_angle;
            ray->m_start.z = size * RandomFloat() - 0.5f * size;
            ray->m_velocity.x = 0.0f;
            ray->m_velocity.y = velocity;
            ray->m_velocity.z = 0.0f;
            break;
        }
        }
        if (!(m_config & 0x800)) {
            AssignVector(ray->m_start, fn_8040B67C(&m_worldMatrix, &ray->m_start));
            AssignVector(ray->m_velocity, fn_8040B618(&m_worldMatrix, &ray->m_velocity));
        }
        AssignVector(ray->m_end, ray->m_start);
        ray->m_cursor = desc->m_rayCommands;
    }
}

void EffectRay::Move(float dt) {
    const UnkRayDesc_804298D4 *desc = reinterpret_cast<const UnkRayDesc_804298D4 *>(m_desc);
    if (!(m_config & 0x100)) {
        float lifetime = 0.0001f + desc->m_lifetime;
        for (EffectRayExecUnit *ray = m_head; ray != 0; ray = ray->m_next) {
            if (m_flags & 2) {
                ray->m_age += dt;
            }
            if (ray->m_age > lifetime) {
                ray->m_state |= 1;
            }
        }
    }
    EffectCommandExec *next;
    EffectCommandExec *exec;
    EffectRayExecUnit *current;
    EffectRayExecUnit *ray = m_head;
    fn_8042A5D0()->SetPool(m_commandPool);
    while (ray != 0) {
        if (!(ray->m_state & 1)) {
            ray->ParseCommands();
            if (!(ray->m_state & 1)) {
                exec = ray->m_commandHead;
                while (exec != 0) {
                    if (!exec->Exec(dt)) {
                        if (exec->m_prev != 0) {
                            exec->m_prev->m_next = exec->m_next;
                        }
                        if (exec->m_next != 0) {
                            exec->m_next->m_prev = exec->m_prev;
                        }
                        if (exec == ray->m_commandHead) {
                            ray->m_commandHead = exec->m_next;
                        }
                        if (exec == ray->m_commandTail) {
                            ray->m_commandTail = exec->m_prev;
                        }
                        next = exec->m_next;
                        delete exec;
                        exec = next;
                    } else {
                        exec = exec->m_next;
                    }
                }
            }
        }
        ray = ray->m_next;
    }
    current = m_head;
    fn_8042A5D0()->SetPool(m_unitPool);
    while (current != 0) {
        EffectRayExecUnit *following = current->m_next;
        if (current->m_state & 1) {
            if (current->m_prev != 0) {
                current->m_prev->m_next = current->m_next;
            }
            if (current->m_next != 0) {
                current->m_next->m_prev = current->m_prev;
            }
            if (current == m_head) {
                m_head = current->m_next;
            }
            if (current == m_tail) {
                m_tail = current->m_prev;
            }
            delete current;
            m_activeCount--;
        }
        current = following;
    }
    for (ray = m_head; ray != 0; ray = ray->m_next) {
        fn_8040CFF8(&ray->m_start, &(const Vec3f &)fn_800091A4(&ray->m_velocity, dt));
        if (m_friction != 0.0f) {
            float length = fn_8040CF50(&ray->m_velocity);
            if (length > 0.0001f) {
                float loss = length * m_friction;
                if (loss > length) {
                    fn_8040CE38(&ray->m_velocity, 0.0f);
                } else {
                    fn_8040CE38(&ray->m_velocity, (length - loss) / length);
                }
            }
        }
    }
}

void EffectRay::Update(float dt) {
    if (!(m_flags & 1)) {
        return;
    }
    if (m_flags & 4) {
        return;
    }
    const UnkRayDesc_804298D4 *desc = reinterpret_cast<const UnkRayDesc_804298D4 *>(m_desc);
    if (m_flags & 2) {
        m_time += dt;
    }
    if (!(m_config & 1) && m_time > 0.0001f + desc->m_duration) {
        m_flags &= ~1;
        return;
    }
    fn_8042A5D0()->SetPool(m_commandPool);
    ParseCommands();
    if (!(m_request & 1)) {
        EffectCommandExec *next;
        EffectCommandExec *exec = m_commandHead;
        while (exec != 0) {
            if (!exec->Exec(dt)) {
                if (exec->m_prev != 0) {
                    exec->m_prev->m_next = exec->m_next;
                }
                if (exec->m_next != 0) {
                    exec->m_next->m_prev = exec->m_prev;
                }
                if (exec == m_commandHead) {
                    m_commandHead = exec->m_next;
                }
                if (exec == m_commandTail) {
                    m_commandTail = exec->m_prev;
                }
                next = exec->m_next;
                delete exec;
                exec = next;
            } else {
                exec = exec->m_next;
            }
        }
    }
    if (m_request & 1) {
        m_flags &= ~1;
        return;
    }
    UpdateTransform(dt);
    if (m_flags & 0x10) {
        if ((m_config & 0x40000) && m_commandHead != 0) {
            Generate(dt);
            Move(dt);
        } else {
            Move(dt);
            if (m_activeCount == 0) {
                m_flags &= ~1;
            }
        }
    } else {
        Generate(dt);
        Move(dt);
    }
}

} // namespace System
} // namespace Effect
