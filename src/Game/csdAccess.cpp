/* csdAccess.cpp (file name from the allocator's records): the lookups of the Chao CSD 2D library
 * into a loaded project. A project's scene tree is a tree of scene nodes; each node holds its
 * scenes and its child nodes together with name tables sorted for bsearch. Paths separate node
 * names with '/'. The file also holds the default values of the cast attributes and the lookup of
 * an attribute in a cast's packed attribute list. All names are inferred.
 *
 * The CSD library is built with its own flags (configure.py): no small data (-sdata 0 -sdata2 0)
 * and automatic inlining one level deep (-inline auto,level=1), so functions come out in
 * definition order and only functions that call no inline function are inlined. Its read-only
 * data is declared const (GC/2.7 then keeps loads of it out of loops).
 *
 * NonMatching: 20 of 21 functions match, and every data section. InitCast (0x8035C294) uses five
 * float constants. GC/2.7 addresses three or more constants of a function through one pooled base
 * register; retail never does so anywhere in the CSD library (each constant is addressed alone,
 * as GC/2.7 does for an external const object whose initializer it has not seen), while it still
 * pools the file's static variables. No flag (-pool, -opt, -str, compiler 1.0..3.0a5) reproduces
 * that, and const objects defined after use land in .data. The same blocker affects 9 functions
 * of the library. */
#include <string.h>

#include <dolphin/types.h>
#include "size_assert.h"

extern "C" {
/* MSL bsearch and strncpy/strncmp (the symbols are still placeholders). */
void *fn_803A7B40(const void *key, const void *base, u32 count, u32 size,
                  int (*compare)(const void *, const void *));
int fn_803AC8DC(const char *a, const char *b, u32 size);
char *fn_803ACABC(char *dst, const char *src, u32 size);
/* The CSD allocator (behaviour inferred): allocating with the source file and line, and freeing. */
void *fn_80360564(u32 size, const char *file, int line);
void fn_80360528(void *p);
/* Sets a cast's matrix to the identity (csdMatrix.cpp; behaviour inferred). */
void fn_8036017C(void *cast);
}

namespace Chao {
namespace CSD {

/* strchr for a modifiable string (the C++ overload of the original library headers, which is
 * inline). */
inline char *FindChar(char *s, int c) {
    return strchr(s, c);
}

/* An entry of a sorted name table. */
struct SNameIndex {
    /* 0x0 */ const char *m_name;
    /* 0x4 */ s32 m_index;
};

/* A key to search a name table with (the path lookups build it with a constructor). */
struct SNameKey : public SNameIndex {
    SNameKey(const char *name) {
        m_name = name;
        m_index = -1;
    }
};

/* An entry of a sorted name table with two values. */
struct SNameIndex2 {
    /* 0x0 */ const char *m_name;
    /* 0x4 */ s32 m_index;
    /* 0x8 */ s32 m_value;
};

struct SScene;

/* A node of a project's scene tree. */
struct SSceneNode {
    /* 0x00 */ s32 m_sceneCount;
    /* 0x04 */ SScene **m_scenes;
    /* 0x08 */ SNameIndex *m_sceneIndex;
    /* 0x0C */ s32 m_childCount;
    /* 0x10 */ const SSceneNode *m_children;
    /* 0x14 */ SNameIndex *m_childIndex;
};
SIZE_ASSERT(SSceneNode, 0x18);

/* A loaded project. */
struct SProject {
    /* 0x0 */ SSceneNode *m_root;
    /* 0x4 */ const char *m_name;
};

/* The owner of a SNameIndex2 table (only the table is used here). */
struct SNameTable2Owner {
    /* 0x00 */ u8 field_0x0[0x2C];
    /* 0x2C */ s32 m_count;
    /* 0x30 */ SNameIndex2 *m_entries;
};

/* A cast attribute value. */
struct CValue {
    CValue() : m_int(0) {}

