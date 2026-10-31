/* TGiNumberDisplay (class name from the RTTI of its shared_ptr control block; Game/TGi.h): prints a
 * string of digits, ':' and '/' from a font texture of 12 glyphs, as RenderWare immediate-mode
 * quads. The other units call its functions by their fn_ names, so they keep them (the file name
 * and the meanings are inferred). Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#define TGI_NO_NUMBER_COLORS
#include "Game/TGi.h"
#include <dolphin/types.h>
#include <string.h>
#include "size_assert.h"

/* RenderWare's texture (as far as this unit reads it): its raster, and its name at 0x10. */
struct RwTexture {
    /* 0x00 */ RwRaster *raster;
    /* 0x04 */ u8 field_0x4[0x10 - 0x4];
    /* 0x10 */ char name[32];
};

struct RwTexDictionary;

/* RenderWare's engine instance, RwEngineInstance (as far as this unit reads it): the near screen
 * depth of immediate-mode 2D, and the device's render state and primitive calls. */
struct TGiRwGlobals {
    /* 0x00 */ u8 field_0x0[0x18];
    /* 0x18 */ float zBufferNear;
    /* 0x1C */ u8 field_0x1C[0x20 - 0x1C];
    /* 0x20 */ int (*fpRenderStateSet)(int state, void *value);
    /* 0x24 */ int (*fpRenderStateGet)(int state, void *value);
    /* 0x28 */ u8 field_0x28[0x30 - 0x28];
    /* 0x30 */ int (*fpIm2DRenderPrimitive)(int primType, TGiVertex *vertices, int count);
};

extern TGiRwGlobals *lbl_805F15B4;

/* Whether the number displays draw (meaning inferred). */
extern u8 lbl_805E3780;

extern "C" {
/* strstr. */
char *fn_803AC6C0(const char *s, const char *sub);
/* RwTexDictionaryForAllTextures (name inferred from its use). */
RwTexDictionary *fn_8048CBE0(RwTexDictionary *dict, RwTexture *(*callback)(RwTexture *, void *),
                             void *data);
RwTexture *fn_800BCE58(RwTexture *texture, void *name);
}

/* Scales value (name inferred; see fn_800BC7BC). */
static inline float Scale(float value, float scale) {
    return scale * value;
}

/* Where the texture search stores what it finds (names inferred). */
static RwRaster **s_raster;
static RwTexture **s_texture;

/* Puts the glyphs at the screen's near depth (name inferred). fn_800BCC54 inlines it; fn_800BCB2C
 * has the same loop written out (its registers differ from the inlined copy). */
static inline void SetDepth(TGiNumberDisplay *display) {
    float z = lbl_805F15B4->zBufferNear;
    display->m_recipZ = 1.0f / z;
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 4; j++) {
            display->m_glyphs[i][j].z = z;
        }
    }
}

TGiNumberDisplay::TGiNumberDisplay() {
    m_raster = 0;
    memset(&m_scaleX, 0, 0x50);
    memset(m_glyphs, 0, sizeof(m_glyphs));
}

TGiNumberDisplay::~TGiNumberDisplay() {}

/* Finds the texture named name in archive (behaviour inferred). */
extern "C" bool fn_800BCEA8(TGiNumberDisplay *display, void *archive, const char *name) {
    if (archive != 0) {
        display->m_raster = 0;
        s_raster = &display->m_raster;
        s_texture = &display->m_texture;
        fn_8048CBE0((RwTexDictionary *)archive, fn_800BCE58, (void *)name);
        return display->m_raster != 0;
    }
    return false;
}

extern "C" RwTexture *fn_800BCE58(RwTexture *texture, void *name) {
    if (fn_803AC6C0(texture->name, (const char *)name) != 0) {
        *s_texture = texture;
        *s_raster = texture->raster;
    }
    return texture;
}

/* Lays the glyphs out on the texture: width by height, cells of cellWidth by cellHeight, columns
 * per row (behaviour inferred). */
extern "C" bool fn_800BCC54(TGiNumberDisplay *display, int width, int height, int cellWidth,
                            int cellHeight, int columns) {
    if (display->m_raster == 0) {
        return false;
    }
    SetDepth(display);
    display->m_scaleX = 1.0f;
    display->m_scaleY = 1.0f;
    display->field_0x14 = 0.0f;
    display->m_cellWidth = cellWidth;
    display->m_cellHeight = cellHeight;
    for (int i = 0; i < 12; i++) {
        int column = i % columns;
        int row = i / columns;
        TGiVertex *glyph = display->m_glyphs[i];
        float u0 = (0.5f + column * cellWidth) / width;
        float u1 = (0.5f + (column + 1) * cellWidth) / width;
        float v0 = (0.5f + row * cellHeight) / height;
        float v1 = (0.5f + (row + 1) * cellHeight) / height;
        glyph[0].u = u0;
        glyph[1].u = u1;
        glyph[2].u = u0;
        glyph[3].u = u1;
        glyph[0].v = v0;
        glyph[1].v = v0;
        glyph[2].v = v1;
        glyph[3].v = v1;
    }
    fn_800BCB7C(display, 0);
    return true;
}

