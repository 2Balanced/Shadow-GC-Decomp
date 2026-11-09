/* Small parts of the motion library (no class with RTTI; file name and all names inferred): its
 * start-up and shut-down (fn_80420C50 and fn_80420C30, which Game/MotionHeap.cpp calls), a list
 * of reference-counted names with ids, byte-order helpers for 16- and 32-bit data, and the switch
 * that tells the library's loaders whether to use them. The library's file readers (0x804108F0
 * to 0x804139B4) are the callers. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
/* NonMatching: 11 of 12 functions match. fn_804210E4 (78%): its inlined byte swaps test the
 * address of the first half-word twice (two addic.) and do not fold the branches of the second
 * test; GC/2.7 keeps the address in a register and folds them (about fifteen spellings of the
 * temporary and the helper tried). */
#include <dolphin/types.h>
#include <string.h>

/* One name of a list (layout from the functions below). */
struct UnkMotionName_80420C74 {
    /* 0x00 */ u32 m_id;
    /* 0x04 */ char *m_name;
    /* 0x08 */ u32 field_0x8;
    /* 0x0C */ s32 m_refs;
    /* 0x10 */ UnkMotionName_80420C74 *m_next;
};

/* A list of names (allocated by fn_804210AC). */
struct UnkMotionNameList_80420C74 {
    /* 0x00 */ UnkMotionName_80420C74 *m_head;
};

extern "C" {
/* The motion library's memory (fn_804110EC sets the callbacks; Game/MotionHeap.cpp). */
void *fn_80411134(int kind, u32 size);
void fn_804110FC(void *data);
/* The rest of the library's start-up, and its ARAM memory (Game/MotionAram.cpp). */
void fn_80415784(void);
void fn_80421560(void);
void fn_80421500(void);

u32 fn_804211D4(u32 swap);
u32 fn_804211CC(void);
void fn_8042118C(u8 *dst, const u8 *src, int count);
void fn_804210E4(u16 *dst, const u16 *src, int count);
UnkMotionNameList_80420C74 *fn_804210AC(void);
void fn_80421050(UnkMotionNameList_80420C74 *list);
u32 fn_80420EE0(UnkMotionNameList_80420C74 *list, const char *name);
u32 fn_80420DA4(UnkMotionNameList_80420C74 *list, const char *name, u32 id);
void fn_80420CE8(UnkMotionNameList_80420C74 *list, const char *name, u32 id);
u32 fn_80420C74(UnkMotionNameList_80420C74 *list, const char *name);
void fn_80420C50(void);
void fn_80420C30(void);
}

/* Whether the library swaps the byte order of the data it loads. */
u32 lbl_805F13E8;

/* Swaps the two bytes of each of count 16-bit values. */
inline void SwapBytes16(u8 *dst, const u8 *src, int count) {
    if (src == 0 || dst == 0) {
        return;
    }
    for (int i = 0; i < count; i++) {
        int lo = src[0];
        int hi = src[1];
        src += 2;
        dst[0] = hi;
        dst[1] = lo;
        dst += 2;
    }
}

/* Sets whether loaded data is byte-swapped; returns the old setting. */
u32 fn_804211D4(u32 swap) {
    u32 old = lbl_805F13E8;
    lbl_805F13E8 = swap;
    return old;
}

u32 fn_804211CC(void) {
    return lbl_805F13E8;
}

void fn_8042118C(u8 *dst, const u8 *src, int count) {
    if (src == 0 || dst == 0) {
        return;
    }
    for (int i = 0; i < count; i++) {
        int lo = src[0];
        int hi = src[1];
        src += 2;
        dst[0] = hi;
        dst[1] = lo;
        dst += 2;
    }
}

/* Reverses the byte order of count 32-bit values. */
void fn_804210E4(u16 *dst, const u16 *src, int count) {
    if (src == 0 || dst == 0) {
        return;
    }
    for (int i = 0; i < count; i++) {
        u16 half[2];
        half[0] = src[0];
        half[1] = src[1];
        u8 *p0 = (u8 *)&half[0];
        SwapBytes16(p0, p0, 1);
        u8 *p1 = (u8 *)&half[1];
        SwapBytes16(p1, p1, 1);
        dst[0] = half[1];
        dst[1] = half[0];
        src += 2;
        dst += 2;
    }
}

UnkMotionNameList_80420C74 *fn_804210AC(void) {
    UnkMotionNameList_80420C74 *list =
        (UnkMotionNameList_80420C74 *)fn_80411134(1, sizeof(UnkMotionNameList_80420C74));
    if (list != 0) {
        list->m_head = 0;
    }
    return list;
}

