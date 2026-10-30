/* The motion file loader (no class with RTTI; file name and all names inferred): a motion file
 * ("NSIF") is a header, an offset table that it relocates into pointers, and chunks ("NSxx")
 * ending with "NEND" or "NOF0". fn_80010290 swaps a file to native byte order once and marks it
 * "RSIF"; it hands camera, light and motion chunks to their own swaps (Game/CameraMotion.cpp,
 * Game/LightMotion.cpp, Game/MotionTrack.cpp). fn_800105C8 finds a chunk by name. The unit is
 * split by its .sdata2 (the four names at 0x805F17E0); the light swap that follows has its own
 * .data (a jump table, 8-byte aligned). Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include <dolphin/types.h>

/* A chunk: its name, the size that follows the first two words, and the offset of its data from
 * the chunk. */
struct UnkMotionChunk_800105C8 {
    /* 0x00 */ char m_name[4];
    /* 0x04 */ u32 m_size; /* of what follows this word */
    /* 0x08 */ u32 m_dataOffset;
    /* 0x0C */ u32 field_0xC;
};

/* A file's header. */
struct UnkMotionFile_80010290 {
    /* 0x00 */ char m_magic[4];
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ u32 field_0x8;
    /* 0x0C */ u32 m_chunkOffset; /* the chunks, the base of the offsets */
    /* 0x10 */ u32 field_0x10;
    /* 0x14 */ u32 m_tableOffset; /* the offset table */
    /* 0x18 */ u32 field_0x18;
    /* 0x1C */ u32 field_0x1C;
};

/* The offset table: the offsets of the words to relocate. */
struct UnkMotionOffsets_80010290 {
    /* 0x00 */ u32 field_0x0;
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ u32 m_count;
    /* 0x0C */ u32 field_0xC;
    /* 0x10 */ u32 m_offsets[1];
};

extern "C" {
int fn_803AC8DC(const char *a, const char *b, u32 n);
char *strcpy(char *dest, const char *src);
/* The chunk swaps of cameras (Game/CameraMotion.cpp), lights and motions. */
void fn_8000F8B4(void *data);
void fn_800106B4(void *data);
void fn_80011244(void *data);
int fn_80010290(char *file);
}

/* Reverses the bytes of a word in place. */
inline void SwapBytes(void *word) {
    u8 *bytes = static_cast<u8 *>(word);
    u8 byte0 = bytes[0];
    u8 byte2 = bytes[2];
    bytes[0] = bytes[3];
    bytes[3] = byte0;
    bytes[2] = bytes[1];
    bytes[1] = byte2;
}

/* The chunk after chunk. */
#define NEXT_CHUNK(chunk)     reinterpret_cast<UnkMotionChunk_800105C8 *>(reinterpret_cast<u8 *>(&(chunk)->m_size + 1) + (chunk)->m_size)

/* Swaps file once, unless it is swapped already. */
extern "C" int fn_80010648(char *file) {
    if (fn_803AC8DC(file, "RSIF", 4) == 0) {
        return 1;
    }
    if (fn_803AC8DC(file, "NSIF", 4) != 0) {
        return 0;
    }
    return fn_80010290(file);
}

/* The chunk named like name (its last three letters) from chunk on, or 0. */
extern "C" UnkMotionChunk_800105C8 *fn_800105C8(UnkMotionChunk_800105C8 *chunks, const char *name) {
    UnkMotionChunk_800105C8 *chunk = chunks;
    const char *wanted = name + 1;
    char *chunkName = chunk->m_name + 1;
    while (fn_803AC8DC(chunkName, "END", 3) != 0) {
        if (fn_803AC8DC(chunkName, wanted, 3) == 0) {
            return chunk;
        }
        chunk = NEXT_CHUNK(chunk);
        chunkName = chunk->m_name + 1;
    }
    return 0;
}

/* Swaps a motion file to native byte order and relocates its offsets. */
extern "C" int fn_80010290(char *file) {
    if (fn_803AC8DC(file, "RSIF", 4) == 0) {
        return 1;
    }
    if (fn_803AC8DC(file, "NSIF", 4) != 0) {
        return 0;
    }
    UnkMotionFile_80010290 *header = reinterpret_cast<UnkMotionFile_80010290 *>(file);
    SwapBytes(&header->field_0x4);
    SwapBytes(&header->field_0x8);
    SwapBytes(&header->m_chunkOffset);
    SwapBytes(&header->field_0x10);
    SwapBytes(&header->m_tableOffset);
    SwapBytes(&header->field_0x18);
    SwapBytes(&header->field_0x1C);
    UnkMotionOffsets_80010290 *table = reinterpret_cast<UnkMotionOffsets_80010290 *>(file + header->m_tableOffset);
    u32 *offset = table->m_offsets;
    SwapBytes(&table->field_0x4);
    SwapBytes(&table->m_count);
    SwapBytes(&table->field_0xC);
    u8 *base = reinterpret_cast<u8 *>(file + header->m_chunkOffset);
    for (u32 i = 0; i < table->m_count; i++) {
        SwapBytes(offset);
        u32 *word = reinterpret_cast<u32 *>(base + (*offset++ & ~3));
        SwapBytes(word);
        *word += reinterpret_cast<u32>(base);
    }
    UnkMotionChunk_800105C8 *chunk = reinterpret_cast<UnkMotionChunk_800105C8 *>(base);
    char *chunkName = chunk->m_name + 1;
    while (fn_803AC8DC(chunkName, "END", 3) != 0 && fn_803AC8DC(chunkName, "OF0", 3) != 0) {
        SwapBytes(&chunk->m_size);
        SwapBytes(&chunk->m_dataOffset);
        SwapBytes(&chunk->field_0xC);
        if (chunk->m_name[1] == 'S') {
            void *data = reinterpret_cast<u8 *>(chunk) + chunk->m_dataOffset;
            switch (chunk->m_name[2]) {
            case 'M':
                fn_80011244(data);
                break;
            case 'C':
                fn_8000F8B4(data);
                break;
            case 'L':
                fn_800106B4(data);
                break;
            }
        }
        chunk = NEXT_CHUNK(chunk);
        chunkName = chunk->m_name + 1;
    }
    strcpy(file, "RSIF");
    return 1;
}
