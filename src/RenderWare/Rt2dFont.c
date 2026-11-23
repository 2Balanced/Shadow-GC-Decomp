/* RenderWare 2D toolkit (Rt2d): fonts, 0x80470AC4..0x80473E9C. Reading a font from a metrics
 * file (name.met along the font path: METRICS1 and METRICS2 bitmap fonts, METRICS3 outline fonts
 * made of paths) or a stream, showing and measuring strings, the list of fonts read so far, and
 * the module's open and close. The functions are the toolkit's font API, kept under their fn_
 * names (the descriptions say which API function each one is): the game reads, shows, measures
 * and destroys fonts, and sets their path and texture dictionary.
 *
 * Source file: its own .sdata2 pool (0x805FB230..0x805FB250; it repeats the brush file's 0.0f),
 * .data (0x8056DCE8..0x8056DD68: the metrics parsers' format strings and the three header
 * lines), .sdata (0x805EDDF8..0x805EDE50: the short strings, the two free lists' parameters,
 * "fonts\\", ".met" and "r"), .bss (0x805E1908..0x805E1950: the two free lists) and .sbss
 * (0x805F1538: the module's open flag). Every function from 0x80470AC4 to 0x80473E8C reads or
 * writes the font members of the toolkit's globals (lbl_805E1950, which the device file after
 * this one defines as the first object of its .bss): fn_80473E8C sets the texture dictionary
 * the readers use. The device file starts with the camera hook fn_80473E9C and pools its
 * constants from 0x805FB250. Old units Game/RwCpp35 (end), RwCpp36 and RwCpp37 (first part).
 *
 * Built as C with CodeWarrior GC 1.3.2, -O4,p, no stmw/lmw, with writable strings
 * (cflags_rw_132_sdata: retail keeps the literals in .data and .sdata).
 * Matching: fn_80472C14, fn_80473118, fn_80473260, fn_80473460, fn_80473528, fn_8047353C,
 * fn_80473584, fn_804736CC, fn_80473CE0, fn_80473D10, fn_80473D38, fn_80473E8C.
 * fn_80470AC4 differs in the order of two prologue moves, fn_80471300 in its unrolled fill
 * loop, fn_80472A60 in two registers of its texture loop, fn_804737E4 in four saved registers,
 * fn_8047149C (96%) in its saved registers. fn_80470C54 (the bitmap show function),
 * fn_80471A54 (METRICS2) and fn_804724AC (METRICS3) are not written yet. */
#include <dolphin/types.h>
#include "Game/rw/r4.h"

/* The 2D toolkit's current transformation matrix (the device file). */
void fn_804747A0(void);             /* Rt2dCTMPush */
void fn_804748C8(void);             /* Rt2dCTMPop */
void fn_804748FC(f32 x, f32 y);     /* Rt2dCTMScale */
void fn_80474964(f32 x, f32 y);     /* Rt2dCTMTranslate */
void fn_80473FD0(RwV2d *origin, RwV2d *xstep, RwV2d *ystep); /* Rt2dDeviceGetMetric */
f32 fn_804823F0(const RwV2d *v);    /* RwV2dLength */

/* Paths (the path files). */
void fn_804771C0(Rt2dPath *path, Rt2dBrush *brush); /* Rt2dPathFill */
s32 fn_80475E2C(Rt2dPath *path);                    /* Rt2dPathDestroy */

/* rwSList, a growable array (RwCpp43). */
RwSList *fn_8047DC90(s32 entrySize, u32 hint); /* create */
void *fn_8047DFAC(RwSList *list);              /* the entries */
s32 fn_8047DFC8(RwSList *list);                /* destroy */

