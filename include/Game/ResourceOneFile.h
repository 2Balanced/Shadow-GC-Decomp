#ifndef GAME_RESOURCEONEFILE_H
#define GAME_RESOURCEONEFILE_H

#include <dolphin/types.h>
#include "size_assert.h"

/* Resource and ResourceOneFile (class names from RTTI; vtables at 0x8051C66C and 0x8051C6BC): a
 * loadable resource, and one that is a single .one archive (Game/ResourceOneFile.cpp). The flag of
 * the constructor (0x80014C58) sets bit 0 of the flag word at 0x1C: load in the background. Units
 * that only construct one see the short declaration; Game/ResourceOneFile.cpp defines
 * RESOURCEONEFILE_FULL for the whole class. */
#if defined(RESOURCEONEFILE_FULL)
/* The whole class, for Game/ResourceOneFile.cpp, which defines it. The entries are those of the
 * .one archive named by m_fileName; Load reads them all, at once or (flag bit 0) in the background
 * through the archive's asynchronous read, whose callback sets the state to 2. */
#include <string>

#include "Game/Resource.h"

/* A loaded entry of the file (struct and member names inferred). m_type says what m_data is: 1 a
 * bone file (.BON, .BOP), 2 a motion (.MTN, .MTP), 3 a clump (.DFF), 4 the texture dictionary
 * (.TXD), 5 a UV animation (.UVA), 6 a .PTB/.CCL file, 7 a .SNB file, 8 an .ADB file, 9 an .EFD
 * file. */
struct ResourceOneFileEntry {
    ResourceOneFileEntry();

    /* 0x00 */ char m_name[0x20];
    /* 0x20 */ void *m_data;
    /* 0x24 */ void *m_extra;
    /* 0x28 */ struct UnkPath_8040E61C *m_file;
    /* 0x2C */ u32 m_type;
    /* 0x30 */ void *m_hierarchy;
    /* 0x34 */ u32 m_motion;
    /* 0x38 */ u32 m_size;
};
SIZE_ASSERT(ResourceOneFileEntry, 0x3C);

class ResourceOneFile : public Resource {
  public:
    ResourceOneFile(const char *fileName, bool flag);
    virtual ~ResourceOneFile();
    virtual void Load();
    virtual bool Release();
    virtual int GetState();
    virtual void *GetData(int index);
    virtual void *GetData(const char *name);
    virtual void *GetHierarchy(int index);
    virtual void *GetHierarchy(const char *name);
    virtual u32 GetMotion(int index);
    virtual u32 GetMotion(const char *name);
    virtual void *vf_0x30(int index);
    virtual void *vf_0x34(const char *name);
    virtual int GetIndex(const char *name);
    virtual u32 GetSize(int index);
    virtual u32 GetSize(const char *name);

    bool Unload();
    bool LoadFiles(struct UnkOneFile_8004CFB0 *file);

    /* 0x04 */ int m_state;
    /* 0x08 */ std::string m_fileName;
    /* 0x14 */ ResourceOneFileEntry *m_entries;
    /* 0x18 */ int m_entryCount;
    /* 0x1C */ u32 m_flags;
    /* 0x20 */ struct UnkOneFile_8004CFB0 *m_file;
    /* 0x24 */ int *m_stateRef;
};
#elif defined(RESOURCEONEFILE_POLYMORPHIC)
/* The class's view of its virtual functions, for units that call them (Game/EffectObjectPJS.cpp,
 * Game/EffectTask.cpp). The sixth one (0x800148E0) returns the data of the file named name inside
 * the resource (name inferred); the others are unknown. */
class ResourceOneFile {
  public:
    ResourceOneFile(const char *fileName, bool flag);
    virtual ~ResourceOneFile();
    virtual void vf0C();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void *GetFile(const char *name);

    /* 0x04 */ u8 field_0x4[0x28 - 0x4];
};
#else
class ResourceOneFile {
  public:
    ResourceOneFile(const char *fileName, bool flag);

    /* 0x00 */ u8 field_0x0[0x28];
};
#endif
SIZE_ASSERT(ResourceOneFile, 0x28);

extern "C" {
/* The resource loader, and adding and removing the resource of a slot (behaviour inferred). */
void *fn_800122D4(void);
void fn_8001299C(void *loader, int slot, ResourceOneFile *resource);
void fn_800127F8(void *loader, int slot);
}

#endif