    union {
        s32 m_int;
        f32 m_float;
    };
};

int CompareName(const void *a, const void *b);
int CompareName2(const void *a, const void *b);
int HasSeparator(const char *path);
void RemoveHead(char *path);
void CopyHead(char *dst, const char *path);

/* Packs two floats into one 64-bit value. */
u64 PackFloat2(f32 x, f32 y) {
    union {
        f32 m_floats[2];
        u64 m_value;
    } value;
    value.m_floats[0] = x;
    value.m_floats[1] = y;
    return value.m_value;
}

/* The index stored for name, or -1. */
s32 FindIndex(const SNameIndex *table, s32 count, const char *name) {
    SNameIndex key;
    key.m_name = name;
    key.m_index = -1;
    SNameIndex *found = (SNameIndex *)fn_803A7B40(&key, table, count, sizeof(SNameIndex), CompareName);
    if (found != 0) {
        return found->m_index;
    }
    return -1;
}

/* The node at path below node. */
const SSceneNode *FindNode(char *path, const SSceneNode *node) {
    if (HasSeparator(path)) {
        char *head = (char *)fn_80360564(strlen(path) + 1, "csdAccess.cpp", 0x416);
        if (head == 0) {
            return 0;
        }
        CopyHead(head, path);
        SNameKey key(head);
        SNameIndex *found = (SNameIndex *)fn_803A7B40(&key, node->m_childIndex, node->m_childCount,
                                                      sizeof(SNameIndex), CompareName);
        fn_80360528(head);
        if (found != 0) {
            RemoveHead(path);
            return FindNode(path, &node->m_children[found->m_index]);
        }
        return 0;
    }
    SNameKey key(path);
    SNameIndex *found = (SNameIndex *)fn_803A7B40(&key, node->m_childIndex, node->m_childCount,
                                                  sizeof(SNameIndex), CompareName);
    if (found != 0) {
        return &node->m_children[found->m_index];
    }
    return 0;
}

/* The node called name anywhere below node. */
const SSceneNode *SearchNode(const char *name, const SSceneNode *node) {
    SNameIndex key;
    key.m_name = name;
    key.m_index = -1;
    SNameIndex *found = (SNameIndex *)fn_803A7B40(&key, node->m_childIndex, node->m_childCount,
                                                  sizeof(SNameIndex), CompareName);
    if (found != 0) {
        return &node->m_children[found->m_index];
    }
    for (s32 i = 0; i < node->m_childCount; i++) {
        const SSceneNode *result = SearchNode(name, &node->m_children[i]);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}

/* The node at path, whose first name is the project's. */
const SSceneNode *GetNode(const char *path, const SProject *project) {
    char *copy = (char *)fn_80360564(strlen(path) + 1, "csdAccess.cpp", 0x3C5);
    strcpy(copy, path);
    if (fn_803AC8DC(copy, project->m_name, FindChar(copy, '/') - copy) != 0) {
        return 0;
    }
    RemoveHead(copy);
    const SSceneNode *node = FindNode(copy, project->m_root);
    fn_80360528(copy);
    return node;
}

/* The node called name anywhere in the project. */
const SSceneNode *SearchNode(const char *name, const SProject *project) {
    return SearchNode(name, project->m_root);
}

/* The scene at path below node; its table name goes to name. */
SScene *FindScene(char *path, const SSceneNode *node, const char **name) {
    if (HasSeparator(path)) {
        char *head = (char *)fn_80360564(strlen(path) + 1, "csdAccess.cpp", 0x305);
        if (head == 0) {
            return 0;
        }
        CopyHead(head, path);
        SNameKey key(head);
        SNameIndex *found = (SNameIndex *)fn_803A7B40(&key, node->m_childIndex, node->m_childCount,
                                                      sizeof(SNameIndex), CompareName);
        fn_80360528(head);
        if (found != 0) {
            RemoveHead(path);
            return FindScene(path, &node->m_children[found->m_index], name);
        }
        return 0;
    }
    SNameKey key(path);
    SNameIndex *found = (SNameIndex *)fn_803A7B40(&key, node->m_sceneIndex, node->m_sceneCount,
                                                  sizeof(SNameIndex), CompareName);
    if (found != 0) {
        if (name != 0) {
            *name = node->m_sceneIndex[found->m_index].m_name;
        }
        return node->m_scenes[found->m_index];
    }
    return 0;
}

/* Whether path holds a separator. */
int HasSeparator(const char *path) {
    bool result = strchr(path, '/') != 0;
    return result;
}

/* Removes the first name and its separator from path. */
void RemoveHead(char *path) {
    char *copy = (char *)fn_80360564(strlen(path) + 1, "csdAccess.cpp", 0x2D3);
    if (copy == 0) {
        strcpy(path, "");
        return;
    }
    strcpy(copy, path);
    strcpy(path, FindChar(path, '/') + 1);
    fn_80360528(copy);
}

/* Copies the first name of path to dst. */
void CopyHead(char *dst, const char *path) {
    const char *separator = strchr(path, '/');
    fn_803ACABC(dst, path, (u32)separator - (u32)path);
    dst[separator - path] = 0;
}

/* The scene at path, whose first name is the project's; its table name goes to name. */
SScene *GetScene(const char *path, const SProject *project, const char **name) {
    if (name != 0) {
        *name = 0;
    }
    char *copy = (char *)fn_80360564(strlen(path) + 1, "csdAccess.cpp", 0x29B);
    strcpy(copy, path);
    if (fn_803AC8DC(copy, project->m_name, FindChar(copy, '/') - copy) != 0) {
        return 0;
    }
    RemoveHead(copy);
    SScene *scene = FindScene(copy, project->m_root, name);
    fn_80360528(copy);
    return scene;
}

/* The entry called name of owner's table. */
SNameIndex2 *FindEntry(const char *name, const SNameTable2Owner *owner) {
    SNameIndex2 key;
    key.m_name = name;
    key.m_index = -1;
    key.m_value = -1;
    SNameIndex2 *found = (SNameIndex2 *)fn_803A7B40(&key, owner->m_entries, owner->m_count,
                                                    sizeof(SNameIndex2), CompareName2);
    if (found != 0) {
        return found;
    }
    return 0;
}

/* The scene called name anywhere below node; its name goes to name. */
SScene *SearchScene(const char *name, const SSceneNode *node, const char **outName) {
    SNameIndex key;
    key.m_name = name;
    key.m_index = -1;
    SNameIndex *found = (SNameIndex *)fn_803A7B40(&key, node->m_sceneIndex, node->m_sceneCount,
                                                  sizeof(SNameIndex), CompareName);
    if (found != 0) {
        if (outName != 0) {
            *outName = found->m_name;
        }
        return node->m_scenes[found->m_index];
    }
    for (s32 i = 0; i < node->m_childCount; i++) {
        SScene *result = SearchScene(name, &node->m_children[i], outName);
        if (result != 0) {
            return result;
        }
    }
    return 0;
}

/* The scene called name anywhere in the project; its name goes to name. */
SScene *SearchScene(const char *name, const SProject *project, const char **outName) {
    if (outName != 0) {
        *outName = 0;
    }
    return SearchScene(name, project->m_root, outName);
}

/* The default cast attributes, whether they are set, and the mask of all attributes. */
static s32 s_defaultsSet;
static CValue s_defaults[15];
static s32 s_cachedIndex;
static s32 s_cachedAttribute;
static u32 s_cachedMask;
static u32 s_allAttributes;

inline void SetDefaults() {
    if (s_defaultsSet == 0) {
        s_defaults[0].m_int = 0;
        s_defaults[1].m_float = 0.0f;
        s_defaults[2].m_float = 0.0f;
        s_defaults[3].m_float = 0.0f;
        s_defaults[4].m_float = 1.0f;
        s_defaults[5].m_float = 1.0f;
        s_defaults[6].m_float = 0.0f;
        s_defaults[7].m_int = -1;
        s_defaults[8].m_int = -1;
        s_defaults[9].m_int = -1;
        s_defaults[10].m_int = -1;
        s_defaults[11].m_int = -1;
        s_defaults[12].m_int = 0;
        s_defaults[13].m_int = 0;
        s_defaults[14].m_int = 0;
        s_defaultsSet = 1;
        for (s32 i = 0; i < 15; i++) {
            s_allAttributes |= 1 << i;
        }
    }
}

/* Clears a cast's state (0x144 bytes) and sets its defaults. */
void InitCast(void *cast) {
    SetDefaults();
    memset(cast, 0, 0x144);
    fn_8036017C(cast);
    u8 *p = (u8 *)cast;
    *(s32 *)(p + 0x24) = -1;
    *(s32 *)(p + 0x44) = -1;
    *(f32 *)(p + 0x38) = 1.0f;
    *(f32 *)(p + 0x3C) = 1.0f;
    *(f32 *)(p + 0x40) = -100000.0f;
    *(s32 *)(p + 0x48) = *(s32 *)(p + 0x4C) = *(s32 *)(p + 0x50) = *(s32 *)(p + 0x54) = -1;
    *(f32 *)(p + 0x6C) = -1.0f;
    *(f32 *)(p + 0x70) = -1000000.0f;
}

int CompareName(const void *a, const void *b) {
    return strcmp(((const SNameIndex *)a)->m_name, ((const SNameIndex *)b)->m_name);
}

int CompareName2(const void *a, const void *b) {
    return strcmp(((const SNameIndex2 *)a)->m_name, ((const SNameIndex2 *)b)->m_name);
}

/* The position of attribute attr in a packed list that holds the attributes of mask, or -1 (the
 * last position found is cached). */
inline s32 FindAttribute(s32 attr, u32 mask) {
    if (mask == 0) {
        return -1;
    }
    s32 index = 0;
    s32 i = 0;
    if (s_cachedMask == mask && s_cachedAttribute <= attr) {
        index = s_cachedIndex;
        i = s_cachedAttribute;
    }
    for (; i < 15; i++) {
        if (i == attr) {
            s_cachedIndex = index;
            s_cachedAttribute = attr;
            s_cachedMask = mask;
            if (mask & (1 << i)) {
                return index;
            }
            return -1;
        }
        if (mask & (1 << i)) {
            index++;
        }
    }
    s_cachedMask = 0;
    return -1;
}

/* Attribute attr of a cast whose packed values present hold the attributes of mask, or its
 * default. */
void GetAttribute(CValue *out, s32 attr, u32 mask, const CValue *present) {
    s32 index = FindAttribute(attr, mask);
    if (index < 0) {
        SetDefaults();
        *out = s_defaults[attr];
    } else {
        *out = present[index];
    }
}

u32 GetAllAttributes() {
    return s_allAttributes;
}

} // namespace CSD
} // namespace Chao