void fn_80421050(UnkMotionNameList_80420C74 *list) {
    if (list != 0) {
        UnkMotionName_80420C74 *entry = list->m_head;
        while (entry != 0) {
            UnkMotionName_80420C74 *next = entry->m_next;
            fn_804110FC(entry->m_name);
            fn_804110FC(entry);
            entry = next;
        }
        fn_804110FC(list);
    }
}

/* Adds a reference to name, giving a new name the lowest id not in use; returns its id. */
u32 fn_80420EE0(UnkMotionNameList_80420C74 *list, const char *name) {
    UnkMotionName_80420C74 *entry;
    UnkMotionName_80420C74 *prev = 0;
    if (list == 0 || name == 0) {
        return 0;
    }
    if (list->m_head == 0) {
        entry = list->m_head =
            (UnkMotionName_80420C74 *)fn_80411134(1, sizeof(UnkMotionName_80420C74));
    } else {
        for (entry = list->m_head; entry != 0; entry = entry->m_next) {
            if (strcmp(entry->m_name, name) == 0) {
                entry->m_refs++;
                return entry->m_id;
            }
            if (entry->m_next == 0) {
                prev = entry;
                entry = (UnkMotionName_80420C74 *)fn_80411134(1, sizeof(UnkMotionName_80420C74));
                break;
            }
        }
    }
    if (entry != 0) {
        u32 id;
        for (id = 1; id <= 0xFFFFFFFF; id++) {
            UnkMotionName_80420C74 *used;
            for (used = list->m_head; used != 0; used = used->m_next) {
                if (id == used->m_id) {
                    break;
                }
            }
            if (used == 0) {
                break;
            }
        }
        entry->field_0x8 = 0;
        entry->m_name = (char *)fn_80411134(1, strlen(name) + 1);
        entry->m_next = 0;
        entry->m_refs = 1;
        strcpy(entry->m_name, name);
        entry->m_id = id;
        if (prev != 0) {
            prev->m_next = entry;
        }
        return entry->m_id;
    }
    return 0;
}

/* Adds a reference to name with id; returns the id, or 0. */
u32 fn_80420DA4(UnkMotionNameList_80420C74 *list, const char *name, u32 id) {
    UnkMotionName_80420C74 *entry;
    UnkMotionName_80420C74 *prev = 0;
    if (list == 0 || name == 0) {
        return 0;
    }
    if (list->m_head == 0) {
        entry = list->m_head =
            (UnkMotionName_80420C74 *)fn_80411134(1, sizeof(UnkMotionName_80420C74));
    } else {
        for (entry = list->m_head; entry != 0; entry = entry->m_next) {
            if (strcmp(entry->m_name, name) == 0 && entry->m_id == id) {
                entry->m_refs++;
                return entry->m_id;
            }
            if (entry->m_next == 0) {
                prev = entry;
                entry = (UnkMotionName_80420C74 *)fn_80411134(1, sizeof(UnkMotionName_80420C74));
                break;
            }
        }
    }
    if (entry != 0) {
        entry->field_0x8 = 0;
        entry->m_name = (char *)fn_80411134(1, strlen(name) + 1);
        entry->m_next = 0;
        strcpy(entry->m_name, name);
        entry->m_id = id;
        entry->m_refs = 1;
        if (prev != 0) {
            prev->m_next = entry;
        }
        return entry->m_id;
    }
    return 0;
}

/* Drops a reference to name with id, and removes it when none is left. */
void fn_80420CE8(UnkMotionNameList_80420C74 *list, const char *name, u32 id) {
    UnkMotionName_80420C74 *entry;
    UnkMotionName_80420C74 *prev = 0;
    if (list == 0 || name == 0) {
        return;
    }
    for (entry = list->m_head; entry != 0; prev = entry, entry = entry->m_next) {
        if (strcmp(entry->m_name, name) == 0 && entry->m_id == id) {
            if (--entry->m_refs == 0) {
                if (prev != 0) {
                    prev->m_next = entry->m_next;
                } else {
                    list->m_head = entry->m_next;
                }
                fn_804110FC(entry->m_name);
                fn_804110FC(entry);
            }
            return;
        }
    }
}

/* The id of name, or 0. */
u32 fn_80420C74(UnkMotionNameList_80420C74 *list, const char *name) {
    if (list == 0 || name == 0) {
        return 0;
    }
    for (UnkMotionName_80420C74 *entry = list->m_head; entry != 0; entry = entry->m_next) {
        if (strcmp(entry->m_name, name) == 0) {
            return entry->m_id;
        }
    }
    return 0;
}

void fn_80420C50(void) {
    fn_80415784();
    fn_80421560();
}

void fn_80420C30(void) {
    fn_80421500();
}
