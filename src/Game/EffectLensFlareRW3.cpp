/* Effect::RW3::EffectLensFlareRW3 (class name and bases from RTTI): the RenderWare version of the
 * lens flare element, which hands its settings, blended in over its blend factor, to the lens
 * flare of the RenderWare effect manager; and that lens flare (no RTTI; class name invented), which
 * the manager keeps at 0x50. It projects the light onto the screen, lays the flare's parts out
 * along the line through the screen centre, draws them, and whitens the screen when the light is
 * near its middle. Member and function names are inferred. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#define EFFECT_ELEMENT_POLYMORPHIC
#define EFFECT_ELEMENT_FULL

#include "Game/EffectManagerPJS.h"
#include "Game/MathTypes.h"

using ::Effect::RW3::EffectManagerRW3;
using ::Effect::RW3::EffectSystemRW3;
using ::Effect::RW3::UnkLensFlareParam_8043232C;
using ::Effect::System::EffectLaunchInfo;
using ::Effect::System::EffectManager;
using ::Effect::System::UnkEffectDesc_801D0AFC;

/* RenderWare's immediate-mode 2D vertex, raster, texture, camera, texture dictionary, 2D vector
 * and engine globals (rw/rwcore.h; the fields used here). */
struct RwIm2DVertex_80430DE8 {
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

struct RwRaster_80430DE8 {
    /* 0x00 */ RwRaster_80430DE8 *parent;
    /* 0x04 */ u8 field_0x4[0xC - 0x4];
    /* 0x0C */ s32 width;
    /* 0x10 */ s32 height;
};

struct RwTexture_80430DE8 {
    /* 0x00 */ RwRaster_80430DE8 *raster;
};

struct RwTexDictionary_80430DE8;

struct RwCamera_80430DE8 {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ Mtx34f viewMatrix; /* an RwMatrix */
    /* 0x50 */ u8 field_0x50[0x60 - 0x50];
    /* 0x60 */ RwRaster_80430DE8 *frameBuffer;
    /* 0x64 */ u8 field_0x64[0x80 - 0x64];
    /* 0x80 */ float nearPlane;
    /* 0x84 */ float farPlane;
};

struct RwV2d_80430DE8 {
    float x;
    float y;
};

typedef int (*RwRenderStateFunction_80430DE8)(int state, void *value);
typedef int (*RwIm2DRenderPrimitiveFunction_80430DE8)(int primType,
                                                       RwIm2DVertex_80430DE8 *vertices, int count);

struct RwGlobals_80430DE8 {
    /* 0x00 */ u8 field_0x0[0x18];
    /* 0x18 */ float zBufferNear;
    /* 0x1C */ float zBufferFar;
    /* 0x20 */ RwRenderStateFunction_80430DE8 fpRenderStateSet;
    /* 0x24 */ RwRenderStateFunction_80430DE8 fpRenderStateGet;
    /* 0x28 */ u8 field_0x28[0x30 - 0x28];
    /* 0x30 */ RwIm2DRenderPrimitiveFunction_80430DE8 fpIm2DRenderPrimitive;
};

extern "C" {
/* The engine instance. */
extern RwGlobals_80430DE8 *lbl_805F15B4;
/* RwCameraBeginUpdate, RwCameraEndUpdate, RwTexDictionaryFindNamedTexture, RwV3dTransformPoint and
 * RwV2dLength (behaviour inferred). */
RwCamera_80430DE8 *fn_804854DC(RwCamera_80430DE8 *camera);
RwCamera_80430DE8 *fn_804854B4(RwCamera_80430DE8 *camera);
RwTexture_80430DE8 *fn_8048CFFC(RwTexDictionary_80430DE8 *dict, const char *name);
Vec3f *fn_80482458(Vec3f *out, const void *point, const Mtx34f *matrix);
float fn_804823F0(const RwV2d_80430DE8 *v);
}

#define RwRenderStateSet(state, value) (lbl_805F15B4->fpRenderStateSet((state), (void *)(value)))
#define RwRenderStateGet(state, value) (lbl_805F15B4->fpRenderStateGet((state), (void *)(value)))
#define RwIm2DRenderPrimitive(prim, vertices, count)                                               \
    (lbl_805F15B4->fpIm2DRenderPrimitive((prim), (vertices), (count)))

enum {
    rwRENDERSTATETEXTURERASTER_80430DE8 = 1,
    rwRENDERSTATEZTESTENABLE_80430DE8 = 6,
    rwRENDERSTATEZWRITEENABLE_80430DE8 = 8,
    rwRENDERSTATETEXTUREFILTER_80430DE8 = 9,
    rwRENDERSTATESRCBLEND_80430DE8 = 10,
    rwRENDERSTATEDESTBLEND_80430DE8 = 11,
    rwRENDERSTATEVERTEXALPHAENABLE_80430DE8 = 12,
    rwRENDERSTATEFOGENABLE_80430DE8 = 14
};

/* One part of the lens flare (as in Game/EffectTask.cpp, which holds the table): its texture and
 * size, its place along the line through the screen centre, its texture coordinates, its colour
 * and the draw group it belongs to. */
struct UnkLensFlarePart_8053C9C0 {
    /* 0x00 */ const char *m_texture;
    /* 0x04 */ s16 m_width;
    /* 0x06 */ s16 m_height;
    /* 0x08 */ float m_position;
    /* 0x0C */ float m_u0;
    /* 0x10 */ float m_u1;
    /* 0x14 */ float m_v0;
    /* 0x18 */ float m_v1;
    /* 0x1C */ u8 m_r;
    /* 0x1D */ u8 m_g;
    /* 0x1E */ u8 m_b;
    /* 0x1F */ u8 m_a;
    /* 0x20 */ u8 m_group;
};

/* The manager's lens flare (0x80431FDC). Part 0 is drawn at the light, the others along the line
 * from it through the screen centre. */
class UnkLensFlare_80431FDC {
  public:
    UnkLensFlare_80431FDC(EffectManagerRW3 *manager, const UnkLensFlareParam_8043232C *param);
    ~UnkLensFlare_80431FDC() {
        if (m_vertices) {
            delete[] m_vertices;
        }
        if (m_textures) {
            delete[] m_textures;
        }
    }
    void Draw(u8 group);
    void Layout();
    void SetParts(RwTexDictionary_80430DE8 *dict, UnkLensFlarePart_8053C9C0 *parts, int count);
    UnkLensFlareParam_8043232C GetParam();
    void SetParam(const UnkLensFlareParam_8043232C &param);
    void SetParam(const UnkLensFlareParam_8043232C *param);

