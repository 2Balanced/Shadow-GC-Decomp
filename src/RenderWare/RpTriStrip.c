/* RenderWare 3.7 world plugin: the mesh optimiser (triangle strips and lists), 0x8045CA38-0x8045F2A0
 * (wave 41 re-cut of the size-cut Game/RwCpp22 tail and Game/RwCpp23 head). The file owns the
 * strip method record (.data 0x8056D868: callback fn_8045EF3C and its data) and one integer
 * constant (.sdata2 0x805FB170). It reads the mesh module record of the mesh file in front of it
 * and calls that file's allocation helper, which the mesh reader expands. The world-sector file
 * behind it starts with its module open/close.
 *
 * Names are inferred from behaviour: fn_8045CA38 is the qsort comparator of the strip builder,
 * fn_8045EF3C RpBuildMeshGenerateDefaultTriStrip, fn_8045EF64 the triangle-list builder and
 * fn_8045F1FC _rpMeshOptimise (strip callback or list builder, then the build mesh is destroyed).
 * Not written yet: fn_8045D898 and fn_8045E904 (the strip builder). Most functions are still
 * untyped drafts. Built with cflags_rw_r3 (configure.py): C, functions in definition order.
 * The integer constant lbl_805FB170 is defined behind its users so that they load it, as retail
 * does; written as a const scalar it still comes out in .sdata instead of .sdata2. */
#include <dolphin/types.h>
#include "Game/rw/r3.h"

/* The untyped functions below still index the engine block by offset. */
#define RWENGINEBYTES ((u8 *)lbl_805F15B4)

extern RwModuleInfo lbl_805F14F0; /* mesh module record (mesh file) */

void *memcpy(void *destination, const void *source, u32 size);
u32 *fn_8045BF30(u32 size);
u32 fn_8045C26C(u32 *object);
u32 *fn_8045C4E4(u32 *object, u32 (*callback)(u8 *, u32 *, u32), u32 argument);
void fn_803AB44C(u32 **, u32 *, u32); /* qsort */
u32 *fn_8047FBD8(u32 size, u32 count, u32 alignment, u32 tag);
void fn_8045E904(u32, u32, u32);
void fn_8045EF3C(u32 first);
u32 *fn_8045EF64(u32 *object, u32 value);

/* The empty mesh header returned for a build mesh without triangles, then the strip method:
 * its callback and data. */
static u32 lbl_8056D868[6] = { 0, 0, 0, 0, (u32)fn_8045EF3C, 0 };

extern const u32 lbl_805FB170;

typedef struct TriStripEdgeRecord {
    u16 first;
    u16 second;
    u16 third;
    u16 padding;
    u32 value;
    u16 fourth;
    u16 fifth;
    u16 sixth;
    u16 seventh;
} TriStripEdgeRecord;

static inline u32 *TriStripLinkEdge(u32 **head, u32 **slot, u32 *allocator, u16 key_a, u16 key_b) {
    u32 *record = *head;
    while (record != NULL) {
        if (key_a == *(u16 *)((u8 *)record + 2) && key_b == *(u16 *)record && record[2] == 0) {
            ((u8 *)record[1])[32]++;
            ((u8 *)slot[0])[32]++;
            record[2] = (u32)slot[0];
            return record;
        }
        record = (u32 *)record[3];
    }
    record = ((u32 *(*)(u32 *, u32))*(u32 *)(RWENGINEBYTES + 0x118))(allocator, 0x00030502);
    *(u16 *)record = key_a;
    *(u16 *)((u8 *)record + 2) = key_b;
    record[1] = (u32)slot[0];
    record[2] = 0;
    record[3] = (u32)*head;
    *head = record;
    return record;
}

