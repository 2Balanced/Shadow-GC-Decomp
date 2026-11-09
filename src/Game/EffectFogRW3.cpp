/* The fog of the RenderWare effect manager (no RTTI; class and file names invented): the manager
 * keeps it at 0x44, Effect::RW3::EffectGlobalParamRW3 changes its settings and Game/PlayerEffect.cpp
 * reads and sets them. Draw lays a copy of the frame buffer over the screen tinted with the fog's
 * colour, then blends a second colour over it, and can brighten the screen. Member and function
 * names are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/EffectFilterRW3.h"

#pragma pool_data off

using ::Effect::RW3::EffectSystemRW3;
using ::Effect::System::EffectManager;

extern "C" {
/* RenderWare: RwRasterCreate, RwRasterDestroy, RwRasterSubRaster, RwRasterGetOffset,
 * RwCameraBeginUpdate and RwCameraEndUpdate. */
RwRaster *fn_80489E74(s32 width, s32 height, s32 depth, s32 flags);
s32 fn_80489BDC(RwRaster *raster);
RwRaster *fn_80489DC4(RwRaster *subRaster, RwRaster *raster, RwRect *rect);
RwRaster *fn_80489B2C(RwRaster *raster, s16 *x, s16 *y);
RwCamera *fn_804854DC(RwCamera *camera);
RwCamera *fn_804854B4(RwCamera *camera);
/* Copies the frame buffer into a raster (behaviour inferred). */
void fn_804951B8(RwRaster *raster, s32 arg);
}

/* The fog (size from its creation). */
class UnkFogFilter_804386E4 {
  public:
    UnkFogFilter_804386E4(EffectManager *manager, const UnkFog_804388A4 *params);
    ~UnkFogFilter_804386E4() {
        if (m_ownRaster != 0) {
            if (m_ownRaster->parent != m_ownRaster) {
                fn_80489BDC(m_ownRaster->parent);
            }
            fn_80489BDC(m_ownRaster);
            m_ownRaster = 0;
        }
        m_raster = 0;
    }
    void SetParams(const UnkFog_804388A4 *params);
    void SetParams(const UnkFog_804388A4 &params);
    UnkFog_804388A4 GetParams() const;
    void Draw();

    /* 0x00 */ UnkFog_804388A4 m_params;
    /* 0x0C */ EffectManager *m_manager;
    /* 0x10 */ RwRaster *m_raster;    /* the frame buffer copy (a sub-raster) */
    /* 0x14 */ RwRaster *m_ownRaster; /* set when the manager had no copy to lend */
    /* 0x18 */ UnkIm2DVertex_804342CC m_quad[4];
};
SIZE_ASSERT(UnkFogFilter_804386E4, 0x78);

extern "C" void fn_804389F0(EffectManager *manager, const UnkFog_804388A4 *fog) {
    ManagerFields(manager)->m_fog = new UnkFogFilter_804386E4(manager, fog);
}

extern "C" void fn_80438974(EffectManager *manager) {
    UnkManagerRW3Fields_804329C4 *fields = reinterpret_cast<UnkManagerRW3Fields_804329C4 *>(manager);
    if (fields->m_fog != 0) {
        delete fields->m_fog;
        fields->m_fog = 0;
    }
}

extern "C" void fn_80438948(EffectManager *manager) {
    if (ManagerFields(manager)->m_fog != 0) {
        ManagerFields(manager)->m_fog->Draw();
    }
}

extern "C" void fn_8043891C(EffectManager *manager, const UnkFog_804388A4 &fog) {
    if (ManagerFields(manager)->m_fog != 0) {
        ManagerFields(manager)->m_fog->SetParams(fog);
    }
}

extern "C" UnkFog_804388A4 fn_804388A4(EffectManager *manager) {
    if (ManagerFields(manager)->m_fog != 0) {
        return ManagerFields(manager)->m_fog->GetParams();
    }
    UnkFog_804388A4 fog(0, 0, 0);
    return fog;
}

UnkFogFilter_804386E4::UnkFogFilter_804386E4(EffectManager *manager,
                                             const UnkFog_804388A4 *params)
    : m_params(0, 0, 0), m_manager(manager) {
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
        m_raster = m_ownRaster = fn_80489E74(0, 0, 0, 5);
        fn_80489DC4(m_ownRaster, fn_80489E74(rect.w, rect.h, 0, 0x505), &rect);
    }
    m_quad[0].z = lbl_805F15B4->dOpenDevice.zBufferNear;
    m_quad[1].z = lbl_805F15B4->dOpenDevice.zBufferNear;
    m_quad[2].z = lbl_805F15B4->dOpenDevice.zBufferNear;
    m_quad[3].z = lbl_805F15B4->dOpenDevice.zBufferNear;
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
    if (params != 0) {
        SetParams(params);
    }
}

void UnkFogFilter_804386E4::SetParams(const UnkFog_804388A4 *params) {
    if (params != 0) {
        m_params = *params;
    }
}

void UnkFogFilter_804386E4::SetParams(const UnkFog_804388A4 &params) {
    m_params = params;
}

/* Retail copies the settings into the returned object one member at a time, each load after
 * the store before it; GC/2.7 overlaps them (it treats the hidden result pointer as unable to
 * alias this). Turning the scheduler off for this function is a workaround that reproduces
 * retail's order; it is not evidence of how the original source was written. */
#pragma scheduling off
UnkFog_804388A4 UnkFogFilter_804386E4::GetParams() const {
    return m_params;
}
#pragma scheduling reset

