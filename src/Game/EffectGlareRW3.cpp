/* The glare of the RenderWare effect manager (no RTTI; class and file names invented): the effect
 * manager keeps one at 0x3C, and the "PreGlare" and "PostGlare" tasks of Game/EffectTask.cpp drive
 * it. PreGlare saves the camera's frame buffer and sets up the texture coordinates of the copies;
 * PostGlare shrinks the frame into two smaller copies (128 and 64 pixels), blurs them by drawing
 * them over themselves at growing offsets, and adds them back over the screen. Member and
 * function names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include "Effect/System/EffectSystem.h"
#include <dolphin/types.h>
#include "size_assert.h"

namespace Effect {
namespace RW3 {
class EffectManagerRW3;
}
} // namespace Effect

/* RenderWare's immediate-mode 2D vertex, raster, camera, colour, rectangle and engine globals
 * (rw/rwcore.h; the fields used here). */
struct RwIm2DVertex_8042E208 {
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

struct RwRaster_8042E208 {
    /* 0x00 */ RwRaster_8042E208 *parent;
    /* 0x04 */ u8 field_0x4[0xC - 0x4];
    /* 0x0C */ s32 width;
    /* 0x10 */ s32 height;
};

struct RwCamera_8042E208 {
    /* 0x00 */ u8 field_0x0[0x60];
    /* 0x60 */ RwRaster_8042E208 *frameBuffer;
    /* 0x64 */ RwRaster_8042E208 *zBuffer;
    /* 0x68 */ u8 field_0x68[0x80 - 0x68];
    /* 0x80 */ float farPlane;
};

struct RwRGBA_8042E208 {
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
};

struct RwRect_8042E208 {
    s32 x;
    s32 y;
    s32 w;
    s32 h;
};

typedef int (*RwRenderStateFunction_8042E208)(int state, void *value);
typedef int (*RwIm2DRenderPrimitiveFunction_8042E208)(int primType,
                                                       RwIm2DVertex_8042E208 *vertices, int count);

struct RwGlobals_8042E208 {
    /* 0x00 */ u8 field_0x0[0x18];
    /* 0x18 */ float zBufferNear;
    /* 0x1C */ u8 field_0x1C[0x20 - 0x1C];
    /* 0x20 */ RwRenderStateFunction_8042E208 fpRenderStateSet;
    /* 0x24 */ RwRenderStateFunction_8042E208 fpRenderStateGet;
    /* 0x28 */ u8 field_0x28[0x30 - 0x28];
    /* 0x30 */ RwIm2DRenderPrimitiveFunction_8042E208 fpIm2DRenderPrimitive;
};

extern "C" {
/* The engine instance. */
extern RwGlobals_8042E208 *lbl_805F15B4;
/* RwRasterCreate, RwRasterSubRaster, RwRasterDestroy, RwRasterGetOffset, RwCameraBeginUpdate,
 * RwCameraEndUpdate and RwCameraClear (behaviour inferred), a raster call that copies the frame
 * into a raster (0x804951B8; purpose inferred), and the camera's world and its removal from it
 * (as in Game/CameraRender.cpp). */
RwRaster_8042E208 *fn_80489E74(s32 width, s32 height, s32 depth, s32 flags);
RwRaster_8042E208 *fn_80489DC4(RwRaster_8042E208 *subRaster, RwRaster_8042E208 *raster,
                               RwRect_8042E208 *rect);
int fn_80489BDC(RwRaster_8042E208 *raster);
RwRaster_8042E208 *fn_80489B2C(RwRaster_8042E208 *raster, s16 *x, s16 *y);
RwCamera_8042E208 *fn_804854DC(RwCamera_8042E208 *camera);
RwCamera_8042E208 *fn_804854B4(RwCamera_8042E208 *camera);
RwCamera_8042E208 *fn_804857BC(RwCamera_8042E208 *camera, RwRGBA_8042E208 *color, int clearMode);
void fn_804951B8(RwRaster_8042E208 *raster, int flags);
void *fn_804621B0(RwCamera_8042E208 *camera);
void fn_80462184(void *world, RwCamera_8042E208 *camera);
}

#define RwRenderStateSet(state, value) (lbl_805F15B4->fpRenderStateSet((state), (void *)(value)))
#define RwRenderStateGet(state, value) (lbl_805F15B4->fpRenderStateGet((state), (void *)(value)))
#define RwIm2DRenderPrimitive(prim, vertices, count)                                               \
    (lbl_805F15B4->fpIm2DRenderPrimitive((prim), (vertices), (count)))
#define RwIm2DGetNearScreenZ() (lbl_805F15B4->zBufferNear)

enum {
    rwRENDERSTATETEXTURERASTER_8042E208 = 1,
    rwRENDERSTATETEXTUREADDRESS_8042E208 = 2,
    rwRENDERSTATEZTESTENABLE_8042E208 = 6,
    rwRENDERSTATESHADEMODE_8042E208 = 7,
    rwRENDERSTATEZWRITEENABLE_8042E208 = 8,
    rwRENDERSTATETEXTUREFILTER_8042E208 = 9,
    rwRENDERSTATESRCBLEND_8042E208 = 10,
    rwRENDERSTATEDESTBLEND_8042E208 = 11,
    rwRENDERSTATEVERTEXALPHAENABLE_8042E208 = 12
};

/* The glare's settings (layout from their copies). */
struct UnkGlareParam_80430524 {
    UnkGlareParam_80430524()
        : enabled(0), large(0), small(0), separate(0), smallFromScreen(0), largeColor(0),
          smallColor(0), largeStep(0.0f), smallStep(0.0f), largeCountX(0), largeCountY(0),
          smallCountX(0), smallCountY(0) {}

