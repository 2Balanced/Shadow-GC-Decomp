/* Effect::System::EffectParticle (class name from RTTI): the portable part of the particle element.
 * It emits particles (Effect::System::EffectParticleExecUnit) from a ring, a sphere, a box or its
 * own position at the description's rate, ages, moves and frees them, and runs both its own
 * command list and each particle's (the 2D rotation, generation quantity, friction and gravity
 * commands are this unit's; their constructors are inline and their destructors and vtables come
 * out here). File name from the class; member names are inferred. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first).
 *
 * NonMatching: 13 of 15 functions match. Generate (88%) has the float/int conversion stack-slot
 * difference of docs/research-notes.md (retail shares one slot, GC/2.7 gives each conversion its
 * own and the frame grows by 0x10), and GC/2.7 also keeps the float constants' base address in a
 * register where retail loads each constant with its own lis. Move (99%) differs only in the
 * register of its last loop's particle (r30 for retail's r29; about 20 declaration shapes
 * tried). */
#define EFFECT_ELEMENT_POLYMORPHIC
#define EFFECT_ELEMENT_FULL
#define EFFECT_SYSTEM_FULL

#include "Effect/System/EffectCommandExec.h"
#include "Effect/System/EffectDesc.h"
#include "Effect/System/EffectParticle.h"
#include "Effect/System/EffectPool.h"
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
float fn_8040CF90(const Vec3f *a, const Vec3f *b);               /* a . b */
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);     /* matrix * point */
Vec3f fn_8040B618(const Mtx34f *matrix, const Vec3f *direction); /* matrix * direction */
}

