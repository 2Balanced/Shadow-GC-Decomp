/* RenderWare 3.7 world plugin: worlds, 0x8045F9E8-0x80460974 (wave 41 re-cut of the size-cut
 * Game/RwCpp24). The file owns the world plugin registry (.data 0x8056D898; the binary-world reader
 * uses it too, so it keeps its address name), the world free list (.bss 0x805E1618), the world
 * module record (.sbss 0x805F1500) and one literal (.sdata2 0x805FB178, 0.0f plus padding).
 *
 * Names are inferred from behaviour and the public RenderWare API:
 *   fn_8045F9E8 / fn_8045FA50  world module close / open (plugin 0x507)
 *   fn_8045FB10  default world-sector render callback (instance pipeline)
 *   fn_8045FBA4  spread the world's bounding box down the sector tree
 *   fn_8045FFA0  RpWorldDestroy   fn_80460120  RpWorldSetSectorRenderCallBack
 *   fn_80460138  RpWorldCreate    fn_80460790 / fn_804607D0  RpWorldRegisterPlugin(Stream)
 *   fn_8046080C  RpWorldPluginAttach
 * RpWorldDestroy frees the sectors' meshes through an inline function (WorldDestroySectorMeshes):
 * written in place, GC/2.7 gives the walk's zero its own li where retail copies the zeroed counter.
 * Status: fn_8045FBA4 (the box copies take other registers) and fn_80460138 (RpWorldCreate, a
 * draft) are left. Most functions are still untyped drafts. Built with cflags_rw_r3 (configure.py);
 * cflags_rw_c gives the same code. */
#include <dolphin/types.h>
#include "Game/rw/r3.h"

void *memset(void *, int, u32);
void *memcpy(void *, const void *, u32);

/* The untyped functions below still index the engine block by offset. */
#define RWENGINEBYTES ((u8 *)lbl_805F15B4)

/* The plugin registry of worlds (0x70 bytes each). */
RwPluginRegistry lbl_8056D898 = { sizeof(RpWorld) + 0x10, sizeof(RpWorld) + 0x10, 0, 0, NULL, NULL };

/* The world free list. */
static RwFreeList worldFreeList;

/* The module record (plugin 0x507). */
static RwModuleInfo worldModule;

extern RwPluginRegistry lbl_8056D880; /* world-sector plugin registry (world-sector file) */

s32 fn_80462798(void);
extern u32 lbl_805F1690;
u32 *fn_8049B674(u32 *, u32 *, u32);
u32 *fn_8045C144(u32, u32);
u32 *fn_8045F458(u32 *, u32 *, u32 *, u32 *);
u32 *fn_804606F0(u32 *object, u32 (*callback)(u32 *, u32 *), u32 *argument);
u32 fn_8045F374(u32 value, u32 *object);
extern u32 lbl_805F1518;
u32 *fn_8045F390(u32 *object);
void fn_804624E8(u8 *, u32, u32 *, u32);
void fn_80462130(u32 *, u32 *);
void fn_8047FCDC(u32 *table);
void fn_80462770(void);
void fn_8045C2D4(u32 *);
void fn_8045B6F0(u32 *);
void fn_80482144(u8 *, u32 *);
void fn_8045F6E0(u32 *);
void fn_8045F7D0(void *);
void fn_8045B788(u32 *);
void fn_8048209C(u8 *, u32 *);
void fn_80481DC8(u8 *, u32, u32, u32, u32, u32);
extern u32 lbl_805F1508;
void fn_804815F0(u8 *, u32, u32, u32, u32);
s32 fn_80485D78(u32, u32, void *, void *);
s32 fn_80461E20(void);
s32 fn_80456B48(void);
s32 fn_8046318C(void);
void fn_8045B048(void);
void fn_8045B0D8(void);
void fn_8045BFBC(void);
void fn_8045BF64(void);
void fn_80458CF8(void);
void fn_80458D10(void);
void fn_80456A50(void);
void fn_80456994(void);
void fn_8045AE98(void);
void fn_8045AE20(void);
void fn_8045F2A0(void);
void fn_8045F2B4(void);
u32 *fn_8045FA50(u32 *, u32);
void fn_80455CF8(void);
void fn_80455CE4(void);

