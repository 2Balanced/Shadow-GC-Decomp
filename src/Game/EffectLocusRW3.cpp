/* Effect::RW3::EffectLocusRW3 (class name and bases from RTTI): the RenderWare trail element. For
 * every point of the trail (Effect::System::EffectLocus) it places one vertex pair per section of
 * the description's cross-section, coloured by the element's colours, optionally turned towards
 * the camera, and draws them as immediate-mode triangle strips. File name from the class; member
 * names are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first).
 *
 * NonMatching: 4 of 5 functions match. BuildVertices (0x80439B18) converts both ways between
 * floats and integers, and retail shares the conversion stack slots, which GC/2.7 does not
 * (docs/research-notes.md); its frame is 0x30 bytes larger and the float registers shift with
 * it. The code is otherwise written out instruction for instruction. */
#include "Game/EffectRW3.h"
#include "Game/Quaternion.h"

#include <cmath>

#pragma pool_data off

using ::Effect::RW3::EffectSystemRW3;
using ::Effect::System::EffectLaunchInfo;
using ::Effect::System::EffectManager;
using ::Effect::System::UnkEffectDesc_801D0AFC;

/* A trail vertex (RwIm3DVertex). */
struct UnkLocusVertex_80439B18 {
    /* 0x00 */ float x;
    /* 0x04 */ float y;
    /* 0x08 */ float z;
    /* 0x0C */ float nx;
    /* 0x10 */ float ny;
    /* 0x14 */ float nz;
    /* 0x18 */ u8 r;
    /* 0x19 */ u8 g;
    /* 0x1A */ u8 b;
    /* 0x1B */ u8 a;
    /* 0x1C */ float u;
    /* 0x20 */ float v;
};

/* One section of the trail's cross-section (layout from its use). */
struct UnkLocusSection_80439B18 {
    /* 0x00 */ u8 m_type; /* 0x15 */
    /* 0x01 */ u8 m_more; /* another section follows */
    /* 0x04 */ Vec3f m_position;
    /* 0x10 */ u8 field_0x10[0x1C - 0x10];
    /* 0x1C */ u8 m_color[4];
};

/* The trail element's description (the fields used here). */
struct UnkLocusDesc_80439B18 {
    /* 0x00 */ u8 field_0x0[0x54];
    /* 0x54 */ const UnkLocusSection_80439B18 *m_sections;
};

/* One point of the trail (Effect::System::EffectLocusExecUnit; the fields used here). */
struct UnkLocusPoint_80439B18 {
    /* 0x00 */ u8 field_0x0[0x4];
    /* 0x04 */ Mtx34f m_matrix;
    /* 0x34 */ u8 field_0x34[0x3C - 0x34];
    /* 0x3C */ UnkLocusPoint_80439B18 *m_next;
};

extern "C" {
/* RenderWare: RwIm3DTransform, RwIm3DRenderPrimitive and RwIm3DEnd. */
void *fn_8049A82C(void *vertices, s32 count, void *matrix, u32 flags);
s32 fn_8049AAF8(s32 primitive);
s32 fn_8049A90C(void);
/* Matrix and vector helpers: a product, an inverse, a point moved by a matrix, a normalisation,
 * a cross and a dot product, a rotation from an axis and an angle, and a rotation applied to a
 * matrix. */
Mtx34f fn_8001E044(const Mtx34f *a, const Mtx34f *b);
Mtx34f fn_8040BEF8(const Mtx34f *matrix);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
void fn_8040CE74(Vec3f *v);
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b);
float fn_8040CF90(const Vec3f *a, const Vec3f *b);
Quatf fn_8040C874(const Vec3f *axis, float angle);
void fn_8040B8C0(Mtx34f *matrix, const Quatf *rotation);
}

inline void CopyMatrix(Mtx34f &to, const Mtx34f &from) {
    fn_8040C148(&to, &from);
}

inline void AssignVector(Vec3f &to, const Vec3f &from) {
    fn_800091D0(&to, &from);
}

inline void CopyVector(Vec3f &to, const Vec3f &from) {
    fn_800091EC(&to, &from);
}

