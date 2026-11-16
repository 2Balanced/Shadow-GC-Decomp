/* The attack collisions a character has been hit by recently (class and file names invented; no
 * class of the unit has RTTI): a std::list of entries that keep a weak_ptr to the attack and a
 * count of frames, which the units that take hits (Game/PlayerBarrierHeatEffect.cpp,
 * Game/VehicleWalker.cpp and one unsplit unit at 0x801BB3D0) construct, update and query through
 * the extern "C" functions below. An attack is remembered for 5 frames after its last hit. The
 * unit also holds the list's instances (Game/MSL stand-in <list>). Names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first); the list's members defined out of line come out at the end of the unit.
 *
 * The list is a class of its own (Game/AttackHitList.h) since wave 39: its constructor was the
 * one function that did not match while it was an extern "C" wrapper (see there). */
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

/* The list class itself (Game/AttackHitList.h), with its member. */
#define ATTACK_HIT_LIST_MEMBERS
#include "Game/AttackHitList.h"

typedef std::list<UnkAttackHit_803556FC> HitList;

UnkAttackHit_803556FC::UnkAttackHit_803556FC(const boost::weak_ptr<CharaColliAttack> &attack, int frames)
    : m_attack(attack), m_frames(frames) {}

UnkAttackHit_803556FC::~UnkAttackHit_803556FC() {}

bool UnkAttackHit_803556FC::Is(const boost::weak_ptr<CharaColliAttack> &attack) const {
    return m_attack.lock() == attack.lock();
}

/* The list's constructor and destructor (0x803555CC, 0x80355478). The classes that hold a list
 * construct it as a member; an extern "C" wrapper around a placement new cannot match here,
 * because GC/2.7 null-checks the placement new's result and the game's code has no such test. */
UnkAttackHitList_803555CC::UnkAttackHitList_803555CC() {}

UnkAttackHitList_803555CC::~UnkAttackHitList_803555CC() {}

/* Whether attack is in the list. */
extern "C" bool fn_803553E8(UnkAttackHitList_803555CC *hits, const boost::weak_ptr<CharaColliAttack> &attack) {
    for (HitList::iterator it = hits->m_list.begin(); it != hits->m_list.end(); it++) {
        if (it->Is(attack)) {
            return true;
        }
    }
    return false;
}

/* Remembers attack: counts another frame for it, or adds it for 5 frames. */
extern "C" void fn_803552DC(UnkAttackHitList_803555CC *hits, const boost::weak_ptr<CharaColliAttack> &attack) {
    for (HitList::iterator it = hits->m_list.begin(); it != hits->m_list.end(); it++) {
        if (it->Is(attack)) {
            it->m_frames++;
            return;
        }
    }
    UnkAttackHit_803556FC hit(attack, 5);
    hits->m_list.push_back(hit);
}

/* Counts frames down. */
extern "C" void fn_80355260(UnkAttackHitList_803555CC *hits, int frames) {
    for (HitList::iterator it = hits->m_list.begin(); it != hits->m_list.end(); it++) {
        it->m_frames -= frames;
    }
}

/* Forgets the attacks whose frames have run out or that no longer exist. */
extern "C" void fn_8035511C(UnkAttackHitList_803555CC *hits) {
    HitList::iterator it = hits->m_list.begin();
    while (it != hits->m_list.end()) {
        if (it->m_frames <= 0 || fn_80053538(&it->m_attack)) {
            it = hits->m_list.erase(it);
        } else {
            it++;
        }
    }
}
