/* Effect::RW3::EffectShimmerRW3 (class name and bases from RTTI): the RenderWare heat-shimmer
 * element. It lays a grid of points over the effect's screen position, moves them with the wave
 * of Effect::System::EffectShimmer and draws a copy of the frame buffer through the moved grid as
 * a triangle strip. The unit also holds the RenderWare manager's screen shimmer, which does the
 * same over the whole screen (no RTTI; Game/EffectFilterRW3.h), and the extern "C" functions the
 * manager and EffectGlobalParamRW3 reach it through. File name from the class; member names are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). Float constants are read through their own addresses, as retail does,
 * with pool_data off; the functions that address the unit's RTTI records through one base
 * register are compiled with it on.
 *
 * NonMatching: 17 of 21 functions match.
 * - EffectShimmerRW3::Draw (914 instructions): GC/2.7 gives the element's 14 floats and the loop
 *   counters other registers than retail's and schedules the uv set-up differently.
 * - The screen shimmer's Draw (0x8043597C): retail reloads rect.w from the stack for the store
 *   after the size check where GC/2.7 keeps it in a register, and the rest of the function's
 *   registers follow from that.
 * - The screen shimmer's Update (0x80436128, 14 words): the phase and frequency of the two wave
 *   tables take f28/f29 the other way round (all 24 declaration orders tried), and the point's
 *   weight and the column's wave trade f2/f3.
 * - EffectShimmerRW3's constructor (1 word): retail starts the vertex loop's byte offset with
 *   mr r3,r9 (a copy of the zeroed counter), GC/2.7 with li r3,0. */
#include "Game/EffectFilterRW3.h"
#include "Game/MathConstants.h"

#include <cmath>

#pragma pool_data off

using ::Effect::RW3::EffectSystemRW3;
using ::Effect::RW3::EffectManagerRW3;
using ::Effect::System::EffectLaunchInfo;
using ::Effect::System::EffectManager;
using ::Effect::System::UnkEffectDesc_801D0AFC;

extern "C" {
/* RenderWare: RwRasterCreate, RwRasterDestroy, RwRasterSubRaster, RwRasterGetOffset,
 * RwCameraBeginUpdate, RwCameraEndUpdate and RwV3dTransformPoints (one point). */
RwRaster *fn_80489E74(s32 width, s32 height, s32 depth, s32 flags);
s32 fn_80489BDC(RwRaster *raster);
RwRaster *fn_80489DC4(RwRaster *subRaster, RwRaster *raster, RwRect *rect);
RwRaster *fn_80489B2C(RwRaster *raster, s16 *x, s16 *y);
RwCamera *fn_804854DC(RwCamera *camera);
RwCamera *fn_804854B4(RwCamera *camera);
RwV3d *fn_80482458(RwV3d *out, const RwV3d *in, const RwMatrix *matrix);
/* Copies the frame buffer into a raster (behaviour inferred). */
void fn_804951B8(RwRaster *raster, s32 arg);
/* Square root. */
float fn_8000FEE4(float x);
}

/* The screen shimmer (size from its creation). */
class UnkScreenShimmer_80436D48 {
  public:
    UnkScreenShimmer_80436D48(EffectManager *manager, const UnkScreenShimmerParams_804370D4 *params);
    ~UnkScreenShimmer_80436D48();
    UnkScreenShimmerParams_804370D4 GetParams() const;
    void SetParams(const UnkScreenShimmerParams_804370D4 &params);
    void SetParams(const UnkScreenShimmerParams_804370D4 *params);
    void Rebuild();
    void BuildGrid();
    void Update(float dt);
    void Draw();

    /* 0x00 */ EffectManager *m_manager;
    /* 0x04 */ RwRaster *m_raster;    /* the frame buffer copy (a sub-raster) */
    /* 0x08 */ RwRaster *m_ownRaster; /* set when the manager had no copy to lend */
    /* 0x0C */ UnkScreenShimmerParams_804370D4 m_params;
    /* 0x44 */ UnkIm2DVertex_804342CC m_quad[4];
    /* 0xA4 */ UnkIm2DVertex_804342CC *m_vertices;
    /* 0xA8 */ RwV3d *m_points;
    /* 0xAC */ float *m_waveY; /* m_y.m_count + 1 */
    /* 0xB0 */ float *m_waveX; /* m_x.m_count + 1 */
    /* 0xB4 */ s32 m_vertexCount;
    /* 0xB8 */ s32 m_pointCount;
    /* 0xBC */ float m_phaseX;
    /* 0xC0 */ float m_phaseY;
    /* 0xC4 */ s32 m_fullWidth;
    /* 0xC8 */ s32 m_fullHeight;
    /* 0xCC */ s32 m_width;
    /* 0xD0 */ s32 m_height;
};
SIZE_ASSERT(UnkScreenShimmer_80436D48, 0xD4);

