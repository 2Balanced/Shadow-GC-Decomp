/* The screen layouts of the camera system (no RTTI; the file and every name here are inferred).
 * A table of five viewports describes the full screen and the two halves of a screen split side
 * by side or one above the other: the viewport index of each player of each layout, its rectangles
 * in two screen sizes and its scale. A CameraScreen (Game/CameraScreen.h) gives a viewport its
 * own camera and z rasters (sub-rasters of the screen's) and installs them as the screen's current
 * ones. The unit before this one (Game/CameraScreenSet.cpp) uses all of it. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include "Game/CameraScreen.h"
#include "Game/MathTypes.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* A RenderWare raster (opaque here). */
struct RwRaster;

/* A RenderWare rectangle. */
struct RwRect {
    s32 x;
    s32 y;
    s32 w;
    s32 h;
};

/* The screen (see WatchCamera.cpp): its current camera and z rasters at 0x60 and 0x64. */
struct UnkScreen_8000DCC0 {
    /* 0x00 */ u8 field_0x0[0x60];
    /* 0x60 */ RwRaster *m_raster;
    /* 0x64 */ RwRaster *m_zRaster;
};

/* What fn_8000DCC0 returns (see WatchCamera.cpp): the screen and its rasters (meanings of 0x4 to
 * 0xC inferred from their use here). */
struct UnkScreenHolder_8000DCC0 {
    /* 0x0 */ UnkScreen_8000DCC0 *m_screen;
    /* 0x4 */ RwRaster *m_raster;
    /* 0x8 */ RwRaster *m_parentRaster;
    /* 0xC */ RwRaster *m_zRaster;
};

extern "C" {
UnkScreenHolder_8000DCC0 *fn_8000DCC0(void);
/* RenderWare raster creation, destruction and sub-rasters (RwRasterCreate, RwRasterDestroy and
 * RwRasterSubRaster by their arguments). */
RwRaster *fn_80489E74(s32 width, s32 height, s32 depth, s32 flags);
s32 fn_80489BDC(RwRaster *raster);
RwRaster *fn_80489DC4(RwRaster *raster, RwRaster *parent, RwRect *rect);
/* vector * scale (behaviour inferred from its body). */
Vec2f fn_80033934(const Vec2f *vector, float scale);
}

/* A point or a size on the screen, in pixels. */
struct ScreenPoint {
    ScreenPoint(s32 x, s32 y) : x(x), y(y) {}
    ScreenPoint(const ScreenPoint &point) : x(point.x), y(point.y) {}

    s32 x;
    s32 y;
};

/* A rectangle on the screen. Its constructor is out of line (0x80045EFC); its copy constructor
 * calls the inline one of ScreenPoint, so it is not inlined but emitted here. */
struct ScreenRect {
    ScreenRect(ScreenPoint position, ScreenPoint size);
    ScreenRect(const ScreenRect &rect) : position(rect.position), size(rect.size) {}

    ScreenPoint position;
    ScreenPoint size;
};

/* One viewport of a screen layout. */
struct ScreenViewport {
    ScreenViewport(s32 player, s32 layout, ScreenRect rect, ScreenRect rect2, const Vec2f &scale)
        : player(player), layout(layout), rect(rect), rect2(rect2), scale(scale) {}
    ~ScreenViewport() {}

    /* 0x00 */ s32 player; /* the player's index within the layout */
    /* 0x04 */ s32 layout; /* 0 full screen, 1 split side by side, 2 split one above the other */
    /* 0x08 */ ScreenRect rect;  /* in the screen size s_screenSize */
    /* 0x18 */ ScreenRect rect2; /* in the screen size s_screenSize2 */
    /* 0x28 */ Vec2f scale;
};
SIZE_ASSERT(ScreenViewport, 0x30);

/* The screen size, set up by the camera unit's static initializer. */
extern "C" ScreenPoint lbl_805F0160;

static ScreenPoint s_screenSize = lbl_805F0160;
static ScreenPoint s_screenSize2 = lbl_805F0160;
/* The aspect correction of the viewports' scale. */
static Vec2f s_aspect(1.0f, 1.0714285f);

