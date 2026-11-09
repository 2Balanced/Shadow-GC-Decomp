/* ResourceOneFile (class name from RTTI, base Resource): a .one archive all of whose entries are
 * loaded, at once or (flag bit 0) in the background. The background read is registered with the
 * registry behind fn_80014B90 while it runs. The unit also holds the singletons that the loaders
 * hand texture dictionaries, clumps and .ADB files to (fn_80014534, fn_800144C8, fn_80013BC4), the
 * flag word helpers many units call (fn_80014E04 ...), and the game's copies of the std::string
 * members (stand-in in MSL_C++, MSL_STRING_IMPL). File name from the class; all other names are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#define MSL_STRING_DEFAULT_CTOR
#define MSL_STRING_CTOR_EXTERN
#define MSL_STRING_IMPL
#define RESOURCEONEFILE_FULL
#include <string>
#include <string.h>

#include "Game/ResourceOneFile.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* The opened archive (fn_8004CFB0; as in Game/ResourceOneFileReq.cpp, layout reduced to the table
 * of contents at 0x34, whose word at 0x10 is the number of entries). */
struct UnkOneFileToc_8004CFB0 {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ int m_count;
};

struct UnkOneFile_8004CFB0 {
    /* 0x00 */ u8 field_0x0[0x34];
    /* 0x34 */ UnkOneFileToc_8004CFB0 *m_toc;
    /* 0x38 */ u8 field_0x38[0x58 - 0x38];
};
SIZE_ASSERT(UnkOneFile_8004CFB0, 0x58);

/* An archive entry's description (fn_8004CA28): its name, and at 0x2C its size. */
struct UnkOneFileEntry_8004CA28 {
    /* 0x00 */ char m_name[0x2C];
    /* 0x2C */ u32 m_size;
};

/* The 0x28-byte object built over a .PTB/.CCL file (__ct__16UnkPath_8040E61CFPv); its data at 0x14. */
struct UnkPath_8040E61C {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ void *m_data;
    /* 0x18 */ u8 field_0x18[0x28 - 0x18];
};
SIZE_ASSERT(UnkPath_8040E61C, 0x28);

struct RpClump;
struct RwTexDictionary;

/* The singletons' objects (class names unknown; layouts reduced to their sizes): the registry
 * that background loads are entered in while they run (fn_80014B90), what texture dictionaries
 * are handed to (fn_80014534; no data), what loaded clumps are added to (fn_800144C8) and what
 * .ADB files are handed to (fn_80013BC4). */
struct UnkTaskRegistry_80014B90 {
    /* 0x0000 */ u32 field_0x0[0x1C08 / 4];
};

struct UnkTexDictManager_80014534 {};

struct UnkClumpManager_800144C8 {
    /* 0x00 */ u32 field_0x0[0x30 / 4];
};

struct UnkAdbManager_80013BC4 {
    /* 0x00 */ u32 field_0x0[0x10 / 4];
};

