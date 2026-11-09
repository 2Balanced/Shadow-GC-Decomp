/* Effect::RW3::EffectRayRW3 (class name and bases from RTTI): the RenderWare version of the ray
 * effect. Each ray (Effect::System::EffectRayExecUnit) becomes a quad of four immediate-mode
 * vertices, two at each end, spread sideways to the camera; Draw renders them as triangle strips.
 * Member names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#define EFFECT_ELEMENT_POLYMORPHIC
#define EFFECT_ELEMENT_FULL

#include "Game/EffectManagerPJS.h"
#include "Game/MathTypes.h"

using ::Effect::System::EffectLaunchInfo;
using ::Effect::System::EffectManager;
using ::Effect::System::EffectRayExecUnit;
using ::Effect::System::UnkEffectDesc_801D0AFC;

/* RenderWare's immediate-mode vertex and engine globals (rw/rwcore.h; as in Game/EffBelt.cpp). */
struct RwV3d_8042C820 {
    float x;
    float y;
    float z;
};

namespace Effect {
namespace RW3 {

/* RwIm3DVertex. */
struct UnkRayVertexRW_8042C820 {
    /* 0x00 */ RwV3d_8042C820 objVertex;
    /* 0x0C */ RwV3d_8042C820 objNormal;
    /* 0x18 */ u8 color[4];
    /* 0x1C */ float u;
    /* 0x20 */ float v;
};

/* The four vertices of one ray. */
struct UnkRayVertex_8042C820 {
    UnkRayVertexRW_8042C820 v[4];
};

} // namespace RW3
} // namespace Effect

typedef int (*RwRenderStateFunction_8042C820)(int state, void *value);

struct RwGlobals_8042C820 {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ RwRenderStateFunction_8042C820 fpRenderStateSet;
    /* 0x24 */ RwRenderStateFunction_8042C820 fpRenderStateGet;
};

/* The parts of a camera and its frame Draw reads: the frame's position. */
struct UnkRwFrame_8042C820 {
    /* 0x00 */ u8 field_0x0[0x40];
    /* 0x40 */ RwV3d_8042C820 pos;
};

struct UnkRwCamera_8042C820 {
    /* 0x00 */ u8 field_0x0[4];
    /* 0x04 */ UnkRwFrame_8042C820 *parent;
};

extern "C" {
/* The engine instance. */
extern RwGlobals_8042C820 *lbl_805F15B4;
/* RwIm3DTransform, RwIm3DRenderPrimitive and RwIm3DEnd. */
void *fn_8049A82C(Effect::RW3::UnkRayVertexRW_8042C820 *vertices, u32 count, void *ltm, u32 flags);
int fn_8049AAF8(int prim);
int fn_8049A90C(void);
/* a - b, matrix * point, a x b, |v|, v made unit length, v *= scale, v * scale (as in
 * Game/GadgetUtil.h). */
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b);
void fn_8040CE74(Vec3f *v);
void fn_8040CE38(Vec3f *v, float scale);
Vec3f fn_8040CDA8(const Vec3f *v, float scale);
}

typedef Vec3f (*VectorOpFn_8042C820)(const Vec3f *a, const Vec3f *b);

/* a - b and a x b by value, and the out-of-line vector assignment (as in Game/EffBelt.cpp). */
#define Subtract(a, b) ((VectorOpFn_8042C820)fn_8040CD74)(&(a), &(b))
#define Cross(a, b) fn_8040CFB8(&(a), &(b))
#define AssignVector(vector, value) fn_800091D0(&(vector), &(const Vec3f &)(value))

/* Sets a vertex's position (RwIm3DVertexSetPos, with its arguments evaluated first). */
#define SetPosition(vertex, px, py, pz)                                                            \
    do {                                                                                           \
        float x_ = (px);                                                                           \
        float y_ = (py);                                                                           \
        float z_ = (pz);                                                                           \
        (vertex).objVertex.x = x_;                                                                 \
        (vertex).objVertex.y = y_;                                                                 \
        (vertex).objVertex.z = z_;                                                                 \
    } while (0)

#define RwRenderStateSet(state, value) (lbl_805F15B4->fpRenderStateSet((state), (void *)(value)))
#define RwRenderStateGet(state, value) (lbl_805F15B4->fpRenderStateGet((state), (void *)(value)))

enum {
    rwRENDERSTATETEXTURERASTER = 1,
    rwRENDERSTATEVERTEXALPHAENABLE = 12,
    rwRENDERSTATEZWRITEENABLE = 14,
    rwRENDERSTATECULLMODE = 20,
    rwCULLMODECULLNONE = 1,
    rwPRIMTYPETRISTRIP = 4
};

