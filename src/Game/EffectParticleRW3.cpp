/* Effect::RW3::EffectParticleRW3 and Effect::RW3::EffectParticle3DRW3 (class names and bases from
 * RTTI): the RenderWare particle effect elements. EffectParticleRW3 draws its particles as a
 * particle tank (PTank) atomic, filled each frame from the particle list; EffectParticle3DRW3 gives
 * each particle a copy of a clump, or else draws it as an immediate-mode quad. File name from the
 * first class; member names are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first).
 *
 * NonMatching: 6 of 8 functions match.
 * - EffectParticleRW3's constructor (98%): GC/2.7 loads desc->m_particleKind for the switch above
 *   the three stores to the tank's plugin data before it; retail loads it after them. Every
 *   other instruction matches (a const_cast of desc does not change it).
 * - EffectParticle3DRW3's constructor (77%): retail reloads the address of each corner
 *   constant (-0.5, 0 and 0.5, separate .rodata objects) after each fn_800091D0 call; GC/2.7
 *   keeps one base address for all of them in a saved register (stmw r30 instead of stw r31),
 *   which renumbers the registers of the whole function. Named temporaries, braced blocks and
 *   an inline const-reference wrapper (which calls Vec3f's constructor out of line) were tried. */
#include "Game/EffectRW3.h"

using ::Effect::System::EffectLaunchInfo;
using ::Effect::System::EffectManager;
using ::Effect::System::UnkEffectDesc_801D0AFC;
using ::Effect::System::EffectParticleExecUnit;

/* A 3D particle's model data: the four vertices of its quad (an RwIm3DVertex each), or a copy of
 * the effect's clump (layout from its use; see also Game/EffectParticleUSA.cpp). */
struct UnkParticleVertex_80433180 {
    /* 0x00 */ RwV3d m_position;
    /* 0x0C */ u8 field_0xC[0x18 - 0xC];
    /* 0x18 */ u8 m_color[4];
    /* 0x1C */ float m_u;
    /* 0x20 */ float m_v;
};

struct UnkParticleModel_80027DC4 {    /* 0x00 */ UnkParticleVertex_80433180 m_vertices[4];
    /* 0x90 */ void *m_clump;
    /* 0x94 */ bool m_inWorld;
};

/* The model data of a 3D particle effect, as an array. */
inline UnkParticleModel_80027DC4 *Models(void *model) {
    return static_cast<UnkParticleModel_80027DC4 *>(model);
}

extern "C" {
/* The effect library's particle tank pool (Game/EffectLensFlareRW3.cpp's unit): takes a tank for
 * count particles from manager's pool, and gives it back. */
void *fn_804324C4(EffectManager *manager, s32 count);
void fn_804324A0(EffectManager *manager, void *atomic, s32 count);
/* RenderWare: RpPTankAtomicCreate, RpPTankAtomicLock and RpPTankAtomicUnlock. */
void *fn_80448074(s32 maxCount, u32 dataFlags, u32 platformFlags);
void *fn_80448408(void *atomic, RpPTankLockStruct *lock, u32 dataFlags, u32 lockFlags);
void fn_80448598(void *atomic);
/* RenderWare: RwFrameCreate, RwFrameDestroy, RwFrameUpdateObjects, RpAtomicSetFrame,
 * RpAtomicGetWorld, RpWorldRemoveAtomic, RpMaterialSetTexture and
 * RwTexDictionaryFindNamedTexture. */
RwFrame *fn_80486D00(void);
s32 fn_80486E2C(RwFrame *frame);
RwFrame *fn_8048722C(RwFrame *frame);
void *fn_80458CBC(void *atomic, RwFrame *frame);
void *fn_804622F0(void *atomic);
void *fn_8046221C(void *world, void *atomic);
void *fn_8045B270(void *material, void *texture);
void *fn_8048CFFC(void *dictionary, const char *name);
/* RenderWare: RpClumpClone, RpClumpDestroy, RpClumpRender, RpClumpGetWorld, RpWorldAddClump and
 * RpWorldRemoveClump. */
void *fn_80457090(void *clump);
s32 fn_80457468(void *clump);
void *fn_80456BC4(void *clump);
void *fn_8046256C(void *clump);
void *fn_804622FC(void *world, void *clump);
void *fn_804623EC(void *world, void *clump);
/* RenderWare: RwIm3DTransform, RwIm3DRenderPrimitive and RwIm3DEnd. */
void *fn_8049A82C(void *vertices, s32 count, void *matrix, u32 flags);
s32 fn_8049AAF8(s32 primitive);
s32 fn_8049A90C(void);
/* Matrix helpers: a point moved by a matrix, a scale by a vector, and a product. */
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
void fn_8040B790(Mtx34f *matrix, const Vec3f *scale);
Mtx34f fn_8001E044(const Mtx34f *a, const Mtx34f *b);
}