u32 *fn_8045F9E8(u32 *object) {
    u32 *table = *(u32 **)(RWENGINEBYTES + worldModule.globalsOffset);
    if (table != NULL) {
        fn_8047FCDC(table);
        *(u32 **)(RWENGINEBYTES + worldModule.globalsOffset) = NULL;
    }
    fn_80462770();
    worldModule.numInstances--;
    return object;
}

u32 *fn_8045FA50(u32 *object, u32 offset) {
    worldModule.globalsOffset = offset;
    if (fn_80462798() == 0) {
        return NULL;
    }
    *(u32 **)(RWENGINEBYTES + worldModule.globalsOffset) =
        (u32 *)fn_8047FC04(16, 8, 4, 1, (u32)&worldFreeList, 0x4000B);
    {
    u32 *table = (u32 *)(RWENGINEBYTES + worldModule.globalsOffset);
    if (table[0] == 0) {
        return NULL;
    }
    *(u32 *)(RWENGINEBYTES + worldModule.globalsOffset + 4) = (u32)(RWENGINEBYTES + worldModule.globalsOffset + 4);
    *(u32 *)(RWENGINEBYTES + worldModule.globalsOffset + 8) = (u32)(RWENGINEBYTES + worldModule.globalsOffset + 4);
    worldModule.numInstances++;
    return object;
}}

u32 *fn_8045FB10(u32 *object) {
    if (((u16 *)object)[66] == 0) {
        return object;
    }
    {
    u32 *callback;
    if (object[31] != 0) {
        callback = (u32 *)object[31];
    } else {
        u32 *engine = *(u32 **)(RWENGINEBYTES + 4);
        if (engine[27] != 0) {
            callback = (u32 *)engine[27];
        } else {
            callback = *(u32 **)(RWENGINEBYTES + lbl_805F1690 + 64);
        }
    }
    if (fn_8049B674(callback, object, 1) != 0) {
        return object;
    }
    return NULL;
}}

/* Spreads the world's bounding box down its sector tree: each plane splits its box at its left and
 * right values, each world sector gets the box of its leaf. */
RpWorld *fn_8045FBA4(RpWorld *world) {
    RwBBox bbox;
    RpSector *stack[64];
    RwBBox bboxStack[64];
    s32 depth = 0;
    RpSector *sector;

    bbox = world->boundingBox;
    sector = world->rootSector;
    while (depth >= 0) {
        if (sector->type < 0) {
            ((RpWorldSector *)sector)->boundingBox = bbox;
            bbox = bboxStack[depth];
            sector = stack[depth--];
        } else {
            RpPlaneSector *plane = (RpPlaneSector *)sector;
            depth++;
            bboxStack[depth] = bbox;
            stack[depth] = plane->rightSubTree;
            *(f32 *)((u8 *)&bboxStack[depth].inf + plane->type) = plane->rightValue;
            *(f32 *)((u8 *)&bbox.sup + plane->type) = plane->leftValue;
            sector = plane->leftSubTree;
        }
    }
    return world;
}

void fn_8045FCFC(u32 first, u32 second) {
    u32 *node = ((u32 *(*)(u32 *, u32))*(u32 *)(RWENGINEBYTES + 0x118))(
        *(u32 **)(RWENGINEBYTES + worldModule.globalsOffset), 0x40507);
    if (node != NULL) {
        node[0] = first;
        node[1] = second;
        node[2] = *(u32 *)(RWENGINEBYTES + worldModule.globalsOffset + 4);
        node[3] = (u32)(RWENGINEBYTES + worldModule.globalsOffset + 4);
        *(u32 *)(*(u32 *)(RWENGINEBYTES + worldModule.globalsOffset + 4) + 4) = (u32)(node + 2);
        *(u32 **)(RWENGINEBYTES + worldModule.globalsOffset + 4) = node + 2;
    }
}

u32 *fn_8045FDB0(u32 *object) {
    volatile u32 buffer[66];
    u32 *node = (u32 *)object[7];
    s32 index = 0;
    if (node == NULL) {
        return NULL;
    }
    do {
        if (*(s32 *)node < 0) {
            u32 *slot = (u32 *)(object[4] + ((u16 *)node)[64] * 4);
            if (node[30] == 0) {
                u32 *value = fn_8045C144(((u16 *)node)[66], object[4]);
                if (value == NULL) {
                    goto value_failed;
                }
                object = fn_8045F458(object, value, node, slot);
                if (object != NULL) {
                    goto pop_node;
                }
                return NULL;
value_failed:
                return NULL;
            }
pop_node:
            node = (u32 *)buffer[index--];
        } else {
            u32 child = node[3];
            node = *(u32 *volatile *)(node + 2);
            buffer[++index] = child;
        }
    } while (index >= 0);
    return object;
}

