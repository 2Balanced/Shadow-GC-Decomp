/* CircusPole (class name from RTTI): the CIRCUS_POLE gadget of Circus Park, a pole (CIRCUSPOLE.BIN)
 * whose "scale Y" parameter stretches it vertically. Its top (the clump PR0302CIRCUSPOLETOP.DFF with
 * the delta morph animation of the .DMA file) is drawn twice, above and below the pole, from a draw
 * callback the gadget registers with its Render. File name inferred from the class. Method names
 * marked "(name inferred)" are not original. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first).
 *
 * The constructor addresses its file names through the start of .rodata, so the linker keeps that
 * section whole: the RTTI names of the two first headers stay there. */
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <string>

#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/Render.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

/* An unused constant of unknown meaning and type (as in ElecCoreWall.cpp): the linker keeps this
 * unit's .rodata whole, and there it comes before the RTTI names of the next two headers. */
static const float lbl_804BEA90[4][4] = {
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
};

#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI names in .rodata. */
#include "Game/CharaColliBaseCharInfo.h"

/* The RenderWare types and functions used here, declared locally because the RenderWare headers
 * need the C library headers (layouts as in rw/rwcore.h, reduced to the members used). */
extern "C" {
typedef struct RwV3d {
    float x;
    float y;
    float z;
} RwV3d;

typedef struct RwSphere {
    RwV3d center;
    float radius;
} RwSphere;

typedef struct RwFrame RwFrame;
typedef struct RwCamera RwCamera;

typedef struct RwObject {
    u8 type;
    u8 subKind;
    u8 flags;
    u8 privateFlags;
    void *parent;
} RwObject;

typedef struct RpClump {
    /* 0x00 */ RwObject object; /* the parent is the clump's frame */
} RpClump;

typedef struct RwGlobals {
    /* 0x000 */ RwCamera *curCamera;
} RwGlobals;

/* the engine instance */
extern RwGlobals *lbl_805F15B4;

/* whether a sphere is in a camera's view frustum (inferred) */
int fn_804856C8(const RwCamera *camera, const RwSphere *sphere);
/* translates a frame */
RwFrame *fn_804875B0(RwFrame *frame, const RwV3d *translation, int combine);
/* Renders a clump. */
RpClump *fn_80456BC4(RpClump *clump);
}

/* The manager of the clumps whose delta morph animations play (class name unknown; by its
 * neighbour DeltaMorphAnimManagerTask, inferred). It is constructed and destroyed out of line. */
class UnkDeltaMorphAnimManager_801DE414 {
  public:
    UnkDeltaMorphAnimManager_801DE414();
    ~UnkDeltaMorphAnimManager_801DE414();

    static UnkDeltaMorphAnimManager_801DE414 *s_cached;

    /* 0x0 */ u8 field_0x0[0x14];
};

extern "C" {
/* Loads a file of the model loader (behaviour inferred). */
void *fn_800122D4(void);
void *fn_80012700(void *loader, int arg1, const char *fileName);
/* Loads model fileName for a gadget (behaviour inferred; arg0's meaning is unknown). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* Attaches the morph animation anim to clump (see Door.cpp). */
void fn_80037A80(RpClump *clump, void *anim);
/* Turns a frame about its y axis to angle (behaviour inferred). */
void fn_800E9CEC(RwFrame *frame, float angle, int combine);
/* Adds and removes a named clump (behaviour inferred). */
void fn_801DE0EC(UnkDeltaMorphAnimManager_801DE414 *manager, const std::string &name, RpClump *clump);
void fn_801DDFDC(UnkDeltaMorphAnimManager_801DE414 *manager, const std::string &name);
}

/* The clump manager (name inferred): a function-local static object, whose address the accessor
 * caches. */
inline UnkDeltaMorphAnimManager_801DE414 &DeltaMorphAnimManagerObject() {
    static UnkDeltaMorphAnimManager_801DE414 s_object;
    return s_object;
}

inline UnkDeltaMorphAnimManager_801DE414 *GetDeltaMorphAnimManager() {
    if (UnkDeltaMorphAnimManager_801DE414::s_cached == 0) {
        UnkDeltaMorphAnimManager_801DE414::s_cached = &DeltaMorphAnimManagerObject();
    }
    return UnkDeltaMorphAnimManager_801DE414::s_cached;
}

