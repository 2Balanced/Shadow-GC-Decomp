#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include "Game/CharInfo.h"

namespace Player {

/* The player character classes (names and bases from RTTI: Player::Player derives from
 * Player::PlayerBase, which derives from Player::PlayerObject, a CharInfo). Gadget code only
 * casts a CharInfo to Player::Player, which emits the RTTI records and base lists of the whole
 * chain; the classes' own members are not mapped. */
class PlayerObject : public CharInfo {
  public:
    virtual ~PlayerObject();
};

class PlayerBase : public PlayerObject {
  public:
    virtual ~PlayerBase();
};

class Player : public PlayerBase {
  public:
    virtual ~Player();
};

} // namespace Player

#endif
