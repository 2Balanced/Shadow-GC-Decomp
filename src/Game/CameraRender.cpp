/* The tasks that bracket the drawing of a frame (class names from RTTI; Game/CameraRender.h):
 * RenderStart, CameraRender, CameraRenderClose, RenderStop and CameraMatrixChangeFor2D. File name
 * from the class that draws; other names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#define MSL_VECTOR_MIDDLE
#include <vector>

#include "Game/CameraRender.h"
#include "Game/HandleUser.h"
#include "Game/ScreenHolder.h"
#include "Game/Task.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* A frame timer (struct name unknown; 0x20 bytes): the time a frame took and when it started. */
struct UnkTimer_80573B78 {
    /* 0x00 */ u64 m_elapsed;
    /* 0x08 */ u64 m_start;
    /* 0x10 */ u8 field_0x10[0x20 - 0x10];
};

/* The timers (name unknown). */
extern UnkTimer_80573B78 lbl_80573B78[];

/* A RenderWare world with its flags at 0x8 (struct name unknown). */
struct UnkWorld_800289B0 {
    /* 0x0 */ u8 field_0x0[0x8];
    /* 0x8 */ u32 m_flags;
};

/* The renderer (class name unknown; see Game/EggLastMechFeverRing.cpp): its world at 0x45C. */
struct UnkRenderer_800289B0 {
    /* 0x000 */ u8 field_0x0[0x45C];
    /* 0x45C */ UnkWorld_800289B0 *m_world;
};

/* The stage, whose areas are drawn (class name unknown; see Game/BossCtrlUnit.cpp). */
class UnkStage_8003A840;

/* The game state; the frame is cleared to the colour it starts with (see Game/TGi.h). */
struct UnkGameState_8057D6A0;
extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* Whether a movie is playing (see Game/AdvLogo.cpp). */
struct UnkMovieStatus_805EE0B0;
extern UnkMovieStatus_805EE0B0 *lbl_805EE0B0;

extern "C" {
/* The screen (Game/ScreenHolder.h). */
UnkScreenHolder_8000DCC0 *fn_8000DCC0(void);
/* The time (see Game/BossSonicTarget.cpp). */
u64 fn_80051A94(void);
/* RenderWare (by their arguments): RwCameraClear, RwCameraBeginUpdate, RwCameraEndUpdate and the
 * world's camera added and removed (behaviour inferred). */
UnkScreen_8000DCC0 *fn_804857BC(UnkScreen_8000DCC0 *camera, void *color, int clearMode);
UnkScreen_8000DCC0 *fn_804854DC(UnkScreen_8000DCC0 *camera);
UnkScreen_8000DCC0 *fn_804854B4(UnkScreen_8000DCC0 *camera);
void fn_80462130(UnkWorld_800289B0 *world, UnkScreen_8000DCC0 *camera);
void fn_80462184(UnkWorld_800289B0 *world, UnkScreen_8000DCC0 *camera);
/* Shows the frame drawn with the camera (behaviour inferred). */
void fn_800519F0(UnkScreen_8000DCC0 *camera);
/* Per-frame calls before the frame is drawn, the second while a movie plays (meanings unknown). */
void fn_80377BC0(void);
void fn_8040305C(void);
/* The renderer and its world drawn (behaviour inferred). */
UnkRenderer_800289B0 *fn_800289B0(void);
void fn_80047DFC(UnkRenderer_800289B0 *renderer, UnkWorld_800289B0 *world);
/* The stage and one of its areas drawn (behaviour inferred). */
UnkStage_8003A840 *fn_8003A840(void);
void fn_80046D58(UnkStage_8003A840 *stage, int area, int arg2);
/* Gives a HandleUser its handle (behaviour inferred). */
void fn_80045A64(HandleUser &user);
}

/* Cleared by RenderStart's constructor (meaning unknown). */
u8 lbl_805EF948;

const char *CameraRender::s_taskName = "CameraRender";
const char *CameraRenderClose::s_taskName = "CameraRenderClose";
const char *RenderStart::s_taskName = "RenderStart";
const char *RenderStop::s_taskName = "RenderStop";

CameraRender::CameraRender(Task *parent, int area) : Task(parent) {
    m_name = s_taskName;
    m_flags |= 0x100;
    fn_80045A64(*this);
    m_area = area;
}

void CameraRender::Render() {
    fn_80046D58(fn_8003A840(), m_area, 0);
}

void CameraRender::Tick(float dt) {
    UnkWorld_800289B0 *world = fn_800289B0()->m_world;
    if (world != 0) {
        fn_80462130(world, fn_8000DCC0()->m_camera);
    }
    fn_804854DC(fn_8000DCC0()->m_camera);
    Render();
    if (world != 0) {
        world->m_flags |= 0x30;
        fn_80047DFC(fn_800289B0(), world);
    }
}

CameraRenderClose::CameraRenderClose(Task *parent) : Task(parent) {
    m_name = s_taskName;
    m_flags |= 0x100;
    fn_80045A64(*this);
}

void CameraRenderClose::Tick(float dt) {
    fn_804854B4(fn_8000DCC0()->m_camera);
    UnkWorld_800289B0 *world = fn_800289B0()->m_world;
    if (world != 0) {
        fn_80462184(world, fn_8000DCC0()->m_camera);
    }
}

RenderStart::RenderStart(Task *parent) : Task(parent) {
    m_name = s_taskName;
    m_flags |= 0x100;
    lbl_805EF948 = 0;
}

/* Starts a frame timer. */
static void StartTimer(int index) {
    lbl_80573B78[index].m_start = fn_80051A94();
}

void RenderStart::Tick(float dt) {
    fn_80377BC0();
    if (lbl_805EE0B0 != 0) {
        fn_8040305C();
    }
    fn_804857BC(fn_8000DCC0()->m_camera, &lbl_8057D6A0, 3);
    StartTimer(0);
}

RenderStop::RenderStop(Task *parent) : Task(parent) {
    m_name = s_taskName;
    m_flags |= 0x100;
}

/* Stops a frame timer. */
static void StopTimer(int index) {
    lbl_80573B78[index].m_elapsed = fn_80051A94() - lbl_80573B78[index].m_start;
}

void RenderStop::Tick(float dt) {
    UnkScreen_8000DCC0 *camera = fn_8000DCC0()->m_camera;
    StopTimer(0);
    fn_800519F0(camera);
}

CameraMatrixChangeFor2D::CameraMatrixChangeFor2D(Task *parent, bool set2D) : Task(parent) {
    m_flags |= 0x100;
    m_set2D = set2D;
    if (m_set2D) {
        m_name = "CameraMatrix->Set2D";
    } else {
        m_name = "CameraMatrix->Reset2D";
    }
}

void CameraMatrixChangeFor2D::Tick(float dt) {
    if (m_set2D) {
        fn_8000DCC0()->ReadCamera();
    } else {
        fn_8000DCC0()->PlaceCamera();
    }
}

CameraMatrixChangeFor2D::~CameraMatrixChangeFor2D() {}
