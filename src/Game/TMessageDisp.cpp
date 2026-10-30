/* TMessageDisp (class name from the RTTI of its boost::shared_ptr control block; see
 * Game/StageManager.cpp and Game/AdvMessage.cpp): a set of messages loaded from fonts/<name>/ in
 * the chosen language, with up to ten fonts that cover their characters, and the drawing of a
 * message with a drop shadow. Other units call its functions by their fn_ names (the constructor is
 * fn_8004ABD0, the destructor fn_8004AB88) and declare the class locally, most as
 * UnkHintTarget_801FB180. Names other than the class's are inferred. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first).
 *
 * NonMatching (17/21; every section's data matches): register allocation only. fn_80049BC0 (99.5%)
 * gives the three saved parameters other registers, fn_8004A684 (99.9%) the saved image path,
 * fn_8004A7B4 (97.5%) swaps the loop counter and the font pointer, and fn_8004A074 (98.2%) loads
 * 0.5 before the height and copies the text pointer later than retail. About 10 source shapes
 * were tried for each. */

#include <dolphin/types.h>
#include "size_assert.h"
#include <string.h>
#include <stdio.h>

/* RenderWare. */
struct RwTexture;
struct RwTexDictionary;
struct RwStream;
struct RtCharset;
struct RwV2d {
    float x;
    float y;
};
struct RwRGBA {
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
};
struct RwMemory {
    void *start;
    u32 length;
};
/* The engine instance (RwEngineInstance): the render state functions. */
struct RwGlobals {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ int (*fpRenderStateSet)(int state, void *value);
    /* 0x24 */ int (*fpRenderStateGet)(int state, void *value);
};
extern RwGlobals *lbl_805F15B4;

enum {
    rwRENDERSTATESRCBLEND = 10,
    rwRENDERSTATEDESTBLEND = 11,
    rwRENDERSTATEVERTEXALPHAENABLE = 20
};

/* The game state (struct name unknown; see Game/PjsEventActionScene.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xA];
    /* 0x00A */ u8 m_showMessages; /* (meaning inferred) */
    /* 0x00B */ u8 field_0xB[0x150 - 0xB];
};
extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* Whether a movie is playing (see Game/AdvLogo.cpp). */
struct UnkMovieStatus_805EE0B0;
extern UnkMovieStatus_805EE0B0 *lbl_805EE0B0;

/* The file system (Game/UnkFileSystem_800371D8.cpp): its second member tells whether a file
 * exists. */
struct UnkFileSystem_800371D8 {
    /* 0x0 */ u8 field_0x0[0x4];
    /* 0x4 */ bool (*m_exists)(const char *path, UnkFileSystem_800371D8 *fileSystem);
};

/* A message: its text (an offset in the file until loaded), its key (id * 1000 + branch * 100 +
 * index) and two values the callers read (the second is the voice). */
struct UnkMessage_80049D38 {
    /* 0x00 */ s16 *m_text;
    /* 0x04 */ s32 m_key;
    /* 0x08 */ s32 field_0x8;
    /* 0x0C */ s32 field_0xC;
    /* 0x10 */ s32 m_voice;
};

/* The messages file: the count, then the messages sorted by key. */
struct UnkMessageFile_8004A684 {
    /* 0x00 */ s32 m_count;
    /* 0x04 */ UnkMessage_80049D38 m_messages[1];
};

/* A font and the characters it covers (a zero-terminated table). */
struct UnkMessageFont_80049F2C {
    /* 0x0 */ RtCharset *m_font;
    /* 0x4 */ s32 *m_chars;
};

class TMessageDisp {
  public:
    UnkMessage_80049D38 *Find(s32 id, s32 branch, s32 index) {
        if (m_messages == 0) {
            return 0;
        }
        s32 key = id * 1000 + branch * 100 + index;
        UnkMessage_80049D38 *messages = m_messages->m_messages;
        s32 low = 0;
        s32 high = m_messages->m_count - 1;
        for (;;) {
            if (high - low <= 1) {
                break;
            }
            s32 middle = (low + high) / 2;

            if (messages[middle].m_key < key) {
                low = middle;
            } else if (messages[middle].m_key > key) {
                high = middle;
            } else {
                return &messages[middle];
            }
        }
        if (key == messages[low].m_key) {
            return &messages[low];
        }
        if (key == messages[high].m_key) {
            return &messages[high];
        }
        return 0;
    }

