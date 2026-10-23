/* Test::EffLocusMultiVtx (class name from RTTI; declared in Game/EffLocusMultiVtx.h): a trail
 * through a set of points its owner moves (the weapons' swing trails). Its state (Impl, class name
 * unknown) records the points every frame in a history (LocusHistory, name inferred), optionally
 * taken through a matrix first, and draws one triangle strip between each pair of neighbouring
 * points' histories with RenderWare's immediate mode; the alpha falls off along the history. Once
 * the owner lets the points go, the history shrinks by one each frame until the task ends. File
 * name inferred from the class. Names marked "(name inferred)" are not original. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
/* boost::scoped_array needs it (see boost/checked_delete.hpp). */
#define BOOST_CHECKED_ARRAY_DELETE

#include "Game/EffLocusMultiVtx.h"
#include "Game/MathTypes.h"
#include "Game/Render.h"
#include "Game/Task.h"
#include "boost/scoped_array.hpp"
#include "boost/scoped_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* RenderWare's immediate-mode vertex and render state calls (rw/rwcore.h; see EffBelt.cpp). */
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
inline void RwIm3DVertexSetPos(RwIm3DVertex *vert, float x, float y, float z) {
    vert->objVertex.x = x;
    vert->objVertex.y = y;
    vert->objVertex.z = z;
}

inline void RwIm3DVertexSetRGBA(RwIm3DVertex *vert, u8 r, u8 g, u8 b, u8 a) {
    vert->color[0] = r;
    vert->color[1] = g;
    vert->color[2] = b;
    vert->color[3] = a;
}

#define RwRenderStateGet(state, value) (lbl_805F15B4->fpRenderStateGet((state), (void *)(value)))
#define RwRenderStateSet(state, value) (lbl_805F15B4->fpRenderStateSet((state), (void *)(value)))

/* Set while the game is paused (as in Game/CameraManagerUnit.cpp; meaning inferred). */
extern u8 lbl_805EE1DC;

struct UnkShadow_8009368C;

extern "C" {
/* matrix * point (see Game/GadgetUtil.h). */
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
/* Sets a shadow's size (see Game/BossCtrlUnit.cpp; behaviour inferred). */
void fn_803563D0(float size, UnkShadow_8009368C *shadow);
}

namespace Test {

/* The positions of the points over the last frames, newest first, and the alpha of each age and the
 * vertices of a strip (class name inferred). */
class LocusHistory {
  public:
    LocusHistory(int length, int pointCount)
        : m_head(0), m_count(0), m_length(length), m_pointCount(pointCount) {
        int total = length * m_pointCount;
        m_positions.reset(new Vec3f[total]);
        m_alphas.reset(new float[length]);
        m_vertices.reset(new RwIm3DVertex[length * 2]);
    }

    /* Where point was age frames ago, or 0. */
    Vec3f *Get(int point, int age) {
        if (age >= m_count) {
            return 0;
        }
        age = m_head - age;
        if (age < 0) {
            age += m_length;
        }
        return &m_positions[age + point * m_length];
    }

    void Push(const Vec3f *points) {
        m_head++;
        if (m_head >= m_length) {
            m_head = 0;
        }
        m_count++;
        if (m_count >= m_length) {
            m_count = m_length - 1;
        }
        for (int i = 0; i < m_pointCount; i++) {
            fn_800091D0(&m_positions[m_head + i * m_length], &points[i]);
        }
    }

    float &Alpha(int age) {
        return m_alphas[age];
    }

    RwIm3DVertex *Vertices() {
        return m_vertices.get();
    }

    /* 0x00 */ boost::scoped_array<Vec3f> m_positions;
    /* 0x04 */ boost::scoped_array<float> m_alphas;
    /* 0x08 */ boost::scoped_array<RwIm3DVertex> m_vertices;
    /* 0x0C */ s32 m_head;
    /* 0x10 */ s32 m_count;
    /* 0x14 */ s32 m_length;
    /* 0x18 */ s32 m_pointCount;
};
SIZE_ASSERT(LocusHistory, 0x1C);

class EffLocusMultiVtx::Impl {
  public:
    Impl(int length, int count, Vec3f *points, const Mtx34f *matrix,
         const UnkColor_8040D2EC *color);
    ~Impl();

    /* Names inferred. */
    bool Update();
    void Draw();

    void Release() {
        m_points = 0;
    }

    /* 0x00 */ boost::scoped_array<Vec3f> m_transformed; /* the points through m_matrix */
    /* 0x04 */ Vec3f *m_points;
    /* 0x08 */ const Mtx34f *m_matrix;
    /* 0x0C */ UnkColor_8040D2EC m_color;
    /* 0x10 */ LocusHistory m_history;
    /* 0x2C */ u8 m_hidden;
};
SIZE_ASSERT(EffLocusMultiVtx::Impl, 0x30);

/* Deletes the trail (name inferred; see checked_delete below). */
inline void DeleteImpl(EffLocusMultiVtx::Impl *impl) {
    delete impl;
}

} // namespace Test