void UnkFogFilter_804386E4::Draw() {
    if (!m_params.m_on && !m_params.m_enabled) {
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
    s32 srcBlend;
    s32 destBlend;
    s32 vertexAlpha;
    s32 zTest;
    s32 zWrite;
    s32 fog;
    s32 textureAddress;
    s16 x;
    s16 y;
    RwRaster *frameBuffer = camera->frameBuffer;
    RwRenderStateGet(10, &srcBlend);
    RwRenderStateGet(11, &destBlend);
    RwRenderStateGet(12, &vertexAlpha);
    RwRenderStateGet(6, &zTest);
    RwRenderStateGet(8, &zWrite);
    RwRenderStateGet(14, &fog);
    RwRenderStateGet(9, &textureAddress);
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
    m_quad[0].x = 0.0f;
    m_quad[0].y = 0.0f;
    m_quad[0].u = u0[0];
    m_quad[0].v = v0[0];
    m_quad[1].x = 0.0f;
    m_quad[1].y = rect.h;
    m_quad[1].u = u0[0];
    m_quad[1].v = v0[1];
    m_quad[2].x = rect.w;
    m_quad[2].y = 0.0f;
    m_quad[2].u = u0[1];
    m_quad[2].v = v0[0];
    m_quad[3].x = rect.w;
    m_quad[3].y = rect.h;
    m_quad[3].u = u0[1];
    m_quad[3].v = v0[1];
    if (m_params.m_on) {
        if (m_params.m_color.r != 0xFF || m_params.m_color.g != 0xFF ||
            m_params.m_color.b != 0xFF || m_params.m_color2.a != 0) {
            if (fn_804854DC(camera) != 0) {
                RwRenderStateSet(12, (void *)1);
                RwRenderStateSet(6, (void *)0);
                RwRenderStateSet(8, (void *)0);
                RwRenderStateSet(14, (void *)0);
                RwRenderStateSet(9, (void *)1);
                fn_804854B4(camera);
            }
            if (m_params.m_color.r != 0xFF || m_params.m_color.g != 0xFF ||
                m_params.m_color.b != 0xFF) {
                fn_804951B8(m_raster, 0);
                m_quad[0].r = m_params.m_color.r;
                m_quad[0].g = m_params.m_color.g;
                m_quad[0].b = m_params.m_color.b;
                m_quad[0].a = m_params.m_color.a;
                m_quad[1].r = m_params.m_color.r;
                m_quad[1].g = m_params.m_color.g;
                m_quad[1].b = m_params.m_color.b;
                m_quad[1].a = m_params.m_color.a;
                m_quad[2].r = m_params.m_color.r;
                m_quad[2].g = m_params.m_color.g;
                m_quad[2].b = m_params.m_color.b;
                m_quad[2].a = m_params.m_color.a;
                m_quad[3].r = m_params.m_color.r;
                m_quad[3].g = m_params.m_color.g;
                m_quad[3].b = m_params.m_color.b;
                m_quad[3].a = m_params.m_color.a;
                if (fn_804854DC(camera) != 0) {
                    RwRenderStateSet(1, m_raster->parent);
                    RwRenderStateSet(10, (void *)2);
                    RwRenderStateSet(11, (void *)1);
                    RwRenderStateSet(12, (void *)1);
                    RwRenderStateSet(6, (void *)0);
                    RwRenderStateSet(8, (void *)0);
                    RwRenderStateSet(14, (void *)0);
                    RwRenderStateSet(9, (void *)1);
                    RwIm2DRenderPrimitive(4, m_quad, 4);
                    fn_804854B4(camera);
                }
            }
            if (m_params.m_color2.a != 0) {
                m_quad[0].r = m_params.m_color2.r;
                m_quad[0].g = m_params.m_color2.g;
                m_quad[0].b = m_params.m_color2.b;
                m_quad[0].a = m_params.m_color2.a;
                m_quad[1].r = m_params.m_color2.r;
                m_quad[1].g = m_params.m_color2.g;
                m_quad[1].b = m_params.m_color2.b;
                m_quad[1].a = m_params.m_color2.a;
                m_quad[2].r = m_params.m_color2.r;
                m_quad[2].g = m_params.m_color2.g;
                m_quad[2].b = m_params.m_color2.b;
                m_quad[2].a = m_params.m_color2.a;
                m_quad[3].r = m_params.m_color2.r;
                m_quad[3].g = m_params.m_color2.g;
                m_quad[3].b = m_params.m_color2.b;
                m_quad[3].a = m_params.m_color2.a;
                if (fn_804854DC(camera) != 0) {
                    RwRenderStateSet(1, (void *)0);
                    RwRenderStateSet(10, (void *)5);
                    RwRenderStateSet(11, (void *)6);
                    RwIm2DRenderPrimitive(4, m_quad, 4);
                    fn_804854B4(camera);
                }
            }
        }
    }
    if (m_params.m_enabled) {
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
        if (fn_804854DC(camera) != 0) {
            RwRenderStateSet(1, (void *)0);
            RwRenderStateSet(10, (void *)10);
            RwRenderStateSet(11, (void *)1);
            RwRenderStateSet(12, (void *)1);
            RwRenderStateSet(6, (void *)0);
            RwRenderStateSet(8, (void *)0);
            RwRenderStateSet(14, (void *)0);
            RwRenderStateSet(9, (void *)1);
            RwIm2DRenderPrimitive(4, m_quad, 4);
            fn_804854B4(camera);
        }
    }
    if (fn_804854DC(camera) != 0) {
        RwRenderStateSet(10, (void *)srcBlend);
        RwRenderStateSet(11, (void *)destBlend);
        RwRenderStateSet(12, (void *)vertexAlpha);
        RwRenderStateSet(6, (void *)zTest);
        RwRenderStateSet(8, (void *)zWrite);
        RwRenderStateSet(14, (void *)fog);
        RwRenderStateSet(9, (void *)textureAddress);
        fn_804854B4(camera);
    }
}
