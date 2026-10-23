/* csdFileSystemRW.cpp (file name inferred, after the library's other RenderWare units,
 * csdTexListRW.cpp and CPlatformRW): the file system the game installs in the Chao CSD library
 * (see Game/CsdFileSystem.h). It reads whole files through the game's file functions, relative to a
 * root directory that stays empty. All names are inferred. Code generation follows
 * Game/csdPlatformRW.cpp (deferred inlining: functions come out in reverse definition order). */
#include <string.h>

#include <string>

#include <dolphin/types.h>

#include "Game/CsdFileSystem.h"
#include "Game/CsdLib.h"

#pragma pool_data off

/* An open file of the game's file functions (the part read here). */
struct UnkFile_8047A178 {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ u32 m_size;
};

extern "C" {
/* Opens a file, reads count items of size bytes, and closes it (fopen/fread/fclose-like; the
 * symbols are still placeholders). */
UnkFile_8047A178 *fn_8047A178(const char *path, const char *mode);
u32 fn_8047A3E0(void *buffer, u32 size, u32 count, UnkFile_8047A178 *file);
s32 fn_8047A388(UnkFile_8047A178 *file);
}

namespace Chao {
namespace CSD {

/* The directory file paths are relative to. */
#pragma explicit_zero_data on
static char s_root[0x100] = "";
#pragma explicit_zero_data reset

/* Returns the size of the file at path (zero if there is none). */
u32 CFileSystemRW::Load(const char *path) {
    char buffer[0x100];
    strcpy(buffer, s_root);
    strcat(buffer, path);
    UnkFile_8047A178 *file = fn_8047A178(buffer, "rb");
    if (file != 0) {
        u32 size = file->m_size;
        fn_8047A388(file);
        return size;
    }
    return 0;
}

/* Reads the file at path into out. */
bool CFileSystemRW::Load(void *out, const char *path) {
    u32 size = Load(path);
    char buffer[0x100];
    strcpy(buffer, s_root);
    strcat(buffer, path);
    UnkFile_8047A178 *file = fn_8047A178(buffer, "rb");
    if (file != 0) {
        fn_8047A3E0(out, size, 1, file);
        fn_8047A388(file);
        return true;
    }
    return false;
}

CFileSystemRW::~CFileSystemRW() {
}

} // namespace CSD
} // namespace Chao
