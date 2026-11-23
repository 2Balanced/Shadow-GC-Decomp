/* RenderWare: three toolkits, each a source file of its own:
 *  - RtCharset, the debug character set (fn_8047832C..fn_80478F74; .rodata font 0x8051BD68,
 *    .data 0x8056DD68, .sdata 0x805EDE70, .sdata2 0x805FB288..0x805FB298);
 *  - RtDict, dictionaries of named entries (fn_80479040..fn_80479284);
 *  - RtFSyst, the file system manager (fn_80479598 on, continued in RwCpp41; .bss 0x805E2A40,
 *    .sbss 0x805F1548).
 * Written from the retail code; the fn_ symbols are kept because game code calls them by
 * address, and each function says which public RenderWare API it is. Built as C: -O4,p, no
 * stmw/lmw, automatic inlining in definition order. */
#include "Game/rw/r5.h"

#ifdef __cplusplus
extern "C" {
#endif

void *memset(void *dst, int c, u32 n);
u32 strlen(const char *s);

/* RtCharset ---------------------------------------------------------------------------------- */

extern const u8 lbl_8051BD68[];   /* the 8x14 font, one byte per pixel row of each character */
extern s8 lbl_805EDE70;           /* the character that is not drawn (' ') */
extern const f32 lbl_805FB288;    /* 1.0f */
extern const f32 lbl_805FB28C;    /* 0.5f */

extern RtCharsetBuffer lbl_8056DD68; /* the buffers of the buffered printing */

void *fn_80487D08(s32 width, s32 height, s32 depth); /* RwImageCreate */
RwBool fn_80487DB0(void *image);                     /* RwImageDestroy */
void *fn_80487E18(void *image);                      /* RwImageAllocatePixels */
RwRaster *fn_80489A1C(RwRaster *raster, void *image); /* RwRasterSetFromImage */
RwBool fn_80489BDC(RwRaster *raster);                /* RwRasterDestroy */
RwRaster *fn_80489E74(s32 width, s32 height, s32 depth, s32 flags); /* RwRasterCreate */

/* Builds the quads of `string` at (x, y) into `vertices` and `indices`, skipping characters off
 * the frame buffer; returns the number of quads. */
s32 fn_8047832C(const char *string, s32 length, RtCharset *charSet, s32 x, s32 y, RwIm2DVertex *vertices,
                u16 *indices, u16 base, RwBool hideSpaces);

/* The character set's layout: character and tile sizes, and the square raster that holds them.
 * Returns the font. */
const u8 *fn_80478AE8(RtCharsetDesc *desc, s32 *size) {
    s32 tilesPerRow;
    s32 rows;
    s32 rasterSize;

    desc->width = 8;
    desc->height = 14;
    desc->width_internal = desc->width + 1;
    desc->height_internal = desc->height + 1;
    rasterSize = 16;
    do {
        rasterSize += rasterSize;
        tilesPerRow = rasterSize / desc->width_internal;
        rows = (tilesPerRow + 90) / tilesPerRow;
    } while (rasterSize < rows * desc->height_internal);
    desc->count = 91;
    desc->tile_width = tilesPerRow;
    desc->tile_height = rows;
    *size = rasterSize;
    return lbl_8051BD68;
}

s32 fn_8047832C(const char *string, s32 length, RtCharset *charSet, s32 x, s32 y, RwIm2DVertex *vertices,
                u16 *indices, u16 base, RwBool hideSpaces) {
    RtCharsetDesc desc;
    s32 height;
    f32 recipHeight;
    f32 recipWidth;
    f32 halfTexelV;
    f32 halfTexelU;
    f32 z;
    s32 width;
    RwIm2DVertex *vertex;
    s32 count;
    s32 i;
    s32 numVertices;
    s32 offset;

    fn_80478AE8(&desc, &height);
    recipHeight = lbl_805FB288 / (f32)charSet->height;
    recipWidth = lbl_805FB288 / (f32)charSet->width;
    height = RWSRCGLOBAL(curCamera)->frameBuffer->height;
    width = RWSRCGLOBAL(curCamera)->frameBuffer->width;
    z = RWSRCGLOBAL(zBufferNear);
    halfTexelV = recipHeight * lbl_805FB28C;
    halfTexelU = recipWidth * lbl_805FB28C;
    if (!hideSpaces) {
        lbl_805EDE70 = 0;
    }
    vertex = vertices;
    count = 0;
    numVertices = 0;
    offset = 0;
    for (i = 0; i < length; i++, string++) {
        s32 c = *string;
        s32 top;
        s32 bottom;
        s32 left;
        s32 right;
        s32 u0;
        s32 v0;
        s32 u1;
        s32 v1;
        s32 tile;
        s32 column;

        if (lbl_805EDE70 == c) {
            continue;
        }
        top = y;
        bottom = y + desc.height;
        left = x + desc.width * i;
        right = left + desc.width;
        if (top >= height || bottom <= 0 || left >= width || right <= 0) {
            continue;
        }
        tile = (u32)(c - 32) / desc.tile_width;
        column = (c - 32) - tile * desc.tile_width;
        u0 = column * desc.width_internal;
        v0 = tile * desc.height_internal;
        u1 = u0 + desc.width;
        v1 = v0 + desc.height;
        if (top < 0) {
            v0 -= top;
            top = 0;
        }
        if (bottom > height - 1) {
            bottom = height - 1;
            v1 = v0 + (bottom - top);
        }
        if (left < 0) {
            u0 -= left;
            left = 0;
        }
        if (right > width - 1) {
            right = width - 1;
            u1 = u0 + (right - left);
        }
        {
            f32 tu0 = recipWidth * (f32)u0 + halfTexelU;
            f32 tv0 = recipHeight * (f32)v0 + halfTexelV;
            f32 tv1 = recipHeight * (f32)v1 + halfTexelV;
            f32 tu1 = recipWidth * (f32)u1 + halfTexelU;

            vertex[0].u = tu0;
            vertex[0].v = tv0;
            vertex[1].u = tu0;
            vertex[1].v = tv1;
            vertex[2].u = tu1;
            vertex[2].v = tv1;
            vertex[3].u = tu1;
            vertex[3].v = tv0;
        }
        vertex[0].x = (f32)left;
        vertex[0].y = (f32)top;
        vertex[1].x = (f32)left;
        vertex[1].y = (f32)bottom;
        vertex[2].x = (f32)right;
        vertex[2].y = (f32)bottom;
        vertex[3].x = (f32)right;
        vertex[3].y = (f32)top;
        {
            RwIm2DVertex *v = (RwIm2DVertex *)((u8 *)vertices + offset);

            v->z = z;
            v->emissiveColor.red = 255;
            v->emissiveColor.green = 255;
            v->emissiveColor.blue = 255;
            v->emissiveColor.alpha = 255;
        }
        vertices[numVertices + 1].z = z;
        vertices[numVertices + 1].emissiveColor.red = 255;
        vertices[numVertices + 1].emissiveColor.green = 255;
        vertices[numVertices + 1].emissiveColor.blue = 255;
        vertices[numVertices + 1].emissiveColor.alpha = 255;
        vertices[numVertices + 2].z = z;
        vertices[numVertices + 2].emissiveColor.red = 255;
        vertices[numVertices + 2].emissiveColor.green = 255;
        vertices[numVertices + 2].emissiveColor.blue = 255;
        vertices[numVertices + 2].emissiveColor.alpha = 255;
        vertices[numVertices + 3].z = z;
        vertices[numVertices + 3].emissiveColor.red = 255;
        vertices[numVertices + 3].emissiveColor.green = 255;
        vertices[numVertices + 3].emissiveColor.blue = 255;
        vertices[numVertices + 3].emissiveColor.alpha = 255;
        indices[0] = base;
        indices[1] = base + 1;
        indices[2] = base + 2;
        indices[3] = base;
        indices[4] = base + 2;
        indices[5] = base + 3;
        base += 4;
        indices += 6;
        vertex += 4;
        numVertices += 4;
        offset += 4 * sizeof(RwIm2DVertex);
        count++;
    }
    return count;
}

/* Prints `string` at (x, y) with the character set: one indexed triangle list, drawn with the
 * render states it needs and the previous ones restored afterwards. */
RtCharset *fn_80478700(RtCharset *charSet, const char *string, s32 x, s32 y) {
    s32 length = strlen(string);
    s32 vertexSize;
    s32 indexSize;
    RwIm2DVertex *vertices;
    u16 *indices;
    s32 count;
    void *zTest;
    void *zWrite;
    void *srcBlend;
    void *destBlend;
    void *raster;
    void *filter;
    void *cull;

    if (length == 0) {
        return charSet;
    }
    vertexSize = length * 4 * sizeof(RwIm2DVertex);
    vertices = (RwIm2DVertex *)RWSRCGLOBAL(rwmalloc)(vertexSize, 0x10180);
    indexSize = length * 6 * sizeof(u16);
    indices = (u16 *)RWSRCGLOBAL(rwmalloc)(indexSize, 0x10180);
    if (vertices == 0 || indices == 0) {
        if (vertices != 0) {
            RWSRCGLOBAL(rwfree)(vertices);
        }
        if (indices != 0) {
            RWSRCGLOBAL(rwfree)(indices);
        }
        R5ERROR(0x180, (0x80000013, vertexSize + indexSize));
        return 0;
    }
    count = fn_8047832C(string, length, charSet, x, y, vertices, indices, 0, 0);
    if (count <= 0) {
        RWSRCGLOBAL(rwfree)(indices);
        RWSRCGLOBAL(rwfree)(vertices);
        return 0;
    }
    RWSRCGLOBAL(renderStateGet)(6, &zTest);
    RWSRCGLOBAL(renderStateGet)(8, &zWrite);
    RWSRCGLOBAL(renderStateGet)(10, &srcBlend);
    RWSRCGLOBAL(renderStateGet)(11, &destBlend);
    RWSRCGLOBAL(renderStateGet)(1, &raster);
    RWSRCGLOBAL(renderStateGet)(9, &filter);
    RWSRCGLOBAL(renderStateGet)(20, &cull);
    RWSRCGLOBAL(renderStateSet)(6, 0);
    RWSRCGLOBAL(renderStateSet)(8, 0);
    RWSRCGLOBAL(renderStateSet)(9, (void *)1);
    RWSRCGLOBAL(renderStateSet)(1, charSet);
    RWSRCGLOBAL(renderStateSet)(10, (void *)5);
    RWSRCGLOBAL(renderStateSet)(11, (void *)6);
    RWSRCGLOBAL(renderStateSet)(20, (void *)1);
    RWSRCGLOBAL(im2DRenderIndexedPrimitive)(3, vertices, count * 4, indices, count * 6);
    RWSRCGLOBAL(renderStateSet)(6, zTest);
    RWSRCGLOBAL(renderStateSet)(8, zWrite);
    RWSRCGLOBAL(renderStateSet)(10, srcBlend);
    RWSRCGLOBAL(renderStateSet)(11, destBlend);
    RWSRCGLOBAL(renderStateSet)(1, raster);
    RWSRCGLOBAL(renderStateSet)(9, filter);
    RWSRCGLOBAL(renderStateSet)(20, cull);
    RWSRCGLOBAL(rwfree)(indices);
    RWSRCGLOBAL(rwfree)(vertices);
    return charSet;
}

/* RtCharsetPrint. */
RtCharset *fn_80478AC8(RtCharset *charSet, const char *string, s32 x, s32 y) {
    return fn_80478700(charSet, string, x, y);
}

/* RtCharsetGetDesc. */
RtCharset *fn_80478B5C(RtCharset *charSet, RtCharsetDesc *desc) {
    if (charSet != 0 && desc != 0) {
        s32 size;

        fn_80478AE8(desc, &size);
        return charSet;
    }
    R5ERROR(0x180, (0x80000016));
    return 0;
}

/* RtCharsetSetColors: draws the font into the raster in the two colours. */
RtCharset *fn_80478C14(RtCharset *charSet, const RwRGBA *foreGround, const RwRGBA *backGround) {
    RtCharset *result = 0;

    if (charSet != 0 && foreGround != 0 && backGround != 0) {
        RtCharsetDesc desc;
        s32 size;
        void *image;

        fn_80478AE8(&desc, &size);
        image = fn_80487D08(size, size, 8);
        if (image != 0) {
            if (fn_80487E18(image) != 0) {
                R5Image *img = (R5Image *)image;
                u8 *pixels;
                s32 stride;
                const u8 *font;
                s32 row;
                s32 column;
                s32 c;

                img->palette[0] = *backGround;
                img->palette[1] = *foreGround;
                pixels = img->cpPixels;
                stride = img->stride;
                fn_80478AE8(&desc, &size);
                font = lbl_8051BD68;
                row = 0;
                column = 0;
                for (c = 1; c <= 91; c++) {
                    u8 *line = pixels + row * stride * desc.height_internal + column * desc.width_internal;
                    s32 y;

                    for (y = 0; y < desc.height_internal; y++) {
                        u8 *p = line;
                        u32 bits;

                        line += stride;
                        memset(p, 0, desc.width_internal);
                        for (bits = *font; (u8)bits != 0; bits >>= 1) {
                            *p++ = bits & 1;
                        }
                        font++;
                    }
                    column++;
                    if (desc.tile_width == column) {
                        column = 0;
                        row++;
                    }
                }
                fn_80489A1C(charSet, image);
                result = charSet;
            }
            fn_80487DB0(image);
        }
    } else {
        R5ERROR(0x180, (0x80000016));
    }
    return result;
}

/* RtCharsetCreate. */
RtCharset *fn_80478DE0(const RwRGBA *foreGround, const RwRGBA *backGround) {
    RtCharset *result = 0;

    if (foreGround != 0 && backGround != 0) {
        RtCharsetDesc desc;
        s32 size;
        RwRaster *raster;

        fn_80478AE8(&desc, &size);
        raster = fn_80489E74(size, size, 0, 4);
        if (raster != 0) {
            if (fn_80478C14(raster, foreGround, backGround) != 0) {
                result = raster;
            } else if (raster != 0) {
                fn_80489BDC(raster);
            } else {
                R5ERROR(0x180, (0x80000016));
            }
        }
    } else {
        R5ERROR(0x180, (0x80000016));
    }
    return result;
}

/* RtCharsetClose: frees the buffers of the buffered printing. */
void fn_80478F00(void) {
    RWSRCGLOBAL(rwfree)(lbl_8056DD68.vertices);
    lbl_8056DD68.vertices = 0;
    RWSRCGLOBAL(rwfree)(lbl_8056DD68.indices);
    lbl_8056DD68.indices = 0;
    lbl_8056DD68.open = 0;
}

/* RtCharsetOpen: the buffers of the buffered printing (1024 characters). */
RwBool fn_80478F74(void) {
    lbl_8056DD68.vertices = (RwIm2DVertex *)RWSRCGLOBAL(rwmalloc)(0x400 * 4 * sizeof(RwIm2DVertex), 0x40180);
    if (lbl_8056DD68.vertices == 0) {
        return 0;
    }
    lbl_8056DD68.indices = (u16 *)RWSRCGLOBAL(rwmalloc)(0x400 * 6 * sizeof(u16), 0x40180);
    if (lbl_8056DD68.indices == 0) {
        RWSRCGLOBAL(rwfree)(lbl_8056DD68.vertices);
        lbl_8056DD68.vertices = 0;
        return 0;
    }
    lbl_8056DD68.field_0x8 = 0;
    lbl_8056DD68.numChars = 0;
    lbl_8056DD68.open = 1;
    return 1;
}

/* RtDict ------------------------------------------------------------------------------------ */

void *fn_8047E0E0(RwSList *list);              /* the first entry */
void *fn_8047E0E8(RwSList *list);              /* past the last entry */
RwSList *fn_8047DF9C(RwSList *list, s32 count); /* drops the last `count` entries */

/* RtDictSchemaInit. */
RtDictSchema *fn_80479040(RtDictSchema *schema) {
    schema->dictionaries = fn_8047DC90(sizeof(RtDict *), schema->dictChunkType | 0x40000);
    if (schema->dictionaries == 0) {
        return 0;
    }
    schema->current = 0;
    return schema;
}

/* RtDictSchemaDestruct. */
RwBool fn_8047909C(RtDictSchema *schema) {
    fn_8047DFC8(schema->dictionaries);
    return 1;
}

/* RtDictSchemaGetCurrentDict. */
RtDict *fn_804790C4(RtDictSchema *schema) {
    return schema->current;
}

/* RtDictSchemaSetCurrentDict. */
RtDictSchema *fn_804790CC(RtDictSchema *schema, RtDict *dict) {
    schema->current = dict;
    return schema;
}

/* RtDictFindNamedEntry. */
void *fn_804790D4(RtDict *dict, const char *name) {
    void **entry;
    void *e;
    void **end;
    const char *(*getName)(void *entry);

    getName = dict->schema->getNameCB;
    end = (void **)fn_8047E0E8(dict->entries);
    for (entry = (void **)fn_8047E0E0(dict->entries); entry != end; entry++) {
        e = *entry;
        if (RWSRCGLOBAL(vecStrcmp)(getName(e), name) == 0) {
            return e;
        }
    }
    return 0;
}

/* Takes a dictionary off the schema's list (only inlined in retail). */
static RtDictSchema *SchemaRemoveDict(RtDictSchema *schema, RtDict *dict) {
    RtDict **entry = (RtDict **)fn_8047E0E0(schema->dictionaries);
    RtDict **end = (RtDict **)fn_8047E0E8(schema->dictionaries);

    for (; entry != end; entry++) {
        if (dict == *entry) {
            *entry = *(RtDict **)fn_8047E0CC(schema->dictionaries, fn_8047E03C(schema->dictionaries) - 1);
            fn_8047DF9C(schema->dictionaries, 1);
            break;
        }
    }
    return schema;
}

/* RtDictDestroy. */
RwBool fn_80479174(RtDict *dict) {
    if (dict == dict->schema->current) {
        dict->schema->current = 0;
    }
    SchemaRemoveDict(dict->schema, dict);
    {
        void **end = (void **)fn_8047E0E8(dict->entries);
        void **entry = (void **)fn_8047E0E0(dict->entries);

        while (entry != end) {
            dict->schema->destroyCB(*entry++);
        }
    }
    fn_8047DFC8(dict->entries);
    RWSRCGLOBAL(rwfree)(dict);
    return 1;
}

/* Adds a dictionary to the schema's list (only inlined in retail). */
static RtDictSchema *SchemaAddDict(RtDictSchema *schema, RtDict *dict) {
    RtDict **entry = (RtDict **)fn_8047DDBC(schema->dictionaries, schema->dictChunkType | 0x30000);

    if (entry == 0) {
        return 0;
    }
    *entry = dict;
    return schema;
}

/* Sets a new dictionary up (only inlined in retail). */
static RtDict *DictInit(RtDict *dict, RtDictSchema *schema) {
    dict->entries = fn_8047DC90(sizeof(void *), schema->dictChunkType | 0x30000);
    if (dict->entries == 0) {
        return 0;
    }
    dict->schema = schema;
    if (SchemaAddDict(schema, dict) == 0) {
        fn_8047DFC8(dict->entries);
        return 0;
    }
    return dict;
}

/* RtDictSchemaCreateDict (only inlined in retail). */
static RtDict *CreateDict(RtDictSchema *schema) {
    RtDict *dict = (RtDict *)RWSRCGLOBAL(rwmalloc)(sizeof(RtDict), schema->dictChunkType | 0x30000);

    if (dict == 0) {
        R5ERROR(0x1BF, (0x80000013, sizeof(RtDict)));
        return 0;
    }
    if (DictInit(dict, schema) == 0) {
        RWSRCGLOBAL(rwfree)(dict);
        return 0;
    }
    return dict;
}

/* RtDictSchemaStreamReadDict. */
RtDict *fn_80479284(RtDictSchema *schema, RwStream *stream) {
    u32 length;
    u32 version;
    u32 numEntries;
    RtDict *dict;

    if (!fn_8047CF50(stream, 1, &length, &version)) {
        return 0;
    }
    if (version < schema->compatibilityVersion) {
        return schema->streamReadCompatibilityCB(stream);
    }
    if (fn_80480D4C(stream, &numEntries, length) == 0) {
        return 0;
    }
    fn_8047D254(&numEntries, 4);
    dict = CreateDict(schema);
    if (dict == 0) {
        return 0;
    }
    while (numEntries--) {
        void *entry;

        if (!fn_8047CF50(stream, schema->elementChunkType, &length, &version) ||
            (entry = schema->streamReadCB(stream)) == 0) {
            fn_80479174(dict);
            return 0;
        }
        *(void **)fn_8047DDBC(dict->entries, dict->schema->dictChunkType | 0x30000) = entry;
        dict->schema->addRefCB(entry);
        schema->destroyCB(entry);
    }
    return dict;
}

/* RtFSyst ----------------------------------------------------------------------------------- */

extern RtFSManager lbl_805E2A40;
extern RtFileSystem *lbl_805F1548; /* the default file system */

s32 fn_80479B48(void);
RwBool fn_80479D08(const char *name);
s32 fn_804798FC(RtFileSystem *fs, const char *deviceName);

/* RtFileSystemInit. Returns an RtFSyst error code. */
s32 fn_80479598(RtFileSystem *fs, s32 maxNbOpenFiles, const char *name, const char *deviceName) {
    if (fn_80479B48()) {
        s32 i;

        if (fn_80479D08(name)) {
            return 6;
        }
        RWSRCGLOBAL(vecStrcpy)(fs->name, name);
        fs->maxNbOpenFiles = maxNbOpenFiles;
        fs->field_0x10 = 0;
        for (i = 0; i < maxNbOpenFiles; i++) {
            RtFile *file = fs->getFileObject(fs, i);

            file->status = 1;
            file->field_0x18 = fs->field_0x10;
        }
    } else {
        return 2;
    }
    fs->deviceName = 0;
    {
        s32 error = fn_804798FC(fs, deviceName);

        switch (error) {
        case 1:
            fs->status = 2;
            fs->registerCB = 0;
            fs->unregisterCB = 0;
            return 1;
        }
        return error;
    }
}

/* The file name without its device prefix ("dev:"). */
const char *fn_80479698(const char *name) {
    const char *p = name;
    s32 i = 0;

    while (i < (s32)RWSRCGLOBAL(vecStrlen)(name)) {
        if (*p == ':') {
            name = i + name;
            name++;
            break;
        }
        i++;
        p++;
    }
    return name;
}

/* Joins a device prefix and a file name into `buffer`, converting the path separators. */
char *fn_8047971C(char *buffer, s32 size, const char *prefix, const char *name, char separator) {
    s32 prefixLength = strlen(prefix);
    s32 nameLength = strlen(name);
    char *dst;
    const char *src;
    s32 i;

    if (prefixLength + nameLength > size - 1) {
        return 0;
    }
    RWSRCGLOBAL(vecStrcpy)(buffer, prefix);
    src = name;
    dst = buffer + prefixLength;
    for (i = 0; i <= nameLength; i++) {
        if (*src == '/' || *src == '\\') {
            *dst = separator;
        } else {
            *dst = *src;
        }
        dst++;
        src++;
    }
    return buffer;
}

RtFile *fn_80479DCC(RtFileSystem *fs, const char *name, u32 flags, void *callBack, void *callBackData);

/* Whether a file exists in a file system: opens it for reading and closes it again. */
RwBool fn_804797F0(RtFileSystem *fs, const char *name) {
    RtFile *file = fn_80479DCC(fs, name, 1, 0, 0);

    if (file != 0) {
        fs->fileClose(file);
        return 1;
    }
    return 0;
}

/* RtFileIsEOF. */
RwBool fn_80479848(RtFile *file) {
    if (file->length == -1) {
        return 1;
    }
    return file->position >= file->length;
}

/* Calls a file system's register (1) or unregister (2) hook with its name. Retail never inlines
 * it (GC/2.7 would). */
#pragma auto_inline off
void fn_80479894(RtFileSystem *fs, s32 what) {
    switch (what) {
    case 1:
        if (fs->registerCB != 0) {
            fs->registerCB(fs->name);
        }
        break;
    case 2:
        if (fs->unregisterCB != 0) {
            fs->unregisterCB(fs->name);
        }
        break;
    }
}
#pragma auto_inline reset

const char *fn_80479FD0(const char *deviceName);

/* RtFileSystemSetDeviceName. */
s32 fn_804798FC(RtFileSystem *fs, const char *deviceName) {
    if (fn_80479FD0(deviceName) == 0) {
        if (fs->deviceName != 0) {
            RWSRCGLOBAL(rwfree)(fs->deviceName);
        }
        fs->deviceName = (char *)RWSRCGLOBAL(rwmalloc)(RWSRCGLOBAL(vecStrlen)(deviceName) + 1, 0x401BE);
        RWSRCGLOBAL(vecStrcpy)(fs->deviceName, deviceName);
    } else {
        return 7;
    }
    return 1;
}

/* Sets whether a file system works asynchronously. */
void fn_804799B8(RtFileSystem *fs, s32 value) {
    fs->field_0x10 = value;
}

s32 fn_804799C0(RtFileSystem *fs) {
    return fs->field_0x10;
}

/* Ends an asynchronous file operation and calls its callback. */
void fn_804799C8(RtFile *file) {
    if (file != 0 && file->field_0x18 != 0) {
        s32 status = file->asyncStatus;

        if (status != 1 && file->asyncCallBack != 0) {
            void *data;

            if (status == 4) {
                data = (void *)(u32)file->length;
            } else {
                data = file->asyncData;
            }
            file->asyncStatus = 1;
            file->asyncCallBack(file, data, file->status, status, file->asyncCallBackData);
        } else {
            file->asyncStatus = 1;
        }
    }
}

/* RtFSManagerOpen. */
RwBool fn_80479A50(s32 maxNbFS) {
    lbl_805E2A40.numFS = 0;
    lbl_805F1548 = 0;
    lbl_805E2A40.maxNbFS = maxNbFS;
    lbl_805E2A40.fsList = 0;
    lbl_805E2A40.isOpen = 1;
    lbl_805E2A40.errorCallback = 0;
    return 1;
}

/* RtFSManagerClose. */
void fn_80479A80(void) {
    if (lbl_805E2A40.isOpen == 1) {
        RtFileSystem *fs = lbl_805E2A40.fsList;

        while (fs != 0) {
            RtFileSystem *next = fs->next;

            fs->close(fs);
            RWSRCGLOBAL(rwfree)(fs->deviceName);
            fs->deviceName = 0;
            fn_80479894(fs, 2);
            RWSRCGLOBAL(rwfree)(fs);
            fs = next;
        }
        lbl_805E2A40.isOpen = 0;
        lbl_805E2A40.numFS = 0;
    }
}

/* Whether one more file system may be registered. */
RwBool fn_80479B48(void) {
    s32 max = lbl_805E2A40.maxNbFS;

    if (max > 0 && lbl_805E2A40.numFS + 1 > max) {
        if (lbl_805E2A40.errorCallback != 0) {
            lbl_805E2A40.errorCallback(5);
        }
        return 0;
    }
    return 1;
}

RtFileSystem *fn_80479C74(const char *name);

/* RtFSManagerRegister. */
RwBool fn_80479BAC(RtFileSystem *fs) {
    if (fn_80479B48()) {
        if (fn_80479C74(fs->name) == 0) {
            fn_80479894(fs, 1);
            fs->next = lbl_805E2A40.fsList;
            lbl_805E2A40.fsList = fs;
            lbl_805E2A40.numFS++;
            return 1;
        }
        return 0;
    }
    return 0;
}

/* RtFSManagerGetFileSystemFromName. */
RtFileSystem *fn_80479C74(const char *name) {
    RtFileSystem *fs;

    for (fs = lbl_805E2A40.fsList; fs != 0; fs = fs->next) {
        if (RWSRCGLOBAL(vecStrcmp)(fs->name, name) == 0) {
            return fs;
        }
    }
    if (lbl_805E2A40.errorCallback != 0) {
        lbl_805E2A40.errorCallback(6);
    }
    return 0;
}

/* Whether a file system of this name is registered already. */
RwBool fn_80479D08(const char *name) {
    if (fn_80479C74(name) != 0) {
        if (lbl_805E2A40.errorCallback != 0) {
            lbl_805E2A40.errorCallback(7);
        }
        return 1;
    }
    return 0;
}

#ifdef __cplusplus
}
#endif
