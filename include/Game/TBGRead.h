#ifndef GAME_TBGREAD_H
#define GAME_TBGREAD_H

#include "Game/Task.h"
#include "Game/Thread.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* The game's file system (Game/UnkFileSystem_800371D8.cpp) and its background reads
 * (Game/TBGRead.cpp). Class names other than TBGReadManager, TBGRead and TFileControlAsycManager
 * (from RTTI) are unknown. */

/* An open file of the game's file functions (as in Game/FadeTask.cpp). */
struct UnkFile_8047A178 {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ u32 m_size;
};

/* The file system (the object fn_800371D8 returns; other units declare the part they use): the
 * task the read tasks hang below, and the check of whether a file exists. */
class UnkFileSystem_800371D8 {
  public:
    UnkFileSystem_800371D8();
    ~UnkFileSystem_800371D8();

    /* 0x0 */ Task *m_task;
    /* 0x4 */ bool (*m_exists)(const char *path);
};

/* A read request: the file, where its data goes, and the callback that receives the request once
 * the data is in. */
struct UnkReadRequest_80045044 {
    /* 0x00 */ const char *m_path;
    /* 0x04 */ void *m_data;
    /* 0x08 */ u32 m_size;
    /* 0x0C */ void *m_user;
    /* 0x10 */ void (*m_callback)(UnkReadRequest_80045044 *request);
    /* 0x14 */ u32 field_0x14;
};
SIZE_ASSERT(UnkReadRequest_80045044, 0x18);

class TBGReadManager : public Task {
  public:
    TBGReadManager();
    virtual ~TBGReadManager();
    virtual void Tick(float dt);

    /* Registers or unregisters the task as loading. */
    void SetLoading(bool loading);

    static const char *s_taskName;

    /* 0x28 */ bool m_loading;
    /* 0x2C */ u32 field_0x2C;
};
SIZE_ASSERT(TBGReadManager, 0x30);

extern "C" {
UnkFileSystem_800371D8 *fn_800371D8(void);
/* Reads file into *data (allocated if 0), chunk bytes at a time when chunk is not 0, and returns
 * the number of bytes read. */
u32 fn_80045128(UnkFileSystem_800371D8 *fileSystem, UnkFile_8047A178 *file, void **data, u32 size,
                s32 chunk);
}

/* Queues a copy of request in queue (0 to 2). */
bool fn_800454F8(UnkReadRequest_80045044 *request, s32 queue);

#endif
