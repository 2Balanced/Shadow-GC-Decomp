/* SonicteamUSA::System::DebugDispLine (class name from RTTI): a line queued for the debug view
 * (see Game/DebugDisp.cpp). A line of width 0 is drawn as a plain line; a wider one with
 * cylinder_model.dff, placed at the line's midpoint, turned along it and scaled to its width and
 * length. File name from the class; method names are inferred. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#include "Game/DebugDisp.h"
#include "Game/DebugDispShape.h"
#include "Game/MathTypes.h"
#include <dolphin/types.h>

using SonicteamUSA::System::DebugDispLine;

extern "C" {
/* Draws a plain line (behaviour inferred). */
void fn_80039920(const UnkDebugLine_80020A20 *line, const UnkColor_8040D2D8 *color);
/* The distance between a and b; a + b; vector * scale (behaviour inferred). */
float fn_8040D1B4(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
/* The two angles that turn the frame's axis along direction (behaviour inferred). */
void fn_8003B9E8(const Vec3f *direction, float *angleA, float *angleB);
/* Turns frame about one of its axes (behaviour inferred). */
void fn_800E9CEC(RwFrame *frame, int combine, float angle);
void fn_800E9C64(RwFrame *frame, int combine, float angle);
}

/* fn_8040CD74 called as returning a - b by value (see Game/ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* The RenderWare vector of v (name inferred). */
inline RwV3d ToRwV3d(const Vec3f &v) {
    RwV3d result = {v.x, v.y, v.z};
    return result;
}

DebugDispLine::DebugDispLine(const UnkDebugLine_80020A20 &line, const UnkColor_8040D2D8 &color,
                             float width)
    : DebugDispBase(color), m_line(line) {
    m_width = width;
}

void DebugDispLine::Draw() {
    Render(m_line, m_color, m_width);
}

void DebugDispLine::Render(const UnkDebugLine_80020A20 &line, const UnkColor_8040D2D8 &color,
                           float width) {
    if (width == 0.0f) {
        fn_80039920(&line, &color);
    } else {
        RpClump *clump;
        RwFrame *frame = (clump = lbl_805F06E4)->frame;
        Vec3f direction;
        RwV3d scale;
        Vec3f middle;

        fn_800091EC(&direction,
                    &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&line.m_end, &line.m_start));
        scale.x = scale.y = width;
        scale.z = fn_8040D1B4(&line.m_start, &line.m_end);
        fn_800091EC(&middle,
                    &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)fn_8040CDD0(&line.m_start, &line.m_end),
                                                0.5f));
        RwV3d position = ToRwV3d(middle);
        fn_804875B0(frame, &position, 0);
        {
            float angleA;
            float angleB;

            fn_8003B9E8(&direction, &angleA, &angleB);
            fn_800E9CEC(frame, 1, angleB);
            fn_800E9C64(frame, 1, angleA);
        }
        fn_8048764C(frame, &scale, 1);
        fn_80037B84(clump, ToRwRGBA(color));
        fn_80456BC4(clump);
    }
}

DebugDispLine::~DebugDispLine() {}