/* Streams, files and textures. */
void *fn_8047D784(RwStream *stream, f32 *reals, u32 size);   /* RwStreamReadReal */
void *fn_8047D868(RwStream *stream, s32 *ints, u32 size);    /* RwStreamReadInt32 */
u32 fn_80480D4C(RwStream *stream, void *buffer, u32 length); /* RwStreamRead */
void *fn_8047A178(const RwChar *name, const RwChar *mode);   /* opens a file */
RwChar *fn_8047A518(RwChar *buffer, s32 size, void *file);   /* reads a line */
s32 fn_8047A388(void *file);                                 /* closes a file */
RwTexture *fn_8048D130(const RwChar *name, const RwChar *mask); /* RwTextureRead */
RwTexDictionary *fn_8048CA64(void);                          /* RwTexDictionaryCreate */
RwTexDictionary *fn_8048D0D8(void);                          /* RwTexDictionaryGetCurrent */
RwBool fn_8047A67C(const RwChar *name);                      /* whether a file exists */
void fn_804885DC(const RwChar *path);                        /* RwImageSetPath */
RwTexDictionary *fn_8048D0C4(RwTexDictionary *dict);         /* RwTexDictionarySetCurrent */
void *fn_8047D684(RwStream *stream, s16 *ints, u32 size);    /* RwStreamReadInt16 */
RwBool fn_8047CF50(RwStream *stream, u32 type, u32 *lengthOut, u32 *versionOut); /* RwStreamFindChunk */
void *fn_8047DEA0(RwSList *list, s32 count, u32 hint);       /* rwSList: count new entries */
Rt2dPath *fn_80476C4C(RwStream *stream);                     /* Rt2dPathStreamRead */
s32 fn_8048CB38(RwTexDictionary *dict);                      /* RwTexDictionaryDestroy */

Rt2dFont *fn_80470C54(Rt2dFont *font, const RwChar *string, f32 height, RwV2d *anchor,
                      Rt2dBrush *brush);
Rt2dFont *fn_80470AC4(Rt2dFont *font, const RwChar *text, const RwV2d *anchor, Rt2dBrush *brush,
                      f32 height);
Rt2dFont *fn_80472A60(Rt2dFont *font, RwStream *stream);
Rt2dFont *fn_80471300(Rt2dFont *font);
Rt2dFont *fn_80471A54(Rt2dFont *font, RwChar *buffer, void *file, RwChar *line);
Rt2dFont *fn_804724AC(Rt2dFont *font, RwChar *buffer, void *file, RwChar *line);
s32 fn_804736CC(Rt2dFont *font);
Rt2dFont *fn_804737E4(const RwChar *name);
const RwChar *fn_80473460(const RwChar *path);
u32 fn_80473528(Rt2dFont *(*callback)(const RwChar *name));

/* Shows `string` with an outline font from `anchor`, each glyph's path filled with `brush` and
 * scaled by `height` (the show function of METRICS3 fonts). */
Rt2dFont *fn_80470AC4(Rt2dFont *font, const RwChar *text, const RwV2d *anchor, Rt2dBrush *brush,
                      f32 height) {
    Rt2dFontGlyph *glyphs;
    const u8 *string;
    s32 step;
    u8 mask;
    s32 hi;
    s32 shiftLo;
    s32 shiftHi;
    s32 code;

    string = (const u8 *)text;
    if (font != NULL) {
        glyphs = (Rt2dFontGlyph *)fn_8047DFAC(font->glyphs);
        if (font->flag & 2) {
            step = 2;
            mask = 0xFF;
            hi = 1;
            shiftLo = 8;
            shiftHi = 0;
        } else {
            step = 1;
            mask = 0;
            hi = 0;
            shiftLo = 0;
            shiftHi = 0;
        }
        fn_804747A0();
        fn_80474964(anchor->x, anchor->y);
        fn_804748FC(height, height);
        fn_80474964(0.0f, -font->baseline);
        code = (string[0] << shiftLo) | ((mask & string[hi]) << shiftHi);
        while (code > 0) {
            s32 glyph;

            if (code >= 0x80) {
                s32 ext = code - font->wideBase;
                if (ext >= 0 && ext < font->wideCount) {
                    glyph = font->wideMap[ext];
                } else {
                    glyph = -1;
                }
            } else {
                glyph = font->map[code];
            }
            string += step;
            if (glyph >= 0) {
                Rt2dFontGlyph *g = &glyphs[glyph];
                if (g->texture == NULL) {
                    fn_804771C0(g->path, brush);
                    fn_80474964(g->width + font->intergap, 0.0f);
                }
            }
            code = (string[0] << shiftLo) | ((mask & string[hi]) << shiftHi);
        }
        fn_804748C8();
    }
    return font;
}