    /* 0x00 */ u8 enabled;
    /* 0x01 */ u8 large;           /* the 128-pixel copy */
    /* 0x02 */ u8 small;           /* the 64-pixel copy */
    /* 0x03 */ u8 separate;        /* add each copy to the screen on its own */
    /* 0x04 */ u8 smallFromScreen; /* shrink the screen, not the large copy, into the small one */
    /* 0x05 */ u8 largeColor;      /* how bright the shrunk copies are */
    /* 0x06 */ u8 smallColor;
    /* 0x08 */ float largeStep;    /* the blur's offset step, in pixels */
    /* 0x0C */ float smallStep;
    /* 0x10 */ s32 largeCountX;    /* blur passes across and down */
    /* 0x14 */ s32 largeCountY;
    /* 0x18 */ s32 smallCountX;
    /* 0x1C */ s32 smallCountY;
};

/* The glare (0x80430524). Level 0 is the screen, 1 the large copy and 2 the small one; each level
 * has two rasters, the second one used when the camera's raster is offset in its parent. */
class UnkGlare_80430524 {
  public:
    UnkGlare_80430524(::Effect::RW3::EffectManagerRW3 *manager,
                      const UnkGlareParam_80430524 *param);
    ~UnkGlare_80430524() {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 2; j++) {
                if (m_owned[i][j]) {
                    if (m_owned[i][j]->parent != m_owned[i][j]) {
                        fn_80489BDC(m_owned[i][j]->parent);
                    }
                    fn_80489BDC(m_owned[i][j]);
                    m_owned[i][j] = 0;
                }
            }
        }
        if (m_raster) {
            fn_80489BDC(m_raster);
            m_raster = 0;
        }
    }
    void Draw();
    void Blur(RwRaster_8042E208 **rasters, s32 *index, s32 countX, s32 countY, float step);
    void SetQuad(float x, float y, float w, float h, float u0, float v0, float u1, float v1);
    void PostGlare();
    void PreGlare();
    UnkGlareParam_80430524 GetParam();
    void SetParam(const UnkGlareParam_80430524 &param);
    void SetParam(const UnkGlareParam_80430524 *param);

    inline void Add(RwRaster_8042E208 *dims, RwRaster_8042E208 *texture) {
        if (fn_804854DC(m_camera)) {
            SetQuad(0.0f, 0.0f, dims->width, dims->height, m_u[m_index[0]][0], m_v[m_index[0]][0],
                    m_u[m_index[0]][1], m_v[m_index[0]][1]);
            RwRenderStateSet(rwRENDERSTATETEXTURERASTER_8042E208, texture->parent);
            RwRenderStateSet(rwRENDERSTATESRCBLEND_8042E208, 2);
            RwRenderStateSet(rwRENDERSTATEDESTBLEND_8042E208, 2);
            RwIm2DRenderPrimitive(4, m_quad, 4);
            fn_804854B4(m_camera);
        }
    }

    inline void SetColor(u8 r, u8 g, u8 b, u8 a) {
        for (int i = 0; i < 4; i++) {
            m_quad[i].red = r;
            m_quad[i].green = g;
            m_quad[i].blue = b;
            m_quad[i].alpha = a;
        }
    }

    /* 0x00 */ RwRaster_8042E208 *m_frameBuffer; /* the camera's, saved by PreGlare */
    /* 0x04 */ RwRaster_8042E208 *m_zBuffer;
    /* 0x08 */ RwRaster_8042E208 *m_rasters[3][2];
    /* 0x20 */ RwRaster_8042E208 *m_owned[3][2]; /* the ones the glare created */
    /* 0x38 */ s32 m_index[3]; /* the level being drawn, then the raster in use of levels 1, 2 */
    /* 0x44 */ RwRaster_8042E208 *m_raster;
    /* 0x48 */ RwIm2DVertex_8042E208 m_quad[4];
    /* 0xA8 */ float m_u[3][2]; /* each level's texture coordinates */
    /* 0xC0 */ float m_v[3][2];
    /* 0xD8 */ float m_invFar;
    /* 0xDC */ RwCamera_8042E208 *m_camera;
    /* 0xE0 */ UnkGlareParam_80430524 m_param;
    /* 0x100 */ ::Effect::RW3::EffectManagerRW3 *m_manager;
};
SIZE_ASSERT(UnkGlare_80430524, 0x104);