UnkScreenShimmerParams_804370D4::UnkScreenShimmerParams_804370D4(
    bool enabled, u8 alpha, s32 countX, float amplitudeX, float cycleX, float waveLengthX,
    float startX, float endX, s32 countY, float amplitudeY, float cycleY, float waveLengthY,
    float startY, float endY) {
    m_enabled = enabled;
    m_alpha = alpha;
    m_x.m_count = countX;
    m_x.m_amplitude = amplitudeX;
    m_x.m_cycle = cycleX;
    m_x.m_waveLength = waveLengthX;
    m_x.m_start = startX;
    m_x.m_end = endX;
    m_y.m_count = countY;
    m_y.m_amplitude = amplitudeY;
    m_y.m_cycle = cycleY;
    m_y.m_waveLength = waveLengthY;
    m_y.m_start = startY;
    m_y.m_end = endY;
}

extern "C" void fn_80437088(EffectManager *manager, const UnkScreenShimmerParams_804370D4 *params) {
    ManagerFields(manager)->m_shimmer = new UnkScreenShimmer_80436D48(manager, params);
}

extern "C" void fn_80437044(EffectManager *manager) {
    if (ManagerFields(manager)->m_shimmer != 0) {
        delete ManagerFields(manager)->m_shimmer;
        ManagerFields(manager)->m_shimmer = 0;
    }
}

extern "C" void fn_80437018(EffectManager *manager, float dt) {
    if (ManagerFields(manager)->m_shimmer != 0) {
        ManagerFields(manager)->m_shimmer->Update(dt);
    }
}

extern "C" void fn_80436FEC(EffectManager *manager) {
    if (ManagerFields(manager)->m_shimmer != 0) {
        ManagerFields(manager)->m_shimmer->Draw();
    }
}

extern "C" void fn_80436FC0(EffectManager *manager, const UnkScreenShimmerParams_804370D4 &params) {
    if (ManagerFields(manager)->m_shimmer != 0) {
        ManagerFields(manager)->m_shimmer->SetParams(params);
    }
}

extern "C" UnkScreenShimmerParams_804370D4 fn_80436F2C(EffectManager *manager) {
    if (ManagerFields(manager)->m_shimmer != 0) {
        return ManagerFields(manager)->m_shimmer->GetParams();
    }
    UnkScreenShimmerParams_804370D4 params(false, 0xFF, 0, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0, 0.0f,
                                           0.0f, 0.0f, 0.0f, 1.0f);
    return params;
}

UnkScreenShimmer_80436D48::UnkScreenShimmer_80436D48(EffectManager *manager,
                                                     const UnkScreenShimmerParams_804370D4 *params)
    : m_manager(manager), m_params(false, 0xFF, 0, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0, 0.0f, 0.0f,
                                   0.0f, 0.0f, 1.0f) {
    EffectSystemRW3 *system = dynamic_cast<EffectSystemRW3 *>(::Effect::System::fn_8042A5D0());
    if (system == 0) {
        return;
    }
    RwCamera *camera = static_cast<RwCamera *>(system->GetCamera());
    if (camera == 0) {
        return;
    }
    RwRect rect = {0, 0, camera->frameBuffer->width, camera->frameBuffer->height};
    if (ManagerFields(manager)->m_raster != 0) {
        m_raster = ManagerFields(manager)->m_raster;
        m_ownRaster = 0;
    } else {
        m_raster = m_ownRaster = fn_80489E74(0, 0, 0, 0x505);
        fn_80489DC4(m_raster, fn_80489E74(rect.w, rect.h, 0, 0x505), &rect);
    }
    float z = lbl_805F15B4->dOpenDevice.zBufferNear;
    for (int i = 0; i < 4; i++) {
        m_quad[i].z = z;
        m_quad[i].r = 0xFF;
        m_quad[i].g = 0xFF;
        m_quad[i].b = 0xFF;
        m_quad[i].a = 0xFF;
    }
    m_fullWidth = rect.w;
    m_fullHeight = rect.h;
    m_points = 0;
    m_vertices = 0;
    m_waveY = 0;
    m_waveX = 0;
    if (params != 0) {
        SetParams(params);
    } else {
        m_params.m_enabled = false;
    }
}

UnkScreenShimmer_80436D48::~UnkScreenShimmer_80436D48() {
    if (m_points != 0) {
        delete[] m_points;
        m_points = 0;
    }
    if (m_vertices != 0) {
        delete[] m_vertices;
        m_vertices = 0;
    }
    if (m_waveX != 0) {
        delete[] m_waveX;
        m_waveX = 0;
    }
    if (m_waveY != 0) {
        delete[] m_waveY;
        m_waveY = 0;
    }
    if (m_ownRaster != 0) {
        if (m_ownRaster->parent != m_ownRaster) {
            fn_80489BDC(m_ownRaster->parent);
        }
        fn_80489BDC(m_ownRaster);
        m_ownRaster = 0;
    }
    m_raster = 0;
}

