/* VehicleArrow (class name from the RTTI of its shared_ptr's control block): the arrow drawn over a
 * vehicle. It hides its state in a VehicleArrowImpl (class name from the RTTI of its draw
 * callback's bind_t) that plays the scene "VHArrow" of the CSD project "CsdFiles/VHArrow" at the
 * screen position of a point, scaled by its depth, while the point lies within a depth range and
 * inside the viewport being drawn. The file name and the names marked "(name inferred)" are not
 * original; the functions other units call keep their fn_ names. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CsdRCPtr.h"
#include "Game/MathTypes.h"
#include "Game/Render.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Chao::CSD::CScene;
using Chao::CSD::RCPtr;

/* The argument a Render passes to its draw callbacks, as far as this unit reads it. */
struct RednerDrawArgs {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ float m_dt;
};

/* The game state (struct name unknown; as far as this unit reads it). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x148];
    /* 0x148 */ u8 m_hudVisible; /* meaning inferred */
    /* 0x149 */ u8 field_0x149[0x150 - 0x149];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;
/* Set while the game is paused (meaning inferred): the scene is drawn without advancing. */
extern u8 lbl_805EE1DC;
/* The player of the viewport being drawn (see Game/CameraScreenSet.cpp). */
extern s32 lbl_805F0170;

/* The RenderWare camera (a reduced view of Game/ScreenHolder.h's struct): its clip planes. */
struct UnkScreen_8000DCC0 {
    /* 0x00 */ u8 field_0x0[0x80];
    /* 0x80 */ float m_nearPlane;
    /* 0x84 */ float m_farPlane;
};

/* The game's screen (a reduced view of Game/ScreenHolder.h's class). */
class UnkScreenHolder_8000DCC0 {
  public:
    /* 0x00 */ UnkScreen_8000DCC0 *m_camera;
};

/* A rectangle on the screen (see Game/ScreenViewport.cpp). */
struct ScreenPoint {
    s32 x;
    s32 y;
};

struct ScreenRect {
    ScreenPoint position;
    ScreenPoint size;
};

/* The loaded CSD resource the projects are built from (class name unknown; 0x4C bytes, fn_8003E808
 * constructs it, fn_8036E114 loads a file into it). */
class UnkCsdResource_8003E808 {
  public:
    UnkCsdResource_8003E808();
    virtual ~UnkCsdResource_8003E808();

    /* 0x04 */ u8 field_0x4[0x4C - 0x4];

    static void *operator new(u32 size) {
        return fn_803605AC()->Alloc(size, "csdBase.h", 40);
    }
};

/* The value a project returns from fn_8036D598 (class name unknown). */
class UnkCsdHandle_80042DF0 {
  public:
    UnkCsdHandle_80042DF0(const UnkCsdHandle_80042DF0 &other);
    ~UnkCsdHandle_80042DF0();

    /* 0x0 */ void *m_object;
};

extern "C" {
/* Creates a Render (behaviour inferred; see Game/Render.h). */
boost::shared_ptr<Render> fn_8004E24C(void *arg0);

/* The resource manager's language-specific path of a file (behaviour inferred, see Gong.cpp). */
void *fn_8003F8D4(void);
const char *fn_800423B8(void *arg0, const char *name, int arg2);
/* Loads file into resource (behaviour inferred). */
void fn_8036E114(UnkCsdResource_8003E808 *resource, const char *file);

/* Projects (behaviour inferred): advancing and drawing, a scene by name, and fn_8036D598, whose
 * result is dropped. */
void fn_8036DA1C(UnkCsdProject_800BFF30 *project, float dt);
void fn_8036D94C(UnkCsdProject_800BFF30 *project);
RCPtr<CScene> fn_8036D660(UnkCsdProject_800BFF30 *project, const char *name);
UnkCsdHandle_80042DF0 fn_8036D598(UnkCsdProject_800BFF30 *project, int arg1);

/* Scenes (behaviour inferred): the position and the scale. */
void fn_80365AC8(CScene *scene, float x, float y);
void fn_803659FC(CScene *scene, float x, float y);

/* The game's screen and the viewport being drawn (behaviour inferred). */
UnkScreenHolder_8000DCC0 *fn_8000DCC0(void);
s32 fn_800623A0(UnkScreenHolder_8000DCC0 *holder);
/* The screen position of a point as the camera of player sees it, its depth in z (behaviour
 * inferred). */
Vec3f fn_8000F654(const Vec3f *position, s32 player);

/* A viewport's rectangle (Game/ScreenViewport.cpp), and its left, top, right and bottom edges. */
const ScreenRect *fn_80272E94(s32 viewport);
s32 fn_8000F850(const ScreenRect *rect);
s32 fn_8000F858(const ScreenRect *rect);
s32 fn_80063018(const ScreenRect *rect);
s32 fn_80063008(const ScreenRect *rect);
}