/* The effect manager's members this unit uses (layout from their uses). */
struct UnkManagerView_80430A08 {
    /* 0x00 */ u8 field_0x0[0x34];
    /* 0x34 */ RwRaster_8042E208 *screenRaster; /* the RenderWare manager's, 0x804325E8 */
    /* 0x38 */ RwRaster_8042E208 *screenRaster2;
    /* 0x3C */ UnkGlare_80430524 *glare;
};

#define View(manager) ((UnkManagerView_80430A08 *)(manager))

using ::Effect::RW3::EffectManagerRW3;
using ::Effect::RW3::EffectSystemRW3;

/* How much of the screen's brightness the shrunk copies keep. */
float lbl_8056AC30 = 0.95f;

extern "C" {

/* Creates the manager's glare. */
void fn_80430A08(EffectManagerRW3 *manager, const UnkGlareParam_80430524 *param) {
    View(manager)->glare = new UnkGlare_80430524(manager, param);
}

/* Destroys it. */
void fn_80430944(EffectManagerRW3 *manager) {
    if (View(manager)->glare) {
        delete View(manager)->glare;
        View(manager)->glare = 0;
    }
}

void fn_80430918(EffectManagerRW3 *manager) {
    if (View(manager)->glare) {
        View(manager)->glare->PreGlare();
    }
}

void fn_804308EC(EffectManagerRW3 *manager) {
    if (View(manager)->glare) {
        View(manager)->glare->PostGlare();
    }
}

void fn_804308C0(EffectManagerRW3 *manager, const UnkGlareParam_80430524 &param) {
    if (View(manager)->glare) {
        View(manager)->glare->SetParam(param);
    }
}

UnkGlareParam_80430524 fn_80430844(EffectManagerRW3 *manager) {
    if (View(manager)->glare) {
        return View(manager)->glare->GetParam();
    }
    UnkGlareParam_80430524 param;
    return param;
}

}

