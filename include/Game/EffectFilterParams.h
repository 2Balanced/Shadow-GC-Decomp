#ifndef GAME_EFFECTFILTERPARAMS_H
#define GAME_EFFECTFILTERPARAMS_H

/* The settings of the RenderWare effect manager's screen filters (Game/EffectFilterRW3.h), which
 * Effect::RW3::EffectGlobalParamRW3 (Game/EffectGlobalParamRW3.cpp) keeps a start and an end copy
 * of. Kept apart from the filters so that Effect/System/EffectGlobalParam.h can hold them. None of
 * these structures has RTTI, so all names are inferred. */
#include <dolphin/types.h>

/* One direction of the screen shimmer's wave. */
struct UnkScreenShimmerAxis_804370D4 {
    UnkScreenShimmerAxis_804370D4() {}
    UnkScreenShimmerAxis_804370D4(const UnkScreenShimmerAxis_804370D4 &other)
        : m_count(other.m_count), m_amplitude(other.m_amplitude), m_cycle(other.m_cycle),
          m_waveLength(other.m_waveLength), m_start(other.m_start), m_end(other.m_end) {}

    /* 0x00 */ s32 m_count;      /* grid cells along the direction (at least 1) */
    /* 0x04 */ float m_amplitude;
    /* 0x08 */ float m_cycle;
    /* 0x0C */ float m_waveLength;
    /* 0x10 */ float m_start;    /* the part of the screen the grid covers (0 to 1) */
    /* 0x14 */ float m_end;
};

/* The screen shimmer's settings (0x804370D4 is the constructor, 0x80436A4C the copy). */
struct UnkScreenShimmerParams_804370D4 {
    UnkScreenShimmerParams_804370D4(bool enabled, u8 alpha, s32 countX, float amplitudeX,
                                    float cycleX, float waveLengthX, float startX, float endX,
                                    s32 countY, float amplitudeY, float cycleY,
                                    float waveLengthY, float startY, float endY);

    /* 0x00 */ bool m_enabled;
    /* 0x01 */ u8 m_alpha;
    /* 0x02 */ u8 field_0x2[3];
    /* 0x08 */ UnkScreenShimmerAxis_804370D4 m_x;
    /* 0x20 */ UnkScreenShimmerAxis_804370D4 m_y;
};

/* The fog's settings (layout from Game/PlayerEffect.cpp, which constructs them). */
struct UnkFogColor_804388A4 {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
};

struct UnkFog_804388A4 {
    /* Defined inline in Game/PlayerEffect.cpp, whose copy (0x800A4664) the effect units call. */
    UnkFog_804388A4(u8 on, u8 enabled, u8 mode);
    UnkFog_804388A4(const UnkFog_804388A4 &other)
        : m_on(other.m_on), m_enabled(other.m_enabled), m_mode(other.m_mode),
          m_color(other.m_color), m_color2(other.m_color2) {}

    /* 0x0 */ u8 m_on;
    /* 0x1 */ u8 m_enabled;
    /* 0x2 */ u8 m_mode;
    /* 0x3 */ UnkFogColor_804388A4 m_color;
    /* 0x7 */ UnkFogColor_804388A4 m_color2;
};

/* The colour filter's settings (the manager's filter at 0x3C; 0x80430844 returns them and
 * 0x804308C0 sets them; layout from their copies). */
struct UnkScreenColor_80430844 {
    UnkScreenColor_80430844()
        : field_0x0(0), field_0x1(0), field_0x2(0), field_0x3(0), field_0x4(0), field_0x5(0),
          field_0x6(0), field_0x8(0.0f), field_0xC(0.0f), field_0x10(0), field_0x14(0),
          field_0x18(0), field_0x1C(0) {}

    /* 0x00 */ bool field_0x0; /* switches */
    /* 0x01 */ bool field_0x1;
    /* 0x02 */ bool field_0x2;
    /* 0x03 */ bool field_0x3;
    /* 0x04 */ bool field_0x4;
    /* 0x05 */ u8 field_0x5;
    /* 0x06 */ u8 field_0x6;
    /* 0x08 */ float field_0x8;
    /* 0x0C */ float field_0xC;
    /* 0x10 */ s32 field_0x10;
    /* 0x14 */ s32 field_0x14;
    /* 0x18 */ s32 field_0x18;
    /* 0x1C */ s32 field_0x1C;
};

/* The screen blur's settings (Game/EffectScreenBlurRW3.cpp, which defines them for itself; its
 * last eight bytes are two floats here, as EffectGlobalParamRW3 blends them). */
struct UnkScreenBlurExtra_8042D974 {
    float field_0x0;
    float field_0x4;
};

struct UnkScreenBlurParam_8042D974 {
    UnkScreenBlurParam_8042D974() : enabled(0), alpha(0), scale(1.0f) {}

    /* 0x0 */ u8 enabled;
    /* 0x1 */ u8 alpha;
    /* 0x4 */ float scale;
    /* 0x8 */ UnkScreenBlurExtra_8042D974 extra;
};

#endif