    /* 0x00 */ EffectManagerRW3 *m_manager;
    /* 0x04 */ UnkLensFlareParam_8043232C m_param;
    /* 0x14 */ RwTexDictionary_80430DE8 *m_dict;
    /* 0x18 */ UnkLensFlarePart_8053C9C0 *m_parts;
    /* 0x1C */ RwTexture_80430DE8 **m_textures; /* one per part */
    /* 0x20 */ u32 field_0x20;
    /* 0x24 */ s32 m_count;
    /* 0x28 */ RwIm2DVertex_80430DE8 *m_vertices; /* four per part */
    /* 0x2C */ RwIm2DVertex_80430DE8 m_quad[4];   /* the screen, for the whiteout */
    /* 0x8C */ u8 m_lightHidden;  /* the light is off the screen */
    /* 0x8D */ u8 m_flareHidden;  /* the light is too far from the screen centre for the flare */
};
SIZE_ASSERT(UnkLensFlare_80431FDC, 0x90);

/* The effect manager's members this unit uses (layout from their uses). */
struct UnkManagerView_804322E0 {
    /* 0x00 */ u8 field_0x0[0x50];
    /* 0x50 */ UnkLensFlare_80431FDC *lensFlare;
};

#define View(manager) ((UnkManagerView_804322E0 *)(manager))

inline UnkLensFlareParam_8043232C::UnkLensFlareParam_8043232C(u8 enabled_, u8 alpha_,
                                                              u8 whiteout_)
    : enabled(enabled_), alpha(alpha_), whiteout(whiteout_) {
    ::Effect::RW3::UnkLensFlarePosition_8043232C zero = {0.0f, 0.0f, 0.0f};
    SetPosition(zero);
}

extern "C" {

/* Creates the manager's lens flare. */
void fn_804322E0(EffectManagerRW3 *manager, const UnkLensFlareParam_8043232C *param) {
    View(manager)->lensFlare = new UnkLensFlare_80431FDC(manager, param);
}

/* Destroys it. */
void fn_80432278(EffectManagerRW3 *manager) {
    if (View(manager)->lensFlare) {
        delete View(manager)->lensFlare;
        View(manager)->lensFlare = 0;
    }
}

void fn_8043224C(EffectManagerRW3 *manager) {
    if (View(manager)->lensFlare) {
        View(manager)->lensFlare->Layout();
    }
}

void fn_80432220(EffectManagerRW3 *manager, u8 group) {
    if (View(manager)->lensFlare) {
        View(manager)->lensFlare->Draw(group);
    }
}

void fn_804321F4(EffectManagerRW3 *manager, const UnkLensFlareParam_8043232C &param) {
    if (View(manager)->lensFlare) {
        View(manager)->lensFlare->SetParam(param);
    }
}

UnkLensFlareParam_8043232C fn_80432174(EffectManagerRW3 *manager) {
    if (View(manager)->lensFlare) {
        return View(manager)->lensFlare->GetParam();
    }
    UnkLensFlareParam_8043232C param;
    return param;
}

void fn_80432148(EffectManagerRW3 *manager, RwTexDictionary_80430DE8 *dict,
                 UnkLensFlarePart_8053C9C0 *parts, int count) {
    if (View(manager)->lensFlare) {
        View(manager)->lensFlare->SetParts(dict, parts, count);
    }
}

}

UnkLensFlare_80431FDC::UnkLensFlare_80431FDC(EffectManagerRW3 *manager,
                                             const UnkLensFlareParam_8043232C *param)
    : m_manager(manager) {
    m_textures = 0;
    m_vertices = 0;
    EffectSystemRW3 *system = dynamic_cast<EffectSystemRW3 *>(::Effect::System::fn_8042A5D0());
    if (!system) {
        return;
    }
    if (!system->GetCamera()) {
        return;
    }
    m_quad[0].z = lbl_805F15B4->zBufferNear;
    m_quad[0].red = 0xFF;
    m_quad[0].green = 0xFF;
    m_quad[0].blue = 0xFF;
    m_quad[0].alpha = 0xFF;
    m_quad[1].z = lbl_805F15B4->zBufferNear;
    m_quad[1].red = 0xFF;
    m_quad[1].green = 0xFF;
    m_quad[1].blue = 0xFF;
    m_quad[1].alpha = 0xFF;
    m_quad[2].z = lbl_805F15B4->zBufferNear;
    m_quad[2].red = 0xFF;
    m_quad[2].green = 0xFF;
    m_quad[2].blue = 0xFF;
    m_quad[2].alpha = 0xFF;
    m_quad[3].z = lbl_805F15B4->zBufferNear;
    m_quad[3].red = 0xFF;
    m_quad[3].green = 0xFF;
    m_quad[3].blue = 0xFF;
    m_quad[3].alpha = 0xFF;
    if (param) {
        SetParam(param);
    } else {
        m_param.enabled = 0;
        m_param.position.x = 0.0f;
        m_param.position.y = 1000.0f;
        m_param.position.z = 0.0f;
        m_param.alpha = 0xFF;
        m_param.whiteout = 0x20;
    }
}

void UnkLensFlare_80431FDC::SetParam(const UnkLensFlareParam_8043232C *param) {
    if (param) {
        m_param = *param;
    }
}

void UnkLensFlare_80431FDC::SetParam(const UnkLensFlareParam_8043232C &param) {
    m_param = param;
}

UnkLensFlareParam_8043232C UnkLensFlare_80431FDC::GetParam() {
    return m_param;
}

void UnkLensFlare_80431FDC::SetParts(RwTexDictionary_80430DE8 *dict,
                                     UnkLensFlarePart_8053C9C0 *parts, int count) {
    m_parts = parts;
    m_dict = dict;
    if (!m_dict || !m_parts) {
        delete[] m_vertices;
        delete[] m_textures;
        m_count = 0;
        return;
    }
    m_count = count;
    EffectSystemRW3 *system = dynamic_cast<EffectSystemRW3 *>(::Effect::System::fn_8042A5D0());
    if (!system) {
        return;
    }
    if (!system->GetCamera()) {
        return;
    }
    m_vertices = new RwIm2DVertex_80430DE8[count * 4];
    m_textures = new RwTexture_80430DE8 *[count];
    for (int i = 0; i < m_count; i++) {
        if (m_parts[i].m_texture) {
            m_textures[i] = fn_8048CFFC(m_dict, m_parts[i].m_texture);
            m_vertices[i * 4 + 0].u = m_parts[i].m_u0;
            m_vertices[i * 4 + 0].v = m_parts[i].m_v0;
            m_vertices[i * 4 + 0].z = lbl_805F15B4->zBufferNear;
            m_vertices[i * 4 + 1].u = m_parts[i].m_u0;
            m_vertices[i * 4 + 1].v = m_parts[i].m_v1;
            m_vertices[i * 4 + 1].z = lbl_805F15B4->zBufferNear;
            m_vertices[i * 4 + 2].u = m_parts[i].m_u1;
            m_vertices[i * 4 + 2].v = m_parts[i].m_v0;
            m_vertices[i * 4 + 2].z = lbl_805F15B4->zBufferNear;
            m_vertices[i * 4 + 3].u = m_parts[i].m_u1;
            m_vertices[i * 4 + 3].v = m_parts[i].m_v1;
            m_vertices[i * 4 + 3].z = lbl_805F15B4->zBufferNear;
        }
    }
}

void UnkLensFlare_80431FDC::Layout() {
    m_lightHidden = 1;
    m_flareHidden = 1;
    EffectSystemRW3 *system = dynamic_cast<EffectSystemRW3 *>(::Effect::System::fn_8042A5D0());
    if (!system) {
        return;
    }
    if (!m_param.enabled) {
        return;
    }
    if (!m_vertices) {
        return;
    }
    if (m_count < 0) {
        return;
    }
    RwCamera_80430DE8 *camera = (RwCamera_80430DE8 *)system->GetCamera();
    if (!camera) {
        return;
    }
    Vec3f view;
    fn_80482458(&view, &m_param.position, &camera->viewMatrix);
    float farPlane;
    float nearPlane = camera->nearPlane;
    farPlane = camera->farPlane;
    if (view.z < nearPlane) {
        return;
    }
    float sx = view.x / view.z;
    float sy = view.y / view.z;
    float width = camera->frameBuffer->width;
    float height = camera->frameBuffer->height;
    RwV2d_80430DE8 centre = {0.5f * width, 0.5f * height};
    RwV2d_80430DE8 light = {width * sx - centre.x, height * sy - centre.y};
    float radius = 0.5f * (width > height ? width : height);
    float x;
    float y;
    if (!(light.x > radius || light.x < -radius || light.y > radius || light.y < -radius)) {
        m_flareHidden = 0;
        float fade = (radius - fn_804823F0(&light)) / radius;
        if (fade < 0.0f) {
            fade = 0.0f;
        }
        fade *= m_param.alpha / 255.0f;
        for (int i = 1; i < m_count; i++) {
            x = light.x * m_parts[i].m_position + centre.x;
            y = light.y * m_parts[i].m_position + centre.y;
            m_vertices[i * 4 + 0].x = x - 0.5f * m_parts[i].m_height;
            m_vertices[i * 4 + 0].y = y - 0.5f * m_parts[i].m_width;
            m_vertices[i * 4 + 0].red = m_parts[i].m_r;
            m_vertices[i * 4 + 0].green = m_parts[i].m_b;
            m_vertices[i * 4 + 0].blue = m_parts[i].m_g;
            m_vertices[i * 4 + 0].alpha = fade * m_parts[i].m_a;
            m_vertices[i * 4 + 1].x = x - 0.5f * m_parts[i].m_height;
            m_vertices[i * 4 + 1].y = y + 0.5f * m_parts[i].m_width;
            m_vertices[i * 4 + 1].red = m_parts[i].m_r;
            m_vertices[i * 4 + 1].green = m_parts[i].m_b;
            m_vertices[i * 4 + 1].blue = m_parts[i].m_g;
            m_vertices[i * 4 + 1].alpha = fade * m_parts[i].m_a;
            m_vertices[i * 4 + 2].x = x + 0.5f * m_parts[i].m_height;
            m_vertices[i * 4 + 2].y = y - 0.5f * m_parts[i].m_width;
            m_vertices[i * 4 + 2].red = m_parts[i].m_r;
            m_vertices[i * 4 + 2].green = m_parts[i].m_b;
            m_vertices[i * 4 + 2].blue = m_parts[i].m_g;
            m_vertices[i * 4 + 2].alpha = fade * m_parts[i].m_a;
            m_vertices[i * 4 + 3].x = x + 0.5f * m_parts[i].m_height;
            m_vertices[i * 4 + 3].y = y + 0.5f * m_parts[i].m_width;
            m_vertices[i * 4 + 3].red = m_parts[i].m_r;
            m_vertices[i * 4 + 3].green = m_parts[i].m_b;
            m_vertices[i * 4 + 3].blue = m_parts[i].m_g;
            m_vertices[i * 4 + 3].alpha = fade * m_parts[i].m_a;
        }
        float whiteout = m_param.whiteout * fade;
        m_quad[0].red = whiteout;
        m_quad[0].green = whiteout;
        m_quad[0].blue = whiteout;
        m_quad[0].alpha = 0xFF;
        m_quad[1].red = whiteout;
        m_quad[1].green = whiteout;
        m_quad[1].blue = whiteout;
        m_quad[1].alpha = 0xFF;
        m_quad[2].red = whiteout;
        m_quad[2].green = whiteout;
        m_quad[2].blue = whiteout;
        m_quad[2].alpha = 0xFF;
        m_quad[3].red = whiteout;
        m_quad[3].green = whiteout;
        m_quad[3].blue = whiteout;
        m_quad[3].alpha = 0xFF;
    }
    if (!(light.x > 1.5f * width || light.x < 0.5f * -width || light.y > 1.5f * height ||
          light.y < 0.5f * -height)) {
        m_lightHidden = 0;
        float position = m_parts[0].m_position;
        float range = farPlane - nearPlane;
        float zNear = lbl_805F15B4->zBufferNear;
        float z = zNear + farPlane / view.z * ((lbl_805F15B4->zBufferFar - zNear) / range * (view.z - nearPlane));
        x = position * light.x + centre.x;
        y = position * light.y + centre.y;
        m_vertices[0].x = x - 0.5f * m_parts[0].m_height;
        m_vertices[0].y = y - 0.5f * m_parts[0].m_width;
        m_vertices[0].z = z;
        m_vertices[0].red = m_parts[0].m_r;
        m_vertices[0].green = m_parts[0].m_b;
        m_vertices[0].blue = m_parts[0].m_g;
        m_vertices[0].alpha = m_parts[0].m_a;
        m_vertices[1].x = x - 0.5f * m_parts[0].m_height;
        m_vertices[1].y = y + 0.5f * m_parts[0].m_width;
        m_vertices[1].z = z;
        m_vertices[1].red = m_parts[0].m_r;
        m_vertices[1].green = m_parts[0].m_b;
        m_vertices[1].blue = m_parts[0].m_g;
        m_vertices[1].alpha = m_parts[0].m_a;
        m_vertices[2].x = x + 0.5f * m_parts[0].m_height;
        m_vertices[2].y = y - 0.5f * m_parts[0].m_width;
        m_vertices[2].z = z;
        m_vertices[2].red = m_parts[0].m_r;
        m_vertices[2].green = m_parts[0].m_b;
        m_vertices[2].blue = m_parts[0].m_g;
        m_vertices[2].alpha = m_parts[0].m_a;
        m_vertices[3].x = x + 0.5f * m_parts[0].m_height;
        m_vertices[3].y = y + 0.5f * m_parts[0].m_width;
        m_vertices[3].z = z;
        m_vertices[3].red = m_parts[0].m_r;
        m_vertices[3].green = m_parts[0].m_b;
        m_vertices[3].blue = m_parts[0].m_g;
        m_vertices[3].alpha = m_parts[0].m_a;
    }
    m_quad[0].x = 0.0f;
    m_quad[0].y = 0.0f;
    m_quad[1].x = 0.0f;
    m_quad[1].y = height;
    m_quad[2].x = width;
    m_quad[2].y = 0.0f;
    m_quad[3].x = width;
    m_quad[3].y = height;
}

void UnkLensFlare_80431FDC::Draw(u8 group) {
    EffectSystemRW3 *system = dynamic_cast<EffectSystemRW3 *>(::Effect::System::fn_8042A5D0());
    if (!system) {
        return;
    }
    if (!m_param.enabled) {
        return;
    }
    RwCamera_80430DE8 *camera = (RwCamera_80430DE8 *)system->GetCamera();
    if (!camera) {
        return;
    }
    if (m_lightHidden && m_flareHidden) {
        return;
    }
    int srcBlend;
    int destBlend;
    int vertexAlpha;
    int zTest;
    int zWrite;
    int fog;
    int textureFilter;
    RwRenderStateGet(rwRENDERSTATESRCBLEND_80430DE8, &srcBlend);
    RwRenderStateGet(rwRENDERSTATEDESTBLEND_80430DE8, &destBlend);
    RwRenderStateGet(rwRENDERSTATEVERTEXALPHAENABLE_80430DE8, &vertexAlpha);
    RwRenderStateGet(rwRENDERSTATEZTESTENABLE_80430DE8, &zTest);
    RwRenderStateGet(rwRENDERSTATEZWRITEENABLE_80430DE8, &zWrite);
    RwRenderStateGet(rwRENDERSTATEFOGENABLE_80430DE8, &fog);
    RwRenderStateGet(rwRENDERSTATETEXTUREFILTER_80430DE8, &textureFilter);
    if (fn_804854DC(camera)) {
        RwRenderStateSet(rwRENDERSTATESRCBLEND_80430DE8, 5);
        RwRenderStateSet(rwRENDERSTATEDESTBLEND_80430DE8, 2);
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE_80430DE8, 1);
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE_80430DE8, 0);
        RwRenderStateSet(rwRENDERSTATEFOGENABLE_80430DE8, 0);
        RwRenderStateSet(rwRENDERSTATETEXTUREFILTER_80430DE8, 1);
        if (!m_lightHidden && group != m_parts[0].m_group) {
            RwRenderStateSet(rwRENDERSTATEZTESTENABLE_80430DE8, 1);
            RwRenderStateSet(rwRENDERSTATETEXTURERASTER_80430DE8, m_textures[0]->raster);
            RwIm2DRenderPrimitive(4, m_vertices, 4);
        }
        if (!m_flareHidden) {
            RwRenderStateSet(rwRENDERSTATEZTESTENABLE_80430DE8, 0);
            for (int i = 1; i < m_count; i++) {
                if (group == m_parts[i].m_group) {
                    RwRenderStateSet(rwRENDERSTATETEXTURERASTER_80430DE8, m_textures[i]->raster);
                    RwIm2DRenderPrimitive(4, &m_vertices[i * 4], 4);
                }
            }
        }
        fn_804854B4(camera);
    }
    if (fn_804854DC(camera)) {
        if (!m_flareHidden && !group) {
            RwRenderStateSet(rwRENDERSTATESRCBLEND_80430DE8, 2);
            RwRenderStateSet(rwRENDERSTATEDESTBLEND_80430DE8, 2);
            RwRenderStateSet(rwRENDERSTATETEXTURERASTER_80430DE8, 0);
            RwIm2DRenderPrimitive(4, m_quad, 4);
        }
        RwRenderStateSet(rwRENDERSTATESRCBLEND_80430DE8, srcBlend);
        RwRenderStateSet(rwRENDERSTATEDESTBLEND_80430DE8, destBlend);
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE_80430DE8, vertexAlpha);
        RwRenderStateSet(rwRENDERSTATEZTESTENABLE_80430DE8, zTest);
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE_80430DE8, zWrite);
        RwRenderStateSet(rwRENDERSTATEFOGENABLE_80430DE8, fog);
        RwRenderStateSet(rwRENDERSTATETEXTUREFILTER_80430DE8, textureFilter);
        fn_804854B4(camera);
    }
}