void UnkScreenShimmer_80436D48::SetParams(const UnkScreenShimmerParams_804370D4 *params) {
    if (params != 0) {
        SetParams(*params);
    }
}

void UnkScreenShimmer_80436D48::SetParams(const UnkScreenShimmerParams_804370D4 &params) {
    UnkScreenShimmerParams_804370D4 old = m_params;
    m_params = params;
    if (m_params.m_y.m_count < 1) {
        m_params.m_y.m_count = 1;
    }
    if (m_params.m_x.m_count < 1) {
        m_params.m_x.m_count = 1;
    }
    if (m_params.m_enabled) {
        if (old.m_x.m_count != m_params.m_x.m_count || old.m_y.m_count != m_params.m_y.m_count) {
            Rebuild();
        } else if (old.m_x.m_end != m_params.m_x.m_end ||
                   old.m_x.m_start != m_params.m_x.m_start ||
                   old.m_y.m_end != m_params.m_y.m_end ||
                   old.m_y.m_start != m_params.m_y.m_start) {
            m_height = 0;
            m_width = 0;
        }
    }
}

UnkScreenShimmerParams_804370D4 UnkScreenShimmer_80436D48::GetParams() const {
    return m_params;
}

void UnkScreenShimmer_80436D48::Rebuild() {
    if (m_points != 0) {
        delete[] m_points;
        m_points = 0;
    }
    if (m_vertices != 0) {
        delete[] m_vertices;
        m_vertices = 0;
    }
    if (m_waveX != 0) {
        delete[] m_waveX;
        m_waveX = 0;
    }
    if (m_waveY != 0) {
        delete[] m_waveY;
        m_waveY = 0;
    }
    m_pointCount = (m_params.m_x.m_count + 1) * (m_params.m_y.m_count + 1);
    m_points = new RwV3d[m_pointCount];
    m_vertexCount = (m_params.m_x.m_count + 1) * 2 * m_params.m_y.m_count;
    m_vertices = new UnkIm2DVertex_804342CC[m_vertexCount];
    m_waveX = new float[m_params.m_x.m_count + 1];
    m_waveY = new float[m_params.m_y.m_count + 1];
    if (m_points == 0 || m_vertices == 0 || m_waveX == 0 || m_waveY == 0) {
        if (m_points != 0) {
            delete[] m_points;
            m_points = 0;
        }
        if (m_vertices != 0) {
            delete[] m_vertices;
            m_vertices = 0;
        }
        if (m_waveX != 0) {
            delete[] m_waveX;
            m_waveX = 0;
        }
        if (m_waveY != 0) {
            delete[] m_waveY;
            m_waveY = 0;
        }
    }
    UnkIm2DVertex_804342CC *vertex = m_vertices;
    dynamic_cast<EffectSystemRW3 *>(::Effect::System::fn_8042A5D0())->GetCamera();
    float z = lbl_805F15B4->dOpenDevice.zBufferNear;
    for (int y = 0; y < m_params.m_y.m_count; y++) {
        for (int x = 0; x < m_params.m_x.m_count + 1; x++) {
            for (int i = 0; i < 2; i++) {
                vertex->z = z;
                vertex->r = 0xFF;
                vertex->g = 0xFF;
                vertex->b = 0xFF;
                vertex->a = 0xFF;
                vertex++;
            }
        }
    }
    m_height = 0;
    m_width = 0;
    m_phaseX = 0.0f;
    m_phaseY = 0.0f;
}

void UnkScreenShimmer_80436D48::BuildGrid() {
    float ratioX = (float)m_fullWidth / (float)m_width;
    float rangeX[2] = {(m_params.m_x.m_start - 0.5f) * ratioX + 0.5f,
                       (m_params.m_x.m_end - 0.5f) * ratioX + 0.5f};
    for (int i = 0; i < 2; i++) {
        if (rangeX[i] > 1.0f) {
            rangeX[i] = 1.0f;
        }
        if (rangeX[i] < 0.0f) {
            rangeX[i] = 0.0f;
        }
    }
    float stepX = (float)m_width * (rangeX[1] - rangeX[0]) / (float)m_params.m_x.m_count;
    float startX = rangeX[0] * (float)m_width;
    float ratioY = (float)m_fullHeight / (float)m_height;
    float rangeY[2] = {(m_params.m_y.m_start - 0.5f) * ratioY + 0.5f,
                       (m_params.m_y.m_end - 0.5f) * ratioY + 0.5f};
    for (int i = 0; i < 2; i++) {
        if (rangeY[i] > 1.0f) {
            rangeY[i] = 1.0f;
        }
        if (rangeY[i] < 0.0f) {
            rangeY[i] = 0.0f;
        }
    }
    float stepY = (float)m_height * (rangeY[1] - rangeY[0]) / (float)m_params.m_y.m_count;
    float startY = rangeY[0] * (float)m_height;
    for (int y = 0; y < m_params.m_y.m_count + 1; y++) {
        for (int x = 0; x < m_params.m_x.m_count + 1; x++) {
            RwV3d *point = &m_points[x + y * (m_params.m_x.m_count + 1)];
            point->x = stepX * (float)x + startX;
            point->y = stepY * (float)y + startY;
            if (y == 0 || x == 0 || y == m_params.m_y.m_count || x == m_params.m_x.m_count) {
                point->z = 0.0f;
            } else {
                point->z = 1.0f;
            }
        }
    }
}

