/* The staff roll's font (class name inferred; see Game/AdvStaffRoll.h): draws the lines of a page
 * character by character from the font texture of staffroll/StaffRoll.txd, with %C colour and %S
 * size escapes, and the logo for lines of type 2. The file name and all names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/AdvStaffRoll.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* The game state (struct name unknown; see Game/AdvMemoryCheck.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x40];
    /* 0x040 */ s32 m_region; /* 0 Japan (meaning inferred; see Game/AdvLogo.cpp) */
    /* 0x044 */ u8 field_0x44[0x150 - 0x44];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

extern "C" {
/* RwTexDictionaryFindNamedTexture (name inferred from its use). */
RwTexture *fn_8048CFFC(RwTexDictionary *dict, const char *name);
}

using namespace AdvReal::StaffRoll;

/* The text colour before any escape. */
static const RwRGBA s_white = {255, 255, 255, 255};

Font::Font(Context *context, RwTexDictionary *textures) {
    File *file = context->m_file;
    m_baseScale = file->m_scale;
    m_scale = m_baseScale;
    m_spacing = file->m_spacing;
    m_glyphs = file->m_glyphs;
    m_glyphCount = file->m_glyphCount;
    m_textures = textures;
    m_texelU = 0.0f;
    m_texelV = 0.0f;
    for (int i = 0; i < 4; i++) {
        m_vertices[i].z = 0.1f;
        m_vertices[i].emissiveColor = s_white;
    }
    m_offsetY = 0;
    m_offsetX = 0;
}

Font::~Font() {}

void Font::SetOffset(s32 x, s32 y) {
    m_offsetX = x;
    m_offsetY = y;
}

void Font::SetColor(const RwRGBA &color) {
    for (int i = 0; i < 4; i++) {
        m_vertices[i].emissiveColor = color;
    }
}

void Font::BindTexture() {
    RwTexture *texture = fn_8048CFFC(m_textures, "font");
    if (texture != 0) {
        RwRaster *raster = texture->raster;
        m_texelU = 1.0f / raster->width;
        m_texelV = 1.0f / raster->height;
        lbl_805F15B4->fpRenderStateSet(rwRENDERSTATETEXTURERASTER, raster);
    }
}

void Font::DrawLine(const Line *line) {
    m_scale = m_baseScale;
    s32 x = line->m_x + m_offsetX;
    if (line->m_type != 2) {
        const char *text = line->m_text;
        s32 spacing = m_spacing;
        float scale = m_scale;
        const Glyph *space = GetGlyph(' ');
        s32 y = line->m_y + m_offsetY;
        s32 spaceWidth = spacing + (s32)(scale * space->m_width);
        while (*text != 0) {
            if (*text == ' ') {
                x += spaceWidth;
            } else if (*text == '%' && *++text != '%') {
                text = ParseEscape(text);
                continue;
            } else {
                DrawGlyph(&x, y, *text);
                x += m_spacing;
            }
            text++;
        }
    } else {
        DrawLogo(x, line->m_y + m_offsetY, line->m_text);
    }
}

void Font::DrawGlyph(s32 *x, s32 y, u8 c) {
    const Glyph *glyph = GetGlyph(c);
    if (glyph != 0) {
        s32 right = *x + (s32)(glyph->m_width * m_scale);
        s32 top = y - (s32)(glyph->m_height * m_scale);
        float u0 = glyph->m_u * m_texelU;
        float v0 = glyph->m_v * m_texelV;
        float u1 = m_texelU * (glyph->m_u + glyph->m_width);
        float v1 = m_texelV * (glyph->m_v + glyph->m_height);
        m_vertices[0].x = *x;
        m_vertices[0].y = top;
        m_vertices[0].u = u0;
        m_vertices[0].v = v0;
        m_vertices[1].x = *x;
        m_vertices[1].y = y;
        m_vertices[1].u = u0;
        m_vertices[1].v = v1;
        m_vertices[2].x = right;
        m_vertices[2].y = top;
        m_vertices[2].u = u1;
        m_vertices[2].v = v0;
        m_vertices[3].x = right;
        m_vertices[3].y = y;
        m_vertices[3].u = u1;
        m_vertices[3].v = v1;
        lbl_805F15B4->fpIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, m_vertices, 4);
        *x = right;
    }
}

