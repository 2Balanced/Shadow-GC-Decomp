#ifndef GAME_CAMERASCREEN_H
#define GAME_CAMERASCREEN_H

#include <dolphin/types.h>
#include "size_assert.h"

/* A RenderWare raster (opaque here). */
struct RwRaster;

namespace SonicteamUSA {
namespace System {
namespace Camera {

/* CameraScreen (class name from the RTTI of its shared_ptr control block,
 * sp_counted_base_impl<CameraScreen *, checked_deleter<CameraScreen>>): the camera and z rasters
 * of one screen viewport (see Game/ScreenViewport.cpp, which defines its members). Member names
 * are inferred. */
class CameraScreen {
  public:
    CameraScreen(s32 viewport);
    ~CameraScreen();
    /* Makes them the screen's current rasters. */
    void Apply();

    /* 0x0 */ s32 m_viewport;
    /* 0x4 */ RwRaster *m_raster; /* the screen's own when m_viewport is 0 */
    /* 0x8 */ RwRaster *m_zRaster;
};
SIZE_ASSERT(CameraScreen, 0xC);

} // namespace Camera
} // namespace System
} // namespace SonicteamUSA

#endif