void UnkScreenShimmer_80436D48::Update(float dt) {
    if (!m_params.m_enabled) {
        return;
    }
    if (m_points == 0 || m_vertices == 0 || m_waveX == 0 || m_waveY == 0) {
        Rebuild();
        if (m_points == 0 || m_vertices == 0 || m_waveX == 0 || m_waveY == 0) {
            return;
        }
    }
    if (m_width == 0 || m_height == 0) {
        return;
    }
    m_phaseY += dt;
    m_phaseX += dt;
    while (m_phaseY > m_params.m_y.m_cycle) {
        m_phaseY -= m_params.m_y.m_cycle;
    }
    while (m_phaseX > m_params.m_x.m_cycle) {
        m_phaseX -= m_params.m_x.m_cycle;
    }
    float twoPi = 2.0f * lbl_805FAD38;
    float phaseX = twoPi * m_phaseX / m_params.m_x.m_cycle;
    float freqY = twoPi * (float)m_height / ((float)m_params.m_y.m_count * m_params.m_y.m_waveLength);
    for (int i = 0; i < m_params.m_y.m_count + 1; i++) {
        m_waveY[i] = m_params.m_y.m_amplitude * std::sin((float)i * freqY + phaseX);
    }
    float phaseY = twoPi * m_phaseY / m_params.m_y.m_cycle;
    float freqX = twoPi * (float)m_width / ((float)m_params.m_x.m_count * m_params.m_x.m_waveLength);
    for (int i = 0; i < m_params.m_x.m_count + 1; i++) {
        m_waveX[i] = m_params.m_x.m_amplitude * std::sin((float)i * freqX + phaseY);
    }
    UnkIm2DVertex_804342CC *vertex = m_vertices;
    int count = m_params.m_x.m_count + 1;
    for (int y = 0; y < m_params.m_y.m_count; y++) {
        int odd = y % 2;
        for (int x = 0; x < count; x++) {
            int column;
            if (odd != 0) {
                column = m_params.m_x.m_count - x;
            } else {
                column = x;
            }
            float waveX = m_waveX[column];
            for (int i = 0; i < 2; i++) {
                RwV3d *point = &m_points[column + count * (y + i)];
                float weight = point->z;
                vertex->x = weight * m_waveY[y + i] + point->x;
                vertex->y = waveX * weight + point->y;
                vertex++;
            }
        }
    }
}