class CircusPole : public Gadget {
  public:
    CircusPole(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~CircusPole();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the pole under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* The draw callback: draws the top above and below the pole (name inferred). */
    void Draw(RednerDrawArgs &args);
    /* Places the model (name inferred). */
    void RebuildTransform();
    /* Copies the owner's position, rotation and parameter (name inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);

    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ float m_scaleY; /* the "scale Y" parameter */
    /* 0x44 */ boost::shared_ptr<Render> m_render;
    /* 0x4C */ RpClump *m_top;
};
SIZE_ASSERT(CircusPole, 0x50);

const char *CircusPole::s_taskName = "CircusPole";

CircusPole::CircusPole(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CB88(0x42, 4, "CircusPole");
    LoadFromOwner(owner);
    m_model = fn_8015F800(1, "CIRCUSPOLE.BIN");
    m_model->fn_80162A68(0, 0);
    void *anim = fn_80012700(fn_800122D4(), 1, "PR0302CIRCUSPOLETOP.DMA");
    m_top = (RpClump *)fn_80012700(fn_800122D4(), 1, "PR0302CIRCUSPOLETOP.DFF");
    fn_80037A80(m_top, anim);
    fn_801DE0EC(GetDeltaMorphAnimManager(), "CircusPole", m_top);
    m_render = fn_8004DFB0(fn_80046178()->field_0x8, "CircusPole");
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&CircusPole::Draw, this, _1));
    RebuildTransform();
}

CircusPole::~CircusPole() {
    fn_801DDFDC(GetDeltaMorphAnimManager(), "CircusPole");
}

void CircusPole::Behave(float dt) {
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }
}

void CircusPole::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    RebuildTransform();
}

void CircusPole::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_scaleY, sizeof(m_scaleY));
}

void CircusPole::RebuildTransform() {
    if (m_model.get() != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B720(&matrix, 1.0f, m_scaleY, 1.0f);
        m_model->fn_801633DC(&matrix);
    }
}

void CircusPole::Draw(RednerDrawArgs &args) {
    RwSphere sphere;

    sphere.radius = 20.0f;
    sphere.center.x = m_chara->m_position.x;
    sphere.center.y = m_chara->m_position.y + 25.0f * m_scaleY;
    sphere.center.z = m_chara->m_position.z;
    if (fn_804856C8(lbl_805F15B4->curCamera, &sphere) != 0) {
        RwFrame *frame = (RwFrame *)m_top->object.parent;

        fn_800E9CEC(frame, m_chara->m_rotation.y, 0);
        fn_804875B0(frame, &sphere.center, 2);
        fn_80456BC4(m_top);
    }

    sphere.center.x = m_chara->m_position.x;
    sphere.center.y = m_chara->m_position.y - 25.0f * m_scaleY - 10.0f;
    sphere.center.z = m_chara->m_position.z;
    if (fn_804856C8(lbl_805F15B4->curCamera, &sphere) != 0) {
        RwFrame *frame = (RwFrame *)m_top->object.parent;

        fn_800E9CEC(frame, m_chara->m_rotation.y, 0);
        fn_804875B0(frame, &sphere.center, 2);
        fn_80456BC4(m_top);
    }
}

extern "C" void fn_801307D8(void) {
    fn_8006B474("CIRCUSPOLE.BIN", 1);
    fn_8006B474("PR0302CIRCUSPOLETOP.DFF", 0);
    fn_8006B474("PR0302CIRCUSPOLETOP.DMA", 0);
}

extern "C" void fn_801307D4(void) {}

void CircusPole::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new CircusPole(fn_801EB304(fn_8001C4F8(), 11), owner);
}

static GadgetParamDesc lbl_805287B4[] = {
    {4, "scale Y", 0, 0, 0, 1.0f, 0.1f, 1000.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80528804 = {
    "CIRCUS_POLE", fn_801307D8, fn_801307D4, CircusPole::Instantiate, 0, 0, 0xC87, 1, 0, 10, lbl_805287B4,
};

UnkDeltaMorphAnimManager_801DE414 *UnkDeltaMorphAnimManager_801DE414::s_cached;
