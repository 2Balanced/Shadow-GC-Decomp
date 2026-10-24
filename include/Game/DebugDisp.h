#ifndef GAME_DEBUGDISP_H
#define GAME_DEBUGDISP_H

/* SonicteamUSA::System::DebugDisp and its shapes (class names from RTTI): the debug view's shape
 * queue. The draw helpers in Game/DebugDisp.cpp (declared for the gadgets in Game/DebugDraw.h under
 * their fn_ names) queue a shape object; DebugDisp's render callbacks draw the queued shapes with
 * the models of DebugModels.one. Each shape class is its own unit (Game/DebugDispCapsule.cpp,
 * Game/DebugDispBox.cpp, Game/DebugDispLine.cpp, Game/DebugDispCylinder.cpp,
 * Game/DebugDispSphere.cpp). Member names are inferred. */

#include "Game/BossDef.h"
#include "Game/MathTypes.h"
#include "Game/Task.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* Four colour bytes built by an out-of-line constructor (see Game/ElecBarrier.cpp, which names the
 * bytes r, g, b, a). The debug shapes read byte 0 as the alpha: 0xFF queues a shape as opaque, and
 * the draw code passes bytes 1, 2, 3, 0 to RenderWare as red, green, blue and alpha. The copy
 * constructor is written out member by member (retail copies the bytes one by one). */
struct UnkColor_8040D2D8 {
    /* An unset colour (Game/WeaponLockonBase.cpp's search sets its colour in its constructor's body). */
    UnkColor_8040D2D8() {}
    UnkColor_8040D2D8(u8 r, u8 g, u8 b, u8 a);
    UnkColor_8040D2D8(const UnkColor_8040D2D8 &other) : r(other.r), g(other.g), b(other.b), a(other.a) {}

    /* 0x0 */ u8 r;
    /* 0x1 */ u8 g;
    /* 0x2 */ u8 b;
    /* 0x3 */ u8 a;
};
SIZE_ASSERT(UnkColor_8040D2D8, 0x4);

/* A line segment: start and end (struct name unknown; its copy constructor is out of line at
 * 0x80021DE4). Other units that only pass it by pointer declare it as 0x18 bytes. */
struct UnkDebugLine_80020A20 {
    UnkDebugLine_80020A20(const UnkDebugLine_80020A20 &line);

    /* 0x00 */ Vec3f m_start;
    /* 0x0C */ Vec3f m_end;
};
SIZE_ASSERT(UnkDebugLine_80020A20, 0x18);

/* A box: centre, size and rotation (struct name unknown; the constructor is out of line at
 * 0x8002DBC8; the meaning of the last member is unknown). */
struct UnkBox_8002DBC8 {
    UnkBox_8002DBC8(const Vec3f &center, const Vec3f &size, const Vec3f &rotation, int arg3);

    /* 0x00 */ Vec3f m_center;
    /* 0x0C */ Vec3f m_size;
    /* 0x18 */ Vec3f m_rotation;
    /* 0x24 */ s32 field_0x24;
};
SIZE_ASSERT(UnkBox_8002DBC8, 0x28);

/* A capsule and a cylinder: centre, half height, radius and rotation (struct names unknown; the
 * constructors are out of line at 0x80030A5C and 0x80034854; the meaning of the last member is
 * unknown). */
struct UnkCapsule_80030A5C {
    UnkCapsule_80030A5C(const Vec3f &center, float halfHeight, float radius, const Vec3f &rotation,
                        int arg4);

    /* 0x00 */ Vec3f m_center;
    /* 0x0C */ float m_halfHeight;
    /* 0x10 */ float m_radius;
    /* 0x14 */ Vec3f m_rotation;
    /* 0x20 */ s32 field_0x20;
};
SIZE_ASSERT(UnkCapsule_80030A5C, 0x24);

struct UnkCylinder_80034854 {
    UnkCylinder_80034854(const Vec3f &center, float halfHeight, float radius, const Vec3f &rotation,
                         int arg4);

    /* 0x00 */ Vec3f m_center;
    /* 0x0C */ float m_halfHeight;
    /* 0x10 */ float m_radius;
    /* 0x14 */ Vec3f m_rotation;
    /* 0x20 */ s32 field_0x20;
};
SIZE_ASSERT(UnkCylinder_80034854, 0x24);

