/* csdRW.cpp (file name inferred): the RenderWare build of the Chao CSD library as the game installs
 * it. InitializeRW, which the game calls once (0x800424DC), installs a GameCube platform, an allocator
 * on RenderWare's memory functions, the RenderWare file system and the two RenderWare texture list
 * types, starts the library and sets the chunk identifiers of the project files. All names are
 * inferred, except CMemoryAlloc (from game RTTI).
 *
 * Code generation follows Game/csdTexListRW.cpp (deferred inlining: functions come out in reverse
 * definition order), with data pooling on, as InitializeRW addresses its static objects from one
 * base. MSL_STRING_CTOR_EXTERN: the unit calls the game's string constructor and has no copy of its
 * own. */
#define CSD_RCPTR_DEALLOC_CTOR
#define MSL_STRING_CTOR_EXTERN
#include <string>

#include <dolphin/types.h>

#include "Game/CsdFileSystem.h"
#include "Game/CsdPlatformRW.h"
#include "Game/CsdTexListRW.h"

extern "C" {
/* Installs the library's allocator (Game/csdBase.cpp). */
void fn_803605BC(Chao::CSD::CMemoryAlloc *allocator);
}

namespace Chao {
namespace CSD {

/* Game/csdPlatform.cpp, Game/csdFileSystem.cpp, Game/csdScreen.cpp, Game/csdTexList.cpp */
void SetPlatform(CPlatform *platform);
CFileSystem *SetFileSystem(CFileSystem *fileSystem);
void SetScreenSize(float w, float h);
void RegisterTexList(CTexList *prototype);

/* See Game/CsdFileSystem.h. */
const char g_noDirectory[8] = "";

/* The platform of the GameCube build: big-endian, so the project files need no swapping. */
class CPlatformRWGC : public CPlatformRW {
  public:
    CPlatformRWGC() {}
    virtual ~CPlatformRWGC();
    virtual s32 IsSwapped();
};

/* The library's allocator on RenderWare's memory functions. */
class CMemoryAllocRW : public CMemoryAlloc {
  public:
    CMemoryAllocRW() {}
    virtual void *Allocate(u32 size);
    virtual void Deallocate(void *p);
};

/* The texture list of type '.RW2': a texture dictionary the game has already loaded, which Load
 * takes in place of a texture list chunk (and does not destroy). */
class CTexListRW2 : public CTexListRW {
  public:
    CTexListRW2() {}
    virtual ~CTexListRW2();
    virtual u32 GetType();
    virtual RCPtr<CTexList> Create();
    virtual s32 Load(const void *data, u32 size);

    /* The tag Create passes to RCPtr: a byte of .bss after InitializeRW's static objects (a static
     * of an inline function, so that it comes out with Create, which deferred inlining generates
     * after InitializeRW; read as an RCDeleteTag, see Game/csdTexListRW.cpp). */
    static RCDeleteTag &DeleteTag() {
        static u8 s_tag;
        return *(RCDeleteTag *)&s_tag;
    }
};

CTexListRW2::~CTexListRW2() {
    m_dictionary = 0;
    if (m_rasters != 0) {
        delete m_rasters;
        m_rasters = 0;
    }
}

s32 CTexListRW2::Load(const void *data, u32 size) {
    if (data == 0) {
        return false;
    }
    m_dictionary = (RwTexDictionary *)data;
    m_rasters = new CRasterArray(m_dictionary);
    if (m_rasters == 0) {
        return false;
    }
    return true;
}

RCPtr<CTexList> CTexListRW2::Create() {
    return RCPtr<CTexList>(new CTexListRW2, DeleteTag());
}

u32 CTexListRW2::GetType() {
    return '.RW2';
}

void CMemoryAllocRW::Deallocate(void *p) {
    lbl_805F15B4->fpFree(p);
}

void *CMemoryAllocRW::Allocate(u32 size) {
    return lbl_805F15B4->fpMalloc(size, 0x30000);
}

/* Sets the library up for the game (width and height are the screen size the projects are laid
 * out for). */
void InitializeRW(float width, float height) {
    /* Eight bytes of .bss that nothing reads come first in retail; their purpose is unknown. */
    static u32 s_unknown[2];
    static CPlatformRWGC s_platform;
    SetPlatform(&s_platform);
    static CMemoryAllocRW s_allocator;
    fn_803605BC(&s_allocator);
    Initialize();
    static CFileSystemRW s_fileSystem;
    SetFileSystem(&s_fileSystem);
    static CTexListRW s_texList;
    static CTexListRW2 s_texList2;
    RegisterTexList(&s_texList);
    RegisterTexList(&s_texList2);
    SetScreenSize(width, height);
    g_chunkInfo = 'NGIF';
    g_chunkTexList = 'NGTL';
    g_chunkObject = 'NGOB';
    g_chunkMotion = 'NGMO';
    g_chunkMM = 'NGMM';
    g_chunkMC = 'NGMC';
    g_chunkML = 'NGML';
    g_chunkMA = 'NGMA';
    g_chunkLI = 'NGLI';
    g_chunkCA = 'NGCA';
    g_chunkMT = 'NGMT';
    g_chunkNN = 'NGNN';
    g_chunkOffsets = 'NOF0';
    g_chunkEnd = 'NEND';
}

s32 CPlatformRWGC::IsSwapped() {
    return 1;
}

CPlatformRWGC::~CPlatformRWGC() {
}

} // namespace CSD
} // namespace Chao