extern "C" {
/* Case-insensitive compare of n characters, and copy of at most n characters (behaviour
 * inferred). */
int fn_803AC8DC(const char *a, const char *b, u32 n);
char *fn_803ACABC(char *dst, const char *src, unsigned long size);
/* Frees memory (behaviour inferred). */
void fn_80049B34(void *data);
/* The archive (Game/ResourceOneFileReq.cpp): opening, whether it opened, closing, an asynchronous
 * read, an entry's description, and loading an entry raw or as a given RenderWare object
 * (behaviour inferred). */
UnkOneFile_8004CFB0 *fn_8004CFB0(UnkOneFile_8004CFB0 *file, const char *name, int arg2);
bool fn_8004CAF8(UnkOneFile_8004CFB0 *file);
void fn_8004CF68(UnkOneFile_8004CFB0 *file, int flags);
bool fn_8004CC6C(UnkOneFile_8004CFB0 *file, const char *name, void (*callback)(void *), void *data,
                 int arg4, int arg5);
UnkOneFileEntry_8004CA28 *fn_8004CA28(UnkOneFile_8004CFB0 *file, int index);
void *fn_8004C358(UnkOneFile_8004CFB0 *file, int index, void *arg2, void *arg3);
RwTexDictionary *fn_8004BF40(UnkOneFile_8004CFB0 *file, int index, void *arg2, void *arg3);
RpClump *fn_8004BB24(UnkOneFile_8004CFB0 *file, int index, void *arg2, void *arg3);
void *fn_8004B70C(UnkOneFile_8004CFB0 *file, int index, void *arg2, void *arg3);
/* The object built over a .PTB/.CCL file: construction and destruction (behaviour inferred). */
UnkPath_8040E61C *__ct__16UnkPath_8040E61CFPv(UnkPath_8040E61C *path, void *data);
void __dt__16UnkPath_8040E61CFv(UnkPath_8040E61C *path, int free);
/* RenderWare and the managers above: texture dictionaries, clumps, hierarchies, motions and UV
 * animations (behaviour inferred). */
RwTexDictionary *fn_8048D0C4(RwTexDictionary *dict);
void fn_80039618(UnkTexDictManager_80014534 *manager, RwTexDictionary *dict, int arg2);
void fn_802E0600(void *dict);
void fn_802E2A60(UnkClumpManager_800144C8 *manager, void *clump, int arg2);
void fn_8003815C(void *clump);
void *fn_8046256C(void *clump);
void fn_804623EC(void *world, void *clump);
void fn_80457468(void *clump);
void *fn_804109C4(void *data, void (*release)(void *));
void *fn_804108F0(void *data, void (*release)(void *));
void fn_804107EC(void *hierarchy);
u16 fn_80412BFC(void *data, const char *name, void (*release)(void *), int arg3);
u16 fn_80412B74(void *data, void (*release)(void *), int arg2);
void fn_804129D4(u16 motion);
void fn_804790CC(void *list, void *animation);
void fn_80479174(void *animation);
/* Prepares a .SNB file (Game/MotionFile.cpp). */
int fn_80010648(void *file);
/* The camera data of a file (Game/MotionTrack.cpp). */
void *fn_80011CE8(void *file, int arg1);
/* The .ADB files: adding and removing one (behaviour inferred). */
void *fn_8004FE88(UnkAdbManager_80013BC4 *manager, void *data);
void fn_8004FE00(UnkAdbManager_80013BC4 *manager, void *data);
/* Entering and removing a background load (behaviour inferred). */
void fn_8025D1C4(UnkTaskRegistry_80014B90 *registry, void *object, int arg2, const char *name);
void fn_8025D144(UnkTaskRegistry_80014B90 *registry, void *object);

UnkAdbManager_80013BC4 *fn_80013BC4(void);
UnkClumpManager_800144C8 *fn_800144C8(void);
UnkTexDictManager_80014534 *fn_80014534(void);
UnkTaskRegistry_80014B90 *fn_80014B90(void);

void *__register_global_object(void *object, void *destructor, void *record);
/* The singletons' constructors and destructors. */
UnkTaskRegistry_80014B90 *fn_8025D240(UnkTaskRegistry_80014B90 *registry);
UnkClumpManager_800144C8 *fn_802E2ADC(UnkClumpManager_800144C8 *manager);
void fn_802E2AA0(UnkClumpManager_800144C8 *manager, int flags);
/* The .ADB manager's constructor and destructor (Game/AdbManager.cpp). */
UnkAdbManager_80013BC4 *__ct__22UnkAdbManager_80013BC4Fv(UnkAdbManager_80013BC4 *manager);
void __dt__22UnkAdbManager_80013BC4Fv(UnkAdbManager_80013BC4 *manager, int flags);
}

/* The singletons are function-local statics in effect (guard, destructor record, object and the
 * pointer the accessor returns); they are written out so that .bss and .sbss keep retail's order,
 * which GC/2.7 does not give local statics. */
