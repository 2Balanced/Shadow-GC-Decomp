/* A screen copy of the RenderWare effect manager (no RTTI; class and file names invented): the
 * effect manager keeps one at 0x40 and Effect::RW3::EffectGlobalParamRW3 drives it. Capture
 * copies the camera's frame buffer into a half-size raster, and Draw puts it back over the screen,
 * scaled about the centre and blended with the given alpha, which gives a zoom blur. Member and
 * function names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#define EFFECT_ELEMENT_POLYMORPHIC

#include "Effect/System/EffectManager.h"
#include "Effect/System/EffectSystem.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* RenderWare's immediate-mode 2D vertex, raster, camera and engine globals (rw/rwcore.h; as in
 * Game/AdvStaffRoll.h). */
struct RwIm2DVertex_8042DA00 {
    /* 0x00 */ float x;
    /* 0x04 */ float y;
    /* 0x08 */ float z;
    /* 0x0C */ u8 red; /* emissiveColor */
    /* 0x0D */ u8 green;
    /* 0x0E */ u8 blue;
    /* 0x0F */ u8 alpha;
    /* 0x10 */ float u;
    /* 0x14 */ float v;
};

struct RwRaster_8042DA00 {
    /* 0x00 */ RwRaster_8042DA00 *parent;
    /* 0x04 */ u8 field_0x4[0xC - 0x4];
    /* 0x0C */ s32 width;
    /* 0x10 */ s32 height;
};

struct RwRect_8042DA00 {
    s32 x;
    s32 y;
    s32 w;
    s32 h;
};

struct RwV2d_8042DA00 {
    float x;
    float y;
};

struct RwCamera_8042DA00 {
    /* 0x00 */ u8 field_0x0[0x60];
    /* 0x60 */ RwRaster_8042DA00 *frameBuffer;
};

typedef int (*RwRenderStateFunction_8042DA00)(int state, void *value);
typedef int (*RwIm2DRenderPrimitiveFunction_8042DA00)(int primType,
                                                       RwIm2DVertex_8042DA00 *vertices, int count);

struct RwGlobals_8042DA00 {
    /* 0x00 */ u8 field_0x0[0x18];
    /* 0x18 */ float zBufferNear;
    /* 0x1C */ u8 field_0x1C[0x20 - 0x1C];
    /* 0x20 */ RwRenderStateFunction_8042DA00 fpRenderStateSet;
    /* 0x24 */ RwRenderStateFunction_8042DA00 fpRenderStateGet;
    /* 0x28 */ u8 field_0x28[0x30 - 0x28];
    /* 0x30 */ RwIm2DRenderPrimitiveFunction_8042DA00 fpIm2DRenderPrimitive;
};

extern "C" {
/* The engine instance. */
extern RwGlobals_8042DA00 *lbl_805F15B4;
/* RwRasterCreate, RwRasterSubRaster, RwRasterDestroy, RwRasterGetOffset, RwCameraBeginUpdate and
 * RwCameraEndUpdate (behaviour inferred), and a raster call that prepares the raster for
 * rendering into (0x804951B8; purpose not known). */
RwRaster_8042DA00 *fn_80489E74(s32 width, s32 height, s32 depth, s32 flags);
RwRaster_8042DA00 *fn_80489DC4(RwRaster_8042DA00 *subRaster, RwRaster_8042DA00 *raster,
                               RwRect_8042DA00 *rect);
int fn_80489BDC(RwRaster_8042DA00 *raster);
RwRaster_8042DA00 *fn_80489B2C(RwRaster_8042DA00 *raster, s16 *x, s16 *y);
RwCamera_8042DA00 *fn_804854DC(RwCamera_8042DA00 *camera);
RwCamera_8042DA00 *fn_804854B4(RwCamera_8042DA00 *camera);
void fn_804951B8(RwRaster_8042DA00 *raster, int flags);
}

