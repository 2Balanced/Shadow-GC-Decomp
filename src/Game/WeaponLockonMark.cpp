/* LockonMark (class name from RTTI: Weapon::LockonMark): the mark a LockonTarget
 * (Game/WeaponLockonBase.cpp) shows on its target. The task hides its state in a LockonMarkImpl
 * (class name from the RTTI of its draw callback's bind_t), which tells the target's character it
 * is locked on and released, follows the target, and draws two quads at its screen position
 * through a Render: the frame "am_tgt0" and the ring "am_tgt1", which fade in while the ring
 * shrinks, then fade out once the target is lost. The unit also holds the first copies of the
 * screen helpers and of the function that sets a Render's draw callback, which other units call by
 * their addresses. File name from the class name; the other names are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#define RENDER_DRAW_CALLBACK_DEFINED
#include <boost/function/function_base.hpp>

/* std::swap for boost::function's functor pointer: the game has one shared copy (0x80019164), which
 * this overload, declared but not defined here, stands in for (see Game/CatchStick.cpp). */
namespace std {
void swap(boost::detail::function::any_pointer &a, boost::detail::function::any_pointer &b);
} // namespace std

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/Render.h"
#include "Game/Task.h"
#include "Game/WeaponLockonTarget.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Weapon::LockonTarget;

/* Target (class name from RTTI): a point other objects can aim at (see Game/BulletHoming.cpp). Its
 * first virtual function returns the point to aim at from a position; the character it belongs to
 * is at 0x28 (names inferred). */
class Target {
  public:
    virtual Vec3f GetPosition(const Vec3f &from);

    /* 0x04 */ u8 field_0x4[0x28 - 0x4];
    /* 0x28 */ boost::weak_ptr<CharInfo> m_chara;
};

/* RenderWare (rw/rwcore.h names): a raster, a texture dictionary, a texture, an immediate-mode 2D
 * vertex, and the engine instance's open device as far as this unit uses it. */
struct RwRaster;
struct RwTexDictionary;

struct RwTexture {
    /* 0x0 */ RwRaster *raster;
};

struct RwIm2DVertex {
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
SIZE_ASSERT(RwIm2DVertex, 0x18);

typedef struct RwGlobals {
    /* 0x00 */ u8 field_0x0[0x18];
    /* 0x18 */ float zBufferNear;                                   /* dOpenDevice.zBufferNear */
    /* 0x1C */ float zBufferFar;                                    /* dOpenDevice.zBufferFar */
    /* 0x20 */ int (*fpRenderStateSet)(int state, void *value);     /* dOpenDevice.fpRenderStateSet */
    /* 0x24 */ int (*fpRenderStateGet)(int state, void *value);     /* dOpenDevice.fpRenderStateGet */
    /* 0x28 */ u8 field_0x28[0x30 - 0x28];
    /* 0x30 */ int (*fpIm2DRenderPrimitive)(int primType, RwIm2DVertex *vertices, int count);
} RwGlobals;

enum {
    rwRENDERSTATETEXTURERASTER = 1,
    rwRENDERSTATEZTESTENABLE = 6,
    rwRENDERSTATEZWRITEENABLE = 8,
    rwRENDERSTATETEXTUREFILTER = 9,
    rwRENDERSTATESRCBLEND = 10,
    rwRENDERSTATEDESTBLEND = 11,
    rwRENDERSTATEVERTEXALPHAENABLE = 12,
    rwRENDERSTATEFOGENABLE = 14,
    rwFILTERNEAREST = 1,
    rwBLENDONE = 2,
    rwBLENDSRCALPHA = 5,
    rwPRIMTYPETRISTRIP = 4
};

/* the RenderWare engine instance */
extern RwGlobals *lbl_805F15B4;

#define RwRenderStateSet(state, value) (lbl_805F15B4->fpRenderStateSet((state), (void *)(value)))
#define RwRenderStateGet(state, value) (lbl_805F15B4->fpRenderStateGet((state), (void *)(value)))
#define RwIm2DRenderPrimitive(type, vertices, count)                                               \
    (lbl_805F15B4->fpIm2DRenderPrimitive((type), (vertices), (count)))

/* The game's screen (see Game/ScreenHolder.h; opaque here). */
class UnkScreenHolder_8000DCC0;

/* A rectangle on the screen (see Game/ScreenViewport.cpp). */
struct ScreenPoint {
    s32 x;
    s32 y;
};

struct ScreenRect {
    ScreenPoint position;
    ScreenPoint size;
};

/* The player of the viewport being drawn and that viewport (see Game/CameraScreenSet.cpp). */
extern s32 lbl_805F0170;
extern s32 lbl_805F0174;

extern "C" {
/* Where new tasks go (see Game/Gadget.h). */
void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);
/* The resource manager, and a resource it has loaded into a slot (see Game/WeaponData.cpp, which
 * loads the weapon resource file into slot 2). */
void *fn_800122D4(void);
void *fn_8001277C(void *loader, s32 slot, int id);
/* RwTexDictionaryFindNamedTexture (by its arguments). */
RwTexture *fn_8048CFFC(RwTexDictionary *dictionary, const char *name);
/* Creates a Render (behaviour inferred; see Game/Render.h). */
boost::shared_ptr<Render> fn_8004E24C(void *arg0);
/* The game's screen, and the screen position of a point as the camera of player sees it, its depth
 * in z (behaviour inferred; see Game/VehicleArrow.cpp). */
UnkScreenHolder_8000DCC0 *fn_8000DCC0(void);
Vec3f fn_8000F654(const Vec3f *position, s32 player);
/* A viewport's rectangle (Game/ScreenViewport.cpp), and its left and top edges. */
const ScreenRect *fn_80272E94(s32 viewport);
s32 fn_8000F850(const ScreenRect *rect);
s32 fn_8000F858(const ScreenRect *rect);
}

