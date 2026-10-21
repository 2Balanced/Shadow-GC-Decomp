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

} // namespace Player

namespace Vehicle {
class TVehicleBase;
}

namespace Player {

namespace Behavior {
/* What runs a player's behaviors (class name unknown; fn_800A7F14 switches it to another). */
class UnkBehaviorExecuter_800A7F14;
} // namespace Behavior

class PlayerMotion;
class PlayerTargetBase;
class WeaponBase;

/* The list at 0x254 of a player (class name unknown): weak_ptr<CharInfo> entries from 0x8; fn_800AADA4
 * returns entry index (behaviour inferred). */
class UnkCharaSlots_800AADA4;

class PlayerBase : public PlayerObject {
  public:
    virtual ~PlayerBase();
    /* Slots after CharInfo's (on the vtable pointer at 0x38; which class declares them is unknown;
     * names inferred from Game/PlayerShadowWeapon.cpp). */
    virtual void vf_0x30();
    virtual void vf_0x34();
    virtual void vf_0x38();
    /* Asked before a newly given weapon is shown (true: keep it hidden). */
    virtual bool IsWeaponHidden();

    /* Members as Player::Shadow::ShadowWeapon reads them (Game/PlayerShadowWeapon.cpp; names
     * inferred). CharInfo's word at 0x60 points at the frame time. */
    /* 0x064 */ u8 field_0x64[0x98 - 0x64];
    /* 0x098 */ float field_0x98;
    /* 0x09C */ u32 field_0x9C;
    /* 0x0A0 */ u32 m_flags; /* a fn_80014E04 flag word (bit 7 tested by the Shadow cannon control) */
    /* 0x0A4 */ u32 field_0xA4;
    /* 0x0A8 */ u32 m_stateBits[2]; /* bit array (fn_8006CD78 tests, fn_80076CA8 sets, fn_80076CC8
                                      clears) */
    /* 0x0B0 */ u8 field_0xB0[0x22C - 0xB0];
    /* 0x22C */ Behavior::UnkBehaviorExecuter_800A7F14 *m_behaviors; /* switches behaviors */
    /* 0x230 */ PlayerMotion *m_motion;
    /* 0x234 */ u32 field_0x234;
    /* 0x238 */ WeaponBase *m_weapon;
    /* 0x23C */ u8 field_0x23C[0x24C - 0x23C];
    /* 0x24C */ PlayerTargetBase *m_target;
    /* 0x250 */ u32 field_0x250;
    /* 0x254 */ UnkCharaSlots_800AADA4 *m_charaSlots;
    /* 0x258 */ boost::weak_ptr<Vehicle::TVehicleBase> m_vehicle; /* the vehicle ridden (inferred) */
};

class Player : public PlayerBase {
  public:
    virtual ~Player();
};

} // namespace Player

#endif
