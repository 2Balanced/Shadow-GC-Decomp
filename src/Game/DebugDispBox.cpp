/* SonicteamUSA::System::DebugDispBox (class name from RTTI): a box queued for the debug view (see
 * Game/DebugDisp.cpp), drawn with cube_model.dff scaled to twice its size. File name from the
 * class; method names are inferred. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#include "Game/DebugDisp.h"
#include "Game/DebugDispShape.h"
#include "Game/MathTypes.h"
#include <dolphin/types.h>

using SonicteamUSA::System::DebugDispBox;

DebugDispBox::DebugDispBox(const Vec3f &center, const Vec3f &size, const Vec3f &rotation,
                           const UnkColor_8040D2D8 &color, int axis)
    : DebugDispBase(color), m_box(center, size, rotation, 0) {
    m_axis = axis;
}

void DebugDispBox::Draw() {
    Render(fn_800169F8(&m_box.m_center), fn_800169F8(&m_box.m_size), m_box.m_rotation, m_color,
           m_axis);
}

void DebugDispBox::Render(const Vec3f &center, const Vec3f &size, const Vec3f &rotation,
                          const UnkColor_8040D2D8 &color, int axis) {
    RpClump *clump;
    Mtx34f matrix;
    Mtx34f translation;

    fn_8040B034(&translation, &center);
    fn_8040C148(&matrix, &translation);
    fn_8001E070(&matrix, &rotation, axis);
    fn_8040B720(&matrix, 2.0f * size.x, 2.0f * size.y, 2.0f * size.z);
    RwFrame *frame = (clump = lbl_805F06E0)->frame;
    frame->modelling = fn_800591F0(&matrix);
    fn_8048722C(frame);
    fn_80037B84(clump, ToRwRGBA(color));
    fn_80456BC4(clump);
}

DebugDispBox::~DebugDispBox() {}
