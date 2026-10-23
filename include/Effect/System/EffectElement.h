#ifndef EFFECT_SYSTEM_EFFECTELEMENT_H
#define EFFECT_SYSTEM_EFFECTELEMENT_H

#include "Game/MathTypes.h"
#include <dolphin/types.h>

namespace Effect {
namespace System {

#ifdef EFFECT_ELEMENT_POLYMORPHIC
class EffectManager;

/* The effect heap fn_8042A5D0 returns (class name unknown): its virtual functions set it up on a
 * block of memory, release it, and allocate and free memory (names inferred; see
 * Game/EffectTask.cpp). */
class UnkEffectHeap_8042A5D0 {
  public:
    virtual void Initialize(void *memory, u32 size);
    virtual void Finalize();
    virtual void *Alloc(u32 size);
    virtual void Free(void *p);
};

extern "C" UnkEffectHeap_8042A5D0 *fn_8042A5D0(void);
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
        field_0x4C = 0;
        field_0x50 = 0;
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
    void SetField14(u32 value) {
        field_0x14 = value;
    }
    void SetField4C(u32 value) {
        field_0x4C = value;
    }
    void SetField50(u32 value) {
        field_0x50 = value;
    }

#ifdef EFFECT_ELEMENT_POLYMORPHIC
    /* The class's own view of itself, for the units that define classes derived from it (see
     * Effect/System/EffectGlobalParam.h): its virtual destructor (0x80425E8C) and its operator
     * delete, which hands the memory back to the effect heap. Gadget code only reaches the members
     * below, so the plain layout above stays the default. */
    virtual ~EffectElement();
    /* Advances the effect by dt, and draws it (names inferred; see Game/EffectManagerPJS.h). */
    virtual void Update(float dt) = 0;
    virtual void Draw() = 0;
    /* Elements live on the effect heap (see Game/EffectManagerPJS.cpp). */
    static void *operator new(unsigned long size) {
        return fn_8042A5D0()->Alloc(size);
    }
    static void operator delete(void *p) {
        fn_8042A5D0()->Free(p);
    }

    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ u32 m_size; /* the size of the most derived class (inferred) */
    /* 0x0C */ u8 field_0xC[0x14 - 0xC];
#else
    /* 0x00 */ u8 field_0x0[0x14];
#endif
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
};

} // namespace System
} // namespace Effect

#endif
