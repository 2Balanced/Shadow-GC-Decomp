#ifndef GAME_PLAYERBASEMEMBERS_H
#define GAME_PLAYERBASEMEMBERS_H

/* The member types of Player::PlayerBase as its own unit (Game/PlayerBase.cpp) constructs and
 * destroys them; Game/Player.h includes this header only where PLAYER_BASE_IMPLEMENTATION is
 * defined. Class names are unknown unless stated; names and meanings are inferred. */
#include "Game/MathTypes.h"
#include "Game/PlayerMotionEvent.h"
#include "Game/Quaternion.h"
#include "Game/UnkWords_8007A4D4.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>

class CharCommandUnit;
struct UnkPlayerInput_800A85B0;
struct UnkShadow_8009368C;
struct UnkPlayerPart_80095BD4;
class UnkPlayerHistory_800A82C8;

extern "C" {
/* A flag word: cleared (see Game/BirdBase.h). */
void fn_80014E04(u32 *flags);
/* The constructor and the destructor of the player's member at 0x270 (class and contents unknown;
 * 0x800938C8 and 0x8009388C, Game/PlayerUtil.cpp). symbols.txt names the destructor after
 * UnkShadow_8009368C, the 0x28-byte object at 0x278, which the player deletes without a
 * destructor call; Game/PlayerBase.cpp calls both by name. */
void fn_800938C8(void *member);
void __dt__18UnkShadow_8009368CFv(void *member, short flags);
}

/* A flag word whose constructor clears it. */
struct UnkPlayerFlags_80014E04 {
    UnkPlayerFlags_80014E04() {
        fn_80014E04(&m_bits);
    }

    /* 0x0 */ u32 m_bits;
};

/* The owner of the player's pad input at 0x218 (class name unknown): it deletes the input when
 * it goes. Its destructor calls the inline Get, so it stays out of line (0x80079E64). */
template <class T> class UnkPlayerOwner_80079E64 {
  public:
    explicit UnkPlayerOwner_80079E64(T *p) : m_ptr(p) {}
    ~UnkPlayerOwner_80079E64() {
        delete Get();
    }
    T *Get() const {
        return m_ptr;
    }

    /* 0x0 */ T *m_ptr;
};

#endif
