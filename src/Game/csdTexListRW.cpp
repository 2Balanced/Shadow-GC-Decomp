/* csdTexListRW.cpp (file name from the source file name the library passes to its allocator here):
 * the RenderWare texture list of the Chao CSD library, which reads a texture dictionary from a
 * project's texture list chunk and sets its rasters as the render state. Names other than the file
 * name are inferred.
 *
 * Code generation: the library's flags with deferred inlining (cflags_csd_rw), so functions come out
 * in reverse definition order and CRasterArray's constructor and destructor, defined first, are
 * inlined into Load and the destructor (the derived texture list of the game's platform unit calls
 * them out of line). CSD_RCPTR_DEALLOC_CTOR declares RCPtr's constructor with a deallocation tag
 * (see Game/CsdRCPtr.h). */
#define CSD_RCPTR_DEALLOC_CTOR

#include <dolphin/types.h>

#include "Game/CsdTexListRW.h"

#pragma pool_data off

namespace Chao {
namespace CSD {

/* The tag Create passes to RCPtr, an object of its own in .bss (one byte; read as an RCDeleteTag,
 * whose constructor is not trivial and would need a static initializer here). */
static u8 s_deleteTag;

RwTexture *CRasterArray::CountTexture(RwTexture *texture, void *data) {
    ++*(s32 *)data;
    return texture;
}

RwTexture *CRasterArray::CollectRaster(RwTexture *texture, void *data) {
    RwRaster ***cursor = (RwRaster ***)data;
    **cursor = texture->raster;
    (*cursor)++;
    return texture;
}

CRasterArray::CRasterArray(RwTexDictionary *dict) : m_rasters(0), m_count(0) {
    if (dict == 0) {
        return;
    }
    fn_8048CBE0(dict, CountTexture, &m_count);
    m_rasters = (RwRaster **)fn_80360564(m_count * 4, "csdTexListRW.cpp", 0x46);
    if (m_rasters == 0) {
        return;
    }
    RwRaster **rasters = m_rasters;
    fn_8048CBE0(dict, CollectRaster, &m_rasters);
    m_rasters = rasters;
}


CRasterArray::~CRasterArray() {
    if (m_rasters != 0) {
        fn_80360528(m_rasters);
    }
}

CTexListRW::~CTexListRW() {
    if (m_rasters != 0) {
        delete m_rasters;
    }
    if (m_dictionary != 0) {
        fn_8048CB38(m_dictionary);
    }
}

s32 CTexListRW::Load(const void *data, u32 size) {
    if (size != 0) {
        RwMemory memory;
        memory.length = size;
        memory.start = data;
        RwStream *stream = fn_80481368(rwSTREAMMEMORY, rwSTREAMREAD, &memory);
        if (stream != 0) {
            if (fn_8047CF50(stream, rwID_TEXDICTIONARY, 0, 0)) {
                m_dictionary = fn_80483FBC(stream);
            }
            fn_8048124C(stream, &memory);
        }
        if (stream == 0 || m_dictionary == 0) {
            return false;
        }
    }
    m_rasters = new CRasterArray(m_dictionary);
    if (m_rasters == 0) {
        return false;
    }
    return true;
}


void CTexListRW::SetTexture(s32 index, const void *state, s32 count) {
    if (index < 0) {
        lbl_805F15B4->fpRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)0);
    } else {
        fn_8048D0C4(m_dictionary);
        if (m_rasters != 0) {
            RwRaster *raster = m_rasters->Get(index);
            if (raster != 0) {
                lbl_805F15B4->fpRenderStateSet(rwRENDERSTATETEXTURERASTER, raster);
            }
        }
    }
}

void CTexListRW::SetWrap(s32 s, s32 t, const void *state, s32 count) {
    s32 modeS;
    s32 modeT;
    switch (s) {
    case 0:
        modeS = 1;
        break;
    case 1:
        modeS = 3;
        break;
    }
    switch (t) {
    case 0:
        modeT = 1;
        break;
    case 1:
        modeT = 3;
        break;
    }
    lbl_805F15B4->fpRenderStateSet(rwRENDERSTATETEXTUREADDRESSU, (void *)modeS);
    lbl_805F15B4->fpRenderStateSet(rwRENDERSTATETEXTUREADDRESSV, (void *)modeT);
}

void CTexListRW::SetFilter(s32 index, s32 filter, const void *state, s32 count) {
    s32 mode;
    switch (filter) {
    case 0:
        mode = 1;
        break;
    case 1:
        mode = 2;
        break;
    }
    lbl_805F15B4->fpRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void *)mode);
}

RCPtr<CTexList> CTexListRW::Create() {
    return RCPtr<CTexList>(new CTexListRW, *(RCDeleteTag *)&s_deleteTag);
}

u32 CTexListRW::GetType() {
    return '.RWT';
}

} // namespace CSD
} // namespace Chao