    s32 IndexOf(RtCharset *font) {
        for (s32 i = 0; i < 10; i++) {
            if (m_fonts[i].m_font == font) {
                return i;
            }
        }
        return -1;
    }

    RtCharset *FindFont(u16 c) {
        for (int i = 0; i < 10; i++) {
            if (m_fonts[i].m_chars != 0) {
                for (int j = 0; m_fonts[i].m_chars[j] != 0; j++) {
                    if (c == m_fonts[i].m_chars[j]) {
                        return m_fonts[i].m_font;
                    }
                }
            }
        }
        return 0;
    }

    /* 0x00 */ RwTexDictionary *m_textures;
    /* 0x04 */ UnkMessageFont_80049F2C m_fonts[10];
    /* 0x54 */ RtCharset *m_currentFont;
    /* 0x58 */ RtCharset *m_debugFont;
    /* 0x5C */ UnkMessageFile_8004A684 *m_messages;
    /* 0x60 */ u8 field_0x60;
    /* 0x61 */ u8 m_binaryFonts; /* whether the fonts come in one .fnb stream */
    /* 0x64 */ float m_scale;
    /* 0x68 */ float field_0x68;
    /* 0x6C */ float m_lineHeight;
    /* 0x70 */ s32 m_align; /* 1: left, 2: centred */
};
SIZE_ASSERT(TMessageDisp, 0x74);

/* The layout of the message drawn last: its text, the font of each character and its position
 * and size (struct name unknown). */
struct UnkMessageLayout_80573340 {
    /* 0x000 */ const u16 *m_text;
    /* 0x004 */ s32 m_next;
    /* 0x008 */ s32 m_count;
    /* 0x00C */ RtCharset *m_fonts[0x200];
    /* 0x80C */ RwV2d m_position;
    /* 0x814 */ float m_width;
    /* 0x818 */ float m_height;
};

extern "C" {
/* The screen (Game/ScreenHolder.h); its camera's value at 0x80 sets the fonts' scale. */
struct UnkScreen_8000DCC0 {
    /* 0x00 */ u8 field_0x0[0x80];
    /* 0x80 */ float field_0x80;
};
UnkScreen_8000DCC0 **fn_8000DCC0(void);
/* Whether the screen is wide (see Game/PjsEventActionScene.cpp). */
s32 fn_80305BE8(void);
/* The file system. */
UnkFileSystem_800371D8 *fn_800371D8(void);
void *fn_8004509C(UnkFileSystem_800371D8 *fileSystem, const char *path, u32 *size);
void fn_80049B34(void *p);
void *fn_80049B60(u32 count, u32 size, u32 hint);
/* Byte swaps. */
void fn_80043D0C(void *value, int size);
void fn_80043CE4(void *value, int size);
/* The texture dictionaries (see Game/LandManager.cpp). */
void *fn_80014534(void);
RwTexDictionary *fn_80039650(void *manager, const char *name);
void fn_802E0600(RwTexDictionary *dict);
RwTexDictionary *fn_8048CBE0(RwTexDictionary *dict, RwTexture *(*callback)(RwTexture *, void *),
                             void *data);
/* The image path: set (returning the previous one), and restored. */
char *fn_80051810(const char *path);
void fn_80051830(char *path);
/* RenderWare streams: open, find a chunk, close. */
RwStream *fn_80481368(int type, int accessType, void *data);
int fn_8047CF50(RwStream *stream, u32 type, u32 *length, u32 *version);
int fn_8048124C(RwStream *stream, void *data);
/* The fonts (RenderWare's charset toolkit and the game's additions to it; meanings inferred). */
RtCharset *fn_8047054C(void);
void fn_80470620(RtCharset *font);
void fn_80470694(RtCharset *font, const RwRGBA *topLeft, const RwRGBA *topRight,
                 const RwRGBA *bottomLeft, const RwRGBA *bottomRight);
RtCharset *fn_80472C14(RwStream *stream);
void fn_80473460(void);
float fn_8047353C(RtCharset *font);
float fn_80473584(RtCharset *font, const u16 *text, float scale);
void fn_804736CC(RtCharset *font);
RtCharset *fn_80473CE0(const char *path);
void fn_80473D10(RtCharset *font, const u16 *text, RwV2d *position, float scale,
                 RtCharset *debugFont);
void fn_80473D38(RtCharset *font, s32 *chars);
void fn_80473E8C(RwTexDictionary *dict);
void fn_80473ECC(void);
void fn_80474228(float scale);
void fn_804747A0(void);
void fn_804748C8(void);

const u16 *fn_80049D38(TMessageDisp *self, s32 id, s32 branch, s32 index, s32 *value,
                       s32 *voice);
void fn_80049EC4(TMessageDisp *self);
s32 fn_80049F2C(TMessageDisp *self);
bool fn_80049F68(TMessageDisp *self, RtCharset *font);
bool fn_80049FB8(TMessageDisp *self, u16 c, RwV2d *position, bool draw);
bool fn_8004A074(TMessageDisp *self, const u16 *text, float x, float y);
void fn_8004A47C(TMessageDisp *self, const u16 *text);
void fn_8004A534(TMessageDisp *self, RwV2d *position, float *width, float *height);
void fn_8004A560(TMessageDisp *self, RwV2d position, float width, float height);
RtCharset *fn_8004A584(TMessageDisp *self, RwStream *stream);
RtCharset *fn_8004A604(TMessageDisp *self, const char *path);
bool fn_8004A684(TMessageDisp *self, const char *name);
void fn_8004A7B4(TMessageDisp *self);
bool fn_8004A86C(TMessageDisp *self, const char *name);
void fn_8004AAE4(TMessageDisp *self);
void fn_8004AB28(TMessageDisp *self);
RwTexture *fn_8004AC3C(RwTexture *texture, void *data);
}

