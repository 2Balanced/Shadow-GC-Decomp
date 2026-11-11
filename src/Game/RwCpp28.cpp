/* RenderWare vertex-array packing and format helpers, 0x80465ED4..0x80466B57.
 * Structure and field names are inferred from behavior. */
#include <string.h>
#include <dolphin/types.h>

extern "C" {
extern u8 *lbl_805F15B4;

static inline u32 rwElementSize(u32 type) {
    switch (type) {
    case 1: return 1;
    case 2: return 2;
    case 3: return 3;
    case 4: return 4;
    case 6: return 4;
    case 7: return 8;
    case 8: return 12;
    case 9: return 16;
    case 10: return 4;
    default: return 0;
    }
}

static inline s32 rwElementEqual(u32 type, const u8 *a, const u8 *b) {
    switch (type) {
    case 1: return *a == *b;
    case 2: return *(const u16 *)a == *(const u16 *)b;
    case 3: return a[0] == b[0] && a[1] == b[1] && a[2] == b[2];
    case 4: return *(const u32 *)a == *(const u32 *)b;
    case 6: return *(const u32 *)a == *(const u32 *)b;
    case 7:
        return *(const u32 *)a == *(const u32 *)b &&
               *(const u32 *)(a + 4) == *(const u32 *)(b + 4);
    case 8:
        return *(const u32 *)a == *(const u32 *)b &&
               *(const u32 *)(a + 4) == *(const u32 *)(b + 4) &&
               *(const u32 *)(a + 8) == *(const u32 *)(b + 8);
    case 9:
        return *(const u32 *)a == *(const u32 *)b &&
               *(const u32 *)(a + 4) == *(const u32 *)(b + 4) &&
               *(const u32 *)(a + 8) == *(const u32 *)(b + 8) &&
               *(const u32 *)(a + 12) == *(const u32 *)(b + 12);
    case 10: return *(const u32 *)a == *(const u32 *)b;
    default: return 0;
    }
}

static inline u32 *rwAlloc(u32 size) {
    return (u32 *)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(size, 0x3050D);
}

u32 *fn_80465ED4(u32 *descriptors, u8 *inputs, u32 streams, u32 vertices) {
    u32 bytes = (streams & 0x1FFFFFFF) * 8;
    for (u32 i = 0; i < streams; i++) {
        bytes += descriptors[i * 2 + 1] * rwElementSize(*(u32 *)(inputs + i * 0x18 + 4));
    }
    u32 *result = rwAlloc(bytes);
    if (result == NULL) return NULL;
    u32 headerBytes = (streams & 0x1FFFFFFF) * 8;
    memset(result, 0, headerBytes);
    for (u32 i = 0, offset = headerBytes; i < streams; i++) {
        result[i * 2] = (u32)result + offset;
        offset += descriptors[i * 2 + 1] * rwElementSize(*(u32 *)(inputs + i * 0x18 + 4));
    }
    for (u32 i = 0; i < streams; i++) {
        u32 size = rwElementSize(*(u32 *)(inputs + i * 0x18 + 4));
        u8 *dest = (u8 *)result[i * 2];
        u8 *source = *(u8 **)(inputs + i * 0x18);
        for (u32 j = 0; j < vertices; j++) {
            if (((s32 *)descriptors[i * 2])[j] == -1) {
                memcpy(dest, source, size);
                dest += size;
            }
            source += size;
        }
        result[i * 2 + 1] = descriptors[i * 2 + 1];
    }
    return result;
}

/* descriptors is an array of 0x18-byte stream descriptors: +0x0 element data
 * pointer, +0x4 element type, +0x8 onward signed stream indices whose elements
 * must match as well, terminated by the first negative index.  The returned
 * block is streams {pointer, count} pairs followed by streams * vertices
 * index words; every vertex starts unassigned (0xFFFFFFFF) and a vertex that
 * duplicates an earlier one stores that earlier vertex index and drops the
 * stream count. */
u32 *fn_80466194(u32 *descriptors, u32 streams, u32 vertices) {
    u32 total = streams * vertices;
    u32 headerBytes = streams * 8;
    u32 *result = rwAlloc(headerBytes + total * 4);
    if (result == NULL) return NULL;
    memset(result, 0, headerBytes);
    u32 *indices = (u32 *)((u8 *)result + headerBytes);
    for (u32 i = 0; i < total; i++) indices[i] = 0xFFFFFFFF;
    u32 *header = result;
    u32 offset = 0;
    for (u32 i = 0; i < streams; i++) {
        header[i * 2] = (u32)((u8 *)indices + offset);
        header[i * 2 + 1] = vertices;
        offset += vertices * 4;
    }
    u8 *descriptor = (u8 *)descriptors;
    for (u32 stream = 0; stream < streams; stream++) {
        u32 size = rwElementSize(*(u32 *)(descriptor + 4));
        for (u32 i = 0; i < vertices; i++) {
            if (((s32 *)header[0])[i] != -1) continue;
            for (u32 j = i; j < vertices; j++) {
                if (i == j) continue;
                if (!rwElementEqual(*(u32 *)(descriptor + 4),
                                    *(u8 **)descriptor + i * size,
                                    *(u8 **)descriptor + j * size))
                    continue;
                s32 equal = 1;
                const s8 *link = (const s8 *)(descriptor + 8);
                u32 linkSize =
                    rwElementSize(*(u32 *)((u8 *)descriptors + (s32)*link * 0x18 + 4));
                while ((s32)*link >= 0) {
                    u8 *other = (u8 *)descriptors + (s32)*link * 0x18;
                    u8 *otherData = *(u8 **)other;
                    if (!rwElementEqual(*(u32 *)(other + 4), otherData + i * linkSize,
                                        otherData + j * linkSize)) {
                        equal = false;
                        break;
                    }
                    link++;
                    linkSize =
                        rwElementSize(*(u32 *)((u8 *)descriptors + (s32)*link * 0x18 + 4));
                }
                if (equal) {
                    ((u32 *)header[0])[j] = i;
                    header[1]--;
                }
            }
        }
        descriptor += 0x18;
        header += 2;
    }
    return result;
}

u32 fn_8046693C(u32 *format) {
    u32 base = format[1] & 1;
    if (base == 1) {
        base = 3;
    } else if (base == 0) {
        base = 2;
    }
    int kind = (format[1] >> 1) & 7;
    switch (kind) {
    case 0:
    case 1: return base;
    case 2:
    case 3: return base << 1;
    case 4: return base << 2;
    default: return 0;
    }
}

u32 fn_804669A0(u32 *format) {
    int kind = (format[1] >> 10) & 7;
    switch (kind) {
    case 1: return 3;
    case 3: return 6;
    case 4: return 12;
    default: return 0;
    }
}

u32 fn_804669EC(u32 *format, u8 channel) {
    int kind = (format[1] >> (channel * 4 + 0xE)) & 7;
    switch (kind) {
    case 0:
    case 3: return 2;
    case 1:
    case 4: return 3;
    case 2:
    case 5: return 4;
    default: return 0;
    }
}

u32 fn_80466A54(u32 *format, u8 channel) {
    u32 present;
    u32 kind;
    switch (channel) {
    case 0:
        present = (format[1] >> 0x15) & 1;
        kind = (format[1] >> 0x16) & 7;
        break;
    case 1:
        present = format[2] & 1;
        kind = (format[2] >> 1) & 7;
        break;
    case 2:
        present = (format[2] >> 9) & 1;
        kind = (format[2] >> 10) & 7;
        break;
    case 3:
        present = (format[2] >> 0x12) & 1;
        kind = (format[2] >> 0x13) & 7;
        break;
    case 4:
        present = (format[2] >> 0x1B) & 1;
        kind = (format[2] >> 0x1C) & 7;
        break;
    case 5:
        present = (format[3] >> 5) & 1;
        kind = (format[3] >> 6) & 7;
        break;
    case 6:
        present = (format[3] >> 0xE) & 1;
        kind = (format[3] >> 0xF) & 7;
        break;
    case 7:
        present = (format[3] >> 0x17) & 1;
        kind = (format[3] >> 0x18) & 7;
        break;
    default:
        return 0;
    }
    u32 base = present;
    if (base == 1) {
        base = 2;
    } else if (base == 0) {
        base = 1;
    }
    int signedKind = kind;
    switch (signedKind) {
    case 0:
    case 1: return base;
    case 2:
    case 3: return base << 1;
    case 4: return base << 2;
    default: return 0;
    }
}
}