UnkGlare_80430524::UnkGlare_80430524(EffectManagerRW3 *manager,
                                     const UnkGlareParam_80430524 *param)
    : m_manager(manager) {
    EffectSystemRW3 *system = dynamic_cast<EffectSystemRW3 *>(::Effect::System::fn_8042A5D0());
    if (!system) {
        return;
    }
    if (!(m_camera = (RwCamera_8042E208 *)system->GetCamera())) {
        return;
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            m_owned[i][j] = 0;
            m_rasters[i][j] = 0;
        }
        m_index[i] = 0;
    }
    m_raster = 0;
    RwRect_8042E208 rect = {0, 0, 0, 0};
    rect.w = m_camera->frameBuffer->width;
    rect.h = m_camera->frameBuffer->height;
    if (View(manager)->screenRaster) {
        m_rasters[0][0] = View(manager)->screenRaster;
    } else {
        m_rasters[0][0] = m_owned[0][0] = fn_80489E74(0, 0, 0, 5);
        fn_80489DC4(m_owned[0][0], fn_80489E74(rect.w, rect.h, 0, 5), &rect);
    }
    m_rasters[1][0] = m_owned[1][0] = fn_80489E74(0x80, 0x80, 0, 0x505);
    m_rasters[2][0] = m_owned[2][0] = fn_80489E74(0x40, 0x40, 0, 0x505);
    if (View(manager)->screenRaster2) {
        m_rasters[0][1] = View(manager)->screenRaster2;
    } else {
        m_rasters[0][1] = m_owned[0][1] = fn_80489E74(0, 0, 0, 5);
        fn_80489DC4(m_owned[0][1], fn_80489E74(rect.w, rect.h, 0, 5), &rect);
    }
    if (!m_rasters[0][0]) {
        return;
    }
    m_invFar = 1.0f / m_camera->farPlane;
    for (int i = 0; i < 4; i++) {
        m_quad[i].z = RwIm2DGetNearScreenZ();
        m_quad[i].red = 0xFF;
        m_quad[i].green = 0xFF;
        m_quad[i].blue = 0xFF;
        m_quad[i].alpha = 0xFF;
    }
    if (param) {
        SetParam(param);
    } else {
        m_param.enabled = 1;
        m_param.separate = 0;
        m_param.smallFromScreen = 0;
        m_param.large = 1;
        m_param.largeColor = 0x20;
        m_param.largeStep = 1.0f;
        m_param.largeCountX = 2;
        m_param.largeCountY = 2;
        m_param.small = 1;
        m_param.smallColor = 0x30;
        m_param.smallStep = 1.0f;
        m_param.smallCountX = 4;
        m_param.smallCountY = 4;
    }
}

void UnkGlare_80430524::SetParam(const UnkGlareParam_80430524 *param) {
    if (param) {
        m_param = *param;
    }
}

void UnkGlare_80430524::SetParam(const UnkGlareParam_80430524 &param) {
    m_param = param;
}

UnkGlareParam_80430524 UnkGlare_80430524::GetParam() {
    return m_param;
}