/* Builds a font's table of glyphs for the character codes from 0x80 on (the codes are in the
 * toolkit's glyph code list while the font is read). */
Rt2dFont *fn_80471300(Rt2dFont *font) {
    s32 lowest;
    s32 num;
    Rt2dGlobals *globals;
    s32 highest;
    s32 found;
    s32 range;
    s16 *table;
    s32 *codes;
    s32 i;

    globals = &lbl_805E1950;
    lowest = 0xFFFF;
    highest = 0;
    found = 0;
    codes = globals->glyphCodes;
    for (i = 0; i < font->glyphCount; i++) {
        s32 code = codes[i];
        if (code > 0) {
            if (code < lowest) {
                lowest = code;
            }
            if (code > highest) {
                highest = code;
            }
            found = 1;
        }
    }
    if (found > 0) {
        range = highest - lowest;
        num = range + 1;
        table = (s16 *)RwMalloc(num * 2, 0x30190);
        if (table == NULL) {
            return NULL;
        }
        for (i = 0; i < range + 1; i++) {
            table[i] = -1;
        }
        for (i = 0; i < font->glyphCount; i++) {
            s32 code = globals->glyphCodes[i];
            if (code > 0) {
                table[code - lowest] = i;
            }
        }
        font->wideCount = num;
        font->wideBase = lowest;
        font->wideMap = table;
    }
    return font;
}

/* Reads a METRICS1 font (a bitmap font in one texture): the texture's name (and its mask's, or
 * an "m" file next to it), the height of a line in pixels, then a line for each glyph with its
 * character code and its rectangle in the texture. */
