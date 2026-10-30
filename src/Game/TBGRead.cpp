/* TBGReadManager, TBGRead and TFileControlAsycManager (class names from RTTI): the background reads
 * of the game's file system (Game/TBGRead.h). A request queued with fn_80045044 waits in one of
 * three queues; TBGReadManager starts a TBGRead thread for the first request once no other one
 * runs, the thread reads the file in chunks, then leaves a TFileControlAsycManager task that
 * closes the file and calls the request's callback on the main thread. Names other than the
 * classes' are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include "Game/TBGRead.h"
#include <dolphin/types.h>
#include "size_assert.h"
#include <string.h>

struct UnkTaskRegistry_80014B90;
struct UnkThreadQueue_8032CCFC;

extern "C" {
/* The registry of the tasks on screen (see Game/TExpandBG.cpp). */
UnkTaskRegistry_80014B90 *fn_80014B90(void);
bool fn_8025D1C4(UnkTaskRegistry_80014B90 *registry, void *task, int kind, const char *name);
bool fn_8025D144(UnkTaskRegistry_80014B90 *registry, void *task);
/* The disc error handler's queue, and OSSleepThread (see Game/TExpandBG.cpp). */
UnkThreadQueue_8032CCFC *fn_8032BAE0(void);
void fn_8037847C(UnkThreadQueue_8032CCFC *queue);
/* The game's heap functions (allocate, free). */
void *fn_80049B90(u32 size, int heap);
void fn_80049B34(void *block);
/* RenderWare's fclose, and an fopen that takes the request (behaviours inferred). */
s32 fn_8047A388(UnkFile_8047A178 *file);
UnkFile_8047A178 *fn_80479F50(const char *path, int arg1, int arg2, void *arg3);
/* OSReport. */
void fn_803A953C(const char *format, ...);
/* Called around the start of a read thread (empty in retail). */
void fn_803591B8(void);
void fn_803591B4(void *thread);
}

/* A queued request. */
struct UnkReadNode_800454F8 {
    /* 0x00 */ UnkReadNode_800454F8 *m_next;
    /* 0x04 */ UnkReadRequest_80045044 m_request;
    /* 0x1C */ char m_path[0x40];
    /* 0x5C */ s32 m_queue;
};
SIZE_ASSERT(UnkReadNode_800454F8, 0x60);

class TBGRead : public SonicteamUSA::System::Thread {
  public:
    TBGRead(UnkReadRequest_80045044 *request, UnkFile_8047A178 *file);
    virtual ~TBGRead();
    virtual bool Run();

    /* Leaves the task that finishes the request. */
    void Notify();

    /* The number of read threads running. */
    static s32 s_count;

    /* 0x32C */ u32 field_0x32C;
    /* 0x330 */ UnkReadRequest_80045044 m_request;
    /* 0x348 */ UnkFile_8047A178 *m_file;
    /* 0x34C */ char m_path[0x44];
};
SIZE_ASSERT(TBGRead, 0x390);

class TFileControlAsycManager : public Task {
  public:
    TFileControlAsycManager(TBGRead *owner, Task *parent, UnkReadRequest_80045044 *request,
                            UnkFile_8047A178 *file);
    virtual ~TFileControlAsycManager();
    virtual void Tick(float dt);

    static void Start(TBGRead *owner, UnkReadRequest_80045044 *request, UnkFile_8047A178 *file);

    static const char *s_taskName;

    /* 0x28 */ UnkReadRequest_80045044 m_request;
    /* 0x40 */ UnkFile_8047A178 *m_file;
    /* 0x44 */ TBGRead *m_owner;
};
SIZE_ASSERT(TFileControlAsycManager, 0x48);

const char *TFileControlAsycManager::s_taskName = "TFileControlAsycManager";
const char *TBGReadManager::s_taskName = "TBGReadManager";

s32 TBGRead::s_count;
/* Whether reading is paused. */
static u8 s_paused;
/* The request queues. */
static UnkReadNode_800454F8 *s_queues[3];

TFileControlAsycManager::~TFileControlAsycManager() {}

TFileControlAsycManager::TFileControlAsycManager(TBGRead *owner, Task *parent,
                                                 UnkReadRequest_80045044 *request,
                                                 UnkFile_8047A178 *file)
    : Task(parent), m_owner(0) {
    m_name = s_taskName;
    m_flags |= 0x100;
    m_request = *request;
    m_file = file;
    m_owner = owner;
    m_flags |= 0x8000;
}