void UnkGlare_80430524::PreGlare() {
    if (!m_rasters[0][0]) {
        return;
    }
    if (!m_param.enabled) {
        return;
    }
    if (!m_param.large && !m_param.small) {
        return;
    }
    EffectSystemRW3 *system = dynamic_cast<EffectSystemRW3 *>(::Effect::System::fn_8042A5D0());
    if (!system) {
        return;
    }
    if (!(m_camera = (RwCamera_8042E208 *)system->GetCamera())) {
        return;
    }
    RwRect_8042E208 rect;
    s16 x;
    s16 y;
    m_frameBuffer = m_camera->frameBuffer;
    m_zBuffer = m_camera->zBuffer;
    fn_80489B2C(m_frameBuffer, &x, &y);
    rect.x = x;
    rect.y = y;
    rect.w = m_frameBuffer->width;
    rect.h = m_frameBuffer->height;
    fn_80489DC4(m_rasters[0][0], m_rasters[0][0]->parent, &rect);
    fn_80489DC4(m_rasters[0][1], m_rasters[0][1]->parent, &rect);
    s32 width = m_frameBuffer->parent->width;
    s32 height = m_frameBuffer->parent->height;
    m_u[0][0] = (0.5f + x) / (u32)width;
    m_v[0][0] = (0.5f + y) / (u32)height;
    m_u[0][1] = (0.5f + (x + rect.w)) / (u32)width;
    m_v[0][1] = (0.5f + (y + rect.h)) / (u32)height;
    m_u[1][0] = m_v[1][0] = 0.0f;
    m_u[1][1] = m_v[1][1] = 1.0f;
    m_u[2][0] = m_v[2][0] = 0.0f;
    m_u[2][1] = m_v[2][1] = 1.0f;
    fn_804951B8(m_rasters[0][1], 0);
    if (x || y) {
        rect.x = x;
        rect.y = y;
        rect.w = 0x80;
        rect.h = 0x80;
        if (m_rasters[1][1]) {
            if (m_owned[1][1]->parent->width != x + 0x80 ||
                m_owned[1][1]->parent->height != y + 0x80) {
                if (m_owned[1][1]->parent != m_owned[1][1]) {
                    fn_80489BDC(m_owned[1][1]->parent);
                }
                fn_80489BDC(m_owned[1][1]);
                m_owned[1][1] = fn_80489E74(0, 0, 0, 0x505);
                fn_80489DC4(m_owned[1][1], fn_80489E74(x + 0x80, y + 0x80, 0, 0x505), &rect);
            }
        } else {
            m_owned[1][1] = fn_80489E74(0, 0, 0, 0x505);
            fn_80489DC4(m_owned[1][1], fn_80489E74(x + 0x80, y + 0x80, 0, 0x505), &rect);
        }
        m_rasters[1][1] = m_owned[1][1];
        m_u[1][0] = (x ? 0.5f + x : x) / (x + 0x80);
        m_v[1][0] = (y ? 0.5f + y : y) / (y + 0x80);
        m_v[1][1] = 1.0f;
        m_u[1][1] = 1.0f;
        rect.w = 0x40;
        rect.h = 0x40;
        if (m_rasters[2][1]) {
            if (m_owned[2][1]->parent->width != x + 0x40 ||
                m_owned[2][1]->parent->height != y + 0x40) {
                if (m_owned[2][1]->parent != m_owned[2][1]) {
                    fn_80489BDC(m_owned[2][1]->parent);
                }
                fn_80489BDC(m_owned[2][1]);
                m_owned[2][1] = fn_80489E74(0, 0, 0, 0x505);
                fn_80489DC4(m_owned[2][1], fn_80489E74(x + 0x40, y + 0x40, 0, 0x505), &rect);
            }
        } else {
            m_owned[2][1] = fn_80489E74(0, 0, 0, 0x505);
            fn_80489DC4(m_owned[2][1], fn_80489E74(x + 0x40, y + 0x40, 0, 0x505), &rect);
        }
        m_rasters[2][1] = m_owned[2][1];
        m_u[2][0] = (x ? 0.5f + x : x) / (x + 0x40);
        m_v[2][0] = (y ? 0.5f + y : y) / (y + 0x40);
        m_v[2][1] = 1.0f;
        m_u[2][1] = 1.0f;
        m_index[1] = 1;
        m_index[2] = 1;
    } else {
        m_index[1] = 0;
        m_index[2] = 0;
    }
    RwRGBA_8042E208 clear = {0, 0, 0, 0};
    fn_804857BC(m_camera, &clear, 1);
    void *world = fn_804621B0(m_camera);
    if (world) {
        fn_80462184(world, m_camera);
    }
}

