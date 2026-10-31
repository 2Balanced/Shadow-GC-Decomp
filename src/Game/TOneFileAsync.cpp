/* The .one archives: opening a file whole or in the background, looking up its entries and reading
 * one (expanding it in a TExpandBG thread when compressed), and TOneFileAsync<T> (class name from
 * RTTI), the task that reads an entry into memory and streams it in as a RenderWare object of
 * type T. While reads run they are listed in the TOneFileAsyncManager (name from its task registry
 * string), at most 16 at a time; TOneFileNotice (class name from RTTI) reports the size of an
 * entry read at once to a callback on the next frame. The archive's class name is unknown (other
 * units declare it as UnkOneFile_8004CFB0 and call its functions by these fn_ names). Names other
 * than the classes' are inferred. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). The manager is written as a class template: its members
 * then come out with the template instances at the end of the unit, as in retail. fn_8004CA98
 * advances its `data` parameter with an integer addition ((u8 *)((u32)data + 12)), which is what
 * keeps the sum in the parameter's register; every pointer-arithmetic form tried used a scratch
 * register for it. */
#define MSL_ALGORITHM_COPY
#define MSL_ALGORITHM_OUTLINE
#define MSL_VECTOR_MIDDLE
#define MSL_VECTOR_BACK
#define MSL_VECTOR_ERASE_ONE
#define MSL_VECTOR_ERASE_ONE_DATA
#include <vector>
#include <typeinfo>
#include <string.h>

#include "Game/Task.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* RenderWare. */
struct RwStream;
struct RwTexDictionary;
struct RpClump;
struct RtDict;
struct RtDictSchema;
struct RpWorld;
struct RpDMorphAnimation;
struct RwMemory {
    void *start;
    u32 length;
};
struct RwChunkHeaderInfo {
    u32 type;
    u32 length;
    u32 version;
    u32 buildNum;
    int isComplex;
};

struct UnkTaskRegistry_80014B90;
struct UnkFileSystem_800371D8;
struct UnkReadRequest_80045044;

/* Called with the size read and the caller's argument. */
typedef void (*OneFileReadCallback)(u32 size, void *arg);

/* The background expander (Game/TExpandBG.cpp; only its constructor and size are needed here). */
class TExpandBG {
  public:
    TExpandBG(const u8 *src, u8 *dst, OneFileReadCallback callback, void *arg);

    u8 field_0x0[0x348];
};

extern "C" {
/* The game's heap functions: set and reset the alignment, allocate, free (behaviour inferred). */
void fn_80049B14(u32 align);
void fn_80049AF4(void);
void *fn_80049B90(u32 size, int heap);
void fn_80049B34(void *block);
/* Byte-swaps n bytes in place (Game/TExpandBG.cpp). */
void fn_80043D0C(void *p, int n);
/* Expands compressed data at once and returns the expanded size (Game/TExpandBG.cpp). */
u32 fn_80043FB8(const u8 *src, u8 *dst);
/* Called around the creation of a thread (Game/Thread.cpp). */
void fn_803591B8(void);
void fn_803591B4(TExpandBG *thread);
/* The file system, its opening of a file as a stream, and its background reads. */
UnkFileSystem_800371D8 *fn_800371D8(void);
RwStream *fn_80044FEC(UnkFileSystem_800371D8 *fileSystem, const char *name);
bool fn_80045044(UnkFileSystem_800371D8 *fileSystem, UnkReadRequest_80045044 *request, int queue);
/* The registry of the tasks on screen (see Game/StageEventList.cpp). */
UnkTaskRegistry_80014B90 *fn_80014B90(void);
bool fn_8025D1C4(UnkTaskRegistry_80014B90 *registry, void *task, int kind, const char *name);
bool fn_8025D144(UnkTaskRegistry_80014B90 *registry, void *task);
/* sscanf, and a substring search. */
int fn_803AB5B8(const char *s, const char *format, ...);
char *fn_803AC6C0(const char *s, const char *sub);
/* RenderWare: streams, texture dictionaries and the stream readers of each type. */
RwStream *fn_80481368(int type, int access, void *data);
int fn_8048124C(RwStream *stream, void *data);
int fn_8047CF50(RwStream *stream, u32 type, u32 *length, u32 *version);
int fn_8047D94C(RwStream *stream, RwChunkHeaderInfo *header);
u32 fn_80480D4C(RwStream *stream, void *buffer, u32 length);
RwTexDictionary *fn_8048D0D8(void);
RwTexDictionary *fn_8048D0C4(RwTexDictionary *dict);
RpDMorphAnimation *fn_80442C24(RwStream *stream);
RpWorld *fn_804556F4(RwStream *stream);
RpClump *fn_80457698(RwStream *stream);
RwTexDictionary *fn_80483FBC(RwStream *stream);
void fn_802E0658(RwTexDictionary *dict);
RtDict *fn_80479284(RtDictSchema *schema, RwStream *stream);
}
extern RtDictSchema lbl_8056D748;

