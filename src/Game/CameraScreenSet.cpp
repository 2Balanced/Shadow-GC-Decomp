/* The game's screen (UnkScreenHolder_8000DCC0, class name unknown; see Game/ScreenHolder.h): the
 * RenderWare camera everything is drawn with, its rasters, the CameraScreen of each viewport and
 * the screen's sizes. The file name and the names of the members are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first).
 *
 * Not matching yet: GetViewport (0x80271D8C) loads m_layout before saving the link register in
 * retail; GC/2.7 saves it first under every form tried (3.0a3 loads first but tail-calls). */
#define MSL_VECTOR_MIDDLE
#define MSL_VECTOR_AT
#define MSL_VECTOR_DESTROY_ALL_LATE
#include "Game/ScreenHolder.h"

using SonicteamUSA::System::Camera::CameraScreen;

/* A point or a size on the screen, in pixels (see Game/ScreenViewport.cpp). */
struct ScreenPoint {
    ScreenPoint(s32 x, s32 y) : x(x), y(y) {}
    ScreenPoint(const ScreenPoint &point) : x(point.x), y(point.y) {}

    s32 x;
    s32 y;
};

/* RenderWare's matrix (RwMatrix): the right, up and at axes and the position, each padded to 0x10
 * bytes. */
struct UnkRwV3d_80271BA8 {
    float x;
    float y;
    float z;
};

struct UnkRwMatrix_80271BA8 {
    /* 0x00 */ UnkRwV3d_80271BA8 right;
    /* 0x0C */ u32 flags;
    /* 0x10 */ UnkRwV3d_80271BA8 up;
    /* 0x1C */ u32 pad1;
    /* 0x20 */ UnkRwV3d_80271BA8 at;
    /* 0x2C */ u32 pad2;
    /* 0x30 */ UnkRwV3d_80271BA8 pos;
    /* 0x3C */ u32 pad3;
};

/* The camera manager (class name unknown; see Game/CameraManagerUnit.h), and one of its cameras
 * (class name unknown). */
class UnkCameraManager_80009548;
class UnkCamera_80009370;

/* The view window's aspect (the camera code reads it too). */
float lbl_805EA980 = 1.3195877f;

static ScreenPoint s_screenSize0(640, 480);
/* The view window of the full screen. */
static Vec2f s_viewWindow(0.5f * lbl_805EA980, 0.5f);
/* The screen size (Game/ScreenViewport.cpp reads it). */
extern "C" {
ScreenPoint lbl_805F0160(640, 480);
}
static ScreenPoint s_screenSize2(s_screenSize0);
/* The player of the current viewport, and the current viewport (names inferred). */
s32 lbl_805F0170;
s32 lbl_805F0174;

/* The initial view window (see the camera code). */
extern Vec2f lbl_805EDFA8;

extern "C" {
/* RenderWare (by their arguments): RwCameraCreate, RwFrameCreate, RwRasterCreate,
 * RwCameraSetViewWindow, RwCameraSetFarClipPlane, RwCameraSetNearClipPlane, RwMatrixUpdate,
 * RwFrameSetIdentity, RwFrameTransform and RwFrameGetLTM. */
UnkScreen_8000DCC0 *fn_80485A58(void);
struct RwFrame *fn_80486D00(void);
RwRaster *fn_80489E74(s32 width, s32 height, s32 depth, s32 flags);
UnkScreen_8000DCC0 *fn_80485950(UnkScreen_8000DCC0 *camera, const Vec2f *window);
UnkScreen_8000DCC0 *fn_8048560C(UnkScreen_8000DCC0 *camera, float farPlane);
UnkScreen_8000DCC0 *fn_80485550(UnkScreen_8000DCC0 *camera, float nearPlane);
UnkRwMatrix_80271BA8 *fn_8047EBA0(UnkRwMatrix_80271BA8 *matrix);
struct RwFrame *fn_80487820(struct RwFrame *frame);
struct RwFrame *fn_804876E8(struct RwFrame *frame, const UnkRwMatrix_80271BA8 *matrix, int combine);
UnkRwMatrix_80271BA8 *RwFrameGetLTM(struct RwFrame *frame);
/* Whether two floats are about equal (behaviour inferred). */
bool fn_801C455C(float a, float b);
/* a - b for two-float vectors (behaviour inferred). */
void fn_801C2800(Vec2f *out, const Vec2f *a, const Vec2f *b);
/* The camera manager, a player's camera placement and camera, and a camera's view window
 * (behaviour inferred). */
UnkCameraManager_80009548 *fn_80009548(void);
const Mtx34f *fn_800D4E3C(UnkCameraManager_80009548 *manager, int player);
UnkCamera_80009370 *fn_80009370(UnkCameraManager_80009548 *manager, int player);
const Vec2f *fn_801C2508(UnkCamera_80009370 *camera);
/* The view window of a size (behaviour inferred). */
void fn_8000F860(Vec2f *out, const Vec2f *size);
/* The viewports (see Game/ScreenViewport.cpp). */
s32 fn_80272EC0(s32 layout, s32 player);
s32 fn_80272EAC(s32 viewport);
void fn_80272DB0(Vec2f *scale, s32 viewport);
}

