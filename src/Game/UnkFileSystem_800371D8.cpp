/* The game's file system (UnkFileSystem_800371D8, class name unknown; Game/TBGRead.h): file sizes,
 * whole-file loads, chunked reads, directory listings and RenderWare streams, and the queueing of
 * background reads (Game/TBGRead.cpp). Other units call these by their fn_ names, with the file
 * system (fn_800371D8) as first argument. The file name and the names here are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/TBGRead.h"
#include <dolphin/types.h>
#include "size_assert.h"
#include <string.h>

struct RwStream;

/* The SDK's DVD directory, directory entry and file (the parts used here). */
struct UnkDVDDir_8037A2E8 {
    /* 0x0 */ u32 field_0x0[3];
};

struct UnkDVDDirEntry_8037A3A8 {
    /* 0x0 */ u32 m_entryNum;
    /* 0x4 */ s32 m_isDir;
    /* 0x8 */ char *m_name;
};

struct UnkDVDFileInfo_80379C58 {
    /* 0x00 */ u8 field_0x0[0x34];
    /* 0x34 */ u32 m_length;
    /* 0x38 */ u32 field_0x38;
};

/* One entry of a directory listing (fn_80044EAC). */
struct UnkDirEntry_80044EAC {
    /* 0x000 */ char m_name[0x100];
    /* 0x100 */ s32 m_size; /* -1 for a directory or a file that does not open */
    /* 0x104 */ s32 m_type; /* 1 directory, 2 file */
};
SIZE_ASSERT(UnkDirEntry_80044EAC, 0x108);

extern "C" {
/* The game's heap functions (allocate, free). */
void *fn_80049B90(u32 size, int heap);
void fn_80049B34(void *block);
/* The game's fopen, and the RenderWare file functions fread, fclose and whether a file exists, and
 * RwStreamOpen (behaviours inferred). */
UnkFile_8047A178 *fn_800513E0(const char *path, const char *mode);
u32 fn_8047A3E0(void *buffer, u32 size, u32 count, UnkFile_8047A178 *file);
s32 fn_8047A388(UnkFile_8047A178 *file);
s32 fn_8047A67C(const char *path);
RwStream *fn_80481368(int type, int accessType, void *data);
/* The file system's path of a file, and its release (see Game/FadeTask.cpp). */
char *fn_80051810(const char *name);
void fn_80051830(char *path);
/* DVDOpenDir, DVDReadDir, DVDOpen and DVDClose (by their arguments). */
s32 fn_8037A2E8(const char *path, UnkDVDDir_8037A2E8 *dir);
s32 fn_8037A3A8(UnkDVDDir_8037A2E8 *dir, UnkDVDDirEntry_8037A3A8 *entry);
s32 fn_80379C58(const char *path, UnkDVDFileInfo_80379C58 *info);
s32 fn_80379D20(UnkDVDFileInfo_80379C58 *info);
/* strstr. */
char *fn_803AC6C0(const char *string, const char *pattern);

bool fn_80044E80(const char *path);
}

/* The number of entries a directory listing has room for. */
s32 lbl_805E3850 = 0x500;

UnkFileSystem_800371D8::UnkFileSystem_800371D8() {
    m_exists = fn_80044E80;
}

UnkFileSystem_800371D8::~UnkFileSystem_800371D8() {}

/* Sets the task the read tasks hang below and starts the read manager. */
extern "C" void fn_80045220(UnkFileSystem_800371D8 *fileSystem, Task *task) {
    fileSystem->m_task = task;
    new TBGReadManager();
}

/* Reads all of file into data and returns the number of bytes read. */
inline u32 ReadWhole(UnkFile_8047A178 *file, void *data) {
    return file->m_size * fn_8047A3E0(data, file->m_size, 1, file);
}

