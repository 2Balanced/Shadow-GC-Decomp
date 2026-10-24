#ifndef GAME_CSDPROJECT_H
#define GAME_CSDPROJECT_H

/* The Chao CSD library's projects (Game/csdmProject.cpp; file names from the allocator's records,
 * csdmProject.cpp and csdmProject.h): a loaded project file, the scenes made from it, kept by name
 * and in drawing order, its texture and font lists, and the lists of scenes the game asks for.
 * A unit includes this header with CSD_OBJECT_LAYER and CSD_PROJECT defined (and the MSL options
 * Game/csdmProject.cpp lists). The library has no RTTI; RCPtr<CSceneList>::RCObject<CSceneList>
 * and CResourceBase<Chao::CSD::Project> are named by game code's RTTI records (the project data
 * is SProject here, as in Game/csdAccess.cpp), CProject by the allocator's file names and the
 * library's naming, and every other name is inferred. Game code declares its own copies of
 * CProject (Game/TGi.h; UnkCsdProject_800BFF30 in Game/CsdRCPtr.h is the same class) and of
 * UnkCsdHandle_80042DF0, so a unit includes either those or this header. */

#include <functional>
#include <string.h>

#include "Game/CsdScene.h"

namespace Chao {
namespace CSD {
class CSceneList;
} // namespace CSD
} // namespace Chao

/* The counted pointer to a list of scenes a project returns (RCPtr<CSceneList> by the RTTI of its
 * RCObject; game code declares it under this name, Game/TGi.h). Its copy constructor and
 * destructor are the game's; the library's own members are inline here. */
class UnkCsdHandle_80042DF0 {
  public:
    typedef Chao::CSD::RCPtr<Chao::CSD::CSceneList>::RCObject<Chao::CSD::CSceneList> Object;

    UnkCsdHandle_80042DF0(void *object);
    UnkCsdHandle_80042DF0(const UnkCsdHandle_80042DF0 &other);
    ~UnkCsdHandle_80042DF0();

    /* Takes object with the deallocation d says. */
    template <class D>
    UnkCsdHandle_80042DF0(Chao::CSD::CSceneList *object, D d) : m_object(0) {
        Set(object, d);
    }
    template <class D>
    void Set(Chao::CSD::CSceneList *object, const D &d) {
        Object *current = m_object;
        if (current != 0) {
            if (current->m_object == object) {
                return;
            }
            current->Release();
            m_object = 0;
        }
        if (object != 0) {
            m_object = CSD_RCOBJECT_NEW(410) Object(object, d);
            m_object->m_count++;
        }
    }
    Chao::CSD::CSceneList *Get() const {
        return m_object != 0 ? m_object->m_object : 0;
    }

    /* 0x0 */ Object *m_object;
};

namespace Chao {
namespace CSD {

/* A node of a project's scene tree (as Game/csdAccess.cpp reads it). */
struct SSceneNode {
    /* 0x00 */ s32 m_sceneCount;
    /* 0x04 */ SSceneInfo **m_scenes;
    /* 0x08 */ SNameIndex *m_sceneIndex;
    /* 0x0C */ s32 m_childCount;
    /* 0x10 */ SSceneNode *m_children;
    /* 0x14 */ SNameIndex *m_childIndex;
};

/* A loaded project (CResourceBase<Chao::CSD::Project> in game code's RTTI). */
struct SProject {
    /* 0x0 */ SSceneNode *m_root;
    /* 0x4 */ const char *m_name;
    /* 0x8 */ u32 m_texListType; /* four characters */
    /* 0xC */ SFontList *m_fontList;
};

/* The project file as the loader relocates it (Game/csdLoader.cpp). */
struct SProjectRoot;

/* The render state the scenes are drawn with (Game/csdLoader.cpp). */
struct SRenderState {
    void End();
    void Begin();
};

/* Game/csdAccess.cpp */
const SSceneNode *GetNode(const char *path, const SProject *project);
const SSceneNode *SearchNode(const char *name, const SProject *project);
SSceneInfo *GetScene(const char *path, const SProject *project, const char **name);
SSceneInfo *SearchScene(const char *name, const SProject *project, const char **outName);

/* Game/csdLoader.cpp */
void LoadFile(void **project, u32 *projectSize, void **textures, u32 *texturesSize, const void *data, u32 size);
void LoadProject(SProjectRoot **project, void *data);
void PrepareProject(SProjectRoot *project);
SRenderState *GetRenderState();

/* Game/csdFileSystem.cpp */
void GetDirectory(char *buffer, u32 size);
void SetDirectory(const char *path);
bool LoadFile(void *out, const char *name);
u32 LoadFile(const char *name);

/* Makes the scenes of a project in place of the library's CScene. */
class CSceneFactory {
  public:
    virtual CScene *Create() = 0;
};

/* Orders scene names (a function object of the binary_function kind, so a default-constructed one
 * is not zero-filled). */
struct LessName : std::binary_function<const char *, const char *, bool> {
    bool operator()(const char *const &a, const char *const &b) const {
        __csd_nothing();
        return strcmp(a, b) < 0;
    }
};

/* A list of scenes by name (what a project hands out for a scene tree). */
class CSceneList : public CBase {
  public:
    typedef std::map<const char *, RCPtr<CScene>, LessName> SceneMap;