/* Sets the four corner colours, white without colors (behaviour inferred). */
extern "C" void fn_800BCB7C(TGiNumberDisplay *display, const TGiColorSet *colors) {
    if (colors != 0) {
        for (int i = 0; i < 4; i++) {
            display->m_colors.m_colors[i] = colors->m_colors[i];
        }
    } else {
        for (int i = 0; i < 4; i++) {
            display->m_colors.m_colors[i].r = 255;
            display->m_colors.m_colors[i].g = 255;
            display->m_colors.m_colors[i].b = 255;
            display->m_colors.m_colors[i].a = 255;
        }
    }
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 4; j++) {
            display->m_glyphs[i][j].color[0] = display->m_colors.m_colors[j].r;
            display->m_glyphs[i][j].color[1] = display->m_colors.m_colors[j].g;
            display->m_glyphs[i][j].color[2] = display->m_colors.m_colors[j].b;
            display->m_glyphs[i][j].color[3] = display->m_colors.m_colors[j].a;
        }
    }
}

/* Puts the glyphs at the screen's near depth (behaviour inferred; the argument is not used). */
extern "C" void fn_800BCB2C(TGiNumberDisplay *display, float depth) {
    float z = lbl_805F15B4->zBufferNear;
    display->m_recipZ = 1.0f / z;
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 4; j++) {
            display->m_glyphs[i][j].z = z;
        }
    }
}

/* Draws text at x, y, aligned left (0), centred (1) or right (2) (behaviour inferred).
 *
 * Two shapes decide the float register chain, which is what retail's code is identified by (x
 * f29, y f30, glyph width f31, height f28): the quad's bottom edge is written as y + height at
 * the store instead of folded into `height` beforehand, and the centred offset is spelled
 * -total / 2.0f. GC/2.7 folds that divide by a power of two into the same multiply by the 0.5f
 * pool constant, but keeps the fneg on `total` itself; -total * 0.5f negates into a scratch
 * register and loads the constant over `total`, two rows off. The height's multiplication by
 * 1.0f survives in retail; passing the factor through an inline (Scale) keeps GC/2.7 from
 * folding it. */
extern "C" bool fn_800BC7BC(TGiNumberDisplay *display, const char *text, float x, int align,
                            float y) {
    if (!lbl_805E3780) {
        return true;
    }
    if (display->m_raster == 0) {
        return false;
    }
    int length = strlen(text);
    if (length <= 0) {
        return true;
    }
    float width = display->m_cellWidth * display->m_scaleX;
    float height = Scale(display->m_cellHeight * display->m_scaleY, 1.0f);
    float total = width * length + display->field_0x14 * (length - 1);
    float offset;
    switch (align) {
    case 1:
        offset = -total / 2.0f;
        break;
    case 2:
        offset = -total;
        break;
    default:
        offset = 0.0f;
        break;
    }
    void *cullMode;
    void *filter;
    void *zTest;
    void *zWrite;
    lbl_805F15B4->fpRenderStateGet(20, &cullMode);
    lbl_805F15B4->fpRenderStateGet(9, &filter);
    lbl_805F15B4->fpRenderStateGet(6, &zTest);
    lbl_805F15B4->fpRenderStateGet(8, &zWrite);
    lbl_805F15B4->fpRenderStateSet(20, (void *)1);
    lbl_805F15B4->fpRenderStateSet(9, (void *)2);
    lbl_805F15B4->fpRenderStateSet(6, (void *)0);
    lbl_805F15B4->fpRenderStateSet(8, (void *)0);
    lbl_805F15B4->fpRenderStateSet(1, display->m_raster);
    for (int i = 0; i < length; i++, text++) {
        int glyph;
        switch ((u8)*text) {
        case ':':
            glyph = 10;
            break;
        case '/':
            glyph = 11;
            break;
        case ' ':
            glyph = -1;
            break;
        default:
            glyph = (u8)*text - '0';
            break;
        }
        if (glyph >= 0) {
            TGiVertex *quad = display->m_glyphs[glyph];
            float left = x + offset;
            float right = width + left;
            quad[0].x = left;
            quad[1].x = right;
            quad[2].x = left;
            quad[3].x = right;
            quad[0].y = y;
            quad[1].y = y;
            quad[2].y = y + height;
            quad[3].y = y + height;
            lbl_805F15B4->fpIm2DRenderPrimitive(4, quad, 4);
        }
        offset += width + display->field_0x14;
    }
    lbl_805F15B4->fpRenderStateSet(1, (void *)0);
    lbl_805F15B4->fpRenderStateSet(20, cullMode);
    lbl_805F15B4->fpRenderStateSet(9, filter);
    lbl_805F15B4->fpRenderStateSet(6, zTest);
    lbl_805F15B4->fpRenderStateSet(8, zWrite);
    return true;
}
