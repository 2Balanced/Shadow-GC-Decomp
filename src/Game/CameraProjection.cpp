/* The camera's projection onto the screen (no class with RTTI; file name and all names
 * inferred): the view window of a field of view (fn_8000F41C, fn_8000F2E8), a point's position on
 * a viewport (fn_8000F57C, fn_8000F654, fn_8000F4A0), with the game's first copies of the float
 * tangent, arc sine and cosine. The unit is split by its .sdata2 (0x805F1788..0x805F17A8): the
 * 0.5f at 0x805F17A8 repeats one of its constants, so fn_8000F860 after it is another unit. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#define MSL_LIST_ACCESSORS
#include "Game/CameraManagerUnit.h"
#include "Game/CameraState.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/ScreenHolder.h"
#include "boost/shared_ptr.hpp"

using SonicteamUSA::System::Camera::CameraManagerUnit;

typedef UnkCameraState_8000AA70 CameraState;

/* The camera manager (Game/CameraManager.cpp). */
class UnkCameraManager_80009548;

/* A rectangle on the screen (as in Game/ScreenViewport.cpp). */
struct UnkScreenRect_8000F4A0 {
    /* 0x0 */ s32 m_x;
    /* 0x4 */ s32 m_y;
    /* 0x8 */ s32 m_width;
    /* 0xC */ s32 m_height;
};

extern "C" {
UnkCameraManager_80009548 *fn_80009548(void);
boost::shared_ptr<CameraManagerUnit> fn_8000D0F0(UnkCameraManager_80009548 *manager, int index);
UnkScreenHolder_8000DCC0 *fn_8000DCC0(void);
/* A viewport's rectangle and scale (Game/ScreenViewport.cpp). */
const UnkScreenRect_8000F4A0 *fn_80272E94(s32 viewport);
void fn_80272DB0(Vec2f *scale, s32 viewport);
/* A placement's view matrix (Game/CameraState.cpp). */
Mtx34f fn_8000A67C(const CameraState *state);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
/* tan, asin and cos of doubles. */
double fn_803B1B84(double x);
double fn_803B11C0(double x);
double cos(double x);
}

/* The screen's aspect ratio (Game/CameraScreenSet.cpp). */
extern float lbl_805EA980;

inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* An empty inline function: calling it keeps the caller out of line (deferred inlining is one
 * level deep). */
inline void Nothing() {}

namespace std {
inline float cosf_(float x) {
    return (float)::cos(x);
}

inline float cos(float x) {
    return cosf_(x);
}
} // namespace std

extern "C" inline float fn_8000F3D4(float x) {
    Nothing();
    return (float)fn_803B1B84(x);
}

extern "C" inline float fn_8000F3F8(float x) {
    Nothing();
    return (float)fn_803B11C0(x);
}

/* A rectangle's position. */
extern "C" s32 fn_8000F858(const UnkScreenRect_8000F4A0 *rect) {
    return rect->m_y;
}

extern "C" s32 fn_8000F850(const UnkScreenRect_8000F4A0 *rect) {
    return rect->m_x;
}

/* A screen's camera placement and view matrix. */
inline const Mtx34f *ViewMatrix(UnkCameraManager_80009548 *manager, int index) {
    CameraManagerUnit *unit = fn_8000D0F0(manager, index).get();
    return &unit->m_viewMtx.value;
}

inline const Vec2f *ViewWindow(UnkCameraManager_80009548 *manager, int index) {
    return reinterpret_cast<const Vec2f *>(fn_8000D0F0(manager, index)->m_state.field_0x1C);
}

extern "C" void fn_8000F4A0(s32 *out, const Vec2f *point, s32 viewport);

/* The position of point on the screen of player index, in pixels, with its depth (returned by
 * value through out, which is copy-constructed out of line). */
extern "C" void fn_8000F654(Vec3f *out, const Vec3f *point, int index) {
    UnkCameraManager_80009548 *manager = fn_80009548();
    UnkScreenHolder_8000DCC0 *screen = fn_8000DCC0();
    const Mtx34f *view = ViewMatrix(manager, index);
    const Vec2f *window = ViewWindow(manager, index);
    s32 viewport = screen->GetViewport(index);
    Vec2f scale;
    fn_80272DB0(&scale, viewport);
    Vec3f local;
    fn_800091EC(&local, &(const Vec3f &)fn_8040B67C(view, point));
    Vec2f position;
    position.x = local.x / -(local.z * fn_8000F3D4(0.5f * window->x)) / scale.x;
    position.y = local.y / (local.z * fn_8000F3D4(0.5f * window->y)) / scale.y;
    s32 pixel[2];
    fn_8000F4A0(pixel, &position, viewport);
    local.x = pixel[0];
    local.y = pixel[1];
    local.z = -local.z;
    fn_800091EC(out, &local);
}

/* The position of point on viewport, from -1 to 1, with its depth. */
extern "C" void fn_8000F57C(Vec3f *out, const CameraState *state, const Vec3f *point, s32 viewport) {
    UnkCameraMtx_8040C148 view(fn_8000A67C(state));
    Vec2f scale;
    fn_80272DB0(&scale, viewport);
    Vec3f local;
    fn_800091EC(&local, &(const Vec3f &)fn_8040B67C(&view.value, point));
    const Vec2f *window = reinterpret_cast<const Vec2f *>(state->field_0x1C);
    local.x = local.x / -(local.z * fn_8000F3D4(0.5f * window->x)) / scale.x;
    local.y = local.y / (local.z * fn_8000F3D4(0.5f * window->y)) / scale.y;
    fn_800091EC(out, &local);
}

/* A point from -1 to 1 on viewport in pixels. */
extern "C" void fn_8000F4A0(s32 *out, const Vec2f *point, s32 viewport) {
    s32 x;
    const UnkScreenRect_8000F4A0 *rect = fn_80272E94(viewport);
    x = fn_8000F850(rect) + (s32)(0.5f * (1.0f + point->x) * rect->m_width);
    s32 y = fn_8000F858(rect) + (s32)(0.5f * (1.0f + point->y) * rect->m_height);
    out[0] = x;
    out[1] = y;
}

/* The view window of a field of view (limited below 180 degrees) on the screen. */
extern "C" void fn_8000F41C(Vec2f *out, float fov) {
    if (fov > DegToRad(180.0f)) {
        fov = DegToRad(179.0f);
    }
    float y = 2.0f * fn_8000F3F8(fn_8000F3D4(0.5f * fov) / lbl_805EA980);
    out->x = fov;
    out->y = y;
}

/* The same for a field of view across the screen's diagonal (behaviour inferred). */
extern "C" void fn_8000F2E8(Vec2f *out, float fov) {
    float diagonal = fn_8000F3F8(1.0f / lbl_805EA980);
    if (fov > DegToRad(180.0f)) {
        fov = DegToRad(179.0f);
    }
    float half = fn_8000F3D4(0.5f * fov);
    float across = half * std::cos(diagonal);
    float up = across / lbl_805EA980;
    float x = 2.0f * fn_8000F3F8(across);
    float y = 2.0f * fn_8000F3F8(up);
    out->x = x;
    out->y = y;
}