/* A lens flare element's description (layout from the constructor). */
struct UnkLensFlareDesc_80430CAC {
    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ u8 alpha;
    /* 0x31 */ u8 whiteout;
    /* 0x32 */ u8 field_0x32[0x38 - 0x32];
    /* 0x38 */ float x;
    /* 0x3C */ float y;
    /* 0x40 */ float z;
};

/* Copies the description's settings. */
inline void ReadLensFlareDesc(UnkLensFlareParam_8043232C *param,
                              const UnkLensFlareDesc_80430CAC *desc) {
    param->alpha = desc->alpha;
    param->whiteout = desc->whiteout;
    param->position.x = desc->x;
    param->position.y = desc->y;
    param->position.z = desc->z;
}

namespace Effect {
namespace RW3 {

#pragma pool_data on
EffectLensFlareRW3::EffectLensFlareRW3(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,
                                       EffectManager *manager)
    : EffectLensFlare(desc, info, manager) {
    m_size -= sizeof(EffectLensFlare);
    m_size += sizeof(EffectLensFlareRW3);
    if (!(m_flags & 1)) {
        return;
    }
    EffectManagerRW3 *managerRW3 = dynamic_cast<EffectManagerRW3 *>(m_manager);
    if (managerRW3) {
        m_startParam = fn_80432174(managerRW3);
        if (field_0xFC & 0x10000) {
            m_param.enabled = 1;
        } else {
            m_param.enabled = 0;
        }
        ReadLensFlareDesc(&m_param, reinterpret_cast<const UnkLensFlareDesc_80430CAC *>(desc));
    }
}
#pragma pool_data reset

EffectLensFlareRW3::~EffectLensFlareRW3() {}

void EffectLensFlareRW3::Update(float dt) {
    EffectLensFlare::Update(dt);
    if (!(m_flags & 1)) {
        return;
    }
    if (m_flags & 4) {
        return;
    }
    EffectManagerRW3 *manager = dynamic_cast<EffectManagerRW3 *>(m_manager);
    if (!manager) {
        return;
    }
    UnkLensFlareParam_8043232C param = m_param;
    if (m_blend < 0.9999f) {
        if (param.enabled || m_startParam.enabled) {
            param.enabled = 1;
        } else {
            param.enabled = 0;
        }
        if (param.alpha != m_startParam.alpha) {
            param.alpha = param.alpha * m_blend + m_startParam.alpha * (1.0f - m_blend);
        }
        if (param.whiteout != m_startParam.whiteout) {
            param.whiteout = param.whiteout * m_blend + m_startParam.whiteout * (1.0f - m_blend);
        }
        param.position.x = param.position.x * m_blend + m_startParam.position.x * (1.0f - m_blend);
        param.position.y = param.position.y * m_blend + m_startParam.position.y * (1.0f - m_blend);
        param.position.z = param.position.z * m_blend + m_startParam.position.z * (1.0f - m_blend);
    }
    fn_804321F4(manager, param);
}

} // namespace RW3
} // namespace Effect
