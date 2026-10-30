/* The renderer behind fn_800289B0 (class name unknown; Game/EffectSystemUSA.cpp creates it): its
 * RenderWare world and the eight lights it keeps in it. Slot 0 is the ambient light and slot 1
 * the directional one; slots 2 to 7 take point and spot lights (fn_80048484). Seventeen light
 * settings (ambient and directional colours and the directional light's angles) are loaded per
 * stage from stgNNNN/stgNNNN_light.bin and chosen by index. Other units call these functions by
 * their fn_ names, so they stay C functions taking the renderer. Names are inferred. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/UnkRenderer_800289B0.h"
#include <dolphin/types.h>
#include "size_assert.h"
#include <string.h>
#include <stdio.h>

extern "C" {
/* RenderWare (by their arguments and use): the light's world, RpWorldRemoveLight,
 * RpWorldAddLight, RpLightSetFrame, RwFrameDestroy, RpLightDestroy, RpLightCreate, RwFrameCreate,
 * RpLightSetColor, RwFrameTranslate, RwFrameRotate, RpLightSetRadius, RpLightSetConeAngle,
 * RpWorldCreate and RpWorldDestroy. */
RpWorld *fn_804626EC(RpLight *light);
RpWorld *fn_80462618(RpWorld *world, RpLight *light);
RpWorld *fn_80462578(RpWorld *world, RpLight *light);
RpLight *fn_8048D7C0(RpLight *light, RwFrame *frame);
int fn_80486E2C(RwFrame *frame);
int fn_8045ACD4(RpLight *light);
RpLight *fn_8045AD34(int type);
RwFrame *fn_80486D00(void);
RpLight *fn_8045A7C4(RpLight *light, RwRGBAReal *color);
RwFrame *fn_804875B0(RwFrame *frame, const RwV3d *translation, int combine);
RwFrame *fn_80487784(RwFrame *frame, const RwV3d *axis, float angle, int combine);
RpLight *fn_8045A784(RpLight *light, float radius);
RpLight *fn_8045AA44(RpLight *light, float angle);
RpWorld *fn_80460138(RwBBox *box);
int fn_8045FFA0(RpWorld *world);

/* The file system (Game/UnkFileSystem_800371D8.cpp): a whole file read into memory, and freed. */
void *fn_800371D8(void);
void *fn_8004509C(void *fileSystem, const char *path, u32 *size);
void fn_80049B34(void *data);
/* Swaps the byte order of count records described by format. */
void fn_80043C58(void *data, const char *format, u32 count);
}

/* The RenderWare light type of each light type: ambient, directional, point, spot and soft
 * spot. */
static char s_rwLightTypes[5] = {2, 1, 0x80, 0x81, 0x82};

static RwV3d s_xAxis = {1.0f, 0.0f, 0.0f};
static RwV3d s_yAxis = {0.0f, 1.0f, 0.0f};

/* One of the renderer's lights (class name unknown). */
class UnkLight_80047D60 {
  public:
    UnkLight_80047D60(s8 slot, UnkLightDesc_80047848 *desc);
    UnkLight_80047D60(s8 slot);
    ~UnkLight_80047D60();

    RpLight *CreateOfType(s8 type);
    RpLight *Create(UnkLightDesc_80047848 *desc);
    bool SetColor(RwRGBAReal &color);

    void Destroy() {
        if (m_light != 0) {
            RpWorld *world = fn_804626EC(m_light);
            RwFrame *frame = m_light->parent;
            if (world != 0) {
                fn_80462618(world, m_light);
            }
            if (frame != 0) {
                fn_8048D7C0(m_light, 0);
                fn_80486E2C(frame);
            }
            fn_8045ACD4(m_light);
        }
        m_type = -1;
    }

    bool SetPosition(RwV3d &position) {
        if (m_light == 0) {
            return false;
        }
        if (m_type == 0 || m_type == 1) {
            return false;
        }
        RwV3d *dst = &m_position;
        dst->x = position.x;
        dst->y = position.y;
        dst->z = position.z;
        RwFrame *frame = m_light->parent;
        if (m_type == 2) {
            fn_804875B0(frame, dst, 0);
        } else {
            fn_80487784(frame, &s_xAxis, m_angleX, 0);
            fn_80487784(frame, &s_yAxis, m_angleY, 2);
            fn_804875B0(frame, dst, 2);
        }
        return true;
    }

