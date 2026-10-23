/* AdvReal::StaffRoll::StaffRollTask and StaffRollDrawTask (class names from RTTI): the staff roll
 * that plays after an ending. StaffRollTask loads "stafroll/StafRoll.srd" (the pages of credits
 * and the font's glyph table), plays a song, turns the pages over the song's length, fading each
 * page in and out, and lets its helpers (Game/AdvStaffRoll.h) load the textures, draw the text and
 * play event movies in a small window beside it; StaffRollDrawTask draws them. The player can end
 * the roll with the start button unless the caller forbade it. The file name and all names marked
 * "(name inferred)" are not original. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#include <string.h>

#include "Game/AdvStaffRoll.h"
#include "Game/FogTable.h"
#include "Game/ScreenFade.h"
#include "Game/Task.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* The pad state (struct name unknown; see Game/SaveCycleMessage.cpp). */
struct UnkPads_80051734 {
    /* 0x000 */ u8 field_0x0[0xD0];
    /* 0x0D0 */ u32 m_buttons; /* bit 0x400: start (meaning inferred) */
};

/* The task tree's owner (struct name unknown; see Game/SaveAction.cpp). */
struct UnkRenderOwner_80046178 {
    /* 0x00 */ u8 field_0x0[0x38];
    /* 0x38 */ Task *m_layer2D; /* the task layer the staff roll draws in (meaning inferred) */
};

/* The object fn_800BADE8 returns (class unknown; see Game/EndAction.cpp). */
struct UnkObject_800BADE8 {
    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ s32 field_0x30; /* meaning unknown */
};

class UnkFileSystem_800371D8;

extern "C" {
/* Byte-swaps size bytes as 32-bit and as 16-bit values (behaviour inferred). */
void fn_8047D254(void *data, u32 size);
void fn_8047D2C4(void *data, u32 size);
/* Compares n characters (see Game/CenFile.cpp). */
int fn_803AC8DC(const char *a, const char *b, u32 n);

/* The file system, a load request to it, and the release of loaded data (see Game/FogTable.cpp). */
UnkFileSystem_800371D8 *fn_800371D8(void);
bool fn_80045044(UnkFileSystem_800371D8 *fileSystem, FogRequest *request, int arg2);
void fn_80049B34(void *data);

/* The pad state (see Game/SaveCycleMessage.cpp). */
UnkPads_80051734 *fn_80051734(void);

/* The sound manager: playing and stopping the music (see Game/AdvBgm.cpp). */
void *fn_80053164(void);
void fn_801D524C(void *sound, const char *file);
void fn_801D522C(void *sound);

/* The screen fade (see Game/AdvOption.cpp). */
void fn_80173834(int arg0, float time);

/* The task tree (see Game/Gadget.h and Game/SaveAction.cpp). */
void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);
UnkRenderOwner_80046178 *fn_80046178(void);

/* Pausing the game around the staff roll (see Game/EndAction.cpp; behaviour unknown). */
UnkObject_800BADE8 *fn_800BADE8(void);
void fn_801E2F28(UnkObject_800BADE8 *object);
void fn_801E2EDC(UnkObject_800BADE8 *object);
}

namespace AdvReal {
namespace StaffRoll {

class StaffRollDrawTask : public Task {
  public:
    StaffRollDrawTask(UnkRenderOwner_80046178 *owner)
        : Task(owner->m_layer2D), m_movie(0), m_text(0), m_context(0) {}
    virtual ~StaffRollDrawTask();
    virtual void Tick(float dt);

    /* 0x28 */ Movie *m_movie;
    /* 0x2C */ Text *m_text;
    /* 0x30 */ Context *m_context;
    /* 0x34 */ u32 field_0x34;
};
SIZE_ASSERT(StaffRollDrawTask, 0x38);

class StaffRollTask : public Task {
  public:
    StaffRollTask(Task *parent, s32 type, s32 noSkip);
    virtual ~StaffRollTask();
    virtual void Tick(float dt);

    s32 Scroll(float dt);
    static void OnLoaded(FogRequest *request);

