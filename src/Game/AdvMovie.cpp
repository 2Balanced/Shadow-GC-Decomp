/* AdvReal::Movie::Phase (class name from RTTI): the front-end phase that plays the opening movie
 * ("Op.sfd"). Its player starts the movie (through the disc error handler when one is set up),
 * waits until the movie can be skipped or has ended, and then moves on to phase 1. The file name,
 * the player's class name and all function and member names are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include <boost/scoped_ptr.hpp>

#include "Game/AdvBase.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* The movie playing (struct name unknown; see Game/StageMovie.cpp). Setting bit 0 of the
 * halfword at 0x4 ends it (meaning inferred). */
struct UnkMovieStatus_805EE0B0 {
    /* 0x00 */ u8 field_0x0[0x4];
    /* 0x04 */ u16 m_flags;
};

extern UnkMovieStatus_805EE0B0 *lbl_805EE0B0;

/* The save data fn_800BB74C returns (class unknown; see Game/SecretKey.cpp). */
struct UnkSaveData_800BB74C;

class UnkManager_801E5E00;

extern "C" {
/* Starts playing a movie file, and draws it (behaviour inferred; see Game/StageMovie.cpp). */
bool fn_80036E78(const char *fileName, int arg1, int arg2, int arg3);
void fn_80036570(int arg0, int arg1, float arg2, float arg3);
void fn_80036244(void);
UnkSaveData_800BB74C *fn_800BB74C(void);
void fn_801E4F70(UnkSaveData_800BB74C *save, const char *fileName);
/* Whether the movie may be skipped now (behaviour inferred). */
bool fn_802B7D5C(int arg0, int arg1);

/* Set-up calls made when the phase starts (behaviour unknown). */
void *fn_801E5F0C(void);
void fn_802B57FC(void *arg0);
void fn_80173834(int arg0, float time);
void *fn_801E5CA8(void);
void fn_802C839C(void *arg0, int arg1);

/* The phase manager and the request for phase id (behaviour inferred; see
 * Game/AdvMemoryCheck.cpp). */
UnkManager_801E5E00 *fn_801E5E00(void);
void fn_802B7528(UnkManager_801E5E00 *manager, int id);
}

namespace AdvReal {
namespace Movie {

/* The phase's state (class name inferred). */
class Player {
  public:
    Player();

    /* Starts the movie and waits for it to end; always returns 1 (name inferred). */
    int Draw();
    /* Moves on once the movie has ended; always returns 1 (name inferred). */
    int Tick();

    /* 0x0 */ s32 m_state; /* 0 starting, 1 playing, 2 done */
};
SIZE_ASSERT(Player, 0x4);

/* Deletes the player (name inferred; see checked_delete below). */
inline void DeletePlayer(Player *player) {
    if (player != 0) {
        delete player;
    }
}

} // namespace Movie
} // namespace AdvReal

/* Retail keeps boost::checked_delete<Player> out of line; this specialization gives that under
 * GC/2.7 (see docs/research-notes.md and Game/EnergyCore.cpp). */
namespace boost {
template <> inline void checked_delete<AdvReal::Movie::Player>(AdvReal::Movie::Player *x) {
    AdvReal::Movie::DeletePlayer(x);
}
} // namespace boost

namespace AdvReal {
namespace Movie {

Player::Player() {
    m_state = 0;
    fn_802B57FC(fn_801E5F0C());
    fn_80173834(0, 0.0f);
    fn_802C839C(fn_801E5CA8(), 0);
}

int Player::Tick() {
    if (m_state == 2) {
        fn_802B7528(fn_801E5E00(), 1);
    }
    return 1;
}

int Player::Draw() {
    switch (m_state) {
    case 0:
        if (lbl_805EE0B0 != 0) {
            m_state = 1;
            fn_801E4F70(fn_800BB74C(), "Op.sfd");
            fn_80036244();
        } else if (fn_80036E78("Op.sfd", 0, 0, 0)) {
            m_state = 1;
            fn_801E4F70(fn_800BB74C(), "Op.sfd");
        } else {
            m_state = 2;
        }
        break;
    case 1: {
        UnkMovieStatus_805EE0B0 *movie = lbl_805EE0B0;
        if (movie != 0) {
            if (fn_802B7D5C(0, 4)) {
                movie->m_flags |= 1;
                m_state = 2;
            } else {
                fn_80036570(0, 0, 1.0f, 1.0f);
            }
        } else {
            m_state = 2;
        }
        break;
    }
    case 2:
        break;
    }
    return 1;
}

class Phase : public AdvBase {
  public:
    Phase();
    virtual ~Phase();
    virtual int Tick();
    virtual int Draw();

    /* 0x4 */ boost::scoped_ptr<Player> m_player;
};
SIZE_ASSERT(Phase, 0x8);

Phase::Phase() : m_player(new Player) {}

Phase::~Phase() {}

int Phase::Tick() {
    return m_player->Tick();
}

int Phase::Draw() {
    return m_player->Draw();
}

} // namespace Movie
} // namespace AdvReal