s32 fn_8045CA38(u32 **left_argument, u32 **right_argument) {
    s32 left_value = 0;
    s32 right_value = 0;
    u32 *left_child = (u32 *)(*left_argument)[2];
    u32 *right_child = (u32 *)(*right_argument)[2];
    if (left_child == right_child) {
        return 0;
    }
    if (left_child != NULL) {
        u32 *nested = (u32 *)left_child[0];
        if (nested != NULL) {
            s32 type = (*(u8 *)(*(u32 *)nested + 35) & 0xF) << 8;
            if (type == 0x100 || type == 0x300 || type == 0x500) {
                left_value = lbl_805FB170;
            }
        }
        if (((u8 *)left_child)[7] != 0xFF) {
            left_value |= lbl_805FB170;
        }
    }
    if (right_child != NULL) {
        u32 *nested = (u32 *)right_child[0];
        if (nested != NULL) {
            s32 type = (*(u8 *)(*(u32 *)nested + 35) & 0xF) << 8;
            if (type == 0x100 || type == 0x300 || type == 0x500) {
                right_value = lbl_805FB170;
            }
        }
        if (((u8 *)right_child)[7] != 0xFF) {
            right_value |= lbl_805FB170;
        }
    }
    left_value |= (*(u16 *)((u8 *)*left_argument + 16) > *(u16 *)((u8 *)*right_argument + 16)) ? 8 : 0;
    right_value |= (*(u16 *)((u8 *)*left_argument + 16) < *(u16 *)((u8 *)*right_argument + 16)) ? 8 : 0;
    left_value |= (*(u16 *)((u8 *)*left_argument + 18) > *(u16 *)((u8 *)*right_argument + 18)) ? 4 : 0;
    right_value |= (*(u16 *)((u8 *)*left_argument + 18) < *(u16 *)((u8 *)*right_argument + 18)) ? 4 : 0;
    left_value |= (*(u16 *)((u8 *)*left_argument + 14) > *(u16 *)((u8 *)*right_argument + 14)) ? 2 : 0;
    right_value |= (*(u16 *)((u8 *)*left_argument + 14) < *(u16 *)((u8 *)*right_argument + 14)) ? 2 : 0;
    left_value |= (*(u16 *)((u8 *)*left_argument + 12) > *(u16 *)((u8 *)*right_argument + 12)) ? 1 : 0;
    right_value |= (*(u16 *)((u8 *)*left_argument + 12) < *(u16 *)((u8 *)*right_argument + 12)) ? 1 : 0;
    return left_value - right_value;
}

u32 *fn_8045CBD4(u32 *object) {
    u32 size;
    u32 *bitset;
    u32 groups;
    u16 *data;
    u32 count = object[1];
    data = (u16 *)object[0];
    groups = count / 3;
    {
    u32 bits = 0;
    u32 i = 0;
    while (i < count) {
        bits |= data[i];
        i++;
    }
    size = (bits + 31) >> 3;
    bitset = ((u32 *(*)(u32, u32))*(u32 *)(RWENGINEBYTES + 0x108))(size, 0x00010502);
    if (bitset != NULL) {
        u32 copy_size = object[1] * 2;
        u16 *copy = ((u16 *(*)(u32, u32))*(u32 *)(RWENGINEBYTES + 0x108))(copy_size, 0x00010502);
        if (copy != NULL) {
            memcpy(copy, data, copy_size);
            {
            u32 clear;
            u16 *source;
            u32 processed = 0;
            while (processed < groups) {
                clear = size;
                {
                u8 *clear_pointer = (u8 *)bitset;
                while (clear-- != 0) {
                    *clear_pointer++ = 0;
                }
                source = copy;
                { u32 j; for (j = 0; j < groups; j++) {
                    s32 first = source[0];
                    s32 second = source[1];
                    s32 third = source[2];
                    s32 mask;
                    if ((first & second & third) == 0xFFFF) {
                        goto next;
                    }
                    mask = 1 << (first & 31);
                    if (bitset[first >> 5] & mask) {
                        goto next;
                    }
                    if (bitset[second >> 5] & (1 << (second & 31))) {
                        goto next;
                    }
                    if (bitset[third >> 5] & (1 << (third & 31))) {
                        goto next;
                    }
                    bitset[first >> 5] |= mask;
                    bitset[source[1] >> 5] |= 1 << (source[1] & 31);
                    bitset[source[2] >> 5] |= 1 << (source[2] & 31);
                    processed++;
                    data[0] = source[0];
                    data[1] = source[1];
                    data[2] = source[2];
                    data += 3;
                    source[2] = 0xFFFF;
                    source[1] = 0xFFFF;
                    source[0] = 0xFFFF;
next:
                    source += 3;
                }}
            }}
            ((void (*)(u16 *))*(u32 *)(RWENGINEBYTES + 0x10C))(copy);
        }}
        ((void (*)(u32 *))*(u32 *)(RWENGINEBYTES + 0x10C))(bitset);
        return object;
    }
    return object;
}}