u32 *fn_8045FEA0(u32 target) {
    u32 *sentinel = (u32 *)(RWENGINEBYTES + worldModule.globalsOffset + 4);
    u32 *current = (u32 *)sentinel[0];
    while (current != sentinel) {
        if (((u8 *)current[-2])[3] & 1) {
            if (target < (u32)current[-2] || target >= (u32)((u8 *)current[-2] + current[-1])) {
                goto advance;
            }
            return (u32 *)current[-2];
        }
        {
        s32 search[2];
        search[0] = (s32)target;
        search[1] = 0;
        fn_804606F0((u32 *)current[-2], (u32 (*)(u32 *, u32 *))fn_8045F374, (u32 *)search);
        if (search[1] != 0) {
            return (u32 *)current[-2];
        }
advance:
        current = (u32 *)current[0];
    }}
    return NULL;
}

u32 *fn_8045FF5C(u32 *object, u32 unused_a, u32 unused_b, u32 last) {
    fn_804624E8(*(u8 **)RWENGINEBYTES, (u32)fn_8045F390, object, last);
    return object;
}

/* Frees the mesh of every world sector of a world's sector tree (walked with a stack). */
static inline void WorldDestroySectorMeshes(u32 *object) {
    u32 buffer[62];
    u32 *node = (u32 *)object[7];
    s32 index = 0;
    if (node != NULL) {
        do {
            if (*(s32 *)node < 0) {
                if (node[30] != 0) {
                    fn_8045C2D4((u32 *)node[30]);
                    node[30] = 0;
                }
                node = (u32 *)buffer[index--];
            } else {
                u32 child = node[3];
                node = (u32 *)node[2];
                buffer[++index] = child;
            }
        } while (index >= 0);
    }
}

u32 fn_8045FFA0(u32 *object) {
    u32 *current;
    u32 *sentinel = (u32 *)(RWENGINEBYTES + worldModule.globalsOffset + 4);
    current = (u32 *)sentinel[0];
    while (current != sentinel) {
        u32 *link = current - 2;
        if (*(u32 volatile *)link == (u32)object) {
            ((u32 *)link[3])[0] = link[2];
            ((u32 *)link[2])[1] = link[3];
            ((void (*)(u32 *, u32 *))*(u32 *)(RWENGINEBYTES + 0x11C))(
                *(u32 **)(RWENGINEBYTES + worldModule.globalsOffset), link);
            break;
        }
        current = (u32 *)current[0];
    }
    WorldDestroySectorMeshes(object);
    fn_8045B6F0(object + 4);
    if (((u8 *)object)[3] & 1) {
        if (object[7] != 0) {
            fn_8045F6E0((u32 *)object[7]);
        }
        fn_80482144((u8 *)&lbl_8056D898, object);
        ((void (*)(u32 *))*(u32 *)(RWENGINEBYTES + 0x10C))(object);
    } else {
        if (object[7] != 0) {
            fn_8045F7D0((u32 *)object[7]);
        }
        fn_80482144((u8 *)&lbl_8056D898, object);
        ((void (*)(u32 *))*(u32 *)(RWENGINEBYTES + 0x10C))(object);
    }
    return 1;
}

void fn_80460120(u32 *object, void (*callback)(void)) {
    if (callback == NULL) {
        callback = (void (*)(void))fn_8045FB10;
    }
    object[0x1A] = (u32)callback;
}

