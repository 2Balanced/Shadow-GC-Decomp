/* The save/load cycle's message window (Game/SaveCycle.cpp runs the cycle). One game-wide object
 * (UnkObject_802E5B2C, reached through fn_80117514) owns the window's state: a phase requests one
 * of the 75 messages below (fn_802E585C, fn_802E58B4), SaveCycle::MessageDrawTask (class name from
 * RTTI) fades the window in, reads the pad and draws the message and its choices each frame, and
 * the answer is read back once a choice is confirmed. The text comes from the message source the
 * stage attaches (fn_802E597C). File name, the state's class name and all member names are
 * inferred; the functions other units call keep their fn_ names. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first).
 *
 * NonMatching: 18 of 19 functions match. DrawWindow (0x802E4C58) differs in its prologue (retail
 * loads m_alpha above the stmw, as in Game/VehiclePathMove.cpp) and in the registers of the frame
 * rectangles (r6-r10 permuted; about 20 source forms tried). The .sdata2 constants and two
 * .rodata strings also come out in another order under GC/2.7, and the weak boost exception
 * classes add RTTI records to .sdata. */
#include "Game/Task.h"
#include "Game/UnkSingletons.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* RenderWare's rectangle and colour. */
struct RwRect {
    s32 x;
    s32 y;
    s32 w;
    s32 h;
};

struct RwRGBA {
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
};

/* A 2D vertex as RwIm2DRenderPrimitive takes it. */
struct UnkIm2DVertex {
    /* 0x00 */ float x;
    /* 0x04 */ float y;
    /* 0x08 */ float z;
    /* 0x0C */ RwRGBA color;
    /* 0x10 */ float u;
    /* 0x14 */ float v;
};
SIZE_ASSERT(UnkIm2DVertex, 0x18);

/* RenderWare's globals (RwEngineInstance; only the members used here). */
struct UnkRaster_805F15B4 {
    /* 0x00 */ u8 field_0x0[0xC];
    /* 0x0C */ s32 width;
    /* 0x10 */ s32 height;
};

struct UnkCamera_805F15B4 {
    /* 0x00 */ u8 field_0x0[0x60];
    /* 0x60 */ UnkRaster_805F15B4 *frameBuffer;
};

struct UnkRwGlobals_805F15B4 {
    /* 0x00 */ UnkCamera_805F15B4 *curCamera;
    /* 0x04 */ u8 field_0x4[0x18 - 0x4];
    /* 0x18 */ float zBufferNear;
    /* 0x1C */ u8 field_0x1C[0x30 - 0x1C];
    /* 0x30 */ int (*im2DRenderPrimitive)(int type, UnkIm2DVertex *vertices, int count);
};

/* The game state (only the frame time is used here; see Game/AdvMemoryCheck.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ float m_frameTime;
};

/* One pad's state (struct name unknown; see Game/StageSequenceMovieTest.cpp). Pad n is entry n + 1
 * (the layout of the first entry is unknown). */
struct UnkPad_80051734 {
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ u32 m_triggered;
    /* 0x0C */ u8 field_0xC[0x28 - 0xC];
};
SIZE_ASSERT(UnkPad_80051734, 0x28);

struct UnkPads_80051734 {
    /* The buttons pressed this frame on pad. */
    u32 GetTriggered(int pad) {
        return m_pads[pad + 1].m_triggered;
    }

    /* Whether all of mask, or all of its pad or stick part, was pressed on pad (names inferred). */
    bool IsPressed(int pad, u32 mask) {
        u32 buttons = m_pads[pad + 1].m_triggered;
        if ((mask & buttons) == mask) {
            return true;
        }
        u32 part = mask & 0x3C0;
        if (part != 0 && (part & buttons) == part) {
            return true;
        }
        part = mask & 0x3C0000;
        if (part != 0 && (part & buttons) == part) {
            return true;
        }
        return false;
    }

    /* 0x00 */ UnkPad_80051734 m_pads[6];
};