u32 *fn_8045CED8(u32 count, u32 *output, u32 **head, u8 *input) {
    u32 *slots = ((u32 *(*)(u32, u32))*(u32 *)(RWENGINEBYTES + 0x108))(count * 4, 0x00030502);
    output[0] = (u32)fn_8047FBD8(36, count, 4, 0x00030502);
    output[1] = (u32)fn_8047FBD8(16, count / 2 + 1, 4, 0x00030502);
    {
    u32 *slot = slots;
    u8 *current = input;
    u32 index = 0;
    while (index < count) {
        slot[0] = (u32)((u32 *(*)(u32 *, u32))*(u32 *)(RWENGINEBYTES + 0x118))(
            (u32 *)output[0], 0x00030502);
        ((u8 *)slot[0])[32] = 0;
        ((u32 *)slot[0])[0] = index;
        ((u32 *)slot[0])[5] = 0;
        ((u32 *)slot[0])[4] = 0;
        ((u32 *)slot[0])[6] = 0;
        ((u32 *)slot[0])[1] = (u32)TriStripLinkEdge(head, (u32 **)slot, (u32 *)output[1],
                                                        *(u16 *)current, *(u16 *)(current + 2));
        ((u32 *)slot[0])[2] = (u32)TriStripLinkEdge(head, (u32 **)slot, (u32 *)output[1],
                                                        *(u16 *)(current + 2), *(u16 *)(current + 4));
        ((u32 *)slot[0])[3] = (u32)TriStripLinkEdge(head, (u32 **)slot, (u32 *)output[1],
                                                        *(u16 *)(current + 4), *(u16 *)current);
        slot++;
        current += 20;
        index++;
    }
    return slots;
}}

void fn_8045D20C(u32 *object, u32 **array, s32 index) {
    if (index < 4) {
        object[7] = 1;
        return;
    }
    object[6] = 1;
    {
    u32 *head = array[((u8 *)object)[32]];
    if (head == object) {
        array[((u8 *)object)[32]] = (u32 *)head[4];
        if (array[((u8 *)object)[32]] != NULL) {
            array[((u8 *)object)[32]][5] = 0;
        }
    } else {
        if (object[4] != 0) {
            ((u32 *)object[4])[5] = object[5];
        }
        if (object[5] != 0) {
            ((u32 *)object[5])[4] = object[4];
        }
    }
    { u8 i; for (i = 0; i < 3; i++) {
        u32 *child;
        u32 *candidate = NULL;
        u32 *container = (u32 *)object[i + 1];
        child = (u32 *)container[1];
        if (child != NULL && child != object && (s32)child[6] == 0) {
            candidate = child;
        } else {
            child = (u32 *)container[2];
            if (child != NULL && (s32)child[6] == 0) {
                candidate = child;
            }
        }
        if (candidate != NULL) {
            head = array[((u8 *)candidate)[32]];
            if (head == candidate) {
                array[((u8 *)candidate)[32]] = (u32 *)head[4];
                if (array[((u8 *)candidate)[32]] != NULL) {
                    array[((u8 *)candidate)[32]][5] = 0;
                }
            } else {
                if (candidate[4] != 0) {
                    ((u32 *)candidate[4])[5] = candidate[5];
                }
                if (candidate[5] != 0) {
                    ((u32 *)candidate[5])[4] = candidate[4];
                }
            }
            ((u8 *)candidate)[32]--;
            candidate[4] = (u32)array[((u8 *)candidate)[32]];
            if (candidate[4] != 0) {
                ((u32 *)candidate[4])[5] = (u32)candidate;
            }
            array[((u8 *)candidate)[32]] = candidate;
            candidate[5] = 0;
        }
    }}
}}

