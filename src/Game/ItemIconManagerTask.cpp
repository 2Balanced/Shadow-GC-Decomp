/* ItemIcon::ManagerTask, ItemIcon::ManagerUnit and ItemIcon::Icon (class names from RTTI; the
 * task's name is "ItemIconManager"): the icons of the items a player picks up, drawn in a row
 * at the bottom of the player's viewport. The manager (class name unknown, a function-local
 * static behind fn_8009B6A0) keeps one ManagerUnit per player; each pickup adds an Icon to the
 * player's unit, and the row disappears 2.2 seconds after the last pickup. The icon textures
 * come from STG_GDT.TXD. File name and member names are inferred. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first).
 *
 * Not linking yet: every function and every data section matches, but GC/2.7 emits the vector
 * members instantiated at the end of the unit class by class (insert_n, destroy_all, reserve and
 * the destructor of the units' vector, then the same for the icons' vector), while retail emits
 * insert_n and reserve of both vectors first and destroy_all and the destructors of both after
 * them (0x80347560..0x80347770). */
#define MSL_VECTOR_MIDDLE
#define MSL_VECTOR_AT
#define MSL_VECTOR_BACK
#define MSL_VECTOR_DESTROY_ALL_LATE
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <vector>

#include "Game/DebugDisp.h"
#include "Game/Render.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" void *memset(void *dst, int value, unsigned long n);

/* RenderWare (rw/rwcore.h): a raster, a texture and a texture dictionary (opaque here). */
struct RwRaster;
struct RwTexDictionary;
struct RwTexture {
    /* 0x0 */ RwRaster *raster;
};

/* A 2D vertex as RwIm2DRenderPrimitive takes it. */
struct UnkIm2DVertex {
    /* 0x00 */ float x;
    /* 0x04 */ float y;
    /* 0x08 */ float z;
    /* 0x0C */ u8 red;
    /* 0x0D */ u8 green;
    /* 0x0E */ u8 blue;
    /* 0x0F */ u8 alpha;
    /* 0x10 */ float u;
    /* 0x14 */ float v;
};
SIZE_ASSERT(UnkIm2DVertex, 0x18);

typedef int (*RwRenderStateSetFunction)(int state, void *value);

typedef struct RwGlobals {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ RwRenderStateSetFunction fpRenderStateSet;
    /* 0x24 */ u8 field_0x24[0x30 - 0x24];
    /* 0x30 */ int (*fpIm2DRenderPrimitive)(int type, UnkIm2DVertex *vertices, int count);
} RwGlobals;

enum {
    rwRENDERSTATETEXTURERASTER = 1,
    rwRENDERSTATETEXTUREFILTER = 9,
    rwRENDERSTATESRCBLEND = 10,
    rwRENDERSTATEDESTBLEND = 11,
    rwRENDERSTATEVERTEXALPHAENABLE = 12,
    rwFILTERLINEAR = 2,
    rwBLENDSRCALPHA = 5,
    rwBLENDINVSRCALPHA = 6,
    rwPRIMTYPETRISTRIP = 4
};

#define RwRenderStateSet(state, value) (lbl_805F15B4->fpRenderStateSet((state), (void *)(value)))

/* The game state (class name unknown; see Game/BlackBull0210ModeCtrlUnit.cpp): the word at 0x8 is
 * the current mode. */
struct UnkObject_80074470 {
    /* 0x0 */ u8 field_0x0[0x8];
    /* 0x8 */ s32 m_mode;
};

/* A point or a size on the screen, and a rectangle (see Game/ScreenViewport.cpp). */
struct ScreenPoint {
    ScreenPoint(s32 x, s32 y) : x(x), y(y) {}
    ScreenPoint(const ScreenPoint &point) : x(point.x), y(point.y) {}

    s32 x;
    s32 y;
};

struct ScreenRect {
    ScreenRect(ScreenPoint position, ScreenPoint size);
    ScreenRect(const ScreenRect &rect) : position(rect.position), size(rect.size) {}

    ScreenPoint position;
    ScreenPoint size;
};

/* The game's screen (see Game/ScreenHolder.h; only the member used here). */
class UnkScreenHolder_8000DCC0 {
  public:
    int GetViewport(int player);
};

/* Set while the game is paused (as in Game/CameraManagerUnit.cpp; meaning inferred). */
extern u8 lbl_805EE1DC;