Rt2dFont *fn_8047149C(Rt2dFont *font, RwChar *buffer, void *file, RwChar *line) {
    RwTexDictionary *current;
    Rt2dFont *result;
    RwChar *name;
    RwTexture *texture;
    s32 numItems;
    s32 numGlyphs;
    s32 capacity;
    s32 *codes;
    s16 *map;
    Rt2dFontGlyph *glyphs;
    s32 width;
    s32 height;
    s32 lineHeight;
    s32 code;
    s32 left;
    s32 top;
    s32 right;
    s32 bottom;
    f32 invWidth;
    f32 invHeight;

    current = fn_8048D0D8();
    fn_8048D0C4(lbl_805E1950.currentTexDict);
    result = font;
    font->isOutline = 0;
    numGlyphs = 0;
    capacity = font->glyphCount;
    if (capacity == 0) {
        capacity = 0x100;
    }
    codes = lbl_805E1950.glyphCodes;
    map = font->map;
    fn_8047DEA0(font->glyphs, capacity, 0x30190);
    glyphs = (Rt2dFontGlyph *)fn_8047DFAC(font->glyphs);
    memset(glyphs, 0, capacity * sizeof(Rt2dFontGlyph));
    fn_8047A518(line, 0x100, file);
    name = lbl_805E1950.fontName;
    numItems = lbl_805F15B4->stringFuncs.rwsscanf(line, "%s %s\n", buffer, name);
    if (numItems == 1 || numItems == 2) {
        fn_804885DC(lbl_805E1950.fontPath);
        if (numItems == 1) {
            lbl_805F15B4->stringFuncs.vecSprintf(name, "%sm%s", lbl_805E1950.fontPath, buffer);
            if (fn_8047A67C(name)) {
                lbl_805F15B4->stringFuncs.vecSprintf(name, "m%s", buffer);
                texture = fn_8048D130(buffer, name);
            } else {
                texture = fn_8048D130(buffer, NULL);
            }
        } else {
            texture = fn_8048D130(buffer, name);
        }
        if (texture != NULL) {
            font->texture[0] = texture;
            width = texture->raster->width;
            height = texture->raster->height;
            fn_8047A518(line, 0x100, file);
            lbl_805F15B4->stringFuncs.rwsscanf(line, "%d", &lineHeight);
            invWidth = 1.0f / (f32)width;
            invHeight = 1.0f / (f32)height;
            while (result == font && fn_8047A518(line, 0x100, file) != NULL) {
                if (lbl_805F15B4->stringFuncs.rwsscanf(line, "%i %i %i %i %i\n", &code, &left, &top,
                                                         &right, &bottom) == 5) {
                    Rt2dFontGlyph *glyph;

                    if (code > 0xFFFF) {
                        result = NULL;
                        continue;
                    }
                    if (numGlyphs >= lbl_805E1950.numGlyphCodes) {
                        s32 newCount = lbl_805E1950.numGlyphCodes + 32;
                        s32 *newCodes = (s32 *)RwMalloc(newCount * sizeof(s32), 0x30190);
                        s32 k;

                        memcpy(newCodes, codes, lbl_805E1950.numGlyphCodes * sizeof(s32));
                        for (k = lbl_805E1950.numGlyphCodes; k < newCount; k++) {
                            newCodes[k] = -1;
                        }
                        if (codes != NULL) {
                            RwFree(codes);
                        }
                        lbl_805E1950.glyphCodes = newCodes;
                        codes = newCodes;
                        lbl_805E1950.numGlyphCodes = newCount;
                    }
                    if (code >= 0x80) {
                        font->flag |= 2;
                        codes[numGlyphs] = code;
                    } else {
                        map[code] = numGlyphs;
                    }
                    glyph = &glyphs[numGlyphs];
                    numGlyphs++;
                    glyph->rect[0] = (0.5f + (f32)left) * invWidth;
                    glyph->rect[1] = (0.5f + (f32)top) * invHeight;
                    glyph->rect[2] = (0.5f + (f32)right) * invWidth;
                    glyph->rect[3] = (0.5f + (f32)bottom) * invHeight;
                    glyph->texture = texture;
                    glyph->width = (f32)(right - left) / (f32)(bottom - top);
                    glyph->textureIndex = 0;
                    if (numGlyphs >= capacity) {
                        fn_8047DEA0(font->glyphs, 32, 0x30190);
                        glyphs = (Rt2dFontGlyph *)fn_8047DFAC(font->glyphs);
                        memset(&glyphs[numGlyphs], 0, 32 * sizeof(Rt2dFontGlyph));
                        capacity += 32;
                    }
                }
            }
            if (result == font) {
                font->glyphCount = numGlyphs;
                fn_80471300(font);
                if (lbl_805E1950.glyphCodes != NULL) {
                    memset(lbl_805E1950.glyphCodes, -1, lbl_805E1950.numGlyphCodes * sizeof(s32));
                }
                font->height = (f32)(bottom - top);
                font->baseline = (f32)lineHeight / font->height;
                font->intergap = 0.0f;
            }
        } else {
            result = NULL;
        }
    }
    fn_8048D0C4(current);
    return result;
}

/* A new, empty font. */
static Rt2dFont *FontCreate(void) {
    Rt2dFont *font = (Rt2dFont *)RwFreeListAlloc(lbl_805E1950.fontFreeList, 0x30199);
    u32 i;

    if (font == 0) {
        return 0;
    }
    font->isOutline = 1;
    font->height = 1.0f;
    font->baseline = 0.0f;
    font->intergap = 0.0f;
    font->flag = 0;
    for (i = 0; i < 4; i++) {
        font->texture[i] = 0;
    }
    font->wideCount = 0;
    font->glyphCount = 0;
    font->wideBase = 0;
    font->wideMap = 0;
    font->glyphs = fn_8047DC90(sizeof(Rt2dFontGlyph), 0x30190);
    font->show = fn_80470C54;
    font->unk13C = 0;
    font->unk140 = 0;
    for (i = 0; i < 128; i++) {
        font->map[i] = -1;
    }
    return font;
}