u32 fn_8045D3A8(u32 *object, u32 *current, u32 **array, s32 mode) {
    u32 processed = 0;
    u32 *alternate = NULL;
    while (current != NULL) {
        u32 *candidate = NULL;
        s32 index = -1;
        if (mode < 4) {
            if (current[1] != 0 && (s32)((u32 *)current[1])[7] == 0) {
                candidate = (u32 *)current[1];
            } else if (current[2] != 0 && (s32)((u32 *)current[2])[7] == 0) {
                candidate = (u32 *)current[2];
            }
        } else {
            if (current[1] != 0 && (s32)((u32 *)current[1])[6] == 0) {
                candidate = (u32 *)current[1];
            } else if (current[2] != 0 && (s32)((u32 *)current[2])[6] == 0) {
                candidate = (u32 *)current[2];
            }
        }
        if (candidate == NULL) {
            return processed;
        }
        processed++;
        fn_8045D20C(candidate, array, mode);
        if ((u32 *)candidate[1] == current) {
            index = 0;
        } else if ((u32 *)candidate[2] == current) {
            index = 1;
        } else if ((u32 *)candidate[3] == current) {
            index = 2;
        }
        {
        s32 next_index = (index + 1) % 3;
        u32 *link = (u32 *)candidate[next_index + 1];
        u16 key = (u32 *)link[1] == candidate ? *(u16 *)((u8 *)link + 2) : *(u16 *)link;
        u16 *output = (u16 *)object[0] + object[1];
        u16 last = output[-1];
        u32 *next = NULL;
        if ((*(u16 *)candidate[1] == last && *(u16 *)((u8 *)candidate[1] + 2) == key) ||
            (*(u16 *)candidate[1] == key && *(u16 *)((u8 *)candidate[1] + 2) == last)) {
            next = (u32 *)candidate[1];
        } else if ((*(u16 *)candidate[2] == last && *(u16 *)((u8 *)candidate[2] + 2) == key) ||
                   (*(u16 *)candidate[2] == key && *(u16 *)((u8 *)candidate[2] + 2) == last)) {
            next = (u32 *)candidate[2];
        } else if ((*(u16 *)candidate[3] == last && *(u16 *)((u8 *)candidate[3] + 2) == key) ||
                   (*(u16 *)candidate[3] == key && *(u16 *)((u8 *)candidate[3] + 2) == last)) {
            next = (u32 *)candidate[3];
        }
        {
        s32 no_valid = 1;
        s32 other = no_valid;
        if (next != NULL) {
            s32 second_valid = 0;
            s32 first_valid = 0;
            if (mode < 4) {
                if (next[2] != 0 && (s32)((u32 *)next[2])[7] == 0) {
                    second_valid = 1;
                }
                if (next[1] != 0 && (s32)((u32 *)next[1])[7] == 0) {
                    first_valid = 1;
                }
            } else {
                if (next[2] != 0 && (s32)((u32 *)next[2])[6] == 0) {
                    second_valid = 1;
                }
                if (next[1] != 0 && (s32)((u32 *)next[1])[6] == 0) {
                    first_valid = 1;
                }
            }
            if (first_valid + second_valid != 0) {
                other = 0;
            }
        }
        if (other == 0) {
            no_valid = 0;
        }
        if (!no_valid) {
            output[0] = key;
            object[1]++;
            current = next;
            continue;
        }
        if ((u32 *)candidate[1] != current && (u32 *)candidate[1] != next) {
            alternate = (u32 *)candidate[1];
        } else if ((u32 *)candidate[2] != current && (u32 *)candidate[2] != next) {
            alternate = (u32 *)candidate[2];
        } else if ((u32 *)candidate[3] != current && (u32 *)candidate[3] != next) {
            alternate = (u32 *)candidate[3];
        }
        {
        s32 any_valid = 0;
        other = any_valid;
        if (alternate != NULL) {
            s32 second_valid = 0;
            s32 first_valid = 0;
            if (mode < 4) {
                if (alternate[2] != 0 && (s32)((u32 *)alternate[2])[7] == 0) {
                    second_valid = 1;
                }
                if (alternate[1] != 0 && (s32)((u32 *)alternate[1])[7] == 0) {
                    first_valid = 1;
                }
            } else {
                if (alternate[2] != 0 && (s32)((u32 *)alternate[2])[6] == 0) {
                    second_valid = 1;
                }
                if (alternate[1] != 0 && (s32)((u32 *)alternate[1])[6] == 0) {
                    first_valid = 1;
                }
            }
            if (first_valid + second_valid != 0) {
                other = 1;
            }
        }
        if (other != 0) {
            any_valid = 1;
        }
        if (any_valid) {
            if (next_index != 0) {
                output[0] = key;
                current = NULL;
                object[1]++;
            } else {
                output[0] = output[-1];
                current = alternate;
                object[1]++;
                output[1] = key;
                object[1]++;
            }
        } else {
            output[0] = key;
            current = NULL;
            object[1]++;
        }
    }}}}
    return processed;
}
/* fn_8045D898: not written */

