/* RenderWare object helpers: validate an object against the current pool entry, traverse objects
 * with a cleanup callback, destroy one entry, select an object's payload, and allocate an object.
 * Function and member names are inferred from their behavior. Definitions are in reverse address
 * order because deferred inlining emits the last definition first. */
#include <dolphin/types.h>
#include "size_assert.h"

struct UnkRwObject_8043B810 {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ void *m_data;
};
SIZE_ASSERT(UnkRwObject_8043B810, 8);

struct UnkRwEntry_8043B89C {
    /* 0x00 */ u8 m_used;
    /* 0x01 */ u8 field_0x1[0xF];
    /* 0x10 */ void *m_data;
};
SIZE_ASSERT(UnkRwEntry_8043B89C, 0x14);

extern void *lbl_805F1404;

extern "C" {
void fn_8043B44C(void *, void (*)(void *));
void fn_8043B4B0(void *, void *, void (*)(void *));
void fn_8043C05C(void *);
void *fn_8043B3DC(void *, void *);
void *fn_8043B560(u32, void *, int);
void fn_8043B89C(void *);

UnkRwObject_8043B810 *fn_8043B910(void *object, void *data) {
    UnkRwObject_8043B810 *result = (UnkRwObject_8043B810 *)fn_8043B560(8, object, 0);
    if (result != NULL) {
        result->m_data = data;
    }
    return result;
}

void *fn_8043B8F0(void *object, void *data) {
    return fn_8043B3DC(object, data);
}

void fn_8043B89C(void *object) {
    UnkRwEntry_8043B89C *entry;
    if (object == NULL) {
        return;
    }
    entry = (UnkRwEntry_8043B89C *)((UnkRwObject_8043B810 *)object)->m_data;
    if (entry == NULL) {
        return;
    }
    if (entry->m_data != NULL) {
        fn_8043C05C(&entry->m_data);
    }
    entry->m_used = 0;
}

void fn_8043B868(void *object, void *data) {
    fn_8043B4B0(data, object, fn_8043B89C);
}

void fn_8043B840(void *object) {
    fn_8043B44C(object, fn_8043B89C);
}

int fn_8043B810(UnkRwObject_8043B810 *object) {
    if (object != NULL &&
        object->m_data == *(void **)((u8 *)lbl_805F1404 + 0xFDFC)) {
        return 0;
    }
    return 1;
}
}