extern "C" u32 fn_80045128(UnkFileSystem_800371D8 *fileSystem, UnkFile_8047A178 *file, void **data,
                           u32 size, s32 chunk) {
    if (file != 0) {
        if (*data == 0) {
            *data = fn_80049B90(file->m_size, 2);
        }
        u32 total;
        if (chunk != 0) {
            s32 remaining = file->m_size;
            total = 0;
            u8 *p = (u8 *)*data;
            while (remaining != 0) {
                if (remaining > chunk) {
                    total += chunk * fn_8047A3E0(p, chunk, 1, file);
                    remaining -= chunk;
                    p += chunk;
                } else {
                    total += ((remaining + 0x1F) & ~0x1F) * fn_8047A3E0(p, remaining, 1, file);
                    remaining = 0;
                }
            }
        } else {
            total = ReadWhole(file, *data);
        }
        return total;
    }
    return 0;
}

/* Reads all of file into newly allocated memory (*data) and returns its size. */
inline u32 AllocAndRead(UnkFile_8047A178 *file, void **data) {
    if (file != 0) {
        *data = fn_80049B90(file->m_size, 2);
        return file->m_size * fn_8047A3E0(*data, file->m_size, 1, file);
    }
    return 0;
}

/* Loads the file path and returns its data (0 if it does not open); its size goes to *size. */
extern "C" void *fn_8004509C(UnkFileSystem_800371D8 *fileSystem, const char *path, u32 *size) {
    UnkFile_8047A178 *file;
    void *data = 0;
    file = fn_800513E0(path, "rb");
    if (file != 0) {
        u32 read = AllocAndRead(file, &data);
        if (size != 0) {
            *size = read;
        }
        fn_8047A388(file);
    }
    return data;
}

/* Queues request in queue if its file exists. */
extern "C" bool fn_80045044(UnkFileSystem_800371D8 *fileSystem, UnkReadRequest_80045044 *request,
                            s32 queue) {
    if (fileSystem->m_exists(request->m_path)) {
        return fn_800454F8(request, queue);
    }
    return false;
}

/* Opens a RenderWare stream on the file name. */
extern "C" RwStream *fn_80044FEC(UnkFileSystem_800371D8 *fileSystem, const char *name) {
    char *path = fn_80051810(name);
    RwStream *stream = fn_80481368(2, 1, path);
    fn_80051830(path);
    return stream;
}

/* Lists the directory path (only the names containing filter, unless it is 0) into a newly
 * allocated array (*entries) and returns the number of entries. */
extern "C" s32 fn_80044EAC(UnkFileSystem_800371D8 *fileSystem, UnkDirEntry_80044EAC **entries,
                           const char *path, const char *filter) {
    UnkDVDFileInfo_80379C58 info;
    UnkDVDDir_8037A2E8 dir;
    UnkDVDDirEntry_8037A3A8 entry;
    UnkDirEntry_80044EAC *e;
    if (fn_8037A2E8(path, &dir) != 0) {
        UnkDirEntry_80044EAC *list =
            (UnkDirEntry_80044EAC *)fn_80049B90(lbl_805E3850 * sizeof(UnkDirEntry_80044EAC), 2);
        s32 count = 0;
        while (fn_8037A3A8(&dir, &entry) != 0) {
            if (filter != 0 && fn_803AC6C0(entry.m_name, filter) == 0) {
                continue;
            }
            e = &list[count];
            strcpy(e->m_name, entry.m_name);
            if (entry.m_isDir != 0) {
                e->m_type = 1;
                e->m_size = -1;
            } else {
                e->m_type = 2;
                if (fn_80379C58(e->m_name, &info) != 0) {
                    e->m_size = info.m_length;
                    fn_80379D20(&info);
                } else {
                    e->m_size = -1;
                }
            }
            count++;
        }
        u32 size = count * sizeof(UnkDirEntry_80044EAC);
        *entries = (UnkDirEntry_80044EAC *)fn_80049B90(size, 2);
        memcpy(*entries, list, size);
        fn_80049B34(list);
        return count;
    }
    *entries = 0;
    return 0;
}

/* Whether the file path exists (the file system's default check). */
extern "C" bool fn_80044E80(const char *path) {
    return (bool)fn_8047A67C(path);
}

/* The size of the file path (0 if it does not open). */
extern "C" u32 fn_80044E34(UnkFileSystem_800371D8 *fileSystem, const char *path) {
    UnkFile_8047A178 *file = fn_800513E0(path, "rb");
    if (file != 0) {
        u32 size = file->m_size;
        fn_8047A388(file);
        return size;
    }
    return 0;
}
