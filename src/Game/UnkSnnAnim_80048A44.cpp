/* The light animation of an event scene's .SNN file (class name unknown; Game/UnkSnnFile_802289B0.cpp
 * builds and plays it): a light motion played onto a light of the renderer (Game/UnkRenderer_800289B0.h).
 * Type 0 animates one of the renderer's light settings, type 1 the directional light and type 2 a
 * point light, which it adds when no slot is given. Names are inferred. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). Other units call the
 * destructor and the advance by their fn_ names, so they stay C functions. */
#include "Game/UnkRenderer_800289B0.h"
#include <dolphin/types.h>
#include "size_assert.h"
#include <string.h>

/* Three angles in 1/65536 of a turn (see Game/LightMotion.cpp). */
struct UnkLightAngles_80010EE0 {
    s32 x, y, z;
};

/* A light's values, as a light motion sets them (see Game/LightMotion.cpp; the size depends on the
 * light's kind). */
struct UnkLightData_80010EE0 {
    /* 0x00 */ u32 m_set;
    /* 0x04 */ RwRGBAReal m_color;
    /* 0x14 */ float field_0x14;
    /* 0x18 */ union {
        UnkLightAngles_80010EE0 m_angles;
        RwV3d m_position;
        float m_values[3];
    };
};

/* A light a light motion plays on (see Game/LightMotion.cpp): its kind and its values. */
struct UnkLight_80010EE0 {
    UnkLight_80010EE0(u32 kind) {
        m_kind = kind;
        m_data = 0;
        switch (m_kind) {
        case 1:
            m_data = (UnkLightData_80010EE0 *)operator new(0x24);
            memset(m_data, 0, 0x24);
            m_data->m_values[1] = -1.0f;
            break;
        case 2:
            m_data = (UnkLightData_80010EE0 *)operator new(0x2C);
            memset(m_data, 0, 0x2C);
            break;
        case 4:
            m_data = (UnkLightData_80010EE0 *)operator new(0x40);
            memset(m_data, 0, 0x40);
            break;
        case 8:
            m_data = (UnkLightData_80010EE0 *)operator new(0x44);
            memset(m_data, 0, 0x44);
            break;
        case 16:
            m_data = (UnkLightData_80010EE0 *)operator new(0x20);
            memset(m_data, 0, 0x20);
            break;
        }
        m_data->m_color.red = m_data->m_color.green = m_data->m_color.blue = 255.0f;
        m_data->field_0x14 = 1.0f;
    }
    ~UnkLight_80010EE0() {
        delete m_data;
    }

    /* 0x00 */ u32 m_kind;
    /* 0x04 */ UnkLightData_80010EE0 *m_data;
};

struct UnkLightMotion_80010BF8;
/* A buffer the animation frees (never set here; type unknown). */
struct UnkSnnBuffer_800489C0 {};

class UnkSnnAnim_80048A44 {
  public:
    UnkSnnAnim_80048A44(void *motion, int type, int slot, int light);

    /* 0x00 */ int m_type;
    /* 0x04 */ UnkLightMotion_80010BF8 *m_motion;
    /* 0x08 */ float m_time;
    /* 0x0C */ float field_0xC;
    /* 0x10 */ float field_0x10;
    /* 0x14 */ float field_0x14;
    /* 0x18 */ UnkLight_80010EE0 *m_light;
    /* 0x1C */ UnkSnnBuffer_800489C0 *field_0x1C;
    /* 0x20 */ s8 m_slot;
};
SIZE_ASSERT(UnkSnnAnim_80048A44, 0x24);

extern "C" {
/* Plays a light motion at a frame onto a light (Game/LightMotion.cpp). */
void fn_80010EE0(UnkLight_80010EE0 *light, UnkLightMotion_80010BF8 *motion, float frame);
/* Advances a motion's time by step; true at its end (see Game/CameraDataCtrl.cpp). */
int fn_80011B7C(UnkLightMotion_80010BF8 *motion, float *time, float *step);

void fn_80048D3C(UnkLight_80010EE0 *light, const UnkLightDesc_80047848 *desc);
void fn_80048DF4(UnkLight_80010EE0 *light, const UnkLightSetting_80047F34 *setting);
}

