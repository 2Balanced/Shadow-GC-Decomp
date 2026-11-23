/* RenderWare GameCube driver, vertex descriptor (second part), quads and the start of the
 * charsets, 0x8046FF34..0x80471A54. Names are inferred from behaviour. Compiled by CodeWarrior
 * GC 1.3.2 with -O4,p. Matching: fn_8047046C and the quad functions fn_80470474..fn_80470A4C
 * (their .sdata2 pool, 0x805FB220..0x805FB230, is this unit's; it is a source file of its own:
 * the charset functions from fn_80470AC4 on use a second pool at 0x805FB230, shared with
 * RwCpp36.cpp and RwCpp37.cpp). fn_8046FF34 differs in two scratch registers of its texture
 * matrix and colour cases, fn_80470AC4 in the order of two prologue moves, fn_80471300 in its
 * unrolled fill loop. fn_80470C54 and fn_8047149C are not written yet. */
#include <dolphin/types.h>
#include <string.h>
#include "Game/rw/r4.h"

extern "C" {
#define SET_FIELD(reg, mask, value) \
    do {                            \
        (reg) &= ~(mask);           \
        (reg) |= (value) & (mask);  \
    } while (0)

/* Counts the texture coordinates the descriptor enables into bits 4..7 of `unk18`. */
#define COUNT_TEX()                             \
    do {                                        \
        u32 i = 8;                              \
        u32 num = 0;                            \
        while (i-- != 0) {                      \
            if (desc->vcdHi & (3 << (i * 2))) { \
                num++;                          \
            }                                   \
        }                                       \
        SET_FIELD(desc->unk18, 0xF0, num << 4); \
    } while (0)

/* GXSetVtxDesc on the descriptor: how one attribute reaches the display list (GX_NONE,
 * GX_DIRECT, GX_INDEX8 or GX_INDEX16). `unk18` keeps the counts GX derives from it: colours
 * in bits 0..1, the normal kind in bits 2..3, texture coordinates in bits 4..7. */
void fn_8046FF34(RwGameCubeVtxDesc *desc, GXAttr attr, GXAttrType type) {
    switch (attr) {
    case GX_VA_PNMTXIDX:
        SET_FIELD(desc->vcdLo, 1, type);
        break;
    case GX_VA_TEX0MTXIDX:
    case GX_VA_TEX0MTXIDX + 1:
    case GX_VA_TEX0MTXIDX + 2:
    case GX_VA_TEX0MTXIDX + 3:
    case GX_VA_TEX0MTXIDX + 4:
    case GX_VA_TEX0MTXIDX + 5:
    case GX_VA_TEX0MTXIDX + 6:
    case GX_VA_TEX0MTXIDX + 7:
        SET_FIELD(desc->vcdLo, 2 << (attr - GX_VA_TEX0MTXIDX), type << attr);
        break;
    case GX_VA_POS:
        SET_FIELD(desc->vcdLo, 0x600, type << 9);
        break;
    case GX_VA_NRM: {
        u32 kind = 0;
        SET_FIELD(desc->vcdLo, 0x1800, type << 11);
        if (type != GX_NONE) {
            kind = 1;
        }
        SET_FIELD(desc->unk18, 0xC, kind << 2);
        break;
    }
    case GX_VA_NBT: {
        u32 kind = 0;
        SET_FIELD(desc->vcdLo, 0x1800, type << 11);
        if (type != GX_NONE) {
            kind = 2;
        }
        SET_FIELD(desc->unk18, 0xC, kind << 2);
        break;
    }
    case GX_VA_CLR0:
    case GX_VA_CLR1: {
        u32 num = 0;
        SET_FIELD(desc->vcdLo, 0x6000 << (attr - GX_VA_CLR0), type << (attr + 2));
        if (desc->vcdLo & 0x6000) {
            num = 1;
        }
        if (desc->vcdLo & 0x18000) {
            num++;
        }
        SET_FIELD(desc->unk18, 3, num);
        break;
    }
    case GX_VA_TEX0:
        SET_FIELD(desc->vcdHi, 3 << 0, type << 0);
        COUNT_TEX();
        break;
    case GX_VA_TEX1:
        SET_FIELD(desc->vcdHi, 3 << 2, type << 2);
        COUNT_TEX();
        break;
    case GX_VA_TEX2:
        SET_FIELD(desc->vcdHi, 3 << 4, type << 4);
        COUNT_TEX();
        break;
    case GX_VA_TEX3:
        SET_FIELD(desc->vcdHi, 3 << 6, type << 6);
        COUNT_TEX();
        break;
    case GX_VA_TEX4:
        SET_FIELD(desc->vcdHi, 3 << 8, type << 8);
        COUNT_TEX();
        break;
    case GX_VA_TEX5:
        SET_FIELD(desc->vcdHi, 3 << 10, type << 10);
        COUNT_TEX();
        break;
    case GX_VA_TEX6:
        SET_FIELD(desc->vcdHi, 3 << 12, type << 12);
        COUNT_TEX();
        break;
    case GX_VA_TEX7:
        SET_FIELD(desc->vcdHi, 3 << 14, type << 14);
        COUNT_TEX();
        break;
    }
}

void fn_8047046C(RwGameCubeVtxDesc *desc, u8 numArrays) {
    desc->numArrays = numArrays;
}
extern RwGameCubeDriver lbl_805E1950;
extern RwFreeList lbl_805E18E0;
extern s32 lbl_805EDDF0;
extern s32 lbl_805EDDF4;

RwFreeList *fn_8047FC04(s32 entrySize, s32 entriesPerBlock, s32 alignment, s32 numBlocks,
                        RwFreeList *freeList, u32 hint); /* RwFreeListCreateAndPreallocateSpace */
void fn_8047FCDC(RwFreeList *freeList);                  /* RwFreeListDestroy */
void fn_8048CD8C(RwTexture *texture);                    /* RwTextureDestroy */

/* Closes the quads: their free list and buffer. */
void fn_80470474(void) {
    fn_8047FCDC(lbl_805E1950.quadFreeList);
    lbl_805E1950.quadFreeList = NULL;
    RwFree(lbl_805E1950.quadBuffer);
    lbl_805E1950.quadBuffer = NULL;
}

/* Opens the quads: a 0x900-byte buffer and a free list of 0x78-byte quads. */
s32 fn_804704D4(void) {
    lbl_805E1950.quadBuffer = RwMalloc(0x900, 0x40190);
    lbl_805E1950.quadFreeList = fn_8047FC04(sizeof(RwGameCubeQuad), lbl_805EDDF0, 16, lbl_805EDDF4,
                                            &lbl_805E18E0, 0x401A2);
    return 1;
}

RwGameCubeQuad *fn_80470694(RwGameCubeQuad *quad, const RwRGBA *c0, const RwRGBA *c1,
                            const RwRGBA *c2, const RwRGBA *c3);
RwGameCubeQuad *fn_80470998(RwGameCubeQuad *quad, const RwGameCubeV2d *uv0, const RwGameCubeV2d *uv1,
                            const RwGameCubeV2d *uv2, const RwGameCubeV2d *uv3);
RwGameCubeQuad *fn_80470A4C(RwGameCubeQuad *quad, RwTexture *texture);

/* Creates a quad: white, untextured, with every texture coordinate zero. */
RwGameCubeQuad *fn_8047054C(void) {
    RwGameCubeQuad *quad = (RwGameCubeQuad *)RwFreeListAlloc(lbl_805E1950.quadFreeList, 0x301A2);

    if (quad != NULL) {
        RwGameCubeV2d uv;
        RwRGBA color;

        memset(quad, 0, sizeof(RwGameCubeQuad));
        color.red = 0xFF;
        color.green = 0xFF;
        color.blue = 0xFF;
        color.alpha = 0xFF;
        fn_80470694(quad, &color, &color, &color, &color);
        uv.v[0] = 0.0f;
        uv.v[1] = 0.0f;
        fn_80470998(quad, &uv, &uv, &uv, &uv);
        fn_80470A4C(quad, NULL);
        quad->unk70 = 0.5f;
        quad->refCount = 1;
        return quad;
    }
    return NULL;
}

/* Releases a quad: the last reference returns it, and its texture, to the free list. */
s32 fn_80470620(RwGameCubeQuad *quad) {
    if (--quad->refCount <= 0) {
        if (quad->texture != NULL) {
            fn_8048CD8C(quad->texture);
        }
        RwFreeListFree(lbl_805E1950.quadFreeList, quad);
        return 1;
    }
    return 0;
}

/* Sets a quad's corner colours. */
RwGameCubeQuad *fn_80470694(RwGameCubeQuad *quad, const RwRGBA *c0, const RwRGBA *c1,
                            const RwRGBA *c2, const RwRGBA *c3) {
    quad->v[0].color.red = c3->red;
    quad->v[0].color.green = c3->green;
    quad->v[0].color.blue = c3->blue;
    quad->v[0].color.alpha = c3->alpha;
    quad->v[1].color.red = c2->red;
    quad->v[1].color.green = c2->green;
    quad->v[1].color.blue = c2->blue;
    quad->v[1].color.alpha = c2->alpha;
    quad->v[1].color.red = quad->v[1].color.red - quad->v[0].color.red;
    quad->v[1].color.green = quad->v[1].color.green - quad->v[0].color.green;
    quad->v[1].color.blue = quad->v[1].color.blue - quad->v[0].color.blue;
    quad->v[1].color.alpha = quad->v[1].color.alpha - quad->v[0].color.alpha;
    quad->v[2].color.red = c0->red;
    quad->v[2].color.green = c0->green;
    quad->v[2].color.blue = c0->blue;
    quad->v[2].color.alpha = c0->alpha;
    quad->v[3].color.red = c1->red;
    quad->v[3].color.green = c1->green;
    quad->v[3].color.blue = c1->blue;
    quad->v[3].color.alpha = c1->alpha;
    quad->v[3].color.red = quad->v[3].color.red - quad->v[2].color.red;
    quad->v[3].color.green = quad->v[3].color.green - quad->v[2].color.green;
    quad->v[3].color.blue = quad->v[3].color.blue - quad->v[2].color.blue;
    quad->v[3].color.alpha = quad->v[3].color.alpha - quad->v[2].color.alpha;
    memcpy(&quad->color, c3, sizeof(RwRGBA));
    if (quad->v[1].color.red == 0.0f && quad->v[3].color.red == 0.0f &&
        quad->v[0].color.red == quad->v[2].color.red && quad->v[1].color.green == 0.0f &&
        quad->v[3].color.green == 0.0f && quad->v[0].color.green == quad->v[2].color.green &&
        quad->v[1].color.blue == 0.0f && quad->v[3].color.blue == 0.0f &&
        quad->v[0].color.blue == quad->v[2].color.blue && quad->v[1].color.alpha == 0.0f &&
        quad->v[3].color.alpha == 0.0f && quad->v[0].color.alpha == quad->v[2].color.alpha) {
        quad->flags &= ~1;
    } else {
        quad->flags |= 1;
    }
    return quad;
}

/* Sets a quad's corner texture coordinates. */
RwGameCubeQuad *fn_80470998(RwGameCubeQuad *quad, const RwGameCubeV2d *uv0, const RwGameCubeV2d *uv1,
                            const RwGameCubeV2d *uv2, const RwGameCubeV2d *uv3) {
    quad->v[0].uv = *uv3;
    quad->v[1].uv.v[0] = uv2->v[0] - uv3->v[0];
    quad->v[1].uv.v[1] = uv2->v[1] - uv3->v[1];
    quad->v[2].uv = *uv0;
    quad->v[3].uv.v[0] = uv1->v[0] - uv0->v[0];
    quad->v[3].uv.v[1] = uv1->v[1] - uv0->v[1];
    if (quad->v[1].uv.v[0] == 0.0f && quad->v[1].uv.v[1] == 0.0f && quad->v[3].uv.v[0] == 0.0f &&
        quad->v[3].uv.v[1] == 0.0f) {
        quad->flags &= ~2;
    } else {
        quad->flags |= 2;
    }
    return quad;
}

/* Sets a quad's texture, taking a reference to it. */
RwGameCubeQuad *fn_80470A4C(RwGameCubeQuad *quad, RwTexture *texture) {
    if (texture != NULL) {
        quad->flags |= 2;
        texture->refCount++;
    } else {
        quad->flags &= ~2;
    }
    if (quad->texture != NULL) {
        fn_8048CD8C(quad->texture);
    }
    quad->texture = texture;
    return quad;
}
/* 0.0f of the charset source file's constant pool (.sdata2 0x805FB230), which the charset
 * functions of RwCpp36.cpp and RwCpp37.cpp share: referenced by address until the units are
 * re-cut along that file. */
extern const f32 lbl_805FB230;

void *fn_8047DFAC(void *data);
void fn_804747A0(void);
void fn_804748C8(void);
void fn_804748FC(f32 x, f32 y);
void fn_80474964(f32 x, f32 y);
void fn_804771C0(void *image, void *arg);

/* Draws `string` with a charset from `pos`, each glyph scaled by `scale`. */
RwGameCubeCharset *fn_80470AC4(RwGameCubeCharset *charset, const char *text, const RwV2d *pos,
                               void *arg, f32 scale) {
    RwGameCubeGlyph *glyphs;
    const u8 *string;
    s32 step;
    u8 mask;
    s32 hi;
    s32 shiftLo;
    s32 shiftHi;
    s32 code;

    string = (const u8 *)text;
    if (charset != NULL) {
                glyphs = (RwGameCubeGlyph *)fn_8047DFAC(charset->glyphData);
        if (charset->flags & 2) {
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
        fn_80474964(pos->x, pos->y);
        fn_804748FC(scale, scale);
        fn_80474964(lbl_805FB230, -charset->height);
        code = (string[0] << shiftLo) | ((mask & string[hi]) << shiftHi);
        while (code > 0) {
            s32 glyph;

            if (code >= 0x80) {
                s32 ext = code - charset->firstExtCode;
                if (ext >= 0 && ext < charset->numExtCodes) {
                    glyph = charset->extGlyphs[ext];
                } else {
                    glyph = -1;
                }
            } else {
                glyph = charset->asciiGlyphs[code];
            }
            string += step;
            if (glyph >= 0) {
                RwGameCubeGlyph *g = &glyphs[glyph];
                if (g->texture == NULL) {
                    fn_804771C0(g->image, arg);
                    fn_80474964(g->advance + charset->spacing, lbl_805FB230);
                }
            }
            code = (string[0] << shiftLo) | ((mask & string[hi]) << shiftHi);
        }
        fn_804748C8();
    }
    return charset;
}

/* Builds a charset's table of glyphs for the character codes from 0x80 on (the codes are
 * in the driver's glyph code list while the charset is read). */
RwGameCubeCharset *fn_80471300(RwGameCubeCharset *charset) {
    s32 lowest;
    s32 num;
    RwGameCubeDriver *driver;
    s32 highest;
    s32 found;
    s32 range;
    s16 *table;
    s32 *codes;
    s32 i;

    driver = &lbl_805E1950;
    lowest = 0xFFFF;
    highest = 0;
    found = 0;
    codes = driver->glyphCodes;
    for (i = 0; i < charset->numGlyphs; i++) {
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
        for (i = 0; i < charset->numGlyphs; i++) {
            s32 code = driver->glyphCodes[i];
            if (code > 0) {
                table[code - lowest] = i;
            }
        }
        charset->numExtCodes = num;
        charset->firstExtCode = lowest;
        charset->extGlyphs = table;
    }
    return charset;
}
}
