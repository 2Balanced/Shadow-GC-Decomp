/* csdTexList.cpp (file name from the source file name the library passes to its allocator here):
 * the Chao CSD library's abstract texture list, loading a file into one, and the registry of the
 * texture lists each type is made from (a prototype per type). Names other than CTexList (from the
 * RTTI of RCPtr<CTexList>::RCObject<CTexList> in game code) and the file name are inferred. Code
 * generation follows Game/csdLoader.cpp.
 *
 * The registry is a std::map, whose instances the library emits after the code that uses them.
 * The library is built without deferred inlining, so the tree is the variant <map> gives where
 * MSL_TREE_SELF_INLINE is defined, with erase (MSL_TREE_ERASE). insert_unique's result flags are
 * the compiler's own constants here: retail has them in .data after the virtual table. */
#define MSL_PAIR_CONVERTING_CONSTRUCTOR
#define MSL_TREE_ERASE
#define MSL_TREE_SELF_INLINE
#include <map>

#include <dolphin/types.h>

#include "Game/CsdLib.h"

#pragma pool_data off

extern "C" {
/* MSL strrchr and strncpy (the symbols are still placeholders). */
char *fn_803AC864(const char *s, int c);
char *fn_803ACABC(char *dst, const char *src, u32 size);
}

namespace Chao {
namespace CSD {

/* Game/csdFileSystem.cpp */
void GetDirectory(char *buffer, u32 size);
void SetDirectory(const char *path);
u32 LoadFile(const char *name);
u32 LoadFile(void *out, const char *name);

/* The registered texture lists by type. */
#pragma pool_data on
static std::map<u32, CTexList *> *s_prototypes;
#pragma pool_data off

/* Loads the textures named by a texture list chunk into texList, relative to directory (if any). */
s32 LoadTexList(CTexList *texList, const void *data, u32 size, const char *directory) {
    char saved[0x100];
    GetDirectory(saved, 0x100);
    if (directory != 0) {
        SetDirectory(directory);
    }
    s32 result = texList->Load(data, size);
    SetDirectory(saved);
    return result;
}

} // namespace CSD
} // namespace Chao

using namespace Chao::CSD;

/* Loads the textures of a texture list file into texList, relative to the file's directory. */
extern "C" s32 fn_80361FB0(CTexList *texList, const char *path) {
    char saved[0x100];
    char directory[0x100];
    u32 size = LoadFile(path);
    void *data = fn_80360564(size, "csdTexList.cpp", 0x89);
    LoadFile(data, path);
    GetDirectory(saved, 0x100);
    const char *separator = fn_803AC864(path, '/');
    if (separator == 0) {
        separator = fn_803AC864(path, '\\');
    }
    if (separator != 0) {
        u32 length = separator - path;
        fn_803ACABC(directory, path, length);
        directory[length] = 0;
        SetDirectory(directory);
    }
    s32 result = texList->Load(data, size);
    fn_80360528(data);
    SetDirectory(saved);
    return result;
}

/* Makes a texture list of the given type (an empty pointer if none is registered for it). */
extern "C" RCPtr<CTexList> fn_80362094(u32 type) {
    std::map<u32, CTexList *>::iterator it = s_prototypes->find(type);
    if (it == s_prototypes->end()) {
        return RCPtr<CTexList>(0);
    }
    return (*it).second->Create();
}

namespace Chao {
namespace CSD {

/* (A declaration here ends fn_80362094 before the destructor: the compiler holds a function that
 * returns a class until the next declaration, and retail has the destructor after it.) */
void RegisterTexList(CTexList *prototype);

CTexList::~CTexList() {
}

/* Registers prototype for its type, in place of any texture list registered for it before. */
void RegisterTexList(CTexList *prototype) {
    std::map<u32, CTexList *>::iterator it = s_prototypes->find(prototype->GetType());
    if (it != s_prototypes->end()) {
        s_prototypes->erase(it);
    }
    s_prototypes->insert(std::pair<u32, CTexList *>(prototype->GetType(), prototype));
}

void ShutdownTexList() {
    if (s_prototypes != 0) {
        delete s_prototypes;
        s_prototypes = 0;
    }
}

void InitTexList() {
    s_prototypes = new std::map<u32, CTexList *>;
}

} // namespace CSD
} // namespace Chao

#include <map_impl>
