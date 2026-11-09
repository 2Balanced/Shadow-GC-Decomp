/* ResourceOneFileReq (class name from RTTI, base Resource): a .one archive whose entries are loaded
 * on request. The global request list (0x80575EE4, other units call it lbl_80575EE4) collects which
 * entries the next archive should load, by index or by name (fn_8006B474, fn_8006B50C); an archive
 * copies the list when it is constructed and loads those entries by file type when Load is called.
 * A .BIN entry also requests the files its model data names. Texture dictionaries (.TXD) load
 * first; afterwards the textures no clump or target ("_tgt") uses are destroyed. File name from the
 * class; all other names are inferred. Other units call the request functions and the callbacks by
 * their addresses, so they keep those names. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#define MSL_ALGORITHM_FIND
#define MSL_STRING_DEFAULT_CTOR
#define MSL_VECTOR_DESTROY_ALL_LATE
#define MSL_VECTOR_DESTROY_ALL_FIRST
#define MSL_VECTOR_INDEX
#define MSL_VECTOR_MIDDLE
#define RESOURCEONEFILEREQ_FULL
#include <algorithm>
#include <string>
#include <vector>
#include <string.h>

#include "Game/CsdRCPtr.h"
#include "Game/ResourceOneFileReq.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* The opened archive (fn_8004CFB0; name as in Game/PjsEventActionScene.cpp, layout reduced to the
 * table of contents at 0x34, whose word at 0x10 is the number of entries). */
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

/* The model data of a .BIN entry (meaning inferred): four tables whose records name the archive's
 * files, the second one with records that are resolved against the first table's names. */
struct UnkBinNode_8040E61C {
    /* 0x00 */ const char *m_name;
    /* 0x04 */ const char *m_file2;
    /* 0x08 */ const char *m_file0;
    /* 0x0C */ const char *m_file1;
    /* 0x10 */ u8 field_0x10[0x40 - 0x10];
};

struct UnkBinLink_8040E61C {
    /* 0x0 */ const char *m_name;
    /* 0x4 */ int m_node;
};

struct UnkBinGroup_8040E61C {
    /* 0x0 */ u8 field_0x0[4];
    /* 0x4 */ UnkBinLink_8040E61C *m_links;
    /* 0x8 */ u8 field_0x8[4];
    /* 0xC */ int m_linkCount;
};

struct UnkBinPart_8040E61C {
    /* 0x00 */ u8 field_0x0[8];
    /* 0x08 */ const char *m_file0;
    /* 0x0C */ u8 field_0xC[4];
    /* 0x10 */ const char *m_file1;
    /* 0x14 */ u8 field_0x14[0x1C - 0x14];
};

struct UnkBinRef_8040E61C {
    /* 0x00 */ u8 field_0x0[4];
    /* 0x04 */ const char *m_file;
    /* 0x08 */ u8 field_0x8[0x18 - 0x8];
};

struct UnkBinData_8040E61C {
    /* 0x00 */ UnkBinNode_8040E61C *m_nodes;
    /* 0x04 */ int m_nodeCount;
    /* 0x08 */ UnkBinGroup_8040E61C *m_groups;
    /* 0x0C */ int m_groupCount;
    /* 0x10 */ UnkBinPart_8040E61C *m_parts;
    /* 0x14 */ int m_partCount;
    /* 0x18 */ UnkBinRef_8040E61C *m_refs;
    /* 0x1C */ int m_refCount;
};

/* The 0x28-byte object built over a .BIN or .PTB/.CCL file (__ct__16UnkPath_8040E61CFPv; name as in
 * Game/AudioDataBank.cpp); its data at 0x14. */
struct UnkPath_8040E61C {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ UnkBinData_8040E61C *m_data;
    /* 0x18 */ u8 field_0x18[0x28 - 0x18];
};
SIZE_ASSERT(UnkPath_8040E61C, 0x28);

