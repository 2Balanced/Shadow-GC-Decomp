#ifndef GAME_NOWLOADING_H
#define GAME_NOWLOADING_H

/* The loading screen (Game/NowLoading.cpp). NowLoading (class name from RTTI) is the task the
 * stages start and stop (fn_802BEBE0, fn_802BEB90); while a stage loads it runs a
 * CNowLoadingThread (class name from RTTI), which draws either the stage's title (when a stage is
 * given) or the "Now Loading" screen, NowLoadingScreen, into the frame buffer about 60 times a
 * second. Member names and the screen's class name are inferred. */

#include "Game/Task.h"
#include <dolphin/types.h>
#include "size_assert.h"

struct RwCamera;
struct RwRaster;

/* The loaded CSD project (Chao::CSD::CProject, Game/csdmProject.cpp; see Game/TGi.h). */
namespace Chao {
namespace CSD {
class CProject {
  public:
    CProject();
    virtual ~CProject();

    /* 0x04 */ u8 field_0x4[0x4C - 0x4];
};
} // namespace CSD
} // namespace Chao

/* The "Now Loading" screen (class name inferred). fn_801E265C keeps the game's one. */
class NowLoadingScreen {
  public:
    NowLoadingScreen();

    /* Loads the project of the current language; returns whether it loaded. */
    bool Load();
    /* Advances and draws the screen; returns whether it is loaded. */
    bool Update();

    /* 0x00 */ bool m_loaded;
    /* 0x04 */ float m_time; /* the time of the last update, in seconds */
    /* 0x08 */ Chao::CSD::CProject m_project;
};
SIZE_ASSERT(NowLoadingScreen, 0x54);

/* Draws the loading screen with its own camera. */
class CNowLoadingThread : public Task {
  public:
    CNowLoadingThread(RwRaster *raster, int stage);
    virtual ~CNowLoadingThread();
    virtual void Tick(float dt);
    /* Draws a frame when one is due; always returns 1. */
    virtual int Run();

    /* Ends the task. */
    void End();

    /* 0x28 */ RwCamera *m_camera;
    /* 0x2C */ s32 m_stage; /* the stage whose title is shown, or -1 */
    /* 0x30 */ s32 field_0x30;
    /* 0x34 */ s32 m_stopped; /* the title is stopped rather than shown when the task ends */
    /* 0x38 */ u64 m_lastTime; /* of the last frame, in microseconds */
};
SIZE_ASSERT(CNowLoadingThread, 0x40);

/* Starts and stops the loading screen as the stages ask. */
class NowLoading : public Task {
  public:
    NowLoading(Task *parent);
    virtual ~NowLoading();
    virtual void Tick(float dt);

    static NowLoading *s_instance;

    /* 0x28 */ CNowLoadingThread *m_thread;
    /* 0x2C */ u32 m_request; /* 1 start, 2 stop */
    /* 0x30 */ s32 m_stage;   /* the stage requested */
    /* 0x34 */ s32 m_current; /* the stage shown */
};
SIZE_ASSERT(NowLoading, 0x38);

extern "C" {
/* Whether the loading screen runs, stopping it, and starting it for stage (or -1). */
int fn_802BEB68(void);
void fn_802BEB90(void);
void fn_802BEBE0(s32 stage);
}

#endif
