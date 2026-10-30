#ifndef GAME_UNKWORDS_8007A4D4_H
#define GAME_UNKWORDS_8007A4D4_H

#include <dolphin/types.h>

/* A word array cleared by its constructor (struct name unknown; constructor 0x8007A4D4, Reset
 * 0x80290374 in Game/PJSCameraTargetInfo.cpp): the player's state bits (word 0xA8 of a player) and
 * a member of PJSCameraTargetInfo. The constructor calls the inline Clear, so it stays out of line
 * where it is used. The copy constructor and the assignment call the inline Copy, so they too are
 * out of line: their one copy is Player::CharaColliMove's (0x80095344 and 0x80095364, in
 * Game/PlayerCharaColliMove.cpp). Names and meanings are inferred. */
struct UnkWords_8007A4D4 {
    UnkWords_8007A4D4() {
        Clear();
    }
    UnkWords_8007A4D4(const UnkWords_8007A4D4 &other) {
        Copy(other);
    }
    UnkWords_8007A4D4 &operator=(const UnkWords_8007A4D4 &other) {
        Copy(other);
        return *this;
    }

    void Reset();

    void Clear() {
        for (int i = 0; i < 3; i++) {
            m_words[i] = 0;
        }
    }

    void Copy(const UnkWords_8007A4D4 &other) {
        for (int i = 0; i < 3; i++) {
            m_words[i] = other.m_words[i];
        }
    }

    /* 0x0 */ u32 m_words[3];
};

#endif
