#ifndef GAME_CHARACTERFILES_H
#define GAME_CHARACTERFILES_H

#include <dolphin/types.h>
#include "boost/scoped_ptr.hpp"
#include "size_assert.h"

/* The character model files (character/<name>.one) loaded on demand, with a reference count per
 * file (class, member and method names inferred; the game has no RTTI for them). The game keeps
 * one CharacterFileManager in a function-local static (fn_80078C88). Defined in
 * Game/CharacterFiles.cpp. */
class CharacterFile {
  public:
    CharacterFile();
    ~CharacterFile();

    /* Load and unload the file, counting references (0x8027ADC0 and 0x8027AD6C). */
    void Load();
    void Unload();
    /* Take and drop the one reference the character tasks hold (0x8027AD2C and 0x8027ACEC). */
    void Acquire();
    void Release();

    /* 0x0 */ s32 m_index; /* into the file name table */
    /* 0x4 */ s32 m_handle; /* the loader slot, -1 when not loaded */
    /* 0x8 */ s32 m_refCount;
    /* 0xC */ bool m_acquired;
};
SIZE_ASSERT(CharacterFile, 0x10);

class CharacterFileTable {
  public:
    CharacterFileTable();

    /* Written through a pointer: retail calls the smart pointer's operator-> out of line before
     * it (see Coaster.cpp). */
    void GetHandle(int index, s32 *handle) {
        *handle = m_files[index].m_handle;
    }

    /* 0x000 */ CharacterFile m_files[20];
};
SIZE_ASSERT(CharacterFileTable, 0x140);

class CharacterFileManager {
  public:
    CharacterFileManager();
    ~CharacterFileManager();

    void ReleaseAll();
    void Release(int index);
    void Acquire(int index);
    void Unload(int index);
    void Load(int index);
    /* Whether the file of index has finished loading (loader state 3). */
    bool IsLoaded(int index);

    /* The loader slot of the file of index. */
    s32 GetHandle(int index) {
        s32 handle;
        m_table->GetHandle(index, &handle);
        return handle;
    }

    /* 0x0 */ boost::scoped_ptr<CharacterFileTable> m_table;
};
SIZE_ASSERT(CharacterFileManager, 0x4);

extern "C" {
/* The game's CharacterFileManager. */
CharacterFileManager *fn_80078C88(void);
/* The file index of character type type (0x8027A994). */
s32 fn_8027A994(int type);
}

#endif