    /* Member names are inferred. */
    /* 0x28 */ s32 m_type;
    /* 0x2C */ s32 m_state; /* 0 loading, 1 rolling, 2 done */
    /* 0x30 */ Movie *m_movie;
    /* 0x34 */ Text *m_text;
    /* 0x38 */ StaffRollDrawTask *m_drawTask;
    /* 0x3C */ Context m_context;
    /* 0x6C */ s32 m_loaded;
    /* 0x70 */ s32 m_noSkip;
    /* 0x74 */ s32 m_end;
    /* 0x78 */ FogRequest m_request;
    /* 0x8C */ u32 field_0x8C;
};
SIZE_ASSERT(StaffRollTask, 0x90);

} // namespace StaffRoll
} // namespace AdvReal

using namespace AdvReal::StaffRoll;

/* The fade colour. */
static const RwRGBA s_black = {0, 0, 0, 0};

/* The song of each staff roll, and its length. */
static const char *s_songs[5] = {
    "sng_vox02.adx", "sng_vox05.adx", "sng_vox04.adx", "sng_vox03.adx", "sng_EV_music3.adx",
};

static const float s_songLengths[5] = {205.0f, 203.75f, 208.5f, 200.5f, 280.0f};

/* The staff roll task while it exists. */
static StaffRollTask *s_instance;

namespace AdvReal {
namespace StaffRoll {

void StaffRollDrawTask::Tick(float dt) {
    if (m_movie != 0) {
        m_movie->Draw(m_context);
    }
    if (m_text != 0) {
        m_text->Draw(m_context);
    }
}

StaffRollDrawTask::~StaffRollDrawTask() {}

} // namespace StaffRoll
} // namespace AdvReal

/* Starts the staff roll of type unless one is running (name inferred). */
extern "C" void fn_80342398(s32 type, s32 noSkip) {
    if (s_instance == 0) {
        s_instance = new StaffRollTask(fn_801EB304(fn_8001C4F8(), 3), type, noSkip);
    }
}

/* Whether the staff roll has ended (name inferred). */
extern "C" s32 fn_80342388(void) {
    return s_instance == 0;
}

/* Ends the staff roll (name inferred). */
extern "C" void fn_80342370(void) {
    if (s_instance != 0) {
        s_instance->m_end = 1;
    }
}