/* RenderWare objects (only the parts used here). */
struct RwTexture {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ char name[0x20];
};
struct RpMaterial {
    /* 0x0 */ RwTexture *texture;
};
struct RpGeometry {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ RpMaterial **materials;
    /* 0x24 */ u32 numMaterials;
};
struct RpAtomic {
    /* 0x00 */ u8 field_0x0[0x18];
    /* 0x18 */ RpGeometry *geometry;
};
struct RpClump;
struct RwTexDictionary;

namespace Chao {
namespace CSD {
/* The loaded CSD project (Chao::CSD::CProject, Game/csdmProject.cpp; declared as in
 * Game/AdvCsdCache.cpp). */
class CProject {
  public:
    CProject();
    virtual ~CProject();

    /* 0x04 */ u8 field_0x4[0x4C - 0x4];

    static void *operator new(u32 size) {
        return fn_803605AC()->Alloc(size, "csdBase.h", 40);
    }
};
} // namespace CSD
} // namespace Chao

using Chao::CSD::CProject;

extern "C" {
/* String helpers: case-insensitive compare of n characters, copy of at most n characters, and
 * strchr and strstr (behaviour inferred). */
int fn_803AC8DC(const char *a, const char *b, u32 n);
char *fn_803ACABC(char *dst, const char *src, unsigned long size);
char *fn_803AC864(const char *s, int c);
char *fn_803AC6C0(const char *s, const char *sub);
/* Frees memory (behaviour inferred). */
void fn_80049B34(void *data);
/* Allocates size bytes from a heap (behaviour inferred). */
void *fn_80049B90(u32 size, int heap);
/* The archive (Game/PjsEventActionScene.cpp): opening, whether it opened, closing, an entry's
 * index by name and its description, and loading an entry raw or as a given RenderWare object
 * (behaviour inferred). */
UnkOneFile_8004CFB0 *fn_8004CFB0(UnkOneFile_8004CFB0 *file, const char *name, int arg2);
bool fn_8004CAF8(UnkOneFile_8004CFB0 *file);
void fn_8004CF68(UnkOneFile_8004CFB0 *file, int flags);
u32 fn_8004C910(UnkOneFile_8004CFB0 *file, const char *name);
UnkOneFileEntry_8004CA28 *fn_8004CA28(UnkOneFile_8004CFB0 *file, int index);
void fn_8004C748(UnkOneFile_8004CFB0 *file, int index, void *buffer, void *arg3, void *arg4);
void *fn_8004C358(UnkOneFile_8004CFB0 *file, int index, void *arg2, void *arg3);
RwTexDictionary *fn_8004BF40(UnkOneFile_8004CFB0 *file, int index, void *arg2, void *arg3);
RpClump *fn_8004BB24(UnkOneFile_8004CFB0 *file, int index, void *arg2, void *arg3);
void *fn_8004B70C(UnkOneFile_8004CFB0 *file, int index, void *arg2, void *arg3);
void *fn_8004ADB0(UnkOneFile_8004CFB0 *file, int index, void *arg2, void *arg3);
/* The object built over a .BIN or .PTB/.CCL file: construction, destruction, and a named part
 * (behaviour inferred). */
UnkPath_8040E61C *__ct__16UnkPath_8040E61CFPv(UnkPath_8040E61C *path, void *data);
void __dt__16UnkPath_8040E61CFv(UnkPath_8040E61C *path, int free);
void *fn_8040E4BC(UnkPath_8040E61C *path, const char *name);
/* RenderWare: texture dictionaries and textures, clumps, the world, hierarchies, motions, UV
 * animations and materials (behaviour inferred). */
RwTexDictionary *fn_8048D0C4(RwTexDictionary *dict);
void fn_80038B58(RwTexDictionary *dict, int arg1);
RwTexDictionary *fn_8048CBE0(RwTexDictionary *dict, RwTexture *(*callback)(RwTexture *, void *), void *data);
void fn_802E0600(void *dict);
void fn_802E05CC(RwTexture *texture);
RpClump *fn_80456C54(RpClump *clump, RpAtomic *(*callback)(RpAtomic *, void *), void *data);
void *fn_800144C8(void);
void fn_802E2A60(void *world, void *clump, int arg2);
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
void fn_80442B98(void *data);
int fn_804A2064(RpAtomic *atomic);
int fn_804A22E4(RpMaterial *material);
RwTexture *fn_804A28D0(RpMaterial *material);
RwTexture *fn_804A2674(RpMaterial *material);
RwTexture *fn_804A2B44(RpMaterial *material);
/* The resources of the .ADB files (behaviour inferred). */
void *fn_80013BC4(void);
void *fn_8004FE88(void *resources, void *data);
void fn_8004FE00(void *resources, void *data);
/* Loads a CSD project from a file image (Game/csdmProject.cpp). */
int fn_8036DB70(CProject *project, const void *file, u32 size, const char *directory);
}

