#ifndef GAME_UNKRENDERER_800289B0_H
#define GAME_UNKRENDERER_800289B0_H

#include <dolphin/types.h>
#include "size_assert.h"
#include <string.h>

/* RenderWare types (the engine is RenderWare 3.7; only the parts used are declared). */
struct RpWorld;
struct RwFrame;
struct RpLight {
    /* 0x0 */ u8 field_0x0[0x4];
    /* 0x4 */ RwFrame *parent;
};
struct RwV3d {
    float x;
    float y;
    float z;
};
struct RwRGBAReal {
    float red;
    float green;
    float blue;
    float alpha;
};
struct RwBBox {
    RwV3d sup;
    RwV3d inf;
};

/* What a light is created from (struct name unknown). */
struct UnkLightDesc_80047848 {
    /* 0x00 */ s8 m_type;
    /* 0x04 */ RwRGBAReal m_color;
    /* 0x14 */ float m_angleX;
    /* 0x18 */ float m_angleY;
    /* 0x1C */ RwV3d m_position;
    /* 0x28 */ float m_radius;
    /* 0x2C */ float m_coneAngle;
};
SIZE_ASSERT(UnkLightDesc_80047848, 0x30);

class UnkLight_80047D60;

/* The directional light's angles in degrees, about the x and y axes. */
struct UnkLightAngles_80047F34 {
    int x;
    int y;
};

/* A light setting: the ambient and directional colours and the directional light's angles in
 * degrees (struct name unknown). */
struct UnkLightSetting_80047F34 {
    /* 0x00 */ RwRGBAReal m_ambient;
    /* 0x10 */ RwRGBAReal m_directional;
    /* 0x20 */ RwV3d m_field_0x20;
    /* 0x2C */ UnkLightAngles_80047F34 m_angles;
};
SIZE_ASSERT(UnkLightSetting_80047F34, 0x34);

/* The settings a stage's light file holds (struct name unknown). */
struct UnkLightSettingTable_80048654 {
    UnkLightSettingTable_80048654 &operator=(const UnkLightSettingTable_80048654 &other) {
        if (&other != this) {
            memcpy(this, &other, sizeof(UnkLightSettingTable_80048654));
        }
        return *this;
    }

    UnkLightSetting_80047F34 m_settings[16];
};

/* The renderer behind fn_800289B0 (class name unknown; Game/UnkRenderer_800289B0.cpp; constructor
 * 0x800487E4, destructor 0x80048790). Game/EffectSystemUSA.cpp creates it; Game/CameraRender.cpp,
 * Game/LightColli.cpp and the ring units declare it locally (LightColli.cpp as
 * UnkLightManager_800289B0). */
class UnkRenderer_800289B0 {
  public:
    UnkRenderer_800289B0();
    ~UnkRenderer_800289B0();

    void Reset() {
        for (u8 i = 0; i < 2; i++) {
            m_field_0x457[i] = 0;
            m_field_0x459[i] = 0;
        }
    }

    UnkLight_80047D60 *GetLight(s8 slot) {
        return m_lights[slot];
    }

    UnkLightSetting_80047F34 *GetSetting(s8 index) {
        return &m_table.m_settings[index];
    }

    UnkLightDesc_80047848 *GetDesc(s8 slot) {
        return &m_descs[slot];
    }

    void SetSettingIndex(s8 index) {
        if (index >= 0 && index < 17) {
            m_settingIndex = index;
        }
    }

    /* 0x000 */ UnkLight_80047D60 *m_lights[8];
    /* 0x020 */ UnkLightSettingTable_80048654 m_table;
    /* 0x360 */ UnkLightSetting_80047F34 m_setting16; /* the seventeenth setting */
    /* 0x394 */ UnkLightDesc_80047848 m_descs[4];
    /* 0x454 */ s8 m_currentSetting;
    /* 0x455 */ s8 m_defaultSetting;
    /* 0x456 */ s8 m_settingIndex;
    /* 0x457 */ s8 m_field_0x457[2];
    /* 0x459 */ s8 m_field_0x459[2];
    /* 0x45C */ RpWorld *m_world;
};
SIZE_ASSERT(UnkRenderer_800289B0, 0x460);

extern "C" {
UnkRenderer_800289B0 *fn_800289B0(void);
/* Sets a slot's light: its colour, its angles and its position (true if the light takes it). */
bool fn_80048238(UnkRenderer_800289B0 *renderer, s8 slot, RwRGBAReal *color);
bool fn_8004815C(UnkRenderer_800289B0 *renderer, s8 slot, float x, float y);
bool fn_80048060(UnkRenderer_800289B0 *renderer, s8 slot, RwV3d *position);
/* Sets a light setting's directional colour and angles. */
void fn_800482B0(UnkRenderer_800289B0 *renderer, s8 index, RwRGBAReal *color);
void fn_80048278(UnkRenderer_800289B0 *renderer, s8 index, float x, float y);
/* Adds a point or spot light; returns its slot, or -1. */
s8 fn_80048484(UnkRenderer_800289B0 *renderer, UnkLightDesc_80047848 *desc);
}

#endif
