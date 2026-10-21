#ifndef GAME_SHOOTINGGAMETABLE_H
#define GAME_SHOOTINGGAMETABLE_H

#include <dolphin/types.h>
#include "size_assert.h"

/* The Circus Park shooting game's table of 256 entries, one per balloon or game id
 * (Game/ShootingGameManager.cpp creates it and defines its accessors; PARK_BALLOON in
 * Game/BalloonGenerator.cpp and SHOOTING_PANEL in Game/ShootingGameScore.cpp read and update it).
 * All names are inferred.
 *
 * The constructors and the destructor are declared only where SHOOTINGGAME_TABLE_CTORS is defined
 * (the manager unit, which defines and uses them): each declaration takes one of CodeWarrior's @N
 * numbers and would renumber the locals of the units that only read the table. */
namespace ShootingGame {

/* The position the table records for a balloon (layout from fn_8013D49C). */
struct BalloonPosition {
    /* 0x0 */ float x;
    /* 0x4 */ float y;
    /* 0x8 */ float z;
    /* 0xC */ float w;
};

/* One entry of the table (its constructor is at 0x8013D86C). */
struct BalloonEntry {
#ifdef SHOOTINGGAME_TABLE_CTORS
    BalloonEntry();
#endif

    /* 0x00 */ float x;
    /* 0x04 */ float y;
    /* 0x08 */ float z;
    /* 0x0C */ float w;
    /* 0x10 */ s32 m_count; /* the score */
    /* 0x14 */ s32 m_state; /* 1 started (reset), 2 missed (cleared), 3 ended (hit) (inferred) */
    /* 0x18 */ u32 m_flags; /* bit 0: hit handled, bit 1: marked (inferred) */
};
SIZE_ASSERT(BalloonEntry, 0x1C);

/* The table (allocated by fn_8013D434; its constructor is at 0x8013D818). */
struct BalloonTable {
#ifdef SHOOTINGGAME_TABLE_CTORS
    BalloonTable();
    /* Declared: retail's delete tests the pointer again, as for a class with a destructor. */
    ~BalloonTable() {}
#endif

    /* 0x0000 */ BalloonEntry m_entries[0x100];
};
SIZE_ASSERT(BalloonTable, 0x1C00);

} // namespace ShootingGame

#endif
