/* TCsdSystem (class name and bases from RTTI): the game's setup of the Chao CSD library. It is the
 * library's memory allocator (Chao::CSD::CMemoryAlloc, on the game heap) and a singleton
 * (Sonicteam::System::Singleton with CreateStatic; the accessor, fn_8003F8D4, and the instance are
 * in the unit before this one). Its constructor starts the library's RenderWare platform at
 * 640 x 480 and installs it as the allocator; it also makes the empty scene, node, texture list and
 * scene list handles the game hands out where it has none. The unit also loads a project's
 * textures and builds the file names of projects. Names other than the class's are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined first);
 * the library's template members the constructor needs follow, and the static initializer's last. */
#define CSD_OBJECT_LAYER
#define CSD_PROJECT
#define CSD_SCENE_LIST_HANDLE
#define MSL_MULTIMAP
#include <stdio.h>

#include "Game/CsdProject.h"
#include "Game/TBGRead.h"

namespace Sonicteam {
namespace System {

/* The singleton holder (class names from RTTI; as Game/EffectManagerPJS.h declares them). */
template <class T> class CreateStatic {};
template <class T, class Creator> class Singleton {};

} // namespace System
} // namespace Sonicteam

using Chao::CSD::CMemoryAlloc;
using Chao::CSD::CNode;
using Chao::CSD::CProject;
using Chao::CSD::CScene;
using Chao::CSD::CTexList;
using Chao::CSD::RCPtr;

namespace Chao {
namespace CSD {
void InitializeRW(float width, float height);
void Terminate();
} // namespace CSD
} // namespace Chao

extern "C" {
/* Game heap: free and allocate. */
void fn_80049B34(void *block);
void *fn_80049B90(u32 size, int heap);
/* Installs the library's allocator (Game/csdRW.cpp). */
void fn_803605BC(CMemoryAlloc *allocator);
/* The texture list of a type (Game/csdTexList.cpp). */
RCPtr<CTexList> fn_80362094(u32 type);
/* Reads a texture archive file into a texture list. */
int fn_80361FB0(CTexList *texList, const char *file);
/* The full path of a file, and its release (Game/FadeTask.cpp). */
char *fn_80051810(const char *name);
void fn_80051830(char *path);
}

class TCsdSystem : public Sonicteam::System::Singleton<TCsdSystem, Sonicteam::System::CreateStatic<TCsdSystem> >,
                   public CMemoryAlloc {
  public:
    TCsdSystem();
    virtual ~TCsdSystem();
    virtual void *Allocate(u32 size);
    virtual void Deallocate(void *p);
};

/* The handles the game uses where it has no scene, node, texture list or scene list (other units
 * name them by address). */
RCPtr<CTexList> lbl_805EE110;
UnkCsdHandle_80042DF0 lbl_805EE114;
RCPtr<CScene> lbl_805EE118;
RCPtr<CNode> lbl_805EE11C;

TCsdSystem::TCsdSystem() {
    Chao::CSD::InitializeRW(640.0f, 480.0f);
    fn_803605BC(this);
    lbl_805EE110 = RCPtr<CTexList>(0);
    lbl_805EE114 = UnkCsdHandle_80042DF0(0);
    lbl_805EE118 = RCPtr<CScene>(0);
    lbl_805EE11C = RCPtr<CNode>(0);
}

TCsdSystem::~TCsdSystem() {
    Chao::CSD::Terminate();
}

void *TCsdSystem::Allocate(u32 size) {
    return fn_80049B90(size, 1);
}

void TCsdSystem::Deallocate(void *p) {
    fn_80049B34(p);
}

/* Whether a file exists; the check also receives the file system (as Game/TMessageDisp.cpp has
 * it). */
inline bool FileExists(UnkFileSystem_800371D8 *fileSystem, const char *path) {
    return ((bool (*)(const char *, UnkFileSystem_800371D8 *))fileSystem->m_exists)(path, fileSystem);
}

/* The project file name of name (fn_800423B8). */
extern "C" const char *fn_800423B8(TCsdSystem *system, const char *name) {
    static char s_path[0x40];
    sprintf(s_path, "%s.gncp", name);
    return s_path;
}

/* The project file name of name in capitals (fn_8004237C). */
extern "C" const char *fn_8004237C(TCsdSystem *system, const char *name) {
    static char s_path[0x50];
    sprintf(s_path, "%s.GNCP", name);
    return s_path;
}

/* Loads project's textures from a texture archive file; false when there is none
 * (fn_800422A8). */
extern "C" bool fn_800422A8(CProject *project, const char *file) {
    if (project != 0) {
        char *path = fn_80051810(file);
        if (FileExists(fn_800371D8(), path)) {
            RCPtr<CTexList> texList(fn_80362094('.RWT'));
            fn_80361FB0(texList.Get(), path);
            fn_8036C64C(project, texList);
            fn_80051830(path);
            return true;
        }
        fn_80051830(path);
    }
    return false;
}

/* Loads project's textures from a texture dictionary already in memory (fn_800421BC). */
extern "C" int fn_800421BC(CProject *project, void *dictionary) {
    if (project != 0 && dictionary != 0) {
        RCPtr<CTexList> texList(fn_80362094('.RW2'));
        if (texList.Get()->Load(dictionary, 0) != 0) {
            fn_8036C64C(project, texList);
        }
    }
    return 0;
}

#include <map_impl>
