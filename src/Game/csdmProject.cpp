/* csdmProject.cpp (file name from the allocator's records): Chao::CSD::CProject, a loaded project of
 * the Chao CSD library and the scenes made from it, and CSceneList, the list of scenes it hands out
 * (see Game/CsdProject.h). Game code calls most of it through the extern "C" fn_ names.
 *
 * Code generation follows Game/csdmScene.cpp: deferred inlining, so the functions are defined in the
 * reverse of their address order and the trees' template members follow at the end of the unit
 * (<map_impl>). The MSL options: MSL_MULTIMAP gives <map> the multimaps the project keeps its scenes
 * in (by name and by frame count); MSL_MAKE_PAIR_BY_VALUE makes make_pair copy its arguments, as the
 * multimaps' equal_range has it; the others are as in Game/csdmScene.cpp.
 *
 * The unit stays NonMatching (159 of 161 functions match):
 * - CProject::iterator::operator* (0x8036C318): GC/2.7 schedules its first load above the
 *   prologue's stores; retail keeps it after them (the later 2.7-era compiler of
 *   docs/research-notes.md, fourth difference).
 * - fn_8036F470: retail gives the key a stack slot above the end() temporary, GC/2.7 below it.
 * - Order: retail places fn_8036F470, CSceneList::Add and the name map's end() between two batches
 *   of tree code (the second with its own copies of two strings), which GC/2.7 does not reproduce;
 *   some inline instances also come early, after functions the linker dropped. */
#define CSD_OBJECT_LAYER
#define CSD_PROJECT
#define CSD_RCPTR_DEALLOC_CTOR
#define CSD_RCOBJECT_DTOR_DECL_ONLY
#define MSL_PAIR_CONVERTING_CONSTRUCTOR
#define MSL_TREE_CONST_ITERATOR_MIXED
#define MSL_TREE_ITERATOR_ASSIGN_CALL
#define MSL_TREE_END_NODE_NAME __tree_end_node_csdmProject
#define MSL_MULTIMAP
#define MSL_MAKE_PAIR_BY_VALUE
#include <iterator>
#include <functional>

#include "Game/CsdProject.h"

#pragma pool_data off
#pragma auto_inline off

extern "C" {
/* MSL strrchr and strncpy (the symbols are still placeholders). */
char *fn_803AC864(const char *s, int c);
char *fn_803ACABC(char *dst, const char *src, u32 size);
/* The texture list of a type (Game/csdTexList.cpp). */
Chao::CSD::RCPtr<Chao::CSD::CTexList> fn_80362094(u32 type);
}