UnkScreenHolder_8000DCC0::UnkScreenHolder_8000DCC0()
    : m_camera(0), m_raster(0), m_parentRaster(0), m_zRaster(0), m_ownRaster(0), m_layout(1),
      m_viewWindow(lbl_805EDFA8) {
    Create();
}

UnkScreenHolder_8000DCC0::~UnkScreenHolder_8000DCC0() {
    Destroy();
    m_screens.clear();
}

void UnkScreenHolder_8000DCC0::CreateScreens() {
    m_screens.clear();
    for (int i = 0; i < 5; i++) {
        m_screens.push_back(boost::shared_ptr<CameraScreen>(new CameraScreen(i)));
    }
}

int UnkScreenHolder_8000DCC0::GetViewport(int player) {
    return fn_80272EC0(m_layout, player);
}

void UnkScreenHolder_8000DCC0::SetLayout(int layout) {
    m_layout = layout;
}

void UnkScreenHolder_8000DCC0::SetViewport(int viewport) {
    if (viewport != -1 && viewport != lbl_805F0174) {
        m_screens.at(viewport)->Apply();
        lbl_805F0174 = viewport;
        lbl_805F0170 = fn_80272EAC(viewport);
    }
}

void UnkScreenHolder_8000DCC0::PlaceCameraFor(int player) {
    UnkCameraManager_80009548 *manager = fn_80009548();
    const Mtx34f *placement = fn_800D4E3C(manager, player);
    UnkRwMatrix_80271BA8 matrix;
    matrix.right.x = placement->values[0][0];
    matrix.right.y = placement->values[1][0];
    matrix.right.z = placement->values[2][0];
    matrix.up.x = placement->values[0][1];
    matrix.up.y = placement->values[1][1];
    matrix.up.z = placement->values[2][1];
    matrix.at.x = placement->values[0][2];
    matrix.at.y = placement->values[1][2];
    matrix.at.z = placement->values[2][2];
    matrix.pos.x = placement->values[0][3];
    matrix.pos.y = placement->values[1][3];
    matrix.pos.z = placement->values[2][3];
    matrix.flags = matrix.pad1 = matrix.pad2 = matrix.pad3 = 0;
    fn_8047EBA0(&matrix);
    struct RwFrame *frame = m_camera->m_frame;
    fn_80487820(frame);
    fn_804876E8(frame, &matrix, 2);
    UnkCamera_80009370 *camera = fn_80009370(manager, player);
    Vec2f scale;
    fn_80272DB0(&scale, fn_80272EC0(m_layout, player));
    Vec2f window(*fn_801C2508(camera));
    window.x *= scale.x;
    window.y *= scale.y;
    if (!fn_801C455C(m_viewWindow.x, window.x) || !fn_801C455C(m_viewWindow.y, window.y)) {
        Vec2f view;
        fn_8000F860(&view, &window);
        fn_80485950(m_camera, &view);
        m_viewWindow.x = window.x;
        m_viewWindow.y = window.y;
    }
}