static u32 s_clumpManagerRecord[3];
static u32 s_adbManagerRecord[3];
static UnkTaskRegistry_80014B90 s_taskRegistry;
static UnkClumpManager_800144C8 s_clumpManager;
static UnkAdbManager_80013BC4 s_adbManager;
static s8 s_taskRegistryInitialized;
static UnkTexDictManager_80014534 s_texDictManager;
static s8 s_clumpManagerInitialized;
static s8 s_adbManagerInitialized;
static UnkClumpManager_800144C8 *s_clumpManagerInstance;
static UnkAdbManager_80013BC4 *s_adbManagerInstance;
static UnkTaskRegistry_80014B90 *s_taskRegistryInstance;
static UnkTexDictManager_80014534 *s_texDictManagerInstance;

/* The UV animation list (meaning inferred). */
extern u8 lbl_8056D748[];

/* Whether name has the extension ext (case-insensitive). */
inline bool HasExtension(const char *name, const char *ext) {
    const char *dot = strchr(name, '.');
    if (dot != 0 && fn_803AC8DC(dot, ext, 4) == 0) {
        return true;
    }
    return false;
}

extern "C" {

/* A 32-bit flag word (struct name unknown): whether bit is set, setting it, clearing all bits, and
 * the constructor. */
bool fn_80014E34(u32 *flags, int bit) {
    return (*flags & (1 << bit)) != 0;
}

void fn_80014E1C(u32 *flags, int bit) {
    *flags |= 1 << bit;
}

void fn_80014E10(u32 *flags) {
    *flags = 0;
}

void fn_80014E04(u32 *flags) {
    *flags = 0;
}

/* The release callbacks of the bone files and the motions. */
void fn_80014DE4(void *data) {
    fn_80049B34(data);
}

void fn_80014DC4(void *data) {
    fn_80049B34(data);
}

/* The callback of the background read: the file is read, the state becomes 2. */
void fn_80014DAC(void *data) {
    if (data != 0) {
        *static_cast<int **>(data)[1] = 2;
    }
}

}

ResourceOneFile::ResourceOneFile(const char *fileName, bool flag) {
    fn_80014E04(&m_flags);
    m_file = 0;
    m_stateRef = 0;
    m_fileName = fileName;
    m_state = 0;
    fn_80014E10(&m_flags);
    if (flag) {
        fn_80014E1C(&m_flags, 0);
    }
}

ResourceOneFile::~ResourceOneFile() {}

/* The registry of background loads. */
extern "C" UnkTaskRegistry_80014B90 *fn_80014B90(void) {
    if (s_taskRegistryInstance == 0) {
        if (!s_taskRegistryInitialized) {
            fn_8025D240(&s_taskRegistry);
            s_taskRegistryInitialized = 1;
        }
        s_taskRegistryInstance = &s_taskRegistry;
    }
    return s_taskRegistryInstance;
}

void ResourceOneFile::Load() {
    switch (m_state) {
    case 0:
        if (!fn_80014E34(&m_flags, 0)) {
            UnkOneFile_8004CFB0 *file = new UnkOneFile_8004CFB0;
            if (file != 0) {
                file = fn_8004CFB0(file, m_fileName.c_str(), 0);
            }
            if (file != 0) {
                LoadFiles(file);
                fn_8004CF68(file, 1);
            }
            m_state = 3;
        } else {
            UnkOneFile_8004CFB0 *file = new UnkOneFile_8004CFB0;
            if (file != 0) {
                file = fn_8004CFB0(file, 0, 0);
            }
            if (file != 0) {
                m_file = file;
                m_stateRef = &m_state;
                const char *name = m_fileName.c_str();
                if (fn_8004CC6C(file, name, fn_80014DAC, &m_file, 0, 0)) {
                    m_state = 1;
                    fn_8025D1C4(fn_80014B90(), this, 0, "ResourceOneFile");
                } else {
                    m_state = 4;
                }
                fn_8004CF68(file, 1);
            } else {
                m_state = 4;
            }
        }
        break;
    case 2:
        if (m_file != 0) {
            LoadFiles(m_file);
            fn_8004CF68(m_file, 1);
            m_file = 0;
            m_stateRef = 0;
            m_state = 3;
            fn_8025D144(fn_80014B90(), this);
        }
        break;
    case 3:
        break;
    }
}