u32 *fn_80460138(u32 *config) {
    u32 errors[4];
    u32 resolve_stack[64];
    u32 cleanup_stack[64];
    u32 *outer = ((u32 *(*)(u32, u32))*(u32 *)(RWENGINEBYTES + 0x108))(
        *(u32 *)&lbl_8056D898, 0x3000B);
    if (outer == NULL) {
        errors[2] = 2;
        errors[3] = fn_8047DC28(0x80000013, *(u32 *)&lbl_8056D898);
        fn_8047DB84((RwError *)(errors + 2));
        return NULL;
    }
    ((u8 *)outer)[0] = 7;
    ((u8 *)outer)[1] = 0;
    ((u8 *)outer)[2] = 0;
    ((u8 *)outer)[3] = 0;
    outer[1] = 0;
    fn_8045B788(outer + 4);
    outer[3] = 2;
    outer[2] = 0;
    {
    u32 *inner = ((u32 *(*)(u32, u32))*(u32 *)(RWENGINEBYTES + 0x108))(
        *(u32 *)&lbl_8056D880, 0x3000B);
    if (inner == NULL) {
        errors[0] = 2;
        errors[1] = fn_8047DC28(0x80000013, 4);
        fn_8047DB84((RwError *)errors);
        ((void (*)(u32 *))*(u32 *)(RWENGINEBYTES + 0x10C))(outer);
        return NULL;
    }
    ((s32 *)inner)[0] = -1;
    inner[13] = 0;
    inner[30] = 0;
    inner[14] = (u32)(inner + 14);
    inner[15] = (u32)(inner + 14);
    inner[16] = (u32)(inner + 16);
    inner[17] = (u32)(inner + 16);
    ((u16 *)inner)[65] = 0;
    ((u16 *)inner)[66] = 0;
    inner[2] = 0;
    inner[1] = 0;
    inner[3] = 0;
    memset(inner + 4, 0, 32);
    inner[12] = 0;
    inner[21] = config[3];
    inner[22] = config[4];
    inner[23] = config[5];
    inner[18] = config[0];
    inner[19] = config[1];
    inner[20] = config[2];
    inner[27] = config[3];
    inner[28] = config[4];
    inner[29] = config[5];
    inner[24] = config[0];
    inner[25] = config[1];
    inner[26] = config[2];
    inner[31] = 0;
    outer[7] = (u32)inner;
    outer[8] = 0;
    {
    f32 outer_value = 0.0f;
    ((f32 *)outer)[19] = outer_value;
    ((f32 *)outer)[18] = outer_value;
    ((f32 *)outer)[17] = outer_value;
    outer[23] = config[3];
    outer[24] = config[4];
    outer[25] = config[5];
    outer[20] = config[0];
    outer[21] = config[1];
    outer[22] = config[2];
    outer[11] = (u32)(outer + 11);
    outer[12] = (u32)(outer + 11);
    outer[9] = 0;
    outer[10] = (u32)(outer + 11);
    outer[13] = (u32)(outer + 13);
    outer[14] = (u32)(outer + 13);
    outer[15] = (u32)(outer + 15);
    outer[16] = (u32)(outer + 15);
    outer[26] = (u32)fn_8045FB10;
    outer[27] = 0;
    {
    u32 config_value = *(u32 *)&lbl_8056D898;
    u32 *link_node = ((u32 *(*)(u32 *, u32))*(u32 *)(RWENGINEBYTES + 0x118))(
        *(u32 **)(RWENGINEBYTES + worldModule.globalsOffset), 0x40507);
    if (link_node != NULL) {
        link_node[0] = (u32)outer;
        link_node[1] = config_value;
        {
        u32 *link = link_node + 2;
        link_node[2] = *(u32 *)(RWENGINEBYTES + worldModule.globalsOffset + 4);
        link_node[3] = (u32)(RWENGINEBYTES + worldModule.globalsOffset + 4);
        *(u32 *)(*(u32 *)(RWENGINEBYTES + worldModule.globalsOffset + 4) + 4) = (u32)link;
        *(u32 **)(RWENGINEBYTES + worldModule.globalsOffset + 4) = link;
    }}
    fn_8048209C((u8 *)&lbl_8056D898, outer);
    fn_8048209C((u8 *)&lbl_8056D880, inner);
    {
    u32 *node = (u32 *)outer[7];
    u32 *resolved = outer;
    s32 index = 0;
    if (node != NULL) {
        do {
            if (*(s32 *)node < 0) {
                u32 *slot = (u32 *)(resolved[4] + ((u16 *)node)[64] * 4);
                if (node[30] == 0) {
                    u32 *value = fn_8045C144(((u16 *)node)[66], resolved[4]);
                    if (value == NULL) {
                        resolved = NULL;
                        break;
                    }
                    resolved = fn_8045F458(resolved, value, node, slot);
                    if (resolved == NULL) {
                        break;
                    }
                }
                node = (u32 *)resolve_stack[index--];
            } else {
                u32 child = node[3];
                node = (u32 *)node[2];
                resolve_stack[++index] = child;
            }
        } while (index >= 0);
    }
    if (resolved == NULL) {
        u32 *current;
        u32 *sentinel = (u32 *)(RWENGINEBYTES + worldModule.globalsOffset + 4);
        current = (u32 *)sentinel[0];
        while (current != sentinel) {
            u32 *link = current - 2;
            if (current[-2] == (u32)outer) {
                ((u32 *)link[3])[0] = link[2];
                ((u32 *)link[2])[1] = link[3];
                ((void (*)(u32 *, u32 *))*(u32 *)(RWENGINEBYTES + 0x11C))(
                    *(u32 **)(RWENGINEBYTES + worldModule.globalsOffset), link);
                break;
            }
            current = (u32 *)current[0];
        }
        node = (u32 *)outer[7];
        index = 0;
        if (node != NULL) {
            u32 zero = index;
            do {
                if (*(s32 *)node < 0) {
                    if (node[30] != 0) {
                        fn_8045C2D4((u32 *)node[30]);
                        node[30] = zero;
                    }
                    node = (u32 *)cleanup_stack[index--];
                } else {
                    u32 child = node[3];
                    node = (u32 *)node[2];
                    cleanup_stack[++index] = child;
                }
            } while (index >= 0);
        }
        fn_8045B6F0(outer + 4);
        if (((u8 *)outer)[3] & 1) {
            if (outer[7] != 0) {
                fn_8045F6E0((u32 *)outer[7]);
            }
            fn_80482144((u8 *)&lbl_8056D898, outer);
            ((void (*)(u32 *))*(u32 *)(RWENGINEBYTES + 0x10C))(outer);
        } else {
            if (outer[7] != 0) {
                fn_8045F7D0((u32 *)outer[7]);
            }
            fn_80482144((u8 *)&lbl_8056D898, outer);
            ((void (*)(u32 *))*(u32 *)(RWENGINEBYTES + 0x10C))(outer);
        }
        return NULL;
    }
    return outer;
}}}}}

