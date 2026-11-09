/* The motion library's registry of loaded files: each file holds entries (a header, then a tree
 * of 0x70-byte nodes); loading one byte-swaps what is in the other byte order, turns the tree's
 * offsets into pointers, enters every entry by name in one dictionary and every node in another,
 * and links the file into a list; unloading reverses it. Names are looked up by their bare file
 * name (directory, extension and anything after an underscore stripped, at most 31 characters).
 * Other units call these by their fn_ names, so they keep them; the file name is invented and
 * everything here is named from what the code does. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first).
 *
 * NonMatching: 12 of 13 functions match. In fn_80410BB4 retail keeps the path in r3 across the
 * directory loop's jump to its test, where GC/2.7 copies it from r29 at the test (3 rows of 50);
 * eight loop shapes, a separate pointer and an inline helper all give GC/2.7's form. */
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" {
unsigned long strlen(const char *s);
char *strcpy(char *dst, const char *src);
char *fn_803AC6C0(const char *s, const char *sub); /* strstr */
}

/* A node of an entry's tree (member names inferred). */
struct UnkMotionNode_80410D40 {
    /* 0x00 */ u32 m_id;
    /* 0x04 */ u16 m_values[5];
    /* 0x10 */ float m_vector0[4];
    /* 0x20 */ float m_vector1[4];
    /* 0x30 */ float m_vector2[4];
    /* 0x40 */ u8 field_0x40[8];
    /* 0x48 */ UnkMotionNode_80410D40 *m_left;  /* an offset from the entry until loaded */
    /* 0x4C */ UnkMotionNode_80410D40 *m_right; /* an offset from the entry until loaded */
    /* 0x50 */ char m_name[0x20];
};
SIZE_ASSERT(UnkMotionNode_80410D40, 0x70);

/* An entry of a file: its header, then the root of its tree (member names inferred). */
struct UnkMotionEntry_80410A4C {
    /* 0x00 */ u8 m_version;
    /* 0x01 */ u8 m_bigEndian;
    /* 0x04 */ u32 m_id;
    /* 0x08 */ u16 field_0x8;
    /* 0x0A */ u16 m_nodeCount;
    /* 0x0C */ u32 field_0xC;
    /* 0x10 */ char m_name[0x20];
    /* 0x30 */ UnkMotionNode_80410D40 m_root;
};

/* A loaded file (member names inferred). */
struct UnkMotionFile_804109C4 {
    /* 0x00 */ int m_count;
    /* 0x04 */ u32 m_id;
    /* 0x08 */ u8 m_owned;
    /* 0x09 */ u8 m_bigEndian;
    /* 0x0C */ void (*m_release)(void *data);
    /* 0x10 */ UnkMotionEntry_80410A4C *m_entries; /* an offset from the file until loaded */
    /* 0x14 */ UnkMotionFile_804109C4 *m_next;
};

/* The entry after entry. */
inline UnkMotionEntry_80410A4C *NextEntry(UnkMotionEntry_80410A4C *entry) {
    return (UnkMotionEntry_80410A4C *)((u8 *)&entry->m_root + entry->m_nodeCount * sizeof(UnkMotionNode_80410D40));
}

extern "C" {
/* The library's dictionaries: look up, remove, add, add returning the entry, destroy and create
 * (behaviour inferred). */
void *fn_80420C74(void *dictionary, const char *name);
void fn_80420CE8(void *dictionary, const char *name, u32 id);
void fn_80420DA4(void *dictionary, const char *name, u32 id);
u32 fn_80420EE0(void *dictionary, const char *name);
void fn_80421050(void *dictionary);
void *fn_804210AC(void);
/* The host's byte order and swapping words and halfwords in place (behaviour inferred). */
int fn_804211CC(void);
void fn_804210E4(void *dst, const void *src, int count);
void fn_8042118C(void *dst, const void *src, int count);
/* The library's memory (Game/SonicteamMotionMemory.cpp). */
void *fn_80411134(u32 size, int arg);
void fn_804110FC(void *data);

void fn_80410D40(UnkMotionEntry_80410A4C *entry, UnkMotionNode_80410D40 *node);
void fn_80410C7C(UnkMotionEntry_80410A4C *entry, UnkMotionNode_80410D40 *node);
int fn_80410BB4(char *name, const char *path);
int fn_80410A4C(UnkMotionFile_804109C4 *file);
u32 fn_804109C4(UnkMotionEntry_80410A4C *entries, void (*release)(void *data));
u32 fn_804108F0(void *data, void (*release)(void *data));
void fn_804107EC(u32 id);
void fn_804107C0(void);
void fn_80410774(void);
u32 *fn_804106FC(u32 id);
void *fn_804106C0(const char *name);
void *fn_80410684(const char *name);
void *fn_80410630(const char *path);
}

