/* RenderWare world plugin, 0x8045C340-0x8045D898: the end of the mesh module that starts in RwCpp21
 * (fn_8045C340 _rpBuildMeshAddTriangle, fn_8045C56C _rpMeshWrite, fn_8045C770 _rpMeshRead), then
 * the triangle-strip builder's helpers (untyped drafts). Names are inferred from behavior.
 * fn_8045C770 has retail's instructions except that GC/2.7 copies the chunk count to r3 before the
 * unrolled index copy (retail unrolls on the count itself); 15 loop spellings were measured
 * (probes/Game_RwCpp22.py). fn_8045CA38 (a qsort comparator) has retail's instruction sequence;
 * GC/2.7 assigns one extra saved register to the final field temporaries. fn_8045D3A8 implements
 * the full graph walk; validity temporaries and output scheduling differ. Still built with the
 * older C++ flags: the mesh functions match under both. */
#include <dolphin/types.h>
#include "Game/rw/r3.h"

/* The untyped functions below still index the engine block by offset. */
#define RWENGINEBYTES ((u8 *)lbl_805F15B4)
extern "C" RwModuleInfo lbl_805F14F0; /* mesh module */
#define lbl_805F14F0_OFFSET ((u32)lbl_805F14F0.globalsOffset)

extern u32 lbl_805FB170;

struct RwCpp22Record {
    u16 first;
    u16 second;
    u16 third;
    u16 padding;
    u32 value;
    u16 fourth;
    u16 fifth;
    u16 sixth;
    u16 seventh;
};

