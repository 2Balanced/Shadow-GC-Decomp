/* GiMissionTarget (class name from the RTTI of its shared_ptr control block,
 * sp_counted_base_impl<GiMissionTarget *, checked_deleter<GiMissionTarget>>, vtable 0x80557658):
 * the 3D marker over a mission target. The project knows the class as GiMissionTarget (other
 * units hold it through boost::shared_ptr<GiMissionTarget>, whose weak instances carry that
 * name), so this unit uses that name until the class is renamed project-wide; the control block's
 * RTTI strings in .rodata then read GiMissionTarget, as retail's do.
 *
 * The class hides its state in a GiMissionTargetImpl (class name from the RTTI of its draw
 * callback's bind_t): a clone of one of the two marker models ("common/mtargetr.dff" for kind 0,
 * "common/mtargetb.dff" for kind 2, loaded once by fn_802B4194 and released by fn_802B4124), drawn
 * through a Render while it is visible, the character being played is the marker's kind and the
 * camera is between 40 and 200 units from it; it faces the camera, turned upside down when asked,
 * and grows past 200 units. Other units call the C functions by their addresses. Member names are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first).
 *
 * Not matching yet (unit NonMatching, 43/44): the RTTI strings above need the class renamed, and
 * Draw keeps its zeroed copy of the camera position in a named local: retail's is a temporary at
 * the bottom of the frame whose initializer heads .rodata, as an inlined helper with an aggregate
 * initializer would give, and GC/2.7 does not inline such helpers (76 of 176 rows, all stack and
 * .rodata offsets). */

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/ClumpFade.h"
#include "Game/Fn8040CopyVector3.h"
#include "Game/MathTypes.h"
#include "Game/Render.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* RenderWare (rw/rwcore.h, rp/rpworld.h names; only the members used are laid out). */
struct RwV3d {
    float x;
    float y;
    float z;
};

struct RwMatrix {
    /* 0x00 */ RwV3d right;
    /* 0x0C */ u32 flags;
    /* 0x10 */ RwV3d up;
    /* 0x1C */ u32 pad1;
    /* 0x20 */ RwV3d at;
    /* 0x2C */ u32 pad2;
    /* 0x30 */ RwV3d pos;
    /* 0x3C */ u32 pad3;
};

struct RwFrame {
    /* 0x00 */ u8 object[0x10];
    /* 0x10 */ RwMatrix modelling;
};

struct RpClump {
    /* 0x0 */ u8 object[0x4];
    /* 0x4 */ RwFrame *frame;
};

struct RwStream;

/* The game state (struct name unknown; see Game/TGi.h). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x148];
    /* 0x148 */ u8 m_hudVisible; /* meaning inferred */
};

/* The player manager fn_8006D6A8 returns (see Game/HintRing.cpp). */
struct UnkPlayerManager_8006D6A8 {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ s32 m_character;
};

/* The camera manager fn_80009548 returns (opaque here). */
struct UnkCameraManager_80009548;
/* The game's screen (opaque here). */
class UnkScreenHolder_8000DCC0;

extern UnkGameState_8057D6A0 lbl_8057D6A0;
/* The player of the viewport being drawn (see Game/CameraScreenSet.cpp). */
extern s32 lbl_805F0170;

