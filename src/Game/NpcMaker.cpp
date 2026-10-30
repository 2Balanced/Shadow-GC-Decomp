/* The maker of a non-player character from its character id (fn_80076218; Game/SetCharacter.cpp
 * calls it by its address, so it keeps that name). The unit has no RTTI or strings of its own: the
 * file name is invented, as is the behaviour's description (the callees are the NPC classes' own
 * makers, one per character id from 1 to 12). */
#include "Game/CharInfo.h"
#include "boost/weak_ptr.hpp"

using boost::weak_ptr;

extern "C" {
/* The makers of the NPCs of the character ids 1 to 12 (behaviour inferred). */
weak_ptr<CharInfo> fn_8020FAE4(void);
weak_ptr<CharInfo> fn_8021262C(void);
weak_ptr<CharInfo> fn_8020F2FC(void);
weak_ptr<CharInfo> fn_802199A4(void);
weak_ptr<CharInfo> fn_802174EC(void);
weak_ptr<CharInfo> fn_80213AC4(void);
weak_ptr<CharInfo> fn_80216154(void);
weak_ptr<CharInfo> fn_80214DBC(void);
weak_ptr<CharInfo> fn_802112C4(void);
weak_ptr<CharInfo> fn_802187E4(void);
weak_ptr<CharInfo> fn_802102C4(void);
weak_ptr<CharInfo> fn_80210AD8(void);
}

/* Makes the NPC of a character id (an empty pointer for other ids). */
extern "C" weak_ptr<CharInfo> fn_80076218(int character) {
    switch (character) {
    case 1:
        return fn_8020FAE4();
    case 2:
        return fn_8021262C();
    case 3:
        return fn_8020F2FC();
    case 4:
        return fn_802199A4();
    case 5:
        return fn_802174EC();
    case 6:
        return fn_80213AC4();
    case 7:
        return fn_80216154();
    case 8:
        return fn_80214DBC();
    case 9:
        return fn_802112C4();
    case 10:
        return fn_802187E4();
    case 11:
        return fn_802102C4();
    case 12:
        return fn_80210AD8();
    }
    return weak_ptr<CharInfo>();
}
