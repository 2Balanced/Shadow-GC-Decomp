#ifndef EFFECT_SYSTEM_EFFECTELEMENT_H
#define EFFECT_SYSTEM_EFFECTELEMENT_H

#include "Game/MathTypes.h"
#ifdef EFFECT_ELEMENT_POLYMORPHIC
/* The effect system fn_8042A5D0 returns gives elements their memory. */
#include "Effect/System/EffectSystem.h"
#endif
#ifdef EFFECT_ELEMENT_FULL
#include "boost/shared_ptr.hpp"
#endif
#include <dolphin/types.h>

namespace Effect {
namespace System {

#ifdef EFFECT_ELEMENT_POLYMORPHIC
class EffectManager;
#endif
#ifdef EFFECT_ELEMENT_FULL
class EffectLaunchInfo;
struct UnkEffectDesc_801D0AFC;
#endif

/* EffectElement (class name from the RTTI of its shared_ptr control block,
 * boost::detail::sp_counted_base_impl<Effect::System::EffectElement *, ...>): a running effect
 * instance handed out by the effect manager. Only the members gadget code touches are known; their
 * meanings are inferred from that use. */
class EffectElement {
  public:
    /* Clears flag bit 0 so the effect winds down (name and meaning inferred). */
    void Retire() {
        m_flags &= ~1;
    }

    /* Makes the effect follow matrix (name inferred). */
    void SetMatrix(const Mtx34f *matrix) {
        m_matrix = matrix;
    }

    /* Stops the effect at once (name inferred; see Game/BulletLaser.cpp). */
    void Stop() {
        m_flags |= 0x10;
        m_matrix = 0;
#ifdef EFFECT_ELEMENT_FULL
        m_position = 0;
        m_rotation = 0;
#else
        field_0x4C = 0;
        field_0x50 = 0;
#endif
    }

    /* Sets flag bit 4, as Stop does, but keeps the matrix (name inferred; see
     * Game/WeaponVacuumPod.cpp). */
    void Kill() {
        m_flags |= 0x10;
    }

    /* Hides and shows the effect (flag bit 3; names inferred from Game/WeaponVacuumPod.cpp). */
    void Hide() {
        m_flags |= 8;
    }
    void Show() {
        m_flags &= ~8;
    }

    /* Whether the effect is still running (name inferred). */
    bool IsActive() const {
        return m_flags & 1;
    }

    /* Clears flag bit 2, and sets the values at 0x14, 0x4C and 0x50 (names and meanings inferred;
     * see Game/EffectPJS.cpp). */
    void ClearFlag4() {
        m_flags &= ~4;
    }
#ifndef EFFECT_ELEMENT_FULL
    void SetField14(u32 value) {
        field_0x14 = value;
    }
    void SetField4C(u32 value) {
        field_0x4C = value;
    }
    void SetField50(u32 value) {
        field_0x50 = value;
    }
#endif

#ifdef EFFECT_ELEMENT_POLYMORPHIC
    /* The class's own view of itself, for the units that define classes derived from it (see
     * Effect/System/EffectGlobalParam.h): its virtual destructor (0x80425E8C) and its operator
     * delete, which hands the memory back to the effect system. Gadget code only reaches the
     * members below, so the plain layout above stays the default. */
    virtual ~EffectElement();
    /* Advances the effect by dt, and draws it (names inferred; see Game/EffectManagerPJS.h). */
    virtual void Update(float dt) = 0;
    virtual void Draw() = 0;
    /* Elements live in the effect system's memory (see Game/EffectManagerPJS.cpp). */
    static void *operator new(unsigned long size) {
        return fn_8042A5D0()->Allocate(size);
    }
    static void operator delete(void *p) {
        fn_8042A5D0()->Free(p);
    }

#ifdef EFFECT_ELEMENT_FULL
    /* The effect library's own units (Game/EffectManager.cpp and the element units after it)
     * define EFFECT_ELEMENT_FULL and see the whole class; its members' names are inferred. */
    EffectElement(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,
                  EffectManager *manager);
    /* Whether flag bit 3 is set (see Hide). */
    bool IsHidden() const {
        return m_flags & 8;
    }
    /* Builds m_worldMatrix from the followed matrix, position and rotation, the offsets, angles
     * and scale, and the velocity from the last two positions (0x80425C34). */
    void UpdateTransform(float dt);

    /* 0x04 */ u8 m_loopDepth;  /* how many command loops are open */
    /* 0x05 */ s8 m_skipLoops;  /* how many of them are skipped */
    /* 0x08 */ u32 m_size; /* the size of the most derived class */
    /* 0x0C */ float m_time;
    /* 0x10 */ float m_speed; /* scales the dt the manager updates the element with */
    /* 0x14 */ const float *m_speedSource; /* where m_speed is read from, if anywhere */
    /* 0x18 */ u32 m_flags; /* bit 0: running, bit 1: updated once */
    /* 0x1C */ EffectManager *m_manager;
    /* 0x20 */ boost::shared_ptr<EffectElement> m_prev; /* the manager's element list */
    /* 0x28 */ boost::shared_ptr<EffectElement> m_next;
    /* 0x30 */ s32 m_drawPass;
    /* 0x34 */ boost::shared_ptr<EffectElement> m_drawPrev; /* the manager's draw list */
    /* 0x3C */ boost::shared_ptr<EffectElement> m_drawNext;
    /* 0x44 */ const UnkEffectDesc_801D0AFC *m_desc;
    /* 0x48 */ const Mtx34f *m_matrix;   /* followed, if set */
    /* 0x4C */ const Vec3f *m_position;  /* followed when no matrix is */
    /* 0x50 */ const Vec3f *m_rotation;  /* followed when no matrix is */
    /* 0x54 */ Mtx34f m_worldMatrix;
    /* 0x84 */ Mtx34f m_localMatrix;
    /* 0xB4 */ Vec3f m_lastPosition;
    /* 0xC0 */ Vec3f m_velocity;
    /* 0xCC */ Vec3f m_offset;
    /* 0xD8 */ Vec3f m_angles;
    /* 0xE4 */ Vec3f m_scale;
    /* 0xF0 */ Vec3f m_offsetSpeed; /* added to m_offset per unit of dt */
#else
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ u32 m_size; /* the size of the most derived class (inferred) */
    /* 0x0C */ float m_time; /* frames since launch (inferred; Game/EffectObjectUSA.cpp divides it
                              * by 60) */
    /* 0x10 */ u8 field_0x10[0x14 - 0x10];
#endif
#else
    /* 0x00 */ u8 field_0x0[0x14];
#endif
#ifndef EFFECT_ELEMENT_FULL
    /* 0x14 */ u32 field_0x14;
    /* 0x18 */ u32 m_flags; /* bit 0 is cleared to let the effect finish (inferred) */
#ifdef EFFECT_ELEMENT_POLYMORPHIC
    /* 0x1C */ EffectManager *m_manager; /* the manager that runs the effect (inferred) */
    /* 0x20 */ u8 field_0x20[0x30 - 0x20];
    /* 0x30 */ s32 m_drawPass; /* which EffectDraw*Task draws it; 6 for shimmer (inferred) */
    /* 0x34 */ u8 field_0x34[0x48 - 0x34];
#else
    /* 0x1C */ u8 field_0x1C[0x48 - 0x1C];
#endif
    /* 0x48 */ const Mtx34f *m_matrix; /* the owner's matrix the effect follows (inferred) */
    /* 0x4C */ u32 field_0x4C;
    /* 0x50 */ u32 field_0x50;
#endif
};

} // namespace System
} // namespace Effect

#endif