extern "C" {
/* Returns a copy of the vector at v (the shapes' centres go through it). */
Vec3f fn_800169F8(const Vec3f *v);
}

namespace SonicteamUSA {
namespace System {

/* A queued shape: its colour, and Draw. */
class DebugDispBase {
  public:
    DebugDispBase(const UnkColor_8040D2D8 &color) : m_color(color) {}
    virtual ~DebugDispBase() {}
    virtual void Draw() = 0;

    /* 0x4 */ UnkColor_8040D2D8 m_color;
};

class DebugDispCapsule : public DebugDispBase {
  public:
    DebugDispCapsule(const Vec3f &center, const Vec3f &rotation, const UnkColor_8040D2D8 &color,
                     int axis, float halfHeight, float radius);
    virtual ~DebugDispCapsule();
    virtual void Draw();

    void Render(const Vec3f &center, float halfHeight, float radius, const Vec3f &rotation,
                const UnkColor_8040D2D8 &color, int axis);

    /* 0x08 */ UnkCapsule_80030A5C m_capsule;
    /* 0x2C */ s32 m_axis;
};

class DebugDispBox : public DebugDispBase {
  public:
    DebugDispBox(const Vec3f &center, const Vec3f &size, const Vec3f &rotation,
                 const UnkColor_8040D2D8 &color, int axis);
    virtual ~DebugDispBox();
    virtual void Draw();

    void Render(const Vec3f &center, const Vec3f &size, const Vec3f &rotation,
                const UnkColor_8040D2D8 &color, int axis);

    /* 0x08 */ UnkBox_8002DBC8 m_box;
    /* 0x30 */ s32 m_axis;
};

class DebugDispLine : public DebugDispBase {
  public:
    DebugDispLine(const UnkDebugLine_80020A20 &line, const UnkColor_8040D2D8 &color, float width);
    virtual ~DebugDispLine();
    virtual void Draw();

    void Render(const UnkDebugLine_80020A20 &line, const UnkColor_8040D2D8 &color, float width);

    /* 0x08 */ UnkDebugLine_80020A20 m_line;
    /* 0x20 */ float m_width;
};

class DebugDispCylinder : public DebugDispBase {
  public:
    DebugDispCylinder(const Vec3f &center, const Vec3f &rotation, const UnkColor_8040D2D8 &color,
                      int axis, float halfHeight, float radius);
    virtual ~DebugDispCylinder();
    virtual void Draw();

    void Render(const Vec3f &center, float halfHeight, float radius, const Vec3f &rotation,
                const UnkColor_8040D2D8 &color, int axis);

    /* 0x08 */ UnkCylinder_80034854 m_cylinder;
    /* 0x2C */ s32 m_axis;
};

class DebugDispSphere : public DebugDispBase {
  public:
    DebugDispSphere(const Vec3f &center, const UnkColor_8040D2D8 &color, float radius);
    virtual ~DebugDispSphere();
    virtual void Draw();

    void Render(const Vec3f &center, const UnkColor_8040D2D8 &color, float radius);

    /* 0x08 */ UnkSphere_80035CEC m_sphere;
};

} // namespace System
} // namespace SonicteamUSA

SIZE_ASSERT(SonicteamUSA::System::DebugDispBase, 0x8);
SIZE_ASSERT(SonicteamUSA::System::DebugDispCapsule, 0x30);
SIZE_ASSERT(SonicteamUSA::System::DebugDispBox, 0x34);
SIZE_ASSERT(SonicteamUSA::System::DebugDispLine, 0x24);
SIZE_ASSERT(SonicteamUSA::System::DebugDispCylinder, 0x30);
SIZE_ASSERT(SonicteamUSA::System::DebugDispSphere, 0x18);

/* The models the shapes are drawn with (loaded from DebugModels.one by DebugDisp). */
struct RpClump;
extern RpClump *lbl_805F06DC; /* sphere_model.dff */
extern RpClump *lbl_805F06E0; /* cube_model.dff */
extern RpClump *lbl_805F06E4; /* cylinder_model.dff */
extern RpClump *lbl_805F06E8; /* capsule_model.dff */
extern RpClump *lbl_805F06EC; /* cone_model.dff */

#endif