bool ResourceOneFile::Release() {
    return Unload();
}

int ResourceOneFile::GetState() {
    return m_state;
}

void *ResourceOneFile::GetData(int index) {
    if (m_entries != 0) {
        if (m_entries[index - 2].m_type == 6) {
            return m_entries[index - 2].m_file->m_data;
        }
        if (m_entries[index - 2].m_type == 8) {
            return m_entries[index - 2].m_extra;
        }
        return m_entries[index - 2].m_data;
    }
    return 0;
}

void *ResourceOneFile::GetData(const char *name) {
    for (int i = m_entryCount - 1; i >= 0; i--) {
        if (strcmp(m_entries[i].m_name, name) == 0) {
            ResourceOneFileEntry *entry = &m_entries[i];
            if (entry->m_type == 6) {
                return entry->m_file->m_data;
            }
            if (entry->m_type == 8) {
                return entry->m_extra;
            }
            return entry->m_data;
        }
    }
    return 0;
}

int ResourceOneFile::GetIndex(const char *name) {
    for (int i = m_entryCount - 1; i >= 0; i--) {
        if (strcmp(m_entries[i].m_name, name) == 0) {
            return i + 2;
        }
    }
    return -1;
}

void *ResourceOneFile::GetHierarchy(int index) {
    int slot = index - 2;
    if (m_entries != 0 && m_entries[slot].m_type == 1) {
        return m_entries[slot].m_hierarchy;
    }
    return 0;
}

void *ResourceOneFile::GetHierarchy(const char *name) {
    for (int i = m_entryCount - 1; i >= 0; i--) {
        if (strcmp(m_entries[i].m_name, name) == 0) {
            return m_entries[i].m_hierarchy;
        }
    }
    return 0;
}

u32 ResourceOneFile::GetMotion(int index) {
    int slot = index - 2;
    if (m_entries != 0 && m_entries[slot].m_type == 2) {
        return m_entries[slot].m_motion;
    }
    return 0;
}

u32 ResourceOneFile::GetMotion(const char *name) {
    for (int i = m_entryCount - 1; i >= 0; i--) {
        if (strcmp(m_entries[i].m_name, name) == 0) {
            return m_entries[i].m_motion;
        }
    }
    return 0;
}

void *ResourceOneFile::vf_0x30(int index) {
    int slot = index - 2;
    if (m_entries != 0 && m_entries[slot].m_data != 0) {
        return fn_80011CE8(m_entries[slot].m_data, 2);
    }
    return 0;
}

void *ResourceOneFile::vf_0x34(const char *name) {
    for (int i = m_entryCount - 1; i >= 0; i--) {
        if (strcmp(m_entries[i].m_name, name) == 0) {
            void *data = m_entries[i].m_data;
            if (data != 0) {
                return fn_80011CE8(data, 2);
            }
        }
    }
    return 0;
}

u32 ResourceOneFile::GetSize(int index) {
    if (m_entries != 0) {
        return m_entries[index - 2].m_size;
    }
    return 0;
}

u32 ResourceOneFile::GetSize(const char *name) {
    for (int i = m_entryCount - 1; i >= 0; i--) {
        if (strcmp(m_entries[i].m_name, name) == 0) {
            return m_entries[i].m_size;
        }
    }
    return 0;
}

ResourceOneFileEntry::ResourceOneFileEntry() {
    strcpy(m_name, "");
    m_data = 0;
    m_extra = 0;
    m_file = 0;
    m_type = 0;
    m_hierarchy = 0;
    m_motion = 0;
    m_size = 0;
}

/* The manager of the texture dictionaries. */
extern "C" UnkTexDictManager_80014534 *fn_80014534(void) {
    if (s_texDictManagerInstance == 0) {
        s_texDictManagerInstance = &s_texDictManager;
    }
    return s_texDictManagerInstance;
}

