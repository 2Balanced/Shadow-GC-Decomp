#ifndef GAME_FOGTABLE_H
#define GAME_FOGTABLE_H

#include <dolphin/types.h>
#include "size_assert.h"

/* The stage's fog table, stgNNNN/stgNNNN.fog (Game/FogTable.cpp loads it; TFogSet in
 * Game/TFogSet.cpp uses it). All names are inferred. */

/* One fog area of the table. */
struct FogArea {
    /* 0x00 */ u32 m_blocks[4]; /* the blocks the area covers, one bit each */
    /* 0x10 */ float m_start;
    /* 0x14 */ float m_end;
    /* 0x18 */ s32 m_type; /* 1: the area also sets density and colour */
    /* 0x1C */ float m_density;
    /* 0x20 */ float m_far; /* set to m_end when below 1 */
    /* 0x24 */ u8 m_red;
    /* 0x25 */ u8 m_green;
    /* 0x26 */ u8 m_blue;
};
SIZE_ASSERT(FogArea, 0x28);

/* The table has this many areas. */
static const int kFogAreaCount = 24;

/* What the file system's load request takes (layout from Game/FogTable.cpp). The callback gets the
 * request back with the data filled in. */
struct FogRequest {
    /* 0x00 */ const char *m_path;
    /* 0x04 */ void *m_data;
    /* 0x08 */ u32 m_size;
    /* 0x0C */ void *m_user;
    /* 0x10 */ void (*m_callback)(FogRequest *request);
};

/* The loaded table and its loading state. A single static instance exists (Get). */
class FogTable {
  public:
    enum State {
        STATE_NONE = 0,
        STATE_LOADING = 1,
        STATE_LOADED = 2,
    };

    FogTable();
    /* The instance, constructed on first use (defined in Game/TFogSet.cpp). */
    static FogTable *Get();
    /* Starts loading the stage's table; false while a load is already running. */
    bool Load();
    void Release();
    void MakeFileName();
    /* Called by the file system when the table is loaded: swaps it to the game's byte order. */
    static void OnLoaded(FogRequest *request);

    /* The instance once Get made it (defined in Game/FogTable.cpp). */
    static FogTable *s_instance;

    /* 0x00 */ s32 m_state;
    /* 0x04 */ FogArea *m_areas;
    /* 0x08 */ u8 m_disabled;
    /* 0x09 */ char m_fileName[0x27];
};
SIZE_ASSERT(FogTable, 0x30);

#endif