class VehicleArrowImpl {
  public:
    VehicleArrowImpl();
    ~VehicleArrowImpl();

    /* The Render's draw callback (name inferred). */
    void Draw(RednerDrawArgs &args);
    /* Places the scene at the point on the full screen; false when it is not shown (name
     * inferred). */
    bool Place(RCPtr<CScene> scene);
    /* Places the scene at the point within viewport (name inferred). */
    bool PlaceInViewport(RCPtr<CScene> scene, s32 viewport);

    static UnkCsdResource_8003E808 *s_resource;

    /* 0x00 */ boost::shared_ptr<Render> m_render;
    /* 0x08 */ UnkCsdProject_800BFF30 *m_project;
    /* 0x0C */ u8 m_visible;
    /* 0x10 */ float m_scale; /* the scene's scale at depth 1 (meaning inferred) */
    /* 0x14 */ float m_near;  /* the depth range the arrow is shown in (meaning inferred) */
    /* 0x18 */ float m_far;
    /* 0x1C */ Vec3f m_position;
};

class VehicleArrow {
  public:
    VehicleArrow();
    ~VehicleArrow();

    /* 0x0 */ boost::scoped_ptr<VehicleArrowImpl> m_impl;
};

/* Deletes the Impl (name inferred; see checked_delete below). */
inline void DeleteImpl(VehicleArrowImpl *impl) {
    delete impl;
}

/* Deletes the arrow (name inferred; see checked_delete below). */
inline void DeleteArrow(VehicleArrow *arrow) {
    delete arrow;
}

/* Retail keeps boost::checked_delete of both classes out of line and their destructors at their
 * own places; these specializations give that order under GC/2.7 (see docs/research-notes.md and
 * Game/EnergyCore.cpp). */
namespace boost {
template <>
inline void checked_delete<VehicleArrow>(VehicleArrow *x) {
    DeleteArrow(x);
}
template <>
inline void checked_delete<VehicleArrowImpl>(VehicleArrowImpl *x) {
    DeleteImpl(x);
}
} // namespace boost

SIZE_ASSERT(VehicleArrowImpl, 0x28);
SIZE_ASSERT(VehicleArrow, 0x4);

/* A value (name inferred). As a call it is not a constant expression, so the defaults below are
 * stored by a static initializer, as in retail. */
inline float Value(float value) {
    return value;
}

/* The defaults of every arrow: the depth range and the scale (names inferred). */
static const float s_near = Value(20.0f);
static const float s_far = Value(200.0f);
static const float s_scale = Value(60.0f);


UnkCsdResource_8003E808 *VehicleArrowImpl::s_resource;

VehicleArrowImpl::VehicleArrowImpl()
    : m_project(0), m_visible(0), m_scale(s_scale), m_near(s_near), m_far(s_far) {
    m_render = fn_8004E24C(fn_80046178()->field_0x30);
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&VehicleArrowImpl::Draw, this, _1));
    if (s_resource != 0) {
        m_project = new UnkCsdProject_800BFF30(s_resource);
        if (m_project != 0) {
            fn_8036D598(m_project, 0);
        }
    }
}

VehicleArrowImpl::~VehicleArrowImpl() {
    if (m_project != 0) {
        delete m_project;
    }
    m_project = 0;
}


