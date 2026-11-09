#ifndef EFFECT_SYSTEM_EFFECTRAY_H
#define EFFECT_SYSTEM_EFFECTRAY_H

/* Effect::System::EffectRay and EffectRayExecUnit (class names from RTTI): the ray element and one
 * of its rays. The element emits rays as Effect/System/EffectParticle.h's element emits particles
 * (Game/EffectRay.cpp); each ray runs from where it was emitted to where it has moved, with a
 * length, a width and the colours of its two ends. The RenderWare class (Game/EffectManagerPJS.h,
 * Game/EffectRayRW3.cpp) draws them. The units that define EFFECT_ELEMENT_FULL see the rays and
 * the element's members past 0x108. Names other than the classes' are inferred. */
#include "Effect/System/EffectElement.h"
#include "Game/MathTypes.h"

namespace Effect {
namespace System {

#ifdef EFFECT_ELEMENT_FULL
class EffectCommandExec;
class UnkEffectPool_8042A344;
struct UnkRayDesc_804298D4;

/* Colours are four words (red, green, blue, alpha), 8 bits each from bit 22. */
class EffectRayExecUnit {
  public:
    /* 0x80429FDC. */
    EffectRayExecUnit(const UnkRayDesc_804298D4 *desc, UnkEffectPool_8042A344 *pool);
    virtual ~EffectRayExecUnit();
    /* Turns commands into executors until a wait (0x80429B48). */
    void ParseCommands();

#ifdef EFFECT_SYSTEM_FULL
    /* Rays live in the effect system's current pool. */
    static void *operator new(unsigned long size) {
        return fn_8042A5D0()->PoolAlloc();
    }
    static void operator delete(void *p) {
        fn_8042A5D0()->PoolFree(p);
    }
#endif

    /* 0x04 */ Vec3f m_start;    /* moves with m_velocity */
    /* 0x10 */ Vec3f m_end;      /* where the ray was emitted */
    /* 0x1C */ Vec3f m_velocity;
    /* 0x28 */ u32 m_state; /* bit 0: dead, or out of executor memory */
    /* 0x2C */ float m_age;
    /* 0x30 */ float m_length; /* the longest the ray is drawn */
    /* 0x34 */ float m_width;
    /* 0x38 */ s32 m_color[4];
    /* 0x48 */ s32 m_topColor[4];  /* the start's colour, times m_color */
    /* 0x58 */ s32 m_lastColor[4]; /* the end's colour, times m_color */
    /* 0x68 */ EffectRayExecUnit *m_prev;
    /* 0x6C */ EffectRayExecUnit *m_next;
    /* 0x70 */ const u8 *m_cursor; /* the next command */
    /* 0x74 */ EffectCommandExec *m_commandHead;
    /* 0x78 */ EffectCommandExec *m_commandTail;
    /* 0x7C */ UnkEffectPool_8042A344 *m_pool; /* where its executors live */
};
#endif

class EffectRay : public EffectElement {
  public:
    virtual ~EffectRay();
    virtual void Update(float dt);

#ifdef EFFECT_ELEMENT_FULL
    /* 0x804298D4 */
    EffectRay(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info, EffectManager *manager);
    /* Turns commands into executors until a wait (0x804293B4). */
    void ParseCommands();
    /* Emits this update's rays (0x80428DFC), and moves them and runs their commands
     * (0x80428B20). */
    void Generate(float dt);
    void Move(float dt);

    /* 0x0FC */ EffectRayExecUnit *m_head; /* the first ray */
    /* 0x100 */ EffectRayExecUnit *m_tail;
#else
    /* 0x054 */ u8 field_0x54[0x104 - 0x54];
#endif
    /* 0x104 */ u32 m_request; /* as EffectParticle's */
    /* 0x108 */ u32 m_config;  /* bits 2 and 3 pick the draw pass (as EffectParticle's); bit 1: no
                                * depth writes, bit 11: the rays are in the element's space */
#ifdef EFFECT_ELEMENT_FULL
    /* 0x10C */ s32 m_unitCount;   /* the most rays */
    /* 0x110 */ s32 m_activeCount; /* the rays in use */
    /* 0x114 */ float m_friction;
    /* 0x118 */ float m_rate; /* rays per unit of dt */
    /* 0x11C */ u8 field_0x11C[0x120 - 0x11C];
    /* 0x120 */ const u8 *m_cursor; /* the next command */
    /* 0x124 */ EffectCommandExec *m_commandHead;
    /* 0x128 */ EffectCommandExec *m_commandTail;
    /* 0x12C */ UnkEffectPool_8042A344 *m_commandPool;
    /* 0x130 */ UnkEffectPool_8042A344 *m_unitPool;
#endif
};

} // namespace System
} // namespace Effect

#endif