static inline bool FileExists(const char *path) {
    UnkFileSystem_800371D8 *fileSystem = fn_800371D8();
    return fileSystem->m_exists(path, fileSystem);
}

/* The language suffixes
 (lbl_805EE180 is the language). */
const char *lbl_8051D528[6] = {"_JP", "_EN", "_FR", "_SP", "_GE", "_IT"};

/* The language (0 to 5). */
s32 lbl_805EE180;

static const RwV2d s_wideScale = {1.0f, 1.0f};
static const RwV2d s_scale = {1.0666667f, 1.0f};

static UnkMessageLayout_80573340 s_layout;
/* The render states that drawing changes, kept to restore. */
static void *s_srcBlend;
static void *s_destBlend;
static void *s_vertexAlpha;
static u16 s_char[2] = {0, 0};


/* Sets a texture's filtering (the texture dictionary callback). */
extern "C" RwTexture *fn_8004AC3C(RwTexture *texture, void *data) {
    u32 *filtering = (u32 *)((u8 *)texture + 0x50);
    *filtering = (*filtering & 0xFFFFFF00) | 2;
    return texture;
}

/* The constructor. */
extern "C" TMessageDisp *fn_8004ABD0(TMessageDisp *self, const char *name, int arg2) {
    self->m_textures = 0;
    self->m_debugFont = 0;
    self->m_binaryFonts = 0;
    fn_8004AB28(self);
    fn_8004A86C(self, name);
    self->m_scale = 0.05f;
    self->field_0x68 = 0.0f;
    self->m_lineHeight = 0.05f;
    self->m_align = 2;
    return self;
}

/* The destructor. */
extern "C" TMessageDisp *fn_8004AB88(TMessageDisp *self, short flags) {
    if (self != 0) {
        fn_8004AAE4(self);
        if (flags > 0) {
            operator delete(self);
        }
    }
    return self;
}

extern "C" void fn_8004AB28(TMessageDisp *self) {
    UnkScreen_8000DCC0 *screen = *fn_8000DCC0();
    fn_80474228(1.01f * screen->field_0x80);

    self->m_currentFont = 0;
    memset(self->m_fonts, 0, sizeof(self->m_fonts));
    fn_80049EC4(self);
}

extern "C" void fn_8004AAE4(TMessageDisp *self) {
    fn_8004A7B4(self);
    if (self->m_debugFont != 0) {
        fn_80470620(self->m_debugFont);
        self->m_debugFont = 0;
    }
}