/* The manager of the clumps. */
extern "C" UnkClumpManager_800144C8 *fn_800144C8(void) {
    if (s_clumpManagerInstance == 0) {
        if (!s_clumpManagerInitialized) {
            __register_global_object(fn_802E2ADC(&s_clumpManager), fn_802E2AA0,
                                     s_clumpManagerRecord);
            s_clumpManagerInitialized = 1;
        }
        s_clumpManagerInstance = &s_clumpManager;
    }
    return s_clumpManagerInstance;
}

/* Loads every entry by its extension; the texture dictionary first. */
bool ResourceOneFile::LoadFiles(UnkOneFile_8004CFB0 *file) {
    UnkOneFileToc_8004CFB0 *toc = file->m_toc;
    if (!fn_8004CAF8(file)) {
        return false;
    }
    m_entryCount = toc->m_count;
    if (m_entryCount <= 0) {
        return false;
    }
    m_entries = new ResourceOneFileEntry[m_entryCount];
    int index0;
    UnkOneFileEntry_8004CA28 *info0;
    int i;
    RwTexDictionary *dict;
    int index;
    UnkOneFileEntry_8004CA28 *info;
    int j;
    for (i = 0; i < m_entryCount; i++) {
        index0 = i + 2;
        info0 = fn_8004CA28(file, index0);
        if (info0 != 0 && HasExtension(info0->m_name, ".TXD")) {
            dict = fn_8004BF40(file, index0, 0, 0);
            if (dict != 0) {
                fn_8048D0C4(dict);
                fn_80039618(fn_80014534(), dict, 6);
                m_entries[i].m_data = dict;
                m_entries[i].m_type = 4;
                m_entries[i].m_size = info0->m_size;
                break;
            }
        }
    }

    for (j = 0; j < m_entryCount; j++) {
        index = j + 2;
        info = fn_8004CA28(file, index);
        if (info == 0) {
            continue;
        }
        memset(m_entries[j].m_name, 0, 0x20);
        fn_803ACABC(m_entries[j].m_name, info->m_name, 0x1F);
        if (HasExtension(info->m_name, ".DFF")) {
            m_entries[j].m_data = fn_8004BB24(file, index, 0, 0);
            m_entries[j].m_type = 3;
            m_entries[j].m_size = info->m_size;
            fn_802E2A60(fn_800144C8(), m_entries[j].m_data, 0);
            fn_8003815C(m_entries[j].m_data);
        } else if (HasExtension(info->m_name, ".BON")) {
            m_entries[j].m_data = fn_8004C358(file, index, 0, 0);
            m_entries[j].m_type = 1;
            m_entries[j].m_hierarchy = fn_804109C4(m_entries[j].m_data, fn_80014DE4);
            m_entries[j].m_size = info->m_size;
        } else if (HasExtension(info->m_name, ".BOP")) {
            m_entries[j].m_data = fn_8004C358(file, index, 0, 0);
            m_entries[j].m_type = 1;
            m_entries[j].m_hierarchy = fn_804108F0(m_entries[j].m_data, fn_80014DE4);
            m_entries[j].m_size = info->m_size;
        } else if (HasExtension(info->m_name, ".MTN")) {
            m_entries[j].m_data = fn_8004C358(file, index, 0, 0);
            m_entries[j].m_type = 2;
            m_entries[j].m_motion =
                fn_80412BFC(m_entries[j].m_data, m_entries[j].m_name, fn_80014DC4, 1);
            m_entries[j].m_size = info->m_size;
        } else if (HasExtension(info->m_name, ".MTP")) {
            m_entries[j].m_data = fn_8004C358(file, index, 0, 0);
            m_entries[j].m_type = 2;
            m_entries[j].m_motion = fn_80412B74(m_entries[j].m_data, fn_80014DC4, 1);
            m_entries[j].m_size = info->m_size;
        } else if (HasExtension(info->m_name, ".SPL")) {
        } else if (HasExtension(info->m_name, ".TXD")) {
        } else if (HasExtension(info->m_name, ".PTB") || HasExtension(info->m_name, ".CCL")) {
            m_entries[j].m_data = fn_8004C358(file, index, 0, 0);
            m_entries[j].m_type = 6;
            UnkPath_8040E61C *path = new UnkPath_8040E61C;
            if (path != 0) {
                path = __ct__16UnkPath_8040E61CFPv(path, m_entries[j].m_data);
            }
            m_entries[j].m_file = path;
            m_entries[j].m_size = info->m_size;
        } else if (HasExtension(info->m_name, ".UVA")) {
            void *animation = fn_8004B70C(file, index, 0, 0);
            fn_804790CC(lbl_8056D748, animation);
            m_entries[j].m_data = animation;
            m_entries[j].m_type = 5;
            m_entries[j].m_size = info->m_size;
        } else if (HasExtension(info->m_name, ".SNB")) {
            m_entries[j].m_data = fn_8004C358(file, index, 0, 0);
            m_entries[j].m_type = 7;
            fn_80010648(m_entries[j].m_data);
            m_entries[j].m_size = info->m_size;
        } else if (HasExtension(info->m_name, ".EFD")) {
            m_entries[j].m_data = fn_8004C358(file, index, 0, 0);
            m_entries[j].m_type = 9;
            m_entries[j].m_size = info->m_size;
        } else if (HasExtension(info->m_name, ".ADB")) {
            m_entries[j].m_data = fn_8004C358(file, index, 0, 0);
            m_entries[j].m_extra = fn_8004FE88(fn_80013BC4(), m_entries[j].m_data);
            m_entries[j].m_type = 8;
            m_entries[j].m_size = info->m_size;
        }
    }
    return true;
}