/* The heap an archive allocates its data from when it has one (an interface; the first and fifth
 * slots allocate and free). */
class UnkOneFileHeap_8004CFB0 {
  public:
    virtual void *Alloc(u32 size);
    virtual void Virtual0C();
    virtual void Virtual10();
    virtual void Virtual14();
    virtual void Free(void *p);
};

/* An entry of the table of contents: its name, size, data (an offset in the file until the file
 * is parsed) and flags (bit 0: compressed). Files older than version 0.? keep the last three at
 * 0x20. */
struct UnkOneFileEntry_8004CA28 {
    /* 0x00 */ char m_name[0x20];
    /* 0x20 */ u32 m_oldSize;
    /* 0x24 */ u32 m_oldData;
    /* 0x28 */ u32 m_oldFlags;
    /* 0x2C */ u32 m_size;
    /* 0x30 */ u8 *m_data;
    /* 0x34 */ u32 m_flags;
};
SIZE_ASSERT(UnkOneFileEntry_8004CA28, 0x38);

/* The table of contents: the version string, then (from index 2) the entries. */
struct UnkOneFileToc_8004CFB0 {
    /* 0x00 */ char m_version[0x10];
    /* 0x10 */ s32 m_count;
    /* 0x14 */ u8 field_0x14[0x34 - 0x14];
};

class UnkOneFile_8004CFB0 {
  public:
    bool IsValid() const {
        if (m_data) {
            return true;
        }
        return m_name[0] != 0;
    }

    void AddPending() {
        m_pending++;
    }

    /* 0x00 */ char m_name[0x2C];
    /* 0x2C */ u8 *m_data; /* the data when it belongs to the caller */
    /* 0x30 */ u32 m_size;
    /* 0x34 */ UnkOneFileToc_8004CFB0 *m_toc;
    /* 0x38 */ u8 *m_ownData; /* the data when the archive allocated it */
    /* 0x3C */ u32 m_ownSize;
    /* 0x40 */ UnkOneFileHeap_8004CFB0 *m_heap;
    /* 0x44 */ bool m_loading;
    /* 0x48 */ void (*m_callback)(void *arg);
    /* 0x4C */ void *m_callbackArg;
    /* 0x50 */ bool field_0x50;
    /* 0x54 */ s32 m_pending; /* the reads with a callback still running */
};
SIZE_ASSERT(UnkOneFile_8004CFB0, 0x58);

/* A read request of the file system (as in Game/TBGRead.h). */
struct UnkReadRequest_80045044 {
    /* 0x00 */ const char *m_path;
    /* 0x04 */ void *m_data;
    /* 0x08 */ u32 m_size;
    /* 0x0C */ void *m_user;
    /* 0x10 */ void (*m_callback)(UnkReadRequest_80045044 *request);
    /* 0x14 */ u32 field_0x14;
};

