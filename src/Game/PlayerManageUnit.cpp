/* The list of player characters (Player::PlayerManageUnit records, class name from RTTI) that
 * gadgets search for the nearest player or a player by number. File name inferred from the class; other names
 * marked "(inferred)" or "(class name unknown)" are not original. Functions other units call by
 * address keep their fn_ names (C functions taking the object). Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#define BOOST_SP_CONVERTING_ASSIGNMENT
#define MSL_VECTOR_MIDDLE
#define MSL_VECTOR_ERASE_ONE
#define MSL_ALGORITHM_COPY
#include <vector>

#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class Line;

/* The player's members used here (struct name unknown; Player::PlayerBase leaves them unmapped). */
struct UnkPlayerFields_800A8B74 {
    /* 0x000 */ u8 field_0x0[0x54];
    /* 0x054 */ s32 m_pad;
    /* 0x058 */ u8 field_0x58[0x298 - 0x58];
    /* 0x298 */ s32 m_number;
};

#define Fields(player) reinterpret_cast<UnkPlayerFields_800A8B74 *>(player)

extern "C" {
float fn_8040D180(const Vec3f *a, const Vec3f *b); /* the squared distance */
float fn_8003C660(const Vec3f *point, const Line *line, int arg2);

/* The player (behaviour inferred). */
void SetField298__Q26Player6PlayerFl(Player::Player *player, int number); /* Player::SetField298 (Game/Player.cpp) */
void fn_80078B54(Player::Player *player);
void fn_80079404(Player::Player *player);
void fn_800794C8(Player::Player *player);

}

namespace Player {

/* PlayerManageUnit (class name from RTTI): a player of the list, which unregisters it when it
 * goes. */
class PlayerManageUnit {
  public:
    PlayerManageUnit(const boost::shared_ptr<Player> &player);
    ~PlayerManageUnit();

    /* 0x0 */ boost::shared_ptr<Player> m_player;
};

/* Deletes a record (name inferred; see checked_delete below). */
inline void DeleteManageUnit(PlayerManageUnit *unit) {
    delete unit;
}

} // namespace Player

/* Retail keeps boost::checked_delete<Player::PlayerManageUnit> out of line (0x800A9174) and the
 * record's destructor at its own place (0x800A937C); this specialization calls an inline helper,
 * which gives retail's code and order (see docs/research-notes.md, EnergyCore). */
namespace boost {
template <> inline void checked_delete<Player::PlayerManageUnit>(Player::PlayerManageUnit *x) {
    Player::DeleteManageUnit(x);
}
} // namespace boost

namespace Player {

PlayerManageUnit::PlayerManageUnit(const boost::shared_ptr<Player> &player) : m_player(player) {}

PlayerManageUnit::~PlayerManageUnit() {
    fn_80078B54(m_player.get());
}

} // namespace Player

/* The list of players (class name unknown; made by fn_80074194). */
class UnkCharaList_80074194 {
  public:
    UnkCharaList_80074194();
    ~UnkCharaList_80074194();

    typedef std::vector<boost::shared_ptr<Player::PlayerManageUnit> > Units;

    /* 0x0 */ Units m_units;
};

/* A count read by the player characters (meaning unknown; 1 here). */
s32 lbl_805E4968 = 1;

/* The number of the local player (inferred from its readers). */
s32 lbl_805EE588;

UnkCharaList_80074194::UnkCharaList_80074194() {
    m_units.reserve(4);
}

UnkCharaList_80074194::~UnkCharaList_80074194() {}

/* Adds player to the list under the first free number. */
extern "C" void fn_800A8F08(UnkCharaList_80074194 *list, const boost::shared_ptr<Player::Player> &player) {
    int number = -1;
    bool used;
    do {
        used = false;
        number++;
        for (UnkCharaList_80074194::Units::iterator it = list->m_units.begin();
             it != list->m_units.end(); ++it) {
            boost::shared_ptr<Player::PlayerManageUnit> unit = *it;
            if (number == Fields(unit->m_player.get())->m_number) {
                used = true;
                break;
            }
        }
    } while (used);
    SetField298__Q26Player6PlayerFl(player.get(), number);
    boost::shared_ptr<Player::PlayerManageUnit> unit(new Player::PlayerManageUnit(player));
    list->m_units.push_back(unit);
}

