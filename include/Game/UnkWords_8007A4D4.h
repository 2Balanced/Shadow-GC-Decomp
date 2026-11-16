#ifndef GAME_UNKWORDS_8007A4D4_H
#define GAME_UNKWORDS_8007A4D4_H

#include <dolphin/types.h>

/* A word array cleared by its constructor (class name unknown; constructor 0x8007A4D4, Reset
 * 0x80290374 in Game/PJSCameraTargetInfo.cpp): the player's state bits (word 0xA8 of a player) and
 * a member of PJSCameraTargetInfo. Names and meanings are inferred.
 *
 * It is an instance of a class template whose constructor is not inline. Game/PlayerBase.cpp shows
 * it (wave 39): retail emits the constructor there among the unit's last template instances,
 * between the shared_ptr control block's destructor and its dispose, which is where GC/2.7 puts a
 * template's out-of-line member; an inline constructor of a plain class lands among the functions
 * PlayerBase's constructor calls instead. Written straight into the constructor, the loop also has
 * retail's registers (li r4,0 for the offset, the zero a copy of it), which the same loop inlined
 * from a helper does not. The out-of-line bit test, set and clear other units call by address
 * (0x8006CD78, 0x80076CA8, 0x80076CC8) fit the same picture, so this is probably the library's
 * bit set of 65 to 96 bits; the placeholder names stay until something names it.
 *
 * The copy constructor and the assignment call the inline Copy, so they stay out of line where
 * they are used: their one copy is Player::CharaColliMove's (0x80095344 and 0x80095364, in
 * Game/PlayerCharaColliMove.cpp). Reset's one copy is an explicit specialization in
 * Game/PJSCameraTargetInfo.cpp. */
template <int N> struct UnkWordArray_8007A4D4 {
    UnkWordArray_8007A4D4();
    UnkWordArray_8007A4D4(const UnkWordArray_8007A4D4 &other) {
        Copy(other);
    }
    UnkWordArray_8007A4D4 &operator=(const UnkWordArray_8007A4D4 &other) {
        Copy(other);
        return *this;
    }

    void Reset();

    void Clear() {
        for (int i = 0; i < N; i++) {
            m_words[i] = 0;
        }
    }

    void Copy(const UnkWordArray_8007A4D4 &other) {
        for (int i = 0; i < N; i++) {
            m_words[i] = other.m_words[i];
        }
    }

    /* 0x0 */ u32 m_words[N];
};

template <int N> UnkWordArray_8007A4D4<N>::UnkWordArray_8007A4D4() {
    for (int i = 0; i < N; i++) {
        m_words[i] = 0;
    }
}

typedef UnkWordArray_8007A4D4<3> UnkWords_8007A4D4;

#endif
