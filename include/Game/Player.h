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

class Task;

namespace Player {

namespace Behavior {
/* What runs a player's behaviors (class name unknown; fn_800A7F14 switches it to another). */
class UnkBehaviorExecuter_800A7F14;
} // namespace Behavior

class PlayerMotion;
class PlayerTargetBase;
class SoundBase;
class WeaponBase;

/* The list at 0x254 of a player (class name unknown): weak_ptr<CharInfo> entries from 0x8; fn_800AADA4
 * returns entry index (behaviour inferred). */
class UnkCharaSlots_800AADA4;

class PlayerBase : public PlayerObject {
  public:
    virtual ~PlayerBase();
    /* CharInfo slots PlayerBase overrides (its vtable 0x80520310: 0x800798E8, 0x800799D0,
     * 0x80079918). */
    virtual void vf_0x0C();
    virtual void PushCommand(CharCommand *command);
    virtual void vf_0x14();
    /* Slots after CharInfo's (on the vtable pointer at 0x38; which class declares them is unknown;
     * names inferred from Game/PlayerShadowWeapon.cpp). */
    virtual void vf_0x30();
    virtual void vf_0x34();
    virtual void vf_0x38();
    /* Asked before a newly given weapon is shown (true: keep it hidden). */
    virtual bool IsWeaponHidden();
    /* Slots 0x40 to 0x50 (0x800735F8, 0x800735FC, 0x80073600, 0x80073604, 0x80079770 in
     * PlayerBase's vtable; roles unknown). */
    virtual void vf_0x40();
    virtual void vf_0x44();
    virtual void vf_0x48();
    virtual void vf_0x4C();
    virtual void vf_0x50();

    /* Members as Player::Shadow::ShadowWeapon reads them (Game/PlayerShadowWeapon.cpp; names
     * inferred). CharInfo's word at 0x60 points at the frame time. */
    /* 0x064 */ u8 field_0x64[0x98 - 0x64];
    /* 0x098 */ float field_0x98;
    /* 0x09C */ u32 field_0x9C;
    /* 0x0A0 */ u32 m_flags; /* a fn_80014E04 flag word (bit 7 tested by the Shadow cannon control) */
    /* 0x0A4 */ u32 field_0xA4;
    /* 0x0A8 */ u32 m_stateBits[2]; /* bit array (fn_8006CD78 tests, fn_80076CA8 sets, fn_80076CC8
                                      clears) */
    /* 0x0B0 */ u8 field_0xB0[0x224 - 0xB0];
    /* 0x224 */ Task *m_task; /* the character's task, the parent of its sub tasks (inferred) */
    /* 0x228 */ u32 field_0x228;
    /* 0x22C */ Behavior::UnkBehaviorExecuter_800A7F14 *m_behaviors; /* switches behaviors */
    /* 0x230 */ PlayerMotion *m_motion;
    /* 0x234 */ u32 field_0x234;
    /* 0x238 */ WeaponBase *m_weapon;
    /* 0x23C */ u32 field_0x23C;
    /* 0x240 */ SoundBase *m_sound; /* the character's sound task (Game/PlayerSoundBase.cpp) */
    /* 0x244 */ u8 field_0x244[0x24C - 0x244];
    /* 0x24C */ PlayerTargetBase *m_target;
    /* 0x250 */ u32 field_0x250;
    /* 0x254 */ UnkCharaSlots_800AADA4 *m_charaSlots;
    /* 0x258 */ boost::weak_ptr<Vehicle::TVehicleBase> m_vehicle; /* the vehicle ridden (inferred) */
    /* Members as Boss Sonic sets them up (Game/BossSonic.cpp; names inferred). */
    /* 0x260 */ boost::weak_ptr<PlayerBase> m_self; /* the character's weak pointer to itself */
    /* 0x268 */ Task *m_layerTask; /* made by fn_80075E30 below the layer-7 task */
    /* 0x26C */ Task *m_rootTask;  /* the parent of the character's tasks */
    /* 0x270 */ u32 field_0x270;
    /* 0x274 */ float field_0x274;
    /* 0x278 */ u8 field_0x278[0x290 - 0x278];
};

class Player : public PlayerBase {
  public:
    virtual ~Player();
};

} // namespace Player

#endif
