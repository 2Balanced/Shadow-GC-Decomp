/* GiHPGaugeEnemy (class name from the RTTI of its shared_ptr's control block): an enemy's health
 * gauge on screen. It hides its state in a GiHPGaugeEnemyImpl (class name from the RTTI of its draw
 * callback's bind_t) that plays the scene "HPBar" of the CSD project "CsdFiles/HPBar" at the screen
 * position of a point, while the point lies within a depth range and inside the viewport being
 * drawn, sets the scene's frame from the health ratio and widens the bar's outline by the maximum
 * health. The file name and the names marked "(name inferred)" are not original; the functions
 * other units call keep their fn_ names. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). The unit also emits the copy constructor of the
 * CSD project (0x800BFF30), which other units call. */
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <functional>

#define GIHPGAUGEENEMY_IMPL
#include "Game/CsdRCPtr.h"
#include "Game/GiHPGaugeEnemy.h"
#include "Game/MathTypes.h"
#include "Game/Render.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Chao::CSD::CNode;
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

/* The containers of a CSD project, as far as its constructor uses them (the library's own
 * declarations are in Game/CsdProject.h, which game code does not include): only their
 * constructors, which the game links out of line. */
namespace Chao {
namespace CSD {
struct LessName;
struct SReorder;
struct SProject;
class CTexList;
class CFontList;
class CProject;
} // namespace CSD
} // namespace Chao

/* Prepares a project (behaviour inferred; Game/csdmProject.cpp). */
extern "C" void fn_8036E39C(Chao::CSD::CProject *project);

namespace std {
template <class Key, class T, class Compare> class multimap {
  public:
    multimap();

    /* 0x0 */ u8 m_tree[0x10];
};
template <class T> class list {
  public:
    list();

    /* 0x0 */ u8 m_list[0xC];
};
} // namespace std

namespace Chao {
namespace CSD {

/* Does nothing (an inline call that keeps its caller out of line, as the library headers' inline
 * members do; see Game/CsdRCPtr.h). */
inline void __csd_project_nothing() {}

/* CResourceBase (class name from game code's RTTI): a resource of the library (see
 * Game/CsdScene.h). */
template <class T> class CResourceBase {
  public:
    CResourceBase();
    virtual ~CResourceBase();
    virtual void CopyFrom(const CResourceBase &other);

    /* 0x4 */ RCPtr<unsigned char> m_data;
    /* 0x8 */ T *m_resource;
};

/* The loaded CSD project (Chao::CSD::CProject, Game/csdmProject.cpp; 0x4C bytes, fn_8003E808
 * constructs an empty one, fn_8036E114 loads a file into it). The copy constructor makes a project
 * that plays a loaded one's data. */
class CProject : public CResourceBase<SProject> {
  public:
    CProject();
    CProject(const CProject &other) {
        __csd_project_nothing();
        fn_8036E39C(this);
        CopyFrom(other);
    }
    virtual ~CProject();
    virtual void CopyFrom(const CResourceBase<SProject> &other);

    static void *operator new(u32 size) {
        return fn_803605AC()->Alloc(size, "csdBase.h", 40);
    }

    /* 0x0C */ std::multimap<const char *, RCPtr<CScene>, LessName> m_scenes;
    /* 0x1C */ std::multimap<float, RCPtr<CScene>, std::less<float> > m_order;
    /* 0x2C */ RCPtr<CTexList> m_texList;
    /* 0x30 */ CFontList *m_fontList;
    /* 0x34 */ std::list<RCPtr<CScene> > m_removals;
    /* 0x40 */ std::list<SReorder> m_reorders;
};

/* A node of a scene (as far as this unit uses it). */
class CNode {
  public:
    float GetX() const;
    void SetX(float x);
};

} // namespace CSD
} // namespace Chao

using Chao::CSD::CProject;

SIZE_ASSERT(CProject, 0x4C);

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
void fn_8036E114(CProject *resource, const char *file);

/* Projects (behaviour inferred): advancing and drawing, a scene by name, and fn_8036D598, whose
 * result is dropped. */
void fn_8036DAD4(CProject *project);
void fn_8036D94C(CProject *project);
RCPtr<CScene> fn_8036D660(CProject *project, const char *name);
UnkCsdHandle_80042DF0 fn_8036D598(CProject *project, int arg1);

/* Scenes (behaviour inferred): the motion frame, the position, the scale, and a node by name. */
void fn_80366740(CScene *scene, float frame);
void fn_80365AC8(CScene *scene, float x, float y);
void fn_803659FC(CScene *scene, float x, float y);
RCPtr<CNode> fn_803666B4(CScene *scene, const char *name);
/* Scales a node (behaviour inferred). */
void fn_8036A2CC(CNode *node, float scale);