u32 *fn_8046066C(u32 *object, u32 (*callback)(u32 *, u32 *), u32 *argument) {
    s32 count = object[5];
    { s32 i; for (i = 0; i < count; i++) {
        if (callback(((u32 **)object[4])[i], argument) == 0) {
            return object;
        }
    }}
    return object;
}

u32 *fn_804606F0(u32 *object, u32 (*callback)(u32 *, u32 *), u32 *argument) {
    u32 buffer[62];
    u32 *node = (u32 *)object[7];
    s32 index = 0;
    do {
        if (*(s32 *)node < 0) {
            if (callback(node, argument) == 0) {
                return object;
            }
            node = (u32 *)buffer[index--];
        } else {
            buffer[++index] = node[3];
            node = (u32 *)node[2];
        }
    } while (index >= 0);
    return object;
}

void fn_80460790(u32 first, u32 second, u32 third, u32 fourth, u32 fifth) {
    fn_80481DC8((u8 *)&lbl_8056D898, first, second, third, fourth, fifth);
}

void fn_804607D0(u32 first, u32 second, u32 third, u32 fourth) {
    fn_804815F0((u8 *)&lbl_8056D898, first, second, third, fourth);
}

u32 fn_8046080C(void) {
    s32 status = fn_80485D78(4, 1281, fn_8045B048, fn_8045B0D8);
    status |= fn_80485D78(48, 1282, fn_8045BFBC, fn_8045BF64);
    status |= fn_80485D78(4, 1283, fn_80458CF8, fn_80458D10);
    status |= fn_80485D78(8, 1284, fn_80456A50, fn_80456994);
    status |= fn_80485D78(4, 1285, fn_8045AE98, fn_8045AE20);
    status |= fn_80485D78(0, 1290, fn_8045F2A0, fn_8045F2B4);
    status |= fn_80485D78(12, 1287, (void *)fn_8045FA50, (void *)fn_8045F9E8);
    status |= fn_80485D78(0, 1291, fn_80455CF8, fn_80455CE4);
    if (status < 0) {
        return 0;
    }
    if (fn_80461E20() == 0) {
        return 0;
    }
    if (fn_80456B48() == 0) {
        return 0;
    }
    return fn_8046318C() != 0;
}
