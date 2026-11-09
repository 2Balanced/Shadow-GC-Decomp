#ifndef GAME_EFFECTFILTERRW3_H
#define GAME_EFFECTFILTERRW3_H

/* The screen filters of the RenderWare effect manager (Effect::RW3::EffectManagerRW3): a screen
 * shimmer (Game/EffectShimmerRW3.cpp), a glow pass drawn with the manager's vf18
 * (Game/EffectGlowRW3.cpp) and a fog (Game/EffectFogRW3.cpp). The manager keeps each one behind a
 * pointer; extern "C" functions with the manager as their first argument create, run and remove
 * them, and EffectGlobalParamRW3 reads and restores their settings. None of these classes has
 * RTTI, so all names are inferred. */
#include "Game/EffectFilterParams.h"
#include "Game/EffectRW3.h"

class UnkScreenShimmer_80436D48;
class UnkGlow_80437118;
class UnkFogFilter_804386E4;

/* The RenderWare manager's own members (the base class's padding covers them). */
struct UnkManagerRW3Fields_804329C4 {
    /* 0x00 */ u8 field_0x0[0x2C];
    /* 0x2C */ void *field_0x2C;
    /* 0x30 */ void *field_0x30;
    /* 0x34 */ RwRaster *m_raster; /* a copy of the frame buffer the filters draw from */
    /* 0x38 */ u8 field_0x38[0x44 - 0x38];
    /* 0x44 */ UnkFogFilter_804386E4 *m_fog;
    /* 0x48 */ UnkScreenShimmer_80436D48 *m_shimmer;
    /* 0x4C */ UnkGlow_80437118 *m_glow;
};

inline UnkManagerRW3Fields_804329C4 *ManagerFields(::Effect::System::EffectManager *manager) {
    return reinterpret_cast<UnkManagerRW3Fields_804329C4 *>(manager);
}

extern "C" {
/* The screen shimmer (Game/EffectShimmerRW3.cpp): its settings, set, draw, update, remove and
 * create. */
UnkScreenShimmerParams_804370D4 fn_80436F2C(::Effect::System::EffectManager *manager);
void fn_80436FC0(::Effect::System::EffectManager *manager,
                 const UnkScreenShimmerParams_804370D4 &params);
void fn_80436FEC(::Effect::System::EffectManager *manager);
void fn_80437018(::Effect::System::EffectManager *manager, float dt);
void fn_80437044(::Effect::System::EffectManager *manager);
void fn_80437088(::Effect::System::EffectManager *manager,
                 const UnkScreenShimmerParams_804370D4 *params);
/* The fog (Game/EffectFogRW3.cpp): its settings, set, draw, remove and create. */
UnkFog_804388A4 fn_804388A4(::Effect::System::EffectManager *manager);
void fn_8043891C(::Effect::System::EffectManager *manager, const UnkFog_804388A4 &fog);
void fn_80438948(::Effect::System::EffectManager *manager);
void fn_80438974(::Effect::System::EffectManager *manager);
void fn_804389F0(::Effect::System::EffectManager *manager, const UnkFog_804388A4 *fog);
}

#endif
