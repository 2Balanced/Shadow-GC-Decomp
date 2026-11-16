/* UnkFlashbackQueue_803546C0 (class name unknown; Game/FlashbackQueue.h): the queue of flashbacks
 * the story mode still has to play. StageSequence::PlayFlashback fills and plays it and
 * StageAction::FlashbackAction pops a stage's entry. The file name is not original.
 *
 * These four functions are a translation unit of their own, not the tail of
 * Game/StageSequencePlayFlashback.cpp: Pop's function-local static stands after that unit's RTTI
 * records in .sdata and its guard after that unit's guards in .sbss, word-aligned, which is where
 * a following unit's first object goes. GC/2.7 creates a function-local static where it parses
 * it, so inside the phase's unit the copy would come before the RTTI records. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/FlashbackQueue.h"

void UnkFlashbackQueue_803546C0::Clear() {
    m_count = 0;
}

void UnkFlashbackQueue_803546C0::Push(UnkFlashbackEntry_803546C0 *entry) {
    m_entries[m_count] = *entry;
    m_count++;
}

UnkFlashbackEntry_803546C0 *UnkFlashbackQueue_803546C0::Pop() {
    if (m_count != 0) {
        /* A copy of the first entry ever popped: the initializer runs once. */
        static UnkFlashbackEntry_803546C0 s_front = m_entries[0];
        for (int i = 0; i < m_count - 1; i++) {
            m_entries[i] = m_entries[i + 1];
        }
        m_count = m_count - 1;
        return &s_front;
    }
    return 0;
}

UnkFlashbackEntry_803546C0 *UnkFlashbackQueue_803546C0::Front() {
    if (m_count != 0) {
        return &m_entries[0];
    }
    return 0;
}