extern "C" {
UnkOneFile_8004CFB0 *fn_8004CFB0(UnkOneFile_8004CFB0 *file, const char *name, UnkOneFileHeap_8004CFB0 *heap);
UnkOneFile_8004CFB0 *fn_8004CF68(UnkOneFile_8004CFB0 *file, s16 free);
bool fn_8004CDF4(UnkOneFile_8004CFB0 *file, const char *name);
void fn_8004CD10(UnkOneFile_8004CFB0 *file);
bool fn_8004CC6C(UnkOneFile_8004CFB0 *file, const char *name, void (*callback)(void *), void *arg,
                 void *data, u32 size);
void fn_8004CC00(UnkReadRequest_80045044 *request);
bool fn_8004CB20(UnkOneFile_8004CFB0 *file);
bool fn_8004CAF8(const UnkOneFile_8004CFB0 *file);
bool fn_8004CA98(UnkOneFile_8004CFB0 *file, u8 *data, u32 size);
UnkOneFileEntry_8004CA28 *fn_8004CA28(const UnkOneFile_8004CFB0 *file, int index);
int fn_8004C910(const UnkOneFile_8004CFB0 *file, const char *name);
int fn_8004C850(const UnkOneFile_8004CFB0 *file, const char *name, int start);
u32 fn_8004C748(const UnkOneFile_8004CFB0 *file, int index, u8 *dst, OneFileReadCallback callback, void *arg);
void *fn_8004C358(UnkOneFile_8004CFB0 *file, int index, void (*callback)(void *, void *), void *arg);
RwTexDictionary *fn_8004BF40(UnkOneFile_8004CFB0 *file, int index,
                             void (*callback)(RwTexDictionary *, void *), void *arg);
RpClump *fn_8004BB24(UnkOneFile_8004CFB0 *file, int index, void (*callback)(RpClump *, void *), void *arg);
RtDict *fn_8004B70C(UnkOneFile_8004CFB0 *file, int index, void (*callback)(RtDict *, void *), void *arg);
RpWorld *fn_8004B2F0(UnkOneFile_8004CFB0 *file, int index, void (*callback)(RpWorld *, void *), void *arg);
RpDMorphAnimation *fn_8004ADB0(UnkOneFile_8004CFB0 *file, int index,
                               void (*callback)(RpDMorphAnimation *, void *), void *arg);
void fn_8004AD5C(OneFileReadCallback callback, u32 size, void *arg);
}

/* The task the reads hang below (set by Game/GameSystem.cpp), and the number of reads with a
 * callback still running. */
Task *lbl_805EE190;
s32 lbl_805EE194;

template <class T> class TOneFileAsyncManagerT;
typedef TOneFileAsyncManagerT<Task> TOneFileAsyncManager;

extern "C" {
TOneFileAsyncManager *fn_8004B13C(void);
bool fn_8004D4D4(TOneFileAsyncManager *manager);
}

/* The reads running in the background. */
template <class T> class TOneFileAsyncManagerT {
  public:
    TOneFileAsyncManagerT();

    int Count() {
        return m_tasks.size();
    }

    bool Add(T *task, UnkOneFile_8004CFB0 *file, int index);
    bool Remove(T *task);

    static TOneFileAsyncManagerT *s_cached;
    static bool s_busy;
    /* The name of the tasks listed. */
    static const char *s_taskName;

    /* 0x0 */ std::vector<T *> m_tasks;
};

inline TOneFileAsyncManager &TOneFileAsyncManager_Instance() {
    static TOneFileAsyncManager s_manager;
    return s_manager;
}

template <> TOneFileAsyncManager *TOneFileAsyncManager::s_cached;
template <> bool TOneFileAsyncManager::s_busy;

/* The manager's members that other units call by these names. Defined as friends of a class
 * template, so that GC/2.7 treats them as the inline functions they stand for: kept out of line and
 * emitted after their first caller. */
template <class T> struct UnkManagerCopies_8004B13C {
    friend TOneFileAsyncManager *fn_8004B13C(void) {
        if (TOneFileAsyncManager::s_cached == 0) {
            TOneFileAsyncManager::s_cached = &TOneFileAsyncManager_Instance();
        }
        return TOneFileAsyncManager::s_cached;
    }
    friend bool fn_8004D4D4(T *manager) {
        return manager->m_tasks.empty();
    }
};
template struct UnkManagerCopies_8004B13C<TOneFileAsyncManager>;

template <class T> const char *TOneFileAsyncManagerT<T>::s_taskName = "TOneFileAsync";

template <class T> TOneFileAsyncManagerT<T>::TOneFileAsyncManagerT() {
    m_tasks.reserve(0x80);
    m_tasks.clear();
}

template <class T> bool TOneFileAsyncManagerT<T>::Add(T *task, UnkOneFile_8004CFB0 *file, int index) {
    s_busy = true;
    if (fn_8004D4D4(this)) {
        fn_8025D1C4(fn_80014B90(), this, 1, "TOneFileAsyncManager");
    }
    for (T **it = m_tasks.begin(); it != m_tasks.end(); it++) {
        if (*it == task) {
            s_busy = false;
            return false;
        }
    }
    T *entry = task;
    m_tasks.push_back(entry);
    s_busy = false;
    return true;
}