extern "C" {
/* the RenderWare engine instance */
extern RwGlobals *lbl_805F15B4;
/* RwTexDictionaryFindNamedTexture and RwTexDictionarySetCurrent (by their arguments). */
RwTexture *fn_8048CFFC(RwTexDictionary *dictionary, const char *name);
RwTexDictionary *fn_8048D0C4(RwTexDictionary *dictionary);
/* The resource manager, and a texture dictionary it loads (see Game/ArkCannon.cpp). */
void *fn_800122D4(void);
RwTexDictionary *fn_80012700(void *manager, int arg1, const char *fileName);
/* Where new tasks go (see Game/Gadget.h). */
void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);
/* Creates a Render (behaviour inferred; see Game/Render.h). */
boost::shared_ptr<Render> fn_8004E24C(void *arg0);
UnkObject_80074470 *fn_80074470(void);
UnkScreenHolder_8000DCC0 *fn_8000DCC0(void);
/* A viewport's rectangle, and a rectangle's left and top edges (behaviour inferred). */
const ScreenRect *fn_80272E7C(int viewport);
s32 fn_8000F850(const ScreenRect *rect);
s32 fn_8000F858(const ScreenRect *rect);
}

namespace ItemIcon {

class ManagerUnit;
class ManagerTask;

/* The manager (class name unknown). */
class Manager {
  public:
    Manager();
    ~Manager();
    void Update(float dt);
    void Clear();
    void Show(int player, int icon);
    void ShowItem(int player, int kind, int count);
    void Release();
    void CreateTask();

    /* 0x00 */ ManagerTask *m_task;
    /* 0x04 */ std::vector<boost::shared_ptr<ManagerUnit> > m_units;
};
SIZE_ASSERT(Manager, 0x10);

class Icon {
  public:
    Icon(ManagerUnit *unit, int kind, int index);
    ~Icon();
    void Draw(RednerDrawArgs &args);

    /* 0x00 */ ManagerUnit *m_unit;
    /* 0x04 */ s32 m_kind;  /* the texture */
    /* 0x08 */ s32 m_index; /* the place in the row */
    /* 0x0C */ boost::shared_ptr<Render> m_render;
};
SIZE_ASSERT(Icon, 0x14);

class ManagerUnit {
  public:
    ManagerUnit(int player);
    ~ManagerUnit();
    void Update(float dt);
    void Add(int kind);
    void Clear();

    /* 0x00 */ s32 m_player;
    /* 0x04 */ float m_timer; /* until the row disappears */
    /* 0x08 */ std::vector<boost::shared_ptr<Icon> > m_icons;
};
SIZE_ASSERT(ManagerUnit, 0x14);

class ManagerTask : public Task {
  public:
    ManagerTask(Task *parent) : Task(parent) {
        m_name = "ItemIconManager";
    }
    virtual ~ManagerTask();
    virtual void Tick(float dt);
};
SIZE_ASSERT(ManagerTask, 0x28);


/* Delete an icon and a unit (names inferred; see checked_delete below). */
inline void DeleteIcon(Icon *icon) {
    delete icon;
}

inline void DeleteUnit(ManagerUnit *unit) {
    delete unit;
}

} // namespace ItemIcon

/* Retail keeps boost::checked_delete out of line for both classes and the destructors at their own
 * places; these specializations call inline helpers, which gives retail's code and order under
 * GC/2.7 (see docs/research-notes.md, EnergyCore). */
namespace boost {
template <> inline void checked_delete<ItemIcon::Icon>(ItemIcon::Icon *x) {
    ItemIcon::DeleteIcon(x);
}
template <> inline void checked_delete<ItemIcon::ManagerUnit>(ItemIcon::ManagerUnit *x) {
    ItemIcon::DeleteUnit(x);
}
} // namespace boost

using namespace ItemIcon;

extern "C" Manager *fn_8009B6A0(void);

/* The icon textures (names inferred). */
static const char *s_txdName = "STG_GDT.TXD";
static const char *s_textureNames[10] = {
    "ob_hni00", "ob_hni01", "ob_hni02", "ob_hni03", "ob_hni04",
    "ob_hni05", "ob_hni06", "ob_hni07", "ob_hni09", "ob_hni08",
};
/* The corners of an icon (name inferred). */
static const float s_corners[4][2] = {
    {-1.0f, -1.0f},
    {1.0f, -1.0f},
    {-1.0f, 1.0f},
    {1.0f, 1.0f},
};
static RwTexDictionary *s_txd;
static RwRaster *s_textures[10];
static UnkIm2DVertex s_vertices[4];

Icon::Icon(ManagerUnit *unit, int kind, int index) : m_unit(unit), m_kind(kind), m_index(index) {
    m_render = fn_8004E24C(fn_80046178()->field_0x4C);
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&Icon::Draw, this, _1));
}

Icon::~Icon() {}