void UnkGlare_80430524::PostGlare() {
    if (!m_rasters[0][0]) {
        return;
    }
    if (!m_param.enabled) {
        return;
    }
    if (!m_param.large && !m_param.small) {
        return;
    }
    EffectSystemRW3 *system = dynamic_cast<EffectSystemRW3 *>(::Effect::System::fn_8042A5D0());
    if (!system) {
        return;
    }
    if (!(m_camera = (RwCamera_8042E208 *)system->GetCamera())) {
        return;
    }
    int srcBlend;
    int destBlend;
    int zTest;
    int zWrite;
    int textureAddress;
    int vertexAlpha;
    int shadeMode;
    int textureFilter;
    RwRenderStateGet(rwRENDERSTATESRCBLEND_8042E208, &srcBlend);
    RwRenderStateGet(rwRENDERSTATEDESTBLEND_8042E208, &destBlend);
    RwRenderStateGet(rwRENDERSTATEZTESTENABLE_8042E208, &zTest);
    RwRenderStateGet(rwRENDERSTATEZWRITEENABLE_8042E208, &zWrite);
    RwRenderStateGet(rwRENDERSTATETEXTUREADDRESS_8042E208, &textureAddress);
    RwRenderStateGet(rwRENDERSTATEVERTEXALPHAENABLE_8042E208, &vertexAlpha);
    RwRenderStateGet(rwRENDERSTATESHADEMODE_8042E208, &shadeMode);
    RwRenderStateGet(rwRENDERSTATETEXTUREFILTER_8042E208, &textureFilter);
    m_camera->zBuffer = 0;
    if (fn_804854DC(m_camera)) {
        RwRenderStateSet(rwRENDERSTATEZTESTENABLE_8042E208, 0);
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE_8042E208, 0);
        RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS_8042E208, 3);
        RwRenderStateSet(rwRENDERSTATESHADEMODE_8042E208, 2);
        RwRenderStateSet(rwRENDERSTATETEXTUREFILTER_8042E208, 2);
        fn_804854B4(m_camera);
    }
    fn_804951B8(m_rasters[0][0], 0);
    Draw();
    if (!fn_804854DC(m_camera)) {
        return;
    }
    SetQuad(0.0f, 0.0f, m_frameBuffer->width, m_frameBuffer->height, m_u[0][0], m_v[0][0],
            m_u[0][1], m_v[0][1]);
    RwRenderStateSet(rwRENDERSTATESRCBLEND_8042E208, 2);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND_8042E208, 2);
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER_8042E208, m_rasters[0][1]->parent);
    RwIm2DRenderPrimitive(4, m_quad, 4);
    RwRenderStateSet(rwRENDERSTATESRCBLEND_8042E208, srcBlend);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND_8042E208, destBlend);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE_8042E208, zTest);
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS_8042E208, textureAddress);
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE_8042E208, vertexAlpha);
    RwRenderStateSet(rwRENDERSTATESHADEMODE_8042E208, shadeMode);
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER_8042E208, textureFilter);
    fn_804854B4(m_camera);
}