    bool SetAngles(float x, float y) {
        if (m_light == 0) {
            return false;
        }
        if (m_type == 0 || m_type == 2) {
            return false;
        }
        m_angleX = x;
        m_angleY = y;
        RwFrame *frame = m_light->parent;
        fn_80487784(frame, &s_xAxis, x, 0);
        fn_80487784(frame, &s_yAxis, y, 2);
        if (m_type != 1) {
            fn_804875B0(frame, &m_position, 2);
        }
        return true;
    }

    bool SetRadius(float radius) {
        if (m_light == 0) {
            return false;
        }
        if (m_type == 0 || m_type == 1) {
            return false;
        }
        m_radius = radius;
        fn_8045A784(m_light, m_radius);
        return true;
    }

    bool SetConeAngle(float angle) {
        if (m_light == 0) {
            return false;
        }
        if (m_type != 3 && m_type != 4) {
            return false;
        }
        m_coneAngle = angle;
        fn_8045AA44(m_light, m_coneAngle);
        return true;
    }

    void RemoveFromWorld() {
        if (m_light != 0) {
            RpWorld *world = fn_804626EC(m_light);
            if (world != 0) {
                fn_80462618(world, m_light);
            }
        }
    }

    void AddToWorld(RpWorld *world) {
        if (m_light != 0) {
            RpWorld *current = fn_804626EC(m_light);
            if (current != 0) {
                fn_80462618(current, m_light);
            }
            fn_80462578(world, m_light);
            m_world = world;
        }
    }

    /* 0x00 */ s8 m_type;
    /* 0x01 */ s8 m_slot;
    /* 0x04 */ int m_field_0x4;
    /* 0x08 */ RpLight *m_light;
    /* 0x0C */ RpWorld *m_world;
    /* 0x10 */ u8 field_0x10[0x14 - 0x10];
    /* 0x14 */ RwRGBAReal m_color;
    /* 0x24 */ float m_angleX;
    /* 0x28 */ float m_angleY;
    /* 0x2C */ RwV3d m_position;
    /* 0x38 */ float m_radius;
    /* 0x3C */ float m_coneAngle;
};
SIZE_ASSERT(UnkLight_80047D60, 0x40);

static UnkLightSetting_80047F34 s_defaultSetting = {
    {0.4f, 0.4f, 0.4f, 0.2f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0, 0}};
static UnkLightSetting_80047F34 s_defaultSetting16 = {
    {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0, 0}};

extern "C" {
void fn_80047780(UnkRenderer_800289B0 *renderer);
void fn_800477BC(UnkRenderer_800289B0 *renderer);
void fn_80047DFC(UnkRenderer_800289B0 *renderer, RpWorld *world);
void fn_80047E70(UnkRenderer_800289B0 *renderer, s8 slot);
void fn_80047EC8(UnkRenderer_800289B0 *renderer, s8 slot, RpWorld *world);
bool fn_8004815C(UnkRenderer_800289B0 *renderer, s8 slot, float x, float y);
bool fn_80048238(UnkRenderer_800289B0 *renderer, s8 slot, RwRGBAReal *color);
void fn_800482E0(UnkRenderer_800289B0 *renderer, s8 index);
void fn_80048438(UnkRenderer_800289B0 *renderer, s8 slot);
void fn_8004851C(UnkRenderer_800289B0 *renderer);
bool fn_80048654(UnkRenderer_800289B0 *renderer, UnkLightSettingTable_80048654 *table, const char *path);
}

UnkRenderer_800289B0::UnkRenderer_800289B0() {
    RwBBox box;
    box.inf.z = -10000000.0f;
    box.inf.y = -10000000.0f;
    box.inf.x = -10000000.0f;
    box.sup.z = 10000000.0f;
    box.sup.y = 10000000.0f;
    box.sup.x = 10000000.0f;
    m_world = fn_80460138(&box);
}

UnkRenderer_800289B0::~UnkRenderer_800289B0() {
    if (m_world != 0) {
        fn_8045FFA0(m_world);
    }
}

/* Loads the stage's light settings. */
extern "C" bool fn_80048710(UnkRenderer_800289B0 *renderer, int stage) {
    char path[0x20];
    sprintf(path, "stg%04d/stg%04d_light.bin", stage, stage);
    if (fn_80048654(renderer, 0, path)) {
        UnkLightSettingTable_80048654 *table = &renderer->m_table;
        *table = renderer->m_table;
        return true;
    }
    return false;
}