#define RwRenderStateSet(state, value) (lbl_805F15B4->fpRenderStateSet((state), (void *)(value)))
#define RwRenderStateGet(state, value) (lbl_805F15B4->fpRenderStateGet((state), (void *)(value)))
#define RwIm2DRenderPrimitive(prim, vertices, count)                                               \
    (lbl_805F15B4->fpIm2DRenderPrimitive((prim), (vertices), (count)))
#define RwIm2DGetNearScreenZ() (lbl_805F15B4->zBufferNear)

enum {
    rwRENDERSTATETEXTURERASTER = 1,
    rwRENDERSTATEZTESTENABLE = 6,
    rwRENDERSTATEZWRITEENABLE = 8,
    rwRENDERSTATETEXTUREADDRESS = 9,
    rwRENDERSTATESRCBLEND = 10,
    rwRENDERSTATEDESTBLEND = 11,
    rwRENDERSTATEVERTEXALPHAENABLE = 12,
    rwRENDERSTATEFOGENABLE = 14,
    rwBLENDZERO = 1,
    rwBLENDONE = 2,
    rwBLENDSRCALPHA = 5,
    rwBLENDINVSRCALPHA = 6,
    rwPRIMTYPETRISTRIP = 4,
    rwTEXTUREADDRESSWRAP = 1
};

/* Eight bytes of the settings the blur does not use (copied as one). */
struct UnkScreenBlurExtra_8042D974 {
    u32 field_0x0;
    u32 field_0x4;
};

/* The blur's settings (layout from their copies). */
struct UnkScreenBlurParam_8042D974 {
    UnkScreenBlurParam_8042D974() : enabled(0), alpha(0), scale(1.0f) {}

    /* 0x0 */ u8 enabled;
    /* 0x1 */ u8 alpha; /* how strongly the copy is blended in */
    /* 0x4 */ float scale; /* the copy's size, 1 for the screen's */
    /* 0x8 */ UnkScreenBlurExtra_8042D974 extra;
};

/* Four 2D vertices of a screen quad. */
struct UnkScreenQuad_8042DA00 {
    RwIm2DVertex_8042DA00 v[4];
};

/* The screen copy (0x8042DA00). */
class UnkScreenBlur_8042DA00 {
  public:
    UnkScreenBlur_8042DA00(::Effect::RW3::EffectManagerRW3 *manager,
                           const UnkScreenBlurParam_8042D974 *param);
    ~UnkScreenBlur_8042DA00() {
        if (m_raster) {
            if (m_raster->parent != m_raster) {
                fn_80489BDC(m_raster->parent);
            }
            fn_80489BDC(m_raster);
            m_raster = 0;
        }
    }
    void Draw();
    void Capture();
    UnkScreenBlurParam_8042D974 GetParam() const;
    void SetParam(const UnkScreenBlurParam_8042D974 &param);
    void SetParam(const UnkScreenBlurParam_8042D974 *param);

    /* 0x00 */ UnkScreenBlurParam_8042D974 m_param;
    /* 0x10 */ ::Effect::RW3::EffectManagerRW3 *m_manager;
    /* 0x14 */ UnkScreenQuad_8042DA00 m_quad;     /* draws the copy over the screen */
    /* 0x74 */ RwRaster_8042DA00 *m_raster;       /* the copy */
    /* 0x78 */ u8 m_captured;
    /* 0x7C */ UnkScreenQuad_8042DA00 m_halfQuad; /* draws the screen into the copy */
    /* 0xDC */ UnkScreenQuad_8042DA00 m_fullQuad;
};
SIZE_ASSERT(UnkScreenBlur_8042DA00, 0x13C);

/* The effect manager's members this unit uses (layout from their uses). */
struct UnkManagerView_8042E034 {
    /* 0x00 */ u8 field_0x0[0x34];
    /* 0x34 */ RwRaster_8042DA00 *screenRaster; /* the RenderWare manager's, 0x804329C4 */
    /* 0x38 */ u8 field_0x38[0x40 - 0x38];
    /* 0x40 */ UnkScreenBlur_8042DA00 *blur;
};