void UnkScreenShimmer_80436D48::Draw() {
    if (!m_params.m_enabled) {
        return;
    }
    if (m_points == 0 || m_vertices == 0 || m_waveX == 0 || m_waveY == 0) {
        return;
    }
    if (m_raster == 0) {
        return;
    }
    EffectSystemRW3 *system = dynamic_cast<EffectSystemRW3 *>(::Effect::System::fn_8042A5D0());
    if (system == 0) {
        return;
    }
    RwCamera *camera = static_cast<RwCamera *>(system->GetCamera());
    if (camera == 0) {
        return;
    }
    RwRect rect;
    float u[2];
    float v[2];
    s32 srcBlend;
    s32 destBlend;
    s32 state9;
    s32 vertexAlpha;
    s32 zTest;
    s32 zWrite;
    s32 state14;
    s32 cullMode;
    s16 x;
    s16 y;
    RwRaster *frameBuffer = camera->frameBuffer;
    fn_80489B2C(frameBuffer, &x, &y);
    rect.x = x;
    rect.y = y;
    rect.w = frameBuffer->width;
    rect.h = frameBuffer->height;
    fn_80489DC4(m_raster, m_raster->parent, &rect);
    RwRaster *parent = frameBuffer->parent;
    u32 width = parent->width;
    u32 height = parent->height;
    float u0[2] = {(0.5f + x) / width, (0.5f + (x + rect.w)) / width};
    float v0[2] = {(0.5f + y) / height, (0.5f + (y + rect.h)) / height};
    for (int i = 0; i < 4; i++) {
        int column = i / 2;
        int row = i % 2;
        m_quad[i].x = column * rect.w;
        m_quad[i].y = row * rect.h;
        m_quad[i].u = u0[column];
        m_quad[i].v = v0[row];
    }
    if (rect.w != m_width || rect.h != m_height) {
        m_width = rect.w;
        m_height = rect.h;
        BuildGrid();
        Update(0.0f);
    }
    float uRange = u0[1] - u0[0];
    float vRange = v0[1] - v0[0];
    UnkIm2DVertex_804342CC *vertex = m_vertices;
    u8 alpha = m_params.m_alpha;
    int count = m_params.m_x.m_count + 1;
    for (int row = 0; row < m_params.m_y.m_count; row++) {
        int odd = row % 2;
        for (int x = 0; x < count; x++) {
            int column;
            if (odd != 0) {
                column = m_params.m_x.m_count - x;
            } else {
                column = x;
            }
            for (int i = 0; i < 2; i++) {
                RwV3d *point = &m_points[column + count * (row + i)];
                vertex->u = uRange * (point->x / m_width) + u0[0];
                vertex->v = vRange * (point->y / m_height) + v0[0];
                vertex->r = 0xFF;
                vertex->g = 0xFF;
                vertex->b = 0xFF;
                vertex->a = alpha;
                vertex++;
            }
        }
    }
    RwRenderStateGet(10, &srcBlend);
    RwRenderStateGet(11, &destBlend);
    RwRenderStateGet(9, &state9);
    RwRenderStateGet(12, &vertexAlpha);
    RwRenderStateGet(6, &zTest);
    RwRenderStateGet(8, &zWrite);
    RwRenderStateGet(14, &state14);
    RwRenderStateGet(20, &cullMode);
    fn_804951B8(m_raster->parent, 0);
    if (fn_804854DC(camera) != 0) {
        RwRenderStateSet(1, m_raster->parent);
        RwRenderStateSet(10, (void *)5);
        RwRenderStateSet(11, (void *)6);
        RwRenderStateSet(12, (void *)1);
        RwRenderStateSet(6, (void *)0);
        RwRenderStateSet(8, (void *)0);
        RwRenderStateSet(14, (void *)0);
        RwRenderStateSet(9, (void *)1);
        RwRenderStateSet(20, (void *)1);
        RwIm2DRenderPrimitive(4, m_vertices, m_vertexCount);
        fn_804854B4(camera);
    }
    if (fn_804854DC(camera) != 0) {
        RwRenderStateSet(10, (void *)srcBlend);
        RwRenderStateSet(11, (void *)destBlend);
        RwRenderStateSet(12, (void *)vertexAlpha);
        RwRenderStateSet(6, (void *)zTest);
        RwRenderStateSet(8, (void *)zWrite);
        RwRenderStateSet(14, (void *)state14);
        RwRenderStateSet(9, (void *)state9);
        RwRenderStateSet(20, (void *)cullMode);
        fn_804854B4(camera);
    }
}

