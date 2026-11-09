/* Effect::RW3::EffectManagerRW3 (class name and base from RTTI): the RenderWare effect manager.
 * It creates the RenderWare element classes, keeps a cache of particle textures and two copies of
 * the camera's frame buffer for the screen effects. Member and method names other than the
 * class's are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). Retail owns the one global EffectElement::operator delete here;
 * the out-of-line definition also places it after this unit's constructor and destructor.
 *
 * NonMatching: all ten function bodies and their text order match. The only DOL differences left
 * are 20 bytes in the exception records for the last three functions: retail lays their extab
 * records out as operator delete, destructor, constructor (08080000, 10080000, 08080000), while
 * this definition order gives destructor, constructor, operator delete. Moving the delete
 * definition after the destructor gives retail's extab order and anonymous labels, but then emits
 * the text as operator delete, destructor, constructor instead; making it inline also makes it
 * weak and leaves its text first. #pragma auto_inline and #pragma dont_inline change neither
 * order. The trailing four-byte .data and two-byte .rodata alignment gaps do not affect the DOL. */
#define EFFECT_ELEMENT_DELETE_OUT_OF_LINE
#include "Game/EffectRW3.h"
#include <dolphin/types.h>

using ::Effect::System::EffectElement;
using ::Effect::System::EffectLaunchInfo;
using ::Effect::System::UnkEffectDesc_801D0AFC;

struct RwTexture;

/* RenderWare's raster and camera (rw/rwcore.h; the fields used here). */
struct RwRaster_80432578 {
    /* 0x00 */ RwRaster_80432578 *parent;
    /* 0x04 */ u8 field_0x4[0xC - 0x4];
    /* 0x0C */ s32 width;
    /* 0x10 */ s32 height;
};

struct RwRect_804325E8 {
    s32 x;
    s32 y;
    s32 w;
    s32 h;
};

struct RwCamera_804325E8 {
    /* 0x00 */ u8 field_0x0[0x60];
    /* 0x60 */ RwRaster_80432578 *frameBuffer;
};

extern "C" {
/* RwRasterCreate, RwRasterSubRaster, RwRasterDestroy and RwTextureDestroy (behaviour inferred). */
RwRaster_80432578 *fn_80489E74(s32 width, s32 height, s32 depth, s32 flags);
RwRaster_80432578 *fn_80489DC4(RwRaster_80432578 *subRaster, RwRaster_80432578 *raster,
                               RwRect_804325E8 *rect);
int fn_80489BDC(RwRaster_80432578 *raster);
int fn_804483E8(RwTexture *texture);
}

/* A cached texture (layout from its uses). */
struct UnkTextureNode_804324C4 {
    /* 0x0 */ UnkTextureNode_804324C4 *next;
    /* 0x4 */ s32 refs; /* updates left before it is released */
    /* 0x8 */ RwTexture *texture;
    /* 0xC */ s32 id;
};

/* EffectManagerRW3's own members (the shared header gives the manager classes one block of
 * padding). */
struct UnkManagerRW3View_804329C4 {
    /* 0x00 */ u8 field_0x0[0x2C];
    /* 0x2C */ UnkTextureNode_804324C4 *textures;
    /* 0x30 */ s32 textureCount;
    /* 0x34 */ RwRaster_80432578 *screenRaster;
    /* 0x38 */ RwRaster_80432578 *screenRaster2;
};

#define View(manager) ((UnkManagerRW3View_804329C4 *)(manager))

