/* The front end's CSD project cache (class names unknown): a vector of loaded CSD projects, each
 * with the name of the file it was loaded from. The phases load their projects through it
 * (fn_802B5504), so that a project another phase loaded is reused; at most two stay loaded, and
 * when memory runs out the oldest ones are freed. fn_801E5F0C returns the cache (a function-local
 * static in another unit). File name and all names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined first). */
#define MSL_STRING_DEFAULT_CTOR
#define MSL_STRING_COPY
#define MSL_VECTOR_MIDDLE
#define MSL_VECTOR_ERASE_ONE
#define MSL_ALGORITHM_COPY
#include <algorithm>
#include <string>
#include <vector>

#include "Game/CsdRCPtr.h"
#include <dolphin/types.h>
#include "size_assert.h"

namespace Chao {
namespace CSD {
/* CTexList (class name from the RTTI of RCPtr<CTexList>::RCObject<CTexList>): a project's texture
 * list. */
class CTexList;
} // namespace CSD
} // namespace Chao

using Chao::CSD::CTexList;
using Chao::CSD::RCPtr;

/* The loaded CSD project (Chao::CSD::CProject, Game/csdmProject.cpp; declared as in Game/TGi.h). */
namespace Chao {
namespace CSD {
class CProject {
  public:
    CProject();
    virtual ~CProject();

    /* 0x04 */ u8 field_0x4[0x4C - 0x4];

    static void *operator new(u32 size) {
        return fn_803605AC()->Alloc(size, "csdBase.h", 40);
    }
};
} // namespace CSD
} // namespace Chao

/* A cached project and the name of its file (class name unknown; member names inferred). */
struct UnkCsdCacheEntry_801E5F0C {
    /* 0x0 */ std::string m_name;
    /* 0xC */ Chao::CSD::CProject *m_project;
};
SIZE_ASSERT(UnkCsdCacheEntry_801E5F0C, 0x10);

/* The cache (class name unknown; member name inferred). */
class UnkCsdCache_801E5F0C {
  public:
    UnkCsdCache_801E5F0C();

    /* 0x0 */ std::vector<UnkCsdCacheEntry_801E5F0C> m_entries;
};
SIZE_ASSERT(UnkCsdCache_801E5F0C, 0xC);

/* An empty texture list (meaning inferred; see Game/AdvOption.cpp). */
extern RCPtr<CTexList> lbl_805EE110;

extern "C" {
/* The resource manager's language-specific path of a file (behaviour inferred, see Gong.cpp). */
void *fn_8003F8D4(void);
const char *fn_800423B8(void *arg0, const char *name, int arg2);
/* Loads file into a project; zero when it did not fit in memory (behaviour inferred). */
int fn_8036E114(Chao::CSD::CProject *project, const char *file);
/* A project's texture list, and setting it (behaviour inferred). */
RCPtr<CTexList> fn_8036C628(Chao::CSD::CProject *project);
void fn_8036C64C(Chao::CSD::CProject *project, const RCPtr<CTexList> &texList);
/* Creates a texture list of the given type (behaviour inferred). */
RCPtr<CTexList> fn_80362094(u32 type);
/* Loads the textures of a file into a list; zero when they did not fit in memory (behaviour
 * inferred). */
int fn_80361FB0(CTexList *texList, const char *file);
/* The name of a file's texture archive, and loading it for a project (behaviour inferred; see
 * Game/GiGameOver.cpp). */
const char *fn_800C02CC(const char *name);
void fn_800422A8(Chao::CSD::CProject *project, const char *file);

Chao::CSD::CProject *fn_802B5498(UnkCsdCache_801E5F0C *cache, const char *name);
bool fn_802B52C0(UnkCsdCache_801E5F0C *cache, Chao::CSD::CProject *project);
bool fn_802B526C(UnkCsdCache_801E5F0C *cache);
void fn_802B53B0(UnkCsdCache_801E5F0C *cache, const UnkCsdCacheEntry_801E5F0C &entry);
const char *fn_802B5234(const char *file);
void fn_802B57FC(UnkCsdCache_801E5F0C *cache);
}

/* The texture archive of a CSD file (name inferred). Inline: retail evaluates the call before the
 * texture list it is passed with. */
static inline const char *TextureArchive(const char *file) {
    return fn_800C02CC(file);
}

UnkCsdCache_801E5F0C::UnkCsdCache_801E5F0C() {
    m_entries.reserve(4);
}

/* Called when the front end starts (name unknown; does nothing). */
extern "C" void fn_802B5858(UnkCsdCache_801E5F0C *cache) {}