extern "C" {
UnkPlayerManager_8006D6A8 *fn_8006D6A8(void);
UnkCameraManager_80009548 *fn_80009548(void);
/* The position of player's camera. */
const Vec3f *fn_80010244(UnkCameraManager_80009548 *manager, s32 player);
/* The game's screen, and the screen position of a point as the camera of player sees it (see
 * Game/WeaponLockonMark.cpp). */
UnkScreenHolder_8000DCC0 *fn_8000DCC0(void);
Vec3f fn_8000F654(const Vec3f *position, s32 player);
/* Creates a Render (see Game/WeaponLockonMark.cpp). */
boost::shared_ptr<Render> fn_8004E24C(void *arg0);
/* The full path of a file name, and freeing it (behaviour inferred). */
char *fn_80051810(const char *name);
void fn_80051830(char *path);
/* RwStreamOpen, RwStreamFindChunk, RwStreamClose. */
RwStream *fn_80481368(s32 type, s32 access, const void *data);
s32 fn_8047CF50(RwStream *stream, u32 type, u32 *length, u32 *version);
s32 fn_8048124C(RwStream *stream, void *data);
/* RpClumpStreamRead, RpClumpClone, RpClumpDestroy, RpClumpGetWorld, RpWorldRemoveClump. */
RpClump *fn_80457698(RwStream *stream);
RpClump *fn_80457090(RpClump *clump);
void fn_80457468(RpClump *clump);
void *fn_8046256C(RpClump *clump);
void fn_804623EC(void *world, RpClump *clump);
/* Prepares a clump for fading, and applies a fade to it (see Game/ClumpFade.h). */
void fn_80038B18(RpClump *clump);
void fn_80038198(RpClump *clump, UnkClumpFade_80027D5C *fade);
/* RwV3dNormalize, RwMatrixScale, RwFrameUpdateObjects, RpClumpRender. */
float fn_8048222C(RwV3d *out, const RwV3d *in);
RwMatrix *fn_8047F398(RwMatrix *matrix, const RwV3d *scale, int combine);
RwFrame *fn_8048722C(RwFrame *frame);
RpClump *fn_80456BC4(RpClump *clump);
}

/* The player whose view is being drawn; the screen is not read (as Game/WeaponLockonMark.cpp's
 * fn_800623A8, written out here). */
inline s32 PlayerOf(UnkScreenHolder_8000DCC0 *screen) {
    return lbl_805F0170;
}

/* Three floats built from a zeroed initializer (the camera position is copied through one). */
struct GiMissionTargetFloat3 {
    float v[3];
};

/* The marker's state (class name from the RTTI of its draw callback's bind_t). */
class GiMissionTargetImpl {
  public:
    GiMissionTargetImpl(int kind, int inverted);
    ~GiMissionTargetImpl();
    /* The Render's draw callback. */
    void Draw(RednerDrawArgs &args);

    void SetNearDistance(float distance) {
        m_near = distance;
    }
    void SetFarDistance(float distance) {
        m_far = distance;
    }

    void SetPosition(const Vec3f &position) {
        m_position = reinterpret_cast<const Fn8040CopyVector3 &>(position);
    }
    u8 SetVisible(u8 visible) {
        u8 old = m_visible;
        m_visible = visible;
        return old;
    }
    u8 IsVisible() {
        return m_visible;
    }

    /* 0x00 */ RpClump *m_clump;
    /* 0x04 */ RwFrame *m_frame;
    /* 0x08 */ boost::shared_ptr<Render> m_render;
    /* 0x10 */ s32 m_inverted; /* the marker is drawn upside down */
    /* 0x14 */ u8 m_visible;
    /* 0x18 */ s32 m_kind; /* the character the marker is shown to */
    /* 0x1C */ float m_near;
    /* 0x20 */ float m_far;
    /* 0x24 */ Fn8040CopyVector3 m_position;
    /* 0x30 */ UnkClumpFade_80027D5C *m_fade;
};
SIZE_ASSERT(GiMissionTargetImpl, 0x34);

/* GiMissionTarget (see above). */
class GiMissionTarget {
  public:
    GiMissionTarget(int kind, int inverted);
    ~GiMissionTarget();

    /* 0x0 */ boost::scoped_ptr<GiMissionTargetImpl> m_impl;
};
SIZE_ASSERT(GiMissionTarget, 0x4);

/* Deletes the Impl and the target (see checked_delete below). */
inline void DeleteImpl(GiMissionTargetImpl *impl) {
    delete impl;
}
inline void DeleteTarget(GiMissionTarget *target) {
    delete target;
}

/* Retail keeps boost::checked_delete out of line and the destructors at their own places; these
 * specializations give that order under GC/2.7 (see docs/research-notes.md and
 * Game/EnergyCore.cpp). */