/* A list of textures that no clump has claimed yet (names inferred): the head's m_prev is the last
 * node and its m_next the first. */
struct UnkTextureNode {
    /* 0x0 */ RwTexture *m_texture;
    /* 0x4 */ UnkTextureNode *m_prev;
    /* 0x8 */ UnkTextureNode *m_next;
};

inline void RemoveTexture(UnkTextureNode *list, RwTexture *texture) {
    for (UnkTextureNode *node = list->m_next; node != 0;) {
        UnkTextureNode *cur = node;
        node = node->m_next;
        if (texture == cur->m_texture) {
            cur->m_prev->m_next = node;
            if (cur->m_next != 0) {
                cur->m_next->m_prev = cur->m_prev;
            } else {
                list->m_prev = cur->m_prev;
            }
            delete cur;
            break;
        }
    }
}

/* The archive index of a request. */
inline int GetRequestIndex(UnkOneFile_8004CFB0 *file, const ResourceRequest &request) {
    return (request.m_flags & 1) ? fn_8004C910(file, request.m_name) : request.m_index;
}

inline UnkTextureNode *NewTextureList() {
    UnkTextureNode *list = new UnkTextureNode;
    list->m_texture = 0;
    list->m_prev = list;
    list->m_next = 0;
    return list;
}

/* Destroys the textures left in the list, and the list. */
inline void DestroyTextureList(UnkTextureNode *list) {
    for (UnkTextureNode *node = list->m_next; node != 0;) {
        UnkTextureNode *cur = node;
        fn_802E05CC(node->m_texture);
        node = node->m_next;
        delete cur;
    }
    delete list;
}

/* Whether name has the extension ext (case-insensitive). */
inline bool HasExtension(const char *name, const char *ext) {
    const char *dot = strchr(name, '.');
    if (dot != 0 && fn_803AC8DC(dot, ext, 4) == 0) {
        return true;
    }
    return false;
}

/* The UV animation list (meaning inferred). */
extern u8 lbl_8056D748[];

/* The global request list (name unknown; other units refer to it by this address name). */
std::vector<ResourceRequest> lbl_80575EE4;

