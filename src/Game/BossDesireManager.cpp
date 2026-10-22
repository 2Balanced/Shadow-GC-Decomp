/* The desire manager of the bosses that choose their behaviours by desire (Boss Sonic and Diablon;
 * class name unknown, no RTTI; see Game/BossDesireManager.h): it holds the boss's desires (Boss::Desire::DesireBase), raises
 * them every frame and starts the behaviour of the desire that became full. Flag bit 0 of a desire
 * (or of the manager) stops it from growing and bit 1 from being chosen; bits 2 and 3 of a desire
 * override those. When several desires are full in one frame, the later one wins and the earlier
 * falls back just below full. File, class and member names are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#define BOOST_CHECKED_ARRAY_DELETE
#include "Game/BossDesireManager.h"
#include "boost/scoped_array.hpp"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

extern "C" {
/* A 32-bit flag word (see Game/BossDesire.cpp and Game/EggLastMech.h): constructs it cleared and
 * tests a bit. */
void fn_80014E04(u32 *flags);
bool fn_80014E34(u32 *flags, int bit);
}

namespace Boss {
namespace Desire {

/* The manager's flag word (struct name unknown). */
struct UnkDesireFlags_803375E8 {
    UnkDesireFlags_803375E8() {
        fn_80014E04(&m_bits);
    }
    /* Sets or clears bit (name inferred). */
    void Set(int bit, bool on);

    /* 0x0 */ u32 m_bits;
};

struct DesireManager::Impl {
    Impl();
    ~Impl();

    /* 0x0 */ s32 m_count;
    /* 0x4 */ UnkDesireFlags_803375E8 m_flags;
    /* 0x8 */ boost::scoped_array<boost::shared_ptr<DesireBase> > m_desires;
};
SIZE_ASSERT(DesireManager::Impl, 0xC);

} // namespace Desire
} // namespace Boss

/* Deletes the state (name inferred; see checked_delete below). */
inline void DeleteImpl(Boss::Desire::DesireManager::Impl *impl) {
    delete impl;
}

/* Retail keeps boost::checked_delete<Impl> out of line (0x803374EC) and ~Impl at its own place
 * (0x80337558): the specialization calls an inline helper (as in Game/BossCtrl.cpp; see
 * docs/research-notes.md). */
namespace boost {
template <> inline void checked_delete<Boss::Desire::DesireManager::Impl>(Boss::Desire::DesireManager::Impl *x) {
    DeleteImpl(x);
}
} // namespace boost

namespace Boss {
namespace Desire {

void UnkDesireFlags_803375E8::Set(int bit, bool on) {
    if (on) {
        m_bits |= 1 << bit;
    } else {
        m_bits &= ~(1 << bit);
    }
}

DesireManager::Impl::Impl() {
    m_count = 0;
}

DesireManager::Impl::~Impl() {}

DesireManager::DesireManager() : m_impl(new Impl) {}

DesireManager::~DesireManager() {}

void DesireManager::Init(int count) {
    m_impl->m_count = count;
    m_impl->m_desires.reset(new boost::shared_ptr<DesireBase>[count]);
}

void DesireManager::Set(int index, const boost::shared_ptr<DesireBase> &desire) {
    m_impl->m_desires[index] = desire;
}

void DesireManager::SetFlag(int bit, bool on) {
    m_impl->m_flags.Set(bit, on);
}

void DesireManager::Tick(float dt) {
    boost::shared_ptr<DesireBase> chosen;
    for (int i = 0; i < m_impl->m_count; i++) {
        boost::shared_ptr<DesireBase> desire = m_impl->m_desires[i];
        if (desire) {
            if (fn_80014E34(&desire->field_0x4, 2) ||
                (!fn_80014E34(&desire->field_0x4, 0) && !fn_80014E34(&m_impl->m_flags.m_bits, 0))) {
                desire->Tick(dt);
            }
            if (desire->IsAtLeast(1.0f) &&
                (fn_80014E34(&desire->field_0x4, 3) ||
                 (!fn_80014E34(&desire->field_0x4, 1) && !fn_80014E34(&m_impl->m_flags.m_bits, 1)))) {
                if (chosen) {
                    chosen->SetDesire(0.98f);
                }
                chosen = desire;
            }
        }
    }
    if (chosen) {
        chosen->Begin();
    }
}

} // namespace Desire
} // namespace Boss
