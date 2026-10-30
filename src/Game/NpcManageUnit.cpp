/* The list of the NPCs (Player::Npc::NpcManageUnit records, class name from RTTI; the list's own
 * class name is unknown) that fn_8006F754 returns: it finds an NPC by kind or by the player it
 * partners, shows and hides them, and drops the ones not marked to be kept. File name from the
 * record's class; other names are inferred. Functions other units call by address keep their fn_
 * names (C functions taking the list). Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#define PLAYER_OBJECT_MEMBERS
#define MSL_VECTOR_MIDDLE
#define MSL_VECTOR_ERASE
#define MSL_VECTOR_ERASE_ONE
#define MSL_VECTOR_ERASE_ONE_FIRST

#define MSL_VECTOR_DESTROY_ALL_LATE
#define MSL_VECTOR_DESTROY_ALL_FIRST

#define MSL_ALGORITHM_COPY
#define MSL_ALGORITHM_REMOVE_IF
#include <vector>

#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/Npc.h"
#include "Game/Player.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using boost::shared_ptr;
using boost::weak_ptr;
using Player::Npc::Npc;

/* The mission manager (see Game/Mission.cpp): the current player at 0x20. */
struct UnkMissionManager_8006D6A8 {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ s32 m_currentPlayer;
};

extern "C" {
UnkMissionManager_8006D6A8 *fn_8006D6A8(void);
/* The NPC helpers (Game/Npc.cpp): whether it has met a player, whether it follows one, its player
 * number, marking it to be kept and whether it is. */
bool fn_800736E4(Npc *npc);
bool fn_80073708(Npc *npc);
s32 fn_80073A08(Npc *npc);
void fn_800739E0(Npc *npc);
bool fn_800739B8(Npc *npc);
/* Takes the character's tasks down (Game/PlayerManageUnit.cpp calls it for players). */
void fn_80078B54(Player::PlayerBase *chara);
}

namespace Player {
namespace Npc {

/* NpcManageUnit (class name from RTTI): an NPC of the list, which takes it down when it goes. */
class NpcManageUnit {
  public:
    NpcManageUnit(const shared_ptr<Npc> &npc);
    ~NpcManageUnit();

    /* 0x0 */ shared_ptr<Npc> m_npc;
};

/* Deletes a record (name inferred; see checked_delete below). */
inline void DeleteManageUnit(NpcManageUnit *unit) {
    delete unit;
}

} // namespace Npc
} // namespace Player

/* Retail keeps boost::checked_delete<Player::Npc::NpcManageUnit> out of line (0x80074F7C) and the
 * record's destructor at its own place (0x8007518C); this specialization calls an inline helper,
 * which gives retail's code and order (see docs/research-notes.md, EnergyCore). */
namespace boost {
template <> inline void checked_delete<Player::Npc::NpcManageUnit>(Player::Npc::NpcManageUnit *x) {
    Player::Npc::DeleteManageUnit(x);
}
} // namespace boost

using Player::Npc::NpcManageUnit;

namespace Player {
namespace Npc {

NpcManageUnit::NpcManageUnit(const shared_ptr<Npc> &npc) : m_npc(npc) {}

NpcManageUnit::~NpcManageUnit() {
    fn_80078B54(m_npc.get());
}

} // namespace Npc
} // namespace Player

/* The list of the NPCs (class name unknown; constructor 0x80075070, destructor 0x80074FD4). */
class UnkCharManager_8006F754 {
  public:
    UnkCharManager_8006F754();
    ~UnkCharManager_8006F754();

    typedef std::vector<shared_ptr<NpcManageUnit> > Units;

    /* 0x00 */ Units m_units;
    /* 0x0C */ bool m_active; /* set every frame an NPC runs its control (meaning unknown) */
    /* 0x0D */ u8 field_0xD[0x14 - 0xD];
};
SIZE_ASSERT(UnkCharManager_8006F754, 0x14);

UnkCharManager_8006F754::UnkCharManager_8006F754() {
    m_units.reserve(4);
    m_active = true;
}

UnkCharManager_8006F754::~UnkCharManager_8006F754() {}

