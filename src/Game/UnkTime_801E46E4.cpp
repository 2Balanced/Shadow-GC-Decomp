/* The out-of-line members of a time split into minutes, seconds and hundredths (class name unknown;
 * see Game/GoalAction.cpp and Game/TGiScoreDisplay.cpp), and the object that holds the game's build
 * date (class name unknown). No RTTI names this unit; the file is named after the time class.
 * Names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). The build date is written out: it is
 * what __DATE__ and __TIME__ gave when the game was built. */
#include <dolphin/types.h>
#include "size_assert.h"
#include <stdio.h>

/* A time split into minutes, seconds and hundredths (class name unknown). */
class UnkTime_801E46E4 {
  public:
    UnkTime_801E46E4();
    UnkTime_801E46E4(float time);

    /* Sets the time from minutes, seconds and hundredths (name inferred). */
    void Set(s8 minutes, s8 seconds, s8 hundredths);
    /* Sets the minutes, seconds and hundredths from time (name inferred). */
    void Split(float time);

    /* 0x0 */ s32 m_minutes;
    /* 0x4 */ s8 m_seconds;
    /* 0x5 */ s8 m_hundredths;
    /* 0x8 */ float m_time;
};
SIZE_ASSERT(UnkTime_801E46E4, 0xC);

/* The game's build date and time as text (class name unknown). */
class UnkBuildDate_801E4700 {
  public:
    UnkBuildDate_801E4700();

    /* 0x00 */ char m_text[0x40];
};
SIZE_ASSERT(UnkBuildDate_801E4700, 0x40);

UnkBuildDate_801E4700::UnkBuildDate_801E4700() {
    sprintf(m_text, "%s %s", "Oct 10 2005", "23:48:24");
}

UnkTime_801E46E4::UnkTime_801E46E4() {
    m_time = 0.0f;
    m_minutes = 0;
    m_seconds = 0;
    m_hundredths = 0;
}

UnkTime_801E46E4::UnkTime_801E46E4(float time) {
    m_time = time;
    s32 total = (s32)time;
    s32 minutes = total / 60;
    m_hundredths = (s32)(100.0f * (time - (float)total));
    m_minutes = minutes;
    m_seconds = total % 60;
}

void UnkTime_801E46E4::Set(s8 minutes, s8 seconds, s8 hundredths) {
    m_minutes = minutes;
    m_seconds = seconds;
    m_hundredths = hundredths;
    m_time = (float)hundredths / 100.0f + (float)(minutes * 60 + seconds);
}

void UnkTime_801E46E4::Split(float time) {
    s32 total = (s32)time;
    s32 minutes = total / 60;
    m_hundredths = (s32)(100.0f * (time - (float)total));
    m_minutes = minutes;
    m_seconds = total % 60;
}

/* Not called anywhere: the linker strips it from the game. Defined last, it makes GC/2.7 pool the
 * default constructor's zero first in .sdata2, as retail has it (retail pools constants in source
 * order; see docs/research-notes.md). */
void UnusedClear(UnkTime_801E46E4 *time) {
    time->m_time = 0.0f;
}
