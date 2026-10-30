#ifndef GAME_PLAYERSHADOWMOTIONFILES_H
#define GAME_PLAYERSHADOWMOTIONFILES_H

#include <dolphin/types.h>
#include "size_assert.h"

/* The motion files Shadow's motion and hands are made with (Game/PlayerShadow.cpp fills them,
 * Game/PlayerShadowMotionHand.cpp reads them). Struct names unknown; names inferred. */

/* A motion file of a character: the file and the motion's index in it. Empty entries are -1. */
struct UnkMotionFile_800ADE6C {
    UnkMotionFile_800ADE6C() : m_file(-1), m_index(-1) {}
    UnkMotionFile_800ADE6C(s32 file, s32 index);

    /* 0x0 */ s32 m_file;
    /* 0x4 */ s32 m_index;
};

/* The files by use (the entries' meanings are not known beyond what their readers do: m_set1
 * holds the hand models, m_common1 the hands' motion file). */
struct UnkMotionFiles_800ADE78 {
    UnkMotionFiles_800ADE78();

    /* 0x00 */ UnkMotionFile_800ADE6C m_main;
    /* 0x08 */ UnkMotionFile_800ADE6C m_set1[5];
    /* 0x30 */ UnkMotionFile_800ADE6C m_set2[5];
    /* 0x58 */ UnkMotionFile_800ADE6C m_set3[2];
    /* 0x68 */ UnkMotionFile_800ADE6C m_common1;
    /* 0x70 */ UnkMotionFile_800ADE6C m_common2;
};
SIZE_ASSERT(UnkMotionFiles_800ADE78, 0x78);

#endif
