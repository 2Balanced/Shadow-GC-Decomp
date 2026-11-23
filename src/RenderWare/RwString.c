/* RenderWare core: the string module (0x80482CA4-0x80483444).
 * The engine's own case-insensitive compare, upper and lower case, strchr and strrchr, the module
 * open (fills the engine's string function table) and close, then strings in streams: the size of
 * a string chunk, writing one, and finding and reading a string or unicode string chunk. The file
 * owns the empty string written for a NULL string (.sdata2 0x805FB2D0, padded to 8).
 * Boundary: the frame list binary at 0x804836EC has its own constants (0x805FB2D8); the absolute
 * path test (0x80483444) and RwBBoxCalculate (0x8048349C) are other modules.
 * NonMatching: fn_80483060 (_rwStringStreamFindAndRead), 44 rows of register numbering in the
 * two inlined readers (declaration orders tried).
 * C, cflags_rw_core (configure.py). */
#include "Game/rw/r6.h"

RwStream *fn_8047CEF0(RwStream *stream, u32 type, s32 size, u32 version, u32 build);
RwBool fn_8047CDB4(RwStream *stream, u32 *type, u32 *length, u32 *version, u32 *build);
u32 fn_80480D4C(RwStream *stream, void *buffer, u32 length);
RwStream *fn_80480ED0(RwStream *stream, const void *buffer, u32 length);
RwStream *fn_804810FC(RwStream *stream, u32 offset);
int vsprintf(char *, const char *, void *);
char *fn_803ACABC(char *, const char *, u32);
char *fn_803AC6C0(const char *, const char *);
s32 fn_803AC8DC(const char *, const char *, u32);
s32 fn_803AB5B8(const char *, const char *, ...);

static const char nullString[] = "";

/* The string table's case-insensitive compare. */
s32 fn_80482CA4(const char *a, const char *b) {
    if (a && b) {
        char ca;
        char cb;
        do {
            ca = *a;
            cb = *b;
            if (ca >= 'A' && ca <= 'Z') {
                ca += 'a' - 'A';
            }
            if (cb >= 'A' && cb <= 'Z') {
                cb += 'a' - 'A';
            }
            if (ca != cb) {
                return ca - cb;
            }
            a++;
            b++;
        } while (ca && cb);
        if (ca != cb) {
            return ca - cb;
        }
    }
    return 0;
}

/* Upper-case in place. */
char *fn_80482D3C(char *text) {
    char *cursor;

    if (text == NULL) {
        return text;
    }
    cursor = text;
    while (*cursor != 0) {
        char current = *cursor;
        if (current >= 'a' && current <= 'z') {
            *cursor = current - 0x20;
        }
        cursor++;
    }
    return text;
}

/* Lower-case in place. */
char *fn_80482D7C(char *text) {
    char *cursor;

    if (text == NULL) {
        return text;
    }
    cursor = text;
    while (*cursor != 0) {
        char current = *cursor;
        if (current >= 'A' && current <= 'Z') {
            *cursor = current + 0x20;
        }
        cursor++;
    }
    return text;
}

/* strchr. */
char *fn_80482DBC(char *text, int c) {
    char *found = NULL;
    char value = (char)c;
    int current;
    do {
        current = *text;
        if (current == value) {
            found = text;
            break;
        }
        text++;
    } while ((char)current != 0);
    return found;
}

/* strrchr. */
char *fn_80482DF0(char *text, int c) {
    char *found = NULL;
    char value = (char)c;
    int current;
    do {
        current = *text;
        if (current == value) {
            found = text;
        }
        text++;
    } while ((char)current != 0);
    return found;
}

