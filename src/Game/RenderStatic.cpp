/* RenderStatic, RenderDiv, CameraEnd and CameraStart (class names from RTTI): the roots of the
 * render task tree, and the object behind fn_80046178 (Game/Render.h) that builds it: one task per
 * render level, named from the table below, with the camera tasks (Game/CameraRender.h) between
 * them. RenderDiv draws its children once per screen when the screen is split. The file is named
 * after the first class. Names marked "(name inferred)" are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include "Game/CameraRender.h"
#include "Game/Render.h"
#include "Game/ScreenHolder.h"
#include "Game/Task.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* The game state (struct name unknown; see Game/BossCtrl.cpp): the screen split and the viewport
 * being drawn (names inferred). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x60];
    /* 0x060 */ s32 m_split;       /* 0 one screen, 1 two, 2 four (inferred) */
    /* 0x064 */ s32 m_players[2];  /* the player each drawn screen follows */
    /* 0x06C */ s32 m_viewport;
    /* 0x070 */ s32 m_screen;      /* the screen being drawn */
    /* 0x074 */ u8 field_0x74[0x150 - 0x74];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

extern "C" {
/* Where new tasks go (see Game/Gadget.h). */
void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);
UnkScreenHolder_8000DCC0 *fn_8000DCC0(void);
/* Ticks a task's children (behaviour inferred). */
void fn_8004EC68(Task *task, float dt);
}

/* A render level (struct name unknown): its task's name, whether a CameraRender draws it (and
 * which area), which 2D matrix change goes before it, and the root it hangs under. */
struct RenderLevel {
    /* 0x00 */ const char *name;
    /* 0x04 */ bool render;
    /* 0x08 */ int area;
    /* 0x0C */ int matrix; /* 1: change to 2D, 2: change back */
    /* 0x10 */ int root;
};

class CameraStart : public Task {
  public:
    CameraStart(Task *parent);
    virtual void Tick(float dt);
};
SIZE_ASSERT(CameraStart, 0x28);

class CameraEnd : public Task {
  public:
    CameraEnd(Task *parent);
    virtual void Tick(float dt);
};
SIZE_ASSERT(CameraEnd, 0x28);

class RenderDiv : public Task {
  public:
    RenderDiv(Task *parent);
    virtual void Tick(float dt);
};
SIZE_ASSERT(RenderDiv, 0x28);

class RenderStatic : public Task {
  public:
    RenderStatic(Task *parent);
    virtual ~RenderStatic();
    virtual void Tick(float dt);
};
SIZE_ASSERT(RenderStatic, 0x28);

CameraStart::CameraStart(Task *parent) : Task(parent) {
    m_flags |= 0x100;
}

void CameraStart::Tick(float dt) {
    fn_8000DCC0()->SetLayout(lbl_8057D6A0.m_split);
}

CameraEnd::CameraEnd(Task *parent) : Task(parent) {
    m_flags |= 0x100;
}

void CameraEnd::Tick(float dt) {}

RenderDiv::RenderDiv(Task *parent) : Task(parent) {
    m_flags |= 0x100;
}

void RenderDiv::Tick(float dt) {
    UnkScreenHolder_8000DCC0 *screen = fn_8000DCC0();
    s32 split = lbl_8057D6A0.m_split;
    lbl_8057D6A0.m_screen = 0;
    if (split == 0) {
        lbl_8057D6A0.m_viewport = 0;
        screen->SetViewport(0);
        screen->PlaceCameraFor(lbl_8057D6A0.m_players[lbl_8057D6A0.m_screen]);
    } else if (split == 1) {
        lbl_8057D6A0.m_viewport = 1;
        screen->SetViewport(1);
        screen->PlaceCameraFor(lbl_8057D6A0.m_players[lbl_8057D6A0.m_screen]);
        fn_8004EC68(this, dt);
        lbl_8057D6A0.m_viewport = 2;
        lbl_8057D6A0.m_screen++;
        screen->SetViewport(2);
        screen->PlaceCameraFor(lbl_8057D6A0.m_players[lbl_8057D6A0.m_screen]);
    } else if (split == 2) {
        lbl_8057D6A0.m_viewport = 3;
        screen->SetViewport(3);
        screen->PlaceCameraFor(lbl_8057D6A0.m_players[lbl_8057D6A0.m_screen]);
        fn_8004EC68(this, dt);
        lbl_8057D6A0.m_viewport = 4;
        lbl_8057D6A0.m_screen++;
        screen->SetViewport(4);
        screen->PlaceCameraFor(lbl_8057D6A0.m_players[lbl_8057D6A0.m_screen]);
    }
}

