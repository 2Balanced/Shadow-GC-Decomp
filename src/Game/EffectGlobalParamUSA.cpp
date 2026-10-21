/* SonicteamUSA::System::Effect::EffectGlobalParamUSA (class name and bases from RTTI): the game's
 * global-parameter effect. Besides what its RenderWare base does, an effect of type 4 takes over
 * the effect manager's fog: it saves the fog it found, and while it runs blends from that fog to
 * its own by the blend factor. File name inferred from the class. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include "Effect/System/EffectGlobalParam.h"

using ::Effect::RW3::EffectGlobalParamRW3;
using SonicteamUSA::System::Effect::EffectGlobalParamUSA;
using SonicteamUSA::System::Effect::EffectManagerUSA;

/* from blended toward to by t, as a byte (name inferred). */
template <class T> inline u8 BlendByte(T from, T to, float t) {
    return to * t + from * (1.0f - t);
}

EffectGlobalParamUSA::EffectGlobalParamUSA() {
    m_size -= sizeof(EffectGlobalParamRW3);
    m_size += sizeof(EffectGlobalParamUSA);
    if (!(m_flags & 1)) {
        return;
    }
    EffectManagerUSA *manager = dynamic_cast<EffectManagerUSA *>(m_manager);
    if (manager != 0) {
        switch (m_data->type) {
        case 4: {
            m_savedFog = fn_800288D8(manager);
            const UnkGlobalParamData_80438A3C *data = m_data;
            if (field_0xFC & 0x10000) {
                m_targetFog.enabled = true;
            } else {
                m_targetFog.enabled = false;
            }
            if (data->flags & 1) {
                m_targetFog.field_0x1 = true;
            } else {
                m_targetFog.field_0x1 = false;
            }
            m_targetFog.field_0x4 = data->field_0x8;
            m_targetFog.color.r = data->r;
            m_targetFog.color.b = data->b;
            m_targetFog.color.g = data->g;
            break;
        }
        }
    }
}

EffectGlobalParamUSA::~EffectGlobalParamUSA() {
    EffectManagerUSA *manager = dynamic_cast<EffectManagerUSA *>(m_manager);
    if (manager != 0) {
        switch (m_data->type) {
        case 4:
            fn_80028984(manager, &m_targetFog);
            break;
        }
    }
}

void EffectGlobalParamUSA::Update(float dt) {
    EffectGlobalParamRW3::Update(dt);
    if ((m_flags & 1) && !(m_flags & 4)) {
        EffectManagerUSA *manager = dynamic_cast<EffectManagerUSA *>(m_manager);
        if (manager != 0) {
            switch (m_data->type) {
            case 4: {
                UnkFogParam_8002893C fog = m_targetFog;
                if (m_blend < 0.9999f) {
                    if (fog.enabled || m_savedFog.enabled) {
                        fog.enabled = true;
                    } else {
                        fog.enabled = false;
                    }
                    if (fog.field_0x4 != m_savedFog.field_0x4) {
                        fog.field_0x4 = BlendByte(m_savedFog.field_0x4, fog.field_0x4, m_blend);
                    }
                    if (fog.color.r != m_savedFog.color.r) {
                        fog.color.r = BlendByte(m_savedFog.color.r, fog.color.r, m_blend);
                    }
                    if (fog.color.b != m_savedFog.color.b) {
                        fog.color.b = BlendByte(m_savedFog.color.b, fog.color.b, m_blend);
                    }
                    if (fog.color.g != m_savedFog.color.g) {
                        fog.color.g = BlendByte(m_savedFog.color.g, fog.color.g, m_blend);
                    }
                }
                fn_80028984(manager, &fog);
                break;
            }
            }
        }
    }
}
