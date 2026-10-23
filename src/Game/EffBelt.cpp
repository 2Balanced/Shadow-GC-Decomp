/* Test::EffBelt (class name from RTTI; declared in Game/EffBelt.h): a ribbon effect. Its state
 * (Impl, class name unknown) keeps the last positions in a ring buffer; each position carries the
 * direction across the ribbon and a rotation about the direction of travel, and the ribbon is drawn
 * as triangle strips between rings of 11 points with RenderWare's immediate mode. The ribbon's
 * alpha falls off along its length and fades out after Finish. File name inferred from the class.
 * Names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
/* boost::scoped_array needs it (see boost/checked_delete.hpp). */
#define BOOST_CHECKED_ARRAY_DELETE

#include "Game/DebugDisp.h"
#include "Game/EffBelt.h"
#include "Game/EffLocusMultiVtx.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/Quaternion.h"
#include "Game/Render.h"
#include "Game/Task.h"
#include "boost/scoped_array.hpp"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* RenderWare's immediate-mode vertex and render state calls (rw/rwcore.h; as in
 * Game/ElecSearchLight.cpp). */
typedef struct RwV3d {
    float x;
    float y;
    float z;
} RwV3d;

typedef struct RwIm3DVertex {
    /* 0x00 */ RwV3d objVertex;
    /* 0x0C */ RwV3d objNormal;
    /* 0x18 */ u8 color[4];
    /* 0x1C */ float u;
    /* 0x20 */ float v;
} RwIm3DVertex;

typedef int (*RwRenderStateSetFunction)(int state, void *value);
typedef int (*RwRenderStateGetFunction)(int state, void *value);

typedef struct RwGlobals {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ RwRenderStateSetFunction fpRenderStateSet;
    /* 0x24 */ RwRenderStateGetFunction fpRenderStateGet;
} RwGlobals;

extern "C" {
/* the engine instance */
extern RwGlobals *lbl_805F15B4;
/* RwIm3DTransform, RwIm3DRenderPrimitive and RwIm3DEnd. */
void *fn_8049A82C(RwIm3DVertex *vertices, u32 count, void *ltm, u32 flags);
int fn_8049AAF8(int prim);
int fn_8049A90C(void);
}

enum {
    rwRENDERSTATETEXTURERASTER = 1,
    rwRENDERSTATESRCBLEND = 10,
    rwRENDERSTATEDESTBLEND = 11,
    rwRENDERSTATEVERTEXALPHAENABLE = 12,
    rwRENDERSTATEFOGENABLE = 14,
    rwRENDERSTATECULLMODE = 20,
    rwCULLMODECULLNONE = 1,
    rwBLENDONE = 2,
    rwBLENDSRCALPHA = 5,
    rwPRIMTYPETRISTRIP = 4
};

/* The vertex setters (rw/rwcore.h; inline functions here, which evaluate their arguments before
 * storing, as retail does). */
inline void RwIm3DVertexSetRGBA(RwIm3DVertex *vert, u8 r, u8 g, u8 b, u8 a) {
    vert->color[0] = r;
    vert->color[1] = g;
    vert->color[2] = b;
    vert->color[3] = a;
}

inline void RwIm3DVertexSetPos(RwIm3DVertex *vert, float x, float y, float z) {
    vert->objVertex.x = x;
    vert->objVertex.y = y;
    vert->objVertex.z = z;
}

#define RwRenderStateGet(state, value) (lbl_805F15B4->fpRenderStateGet((state), (void *)(value)))
#define RwRenderStateSet(state, value) (lbl_805F15B4->fpRenderStateSet((state), (void *)(value)))

/* Set while the game is paused (as in Game/CameraManagerUnit.cpp; meaning inferred). */
extern u8 lbl_805EE1DC;

typedef Vec3f (*VectorOpFn)(const Vec3f *a, const Vec3f *b);

extern "C" {
/* The matrix of a rotation, matrix * point (see Game/GadgetUtil.h), a rotation by angle about
 * axis, a cross product, a unit vector, vector * scale (behaviour inferred). */
Mtx34f fn_8040B228(const Quatf *rotation);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Quatf fn_8040C874(const Vec3f *axis, float angle);
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CEE8(const Vec3f *vector);
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
/* The out-of-line assignment of a rotation (behaviour from its body). */
void fn_8007A34C(Quatf *rotation, const Quatf *value);
/* color -= value (behaviour inferred). */
void fn_8040D254(UnkColor_8040D2EC *color, const UnkColor_8040D2D8 &value);
}

/* color -= value (name inferred). */
inline void SubtractColor(UnkColor_8040D2EC &color, const UnkColor_8040D2D8 &value) {
    fn_8040D254(&color, value);
}

/* Copies value into vector through the game's out-of-line assignment (as in Game/GadgetUtil.h). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* vector * scale (name inferred; as in Game/WeaponSpecialVacuumEgg.cpp). */
inline Vec3f Scale(const Vec3f &vector, float scale) {
    return fn_8040CDA8(&vector, scale);
}