/* Loads the messages and fonts of name in the chosen language. */
extern "C" bool fn_8004A86C(TMessageDisp *self, const char *name) {
    char path[0x100];
    char metrics[0x100];
    RwMemory memory;
    u32 size;
    if (name == 0) {
        return false;
    }
    sprintf(path, "fonts/%s/%s%s", name, name, lbl_8051D528[lbl_805EE180]);
    if (!fn_8004A684(self, path)) {
        return false;
    }
    sprintf(path, "fonts/%s/%s%s.txd", name, name, lbl_8051D528[lbl_805EE180]);
    self->m_textures = fn_80039650(fn_80014534(), path);
    if (self->m_textures == 0) {
        return false;
    }
    fn_8048CBE0(self->m_textures, fn_8004AC3C, 0);
    fn_80473E8C(self->m_textures);
    if (self->m_binaryFonts) {
        sprintf(path, "fonts/%s/%s%s.fnb", name, name, lbl_8051D528[lbl_805EE180]);
        memory.start = fn_8004509C(fn_800371D8(), path, &size);
        memory.length = size;
        RwStream *stream = fn_80481368(3, 1, &memory);
        if (stream == 0) {
            return false;
        }
        while (fn_8047CF50(stream, 0x199, 0, 0)) {
            fn_8004A584(self, stream);
        }
        fn_8048124C(stream, 0);
        fn_80049B34(memory.start);
    } else {
        int i = 0;
        sprintf(path, "%s/%s%s%02d", name, name, lbl_8051D528[lbl_805EE180], i);
        do {
            if (fn_8004A604(self, path) == 0) {
                return false;
            }
            i++;
            sprintf(path, "%s/%s%s%02d", name, name, lbl_8051D528[lbl_805EE180], i);
            sprintf(metrics, "fonts/%s.met", path);
        } while (FileExists(metrics));
    }
    memset(&s_layout, 0, sizeof(s_layout));
    return true;
}

/* Frees the fonts, the textures and the messages. */
extern "C" void fn_8004A7B4(TMessageDisp *self) {
    for (int i = 0; i < 10; i++) {
        if (self->m_fonts[i].m_font != 0) {
            fn_804736CC(self->m_fonts[i].m_font);
            self->m_fonts[i].m_font = 0;
            if (self->m_fonts[i].m_chars != 0) {
                fn_80049B34(self->m_fonts[i].m_chars);
                self->m_fonts[i].m_chars = 0;
            }
        }
    }
    if (self->m_currentFont == 0) {
        self->m_currentFont = 0;
    }
    if (self->m_textures != 0) {
        fn_802E0600(self->m_textures);
        self->m_textures = 0;
    }
    if (self->m_messages != 0) {
        fn_80049B34(self->m_messages);
        self->m_messages = 0;
    }
}

/* Loads the messages file name.fnt and makes its texts usable. */
extern "C" bool fn_8004A684(TMessageDisp *self, const char *name) {
    char path[0x100];
    char *previous = fn_80051810("./fonts/");
    fn_80473460();
    fn_80051830(previous);
    sprintf(path, "%s.fnt", name);
    self->m_messages = (UnkMessageFile_8004A684 *)fn_8004509C(fn_800371D8(), path, 0);
    if (self->m_messages == 0) {
        return false;
    }
    fn_80043D0C(self->m_messages, 4);
    UnkMessage_80049D38 *message;
    int i;
    UnkMessage_80049D38 *messages;
    messages = self->m_messages->m_messages;
    for (i = 0; i < self->m_messages->m_count; i++) {
        message = &messages[i];
        fn_80043D0C(&message->m_text, 4);
        fn_80043D0C(&message->m_key, 4);
        fn_80043D0C(&message->field_0x8, 4);
        fn_80043D0C(&message->field_0xC, 4);
        fn_80043D0C(&message->m_voice, 4);
        message->m_text = (s16 *)((u32)message->m_text + (u32)self->m_messages);
        s16 *text = message->m_text;
        while (true) {
            fn_80043CE4(text, 2);
            if (*text == 0) {
                break;
            }
            text++;
        }
    }
    return true;
}

/* Loads a font file and the characters it covers. */
extern "C" RtCharset *fn_8004A604(TMessageDisp *self, const char *path) {
    s32 i = fn_80049F2C(self);
    if (i < 0) {
        return 0;
    }
    RtCharset *font = fn_80473CE0(path);
    if (font != 0) {
        self->m_fonts[i].m_font = font;
        self->m_fonts[i].m_chars = (s32 *)fn_80049B60(0x400, 1, 1);
        fn_80473D38(self->m_fonts[i].m_font, self->m_fonts[i].m_chars);
    }
    return font;
}