namespace Effect {
namespace System {

void EffectElement::operator delete(void *p) {
    fn_8042A5D0()->Free(p);
}

} // namespace System
namespace RW3 {

EffectManagerRW3::EffectManagerRW3() {
    View(this)->textures = 0;
    View(this)->textureCount = 0;
    View(this)->screenRaster = 0;
}

EffectManagerRW3::~EffectManagerRW3() {
    if (View(this)->screenRaster) {
        fn_80489BDC(View(this)->screenRaster);
        View(this)->screenRaster = 0;
    }
}

EffectElement *EffectManagerRW3::CreateElement(const UnkEffectDesc_801D0AFC *desc,
                                               EffectLaunchInfo *info) {
    EffectLaunchInfoRW3 *launch = dynamic_cast<EffectLaunchInfoRW3 *>(info);
    if (launch) {
        switch (desc->m_type) {
        case 2:
            return new EffectParticleRW3(desc, launch, this);
        case 3:
            return new EffectArrangerRW3(desc, launch, this);
        case 4:
            return new EffectObjectRW3(desc, launch, this);
        case 5:
            return new EffectParticle3DRW3(desc, launch, this);
        case 6:
            return new EffectRayRW3(desc, launch, this);
        case 8:
            return new EffectGlobalParamRW3(desc, launch, this);
        case 9:
            return new EffectLensFlareRW3(desc, launch, this);
        case 10:
            return new EffectShimmerRW3(desc, launch, this);
        }
    }
    return EffectManager::CreateElement(desc, info);
}

extern "C" void fn_804325E8(EffectManagerRW3 *manager) {
    EffectSystemRW3 *system = dynamic_cast<EffectSystemRW3 *>(::Effect::System::fn_8042A5D0());
    if (!system) {
        return;
    }
    RwCamera_804325E8 *camera = (RwCamera_804325E8 *)system->GetCamera();
    if (!camera) {
        return;
    }
    s32 width = camera->frameBuffer->width;
    s32 height = camera->frameBuffer->height;
    RwRect_804325E8 rect = {0, 0, width, height};
    View(manager)->screenRaster = fn_80489E74(0, 0, 0, 5);
    fn_80489DC4(View(manager)->screenRaster, fn_80489E74(width, height, 0, 0x505), &rect);
    View(manager)->screenRaster2 = fn_80489E74(0, 0, 0, 5);
    fn_80489DC4(View(manager)->screenRaster2, fn_80489E74(width, height, 0, 0x505), &rect);
}

extern "C" void fn_80432578(EffectManagerRW3 *manager) {
    if (View(manager)->screenRaster) {
        fn_80489BDC(View(manager)->screenRaster->parent);
        fn_80489BDC(View(manager)->screenRaster);
        View(manager)->screenRaster = 0;
    }
    if (View(manager)->screenRaster2) {
        fn_80489BDC(View(manager)->screenRaster2->parent);
        fn_80489BDC(View(manager)->screenRaster2);
        View(manager)->screenRaster2 = 0;
    }
}

extern "C" RwTexture *fn_804324C4(EffectManagerRW3 *manager, int id) {
    if (!View(manager)->textures) {
        return 0;
    }
    UnkTextureNode_804324C4 *prev = 0;
    for (UnkTextureNode_804324C4 *node = View(manager)->textures; node; node = node->next) {
        if (node->id == id) {
            RwTexture *texture = node->texture;
            if (prev) {
                prev->next = node->next;
            } else {
                View(manager)->textures = node->next;
            }
            ::Effect::System::fn_8042A5D0()->Free(node);
            View(manager)->textureCount--;
            return texture;
        }
        prev = node;
    }
    return 0;
}

extern "C" void fn_804324A0(EffectManagerRW3 *manager, RwTexture *texture) {
    fn_804483E8(texture);
}

extern "C" void fn_804323F8(EffectManagerRW3 *manager) {
    UnkTextureNode_804324C4 *node;
    UnkTextureNode_804324C4 *next;
    UnkTextureNode_804324C4 *prev = 0;
    for (node = View(manager)->textures; node; node = next) {
        next = node->next;
        if (node->refs == 0) {
            if (node->texture) {
                fn_804483E8(node->texture);
            }
            if (prev) {
                prev->next = next;
            } else {
                View(manager)->textures = next;
            }
            View(manager)->textureCount--;
            ::Effect::System::fn_8042A5D0()->Free(node);
        } else {
            node->refs--;
            prev = node;
        }
    }
}

extern "C" void fn_8043237C(EffectManagerRW3 *manager) {
    UnkTextureNode_804324C4 *node;
    UnkTextureNode_804324C4 *next;
    for (node = View(manager)->textures; node; node = next) {
        next = node->next;
        if (node->texture) {
            fn_804483E8(node->texture);
        }
        View(manager)->textureCount--;
        ::Effect::System::fn_8042A5D0()->Free(node);
    }
    View(manager)->textures = 0;
}

} // namespace RW3
} // namespace Effect
