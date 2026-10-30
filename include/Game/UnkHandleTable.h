#ifndef GAME_UNKHANDLETABLE_H
#define GAME_UNKHANDLETABLE_H

#include "Game/HandleUser.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* The handle table behind HandleUser (class and struct names unknown). fn_80045A00 returns the
 * table, fn_80045A64 gives a HandleUser a handle, fn_80045A98 finds the HandleUser of a handle and
 * fn_800459CC takes the handle back (Game/UnkHandles_800459CC.cpp); the table's members and a
 * pool of handle nodes are in Game/UnkHandleTable_80045AD0.cpp. */

/* A handle: a serial number and an index into the table (HandleUser's words at 0x4). */
struct UnkHandle_80045A98 {
#ifdef UNKHANDLE_DEFAULT_CTOR /* Game/LandManager.cpp; the game's copy is 0x8003B020 */
    UnkHandle_80045A98() : m_serial(0), m_index(-1) {}
#endif
#ifdef UNKHANDLE_DTOR /* Game/TBlockManage.cpp; the game's copy is 0x80021C44 */
    ~UnkHandle_80045A98() {}
#endif
    UnkHandle_80045A98(const UnkHandle_80045A98 &other) {
        m_serial = other.m_serial;
        m_index = other.m_index;
    }
    bool operator==(const UnkHandle_80045A98 &other) const {
        return m_index == other.m_index && m_serial == other.m_serial;
    }
    s16 GetIndex() const;

    /* 0x0 */ u16 m_serial;
    /* 0x2 */ s16 m_index;
};

/* One entry of the table. */
struct UnkHandleEntry_80045A00 {
    HandleUser *Get(UnkHandle_80045A98 handle);
    void Set(HandleUser *user);

    /* 0x0 */ UnkHandle_80045A98 m_handle;
    /* 0x4 */ HandleUser *m_user;
};

/* The table: 0x1000 entries. */
class UnkHandleTable_80045A00 {
  public:
    UnkHandleTable_80045A00();
    ~UnkHandleTable_80045A00();

    HandleUser *Get(const UnkHandle_80045A98 &handle);
    bool Release(UnkHandle_80045A98 &handle);
    bool Register(HandleUser *user);

    static UnkHandleTable_80045A00 *s_cached;

    /* 0x0 */ UnkHandleEntry_80045A00 *m_entries;
    /* 0x4 */ s32 m_next; /* where the search for a free entry starts */
};
SIZE_ASSERT(UnkHandleTable_80045A00, 0x8);

/* A node of the pool. */
struct UnkHandleNode_80045AD0 {
    /* 0x0 */ UnkHandle_80045A98 m_handle;
    /* 0x4 */ UnkHandleNode_80045AD0 *m_prev;
    /* 0x8 */ UnkHandleNode_80045AD0 *m_next;
    /* 0xC */ u32 field_0xC;
};

/* A pool of handle nodes (only created). */
class UnkHandlePool_80045AD0 {
  public:
    UnkHandlePool_80045AD0(s32 capacity);
    ~UnkHandlePool_80045AD0();

    static UnkHandlePool_80045AD0 *s_cached;

    /* 0x00 */ UnkHandleNode_80045AD0 *m_nodes;
    /* 0x04 */ UnkHandleNode_80045AD0 *m_free;
    /* 0x08 */ s32 m_capacity;
    /* 0x0C */ s32 m_used;
    /* 0x10 */ u32 field_0x10;
};
SIZE_ASSERT(UnkHandlePool_80045AD0, 0x14);

extern "C" {
UnkHandleTable_80045A00 *fn_80045A00(void);
UnkHandlePool_80045AD0 *fn_80045AD0(void);
}

#endif