extern "C" {

/* Adds npc to the list. */
void fn_80074D94(UnkCharManager_8006F754 *manager, const shared_ptr<Npc> &npc) {
    shared_ptr<NpcManageUnit> unit(new NpcManageUnit(npc));
    manager->m_units.push_back(unit);
}

/* Takes npc off the list. */
void fn_80074CE8(UnkCharManager_8006F754 *manager, const shared_ptr<Npc> &npc) {
    for (UnkCharManager_8006F754::Units::iterator it = manager->m_units.begin(); it != manager->m_units.end();
         ++it) {
        shared_ptr<NpcManageUnit> unit = *it;
        if (unit->m_npc == npc) {
            manager->m_units.erase(it);
            break;
        }
    }
}

/* Empties the list. */
void fn_80074CA8(UnkCharManager_8006F754 *manager) {
    manager->m_units.clear();
}

/* Whether the record's NPC is not marked to be kept. */
static bool IsKept(shared_ptr<NpcManageUnit> unit) {
    return !fn_800739B8(unit->m_npc.get());
}

/* Drops the NPCs that are not marked to be kept. */
void fn_80074C10(UnkCharManager_8006F754 *manager) {
    manager->m_units.erase(std::remove_if(manager->m_units.begin(), manager->m_units.end(), IsKept),
                           manager->m_units.end());
}

/* Shows the NPC of the current player (as control 4). */
void fn_80074B58(UnkCharManager_8006F754 *manager) {
    int player = fn_8006D6A8()->m_currentPlayer;
    for (UnkCharManager_8006F754::Units::iterator it = manager->m_units.begin(); it != manager->m_units.end();
         ++it) {
        shared_ptr<NpcManageUnit> unit = *it;
        if (fn_80073A08(unit->m_npc.get()) == player) {
            unit->m_npc->PushCommand(new Player::Npc::ShowCommand(2, 0));
        }
    }
}

/* Marks every NPC to be kept. */
void fn_80074AEC(UnkCharManager_8006F754 *manager) {
    for (UnkCharManager_8006F754::Units::iterator it = manager->m_units.begin(); it != manager->m_units.end();
         ++it) {
        shared_ptr<NpcManageUnit> unit = *it;
        fn_800739E0(unit->m_npc.get());
    }
}

/* Shows the NPC of player (greeting it) and hides the others. */
void fn_800749FC(UnkCharManager_8006F754 *manager, int player) {
    for (UnkCharManager_8006F754::Units::iterator it = manager->m_units.begin(); it != manager->m_units.end();
         ++it) {
        shared_ptr<NpcManageUnit> unit = *it;
        shared_ptr<Npc> npc = unit->m_npc;
        if (fn_80073A08(npc.get()) == player) {
            npc->PushCommand(new Player::Npc::ShowCommand(1, 4));
        } else {
            npc->PushCommand(new Player::Npc::HideCommand());
        }
    }
}

/* The NPC of a kind. */
weak_ptr<CharInfo> fn_8007488C(UnkCharManager_8006F754 *manager, int kind) {
    for (UnkCharManager_8006F754::Units::iterator it = manager->m_units.begin(); it != manager->m_units.end();
         ++it) {
        shared_ptr<NpcManageUnit> unit = *it;
        if (kind == unit->m_npc->m_kind) {
            return weak_ptr<CharInfo>(unit->m_npc);
        }
    }
    return weak_ptr<CharInfo>();
}

/* The kind of the NPC that follows the current player (0: none). */
s32 fn_800747E0(UnkCharManager_8006F754 *manager) {
    int player = fn_8006D6A8()->m_currentPlayer;
    for (UnkCharManager_8006F754::Units::iterator it = manager->m_units.begin(); it != manager->m_units.end();
         ++it) {
        shared_ptr<Npc> npc = (*it)->m_npc;
        if (fn_80073708(npc.get()) && fn_80073A08(npc.get()) == player) {
            return npc->m_kind;
        }
    }
    return 0;
}

/* The kind of the NPC of player that has met a player (0: none). */
s32 fn_800746E0(UnkCharManager_8006F754 *manager, int player) {
    for (UnkCharManager_8006F754::Units::iterator it = manager->m_units.begin(); it != manager->m_units.end();
         ++it) {
        shared_ptr<Npc> npc = (*it)->m_npc;
        if (fn_800736E4(npc.get()) && fn_80073A08(npc.get()) == player) {
            return npc->m_kind;
        }
    }
    return 0;
}

} // extern "C"

#include <vector_impl>