/* The game's screen and the viewport being drawn (behaviour inferred). */
UnkScreenHolder_8000DCC0 *fn_8000DCC0(void);
s32 fn_800623A0(UnkScreenHolder_8000DCC0 *holder);
/* The screen position of a point as the camera of player sees it, its depth in z (behaviour
 * inferred). */
Vec3f fn_8000F654(const Vec3f *position, s32 player);

/* A viewport's rectangle and the full screen's (Game/ScreenViewport.cpp), and a rectangle's left,
 * top, right and bottom edges. */
const ScreenRect *fn_80272E94(s32 viewport);
const ScreenRect *fn_80272E7C(s32 viewport);
s32 fn_8000F850(const ScreenRect *rect);
s32 fn_8000F858(const ScreenRect *rect);
s32 fn_80063018(const ScreenRect *rect);
s32 fn_80063008(const ScreenRect *rect);

bool fn_800BF124(GiHPGaugeEnemy *gauge);
}

/* The game's vector assignment, out of line (as in Game/JumpBoard.cpp). */
inline void AssignVector(Vec3f &to, const Vec3f &from) {
    fn_800091D0(&to, &from);
}

class GiHPGaugeEnemyImpl {
  public:
    GiHPGaugeEnemyImpl();
    ~GiHPGaugeEnemyImpl();

    /* The Render's draw callback (name inferred). */
    void Draw(RednerDrawArgs &args);
    /* Widens the outline by the maximum health (name inferred). */
    void UpdateBars(RCPtr<CScene> scene);
    /* Places the scene at the point within viewport; false when it is not shown (name inferred). */
    bool Place(RCPtr<CScene> scene, s32 viewport);

    /* Setters (names inferred). */
    void SetMaxHealth(float value) {
        m_maxHealth = value;
    }
    void SetFar(float value) {
        m_far = value;
    }
    void SetScale(float value) {
        m_scale = value;
    }
    void SetPosition(const Vec3f *position) {
        AssignVector(m_position, *position);
    }
    float SetRatio(float ratio) {
        float old = m_ratio;
        m_ratio = ratio;
        return old;
    }
    bool SetVisible(bool visible) {
        bool old = m_visible;
        m_visible = visible;
        return old;
    }
    bool IsVisible() const {
        return m_visible;
    }

    static CProject *s_resource;

    /* 0x00 */ boost::shared_ptr<Render> m_render;
    /* 0x08 */ CProject *m_project;
    /* 0x0C */ bool m_visible;
    /* 0x10 */ float m_ratio; /* the health left, 0 to 1 (meaning inferred) */
    /* 0x14 */ float m_scale; /* the scene's scale at depth 1 (meaning inferred) */
    /* 0x18 */ float m_near;  /* the depth range the gauge is shown in (meaning inferred) */
    /* 0x1C */ float m_far;
    /* 0x20 */ Vec3f m_position;
    /* 0x2C */ Vec3f m_screen; /* the point on the screen, its depth in z */
    /* 0x38 */ float m_scaleX;
    /* 0x3C */ float m_scaleY;
    /* 0x40 */ float m_maxHealth; /* negative: the default width (meaning inferred) */
    /* 0x44 */ float m_outlineX;  /* the outline nodes' positions as loaded (meaning inferred) */
    /* 0x48 */ float m_leftX;
    /* 0x4C */ float m_rightX;
};
SIZE_ASSERT(GiHPGaugeEnemyImpl, 0x50);

/* Deletes the Impl (name inferred; see checked_delete below). */
inline void DeleteImpl(GiHPGaugeEnemyImpl *impl) {
    delete impl;
}

/* Deletes the gauge (name inferred; see checked_delete below). */
inline void DeleteGauge(GiHPGaugeEnemy *gauge) {
    delete gauge;
}

/* Retail keeps boost::checked_delete of both classes out of line and their destructors at their
 * own places; these specializations give that order under GC/2.7 (see docs/research-notes.md and
 * Game/EnergyCore.cpp). */
namespace boost {
template <>
inline void checked_delete<GiHPGaugeEnemy>(GiHPGaugeEnemy *x) {
    DeleteGauge(x);
}
template <>
inline void checked_delete<GiHPGaugeEnemyImpl>(GiHPGaugeEnemyImpl *x) {
    DeleteImpl(x);
}
} // namespace boost

