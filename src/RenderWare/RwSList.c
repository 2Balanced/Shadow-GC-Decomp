/* RenderWare growable arrays (rwSList), 0x8047DC90..0x8047E100: create, empty, new entries,
 * destroy, the array and its entries. No data of its own.
 *
 * Written from the retail code; the fn_ symbols are kept because game code calls them by address.
 * C, built with cflags_rw_r5 (-O4,p, no stmw/lmw, automatic inlining in definition order). */
#include "Game/rw/r5.h"

struct RwSList {
    /* 0x00 */ u8 *entries;
    /* 0x04 */ s32 numElementsFilled;
    /* 0x08 */ s32 numElementsAlloced;
    /* 0x0C */ s32 entrySize;
};

/* rwSListCreate. */
RwSList *fn_8047DC90(s32 entrySize, u32 hint) {
    RwSList *list = (RwSList *)RWSRCGLOBAL(rwmalloc)(sizeof(RwSList), 0x30000);

    if (list == 0) {
        R5ERROR(1, (0x80000013, sizeof(RwSList)));
        return 0;
    }
    list->numElementsFilled = 0;
    list->numElementsAlloced = 20;
    list->entrySize = entrySize;
    list->entries = (u8 *)RWSRCGLOBAL(rwmalloc)(list->numElementsAlloced * list->entrySize, hint | 0x1000000);
    if (list->entries == 0) {
        R5ERROR(1, (0x80000013, list->numElementsAlloced * list->entrySize));
        RWSRCGLOBAL(rwfree)(list);
        return 0;
    }
    return list;
}

/* rwSListEmpty. */
RwSList *fn_8047DDB0(RwSList *list) {
    list->numElementsFilled = 0;
    return list;
}

/* rwSListGetNewEntry: grows the array by a quarter when it is full. */
void *fn_8047DDBC(RwSList *list, u32 hint) {
    if (list->numElementsFilled >= list->numElementsAlloced) {
        u8 *entries = (u8 *)RWSRCGLOBAL(rwrealloc)(
            list->entries, list->entrySize * (list->numElementsAlloced + list->numElementsAlloced / 4), hint);

        if (entries == 0) {
            R5ERROR(1, (0x80000013, list->entrySize * (list->numElementsAlloced + list->numElementsAlloced / 4)));
            return 0;
        }
        list->entries = entries;
        list->numElementsAlloced += list->numElementsAlloced / 4;
    }
    {
        u8 *entry = list->entries + list->numElementsFilled * list->entrySize;

        list->numElementsFilled++;
        return entry;
    }
}

/* rwSListGetNewEntries. */
void *fn_8047DEA0(RwSList *list, s32 count, u32 hint) {
    if (list->numElementsFilled + count >= list->numElementsAlloced) {
        u8 *entries = (u8 *)RWSRCGLOBAL(rwrealloc)(
            list->entries, list->entrySize * (list->numElementsAlloced + count + list->numElementsAlloced / 4), hint);

        if (entries == 0) {
            R5ERROR(1, (0x80000013,
                        list->entrySize * (list->numElementsAlloced + count + list->numElementsAlloced / 4)));
            return 0;
        }
        list->entries = entries;
        list->numElementsAlloced = list->numElementsAlloced + count + list->numElementsAlloced / 4;
    }
    {
        u8 *entry = list->entries + list->numElementsFilled * list->entrySize;

        list->numElementsFilled += count;
        return entry;
    }
}

/* rwSListDestroyEndEntries. */
RwSList *fn_8047DF9C(RwSList *list, s32 count) {
    list->numElementsFilled -= count;
    return list;
}

/* rwSListGetArray. */
void *fn_8047DFAC(RwSList *list) {
    if (list->numElementsFilled != 0) {
        return list->entries;
    }
    return 0;
}

/* rwSListDestroy. */
RwBool fn_8047DFC8(RwSList *list) {
    if (list->entries != 0 && list->numElementsAlloced != 0) {
        RWSRCGLOBAL(rwfree)(list->entries);
        list->entries = 0;
        list->numElementsAlloced = 0;
    }
    RWSRCGLOBAL(rwfree)(list);
    return 1;
}

/* rwSListGetNumEntries. */
s32 fn_8047E03C(RwSList *list) {
    return list->numElementsFilled;
}

/* rwSListToArray: frees the list and hands its entries over. */
void *fn_8047E044(RwSList *list) {
    void *array = 0;

    if (list->numElementsFilled != 0) {
        array = list->entries;
    } else if (list->entries != 0) {
        RWSRCGLOBAL(rwfree)(list->entries);
        list->entries = 0;
        array = 0;
    }
    RWSRCGLOBAL(rwfree)(list);
    return array;
}

/* rwSListGetEntry. */
void *fn_8047E0CC(RwSList *list, s32 index) {
    return list->entries + list->entrySize * index;
}

/* rwSListGetBegin. */
void *fn_8047E0E0(RwSList *list) {
    return list->entries;
}

/* rwSListGetEnd. */
void *fn_8047E0E8(RwSList *list) {
    return list->entries + list->entrySize * list->numElementsFilled;
}