namespace boost {
template <>
inline void checked_delete<GiMissionTargetImpl>(GiMissionTargetImpl *x) {
    DeleteImpl(x);
}
template <>
inline void checked_delete<GiMissionTarget>(GiMissionTarget *x) {
    DeleteTarget(x);
}
} // namespace boost

/* The two marker models. */
static RpClump *s_models[2] = {0, 0};

/* Sets two and three floats to value (names inferred). */
inline void Fill2(float *values, float value) {
    for (int i = 0; i < 2; i++) {
        values[i] = value;
    }
}

inline void Fill3(float *values, float value) {
    values[0] = value;
    values[1] = value;
    values[2] = value;
}

/* Sets the fade's other channels to 1 (name inferred). It calls inline functions, so GC/2.7 keeps
 * it out of line, as retail has it. */
inline void ResetFade(UnkClumpFade_80027D5C *fade) {
    Fill2(fade->field_0x10, 1.0f);
    Fill3(fade->field_0x18, 1.0f);
}

GiMissionTargetImpl::GiMissionTargetImpl(int kind, int inverted)
    : m_render(), m_inverted(inverted), m_kind(kind) {
    m_visible = 0;
    m_near = 40.0f;
    m_far = 200.0f;



    m_render = fn_8004E24C(fn_80046178()->field_0x30);
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&GiMissionTargetImpl::Draw, this, _1));
    m_clump = 0;
    m_frame = 0;
    m_fade = 0;
    RpClump *model;
    switch (kind) {
    case 1:
        break;
    case 2:
        model = s_models[1];
        break;
    case 0:
        model = s_models[0];
        break;
    }
    if (model != 0) {
        m_clump = fn_80457090(model);
        if (m_clump != 0) {
            m_frame = m_clump->frame;
            fn_80038B18(m_clump);
            UnkClumpFade_80027D5C *fade =
                static_cast<UnkClumpFade_80027D5C *>(operator new(sizeof(UnkClumpFade_80027D5C)));
            if (fade != 0) {
                fade = fn_80027D5C(fade);
            }
            m_fade = fade;
            if (m_fade != 0) {
                m_fade->m_color[3] = 0.8f;
                ResetFade(m_fade);
                fn_80038198(m_clump, m_fade);
            }
        }
    }
}

GiMissionTargetImpl::~GiMissionTargetImpl() {
    if (m_clump != 0) {
        void *world = fn_8046256C(m_clump);
        if (world != 0) {
            fn_804623EC(world, m_clump);
        }
        fn_80457468(m_clump);
        m_frame = 0;
        m_clump = 0;
        if (m_fade != 0) {
            delete m_fade;
        }
    }
}

