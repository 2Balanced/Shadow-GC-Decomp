/* The staff roll's movie window (class name inferred; see Game/AdvStaffRoll.h): plays the event
 * movies the player has seen in a small window beside the text, one after another over the pages
 * (in the staff roll of type 4 the pages choose them), with a frame around the window and a fade
 * at the change of movie. The file name and all names are inferred. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include "Game/AdvStaffRoll.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* The movie playing (struct name unknown; see Game/AdvMovie.cpp). */
struct UnkMovieStatus_805EE0B0 {
    /* 0x00 */ u8 field_0x0[0x4];
    /* 0x04 */ u16 m_flags;
};

extern UnkMovieStatus_805EE0B0 *lbl_805EE0B0;

/* The save data (struct name unknown; see Game/AdvStoryLibrary.cpp). */
struct UnkSaveData_800BB74C {
    /* 0x0000 */ u8 field_0x0[0x1754];
    /* 0x1754 */ u8 m_events[4]; /* the events seen (meaning inferred) */
};

extern "C" {
/* Movies: starting one, its time, and drawing it at a position and scale (behaviour inferred; see
 * Game/AdvMovie.cpp). */
bool fn_80036E78(const char *fileName, int arg1, int arg2, int arg3);
float fn_80036464(float arg0);
void fn_80036570(int x, int y, float scaleX, float scaleY);

/* The story an event belongs to, the save data and its event flags (behaviour inferred). */
int fn_8025D280(int event);
UnkSaveData_800BB74C *fn_800BB74C(void);
bool fn_80174424(u8 *flags, int index);
}

using namespace AdvReal::StaffRoll;

/* The events whose movies the staff roll shows once they have been seen, and the movies with their
 * lengths. */
static const s32 s_movieEvents[8] = {0x3E9, 0x1005, 0x1069, 0x1006, 0x13A0, 0x1F43, 0x2009, 0x3E9};

static const char *s_movieFiles[8] = {
    "E1001.sfd", "E4101.sfd", "E4201.sfd", "E4102.sfd", "E5024.sfd", "E8003.sfd", "E8201.sfd", "OP.sfd",
};

static const float s_movieLengths[8] = {105.6f, 63.9f, 24.19f, 63.9f, 36.0f, 20.19f, 42.24f, 76.9f};

/* Where the movie goes for each layout. */
static Position s_moviePositions[5] = {{32, 128}, {32, 128}, {336, 128}, {336, 128}, {32, 128}};

/* NonMatching (90%): retail reads context->m_type before the member stores and stores the zeros
 * from the register that holds movie; GC/2.7 keeps a separate zero and schedules the loads
 * differently for every order tried. */
Movie::Movie(Context *context) {
    s32 type = context->m_type;
    u32 i;
    u32 movie;
    MovieEntry *entry;
    Page *page;
    File *file;
    m_time = -1.0f;
    m_entries = 0;
    m_current = 0;
    m_brightness = 1.0f;
    movie = 0;
    if (type != 4) {
        BuildList(context);
    } else {
        file = context->m_file;
        page = file->m_pages;
        entry = new MovieEntry;
        entry->m_movie = 6;
        entry->m_firstPage = 0;
        entry->m_lastPage = 1000;
        entry->m_next = 0;
        m_current = entry;
        m_entries = entry;
        s32 shown = page->m_movie;
        page++;
        for (i = 1; i < file->m_pageCount; i++, page++) {
            if (page->m_movie != shown) {
                entry->m_lastPage = i - 1;
                entry->m_next = new MovieEntry;
                entry = entry->m_next;
                entry->m_movie = movie;
                entry->m_firstPage = i;
                entry->m_lastPage = 1000;
                entry->m_next = 0;
                shown = page->m_movie;
                if (movie < 7) {
                    movie++;
                }
            }
        }
    }
    fn_80036E78(s_movieFiles[m_current->m_movie], 0, 0, 0);
    m_time = fn_80036464(0.0f);
}

Movie::~Movie() {
    UnkMovieStatus_805EE0B0 *status = lbl_805EE0B0;
    MovieEntry *entry = m_entries;
    if (entry != 0) {
        while (entry != 0) {
            MovieEntry *done = entry;
            entry = entry->m_next;
            delete done;
        }
        m_entries = 0;
    }
    if (status != 0) {
        status->m_flags |= 1;
    }
}

void Movie::Update(Context *context) {
    UpdateMovie(context);
}