static inline u32 *RwCpp22Link(u32 **head, u32 **slot, u32 *allocator, u16 key_a, u16 key_b) {
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

extern "C" {

/* _rpBuildMeshAddTriangle: appends one triangle, growing the buffer by one record when full. */
RpBuildMesh *fn_8045C340(RpBuildMesh *mesh, RpMaterial *material, u16 vert1, u16 vert2, u16 vert3,
                         u16 matIndex, u16 textureIndex, u16 rasterIndex, u16 pipelineIndex) {
    if (mesh->numTriangles >= mesh->triangleBufferSize) {
        RpBuildMeshTriangle *triangles;
        u32 size = (mesh->numTriangles + 1) * sizeof(RpBuildMeshTriangle);
        if (mesh->numTriangles != 0) {
            triangles = (RpBuildMeshTriangle *)RwRealloc(mesh->meshTriangles, size, 0x1030502);
        } else {
            triangles = (RpBuildMeshTriangle *)RwMalloc(size, 0x1030502);
        }
        if (triangles == NULL) {
            RWERROR((E_RW_NOMEM, size));
            return NULL;
        }
        mesh->meshTriangles = triangles;
        mesh->triangleBufferSize = mesh->numTriangles + 1;
    }
    mesh->meshTriangles[mesh->numTriangles].material = material;
    mesh->meshTriangles[mesh->numTriangles].vertIndex[0] = vert1;
    mesh->meshTriangles[mesh->numTriangles].vertIndex[1] = vert2;
    mesh->meshTriangles[mesh->numTriangles].vertIndex[2] = vert3;
    mesh->meshTriangles[mesh->numTriangles].matIndex = matIndex;
    mesh->meshTriangles[mesh->numTriangles].textureIndex = textureIndex;
    mesh->meshTriangles[mesh->numTriangles].rasterIndex = rasterIndex;
    mesh->meshTriangles[mesh->numTriangles].pipelineIndex = pipelineIndex;
    mesh->numTriangles++;
    return mesh;
}
RwStream *fn_8047D458(RwStream *stream, const void *data, u32 size); /* RwStreamWriteInt32 */
RwStream *fn_8047D868(RwStream *stream, void *data, u32 size);       /* RwStreamReadInt32 */
s32 fn_8045B8EC(const RpMaterialList *matList, const RpMaterial *material);
RpMaterial *fn_8045B79C(const RpMaterialList *matList, s32 index);

/* Mesh chunk records. */
struct RpMeshHeaderChunk {
    u32 flags;
    u32 numMeshes;
    u32 totalIndicesInMesh;
};

struct RpMeshChunk {
    u32 numIndices;
    s32 matIndex;
};

/* The object that owns the meshes is a geometry (type 8) or a world sector's world (type 7);
 * both keep their flags at 0x08, and a native one has no indices in the stream. */
struct RpMeshOwner {
    RwObject object;
    u32 flags;
};

#define RPMESHOWNERHASINDICES(type, owner)                                 \
    (((type) == 8 && !((owner)->flags & rpGEOMETRYNATIVE)) ||              \
     ((type) == 7 && !((owner)->flags & rpGEOMETRYNATIVE)))

/* _rpMeshWrite: the header, then per mesh its size and material index and its indices as words. */
RwStream *fn_8045C56C(const RpMeshHeader *meshHeader, const RpMeshOwner *owner, RwStream *stream,
                      const RpMaterialList *matList) {
    u32 buffer[256];
    RpMeshHeaderChunk headerChunk;
    RpMeshChunk meshChunk;
    u32 count;
    u32 type;
    u32 numMeshes;
    const RpMesh *mesh;
    u32 remaining;
    const u16 *indices;

    headerChunk.flags = meshHeader->flags;
    headerChunk.numMeshes = meshHeader->numMeshes;
    headerChunk.totalIndicesInMesh = meshHeader->totalIndicesInMesh;
    if (fn_8047D458(stream, &headerChunk, sizeof(headerChunk)) == NULL) {
        return NULL;
    }
    numMeshes = meshHeader->numMeshes;
    mesh = (const RpMesh *)(meshHeader + 1);
    type = owner->object.type;
    while (numMeshes--) {
        meshChunk.numIndices = mesh->numIndices;
        meshChunk.matIndex = fn_8045B8EC(matList, mesh->material);
        if (meshChunk.matIndex < 0) {
            meshChunk.matIndex = 0;
        }
        if (fn_8047D458(stream, &meshChunk, sizeof(meshChunk)) == NULL) {
            return NULL;
        }
        if (RPMESHOWNERHASINDICES(type, owner)) {
            remaining = mesh->numIndices;
            indices = mesh->indices;
            while (remaining != 0) {
                u32 i;
                count = 256;
                if (remaining < 256) {
                    count = remaining;
                }
                for (i = 0; i < count; i++) {
                    buffer[i] = *indices++;
                }
                if (fn_8047D458(stream, buffer, count * sizeof(u32)) == NULL) {
                    return NULL;
                }
                remaining -= count;
            }
        }
        mesh++;
    }
    return stream;
}

/* The mesh header allocator of the previous unit (fn_8045BF30), which the reader expands. */
static inline RpMeshHeader *RpMeshHeaderAlloc(u32 size) {
    return (RpMeshHeader *)RwMalloc(size, 0x30502);
}

/* _rpMeshRead: the inverse of the writer; the header gets the module's next serial number. */
RpMeshHeader *fn_8045C770(RwStream *stream, const RpMeshOwner *owner, const RpMaterialList *matList) {
    u32 buffer[256];
    RpMeshHeaderChunk headerChunk;
    RpMeshChunk meshChunk;
    RpMeshHeader *meshHeader;
    u32 type;
    RpMesh *mesh;
    u16 *indices;
    u32 count;
    u32 numMeshes;
    u32 remaining;
    u32 *source;
    u32 size;

    if (fn_8047D868(stream, &headerChunk, sizeof(headerChunk)) == NULL) {
        return NULL;
    }
    size = sizeof(RpMeshHeader) + headerChunk.numMeshes * (sizeof(RpMesh) + sizeof(u32));
    if (RPMESHOWNERHASINDICES(owner->object.type, owner)) {
        size += headerChunk.totalIndicesInMesh * sizeof(u16);
    }
    meshHeader = RpMeshHeaderAlloc(size);
    if (meshHeader != NULL) {
        mesh = (RpMesh *)(meshHeader + 1);
        indices = (u16 *)(mesh + headerChunk.numMeshes);
        type = owner->object.type;
        meshHeader->flags = headerChunk.flags;
        meshHeader->numMeshes = headerChunk.numMeshes;
        meshHeader->serialNum = RWPLUGINGLOBAL(rpMeshGlobals, lbl_805F14F0).nextSerialNum;
        meshHeader->totalIndicesInMesh = headerChunk.totalIndicesInMesh;
        meshHeader->firstMeshOffset = 0;
        RWPLUGINGLOBAL(rpMeshGlobals, lbl_805F14F0).nextSerialNum++;
        numMeshes = meshHeader->numMeshes;
        while (numMeshes--) {
            if (fn_8047D868(stream, &meshChunk, sizeof(meshChunk)) == NULL) {
                return NULL;
            }
            mesh->numIndices = meshChunk.numIndices;
            mesh->material = fn_8045B79C(matList, meshChunk.matIndex);
            mesh->indices = indices;
            if (RPMESHOWNERHASINDICES(type, owner)) {
                remaining = mesh->numIndices;
                while (remaining != 0) {
                    source = buffer;
                    count = 256;
                    if (remaining < 256) {
                        count = remaining;
                    }
                    if (fn_8047D868(stream, source, count * sizeof(u32)) == NULL) {
                        return NULL;
                    }
                    remaining -= count;
                    while (count != 0) {
                        *indices++ = *source++;
                        count--;
                    }
                }
            }
            mesh++;
        }
    }
    return meshHeader;
}

void *memcpy(void *destination, const void *source, u32 size);

u32 *fn_8045CBD4(u32 *object) {
    u32 size;
    u32 *bitset;
    u32 groups;
    u16 *data;
    u32 count = object[1];
    data = (u16 *)object[0];
    groups = count / 3;
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
            u32 clear;
            u16 *source;
            u32 processed = 0;
            while (processed < groups) {
                clear = size;
                u8 *clear_pointer = (u8 *)bitset;
                while (clear-- != 0) {
                    *clear_pointer++ = 0;
                }
                source = copy;
                for (u32 j = 0; j < groups; j++) {
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
                }
            }
            ((void (*)(u16 *))*(u32 *)(RWENGINEBYTES + 0x10C))(copy);
        }
        ((void (*)(u32 *))*(u32 *)(RWENGINEBYTES + 0x10C))(bitset);
        return object;
    }
    return object;
}