/* Reads the glyphs and the textures of a bitmap font from `stream`. */
Rt2dFont *fn_80472A60(Rt2dFont *font, RwStream *stream) {
    char name[32];
    s32 numTextures;
    Rt2dFontGlyph *glyph;
    s32 i;
    Rt2dFontGlyph *glyphs;

    glyphs = (Rt2dFontGlyph *)fn_8047DFAC(font->glyphs);
    glyph = glyphs;
    for (i = 0; i < font->glyphCount; i++) {
        if (fn_8047D784(stream, glyph->rect, 16) == NULL) {
            return NULL;
        }
        if (fn_8047D784(stream, &glyph->width, 4) == NULL) {
            return NULL;
        }
        if (fn_80480D4C(stream, &glyph->textureIndex, 1) != 1) {
            return NULL;
        }
        glyph->path = NULL;
        glyph++;
    }
    if (fn_8047D868(stream, &numTextures, 4) == NULL) {
        return NULL;
    }
    for (i = 0; i < numTextures; i++) {
        if (fn_80480D4C(stream, name, 32) != 32) {
            return NULL;
        }
        if ((font->texture[i] = fn_8048D130(name, NULL)) == NULL) {
            return NULL;
        }
    }
    glyph = glyphs;
    for (i = 0; i < font->glyphCount; i++) {
        glyph->texture = font->texture[glyph->textureIndex];
        if (glyph->texture->raster == NULL) {
            return NULL;
        }
        glyph++;
    }
    font->unk13C = 0;
    font->unk140 = 0;
    font->show = fn_80470C54;
    return font;
}

/* Reads the paths of an outline font's glyphs from `stream`: each glyph's width, and its path
 * when the stream holds one (marked by 5, in a path chunk). */
static Rt2dFont *FontStreamReadOutline(Rt2dFont *font, RwStream *stream) {
    Rt2dFontGlyph *glyph = (Rt2dFontGlyph *)fn_8047DFAC(font->glyphs);
    s32 i;

    for (i = 0; i < font->glyphCount; i++) {
        s32 value;
        Rt2dPath *path = NULL;

        if (fn_8047D868(stream, &value, 4) == NULL) {
            return NULL;
        }
        if (fn_8047D784(stream, &glyph->width, 4) == NULL) {
            return NULL;
        }
        if (fn_8047D868(stream, &value, 4) == NULL) {
            return NULL;
        }
        if (value == 5) {
            if (fn_8047CF50(stream, 0x1A1, NULL, NULL) == 0) {
                return NULL;
            }
            path = fn_80476C4C(stream);
            if (fn_8047D868(stream, &value, 4) == NULL) {
                return NULL;
            }
        }
        if (fn_8047D868(stream, &value, 4) == NULL) {
            return NULL;
        }
        glyph->path = path;
        glyph++;
    }
    font->show = (Rt2dFontShowFunc)fn_80470AC4;
    return font;
}

/* Rt2dFontStreamRead: a font from `stream` (an outline font's paths, or a bitmap font's glyphs
 * and textures, read with the toolkit's texture dictionary current). */
Rt2dFont *fn_80472C14(RwStream *stream) {
    Rt2dFont *font = FontCreate();
    RwUInt32 version;

    if (font == NULL) {
        return NULL;
    }
    if (fn_8047D868(stream, (s32 *)&version, 4) == NULL) {
        return NULL;
    }
    if ((RwInt32)(version >> 24) < 1) {
        return NULL;
    }
    if (fn_8047D868(stream, &font->isOutline, 4) == NULL) {
        return NULL;
    }
    if (fn_8047D784(stream, &font->height, 4) == NULL) {
        return NULL;
    }
    if (fn_8047D784(stream, &font->baseline, 4) == NULL) {
        return NULL;
    }
    if (fn_8047D784(stream, &font->intergap, 4) == NULL) {
        return NULL;
    }
    if (fn_8047D868(stream, (s32 *)&font->flag, 4) == NULL) {
        return NULL;
    }
    if (fn_8047D868(stream, &font->wideBase, 4) == NULL) {
        return NULL;
    }
    if (fn_8047D868(stream, &font->glyphCount, 4) == NULL) {
        return NULL;
    }
    if (fn_8047D868(stream, &font->wideCount, 4) == NULL) {
        return NULL;
    }
    if (font->wideCount > 0) {
        font->wideMap = (s16 *)RwMalloc(font->wideCount * 2, 0x30190);
        memset(font->wideMap, 0, font->wideCount * 2);
        if (font->wideMap == NULL) {
            fn_804736CC(font);
            return NULL;
        }
        if (fn_8047D684(stream, font->wideMap, font->wideCount * 2) == NULL) {
            return NULL;
        }
    }
    if (fn_8047D684(stream, font->map, 0x100) == NULL) {
        return NULL;
    }
    fn_8047DEA0(font->glyphs, font->glyphCount, 0x30190);
    memset(fn_8047DFAC(font->glyphs), 0, font->glyphCount * sizeof(Rt2dFontGlyph));
    if (font->isOutline) {
        if (FontStreamReadOutline(font, stream) == NULL) {
            fn_804736CC(font);
            font = NULL;
        }
    } else {
        RwTexDictionary *current = fn_8048D0D8();

        fn_8048D0C4(lbl_805E1950.currentTexDict);
        if (fn_80472A60(font, stream) == NULL) {
            fn_804736CC(font);
            font = NULL;
        }
        fn_8048D0C4(current);
    }
    return font;
}

