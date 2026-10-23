#ifndef GAME_FLASHBACKQUEUE_H
#define GAME_FLASHBACKQUEUE_H

#include <dolphin/types.h>
#include "size_assert.h"

/* A flashback the story mode still has to play (struct name unknown): an event movie, or a stage
 * and the event played after it. Member names are inferred. */
struct UnkFlashbackEntry_803546C0 {
    /* 0x0 */ s32 m_event;   /* the movie (E<event>.sfd) or the event played after the stage */
    /* 0x4 */ s32 m_stageId; /* 0 for a movie */
};
SIZE_ASSERT(UnkFlashbackEntry_803546C0, 0x8);

/* The flashbacks still to play (class name unknown; the stage info at 0x80584720 keeps one at
 * 0x24). StageSequence::PlayFlashback (Game/StageSequencePlayFlashback.cpp) fills it and plays the
 * front entry; StageAction::FlashbackAction (Game/FlashbackAction.cpp) pops a stage's entry once
 * its event can play. The methods are in Game/StageSequencePlayFlashback.cpp. Names are inferred. */
class UnkFlashbackQueue_803546C0 {
  public:
    /* The front entry, or null when the queue is empty (0x803546C0). */
    UnkFlashbackEntry_803546C0 *Front();
    /* Removes the front entry; returns a copy of the first entry ever popped (a function-local
     * static), or null when the queue is empty (0x803546D4). */
    UnkFlashbackEntry_803546C0 *Pop();
    /* Appends an entry (0x80354750). */
    void Push(UnkFlashbackEntry_803546C0 *entry);
    /* Empties the queue (0x8035477C). */
    void Clear();

    /* 0x000 */ UnkFlashbackEntry_803546C0 m_entries[64];
    /* 0x200 */ s32 m_count;
};
SIZE_ASSERT(UnkFlashbackQueue_803546C0, 0x204);

#endif
