#ifndef GAME_CSDPLATFORMRW_H
#define GAME_CSDPLATFORMRW_H

/* The RenderWare platform layer of the Chao CSD library (Game/csdPlatformRW.cpp). */

#include <dolphin/types.h>

#include "Game/CsdLib.h"
#include "Game/CsdRW.h"

namespace Chao {
namespace CSD {

/* The RenderWare platform (IsSwapped is left to the class that derives from it). */
class CPlatformRW : public CPlatform {
  public:
    CPlatformRW() {}
    virtual ~CPlatformRW();
    virtual void Print(const char *message);
    virtual void SetBlend(s32 blend, const void *state, s32 count);
    virtual void SetAlphaTest(s32 alpha, const void *state, s32 count);
    virtual void DrawTextured(const void *vertices, s32 count, const void *state, s32 count2);
    virtual void BeginTextured(const void *state, s32 count);
    virtual void SetAlphaTestOff(s32 alpha, const void *state, s32 count);
    virtual void Draw(const void *vertices, s32 count, const void *state, s32 count2);
    virtual void BeginColored(const void *state, s32 count);
    virtual void Begin();
    virtual void End();

    /* 0x08 */ RwIm2DVertex m_vertices[4]; /* the primitive being drawn */
    /* The render states and the current texture dictionary Begin saves and End restores. */
    /* 0x68 */ u32 m_srcBlend;
    /* 0x6C */ u32 m_destBlend;
    /* 0x70 */ u32 m_vertexAlpha;
    /* 0x74 */ u32 m_zWrite;
    /* 0x78 */ u32 m_zTest;
    /* 0x7C */ u32 m_cullMode;
    /* 0x80 */ RwTexDictionary *m_texDictionary;
};

} // namespace CSD
} // namespace Chao

#endif
