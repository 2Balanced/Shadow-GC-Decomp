#ifndef GAME_ADVSTAFFROLL_H
#define GAME_ADVSTAFFROLL_H

/* The staff roll (AdvReal::StaffRoll; Game/AdvStaffRoll.cpp has the tasks). Its helpers are three
 * more units: the font (Game/AdvStaffRollFont.cpp), the movie window (Game/AdvStaffRollMovie.cpp)
 * and the text (Game/AdvStaffRollText.cpp); retail pools their float constants apart, so they are
 * separate translation units. All names are inferred. */

#include "Game/FogTable.h"
#include "Game/ScreenFade.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* RenderWare's immediate-mode 2D vertex, textures and render state calls (rw/rwcore.h; as in
 * Game/NowLoading.cpp). */
typedef struct RwIm2DVertex {
    /* 0x00 */ float x;
    /* 0x04 */ float y;
    /* 0x08 */ float z;
    /* 0x0C */ RwRGBA emissiveColor;
    /* 0x10 */ float u;
    /* 0x14 */ float v;
} RwIm2DVertex;

typedef struct RwRaster {
    /* 0x00 */ u8 field_0x0[0xC];
    /* 0x0C */ s32 width;
    /* 0x10 */ s32 height;
} RwRaster;

typedef struct RwTexture {
    /* 0x00 */ RwRaster *raster;
} RwTexture;

struct RwTexDictionary;

typedef int (*RwRenderStateSetFunction)(int state, void *value);
typedef int (*RwRenderStateGetFunction)(int state, void *value);
typedef int (*RwIm2DRenderPrimitiveFunction)(int primType, RwIm2DVertex *vertices, int numVertices);

typedef struct RwGlobals {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ RwRenderStateSetFunction fpRenderStateSet;          /* dOpenDevice.fpRenderStateSet */
    /* 0x24 */ RwRenderStateGetFunction fpRenderStateGet;          /* dOpenDevice.fpRenderStateGet */
    /* 0x28 */ u8 field_0x28[0x30 - 0x28];
    /* 0x30 */ RwIm2DRenderPrimitiveFunction fpIm2DRenderPrimitive; /* dOpenDevice.fpIm2DRenderPrimitive */
} RwGlobals;

/* The engine instance (RwEngineInstance). */
extern RwGlobals *lbl_805F15B4;

enum {
    rwRENDERSTATETEXTURERASTER = 1,
    rwRENDERSTATEZTESTENABLE = 6,
    rwRENDERSTATEZWRITEENABLE = 8,
    rwRENDERSTATEVERTEXALPHAENABLE = 12,
    rwRENDERSTATECULLMODE = 20,
};

enum {
    rwCULLMODECULLNONE = 1,
};

enum {
    rwPRIMTYPETRISTRIP = 4,
};