#define View(manager) ((UnkManagerView_8042E034 *)(manager))

using ::Effect::RW3::EffectManagerRW3;
using ::Effect::RW3::EffectSystemRW3;

extern "C" {

/* Creates the manager's screen copy. */
void fn_8042E1BC(EffectManagerRW3 *manager, const UnkScreenBlurParam_8042D974 *param) {
    View(manager)->blur = new UnkScreenBlur_8042DA00(manager, param);
}

/* Destroys it. */
void fn_8042E148(EffectManagerRW3 *manager) {
    if (View(manager)->blur) {
        delete View(manager)->blur;
        View(manager)->blur = 0;
    }
}

void fn_8042E11C(EffectManagerRW3 *manager) {
    if (View(manager)->blur) {
        View(manager)->blur->Draw();
    }
}

void fn_8042E0F0(EffectManagerRW3 *manager) {
    if (View(manager)->blur) {
        View(manager)->blur->Capture();
    }
}

void fn_8042E0C4(EffectManagerRW3 *manager, const UnkScreenBlurParam_8042D974 *param) {
    if (View(manager)->blur) {
        View(manager)->blur->SetParam(param);
    }
}

void fn_8042E098(EffectManagerRW3 *manager, const UnkScreenBlurParam_8042D974 &param) {
    if (View(manager)->blur) {
        View(manager)->blur->SetParam(param);
    }
}

UnkScreenBlurParam_8042D974 fn_8042E034(EffectManagerRW3 *manager) {
    if (View(manager)->blur) {
        return View(manager)->blur->GetParam();
    }
    UnkScreenBlurParam_8042D974 param;
    return param;
}

}

UnkScreenBlur_8042DA00::UnkScreenBlur_8042DA00(EffectManagerRW3 *manager,
                                               const UnkScreenBlurParam_8042D974 *param)
    : m_manager(manager) {
    EffectSystemRW3 *system = dynamic_cast<EffectSystemRW3 *>(::Effect::System::fn_8042A5D0());
    if (!system) {
        return;
    }
    RwCamera_8042DA00 *camera = (RwCamera_8042DA00 *)system->GetCamera();
    if (!camera) {
        return;
    }
    m_quad.v[0].z = RwIm2DGetNearScreenZ();
    m_quad.v[0].red = 0xFF;
    m_quad.v[0].green = 0xFF;
    m_quad.v[0].blue = 0xFF;
    m_quad.v[0].alpha = 0xFF;
    m_quad.v[1].z = RwIm2DGetNearScreenZ();
    m_quad.v[1].red = 0xFF;
    m_quad.v[1].green = 0xFF;
    m_quad.v[1].blue = 0xFF;
    m_quad.v[1].alpha = 0xFF;
    m_quad.v[2].z = RwIm2DGetNearScreenZ();
    m_quad.v[2].red = 0xFF;
    m_quad.v[2].green = 0xFF;
    m_quad.v[2].blue = 0xFF;
    m_quad.v[2].alpha = 0xFF;
    m_quad.v[3].z = RwIm2DGetNearScreenZ();
    m_quad.v[3].red = 0xFF;
    m_quad.v[3].green = 0xFF;
    m_quad.v[3].blue = 0xFF;
    m_quad.v[3].alpha = 0xFF;
    m_raster = 0;
    m_captured = 0;
    RwRaster_8042DA00 *screen = camera->frameBuffer->parent;
    s32 width = screen->width;
    s32 height = screen->height;
    RwRect_8042DA00 rect = {0, 0, width, height};
    RwV2d_8042DA00 u = {0.5f / (u32)width, (0.5f + width) / (u32)width};
    RwV2d_8042DA00 v = {0.5f / (u32)height, (0.5f + height) / (u32)height};
    m_fullQuad.v[0] = m_quad.v[0];
    m_fullQuad.v[1] = m_quad.v[1];
    m_fullQuad.v[2] = m_quad.v[2];
    m_fullQuad.v[3] = m_quad.v[3];
    m_fullQuad.v[0].x = 0.0f;
    m_fullQuad.v[0].y = 0.0f;
    m_fullQuad.v[0].u = u.x;
    m_fullQuad.v[0].v = v.x;
    m_fullQuad.v[1].x = 0.0f;
    m_fullQuad.v[1].y = rect.h;
    m_fullQuad.v[1].u = u.x;
    m_fullQuad.v[1].v = v.y;
    m_fullQuad.v[2].x = rect.w;
    m_fullQuad.v[2].y = 0.0f;
    m_fullQuad.v[2].u = u.y;
    m_fullQuad.v[2].v = v.x;
    m_fullQuad.v[3].x = rect.w;
    m_fullQuad.v[3].y = rect.h;
    m_fullQuad.v[3].u = u.y;
    m_fullQuad.v[3].v = v.y;
    rect.w /= 2;
    rect.h /= 2;
    m_halfQuad.v[0] = m_fullQuad.v[0];
    m_halfQuad.v[1] = m_fullQuad.v[1];
    m_halfQuad.v[2] = m_fullQuad.v[2];
    m_halfQuad.v[3] = m_fullQuad.v[3];
    m_halfQuad.v[0].x = 0.0f;
    m_halfQuad.v[0].y = 0.0f;
    m_halfQuad.v[1].x = 0.0f;
    m_halfQuad.v[1].y = rect.h;
    m_halfQuad.v[2].x = rect.w;
    m_halfQuad.v[2].y = 0.0f;
    m_halfQuad.v[3].x = rect.w;
    m_halfQuad.v[3].y = rect.h;
    m_raster = fn_80489E74(0, 0, 0, 0x505);
    fn_80489DC4(m_raster, fn_80489E74(rect.w, rect.h, 0, 0x505), &rect);
    if (param) {
        SetParam(param);
    } else {
        m_param.enabled = 0;
        m_param.alpha = 0x80;
        m_param.scale = 1.0f;
    }
}