/* A Render's draw callback (Render's 0x14; struct name unknown): the function it calls. */
struct UnkRenderCallback_80062CD0 {
    /* 0x0 */ boost::function1<void, RednerDrawArgs &> m_function;
};

/* Sets a Render's draw callback (other units call it by its address; see Game/Render.h, which
 * declares it as returning nothing). It calls the inline function1 assignment, so it is emitted as a
 * weak copy after its first caller. */
extern "C" inline void *fn_80062CD0(void *callbacks, const boost::function1<void, RednerDrawArgs &> &callback) {
    static_cast<UnkRenderCallback_80062CD0 *>(callbacks)->m_function = callback;
    return callbacks;
}

namespace Weapon {

/* The mark's state (class name from the RTTI of its draw callback's bind_t). State 0: fading in,
 * 1: following the target, 2: fading out. */
class LockonMarkImpl {
  public:
    LockonMarkImpl(LockonTarget *target);
    ~LockonMarkImpl();

    /* Runs the state; false once the mark has faded out. */
    bool Update(float dt);
    /* Tell the target's character that it is locked on (AnnounceAttackCommand 1, 4) and released
     * (2, 4). */
    void NotifyLocked();
    void NotifyReleased();
    /* State 0: fades the mark in and shrinks the ring; false when the target is gone. */
    bool FadeIn(float dt);
    /* State 1: follows the target, and starts fading out once it is gone. */
    bool Follow();
    /* State 2: fades the mark out; false once it has gone. */
    bool FadeOut(float dt) {
        m_alpha -= 2.0f * dt;
        if (m_alpha <= 0.0f) {
            m_alpha = 0.0f;
            return false;
        }
        return true;
    }
    /* An unused out-of-line copy of FadeOut (see its definition). */
    bool FadeOutOutOfLine(float dt);
    /* Builds a quad of half size size around center as a triangle strip. */
    void MakeQuad(RwIm2DVertex *vertices, const Vec3f &center, float size, float alpha);
    /* The Render's draw callback. */
    void Draw(RednerDrawArgs &args);
    /* The point of target to aim at from m_from, or the origin when the target is gone. */
    Vec3f TargetPosition(const boost::weak_ptr<Target> &target);

    /* 0x00 */ boost::weak_ptr<Target> m_target;
    /* 0x08 */ Vec3f m_from; /* where the weapon aims from (LockonMark's 0x80061D10) */
    /* 0x14 */ Vec3f m_position;
    /* 0x20 */ float m_alpha;
    /* 0x24 */ float m_ringScale;
    /* 0x28 */ RwTexture *m_frameTexture;
    /* 0x2C */ RwTexture *m_ringTexture;
    /* 0x30 */ s32 m_state;
    /* 0x34 */ boost::shared_ptr<Render> m_render;
};
SIZE_ASSERT(LockonMarkImpl, 0x3C);

/* The mark task. It clears the slot it is given when it dies. */
class LockonMark : public Task {
  public:
    LockonMark(LockonMark **slot, LockonTarget *target);
    virtual ~LockonMark();
    virtual void Tick(float dt);

    /* Forgets the slot, and moves where the weapon aims from. */
    void fn_80061D04();
    void fn_80061D10(const Vec3f *position);

    /* 0x28 */ boost::scoped_ptr<LockonMarkImpl> m_impl;
    /* 0x2C */ LockonMark **m_slot;
};
SIZE_ASSERT(LockonMark, 0x30);

} // namespace Weapon

