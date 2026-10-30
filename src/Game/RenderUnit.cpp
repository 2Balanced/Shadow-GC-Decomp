/* Render (class name from the RTTI of its boost::shared_ptr control block) and RenderUnit (class
 * name from RTTI), the task that runs a Render's callbacks under a render layer every frame: a plain
 * callback that receives the Render, and a boost::function that receives the Render and the frame
 * time (RednerDrawArgs, the game's spelling). Other units create Renders through fn_8004DFB0,
 * fn_8004E24C and fn_8004E2D4 and declare the class locally (Game/Render.h); Game/DebugDisp.cpp
 * deletes the ones it creates. Names other than the classes' are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#define MSL_RUNTIME_ERROR_STRING
#include <boost/function.hpp>

#include "Game/DebugDraw.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class Render;

/* The argument of a Render's draw function (struct name from the game's RTTI). */
struct RednerDrawArgs {
    /* 0x0 */ Render *m_render;
    /* 0x4 */ float m_dt;
};

/* A Render's draw function (Render's 0x14; struct name unknown, as in Game/WeaponLockonMark.cpp,
 * which assigns it). */
struct UnkRenderCallback_80062CD0 {
    bool Empty() {
        return m_function.empty();
    }
    void Call(RednerDrawArgs &args) {
        if (!m_function.empty()) {
            m_function(args);
        }
    }

    /* 0x0 */ boost::function1<void, RednerDrawArgs &> m_function;
};

typedef void (*RenderCallback)(Render *render);

class RenderUnit;

class Render {
  public:
    Render(void *layer, void *user, RenderCallback callback);
    Render(void *user, RenderCallback callback);
    ~Render();

    /* 0x00 */ UnkSphere_8004E6CC m_sphere;
    /* 0x10 */ RenderCallback m_callback;
    /* 0x14 */ UnkRenderCallback_80062CD0 m_drawCallbacks;
    /* 0x20 */ void *m_user;
    /* 0x24 */ RenderUnit *m_task;
};
SIZE_ASSERT(Render, 0x28);

/* Deletes a Render (name inferred; see checked_delete below). */
inline void DeleteRender(Render *render) {
    delete render;
}

/* Retail keeps boost::checked_delete<Render> out of line (0x8004E1F4); GC/2.7 does so only when its
 * body calls an inline function (see docs/research-notes.md, EnergyCore). */
namespace boost {
template <> inline void checked_delete<Render>(Render *x) {
    DeleteRender(x);
}
} // namespace boost

class RenderUnit : public Task {
  public:
    RenderUnit(Task *parent, Render *render);
    virtual ~RenderUnit();
    virtual void Tick(float dt);

    static const char *s_taskName;

    /* 0x28 */ Render *m_render;
    /* 0x2C */ u32 field_0x2C;
};
SIZE_ASSERT(RenderUnit, 0x30);

extern "C" {
Render *fn_8004E2D4(void *layer, void *user, RenderCallback callback);
boost::shared_ptr<Render> fn_8004E24C(void *layer);
boost::shared_ptr<Render> fn_8004DFB0(void *layer, const char *name);
}

const char *RenderUnit::s_taskName = "RenderUnit";


Render::Render(void *user, RenderCallback callback) {
    m_user = user;
    m_callback = callback;
}

Render::Render(void *layer, void *user, RenderCallback callback) {
    m_user = user;
    m_callback = callback;
    m_task = new RenderUnit((Task *)layer, this);
}

Render::~Render() {
    if (m_task) {
        m_task->m_flags |= 1;
        m_task = 0;
    }
}

RenderUnit::RenderUnit(Task *parent, Render *render) : Task(parent) {
    m_name = s_taskName;
    m_flags |= 0x100;
    m_render = render;
}

RenderUnit::~RenderUnit() {}

void RenderUnit::Tick(float dt) {
    if (m_render->m_callback) {
        m_render->m_callback(m_render);
    }
    if (!m_render->m_drawCallbacks.Empty()) {
        RednerDrawArgs args;
        args.m_render = m_render;
        args.m_dt = dt;
        m_render->m_drawCallbacks.Call(args);
    }
}

Render *fn_8004E2D4(void *layer, void *user, RenderCallback callback) {
    Render *render = new Render(user, callback);
    render->m_task = new RenderUnit((Task *)layer, render);
    return render;
}

boost::shared_ptr<Render> fn_8004E24C(void *layer) {
    boost::shared_ptr<Render> render = boost::shared_ptr<Render>(new Render(layer, 0, 0));
    return render;
}

boost::shared_ptr<Render> fn_8004DFB0(void *layer, const char *name) {
    boost::shared_ptr<Render> render = boost::shared_ptr<Render>(new Render(layer, 0, 0));
    render->m_task->m_name = name;
    return render;
}