void UnkScreenBlur_8042DA00::SetParam(const UnkScreenBlurParam_8042D974 *param) {
    if (param) {
        m_param = *param;
    }
}

void UnkScreenBlur_8042DA00::SetParam(const UnkScreenBlurParam_8042D974 &param) {
    m_param = param;
}

#pragma scheduling off
UnkScreenBlurParam_8042D974 UnkScreenBlur_8042DA00::GetParam() const {
    return m_param;
}
#pragma scheduling reset

void UnkScreenBlur_8042DA00::Capture() {
    m_captured = 0;
    if (!m_param.enabled) {
        return;
    }
    if (!m_param.alpha) {
        return;
    }
    EffectSystemRW3 *system = dynamic_cast<EffectSystemRW3 *>(::Effect::System::fn_8042A5D0());
    if (!system) {
        return;
    }
    RwCamera_8042DA00 *camera = (RwCamera_8042DA00 *)system->GetCamera();
    if (!camera) {
        return;
    }
    RwRaster_8042DA00 *screen = View(m_manager)->screenRaster->parent;
    fn_804951B8(screen, 0);
    if (fn_804854DC(camera)) {
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, screen);
        RwRenderStateSet(rwRENDERSTATESRCBLEND, rwBLENDONE);
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, rwBLENDZERO);
        RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, m_halfQuad.v, 4);
        fn_804854B4(camera);
    }
    fn_804951B8(m_raster, 0);
    if (fn_804854DC(camera)) {
        RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, m_fullQuad.v, 4);
        fn_804854B4(camera);
    }
    m_captured = 1;
}