UnkMotionFile_804109C4 *lbl_805F1388;
void *lbl_805F138C;
void *lbl_805F1390;

/* Byte-swaps a node when its entry is in the other byte order, and enters it in the node
 * dictionary. */
inline void LoadNode(UnkMotionEntry_80410A4C *entry, UnkMotionNode_80410D40 *node) {
    if ((entry->m_bigEndian && !fn_804211CC()) || (!entry->m_bigEndian && fn_804211CC())) {
        fn_804210E4(&node->m_id, &node->m_id, 1);
        fn_8042118C(&node->m_values[0], &node->m_values[0], 1);
        fn_8042118C(&node->m_values[1], &node->m_values[1], 1);
        fn_8042118C(&node->m_values[2], &node->m_values[2], 1);
        fn_8042118C(&node->m_values[3], &node->m_values[3], 1);
        fn_8042118C(&node->m_values[4], &node->m_values[4], 1);
        fn_804210E4(node->m_vector0, node->m_vector0, 4);
        fn_804210E4(node->m_vector1, node->m_vector1, 4);
        fn_804210E4(node->m_vector2, node->m_vector2, 4);
        fn_804210E4(&node->m_left, &node->m_left, 1);
        fn_804210E4(&node->m_right, &node->m_right, 1);
    }
    fn_80420DA4(lbl_805F1390, node->m_name, node->m_id);
}

/* A child node loaded with its own children. */
inline void LoadChild(UnkMotionEntry_80410A4C *entry, UnkMotionNode_80410D40 *node) {
    if ((entry->m_bigEndian && !fn_804211CC()) || (!entry->m_bigEndian && fn_804211CC())) {
        fn_804210E4(&node->m_id, &node->m_id, 1);
        fn_8042118C(&node->m_values[0], &node->m_values[0], 1);
        fn_8042118C(&node->m_values[1], &node->m_values[1], 1);
        fn_8042118C(&node->m_values[2], &node->m_values[2], 1);
        fn_8042118C(&node->m_values[3], &node->m_values[3], 1);
        fn_8042118C(&node->m_values[4], &node->m_values[4], 1);
        fn_804210E4(node->m_vector0, node->m_vector0, 4);
        fn_804210E4(node->m_vector1, node->m_vector1, 4);
        fn_804210E4(node->m_vector2, node->m_vector2, 4);
        fn_804210E4(&node->m_left, &node->m_left, 1);
        fn_804210E4(&node->m_right, &node->m_right, 1);
    }
    fn_80420DA4(lbl_805F1390, node->m_name, node->m_id);
    if (node->m_left != 0) {
        node->m_left = (UnkMotionNode_80410D40 *)((u32)node->m_left + (u32)entry);
        fn_80410D40(entry, node->m_left);
    }
    if (node->m_right != 0) {
        node->m_right = (UnkMotionNode_80410D40 *)((u32)node->m_right + (u32)entry);
        fn_80410D40(entry, node->m_right);
    }
}

/* Loads a node and its subtree. */
void fn_80410D40(UnkMotionEntry_80410A4C *entry, UnkMotionNode_80410D40 *node) {
    LoadNode(entry, node);
    if (node->m_left != 0) {
        node->m_left = (UnkMotionNode_80410D40 *)((u32)node->m_left + (u32)entry);
        LoadChild(entry, node->m_left);
    }
    if (node->m_right != 0) {
        node->m_right = (UnkMotionNode_80410D40 *)((u32)node->m_right + (u32)entry);
        LoadChild(entry, node->m_right);
    }
}