/* The message source (class name unknown; only the members used here). */
class UnkHintTarget_801FB180 {
  public:
    /* 0x00 */ u8 field_0x0[0x58];
    /* 0x58 */ void *m_font;
    /* 0x5C */ u8 field_0x5C[0x64 - 0x5C];
    /* 0x64 */ float m_scale;
    /* 0x68 */ u8 field_0x68[0x70 - 0x68];
    /* 0x70 */ s32 m_align;
};

struct UnkRenderOwner_80046178 {
    /* 0x00 */ u8 field_0x0[0x40];
    /* 0x40 */ Task *m_drawLayer;
};

extern UnkRwGlobals_805F15B4 *lbl_805F15B4;
extern UnkGameState_8057D6A0 lbl_8057D6A0;

extern "C" {
/* Fills rect with color (behaviour inferred; see Game/PjsEventActionFade.cpp). */
void fn_800396D0(const RwRect *rect, const RwRGBA *color, int arg2);
/* The font's corner colours (behaviour inferred). */
void fn_80470694(void *font, const RwRGBA *topLeft, const RwRGBA *topRight,
                 const RwRGBA *bottomLeft, const RwRGBA *bottomRight);
/* Draws page index of message id at x, y (behaviour inferred; see Game/MessageTask.cpp). */
void *fn_80049BC0(UnkHintTarget_801FB180 *target, s32 id, s32 branch, s32 index, float x, float y,
                  s32 *voice, u8 changed);
/* Whether the vertical choices sit at their default place (meaning unknown). */
int fn_80305BE8(void);
/* The pads, and whether button was pressed on pad (behaviour inferred). */
UnkPads_80051734 *fn_80051734(void);
bool fn_80356260(int button, int pad);
/* The sound manager, and playing sound id through it. */
void *fn_80053164(void);
void fn_801D560C(void *manager, int soundId, int arg2, int arg3);
UnkRenderOwner_80046178 *fn_80046178(void);
}

namespace SaveCycle {

/* SaveCycle::MessageDrawTask (RTTI): runs and draws the window each frame. */
class MessageDrawTask : public Task {
  public:
    MessageDrawTask(Task *parent);
    virtual ~MessageDrawTask();
    virtual void Tick(float dt);
};
SIZE_ASSERT(MessageDrawTask, 0x28);

} // namespace SaveCycle

/* A message: its text, its choices and the answer each choice gives. */
struct UnkSaveMessage {
    /* 0x00 */ s32 m_text;
    /* 0x04 */ s32 m_numChoices;
    /* 0x08 */ s32 m_defaultChoice;
    /* 0x0C */ s32 m_layout; /* 1: side by side, 2: one below the other */
    /* 0x10 */ s32 m_choices[3];
    /* 0x1C */ s32 *m_results;
    /* 0x20 */ s32 m_kind; /* without choices: 1 answers with A or B, 2 with the confirm button */
};
SIZE_ASSERT(UnkSaveMessage, 0x24);

/* The window's state (class name unknown). */
class UnkSaveMessage_802E5B74 {
  public:
    UnkSaveMessage_802E5B74();
    ~UnkSaveMessage_802E5B74();

    void Input();
    void DrawText();
    void DrawWindow();

    /* 0x00 */ boost::weak_ptr<UnkHintTarget_801FB180> m_target;
    /* 0x08 */ s32 m_request;
    /* 0x0C */ s32 m_current;
    /* 0x10 */ s32 m_cursor;
    /* 0x14 */ s32 m_result;
    /* 0x18 */ float m_alpha;
    /* 0x1C */ s32 m_closeDelay;
    /* 0x20 */ float m_timer;
    /* 0x24 */ float m_time;
    /* 0x28 */ SaveCycle::MessageDrawTask *m_drawTask;
    /* 0x2C */ s32 m_closing;
    /* 0x30 */ s32 m_pad;
    /* 0x34 */ s32 m_quick;
};
SIZE_ASSERT(UnkSaveMessage_802E5B74, 0x38);

