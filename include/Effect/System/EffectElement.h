#ifndef EFFECT_SYSTEM_EFFECTELEMENT_H
#define EFFECT_SYSTEM_EFFECTELEMENT_H

#include "Game/MathTypes.h"
#include <dolphin/types.h>

namespace Effect {
namespace System {

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

    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ u32 field_0x14;
    /* 0x18 */ u32 m_flags; /* bit 0 is cleared to let the effect finish (inferred) */
    /* 0x1C */ u8 field_0x1C[0x48 - 0x1C];
    /* 0x48 */ const Mtx34f *m_matrix; /* the owner's matrix the effect follows (inferred) */
    /* 0x4C */ u32 field_0x4C;
    /* 0x50 */ u32 field_0x50;
};

} // namespace System
} // namespace Effect

#endif
