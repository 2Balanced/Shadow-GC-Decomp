#ifndef GAME_MONSTERCHARINFO_H
#define GAME_MONSTERCHARINFO_H

#include "Game/GadgetCharInfo.h"
#include "size_assert.h"

class Monster;
class MonsterControl;

/* MonsterCharInfo (class name from RTTI): the character info of the Monster gadget (Monster.cpp),
 * through which a MonsterControl (MonsterControl.cpp) finds and commands it. */
class MonsterCharInfo : public GadgetCharInfo {
  public:
    MonsterCharInfo(Monster *monster) : GadgetCharInfo(0x4B), m_monster(monster) {}
    virtual ~MonsterCharInfo();

    /* Sets the word at 0x5C (4 here; 1 for a player, see CharInfo.h; name inferred). An inline
     * function in the original: retail calls the shared_ptr's operator-> out of line before it. */
    void SetField5C(s32 value) {
        field_0x5C = value;
    }

    /* Passes command to the monster, then lets control react (name inferred). */
    void Issue(int command, MonsterControl *control);

    /* 0x6C */ Monster *m_monster;
};
SIZE_ASSERT(MonsterCharInfo, 0x70);

#endif