/* NonMatching (89%): retail loads context->m_layout first; GC/2.7 schedules it after the load of
 * lbl_805EE0B0 for every shape tried. */
void Movie::Draw(Context *context) {
    s32 layout = context->m_layout;
    Position *position = &s_moviePositions[layout];
    if (lbl_805EE0B0 != 0) {
        fn_80036570(position->x, position->y, 0.425f, 0.425f);
    }
    DrawFrame(position->x, position->y, 0.425f);
    if (m_brightness < 1.0f) {
        DrawFade(position->x, position->y, 0.425f);
    }
}

void Movie::UpdateMovie(Context *context) {
    MovieEntry *current = m_current;
    if (current != 0) {
        if (context->m_page > current->m_lastPage) {
            if (lbl_805EE0B0 != 0) {
                lbl_805EE0B0->m_flags |= 1;
            } else {
                MovieEntry *next = current->m_next;
                if (next == 0) {
                    next = m_entries;
                }
                if (fn_80036E78(s_movieFiles[next->m_movie], 0, 0, 0)) {
                    m_current = next;
                    m_time = -1.0f;
                }
            }
        }
        if (m_time < 0.0f) {
            m_time = fn_80036464(0.0f);
        }
        float clock = context->m_pageClock;
        float fade = context->m_fadeTime;
        if (context->m_page == m_current->m_firstPage && clock < fade) {
            m_brightness = 1.0f - context->m_alpha;
        }
        if (context->m_page == m_current->m_lastPage && context->m_pageTime - clock - context->m_gapTime < fade) {
            m_brightness = 1.0f - context->m_alpha;
        }
    }
}

/* NonMatching (99.5%): retail keeps scale in f26 and the outer bottom edge in f27; GC/2.7 swaps
 * them for every declaration order tried. */
void Movie::DrawFrame(s32 x, s32 y, float scale) {
    /* The frame around the movie, outer and inner edges, at scale 1. */
    float frameX[4] = {-1.0f, 641.0f, 8.0f, 632.0f};
    float frameY[4] = {-1.0f, 481.0f, 8.0f, 472.0f};
    RwIm2DVertex vertices[4];
    vertices[0].z = 0.1f;
    vertices[0].emissiveColor.red = 0;
    vertices[0].emissiveColor.green = 0;
    vertices[0].emissiveColor.blue = 0;
    vertices[0].emissiveColor.alpha = 255;
    vertices[1].z = 0.1f;
    vertices[1].emissiveColor.red = 0;
    vertices[1].emissiveColor.green = 0;
    vertices[1].emissiveColor.blue = 0;
    vertices[1].emissiveColor.alpha = 0;
    vertices[2].z = 0.1f;
    vertices[2].emissiveColor.red = 0;
    vertices[2].emissiveColor.green = 0;
    vertices[2].emissiveColor.blue = 0;
    vertices[2].emissiveColor.alpha = 255;
    vertices[3].z = 0.1f;
    vertices[3].emissiveColor.red = 0;
    vertices[3].emissiveColor.green = 0;
    vertices[3].emissiveColor.blue = 0;
    vertices[3].emissiveColor.alpha = 0;
    lbl_805F15B4->fpRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)0);
    float outerLeft;
    float innerLeft;
    float outerTop;
    float innerTop;
    float outerBottom;
    float outerRight;
    float innerBottom;
    float innerRight;
    outerLeft = frameX[0] * scale + x;
    outerTop = frameY[0] * scale + y;
    innerLeft = frameX[2] * scale + x;
    innerTop = frameY[2] * scale + y;
    outerBottom = frameY[1] * scale + y;
    innerBottom = frameY[3] * scale + y;
    vertices[0].x = outerLeft;
    vertices[0].y = outerTop;
    vertices[1].x = innerLeft;
    vertices[1].y = innerTop;
    vertices[2].x = outerLeft;
    vertices[2].y = outerBottom;
    vertices[3].x = innerLeft;
    vertices[3].y = innerBottom;
    lbl_805F15B4->fpIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, vertices, 4);
    outerRight = frameX[1] * scale + x;
    innerRight = frameX[3] * scale + x;
    vertices[0].x = outerLeft;
    vertices[0].y = outerTop;
    vertices[1].x = innerLeft;
    vertices[1].y = innerTop;
    vertices[2].x = outerRight;
    vertices[2].y = outerTop;
    vertices[3].x = innerRight;
    vertices[3].y = innerTop;
    lbl_805F15B4->fpIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, vertices, 4);
    vertices[0].x = outerRight;
    vertices[0].y = outerTop;
    vertices[1].x = innerRight;
    vertices[1].y = innerTop;
    vertices[2].x = outerRight;
    vertices[2].y = outerBottom;
    vertices[3].x = innerRight;
    vertices[3].y = innerBottom;
    lbl_805F15B4->fpIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, vertices, 4);
    vertices[0].x = outerRight;
    vertices[0].y = outerBottom;
    vertices[1].x = innerRight;
    vertices[1].y = innerBottom;
    vertices[2].x = outerLeft;
    vertices[2].y = outerBottom;
    vertices[3].x = innerLeft;
    vertices[3].y = innerBottom;
    lbl_805F15B4->fpIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, vertices, 4);
}

