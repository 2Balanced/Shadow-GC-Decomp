#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include "Game/CharInfo.h"
#ifdef PLAYER_BASE_IMPLEMENTATION
#include "Game/PlayerBaseMembers.h"
#endif

namespace Player {

/* The player character classes (names and bases from RTTI: Player::Player derives from
 * Player::PlayerBase, which derives from Player::PlayerObject, a CharInfo). Gadget code only
 * casts a CharInfo to Player::Player, which emits the RTTI records and base lists of the whole
 * chain; the classes' own members are not mapped. */
class PlayerObject : public CharInfo {
  public:
#ifdef PLAYER_OBJECT_MEMBERS
    /* As Game/PlayerOption.cpp sees it: the constructor and destructor are inline (the game's
     * out-of-line destructor 0x8007A418 is a weak copy), and the word at 0x64 is the character's
     * kind (name inferred). */
    PlayerObject(int type) : CharInfo(type) {
        m_kind = 0;
    }
    virtual ~PlayerObject() {}

    /* Setters of the words at 0x5C (1 for players), 0x60 (the frame time) and 0x64 (names
     * inferred). */
    void SetPlayerFlag(s32 value) {
        field_0x5C = value;
    }
    void SetFrameTime(const float *time) {
        *reinterpret_cast<const float **>(field_0x60) = time;
    }
    void SetKind(s32 kind) {
        m_kind = kind;
    }

    /* 0x64 */ s32 m_kind;
#else
    virtual ~PlayerObject();
#endif
};

} // namespace Player

namespace Vehicle {
class TVehicleBase;
}

class PickupCharInfo;
class Task;