extern "C" {

/* The texture callback of the dictionaries: textures that are no target textures ("_tgt") and
 * have no extension are added to the list of unclaimed textures. */
RwTexture *fn_8006B7C8(RwTexture *texture, void *data) {
    UnkTextureNode *list = static_cast<UnkTextureNode *>(data);
    const char *name = texture->name;
    if (fn_803AC864(name, '.') == 0 && fn_803AC6C0(name, "_tgt") == 0) {
        UnkTextureNode *node = new UnkTextureNode;
        node->m_texture = texture;
        node->m_prev = list->m_prev;
        node->m_next = 0;
        list->m_prev = node;
        node->m_prev->m_next = node;
    }
    return texture;
}

/* The atomic callback of the clumps: the textures of the atomic's materials are claimed. */
RpAtomic *fn_8006B638(RpAtomic *atomic, void *data) {
    UnkTextureNode *list = static_cast<UnkTextureNode *>(data);
    RpGeometry *geometry = atomic->geometry;
    u32 count = geometry->numMaterials;
    for (u32 i = 0; i < count; i++) {
        RwTexture *texture;
        RpMaterial *material = geometry->materials[i];
        if (material->texture != 0) {
            RemoveTexture(list, material->texture);
        }
        if (fn_804A2064(atomic) != 0) {
            texture = 0;
            switch (fn_804A22E4(material)) {
            case 2:
                texture = fn_804A28D0(material);
                break;
            case 1:
                texture = fn_804A2674(material);
                break;
            case 4:
            case 6:
                texture = fn_804A2B44(material);
                break;
            }
            if (texture != 0) {
                RemoveTexture(list, texture);
            }
        }
    }
    return atomic;
}

/* The release callbacks of the bone files and the motions. */
void fn_8006B618(void *data) {
    fn_80049B34(data);
}

void fn_8006B5F8(void *data) {
    fn_80049B34(data);
}

/* Empties the request list. */
void fn_8006B5A0(void) {
    lbl_80575EE4.reserve(0x200);
    lbl_80575EE4.clear();
}

/* Requests entry index of the next archive. */
void fn_8006B50C(int index, int type) {
    ResourceRequest request(index, type);
    ResourceRequest *it = std::find(lbl_80575EE4.begin(), lbl_80575EE4.end(), request);
    if (it == lbl_80575EE4.end()) {
        lbl_80575EE4.push_back(request);
    }
}

/* Requests the entry of the next archive with the given name. */
void fn_8006B474(const char *name, int type) {
    ResourceRequest request(name, type);
    ResourceRequest *it = std::find(lbl_80575EE4.begin(), lbl_80575EE4.end(), request);
    if (it == lbl_80575EE4.end()) {
        lbl_80575EE4.push_back(request);
    }
}

}

ResourceOneFileReq::ResourceOneFileReq(const char *fileName, void *list) {
    m_fileName = fileName;
    m_state = 0;
    m_entries = 0;
    m_entryCount = 0;
    int count = static_cast<std::vector<ResourceRequest> *>(list)->size();
    m_requests.reserve(0x200);
    for (int i = 0; i < count; i++) {
        m_requests.push_back((*static_cast<std::vector<ResourceRequest> *>(list))[i]);
    }
}

ResourceOneFileReq::~ResourceOneFileReq() {}

void ResourceOneFileReq::Load() {
    switch (m_state) {
    case 0: {
        UnkOneFile_8004CFB0 *file = new UnkOneFile_8004CFB0;
        if (file != 0) {
            file = fn_8004CFB0(file, m_fileName.c_str(), 0);
        }
        if (file != 0 && fn_8004CAF8(file)) {
            m_entryCount = file->m_toc->m_count;
            if (m_entryCount <= 0) {
                break;
            }
            m_entries = new ResourceOneFileReqEntry[m_entryCount];
            LoadBinFiles(file);
            LoadFiles(file);
            fn_8004CF68(file, 1);
        }
        m_state = 3;
        break;
    }
    case 3:
        break;
    case 4:
        break;
    }
}

bool ResourceOneFileReq::Release() {
    return Unload();
}

int ResourceOneFileReq::GetState() {
    return m_state;
}

void *ResourceOneFileReq::GetData(int index) {
    if (m_entries != 0) {
        if (m_entries[index - 2].m_type == 6) {
            return fn_8040E4BC(m_entries[index - 2].m_file, "SCENE");
        }
        if (m_entries[index - 2].m_type == 7) {
            return m_entries[index - 2].m_file->m_data;
        }
        if (m_entries[index - 2].m_type == 10) {
            return m_entries[index - 2].m_extra;
        }
        return m_entries[index - 2].m_data;
    }
    return 0;
}

void *ResourceOneFileReq::GetData(const char *name) {
    for (int i = 0; i < m_entryCount; i++) {
        if (strcmp(m_entries[i].m_name, name) == 0) {
            return GetData(i + 2);
        }
    }
    return 0;
}

int ResourceOneFileReq::GetIndex(const char *name) {
    for (int i = 0; i < m_entryCount; i++) {
        if (strcmp(m_entries[i].m_name, name) == 0) {
            return i + 2;
        }
    }
    return -1;
}

