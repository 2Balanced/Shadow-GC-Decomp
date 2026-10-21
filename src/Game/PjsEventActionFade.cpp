/* TPjsEventActionFade (class name from RTTI): a task that fades the screen for an event. Type 1
 * fades in white and type 2 in black, through the screen fade at 0x80173878; type 3 instead draws a
 * translucent orange rectangle over the screen from a draw callback it registers with a Render.
 * fn_80328DDC starts one below the current event's task. File name inferred from the class. Names
 * marked "(name inferred)" are not original; the functions other units call keep their fn_ names.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/Render.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The RenderWare types used here, declared locally (layouts as in rw/rwcore.h). */
extern "C" {
typedef struct RwRGBA {
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
} RwRGBA;

typedef struct RwRect {
    s32 x;
    s32 y;
    s32 w;
    s32 h;
} RwRect;
}

/* The game state (struct name unknown; see Game/CityBombSmall.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x44];
    /* 0x044 */ s32 m_screenWidth;  /* (name inferred) */
    /* 0x048 */ s32 m_screenHeight; /* (name inferred) */
    /* 0x04C */ u8 field_0x4C[0x150 - 0x4C];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* What fn_801723E0 returns (class name unknown): the event's task, which a fade becomes a child
 * of. */
struct UnkEventHolder_801723E0 {
    /* The task, if any (name inferred). */
    Task *GetTask() {
        return m_task ? m_task : 0;
    }

    /* 0x0 */ Task *m_task;
};

extern "C" {
/* Creates a Render (behaviour inferred; see Game/EggLastMechSlotDisp2D.cpp). */
boost::shared_ptr<Render> fn_8004E24C(void *arg0);
/* Fills rect with color (behaviour inferred). */
void fn_800396D0(const RwRect *rect, const RwRGBA *color, int arg2);
/* The fade's time and colour (behaviour inferred). */
void fn_802051F8(float time);
void fn_80204874(const RwRGBA *color);
/* Screen fades (behaviour inferred; see Game/AdvMemoryCheck.cpp). */
void fn_80173834(int arg0, float time);
void fn_80173878(int arg0, int arg1, float time);
/* Whether the screen fade has finished (behaviour inferred; see Game/AdvMemoryCheck.cpp). */
int fn_80044E10(void);
UnkEventHolder_801723E0 *fn_801723E0(void);

void fn_803285EC(void);
}

/* Set while fades may not start (name inferred). */
static bool s_fadeLocked;

/* The fade colours: the reset (transparent black), type 3's rectangle (translucent orange), and
 * types 1 (white) and 2 (black). */
const RwRGBA lbl_805F9C80 = {0x00, 0x00, 0x00, 0x00};
const RwRGBA lbl_805F9C84 = {0xFF, 0x80, 0x00, 0x40};
const RwRGBA lbl_805F9C88 = {0xFF, 0xFF, 0xFF, 0xFF};
const RwRGBA lbl_805F9C8C = {0x00, 0x00, 0x00, 0x00};

class TPjsEventActionFade : public Task {
  public:
    TPjsEventActionFade(Task *parent, int type);
    virtual ~TPjsEventActionFade();
    virtual void Tick(float dt);

    /* The Render's draw callback (name inferred). */
    void Draw(RednerDrawArgs &args);

    /* 0x28 */ s32 m_type;
    /* 0x2C */ float m_time;
    /* 0x30 */ u8 m_starting;
    /* 0x34 */ RwRect m_rect;
    /* 0x44 */ boost::shared_ptr<Render> m_render;
    /* 0x4C */ u32 field_0x4C;
};
SIZE_ASSERT(TPjsEventActionFade, 0x50);

/* Starts a fade of type below the current event's task (name inferred). */
extern "C" void fn_80328DDC(int type) {
    if (!s_fadeLocked) {
        Task *parent = fn_801723E0()->GetTask();
        if (parent != 0) {
            new TPjsEventActionFade(parent, type);
        }
    }
}

TPjsEventActionFade::TPjsEventActionFade(Task *parent, int type) : Task(parent), m_type(type), m_starting(1) {
    m_name = "TPjsEventActionFade";
    switch (type) {
    case 1:
        m_time = 1.0f / 6.0f;
        fn_802051F8(m_time);
        fn_80204874(&lbl_805F9C88);
        fn_80173878(0, 1, 1.0f);
        break;
    case 2:
        m_time = 1.0f / 12.0f;
        fn_802051F8(m_time);
        fn_80204874(&lbl_805F9C8C);
        fn_80173878(0, 1, 1.0f);
        break;
    case 3:
        m_render = fn_8004E24C(fn_80046178()->field_0x18);
        fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&TPjsEventActionFade::Draw, this, _1));
        m_rect.x = 0;
        m_rect.y = 0;
        m_rect.w = lbl_8057D6A0.m_screenWidth;
        m_rect.h = lbl_8057D6A0.m_screenHeight;
        break;
    default:
        return;
    }
}

TPjsEventActionFade::~TPjsEventActionFade() {
    if (m_type != 3) {
        fn_803285EC();
    }
}

void TPjsEventActionFade::Tick(float dt) {
    if (m_type != 3 && s_fadeLocked) {
        m_flags |= 1;
    } else {
        switch (m_type) {
        case 1:
            if (m_starting) {
                if (fn_80044E10() == 1) {
                    m_starting = 0;
                    fn_80173834(1, 0.0f);
                }
            } else if (fn_80044E10() == 0) {
                m_flags |= 1;
            }
            break;
        case 3:
            break;
        case 2:
        default:
            m_flags |= 1;
            break;
        }
    }
}

void TPjsEventActionFade::Draw(RednerDrawArgs &args) {
    switch (m_type) {
    case 3:
        fn_800396D0(&m_rect, &lbl_805F9C84, 0);
        break;
    }
}

/* Lets fades start again (name inferred). */
extern "C" void fn_80328640(void) {
    s_fadeLocked = false;
}

/* Stops fades from starting and resets the fade (name inferred). */
extern "C" void fn_80328618(void) {
    s_fadeLocked = true;
    fn_803285EC();
}

/* Resets the fade (name inferred). */
extern "C" void fn_803285EC(void) {
    fn_802051F8(1.0f);
    fn_80204874(&lbl_805F9C80);
}