namespace Player {

namespace Behavior {
/* What runs a player's behaviors (class name unknown; fn_800A7F14 switches it to another). */
class UnkBehaviorExecuter_800A7F14;
} // namespace Behavior

class PlayerMotion;
class PlayerTargetBase;
#ifdef PLAYER_BASE_IMPLEMENTATION
class CharaColliMove;
class UnkPlayerData_800A25E0;
#endif
class SoundBase;
class WeaponBase;

/* The list at 0x254 of a player (class name unknown): weak_ptr<CharInfo> entries from 0x8; fn_800AADA4
 * returns entry index (behaviour inferred). */
class UnkCharaSlots_800AADA4;

class PlayerBase : public PlayerObject {
  public:
#ifdef PLAYER_PLAYER_IMPLEMENTATION
    /* Constructor 0x80079F04 (type goes to CharInfo; meanings of the others unknown). */
    PlayerBase(int type, int arg1, const char *name);
#endif
#if defined(PLAYER_BASE_INLINE_SLOTS) && !defined(PLAYER_PLAYER_IMPLEMENTATION)
    PlayerBase(int type, int arg1, const char *name);
#endif
    virtual ~PlayerBase();
    /* CharInfo slots PlayerBase overrides (its vtable 0x80520310: 0x800798E8, 0x800799D0,
     * 0x80079918). */
    virtual void vf_0x0C();
    virtual void PushCommand(CharCommand *command);
    virtual void vf_0x14();
    /* Slots after CharInfo's (on the vtable pointer at 0x38; which class declares them is unknown;
     * names inferred from Game/PlayerShadowWeapon.cpp). */
    virtual void vf_0x30();
#ifdef PLAYER_BASE_IMPLEMENTATION
    /* Pure in PlayerBase (its vtable has null slots; Game/PlayerBase.cpp). */
    virtual void vf_0x34() = 0;
    virtual void vf_0x38() = 0;
    virtual bool IsWeaponHidden() = 0;
#else
    virtual void vf_0x34();
    virtual void vf_0x38();
    /* Asked before a newly given weapon is shown (true: keep it hidden). */
    virtual bool IsWeaponHidden();
#endif
    /* Slots 0x40 to 0x50 (0x800735F8, 0x800735FC, 0x80073600, 0x80073604, 0x80079770 in
     * PlayerBase's vtable; roles unknown). Player::Player's slots 0x40 and 0x44 (0x80077180 and
     * 0x8007707C) keep and drop the object the player carries (see
     * Game/PlayerBehaviorPickupObject.cpp). */
#ifdef PLAYER_BASE_INLINE_SLOTS
    /* Where PLAYER_BASE_INLINE_SLOTS is defined (Game/Npc.cpp and PlayerBase's own unit), slots
     * 0x40 to 0x4C are inline, as the game has them: Npc's vtable emits the out-of-line copies the
     * game links (0x800735F8..0x80073604, Game/Npc.cpp). Slot 0x4C returns no carried object. */
    virtual void vf_0x40() {}
    virtual void vf_0x44() {}
    virtual void vf_0x48() {}
    virtual boost::weak_ptr<PickupCharInfo> vf_0x4C() {
        return boost::weak_ptr<PickupCharInfo>();
    }
#else
#ifdef PLAYER_PICKUP_SLOT
    /* Slot 0x40 with the parameter its callers pass; the vtables of other units name the symbol
     * without it. */
    virtual void vf_0x40(const boost::weak_ptr<PickupCharInfo> &object);
#else
    virtual void vf_0x40();
#endif
    virtual void vf_0x44();
    virtual void vf_0x48();
#ifdef PLAYER_PICKUP_SLOT
    /* Slot 0x4C returns the object the player carries (Player::Player's 0x80076D24; see
     * Game/PlayerBehaviorThrow.cpp). */
    virtual boost::weak_ptr<PickupCharInfo> vf_0x4C();
#else
    virtual void vf_0x4C();
#endif
#endif
    /* Slot 0x50 returns a flag (Game/PlayerBehaviorElectricCircuit.cpp tests it). */
    virtual bool vf_0x50();

#ifdef PLAYER_BASE_IMPLEMENTATION
    /* The members as PlayerBase's own unit constructs and destroys them (Game/PlayerBase.cpp;
     * types in Game/PlayerBaseMembers.h; names inferred, as the ones below where they agree). */
    /* 0x068 */ UnkCollisionEventCallback_80086228 m_collisionCallback;
    /* 0x074 */ UnkMotionEventCallback_8007B470 m_motionCallback;
    /* 0x080 */ s32 m_counter80; /* counts of nested requests for state bits 0x17 and 0x3F */
    /* 0x084 */ s32 m_counter84;
    /* 0x088 */ s32 m_counter88; /* state bit 0x2A */
    /* 0x08C */ float m_time8C;
    /* 0x090 */ s32 m_counter90; /* state bit 0x2E */
    /* 0x094 */ float m_time94;
    /* 0x098 */ float field_0x98;
    /* 0x09C */ float field_0x9C;
    /* 0x0A0 */ UnkPlayerFlags_80014E04 m_flags;
    /* 0x0A4 */ UnkPlayerFlags_80014E04 field_0xA4;
    /* 0x0A8 */ UnkWords_8007A4D4 m_stateBits;
    /* 0x0B4 */ UnkPlayerFlags_80014E04 field_0xB4;
    /* 0x0B8 */ Vec3f field_0xB8;
    /* 0x0C4 */ Vec3f field_0xC4;
    /* 0x0D0 */ Vec3f field_0xD0;
    /* 0x0DC */ u8 field_0xDC[0xE8 - 0xDC];
    /* 0x0E8 */ Vec3f m_up;
    /* 0x0F4 */ Vec3f field_0xF4;
    /* 0x100 */ Vec3f field_0x100;
    /* 0x10C */ u8 field_0x10C[0x11C - 0x10C];
    /* 0x11C */ Mtx34f field_0x11C;
    /* 0x14C */ u8 field_0x14C[0x17C - 0x14C];
    /* 0x17C */ Vec3f field_0x17C;
    /* 0x188 */ Quatf field_0x188;
    /* 0x198 */ Mtx34f field_0x198;
    /* 0x1C8 */ Vec3f field_0x1C8;
    /* 0x1D4 */ Quatf field_0x1D4;
    /* 0x1E4 */ Mtx34f field_0x1E4;
    /* 0x214 */ UnkPlayerFlags_80014E04 field_0x214;
    /* 0x218 */ UnkPlayerOwner_80079E64<UnkPlayerInput_800A85B0> m_input;
    /* 0x21C */ boost::shared_ptr<CharCommandUnit> m_commandUnit;
    /* 0x224 */ Task *m_task;
    /* 0x228 */ CharaColliMove *m_colliMove;
    /* 0x22C */ Behavior::UnkBehaviorExecuter_800A7F14 *m_behaviors;
    /* 0x230 */ PlayerMotion *m_motion;
    /* 0x234 */ Task *m_attack;
    /* 0x238 */ WeaponBase *m_weapon;
    /* 0x23C */ u32 field_0x23C;
    /* 0x240 */ SoundBase *m_sound;
    /* 0x244 */ UnkPlayerData_800A25E0 *m_data;
    /* 0x248 */ UnkPlayerPart_80095BD4 *m_part;
    /* 0x24C */ PlayerTargetBase *m_target;
    /* 0x250 */ UnkPlayerHistory_800A82C8 *m_history;
    /* 0x254 */ UnkCharaSlots_800AADA4 *m_charaSlots;
    /* 0x258 */ boost::weak_ptr<Vehicle::TVehicleBase> m_vehicle;
    /* 0x260 */ boost::weak_ptr<PlayerBase> m_self;
    /* 0x268 */ Task *m_layerTask;
    /* 0x26C */ Task *m_rootTask;
    /* 0x270 */ u8 field_0x270[4]; /* constructed and destroyed by name (see the constructor) */
    /* 0x274 */ float field_0x274; /* the height of the shadow's ray (inferred) */
    /* 0x278 */ UnkShadow_8009368C *m_shadow;
    /* 0x27C */ float m_time27C; /* state bit 0x18 is set while it runs */
    /* 0x280 */ s32 m_weightCount[2];
    /* 0x288 */ float m_weight[2];
#else
    /* Members as Player::Shadow::ShadowWeapon reads them (Game/PlayerShadowWeapon.cpp; names
     * inferred). CharInfo's word at 0x60 points at the frame time. */
#ifdef PLAYER_OBJECT_MEMBERS
    /* 0x068 */ u8 field_0x68[0x98 - 0x68];
#else
    /* 0x064 */ u8 field_0x64[0x98 - 0x64];
#endif
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
#endif
};

/* Player (class name from RTTI; vtable 0x805200AC; Game/Player.cpp): a player-controlled character.
 * Its own slots are declared only where PLAYER_PLAYER_IMPLEMENTATION is defined. */
class Player : public PlayerBase {
  public:
#ifdef PLAYER_PLAYER_IMPLEMENTATION
    Player(int arg0, const char *name);
#endif
    virtual ~Player();
#ifdef PLAYER_PLAYER_IMPLEMENTATION
    virtual bool vf_0x18();
    virtual bool vf_0x1C();
    virtual bool vf_0x20();
    virtual bool vf_0x24();
    virtual bool vf_0x28();
    virtual void vf_0x34();
    virtual void vf_0x38();
    virtual bool IsWeaponHidden();
    virtual void vf_0x40(const boost::weak_ptr<PickupCharInfo> &object);
    virtual void vf_0x44();
    virtual void vf_0x48();
    virtual boost::weak_ptr<PickupCharInfo> vf_0x4C();

    /* Sets the word at 0x298 (meaning unknown; called by 0x800A8F08). */
    void SetField298(s32 value);

    /* 0x290 */ boost::weak_ptr<PickupCharInfo> m_object; /* the object the player carries */
    /* 0x298 */ s32 field_0x298;
#endif
};

} // namespace Player

#endif
