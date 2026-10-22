/* SonicteamUSA::System::DebugDispCapsule (class name from RTTI): a capsule queued for the debug
 * view (see Game/DebugDisp.cpp). It is drawn with capsule_model.dff, whose frame holds the two
 * caps and the body as child frames: the caps are moved to the ends and every part is scaled to the
 * radius. File name from the class; method names are inferred. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#include "Game/DebugDisp.h"
#include "Game/DebugDispShape.h"
#include "Game/MathTypes.h"
#include <dolphin/types.h>

using SonicteamUSA::System::DebugDispCapsule;

/* Not original: data that only unreferenced objects of the original placed here (four rows of
 * floats and the class's short name). */
static const float s_unused[4][4] = {
    {1.0f, 0.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
};
static const char s_name[] = "DebugDispCapsule";

DebugDispCapsule::DebugDispCapsule(const Vec3f &center, const Vec3f &rotation,
                                   const UnkColor_8040D2D8 &color, int axis, float halfHeight,
                                   float radius)
    : DebugDispBase(color), m_capsule(center, halfHeight, radius, rotation, 0) {
    m_axis = axis;
}

void DebugDispCapsule::Draw() {
    Render(fn_800169F8(&m_capsule.m_center), m_capsule.m_halfHeight, m_capsule.m_radius,
           m_capsule.m_rotation, m_color, m_axis);
}

void DebugDispCapsule::Render(const Vec3f &center, float halfHeight, float radius,
                              const Vec3f &rotation, const UnkColor_8040D2D8 &color, int axis) {
    RpClump *clump;
    Mtx34f matrix;
    Mtx34f translation;

    fn_8040B034(&translation, &center);
    fn_8040C148(&matrix, &translation);
    fn_8001E070(&matrix, &rotation, axis);
    RwFrame *frame = (clump = lbl_805F06E8)->frame;
    frame->modelling = fn_800591F0(&matrix);
    fn_8048722C(frame);
    fn_80037B84(clump, ToRwRGBA(color));
    RwFrame *part = frame->child->child;
    RwV3d bodyScale = {2.0f * radius, 2.0f * radius, 2.0f * halfHeight};
    RwV3d capScale = {2.0f * radius, 2.0f * radius, 2.0f * radius};
    RwV3d bottom = {0.0f, 0.0f, -halfHeight};
    RwV3d top = {0.0f, 0.0f, halfHeight};
    fn_804875B0(part, &bottom, 0);
    fn_8048764C(part, &capScale, 1);
    part = part->next;
    fn_804875B0(part, &top, 0);
    fn_8048764C(part, &capScale, 1);
    fn_8048764C(part->next, &bodyScale, 0);
    fn_80456BC4(clump);
}

DebugDispCapsule::~DebugDispCapsule() {}