void *ResourceOneFileReq::GetHierarchy(int index) {
    if (m_entries != 0 && m_entries[index - 2].m_type == 1) {
        return m_entries[index - 2].m_hierarchy;
    }
    return 0;
}

void *ResourceOneFileReq::GetHierarchy(const char *name) {
    for (int i = m_entryCount - 1; i >= 0; i--) {
        if (strcmp(m_entries[i].m_name, name) == 0) {
            return m_entries[i].m_hierarchy;
        }
    }
    return 0;
}

u32 ResourceOneFileReq::GetMotion(int index) {
    if (m_entries != 0 && m_entries[index - 2].m_type == 2) {
        return m_entries[index - 2].m_motion;
    }
    return 0;
}

u32 ResourceOneFileReq::GetMotion(const char *name) {
    for (int i = m_entryCount - 1; i >= 0; i--) {
        if (strcmp(m_entries[i].m_name, name) == 0) {
            return m_entries[i].m_motion;
        }
    }
    return 0;
}

void *ResourceOneFileReq::vf_0x30(int index) {
    return 0;
}

void *ResourceOneFileReq::vf_0x34(const char *name) {
    return 0;
}

u32 ResourceOneFileReq::GetSize(int index) {
    if (m_entries != 0) {
        return m_entries[index - 2].m_size;
    }
    return 0;
}

u32 ResourceOneFileReq::GetSize(const char *name) {
    for (int i = 0; i < m_entryCount; i++) {
        if (strcmp(m_entries[i].m_name, name) == 0) {
            return GetSize(i + 2);
        }
    }
    return 0;
}