namespace AdvReal {
namespace StaffRoll {

StaffRollTask::StaffRollTask(Task *parent, s32 type, s32 noSkip)
    : Task(parent), m_type(type), m_state(0), m_movie(0), m_text(0), m_drawTask(0), m_loaded(0),
      m_noSkip(noSkip), m_end(0) {
    memset(&m_context, 0, sizeof(m_context));
    m_context.m_type = type;
    m_context.m_length = s_songLengths[type];
    m_context.m_fadeTime = 0.75f;
    m_context.m_gapTime = 0.1f;
    m_context.m_alpha = 1.0f;
    m_request.m_path = "stafroll/StafRoll.srd";
    m_request.m_data = 0;
    m_request.m_size = 0;
    m_request.m_user = this;
    m_request.m_callback = OnLoaded;
    if (!fn_80045044(fn_800371D8(), &m_request, 1)) {
        m_flags |= 1;
    }
    fn_802051F8(0.75f);
    fn_80204874(&s_black);
    fn_80044648(fn_80044254(), 1.0f);
    UnkObject_800BADE8 *object = fn_800BADE8();
    fn_801E2F28(object);
    fn_801E2EDC(object);
    object->field_0x30 = 0;
}

StaffRollTask::~StaffRollTask() {
    if (m_drawTask != 0) {
        m_drawTask->m_flags |= 1;
        m_drawTask = 0;
    }
    if (m_text != 0) {
        delete m_text;
        m_text = 0;
    }
    if (m_movie != 0) {
        delete m_movie;
        m_movie = 0;
    }
    if (m_request.m_data != 0) {
        fn_80049B34(m_request.m_data);
        m_request.m_data = 0;
    }
    s_instance = 0;
    UnkObject_800BADE8 *object = fn_800BADE8();
    object->field_0x30 = 1;
}

void StaffRollTask::Tick(float dt) {
    switch (m_state) {
    case 2:
        break;
    case 0:
        if (m_loaded != 0) {
            if (m_text == 0) {
                m_text = new Text(&m_context);
            }
            if (m_text->Update(&m_context)) {
                m_context.m_pageTime = m_context.m_length / m_context.m_file->m_pageCount;
                m_movie = new Movie(&m_context);
                m_drawTask = new StaffRollDrawTask(fn_80046178());
                m_drawTask->m_context = &m_context;
                m_drawTask->m_movie = m_movie;
                m_drawTask->m_text = m_text;
                m_state = 1;
                fn_80173834(1, 0.0f);
                fn_801D524C(fn_80053164(), s_songs[m_type]);
            }
        }
        break;
    case 1:
        if (Scroll(dt)) {
            m_movie->Update(&m_context);
            m_text->Tick(&m_context);
        } else {
            m_state = 2;
            m_drawTask->m_flags |= 1;
            m_drawTask = 0;
            m_flags |= 1;
            fn_801D522C(fn_80053164());
        }
        break;
    }
}

s32 StaffRollTask::Scroll(float dt) {
    if ((m_noSkip == 0 && (fn_80051734()->m_buttons & 0x400) == 0x400) || m_end != 0) {
        return false;
    }
    m_context.m_time += dt;
    m_context.m_pageClock += dt;
    if (m_context.m_pageClock >= m_context.m_pageTime) {
        m_context.m_pageClock = 0.0f;
        m_context.m_page++;
        if (m_context.m_type != 4) {
            m_context.m_layout = 4;
        } else {
            m_context.m_layout = m_context.m_file->m_pages[m_context.m_page].m_movie;
        }
        if ((u32)m_context.m_page >= m_context.m_file->m_pageCount) {
            return false;
        }
    }
    /* The time into the page, then the time left before its gap. */
    float t = m_context.m_pageClock;
    float fade = m_context.m_fadeTime;
    float gap = m_context.m_gapTime;
    if (t < fade) {
        m_context.m_alpha = t / fade;
    } else {
        t = m_context.m_pageTime - t;
        t -= gap;
        m_context.m_alpha = 1.0f;
        if ((u32)m_context.m_page < m_context.m_file->m_pageCount - 1) {
            if (t < fade) {
                m_context.m_alpha = t / fade;
                if (m_context.m_alpha < 0.0f) {
                    m_context.m_alpha = 0.0f;
                }
            }
        } else if (t < 1.0f + fade) {
            fn_80173878(0, 1, 1.0f);
        }
    }
    return true;
}

void StaffRollTask::OnLoaded(FogRequest *request) {
    File *file = (File *)request->m_data;
    fn_803AC8DC(file->m_magic, "SSRD", 4);
    fn_8047D254(&file->m_spacing, 0x1C);
    file->m_pages = (Page *)((u8 *)file + (u32)file->m_pages);
    file->m_glyphs = (Glyph *)((u8 *)file + (u32)file->m_glyphs);
    Page *page = file->m_pages;
    for (u32 i = 0; i < file->m_pageCount; i++, page++) {
        fn_8047D254(page, 0xC);
        page->m_lines = (Line *)((u8 *)file + (u32)page->m_lines);
        Line *line = page->m_lines;
        for (u32 j = 0; j < page->m_lineCount; j++, line++) {
            fn_8047D254(&line->m_type, 4);
            fn_8047D2C4(&line->m_x, 4);
            fn_8047D254(&line->m_text, 4);
            line->m_text = (char *)file + (u32)line->m_text;
        }
    }
    Glyph *glyph = file->m_glyphs;
    for (u32 k = 0; k < file->m_glyphCount; k++, glyph++) {
        fn_8047D2C4(glyph, 8);
    }
    StaffRollTask *task = (StaffRollTask *)request->m_user;
    task->m_request.m_data = request->m_data;
    task->m_context.m_file = file;
    task->m_loaded = 1;
}

} // namespace StaffRoll
} // namespace AdvReal

