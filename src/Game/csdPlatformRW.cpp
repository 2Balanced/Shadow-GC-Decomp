/* csdPlatformRW.cpp (file name inferred from CPlatformRW, which the library's assert message
 * names): the RenderWare implementation of the Chao CSD library's platform layer (see CPlatform in
 * Game/CsdLib.h). It draws the casts as immediate-mode 2D primitives of up to four vertices and
 * saves and restores the render states it changes. The concrete platform the game installs derives
 * from it. Names other than CPlatformRW and DrawPrimitive2D are inferred.
 *
 * Code generation follows Game/csdLoader.cpp, except that the library's RenderWare units are built
 * with deferred inlining (cflags_csd_rw; see Game/csdTexListRW.cpp): their functions come out in
 * reverse definition order. */
#include <dolphin/types.h>

#include "Game/CsdPlatformRW.h"

#pragma pool_data off

namespace Chao {
namespace CSD {

/* Sets the blend mode: 0 adds, 1 blends by alpha. */
void CPlatformRW::SetBlend(s32 blend, const void *state, s32 count) {
    s32 src;
    s32 dst;
    switch (blend) {
    case 0:
        src = 5;
        dst = 2;
        break;
    case 1:
        src = 5;
        dst = 6;
        break;
    }
    lbl_805F15B4->fpRenderStateSet(rwRENDERSTATESRCBLEND, (void *)src);
    lbl_805F15B4->fpRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)dst);
}

void CPlatformRW::SetAlphaTest(s32 alpha, const void *state, s32 count) {
    lbl_805F15B4->fpRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)(alpha == 1 ? 1 : 0));
}

/* Draws a textured primitive (vertices are SVertexT). */
void CPlatformRW::DrawTextured(const void *vertices, s32 count, const void *state, s32 count2) {
    if (count > 4) {
        fn_80361DC4("libcsd_rw ASSERT: CPlatformRW::DrawPrimitive2D VERTEX_BUFFER_SIZE over\n");
    } else {
        const SVertexT *src = (const SVertexT *)vertices;
        for (int i = 0; i < count; i++) {
            u32 color;
            RwIm2DVertex *dst;
            const SVertexT *s;
            dst = &m_vertices[i];
            s = &src[i];
            color = s->m_color;
            dst->x = s->m_pos.m_x;
            dst->y = s->m_pos.m_y;
            dst->emissiveColor.red = color >> 24;
            dst->emissiveColor.green = color >> 16;
            dst->emissiveColor.blue = color >> 8;
            dst->emissiveColor.alpha = color;
            dst->u = s->m_uv.m_x;
            dst->v = s->m_uv.m_y;
        }
        lbl_805F15B4->fpIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, m_vertices, count);
    }
}

void CPlatformRW::SetAlphaTestOff(s32 alpha, const void *state, s32 count) {
    lbl_805F15B4->fpRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)(alpha == 1 ? 1 : 0));
}

/* Draws an untextured primitive (vertices are SVertex). */
void CPlatformRW::Draw(const void *vertices, s32 count, const void *state, s32 count2) {
    lbl_805F15B4->fpRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)0);
    if (count <= 4) {
        const SVertex *src = (const SVertex *)vertices;
        s32 n = count;
        for (int i = 0; count > 0; count--, i++) {
            u32 color;
            RwIm2DVertex *dst;
            dst = &m_vertices[i];
            color = src[i].m_color;
            dst->x = src[i].m_pos.m_x;
            dst->y = src[i].m_pos.m_y;
            dst->emissiveColor.red = color >> 24;
            dst->emissiveColor.green = color >> 16;
            dst->emissiveColor.blue = color >> 8;
            dst->emissiveColor.alpha = color;
        }
        lbl_805F15B4->fpIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, m_vertices, n);
    }
}

/* Saves the render states and sets the ones the library draws with. */
void CPlatformRW::Begin() {
    lbl_805F15B4->fpRenderStateGet(rwRENDERSTATESRCBLEND, &m_srcBlend);
    lbl_805F15B4->fpRenderStateGet(rwRENDERSTATEDESTBLEND, &m_destBlend);
    lbl_805F15B4->fpRenderStateGet(rwRENDERSTATEVERTEXALPHAENABLE, &m_vertexAlpha);
    lbl_805F15B4->fpRenderStateGet(rwRENDERSTATEZWRITEENABLE, &m_zWrite);
    lbl_805F15B4->fpRenderStateGet(rwRENDERSTATEZTESTENABLE, &m_zTest);
    lbl_805F15B4->fpRenderStateGet(rwRENDERSTATECULLMODE, &m_cullMode);
    m_texDictionary = fn_8048D0D8();
    lbl_805F15B4->fpRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)0);
    lbl_805F15B4->fpRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)0);
    lbl_805F15B4->fpRenderStateSet(rwRENDERSTATECULLMODE, (void *)1);
    for (int i = 0; i < 4; i++) {
    }
}

/* Restores the render states Begin saved. */
void CPlatformRW::End() {
    lbl_805F15B4->fpRenderStateSet(rwRENDERSTATESRCBLEND, (void *)m_srcBlend);
    lbl_805F15B4->fpRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)m_destBlend);
    lbl_805F15B4->fpRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)m_vertexAlpha);
    lbl_805F15B4->fpRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)m_zWrite);
    lbl_805F15B4->fpRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)m_zTest);
    lbl_805F15B4->fpRenderStateSet(rwRENDERSTATECULLMODE, (void *)m_cullMode);
    fn_8048D0C4(m_texDictionary);
}

void CPlatformRW::Print(const char *message) {
}

void CPlatformRW::BeginColored(const void *state, s32 count) {
}

void CPlatformRW::BeginTextured(const void *state, s32 count) {
}

CPlatformRW::~CPlatformRW() {
}

} // namespace CSD
} // namespace Chao