namespace Test {

/* A position of the ribbon (struct name inferred). */
struct BeltPoint {
    BeltPoint() {}

    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ Vec3f m_side;     /* from the position to the ring's first point */
    /* 0x18 */ Quatf m_rotation; /* turns m_side to the ring's next point */
    /* 0x28 */ float m_alpha;
};
SIZE_ASSERT(BeltPoint, 0x2C);

/* The last positions, newest first (class name inferred). */
class BeltRing {
  public:
    BeltRing(int capacity) : m_head(0), m_count(0), m_capacity(capacity) {
        m_points.reset(new BeltPoint[capacity]);
    }

    /* The index-th newest position, or 0. */
    BeltPoint *Get(int index) {
        if (index >= m_count) {
            return 0;
        }
        index = m_head - index;
        if (index < 0) {
            index += m_capacity;
        }
        return &m_points[index];
    }

    void Push(const Vec3f &position, const Vec3f &side, const Quatf &rotation) {
        m_head++;
        if (m_head >= m_capacity) {
            m_head = 0;
        }
        m_count++;
        if (m_count >= m_capacity) {
            m_count = m_capacity - 1;
        }
        BeltPoint &point = m_points[m_head];
        fn_800091D0(&point.m_position, &position);
        fn_800091D0(&point.m_side, &side);
        fn_8007A34C(&point.m_rotation, &rotation);
    }

    /* 0x0 */ boost::scoped_array<BeltPoint> m_points;
    /* 0x4 */ s32 m_head;
    /* 0x8 */ s32 m_count;
    /* 0xC */ s32 m_capacity;
};
SIZE_ASSERT(BeltRing, 0x10);

class EffBelt::Impl {
  public:
    Impl(int count, const UnkColor_8040D2EC &color, float width);
    Impl(const boost::weak_ptr<Player::PlayerBase> &player, int count,
         const UnkColor_8040D2EC &color, float width);
    ~Impl();

    /* Names inferred. */
    void Add(const Vec3f &position);
    bool Update();
    void Draw();

    void Finish() {
        m_finished = true;
    }

    /* 0x00 */ boost::weak_ptr<Player::PlayerBase> m_player;
    /* 0x08 */ UnkColor_8040D2EC m_color; /* m_a is the ribbon's alpha */
    /* 0x0C */ float m_width;
    /* 0x10 */ BeltRing m_ring;
    /* 0x20 */ bool m_finished;
    /* 0x21 */ bool m_visible;
};
SIZE_ASSERT(EffBelt::Impl, 0x24);

/* Deletes the ribbon (name inferred; see checked_delete below). */
inline void DeleteImpl(EffBelt::Impl *impl) {
    delete impl;
}

} // namespace Test

/* Retail keeps boost::checked_delete<Impl> out of line (0x802DDDF8) and ~Impl at its own place;
 * this specialization calls an inline helper, which gives retail's code and order (as in
 * Game/DiablonExecuter.cpp; see docs/research-notes.md). */
namespace boost {
template <> inline void checked_delete<Test::EffBelt::Impl>(Test::EffBelt::Impl *x) {
    Test::DeleteImpl(x);
}
} // namespace boost

