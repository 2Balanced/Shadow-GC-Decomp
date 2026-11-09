#ifndef GAME_EFFECTRW3_H
#define GAME_EFFECTRW3_H

/* The RenderWare layer of the effect library (namespace Effect::RW3; class names from RTTI): the
 * declarations its units (Game/EffectManagerRW3.cpp and the element units after it) share. The
 * element classes themselves are in Game/EffectManagerPJS.h. Member names are inferred. */
#define EFFECT_ELEMENT_POLYMORPHIC
#define EFFECT_ELEMENT_FULL

#include "Game/EffectManagerPJS.h"
#include "size_assert.h"

/* The few RenderWare structures the effect library reads (layouts from its uses). */
struct RwRaster {
    /* 0x00 */ RwRaster *parent;
    /* 0x04 */ u8 *cpPixels;
    /* 0x08 */ u8 *palette;
    /* 0x0C */ s32 width;
    /* 0x10 */ s32 height;
};


struct RwV3d {
    float x;
    float y;
    float z;
};

struct RwMatrix {
    /* 0x00 */ RwV3d right;
    /* 0x0C */ u32 flags;
    /* 0x10 */ RwV3d up;
    /* 0x1C */ u32 pad1;
    /* 0x20 */ RwV3d at;
    /* 0x2C */ u32 pad2;
    /* 0x30 */ RwV3d pos;
    /* 0x3C */ u32 pad3;
};

struct RwCamera {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ RwMatrix viewMatrix;
    /* 0x60 */ RwRaster *frameBuffer;
    /* 0x64 */ u8 field_0x64[0x80 - 0x64];
    /* 0x80 */ float nearPlane;
    /* 0x84 */ float farPlane;
};

struct RwFrame {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ RwMatrix modelling;
};

struct RpClump {
    /* 0x0 */ u8 field_0x0[0x4];
    /* 0x4 */ RwFrame *frame; /* the clump's frame (object.parent) */
};

struct RpGeometry {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ void **materials; /* matList.materials */
};

struct RpAtomic {
    /* 0x00 */ u8 field_0x0[0x18];
    /* 0x18 */ RpGeometry *geometry;
    /* 0x1C */ u8 field_0x1C[0x48 - 0x1C];
    /* 0x48 */ RpAtomic *(*renderCallBack)(RpAtomic *atomic);
};

/* The open device's render state functions, reached through RwEngineInstance (RwRenderStateSet
 * and RwRenderStateGet). */
struct RwDevice {
    /* 0x00 */ float gammaCorrection;
    /* 0x04 */ void *fpSystem;
    /* 0x08 */ float zBufferNear;
    /* 0x0C */ float zBufferFar;
    /* 0x10 */ s32 (*fpRenderStateSet)(s32 state, void *value);
    /* 0x14 */ s32 (*fpRenderStateGet)(s32 state, void *value);
    /* 0x18 */ void *fpIm2DRenderLine;
    /* 0x1C */ void *fpIm2DRenderTriangle;
    /* 0x20 */ s32 (*fpIm2DRenderPrimitive)(s32 primType, void *vertices, s32 count);
};

struct RwGlobals {
    /* 0x00 */ void *curCamera;
    /* 0x04 */ void *curWorld;
    /* 0x08 */ u16 renderFrame;
    /* 0x0A */ u16 lightFrame;
    /* 0x0C */ u16 pad[2];
    /* 0x10 */ RwDevice dOpenDevice;
};

/* A particle tank's locked data: where the first particle's value is and how far apart they are
 * (RpPTankLockStruct). */
struct RpPTankLockStruct {
    /* 0x0 */ u8 *data;
    /* 0x4 */ s32 stride;
};

/* A particle tank atomic's plugin data (layout from its uses). */
struct UnkPTankData_80448074 {
    /* 0x00 */ u8 field_0x0[0x4];
    /* 0x04 */ s32 m_activeCount;
    /* 0x08 */ u8 field_0x8[0x40 - 0x8];
    /* 0x40 */ u32 m_flags;
    /* 0x44 */ u8 field_0x44[0xA4 - 0x44];
    /* 0xA4 */ s32 m_srcBlend;
    /* 0xA8 */ s32 m_dstBlend;
    /* 0xAC */ s32 m_vertexAlphaBlend;
};

extern "C" {
/* RenderWare: RwEngineInstance, and the offset of the particle tank plugin's data in an atomic. */
extern RwGlobals *lbl_805F15B4;
extern s32 lbl_805F1448;
}

inline void RwRenderStateSet(s32 state, void *value) {
    lbl_805F15B4->dOpenDevice.fpRenderStateSet(state, value);
}

inline void RwRenderStateGet(s32 state, void *value) {
    lbl_805F15B4->dOpenDevice.fpRenderStateGet(state, value);
}

inline void RwIm2DRenderPrimitive(s32 primType, void *vertices, s32 count) {
    lbl_805F15B4->dOpenDevice.fpIm2DRenderPrimitive(primType, vertices, count);
}

inline UnkPTankData_80448074 *PTankData(void *atomic) {
    return *reinterpret_cast<UnkPTankData_80448074 **>(static_cast<u8 *>(atomic) + lbl_805F1448);
}

struct RwRect {
    /* 0x0 */ s32 x;
    /* 0x4 */ s32 y;
    /* 0x8 */ s32 w;
    /* 0xC */ s32 h;
};

namespace Effect {
namespace RW3 {

/* EffectLaunchInfoRW3 (class name from RTTI): the RenderWare effect library's launch description.
 * Its constructor and destructor are inline (Game/EffectArrangerRW3.cpp emits them). */
class EffectLaunchInfoRW3 : public ::Effect::System::EffectLaunchInfo {
  public:
    EffectLaunchInfoRW3(void *data, void *arg2, void *arg3, void *arg4)
        : EffectLaunchInfo(data), field_0x8(arg2), field_0xC(arg3), field_0x10(arg4) {}
    virtual ~EffectLaunchInfoRW3() {}

    /* 0x08 */ void *field_0x8;
    /* 0x0C */ void *field_0xC;
    /* 0x10 */ void *field_0x10;
};
SIZE_ASSERT(EffectLaunchInfoRW3, 0x14);

} // namespace RW3
} // namespace Effect

#endif