/* pi (another unit's constant). */
extern const float lbl_805FAD38;

inline void AssignVector(Vec3f &to, const Vec3f &from) {
    fn_800091D0(&to, &from);
}

namespace Effect {
namespace RW3 {

EffectParticleRW3::EffectParticleRW3(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,
                                     EffectManager *manager)
    : EffectParticle(desc, info, manager) {
    m_model = 0;
    m_frame = 0;
    field_0x144 = 0;
    m_size -= sizeof(EffectParticle);
    m_size += sizeof(EffectParticleRW3);
    if (!(m_flags & 1)) {
        return;
    }
    m_model = fn_804324C4(m_manager, m_count);
    if (m_model == 0) {
        m_model = fn_80448074(m_count, 0xA7, 0);
    }
    if (m_model == 0) {
        m_flags &= ~1;
        return;
    }
    m_frame = fn_80486D00();
    if (m_frame == 0) {
        m_flags &= ~1;
        return;
    }
    fn_80458CBC(m_model, m_frame);
    PTankData(m_model)->m_flags |= 0x800000;
    PTankData(m_model)->m_activeCount = 0;
    PTankData(m_model)->m_vertexAlphaBlend = 1;
    switch (desc->m_particleKind) {
    case 0:
        if (m_config & 4) {
            PTankData(m_model)->m_srcBlend = 5;
            PTankData(m_model)->m_dstBlend = 6;
            PTankData(m_model)->m_flags |= 0x10000000;
        } else {
            PTankData(m_model)->m_srcBlend = 2;
            PTankData(m_model)->m_dstBlend = 1;
            PTankData(m_model)->m_flags |= 0x10000000;
        }
        break;
    case 1:
        if (m_config & 4) {
            PTankData(m_model)->m_srcBlend = 5;
            PTankData(m_model)->m_dstBlend = 2;
            PTankData(m_model)->m_flags |= 0x10000000;
        } else {
            PTankData(m_model)->m_srcBlend = 2;
            PTankData(m_model)->m_dstBlend = 2;
            PTankData(m_model)->m_flags |= 0x10000000;
        }
        break;
    case 2:
        PTankData(m_model)->m_srcBlend = 9;
        PTankData(m_model)->m_dstBlend = 1;
        PTankData(m_model)->m_flags |= 0x10000000;
        break;
    case 3:
        break;
    }
    if ((m_config & 0x10) && desc->m_texture != 0) {
        EffectLaunchInfoRW3 *rw3Info = dynamic_cast<EffectLaunchInfoRW3 *>(info);
        if (rw3Info != 0 && rw3Info->field_0x8 != 0) {
            void *texture = fn_8048CFFC(rw3Info->field_0x8,
                                        reinterpret_cast<const char *>(desc->m_texture));
            if (texture != 0) {
                fn_8045B270(static_cast<RpAtomic *>(m_model)->geometry->materials[0], texture);
            }
        }
    }
    RpPTankLockStruct position;
    RpPTankLockStruct color;
    RpPTankLockStruct size;
    RpPTankLockStruct rotation;
    RpPTankLockStruct uv;
    fn_80448408(m_model, &position, 1, 0x40000000);
    fn_80448408(m_model, &color, 2, 0x40000000);
    fn_80448408(m_model, &size, 4, 0x40000000);
    fn_80448408(m_model, &rotation, 0x20, 0x40000000);
    fn_80448408(m_model, &uv, 0x80, 0x40000000);
    if (position.data == 0 || color.data == 0 || size.data == 0 || rotation.data == 0 ||
        uv.data == 0) {
        m_flags &= ~1;
    }
    fn_80448598(m_model);
}

EffectParticleRW3::~EffectParticleRW3() {
    if (m_model != 0) {
        void *world = fn_804622F0(m_model);
        if (world != 0) {
            fn_8046221C(world, m_model);
        }
        fn_80458CBC(m_model, 0);
        PTankData(m_model)->m_flags |= 0x800000;
        PTankData(m_model)->m_activeCount = 0;
        fn_8045B270(static_cast<RpAtomic *>(m_model)->geometry->materials[0], 0);
        fn_804324A0(m_manager, m_model, m_count);
    }
    if (m_frame != 0) {
        fn_80486E2C(m_frame);
    }
}

void EffectParticleRW3::Update(float dt) {
    EffectParticle::Update(dt);
    if ((m_flags & 1) && !(m_flags & 4) && m_liveCount != 0 && m_frame != 0) {
        RwMatrix *matrix = &m_frame->modelling;
        matrix->right.x = m_localMatrix.values[0][0];
        matrix->right.y = m_localMatrix.values[1][0];
        matrix->right.z = m_localMatrix.values[2][0];
        matrix->up.x = m_localMatrix.values[0][1];
        matrix->up.y = m_localMatrix.values[1][1];
        matrix->up.z = m_localMatrix.values[2][1];
        matrix->at.x = m_localMatrix.values[0][2];
        matrix->at.y = m_localMatrix.values[1][2];
        matrix->at.z = m_localMatrix.values[2][2];
        matrix->pos.x = m_localMatrix.values[0][3];
        matrix->pos.y = m_localMatrix.values[1][3];
        matrix->pos.z = m_localMatrix.values[2][3];
        fn_8048722C(m_frame);
    }
}

void EffectParticleRW3::Draw() {
    if ((m_flags & 1) && !(m_flags & 8) && m_liveCount != 0) {
        RpPTankLockStruct position;
        RpPTankLockStruct color;
        RpPTankLockStruct size;
        RpPTankLockStruct rotation;
        RpPTankLockStruct uv;
        fn_80448408(m_model, &position, 1, 0x40000000);
        fn_80448408(m_model, &color, 2, 0x40000000);
        fn_80448408(m_model, &size, 4, 0x40000000);
        fn_80448408(m_model, &rotation, 0x20, 0x40000000);
        fn_80448408(m_model, &uv, 0x80, 0x40000000);
        EffectParticleExecUnit *particle = m_particles;
        if (m_config & 0x800) {
            for (; particle != 0; particle = particle->m_next) {
                Vec3f world;
                AssignVector(world, fn_8040B67C(&m_worldMatrix, &particle->m_position));
                RwV3d *out = reinterpret_cast<RwV3d *>(position.data);
                position.data += position.stride;
                out->x = world.x;
                out->y = world.y;
                out->z = world.z;
            }
        } else {
            for (; particle != 0; particle = particle->m_next) {
                RwV3d *out = reinterpret_cast<RwV3d *>(position.data);
                position.data += position.stride;
                out->x = particle->m_position.x;
                out->y = particle->m_position.y;
                out->z = particle->m_position.z;
            }
        }
        for (particle = m_particles; particle != 0; particle = particle->m_next) {
            u8 *outColor = color.data;
            color.data += color.stride;
            outColor[0] = (particle->m_color[0] >> 22) & 0xFF;
            outColor[1] = (particle->m_color[1] >> 22) & 0xFF;
            outColor[2] = (particle->m_color[2] >> 22) & 0xFF;
            outColor[3] = (particle->m_color[3] >> 22) & 0xFF;
            float *outSize = reinterpret_cast<float *>(size.data);
            size.data += size.stride;
            outSize[0] = particle->m_scale.x;
            outSize[1] = particle->m_scale.y;
            float *outRotation = reinterpret_cast<float *>(rotation.data);
            rotation.data += rotation.stride;
            while (particle->m_rotation.z > lbl_805FAD38 ||
                   particle->m_rotation.z < -lbl_805FAD38) {
                if (particle->m_rotation.z < 0.0f) {
                    particle->m_rotation.z += 2.0f * lbl_805FAD38;
                } else {
                    particle->m_rotation.z -= 2.0f * lbl_805FAD38;
                }
            }
            *outRotation = particle->m_rotation.z;
            float *outUv = reinterpret_cast<float *>(uv.data);
            uv.data += uv.stride;
            outUv[0] = particle->m_uv0[0];
            outUv[1] = particle->m_uv0[1];
            outUv[2] = particle->m_uv1[0];
            outUv[3] = particle->m_uv1[1];
        }
        fn_80448598(m_model);
        PTankData(m_model)->m_flags |= 0x800000;
        PTankData(m_model)->m_activeCount = m_liveCount;
        if (field_0x144 == 0) {
            s32 fog;
            RwRenderStateGet(14, &fog);
            if (m_config & 2) {
                RwRenderStateSet(14, reinterpret_cast<void *>(0));
            } else {
                RwRenderStateSet(14, reinterpret_cast<void *>(1));
            }
            static_cast<RpAtomic *>(m_model)->renderCallBack(static_cast<RpAtomic *>(m_model));
            RwRenderStateSet(14, reinterpret_cast<void *>(fog));
        }
    }
}

EffectParticle3DRW3::EffectParticle3DRW3(const UnkEffectDesc_801D0AFC *desc,
                                         EffectLaunchInfo *info, EffectManager *manager)
    : EffectParticle(desc, info, manager), m_model(0), m_clump(0), m_texDict(0), m_world(0) {
    m_size -= sizeof(EffectParticle);
    m_size += sizeof(EffectParticle3DRW3);
    if (!(m_flags & 1)) {
        return;
    }
    m_model = new UnkParticleModel_80027DC4[m_count];
    if (m_model == 0) {
        m_flags &= ~1;
        return;
    }
    for (int i = 0; i < m_count; i++) {
        Models(m_model)[i].m_clump = 0;
        Models(m_model)[i].m_inWorld = false;
    }
    {
        Vec3f corner(-0.5f, -0.5f, 0.0f);
        fn_800091D0(&m_corners[0], &corner);
    }
    {
        Vec3f corner(-0.5f, 0.5f, 0.0f);
        fn_800091D0(&m_corners[1], &corner);
    }
    {
        Vec3f corner(0.5f, -0.5f, 0.0f);
        fn_800091D0(&m_corners[2], &corner);
    }
    {
        Vec3f corner(0.5f, 0.5f, 0.0f);
        fn_800091D0(&m_corners[3], &corner);
    }
    m_size += m_count * sizeof(UnkParticleModel_80027DC4);
}

EffectParticle3DRW3::~EffectParticle3DRW3() {
    if (m_model != 0) {
        for (int i = 0; i < m_count; i++) {
            if (Models(m_model)[i].m_clump != 0) {
                void *world = fn_8046256C(Models(m_model)[i].m_clump);
                if (world != 0) {
                    fn_804623EC(world, Models(m_model)[i].m_clump);
                }
                fn_80457468(Models(m_model)[i].m_clump);
            }
        }
        delete[] static_cast<UnkParticleModel_80027DC4 *>(m_model);
        m_model = 0;
    }
}

void EffectParticle3DRW3::Update(float dt) {
    EffectParticle::Update(dt);
    if ((m_flags & 1) && !(m_flags & 4)) {
        if (m_clump != 0) {
            EffectParticleExecUnit *particle = m_particles;
            for (int i = 0; i < m_count; i++) {
                if (particle == 0) {
                    return;
                }
                UnkParticleModel_80027DC4 *models =
                    static_cast<UnkParticleModel_80027DC4 *>(m_model);
                if (models[i].m_clump == 0) {
                    static_cast<UnkParticleModel_80027DC4 *>(m_model)[i].m_clump =
                        fn_80457090(m_clump);
                }
                Mtx34f matrix;
                fn_8040C148(&matrix, &lbl_805E0720);
                fn_8040B59C(&matrix, &particle->m_position);
                fn_8040B80C(&matrix, particle->m_rotation.z);
                fn_8040B848(&matrix, particle->m_rotation.y);
                fn_8040B884(&matrix, particle->m_rotation.x);
                fn_8040B790(&matrix, &particle->m_scale);
                if (m_config & 0x800) {
                    fn_8040C148(&matrix, &(const Mtx34f &)fn_8001E044(&m_worldMatrix, &matrix));
                }
                RwFrame *frame =
                    static_cast<RpClump *>(
                        static_cast<UnkParticleModel_80027DC4 *>(m_model)[i].m_clump)
                        ->frame;
                frame->modelling.right.x = matrix.values[0][0];
                frame->modelling.right.y = matrix.values[1][0];
                frame->modelling.right.z = matrix.values[2][0];
                frame->modelling.up.x = matrix.values[0][1];
                frame->modelling.up.y = matrix.values[1][1];
                frame->modelling.up.z = matrix.values[2][1];
                frame->modelling.at.x = matrix.values[0][2];
                frame->modelling.at.y = matrix.values[1][2];
                frame->modelling.at.z = matrix.values[2][2];
                frame->modelling.pos.x = matrix.values[0][3];
                frame->modelling.pos.y = matrix.values[1][3];
                frame->modelling.pos.z = matrix.values[2][3];
                frame->modelling.flags &= ~0x20003;
                fn_8048722C(frame);
                particle = particle->m_next;
            }
        } else {
            EffectParticleExecUnit *particle = m_particles;
            for (int i = 0; i < m_count; i++) {
                if (particle == 0) {
                    return;
                }
                Mtx34f matrix;
                fn_8040C148(&matrix, &lbl_805E0720);
                fn_8040B59C(&matrix, &particle->m_position);
                fn_8040B80C(&matrix, particle->m_rotation.z);
                fn_8040B848(&matrix, particle->m_rotation.y);
                fn_8040B884(&matrix, particle->m_rotation.x);
                fn_8040B790(&matrix, &particle->m_scale);
                if (m_config & 0x800) {
                    fn_8040C148(&matrix, &(const Mtx34f &)fn_8001E044(&m_worldMatrix, &matrix));
                }
                for (int j = 0; j < 4; j++) {
                    Vec3f corner;
                    AssignVector(corner, fn_8040B67C(&matrix, &m_corners[j]));
                    float x = corner.x;
                    float y = corner.y;
                    float z = corner.z;
                    Models(m_model)[i].m_vertices[j].m_position.x = x;
                    Models(m_model)[i].m_vertices[j].m_position.y = y;
                    Models(m_model)[i].m_vertices[j].m_position.z = z;
                    u8 r = (particle->m_color[0] >> 22) & 0xFF;
                    u8 g = (particle->m_color[1] >> 22) & 0xFF;
                    u8 b = (particle->m_color[2] >> 22) & 0xFF;
                    u8 a = (particle->m_color[3] >> 22) & 0xFF;
                    Models(m_model)[i].m_vertices[j].m_color[0] = r;
                    Models(m_model)[i].m_vertices[j].m_color[1] = g;
                    Models(m_model)[i].m_vertices[j].m_color[2] = b;
                    Models(m_model)[i].m_vertices[j].m_color[3] = a;
                    Models(m_model)[i].m_vertices[j].m_u =
                        (j / 2) ? particle->m_uv0[0] : particle->m_uv1[0];
                    Models(m_model)[i].m_vertices[j].m_v =
                        (j % 2) ? particle->m_uv0[1] : particle->m_uv1[1];
                }
                particle = particle->m_next;
            }
        }
    }
}

void EffectParticle3DRW3::Draw() {
    if (!(m_flags & 1) || (m_flags & 8)) {
        if (m_world != 0) {
            for (int i = 0; i < m_count; i++) {
                UnkParticleModel_80027DC4 *model =
                    &Models(m_model)[i];
                if (model->m_clump != 0 && model->m_inWorld) {
                    void *world = fn_8046256C(model->m_clump);
                    if (world != 0) {
                        fn_804623EC(world,
                                    Models(m_model)[i].m_clump);
                    }
                    Models(m_model)[i].m_inWorld = false;
                }
            }
        }
        return;
    }
    if (m_clump != 0) {
        EffectParticleExecUnit *particle = m_particles;
        for (int i = 0; i < m_count; i++) {
            if (particle != 0) {
                if (m_world != 0) {
                    UnkParticleModel_80027DC4 *model =
                        &Models(m_model)[i];
                    if (!model->m_inWorld) {
                        fn_804622FC(m_world, model->m_clump);
                        Models(m_model)[i].m_inWorld = true;
                    }
                } else {
                    fn_80456BC4(Models(m_model)[i].m_clump);
                }
                particle = particle->m_next;
            } else {
                UnkParticleModel_80027DC4 *model =
                    &Models(m_model)[i];
                if (model->m_inWorld) {
                    void *world = fn_8046256C(model->m_clump);
                    if (world != 0) {
                        fn_804623EC(world,
                                    Models(m_model)[i].m_clump);
                    }
                    Models(m_model)[i].m_inWorld = false;
                }
            }
        }
        return;
    }
    s32 cullMode;
    s32 vertexAlpha;
    s32 fog;
    s32 raster;
    s32 filter;
    RwRenderStateGet(20, &cullMode);
    RwRenderStateGet(12, &vertexAlpha);
    RwRenderStateGet(14, &fog);
    RwRenderStateGet(1, &raster);
    RwRenderStateGet(9, &filter);
    RwRenderStateSet(20, reinterpret_cast<void *>(1));
    RwRenderStateSet(9, reinterpret_cast<void *>(2));
    RwRenderStateSet(12, reinterpret_cast<void *>(1));
    if (m_config & 2) {
        RwRenderStateSet(14, reinterpret_cast<void *>(0));
    } else {
        RwRenderStateSet(14, reinterpret_cast<void *>(1));
    }
    if (m_texDict != 0) {
        RwRenderStateSet(1, *static_cast<void **>(m_texDict));
    } else {
        RwRenderStateSet(1, reinterpret_cast<void *>(0));
    }
    EffectParticleExecUnit *particle = m_particles;
    for (int i = 0; i < m_count; i++) {
        if (particle == 0) {
            break;
        }
        if (fn_8049A82C(&Models(m_model)[i], 4, 0, 0x19) != 0) {
            fn_8049AAF8(4);
        }
        particle = particle->m_next;
    }
    fn_8049A90C();
    RwRenderStateSet(14, reinterpret_cast<void *>(fog));
    RwRenderStateSet(12, reinterpret_cast<void *>(vertexAlpha));
    RwRenderStateSet(20, reinterpret_cast<void *>(cullMode));
    RwRenderStateSet(1, reinterpret_cast<void *>(raster));
    RwRenderStateSet(9, reinterpret_cast<void *>(filter));
}

} // namespace RW3
} // namespace Effect