using Weapon::LockonMark;
using Weapon::LockonMarkImpl;

/* Deletes the Impl (see checked_delete below). */
inline void DeleteImpl(LockonMarkImpl *impl) {
    delete impl;
}

/* Retail keeps boost::checked_delete<LockonMarkImpl> out of line and the Impl's destructor at its own
 * place; this specialization gives that order under GC/2.7 (see docs/research-notes.md and
 * Game/EnergyCore.cpp). */
namespace boost {
template <>
inline void checked_delete<LockonMarkImpl>(LockonMarkImpl *x) {
    DeleteImpl(x);
}
} // namespace boost

/* A rectangle's right and bottom edges (other units call them by their addresses). */
extern "C" s32 fn_80063018(const ScreenRect *rect) {
    return rect->position.x + rect->size.x;
}

extern "C" s32 fn_80063008(const ScreenRect *rect) {
    return rect->position.y + rect->size.y;
}

namespace Weapon {

LockonMarkImpl::LockonMarkImpl(LockonTarget *target)
    : m_target(target->m_target), m_frameTexture(0), m_ringTexture(0) {
    m_alpha = 0.0f;
    m_state = 0;
    fn_800091D0(&m_from, &lbl_805E07A0);
    RwTexDictionary *textures = static_cast<RwTexDictionary *>(fn_8001277C(fn_800122D4(), 2, 0x56));
    if (textures != 0) {
        m_frameTexture = fn_8048CFFC(textures, "am_tgt0");
        m_ringTexture = fn_8048CFFC(textures, "am_tgt1");
    }
    NotifyLocked();
    m_render = fn_8004E24C(fn_80046178()->field_0x30);
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&LockonMarkImpl::Draw, this, _1));
}

LockonMarkImpl::~LockonMarkImpl() {
    NotifyReleased();
}

bool LockonMarkImpl::Update(float dt) {
    bool alive = true;
    switch (m_state) {
    case 0:
        alive = FadeIn(dt);
        break;
    case 1:
        alive = Follow();
        break;
    case 2:
        alive = FadeOut(dt);
        break;
    }
    return alive;
}

void LockonMarkImpl::NotifyLocked() {
    if (!m_target.expired()) {
        boost::shared_ptr<CharInfo> chara = m_target.lock()->m_chara.lock();
        if (chara.get() != 0) {
            chara->PushCommand(new AnnounceAttackCommand(boost::weak_ptr<CharInfo>(), 1, 4));
        }
    }
}

void LockonMarkImpl::NotifyReleased() {
    if (!m_target.expired()) {
        boost::shared_ptr<CharInfo> chara = m_target.lock()->m_chara.lock();
        if (chara.get() != 0) {
            chara->PushCommand(new AnnounceAttackCommand(boost::weak_ptr<CharInfo>(), 2, 4));
        }
    }
}

bool LockonMarkImpl::FadeIn(float dt) {
    if (m_target.expired()) {
        return false;
    }
    m_alpha += 10.0f * dt;
    m_ringScale = 1.0f - 0.25f * m_alpha;
    if (m_alpha >= 1.0f) {
        m_alpha = 1.0f;
        m_ringScale = 0.75f;
        m_state = 1;
    }
    fn_800091D0(&m_position, &(const Vec3f &)TargetPosition(m_target));
    return true;
}

/* Not called anywhere: the linker strips it from the game. The original pools FadeOut's float literal
 * between those of MakeQuad and FadeIn, which is what compiling an out-of-line function that inlines
 * it at this point of the file does (see Game/ScatteringWeapon.cpp and docs/research-notes.md). */
bool LockonMarkImpl::FadeOutOutOfLine(float dt) {
    return FadeOut(dt);
}

bool LockonMarkImpl::Follow() {
    if (m_target.expired()) {
        m_state = 2;
        return true;
    }
    fn_800091D0(&m_position, &(const Vec3f &)TargetPosition(m_target));
    return true;
}

