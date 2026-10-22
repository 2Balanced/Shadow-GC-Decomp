/* SonicteamUSA::System::DebugDispSphere (class name from RTTI): a sphere queued for the debug view
 * (see Game/DebugDisp.cpp), drawn with sphere_model.dff scaled to its diameter. File name from the
 * class; method names are inferred. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#include "Game/DebugDisp.h"
#include "Game/DebugDispShape.h"
#include "Game/MathTypes.h"
#include <dolphin/types.h>

using SonicteamUSA::System::DebugDispSphere;

DebugDispSphere::DebugDispSphere(const Vec3f &center, const UnkColor_8040D2D8 &color, float radius)
    : DebugDispBase(color), m_sphere(center, radius) {}

void DebugDispSphere::Draw() {
    Render(fn_800169F8(&m_sphere.m_center), m_color, m_sphere.m_radius);
}

void DebugDispSphere::Render(const Vec3f &center, const UnkColor_8040D2D8 &color, float radius) {
    RpClump *clump;
    Mtx34f matrix;
    Mtx34f translation;

    fn_8040B034(&translation, &center);
    fn_8040C148(&matrix, &translation);
    radius *= 2.0f;
    fn_8040B720(&matrix, radius, radius, radius);
    RwFrame *frame = (clump = lbl_805F06DC)->frame;
    frame->modelling = fn_800591F0(&matrix);
    fn_8048722C(frame);
    fn_80037B84(clump, ToRwRGBA(color));
    fn_80456BC4(clump);
}

DebugDispSphere::~DebugDispSphere() {}