template <class T> bool TOneFileAsyncManagerT<T>::Remove(T *task) {
    s_busy = true;
    for (T **it = m_tasks.begin(); it != m_tasks.end(); it++) {
        if (*it == task) {
            m_tasks.erase(it);
            if (fn_8004D4D4(this)) {
                fn_8025D144(fn_80014B90(), this);
            }
            s_busy = false;
            return true;
        }
    }
    s_busy = false;
    return false;
}

/* Reads an entry of file into memory and, once read, streams it in as a T. */
template <class T> class TOneFileAsync : public Task {
  public:
    typedef void (*Callback)(T *result, void *arg);

    TOneFileAsync(Task *parent, UnkOneFile_8004CFB0 *file, int index, Callback callback, void *arg)
        : Task(parent) {
        m_name = TOneFileAsyncManager::s_taskName;
        m_callback = callback;
        m_arg = arg;
        m_state = 0;
        m_index = index;
        m_result = 0;
        m_file = file;
        m_texDict = fn_8048D0D8();
        m_flags |= 0x100;
        if (m_callback) {
            m_file->AddPending();
            lbl_805EE194++;
        }
    }

    virtual ~TOneFileAsync() {}
    virtual void Tick(float dt) {
        Poll(0);
    }

    static void OnRead(u32 size, void *arg) {
        TOneFileAsync *task = (TOneFileAsync *)arg;
        task->m_memory.length = size;
        task->m_reading = false;
    }

    /* Streams the result in (one specialization per type). */
    void Read();

    /* Advances the read; true once done (the result is then in *result). */
    bool Poll(T **result) {
        switch (m_state) {
        case 0:
            if (fn_8004B13C()->Count() >= 16) {
                return false;
            }
            m_buffer = (u8 *)fn_80049B90((fn_8004CA28(m_file, m_index)->m_size + 0x401F) & ~0x1F, 2);
            if (m_buffer) {
                fn_80049B34(m_buffer);
                m_buffer = (u8 *)fn_80049B90((fn_8004CA28(m_file, m_index)->m_size + 0x1F) & ~0x1F, 2);
            }
            if (!m_buffer) {
                m_buffer = (u8 *)fn_80049B90((fn_8004CA28(m_file, m_index)->m_size + 0x1F) & ~0x1F, 1);
            }
            fn_8004B13C()->Add(this, m_file, m_index);
            if (m_callback) {
                fn_8004C748(m_file, m_index, m_buffer, OnRead, this);
                m_reading = true;
            } else {
                m_memory.length = fn_8004C748(m_file, m_index, m_buffer, 0, 0);
                m_reading = false;
            }
            m_state = 1;
            return false;
        case 1:
            if (!m_reading) {
                if (m_memory.length == 0) {
                    fn_80049B34(m_buffer);
                    m_state = 4;
                    return false;
                }
                m_state = 2;
                fn_8004B13C()->Remove(this);
            }
            return false;
        case 2:
            m_memory.start = m_buffer;
            m_stream = fn_80481368(3, 1, &m_memory);
            Read();
            m_state = 4;
            break;
        case 3:
            if (m_abort) {
                m_state = 4;
            }
            break;
        case 4:
            if (typeid(T *) != typeid(void *)) {
                if (m_buffer) {
                    fn_80049B34(m_buffer);
                    m_buffer = 0;
                }
            }
            if (m_stream) {
                fn_8048124C(m_stream, 0);
            }
            if (result) {
                *result = m_result;
            }
            if (m_callback) {
                m_callback(m_result, m_arg);
                m_file->m_pending--;
                lbl_805EE194--;
            }
            m_flags |= 1;
            return true;
        }
        return false;
    }

    /* 0x28 */ s32 m_state;
    /* 0x2C */ Callback m_callback;
    /* 0x30 */ void *m_arg;
    /* 0x34 */ int m_index;
    /* 0x38 */ u8 *m_buffer;
    /* 0x3C */ T *m_result;
    /* 0x40 */ UnkOneFile_8004CFB0 *m_file;
    /* 0x44 */ RwMemory m_memory;
    /* 0x4C */ RwStream *m_stream;
    /* 0x50 */ bool m_reading;
    /* 0x54 */ RwTexDictionary *m_texDict;
    /* 0x58 */ bool m_abort;
    /* 0x5C */ u32 field_0x5C;
};

template <> inline void TOneFileAsync<void>::Read() {
    m_result = m_buffer;
}