namespace Test {

EffBelt::Impl::Impl(const boost::weak_ptr<Player::PlayerBase> &player, int count,
                    const UnkColor_8040D2EC &color, float width)
    : m_player(player), m_color(color), m_width(width), m_ring(count) {
    m_finished = false;
    m_visible = true;
}

EffBelt::Impl::Impl(int count, const UnkColor_8040D2EC &color, float width)
    : m_color(color), m_width(width), m_ring(count) {
    m_finished = false;
    m_visible = true;
}

EffBelt::Impl::~Impl() {}

/* NonMatching (92%): retail reads m_width before normalizing and keeps it in f31 across the
 * call; GC/2.7 reads it after the call for every shape tried (a local, inline Scale/Normalize
 * helpers). */
void EffBelt::Impl::Add(const Vec3f &position) {
    Vec3f direction;
    BeltPoint *last = m_ring.Get(0);
    if (last != 0) {
        AssignVector(direction, fn_8040CEE8(&(const Vec3f &)((VectorOpFn)fn_8040CD74)(
                                    &last->m_position, &position)));
    } else {
        fn_800091D0(&direction, &(const Vec3f &)Vec3f(0.0f, 0.0f, -1.0f));
    }
    Quatf rotation(fn_8040C874(&direction, 36.0f * (lbl_805FAD38 / 180.0f)));
    Vec3f side(0.0f, 1.0f, 0.0f);
    Vec3f across;
    AssignVector(across, fn_8040CFB8(&side, &direction));
    AssignVector(side, fn_8040CFB8(&direction, &across));
    float width = m_width;
    AssignVector(side, Scale(fn_8040CEE8(&side), width));
    m_ring.Push(position, side, rotation);
}

/* NonMatching (99%): retail gives count and the loop index each other's registers. */
bool EffBelt::Impl::Update() {
    if (!m_player.expired()) {
        boost::shared_ptr<Player::PlayerBase> player = m_player.lock();
        m_visible = !player->IsWeaponHidden();
    }
    if (m_finished) {
        SubtractColor(m_color, UnkColor_8040D2D8(8, 0, 0, 0));
    }
    int alpha = m_color.m_a;
    if (m_color.m_a == 0) {
        return false;
    }
    s32 count = m_ring.m_count;
    if (count == 0) {
        return true;
    }
    BeltPoint *point = m_ring.Get(0);
    point->m_alpha = 0.0f;
    int i;
    for (i = 1; i < count; i++) {
        point = m_ring.Get(i);
        point->m_alpha = (int)alpha;
    }
    int fade = 0;
    for (i = count - 1; i >= count - 9; i--) {
        point = m_ring.Get(i);
        point->m_alpha = fade;
        fade += 0x20;
        if (fade > m_color.m_a) {
            break;
        }
    }
    return true;
}

/* NonMatching (98%): retail loads the colour bytes and the position components in a slightly
 * different order (left to right); the vertex setters and locals tried get most of it. */
void EffBelt::Impl::Draw() {
    RwIm3DVertex vertices[22];
    u32 srcBlend;
    u32 destBlend;
    u32 cullMode;
    u32 vertexAlpha;
    u32 fog;
    BeltPoint *newest = m_ring.Get(0);
    BeltPoint *point;
    int index = 1;
    point = m_ring.Get(index);
    if (m_visible && point != 0) {
        RwRenderStateGet(rwRENDERSTATESRCBLEND, &srcBlend);
        RwRenderStateGet(rwRENDERSTATEDESTBLEND, &destBlend);
        RwRenderStateGet(rwRENDERSTATECULLMODE, &cullMode);
        RwRenderStateGet(rwRENDERSTATEVERTEXALPHAENABLE, &vertexAlpha);
        RwRenderStateGet(rwRENDERSTATEFOGENABLE, &fog);
        RwRenderStateSet(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, rwBLENDONE);
        RwRenderStateSet(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, 0);
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, 1);
        RwRenderStateSet(rwRENDERSTATEFOGENABLE, 0);
        {
            Mtx34f turn;
            fn_8040C148(&turn, &(const Mtx34f &)fn_8040B228(&point->m_rotation));
            Vec3f side;
            fn_800091EC(&side, &point->m_side);
            for (int i = 0; i <= 10; i++) {
                RwIm3DVertex *vertex = &vertices[i * 2];
                RwIm3DVertexSetRGBA(vertex, m_color.m_r, m_color.m_g, m_color.m_b,
                                    (int)newest->m_alpha);
                float x = newest->m_position.x + side.x;
                float y = newest->m_position.y + side.y;
                float z = newest->m_position.z + side.z;
                vertex->objVertex.x = x;
                vertex->objVertex.y = y;
                vertex->objVertex.z = z;
                AssignVector(side, fn_8040B67C(&turn, &side));
            }
        }
        u32 parity = 0;
        while (point != 0) {
            Mtx34f turn;
            fn_8040C148(&turn, &(const Mtx34f &)fn_8040B228(&point->m_rotation));
            Vec3f side;
            fn_800091EC(&side, &point->m_side);
            parity ^= 1;
            for (int i = 0; i <= 10; i++) {
                RwIm3DVertex *vertex = &vertices[parity + i * 2];
                RwIm3DVertexSetRGBA(vertex, m_color.m_r, m_color.m_g, m_color.m_b,
                                    (int)point->m_alpha);
                float x = point->m_position.x + side.x;
                float y = point->m_position.y + side.y;
                float z = point->m_position.z + side.z;
                vertex->objVertex.x = x;
                vertex->objVertex.y = y;
                vertex->objVertex.z = z;
                AssignVector(side, fn_8040B67C(&turn, &side));
            }
            if (fn_8049A82C(vertices, 22, 0, 0x18) != 0) {
                fn_8049AAF8(rwPRIMTYPETRISTRIP);
            }
            point = m_ring.Get(++index);
        }
        fn_8049A90C();
        RwRenderStateSet(rwRENDERSTATEFOGENABLE, fog);
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, vertexAlpha);
        RwRenderStateSet(rwRENDERSTATECULLMODE, cullMode);
        RwRenderStateSet(rwRENDERSTATESRCBLEND, srcBlend);
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, destBlend);
    }
}

EffBelt::EffBelt(const boost::weak_ptr<Player::PlayerBase> &player, int count,
                 const UnkColor_8040D2EC &color, float width)
    : Task(fn_80046178()->field_0x20), m_impl(new Impl(player, count, color, width)) {
    m_name = "Eff::Belt";
    m_flags |= 0x100;
}

EffBelt::EffBelt(int count, const UnkColor_8040D2EC &color, float width)
    : Task(fn_80046178()->field_0x20), m_impl(new Impl(count, color, width)) {
    m_name = "Eff::Belt";
    m_flags |= 0x100;
}

EffBelt::~EffBelt() {}

void EffBelt::Tick(float dt) {
    if (lbl_805EE1DC == 0) {
        if (!m_impl->Update()) {
            m_flags |= 1;
        }
    }
    m_impl->Draw();
}

void EffBelt::AddPosition(const Vec3f &position) {
    m_impl->Add(position);
}

void EffBelt::Finish() {
    m_impl->Finish();
}

} // namespace Test