/* The manager of the .ADB files. */
extern "C" UnkAdbManager_80013BC4 *fn_80013BC4(void) {
    if (s_adbManagerInstance == 0) {
        if (!s_adbManagerInitialized) {
            __register_global_object(__ct__22UnkAdbManager_80013BC4Fv(&s_adbManager),
                                     __dt__22UnkAdbManager_80013BC4Fv, s_adbManagerRecord);
            s_adbManagerInitialized = 1;
        }
        s_adbManagerInstance = &s_adbManager;
    }
    return s_adbManagerInstance;
}

/* Frees the loaded entries; the texture dictionary last. */
bool ResourceOneFile::Unload() {
    if (m_entryCount <= 0) {
        return true;
    }
    for (int i = m_entryCount - 1; i >= 0; i--) {
        ResourceOneFileEntry &entry = m_entries[i];
        switch (entry.m_type) {
        case 1:
            fn_804107EC(entry.m_hierarchy);
            break;
        case 2:
            fn_804129D4(entry.m_motion);
            break;
        case 3: {
            void *world = fn_8046256C(entry.m_data);
            if (world != 0) {
                fn_804623EC(world, m_entries[i].m_data);
            }
            fn_80457468(m_entries[i].m_data);
            break;
        }
        case 4:
            continue;
        case 5:
            if (entry.m_data != 0) {
                fn_80479174(entry.m_data);
            }
            break;
        case 6:
            if (entry.m_file != 0) {
                __dt__16UnkPath_8040E61CFv(entry.m_file, 1);
            }
            fn_80049B34(m_entries[i].m_data);
            break;
        case 8:
            fn_8004FE00(fn_80013BC4(), entry.m_extra);
            break;
        case 7:
        case 9:
            if (entry.m_data != 0) {
                fn_80049B34(entry.m_data);
            }
            break;
        }
        m_entries[i].m_data = 0;
        m_entries[i].m_extra = 0;
        m_entries[i].m_file = 0;
        m_entries[i].m_type = 0;
        m_entries[i].m_size = 0;
    }
    for (int i = m_entryCount - 1; i >= 0; i--) {
        if (m_entries[i].m_type == 4) {
            fn_802E0600(m_entries[i].m_data);
            m_entries[i].m_data = 0;
            m_entries[i].m_file = 0;
            m_entries[i].m_type = 0;
            m_entries[i].m_size = 0;
        }
    }
    if (m_entries != 0) {
        delete[] m_entries;
        m_entries = 0;
    }
    return true;
}

/* The string terminator (std::__string_nul<char>, in MSL_C++) is defined when the unit ends; retail
 * has it in .sdata although it is zero. */
#pragma explicit_zero_data on