template <> inline void TOneFileAsync<RwTexDictionary>::Read() {
    if (fn_8047CF50(m_stream, 0x16, 0, 0)) {
        m_result = fn_80483FBC(m_stream);
        fn_802E0658(m_result);
    }
}

template <> inline void TOneFileAsync<RpClump>::Read() {
    RwTexDictionary *dict = m_texDict;
    if (fn_8047CF50(m_stream, 0x10, 0, 0)) {
        fn_8048D0C4(dict);
        m_result = fn_80457698(m_stream);
    }
}

template <> inline void TOneFileAsync<RtDict>::Read() {
    if (fn_8047CF50(m_stream, 0x2B, 0, 0)) {
        m_result = fn_80479284(&lbl_8056D748, m_stream);
    }
}

template <> inline void TOneFileAsync<RpWorld>::Read() {
    RwTexDictionary *dict = m_texDict;
    if (fn_8047CF50(m_stream, 0xB, 0, 0)) {
        fn_8048D0C4(dict);
        m_result = fn_804556F4(m_stream);
    }
}

template <> inline void TOneFileAsync<RpDMorphAnimation>::Read() {
    if (fn_8047CF50(m_stream, 0x1E, 0, 0)) {
        m_result = fn_80442C24(m_stream);
    }
}

/* Reports the size of an entry read at once to a callback on the next frame. */
class TOneFileNotice : public Task {
  public:
    TOneFileNotice(Task *parent, OneFileReadCallback callback, u32 size, void *arg);
    virtual ~TOneFileNotice();
    virtual void Tick(float dt);

    /* 0x28 */ OneFileReadCallback m_callback;
    /* 0x2C */ u32 m_size;
    /* 0x30 */ void *m_arg;
    /* 0x34 */ u32 field_0x34;
};
SIZE_ASSERT(TOneFileNotice, 0x38);

UnkOneFile_8004CFB0 *fn_8004CFB0(UnkOneFile_8004CFB0 *file, const char *name, UnkOneFileHeap_8004CFB0 *heap) {
    file->m_pending = 0;
    file->field_0x50 = false;
    file->m_heap = heap;
    file->m_name[0] = 0;
    memset(&file->m_data, 0, 8);
    memset(&file->m_ownData, 0, 8);
    file->m_toc = 0;
    file->m_loading = false;
    if (name) {
        fn_8004CDF4(file, name);
    }
    return file;
}

UnkOneFile_8004CFB0 *fn_8004CF68(UnkOneFile_8004CFB0 *file, s16 free) {
    if (file) {
        fn_8004CB20(file);
        if (free > 0) {
            delete file;
        }
    }
    return file;
}

bool fn_8004CDF4(UnkOneFile_8004CFB0 *file, const char *name) {
    if (file->m_loading) {
        return false;
    }
    if (name == 0) {
        if (file->IsValid()) {
            return true;
        }
        fn_8004CB20(file);
        return false;
    }
    if (strcmp(file->m_name, name) == 0) {
        return true;
    }
    fn_8004CB20(file);
    RwStream *stream = fn_80044FEC(fn_800371D8(), name);
    if (!stream) {
        return false;
    }
    RwChunkHeaderInfo header;
    fn_8047D94C(stream, &header);
    u8 *data = (u8 *)header.length;
    fn_80049B14(0x40);
    if (file->m_heap) {
        data = (u8 *)file->m_heap->Alloc((u32)data);
        fn_80049AF4();
    } else {
        data = (u8 *)fn_80049B90((u32)data, 2);
        fn_80049AF4();
    }
    if (!data) {
        return false;
    }
    header.length = fn_80480D4C(stream, data, header.length);
    fn_8048124C(stream, 0);
    strcpy(file->m_name, name);
    file->m_ownData = data;
    file->m_ownSize = header.length;
    file->m_toc = (UnkOneFileToc_8004CFB0 *)file->m_ownData;
    fn_8004CD10(file);
    return true;
}

void fn_8004CD10(UnkOneFile_8004CFB0 *file) {
    float version;
    fn_803AB5B8(file->m_toc->m_version, "One Ver %f", &version);
    fn_80043D0C(&file->m_toc->m_count, 4);
    for (int i = 0; i < file->m_toc->m_count + 2; i++) {
        UnkOneFileEntry_8004CA28 *entry = fn_8004CA28(file, i);
        if (entry) {
            if (version <= 0.59f) {
                entry->m_flags = entry->m_oldFlags;
                entry->m_data = (u8 *)entry->m_oldData;
                entry->m_size = entry->m_oldSize;
            }
            fn_80043D0C(&entry->m_size, 4);
            fn_80043D0C(&entry->m_data, 4);
            fn_80043D0C(&entry->m_flags, 4);
            entry->m_data = (u8 *)file->m_toc + (u32)entry->m_data;
        }
    }
}

