#ifndef GAME_GADGETPARTICLE_H
#define GAME_GADGETPARTICLE_H

#include "boost/weak_ptr.hpp"
#include "size_assert.h"

namespace GadgetParticle {
/* GadgetParticleCtrl (class name from the RTTI of its shared_ptr control block): the particle
 * emitter of a gadget model. */
class GadgetParticleCtrl;
} // namespace GadgetParticle

/* The controller's definition is compiled only where GADGETPARTICLE_IMPL is defined before this
 * header (Game/GadgetParticle.cpp): <vector>'s template declarations would advance CodeWarrior's
 * @N counter in the other units that include it. */
#ifdef GADGETPARTICLE_IMPL
#include <vector>

#include "Effect/System/EffectElement.h"
#include "Game/MathTypes.h"

struct RwFrame;
struct RpClump;
struct RpAtomic;

namespace GadgetParticle {

/* A matrix whose copies go through fn_8040C148 (struct name inferred; as UnkMatrix_80325384 in
 * Game/DevilDoomCtrlUnitBit.cpp). */
struct ParticleMatrix : public Mtx34f {
    ParticleMatrix() {}
    ParticleMatrix(const ParticleMatrix &matrix) {
        fn_8040C148(this, &matrix);
    }
    ParticleMatrix &operator=(const ParticleMatrix &matrix) {
        fn_8040C148(this, &matrix);
        return *this;
    }
};

/* One effect a model's atomic carries, read from the atomic's geometry user data (struct and member
 * names inferred; the user data names are ATTR_PTCL.PTCLCATEGORY, .PTCLID and .PTCLFLAG and
 * ATTR_ID.ATTR_ID). */
struct ParticleEntry {
    ParticleEntry() : m_category(-1), m_effectId(-1), m_flags(-1), m_attrId(0), m_frame(0) {}

    /* Whether the user data lacked one of the particle values. */
    bool IsIncomplete() const {
        return m_category == -1 || m_effectId == -1 || m_flags == -1;
    }

    /* Lets a running effect finish, and forgets it. */
    void Retire() {
        if (!m_effect.expired()) {
            boost::shared_ptr<Effect::System::EffectElement> effect(m_effect.lock());
            if (effect) {
                effect->Retire();
                m_effect.reset();
            }
        }
    }

    /* Stops a running effect at once, and forgets it. */
    void Stop() {
        if (!m_effect.expired()) {
            boost::shared_ptr<Effect::System::EffectElement> effect(m_effect.lock());
            if (effect) {
                effect->Stop();
                m_effect.reset();
            }
        }
    }

    /* 0x00 */ s32 m_category; /* 0: fn_80168C48's effects, 1: fn_8015D7DC's */
    /* 0x04 */ s32 m_effectId;
    /* 0x08 */ s32 m_flags;    /* bit 0: started by the controller, bit 1: atomic stays drawn,
                                * bit 2: following the frame */
    /* 0x0C */ s32 m_attrId;
    /* 0x10 */ RwFrame *m_frame;
    /* 0x14 */ ParticleMatrix m_matrix;
    /* 0x44 */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
};
SIZE_ASSERT(ParticleEntry, 0x4C);

/* The controller's entries (struct name inferred). */
struct ParticleEntryList : public std::vector<ParticleEntry> {};
SIZE_ASSERT(ParticleEntryList, 0xC);

class GadgetParticleCtrl {
  public:
    /* Reads the entries of clump's atomics (none without a clump). */
    GadgetParticleCtrl(RpClump *clump);
    virtual ~GadgetParticleCtrl();

    /* RpClumpForAllAtomics callback: adds the atomic's entry to the list at data (name
     * inferred). */
    static RpAtomic *AddAtomic(RpAtomic *atomic, void *data);

    /* 0x4 */ ParticleEntryList m_entries;
};
SIZE_ASSERT(GadgetParticleCtrl, 0x10);

} // namespace GadgetParticle
#endif

/* The parameter a gadget passes with fn_80169D00 to fn_8015FA78, which calls it for each part of a
 * model so the particle controller is attached to every part (name and purpose inferred). Its
 * constructor and destructor are shared weak instances at 0x800CC6C8 and 0x800CC62C. */
struct UnkParticleAttach_80169D00 {
    /* 0x0 */ boost::weak_ptr<GadgetParticle::GadgetParticleCtrl> ctrl;
};
SIZE_ASSERT(UnkParticleAttach_80169D00, 0x8);

#endif