s32 fn_8045CA38(u32 **left_argument, u32 **right_argument);
u32 *fn_8045CBD4(u32 *object);

static inline void TriStripFreeNode(u32 *node) {
    ((void (*)(u32 *, u32 *))*(u32 *)(RWENGINEBYTES + 0x11C))(
        *(u32 **)(RWENGINEBYTES + lbl_805F14F0.globalsOffset + 4), node);
}

static inline void TriStripFree(u32 *node) {
    ((void (*)(u32 *))*(u32 *)(RWENGINEBYTES + 0x10C))(node);
}

u32 *fn_8045E51C(u32 *owner, u32 mode) {
    u32 *current;
    u32 *next;
    u32 *node;
    u32 count;
    u16 *output;
    if (owner[0] == 0) {
        return NULL;
    }
    {
    u32 *object = ((u32 *(*)(u32 *, u32))*(u32 *)(RWENGINEBYTES + 0x118))(
        *(u32 **)(RWENGINEBYTES + lbl_805F14F0.globalsOffset + 4), 0x00030502);
    object[1] = 0;
    object[2] = 0;
    for (current = owner; current != NULL; current = (u32 *)current[3]) {
        object[2] += current[2] + 6;
    }
    object[0] = (u32)((u32 *(*)(u32, u32))*(u32 *)(RWENGINEBYTES + 0x108))(object[2] * 4, 0x00030502);
    count = 0;
    for (current = owner; current != NULL; current = (u32 *)current[3]) {
        ((u16 *)object[0])[count] = *(u16 *)current;
        count++;
        TriStripFreeNode(current);
    }
    next = (u32 *)owner[3];
    while (next != NULL) {
        current = next;
stage1:
        if (current == NULL) {
            goto stage2_reset;
        }
        count = object[1];
        output = (u16 *)object[0] + count;
        if (output[-1] == *(u16 *)current && (count & 1) != 0 && mode != 0) {
            output[0] = output[-1];
            object[1]++;
            goto stage2_reset;
        }
        current = (u32 *)current[3];
        goto stage1;
stage2_reset:
        if (current != NULL) {
            goto final_loop;
        }
        current = next;
stage2_condition:
        if (current != NULL) {
            goto stage2_body;
        }
        goto stage3_body;
stage2_body:
        count = object[1];
        output = (u16 *)object[0] + count;
        if (output[-2] == *(u16 *)current && (count & 1) == 0 && mode != 0) {
            output[0] = output[-2];
            output[1] = *(u16 *)current;
            object[1] += 2;
            goto final_loop;
        }
        output[0] = output[-2];
        object[1]++;
        goto final_loop;
stage3_body:
        count = object[1];
        output = (u16 *)object[0] + count;
        output[0] = output[-1];
        if ((count & 1) == 0 || mode == 0) {
            output[1] = *(u16 *)next;
            object[1] += 2;
        } else {
            output[1] = *(u16 *)next;
            output[2] = *(u16 *)next;
            object[1] += 3;
        }
final_loop:
        while (current != NULL) {
            count = object[1];
            output = (u16 *)object[0] + count;
            output[0] = *(u16 *)current;
            object[1]++;
            TriStripFreeNode(current);
            current = (u32 *)current[3];
        }
        TriStripFree(current);
        current[0] = 0;
        if (next == current) {
            next = (u32 *)current[3];
            TriStripFreeNode(current);
        } else {
            node = next;
            while ((u32 *)node[3] != current) {
                node = (u32 *)node[3];
            }
            node[3] = current[3];
            TriStripFreeNode(current);
        }
    }
    owner[0] = (u32)object;
    object[3] = 0;
    return (u32 *)1;
}}
/* fn_8045E904: not written */