/* The parameters of the fonts' and the font list's free lists, and their storage. */
static RwInt32 fontFreeListBlockSize = 8;
static RwInt32 fontFreeListPreallocBlocks = 1;
static RwInt32 fontDictFreeListBlockSize = 16;
static RwInt32 fontDictFreeListPreallocBlocks = 1;
static RwFreeList fontFreeList;
static RwFreeList fontDictFreeList;
static RwInt32 fontModuleOpen;

/* Closes the fonts: every font read, the texture dictionary, the free lists and the buffers. */
void fn_80473118(void) {
    Rt2dFontDictNode *node = lbl_805E1950.fontDict;

    while (node != NULL) {
        Rt2dFontDictNode *next;

        RwFree(node->name);
        fn_804736CC(node->font);
        next = node->next;
        RwFreeListFree(lbl_805E1950.dictFreeList, node);
        node = next;
    }
    lbl_805E1950.fontDict = NULL;
    if (lbl_805E1950.texDict != NULL) {
        fn_8048CB38(lbl_805E1950.texDict);
    }
    lbl_805E1950.texDict = NULL;
    lbl_805E1950.currentTexDict = NULL;
    fn_8047FCDC(lbl_805E1950.dictFreeList);
    lbl_805E1950.dictFreeList = NULL;
    fn_8047FCDC(lbl_805E1950.fontFreeList);
    lbl_805E1950.fontFreeList = NULL;
    if (lbl_805E1950.glyphCodes != NULL) {
        RwFree(lbl_805E1950.glyphCodes);
    }
    lbl_805E1950.glyphCodes = NULL;
    if (lbl_805E1950.fontPath != NULL) {
        RwFree(lbl_805E1950.fontPath);
    }
    lbl_805E1950.fontPath = NULL;
    lbl_805E1950.fontPathSize = 0;
    fontModuleOpen = 0;
}

/* Opens the fonts: their free lists, the path buffer (set to "fonts\\"), the glyph quads'
 * index table and the texture dictionary. Returns 0 when out of memory. */
s32 fn_80473260(void) {
    u16 base;
    s32 i;
    u16 (*quads)[6];

    lbl_805E1950.fontFreeList = fn_8047FC04(0x144, fontFreeListBlockSize, 4,
                                            fontFreeListPreallocBlocks, &fontFreeList, 0x40199);
    if (lbl_805E1950.fontFreeList == NULL) {
        return 0;
    }
    lbl_805E1950.dictFreeList = fn_8047FC04(sizeof(Rt2dFontDictNode), fontDictFreeListBlockSize, 4,
                                            fontDictFreeListPreallocBlocks, &fontDictFreeList,
                                            0x40190);
    if (lbl_805E1950.dictFreeList == NULL) {
        return 0;
    }
    lbl_805E1950.fontPath = (RwChar *)RwMalloc(0x51, 0x1040190);
    if (lbl_805E1950.fontPath == NULL) {
        return 0;
    }
    lbl_805E1950.fontPathSize = 0x51;
    fn_80473460("fonts\\");
    quads = lbl_805E1950.quadIndices;
    base = 0;
    for (i = 0; i < 16; i++) {
        u16 idx = base;
        quads[i][0] = base;
        quads[i][1] = idx + 2;
        quads[i][2] = idx + 3;
        quads[i][3] = base;
        quads[i][4] = idx + 3;
        quads[i][5] = idx + 1;
        base += 4;
    }
    lbl_805E1950.fontDict = NULL;
    lbl_805E1950.currentTexDict = lbl_805E1950.texDict = fn_8048CA64();
    lbl_805E1950.glyphCodes = NULL;
    lbl_805E1950.numGlyphCodes = 0;
    fn_80473528(fn_804737E4);
    fontModuleOpen = 1;
    return 1;
}