namespace Effect {
namespace RW3 {

EffectRayRW3::EffectRayRW3(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,
                           EffectManager *manager)
    : EffectRay(desc, info, manager), m_vertices(0) {
    m_size -= sizeof(EffectRay);
    m_size += sizeof(EffectRayRW3);
    if (!(m_flags & 1)) {
        return;
    }
    m_vertices = new UnkRayVertex_8042C820[m_unitCount];
    if (!m_vertices) {
        m_flags &= ~1;
        return;
    }
    m_size += m_unitCount * sizeof(UnkRayVertex_8042C820);
}

EffectRayRW3::~EffectRayRW3() {
    if (m_vertices) {
        delete[] m_vertices;
        m_vertices = 0;
    }
}

void EffectRayRW3::Update(float dt) {
    EffectRay::Update(dt);
    if ((m_flags & 1) && !(m_flags & 4)) {
        if (m_activeCount == 0) {
            return;
        }
    }
}

void EffectRayRW3::Draw() {
    if (!(m_flags & 1)) {
        return;
    }
    if (m_flags & 8) {
        return;
    }
    if (m_activeCount == 0) {
        return;
    }
    UnkRwCamera_8042C820 *camera = (UnkRwCamera_8042C820 *)dynamic_cast<EffectSystemRW3 *>(
                                       ::Effect::System::fn_8042A5D0())
                                       ->GetCamera();
    Vec3f cameraPos(camera->parent->pos.x, camera->parent->pos.y, camera->parent->pos.z);
    EffectRayExecUnit *unit = m_head;
    int i = 0;
    for (; i < m_unitCount; unit = unit->m_next, i++) {
        if (!unit) {
            break;
        }
        Vec3f start;
        AssignVector(start, unit->m_start);
        Vec3f end;
        Vec3f dir;
        AssignVector(dir, Subtract(start, unit->m_end));
        float length = fn_8040CF50(&dir);
        if (length < 0.0001f) {
            continue;
        }
        if (length < unit->m_length) {
            AssignVector(end, unit->m_end);
        } else {
            AssignVector(end, Subtract(start, (const Vec3f &)fn_8040CDA8(&dir, unit->m_length / length)));
        }
        if (m_config & 0x800) {
            AssignVector(start, fn_8040B67C(&m_worldMatrix, &start));
            AssignVector(end, fn_8040B67C(&m_worldMatrix, &end));
        }
        AssignVector(dir, Subtract(start, end));
        Vec3f toCamera;
        AssignVector(toCamera, Subtract(end, cameraPos));
        Vec3f side;
        fn_800091EC(&side, &(const Vec3f &)Cross(dir, toCamera));
        if (fn_8040CF50(&side) < 0.0001f) {
            AssignVector(toCamera, Subtract(start, cameraPos));
            AssignVector(side, Cross(dir, toCamera));
            if (fn_8040CF50(&side) < 0.0001f) {
                continue;
            }
        }
        fn_8040CE74(&side);
        fn_8040CE38(&side, 0.5f * unit->m_width);
        float red = (s32)((unit->m_color[0] >> 22) & 0xFF) / 255.0f;
        float green = (s32)((unit->m_color[1] >> 22) & 0xFF) / 255.0f;
        float blue = (s32)((unit->m_color[2] >> 22) & 0xFF) / 255.0f;
        float alpha = (s32)((unit->m_color[3] >> 22) & 0xFF) / 255.0f;
        u8 r = (s32)((unit->m_topColor[0] >> 22) & 0xFF) * red;
        u8 g = (s32)((unit->m_topColor[1] >> 22) & 0xFF) * green;
        u8 b = (s32)((unit->m_topColor[2] >> 22) & 0xFF) * blue;
        u8 a = (s32)((unit->m_topColor[3] >> 22) & 0xFF) * alpha;
        SetPosition(m_vertices[i].v[0], start.x + side.x, start.y + side.y, start.z + side.z);
        SetPosition(m_vertices[i].v[1], start.x - side.x, start.y - side.y, start.z - side.z);
        m_vertices[i].v[0].color[0] = r;
        m_vertices[i].v[0].color[1] = g;
        m_vertices[i].v[0].color[2] = b;
        m_vertices[i].v[0].color[3] = a;
        m_vertices[i].v[1].color[0] = r;
        m_vertices[i].v[1].color[1] = g;
        m_vertices[i].v[1].color[2] = b;
        m_vertices[i].v[1].color[3] = a;
        r = (s32)((unit->m_lastColor[0] >> 22) & 0xFF) * red;
        g = (s32)((unit->m_lastColor[1] >> 22) & 0xFF) * green;
        b = (s32)((unit->m_lastColor[2] >> 22) & 0xFF) * blue;
        a = (s32)((unit->m_lastColor[3] >> 22) & 0xFF) * alpha;
        SetPosition(m_vertices[i].v[2], end.x + side.x, end.y + side.y, end.z + side.z);
        SetPosition(m_vertices[i].v[3], end.x - side.x, end.y - side.y, end.z - side.z);
        m_vertices[i].v[2].color[0] = r;
        m_vertices[i].v[2].color[1] = g;
        m_vertices[i].v[2].color[2] = b;
        m_vertices[i].v[2].color[3] = a;
        m_vertices[i].v[3].color[0] = r;
        m_vertices[i].v[3].color[1] = g;
        m_vertices[i].v[3].color[2] = b;
        m_vertices[i].v[3].color[3] = a;
    }
    int cullMode;
    int vertexAlpha;
    int zWrite;
    int raster;
    RwRenderStateGet(rwRENDERSTATECULLMODE, &cullMode);
    RwRenderStateGet(rwRENDERSTATEVERTEXALPHAENABLE, &vertexAlpha);
    RwRenderStateGet(rwRENDERSTATEZWRITEENABLE, &zWrite);
    RwRenderStateGet(rwRENDERSTATETEXTURERASTER, &raster);
    RwRenderStateSet(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, 1);
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, 0);
    if (m_config & 2) {
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, 0);
    } else {
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, 1);
    }
    EffectRayExecUnit *ray = m_head;
    int j = 0;
    for (; j < m_unitCount; ray = ray->m_next, j++) {
        if (!ray) {
            break;
        }
        if (fn_8049A82C(m_vertices[j].v, 4, 0, 0x18)) {
            fn_8049AAF8(rwPRIMTYPETRISTRIP);
        }
    }
    fn_8049A90C();
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, zWrite);
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, vertexAlpha);
    RwRenderStateSet(rwRENDERSTATECULLMODE, cullMode);
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, raster);
}

} // namespace RW3
} // namespace Effect