void LockonMarkImpl::MakeQuad(RwIm2DVertex *vertices, const Vec3f &center, float size, float alpha) {
    vertices[0].x = center.x - size;
    vertices[0].y = center.y - size;
    vertices[1].x = center.x - size;
    vertices[1].y = center.y + size;
    vertices[2].x = center.x + size;
    vertices[2].y = center.y - size;
    vertices[3].x = center.x + size;
    vertices[3].y = center.y + size;
    vertices[0].z = center.z;
    vertices[1].z = center.z;
    vertices[2].z = center.z;
    vertices[3].z = center.z;
    float full = 255.0f;
    int white = full;
    vertices[0].red = white;
    vertices[0].green = white;
    vertices[0].blue = white;
    int opacity = full * alpha;
    vertices[0].alpha = opacity;
    vertices[1].red = white;
    vertices[1].green = white;
    vertices[1].blue = white;
    vertices[1].alpha = opacity;
    vertices[2].red = white;
    vertices[2].green = white;
    vertices[2].blue = white;
    vertices[2].alpha = opacity;
    vertices[3].red = white;
    vertices[3].green = white;
    vertices[3].blue = white;
    vertices[3].alpha = opacity;
    vertices[0].u = 0.0f;
    vertices[0].v = 0.0f;
    vertices[1].u = 0.0f;
    vertices[1].v = 1.0f;
    vertices[2].u = 1.0f;
    vertices[2].v = 0.0f;
    vertices[3].u = 1.0f;
    vertices[3].v = 1.0f;
}

} // namespace Weapon

/* The player whose view is being drawn and the viewport being drawn; the screen is not read (other
 * units call them by their addresses). */
extern "C" s32 fn_800623A8(UnkScreenHolder_8000DCC0 *screen) {
    return lbl_805F0170;
}

extern "C" s32 fn_800623A0(UnkScreenHolder_8000DCC0 *screen) {
    return lbl_805F0174;
}

namespace Weapon {

void LockonMarkImpl::Draw(RednerDrawArgs &args) {
    float size = 32.0f;
    float nearZ = lbl_805F15B4->zBufferNear;
    s32 player = fn_800623A8(fn_8000DCC0());
    Vec3f screen;
    fn_800091EC(&screen, &(const Vec3f &)fn_8000F654(&m_position, player));
    Vec3f position(screen.x, screen.y, nearZ);
    const ScreenRect *rect = fn_80272E94(fn_800623A0(fn_8000DCC0()));
    position.x -= fn_8000F850(rect);
    position.y -= fn_8000F858(rect);
    float min = -size;
    float maxY = size + (fn_80063008(rect) - fn_8000F858(rect));
    float maxX = size + (fn_80063018(rect) - fn_8000F850(rect));
    if (screen.z < 0.0f || position.x > maxX || position.x < min || position.y > maxY || position.y < min) {
        return;
    }
    void *zTest;
    void *zWrite;
    void *fog;
    void *srcBlend;
    void *destBlend;
    RwRenderStateGet(rwRENDERSTATEZTESTENABLE, &zTest);
    RwRenderStateGet(rwRENDERSTATEZWRITEENABLE, &zWrite);
    RwRenderStateGet(rwRENDERSTATEFOGENABLE, &fog);
    RwRenderStateGet(rwRENDERSTATESRCBLEND, &srcBlend);
    RwRenderStateGet(rwRENDERSTATEDESTBLEND, &destBlend);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, 0);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, 0);
    RwRenderStateSet(rwRENDERSTATEFOGENABLE, 0);
    RwRenderStateSet(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, rwBLENDONE);
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, 1);
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, rwFILTERNEAREST);
    RwIm2DVertex vertices[4];
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, m_frameTexture->raster);
    MakeQuad(vertices, position, size, m_alpha);
    RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, vertices, 4);
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, m_ringTexture->raster);
    MakeQuad(vertices, position, size * m_ringScale, m_alpha);
    RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, vertices, 4);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, zTest);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, zWrite);
    RwRenderStateSet(rwRENDERSTATEFOGENABLE, fog);
    RwRenderStateSet(rwRENDERSTATESRCBLEND, srcBlend);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, destBlend);
}

Vec3f LockonMarkImpl::TargetPosition(const boost::weak_ptr<Target> &target) {
    if (target.expired()) {
        return Vec3f(0.0f, 0.0f, 0.0f);
    }
    boost::shared_ptr<Target> locked = target.lock();
    return locked->GetPosition(m_from);
}

LockonMark::LockonMark(LockonMark **slot, LockonTarget *target)
    : Task(fn_801EB304(fn_8001C4F8(), 0xF)), m_impl(new LockonMarkImpl(target)), m_slot(slot) {
    m_name = "LockonMark";
    m_flags |= 0x100;
    if (m_slot != 0) {
        *m_slot = this;
    }
}

LockonMark::~LockonMark() {
    if (m_slot != 0) {
        *m_slot = 0;
    }
}

void LockonMark::Tick(float dt) {
    if (!m_impl->Update(dt)) {
        m_flags |= 1;
    }
}

void LockonMark::fn_80061D10(const Vec3f *position) {
    fn_800091D0(&m_impl->m_from, position);
}

void LockonMark::fn_80061D04() {
    m_slot = 0;
}

} // namespace Weapon