bool fn_8004CC6C(UnkOneFile_8004CFB0 *file, const char *name, void (*callback)(void *), void *arg,
                 void *data, u32 size) {
    if (file->m_loading) {
        return false;
    }
    fn_8004CB20(file);
    UnkReadRequest_80045044 request;
    request.m_path = name;
    request.m_data = data;
    request.m_size = size;
    request.m_callback = fn_8004CC00;
    request.m_user = file;
    file->m_callback = callback;
    file->m_callbackArg = arg;
    if (fn_80045044(fn_800371D8(), &request, 1)) {
        file->m_loading = true;
        return true;
    }
    return false;
}

void fn_8004CC00(UnkReadRequest_80045044 *request) {
    UnkOneFile_8004CFB0 *file = (UnkOneFile_8004CFB0 *)request->m_user;
    if (request->m_data) {
        fn_8004CA98(file, (u8 *)request->m_data, request->m_size);
        if (file->m_callback) {
            file->m_callback(file->m_callbackArg);
        }
    } else {
        fn_8004CD10(file);
    }
    file->m_loading = false;
}

bool fn_8004CB20(UnkOneFile_8004CFB0 *file) {
    if (file->IsValid()) {
        if (file->m_data) {
            if (file->m_heap) {
                file->m_heap->Free(file->m_data);
            } else {
                fn_80049B34(file->m_data);
            }
            memset(&file->m_data, 0, 8);
        } else {
            UnkOneFileHeap_8004CFB0 *heap = file->m_heap;
            u8 *data = file->m_ownData;
            if (heap) {
                heap->Free(data);
            } else {
                fn_80049B34(data);
            }
        }
        file->m_toc = 0;
        file->m_ownData = 0;
        file->m_name[0] = 0;
        return true;
    }
    return false;
}

bool fn_8004CAF8(const UnkOneFile_8004CFB0 *file) {
    return file->IsValid();
}

bool fn_8004CA98(UnkOneFile_8004CFB0 *file, u8 *data, u32 size) {
    fn_8004CB20(file);
    file->m_data = data;
    file->m_size = size;
    /* The +12 is written as an integer addition: that is what makes GC/2.7 advance the parameter's
     * own register (addi r30,r30,12) instead of keeping the sum in a scratch one. */
    data = (u8 *)((u32)data + 12);
    file->m_ownData = data;
    file->m_ownSize = size - 12;
    file->m_toc = (UnkOneFileToc_8004CFB0 *)file->m_ownData;
    fn_8004CD10(file);
    return true;
}

UnkOneFileEntry_8004CA28 *fn_8004CA28(const UnkOneFile_8004CFB0 *file, int index) {
    if (!file->IsValid()) {
        return 0;
    }
    if (index < 2) {
        return 0;
    }
    if (index >= file->m_toc->m_count + 2) {
        return 0;
    }
    return (UnkOneFileEntry_8004CA28 *)((u8 *)file->m_toc + index * 0x38 + 0x34);
}

int fn_8004C910(const UnkOneFile_8004CFB0 *file, const char *name) {
    if (!file->IsValid()) {
        return -1;
    }
    if (name == 0) {
        return -1;
    }
    char upper[0x2C];
    for (int i = 0; i < 0x2C; i++) {
        if (name[i] >= 'a' && name[i] <= 'z') {
            upper[i] = name[i] - 0x20;
        } else {
            upper[i] = name[i];
        }
        if (upper[i] == 0) {
            break;
        }
    }
    UnkOneFileToc_8004CFB0 *toc = file->m_toc;
    for (int index = 2; index < toc->m_count + 2; index++) {
        UnkOneFileEntry_8004CA28 *entry = fn_8004CA28(file, index);
        if (!entry) {
            break;
        }
        if (strcmp(entry->m_name, upper) == 0) {
            return index;
        }
    }
    return -1;
}

