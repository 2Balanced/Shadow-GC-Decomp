/* The staff roll's text (class name inferred; see Game/AdvStaffRoll.h): loads the textures,
 * stafroll/StafRoll.txd, makes the font and draws the current page's lines, highlighted ones in
 * yellow, faded with the page. The file name and all names are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/AdvStaffRoll.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* RenderWare's memory stream (rw/rwcore.h). */
struct RwStream;

typedef struct RwMemory {
    /* 0x0 */ void *start;
    /* 0x4 */ u32 length;
} RwMemory;

class UnkFileSystem_800371D8;

extern "C" {
/* RenderWare: RwTexDictionaryDestroy, RwStreamOpen, RwStreamFindChunk, RwTexDictionaryStreamRead
 * and RwStreamClose (names inferred from their use). */
int fn_8048CB38(RwTexDictionary *dict);
RwStream *fn_80481368(int type, int accessType, void *data);
int fn_8047CF50(RwStream *stream, u32 type, u32 *lengthOut, u32 *versionOut);
RwTexDictionary *fn_80483FBC(RwStream *stream);
int fn_8048124C(RwStream *stream, void *data);

/* The file system, a load request to it, and the release of loaded data (see Game/FogTable.cpp). */
UnkFileSystem_800371D8 *fn_800371D8(void);
bool fn_80045044(UnkFileSystem_800371D8 *fileSystem, FogRequest *request, int arg2);
void fn_80049B34(void *data);
}

using namespace AdvReal::StaffRoll;

/* The text colours. */
static const RwRGBA s_white = {255, 255, 255, 255};
static const RwRGBA s_yellow = {255, 255, 0, 255};

Text::Text(Context *context) {
    m_font = 0;
    m_textures = 0;
    m_loaded = 0;
    m_request.m_path = "stafroll/StafRoll.txd";
    m_request.m_data = 0;
    m_request.m_size = 0;
    m_request.m_user = this;
    m_request.m_callback = OnLoaded;
    fn_80045044(fn_800371D8(), &m_request, 1);
}

Text::~Text() {
    if (m_font != 0) {
        delete m_font;
        m_font = 0;
    }
    if (m_textures != 0) {
        fn_8048CB38(m_textures);
        m_textures = 0;
    }
    if (m_request.m_data != 0) {
        fn_80049B34(m_request.m_data);
        m_request.m_data = 0;
    }
}

s32 Text::Update(Context *context) {
    if (m_loaded != 0) {
        if (m_font == 0) {
            m_font = new Font(context, m_textures);
        }
        return true;
    }
    return false;
}

void Text::Tick(Context *context) {}

void Text::Draw(Context *context) {
    if (m_textures != 0) {
        /* Where the text goes for each layout. */
        Position positions[5] = {{320, 16}, {320, 16}, {32, 16}, {32, 16}, {320, 16}};
        RwRGBA white = s_white;
        RwRGBA yellow = s_yellow;
        Page *page = &context->m_file->m_pages[context->m_page];
        Position *position = &positions[context->m_layout];
        u32 alpha = 255.0f * context->m_alpha;
        void *vertexAlpha;
        void *zTest;
        void *zWrite;
        void *cullMode;
        lbl_805F15B4->fpRenderStateGet(rwRENDERSTATEVERTEXALPHAENABLE, &vertexAlpha);
        lbl_805F15B4->fpRenderStateGet(rwRENDERSTATEZTESTENABLE, &zTest);
        lbl_805F15B4->fpRenderStateGet(rwRENDERSTATEZWRITEENABLE, &zWrite);
        lbl_805F15B4->fpRenderStateGet(rwRENDERSTATECULLMODE, &cullMode);
        lbl_805F15B4->fpRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)1);
        lbl_805F15B4->fpRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)0);
        lbl_805F15B4->fpRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)0);
        lbl_805F15B4->fpRenderStateSet(rwRENDERSTATECULLMODE, (void *)rwCULLMODECULLNONE);
        lbl_805F15B4->fpRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)0);
        RwRGBA color;
        color = white;
        color.alpha = alpha;
        m_font->SetOffset(position->x, position->y);
        m_font->BindTexture();
        m_font->SetColor(color);
        for (u32 i = 0; i < page->m_lineCount; i++) {
            if (page->m_lines[i].m_type == 1) {
                color = yellow;
                color.alpha = alpha;
                m_font->SetColor(color);
            }
            m_font->DrawLine(&page->m_lines[i]);
            if (page->m_lines[i].m_type == 1) {
                color = white;
                color.alpha = alpha;
                m_font->SetColor(color);
            }
        }
        lbl_805F15B4->fpRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, vertexAlpha);
        lbl_805F15B4->fpRenderStateSet(rwRENDERSTATEZTESTENABLE, zTest);
        lbl_805F15B4->fpRenderStateSet(rwRENDERSTATEZWRITEENABLE, zWrite);
        lbl_805F15B4->fpRenderStateSet(rwRENDERSTATECULLMODE, cullMode);
    }
}

void Text::OnLoaded(FogRequest *request) {
    Text *text = (Text *)request->m_user;
    if (request->m_data != 0) {
        RwMemory memory;
        memory.start = request->m_data;
        if ((s32)request->m_size != 0) {
            memory.length = request->m_size;
        } else {
            memory.length = 0x25000;
        }
        RwStream *stream = fn_80481368(3, 1, &memory);
        if (fn_8047CF50(stream, 0x16, 0, 0)) {
            text->m_textures = fn_80483FBC(stream);
            fn_8048124C(stream, &memory);
        }
    }
    text->m_loaded = 1;
    if (request != 0 && request->m_data != 0) {
        fn_80049B34(request->m_data);
        request->m_data = 0;
    }
}