/* A child node removed from the node dictionary with its own children. */
inline void UnloadChild(UnkMotionEntry_80410A4C *entry, UnkMotionNode_80410D40 *node) {
    fn_80420CE8(lbl_805F1390, node->m_name, node->m_id);
    if (node->m_left != 0) {
        fn_80410C7C(entry, node->m_left);
    }
    if (node->m_right != 0) {
        fn_80410C7C(entry, node->m_right);
    }
}

/* Removes a node and its subtree from the node dictionary. */
void fn_80410C7C(UnkMotionEntry_80410A4C *entry, UnkMotionNode_80410D40 *node) {
    fn_80420CE8(lbl_805F1390, node->m_name, node->m_id);
    if (node->m_left != 0) {
        UnloadChild(entry, node->m_left);
    }
    if (node->m_right != 0) {
        UnloadChild(entry, node->m_right);
    }
}

/* The bare name of a path: no directory, extension or underscore suffix (0 if it is too long). */
int fn_80410BB4(char *name, const char *path) {
    char buffer[0x100];
    const char *separator = "/";
    char *found;
    while ((found = fn_803AC6C0(path, separator)) != 0) {
        path = found + strlen(separator);
    }
    strcpy(buffer, path);
    char *dot = fn_803AC6C0(buffer, ".");
    if (dot != 0) {
        *dot = 0;
    }
    char *underscore = fn_803AC6C0(buffer, "_");
    if (underscore != 0) {
        *underscore = 0;
    }
    char *bare = buffer;
    if (strlen(bare) < 0x20) {
        strcpy(name, bare);
        return 1;
    }
    return 0;
}

/* Checks an entry's version and byte-swaps its header when it is in the other byte order. */
inline int LoadEntryHeader(UnkMotionEntry_80410A4C *entry) {
    if (entry->m_version >= 4) {
        if ((entry->m_bigEndian && !fn_804211CC()) || (!entry->m_bigEndian && fn_804211CC())) {
            fn_804210E4(&entry->m_id, &entry->m_id, 1);
            fn_8042118C(&entry->field_0x8, &entry->field_0x8, 1);
            fn_8042118C(&entry->m_nodeCount, &entry->m_nodeCount, 1);
            fn_804210E4(&entry->field_0xC, &entry->field_0xC, 1);
        }
        return 1;
    }
    return 0;
}

/* Loads a file's entries and links it into the list. */
int fn_80410A4C(UnkMotionFile_804109C4 *file) {
    int i = 0;
    UnkMotionEntry_80410A4C *entry = file->m_entries;
    file->m_id = 0;
    for (; i < file->m_count; i++) {
        if (!LoadEntryHeader(entry)) {
            return 0;
        }
        entry->m_id = fn_80420EE0(lbl_805F138C, entry->m_name);
        if (entry->m_id != 0) {
            file->m_id = entry->m_id;
        }
        fn_80410D40(entry, &entry->m_root);
        entry = NextEntry(entry);
    }
    if (file->m_id == 0) {
        return 0;
    }
    if (lbl_805F1388 != 0) {
        UnkMotionFile_804109C4 *last;
        for (last = lbl_805F1388; last->m_next != 0; last = last->m_next) {
        }
        last->m_next = file;
    } else {
        lbl_805F1388 = file;
    }
    return 1;
}

/* Loads one entry held in memory, through a file the library allocates. */
u32 fn_804109C4(UnkMotionEntry_80410A4C *entries, void (*release)(void *data)) {
    UnkMotionFile_804109C4 *file = (UnkMotionFile_804109C4 *)fn_80411134(1, 0x20);
    file->m_count = 1;
    file->m_owned = 1;
    if (fn_804211CC()) {
        file->m_bigEndian = 1;
    }
    file->m_release = release;
    file->m_entries = entries;
    if (!fn_80410A4C(file)) {
        fn_804110FC(file);
        return 0;
    }
    return file->m_id;
}

