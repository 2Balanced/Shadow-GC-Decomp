/* A relocatable data file as the game loads it (class name unknown; the project knows it as
 * UnkPath_8040E61C): a 0x20-byte header, then the data, a packed relocation stream, a table of named
 * entries, a second table and the entry names. Byte 0x17 of the header says whether the file is
 * big-endian ('B'); otherwise every word is byte-swapped as it is read. Constructing one over a
 * loaded file applies the relocations in place, turning the data's offsets into pointers. Units
 * that read sound banks, hint files and resources (Game/AudioDataBank.cpp, Game/HintController.cpp,
 * Game/ResourceOneFile.cpp, Game/ResourceOneFileReq.cpp) construct it and look entries up by name.
 * The file, class and member names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first).
 *
 * NonMatching: 2 of 4 functions match. In the constructor GC/2.7 picks other volatile registers
 * for the two table sizes (10 rows of 127); in fn_8040E4BC retail schedules the copy of name after
 * the first load (1 row of 48). Both survived about 15 spellings each (local order, separate
 * declarations and assignments, the sizes as locals or an array, member and free functions). */
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" void *memcpy(void *dst, const void *src, unsigned long n);
extern "C" int strcmp(const char *a, const char *b);

/* A word of a file in the machine's byte order. */
inline u32 FileWord(u32 value, bool bigEndian) {
    return bigEndian ? value : (u32)(((value & 0xFF) << 24) | ((value & 0xFF00) << 8) | ((value >> 8) & 0xFF00) | ((value >> 24) & 0xFF));
}

/* A named entry: its offset in the data and its name's offset in the names. */
struct UnkRelocatableEntry_8040E4BC {
    /* 0x0 */ u32 m_offset;
    /* 0x4 */ u32 m_name;
};

struct UnkPath_8040E61C {
    UnkPath_8040E61C(void *file);
    ~UnkPath_8040E61C();

    /* A word of the file in the machine's byte order. */
    u32 Read(u32 value) {
        return m_bigEndian ? value
                           : (u32)(((value & 0xFF) << 24) | ((value & 0xFF00) << 8) | ((value >> 8) & 0xFF00) |
                                   ((value >> 24) & 0xFF));
    }

    /* 0x00 */ u8 m_owned; /* the blocks below were allocated (never set here) */
    /* 0x01 */ bool m_bigEndian;
    /* 0x04 */ u32 m_dataSize;
    /* 0x08 */ u32 m_relocationSize;
    /* 0x0C */ u32 m_entryCount;
    /* 0x10 */ u32 m_count2;
    /* 0x14 */ u8 *m_data;
    /* 0x18 */ u8 *m_relocations;
    /* 0x1C */ UnkRelocatableEntry_8040E4BC *m_entries;
    /* 0x20 */ u8 *m_table2;
    /* 0x24 */ char *m_names;
};
SIZE_ASSERT(UnkPath_8040E61C, 0x28);

extern "C" {
void fn_8040E818(u8 *data, const void *stream, u32 size, bool bigEndian);
void *fn_8040E4BC(UnkPath_8040E61C *file, const char *name);
}

/* Applies a packed relocation stream to data: each code skips a number of words (in one, two or
 * four bytes, after a two-bit size tag) to the next word to relocate, which holds an offset in the
 * data (name inferred). */
void fn_8040E818(u8 *data, const void *stream, u32 size, bool bigEndian) {
    u8 tag;
    u8 byte1;
    u8 byte2;
    u8 byte3;
    const u8 *relocations = (const u8 *)stream;
    u32 offset = 0;
    while (size != 0) {
        u32 skip;
        memcpy(&tag, relocations, 1);
        relocations++;
        size--;
        switch (tag & 0xC0) {
        case 0x00:
            continue;
        case 0x40:
            skip = tag & 0x3F;
            break;
        case 0x80:
            memcpy(&byte1, relocations, 1);
            relocations++;
            size--;
            skip = ((tag & 0x3F) << 8) | byte1;
            break;
        case 0xC0:
            memcpy(&byte1, relocations, 1);
            memcpy(&byte2, relocations + 1, 1);
            memcpy(&byte3, relocations + 2, 1);
            relocations += 3;
            size -= 3;
            skip = ((tag & 0x3F) << 24) | (byte1 << 16) | (byte2 << 8) | byte3;
            break;
        default:
            continue;
        }
        offset += skip * 4;
        u32 value = *(u32 *)(data + offset);
        if (value != 0) {
            *(u32 *)(data + offset) = (u32)data + FileWord(value, bigEndian);
        }
    }
}

UnkPath_8040E61C::UnkPath_8040E61C(void *file) {
    u32 *header = (u32 *)file;
    m_owned = 0;
    m_bigEndian = ((char *)file)[0x17] == 'B';
    m_dataSize = Read(header[1]);
    m_relocationSize = Read(header[2]);
    m_entryCount = Read(header[3]);
    m_count2 = Read(header[4]);
    u32 sizes[2];
    sizes[0] = 8 * Read(header[3]);
    sizes[1] = 8 * Read(header[4]);
    u8 *block = (u8 *)file + 0x20;
    m_data = block;
    block += Read(header[1]);
    m_relocations = block;
    block += Read(header[2]);
    m_entries = (UnkRelocatableEntry_8040E4BC *)block;
    block += sizes[0];
    m_table2 = block;
    block += sizes[1];
    m_names = (char *)block;
    fn_8040E818(m_data, m_relocations, Read(header[2]), m_bigEndian);
}

UnkPath_8040E61C::~UnkPath_8040E61C() {
    if (m_owned) {
        if (m_data != 0) {
            delete[] m_data;
        }
        if (m_relocations != 0) {
            delete[] m_relocations;
        }
        if (m_entries != 0) {
            delete[] m_entries;
        }
        if (m_table2 != 0) {
            delete[] m_table2;
        }
        if (m_names != 0) {
            delete[] m_names;
        }
    }
}

/* The data of the entry called name, or 0 (name inferred). */
void *fn_8040E4BC(UnkPath_8040E61C *file, const char *name) {
    u8 bigEndian;
    char *names;
    UnkRelocatableEntry_8040E4BC *entry;
    u32 count;
    entry = file->m_entries;
    count = file->m_entryCount;
    names = file->m_names;
    bigEndian = file->m_bigEndian;
    while (count != 0) {
        if (strcmp(name, names + FileWord(entry->m_name, bigEndian)) == 0) {
            return file->m_data + FileWord(entry->m_offset, file->m_bigEndian);
        }
        count--;
        entry++;
    }
    return 0;
}