/* Reads a font and the characters it covers from a stream. */
extern "C" RtCharset *fn_8004A584(TMessageDisp *self, RwStream *stream) {
    s32 i = fn_80049F2C(self);
    if (i < 0) {
        return 0;
    }
    RtCharset *font = fn_80472C14(stream);
    if (font != 0) {
        self->m_fonts[i].m_font = font;
        self->m_fonts[i].m_chars = (s32 *)fn_80049B60(0x400, 1, 1);
        fn_80473D38(self->m_fonts[i].m_font, self->m_fonts[i].m_chars);
    }
    return font;
}

extern "C" void fn_8004A560(TMessageDisp *self, RwV2d position, float width, float height) {
    s_layout.m_position = position;
    s_layout.m_width = width;
    s_layout.m_height = height;
}

extern "C" void fn_8004A534(TMessageDisp *self, RwV2d *position, float *width, float *height) {
    *position = s_layout.m_position;
    *width = s_layout.m_width;
    *height = s_layout.m_height;
}

/* Lays text out: the font of each character. */
extern "C" void fn_8004A47C(TMessageDisp *self, const u16 *text) {
    s_layout.m_count = 0;
    s_layout.m_text = text;
    RtCharset **fonts = s_layout.m_fonts;
    for (; *text != 0; text++) {
        if (*text != '\n') {
            s_layout.m_count++;
            *fonts++ = self->FindFont(*text);
        }
    }
    s_layout.m_next = 0;
}

/* Draws text at x, y (in screen fractions) with a drop shadow. */
extern "C" bool fn_8004A074(TMessageDisp *self, const u16 *text, float x, float y) {
    bool layout = true;
    const u16 *c;
    x *= s_scale.x;
    y *= s_scale.y;
    if (fn_80305BE8() != 0) {
        x *= s_wideScale.x;
        y *= s_wideScale.y;
    }
    fn_804747A0();
    fn_80473ECC();
    if (text != s_layout.m_text) {
        fn_8004A47C(self, text);
    } else {
        layout = false;
    }
    RwV2d position = {x, y};
    float width = 0.0f;
    float height = 0.0f;
    if (layout) {
        RwV2d pen = {0.0f, -1.0f};
        s_layout.m_next = 0;
        for (c = text; *c != 0; c++) {
            if (*c != '\n') {
                fn_80049FB8(self, *c, &pen, true);
            } else {
                if (width < pen.x) {
                    width = pen.x;
                }
                pen.x = 0.0f;
                pen.y -= self->m_lineHeight;
                height -= self->m_lineHeight;
            }
        }
        if (width < pen.x) {
            width = pen.x;
        }
        fn_8004A560(self, position, width, height);
    } else {
        fn_8004A534(self, &position, &width, &height);
    }
    lbl_805F15B4->fpRenderStateGet(rwRENDERSTATESRCBLEND, &s_srcBlend);
    lbl_805F15B4->fpRenderStateGet(rwRENDERSTATEDESTBLEND, &s_destBlend);
    lbl_805F15B4->fpRenderStateGet(rwRENDERSTATEVERTEXALPHAENABLE, &s_vertexAlpha);
    switch (self->m_align) {
    case 0:
        break;
    case 1:
        position.x = x;
        position.y = y - height * 0.5f;
        break;
    case 2:
        position.x = x - width * 0.5f;
        position.y = y - height * 0.5f;
        break;
    }
    RwV2d pen = {0.005f + position.x, position.y - 0.005f};
    if (lbl_805EE0B0 == 0) {
        s_layout.m_next = 0;
        lbl_805F15B4->fpRenderStateSet(rwRENDERSTATESRCBLEND, (void *)1);
        lbl_805F15B4->fpRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)4);
        for (c = text; *c != 0; c++) {
            if (*c != '\n') {
                fn_80049FB8(self, *c, &pen, true);
            } else {
                pen.x = 0.005f + position.x;
                pen.y -= self->m_lineHeight;
            }
        }
    }
    lbl_805F15B4->fpRenderStateSet(rwRENDERSTATESRCBLEND, (void *)2);
    lbl_805F15B4->fpRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)2);
    pen = position;
    s_layout.m_next = 0;
    for (; *text != 0; text++) {
        if (*text != '\n') {
            fn_80049FB8(self, *text, &pen, true);
        } else {
            pen.x = position.x;
            pen.y -= self->m_lineHeight;
        }
    }
    lbl_805F15B4->fpRenderStateSet(rwRENDERSTATESRCBLEND, s_srcBlend);
    lbl_805F15B4->fpRenderStateSet(rwRENDERSTATEDESTBLEND, s_destBlend);
    lbl_805F15B4->fpRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, s_vertexAlpha);
    fn_804748C8();
    return true;
}