/* Loads the requested entries other than .BIN files by their extension. */
bool ResourceOneFileReq::LoadFiles(UnkOneFile_8004CFB0 *file) {
    UnkTextureNode *textures = 0;
    int index0;
    UnkOneFileEntry_8004CA28 *info0;
    int i;
    RwTexDictionary *dict;
    UnkTextureNode *list;
    for (i = 0; i < m_entryCount; i++) {
        index0 = i + 2;
        info0 = fn_8004CA28(file, index0);
        if (info0 != 0 && HasExtension(info0->m_name, ".TXD")) {
            dict = fn_8004BF40(file, index0, 0, 0);
            if (dict != 0) {
                fn_8048D0C4(dict);
                fn_80038B58(dict, 6);
                fn_803ACABC(m_entries[i].m_name, info0->m_name, 0x1F);
                m_entries[i].m_data = dict;
                m_entries[i].m_type = 4;
                m_entries[i].m_size = info0->m_size;
                list = NewTextureList();
                fn_8048CBE0(dict, fn_8006B7C8, list);
                textures = list;
                break;
            }
        }
    }

    for (ResourceRequest *request = m_requests.begin(); request != m_requests.end(); ++request) {
        if (request->m_type == 1) {
            continue;
        }
        int index = (request->m_flags & 1) ? fn_8004C910(file, request->m_name) : request->m_index;
        UnkOneFileEntry_8004CA28 *info = fn_8004CA28(file, index);
        if (info == 0) {
            continue;
        }
        int slot = index - 2;
        memset(m_entries[slot].m_name, 0, 0x20);
        fn_803ACABC(m_entries[slot].m_name, info->m_name, 0x1F);
        if (HasExtension(info->m_name, ".DFF")) {
            m_entries[slot].m_data = fn_8004BB24(file, index, 0, 0);
            m_entries[slot].m_type = 3;
            m_entries[slot].m_size = info->m_size;
            if (textures != 0) {
                fn_80456C54(static_cast<RpClump *>(m_entries[slot].m_data), fn_8006B638, textures);
            }
            fn_802E2A60(fn_800144C8(), m_entries[slot].m_data, 0);
            fn_8003815C(m_entries[slot].m_data);
        } else if (HasExtension(info->m_name, ".RP2")) {
            m_entries[slot].m_data = fn_8004BB24(file, index, 0, 0);
            m_entries[slot].m_type = 3;
            m_entries[slot].m_size = info->m_size;
            fn_8003815C(m_entries[slot].m_data);
        } else if (HasExtension(info->m_name, ".BON")) {
            m_entries[slot].m_data = fn_8004C358(file, index, 0, 0);
            m_entries[slot].m_type = 1;
            m_entries[slot].m_hierarchy = fn_804109C4(m_entries[slot].m_data, fn_8006B618);
            m_entries[slot].m_size = info->m_size;
        } else if (HasExtension(info->m_name, ".BOP")) {
            m_entries[slot].m_data = fn_8004C358(file, index, 0, 0);
            m_entries[slot].m_type = 1;
            m_entries[slot].m_hierarchy = fn_804108F0(m_entries[slot].m_data, fn_8006B618);
            m_entries[slot].m_size = info->m_size;
        } else if (HasExtension(info->m_name, ".MTN")) {
            m_entries[slot].m_data = fn_8004C358(file, index, 0, 0);
            m_entries[slot].m_type = 2;
            m_entries[slot].m_motion =
                fn_80412BFC(m_entries[slot].m_data, m_entries[slot].m_name, fn_8006B5F8, 1);
            m_entries[slot].m_size = info->m_size;
        } else if (HasExtension(info->m_name, ".MTP")) {
            m_entries[slot].m_data = fn_8004C358(file, index, 0, 0);
            m_entries[slot].m_type = 2;
            m_entries[slot].m_motion = fn_80412B74(m_entries[slot].m_data, fn_8006B5F8, 1);
            m_entries[slot].m_size = info->m_size;
        } else if (HasExtension(info->m_name, ".UVA")) {
            void *animation = fn_8004B70C(file, index, 0, 0);
            fn_804790CC(lbl_8056D748, animation);
            m_entries[slot].m_data = animation;
            m_entries[slot].m_type = 5;
            m_entries[slot].m_size = info->m_size;
        } else if (HasExtension(info->m_name, ".PTB") || HasExtension(info->m_name, ".CCL")) {
            m_entries[slot].m_data = fn_8004C358(file, index, 0, 0);
            m_entries[slot].m_type = 7;
            UnkPath_8040E61C *path = new UnkPath_8040E61C;
            if (path != 0) {
                path = __ct__16UnkPath_8040E61CFPv(path, m_entries[slot].m_data);
            }
            m_entries[slot].m_file = path;
            m_entries[slot].m_size = info->m_size;
        } else if (HasExtension(info->m_name, ".SPL")) {
        } else if (HasExtension(info->m_name, ".TXD")) {
        } else if (HasExtension(info->m_name, ".DMA")) {
            m_entries[slot].m_data = fn_8004ADB0(file, index, 0, 0);
            m_entries[slot].m_type = 8;
            m_entries[slot].m_size = info->m_size;
        } else if (HasExtension(info->m_name, ".SNCP") || HasExtension(info->m_name, ".GNCP") ||
                   HasExtension(info->m_name, ".XNCP")) {
            CProject *project = new CProject;
            void *data = fn_8004C358(file, index, 0, 0);
            if (data != 0) {
                fn_8036DB70(project, data, info->m_size, 0);
                fn_80049B34(data);
            }
            m_entries[slot].m_data = project;
            m_entries[slot].m_size = 0x4C;
            m_entries[slot].m_type = 9;
        } else if (HasExtension(info->m_name, ".ADB")) {
            m_entries[slot].m_data = fn_8004C358(file, index, 0, 0);
            m_entries[slot].m_extra = fn_8004FE88(fn_80013BC4(), m_entries[slot].m_data);
            m_entries[slot].m_type = 10;
            m_entries[slot].m_size = info->m_size;
        }
    }

    DestroyTextureList(textures);
    return true;
}

