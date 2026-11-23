/* RenderWare GameCube driver, charsets (bitmap fonts), 0x80471A54..0x80473584: reading a
 * charset file, the list of loaded charsets, and the module's open and close. Names are
 * inferred from behaviour. Compiled by CodeWarrior GC 1.3.2 with -O4,p. Matching:
 * fn_80473118, fn_80473260, fn_80473460, fn_80473528, fn_8047353C. fn_80472A60 differs in the
 * registers of its texture loop (the counter and the pointer swap). fn_80471A54, fn_804724AC and
 * fn_80472C14 are not written yet. */
#include <dolphin/types.h>
#include "Game/rw/r4.h"

extern "C" {
extern RwGameCubeDriver lbl_805E1950;
extern RwFreeList lbl_805E1908;
extern RwFreeList lbl_805E192C;
extern s32 lbl_805EDE28;
extern s32 lbl_805EDE2C;
extern s32 lbl_805EDE30;
extern s32 lbl_805EDE34;
extern u32 lbl_805F1538;
/* "fonts\\": the charset source file's .sdata string (0x805EDE38), shared with RwCpp35.cpp and
 * RwCpp37.cpp; referenced by address until the units are re-cut along that file. */
extern char lbl_805EDE38[7];

RwFreeList *fn_8047FC04(s32 entrySize, s32 entriesPerBlock, s32 alignment, s32 numBlocks,
                        RwFreeList *freeList, u32 hint); /* RwFreeListCreateAndPreallocateSpace */
void fn_8047FCDC(RwFreeList *freeList);                  /* RwFreeListDestroy */
void fn_804736CC(void *charset);
void fn_804737E4(void);
void *fn_8048CA64(void);
void fn_8048CB38(void *dict);
void fn_80473FD0(RwV2d *a, RwV2d *b, RwV2d *c);
f32 fn_804823F0(const RwV2d *v);
const char *fn_80473460(const char *path);
u32 fn_80473528(void *callback);

void *fn_8047DFAC(void *data);
void *fn_8047D784(void *stream, f32 *reals, u32 size);   /* RwStreamReadReal */
void *fn_8047D868(void *stream, s32 *ints, u32 size);    /* RwStreamReadInt32 */
u32 fn_80480D4C(void *stream, void *buffer, u32 length); /* RwStreamRead */
RwTexture *fn_8048D130(const char *name, const char *mask); /* RwTextureRead */
void fn_80470C54(void);

/* Reads the glyphs and the textures of a texture charset from `stream`. */
RwGameCubeCharset *fn_80472A60(RwGameCubeCharset *charset, void *stream) {
    char name[32];
    s32 numTextures;
    RwGameCubeGlyph *glyph;
    s32 i;
    RwGameCubeGlyph *glyphs;

    glyphs = (RwGameCubeGlyph *)fn_8047DFAC(charset->glyphData);
    glyph = glyphs;
    for (i = 0; i < charset->numGlyphs; i++) {
        if (fn_8047D784(stream, glyph->rect, 16) == NULL) {
            return NULL;
        }
        if (fn_8047D784(stream, &glyph->advance, 4) == NULL) {
            return NULL;
        }
        if (fn_80480D4C(stream, &glyph->textureIndex, 1) != 1) {
            return NULL;
        }
        glyph->image = NULL;
        glyph++;
    }
    if (fn_8047D868(stream, &numTextures, 4) == NULL) {
        return NULL;
    }
    for (i = 0; i < numTextures; i++) {
        if (fn_80480D4C(stream, name, 32) != 32) {
            return NULL;
        }
        if ((charset->textures[i] = fn_8048D130(name, NULL)) == NULL) {
            return NULL;
        }
    }
    glyph = glyphs;
    for (i = 0; i < charset->numGlyphs; i++) {
        glyph->texture = charset->textures[glyph->textureIndex];
        if (glyph->texture->raster == NULL) {
            return NULL;
        }
        glyph++;
    }
    charset->unk13C = 0;
    charset->unk140 = 0;
    charset->draw = (void *)fn_80470C54;
    return charset;
}

/* Closes the charsets: every loaded charset, the texture dictionary, the free lists and the
 * buffers. */
void fn_80473118(void) {
    RwGameCubeCharsetNode *node = lbl_805E1950.charsets;

    while (node != NULL) {
        RwGameCubeCharsetNode *next;

        RwFree(node->name);
        fn_804736CC(node->charset);
        next = node->next;
        RwFreeListFree(lbl_805E1950.nodeFreeList, node);
        node = next;
    }
    lbl_805E1950.charsets = NULL;
    if (lbl_805E1950.texDictionary != NULL) {
        fn_8048CB38(lbl_805E1950.texDictionary);
    }
    lbl_805E1950.texDictionary = NULL;
    lbl_805E1950.currentTexDictionary = NULL;
    fn_8047FCDC(lbl_805E1950.nodeFreeList);
    lbl_805E1950.nodeFreeList = NULL;
    fn_8047FCDC(lbl_805E1950.charsetFreeList);
    lbl_805E1950.charsetFreeList = NULL;
    if (lbl_805E1950.glyphCodes != NULL) {
        RwFree(lbl_805E1950.glyphCodes);
    }
    lbl_805E1950.glyphCodes = NULL;
    if (lbl_805E1950.path != NULL) {
        RwFree(lbl_805E1950.path);
    }
    lbl_805E1950.path = NULL;
    lbl_805E1950.pathSize = 0;
    lbl_805F1538 = 0;
}

/* Opens the charsets: their free lists, the path buffer (set to "fonts\\"), the quad index
 * table and the texture dictionary. Returns 0 when out of memory. */
s32 fn_80473260(void) {
    u16 base;
    s32 i;
    u16 (*quads)[6];

    lbl_805E1950.charsetFreeList = fn_8047FC04(0x144, lbl_805EDE28, 4, lbl_805EDE2C, &lbl_805E1908,
                                               0x40199);
    if (lbl_805E1950.charsetFreeList == NULL) {
        return 0;
    }
    lbl_805E1950.nodeFreeList = fn_8047FC04(sizeof(RwGameCubeCharsetNode), lbl_805EDE30, 4,
                                            lbl_805EDE34, &lbl_805E192C, 0x40190);
    if (lbl_805E1950.nodeFreeList == NULL) {
        return 0;
    }
    lbl_805E1950.path = (char *)RwMalloc(0x51, 0x1040190);
    if (lbl_805E1950.path == NULL) {
        return 0;
    }
    lbl_805E1950.pathSize = 0x51;
    fn_80473460(lbl_805EDE38);
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
    lbl_805E1950.charsets = NULL;
    lbl_805E1950.currentTexDictionary = lbl_805E1950.texDictionary = fn_8048CA64();
    lbl_805E1950.glyphCodes = NULL;
    lbl_805E1950.numGlyphCodes = 0;
    fn_80473528(fn_804737E4);
    lbl_805F1538 = 1;
    return 1;
}

/* Sets the directory charsets are read from (copied into the driver's buffer). */
const char *fn_80473460(const char *path) {
    if (path != NULL) {
        u32 size = lbl_805F15B4->stringFuncs.rwstrlen(path) + 1;

        if (size > lbl_805E1950.pathSize) {
            char *buffer = (char *)RwRealloc(lbl_805E1950.path, size, 0x1040190);
            if (buffer == NULL) {
                return NULL;
            }
            lbl_805E1950.path = buffer;
            lbl_805E1950.pathSize = size;
        }
        lbl_805F15B4->stringFuncs.rwstrcpy(lbl_805E1950.path, path);
        return path;
    }
    return NULL;
}

/* Sets the callback that reads a charset. */
u32 fn_80473528(void *callback) {
    lbl_805E1950.readCallback = callback;
    return 1;
}

/* The scale of `object` times the length of a vector fn_80473FD0 computes. */
f32 fn_8047353C(f32 *object) {
    RwV2d a;
    RwV2d b;
    RwV2d c;

    fn_80473FD0(&a, &b, &c);
    return object[1] * fn_804823F0(&b);
}
}
