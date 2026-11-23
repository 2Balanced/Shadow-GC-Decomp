/* RenderWare stream chunks and typed stream data, 0x8047CCF8..0x8047DB10: chunk headers (whether a
 * chunk type holds other chunks, reading and writing a header, RwStreamFindChunk,
 * RwStreamReadChunkHeaderInfo), the memory byte-order conversions (RwMem*) and the typed reads and
 * writes (RwStreamRead/WriteReal, Int32, Int16), which inline the conversions.
 *
 * One source file: its .data is the jump table of the chunk type switch (0x8056DEE0, padded to
 * 0x8056DF50), and retail inlines the header reader and the conversions into the functions that
 * follow them. The basic stream functions (RwStreamOpen, Read, Write, Skip, Close) are another
 * file, in range r6.
 *
 * NonMatching: fn_8047D94C (see its comment).
 *
 * Written from the retail code; the fn_ symbols are kept because game code calls them by address.
 * C.
 * Flags: GC/2.7 -O4,p, no stmw/lmw, automatic inlining, -lang=c, writable pooled strings
 * (cflags_rw_r5). Measured per file against the other RenderWare flag sets: GC 2.0-2.7 give the
 * same code, GC 1.3.2, -O4,s, stmw/lmw and -inline noauto match fewer functions, and the
 * read-only string sets (cflags_rw_r3/r6/c) would move strings out of .data/.sdata. */
#include "Game/rw/r5.h"

void *memcpy(void *dst, const void *src, u32 n);
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

/* RwMemLittleEndian32: byte-swaps the 32-bit words in place. Written with shifts, which the
 * compiler turns into stwbrx: the __stwbrx intrinsic gives a loop GC/2.7 unrolls eight times with
 * lwzu, where retail's is unrolled four times. */
void *fn_8047D158(void *mem, u32 size) {
    u32 v;
    u32 n;
    u32 *p = (u32 *)mem;

    for (n = size >> 2; n != 0; n--) {
        v = *p;
        *p = (v >> 24) | ((v >> 8) & 0xFF00) | ((v & 0xFF00) << 8) | (v << 24);
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
    u32 v;
    u32 n;
    u32 *p = (u32 *)mem;

    for (n = size >> 2; n != 0; n--) {
        v = *p;
        *p = (v >> 24) | ((v >> 8) & 0xFF00) | ((v & 0xFF00) << 8) | (v << 24);
        p++;
    }
    return mem;
}

/* RwMemNative16. */
void *fn_8047D2C4(void *mem, u32 size) {
    u32 n;
    u16 *p = (u16 *)mem;

    for (n = size >> 1; n != 0; n--) {
        *p = (*p >> 8) | (*p << 8);
        p++;
    }
    return mem;
}

/* RwStreamWriteReal: through a 256-byte buffer, converted to the stream's byte order. */
RwStream *fn_8047D350(RwStream *stream, const f32 *reals, u32 numBytes) {
    u8 buffer[0x100];

    while (numBytes != 0) {
        u32 size = 0x100;

        if (numBytes < 0x100) {
            size = numBytes;
        }
        memcpy(buffer, reals, size);
        fn_8047D158(buffer, size);
        if (fn_80480ED0(stream, buffer, size) == 0) {
            return 0;
        }
        numBytes -= size;
        reals = (const f32 *)((const u8 *)reals + size);
    }
    return stream;
}

/* RwStreamWriteInt32. */
RwStream *fn_8047D458(RwStream *stream, const s32 *ints, u32 numBytes) {
    u8 buffer[0x100];

    while (numBytes != 0) {
        u32 size = 0x100;

        if (numBytes < 0x100) {
            size = numBytes;
        }
        memcpy(buffer, ints, size);
        fn_8047D158(buffer, size);
        if (fn_80480ED0(stream, buffer, size) == 0) {
            return 0;
        }
        numBytes -= size;
        ints = (const s32 *)((const u8 *)ints + size);
    }
    return stream;
}

/* RwStreamWriteInt16. */
RwStream *fn_8047D560(RwStream *stream, const s16 *ints, u32 numBytes) {
    u8 buffer[0x100];

    while (numBytes != 0) {
        u32 size = 0x100;

        if (numBytes < 0x100) {
            size = numBytes;
        }
        memcpy(buffer, ints, size);
        fn_8047D2C4(buffer, size);
        if (fn_80480ED0(stream, buffer, size) == 0) {
            return 0;
        }
        numBytes -= size;
        ints = (const s16 *)((const u8 *)ints + size);
    }
    return stream;
}

/* RwStreamReadInt16. */
RwStream *fn_8047D684(RwStream *stream, s16 *ints, u32 numBytes) {
    if (fn_80480D4C(stream, ints, numBytes) == 0) {
        R5ERROR(1, (0x8000001A));
        return 0;
    }
    fn_8047D2C4(ints, numBytes);
    return stream;
}

/* RwStreamReadReal. */
RwStream *fn_8047D784(RwStream *stream, f32 *reals, u32 numBytes) {
    if (fn_80480D4C(stream, reals, numBytes) == 0) {
        R5ERROR(1, (0x8000001A));
        return 0;
    }
    fn_8047D254(reals, numBytes);
    return stream;
}

/* RwStreamReadInt32. */
RwStream *fn_8047D868(RwStream *stream, s32 *ints, u32 numBytes) {
    if (fn_80480D4C(stream, ints, numBytes) == 0) {
        R5ERROR(1, (0x8000001A));
        return 0;
    }
    fn_8047D254(ints, numBytes);
    return stream;
}

/* RwStreamReadChunkHeaderInfo.
 * NonMatching (61 rows, 103 of retail's 113 instructions): retail inlines the header reader and,
 * inside it, RwMemNative32 as a loop over the constant 12 that is not unrolled, and it keeps the
 * header reader's null tests of the addresses of this function's locals. GC/2.7 (and 1.3.2-2.6)
 * unrolls the loop and drops the tests; no inlining or optimisation pragma tried reproduces it
 * (probes/RenderWare_RwStreamChunk_R4.py). */
RwStream *fn_8047D94C(RwStream *stream, R5ChunkHeaderInfo *headerInfo) {
    u32 type;
    u32 length;
    u32 build;
    u32 version;

    if (!fn_8047CDB4(stream, &type, &length, &version, &build)) {
        return 0;
    }
    headerInfo->type = type;
    headerInfo->length = length;
    headerInfo->version = version;
    headerInfo->buildNum = build;
    headerInfo->isComplex = fn_8047CCF8(headerInfo);
    return stream;
}
