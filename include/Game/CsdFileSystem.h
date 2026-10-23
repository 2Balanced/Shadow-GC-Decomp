#ifndef GAME_CSDFILESYSTEM_H
#define GAME_CSDFILESYSTEM_H

/* The file system the Chao CSD library loads through (Game/csdFileSystem.cpp), which the game
 * installs (Game/csdFileSystemRW.cpp is its implementation). It keeps the directory that file names
 * are relative to. Names are inferred. The includer includes <string> first. */

#include <dolphin/types.h>

#include "Game/CsdLib.h"

namespace Chao {
namespace CSD {

/* The empty directory a file system starts with (Game/csdRW.cpp, which constructs the file system):
 * retail passes an eight-byte block of zeros there, not a one-byte string. */
extern const char g_noDirectory[8];

class CFileSystem {
  public:
    /* (The empty inline call keeps the constructor out of line; see CPlatform in Game/CsdLib.h.) */
    CFileSystem() : m_directory(g_noDirectory) {
        __csd_nothing();
    }

    /* Returns the size of a file, and reads a file into out. */
    virtual u32 Load(const char *path) = 0;
    virtual bool Load(void *out, const char *path) = 0;
    virtual ~CFileSystem() {}

    void GetDirectory(char *buffer, u32 size);
    void SetDirectory(const char *path);
    bool LoadFile(void *out, const char *name);
    u32 LoadFile(const char *name);
    void MakePath(char *out, const char *name);

    /* 0x4 */ std::string m_directory;
};

/* The file system of the RenderWare build (Game/csdFileSystemRW.cpp; name inferred). */
class CFileSystemRW : public CFileSystem {
  public:
    CFileSystemRW() {}
    virtual u32 Load(const char *path);
    virtual bool Load(void *out, const char *path);
    virtual ~CFileSystemRW();
};

} // namespace CSD
} // namespace Chao

#endif