#define STATE(object) ((UnkSaveMessage_802E5B74 *)(object)->field_0x0)

/* Not original: data that only unreferenced objects of the original placed here (four rows of
 * floats; the unit's .rodata is kept whole). */
static const float s_unused[4][4] = {
    {1.0f, 0.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
};

/* The answers: of the single-choice messages, and of the choices (names inferred). */
static s32 s_singleResults[12] = {2, 1, 3, 2, 3, 1, 1, 3, 2, 3, 2, 1};
static s32 s_choiceResults[3] = {1, 2, 3};

static const UnkSaveMessage s_messages[75] = {
    {0x177C, 0, 0, 0, {0, 0, 0}, 0, 0},
    {0x178A, 2, 0, 2, {0x17A6, 0x179B, 0}, s_choiceResults, 0},
    {0x177E, 2, 0, 2, {0x17A6, 0x179B, 0}, s_choiceResults, 0},
    {0x177F, 2, 0, 2, {0x179F, 0x17A0, 0}, s_choiceResults, 0},
    {0x1780, 2, 0, 2, {0x17A6, 0x179B, 0}, s_choiceResults, 0},
    {0x1781, 3, 0, 2, {0x17A6, 0x179B, 0x179D}, s_choiceResults, 0},
    {0x1782, 3, 0, 2, {0x17A6, 0x179B, 0x179D}, s_choiceResults, 0},
    {0x177D, 2, 0, 2, {0x179C, 0x179B, 0}, s_choiceResults, 0},
    {0x1784, 2, 0, 2, {0x17A6, 0x179B, 0}, s_choiceResults, 0},
    {0x1785, 2, 0, 2, {0x17A6, 0x179B, 0}, s_choiceResults, 0},
    {0x1786, 2, 0, 2, {0x179F, 0x17A0, 0}, s_choiceResults, 0},
    {0x177E, 2, 0, 2, {0x179C, 0x179B, 0}, s_choiceResults, 0},
    {0x1788, 2, 0, 2, {0x179F, 0x17A0, 0}, s_choiceResults, 0},
    {0x1789, 2, 0, 2, {0x17A3, 0x179B, 0}, s_choiceResults, 0},
    {0x1780, 2, 0, 2, {0x179C, 0x179B, 0}, s_choiceResults, 0},
    {0x1781, 3, 0, 2, {0x179C, 0x179B, 0x179D}, s_choiceResults, 0},
    {0x178C, 2, 0, 2, {0x17A3, 0x179B, 0}, s_choiceResults, 0},
    {0x1786, 2, 0, 2, {0x17A3, 0x179B, 0}, s_choiceResults, 0},
    {0x178E, 2, 0, 2, {0x17A3, 0x179B, 0}, s_choiceResults, 0},
    {0x1782, 3, 0, 2, {0x179C, 0x179B, 0x179D}, s_choiceResults, 0},
    {0x178B, 3, 0, 2, {0x17A3, 0x179B, 0x179E}, s_choiceResults, 0},
    {0x1787, 2, 0, 2, {0x179C, 0x179B, 0}, s_choiceResults, 0},
    {0x1785, 2, 0, 2, {0x179C, 0x179B, 0}, s_choiceResults, 0},
    {0x1793, 0, 0, 0, {0, 0, 0}, 0, 0},
    {0x1594, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
    {0x1595, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
    {0x1596, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
    {0x1597, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
    {0x1598, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
    {0x1599, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
    {0x1794, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
    {0x1795, 2, 0, 2, {0x179C, 0x179B, 0}, s_choiceResults, 0},
    {0x1796, 0, 0, 0, {0, 0, 0}, 0, 0},
    {0x1797, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
    {0x1798, 0, 0, 0, {0, 0, 0}, 0, 0},
    {0x1799, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
    {0x179A, 2, 0, 2, {0x17A7, 0x179B, 0}, s_choiceResults, 0},
    {0x17BD, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
    {0x17BF, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
    {0x178B, 3, 0, 2, {0x179C, 0x179B, 0x179E}, s_choiceResults, 0},
    {0x178D, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
    {0x1782, 3, 0, 2, {0x17A3, 0x179B, 0x179D}, s_choiceResults, 0},
    {0x1783, 2, 1, 2, {0x179F, 0x17A0, 0}, s_choiceResults, 0},
    {0x179D, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
    {0x179E, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
    {0x17A9, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
    {0x178B, 2, 0, 2, {0x17A3, 0x179B, 0}, s_choiceResults, 0},
    {0x178B, 3, 0, 2, {0x17A7, 0x179B, 0x179E}, s_choiceResults, 0},
    {0x17A9, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
    {0x17AA, 3, 0, 2, {0x179C, 0x179B, 0x17A1}, s_choiceResults, 0},
    {0x1781, 3, 0, 2, {0x17A3, 0x179B, 0x179D}, s_choiceResults, 0},
    {0x1791, 0, 0, 0, {0, 0, 0}, 0, 0},
    {0x1792, 2, 0, 2, {0x17A3, 0x179B, 0}, s_choiceResults, 0},
    {0x1572, 2, 0, 2, {0x179F, 0x17A0, 0}, s_choiceResults, 0},
    {0x1576, 0, 0, 0, {0, 0, 0}, 0, 0},
    {0x1577, 2, 1, 1, {0x179F, 0x17A0, 0}, s_singleResults, 0},
    {0x1578, 2, 1, 1, {0x179F, 0x17A0, 0}, s_singleResults, 0},
    {0x1579, 2, 1, 1, {0x179F, 0x17A0, 0}, s_singleResults, 0},
    {0x15AE, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
    {0x15AF, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
    {0x15B0, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
    {0x15B1, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
    {0x15B2, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
    {0x15B3, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
    {0x15B4, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
    {0x15B5, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
    {0x15B6, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
    {0x15B7, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
    {0x15B8, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
    {0x1598, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
    {0x1598, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
    {0x1598, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
    {0x1598, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
    {0x15E4, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
    {0x17AB, 1, 0, 1, {0x17A1, 0, 0}, s_singleResults, 0},
};

/* Where the text goes (names inferred). */
struct UnkTextPos {
    float x;
    float y;
};
static UnkTextPos s_textPos = {0.61f, 0.5f};
static UnkTextPos s_textOffset = {0.0f, 0.1f};
/* The colours (names inferred). */
static RwRGBA s_textColor = {0xC0, 0xC0, 0xC0, 0xFF};
static RwRGBA s_choiceColor = {0x80, 0x80, 0x80, 0xFF};
static RwRGBA s_selectedColor = {0xFF, 0xFF, 0x00, 0xFF};
static RwRGBA s_dimColor = {0x00, 0x00, 0x00, 0xA0};
static RwRGBA s_windowColor = {0x00, 0x00, 0x50, 0xDC};
static RwRGBA s_frameColor = {0x10, 0x10, 0x80, 0xDC};
static RwRGBA s_shadeColor = {0x00, 0x00, 0x10, 0xDC};
static RwRGBA s_lightColor = {0x30, 0x30, 0xC8, 0xDC};

extern "C" void fn_802E56D4(UnkObject_802E5B2C *object);
extern "C" void fn_802E5570(UnkObject_802E5B2C *object);

namespace SaveCycle {

void MessageDrawTask::Tick(float dt) {
    fn_802E56D4(fn_80117514());
    fn_802E5570(fn_80117514());
}

MessageDrawTask::~MessageDrawTask() {}

} // namespace SaveCycle

UnkSaveMessage_802E5B74::UnkSaveMessage_802E5B74() {
    m_request = -1;
    m_current = -2;
    m_result = 0;
    m_alpha = 0.0f;
    m_closeDelay = 0;
    m_timer = 0.0f;
    m_time = 0.0f;
    m_drawTask = 0;
    m_closing = 1;
}

UnkObject_802E5B2C::UnkObject_802E5B2C() {
    field_0x0 = (u32) new UnkSaveMessage_802E5B74;
}

UnkSaveMessage_802E5B74::~UnkSaveMessage_802E5B74() {
    if (m_drawTask != 0) {
        m_drawTask->m_flags |= 1;
        m_drawTask = 0;
    }
}

UnkObject_802E5B2C::~UnkObject_802E5B2C() {
    if (field_0x0 != 0) {
        delete STATE(this);
        field_0x0 = 0;
    }
}

namespace SaveCycle {

MessageDrawTask::MessageDrawTask(Task *parent) : Task(parent) {
    m_name = "SaveMessageDrawTask";
    m_flags |= 0x100;
}

} // namespace SaveCycle

/* Creates the drawing task once (name inferred). */
extern "C" void fn_802E59A0(UnkObject_802E5B2C *object) {
    if (STATE(object)->m_drawTask == 0) {
        STATE(object)->m_drawTask = new SaveCycle::MessageDrawTask(fn_80046178()->m_drawLayer);
    }
}

/* Attaches the message source. */
extern "C" void fn_802E597C(UnkObject_802E5B2C *object,
                            const boost::weak_ptr<UnkHintTarget_801FB180> &target) {
    STATE(object)->m_target = target;
}

/* Detaches the message source. */
extern "C" void fn_802E5958(UnkObject_802E5B2C *object) {
    STATE(object)->m_target.reset();
}

/* Requests message id for pad (messages 0x45-0x48 even while another is shown). */
inline void RequestMessage(UnkObject_802E5B2C *object, int id, int pad, int quick) {
    if (id >= 0x45 && id <= 0x48) {
        STATE(object)->m_request = id;
        STATE(object)->m_pad = pad;
        STATE(object)->m_quick = quick;
        return;
    }
    if (STATE(object)->m_request == STATE(object)->m_current) {
        return;
    }
    STATE(object)->m_request = id;
    STATE(object)->m_pad = pad;
    STATE(object)->m_quick = quick;
}

/* Requests message id and returns its answer: 0 while none, 2 for a message without one. */
extern "C" int fn_802E58B4(UnkObject_802E5B2C *object, int id, int pad, int quick) {
    int result = 0;
    RequestMessage(object, id, pad, quick);
    const UnkSaveMessage *message = &s_messages[id];
    if (message->m_numChoices > 0 || message->m_kind != 0) {
        if (STATE(object)->m_current == id) {
            result = STATE(object)->m_result;
        }
    } else {
        result = 2;
    }
    return result;
}

extern "C" void fn_802E585C(UnkObject_802E5B2C *object, int id, int pad, int quick) {
    RequestMessage(object, id, pad, quick);
}

/* Closes the window at once. */
extern "C" void fn_802E582C(UnkObject_802E5B2C *object) {
    UnkSaveMessage_802E5B74 *state = STATE(object);
    state->m_request = -1;
    state->m_current = -2;
    state->m_closing = 1;
    state->m_alpha = 0.0f;
    state->m_closeDelay = 0;
}

/* Whether the window is fully hidden. */
extern "C" bool fn_802E5810(UnkObject_802E5B2C *object) {
    return STATE(object)->m_alpha == 0.0f;
}

/* Reads the pad and fades the window in or out. */
extern "C" void fn_802E56D4(UnkObject_802E5B2C *object) {
    STATE(object)->Input();
    UnkSaveMessage_802E5B74 *state = STATE(object);
    if (state->m_closing == 0) {
        if (state->m_time <= 0.5f && state->m_alpha <= 0.0f) {
            return;
        }
        if (state->m_alpha < 1.0f) {
            float dt = lbl_8057D6A0.m_frameTime;
            if (dt <= 0.0f) {
                dt = 1.0f / 60.0f;
            }
            state->m_alpha += dt / 0.3f;
            if (state->m_alpha > 1.0f) {
                state->m_alpha = 1.0f;
            }
        }
    } else if (state->m_closeDelay <= 0) {
        if (state->m_alpha > 0.0f) {
            float dt = lbl_8057D6A0.m_frameTime;
            if (dt <= 0.0f) {
                dt = 1.0f / 60.0f;
            }
            state->m_alpha -= dt / 0.3f;
            if (state->m_alpha < 0.0f) {
                state->m_alpha = 0.0f;
            }
        } else {
            state->m_current = -2;
        }
    } else {
        state->m_closeDelay--;
    }
}

/* Takes the requested message and draws the window. */
extern "C" void fn_802E5570(UnkObject_802E5B2C *object) {
    if (STATE(object)->m_alpha > 0.0f) {
        STATE(object)->DrawWindow();
    }
    UnkSaveMessage_802E5B74 *state = STATE(object);
    if (state->m_request >= 0) {
        const UnkSaveMessage *message = &s_messages[state->m_request];
        if (state->m_request != state->m_current) {
            state->m_current = state->m_request;
            STATE(object)->m_cursor = message->m_defaultChoice;
            STATE(object)->m_result = 0;
            STATE(object)->m_timer = 1.5f;
            STATE(object)->m_closing = 0;
            if (STATE(object)->m_quick != 0) {
                STATE(object)->m_time = 0.0f;
            } else {
                STATE(object)->m_time = 0.5f;
            }
        }
    } else if (state->m_closing == 0) {
        if (state->m_time <= 0.5f && state->m_alpha <= 0.0f) {
            state->m_closing = 1;
            STATE(object)->m_closeDelay = 0;
        } else if (state->m_timer < 0.0f) {
            state->m_closing = 1;
            STATE(object)->m_closeDelay = 3;
        }
    } else {
        state->m_request = -1;
        STATE(object)->m_current = -2;
        return;
    }
    STATE(object)->DrawText();
    STATE(object)->m_request = -1;
}

inline void PlaySound(int id) {
    fn_801D560C(fn_80053164(), id, 0, 0);
}

void UnkSaveMessage_802E5B74::Input() {
    if (m_alpha < 1.0f || m_request < 0 || m_closing != 0) {
        return;
    }
    const UnkSaveMessage *message = &s_messages[m_current];
    m_result = 0;
    if (message->m_numChoices > 0) {
        if (fn_80356260(1, m_pad)) {
            m_result = message->m_results[m_cursor];
            PlaySound(0xE003);
        } else if (message->m_layout != 0 && message->m_numChoices > 1) {
            const u32 masks[2][4] = {
                {0x00100100, 0x00200200, 0, 0},
                {0x00040040, 0x00080080, 0, 0},
            };
            if (fn_80051734()->IsPressed(m_pad, masks[message->m_layout - 1][0])) {
                if (--m_cursor < 0) {
                    m_cursor = 0;
                } else {
                    PlaySound(0xE002);
                }
            } else if (fn_80051734()->IsPressed(m_pad, masks[message->m_layout - 1][1])) {
                if (++m_cursor >= message->m_numChoices) {
                    m_cursor = message->m_numChoices - 1;
                } else {
                    PlaySound(0xE002);
                }
            }
        }
    } else if (message->m_kind == 1) {
        if ((fn_80051734()->GetTriggered(m_pad) & 1) == 1) {
            m_result = message->m_results[0];
            PlaySound(0xE003);
        } else if ((fn_80051734()->GetTriggered(m_pad) & 2) == 2) {
            m_result = message->m_results[1];
            PlaySound(0xE004);
        }
    } else if (message->m_kind == 2) {
        if (fn_80356260(0, m_pad)) {
            m_result = message->m_results[0];
            PlaySound(0xE003);
        }
    }
}

void UnkSaveMessage_802E5B74::DrawText() {
    static const float s_rowChoices[3][2] = {{0.0f, 0.2f}, {-0.1f, 0.2f}, {-0.2f, 0.2f}};
    static const float s_columnChoices[3][2] = {
        {-0.18f, -0.08f}, {-0.17f, -0.08f}, {-0.15f, -0.06f}};
    float step =
        lbl_8057D6A0.m_frameTime <= 1.0f / 60.0f ? 1.0f / 60.0f : lbl_8057D6A0.m_frameTime;
    m_time += step;
    if (m_alpha < 1.0f || m_current < 0) {
        return;
    }
    m_timer -= step;
    boost::shared_ptr<UnkHintTarget_801FB180> target = m_target.lock();
    if (target) {
        void *font;
        const UnkSaveMessage *message = &s_messages[m_current];
        font = target->m_font;
        fn_80470694(font, &s_textColor, &s_textColor, &s_textColor, &s_textColor);
        target->m_align = 2;
        target->m_scale = 0.05f;
        fn_80049BC0(target.get(), message->m_text, 1, 0, s_textPos.x + s_textOffset.x,
                    s_textPos.y + s_textOffset.y, 0, 0);
        if (message->m_numChoices > 0 && message->m_choices[0] >= 0) {
            int last = message->m_numChoices - 1;
            float x;
            float dx;
            float y;
            float dy;
            if (message->m_layout == 1) {
                x = s_textPos.x + s_rowChoices[last][0];
                dx = s_rowChoices[last][1];
                y = s_textPos.y - 0.2f;
                dy = 0.0f;
            } else if (message->m_layout == 2) {
                x = s_textPos.x;
                dx = 0.0f;
                if (fn_80305BE8() == 0) {
                    y = s_textPos.y + s_columnChoices[last][0];
                    dy = s_columnChoices[last][1];
                }
            }
            for (int i = 0; i < message->m_numChoices; i++) {
                if (i != m_cursor) {
                    fn_80470694(font, &s_choiceColor, &s_choiceColor, &s_choiceColor,
                                &s_choiceColor);
                } else {
                    fn_80470694(font, &s_selectedColor, &s_selectedColor, &s_selectedColor,
                                &s_selectedColor);
                }
                fn_80049BC0(target.get(), message->m_choices[i], 1, 0, x, y, 0, 0);
                x += dx;
                y += dy;
            }
            fn_80470694(font, &s_textColor, &s_textColor, &s_textColor, &s_textColor);
        }
    }
}

void UnkSaveMessage_802E5B74::DrawWindow() {
    u32 width = 299.0f * m_alpha;
    s32 left = 0x140 - width;
    width *= 2;
    UnkRaster_805F15B4 *raster = lbl_805F15B4->curCamera->frameBuffer;
    RwRect screen = {0, 0, raster->width, raster->height};
    fn_800396D0(&screen, &s_dimColor, 0);
    s32 top = 0x46;
    s32 height = 0x14A;
    RwRect window = {left + 5, top + 5, width - 10, height - 10};
    fn_800396D0(&window, &s_windowColor, 0);
    RwRect frame[4] = {
        {left, top, 4, height},
        {left + width - 4, top, 4, height},
        {left + 4, top, width - 8, 4},
        {left + 4, top + height - 4, width - 8, 4},
    };
    fn_800396D0(&frame[0], &s_frameColor, 0);
    fn_800396D0(&frame[1], &s_frameColor, 0);
    fn_800396D0(&frame[2], &s_frameColor, 0);
    fn_800396D0(&frame[3], &s_frameColor, 0);
    u32 points[5][2] = {
        {window.x - 1, window.y + window.h},
        {window.x - 1, window.y - 1},
        {window.x + window.w, window.y - 1},
        {window.x + window.w, window.y + window.h},
        {window.x - 1, window.y + window.h},
    };
    UnkIm2DVertex vertices[5];
    float z = lbl_805F15B4->zBufferNear;
    int i;
    for (i = 0; i < 5; i++) {
        vertices[i].x = points[i][0];
        vertices[i].y = points[i][1];
        vertices[i].z = z;
        vertices[i].color = s_shadeColor;
    }
    lbl_805F15B4->im2DRenderPrimitive(2, vertices, 3);
    for (i = 0; i < 5; i++) {
        vertices[i].color = s_lightColor;
    }
    lbl_805F15B4->im2DRenderPrimitive(2, &vertices[2], 3);
}