/* Not original: an unused table that pools the unit's .rodata strings in retail's order (the
 * linker drops the table; the RTTI names and literals below reuse these copies). The kept-whole
 * .rodata also holds RTTI names of the CSD library headers' inline classes, which this unit does
 * not reconstruct (see Game/Gong.cpp). */
static const char *s_rodataPool[] = {
    "boost::_bi::bind_t<void, boost::_mfi::mf1<void, GiHPGaugeEnemyImpl, RednerDrawArgs &>, boost::_bi::list2<boost::_bi::value<GiHPGaugeEnemyImpl *>, boost::arg<1>>>",
    "csdBase.h",
    "CsdFiles/HPBar",
    "Not inititalized m_eDealloctor\n",
    "csdRCObject.h",
    "outlinel",
    "outliner",
    "Memory allocation failure",
    "Chao::CSD::RCPtr<Chao::CSD::CSceneList>::RCObject<Chao::CSD::CSceneList>::CFunctorBase",
    "Chao::CSD::CBase",
    "Chao::CSD::RCPtr<Chao::CSD::CSceneList>::RCObject<Chao::CSD::CSceneList>",
    "Chao::CSD::RCPtr<Chao::CSD::CNode>::RCObject<Chao::CSD::CNode>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CNode>::RCObject<Chao::CSD::CNode>",
    "Chao::CSD::RCPtr<Chao::CSD::CScene>::RCObject<Chao::CSD::CScene>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CScene>::RCObject<Chao::CSD::CScene>",
    "boost::detail::sp_counted_base_impl<GiHPGaugeEnemy *, boost::checked_deleter<GiHPGaugeEnemy>>",
    "boost::detail::sp_counted_base",
    "std::exception",
    "exception",
    "std::bad_alloc",
    "bad_alloc",
    "Chao::CSD::RCPtr<unsigned char>::RCObject<unsigned char>::CFunctorBase",
    "Chao::CSD::RCPtr<unsigned char>::RCObject<unsigned char>",
    "Chao::CSD::CResourceBase<Chao::CSD::Project>",
    "Chao::CSD::RCPtr<Chao::CSD::CTexList>::RCObject<Chao::CSD::CTexList>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CTexList>::RCObject<Chao::CSD::CTexList>",
    "boost::checked_deleter<GiHPGaugeEnemy>",
};

CProject *GiHPGaugeEnemyImpl::s_resource;

GiHPGaugeEnemyImpl::GiHPGaugeEnemyImpl()
    : m_visible(false), m_ratio(1.0f), m_scale(50.0f), m_near(20.0f), m_far(200.0f), m_maxHealth(-1.0f) {
    m_render = fn_8004E24C(fn_80046178()->field_0x30);
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&GiHPGaugeEnemyImpl::Draw, this, _1));
    if (s_resource != 0) {
        m_project = new CProject(*s_resource);
        if (m_project != 0) {
            fn_8036D598(m_project, 0);
            RCPtr<CScene> scene = fn_8036D660(m_project, "HPBar");
            RCPtr<CNode> node;
            node = fn_803666B4(scene.Get(), "outline");
            if (node.Get() != 0) {
                m_outlineX = node.Get()->GetX();
            }
            node = fn_803666B4(scene.Get(), "outlinel");
            if (node.Get() != 0) {
                m_leftX = node.Get()->GetX();
            }
            node = fn_803666B4(scene.Get(), "outliner");
            if (node.Get() != 0) {
                m_rightX = node.Get()->GetX();
            }
        }
    }
}

GiHPGaugeEnemyImpl::~GiHPGaugeEnemyImpl() {
    if (m_project != 0) {
        delete m_project;
    }
}

bool GiHPGaugeEnemyImpl::Place(RCPtr<CScene> scene, s32 viewport) {
    UnkScreenHolder_8000DCC0 *holder = fn_8000DCC0();
    AssignVector(m_screen, fn_8000F654(&m_position, lbl_805F0170));
    if (m_screen.z < holder->m_camera->m_nearPlane) {
        return false;
    }
    if (m_screen.z > holder->m_camera->m_farPlane) {
        return false;
    }
    if (m_screen.z < m_near) {
        return false;
    }
    if (m_screen.z > m_far) {
        return false;
    }
    const ScreenRect *rect = fn_80272E94(viewport);
    if (m_screen.x > fn_80063018(rect) || m_screen.x < fn_8000F850(rect) || m_screen.y > fn_80063008(rect) ||
        m_screen.y < fn_8000F858(rect)) {
        return false;
    }
    m_scaleX = m_scaleY = m_scale / m_screen.z;
    const ScreenRect *full = fn_80272E7C(viewport);
    m_scaleX = m_scaleX * rect->size.x / full->size.x;
    m_scaleY = m_scaleY * rect->size.y / full->size.y;
    fn_80365AC8(scene.Get(), m_screen.x - rect->position.x, m_screen.y - rect->position.y);
    fn_803659FC(scene.Get(), m_scaleX, m_scaleY);
    return true;
}

