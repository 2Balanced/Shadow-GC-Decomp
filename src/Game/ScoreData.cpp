/* The score table: "common/ScoreData.bin", loaded with the stage (fn_802064E4) and freed with it
 * (fn_80206494). Each of its rows holds the five values of a ScoreCommand; fn_8020642C sends a
 * character the command of a row (the gadgets call it with the row of what was destroyed or
 * collected). Two initializers of a record (fn_802062FC, fn_80206354) come first. File name
 * inferred from the data file. Names marked "(name inferred)" are not original, nor are the fn_
 * names the functions other units call keep. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" void *memset(void *dst, int value, unsigned long count);

/* ScoreCommand (command type 0x1C) is declared in Game/CharCommand.h. */

/* A row of the score table: a ScoreCommand's values (struct names unknown). */
struct UnkScoreValues {
    UnkScoreValues(s32 value0, s32 value1, s32 value2, s32 value3, s32 value4)
        : m_value0(value0), m_value1(value1), m_value2(value2), m_value3(value3),
          m_value4(value4) {}

    /* 0x00 */ s32 m_value0;
    /* 0x04 */ s32 m_value1;
    /* 0x08 */ s32 m_value2;
    /* 0x0C */ s32 m_value3;
    /* 0x10 */ s32 m_value4;
};

struct UnkScoreRow {
    /* 0x00 */ UnkScoreValues m_command;
    /* 0x14 */ s32 field_0x14;
};

/* The loaded table file (class name unknown; 0x28 bytes, constructed from the file's data at
 * 0x8040E61C and destroyed at 0x8040E57C). The rows start at the first column. */
struct UnkTableFile_8040E61C {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ UnkScoreRow **m_columns;
    /* 0x18 */ u8 field_0x18[0x28 - 0x18];
};

extern "C" UnkTableFile_8040E61C *fn_8040E61C(UnkTableFile_8040E61C *table, void *data);
extern "C" void fn_8040E57C(UnkTableFile_8040E61C *table, int free);

/* The record the two initializers below set up (struct name unknown). */
struct UnkRecord_802062FC {
    /* 0x00 */ u8 field_0x0[0x5];
    /* 0x05 */ u8 field_0x5;
    /* 0x06 */ u8 field_0x6[0x8 - 0x6];
    /* 0x08 */ u8 field_0x8[0x50];
    /* 0x58 */ u8 field_0x58;
    /* 0x59 */ u8 field_0x59;
    /* 0x5A */ u8 field_0x5A;
    /* 0x5C */ s32 field_0x5C;
    /* 0x60 */ u8 field_0x60[0x70 - 0x60];
    /* 0x70 */ s32 field_0x70;
    /* 0x74 */ s32 field_0x74;
};

class UnkFileSystem_800371D8;

/* SaveRecord.cpp's 30000.0f. */
extern const float lbl_805F4BF0;

extern "C" {
UnkFileSystem_800371D8 *fn_800371D8(void);
/* Reads a whole file; size receives its length (behaviour inferred). */
void *fn_8004509C(UnkFileSystem_800371D8 *fileSystem, const char *path, u32 *size);
void fn_80049B34(void *data);
}

/* An unused copy of 30000 (name and meaning unknown); setting it is the unit's static
 * initializer. */
int lbl_805EFC28 = lbl_805F4BF0;

/* The file's data, the table and its rows (names inferred). */
static void *s_file;
static UnkTableFile_8040E61C *s_table;
static UnkScoreRow *s_rows;

/* Loads the table (name inferred). */
extern "C" void fn_802064E4(void) {
    u32 size = 0;
    s_file = fn_8004509C(fn_800371D8(), "common/ScoreData.bin", &size);
    if (s_file != 0) {
        UnkTableFile_8040E61C *table = (UnkTableFile_8040E61C *)operator new(0x28);
        if (table != 0) {
            table = fn_8040E61C(table, s_file);
        }
        s_table = table;
        s_rows = s_table->m_columns[0];
    }
}

/* Frees the table (name inferred). */
extern "C" void fn_80206494(void) {
    if (s_table != 0) {
        fn_8040E57C(s_table, 1);
        s_table = 0;
    }
    if (s_file != 0) {
        fn_80049B34(s_file);
        s_file = 0;
    }
}

extern "C" int fn_802063BC(const boost::shared_ptr<CharInfo> &character,
                           const UnkScoreValues &values);

/* Sends character the command of row (none for row 0x27; name inferred). */
extern "C" int fn_8020642C(const boost::shared_ptr<CharInfo> &character, int row) {
    if (row == 0x27) {
        return 0;
    }
    return fn_802063BC(character,
                       UnkScoreValues(s_rows[row].m_command.m_value0, s_rows[row].m_command.m_value1,
                                      s_rows[row].m_command.m_value2, s_rows[row].m_command.m_value3,
                                      s_rows[row].m_command.m_value4));
}

/* Sends character a ScoreCommand with values (name inferred). */
extern "C" int fn_802063BC(const boost::shared_ptr<CharInfo> &character,
                           const UnkScoreValues &values) {
    character->PushCommand(
        new ScoreCommand(values.m_value0, values.m_value1, values.m_value2, values.m_value3,
                         values.m_value4));
    return 1;
}

/* Sets up a record (name inferred). */
extern "C" void fn_80206354(UnkRecord_802062FC *record) {
    memset(record->field_0x8, 0, sizeof(record->field_0x8));
    record->field_0x5 = 1;
    record->field_0x58 = 0;
    record->field_0x59 = 0;
    record->field_0x5A = 0;
    record->field_0x5C = 5;
    record->field_0x70 = 14;
    record->field_0x74 = 15;
}

/* Sets up a record (name inferred). */
extern "C" void fn_802062FC(UnkRecord_802062FC *record) {
    memset(record->field_0x8, 0, sizeof(record->field_0x8));
    record->field_0x5 = 1;
    record->field_0x58 = 0;
    record->field_0x59 = 0;
    record->field_0x5A = 0;
    record->field_0x5C = 5;
}
