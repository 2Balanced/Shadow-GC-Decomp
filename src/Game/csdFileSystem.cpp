/* csdFileSystem.cpp (file name inferred; the library's records name no file here): the file system
 * the Chao CSD library loads through, which the game installs, and the directory file names are
 * relative to. All names are inferred.
 *
 * Code generation follows Game/csdLoader.cpp. MSL_STRING_COMPARE declares basic_string::compare and
 * the operator!= this unit emits (see <string>). */
#include <stdio.h>
#include <string.h>

#define MSL_STRING_DEFAULT_CTOR
#define MSL_STRING_COMPARE
#include <string>

#include <dolphin/types.h>

#include "Game/CsdFileSystem.h"
#include "Game/CsdLib.h"

#pragma pool_data off

extern "C" {
/* MSL strncpy (the symbol is still a placeholder). */
char *fn_803ACABC(char *dst, const char *src, u32 size);
}

namespace Chao {
namespace CSD {

#pragma pool_data on
static CFileSystem *s_fileSystem;
#pragma pool_data off

void GetDirectory(char *buffer, u32 size) {
    s_fileSystem->GetDirectory(buffer, size);
}

void SetDirectory(const char *path) {
    s_fileSystem->SetDirectory(path);
}

void CFileSystem::GetDirectory(char *buffer, u32 size) {
    fn_803ACABC(buffer, m_directory.c_str(), size);
}

void CFileSystem::SetDirectory(const char *path) {
    char buffer[0x200];
    strcpy(buffer, path);
    switch (buffer[strlen(buffer) - 1]) {
    case '\\':
    case '/':
        buffer[strlen(buffer) - 1] = 0;
        break;
    }
    m_directory = buffer;
}

bool CFileSystem::LoadFile(void *out, const char *name) {
    char path[0x200];
    MakePath(path, name);
    return Load(out, path);
}

u32 CFileSystem::LoadFile(const char *name) {
    char path[0x200];
    MakePath(path, name);
    return Load(path);
}

void CFileSystem::MakePath(char *out, const char *name) {
    if (m_directory != "") {
        const char *directory = m_directory.c_str();
        sprintf(out, "%s/%s", directory, name);
    } else {
        strcpy(out, name);
    }
}

bool LoadFile(void *out, const char *name) {
    return s_fileSystem->LoadFile(out, name);
}

u32 LoadFile(const char *name) {
    return s_fileSystem->LoadFile(name);
}

/* Installs fileSystem unless it is zero; returns the one installed before (or the current one). */
CFileSystem *SetFileSystem(CFileSystem *fileSystem) {
    if (fileSystem != 0) {
        CFileSystem *old = s_fileSystem;
        s_fileSystem = fileSystem;
        return old;
    }
    return s_fileSystem;
}

} // namespace CSD
} // namespace Chao