namespace Effect {
namespace RW3 {

EffectLocusRW3::EffectLocusRW3(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,
                               EffectManager *manager)
    : EffectLocus(desc, info, manager) {
    m_vertices = 0;
    m_size -= sizeof(EffectLocus);
    m_size += sizeof(EffectLocusRW3);
    if (!(m_flags & 1)) {
        return;
    }
    m_vertices = static_cast<UnkLocusVertex_80439B18 *>(::Effect::System::fn_8042A5D0()->Allocate(
        m_maxPoints * m_sectionCount * 2 * sizeof(UnkLocusVertex_80439B18)));
    if (m_vertices == 0) {
        m_flags &= ~1;
        return;
    }
    m_size += sizeof(UnkLocusVertex_80439B18) * m_maxPoints * m_sectionCount * 2;
}

EffectLocusRW3::~EffectLocusRW3() {
    if (m_vertices != 0) {
        ::Effect::System::fn_8042A5D0()->Free(m_vertices);
        m_vertices = 0;
    }
}

void EffectLocusRW3::Update(float dt) {
    EffectLocus::Update(dt);
    if ((m_flags & 1) && !(m_flags & 4) && m_pointCount != 0 && m_last != 0 &&
        !(m_config & 0x4000)) {
        BuildVertices();
    }
}

void EffectLocusRW3::Draw() {
    if ((m_flags & 1) && !(m_flags & 8) && m_pointCount != 0 && m_last != 0) {
        if (m_config & 0x4000) {
            BuildVertices();
        }
        s32 cullMode;
        s32 vertexAlpha;
        s32 fog;
        s32 raster;
        RwRenderStateGet(20, &cullMode);
        RwRenderStateGet(12, &vertexAlpha);
        RwRenderStateGet(14, &fog);
        RwRenderStateGet(1, &raster);
        RwRenderStateSet(20, (void *)1);
        RwRenderStateSet(12, (void *)1);
        RwRenderStateSet(1, (void *)0);
        if (m_config & 2) {
            RwRenderStateSet(14, (void *)0);
        } else {
            RwRenderStateSet(14, (void *)1);
        }
        if (m_maxPoints <= m_sectionCount) {
            int i = 0;
            for (UnkLocusPoint_80439B18 *point = reinterpret_cast<UnkLocusPoint_80439B18 *>(m_head);
                 point != 0; point = point->m_next) {
                if (fn_8049A82C(&m_vertices[i * m_sectionCount * 2], m_sectionCount * 2, 0,
                                0x18) != 0) {
                    fn_8049AAF8(4);
                }
                i++;
            }
            fn_8049A90C();
        } else {
            for (int i = 0; i < m_sectionCount - 1; i++) {
                if (fn_8049A82C(&m_vertices[i * m_maxPoints * 2], (m_pointCount + 1) * 2, 0,
                                0x18) != 0) {
                    fn_8049AAF8(4);
                }
            }
        }
        RwRenderStateSet(14, (void *)fog);
        RwRenderStateSet(12, (void *)vertexAlpha);
        RwRenderStateSet(20, (void *)cullMode);
        RwRenderStateSet(1, (void *)raster);
    }
}

void EffectLocusRW3::BuildVertices() {
    const UnkLocusDesc_80439B18 *desc = reinterpret_cast<const UnkLocusDesc_80439B18 *>(m_desc);
    Mtx34f matrix;
    Vec3f cameraPos;
    if (m_config & 0x4000) {
        RwCamera *camera = static_cast<RwCamera *>(
            dynamic_cast<EffectSystemRW3 *>(::Effect::System::fn_8042A5D0())->GetCamera());
        RwFrame *frame = reinterpret_cast<RpClump *>(camera)->frame;
        cameraPos.x = frame->modelling.pos.x;
        cameraPos.y = frame->modelling.pos.y;
        cameraPos.z = frame->modelling.pos.z;
    }
    float scaleR = (s32)((m_color[0] >> 22) & 0xFF) / 255.0f;
    float scaleG = (s32)((m_color[1] >> 22) & 0xFF) / 255.0f;
    float scaleB = (s32)((m_color[2] >> 22) & 0xFF) / 255.0f;
    float scaleA = (s32)((m_color[3] >> 22) & 0xFF) / 255.0f;
    float topR = (s32)((m_topColor[0] >> 22) & 0xFF) / 255.0f;
    float topG = (s32)((m_topColor[1] >> 22) & 0xFF) / 255.0f;
    float topB = (s32)((m_topColor[2] >> 22) & 0xFF) / 255.0f;
    float topA = (s32)((m_topColor[3] >> 22) & 0xFF) / 255.0f;
    float lastR = (s32)((m_lastColor[0] >> 22) & 0xFF) / 255.0f;
    float lastG = (s32)((m_lastColor[1] >> 22) & 0xFF) / 255.0f;
    float lastB = (s32)((m_lastColor[2] >> 22) & 0xFF) / 255.0f;
    float lastA = (s32)((m_lastColor[3] >> 22) & 0xFF) / 255.0f;
    float color[4] = {lastR, lastG, lastB, lastA};
    UnkLocusPoint_80439B18 *point = reinterpret_cast<UnkLocusPoint_80439B18 *>(m_head);
    for (int i = 0; point != 0; i++) {
        if (m_config & 0x100000) {
            float t = (float)i / (float)m_pointCount;
            float s = 1.0f - t;
            color[0] = s * lastR + t * topR;
            color[1] = s * lastG + t * topG;
            color[2] = s * lastB + t * topB;
            color[3] = s * lastA + t * topA;
        }
        color[0] *= scaleR;
        color[1] *= scaleG;
        color[2] *= scaleB;
        color[3] *= scaleA;
        bool transform;
        if ((m_config & 0x800) || (m_config & 0x4000) ||
            i + m_removedCount > m_pointCount - 1 - m_addedCount) {
            transform = true;
            if (m_config & 0x800) {
                CopyMatrix(matrix, fn_8001E044(&m_worldMatrix, &point->m_matrix));
            } else {
                fn_8040C148(&matrix, &point->m_matrix);
            }
            if (m_config & 0x4000) {
                Vec3f axis(1.0f, 0.0f, 0.0f);
                Mtx34f inverse = fn_8040BEF8(&matrix);
                Vec3f direction;
                AssignVector(direction, fn_8040B67C(&inverse, &cameraPos));
                direction.z = 0.0f;
                fn_8040CE74(&direction);
                Vec3f normal;
                CopyVector(normal, fn_8040CFB8(&axis, &direction));
                if (fn_8040CF50(&normal) > 0.0001f) {
                    fn_8040CE74(&normal);
                    Quatf rotation(
                        fn_8040C874(&normal, std::acos(fn_8040CF90(&axis, &direction))));
                    fn_8040B8C0(&matrix, &rotation);
                }
            }
        } else {
            transform = false;
        }
        const UnkLocusSection_80439B18 *section = desc->m_sections;
        for (int j = 0; section != 0; j++) {
            if (section->m_type != 0x15) {
                break;
            }
            int index;
            int previous;
            if (m_maxPoints <= m_sectionCount) {
                index = (j + i * m_sectionCount) * 2;
                if (i > 0) {
                    previous = (j + (i - 1) * m_sectionCount) * 2 + 1;
                } else {
                    previous = -1;
                }
            } else {
                index = (i + j * m_maxPoints) * 2;
                if (j > 0) {
                    previous = (i + (j - 1) * m_maxPoints) * 2 + 1;
                } else {
                    previous = -1;
                }
            }
            if (transform) {
                Vec3f position(section->m_position.x, section->m_position.y,
                               section->m_position.z);
                AssignVector(position, fn_8040B67C(&matrix, &position));
                m_vertices[index].x = position.x;
                m_vertices[index].y = position.y;
                m_vertices[index].z = position.z;
            } else {
                int source;
                if (m_maxPoints <= m_sectionCount) {
                    source = (j + m_sectionCount * (i + m_removedCount)) * 2;
                } else {
                    source = (i + (m_removedCount + j * m_maxPoints)) * 2;
                }
                m_vertices[index] = m_vertices[source];
            }
            m_vertices[index].r = color[0] * section->m_color[0];
            m_vertices[index].g = color[1] * section->m_color[1];
            m_vertices[index].b = color[2] * section->m_color[2];
            m_vertices[index].a = color[3] * section->m_color[3];
            if (previous >= 0) {
                m_vertices[previous] = m_vertices[index];
            }
            if (!section->m_more) {
                if (m_config & 0x2000) {
                    if (m_maxPoints <= m_sectionCount) {
                        m_vertices[(j + 1 + i * m_sectionCount) * 2] =
                            m_vertices[i * m_sectionCount * 2];
                        if (i > 0) {
                            m_vertices[(j + 1 + (i - 1) * m_sectionCount) * 2 + 1] =
                                m_vertices[(i - 1) * m_sectionCount * 2 + 1];
                        }
                    } else {
                        m_vertices[(i + j * m_maxPoints) * 2 + 1] = m_vertices[i * 2];
                    }
                }
                break;
            }
            section++;
        }
        if (point->m_next == 0) {
            if (point != reinterpret_cast<UnkLocusPoint_80439B18 *>(m_last)) {
                color[0] = topR;
                color[1] = topG;
                color[2] = topB;
                color[3] = topA;
                point = reinterpret_cast<UnkLocusPoint_80439B18 *>(m_last);
            } else {
                point = 0;
            }
        } else {
            point = point->m_next;
            color[0] = color[1] = color[2] = color[3] = 1.0f;
        }
    }
}

} // namespace RW3
} // namespace Effect