void Movie::DrawFade(s32 x, s32 y, float scale) {
    RwIm2DVertex vertices[4];
    float left = (float)x - 4.0f;
    float right = left + (640.0f * scale + 8.0f);
    float top = (float)y - 4.0f;
    float bottom = 4.0f + top;
    vertices[0].x = left;
    vertices[0].y = top;
    vertices[0].z = 0.1f;
    vertices[0].emissiveColor.red = 0;
    vertices[0].emissiveColor.green = 0;
    vertices[0].emissiveColor.blue = 0;
    vertices[0].emissiveColor.alpha = (u32)(255.0f * m_brightness);
    vertices[1].x = right;
    vertices[1].y = top;
    vertices[1].z = 0.1f;
    vertices[1].emissiveColor.red = 0;
    vertices[1].emissiveColor.green = 0;
    vertices[1].emissiveColor.blue = 0;
    vertices[1].emissiveColor.alpha = (u32)(255.0f * m_brightness);
    vertices[2].x = left;
    vertices[2].y = bottom;
    vertices[2].z = 0.1f;
    vertices[2].emissiveColor.red = 0;
    vertices[2].emissiveColor.green = 0;
    vertices[2].emissiveColor.blue = 0;
    vertices[2].emissiveColor.alpha = (u32)(255.0f * m_brightness);
    vertices[3].x = right;
    vertices[3].y = bottom;
    vertices[3].z = 0.1f;
    vertices[3].emissiveColor.red = 0;
    vertices[3].emissiveColor.green = 0;
    vertices[3].emissiveColor.blue = 0;
    vertices[3].emissiveColor.alpha = (u32)(255.0f * m_brightness);
    lbl_805F15B4->fpRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)0);
    lbl_805F15B4->fpIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, vertices, 4);
}

/* NonMatching (96%): retail loads the file pointer before the int-to-float constant; GC/2.7
 * hoists the constant's load above it (see docs/research-notes.md on load scheduling). */
void Movie::BuildList(Context *context) {
    MovieEntry *entry = m_entries;
    if (entry != 0) {
        while (entry != 0) {
            MovieEntry *done = entry;
            entry = entry->m_next;
            delete done;
        }
        m_entries = 0;
    }
    MovieEntry *last = new MovieEntry;
    last->m_movie = 0;
    last->m_firstPage = 0;
    last->m_lastPage = 1000;
    last->m_next = 0;
    float length = 0.0f;
    length += s_movieLengths[0];
    m_current = last;
    m_entries = last;
    for (u32 i = 1; i < 5; i++) {
        int story = fn_8025D280(s_movieEvents[i]);
        if (story != -1 && fn_80174424(fn_800BB74C()->m_events, story)) {
            MovieEntry *added = new MovieEntry;
            added->m_movie = i;
            added->m_firstPage = 0;
            added->m_lastPage = 0;
            added->m_next = 0;
            length += s_movieLengths[i];
            last->m_next = added;
            last = added;
        }
    }
    last->m_next = new MovieEntry;
    MovieEntry *final = last->m_next;
    final->m_movie = 7;
    length += s_movieLengths[7];
    File *file = context->m_file;
    final->m_lastPage = file->m_pageCount;
    final->m_next = 0;
    float pageLength = length / file->m_pageCount;
    s32 page = 0;
    entry = m_entries;
    do {
        s32 count = 0.5f + s_movieLengths[entry->m_movie] / pageLength;
        page += count > 0 ? count : 1;
        entry->m_lastPage = page;
        entry = entry->m_next;
        entry->m_firstPage = page + 1;
    } while (entry->m_next != 0);
}
