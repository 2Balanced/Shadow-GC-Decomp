/* The attack collisions a character has been hit by recently (class and file names invented; no
 * class of the unit has RTTI): a std::list of entries that keep a weak_ptr to the attack and a
 * count of frames, which the units that take hits (Game/PlayerBarrierHeatEffect.cpp,
 * Game/VehicleWalker.cpp and one unsplit unit at 0x801BB3D0) construct, update and query through
 * the extern "C" functions below. An attack is remembered for 5 frames after its last hit. The
 * unit also holds the list's instances (Game/MSL stand-in <list>). Names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first); the list's members defined out of line come out at the end of the unit.
 *
 * NonMatching (47/48 functions, every section 100% but .text): fn_803555CC, see there. */
#include "Game/AttackIgnoreList.h"
#include <dolphin/types.h>
#include "size_assert.h"
#include <list>
#include <new>

using SonicteamUSA::System::CharaColli::CharaColliAttack;

/* An attack and the frames it is still remembered for (struct name unknown). */
struct UnkAttackHit_803556FC {
    UnkAttackHit_803556FC(const boost::weak_ptr<CharaColliAttack> &attack, int frames);
    ~UnkAttackHit_803556FC();

    /* Whether this is the entry of attack. */
    bool Is(const boost::weak_ptr<CharaColliAttack> &attack) const;

    /* 0x0 */ boost::weak_ptr<CharaColliAttack> m_attack;
    /* 0x8 */ s32 m_frames;
};
SIZE_ASSERT(UnkAttackHit_803556FC, 0xC);

typedef std::list<UnkAttackHit_803556FC> UnkAttackHitList_803555CC;
SIZE_ASSERT(UnkAttackHitList_803555CC, 0xC);

UnkAttackHit_803556FC::UnkAttackHit_803556FC(const boost::weak_ptr<CharaColliAttack> &attack, int frames)
    : m_attack(attack), m_frames(frames) {}

UnkAttackHit_803556FC::~UnkAttackHit_803556FC() {}

bool UnkAttackHit_803556FC::Is(const boost::weak_ptr<CharaColliAttack> &attack) const {
    return m_attack.lock() == attack.lock();
}

/* Constructs and destroys a list. NonMatching: the game constructs the list without a null check
 * (as a constructor of a class holding it would), which a placement new always has under GC/2.7;
 * other units call this by its extern "C" name, so it cannot be that constructor. */
extern "C" UnkAttackHitList_803555CC *fn_803555CC(UnkAttackHitList_803555CC *hits) {
    new (hits) UnkAttackHitList_803555CC;
    return hits;
}

extern "C" UnkAttackHitList_803555CC *fn_80355478(UnkAttackHitList_803555CC *hits, short flag) {
    if (hits != 0) {
        hits->~UnkAttackHitList_803555CC();
        if (flag > 0) {
            operator delete(hits);
        }
    }
    return hits;
}

/* Whether attack is in the list. */
extern "C" bool fn_803553E8(UnkAttackHitList_803555CC *hits, const boost::weak_ptr<CharaColliAttack> &attack) {
    for (UnkAttackHitList_803555CC::iterator it = hits->begin(); it != hits->end(); it++) {
        if (it->Is(attack)) {
            return true;
        }
    }
    return false;
}

/* Remembers attack: counts another frame for it, or adds it for 5 frames. */
extern "C" void fn_803552DC(UnkAttackHitList_803555CC *hits, const boost::weak_ptr<CharaColliAttack> &attack) {
    for (UnkAttackHitList_803555CC::iterator it = hits->begin(); it != hits->end(); it++) {
        if (it->Is(attack)) {
            it->m_frames++;
            return;
        }
    }
    UnkAttackHit_803556FC hit(attack, 5);
    hits->push_back(hit);
}

/* Counts frames down. */
extern "C" void fn_80355260(UnkAttackHitList_803555CC *hits, int frames) {
    for (UnkAttackHitList_803555CC::iterator it = hits->begin(); it != hits->end(); it++) {
        it->m_frames -= frames;
    }
}

/* Forgets the attacks whose frames have run out or that no longer exist. */
extern "C" void fn_8035511C(UnkAttackHitList_803555CC *hits) {
    UnkAttackHitList_803555CC::iterator it = hits->begin();
    while (it != hits->end()) {
        if (it->m_frames <= 0 || fn_80053538(&it->m_attack)) {
            it = hits->erase(it);
        } else {
            it++;
        }
    }
}