void UnkGlare_80430524::SetQuad(float x, float y, float w, float h, float u0, float v0, float u1,
                                float v1) {
    float left = 0.0f;
    if (x >= left) {
        left = x;
        u1 -= (u1 - u0) * (x / w);
    } else {
        u0 -= (u1 - u0) * (x / w);
        w += x;
    }
    float top = 0.0f;
    if (y >= top) {
        top = y;
        v1 -= (v1 - v0) * (y / h);
    } else {
        v0 -= (v1 - v0) * (y / h);
        h += y;
    }
    m_quad[0].x = left;
    m_quad[0].y = top;
    m_quad[0].u = u0;
    m_quad[0].v = v0;
    m_quad[1].x = left;
    m_quad[1].y = h;
    m_quad[1].u = u0;
    m_quad[1].v = v1;
    m_quad[2].x = w;
    m_quad[2].y = top;
    m_quad[2].u = u1;
    m_quad[2].v = v0;
    m_quad[3].x = w;
    m_quad[3].y = h;
    m_quad[3].u = u1;
    m_quad[3].v = v1;
}

void UnkGlare_80430524::Blur(RwRaster_8042E208 **rasters, s32 *index, s32 countX, s32 countY,
                             float step) {
    RwRGBA_8042E208 clear = {0, 0, 0, 0xFF};
    if (countX) {
        fn_804857BC(m_camera, &clear, 1);
        if (fn_804854DC(m_camera)) {
            RwRenderStateSet(rwRENDERSTATESRCBLEND_8042E208, 2);
            RwRenderStateSet(rwRENDERSTATEDESTBLEND_8042E208, 2);
            RwRenderStateSet(rwRENDERSTATETEXTURERASTER_8042E208, rasters[*index]->parent);
            SetQuad(-0.5f, -0.5f, rasters[*index]->width, rasters[*index]->height,
                    m_u[m_index[0]][0], m_v[m_index[0]][0], m_u[m_index[0]][1],
                    m_v[m_index[0]][1]);
            RwIm2DRenderPrimitive(4, m_quad, 4);
            float sign = 1.0f;
            for (int i = 0; i < countX; i++) {
                SetQuad((i / 2 + 1) * (sign * step) - 0.5f, -0.5f, rasters[*index]->width,
                        rasters[*index]->height, m_u[m_index[0]][0], m_v[m_index[0]][0],
                        m_u[m_index[0]][1], m_v[m_index[0]][1]);
                RwIm2DRenderPrimitive(4, m_quad, 4);
                if (sign > 0.0f) {
                    sign = -1.0f;
                } else {
                    sign = 1.0f;
                }
            }
            fn_804854B4(m_camera);
        }
        fn_804951B8(rasters[*index], 0);
    }
    if (countY) {
        fn_804857BC(m_camera, &clear, 1);
        if (fn_804854DC(m_camera)) {
            RwRenderStateSet(rwRENDERSTATESRCBLEND_8042E208, 2);
            RwRenderStateSet(rwRENDERSTATEDESTBLEND_8042E208, 2);
            RwRenderStateSet(rwRENDERSTATETEXTURERASTER_8042E208, rasters[*index]->parent);
            SetQuad(-0.5f, -0.5f, rasters[*index]->width, rasters[*index]->height,
                    m_u[m_index[0]][0], m_v[m_index[0]][0], m_u[m_index[0]][1],
                    m_v[m_index[0]][1]);
            RwIm2DRenderPrimitive(4, m_quad, 4);
            float sign = 1.0f;
            for (int i = 0; i < countY; i++) {
                SetQuad(-0.5f, (i / 2 + 1) * (sign * step) - 0.5f, rasters[*index]->width,
                        rasters[*index]->height, m_u[m_index[0]][0], m_v[m_index[0]][0],
                        m_u[m_index[0]][1], m_v[m_index[0]][1]);
                RwIm2DRenderPrimitive(4, m_quad, 4);
                if (sign > 0.0f) {
                    sign = -1.0f;
                } else {
                    sign = 1.0f;
                }
            }
            fn_804854B4(m_camera);
        }
        fn_804951B8(rasters[*index], 0);
    }
}