/* Loads the requested .BIN entries, and requests the files their model data names. */
bool ResourceOneFileReq::LoadBinFiles(UnkOneFile_8004CFB0 *file) {
    UnkOneFileEntry_8004CA28 *info;
    for (ResourceRequest *request = m_requests.begin(); request != m_requests.end(); ++request) {
        if (request->m_type != 1) {
            continue;
        }
        int index = (request->m_flags & 1) ? fn_8004C910(file, request->m_name) : request->m_index;
        info = fn_8004CA28(file, index);
        if (info == 0) {
            continue;
        }
        int slot = index - 2;
        fn_803ACABC(m_entries[slot].m_name, info->m_name, 0x1F);
        if (HasExtension(info->m_name, ".BIN")) {
            void *data = fn_80049B90(info->m_size, 2);
            fn_8004C748(file, index, data, 0, 0);
            m_entries[slot].m_data = data;
            m_entries[slot].m_type = 6;
            UnkPath_8040E61C *path = new UnkPath_8040E61C;
            if (path != 0) {
                path = __ct__16UnkPath_8040E61CFPv(path, data);
            }
            m_entries[slot].m_file = path;
            RequestReferences(path->m_data, file);
            m_entries[slot].m_size = info->m_size;
        }
    }
    return true;
}

/* Frees the loaded entries; the texture dictionaries last. */
bool ResourceOneFileReq::Unload() {
    if (m_entryCount <= 0) {
        return true;
    }
    for (int i = m_entryCount - 1; i >= 0; i--) {
        ResourceOneFileReqEntry &entry = m_entries[i];
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
        case 7:
            if (entry.m_file != 0) {
                __dt__16UnkPath_8040E61CFv(entry.m_file, 1);
            }
            fn_80049B34(m_entries[i].m_data);
            break;
        case 8:
            fn_80442B98(entry.m_data);
            break;
        case 9:
            if (entry.m_data != 0) {
                delete static_cast<CProject *>(entry.m_data);
            }
            break;
        case 10:
            fn_8004FE00(fn_80013BC4(), entry.m_extra);
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

/* Requests the files that a .BIN entry's model data names, and resolves its links to the nodes. */
void ResourceOneFileReq::RequestReferences(UnkBinData_8040E61C *data, UnkOneFile_8004CFB0 *file) {
    if (data == 0 || file == 0) {
        return;
    }
    UnkBinNode_8040E61C *node;
    int i;
    for (i = 0; i < data->m_nodeCount; i++) {
        node = &data->m_nodes[i];
        if (node->m_file0 != 0) {
            int index = fn_8004C910(file, node->m_file0);
            if (index != -1) {
                Request(index);
            }
        }
        if (node->m_file1 != 0) {
            int index = fn_8004C910(file, node->m_file1);
            if (index != -1) {
                Request(index);
            }
        }
        if (node->m_file2 != 0) {
            int index = fn_8004C910(file, node->m_file2);
            if (index != -1) {
                Request(index);
            }
        }
    }
    for (int ref = 0; ref < data->m_refCount; ref++) {
        if (data->m_refs[ref].m_file != 0) {
            int index = fn_8004C910(file, data->m_refs[ref].m_file);
            if (index != -1) {
                Request(index);
            }
        }
    }
    for (i = 0; i < data->m_partCount; i++) {
        UnkBinPart_8040E61C *part = &data->m_parts[i];
        if (part->m_file0 != 0) {
            int index = fn_8004C910(file, part->m_file0);
            if (index != -1) {
                Request(index);
            }
        }
        if (part->m_file1 != 0) {
            int index = fn_8004C910(file, part->m_file1);
            if (index != -1) {
                Request(index);
            }
        }
    }
    for (i = 0; i < data->m_groupCount; i++) {
        UnkBinGroup_8040E61C *group = &data->m_groups[i];
        for (int j = 0; j < group->m_linkCount; j++) {
            group->m_links[j].m_node = -1;
            for (int k = 0; k < data->m_nodeCount; k++) {
                if (strcmp(group->m_links[j].m_name, data->m_nodes[k].m_name) == 0) {
                    group->m_links[j].m_node = k;
                    break;
                }
            }
        }
    }
}

/* Requests entry index of this archive. */
void ResourceOneFileReq::Request(int index) {
    ResourceRequest request(index, 0);
    ResourceRequest *it = std::find(m_requests.begin(), m_requests.end(), request);
    if (it == m_requests.end()) {
        m_requests.push_back(request);
    }
}

#include <vector_impl>