namespace Effect {
namespace RW3 {

#pragma pool_data on
EffectShimmerRW3::EffectShimmerRW3(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,
                                   EffectManager *manager)
    : EffectShimmer(desc, info, manager) {
    m_vertices = 0;
    m_points = 0;
    m_waveX = 0;
    m_waveY = 0;
    m_size -= sizeof(EffectShimmer);
    m_size += sizeof(EffectShimmerRW3);
    if (!(m_flags & 1)) {
        return;
    }
    EffectSystemRW3 *system = dynamic_cast<EffectSystemRW3 *>(::Effect::System::fn_8042A5D0());
    if (system == 0) {
        return;
    }
    if (system->GetCamera() == 0) {
        return;
    }
    float z = lbl_805F15B4->dOpenDevice.zBufferNear;
    m_quad[0].z = z;
    m_quad[1].z = z;
    m_quad[2].z = z;
    m_quad[3].z = z;
    m_quad[0].r = 0xFF;
    m_quad[0].g = 0xFF;
    m_quad[0].b = 0xFF;
    m_quad[0].a = 0xFF;
    m_quad[1].r = 0xFF;
    m_quad[1].g = 0xFF;
    m_quad[1].b = 0xFF;
    m_quad[1].a = 0xFF;
    m_quad[2].r = 0xFF;
    m_quad[2].g = 0xFF;
    m_quad[2].b = 0xFF;
    m_quad[2].a = 0xFF;
    m_quad[3].r = 0xFF;
    m_quad[3].g = 0xFF;
    m_quad[3].b = 0xFF;
    m_quad[3].a = 0xFF;
    m_phaseX = m_phaseY = 0.0f;
    m_pointCount = (m_counts[0] + 1) * (m_counts[1] + 1);
    m_points = static_cast<RwV3d *>(system->Allocate(m_pointCount * sizeof(RwV3d)));
    if (m_points == 0) {
        m_flags &= ~1;
        return;
    }
    m_size += m_pointCount * sizeof(RwV3d);
    float stepX = field_0x110.x / (float)m_counts[0];
    float stepY = field_0x110.y / (float)m_counts[1];
    for (int y = 0; y < m_counts[1] + 1; y++) {
        for (int x = 0; x < m_counts[0] + 1; x++) {
            RwV3d *point = &m_points[x + y * (m_counts[0] + 1)];
            point->x = stepX * (float)x - 0.5f * field_0x110.x;
            point->y = stepY * (float)y - 0.5f * field_0x110.y;
            point->z = 0.0f;
        }
    }
    m_vertexCount = m_counts[1] * ((m_counts[0] + 1) * 2);
    if (m_counts[1] > 1) {
        m_vertexCount -= m_counts[1] - 1;
    }
    m_vertices = static_cast<UnkIm2DVertex_804342CC *>(
        system->Allocate(m_vertexCount * sizeof(UnkIm2DVertex_804342CC)));
    if (m_vertices == 0) {
        m_flags &= ~1;
        return;
    }
    m_size += m_vertexCount * sizeof(UnkIm2DVertex_804342CC);
    for (int i = 0; i < m_vertexCount; i++) {
        m_vertices[i].z = z;
        m_vertices[i].r = 0xFF;
        m_vertices[i].g = 0xFF;
        m_vertices[i].b = 0xFF;
        m_vertices[i].a = 0xFF;
    }
    switch (*static_cast<const s32 *>(m_params)) {
    case 0:
        m_waveX = static_cast<float *>(system->Allocate(m_counts[0] * sizeof(float) + 1));
        if (m_waveX == 0) {
            m_flags &= ~1;
            return;
        }
        m_size += m_counts[0] * sizeof(float) + 1;
        m_waveY = static_cast<float *>(system->Allocate(m_counts[1] * sizeof(float) + 1));
        if (m_waveY == 0) {
            m_flags &= ~1;
            return;
        }
        m_size += m_counts[1] * sizeof(float) + 1;
        break;
    }
}

EffectShimmerRW3::~EffectShimmerRW3() {
    EffectSystemRW3 *system = dynamic_cast<EffectSystemRW3 *>(::Effect::System::fn_8042A5D0());
    if (system != 0) {
        if (m_points != 0) {
            system->Free(m_points);
        }
        if (m_vertices != 0) {
            system->Free(m_vertices);
        }
        if (m_waveX != 0) {
            system->Free(m_waveX);
        }
        if (m_waveY != 0) {
            system->Free(m_waveY);
        }
    }
}

#pragma pool_data off
void EffectShimmerRW3::Update(float dt) {
    EffectShimmer::Update(dt);
    if (!(m_flags & 1) || (m_flags & 4) || m_points == 0 || m_vertices == 0) {
        return;
    }
    float twoPi = 2.0f * lbl_805FAD38;
    switch (*static_cast<const s32 *>(m_params)) {
    case 0: {
        m_phaseX += dt / 60.0f;
        m_phaseY += dt / 60.0f;
        if (m_cycle.x > 0.0001f) {
            while (m_phaseX > m_cycle.x) {
                m_phaseX -= m_cycle.x;
            }
        }
        if (m_cycle.y > 0.0001f) {
            while (m_phaseY > m_cycle.y) {
                m_phaseY -= m_cycle.y;
            }
        }
        float freqX = twoPi * field_0x110.x / ((float)m_counts[0] * m_waveLength.x);
        float phaseX = twoPi * m_phaseX / m_cycle.x;
        float freqY = twoPi * field_0x110.y / ((float)m_counts[1] * m_waveLength.y);
        float phaseY = twoPi * m_phaseY / m_cycle.y;
        for (int i = 0; i < m_counts[0] + 1; i++) {
            m_waveX[i] = m_amplitude.x * std::sin((float)i * freqX + phaseX);
        }
        for (int i = 0; i < m_counts[1] + 1; i++) {
            m_waveY[i] = m_amplitude.y * std::sin((float)i * freqY + phaseY);
        }
        break;
    }
    case 1:
        m_phaseX += dt / 60.0f;
        if ((s32)field_0x13C == 0 && m_cycle.x > 0.0001f) {
            while (m_phaseX > m_cycle.x) {
                m_phaseX -= m_cycle.x;
            }
        }
        break;
    }
}

#pragma pool_data off
void EffectShimmerRW3::Draw() {
    if (!(m_flags & 1) || (m_flags & 8) || m_points == 0 || m_vertices == 0) {
        return;
    }
    EffectSystemRW3 *system = dynamic_cast<EffectSystemRW3 *>(::Effect::System::fn_8042A5D0());
    if (system == 0) {
        return;
    }
    RwCamera *camera = static_cast<RwCamera *>(system->GetCamera());
    if (camera == 0) {
        return;
    }
    RwRaster *frameBuffer = camera->frameBuffer;
    RwRaster *raster = ManagerFields(dynamic_cast<EffectManagerRW3 *>(m_manager))->m_raster;
    if (raster == 0) {
        return;
    }
    RwRaster *parent = frameBuffer->parent;
    if (parent == 0) {
        return;
    }
    RwV3d position = {m_lastPosition.x, m_lastPosition.y, m_lastPosition.z};
    fn_80482458(&position, &position, &camera->viewMatrix);
    float nearPlane = camera->nearPlane;
    float farPlane = camera->farPlane;
    if (position.z < nearPlane || position.z > farPlane) {
        return;
    }
    s16 x;
    s16 y;
    fn_80489B2C(frameBuffer, &x, &y);
    RwRect rect;
    rect.x = x;
    rect.y = y;
    rect.w = frameBuffer->width;
    rect.h = frameBuffer->height;
    fn_80489DC4(raster, raster->parent, &rect);
    u32 width = parent->width;
    u32 height = parent->height;
    float u[2] = {(0.5f + x) / width, (0.5f + (x + rect.w)) / width};
    float v[2] = {(0.5f + y) / height, (0.5f + (y + rect.h)) / height};
    float screenWidth = rect.w;
    float screenHeight = rect.h;
    float nearZ = lbl_805F15B4->dOpenDevice.zBufferNear;
    float zScale = (lbl_805F15B4->dOpenDevice.zBufferFar - nearZ) / (farPlane - nearPlane);
    float recipZ = 1.0f / position.z;
    float screen[2] = {recipZ * (position.x * (float)rect.w), recipZ * (position.y * (float)rect.h)};
    float z = farPlane / position.z * ((position.z - nearPlane) * zScale) + nearZ;
    int size = rect.h;
    if (rect.w > size) {
        size = rect.w;
    }
    float scale = recipZ * (float)size;
    float uRange = u[1] - u[0];
    float vRange = v[1] - v[0];
    for (int i = 0; i < 4; i++) {
        int index;
        switch (i) {
        case 0:
            index = 0;
            break;
        case 1:
            index = m_counts[1] * (m_counts[0] + 1);
            break;
        case 2:
            index = m_counts[0];
            break;
        case 3:
            index = m_pointCount - 1;
            break;
        }
        float qx = scale * m_points[index].x + screen[0];
        float qy = scale * m_points[index].y + screen[1];
        if (qx < 0.0f) {
            qx = 0.0f;
        }
        if (qx > screenWidth) {
            qx = screenWidth;
        }
        if (qy < 0.0f) {
            qy = 0.0f;
        }
        if (qy > screenHeight) {
            qy = screenHeight;
        }
        m_quad[i].x = qx;
        m_quad[i].y = qy;
        m_quad[i].u = uRange * (qx / rect.w) + u[0];
        m_quad[i].v = vRange * (qy / rect.h) + v[0];
    }
    u8 alpha = reinterpret_cast<const u8 *>(m_desc)[0x30];
    float twoPi = 2.0f * lbl_805FAD38;
    switch (*static_cast<const s32 *>(m_params)) {
    case 0: {
        float uEnd = u[0] + u[1];
        float vEnd = v[0] + v[1];
        for (int row = 0; row < m_counts[1]; row++) {
            int odd = row % 2;
            for (int col = 0; col < m_counts[0] + 1; col++) {
                int column = col;
                if (odd != 0) {
                    column = m_counts[0] - col;
                }
                float waveX = m_waveX[column] * scale;
                for (int i = 0; i < 2; i++) {
                    if (row != 0 && col == 0 && i == 0) {
                        continue;
                    }
                    int count = m_counts[0] + 1;
                    RwV3d *point = &m_points[column + count * (row + i)];
                    float fx = point->x / (0.5f * field_0x110.x);
                    float fy = point->y / (0.5f * field_0x110.y);
                    float waveY = m_waveY[row + i];
                    int index = row * (count * 2) + i + col * 2 - row;
                    float px = scale * point->x + screen[0];
                    float falloff = (1.0f - fx * fx) * (1.0f - fy * fy);
                    if (px < 0.0f) {
                        m_vertices[index].u = u[0];
                        m_vertices[index].x = 0.0f;
                    } else if (px > rect.w) {
                        m_vertices[index].u = uEnd;
                        m_vertices[index].x = screenWidth;
                    } else {
                        m_vertices[index].u = uRange * (px / rect.w) + u[0];
                        m_vertices[index].x = falloff * (waveY * scale) + px;
                    }
                    float py = scale * point->y + screen[1];
                    if (py < 0.0f) {
                        m_vertices[index].v = v[0];
                        m_vertices[index].y = 0.0f;
                    } else if (py > rect.h) {
                        m_vertices[index].v = vEnd;
                        m_vertices[index].y = screenHeight;
                    } else {
                        m_vertices[index].v = vRange * (py / rect.h) + v[0];
                        m_vertices[index].y = falloff * waveX + py;
                    }
                    m_vertices[index].z = z;
                    m_vertices[index].r = 0xFF;
                    m_vertices[index].g = 0xFF;
                    m_vertices[index].b = 0xFF;
                    m_vertices[index].a = alpha;
                }
            }
        }
        break;
    }
    case 1: {
        float freq = twoPi / m_waveLength.x;
        float phase = twoPi * m_phaseX / m_cycle.x;
        float limit = (s32)field_0x13C;
        bool ended = true;
        float uEnd = u[0] + u[1];
        float vEnd = v[0] + v[1];
        for (int row = 0; row < m_counts[1]; row++) {
            int odd = row % 2;
            for (int col = 0; col < m_counts[0] + 1; col++) {
                int column = col;
                if (odd != 0) {
                    column = m_counts[0] - col;
                }
                for (int i = 0; i < 2; i++) {
                    if (row != 0 && col == 0 && i == 0) {
                        continue;
                    }
                    int count = m_counts[0] + 1;
                    int offset = column + count * (row + i);
                    int index = row * (count * 2) + i + col * 2 - row;
                    float distance = fn_8000FEE4(m_points[offset].x * m_points[offset].x +
                                                 m_points[offset].y * m_points[offset].y);
                    float amplitude;
                    if ((s32)field_0x13C == 0) {
                        amplitude = m_amplitude.x * std::sin(distance * freq + phase);
                        ended = false;
                    } else {
                        float t = (m_phaseX - distance * m_cycle.x / m_waveLength.x) / m_cycle.x;
                        if (t > limit) {
                            amplitude = 0.0f;
                        } else if (t < 0.0f) {
                            amplitude = 0.0f;
                            ended = false;
                        } else {
                            amplitude = m_amplitude.x * std::sin(twoPi * t);
                            ended = false;
                        }
                    }
                    if (distance > 0.0001f) {
                        amplitude /= distance;
                    } else {
                        amplitude = 0.0f;
                    }
                    float fx = m_points[offset].x / (0.5f * field_0x110.x);
                    float fy = m_points[offset].y / (0.5f * field_0x110.y);
                    float px = scale * m_points[offset].x + screen[0];
                    float falloffX = 1.0f - fx * fx;
                    float falloffY = 1.0f - fy * fy;
                    float falloff = falloffX * falloffY;
                    if (px < 0.0f) {
                        m_vertices[index].u = u[0];
                        m_vertices[index].x = 0.0f;
                    } else if (px > rect.w) {
                        m_vertices[index].u = uEnd;
                        m_vertices[index].x = screenWidth;
                    } else {
                        m_vertices[index].u = uRange * (px / rect.w) + u[0];
                        m_vertices[index].x =
                            falloff * (falloffY * (scale * (amplitude * m_points[offset].x))) + px;
                    }
                    float py = scale * m_points[offset].y + screen[1];
                    if (py < 0.0f) {
                        m_vertices[index].v = v[0];
                        m_vertices[index].y = 0.0f;
                    } else if (py > rect.h) {
                        m_vertices[index].v = vEnd;
                        m_vertices[index].y = screenHeight;
                    } else {
                        m_vertices[index].v = vRange * (py / rect.h) + v[0];
                        m_vertices[index].y =
                            falloff * (falloffY * (scale * (amplitude * m_points[offset].y))) + py;
                    }
                    m_vertices[index].z = z;
                    m_vertices[index].r = 0xFF;
                    m_vertices[index].g = 0xFF;
                    m_vertices[index].b = 0xFF;
                    m_vertices[index].a = alpha;
                }
            }
        }
        if (ended) {
            m_flags &= ~1;
            return;
        }
        break;
    }
    }
    fn_804951B8(raster->parent, 0);
    if (fn_804854DC(camera) != 0) {
        RwRenderStateSet(1, raster->parent);
        RwRenderStateSet(10, (void *)5);
        RwRenderStateSet(11, (void *)6);
        RwRenderStateSet(12, (void *)1);
        RwRenderStateSet(6, (void *)1);
        RwRenderStateSet(8, (void *)0);
        RwRenderStateSet(14, (void *)0);
        RwRenderStateSet(9, (void *)1);
        RwIm2DRenderPrimitive(4, m_vertices, m_vertexCount);
        fn_804854B4(camera);
    }
}

} // namespace RW3
} // namespace Effect
