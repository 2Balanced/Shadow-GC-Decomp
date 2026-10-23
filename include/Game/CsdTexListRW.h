#ifndef GAME_CSDTEXLISTRW_H
#define GAME_CSDTEXLISTRW_H

/* The RenderWare texture list of the Chao CSD library (Game/csdTexListRW.cpp). */

#include <dolphin/types.h>

#include "Game/CsdLib.h"
#include "Game/CsdRW.h"

namespace Chao {
namespace CSD {

/* The rasters of a texture dictionary's textures, by index (names inferred). */
class CRasterArray {
  public:
    CRasterArray(RwTexDictionary *dict);
    ~CRasterArray();

    RwRaster *Get(s32 index) {
        if (index < m_count && m_count != 0) {
            return m_rasters[index];
        }
        return 0;
    }

    /* RwTexDictionaryForAllTextures callbacks: count the textures, and collect their rasters. */
    static RwTexture *CountTexture(RwTexture *texture, void *data);
    static RwTexture *CollectRaster(RwTexture *texture, void *data);

    /* 0x0 */ RwRaster **m_rasters;
    /* 0x4 */ s32 m_count;
};

/* The texture list of the RenderWare build: a texture dictionary read from a texture list chunk
 * (type '.RWT'; names inferred). */
class CTexListRW : public CTexList {
  public:
    CTexListRW() : m_dictionary(0), m_rasters(0) {}
    virtual ~CTexListRW();
    virtual u32 GetType();
    virtual RCPtr<CTexList> Create();
    virtual s32 Load(const void *data, u32 size);
    virtual void SetTexture(s32 index, const void *state, s32 count);
    virtual void SetFilter(s32 index, s32 filter, const void *state, s32 count);
    virtual void SetWrap(s32 s, s32 t, const void *state, s32 count);

    /* 0x8 */ RwTexDictionary *m_dictionary;
    /* 0xC */ CRasterArray *m_rasters;
};

} // namespace CSD
} // namespace Chao

#endif