void GiMissionTargetImpl::Draw(RednerDrawArgs &args) {
    if (lbl_8057D6A0.m_hudVisible == 0) {
        return;
    }
    if (!m_visible) {
        return;
    }
    if (fn_8006D6A8()->m_character != m_kind) {
        return;
    }
    if (m_clump == 0) {
        return;
    }
    if (m_frame == 0) {
        return;
    }
    Vec3f screen;
    Vec3f camera;
    RwV3d eye;
    s32 player;
    RwMatrix *matrix = &m_frame->modelling;
    matrix->pos.x = m_position.x;
    matrix->pos.y = m_position.y;
    matrix->pos.z = m_position.z;
    fn_800091EC(&screen, &(const Vec3f &)fn_8000F654(reinterpret_cast<const Vec3f *>(&m_position),
                                                     PlayerOf(fn_8000DCC0())));
    if (screen.z < m_near) {
        return;
    }
    if (screen.z > m_far) {
        return;
    }
    player = lbl_805F0170;
    fn_800091EC(&camera, fn_80010244(fn_80009548(), player));
    {
        GiMissionTargetFloat3 c = {0.0f, 0.0f, 0.0f};
        c.v[0] = camera.x;
        c.v[1] = camera.y;
        c.v[2] = camera.z;
        *reinterpret_cast<GiMissionTargetFloat3 *>(&eye) = c;
    }
    matrix->at.x = eye.x - matrix->pos.x;
    matrix->at.y = eye.y - matrix->pos.y;
    matrix->at.z = eye.z - matrix->pos.z;
    fn_8048222C(&matrix->at, &matrix->at);
    RwV3d up = {0.0f, 0.0f, 0.0f};
    up.y = m_inverted == 0 ? 1.0f : -1.0f;
    matrix->right.x = up.y * matrix->at.z - up.z * matrix->at.y;
    matrix->right.y = up.z * matrix->at.x - up.x * matrix->at.z;
    matrix->right.z = up.x * matrix->at.y - up.y * matrix->at.x;
    fn_8048222C(&matrix->right, &matrix->right);
    matrix->up.x = matrix->at.y * matrix->right.z - matrix->at.z * matrix->right.y;
    matrix->up.y = matrix->at.z * matrix->right.x - matrix->at.x * matrix->right.z;
    matrix->up.z = matrix->at.x * matrix->right.y - matrix->at.y * matrix->right.x;
    RwV3d scale = {6.0f, 6.0f, 6.0f};
    if (screen.z > 200.0f) {
        float grow = screen.z / 200.0f;
        scale.x *= grow;
        scale.y *= grow;
        scale.z *= grow;
    }
    fn_8047F398(matrix, &scale, 1);
    fn_8048722C(m_frame);
    fn_80456BC4(m_clump);
}

GiMissionTarget::GiMissionTarget(int kind, int inverted) : m_impl(new GiMissionTargetImpl(kind, inverted)) {}

GiMissionTarget::~GiMissionTarget() {}

extern "C" {

u8 fn_802B4548(GiMissionTarget *target) {
    return target->m_impl->IsVisible();
}

u8 fn_802B4510(GiMissionTarget *target, u8 visible) {
    return target->m_impl->SetVisible(visible);
}

void fn_802B44C8(GiMissionTarget *target, const Vec3f *position) {
    target->m_impl->SetPosition(*position);
}

void fn_802B4498(GiMissionTarget *target, float distance) {
    target->m_impl->SetFarDistance(distance);
}

/* Not called anywhere: the linker strips it from the game. Retail emits the impl pointer's
 * operator-> ahead of fn_802B4498, which is where it lands when an unused caller is defined after
 * it (name inferred). */
void GiMissionTargetSetNear(GiMissionTarget *target, float distance) {
    target->m_impl->SetNearDistance(distance);
}

/* Makes a marker of kind (0 or 2; any other kind makes none). */
boost::shared_ptr<GiMissionTarget> fn_802B4254(int kind, int inverted) {
    if (kind == 2 || kind == 0) {
        boost::shared_ptr<GiMissionTarget> target(new GiMissionTarget(kind, inverted));
        return target;
    }
    return boost::shared_ptr<GiMissionTarget>();
}

/* Loads the model of kind (0 or 2). */
void fn_802B4194(int kind) {
    static const char *s_fileNames[2] = {"common/mtargetr.dff", "common/mtargetb.dff"};
    int index;
    if (kind == 0) {
        index = 0;
    } else if (kind == 2) {
        index = 1;
    } else {
        return;
    }
    char *path = fn_80051810(s_fileNames[index]);
    RwStream *stream = fn_80481368(2, 1, path);
    fn_80051830(path);
    if (stream != 0) {
        if (fn_8047CF50(stream, 0x10, 0, 0) != 0) {
            s_models[index] = fn_80457698(stream);
        }
        fn_8048124C(stream, 0);
    }
}

/* Releases both models. */
void fn_802B4124(void) {
    for (int i = 0; i < 2; i++) {
        if (s_models[i] != 0) {
            void *world = fn_8046256C(s_models[i]);
            if (world != 0) {
                fn_804623EC(world, s_models[i]);
            }
            fn_80457468(s_models[i]);
            s_models[i] = 0;
        }
    }
}

}