namespace Effect {
namespace System {

/* What a particle starts with (layout from the element's constructor; at 0x2C of the
 * description). */
struct UnkParticleParams_80427EC8 {
    /* 0x00 */ u32 m_flags;
    /* 0x04 */ float m_speed;
    /* 0x08 */ float m_speedRange; /* added times a random factor */
    /* 0x0C */ float m_lifetime;
    /* 0x10 */ float m_scale;
    /* 0x14 */ float m_gravity;
    /* 0x18 */ float m_friction;
    /* 0x1C */ const u8 *m_commands; /* each particle's command list */
};

/* The shape particles are emitted from (layout from Generate). */
struct UnkParticleShape_80427EC8 {
    /* 0x00 */ s32 m_type; /* 0: ring, 1: sphere, 2: box, other: the element's position */
    /* 0x04 */ u32 m_flags;
    /* 0x08 */ float m_radius;
    /* 0x0C */ float m_spread;
    /* 0x10 */ float m_angle;
    /* 0x14 */ float m_height;
};

/* How the element emits (layout from the constructor). */
struct UnkParticleEmitter_80427EC8 {
    /* 0x00 */ u32 m_flags;
    /* 0x04 */ s16 m_count;        /* the most particles at once */
    /* 0x06 */ s16 m_commandCount; /* executors that can run at once; 0 to count them */
    /* 0x08 */ float m_duration;
    /* 0x0C */ const u8 *m_commands; /* the element's command list */
    /* 0x10 */ float m_rate;
    /* 0x14 */ const UnkParticleShape_80427EC8 *m_shape;
};

/* The two particle descriptions (types 2 and 5; layouts from the constructors). */
struct UnkParticleDesc_80427EC8 {
    /* 0x00 */ u8 field_0x0[0x2C];
    /* 0x2C */ UnkParticleParams_80427EC8 m_params;
    /* 0x4C */ u32 m_flags;
    /* 0x50 */ u8 field_0x50[0x54 - 0x50];
    /* 0x54 */ u8 m_alpha;
    /* 0x55 */ u8 m_red;
    /* 0x56 */ u8 m_green;
    /* 0x57 */ u8 m_blue;
    /* 0x58 */ u8 field_0x58[0x5C - 0x58];
    /* 0x5C */ UnkParticleEmitter_80427EC8 m_emitter;
};

struct UnkParticle3DDesc_80427EC8 {
    /* 0x00 */ u8 field_0x0[0x2C];
    /* 0x2C */ UnkParticleParams_80427EC8 m_params;
    /* 0x4C */ u32 m_flags;
    /* 0x50 */ u8 field_0x50[0x54 - 0x50];
    /* 0x54 */ u8 m_alpha;
    /* 0x55 */ u8 m_red;
    /* 0x56 */ u8 m_green;
    /* 0x57 */ u8 m_blue;
    /* 0x58 */ u8 field_0x58[0x60 - 0x58];
    /* 0x60 */ UnkParticleEmitter_80427EC8 m_emitter;
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

class EffCmd2DRotExec : public EffCmdSingleExec {
  public:
    EffCmd2DRotExec(const u8 *data, u32 *state, float *target)
        : EffCmdSingleExec(data, state, target) {}
};

class EffCmdGravityExec : public EffCmdSingleExec {
  public:
    EffCmdGravityExec(const u8 *data, u32 *state, float *target)
        : EffCmdSingleExec(data, state, target) {}
};

EffectParticleExecUnit::EffectParticleExecUnit(const UnkParticleDesc_80427EC8 *desc,
                                               UnkEffectPool_8042A344 *pool)
    : m_rotation(0.0f, 0.0f, 0.0f),
      m_scale(desc->m_params.m_scale, desc->m_params.m_scale, desc->m_params.m_scale) {
    m_uv0[0] = 0.0f;
    m_uv0[1] = 0.0f;
    field_0x3C = 0.0f;
    m_uv1[0] = 1.0f;
    m_uv1[1] = 1.0f;
    field_0x48 = 1.0f;
    m_state = 0;
    m_age = 0.0f;
    m_prev = 0;
    m_next = 0;
    m_cursor = 0;
    m_commandHead = 0;
    m_commandTail = 0;
    m_pool = pool;
    m_color[0] = (desc->m_red & 0xFF) << 22;
    m_color[1] = (desc->m_green & 0xFF) << 22;
    m_color[2] = (desc->m_blue & 0xFF) << 22;
    m_color[3] = (desc->m_alpha & 0xFF) << 22;
}

EffectParticleExecUnit::EffectParticleExecUnit(const UnkParticle3DDesc_80427EC8 *desc,
                                               UnkEffectPool_8042A344 *pool)
    : m_rotation(0.0f, 0.0f, 0.0f),
      m_scale(desc->m_params.m_scale, desc->m_params.m_scale, desc->m_params.m_scale) {
    m_uv0[0] = 0.0f;
    m_uv0[1] = 0.0f;
    field_0x3C = 0.0f;
    m_uv1[0] = 1.0f;
    m_uv1[1] = 1.0f;
    field_0x48 = 1.0f;
    m_state = 0;
    m_age = 0.0f;
    m_prev = 0;
    m_next = 0;
    m_cursor = 0;
    m_commandHead = 0;
    m_commandTail = 0;
    m_pool = pool;
    m_color[0] = (desc->m_red & 0xFF) << 22;
    m_color[1] = (desc->m_green & 0xFF) << 22;
    m_color[2] = (desc->m_blue & 0xFF) << 22;
    m_color[3] = (desc->m_alpha & 0xFF) << 22;
}

EffectParticleExecUnit::~EffectParticleExecUnit() {
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

void EffectParticleExecUnit::ParseCommands() {
    while (m_cursor != 0) {
        if (m_commandTail != 0 && dynamic_cast<EffCmdWaitExec *>(m_commandTail) != 0) {
            return;
        }
        EffectCommandExec *exec = 0;
        u32 length = 0;
        switch (m_cursor[0]) {
        case 1:
            exec = new EffCmdPtnAnimExec(m_cursor, &m_state, reinterpret_cast<Vec2f *>(m_uv0),
                                         reinterpret_cast<Vec2f *>(m_uv1));
            length = 0x10;
            break;
        case 3:
            exec = new EffCmd3DPosExec(m_cursor, &m_state, &m_position);
            length = 0x24;
            break;
        case 4:
            exec = new EffCmd3DRotExec(m_cursor, &m_state, &m_rotation);
            length = 0x24;
            break;
        case 5:
            exec = new EffCmd2DRotExec(m_cursor, &m_state, &m_rotation.z);
            length = 0x14;
            break;
        case 6:
            exec = new EffCmd3DSclExec(m_cursor, &m_state, &m_scale);
            length = 0x24;
            break;
        case 7:
            exec = new EffCmd2DSclExec(m_cursor, &m_state, reinterpret_cast<Vec2f *>(&m_scale));
            length = 0x1C;
            break;
        case 8:
            exec = new EffCmd3DVelExec(m_cursor, &m_state, &m_velocity);
            length = 0x24;
            break;
        case 0xB:
            exec = new EffCmdMatColorExec(m_cursor, &m_state, m_color);
            length = 0x14;
            break;
        case 0xC:
            exec = new EffCmdMatAlphaExec(m_cursor, &m_state, &m_color[3]);
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

EffectParticle::EffectParticle(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,
                               EffectManager *manager)
    : EffectElement(desc, info, manager), m_particles(0), m_lastParticle(0), m_request(0),
      m_config(0), m_liveCount(0), m_gravity(0.0f), m_friction(0.0f), m_cursor(0), m_commandHead(0),
      m_commandTail(0), m_commandPool(0), m_particlePool(0) {
    m_size -= sizeof(EffectElement);
    m_size += sizeof(EffectParticle);
    if (!(m_flags & 1)) {
        return;
    }
    switch (m_desc->m_type) {
    case 2: {
        const UnkParticleDesc_80427EC8 *params =
            reinterpret_cast<const UnkParticleDesc_80427EC8 *>(desc);
        m_config |= params->m_flags;
        m_params = &params->m_params;
        m_emitter = &params->m_emitter;
        break;
    }
    case 5: {
        const UnkParticle3DDesc_80427EC8 *params =
            reinterpret_cast<const UnkParticle3DDesc_80427EC8 *>(desc);
        m_config |= params->m_flags;
        m_params = &params->m_params;
        m_emitter = &params->m_emitter;
        break;
    }
    }
    m_count = m_emitter->m_count;
    m_rate = m_emitter->m_rate;
    m_config |= m_emitter->m_flags;
    m_config |= m_params->m_flags;
    if (m_emitter->m_shape != 0) {
        m_config |= m_emitter->m_shape->m_flags;
    }
    if (m_config & 0x200) {
        m_gravity = m_params->m_gravity;
    }
    if (m_config & 0x400) {
        m_friction = m_params->m_friction;
    }
    m_particlePool = new UnkEffectPool_8042A344(0x7C, m_count);
    if (m_particlePool == 0 || (m_count != 0 && m_particlePool->GetSize() == 0)) {
        m_flags &= ~1;
        return;
    }
    m_size += m_particlePool->GetSize() + sizeof(UnkEffectPool_8042A344);
    s32 commandCount = m_emitter->m_commandCount;
    if (commandCount == 0) {
        commandCount = m_count * EffectCommandExec::CountCommands(m_params->m_commands);
        commandCount += EffectCommandExec::CountCommands(m_emitter->m_commands);
    }
    m_commandPool = new UnkEffectPool_8042A344(0x30, commandCount);
    if (m_commandPool == 0 || (commandCount != 0 && m_commandPool->GetSize() == 0)) {
        m_flags &= ~1;
        return;
    }
    m_size += m_commandPool->GetSize() + sizeof(UnkEffectPool_8042A344);
    m_cursor = m_emitter->m_commands;
}

EffectParticle::~EffectParticle() {
    fn_8042A5D0()->SetPool(m_particlePool);
    EffectParticleExecUnit *particle = m_particles;
    while (particle != 0) {
        EffectParticleExecUnit *next = particle->m_next;
        delete particle;
        particle = next;
    }
    m_particles = 0;
    m_lastParticle = 0;
    EffectCommandExec *exec = m_commandHead;
    fn_8042A5D0()->SetPool(m_commandPool);
    while (exec != 0) {
        EffectCommandExec *next = exec->m_next;
        delete exec;
        exec = next;
    }
    if (m_particlePool != 0) {
        delete m_particlePool;
    }
    if (m_commandPool != 0) {
        delete m_commandPool;
    }
}

void EffectParticle::ParseCommands() {
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
        case 9:
            exec = new EffCmdGravityExec(m_cursor, &m_request, &m_gravity);
            length = 0x14;
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

void EffectParticle::Generate(float dt) {
    int count = 0;
    float quantity = m_rate * dt;
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
    float speed = 0.0f;
    if (m_config & 0x1000) {
        speed = fn_8040CF50(&m_velocity);
    }
    for (int i = 0; i < count; i++) {
        if (m_count == m_liveCount) {
            return;
        }
        fn_8042A5D0()->SetPool(m_particlePool);
        EffectParticleExecUnit *particle;
        switch (m_desc->m_type) {
        case 2:
            particle = new EffectParticleExecUnit(
                reinterpret_cast<const UnkParticleDesc_80427EC8 *>(m_desc), m_commandPool);
            break;
        case 5:
            particle = new EffectParticleExecUnit(
                reinterpret_cast<const UnkParticle3DDesc_80427EC8 *>(m_desc), m_commandPool);
            break;
        }
        if (particle == 0) {
            return;
        }
        m_liveCount++;
        if (m_lastParticle != 0) {
            m_lastParticle->m_next = particle;
            particle->m_prev = m_lastParticle;
            m_lastParticle = particle;
        } else {
            m_lastParticle = particle;
            m_particles = particle;
        }
        const UnkParticleParams_80427EC8 *params = m_params;
        float velocity = params->m_speedRange * RandomFloat() + params->m_speed;
        const UnkParticleShape_80427EC8 *shape = m_emitter->m_shape;
        switch (shape->m_type) {
        default: {
            AssignVector(particle->m_position, m_lastPosition);
            Vec3f up;
            up.y = velocity;
            AssignVector(particle->m_velocity, up);
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
            particle->m_position.x = s * radius;
            particle->m_position.y = height;
            particle->m_position.z = c * radius;
            float ts = std::sin(tilt);
            float tc = std::cos(tilt);
            particle->m_velocity.x = s * (ts * velocity);
            particle->m_velocity.y = tc * velocity;
            particle->m_velocity.z = c * (ts * velocity);
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
            particle->m_position.x = x * radius;
            particle->m_position.y = tc * radius;
            particle->m_position.z = z * radius;
            particle->m_velocity.x = x * velocity;
            particle->m_velocity.y = tc * velocity;
            particle->m_velocity.z = z * velocity;
            if (m_config & 0x20) {
                particle->m_velocity.x = -particle->m_velocity.x;
                particle->m_velocity.y = -particle->m_velocity.y;
                particle->m_velocity.z = -particle->m_velocity.z;
            }
            break;
        }
        case 2: {
            float size = shape->m_radius;
            particle->m_position.x = size * RandomFloat() - 0.5f * size;
            size = shape->m_spread;
            particle->m_position.y = size * RandomFloat() - 0.5f * size;
            size = shape->m_angle;
            particle->m_position.z = size * RandomFloat() - 0.5f * size;
            particle->m_velocity.x = 0.0f;
            particle->m_velocity.y = velocity;
            particle->m_velocity.z = 0.0f;
            break;
        }
        }
        if (!(m_config & 0x800)) {
            AssignVector(particle->m_position, fn_8040B67C(&m_worldMatrix, &particle->m_position));
            AssignVector(particle->m_velocity, fn_8040B618(&m_worldMatrix, &particle->m_velocity));
            if (speed > 0.0001f) {
                float dot = fn_8040CF90(&particle->m_velocity, &m_velocity);
                float scale = dot / (speed * fn_8040CF50(&particle->m_velocity));
                fn_8040CFF8(&particle->m_velocity,
                            &(const Vec3f &)fn_800091A4(&m_velocity,
                                                        scale > 0.0 ? scale : -scale));
            }
        }
        particle->m_cursor = m_params->m_commands;
    }
}

void EffectParticle::Move(float dt) {
    if (!(m_config & 0x100)) {
        float lifetime = 0.0001f + m_params->m_lifetime;
        for (EffectParticleExecUnit *particle = m_particles; particle != 0;
             particle = particle->m_next) {
            if (m_flags & 2) {
                particle->m_age += dt;
            }
            if (particle->m_age > lifetime) {
                particle->m_state |= 1;
            }
        }
    }
    EffectCommandExec *next;
    EffectCommandExec *exec;
    EffectParticleExecUnit *current;
    EffectParticleExecUnit *particle = m_particles;
    fn_8042A5D0()->SetPool(m_commandPool);
    while (particle != 0) {
        if (!(particle->m_state & 1)) {
            particle->ParseCommands();
            if (!(particle->m_state & 1)) {
                exec = particle->m_commandHead;
                while (exec != 0) {
                    if (!exec->Exec(dt)) {
                        if (exec->m_prev != 0) {
                            exec->m_prev->m_next = exec->m_next;
                        }
                        if (exec->m_next != 0) {
                            exec->m_next->m_prev = exec->m_prev;
                        }
                        if (exec == particle->m_commandHead) {
                            particle->m_commandHead = exec->m_next;
                        }
                        if (exec == particle->m_commandTail) {
                            particle->m_commandTail = exec->m_prev;
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
        particle = particle->m_next;
    }
    current = m_particles;
    fn_8042A5D0()->SetPool(m_particlePool);
    while (current != 0) {
        EffectParticleExecUnit *following = current->m_next;
        if (current->m_state & 1) {
            if (current->m_prev != 0) {
                current->m_prev->m_next = current->m_next;
            }
            if (current->m_next != 0) {
                current->m_next->m_prev = current->m_prev;
            }
            if (current == m_particles) {
                m_particles = current->m_next;
            }
            if (current == m_lastParticle) {
                m_lastParticle = current->m_prev;
            }
            delete current;
            m_liveCount--;
        }
        current = following;
    }
    for (particle = m_particles; particle != 0; particle = particle->m_next) {
        fn_8040CFF8(&particle->m_position,
                    &(const Vec3f &)fn_800091A4(&particle->m_velocity, dt));
        if (m_friction != 0.0f) {
            float length = fn_8040CF50(&particle->m_velocity);
            if (length > 0.0001f) {
                float loss = length * m_friction;
                if (loss > length) {
                    fn_8040CE38(&particle->m_velocity, 0.0f);
                } else {
                    fn_8040CE38(&particle->m_velocity, (length - loss) / length);
                }
            }
        }
        if (m_gravity != 0.0f) {
            particle->m_velocity.y -= dt * m_gravity;
        }
    }
}

void EffectParticle::Update(float dt) {
    if (!(m_flags & 1)) {
        return;
    }
    if (m_flags & 4) {
        return;
    }
    if (m_flags & 2) {
        m_time += dt;
    }
    if (!(m_config & 1) && m_time > 0.0001f + m_emitter->m_duration) {
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
            if (m_liveCount == 0) {
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