    CSceneList() {}

    void Add(const RCPtr<CScene> &scene);

    /* 0x0 */ SceneMap m_scenes;
};

/* A scene whose drawing order is to be redone, and its old order. */
struct SReorder {
    /* 0x0 */ CScene *m_scene;
    /* 0x4 */ f32 m_order;
};

class CProject;

} // namespace CSD
} // namespace Chao

/* CProject's functions that game code calls by these names. */
extern "C" {
void fn_8036E39C(Chao::CSD::CProject *project);
void fn_8036D6E0(Chao::CSD::CProject *project, Chao::CSD::CScene *scene);
void fn_8036C64C(Chao::CSD::CProject *project, const Chao::CSD::RCPtr<Chao::CSD::CTexList> &texList);
}

namespace Chao {
namespace CSD {

/* CProject: a project and the scenes made from it. The scenes are kept by name and by their frame
 * count, which orders their updates and drawing. */
class CProject : public CResourceBase<SProject>, public CBase {
  public:
    typedef std::multimap<const char *, RCPtr<CScene>, LessName> SceneMap;
    typedef std::multimap<float, RCPtr<CScene> > OrderMap;
    typedef std::list<RCPtr<CScene> > SceneList;
    typedef std::list<SReorder> ReorderList;
    class iterator;

    CProject() {
        fn_8036E39C(this);
    }
    virtual ~CProject();
    virtual void CopyFrom(const CResourceBase<SProject> &other);

    RCPtr<CScene> CreateScene(SSceneInfo *info, const char *name, const UnkCsdHandle_80042DF0 &list,
                              CSceneFactory *factory);
    RCPtr<CScene> GetScene(const char *path, CSceneFactory *factory);
    void ProcessRemovals();
    void SetFontList(const CFontList *fontList);
    void Reorder(ReorderList::iterator &reorder);
    void ProcessReorders();
    iterator End();
    iterator Begin();
    RCPtr<CScene> FindScene(CScene *scene);
    void RemoveOrder(const RCPtr<CScene> &scene);
    void RemoveName(const RCPtr<CScene> &scene);
    void RemoveScene(const RCPtr<CScene> &scene);
    void AddScene(const RCPtr<CScene> &scene);
    void AddListScene(SSceneInfo *info, const char *name, const UnkCsdHandle_80042DF0 &list, CSceneFactory *factory);
    void AddListScenes(SSceneNode *node, const UnkCsdHandle_80042DF0 &list, CSceneFactory *factory);

    /* Has the scene removed at the next update. */
    void RequestRemove(CScene *scene) {
        RCPtr<CScene> found = FindScene(scene);
        if (found.Get() != 0) {
            m_removals.push_back(found);
        }
    }
    /* Takes the project file data, with d to free it, and its font list (csdmProject.h). */
    template <class D>
    void Init(SProjectRoot *root, unsigned char *data, D d) {
        PrepareProject(root);
        SetData(root, data, d);
        if (m_resource->m_fontList != 0) {
            CFontList *fontList = new ("csdmProject.h", 0xc5) CFontList(m_resource->m_fontList, m_data);
            SetFontList(fontList);
            delete fontList;
        }
    }
    template <class D>
    void SetData(SProjectRoot *root, unsigned char *data, D d) {
        m_data.Attach(data, d);
        m_resource = (SProject *)root;
    }

    /* 0x0C */ SceneMap m_scenes;
    /* 0x1C */ OrderMap m_order;
    /* 0x2C */ RCPtr<CTexList> m_texList;
    /* 0x30 */ CFontList *m_fontList;
    /* 0x34 */ SceneList m_removals;
    /* 0x40 */ ReorderList m_reorders;

    /* Steps through the scenes in drawing order. (Defined after the members, so that the name
     * multimap's classes are instantiated before the order multimap's ones.) */
    class iterator {
      public:
        iterator() {}
        iterator(OrderMap::iterator it) : m_it(it) {}
        iterator &operator=(const iterator &other) {
            m_it = other.m_it;
            return *this;
        }
        CScene &operator*() const {
            return *(*m_it).second;
        }
        iterator &operator++() {
            ++m_it;
            return *this;
        }
        bool operator!=(const iterator &other) const {
            return m_it != other.m_it;
        }

        /* 0x0 */ OrderMap::iterator m_it;
    };
};

} // namespace CSD
} // namespace Chao

#endif