/* Takes a light setting's directional colour and angles. */
extern "C" void fn_80048DF4(UnkLight_80010EE0 *light, const UnkLightSetting_80047F34 *setting) {
    UnkLightData_80010EE0 *data = light->m_data;
    data->m_color = setting->m_directional;
    data->m_angles.x = setting->m_angles.x * 32768 / 180.0f;
    data->m_angles.y = setting->m_angles.y * 32768 / 180.0f;
}

/* Takes a light's colour, and its angles or position. */
extern "C" void fn_80048D3C(UnkLight_80010EE0 *light, const UnkLightDesc_80047848 *desc) {
    light->m_data->m_color = desc->m_color;
    switch (light->m_kind) {
    case 1: {
        UnkLightAngles_80010EE0 *angles = &light->m_data->m_angles;
        angles->x = 32768.0f * desc->m_angleX / 180.0f;
        angles->y = 32768.0f * desc->m_angleY / 180.0f;
        break;
    }
    case 2:
        memcpy(&light->m_data->m_position, &desc->m_position, sizeof(RwV3d));
        break;
    }
}

UnkSnnAnim_80048A44::UnkSnnAnim_80048A44(void *motion, int type, int slot, int light)
    : m_type(type), m_motion((UnkLightMotion_80010BF8 *)motion), m_time(0.0f), field_0x1C(0), m_slot(slot) {
    int kinds[3] = {1, 1, 2};
    if (m_motion == 0) {
        return;
    }
    m_light = new UnkLight_80010EE0(kinds[m_type]);
    field_0xC = field_0x10 = field_0x14 = 0.0f;
    if (m_type == 0) {
        if (m_slot < 0 || m_slot >= 16) {
            return;
        }
        if (fn_800289B0()->m_lights[1] == 0) {
            return;
        }
        fn_80048DF4(m_light, fn_800289B0()->GetSetting(m_slot));
    } else {
        char lightTypes[3] = {1, 1, 2};
        if (m_slot < 0) {
            UnkLightDesc_80047848 desc;
            memset(&desc, 0, sizeof(desc));
            desc.m_type = lightTypes[m_type];
            if ((m_slot = fn_80048484(fn_800289B0(), &desc)) < 0) {
                return;
            }
        } else {
            if (fn_800289B0()->GetLight(m_slot) == 0) {
                return;
            }
            UnkLightDesc_80047848 *desc = fn_800289B0()->GetDesc(m_slot);
            if (lightTypes[m_type] != desc->m_type) {
                return;
            }
            fn_80048D3C(m_light, desc);
        }
        if ((UnkLight_80010EE0 *)light != 0) {
            *m_light = *(UnkLight_80010EE0 *)light;
        }
    }
}

/* Destroys the animation (a destructor taking the delete flag). */
extern "C" UnkSnnAnim_80048A44 *fn_800489C0(UnkSnnAnim_80048A44 *anim, short flags) {
    if (anim != 0) {
        if (anim->field_0x1C != 0) {
            delete anim->field_0x1C;
            anim->field_0x1C = 0;
        }
        if (anim->m_light != 0) {
            delete anim->m_light;
            anim->m_light = 0;
        }
        if (flags > 0) {
            operator delete(anim);
        }
    }
    return anim;
}

/* Plays the animation and advances it by step; true at its end. */
extern "C" int fn_8004883C(UnkSnnAnim_80048A44 *anim, float step) {
    fn_80010EE0(anim->m_light, anim->m_motion, anim->m_time);
    UnkLightData_80010EE0 *data;
    int ended = fn_80011B7C(anim->m_motion, &anim->m_time, &step);
    data = anim->m_light->m_data;
    switch (anim->m_type) {
    case 0:
        fn_800482B0(fn_800289B0(), anim->m_slot, &data->m_color);
        fn_80048278(fn_800289B0(), anim->m_slot, 180.0f * data->m_angles.x / 32768.0f,
                    180.0f * data->m_angles.y / 32768.0f);
        break;
    case 1:
        fn_80048238(fn_800289B0(), anim->m_slot, &data->m_color);
        fn_8004815C(fn_800289B0(), anim->m_slot, 180.0f * data->m_angles.x / 32768.0f,
                    180.0f * data->m_angles.y / 32768.0f);
        break;
    case 2:
        fn_80048238(fn_800289B0(), anim->m_slot, &data->m_color);
        fn_80048060(fn_800289B0(), anim->m_slot, &data->m_position);
        break;
    }
    return ended;
}
