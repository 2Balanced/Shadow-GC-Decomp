/* RenderWare, RwCpp42 (0x8047B720-0x8047D2C4): two large collision tests of the intersection
 * toolkit (fn_8047B720 and fn_8047BDD0, a sphere against a triangle and a larger triangle test;
 * not written yet), then the stream chunk headers (whether a chunk type is complex, reading and
 * writing a header, RwStreamFindChunk) and the memory byte-order conversions (RwMem*).
 * Written from the retail code; the fn_ symbols are kept because game code calls them by
 * address. Built as C: -O4,p, no stmw/lmw, automatic inlining in definition order. */
#include "Game/rw/r5.h"

#ifdef __cplusplus
extern "C" {
#endif

RwStream *fn_80480ED0(RwStream *stream, const void *buffer, u32 length); /* RwStreamWrite */
RwStream *fn_804810FC(RwStream *stream, u32 offset);                     /* RwStreamSkip */

/* Whether chunks of this type hold other chunks. Retail never inlines it. */
#pragma auto_inline off
RwBool fn_8047CCF8(const R5ChunkHeaderInfo *info) {
    switch (info->type) {
    case 1:
        return 0;
    case 2:
        return 0;
    case 3:
        return 0;
    case 5:
        return 1;
    case 6:
        return 1;
    case 7:
        return 1;
    case 8:
        return 1;
    case 9:
        return 1;
    case 10:
        return 1;
    case 11:
        return 1;
    case 13:
        return 0;
    case 14:
        return 1;
    case 15:
        return 1;
    case 16:
        return 1;
    case 18:
        return 1;
    case 19:
        return 0;
    case 20:
        return 1;
    case 26:
        return 1;
    }
    return 0;
}
#pragma auto_inline reset

/* Reads a chunk header: its type, length, library version and build. */
RwBool fn_8047CDB4(RwStream *stream, u32 *type, u32 *length, u32 *version, u32 *buildNum) {
    R5ChunkHeader header;
    R5ChunkHeaderInfo info;
    RwBool ok = fn_80480D4C(stream, &header, sizeof(header)) == sizeof(header);

    if (!ok) {
        R5ERROR(1, (0x8000001A));
        return 0;
    }
    fn_8047D254(&header, sizeof(header));
    info.type = header.type;
    info.length = header.length;
    if (!(header.libraryID & 0xFFFF0000)) {
        info.version = header.libraryID << 8;
        info.buildNum = 0;
    } else {
        info.version = (((header.libraryID >> 14) & 0x3FF00) + 0x30000) | ((header.libraryID >> 16) & 0x3F);
        info.buildNum = header.libraryID & 0xFFFF;
    }
    info.isComplex = fn_8047CCF8(&info);
    if (type != 0) {
        *type = info.type;
    }
    if (length != 0) {
        *length = info.length;
    }
    if (buildNum != 0) {
        *buildNum = info.buildNum;
    }
    if (version != 0) {
        *version = info.version;
    }
    return 1;
}

/* Writes a chunk header with the library version and build. */
RwStream *fn_8047CEF0(RwStream *stream, s32 type, s32 size, u32 version, u32 buildNum) {
    R5ChunkHeader header;

    header.type = type;
    header.length = size;
    header.libraryID = (((version - 0x30000) & 0x3FF00) << 14) | ((version & 0x3F) << 16) | (buildNum & 0xFFFF);
    fn_8047D158(&header, sizeof(header));
    return fn_80480ED0(stream, &header, sizeof(header));
}

/* RwStreamFindChunk: skips chunks until one of `type`, checking its version. */
RwBool fn_8047CF50(RwStream *stream, u32 type, u32 *lengthOut, u32 *versionOut) {
    u32 readType;
    u32 readLength;
    u32 readVersion;

    while (fn_8047CDB4(stream, &readType, &readLength, &readVersion, 0)) {
        if (readType == type) {
            if (readVersion < 0x35000) {
                R5ERROR(1, (0x80000004));
                return 0;
            }
            if (readVersion > 0x37002) {
                R5ERROR(1, (0x80000004));
                return 0;
            }
            if (lengthOut != 0) {
                *lengthOut = readLength;
            }
            if (versionOut != 0) {
                *versionOut = readVersion;
            }
            return 1;
        }
        if (fn_804810FC(stream, readLength) == 0) {
            return 0;
        }
    }
    return 0;
}

/* RwMemLittleEndian32: byte-swaps the 32-bit words in place.
 * NonMatching: retail unrolls the loop four times and forms each store's address with addi;
 * GC/2.7 unrolls it eight times with lwzu (five loop shapes tried). */
void *fn_8047D158(void *mem, u32 size) {
    u32 *p = (u32 *)mem;
    u32 n;

    for (n = size >> 2; n != 0; n--) {
        __stwbrx(*p, p, 0);
        p++;
    }
    return mem;
}

/* RwMemLittleEndian16. */
void *fn_8047D1C8(void *mem, u32 size) {
    u16 *p = (u16 *)mem;
    u32 n;

    for (n = size >> 1; n != 0; n--) {
        *p = (*p >> 8) | (*p << 8);
        p++;
    }
    return mem;
}

/* RwMemNative32. */
void *fn_8047D254(void *mem, u32 size) {
    u32 *p = (u32 *)mem;
    u32 n;

    for (n = size >> 2; n != 0; n--) {
        __stwbrx(*p, p, 0);
        p++;
    }
    return mem;
}

#ifdef __cplusplus
}
#endif
