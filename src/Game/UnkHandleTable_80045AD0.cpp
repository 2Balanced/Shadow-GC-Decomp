/* The handle table's members and a pool of handle nodes (Game/UnkHandleTable.h). The pool is only
 * created (fn_80045AD0, from the system manager). The file name and the names here are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/UnkHandleTable.h"
#include <dolphin/types.h>

extern "C" {
/* The game's heap functions (allocate, allocate an array, free). */
void *fn_80049B90(u32 size, int heap);
void *fn_80049B60(u32 size, s32 count, int heap);
void fn_80049B34(void *block);
}

/* The pool's instance (inlined into fn_80045B04). */
inline UnkHandlePool_80045AD0 &UnkHandlePool_80045AD0_Instance() {
    static UnkHandlePool_80045AD0 s_pool(0x80);
    return s_pool;
}

UnkHandlePool_80045AD0 *UnkHandlePool_80045AD0::s_cached;

void UnkHandleEntry_80045A00::Set(HandleUser *user) {
    m_handle.m_serial++;
    user->field_0x4 = m_handle.m_serial;
    user->field_0x6 = m_handle.m_index;
    m_user = user;
}

UnkHandleTable_80045A00::UnkHandleTable_80045A00() {
    m_entries = (UnkHandleEntry_80045A00 *)fn_80049B90(0x8000, 2);
    for (s16 i = 0; i < 0x1000; i++) {
        UnkHandleEntry_80045A00 *entry = &m_entries[i];
        entry->m_handle.m_serial = 0;
        entry->m_handle.m_index = i;
        entry->m_user = 0;
    }
    m_next = 0;
}

UnkHandleTable_80045A00::~UnkHandleTable_80045A00() {
    if (m_entries != 0) {
        fn_80049B34(m_entries);
        m_entries = 0;
    }
}

bool UnkHandleTable_80045A00::Register(HandleUser *user) {
    s32 tries = 0;
    do {
        tries++;
        if (m_next >= 0x1000) {
            m_next -= 0x1000;
        }
        UnkHandleEntry_80045A00 *entry = &m_entries[m_next];
        if (entry->m_user == 0) {
            entry->Set(user);
            return true;
        }
        m_next++;
    } while (tries <= 0x1000);
    return false;
}

s16 UnkHandle_80045A98::GetIndex() const {
    return m_index;
}

bool UnkHandleTable_80045A00::Release(UnkHandle_80045A98 &handle) {
    if (handle.m_index != -1) {
        UnkHandleEntry_80045A00 *entry = &m_entries[handle.GetIndex()];
        handle.m_serial = 0;
        handle.m_index = -1;
        entry->m_user = 0;
        return true;
    }
    return false;
}

HandleUser *UnkHandleEntry_80045A00::Get(UnkHandle_80045A98 handle) {
    if (handle == m_handle) {
        return m_user;
    }
    return 0;
}

HandleUser *UnkHandleTable_80045A00::Get(const UnkHandle_80045A98 &handle) {
    if (handle.m_index != -1) {
        return m_entries[handle.m_index].Get(handle);
    }
    return 0;
}

UnkHandlePool_80045AD0::UnkHandlePool_80045AD0(s32 capacity) {
    if ((m_nodes = (UnkHandleNode_80045AD0 *)fn_80049B60(0x10, capacity, 2)) == 0) {
        for (;;) {
        }
    }
    m_capacity = capacity;
    m_used = 0;
    m_free = m_nodes;
    UnkHandleNode_80045AD0 *node = m_free;
    for (s32 i = 0; i < capacity; i++, node++) {
        node->m_handle.m_serial = 0;
        node->m_handle.m_index = -1;
        if (i == 0) {
            node->m_prev = 0;
        } else {
            node->m_prev = node - 1;
        }
        if (i == capacity - 1) {
            node->m_next = 0;
        } else {
            node->m_next = node + 1;
        }
    }
}

UnkHandlePool_80045AD0::~UnkHandlePool_80045AD0() {
    if (m_nodes != 0) {
        fn_80049B34(m_nodes);
        m_nodes = 0;
    }
}

UnkHandlePool_80045AD0 *fn_80045B04(void) {
    return &UnkHandlePool_80045AD0_Instance();
}

/* The handle pool. */
extern "C" UnkHandlePool_80045AD0 *fn_80045AD0(void) {
    if (UnkHandlePool_80045AD0::s_cached == 0) {
        UnkHandlePool_80045AD0::s_cached = fn_80045B04();
    }
    return UnkHandlePool_80045AD0::s_cached;
}