/* Draws (or measures) the next character of the laid-out text and moves the pen past it. */
extern "C" bool fn_80049FB8(TMessageDisp *self, u16 c, RwV2d *position, bool draw) {
    RtCharset *font = s_layout.m_fonts[s_layout.m_next];
    s_layout.m_next++;
    if (font == 0) {
        return false;
    }
    fn_80049F68(self, font);
    s_char[0] = c;
    if (draw) {
        fn_80473D10(self->m_currentFont, s_char, position, self->m_scale, self->m_debugFont);
    }
    position->x += fn_80473584(font, s_char, self->m_scale * fn_8047353C(font));
    return true;
}

/* Makes font the current one. */
extern "C" bool fn_80049F68(TMessageDisp *self, RtCharset *font) {
    if (self->IndexOf(font) < 0) {
        return false;
    }
    self->m_currentFont = font;
    return true;
}

/* A free font slot, or -1. */
extern "C" s32 fn_80049F2C(TMessageDisp *self) {
    return self->IndexOf(0);
}

/* Creates the debug font. */
extern "C" void fn_80049EC4(TMessageDisp *self) {
    RwRGBA color = {254, 254, 254, 254};
    if (self->m_debugFont != 0) {
        fn_80470620(self->m_debugFont);
        self->m_debugFont = 0;
    }
    self->m_debugFont = fn_8047054C();
    fn_80470694(self->m_debugFont, &color, &color, &color, &color);
}

/* A message, or 0. */
extern "C" UnkMessage_80049D38 *fn_80049E1C(TMessageDisp *self, s32 id, s32 branch, s32 index) {
    if (self->m_messages == 0) {
        return 0;
    }
    s32 key = id * 1000 + branch * 100 + index;
    UnkMessage_80049D38 *messages = self->m_messages->m_messages;
    s32 low = 0;
    s32 high = self->m_messages->m_count - 1;
    for (;;) {
        if (high - low <= 1) {
            break;
        }
        s32 middle = (low + high) / 2;
        if (messages[middle].m_key < key) {
            low = middle;
        } else if (messages[middle].m_key > key) {
            high = middle;
        } else {
            return &messages[middle];
        }
    }
    if (key == messages[low].m_key) {
        return &messages[low];
    }
    if (key == messages[high].m_key) {
        return &messages[high];
    }
    return 0;
}


/* A message's text and values, or 0. */
extern "C" const u16 *fn_80049D38(TMessageDisp *self, s32 id, s32 branch, s32 index, s32 *value,
                                  s32 *voice) {
    UnkMessage_80049D38 *message = self->Find(id, branch, index);
    if (message != 0) {
        if (value != 0) {
            *value = message->field_0xC;
        }
        if (voice != 0) {
            *voice = message->m_voice;
        }
        return (const u16 *)message->m_text;
    }
    return 0;
}

/* Draws a message (or, when missing, its first branch) at x, y; returns it, or 0. */
extern "C" UnkMessage_80049D38 *fn_80049BC0(TMessageDisp *self, s32 id, s32 branch, s32 index,
                                            float x, float y, s32 *voice) {
    if (lbl_8057D6A0.m_showMessages == 0) {
        return 0;
    }
    const u16 *text = fn_80049D38(self, id, branch, index, 0, voice);
    if (text == 0 && branch != 0) {
        branch = 0;
        text = fn_80049D38(self, id, branch, index, 0, voice);
    }

    if (text == 0) {
        return 0;
    }
    fn_8004A074(self, text, x, y);
    return self->Find(id, branch, index);

}
