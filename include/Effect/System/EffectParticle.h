#ifndef EFFECT_SYSTEM_EFFECTPARTICLE_H
#define EFFECT_SYSTEM_EFFECTPARTICLE_H

/* Effect::System::EffectParticle and EffectParticleExecUnit (class names from RTTI): the particle
 * element and one of its particles. The element emits particles from a shape at a rate, moves them
 * with gravity and friction, and runs each particle's own command list (Game/EffectParticle.cpp);
 * the RenderWare classes (Game/EffectManagerPJS.h, Game/EffectParticleRW3.cpp) draw them. The
 * units that define EFFECT_ELEMENT_FULL see the constructors and the element's members below 0xFC.
 * Names other than the classes' are inferred. */
#include "Effect/System/EffectElement.h"
#include "Game/MathTypes.h"
#include "size_assert.h"

namespace Effect {
namespace System {

class EffectCommandExec;
class UnkEffectPool_8042A344;
struct UnkParticleDesc_80427EC8;
struct UnkParticle3DDesc_80427EC8;
struct UnkParticleParams_80427EC8;
struct UnkParticleEmitter_80427EC8;

class EffectParticleExecUnit {
  public:
#ifdef EFFECT_ELEMENT_FULL
    /* One constructor for each description layout (0x804286B8 and 0x8042860C). */
    EffectParticleExecUnit(const UnkParticleDesc_80427EC8 *desc, UnkEffectPool_8042A344 *pool);
    EffectParticleExecUnit(const UnkParticle3DDesc_80427EC8 *desc, UnkEffectPool_8042A344 *pool);
    virtual ~EffectParticleExecUnit();
    /* Turns commands into executors until a wait (0x804281D4). */
    void ParseCommands();

#ifdef EFFECT_SYSTEM_FULL
    /* Particles live in the effect system's current pool. */
    static void *operator new(unsigned long size) {
        return fn_8042A5D0()->PoolAlloc();
    }
    static void operator delete(void *p) {
        fn_8042A5D0()->PoolFree(p);
    }
#endif
#else
    /* 0x00 */ u8 field_0x0[0x4];
#endif
    /* 0x04 */ Vec3f m_position; /* the RW3 elements draw them here (Game/EffectParticleRW3.cpp) */
    /* 0x10 */ Vec3f m_rotation; /* about X, Y and Z (the 2D particles use Z only) */
    /* 0x1C */ Vec3f m_velocity;
    /* 0x28 */ Vec3f m_scale;    /* the 2D particles use X and Y as their size */
    /* 0x34 */ float m_uv0[2];   /* texture coordinates of two corners (the pattern animation
                                  * command moves them) */
    /* 0x3C */ float field_0x3C;
    /* 0x40 */ float m_uv1[2];
    /* 0x48 */ float field_0x48;
    /* 0x4C */ u32 m_state; /* bit 0: dead, or out of executor memory */
    /* 0x50 */ float m_age;
    /* 0x54 */ u32 m_color[4]; /* red, green, blue, alpha: 8 bits each, from bit 22 */
    /* 0x64 */ EffectParticleExecUnit *m_prev;
    /* 0x68 */ EffectParticleExecUnit *m_next;
    /* 0x6C */ const u8 *m_cursor; /* the next command */
    /* 0x70 */ EffectCommandExec *m_commandHead;
    /* 0x74 */ EffectCommandExec *m_commandTail;
    /* 0x78 */ UnkEffectPool_8042A344 *m_pool; /* where its executors live */
};

class EffectParticle : public EffectElement {
  public:
#ifdef EFFECT_ELEMENT_FULL
    /* 0x80427EC8 (Game/EffectParticleRW3.cpp calls it). */
    EffectParticle(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,
                   EffectManager *manager);
#endif
    virtual ~EffectParticle();
    virtual void Update(float dt);

#ifdef EFFECT_ELEMENT_FULL
    /* Turns commands into executors until a wait (0x80427970). */
    void ParseCommands();
    /* Emits this update's particles (0x804272E8), and moves them and runs their commands
     * (0x80426FF4). */
    void Generate(float dt);
    void Move(float dt);
#else
    /* 0x054 */ u8 field_0x54[0xFC - 0x54];
#endif
    /* 0x0FC */ EffectParticleExecUnit *m_particles; /* the first particle */
    /* 0x100 */ EffectParticleExecUnit *m_lastParticle;
    /* 0x104 */ u32 m_request; /* bit 0: out of executor memory, bit 1: show or hide requested,
                                * bit 2: hide (as EffectLocusRW3's field_0x108) */
    /* 0x108 */ u32 m_config;  /* the description's flags; bit 0: never times out, bits 2 and 3
                                * pick the draw pass, bit 4: has textures, bit 15: has a model */
    /* 0x10C */ s32 m_count;     /* the number of particles */
    /* 0x110 */ s32 m_liveCount; /* how many of them are alive */
    /* 0x114 */ float m_gravity;
    /* 0x118 */ float m_friction;
    /* 0x11C */ float m_rate; /* particles per unit of dt */
    /* 0x120 */ const UnkParticleEmitter_80427EC8 *m_emitter;
    /* 0x124 */ const UnkParticleParams_80427EC8 *m_params;
    /* 0x128 */ const u8 *m_cursor; /* the next command */
    /* 0x12C */ EffectCommandExec *m_commandHead;
    /* 0x130 */ EffectCommandExec *m_commandTail;
    /* 0x134 */ UnkEffectPool_8042A344 *m_commandPool;
    /* 0x138 */ UnkEffectPool_8042A344 *m_particlePool;
};
SIZE_ASSERT(EffectParticle, 0x13C);

} // namespace System
} // namespace Effect

#endif
