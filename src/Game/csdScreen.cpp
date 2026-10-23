/* csdScreen.cpp (file name inferred; the library's records name no file here): the screen the Chao
 * CSD projects are laid out for and the one they are drawn to, and the conversions between them.
 * All names are inferred. Code generation follows Game/csdLoader.cpp. */
#include <dolphin/types.h>

#include "Game/CsdLib.h"

#pragma pool_data off

namespace Chao {
namespace CSD {

/* A screen's size, its inverse, the offset of its origin and its aspect ratio. */
struct SScreen {
    SScreen() {
        m_size[0] = 0.0f;
        m_size[1] = 0.0f;
        m_invSize[0] = 0.0f;
        m_invSize[1] = 0.0f;
        m_offset[0] = 0.0f;
        m_offset[1] = 0.0f;
    }

    /* 0x00 */ f32 m_size[2];
    /* 0x08 */ f32 m_invSize[2];
    /* 0x10 */ f32 m_offset[2];
    /* 0x18 */ f32 m_aspect;
};

/* The screen the projects are laid out for, and the one drawn to. */
#pragma pool_data on
static SScreen s_layout;
static SScreen s_screen;
#pragma pool_data off

float GetScreenAspect() {
    return s_screen.m_aspect;
}

void ScaleToScreen(float *x, float *y) {
    *x *= s_layout.m_size[0];
    *y *= s_layout.m_size[1];
}

void GetLayoutOffset(float *x, float *y) {
    *x = s_layout.m_offset[0];
    *y = s_layout.m_offset[1];
}

#pragma dont_inline on
void GetScreenOffset(float *x, float *y) {
    GetLayoutOffset(x, y);
    ScaleToScreen(x, y);
}
#pragma dont_inline reset

void GetScreenBase(float *x, float *y) {
    *x = s_screen.m_size[0];
    *y = s_screen.m_size[1];
}

void GetScreenSize(float *x, float *y) {
    *x = s_layout.m_size[0];
    *y = s_layout.m_size[1];
}

void GetCharSize(float *x, float *y) {
    *x *= s_screen.m_invSize[0];
    *y *= s_screen.m_invSize[1];
    *x -= s_screen.m_offset[0];
    *y -= s_screen.m_offset[1];
}

void ToLayout(float *x, float *y) {
    *x *= s_layout.m_invSize[0];
    *y *= s_layout.m_invSize[1];
    *x -= s_layout.m_offset[0];
    *y -= s_layout.m_offset[1];
}

void ScaleCorner(float *x, float *y) {
    *x = s_screen.m_size[0] * (*x + s_screen.m_offset[0]);
    *y = s_screen.m_size[1] * (*y + s_screen.m_offset[1]);
}

void FromLayout(float *x, float *y) {
    *x = s_layout.m_size[0] * (*x + s_layout.m_offset[0]);
    *y = s_layout.m_size[1] * (*y + s_layout.m_offset[1]);
}

void SetScreen(float w, float h) {
    s_screen.m_size[0] = w;
    s_screen.m_size[1] = h;
    s_screen.m_invSize[0] = 1.0f / w;
    s_screen.m_invSize[1] = 1.0f / h;
    s_screen.m_aspect = s_screen.m_size[0] / s_screen.m_size[1];
}

void SetLayout(float w, float h) {
    s_layout.m_size[0] = w;
    s_layout.m_size[1] = h;
    s_layout.m_invSize[0] = 1.0f / w;
    s_layout.m_invSize[1] = 1.0f / h;
    s_layout.m_aspect = s_layout.m_size[0] / s_layout.m_size[1];
}

#pragma dont_inline on
void SetScreenSize(float w, float h) {
    SetLayout(w, h);
    SetScreen(w, h);
}
#pragma dont_inline reset

} // namespace CSD
} // namespace Chao