/* Whether two view windows are about equal. */
static bool SameWindow(const Vec2f *a, const Vec2f *b) {
    Vec2f d;
    fn_801C2800(&d, a, b);
    return d.x * d.x + d.y * d.y < 0.0001f;
}

void UnkScreenHolder_8000DCC0::ReadCamera() {
    SetViewport(0);
    struct RwFrame *frame = m_camera->m_frame;
    UnkRwMatrix_80271BA8 *ltm = RwFrameGetLTM(frame);
    m_placement.values[0][0] = ltm->right.x;
    m_placement.values[0][1] = ltm->up.x;
    m_placement.values[0][2] = ltm->at.x;
    m_placement.values[0][3] = ltm->pos.x;
    m_placement.values[1][0] = ltm->right.y;
    m_placement.values[1][1] = ltm->up.y;
    m_placement.values[1][2] = ltm->at.y;
    m_placement.values[1][3] = ltm->pos.y;
    m_placement.values[2][0] = ltm->right.z;
    m_placement.values[2][1] = ltm->up.z;
    m_placement.values[2][2] = ltm->at.z;
    m_placement.values[2][3] = ltm->pos.z;
    fn_80487820(frame);
    m_cameraWindow = m_camera->m_viewWindow;
    if (!SameWindow(&s_viewWindow, &m_cameraWindow)) {
        Vec2f window(s_viewWindow);
        fn_80485950(m_camera, &window);
    }
}

void UnkScreenHolder_8000DCC0::PlaceCamera() {
    UnkRwMatrix_80271BA8 matrix;
    matrix.right.x = m_placement.values[0][0];
    matrix.right.y = m_placement.values[1][0];
    matrix.right.z = m_placement.values[2][0];
    matrix.up.x = m_placement.values[0][1];
    matrix.up.y = m_placement.values[1][1];
    matrix.up.z = m_placement.values[2][1];
    matrix.at.x = m_placement.values[0][2];
    matrix.at.y = m_placement.values[1][2];
    matrix.at.z = m_placement.values[2][2];
    matrix.pos.x = m_placement.values[0][3];
    matrix.pos.y = m_placement.values[1][3];
    matrix.pos.z = m_placement.values[2][3];
    matrix.flags = matrix.pad1 = matrix.pad2 = matrix.pad3 = 0;
    fn_8047EBA0(&matrix);
    struct RwFrame *frame = m_camera->m_frame;
    fn_80487820(frame);
    fn_804876E8(frame, &matrix, 2);
    UnkScreen_8000DCC0 *camera = m_camera;
    if (!fn_801C455C(m_cameraWindow.x, camera->m_viewWindow.x) ||
        !fn_801C455C(m_cameraWindow.y, camera->m_viewWindow.y)) {
        Vec2f window = m_cameraWindow;
        fn_80485950(m_camera, &window);
    }
}

void UnkScreenHolder_8000DCC0::Create() {
    if (m_camera == 0) {
        m_camera = fn_80485A58();
        if (m_camera != 0) {
            fn_8048D7C0(m_camera, fn_80486D00());
            m_ownRaster = fn_80489E74(lbl_805F0160.x, lbl_805F0160.y, 0, 2);
            m_zRaster = fn_80489E74(lbl_805F0160.x, lbl_805F0160.y, 0, 1);
            m_raster = m_parentRaster = m_ownRaster;
            m_camera->m_raster = m_raster;
            m_camera->m_zRaster = m_zRaster;
            if (m_camera->m_frame != 0 && m_camera->m_raster != 0 && m_camera->m_zRaster != 0) {
                Vec2f window(s_viewWindow);
                fn_80485950(m_camera, &window);
                fn_8048560C(m_camera, 30000.0f);
                fn_80485550(m_camera, 1.0f);
            } else {
                Destroy();
            }
        }
    }
}

#include <vector_impl>