RenderStatic::RenderStatic(Task *parent) : Task(parent) {
    m_flags |= 0x100;
}

void RenderStatic::Tick(float dt) {
    UnkScreenHolder_8000DCC0 *screen = fn_8000DCC0();
    lbl_8057D6A0.m_screen = 0;
    lbl_8057D6A0.m_viewport = 0;
    screen->SetViewport(0);
    screen->PlaceCameraFor(lbl_8057D6A0.m_players[lbl_8057D6A0.m_screen]);
}

UnkRenderOwner_80046178::UnkRenderOwner_80046178() {
    static const RenderLevel s_levels[] = {
        {"RenderLevel_PreRender", false, 0x0, 0, 0},
        {"RenderLevel_OpeqWorld", false, 0x0, 0, 1},
        {"RenderLevel_Opeq", true, 0x20, 0, 1},
        {"RenderLevel_PunchWorld", false, 0x0, 0, 1},
        {"RenderLevel_Punch", true, 0x800, 0, 1},
        {"RenderLevel_TransWorld", false, 0x0, 0, 1},
        {"RenderLevel_Trans", true, 0x40, 0, 1},
        {"RenderLevel_AddWorld", false, 0x0, 0, 1},
        {"RenderLevel_Add", true, 0x80, 0, 1},
        {"RenderLevel_GlareWorld", false, 0x0, 0, 1},
        {"RenderLevel_Glare", true, 0x80, 0, 1},
        {"RenderLevel_PostGlare", false, 0x0, 0, 1},
        {"RenderLevel_Sprite3D", true, 0x20, 0, 1},
        {"RenderLevel_Last", false, 0x0, 0, 1},
        {"RenderLevel_Sprite", true, 0x418, 1, 2},
        {"RenderLevel_OnFade", true, 0x418, 0, 2},
        {"RenderLevel_Gindows", true, 0x418, 0, 2},
        {"RenderLevel_SpriteLast", false, 0x0, 2, 2},
        {"RenderLevel_PostRender", false, 0x0, 0, 3},
        {"RenderLevel_SpriteBack", false, 0x0, 0, -1},
        {"RenderLevel_SpriteMiddle", false, 0x0, 0, -1},
        {"RenderLevel_SpriteFront", false, 0x0, 0, -1},
        {"RenderLevel_PostEffect", false, 0x0, 0, -1},
    };

    Task **levels = reinterpret_cast<Task **>(this);
    m_roots[0] = new CameraStart(fn_801EB304(fn_8001C4F8(), 5));
    m_roots[0]->m_name = "RenderAttach_Start";
    m_roots[1] = new RenderDiv(fn_801EB304(fn_8001C4F8(), 5));
    m_roots[1]->m_name = "RenderAttach_Div";
    m_roots[2] = new RenderStatic(fn_801EB304(fn_8001C4F8(), 5));
    m_roots[2]->m_name = "RenderAttach_Static";
    m_roots[3] = new CameraEnd(fn_801EB304(fn_8001C4F8(), 5));
    m_roots[3]->m_name = "RenderAttach_End";

    Task *root = m_roots[2];
    field_0x58 = new Task(root);
    field_0x58->m_flags |= 0x100;
    field_0x58->m_name = s_levels[22].name;
    int i;
    for (i = 0; i <= 18; i++) {
        levels[i] = new Task(m_roots[s_levels[i].root]);
        levels[i]->m_flags |= 0x100;
        levels[i]->m_name = s_levels[i].name;
    }

    new RenderStart(levels[0]);
    for (i = 1; i < 18; i++) {
        switch (s_levels[i].matrix) {
        case 0:
            break;
        case 1:
            new CameraMatrixChangeFor2D(levels[i], true);
            break;
        case 2:
            new CameraMatrixChangeFor2D(levels[i], false);
            break;
        }
        if (s_levels[i].render) {
            new CameraRender(levels[i], s_levels[i].area);
            new CameraRenderClose(levels[i + 1]);
        }
    }
    new RenderStop(levels[18]);

    for (i = 19; i <= 21; i++) {
        Task *parent = levels[14];
        levels[i] = new Task(parent);
        levels[i]->m_flags |= 0x100;
        levels[i]->m_name = s_levels[i].name;
    }
}

UnkRenderOwner_80046178::~UnkRenderOwner_80046178() {
    Task **levels = reinterpret_cast<Task **>(this);
    for (int i = 0; i < 0x17; i++) {
        levels[i] = 0;
    }
}

RenderStatic::~RenderStatic() {}