namespace AdvReal {
namespace StaffRoll {

/* The loaded staff roll file, stafroll/StafRoll.srd. Its offsets are made into pointers when it
 * is loaded. */

/* A line of a page: text, highlighted text (type 1) or the logo (type 2). */
struct Line {
    /* 0x0 */ s32 m_type;
    /* 0x4 */ s16 m_x;
    /* 0x6 */ s16 m_y;
    /* 0x8 */ char *m_text;
};

/* A page of the roll. */
struct Page {
    /* 0x0 */ s32 m_movie; /* the movie shown with it (in the staff roll of type 4) */
    /* 0x4 */ u32 m_lineCount;
    /* 0x8 */ Line *m_lines;
};

/* A character of the font texture. */
struct Glyph {
    /* 0x0 */ u16 m_u;
    /* 0x2 */ u16 m_v;
    /* 0x4 */ u16 m_width;
    /* 0x6 */ u16 m_height;
};

struct File {
    /* 0x00 */ char m_magic[4]; /* "SSRD" */
    /* 0x04 */ s32 m_spacing;
    /* 0x08 */ u32 field_0x8;
    /* 0x0C */ float m_scale;
    /* 0x10 */ u32 m_pageCount;
    /* 0x14 */ Page *m_pages;
    /* 0x18 */ u32 m_glyphCount;
    /* 0x1C */ Glyph *m_glyphs;
};

/* The state the staff roll's parts share. */
struct Context {
    /* 0x00 */ File *m_file;
    /* 0x04 */ s32 m_type;
    /* 0x08 */ float m_length;    /* the song's length */
    /* 0x0C */ float m_pageTime;  /* m_length / the page count */
    /* 0x10 */ float m_fadeTime;
    /* 0x14 */ float m_gapTime;   /* blank time at the end of a page */
    /* 0x18 */ float m_time;
    /* 0x1C */ float m_pageClock;
    /* 0x20 */ u32 field_0x20;
    /* 0x24 */ float m_alpha;     /* of the page's text */
    /* 0x28 */ s32 m_page;
    /* 0x2C */ s32 m_layout;      /* where the movie and the text go */
};
SIZE_ASSERT(Context, 0x30);

/* A position on the screen. */
struct Position {
    s32 x;
    s32 y;
};

/* Draws the text with the font texture. Its vertices are a triangle strip of one character. */
class Font {
  public:
    Font(Context *context, RwTexDictionary *textures);
    ~Font();

    void SetOffset(s32 x, s32 y);
    void SetColor(const RwRGBA &color);
    void BindTexture();
    void DrawLine(const Line *line);
    void DrawGlyph(s32 *x, s32 y, u8 c);
    void DrawLogo(s32 x, s32 y, const char *text);
    const char *ParseEscape(const char *text);

    /* The glyph of character c, or 0. */
    const Glyph *GetGlyph(char c) {
        if ((u32)c >= 0x20 && (u32)c <= m_glyphCount + 0x20) {
            return &m_glyphs[c - 0x20];
        }
        return 0;
    }

    /* 0x00 */ float m_baseScale;
    /* 0x04 */ float m_scale;
    /* 0x08 */ s32 m_spacing;
    /* 0x0C */ Glyph *m_glyphs;
    /* 0x10 */ u32 m_glyphCount;
    /* 0x14 */ RwTexDictionary *m_textures;
    /* 0x18 */ float m_texelU;
    /* 0x1C */ float m_texelV;
    /* 0x20 */ RwIm2DVertex m_vertices[4];
    /* 0x80 */ s32 m_offsetX;
    /* 0x84 */ s32 m_offsetY;
};
SIZE_ASSERT(Font, 0x88);

/* A run of pages that shows one movie. */
struct MovieEntry {
    /* 0x0 */ s32 m_movie;
    /* 0x4 */ s32 m_firstPage;
    /* 0x8 */ s32 m_lastPage;
    /* 0xC */ MovieEntry *m_next;
};

/* The movies beside the text. */
class Movie {
  public:
    Movie(Context *context);
    ~Movie();

    void Update(Context *context);
    void Draw(Context *context);
    void UpdateMovie(Context *context);
    void DrawFrame(s32 x, s32 y, float scale);
    void DrawFade(s32 x, s32 y, float scale);
    void BuildList(Context *context);

    /* 0x0 */ float m_time;
    /* 0x4 */ MovieEntry *m_entries;
    /* 0x8 */ MovieEntry *m_current;
    /* 0xC */ float m_brightness;
};
SIZE_ASSERT(Movie, 0x10);

/* The text of the roll: loads the textures and makes the font. */
class Text {
  public:
    Text(Context *context);
    ~Text();

    s32 Update(Context *context);
    void Tick(Context *context);
    void Draw(Context *context);
    static void OnLoaded(FogRequest *request);

    /* 0x00 */ Font *m_font;
    /* 0x04 */ RwTexDictionary *m_textures;
    /* 0x08 */ s32 m_loaded;
    /* 0x0C */ FogRequest m_request;
    /* 0x20 */ u32 field_0x20;
};
SIZE_ASSERT(Text, 0x24);

} // namespace StaffRoll
} // namespace AdvReal

#endif
