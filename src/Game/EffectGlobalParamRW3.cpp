/* Effect::RW3::EffectGlobalParamRW3 (class name and bases from RTTI): the RenderWare
 * global-parameter element. Depending on its data it takes over one of the RenderWare effect
 * manager's screen filters (the colour filter, the screen blur, the fog or the screen shimmer;
 * Game/EffectFilterRW3.h): it keeps the filter's settings from when it started and the settings
 * its data asks for, blends between them by the element's blend factor while it runs, and leaves
 * the filter at the requested settings when it ends. File name from the class; member names are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#define EFFECT_GLOBAL_PARAM_RW3_FULL
#include "Game/EffectFilterRW3.h"

using ::Effect::RW3::EffectManagerRW3;
using ::Effect::System::EffectLaunchInfo;
using ::Effect::System::EffectManager;
using ::Effect::System::UnkEffectDesc_801D0AFC;

extern "C" {
/* The colour filter (the unit at 0x804303D8) and the screen blur (Game/EffectScreenBlurRW3.cpp):
 * their settings, and setting them. */
UnkScreenColor_80430844 fn_80430844(EffectManager *manager);
void fn_804308C0(EffectManager *manager, const UnkScreenColor_80430844 &color);
UnkScreenBlurParam_8042D974 fn_8042E034(EffectManager *manager);
void fn_8042E098(EffectManager *manager, const UnkScreenBlurParam_8042D974 &blur);
}

/* The data a global-parameter element is launched with, in its four layouts (from the
 * constructor). */
struct UnkGlobalParamColorData_80439554 {
    /* 0x00 */ s32 m_type; /* 0 */
    /* 0x04 */ u32 m_flags;
    /* 0x08 */ u8 field_0x8;
    /* 0x09 */ u8 field_0x9;
    /* 0x0C */ float field_0xC;
    /* 0x10 */ float field_0x10;
    /* 0x14 */ s32 field_0x14;
    /* 0x18 */ s32 field_0x18;
    /* 0x1C */ s32 field_0x1C;
    /* 0x20 */ s32 field_0x20;
};

struct UnkGlobalParamBlurData_80439554 {
    /* 0x00 */ s32 m_type; /* 1 */
    /* 0x04 */ u32 m_flags;
    /* 0x08 */ u8 m_alpha;
    /* 0x0C */ float m_scale;
    /* 0x10 */ float field_0x10;
    /* 0x14 */ float field_0x14;
};

struct UnkGlobalParamFogData_80439554 {
    /* 0x00 */ s32 m_type; /* 2 */
    /* 0x04 */ u32 m_flags; /* bit 4: m_enabled, bit 5: m_mode */
    /* 0x08 */ UnkFogColor_804388A4 m_color;
    /* 0x0C */ UnkFogColor_804388A4 m_color2;
};

struct UnkGlobalParamShimmerData_80439554 {
    /* 0x00 */ s32 m_type; /* 3 */
    /* 0x04 */ u32 m_flags;
    /* 0x08 */ u32 field_0x8;
    /* 0x0C */ s32 m_count[2];
    /* 0x14 */ float m_start[2];
    /* 0x1C */ float m_end[2];
    /* 0x24 */ float m_amplitude[2];
    /* 0x2C */ float m_cycle[2];
    /* 0x34 */ float m_waveLength[2];
};

