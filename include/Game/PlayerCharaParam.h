#ifndef GAME_PLAYERCHARAPARAM_H
#define GAME_PLAYERCHARAPARAM_H

#include <dolphin/types.h>
#include "size_assert.h"

/* A player character's parameters (struct name unknown): one 0x2C-byte record per character kind
 * (the player's word at 0x64) in the table lbl_804F1834. Meanings are inferred from the behaviors
 * that read them (Game/PlayerBehaviorFall.cpp, Game/PlayerBehaviorGround.cpp,
 * Game/PlayerBehaviorHomingAttack.cpp, Game/PlayerBehaviorJump.cpp). */
struct UnkCharaParam_804F1834 {
    /* 0x00 */ u8 field_0x0[0x4];
    /* 0x04 */ float m_runSpeed; /* scales the ground behavior's top speed */
    /* 0x08 */ bool m_canHomingAttack; /* Player::Behavior::HomingAttack may start */
    /* 0x09 */ bool m_canJumpDash;     /* Player::Behavior::JumpDash may start */
    /* 0x0A */ u8 m_hasFallEffect;
    /* 0x0B */ u8 field_0xB[0x18 - 0xB];
    /* 0x18 */ s32 m_jumpEffect; /* the effect the jump's curl starts */
    /* 0x1C */ s32 m_fallEffect;
    /* 0x20 */ u8 field_0x20[0x2C - 0x20];
};
SIZE_ASSERT(UnkCharaParam_804F1834, 0x2C);

extern const UnkCharaParam_804F1834 lbl_804F1834[];

#endif