/* Reads a light file into table (the renderer's own when 0). */
extern "C" bool fn_80048654(UnkRenderer_800289B0 *renderer, UnkLightSettingTable_80048654 *table, const char *path) {
    if (table == 0) {
        table = &renderer->m_table;
    }
    u32 size;
    void *data = fn_8004509C(fn_800371D8(), path, &size);
    if (data != 0) {
        if (size >= sizeof(UnkLightSettingTable_80048654)) {
            memcpy(table, data, sizeof(UnkLightSettingTable_80048654));
            fn_80043C58(table, "xxxxxxxxxxxii", 16);
        } else {
            memcpy(table, data, size);
            fn_80043C58(table, "xxxxxxxxxxxii", size / sizeof(UnkLightSetting_80047F34));
        }
        fn_80049B34(data);
        return true;
    }
    return false;
}

/* Creates the ambient and directional lights (from StageManager::SetUpBlocks). */
extern "C" bool fn_8004854C(UnkRenderer_800289B0 *renderer) {
    UnkLight_80047D60 *light = new UnkLight_80047D60(0);
    if (light != 0) {
        renderer->m_lights[0] = light;
    } else {
        return false;
    }
    light = new UnkLight_80047D60(1);
    if (light != 0) {
        renderer->m_lights[1] = light;
    } else {
        fn_80048438(renderer, 0);
        return false;
    }
    renderer->m_defaultSetting = 4;
    renderer->SetSettingIndex(renderer->m_defaultSetting);
    fn_800477BC(renderer);
    renderer->Reset();
    fn_80047DFC(renderer, 0);
    return true;
}

extern "C" void fn_8004851C(UnkRenderer_800289B0 *renderer) {
    for (u8 i = 0; i < 2; i++) {
        renderer->m_field_0x457[i] = 0;
        renderer->m_field_0x459[i] = 0;
    }
}

/* Adds a point or spot light in a free slot; returns the slot, or -1. */
extern "C" s8 fn_80048484(UnkRenderer_800289B0 *renderer, UnkLightDesc_80047848 *desc) {
    for (s8 i = 2; i < 8; i++) {
        if (renderer->m_lights[i] == 0) {
            UnkLight_80047D60 *light = new UnkLight_80047D60(i, desc);
            if (light != 0) {
                renderer->m_lights[i] = light;
                return i;
            }
            return -1;
        }
    }
    return -1;
}

extern "C" void fn_80048438(UnkRenderer_800289B0 *renderer, s8 slot) {
    if (renderer->m_lights[slot] != 0) {
        delete renderer->m_lights[slot];
        renderer->m_lights[slot] = 0;
    }
}

/* Removes the ambient and directional lights (from StageManager::ReleaseBlocks). */
extern "C" void fn_800483FC(UnkRenderer_800289B0 *renderer) {
    fn_80048438(renderer, 0);
    fn_80048438(renderer, 1);
}

/* Removes the point and spot lights (from StageManager::ReleaseBlocks). */
extern "C" void fn_800483B0(UnkRenderer_800289B0 *renderer) {
    for (s8 i = 2; i < 8; i++) {
        fn_80048438(renderer, i);
    }
}

extern "C" void fn_80048390(UnkRenderer_800289B0 *renderer, s8 index) {
    if (index < 0 || index >= 17) {
        return;
    }
    renderer->m_settingIndex = index;
}

/* Applies a light setting to the ambient and directional lights. */
extern "C" void fn_800482E0(UnkRenderer_800289B0 *renderer, s8 index) {
    if (index != renderer->m_currentSetting) {
        UnkLightSetting_80047F34 &setting = renderer->m_table.m_settings[index];
        fn_80048238(renderer, 0, &setting.m_ambient);
        fn_80048238(renderer, 1, &setting.m_directional);
        fn_8004815C(renderer, 1, setting.m_angles.x, setting.m_angles.y);
        renderer->m_currentSetting = index;
    }
}

extern "C" void fn_800482B0(UnkRenderer_800289B0 *renderer, s8 index, RwRGBAReal *color) {
    renderer->m_table.m_settings[index].m_directional = *color;
}

extern "C" void fn_80048278(UnkRenderer_800289B0 *renderer, s8 index, float x, float y) {
    renderer->m_table.m_settings[index].m_angles.x = x;
    renderer->m_table.m_settings[index].m_angles.y = y;
}

extern "C" bool fn_80048238(UnkRenderer_800289B0 *renderer, s8 slot, RwRGBAReal *color) {
    UnkLight_80047D60 *light = renderer->m_lights[slot];
    if (light != 0) {
        return light->SetColor(*color);
    }
    return false;
}

extern "C" bool fn_8004815C(UnkRenderer_800289B0 *renderer, s8 slot, float x, float y) {
    UnkLight_80047D60 *light = renderer->m_lights[slot];
    if (light != 0) {
        return light->SetAngles(x, y);
    }
    return false;
}