void GiHPGaugeEnemyImpl::UpdateBars(RCPtr<CScene> scene) {
    float width = m_maxHealth < 0.0f ? 4.0f : m_maxHealth;
    RCPtr<CNode> outline = fn_803666B4(scene.Get(), "outline");
    RCPtr<CNode> left = fn_803666B4(scene.Get(), "outlinel");
    RCPtr<CNode> right = fn_803666B4(scene.Get(), "outliner");
    if (outline.Get() == 0 || left.Get() == 0 || right.Get() == 0) {
        return;
    }
    width *= 0.25f;
    fn_8036A2CC(outline.Get(), width);
    outline.Get()->SetX(m_outlineX * width);
    left.Get()->SetX(m_leftX * width);
    right.Get()->SetX(m_rightX * width);
}

void GiHPGaugeEnemyImpl::Draw(RednerDrawArgs &args) {
    if (lbl_8057D6A0.m_hudVisible && m_visible && m_project != 0) {
        RCPtr<CScene> scene = fn_8036D660(m_project, "HPBar");
        if (scene.Get() != 0) {
            s32 viewport = fn_800623A0(fn_8000DCC0());
            if (!Place(scene, viewport)) {
                return;
            }
            UpdateBars(scene);
            float frame = 20.0f * (1.0f - m_ratio);
            fn_80366740(scene.Get(), frame);
            fn_8036DAD4(m_project);
        }
        fn_8036D94C(m_project);
    }
}

GiHPGaugeEnemy::GiHPGaugeEnemy() : m_impl(new GiHPGaugeEnemyImpl) {}

GiHPGaugeEnemy::~GiHPGaugeEnemy() {}

/* Whether the gauge shows (name inferred). */
extern "C" bool fn_800BF124(GiHPGaugeEnemy *gauge) {
    return gauge->m_impl->IsVisible();
}

/* Shows or hides the gauge; returns whether it showed (name inferred). */
extern "C" bool fn_800BF0EC(GiHPGaugeEnemy *gauge, bool visible) {
    return gauge->m_impl->SetVisible(visible);
}

/* Sets the health ratio; returns the previous one (name inferred). */
extern "C" float fn_800BF0B0(GiHPGaugeEnemy *gauge, float ratio) {
    return gauge->m_impl->SetRatio(ratio);
}

/* Moves the gauge to position (name inferred). */
extern "C" void fn_800BF068(GiHPGaugeEnemy *gauge, const Vec3f *position) {
    gauge->m_impl.get()->SetPosition(position);
}

/* Sets the scene's scale at depth 1 (name inferred). */
extern "C" void fn_800BF038(GiHPGaugeEnemy *gauge, float scale) {
    gauge->m_impl->SetScale(scale);
}

/* Sets the far end of the depth range (name inferred). */
extern "C" void fn_800BF008(GiHPGaugeEnemy *gauge, float far) {
    gauge->m_impl->SetFar(far);
}

/* Sets the maximum health, which widens the bar (name inferred). */
extern "C" void fn_800BEFD0(GiHPGaugeEnemy *gauge, float health) {
    gauge->m_impl->SetMaxHealth(health);
}

/* Creates a gauge (name inferred). */
extern "C" boost::shared_ptr<GiHPGaugeEnemy> fn_800BED6C(void) {
    boost::shared_ptr<GiHPGaugeEnemy> gauge(new GiHPGaugeEnemy);
    return gauge;
}

/* Loads the CSD resource (name inferred). */
extern "C" void fn_800BECF8(void) {
    GiHPGaugeEnemyImpl::s_resource = new CProject();
    fn_8036E114(GiHPGaugeEnemyImpl::s_resource, fn_800423B8(fn_8003F8D4(), "CsdFiles/HPBar", 0));
}

/* Frees the CSD resource (name inferred). */
extern "C" void fn_800BECB0(void) {
    if (GiHPGaugeEnemyImpl::s_resource != 0) {
        delete GiHPGaugeEnemyImpl::s_resource;
    }
    GiHPGaugeEnemyImpl::s_resource = 0;
}