/* _rwStringOpen: fills the engine's string function table. */
RwBool fn_80482E20(void) {
    RwEngineInstance->stringFuncs.vecSprintf = (void *)sprintf;
    RwEngineInstance->stringFuncs.vecVsprintf = (void *)vsprintf;
    RwEngineInstance->stringFuncs.vecStrcpy = strcpy;
    RwEngineInstance->stringFuncs.vecStrncpy = (void *)fn_803ACABC;
    RwEngineInstance->stringFuncs.vecStrcat = (void *)strcat;
    RwEngineInstance->stringFuncs.vecStrncat = (void *)strncat;
    RwEngineInstance->stringFuncs.vecStrrchr = (void *)fn_80482DF0;
    RwEngineInstance->stringFuncs.vecStrchr = (void *)fn_80482DBC;
    RwEngineInstance->stringFuncs.vecStrstr = (void *)fn_803AC6C0;
    RwEngineInstance->stringFuncs.vecStrcmp = (void *)strcmp;
    RwEngineInstance->stringFuncs.vecStrncmp = (void *)fn_803AC8DC;
    RwEngineInstance->stringFuncs.vecStricmp = (void *)fn_80482CA4;
    RwEngineInstance->stringFuncs.vecStrlen = (u32 (*)(const char *))strlen;
    RwEngineInstance->stringFuncs.vecStrupr = (void *)fn_80482D3C;
    RwEngineInstance->stringFuncs.vecStrlwr = (void *)fn_80482D7C;
    RwEngineInstance->stringFuncs.vecStrtok = (void *)strtok;
    RwEngineInstance->stringFuncs.vecSscanf = (void *)fn_803AB5B8;
    return TRUE;
}

/* _rwStringClose. */
void fn_80482F60(void) {
}

/* _rwStringStreamGetSize: the length with its terminator, padded to four bytes. */
u32 fn_80482F64(const char *string) {
    if (!string) {
        string = nullString;
    }
    return (RwEngineInstance->stringFuncs.vecStrlen(string) + 4) & ~3;
}

/* _rwStringStreamWrite. */
const char *fn_80482FA4(const char *string, RwStream *stream) {
    u32 size;

    if (!string) {
        string = nullString;
    }
    size = fn_80482F64(string);
    if (!fn_8047CEF0(stream, 2, size, 0x37002, 0x37)) {
        return NULL;
    }
    if (!fn_80480ED0(stream, string, size)) {
        return NULL;
    }
    return string;
}

static inline char *StringStreamRead(char *string, RwStream *stream, u32 length) {
    char buffer[64];
    u32 count;
    char *out;

    if (!string) {
        string = (char *)RwEngineInstance->memoryFuncs.rwmalloc(length, 0x30002);
        if (!string) {
            RWERROR((0x80000013, length));
            return NULL;
        }
    }
    out = string;
    while (length > 0) {
        u32 i;
        count = (length > 64) ? 64 : length;
        if (count != fn_80480D4C(stream, buffer, count)) {
            return NULL;
        }
        length -= count;
        for (i = 0; i < count; i++) {
            out[i] = buffer[i];
        }
        out += count;
    }
    return string;
}

static inline char *UnicodeStringStreamRead(char *string, RwStream *stream, u32 length) {
    u16 buffer[64];
    u32 count;
    char *out;
    RwBool allocated = FALSE;

    if (!string) {
        string = (char *)RwEngineInstance->memoryFuncs.rwmalloc(length, 0x30002);
        if (!string) {
            RWERROR((0x80000013, length));
            return NULL;
        }
        allocated = TRUE;
    }
    out = string;
    while (length > 0) {
        u32 i;
        count = (length > 128) ? 128 : length;
        if (count != fn_80480D4C(stream, buffer, count)) {
            if (allocated) {
                RwEngineInstance->memoryFuncs.rwfree(string);
            }
            return NULL;
        }
        length -= count;
        for (i = 0; i < count / 2; i++) {
            out[i] = (char)buffer[i];
        }
        out += count / 2;
    }
    return string;
}

/* _rwStringStreamFindAndRead: skips chunks until a string (2) or unicode string (0x13) chunk. */
char *fn_80483060(char *string, RwStream *stream) {
    u32 type;
    u32 length;
    u32 version;

    while (fn_8047CDB4(stream, &type, &length, &version, NULL)) {
        RwBool valid = FALSE;
        if (version >= 0x35000 && version <= 0x37002) {
            valid = TRUE;
        }
        if (!valid) {
            RWERROR((0x80000004));
            return NULL;
        }
        if (type == 2) {
            return StringStreamRead(string, stream, length);
        } else if (type == 0x13) {
            return UnicodeStringStreamRead(string, stream, length);
        }
        if (!fn_804810FC(stream, length)) {
            return NULL;
        }
    }
    return NULL;
}