/* Retail keeps boost::checked_delete<Impl> out of line (0x802DEB1C) and ~Impl at its own place;
 * this specialization calls an inline helper, which gives retail's code and order (as in
 * Game/DiablonExecuter.cpp; see docs/research-notes.md). */
namespace boost {
template <>
inline void checked_delete<Test::EffLocusMultiVtx::Impl>(Test::EffLocusMultiVtx::Impl *x) {
    Test::DeleteImpl(x);
}
} // namespace boost

namespace Test {

EffLocusMultiVtx::Impl::Impl(int length, int count, Vec3f *points, const Mtx34f *matrix,
                             const UnkColor_8040D2EC *color)
    : m_points(points), m_matrix(matrix), m_color(*color), m_history(length, count) {
    m_transformed.reset(new Vec3f[count]);
    m_hidden = 0;
}

EffLocusMultiVtx::Impl::~Impl() {}

/* NonMatching (99.8%): retail gives pointCount and the destination of the first loop each other's
 * registers. */
bool EffLocusMultiVtx::Impl::Update() {
    if (m_points != 0) {
        int pointCount = m_history.m_pointCount;
        if (m_matrix != 0) {
            for (int i = 0; i < pointCount; i++) {
                fn_800091D0(&m_transformed[i],
                            &(const Vec3f &)fn_8040B67C(m_matrix, &m_points[i]));
            }
            m_history.Push(m_transformed.get());
        } else {
            m_history.Push(m_points);
        }
    } else {
        if (m_history.m_count != 0) {
            m_history.m_count--;
        }
        if (m_history.m_count == 0) {
            return false;
        }
    }
    int i;
    int count = m_history.m_count;
    for (i = 0; i < count; i++) {
        float &alpha = m_history.Alpha(i);
        alpha = (float)(count - i - 1) / (float)(count - 1);
    }
    return true;
}

/* NonMatching (99.7%): retail colours count, vertices, pointCount and the vertex count with other
 * registers and loads the colour's green byte before the blue one; colour locals fix the load order
 * but move every register. */
void EffLocusMultiVtx::Impl::Draw() {
    u32 srcBlend;
    u32 destBlend;
    u32 cullMode;
    u32 vertexAlpha;
    u32 fog;
    if (!m_hidden && m_history.m_count >= 2) {
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
        RwIm3DVertex *vertices = m_history.Vertices();
        int count = m_history.m_count;
        int pointCount = m_history.m_pointCount;
        int vertexCount = count * 2;
        int parity = 0;
        for (int i = 0; i < pointCount; i++) {
            for (int j = 0; j < count; j++) {
                Vec3f *position = m_history.Get(i, j);
                float &alpha = m_history.Alpha(j);
                RwIm3DVertex *vertex = &vertices[parity + j * 2];
                float x = position->x;
                float y = position->y;
                float z = position->z;
                vertex->objVertex.x = x;
                vertex->objVertex.y = y;
                vertex->objVertex.z = z;
                RwIm3DVertexSetRGBA(vertex, m_color.m_r, m_color.m_g, m_color.m_b,
                                    (int)(m_color.m_a * alpha));
            }
            if (i > 0) {
                if (fn_8049A82C(vertices, vertexCount, 0, 0x18) != 0) {
                    fn_8049AAF8(rwPRIMTYPETRISTRIP);
                }
            }
            parity = parity == 0 ? 1 : 0;
        }
        fn_8049A90C();
        RwRenderStateSet(rwRENDERSTATEFOGENABLE, fog);
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, vertexAlpha);
        RwRenderStateSet(rwRENDERSTATECULLMODE, cullMode);
        RwRenderStateSet(rwRENDERSTATESRCBLEND, srcBlend);
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, destBlend);
    }
}

EffLocusMultiVtx::EffLocusMultiVtx(int length, int count, Vec3f *points, int arg,
                                   const UnkColor_8040D2EC *color)
    : Task(fn_80046178()->field_0x20),
      m_impl(new Impl(length, count, points, (const Mtx34f *)arg, color)) {
    m_name = "Eff::LocusMultiVtx";
    m_flags |= 0x100;
}

EffLocusMultiVtx::~EffLocusMultiVtx() {}

void EffLocusMultiVtx::Tick(float dt) {
    if (lbl_805EE1DC == 0) {
        if (!m_impl->Update()) {
            m_flags |= 1;
        }
    }
    m_impl->Draw();
}

void EffLocusMultiVtx::fn_802DE9DC() {
    m_impl->Release();
}

void EffLocusMultiVtx::fn_802DE9CC() {
    m_impl->m_hidden = 1;
}

void EffLocusMultiVtx::fn_802DE9BC() {
    m_impl->m_hidden = 0;
}

} // namespace Test

/* Sets a shadow's size (name of the function unknown; Game/BossCtrlUnit.cpp calls it). */
extern "C" void fn_802DE99C(float size, UnkShadow_8009368C *shadow) {
    fn_803563D0(size, shadow);
}