/* Loads a file held in memory. */
u32 fn_804108F0(void *data, void (*release)(void *data)) {
    UnkMotionFile_804109C4 *file = (UnkMotionFile_804109C4 *)data;
    if (file == 0) {
        return 0;
    }
    if ((file->m_bigEndian && !fn_804211CC()) || (!file->m_bigEndian && fn_804211CC())) {
        fn_804210E4(&file->m_count, &file->m_count, 1);
        fn_804210E4(&file->m_id, &file->m_id, 1);
        fn_804210E4(&file->m_entries, &file->m_entries, 1);
        fn_804210E4(&file->m_next, &file->m_next, 1);
    }
    file->m_entries = (UnkMotionEntry_80410A4C *)((u32)file->m_entries + (u32)file);
    file->m_release = release;
    if (fn_80410A4C(file)) {
        return file->m_id;
    }
    return 0;
}

/* Unloads the file whose id is id. */
void fn_804107EC(u32 id) {
    int i;
    UnkMotionFile_804109C4 *file;
    if (lbl_805F1388 == 0) {
        return;
    }
    UnkMotionFile_804109C4 *previous = 0;
    for (file = lbl_805F1388; file != 0; previous = file, file = file->m_next) {
        if (file->m_id == id) {
            if (previous != 0) {
                previous->m_next = file->m_next;
            } else {
                lbl_805F1388 = file->m_next;
            }
            UnkMotionEntry_80410A4C *entry = file->m_entries;
            for (i = 0; i < file->m_count; i++) {
                fn_80410C7C(entry, &entry->m_root);
                fn_80420CE8(lbl_805F138C, entry->m_name, entry->m_id);
                entry = NextEntry(entry);
            }
            void (*release)(void *data) = file->m_release;
            if (file->m_owned) {
                if (release != 0) {
                    release(file->m_entries);
                }
                fn_804110FC(file);
            } else if (release != 0) {
                release(file);
            }
            return;
        }
    }
}

/* Creates the dictionaries. */
void fn_804107C0(void) {
    lbl_805F138C = fn_804210AC();
    lbl_805F1390 = fn_804210AC();
}

/* Destroys the dictionaries. */
void fn_80410774(void) {
    if (lbl_805F138C != 0) {
        fn_80421050(lbl_805F138C);
        lbl_805F138C = 0;
    }
    if (lbl_805F1390 != 0) {
        fn_80421050(lbl_805F1390);
        lbl_805F1390 = 0;
    }
}

/* The entry whose id is id. */
inline UnkMotionEntry_80410A4C *FindEntry(u32 id) {
    UnkMotionFile_804109C4 *file = lbl_805F1388;
    if (file == 0) {
        return 0;
    }
    for (; file != 0; file = file->m_next) {
        UnkMotionEntry_80410A4C *entry = file->m_entries;
        for (int i = 0; i < file->m_count; i++) {
            if (id == entry->m_id) {
                return entry;
            }
            entry = (UnkMotionEntry_80410A4C *)((u8 *)&entry->m_root + entry->m_nodeCount * sizeof(UnkMotionNode_80410D40));
        }
    }
    return 0;
}

/* The entry whose id is id (its id's address), or 0. */
u32 *fn_804106FC(u32 id) {
    UnkMotionEntry_80410A4C *entry = FindEntry(id);
    if (entry != 0) {
        return &entry->m_id;
    }
    return 0;
}

/* The entry called name. */
void *fn_804106C0(const char *name) {
    if (lbl_805F138C != 0) {
        return fn_80420C74(lbl_805F138C, name);
    }
    return 0;
}

/* The node called name. */
void *fn_80410684(const char *name) {
    if (lbl_805F1390 != 0) {
        return fn_80420C74(lbl_805F1390, name);
    }
    return 0;
}

/* The entry named by a path. */
void *fn_80410630(const char *path) {
    char name[0x20];
    if (fn_80410BB4(name, path)) {
        if (lbl_805F138C != 0) {
            return fn_80420C74(lbl_805F138C, name);
        }
        return 0;
    }
    return 0;
}