static ScreenViewport s_viewports[] = {
    ScreenViewport(0, 0, ScreenRect(ScreenPoint(0, 0), s_screenSize),
                   ScreenRect(ScreenPoint(0, 0), s_screenSize2), Vec2f(1.0f, 1.0f)),
    ScreenViewport(0, 1, ScreenRect(ScreenPoint(0, 0), ScreenPoint(s_screenSize.x / 2, s_screenSize.y)),
                   ScreenRect(ScreenPoint(0, 0), ScreenPoint(s_screenSize2.x / 2, s_screenSize2.y)),
                   Vec2f(0.5f, 1.0f)),
    ScreenViewport(1, 1,
                   ScreenRect(ScreenPoint(s_screenSize.x / 2, 0), ScreenPoint(s_screenSize.x / 2, s_screenSize.y)),
                   ScreenRect(ScreenPoint(s_screenSize2.x / 2, 0),
                              ScreenPoint(s_screenSize2.x / 2, s_screenSize2.y)),
                   Vec2f(0.5f, 1.0f)),
    ScreenViewport(0, 2, ScreenRect(ScreenPoint(0, 0), ScreenPoint(s_screenSize.x, s_screenSize.y / 2)),
                   ScreenRect(ScreenPoint(0, 0), ScreenPoint(s_screenSize2.x, s_screenSize2.y / 2)),
                   Vec2f(1.0f, 0.5f)),
    ScreenViewport(1, 2,
                   ScreenRect(ScreenPoint(0, s_screenSize.y / 2), ScreenPoint(s_screenSize.x, s_screenSize.y / 2)),
                   ScreenRect(ScreenPoint(0, s_screenSize2.y / 2),
                              ScreenPoint(s_screenSize2.x, s_screenSize2.y / 2)),
                   Vec2f(1.0f, 0.5f)),
};

/* The viewport index of each player of each layout (-1: none). */
static const s32 s_layout0[] = {0, -1};
static const s32 s_layout1[] = {1, 2};
static const s32 s_layout2[] = {3, 4};
static const s32 *s_layouts[] = {s_layout0, s_layout1, s_layout2};

using SonicteamUSA::System::Camera::CameraScreen;

extern "C" s32 fn_80272EC0(s32 layout, s32 player) {
    return s_layouts[layout][player];
}

extern "C" s32 fn_80272EAC(s32 viewport) {
    return s_viewports[viewport].player;
}

extern "C" const ScreenRect *fn_80272E94(s32 viewport) {
    return &s_viewports[viewport].rect;
}

extern "C" const ScreenRect *fn_80272E7C(s32 viewport) {
    return &s_viewports[viewport].rect2;
}

/* A two-dimensional vector (EggLastMechCamera.cpp calls this too). */
extern "C" Vec2f fn_80272E70(float x, float y) {
    return Vec2f(x, y);
}

/* The scale of a viewport (-1: none). The original probably returns the vector by value: the
 * game's compiler builds it in the caller's result in place, which GC/2.7 does not do for a named
 * local, so it is written with the result pointer explicit. */
extern "C" void fn_80272DB0(Vec2f *scale, s32 viewport) {
    if (viewport != -1) {
        *scale = s_viewports[viewport].scale;
        scale->x *= s_aspect.x;
        scale->y *= s_aspect.y;
        if (viewport != 0) {
            *scale = fn_80033934(scale, 1.25f);
        }
    } else {
        *scale = fn_80272E70(1.0f, 1.0f);
    }
}

/* A viewport's rectangle as a RenderWare one. */
inline RwRect GetRwRect(s32 viewport) {
    return *reinterpret_cast<const RwRect *>(fn_80272E94(viewport));
}

CameraScreen::CameraScreen(s32 viewport) : m_viewport(viewport), m_raster(0), m_zRaster(0) {
    if (m_viewport != 0) {
        m_raster = fn_80489E74(0, 0, 0, 2);
        m_zRaster = fn_80489E74(0, 0, 0, 1);
        RwRect rect = GetRwRect(m_viewport);
        fn_80489DC4(m_raster, fn_8000DCC0()->m_parentRaster, &rect);
        fn_80489DC4(m_zRaster, fn_8000DCC0()->m_zRaster, &rect);
    } else {
        m_raster = fn_8000DCC0()->m_raster;
        m_zRaster = fn_8000DCC0()->m_zRaster;
    }
}

CameraScreen::~CameraScreen() {
    if (m_raster != 0) {
        if (m_viewport != 0) {
            fn_80489BDC(m_raster);
            fn_80489BDC(m_zRaster);
        }
        m_raster = 0;
        m_zRaster = 0;
    }
}

void CameraScreen::Apply() {
    UnkScreenHolder_8000DCC0 *holder = fn_8000DCC0();
    holder->m_screen->m_raster = m_raster;
    holder->m_screen->m_zRaster = m_zRaster;
}