namespace Chao {
namespace CSD {

/* "[%c%c%c%c] wo shori suru tekusucha shisutemu ga mitsukarimasen." (no texture list handles the
 * project's texture list type; Shift-JIS). */
#define s_noTexList                                                                                   \
    "[%c%c%c%c]\x82\xF0\x8F\x88\x97\x9D\x82\xB7\x82\xE9\x83" "e\x83N\x83X\x83`\x83\x83\x83V\x83X\x83" \
    "e\x83\x80\x82\xAA\x8C\xA9\x82\xC2\x82\xA9\x82\xE8\x82\xDC\x82\xB9\x82\xF1\x81" "B\n"

/* The scene of list named name (game code's fn_8036F470). */
extern "C" RCPtr<CScene> fn_8036F470(CSceneList *list, const char *name) {
    CSceneList::SceneMap::const_iterator it;
    it = list->m_scenes.find(name);
    if (it != list->m_scenes.end()) {
        return (*it).second;
    }
    return RCPtr<CScene>(0);
}

void CSceneList::Add(const RCPtr<CScene> &scene) {
    m_scenes.insert(std::pair<const char *, RCPtr<CScene> >((const char *)scene->GetOwner(), scene));
}

CProject::~CProject() {
    SceneMap::iterator it;
    for (it = m_scenes.begin(); it != m_scenes.end();) {
        (it++)->second->SetProject(0);
    }
    if (m_fontList != 0) {
        delete m_fontList;
        m_fontList = 0;
    }
}

void CProject::CopyFrom(const CResourceBase<SProject> &base) {
    const CProject &other = static_cast<const CProject &>(base);
    CResourceBase<SProject>::CopyFrom(other);
    m_texList = other.m_texList;
    if (m_fontList != 0) {
        delete m_fontList;
        m_fontList = 0;
    }
    if (other.m_fontList != 0) {
        m_fontList = new ("csdmProject.cpp", 0x53) CFontList(other.m_fontList);
    }
}

extern "C" void fn_8036E39C(CProject *project) {
    project->m_fontList = 0;
}

/* Loads the project file path, with its textures from path's directory; returns the texture
 * list's result. */
extern "C" int fn_8036E114(CProject *project, const char *path) {
    int result = 1;
    if (project->m_resource != 0) {
        return 0;
    }
    u32 size = LoadFile(path);
    if (size == 0) {
        return 0;
    }
    void *file = fn_80360564(size, "csdmProject.cpp", 0x86);
    LoadFile(file, path);
    void *data;
    void *textures;
    u32 dataSize;
    u32 texturesSize;
    LoadFile(&data, &dataSize, &textures, &texturesSize, file, size);
    SProjectRoot *root;
    LoadProject(&root, data);
    if (textures != 0) {
        char directory[0x100];
        GetDirectory(directory, 0x100);
        /* The directory part of path: up to its last '/', or failing that its last '\\'. */
        const char *slash = fn_803AC864(path, '/');
        const char *separator = slash;
        if (slash == 0) {
            separator = fn_803AC864(path, '\\');
        }
        if (separator != 0) {
            char *dir = (char *)fn_80360564(strlen(path) + 1, "csdmProject.cpp", 0xa1);
            u32 length = separator - path;
            fn_803ACABC(dir, path, length);
            dir[length] = 0;
            SetDirectory(dir);
            fn_80360528(dir);
        }
        RCPtr<CTexList> texList = fn_80362094(((SProject *)root)->m_texListType);
        if (texList.Get() != 0) {
            result = texList->Load(textures, texturesSize);
            if (result > 0) {
                fn_8036C64C(project, texList);
            }
        } else {
            const char *type = (const char *)&((SProject *)root)->m_texListType;
            if (GetPlatform()->IsSwapped()) {
                fn_80361DC4(s_noTexList, type[0], type[1], type[2], type[3]);
            } else {
                fn_80361DC4(s_noTexList, type[3], type[2], type[1], type[0]);
            }
        }
        fn_80360528(textures);
        SetDirectory(directory);
    }
    project->Init(root, (unsigned char *)data, RCFreeTag());
    fn_80360528(file);
    return result;
}

/* Takes a project file of size bytes at file (its textures read from directory). */
extern "C" int fn_8036DB70(CProject *project, const void *file, u32 size, const char *directory) {
    int result = 1;
    void *data;
    void *textures;
    u32 dataSize;
    u32 texturesSize;
    LoadFile(&data, &dataSize, &textures, &texturesSize, file, size);
    SProjectRoot *root;
    LoadProject(&root, data);
    project->Init(root, (unsigned char *)data, RCFreeTag());
    if (textures != 0) {
        char current[0x100];
        GetDirectory(current, 0x100);
        if (directory != 0) {
            SetDirectory(directory);
        }
        RCPtr<CTexList> texList = fn_80362094(((SProject *)root)->m_texListType);
        if (texList.Get() != 0) {
            result = texList->Load(textures, texturesSize);
            if (result != 0) {
                fn_8036C64C(project, texList);
            }
        } else {
            /* (Here the type's value itself is taken as the address of its characters.) */
            const char *type = (const char *)((SProject *)root)->m_texListType;
            if (GetPlatform()->IsSwapped()) {
                fn_80361DC4(s_noTexList, type[0], type[1], type[2], type[3]);
            } else {
                fn_80361DC4(s_noTexList, type[3], type[2], type[1], type[0]);
            }
        }
        fn_80360528(textures);
        SetDirectory(current);
    }
    return result;
}

/* Advances every scene by one frame of the default length. */
extern "C" void fn_8036DAD4(CProject *project) {
    if (project->m_resource != 0) {
        project->ProcessRemovals();
        project->ProcessReorders();
        CProject::OrderMap::iterator it;
        for (it = project->m_order.begin(); it != project->m_order.end(); ++it) {
            fn_80366F9C((*it).second.Get());
        }
    }
}

/* Advances every scene by dt. */
extern "C" void fn_8036DA1C(CProject *project, float dt) {
    if (project->m_resource != 0) {
        project->ProcessRemovals();
        project->ProcessReorders();
        CProject::OrderMap::iterator it;
        for (it = project->m_order.begin(); it != project->m_order.end(); ++it) {
            (*it).second->Update(dt);
        }
    }
}

/* Draws every scene in order. */
extern "C" void fn_8036D94C(CProject *project) {
    if (project->m_resource != 0 && project->m_texList.Get() != 0) {
        GetRenderState()->Begin();
        CProject::OrderMap::iterator it;
        for (it = project->m_order.begin(); it != project->m_order.end(); ++it) {
            (*it).second->Render(project->m_texList);
        }
        GetRenderState()->End();
    }
}

/* The scene at path, playing its motion number motion. */
extern "C" RCPtr<CScene> fn_8036D890(CProject *project, const char *path, s32 motion, CSceneFactory *factory) {
    if (project->m_resource == 0) {
        return RCPtr<CScene>(0);
    }
    RCPtr<CScene> scene = project->GetScene(path, factory);
    if (scene.Get() != 0) {
        fn_80367274(scene.Get(), motion);
    }
    return scene;
}

/* The scene at path, playing its motion named motion. */
extern "C" RCPtr<CScene> fn_8036D7D4(CProject *project, const char *path, const char *motion,
                                     CSceneFactory *factory) {
    if (project->m_resource == 0) {
        return RCPtr<CScene>(0);
    }
    RCPtr<CScene> scene = project->GetScene(path, factory);
    if (scene.Get() != 0) {
        fn_80367238(scene.Get(), motion);
    }
    return scene;
}

/* Has the project remove scene at its next update. */
extern "C" void fn_8036D6E0(CProject *project, CScene *scene) {
    project->RequestRemove(scene);
}

/* The first scene named name. */
extern "C" RCPtr<CScene> fn_8036D660(CProject *project, const char *name) {
    std::pair<CProject::SceneMap::iterator, CProject::SceneMap::iterator> range = project->m_scenes.equal_range(name);
    if (range.first != range.second) {
        return (*range.first).second;
    }
    return RCPtr<CScene>(0);
}

/* A list of every scene of the project. */
extern "C" UnkCsdHandle_80042DF0 fn_8036D598(CProject *project, CSceneFactory *factory) {
    if (project->m_resource == 0) {
        return UnkCsdHandle_80042DF0(0);
    }
    UnkCsdHandle_80042DF0 list(new ("csdmProject.cpp", 0x296) CSceneList(), RCDeleteTag());
    project->AddListScenes(project->m_resource->m_root, list, factory);
    return list;
}

/* A list of the scenes under the node at path. */
extern "C" UnkCsdHandle_80042DF0 fn_8036D2FC(CProject *project, const char *path, CSceneFactory *factory) {
    if (project->m_resource == 0) {
        return UnkCsdHandle_80042DF0(0);
    }
    const SSceneNode *node;
    if (strchr(path, '/') != 0) {
        node = GetNode(path, project->m_resource);
    } else {
        node = SearchNode(path, project->m_resource);
    }
    if (node == 0) {
        return UnkCsdHandle_80042DF0(0);
    }
    UnkCsdHandle_80042DF0 list(new ("csdmProject.cpp", 0x2ce) CSceneList(), RCDeleteTag());
    /* (AddListScenes reads the node through a plain pointer: it reloads its fields every pass.) */
    project->AddListScenes((SSceneNode *)node, list, factory);
    return list;
}

/* Adds the scenes of node and of the nodes under it to list. */
void CProject::AddListScenes(SSceneNode *node, const UnkCsdHandle_80042DF0 &list, CSceneFactory *factory) {
    for (s32 i = 0; i < node->m_sceneCount; i++) {
        AddListScene(node->m_scenes[node->m_sceneIndex[i].m_index], node->m_sceneIndex[i].m_name, list, factory);
    }
    for (s32 i = 0; i < node->m_childCount; i++) {
        AddListScenes(&node->m_children[i], list, factory);
    }
}

void CProject::AddListScene(SSceneInfo *info, const char *name, const UnkCsdHandle_80042DF0 &list,
                            CSceneFactory *factory) {
    RCPtr<CScene> scene = CreateScene(info, name, list, factory);
    fn_80367274(scene.Get(), 0);
}

void CProject::AddScene(const RCPtr<CScene> &scene) {
    m_scenes.insert(std::pair<const char *, RCPtr<CScene> >((const char *)scene->GetOwner(), scene));
    m_order.insert(std::pair<float, RCPtr<CScene> >(scene->GetFrameCount(), scene));
}

void CProject::RemoveScene(const RCPtr<CScene> &scene) {
    /* (Tested without "!= 0": the pointer's operator-> stays out of line then, as in retail.) */
    if (scene->GetProject()) {
        RemoveName(scene);
        scene->SetProject(0);
        RemoveOrder(scene);
    }
}

void CProject::RemoveName(const RCPtr<CScene> &scene) {
    std::pair<SceneMap::iterator, SceneMap::iterator> range = m_scenes.equal_range((const char *)scene->GetOwner());
    for (; range.first != range.second; ++range.first) {
        if ((*range.first).second == scene) {
            m_scenes.erase(range.first);
            break;
        }
    }
}

void CProject::RemoveOrder(const RCPtr<CScene> &scene) {
    std::pair<OrderMap::iterator, OrderMap::iterator> range = m_order.equal_range(scene->GetFrameCount());
    for (; range.first != range.second; ++range.first) {
        if ((*range.first).second == scene) {
            m_order.erase(range.first);
            break;
        }
    }
}

RCPtr<CScene> CProject::FindScene(CScene *scene) {
    std::pair<OrderMap::iterator, OrderMap::iterator> range = m_order.equal_range(scene->GetFrameCount());
    for (; range.first != range.second; ++range.first) {
        if (&*(*range.first).second == scene) {
            return (*range.first).second;
        }
    }
    return RCPtr<CScene>(0);
}

CProject::iterator CProject::Begin() {
    return iterator(m_order.begin());
}

CProject::iterator CProject::End() {
    return iterator(m_order.end());
}

void CProject::ProcessReorders() {
    ReorderList::iterator it;
    for (it = m_reorders.begin(); it != m_reorders.end(); ++it) {
        Reorder(it);
    }
    m_reorders.clear();
}

/* Moves the scene of a reorder to its new place in the drawing order. */
void CProject::Reorder(ReorderList::iterator &reorder) {
    std::pair<OrderMap::iterator, OrderMap::iterator> range = m_order.equal_range(reorder->m_order);
    RCPtr<CScene> found;
    for (; range.first != range.second; ++range.first) {
        if (&*(*range.first).second == reorder->m_scene) {
            found = (*range.first).second;
            m_order.erase(range.first);
            break;
        }
    }
    if (found != RCPtr<CScene>(0)) {
        m_order.insert(std::pair<float, RCPtr<CScene> >(found->GetFrameCount(), found));
    }
}

/* Sets and gets the project's texture list. */
extern "C" void fn_8036C64C(CProject *project, const RCPtr<CTexList> &texList) {
    project->m_texList = texList;
}

extern "C" RCPtr<CTexList> fn_8036C628(CProject *project) {
    return project->m_texList;
}

void CProject::SetFontList(const CFontList *fontList) {
    if (m_fontList != 0) {
        delete m_fontList;
        m_fontList = 0;
    }
    if (fontList != 0) {
        m_fontList = new ("csdmProject.cpp", 0x526) CFontList(fontList);
    }
    for (SceneMap::iterator it = m_scenes.begin(); it != m_scenes.end(); ++it) {
        (*it).second->SetFontList(fontList);
    }
}

void CProject::ProcessRemovals() {
    for (SceneList::iterator it = m_removals.begin(); it != m_removals.end(); ++it) {
        /* (Through operator->, which retail inlines here, unlike the list's operator*.) */
        RemoveScene(*it.operator->());
    }
    m_removals.clear();
}

/* Removes every scene at the next update. */
extern "C" void fn_8036C248(CProject *project) {
    CProject::iterator it;
    for (it = project->Begin(); it != project->End(); ++it) {
        fn_8036D6E0(project, &*it);
    }
}

/* Makes the scene at path (a path, or a name searched for), with factory if there is one. */
RCPtr<CScene> CProject::GetScene(const char *path, CSceneFactory *factory) {
    if (m_resource == 0) {
        return RCPtr<CScene>(0);
    }
    const char *name = 0;
    SSceneInfo *info;
    if (strchr(path, '/') != 0) {
        info = Chao::CSD::GetScene(path, m_resource, &name);
    } else {
        info = SearchScene(path, m_resource, &name);
    }
    if (info == 0) {
        return RCPtr<CScene>(0);
    }
    RCPtr<CScene> scene((CScene *)0);
    if (factory != 0) {
        scene = factory->Create();
    } else {
        scene.Attach(new ("csdmProject.cpp", 0x579) CScene(), RCDeleteTag());
    }
    scene->Initialize(info, m_data, (void *)name, this, m_fontList);
    AddScene(scene);
    return scene;
}

/* Makes the scene of info, named name, for list. */
RCPtr<CScene> CProject::CreateScene(SSceneInfo *info, const char *name, const UnkCsdHandle_80042DF0 &list,
                                    CSceneFactory *factory) {
    RCPtr<CScene> scene((CScene *)0);
    if (factory != 0) {
        scene = factory->Create();
    } else {
        scene.Attach(new ("csdmProject.cpp", 0x593) CScene(), RCDeleteTag());
    }
    scene->Initialize(info, m_data, (void *)name, this, m_fontList);
    AddScene(scene);
    list.Get()->Add(scene);
    return scene;
}

} // namespace CSD
} // namespace Chao

#include <map_impl>