u32 *fn_8047FBD8(u32 size, u32 count, u32 alignment, u32 tag);

u32 *fn_8045CED8(u32 count, u32 *output, u32 **head, u8 *input) {
    u32 *slots = ((u32 *(*)(u32, u32))*(u32 *)(RWENGINEBYTES + 0x108))(count * 4, 0x00030502);
    output[0] = (u32)fn_8047FBD8(36, count, 4, 0x00030502);
    output[1] = (u32)fn_8047FBD8(16, count / 2 + 1, 4, 0x00030502);
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
        ((u32 *)slot[0])[1] = (u32)RwCpp22Link(head, (u32 **)slot, (u32 *)output[1],
                                                        *(u16 *)current, *(u16 *)(current + 2));
        ((u32 *)slot[0])[2] = (u32)RwCpp22Link(head, (u32 **)slot, (u32 *)output[1],
                                                        *(u16 *)(current + 2), *(u16 *)(current + 4));
        ((u32 *)slot[0])[3] = (u32)RwCpp22Link(head, (u32 **)slot, (u32 *)output[1],
                                                        *(u16 *)(current + 4), *(u16 *)current);
        slot++;
        current += 20;
        index++;
    }
    return slots;
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

void fn_8045D20C(u32 *object, u32 **array, s32 index) {
    if (index < 4) {
        object[7] = 1;
        return;
    }
    object[6] = 1;
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
    for (u8 i = 0; i < 3; i++) {
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
    }
}

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
    }
    return processed;
}

u32 fn_8045C9C8(u32 *object, u8 *entry) {
    if (entry[0] == 8 && (*(u32 *)(entry + 8) & 0x01000000)) {
        goto fixed_size;
    }
    if (entry[0] == 7 && (*(u32 *)(entry + 8) & 0x01000000)) {
        goto fixed_size;
    }
    goto variable_size;
fixed_size:
    return *(u16 *)((u8 *)object + 4) * 8 + 12;
variable_size:
    {
        u32 count = *(u16 *)((u8 *)object + 4);
        u32 result = count * 8;
        result += object[2] * 4;
        result += 12;
        return result;
    }
}

u32 *fn_8045C4E4(u32 *object, u32 (*callback)(u8 *, u32 *, u32), u32 argument) {
    s32 count = *(u16 *)((u8 *)object + 4);
    u8 *entry = (u8 *)object + object[3];
    entry += 16;
    while (count-- != 0) {
        if (callback(entry, object, argument) == 0) {
            return object;
        }
        entry += 12;
    }
    return object;
}

void fn_8045CA20(void) {
    s16 *target = (s16 *)((u8 *)RWENGINEBYTES + lbl_805F14F0.globalsOffset);
    *target += 1;
}
}