/* Rt2dFontSetPath: the directories fonts are read from (copied into the toolkit's buffer). */
const RwChar *fn_80473460(const RwChar *path) {
    if (path != NULL) {
        u32 size = rwstrlen(path) + 1;

        if (size > lbl_805E1950.fontPathSize) {
            RwChar *buffer = (RwChar *)RwRealloc(lbl_805E1950.fontPath, size, 0x1040190);
            if (buffer == NULL) {
                return NULL;
            }
            lbl_805E1950.fontPath = buffer;
            lbl_805E1950.fontPathSize = size;
        }
        rwstrcpy(lbl_805E1950.fontPath, path);
        return path;
    }
    return NULL;
}

/* Sets the function that reads a font (fn_80473CE0 calls it). */
u32 fn_80473528(Rt2dFont *(*callback)(const RwChar *name)) {
    lbl_805E1950.fontRead = callback;
    return 1;
}

/* Rt2dFontGetHeight: the font's height in the current 2D space (the length of the device's
 * y step). */
f32 fn_8047353C(Rt2dFont *font) {
    RwV2d origin;
    RwV2d xstep;
    RwV2d ystep;

    fn_80473FD0(&origin, &xstep, &ystep);
    return font->height * fn_804823F0(&xstep);
}

/* Rt2dFontGetStringWidth: the width of `string` in the font, scaled by `height`. */
f32 fn_80473584(Rt2dFont *font, const u8 *string, f32 height) {
    f32 width = 0.0f;
    Rt2dFontGlyph *glyphs = (Rt2dFontGlyph *)fn_8047DFAC(font->glyphs);
    const u8 *p;
    s32 step;
    s32 wide;
    s32 next;
    s32 shiftHigh;
    s32 shiftLow;
    u8 mask;
    s32 c;
    s32 glyph;

    if (font->flag & 2) {
        step = 2;
        mask = 0xFF;
        next = 1;
        shiftHigh = 8;
        shiftLow = 0;
    } else {
        step = 1;
        mask = 0;
        next = 0;
        shiftHigh = 0;
        shiftLow = 0;
    }

    c = (string[0] << shiftHigh) | ((mask & string[next]) << shiftLow);
    p = string;
    while (c > 0) {
        if (c >= 0x80) {
            wide = c - font->wideBase;
            if (wide >= 0 && wide < font->wideCount) {
                glyph = font->wideMap[wide];
            } else {
                glyph = -1;
            }
        } else {
            glyph = font->map[c];
        }
        p += step;
        if (glyph >= 0) {
            {
                Rt2dFontGlyph *g = &glyphs[glyph];
                width += g->width + font->intergap;
            }
        }
        c = (p[0] << shiftHigh) | ((mask & p[next]) << shiftLow);
    }
    return width * height;
}