namespace Effect {
namespace RW3 {

EffectGlobalParamRW3::EffectGlobalParamRW3(const UnkEffectDesc_801D0AFC *desc,
                                           EffectLaunchInfo *info, EffectManager *manager)
    : EffectGlobalParam(desc, info, manager), m_fromFog(0, 0, 0), m_toFog(0, 0, 0),
      m_fromShimmer(false, 0xFF, 0, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f),
      m_toShimmer(false, 0xFF, 0, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f) {
    m_size -= sizeof(EffectGlobalParam);
    m_size += sizeof(EffectGlobalParamRW3);
    if (!(m_flags & 1)) {
        return;
    }
    EffectManagerRW3 *rw3Manager = dynamic_cast<EffectManagerRW3 *>(m_manager);
    if (rw3Manager != 0) {
        switch (m_data->type) {
        case 0: {
            m_fromColor = fn_80430844(rw3Manager);
            const UnkGlobalParamColorData_80439554 *data =
                reinterpret_cast<const UnkGlobalParamColorData_80439554 *>(m_data);
            if (field_0xFC & 0x10000) {
                m_toColor.field_0x0 = true;
            } else {
                m_toColor.field_0x0 = false;
            }
            if (data->m_flags & 1) {
                m_toColor.field_0x1 = true;
            } else {
                m_toColor.field_0x1 = false;
            }
            if (data->m_flags & 2) {
                m_toColor.field_0x2 = true;
            } else {
                m_toColor.field_0x2 = false;
            }
            if (data->m_flags & 4) {
                m_toColor.field_0x3 = true;
            } else {
                m_toColor.field_0x3 = false;
            }
            if (data->m_flags & 8) {
                m_toColor.field_0x4 = true;
            } else {
                m_toColor.field_0x4 = false;
            }
            m_toColor.field_0x5 = data->field_0x8;
            m_toColor.field_0x6 = data->field_0x9;
            m_toColor.field_0x8 = data->field_0xC;
            m_toColor.field_0xC = data->field_0x10;
            m_toColor.field_0x10 = data->field_0x14;
            m_toColor.field_0x14 = data->field_0x18;
            m_toColor.field_0x18 = data->field_0x1C;
            m_toColor.field_0x1C = data->field_0x20;
            break;
        }
        case 1: {
            m_fromBlur = fn_8042E034(rw3Manager);
            const UnkGlobalParamBlurData_80439554 *data =
                reinterpret_cast<const UnkGlobalParamBlurData_80439554 *>(m_data);
            if (field_0xFC & 0x10000) {
                m_toBlur.enabled = true;
            } else {
                m_toBlur.enabled = false;
            }
            m_toBlur.alpha = data->m_alpha;
            m_toBlur.scale = data->m_scale;
            m_toBlur.extra.field_0x0 = data->field_0x10;
            m_toBlur.extra.field_0x4 = data->field_0x14;
            break;
        }
        case 2: {
            m_fromFog = fn_804388A4(rw3Manager);
            const UnkGlobalParamFogData_80439554 *data =
                reinterpret_cast<const UnkGlobalParamFogData_80439554 *>(m_data);
            if (field_0xFC & 0x10000) {
                m_toFog.m_on = true;
            } else {
                m_toFog.m_on = false;
            }
            if (data->m_flags & 0x10) {
                m_toFog.m_enabled = true;
            } else {
                m_toFog.m_enabled = false;
            }
            if (data->m_flags & 0x20) {
                m_toFog.m_mode = true;
            } else {
                m_toFog.m_mode = false;
            }
            m_toFog.m_color.r = data->m_color.r;
            m_toFog.m_color.g = data->m_color.g;
            m_toFog.m_color.b = data->m_color.b;
            m_toFog.m_color.a = data->m_color.a;
            m_toFog.m_color2.r = data->m_color2.r;
            m_toFog.m_color2.g = data->m_color2.g;
            m_toFog.m_color2.b = data->m_color2.b;
            m_toFog.m_color2.a = data->m_color2.a;
            break;
        }
        case 3: {
            m_fromShimmer = fn_80436F2C(rw3Manager);
            const UnkGlobalParamShimmerData_80439554 *data =
                reinterpret_cast<const UnkGlobalParamShimmerData_80439554 *>(m_data);
            if (field_0xFC & 0x10000) {
                m_toShimmer.m_enabled = true;
            } else {
                m_toShimmer.m_enabled = false;
            }
            m_toShimmer.m_x.m_count = data->m_count[0];
            m_toShimmer.m_x.m_amplitude = data->m_amplitude[0];
            m_toShimmer.m_x.m_cycle = data->m_cycle[0];
            m_toShimmer.m_x.m_waveLength = data->m_waveLength[0];
            m_toShimmer.m_x.m_start = data->m_start[0];
            m_toShimmer.m_x.m_end = data->m_end[0];
            m_toShimmer.m_y.m_count = data->m_count[1];
            m_toShimmer.m_y.m_amplitude = data->m_amplitude[1];
            m_toShimmer.m_y.m_cycle = data->m_cycle[1];
            m_toShimmer.m_y.m_waveLength = data->m_waveLength[1];
            m_toShimmer.m_y.m_start = data->m_start[1];
            m_toShimmer.m_y.m_end = data->m_end[1];
            break;
        }
        }
    }
}

EffectGlobalParamRW3::~EffectGlobalParamRW3() {
    EffectManagerRW3 *rw3Manager = dynamic_cast<EffectManagerRW3 *>(m_manager);
    if (rw3Manager != 0) {
        switch (m_data->type) {
        case 0:
            fn_804308C0(rw3Manager, m_toColor);
            break;
        case 1:
            fn_8042E098(rw3Manager, m_toBlur);
            break;
        case 2:
            fn_8043891C(rw3Manager, m_toFog);
            break;
        case 3:
            fn_80436FC0(rw3Manager, m_toShimmer);
            break;
        }
    }
}

void EffectGlobalParamRW3::Update(float dt) {
    EffectGlobalParam::Update(dt);
    if ((m_flags & 1) && !(m_flags & 4)) {
        EffectManagerRW3 *rw3Manager = dynamic_cast<EffectManagerRW3 *>(m_manager);
        if (rw3Manager != 0) {
            switch (m_data->type) {
            case 0: {
                UnkScreenColor_80430844 color = m_toColor;
                if (m_blend < 0.9999f) {
                    if (color.field_0x0 || m_fromColor.field_0x0) {
                        color.field_0x0 = true;
                    } else {
                        color.field_0x0 = false;
                    }
                    if (color.field_0x1 || m_fromColor.field_0x1) {
                        color.field_0x1 = true;
                    } else {
                        color.field_0x1 = false;
                    }
                    if (color.field_0x2 || m_fromColor.field_0x2) {
                        color.field_0x2 = true;
                    } else {
                        color.field_0x2 = false;
                    }
                    if (color.field_0x5 != m_fromColor.field_0x5) {
                        color.field_0x5 =
                            color.field_0x5 * m_blend + m_fromColor.field_0x5 * (1.0f - m_blend);
                    }
                    if (color.field_0x6 != m_fromColor.field_0x6) {
                        color.field_0x6 =
                            color.field_0x6 * m_blend + m_fromColor.field_0x6 * (1.0f - m_blend);
                    }
                    color.field_0x8 = color.field_0x8 * m_blend + m_fromColor.field_0x8 * (1.0f - m_blend);
                    color.field_0xC = color.field_0xC * m_blend + m_fromColor.field_0xC * (1.0f - m_blend);
                    if (color.field_0x10 != m_fromColor.field_0x10) {
                        color.field_0x10 =
                            color.field_0x10 * m_blend + m_fromColor.field_0x10 * (1.0f - m_blend);
                    }
                    if (color.field_0x14 != m_fromColor.field_0x14) {
                        color.field_0x14 =
                            color.field_0x14 * m_blend + m_fromColor.field_0x14 * (1.0f - m_blend);
                    }
                    if (color.field_0x18 != m_fromColor.field_0x18) {
                        color.field_0x18 =
                            color.field_0x18 * m_blend + m_fromColor.field_0x18 * (1.0f - m_blend);
                    }
                    if (color.field_0x1C != m_fromColor.field_0x1C) {
                        color.field_0x1C =
                            color.field_0x1C * m_blend + m_fromColor.field_0x1C * (1.0f - m_blend);
                    }
                }
                fn_804308C0(rw3Manager, color);
                break;
            }
            case 1: {
                UnkScreenBlurParam_8042D974 blur = m_toBlur;
                if (m_blend < 0.9999f) {
                    if (blur.enabled || m_fromBlur.enabled) {
                        blur.enabled = true;
                    } else {
                        blur.enabled = false;
                    }
                    if (blur.alpha != m_fromBlur.alpha) {
                        blur.alpha = blur.alpha * m_blend + m_fromBlur.alpha * (1.0f - m_blend);
                    }
                    blur.scale = blur.scale * m_blend + m_fromBlur.scale * (1.0f - m_blend);
                    blur.extra.field_0x0 =
                        blur.extra.field_0x0 * m_blend + m_fromBlur.extra.field_0x0 * (1.0f - m_blend);
                    blur.extra.field_0x4 =
                        blur.extra.field_0x4 * m_blend + m_fromBlur.extra.field_0x4 * (1.0f - m_blend);
                }
                fn_8042E098(rw3Manager, blur);
                break;
            }
            case 2: {
                UnkFog_804388A4 fog = m_toFog;
                if (m_blend < 0.9999f) {
                    if (fog.m_on || m_fromFog.m_on) {
                        fog.m_on = true;
                    } else {
                        fog.m_on = false;
                    }
                    if (fog.m_color.r != m_fromFog.m_color.r) {
                        fog.m_color.r = fog.m_color.r * m_blend + m_fromFog.m_color.r * (1.0f - m_blend);
                    }
                    if (fog.m_color.g != m_fromFog.m_color.g) {
                        fog.m_color.g = fog.m_color.g * m_blend + m_fromFog.m_color.g * (1.0f - m_blend);
                    }
                    if (fog.m_color.b != m_fromFog.m_color.b) {
                        fog.m_color.b = fog.m_color.b * m_blend + m_fromFog.m_color.b * (1.0f - m_blend);
                    }
                    if (fog.m_color.a != m_fromFog.m_color.a) {
                        fog.m_color.a = fog.m_color.a * m_blend + m_fromFog.m_color.a * (1.0f - m_blend);
                    }
                    if (fog.m_color2.r != m_fromFog.m_color2.r) {
                        fog.m_color2.r =
                            fog.m_color2.r * m_blend + m_fromFog.m_color2.r * (1.0f - m_blend);
                    }
                    if (fog.m_color2.g != m_fromFog.m_color2.g) {
                        fog.m_color2.g =
                            fog.m_color2.g * m_blend + m_fromFog.m_color2.g * (1.0f - m_blend);
                    }
                    if (fog.m_color2.b != m_fromFog.m_color2.b) {
                        fog.m_color2.b =
                            fog.m_color2.b * m_blend + m_fromFog.m_color2.b * (1.0f - m_blend);
                    }
                    if (fog.m_color2.a != m_fromFog.m_color2.a) {
                        fog.m_color2.a =
                            fog.m_color2.a * m_blend + m_fromFog.m_color2.a * (1.0f - m_blend);
                    }
                }
                fn_8043891C(rw3Manager, fog);
                break;
            }
            case 3: {
                UnkScreenShimmerParams_804370D4 shimmer = m_toShimmer;
                if (m_blend < 0.9999f) {
                    if (shimmer.m_enabled || m_fromShimmer.m_enabled) {
                        shimmer.m_enabled = true;
                    } else {
                        shimmer.m_enabled = false;
                    }
                    shimmer.m_x.m_amplitude = shimmer.m_x.m_amplitude * m_blend +
                                              m_fromShimmer.m_x.m_amplitude * (1.0f - m_blend);
                    shimmer.m_x.m_cycle = shimmer.m_x.m_cycle * m_blend + m_fromShimmer.m_x.m_cycle * (1.0f - m_blend);
                    shimmer.m_x.m_waveLength =
                        shimmer.m_x.m_waveLength * m_blend + m_fromShimmer.m_x.m_waveLength * (1.0f - m_blend);
                    shimmer.m_x.m_start = shimmer.m_x.m_start * m_blend + m_fromShimmer.m_x.m_start * (1.0f - m_blend);
                    shimmer.m_x.m_end = shimmer.m_x.m_end * m_blend + m_fromShimmer.m_x.m_end * (1.0f - m_blend);
                    shimmer.m_y.m_amplitude = shimmer.m_y.m_amplitude * m_blend +
                                              m_fromShimmer.m_y.m_amplitude * (1.0f - m_blend);
                    shimmer.m_y.m_cycle = shimmer.m_y.m_cycle * m_blend + m_fromShimmer.m_y.m_cycle * (1.0f - m_blend);
                    shimmer.m_y.m_waveLength =
                        shimmer.m_y.m_waveLength * m_blend + m_fromShimmer.m_y.m_waveLength * (1.0f - m_blend);
                    shimmer.m_y.m_start = shimmer.m_y.m_start * m_blend + m_fromShimmer.m_y.m_start * (1.0f - m_blend);
                    shimmer.m_y.m_end = shimmer.m_y.m_end * m_blend + m_fromShimmer.m_y.m_end * (1.0f - m_blend);
                }
                fn_80436FC0(rw3Manager, shimmer);
                break;
            }
            }
        }
    }
}

} // namespace RW3
} // namespace Effect
