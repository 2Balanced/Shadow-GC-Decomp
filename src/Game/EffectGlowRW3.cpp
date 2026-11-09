/* The glow pass of the RenderWare effect manager (no RTTI; class and file names invented): the
 * manager keeps it at 0x4C and draws a model through it with its vf18 (Effect::RW3::
 * EffectManagerRW3::vf18, defined here). Begin clears the camera and copies the frame buffer,
 * the model is drawn, and End blends the copies back over the screen, the glow added up to count
 * times and tinted with the given colour. Member and function names are inferred. Definitions are
 * in reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/EffectFilterRW3.h"

#pragma pool_data off

using ::Effect::RW3::EffectManagerRW3;
using ::Effect::RW3::EffectSystemRW3;

extern "C" {
/* RenderWare: RwRasterSubRaster, RwRasterGetOffset, RwCameraBeginUpdate, RwCameraEndUpdate,
 * RwCameraClear and RpClumpRender. */
RwRaster *fn_80489DC4(RwRaster *subRaster, RwRaster *raster, RwRect *rect);
RwRaster *fn_80489B2C(RwRaster *raster, s16 *x, s16 *y);
RwCamera *fn_804854DC(RwCamera *camera);
RwCamera *fn_804854B4(RwCamera *camera);
RwCamera *fn_804857BC(RwCamera *camera, const UnkFogColor_804388A4 *color, s32 flags);
void *fn_80456BC4(RpClump *clump);
/* Copies the frame buffer into a raster (behaviour inferred). */
void fn_804951B8(RwRaster *raster, s32 arg);
/* GXSetBlendMode. */
void fn_80398D00(s32 type, s32 srcFactor, s32 dstFactor, s32 op);
}

/* The glow pass (layout from its uses). */
class UnkGlow_80437118 {
  public:
    void Begin();
    void End(bool additive, int count, const UnkFogColor_804388A4 *color);
    void Draw(RpClump *clump, bool additive, int count, void *color);

    /* 0x00 */ u32 field_0x0;
    /* 0x04 */ RwCamera *m_camera;
    /* 0x08 */ UnkIm2DVertex_804342CC m_quad[4];
    /* 0x68 */ RwRaster *m_raster;  /* the frame buffer copies (sub-rasters) */
    /* 0x6C */ u32 field_0x6C;
    /* 0x70 */ RwRaster *m_raster2;
    /* 0x74 */ u32 field_0x74;
    /* 0x78 */ bool m_additive;
    /* 0x7C */ RwRaster *m_frameBuffer;
    /* 0x80 */ s32 m_vertexAlpha; /* the render states Begin saves and End restores */
    /* 0x84 */ s32 m_zTest;
    /* 0x88 */ s32 m_zWrite;
    /* 0x8C */ s32 m_fog;
    /* 0x90 */ s32 m_srcBlend;
    /* 0x94 */ s32 m_destBlend;
    /* 0x98 */ s32 m_textureAddress;
    /* 0x9C */ s32 m_shadeMode;
    /* 0xA0 */ s32 m_cullMode;
};

namespace Effect {
namespace RW3 {

void EffectManagerRW3::vf18(RpClump *clump, bool arg2, int count, void *arg4) {
    if (ManagerFields(this)->m_glow != 0) {
        ManagerFields(this)->m_glow->Draw(clump, arg2, count, arg4);
    }
}

} // namespace RW3
} // namespace Effect

void UnkGlow_80437118::Begin() {
    if (m_raster != 0 && m_raster2 != 0) {
        EffectSystemRW3 *system = dynamic_cast<EffectSystemRW3 *>(::Effect::System::fn_8042A5D0());
        if (system == 0) {
            return;
        }
        m_camera = static_cast<RwCamera *>(system->GetCamera());
        if (m_camera == 0) {
            return;
        }
        m_frameBuffer = m_camera->frameBuffer;
        RwRenderStateGet(10, &m_srcBlend);
        RwRenderStateGet(11, &m_destBlend);
        RwRenderStateGet(12, &m_vertexAlpha);
        RwRenderStateGet(6, &m_zTest);
        RwRenderStateGet(8, &m_zWrite);
        RwRenderStateGet(14, &m_fog);
        RwRenderStateGet(9, &m_textureAddress);
        RwRenderStateGet(7, &m_shadeMode);
        RwRenderStateGet(20, &m_cullMode);
        RwRect rect;
        s16 x;
        s16 y;
        fn_80489B2C(m_frameBuffer, &x, &y);
        rect.x = x;
        rect.y = y;
        rect.w = m_frameBuffer->width;
        rect.h = m_frameBuffer->height;
        fn_80489DC4(m_raster, m_raster->parent, &rect);
        fn_80489DC4(m_raster2, m_raster2->parent, &rect);
        RwRaster *parent = m_frameBuffer->parent;
        u32 width = parent->width;
        u32 height = parent->height;
        float u[2] = {(0.5f + x) / width, (0.5f + (x + rect.w)) / width};
        float v[2] = {(0.5f + y) / height, (0.5f + (y + rect.h)) / height};
        m_quad[0].x = 0.0f;
        m_quad[0].y = 0.0f;
        m_quad[0].u = u[0];
        m_quad[0].v = v[0];
        m_quad[1].x = 0.0f;
        m_quad[1].y = rect.h;
        m_quad[1].u = u[0];
        m_quad[1].v = v[1];
        m_quad[2].x = rect.w;
        m_quad[2].y = 0.0f;
        m_quad[2].u = u[1];
        m_quad[2].v = v[0];
        m_quad[3].x = rect.w;
        m_quad[3].y = rect.h;
        m_quad[3].u = u[1];
        m_quad[3].v = v[1];
        fn_804951B8(m_raster2, 0);
        UnkFogColor_804388A4 clear = {0, 0, 0, 0};
        fn_804857BC(m_camera, &clear, 1);
        if (fn_804854DC(m_camera) != 0) {
            RwRenderStateSet(9, (void *)1);
            RwRenderStateSet(7, (void *)1);
            RwRenderStateSet(8, (void *)0);
            RwRenderStateSet(12, (void *)0);
            RwRenderStateSet(14, (void *)0);
            RwRenderStateSet(20, (void *)2);
            RwRenderStateSet(10, (void *)2);
            RwRenderStateSet(11, (void *)2);
            fn_804854B4(m_camera);
        }
    }
}