extern "C" {

/* Takes player off the list. */
void fn_800A8E5C(UnkCharaList_80074194 *list, const boost::shared_ptr<Player::Player> &player) {
    for (UnkCharaList_80074194::Units::iterator it = list->m_units.begin();
         it != list->m_units.end(); ++it) {
        boost::shared_ptr<Player::PlayerManageUnit> unit = *it;
        if (unit->m_player == player) {
            list->m_units.erase(it);
            break;
        }
    }
}

/* Calls each player's slot 0x34. */
void fn_800A8DFC(UnkCharaList_80074194 *list) {
    for (UnkCharaList_80074194::Units::iterator it = list->m_units.begin();
         it != list->m_units.end(); ++it) {
        (*it)->m_player->vf_0x34();
    }
}

/* Calls each player's slot 0x38. */
void fn_800A8D9C(UnkCharaList_80074194 *list) {
    for (UnkCharaList_80074194::Units::iterator it = list->m_units.begin();
         it != list->m_units.end(); ++it) {
        (*it)->m_player->vf_0x38();
    }
}

void fn_800A8D48(UnkCharaList_80074194 *list) {
    for (UnkCharaList_80074194::Units::iterator it = list->m_units.begin();
         it != list->m_units.end(); ++it) {
        fn_800794C8((*it)->m_player.get());
    }
}

void fn_800A8CF4(UnkCharaList_80074194 *list) {
    for (UnkCharaList_80074194::Units::iterator it = list->m_units.begin();
         it != list->m_units.end(); ++it) {
        fn_80079404((*it)->m_player.get());
    }
}

/* The player of a pad. */
boost::weak_ptr<CharInfo> fn_800A8C54(UnkCharaList_80074194 *list, int pad) {
    for (UnkCharaList_80074194::Units::iterator it = list->m_units.begin();
         it != list->m_units.end(); ++it) {
        boost::shared_ptr<Player::PlayerManageUnit> unit = *it;
        if (pad == Fields(unit->m_player.get())->m_pad) {
            return boost::weak_ptr<CharInfo>(unit->m_player);
        }
    }
    return boost::weak_ptr<CharInfo>();
}

/* The player of a number. */
boost::weak_ptr<CharInfo> fn_800A8B74(UnkCharaList_80074194 *list, int number) {
    for (UnkCharaList_80074194::Units::iterator it = list->m_units.begin();
         it != list->m_units.end(); ++it) {
        boost::shared_ptr<Player::PlayerManageUnit> unit = *it;
        if (number == Fields(unit->m_player.get())->m_number) {
            return boost::weak_ptr<CharInfo>(unit->m_player);
        }
    }
    return boost::weak_ptr<CharInfo>();
}

/* The player nearest to position. */
boost::weak_ptr<CharInfo> fn_800A8A7C(UnkCharaList_80074194 *list, const Vec3f *position) {
    boost::shared_ptr<CharInfo> nearest;
    float distance;
    for (UnkCharaList_80074194::Units::iterator it = list->m_units.begin();
         it != list->m_units.end(); ++it) {
        boost::shared_ptr<Player::PlayerManageUnit> unit = *it;
        if (nearest) {
            float d = fn_8040D180(position, &unit->m_player->m_position);
            if (distance > d) {
                nearest = unit->m_player;
                distance = d;
            }
        } else {
            nearest = unit->m_player;
            distance = fn_8040D180(position, &unit->m_player->m_position);
        }
    }
    return boost::weak_ptr<CharInfo>(nearest);
}

/* The player nearest to a line. */
boost::weak_ptr<CharInfo> fn_800A8810(UnkCharaList_80074194 *list, const Line *line) {
    boost::shared_ptr<CharInfo> nearest;
    float distance;
    for (UnkCharaList_80074194::Units::iterator it = list->m_units.begin();
         it != list->m_units.end(); ++it) {
        boost::shared_ptr<Player::PlayerManageUnit> unit = *it;
        if (nearest) {
            float d = fn_8003C660(&unit->m_player->m_position, line, 0);
            if (distance > d) {
                nearest = unit->m_player;
                distance = d;
            }
        } else {
            nearest = unit->m_player;
            distance = fn_8003C660(&unit->m_player->m_position, line, 0);
        }
    }
    return boost::weak_ptr<CharInfo>(nearest);
}

} // extern "C"