int fn_8004C850(const UnkOneFile_8004CFB0 *file, const char *name, int start) {
    if (!file->IsValid()) {
        return -1;
    }
    if (name == 0) {
        return -1;
    }
    UnkOneFileToc_8004CFB0 *toc = file->m_toc;
    for (int index = start; index < toc->m_count + 2; index++) {
        UnkOneFileEntry_8004CA28 *entry = fn_8004CA28(file, index);
        if (!entry) {
            break;
        }
        if (fn_803AC6C0(entry->m_name, name)) {
            return index;
        }
    }
    return -1;
}

u32 fn_8004C748(const UnkOneFile_8004CFB0 *file, int index, u8 *dst, OneFileReadCallback callback, void *arg) {
    u32 size = 0;
    if (!file->IsValid()) {
        return 0;
    }
    UnkOneFileEntry_8004CA28 *entry = fn_8004CA28(file, index);
    if (entry) {
        if (entry->m_flags & 1) {
            if (callback) {
                fn_803591B8();
                fn_803591B4(new TExpandBG(entry->m_data, dst, callback, arg));
            } else {
                size = fn_80043FB8(entry->m_data, dst);
            }
        } else {
            memcpy(dst, entry->m_data, entry->m_size);
            if (callback) {
                fn_8004AD5C(callback, entry->m_size, arg);
            }
            return entry->m_size;
        }
    }
    return size;
}

void *fn_8004C358(UnkOneFile_8004CFB0 *file, int index, void (*callback)(void *, void *), void *arg) {
    TOneFileAsync<void> *task = new TOneFileAsync<void>(lbl_805EE190, file, index, callback, arg);
    if (callback) {
        return 0;
    }
    void *result;
    while (!task->Poll(&result)) {
    }
    return result;
}

RwTexDictionary *fn_8004BF40(UnkOneFile_8004CFB0 *file, int index,
                             void (*callback)(RwTexDictionary *, void *), void *arg) {
    TOneFileAsync<RwTexDictionary> *task =
        new TOneFileAsync<RwTexDictionary>(lbl_805EE190, file, index, callback, arg);
    if (callback) {
        return 0;
    }
    RwTexDictionary *result;
    while (!task->Poll(&result)) {
    }
    return result;
}

RpClump *fn_8004BB24(UnkOneFile_8004CFB0 *file, int index, void (*callback)(RpClump *, void *), void *arg) {
    TOneFileAsync<RpClump> *task = new TOneFileAsync<RpClump>(lbl_805EE190, file, index, callback, arg);
    if (callback) {
        return 0;
    }
    RpClump *result;
    while (!task->Poll(&result)) {
    }
    return result;
}

RtDict *fn_8004B70C(UnkOneFile_8004CFB0 *file, int index, void (*callback)(RtDict *, void *), void *arg) {
    TOneFileAsync<RtDict> *task = new TOneFileAsync<RtDict>(lbl_805EE190, file, index, callback, arg);
    if (callback) {
        return 0;
    }
    RtDict *result;
    while (!task->Poll(&result)) {
    }
    return result;
}

RpWorld *fn_8004B2F0(UnkOneFile_8004CFB0 *file, int index, void (*callback)(RpWorld *, void *), void *arg) {
    TOneFileAsync<RpWorld> *task = new TOneFileAsync<RpWorld>(lbl_805EE190, file, index, callback, arg);
    if (callback) {
        return 0;
    }
    RpWorld *result;
    while (!task->Poll(&result)) {
    }
    return result;
}

RpDMorphAnimation *fn_8004ADB0(UnkOneFile_8004CFB0 *file, int index,
                               void (*callback)(RpDMorphAnimation *, void *), void *arg) {
    TOneFileAsync<RpDMorphAnimation> *task =
        new TOneFileAsync<RpDMorphAnimation>(lbl_805EE190, file, index, callback, arg);
    if (callback) {
        return 0;
    }
    RpDMorphAnimation *result;
    while (!task->Poll(&result)) {
    }
    return result;
}

void fn_8004AD5C(OneFileReadCallback callback, u32 size, void *arg) {
    new TOneFileNotice(lbl_805EE190, callback, size, arg);
}

TOneFileNotice::TOneFileNotice(Task *parent, OneFileReadCallback callback, u32 size, void *arg)
    : Task(parent) {
    m_callback = callback;
    m_size = size;
    m_arg = arg;
    m_name = "TOneFileNoice";
    m_flags |= 0x100;
}

void TOneFileNotice::Tick(float dt) {
    m_callback(m_size, m_arg);
    m_flags |= 1;
}

TOneFileNotice::~TOneFileNotice() {}