extern "C" bool fn_80048060(UnkRenderer_800289B0 *renderer, s8 slot, RwV3d *position) {
    UnkLight_80047D60 *light = renderer->m_lights[slot];
    if (light != 0) {
        return light->SetPosition(*position);
    }
    return false;
}

/* Resets the light settings (from StageManager::SetUpBlocks). */
extern "C" void fn_80047F34(UnkRenderer_800289B0 *renderer) {
    for (s8 i = 0; i < 17; i++) {
        if (i == 16) {
            renderer->m_table.m_settings[i] = s_defaultSetting16;
        } else {
            renderer->m_table.m_settings[i] = s_defaultSetting;
        }
    }
}

extern "C" void fn_80047EC8(UnkRenderer_800289B0 *renderer, s8 slot, RpWorld *world) {
    UnkLight_80047D60 *light = renderer->m_lights[slot];
    if (light != 0) {
        light->AddToWorld(world);
    }
}

extern "C" void fn_80047E70(UnkRenderer_800289B0 *renderer, s8 slot) {
    UnkLight_80047D60 *light = renderer->m_lights[slot];
    if (light != 0) {
        light->RemoveFromWorld();
    }
}

/* Puts the lights into world, or takes them out of their worlds when it is 0. */
extern "C" void fn_80047DFC(UnkRenderer_800289B0 *renderer, RpWorld *world) {
    for (s8 i = 0; i < 8; i++) {
        if (world == 0) {
            fn_80047E70(renderer, i);
        } else {
            fn_80047EC8(renderer, i, world);
            fn_80047780(renderer);
        }
    }
}

extern "C" void fn_80047DD4(UnkRenderer_800289B0 *renderer, s8 index, s8 value) {
    if (value < 0 || value >= 17) {
        return;
    }
    renderer->m_field_0x457[index] = value;
}

UnkLight_80047D60::UnkLight_80047D60(s8 slot) : m_slot(slot) {
    m_type = -1;
    m_light = 0;
    if (m_slot == 0) {
        CreateOfType(0);
    } else if (m_slot == 1) {
        CreateOfType(1);
    }
    m_field_0x4 = 3;
}

UnkLight_80047D60::UnkLight_80047D60(s8 slot, UnkLightDesc_80047848 *desc) : m_slot(slot) {
    m_type = -1;
    m_light = 0;
    Create(desc);
    m_field_0x4 = 3;
}

UnkLight_80047D60::~UnkLight_80047D60() {
    if (m_light != 0) {
        Destroy();
    }
}

RpLight *UnkLight_80047D60::CreateOfType(s8 type) {
    if (type >= 5 || type < 0) {
        return 0;
    }
    if (m_light != 0) {
        Destroy();
    }
    m_light = fn_8045AD34(s_rwLightTypes[type]);
    if (m_light != 0) {
        if (type != 0) {
            RwFrame *frame = fn_80486D00();
            if (frame == 0) {
                Destroy();
                return 0;
            }
            fn_8048D7C0(m_light, frame);
        }
        m_type = type;
        return m_light;
    }
    return 0;
}

RpLight *UnkLight_80047D60::Create(UnkLightDesc_80047848 *desc) {
    if (m_light != 0) {
        Destroy();
    }
    m_light = fn_8045AD34(s_rwLightTypes[desc->m_type]);
    if (m_light != 0) {
        if (desc->m_type != 0) {
            RwFrame *frame = fn_80486D00();
            if (frame == 0) {
                Destroy();
                return 0;
            }
            fn_8048D7C0(m_light, frame);
        }
        m_type = desc->m_type;
        SetColor(desc->m_color);
        SetPosition(desc->m_position);
        SetAngles(desc->m_angleX, desc->m_angleY);
        SetRadius(desc->m_radius);
        SetConeAngle(desc->m_coneAngle);
        return m_light;
    }
    return 0;
}

bool UnkLight_80047D60::SetColor(RwRGBAReal &color) {
    if (m_light != 0) {
        RwRGBAReal *dst = &m_color;
        *dst = color;
        fn_8045A7C4(m_light, dst);
    } else {
        return false;
    }
    return true;
}

/* Applies the chosen light setting. */
extern "C" void fn_800477BC(UnkRenderer_800289B0 *renderer) {
    fn_800482E0(renderer, renderer->m_settingIndex);
}

/* Goes back to the default light setting. */
extern "C" void fn_80047780(UnkRenderer_800289B0 *renderer) {
    renderer->SetSettingIndex(renderer->m_defaultSetting);
    fn_800477BC(renderer);
}