/* Draws src shrunk into dst's size, and copies the frame into dst. */
#define SHRINK(src, dst)                                                                           \
    if ((src) && fn_804854DC(m_camera)) {                                                          \
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER_8042E208, (src)->parent);                     \
        RwRenderStateSet(rwRENDERSTATESRCBLEND_8042E208, 2);                                      \
        RwRenderStateSet(rwRENDERSTATEDESTBLEND_8042E208, 1);                                     \
        SetQuad(-0.5f, -0.5f, (dst)->width, (dst)->height, m_u[m_index[0]][0],                    \
                m_v[m_index[0]][0], m_u[m_index[0]][1], m_v[m_index[0]][1]);                      \
        RwIm2DRenderPrimitive(4, m_quad, 4);                                                      \
        fn_804854B4(m_camera);                                                                    \
    }                                                                                              \
    fn_804951B8((dst), 0);

void UnkGlare_80430524::Draw() {
    RwRaster_8042E208 *src;
    RwRaster_8042E208 *dst;
    u8 color;
    if (m_param.large) {
        m_index[0] = 0;
        color = m_param.largeColor * lbl_8056AC30;
        src = m_rasters[0][0];
        dst = m_rasters[1][m_index[1]];
        SetColor(color, color, color, 0xFF);
        SHRINK(src, dst);
        SetColor(0xFF, 0xFF, 0xFF, 0xFF);
        m_index[0] = 1;
        Blur(m_rasters[1], &m_index[1], m_param.largeCountX, m_param.largeCountY,
             m_param.largeStep);
    }
    if (m_param.small) {
        if (m_param.smallFromScreen) {
            m_index[0] = 0;
            color = m_param.smallColor * lbl_8056AC30;
            src = m_rasters[0][0];
            dst = m_rasters[2][m_index[2]];
            SetColor(color, color, color, 0xFF);
            SHRINK(src, dst);
            SetColor(0xFF, 0xFF, 0xFF, 0xFF);
        } else if (m_param.large) {
            m_index[0] = 1;
            src = m_rasters[1][m_index[1]];
            dst = m_rasters[2][m_index[2]];
            SetColor(m_param.smallColor, m_param.smallColor, m_param.smallColor, 0xFF);
            SHRINK(src, dst);
            SetColor(0xFF, 0xFF, 0xFF, 0xFF);
        } else {
            m_index[0] = 0;
            color = m_param.smallColor * lbl_8056AC30;
            src = m_rasters[0][0];
            dst = m_rasters[2][m_index[2]];
            SetColor(color, color, color, 0xFF);
            SHRINK(src, dst);
            SetColor(0xFF, 0xFF, 0xFF, 0xFF);
        }
        m_index[0] = 2;
        Blur(m_rasters[2], &m_index[2], m_param.smallCountX, m_param.smallCountY,
             m_param.smallStep);
    }
    RwRGBA_8042E208 clear = {0, 0, 0, 0};
    fn_804857BC(m_camera, &clear, 1);
    if (m_param.separate) {
        if (m_param.large) {
            m_index[0] = 1;
            Add(m_rasters[0][0], m_rasters[1][m_index[1]]);
        }
        if (m_param.small) {
            m_index[0] = 2;
            Add(m_rasters[0][0], m_rasters[2][m_index[2]]);
        }
    } else {
        if (m_param.small) {
            if (m_param.large) {
                m_index[0] = 1;
                Add(m_rasters[1][m_index[1]], m_rasters[1][m_index[1]]);
                m_index[0] = 2;
                Add(m_rasters[1][m_index[1]], m_rasters[2][m_index[2]]);
                fn_804951B8(m_rasters[1][m_index[1]], 0);
                fn_804857BC(m_camera, &clear, 1);
            } else {
                m_index[0] = 2;
                Add(m_rasters[0][0], m_rasters[2][m_index[2]]);
            }
        }
        if (m_param.large) {
            m_index[0] = 1;
            Add(m_rasters[0][0], m_rasters[1][m_index[1]]);
        }
    }
    m_index[0] = 0;
    Add(m_rasters[0][0], m_rasters[0][0]);
}