/* Called when the front end ends: frees every project (name unknown). */
extern "C" void fn_802B5838(UnkCsdCache_801E5F0C *cache) {
    fn_802B57FC(cache);
}

/* Frees every project (name unknown). */
extern "C" void fn_802B57FC(UnkCsdCache_801E5F0C *cache) {
    while (fn_802B52C0(cache, 0)) {
    }
}

/* Frees every project but the given one and loads its file's texture archive (name unknown). */
extern "C" void fn_802B5760(UnkCsdCache_801E5F0C *cache, Chao::CSD::CProject *project, const char *file) {
    for (UnkCsdCacheEntry_801E5F0C *it = cache->m_entries.begin(); it != cache->m_entries.end();) {
        if (it->m_project != project) {
            delete it->m_project;
            cache->m_entries.erase(it);
        } else {
            it++;
        }
    }
    fn_800422A8(project, TextureArchive(file));
}

/* Whether two texture lists are the same (name inferred; as in Game/AdvOption.cpp). */
static inline int SameTexList(const RCPtr<CTexList> &a, const RCPtr<CTexList> &b) {
    if (a.m_object != 0 && b.m_object != 0) {
        return a.m_object->m_object == b.m_object->m_object;
    }
    return a.m_object == b.m_object;
}

/* The project of a file: the cached one, or a newly loaded one, freeing the oldest projects while
 * it does not fit in memory; zero if it cannot be loaded (name unknown). */
extern "C" Chao::CSD::CProject *fn_802B5504(UnkCsdCache_801E5F0C *cache, const char *file) {
    UnkCsdCacheEntry_801E5F0C entry;
    entry.m_name = fn_802B5234(file);
    Chao::CSD::CProject *project = fn_802B5498(cache, entry.m_name.c_str());
    if (project != 0) {
        return project;
    }
    entry.m_project = new Chao::CSD::CProject;
    fn_802B526C(cache);
    while (fn_8036E114(entry.m_project, entry.m_name.c_str()) == 0) {
        if (!fn_802B52C0(cache, 0)) {
            return 0;
        }
    }
    if (SameTexList(fn_8036C628(entry.m_project), lbl_805EE110)) {
        RCPtr<CTexList> texList = fn_80362094(0x2E525754);
        while (fn_80361FB0(texList.Get(), TextureArchive(file)) == 0) {
            if (!fn_802B52C0(cache, 0)) {
                return 0;
            }
        }
        fn_8036C64C(entry.m_project, texList);
    }
    fn_802B53B0(cache, entry);
    return entry.m_project;
}

/* The cached project loaded from name (name unknown). */
extern "C" Chao::CSD::CProject *fn_802B5498(UnkCsdCache_801E5F0C *cache, const char *name) {
    for (UnkCsdCacheEntry_801E5F0C *it = cache->m_entries.begin(); it != cache->m_entries.end(); it++) {
        if (it->m_name == name) {
            return it->m_project;
        }
    }
    return 0;
}

/* Whether project is cached (name unknown). */
extern "C" bool fn_802B5434(UnkCsdCache_801E5F0C *cache, Chao::CSD::CProject *project) {
    for (UnkCsdCacheEntry_801E5F0C *it = cache->m_entries.begin(); it != cache->m_entries.end(); it++) {
        if (it->m_project == project) {
            return true;
        }
    }
    return false;
}

/* Adds an entry (name unknown). */
extern "C" void fn_802B53B0(UnkCsdCache_801E5F0C *cache, const UnkCsdCacheEntry_801E5F0C &entry) {
    cache->m_entries.push_back(entry);
}

/* Frees project, or the oldest project when it is zero; false if there was none (name unknown). */
extern "C" bool fn_802B52C0(UnkCsdCache_801E5F0C *cache, Chao::CSD::CProject *project) {
    for (UnkCsdCacheEntry_801E5F0C *it = cache->m_entries.begin(); it != cache->m_entries.end(); it++) {
        if (it->m_project == project || project == 0) {
            delete it->m_project;
            cache->m_entries.erase(it);
            return true;
        }
    }
    return false;
}

/* Frees the oldest projects until at most one is left (name unknown). */
extern "C" bool fn_802B526C(UnkCsdCache_801E5F0C *cache) {
    while (cache->m_entries.size() >= 2) {
        fn_802B52C0(cache, 0);
    }
    return true;
}

/* The language-specific path of a CSD file (name unknown). */
extern "C" const char *fn_802B5234(const char *file) {
    return fn_800423B8(fn_8003F8D4(), file, 0);
}