void TFileControlAsycManager::Start(TBGRead *owner, UnkReadRequest_80045044 *request,
                                    UnkFile_8047A178 *file) {
    new TFileControlAsycManager(owner, fn_800371D8()->m_task, request, file);
}

void TFileControlAsycManager::Tick(float dt) {
    if (m_flags & 0x8000) {
        if (m_owner != 0) {
            m_owner = 0;
        }
        if (m_file != 0) {
            fn_8047A388(m_file);
            if (m_request.m_callback != 0) {
                m_request.m_callback(&m_request);
            }
            m_file = 0;
        }
        m_flags |= 1;
    }
}

TBGRead::TBGRead(UnkReadRequest_80045044 *request, UnkFile_8047A178 *file) : Thread(0) {
    strcpy(m_path, request->m_path);
    m_request = *request;
    m_file = file;
    s_count++;
}

TBGRead::~TBGRead() {
    s_count--;
}

void TBGRead::Notify() {
    TFileControlAsycManager::Start(this, &m_request, m_file);
}

bool TBGRead::Run() {
    UnkThreadQueue_8032CCFC *queue = fn_8032BAE0();
    if (queue != 0) {
        fn_8037847C(queue);
    }
    UnkFile_8047A178 *file = m_file;
    if (file != 0) {
        m_request.m_size =
            fn_80045128(fn_800371D8(), file, &m_request.m_data, m_request.m_size, 0x2000);
    }
    Notify();
    return false;
}

TBGReadManager::TBGReadManager() : Task(fn_800371D8()->m_task) {
    m_loading = false;
    m_name = s_taskName;
    m_flags |= 0x100;
    memset(s_queues, 0, sizeof(s_queues));
    s_paused = 0;
}

bool fn_800454F8(UnkReadRequest_80045044 *request, s32 queue) {
    UnkReadNode_800454F8 *node = (UnkReadNode_800454F8 *)fn_80049B90(0x60, 2);
    node->m_queue = queue;
    node->m_next = 0;
    UnkReadNode_800454F8 *last = s_queues[queue];
    if (last != 0) {
        while (last->m_next != 0) {
            last = last->m_next;
        }
        last->m_next = node;
    } else {
        s_queues[queue] = node;
    }
    if (node == 0) {
        return false;
    }
    node->m_request = *request;
    strcpy(node->m_path, request->m_path);
    node->m_request.m_path = node->m_path;
    node->m_queue = queue;
    return true;
}

void TBGReadManager::Tick(float dt) {
    if (TBGRead::s_count != 0 || s_paused != 0) {
        return;
    }
    s32 queue = 0;
    for (s32 i = 0; i < 3; i++) {
        if (s_queues[i] != 0) {
            break;
        }
        queue++;
    }
    if (queue >= 3) {
        SetLoading(false);
        return;
    }
    SetLoading(true);
    UnkFile_8047A178 *file;
    /* The unsigned index keeps GC/2.7 from sharing the queue's address with the removal below,
     * which retail computes afresh. */
    UnkReadRequest_80045044 *request = &s_queues[(u32)queue]->m_request;
    file = fn_80479F50(request->m_path, 1, 0, request);
    if (file != 0) {
        if (request->m_data == 0) {
            request->m_size = file->m_size;
            request->m_size = (request->m_size + 0x1F) & ~0x1F;
            request->m_data = fn_80049B90(request->m_size, 2);
        }
        fn_803591B8();
        fn_803591B4(new TBGRead(request, file));
    }
    UnkReadNode_800454F8 *node = s_queues[queue];
    if (node != 0) {
        UnkReadNode_800454F8 *next = node->m_next;
        fn_80049B34(node);
        s_queues[queue] = next;
    }
}

void TBGReadManager::SetLoading(bool loading) {
    if (m_loading != loading) {
        if (loading) {
            fn_8025D1C4(fn_80014B90(), this, 1, "File Loading");
        } else {
            fn_8025D144(fn_80014B90(), this);
        }
        m_loading = loading;
    }
}

/* Pauses (1) or resumes (0) the reads (the messages: "background read stopped" and "resumed"). */
extern "C" void fn_800452F8(u8 paused) {
    s_paused = paused;
    if (paused == 1) {
        fn_803A953C("\x97\xA0\x93\xC7\x82\xDD\x92\xE2\x8E\x7E\n");
    } else {
        fn_803A953C("\x97\xA0\x93\xC7\x82\xDD\x8D\xC4\x8A\x4A\n");
    }
}

TBGReadManager::~TBGReadManager() {}
