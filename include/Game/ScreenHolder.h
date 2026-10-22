#ifndef GAME_SCREENHOLDER_H
#define GAME_SCREENHOLDER_H

/* Include <vector> with MSL_VECTOR_MIDDLE defined before this header (see Game/CameraScreenSet.cpp,
 * which defines the members). */
#include <vector>

#include "Game/CameraScreen.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The RenderWare camera (RwCamera; the name Game/ScreenViewport.cpp gives it): its frame, its
 * frame and z buffers and its view window. */
struct UnkScreen_8000DCC0 {
    /* 0x00 */ u8 field_0x0[0x4];
    /* 0x04 */ struct RwFrame *m_frame;
    /* 0x08 */ u8 field_0x8[0x60 - 0x8];
    /* 0x60 */ RwRaster *m_raster;
    /* 0x64 */ RwRaster *m_zRaster;
    /* 0x68 */ Vec2f m_viewWindow;
};

extern "C" {
/* RenderWare (by their arguments): RwCameraSetFrame, RwFrameDestroy, RwRasterDestroy and
 * RwCameraDestroy. */
void fn_8048D7C0(UnkScreen_8000DCC0 *camera, struct RwFrame *frame);
s32 fn_80486E2C(struct RwFrame *frame);
s32 fn_80489BDC(RwRaster *raster);
s32 fn_804859F8(UnkScreen_8000DCC0 *camera);
}

/* The game's screen (class name unknown; Game/ScreenViewport.cpp and Game/TFogSet.cpp use reduced
 * views of it under the same name): a function-local static object behind fn_8000DCC0. It owns
 * the RenderWare camera everything is drawn with and its rasters, and the CameraScreen of each of
 * the five viewports (see Game/ScreenViewport.cpp); a camera unit's placement is copied into the
 * RenderWare camera's frame. Member names are inferred. */
class UnkScreenHolder_8000DCC0 {
  public:
    UnkScreenHolder_8000DCC0();
    ~UnkScreenHolder_8000DCC0();

    void Create();
    /* Destroys the camera and its rasters. */
    void Destroy() {
        if (m_camera != 0) {
            struct RwFrame *frame = m_camera->m_frame;
            if (frame != 0) {
                fn_8048D7C0(m_camera, 0);
                fn_80486E2C(frame);
            }
            m_camera->m_raster = 0;
            m_camera->m_zRaster = 0;
            if (m_ownRaster != 0) {
                fn_80489BDC(m_ownRaster);
                m_ownRaster = 0;
            }
            if (m_zRaster != 0) {
                fn_80489BDC(m_zRaster);
                m_zRaster = 0;
            }
            if (m_raster != 0) {
                m_parentRaster = 0;
                m_raster = 0;
            }
            fn_804859F8(m_camera);
            m_camera = 0;
        }
    }
    void PlaceCamera();
    void ReadCamera();
    void PlaceCameraFor(int player);
    void SetViewport(int viewport);
    void SetLayout(int layout);
    int GetViewport(int player);
    void CreateScreens();

    /* 0x00 */ UnkScreen_8000DCC0 *m_camera;
    /* 0x04 */ RwRaster *m_raster;
    /* 0x08 */ RwRaster *m_parentRaster;
    /* 0x0C */ RwRaster *m_zRaster;
    /* 0x10 */ RwRaster *m_ownRaster;
    /* 0x14 */ std::vector<boost::shared_ptr<SonicteamUSA::System::Camera::CameraScreen> > m_screens;
    /* 0x20 */ int m_layout;
    /* 0x24 */ Vec2f m_viewWindow;
    /* 0x2C */ u32 field_0x2C;
    /* 0x30 */ Vec2f m_cameraWindow;
    /* 0x38 */ Mtx34f m_placement;
};
SIZE_ASSERT(UnkScreenHolder_8000DCC0, 0x68);

#endif