void Icon::Draw(RednerDrawArgs &args) {
    if (fn_80074470()->m_mode == 5) {
        return;
    }
    int viewport = fn_8000DCC0()->GetViewport(m_unit->m_player);
    if (viewport == -1) {
        return;
    }
    ScreenRect rect(*fn_80272E7C(viewport));
    float x = fn_8000F850(&rect) + 0.5f * rect.size.x;
    float y = 372.0f + fn_8000F858(&rect);
    float left = -(26.0f * (m_unit->m_icons.size() - 1.0f));
    x += left + 50.0f * m_index;
    UnkColor_8040D2D8 color(0xFF, 0xFF, 0xFF, 0xFF);
    if (m_unit->m_timer < 0.2f) {
        color.r = 255.0f * (m_unit->m_timer / 0.2f);
    }
    RwRenderStateSet(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, 1);
    if (s_txd != 0) {
        fn_8048D0C4(s_txd);
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, s_textures[m_kind]);
        RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, rwFILTERLINEAR);
    }
    for (int i = 0; i < 4; i++) {
        s_vertices[i].red = color.g;
        s_vertices[i].green = color.b;
        s_vertices[i].blue = color.a;
        s_vertices[i].alpha = color.r;
        s_vertices[i].x = 24.0f * s_corners[i][0] + x;
        s_vertices[i].y = 24.0f * s_corners[i][1] + y;
    }
    lbl_805F15B4->fpIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, s_vertices, 4);
}

ManagerUnit::ManagerUnit(int player) : m_player(player) {
    m_icons.reserve(16);
}

ManagerUnit::~ManagerUnit() {
    Clear();
}

void ManagerUnit::Update(float dt) {
    if (m_icons.empty()) {
        return;
    }
    s32 mode = fn_80074470()->m_mode;
    if (mode != 3 && mode != 9 && mode != 5) {
        Clear();
    } else {
        if (!lbl_805EE1DC) {
            m_timer -= dt;
        }
        if (m_timer < 0.0f) {
            Clear();
        }
    }
}

void ManagerUnit::Add(int kind) {
    m_timer = 2.2f;
    m_icons.push_back(boost::shared_ptr<Icon>(new Icon(this, kind, m_icons.size())));
}

void ManagerUnit::Clear() {
    m_icons.clear();
    m_timer = 0.0f;
}

ManagerTask::~ManagerTask() {
    fn_8009B6A0()->Release();
}

void ManagerTask::Tick(float dt) {
    fn_8009B6A0()->Update(dt);
}

Manager::Manager() : m_task(0) {
    s_txd = 0;
    memset(s_textures, 0, sizeof(s_textures));
    for (int i = 0; i < 4; i++) {
        s_vertices[i].u = 0.5f * (1.0f + s_corners[i][0]);
        s_vertices[i].v = 0.5f * (1.0f + s_corners[i][1]);
    }
    m_units.reserve(2);
    for (int i = 0; i < 2; i++) {
        m_units.push_back(boost::shared_ptr<ManagerUnit>(new ManagerUnit(i)));
    }
}

Manager::~Manager() {
    m_units.clear();
    memset(s_textures, 0, sizeof(s_textures));
    s_txd = 0;
}

void Manager::CreateTask() {
    m_task = new ManagerTask(fn_801EB304(fn_8001C4F8(), 15));
}

void Manager::Release() {
    Clear();
    memset(s_textures, 0, sizeof(s_textures));
    s_txd = 0;
    m_task = 0;
}

/* Shows the icon of an item kind picked up (kind 0, rings, by count) (name inferred). */
void Manager::ShowItem(int player, int kind, int count) {
    int icon = -1;
    switch (kind) {
    case 0:
        if (count == 5) {
            icon = 0;
        } else if (count == 10) {
            icon = 1;
        } else if (count == 20) {
            icon = 2;
        }
        break;
    case 1:
        icon = 3;
        break;
    case 2:
        icon = 4;
        break;
    case 3:
        icon = 5;
        break;
    case 4:
        icon = 6;
        break;
    case 5:
        icon = 9;
        break;
    case 6:
        icon = 8;
        break;
    case 9:
        icon = 7;
        break;
    }
    Show(player, icon);
}

void Manager::Show(int player, int icon) {
    if (player >= 0 && player < 2 && icon > -1 && icon < 10) {
        if (s_txd == 0) {
            s_txd = fn_80012700(fn_800122D4(), 0, s_txdName);
            if (s_txd != 0) {
                for (int i = 0; i < 10; i++) {
                    RwTexture *texture = fn_8048CFFC(s_txd, s_textureNames[i]);
                    if (texture != 0) {
                        s_textures[i] = texture->raster;
                    }
                }
            } else {
                return;
            }
        }
        m_units.at(player)->Add(icon);
    }
}

void Manager::Clear() {
    for (std::vector<boost::shared_ptr<ManagerUnit> >::iterator it = m_units.begin(); it != m_units.end(); ++it) {
        (*it)->Clear();
    }
}

void Manager::Update(float dt) {
    for (std::vector<boost::shared_ptr<ManagerUnit> >::iterator it = m_units.begin(); it != m_units.end(); ++it) {
        (*it)->Update(dt);
    }
}

#include <vector_impl>