/* Rt2dFontDestroy: its glyph paths, its glyph list, the wide map and the textures. */
s32 fn_804736CC(Rt2dFont *font) {
    if (font != 0) {
        u32 i;

        if (font->glyphs != 0) {
            Rt2dFontGlyph *glyph = (Rt2dFontGlyph *)fn_8047DFAC(font->glyphs);
            for (i = 0; i < font->glyphCount; i++) {
                if (glyph[i].path != 0) {
                    fn_80475E2C(glyph[i].path);
                    glyph[i].path = 0;
                }
            }
            fn_8047DFC8(font->glyphs);
        }
        font->glyphs = 0;
        if (font->wideMap != 0) {
            RwFree(font->wideMap);
        }
        font->wideMap = 0;
        for (i = 0; i < 4; i++) {
            if (font->texture[i] != 0) {
                fn_8048CD8C(font->texture[i]);
            }
            font->texture[i] = 0;
        }
        RwFreeListFree(lbl_805E1950.fontFreeList, font);
        return 1;
    }
    return 0;
}


/* The default font reader (fn_80473CE0 calls it): looks for name.met along the font path and
 * hands the file to the reader of its metrics format.
 * NonMatching (97.7%): same instructions, but retail keeps the file in r28, the path cursor in
 * r27, the path in r26 and the name in r25. */
Rt2dFont *fn_804737E4(const RwChar *name) {
    Rt2dFont *font = FontCreate();
    RwChar line[0x100];
    RwChar buffer[0x80];
    const RwChar *p;
    void *file;
    s32 i;
    s32 c;
    Rt2dFont *result;
    RwChar *path;
    s32 length;

    if (font == 0) {
        return 0;
    }
    length = rwstrlen(lbl_805E1950.fontPath);
    i = rwstrlen(name) + 5;
    path = (RwChar *)RwMalloc(length + i, 0x10190);
    if (path != 0) {
        i = 0;
        p = lbl_805E1950.fontPath;
        file = 0;
        do {
            c = *(const u8 *)p;
            if (c == ';' || (RwChar)c == 0) {
                path[i] = 0;
                lbl_805F15B4->stringFuncs.rwstrcat(path, name);
                lbl_805F15B4->stringFuncs.rwstrcat(path, ".met");
                file = fn_8047A178(path, "r");
                i = 0;
            } else {
                path[i] = c;
                i++;
            }
            p++;
        } while ((RwChar)c != 0 && file == 0);

        result = 0;
        if (file != 0) {
            fn_8047A518(line, sizeof(line), file);
            if (lbl_805F15B4->stringFuncs.rwstrcmp(line, "METRICS1\n") == 0) {
                result = fn_8047149C(font, buffer, file, line);
            } else if (lbl_805F15B4->stringFuncs.rwstrcmp(line, "METRICS2\n") == 0) {
                result = fn_80471A54(font, buffer, file, line);
            } else if (lbl_805F15B4->stringFuncs.rwstrcmp(line, "METRICS3\n") == 0) {
                result = fn_804724AC(font, buffer, file, line);
            }
            fn_8047A388(file);
        }
        RwFree(path);
        if (result != font) {
            fn_804736CC(font);
            font = 0;
        }
    } else {
        fn_804736CC(font);
        font = 0;
    }
    return font;
}

/* Rt2dFontRead: reads a font through the toolkit's reader. */
Rt2dFont *fn_80473CE0(const RwChar *name) {
    return lbl_805E1950.fontRead(name);
}

/* Rt2dFontShow: shows a string through the font's own show function. */
Rt2dFont *fn_80473D10(Rt2dFont *font, const RwChar *string, f32 height, RwV2d *anchor,
                      Rt2dBrush *brush) {
    return font->show(font, string, height, anchor, brush);
}

/* Lists the characters the font has a glyph for; returns their count. */
s32 fn_80473D38(Rt2dFont *font, s32 *chars) {
    s32 i;
    s32 count = 0;

    for (i = 0; i < 128; i++) {
        if (font->map[i] >= 0) {
            chars[count] = i;
            count++;
        }
    }
    for (i = 0; (u32)i < (u32)font->wideCount; i++) {
        if (font->wideMap[i] >= 0) {
            chars[count] = i + font->wideBase;
            count++;
        }
    }
    return count;
}

/* Rt2dFontTexDictionarySet: the texture dictionary fonts read their textures from. */
RwTexDictionary *fn_80473E8C(RwTexDictionary *dict) {
    lbl_805E1950.currentTexDict = dict;
    return dict;
}
