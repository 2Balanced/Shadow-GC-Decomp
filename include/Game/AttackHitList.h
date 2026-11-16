#ifndef GAME_ATTACKHITLIST_H
#define GAME_ATTACKHITLIST_H

/* The attack collisions a character has been hit by recently (class name invented; the class has
 * no RTTI): a std::list of entries that keep a weak_ptr to the attack and a count of frames. The
 * classes that take hits hold one as a member (Game/PlayerBarrierHeatEffect.cpp,
 * Game/TVehicleCarMove.cpp, Game/VehicleWalker.cpp); Game/UnkAttackHitList_803555CC.cpp defines
 * its constructor (0x803555CC), its destructor (0x80355478) and the functions that work on it
 * (fn_803553E8, fn_803552DC, fn_80355260, fn_8035511C), which the users declare themselves. Only that unit sees the list itself: it defines
 * ATTACK_HIT_LIST_MEMBERS, with <list> and UnkAttackHit_803556FC, before including this header. */

#include "boost/weak_ptr.hpp"
#include "dolphin/types.h"
#include "size_assert.h"

namespace SonicteamUSA {
namespace System {
namespace CharaColli {
class CharaColliAttack;
} // namespace CharaColli
} // namespace System
} // namespace SonicteamUSA

struct UnkAttackHitList_803555CC {
    UnkAttackHitList_803555CC();
    ~UnkAttackHitList_803555CC();

#ifdef ATTACK_HIT_LIST_MEMBERS
    /* 0x0 */ std::list<UnkAttackHit_803556FC> m_list;
#else
    /* 0x0 */ u32 m_list[3]; /* a std::list<UnkAttackHit_803556FC> */
#endif
};
SIZE_ASSERT(UnkAttackHitList_803555CC, 0xC);

#endif