void UnkScreenBlur_8042DA00::Draw() {
    if (!m_param.enabled) {
        return;
    }
    if (!m_param.alpha) {
        return;
    }
    EffectSystemRW3 *system = dynamic_cast<EffectSystemRW3 *>(::Effect::System::fn_8042A5D0());
    if (!system) {
        return;
    }
    RwCamera_8042DA00 *camera = (RwCamera_8042DA00 *)system->GetCamera();
    if (!camera) {
        return;
    }
    if (!m_captured) {
        return;
    }
    int srcBlend;
    int destBlend;
    int vertexAlpha;
    int zTest;
    int zWrite;
    int fog;
    int textureAddress;
    RwRenderStateGet(rwRENDERSTATESRCBLEND, &srcBlend);
    RwRenderStateGet(rwRENDERSTATEDESTBLEND, &destBlend);
    RwRenderStateGet(rwRENDERSTATEVERTEXALPHAENABLE, &vertexAlpha);
    RwRenderStateGet(rwRENDERSTATEZTESTENABLE, &zTest);
    RwRenderStateGet(rwRENDERSTATEZWRITEENABLE, &zWrite);
    RwRenderStateGet(rwRENDERSTATEFOGENABLE, &fog);
    RwRenderStateGet(rwRENDERSTATETEXTUREADDRESS, &textureAddress);
    RwRaster_8042DA00 *frameBuffer = camera->frameBuffer;
    s16 x;
    s16 y;
    fn_80489B2C(frameBuffer, &x, &y);
    s32 width = frameBuffer->width;
    s32 height = frameBuffer->height;
    RwRaster_8042DA00 *screen = frameBuffer->parent;
    RwV2d_8042DA00 u = {(0.5f + x) / (u32)screen->width, (0.5f + (x + width)) / (u32)screen->width};
    RwV2d_8042DA00 v = {(0.5f + y) / (u32)screen->height,
                        (0.5f + (y + height)) / (u32)screen->height};
    m_quad.v[0].u = u.x;
    m_quad.v[0].v = v.x;
    m_quad.v[1].u = u.x;
    m_quad.v[1].v = v.y;
    m_quad.v[2].u = u.y;
    m_quad.v[2].v = v.x;
    m_quad.v[3].u = u.y;
    m_quad.v[3].v = v.y;
    m_quad.v[0].red = 0xFF;
    m_quad.v[0].green = 0xFF;
    m_quad.v[0].blue = 0xFF;
    m_quad.v[0].alpha = m_param.alpha;
    m_quad.v[1].red = 0xFF;
    m_quad.v[1].green = 0xFF;
    m_quad.v[1].blue = 0xFF;
    m_quad.v[1].alpha = m_param.alpha;
    m_quad.v[2].red = 0xFF;
    m_quad.v[2].green = 0xFF;
    m_quad.v[2].blue = 0xFF;
    m_quad.v[2].alpha = m_param.alpha;
    m_quad.v[3].red = 0xFF;
    m_quad.v[3].green = 0xFF;
    m_quad.v[3].blue = 0xFF;
    m_quad.v[3].alpha = m_param.alpha;
    float dx = 0.5f * ((m_param.scale - 1.0f) * width);
    float dy = 0.5f * ((m_param.scale - 1.0f) * height);
    m_quad.v[0].x = -dx;
    m_quad.v[0].y = -dy;
    m_quad.v[1].x = -dx;
    m_quad.v[1].y = height + dy;
    m_quad.v[2].x = width + dx;
    m_quad.v[2].y = -dy;
    m_quad.v[3].x = width + dx;
    m_quad.v[3].y = height + dy;
    RwRaster_8042DA00 *copy = m_raster->parent;
    if (!fn_804854DC(camera)) {
        return;
    }
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, copy);
    RwRenderStateSet(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, 1);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, 0);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, 0);
    RwRenderStateSet(rwRENDERSTATEFOGENABLE, 0);
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, rwTEXTUREADDRESSWRAP);
    RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, m_quad.v, 4);
    RwRenderStateSet(rwRENDERSTATESRCBLEND, srcBlend);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, destBlend);
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, vertexAlpha);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, zTest);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, zWrite);
    RwRenderStateSet(rwRENDERSTATEFOGENABLE, fog);
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, textureAddress);
    fn_804854B4(camera);
}