bool VehicleArrowImpl::Place(RCPtr<CScene> scene) {
    UnkScreenHolder_8000DCC0 *holder = fn_8000DCC0();
    Vec3f screen;
    fn_800091EC(&screen, &(const Vec3f &)fn_8000F654(&m_position, 0));
    if (screen.z < holder->m_camera->m_nearPlane) {
        return false;
    }
    if (screen.z > holder->m_camera->m_farPlane) {
        return false;
    }
    if (screen.z < m_near) {
        return false;
    }
    if (screen.z > m_far) {
        return false;
    }
    const ScreenRect *rect = fn_80272E94(0);
    if (screen.x > fn_80063018(rect) || screen.x < fn_8000F850(rect) || screen.y > fn_80063008(rect) ||
        screen.y < fn_8000F858(rect)) {
        return false;
    }
    fn_80365AC8(scene.Get(), screen.x, screen.y);
    float scale = m_scale / screen.z;
    fn_803659FC(scene.Get(), scale, scale);
    return true;
}

bool VehicleArrowImpl::PlaceInViewport(RCPtr<CScene> scene, s32 viewport) {
    UnkScreenHolder_8000DCC0 *holder = fn_8000DCC0();
    Vec3f screen;
    fn_800091EC(&screen, &(const Vec3f &)fn_8000F654(&m_position, lbl_805F0170));
    if (screen.z < holder->m_camera->m_nearPlane) {
        return false;
    }
    if (screen.z > holder->m_camera->m_farPlane) {
        return false;
    }
    if (screen.z < m_near) {
        return false;
    }
    if (screen.z > m_far) {
        return false;
    }
    const ScreenRect *rect = fn_80272E94(viewport);
    if (screen.x > fn_80063018(rect) || screen.x < fn_8000F850(rect) || screen.y > fn_80063008(rect) ||
        screen.y < fn_8000F858(rect)) {
        return false;
    }
    screen.x -= rect->position.x;
    screen.y -= rect->position.y;
    fn_80365AC8(scene.Get(), screen.x, screen.y);
    float scale = m_scale / screen.z;
    fn_803659FC(scene.Get(), scale, scale);
    return true;
}

void VehicleArrowImpl::Draw(RednerDrawArgs &args) {
    if (lbl_8057D6A0.m_hudVisible && m_visible && m_project != 0) {
        RCPtr<CScene> scene = fn_8036D660(m_project, "VHArrow");
        if (scene.Get() != 0) {
            s32 viewport = fn_800623A0(fn_8000DCC0());
            if (viewport == 0) {
                if (!Place(scene)) {
                    return;
                }
            } else {
                if (!PlaceInViewport(scene, viewport)) {
                    return;
                }
            }
            if (!lbl_805EE1DC) {
                fn_8036DA1C(m_project, args.m_dt);
            }
            fn_8036D94C(m_project);
        }
    }
}

VehicleArrow::VehicleArrow() : m_impl(new VehicleArrowImpl) {}

VehicleArrow::~VehicleArrow() {}

/* Shows or hides the arrow (name inferred). */
extern "C" void fn_8033AC24(VehicleArrow *arrow, bool visible) {
    arrow->m_impl->m_visible = visible;
}

/* Moves the arrow to position (name inferred). */
extern "C" void fn_8033ABFC(VehicleArrow *arrow, const Vec3f *position) {
    fn_800091D0(&arrow->m_impl->m_position, position);
}

/* Creates an arrow, or none while the CSD resource is not loaded (name inferred). */
extern "C" boost::shared_ptr<VehicleArrow> fn_8033A9B8(void) {
    if (VehicleArrowImpl::s_resource != 0) {
        boost::shared_ptr<VehicleArrow> arrow(new VehicleArrow);
        return arrow;
    }
    boost::shared_ptr<VehicleArrow> none;
    return none;

}

/* Loads the CSD resource (name inferred). */
extern "C" void fn_8033A938(void) {
    if (VehicleArrowImpl::s_resource == 0) {
        VehicleArrowImpl::s_resource = new UnkCsdResource_8003E808();
        fn_8036E114(VehicleArrowImpl::s_resource, fn_800423B8(fn_8003F8D4(), "CsdFiles/VHArrow", 0));
    }
}

/* Frees the CSD resource (name inferred). */
extern "C" void fn_8033A8F0(void) {
    if (VehicleArrowImpl::s_resource != 0) {
        delete VehicleArrowImpl::s_resource;
        VehicleArrowImpl::s_resource = 0;
    }
}