void UnkGlow_80437118::End(bool additive, int count, const UnkFogColor_804388A4 *color) {
    if (m_camera != 0 && m_raster != 0 && m_raster2 != 0) {
        for (int i = 0; i < 4; i++) {
            m_quad[i].r = 0xFF;
            m_quad[i].g = 0xFF;
            m_quad[i].b = 0xFF;
            m_quad[i].a = 0xFF;
        }
        fn_804951B8(m_raster, 0);
        if (fn_804854DC(m_camera) != 0) {
            RwRenderStateSet(20, (void *)1);
            RwRenderStateSet(1, m_raster->parent);
            RwRenderStateSet(10, (void *)2);
            RwRenderStateSet(11, (void *)2);
            if (!m_additive) {
                for (int n = count; n != 0; n /= 2) {
                    RwIm2DRenderPrimitive(4, m_quad, 4);
                }
            }
            fn_804854B4(m_camera);
        }
        fn_804951B8(m_raster, 0);
        if (fn_804854DC(m_camera) != 0) {
            RwRenderStateSet(10, (void *)2);
            RwRenderStateSet(11, (void *)1);
            RwRenderStateSet(1, m_raster2->parent);
            RwIm2DRenderPrimitive(4, m_quad, 4);
            fn_804854B4(m_camera);
        }
        int a = color->a;
        for (int i = 0; i < 4; i++) {
            m_quad[i].r = color->r;
            m_quad[i].g = color->g;
            m_quad[i].b = color->b;
            m_quad[i].a = a;
        }
        if (fn_804854DC(m_camera) != 0) {
            RwRenderStateSet(1, m_raster->parent);
            if (additive || m_additive) {
                RwRenderStateSet(10, (void *)2);
                RwRenderStateSet(11, (void *)2);
            } else {
                fn_80398D00(3, 1, 1, 0xF);
            }
            RwIm2DRenderPrimitive(4, m_quad, 4);
            RwRenderStateSet(10, (void *)m_srcBlend);
            RwRenderStateSet(11, (void *)m_destBlend);
            RwRenderStateSet(12, (void *)m_vertexAlpha);
            RwRenderStateSet(6, (void *)m_zTest);
            RwRenderStateSet(8, (void *)m_zWrite);
            RwRenderStateSet(14, (void *)m_fog);
            RwRenderStateSet(9, (void *)m_textureAddress);
            RwRenderStateSet(7, (void *)m_shadeMode);
            RwRenderStateSet(20, (void *)m_cullMode);
            fn_804854B4(m_camera);
        }
    }
}

void UnkGlow_80437118::Draw(RpClump *clump, bool additive, int count, void *color) {
    EffectSystemRW3 *system = dynamic_cast<EffectSystemRW3 *>(::Effect::System::fn_8042A5D0());
    if (system == 0) {
        return;
    }
    RwCamera *camera = static_cast<RwCamera *>(system->GetCamera());
    if (camera == 0) {
        return;
    }
    Begin();
    if (fn_804854DC(camera) != 0) {
        fn_80456BC4(clump);
        fn_804854B4(camera);
    }
    if (m_camera != 0 && m_raster != 0 && m_raster2 != 0 && fn_804854DC(m_camera) != 0) {
        RwRenderStateSet(20, (void *)3);
        if (m_additive) {
            RwRenderStateSet(10, (void *)1);
            RwRenderStateSet(11, (void *)2);
        } else {
            fn_80398D00(3, 1, 1, 0xF);
        }
        fn_804854B4(m_camera);
    }
    if (fn_804854DC(camera) != 0) {
        fn_80456BC4(clump);
        fn_804854B4(camera);
    }
    End(additive, count, static_cast<const UnkFogColor_804388A4 *>(color));
}
