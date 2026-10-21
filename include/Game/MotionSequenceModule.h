#ifndef GAME_MOTIONSEQUENCEMODULE_H
#define GAME_MOTIONSEQUENCEMODULE_H

#include <dolphin/types.h>
#include "size_assert.h"

struct _stMtnPlayWork;
class MotionSequenceModule;

/* One step of a motion sequence (8 bytes; layout from fn_800C9178, which plays it; names
 * inferred): the motion, two frame numbers, a blend time and flags (bits 0-1: the motion loops). */
struct MotionSequenceStep {
    /* 0x0 */ s16 motion; /* negative: keep the sequence's own motion */
    /* 0x2 */ s16 field_0x2;
    /* 0x4 */ s16 field_0x4;
    /* 0x6 */ s8 blend;
    /* 0x7 */ u8 flags;
};
SIZE_ASSERT(MotionSequenceStep, 0x8);

/* A motion sequence: its steps and what follows it (names inferred). */
struct MotionSequence {
    /* 0x0 */ const MotionSequenceStep *steps;
    /* 0x4 */ s8 count;
    /* 0x5 */ s8 next; /* the sequence to play next; -1 hold, -2 restart, -3 reset */
};
SIZE_ASSERT(MotionSequence, 0x8);

/* The sequences a module chooses from (names inferred). */
struct MotionSequenceTable {
    /* 0x0 */ const MotionSequence *sequences;
    /* 0x4 */ s8 count;
};
SIZE_ASSERT(MotionSequenceTable, 0x8);

extern "C" {
/* Starts sequence index of the table; false when there is none (behaviour inferred from its
 * body). */
bool fn_800C9484(MotionSequenceModule *module, int index);
/* Starts sequence (behaviour inferred). */
void fn_800C961C(MotionSequenceModule *module, const MotionSequence *sequence);
/* Advances the sequence by dt on work, whose motion numbers start at base, and updates the end
 * flag from work (behaviour inferred). */
void fn_800C9178(MotionSequenceModule *module, _stMtnPlayWork *work, int base, float dt);
void fn_800C9554(MotionSequenceModule *module, _stMtnPlayWork *work);
}

/* MotionSequenceModule (class name from RTTI; vtable at 0x8052A510, which the VINE gadget's unit
 * holds): plays a table of motion sequences on a motion work. EnemyMotionSequenceModule derives
 * from it. Its constructor and destructor are inline (weak copies at 0x8014F144 and 0x8014EFB4,
 * in Vine.cpp); its other code is at 0x800C9178 to 0x800C961C. Field and method names are
 * inferred. */
class MotionSequenceModule {
  public:
    /* Retail keeps the constructor out of line (it calls the inline Clear). */
    MotionSequenceModule() {
        Clear();
    }
    virtual ~MotionSequenceModule() {}
    /* Called when a looping step ends; true when it changed the sequence (0x800C954C; meaning
     * inferred). */
    virtual bool vf_0x0C() {
        return false;
    }
    /* Starts the default sequence again (0x800C9528). */
    virtual void Reset() {
        fn_800C9484(this, m_defaultIndex);
    }

    /* Resets every field (name inferred). */
    void Clear() {
        m_table = 0;
        m_sequence = 0;
        m_previous = 0;
        m_next = -3;
        m_motion = -1;
        m_started = 0;
        m_looping = 0;
        m_finished = false;
        m_ended = 0;
        field_0x1C = 0;
        field_0x1E = 0;
        m_index = 0;
        m_defaultIndex = 0;
        m_stepCount = 0;
        m_step = 0;
        field_0x30 = 0;
        m_time = 0.0f;
        m_stepTime = 0.0f;
        field_0x1D = 1;
    }

    /* Starts sequence index when the table has it. */
    void Start(int index) {
        if (m_table != 0 && index < m_table->count) {
            m_index = index;
            fn_800C961C(this, &m_table->sequences[index]);
        }
    }

    bool HasEnded() const {
        return m_finished;
    }

    /* 0x04 */ const MotionSequenceTable *m_table;
    /* 0x08 */ const MotionSequence *m_sequence;
    /* 0x0C */ const MotionSequence *m_previous;
    /* 0x10 */ s32 m_next;
    /* 0x14 */ s32 m_motion;
    /* 0x18 */ u8 m_started;
    /* 0x19 */ u8 m_looping;
    /* 0x1A */ bool m_finished;
    /* 0x1B */ u8 m_ended;
    /* 0x1C */ u8 field_0x1C;
    /* 0x1D */ u8 field_0x1D;
    /* 0x1E */ u8 field_0x1E;
    /* 0x20 */ s32 m_index;
    /* 0x24 */ s32 m_defaultIndex;
    /* 0x28 */ s32 m_stepCount;
    /* 0x2C */ s32 m_step;
    /* 0x30 */ s32 field_0x30;
    /* 0x34 */ float m_time;
    /* 0x38 */ float m_stepTime;
};
SIZE_ASSERT(MotionSequenceModule, 0x3C);

#endif