void Font::DrawLogo(s32 x, s32 y, const char *text) {
    const char *name;
    switch (lbl_8057D6A0.m_region) {
    case 0:
        name = "segalogo_j";
        break;
    default:
        name = "segalogo_e";
        break;
    }
    RwTexture *texture = fn_8048CFFC(m_textures, name);
    if (texture != 0) {
        RwRaster *raster = texture->raster;
        float left = x;
        float right = x + raster->width;
        float top = y;
        float bottom = y + raster->height;
        void *previous;
        lbl_805F15B4->fpRenderStateGet(rwRENDERSTATETEXTURERASTER, &previous);
        m_vertices[0].x = left;
        m_vertices[0].y = top;
        m_vertices[0].u = 0.0f;
        m_vertices[0].v = 0.0f;
        m_vertices[1].x = left;
        m_vertices[1].y = bottom;
        m_vertices[1].u = 0.0f;
        m_vertices[1].v = 1.0f;
        m_vertices[2].x = right;
        m_vertices[2].y = top;
        m_vertices[2].u = 1.0f;
        m_vertices[2].v = 0.0f;
        m_vertices[3].x = right;
        m_vertices[3].y = bottom;
        m_vertices[3].u = 1.0f;
        m_vertices[3].v = 1.0f;
        lbl_805F15B4->fpRenderStateSet(rwRENDERSTATETEXTURERASTER, raster);
        lbl_805F15B4->fpIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, m_vertices, 4);
        lbl_805F15B4->fpRenderStateSet(rwRENDERSTATETEXTURERASTER, previous);
    }
}

/* A two-digit hexadecimal value (name inferred). Retail reads the second character for both
 * digits and shifts the first by 8, so only that character counts. */
static inline u8 ParseHex(const char *text) {
    s32 digits[2] = {0, 0};
    for (int i = 0; i < 2; i++) {
        if (text[1] >= '0' && text[1] <= '9') {
            digits[i] = text[1] - '0';
        } else if (text[1] >= 'a' && text[1] <= 'f') {
            digits[i] = text[1] - 'a' + 10;
        } else if (text[1] >= 'A' && text[1] <= 'F') {
            digits[i] = text[1] - 'A' + 10;
        }
    }
    return digits[0] << 8 | digits[1];
}

/* NonMatching (85%): retail reloads text[1] in every pass of ParseHex's loop; GC/2.7 hoists the
 * load out of the loop for every shape tried (a local char, a pointer or reference parameter, the
 * digits as a struct member, through a pointer or volatile), and keeps p in r4 instead of r5. */
const char *Font::ParseEscape(const char *text) {
    const char *p = text + 1;
    switch (*text) {
    case 'C': {
        u8 alpha = m_vertices[0].emissiveColor.alpha;
        u8 red = ParseHex(p);
        u8 green = ParseHex(p + 2);
        u8 blue = ParseHex(p + 4);
        p += 7;
        for (int i = 0; i < 4; i++) {
            m_vertices[i].emissiveColor.red = red;
            m_vertices[i].emissiveColor.green = green;
            m_vertices[i].emissiveColor.blue = blue;
            m_vertices[i].emissiveColor.alpha = alpha;
        }
        break;
    }
    case 'S':
        if (*p == '+' || *p == '-') {
            float sign;
            if (*p == '+') {
                sign = 1.0f;
            } else {
                sign = -1.0f;
            }
            if (p[1] >= '0' && p[1] <= '9') {
                m_scale = m_baseScale * (0.1f * (sign * (p[1] - '0')) + 1.0f);
            }
            p += 2;
        }
        break;
    }
    return p;
}