void fn_8045EF3C(u32 first) {
    fn_8045E904(first, 0, 1);
}

u32 *fn_8045EF64(u32 *input, u32) {
    u32 count = input[1];
    u32 *array = ((u32 *(*)(u32, u32))*(u32 *)(RWENGINEBYTES + 0x108))(count * 4, 0x00010502);
    if (array == NULL) {
        goto initial_error;
    }
    {
    u32 i = 0;
    u32 offset = 0;
    u32 *destination = array;
    do {
        destination[i] = input[2] + offset;
        i++;
        offset += 20;
    } while (i < count);
    fn_803AB44C((u32 **)array, (u32 *)fn_8045CA38, 4);
    {
    u32 groups = 1;
    if (count >= 2) {
        u32 previous = ((u32 *)array[0])[2];
        { u32 i; for (i = 1; i < count; i++) {
            u32 current = ((u32 *)array[i])[2];
            if (current != previous) {
                previous = current;
                groups++;
            }
        }}
    }
    {
    u32 total = groups * 12 + count * 6 + 16;
    u32 *object = fn_8045BF30(total);
    if (object == NULL) {
        ((void (*)(u32 *))*(u32 *)(RWENGINEBYTES + 0x10C))(array);
        {
        u32 error[2];
        error[0] = 2;
        error[1] = fn_8047DC28(0x80000013, total);
        fn_8047DB84((RwError *)error);
        return NULL;
    }}
    object[0] = 0;
    *(u16 *)((u8 *)object + 4) = 1;
    *(s16 *)((u8 *)object + 6) = *(s16 *)(RWENGINEBYTES + lbl_805F14F0.globalsOffset);
    object[3] = 0;
    object[2] = count * 3;
    (*(s16 *)(RWENGINEBYTES + lbl_805F14F0.globalsOffset))++;
    object[5] = 0;
    object[6] = ((u32 *)array[0])[2];
    {
    u32 *group = object + 4;
    u16 *values = (u16 *)((u8 *)group + groups * 12);
    object[4] = (u32)values;
    { u32 i; for (i = 1; i < count; i++) {
        u32 *record = (u32 *)array[i - 1];
        if (record[2] != group[2]) {
            group[4] = 0;
            group[5] = record[2];
            group += 3;
            (*(u16 *)((u8 *)object + 4))++;
        }
        *(u16 *)values = *(u16 *)record;
        *(u16 *)(values + 1) = *(u16 *)((u8 *)record + 2);
        *(u16 *)(values + 2) = *(u16 *)((u8 *)record + 4);
        values += 3;
        group[1] += 3;
    }}
    fn_8045C4E4(object, (u32 (*)(u8 *, u32 *, u32))fn_8045CBD4, 0);
    ((void (*)(u32 *))*(u32 *)(RWENGINEBYTES + 0x10C))(array);
    return object;
initial_error:
    {
        u32 error[2];
        error[0] = 2;
        error[1] = fn_8047DC28(0x80000013, count * 4);
        fn_8047DB84((RwError *)error);
        return NULL;
    }
}}}}}

u32 fn_8045F2C8(u32 argument);
u32 fn_804566D0(u32 *node);
s32 fn_804856C8(u32, u32);

u32 *fn_8045F1FC(u32 *object, u32 mode) {
    if (object != NULL) {
        u32 *result;
        u32 *(*callback)(u32 *, u32);
        {
        u32 argument;
        if (object[1] == 0) {
            fn_8045C26C(object);
            return (u32 *)lbl_8056D868;
        }
        if ((mode & 1) != 0) {
            callback = (u32 *(*)(u32 *, u32))lbl_8056D868[4];
            argument = lbl_8056D868[5];
        } else {
            callback = fn_8045EF64;
            argument = 0;
        }
        result = callback(object, argument);
        if (result != NULL) {
            fn_8045C26C(object);
            return result;
        }
    }}
    return NULL;
}

/* Defined behind its users, so they load it rather than folding it. */
const u32 lbl_805FB170 = 0x10;
