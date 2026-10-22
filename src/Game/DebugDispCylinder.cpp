/* SonicteamUSA::System::DebugDispCylinder (class name from RTTI): a cylinder queued for the debug
 * view (see Game/DebugDisp.cpp), drawn with cylinder_model.dff scaled to its radius and height.
 * File name from the class; method names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include "Game/DebugDisp.h"
#include "Game/DebugDispShape.h"
#include "Game/MathTypes.h"
#include <dolphin/types.h>

using SonicteamUSA::System::DebugDispCylinder;

DebugDispCylinder::DebugDispCylinder(const Vec3f &center, const Vec3f &rotation,
                                     const UnkColor_8040D2D8 &color, int axis, float halfHeight,
                                     float radius)
    : DebugDispBase(color), m_cylinder(center, halfHeight, radius, rotation, 0) {
    m_axis = axis;
}

void DebugDispCylinder::Draw() {
    Render(fn_800169F8(&m_cylinder.m_center), m_cylinder.m_halfHeight, m_cylinder.m_radius,
           m_cylinder.m_rotation, m_color, m_axis);
}

void DebugDispCylinder::Render(const Vec3f &center, float halfHeight, float radius,
                               const Vec3f &rotation, const UnkColor_8040D2D8 &color, int axis) {
    RpClump *clump;
    Mtx34f matrix;
    Mtx34f translation;

    fn_8040B034(&translation, &center);
    fn_8040C148(&matrix, &translation);
    fn_8001E070(&matrix, &rotation, axis);
    fn_8040B720(&matrix, 2.0f * radius, 2.0f * radius, 2.0f * halfHeight);
    RwFrame *frame = (clump = lbl_805F06E4)->frame;
    frame->modelling = fn_800591F0(&matrix);
    fn_8048722C(frame);
    fn_80037B84(clump, ToRwRGBA(color));
    fn_80456BC4(clump);
}

DebugDispCylinder::~DebugDispCylinder() {}
