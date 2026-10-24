#ifndef GAME_CAMERARENDER_H
#define GAME_CAMERARENDER_H

#include "Game/HandleUser.h"
#include "Game/Task.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* The tasks that bracket the drawing of a frame (class names from RTTI; Game/CameraRender.cpp):
 * RenderStart clears the screen's camera, CameraRender draws an area of the stage through it,
 * CameraRenderClose ends the camera's update, RenderStop shows the frame, and
 * CameraMatrixChangeFor2D switches the camera to its 2D placement and back. Member names are
 * inferred. */

/* CameraRender (vtable 0x8053C47C; 0x98 bytes): draws an area of the stage through the screen's
 * camera. */
class CameraRender : public Task, public HandleUser {
  public:
    CameraRender(Task *parent, int area);
    virtual void Tick(float dt);
    /* Draws the area. */
    void Render();

    static const char *s_taskName;

    /* 0x30 */ int m_area;
    /* 0x34 */ u8 field_0x34[0x98 - 0x34];
};
SIZE_ASSERT(CameraRender, 0x98);

/* CameraRenderClose (vtable 0x8053C448): ends the screen camera's update. */
class CameraRenderClose : public Task, public HandleUser {
  public:
    CameraRenderClose(Task *parent);
    virtual void Tick(float dt);

    static const char *s_taskName;
};
SIZE_ASSERT(CameraRenderClose, 0x30);

/* RenderStart (vtable 0x8053C424): clears the screen's camera and starts the frame timer. */
class RenderStart : public Task {
  public:
    RenderStart(Task *parent);
    virtual void Tick(float dt);

    static const char *s_taskName;
};
SIZE_ASSERT(RenderStart, 0x28);

/* RenderStop (vtable 0x8053C408): stops the frame timer and shows the frame. */
class RenderStop : public Task {
  public:
    RenderStop(Task *parent);
    virtual void Tick(float dt);

    static const char *s_taskName;
};
SIZE_ASSERT(RenderStop, 0x28);

/* CameraMatrixChangeFor2D (vtable 0x8053C3EC): puts the screen's camera in its 2D placement, or
 * back in the camera unit's placement. */
class CameraMatrixChangeFor2D : public Task {
  public:
    CameraMatrixChangeFor2D(Task *parent, bool set2D);
    virtual ~CameraMatrixChangeFor2D();
    virtual void Tick(float dt);

    /* 0x28 */ bool m_set2D;
    /* 0x2C */ u32 field_0x2C;
};
SIZE_ASSERT(CameraMatrixChangeFor2D, 0x30);

#endif
