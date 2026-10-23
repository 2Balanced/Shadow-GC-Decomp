/* csdmScene.cpp (file name from the allocator's records): CScene, a scene of a CSD project at run
 * time. It holds the scene's data, its motion, font list and run-time state (each placed in a pool
 * the scene allocates), the frame range it plays, the nodes it has handed out, and the observers
 * and functions it calls before and after it updates and draws. Class names other than CScene,
 * CNode, CResourceBase, RCPtr and CBase, and all member names, are inferred.
 *
 * Code generation: the object layer is built with deferred inlining (cflags_csd_rw), one level
 * deep, so the functions are defined here in the reverse of their address order, and an inline
 * function that is not expanded (one that calls another inline function, or a template member
 * defined outside its class) comes out after the code that first uses it. The empty inline calls
 * (__csd_nothing and the like) keep the library's small inline members out of line, as the game
 * has them. CScene::Unused and the copy constructor are not used by the game (the linker drops
 * them); they stand in for library code that instantiated the helpers the game keeps in this
 * order.
 *
 * NonMatching: 193 of 195 functions match, and .text has the game's layout. Update (0x803669B8)
 * schedules two loads of UpdateScene's arguments the other way round, and IsParentVisible
 * (0x8036635C) indexes the layers with other registers. In .data, the space the linker leaves for
 * the weak RCObject<CNode> vtables (whose copies the game keeps elsewhere) comes after a different
 * vtable than in the game. */
#define CSD_OBJECT_LAYER
#define CSD_RCPTR_DEALLOC_CTOR
#define MSL_PAIR_CONVERTING_CONSTRUCTOR
#define MSL_TREE_CONST_ITERATOR_MIXED
#define MSL_TREE_END_NODE_NAME __tree_end_node_csdmScene
#include <iterator>
#include <functional>

#include "Game/CsdScene.h"

#pragma pool_data off
#pragma auto_inline off

extern "C" {
/* The platform layer (Game/csdPlatform.cpp). */
}

namespace Chao {
namespace CSD {

/* The time a frame takes when the scene is advanced without one. */
static f32 s_frameTime = 0.016666f;

template <>
void CObserver<CScene>::OnSubjectDestroyed(CScene *subject) {
    m_subjects.remove(subject);
}

template <>
CObserver<CScene>::~CObserver() {}

template <>
CSubject<CScene, CSceneObserver>::~CSubject() {}

CScene::~CScene() {
    Notify(&CSceneObserver::OnSubjectDestroyed, GetSubject());
    m_observers.clear();
    NodeMap::const_iterator it;
    for (it = m_nodes.begin(); it != m_nodes.end();) {
        (it++)->second->DetachScene();
    }
    if (m_motion != 0) {
        m_motion->~CMotion();
        m_motion = 0;
    }
    if (m_fontList != 0) {
        m_fontList->~CFontList();
        m_fontList = 0;
    }
    if (m_palette != 0) {
        m_palette->~CMotionPalette();
        m_palette = 0;
    }
    fn_80360528(m_pool);
    if (m_pattern != 0) {
        delete m_pattern;
        m_pattern = 0;
    }
}

void CScene::CopyFrom(const CResourceBase<SSceneInfo> &base) {
    const CScene &other = static_cast<const CScene &>(base);
    CResourceBase<SSceneInfo>::CopyFrom(other);
    if (m_motion != 0) {
        m_motion->~CMotion();
        m_motion = 0;
    }
    if (other.m_motion != 0) {
        m_motion = new (m_pool->m_motion) CMotion(other.m_motion);
    }
    if (m_fontList != 0) {
        m_fontList->~CFontList();
        m_fontList = 0;
    }
    if (other.m_fontList != 0) {
        m_fontList = new (m_pool->m_fontList) CFontList(other.m_fontList);
    }
    InitState();
    m_parent = other.m_parent;
    m_prevFrame = other.m_prevFrame;
    m_frame = other.m_frame;
    m_speed = other.m_speed;
    m_startFrame = other.m_startFrame;
    m_endFrame = other.m_endFrame;
    m_motionIndex = other.m_motionIndex;
    m_end = other.m_end;
    m_playMode = other.m_playMode;
    m_motionIndex = other.m_motionIndex;
}

void CScene::Reset() {
    m_motion = 0;
    m_fontList = 0;
    m_motionIndex = -1;
    m_prevFrame = 0.0f;
    m_frame = 0.0f;
    m_speed = 1.0f;
    m_palette = 0;
    m_parent = 0;
    m_pattern = 0;
    m_end = 0;
    m_owner = 0;
    m_frameCount = 0.0f;
    m_project = 0;
    if (m_pool == 0) {
        m_pool = (SScenePool *)fn_80360564(sizeof(SScenePool), "csdmScene.cpp", 0xad);
    }
}

void *CScene::GetOwner() {
    return m_owner;
}

/* (Not used by the game; the linker drops it.) */
void CScene::SetMotion(const CMotion *motion) {
    ClearMotion();
    if (motion != 0) {
        m_motion = new (m_pool->m_motion) CMotion(motion);
    }
}

extern "C" void fn_80367274(CScene *scene, s32 index) {
    if (index != scene->m_motionIndex && index < scene->m_resource->m_motionCount) {
        if (scene->m_motion != 0) {
            scene->m_motion->~CMotion();
            scene->m_motion = 0;
        }
        scene->m_motion = new (scene->m_pool->m_motion) CMotion(&scene->m_resource->m_motions[index], scene->m_data);
        scene->m_motionIndex = index;
        if (scene->m_resource->m_version >= 2 && scene->m_resource->m_motionFrames != 0) {
            SVec2 *frames = scene->m_resource->m_motionFrames;
            scene->m_startFrame = frames[index].m_x;
            scene->m_endFrame = frames[index].m_y;
        }
    }
}

extern "C" void fn_80367238(CScene *scene, const char *name) {
    s32 index = scene->FindMotion(name);
    if (index >= 0) {
        fn_80367274(scene, index);
    }
}

/* (Not used by the game; the linker drops it.) */
void CScene::ClearMotion() {
    if (m_motion != 0) {
        m_motion->~CMotion();
        m_motion = 0;
    }
}

s32 CScene::FindMotion(const char *name) {
    return FindMotionIndex((const SScene2 *)m_resource, name);
}

extern "C" void fn_80367140(CScene *scene, float speed) {
    scene->m_speed = speed;
}

extern "C" float fn_80367138(CScene *scene) {
    return scene->m_speed;
}

void CScene::InitState() {
    if (m_palette != 0) {
        m_palette->~CMotionPalette();
        m_palette = 0;
    }
    SSceneInfo *info = m_resource;
    if (info != 0) {
        if (m_fontList != 0) {
            m_palette = new (m_pool->m_palette) CMotionPalette(info, m_fontList->m_resource);
        } else {
            m_palette = new (m_pool->m_palette) CMotionPalette(info, 0);
        }
    }
}

/* Advances the scene by one frame of the default length. */
extern "C" void fn_80366F9C(CScene *scene) {
    scene->Update(s_frameTime);
}

#pragma pool_data on
void CScene::Update(float dt) {
    if (m_resource == 0) {
        return;
    }
    Notify(&CSceneObserver::OnUpdate, this);
    FunctorList::iterator update = m_onUpdate.begin();
    while (update != m_onUpdate.end()) {
        if (!(*update->Get())(this)) {
            update = m_onUpdate.erase(update);
        } else {
            ++update;
        }
    }
    if (m_prevFrame >= m_endFrame && m_speed > 0.0f) {
        m_playMode.OnEnd(this);
        Notify(&CSceneObserver::OnEnd, this);
        FunctorList::iterator end = m_onEnd.begin();
        while (end != m_onEnd.end()) {
            if (!(*end->Get())(this)) {
                end = m_onEnd.erase(end);
            } else {
                ++end;
            }
        }
    }
    if (m_prevFrame <= m_startFrame && m_speed < 0.0f) {
        m_playMode.OnStart(this);
        Notify(&CSceneObserver::OnStart, this);
        FunctorList::iterator start = m_onStart.begin();
        while (start != m_onStart.end()) {
            if (!(*start->Get())(this)) {
                start = m_onStart.erase(start);
            } else {
                ++start;
            }
        }
    }
    SMotion *motion = 0;
    if (m_motion != 0) {
        motion = m_motion->m_resource;
    }
    SCast *parent = 0;
    if (m_parent.Get() != 0) {
        parent = m_parent->m_pattern->m_resource;
    } else if (m_pattern != 0) {
        m_pattern->Update();
        parent = m_pattern->m_resource;
    }
    void *fontList = 0;
    if (m_fontList != 0) {
        fontList = m_fontList->m_resource;
    }
    NodeMap::const_iterator node;
    for (node = m_nodes.begin(); node != m_nodes.end(); ++node) {
        (*node).second->Update();
    }
    UpdateScene(m_frame, m_palette->m_resource, m_resource, motion, fontList, parent);
    Notify(&CSceneObserver::OnPostUpdate, this);
    FunctorList::iterator post = m_onPostUpdate.begin();
    while (post != m_onPostUpdate.end()) {
        if (!(*post->Get())(this)) {
            post = m_onPostUpdate.erase(post);
        } else {
            ++post;
        }
    }
    if (m_end == 0) {
        m_prevFrame = m_frame;
        m_frame += dt * (m_speed * m_resource->m_frameRate);
        if (m_frame > m_endFrame) {
            m_frame = m_endFrame;
        } else if (m_frame < m_startFrame) {
            m_frame = m_startFrame;
        }
    }
    for (node = m_nodes.begin(); node != m_nodes.end(); ++node) {
        if ((*node).second->GetObserverCount() != 0) {
            (*node).second->NotifyObservers();
        }
    }
}
#pragma pool_data off

void CScene::Render(const RCPtr<CTexList> &texList) {
    if (m_resource == 0) {
        return;
    }
    Notify(&CSceneObserver::OnDraw, this);
    FunctorList::iterator it = m_onDraw.begin();
    while (it != m_onDraw.end()) {
        if (!(*it->Get())(this)) {
            it = m_onDraw.erase(it);
        } else {
            ++it;
        }
    }
    BeginPlatform(GetPlatform());
    void *fontList = 0;
    if (m_fontList != 0) {
        fontList = m_fontList->m_resource;
    }
    DrawScene(m_resource, texList, fontList, m_palette->m_resource);
    EndPlatform(GetPlatform());
    Notify(&CSceneObserver::OnPostDraw, this);
    FunctorList::iterator post = m_onPostDraw.begin();
    while (post != m_onPostDraw.end()) {
        if (!(*post->Get())(this)) {
            post = m_onPostDraw.erase(post);
        } else {
            ++post;
        }
    }
}

void CScene::InitFrames() {
    m_startFrame = GetInfo()->m_startFrame;
    m_endFrame = GetInfo()->m_endFrame;
    m_frame = m_prevFrame = m_startFrame;
}

extern "C" void fn_80366740(CScene *scene, float frame) {
    scene->m_prevFrame = frame;
    scene->m_frame = frame;
}

extern "C" float fn_80366738(CScene *scene) {
    return scene->m_prevFrame;
}

float CScene::GetStartFrame() {
    return m_startFrame;
}

extern "C" float fn_80366728(CScene *scene) {
    return scene->m_endFrame;
}

extern "C" RCPtr<CNode> fn_803666B4(CScene *scene, const char *name) {
    SNameIndex2 entry = scene->FindNodeEntry(name);
    if (entry.m_name != 0) {
        return scene->GetNode(&entry);
    }
    return RCPtr<CNode>(0);
}

SNameIndex2 CScene::FindNodeEntry(const char *name) {
    SNameIndex2 *found = FindEntry(name, (const SNameTable2Owner *)m_resource);
    if (found != 0) {
        return *found;
    }
    SNameIndex2 none;
    none.m_name = 0;
    return none;
}

RCPtr<CNode> CScene::GetNode(const SNameIndex2 *entry) {
    if (entry != 0) {
        SCastIndex index(entry->m_index, entry->m_value);
        NodeMap::iterator it = m_nodes.find(index);
        if (it != m_nodes.end()) {
            return (*it).second;
        }
        SCastInfo *info = m_resource->m_layers[entry->m_index].m_casts[entry->m_value];
        RCPtr<CNode> node(new ("csdmScene.cpp", 0x3ce) CNode(info, &m_data, m_palette, entry, this, m_fontList),
                          s_deleteTag);
        m_nodes.insert(std::pair<SCastIndex, RCPtr<CNode> >(index, node));
        return node;
    }
    return RCPtr<CNode>(0);
}

extern "C" void fn_803663C0(CScene *scene, const RCPtr<CNode> &parent) {
    scene->m_parent = parent;
    if (scene->m_parent.Get() != 0 && scene->IsParentVisible()) {
        scene->m_parent->Refresh();
    }
    if (scene->m_pattern != 0) {
        delete scene->m_pattern;
        scene->m_pattern = 0;
    }
}

/* Whether a layer's root cast needs its own companion matrices. */
s32 CScene::IsParentVisible() {
    SSceneInfo *info = m_resource;
    SSceneState *state = m_palette->m_resource;
    for (s32 i = 0; i < state->m_layerCount; i++) {
        if (state->m_layers[i].m_casts[info->m_layers[i].m_root].m_matrix.m_scale != 0) {
            return 1;
        }
    }
    return 0;
}

void CScene::SetPattern(CMotionPattern *pattern) {
    if (m_pattern != 0) {
        delete m_pattern;
        m_pattern = 0;
    }
    m_parent = 0;
    if (pattern != 0) {
        m_pattern = new ("csdmScene.cpp", 0x44e) CMotionPattern(pattern);
    }
}

/* The cast that places the whole scene (made the first time it is asked for). */
CMotionPattern *CScene::GetPattern() {
    if (m_pattern != 0) {
        return m_pattern;
    }
    CMotionPattern *pattern = new ("csdmScene.cpp", 0x464) CMotionPattern(GetAspect());
    SetPattern(pattern);
    delete pattern;
    return m_pattern;
}

float CScene::GetAspect() {
    if (m_resource->m_version >= 1) {
        return m_resource->m_aspect;
    }
    return 1.3333f;
}

extern "C" void fn_80365B40(CScene *scene, s32 hide) {
    scene->GetPattern()->SetHide(hide);
}

extern "C" void fn_80365B18(CScene *scene, const SVec2 &position) {
    fn_80365AC8(scene, position.m_x, position.m_y);
}

/* Places the scene at (x, y) of the layout. */
extern "C" void fn_80365AC8(CScene *scene, float x, float y) {
    float ly;
    float lx;
    lx = x;
    ly = y;
    ToLayout(&lx, &ly);
    scene->GetPattern()->SetPosition(lx, ly);
}

/* (Not used by the game; the linker drops it.) */
void CScene::SetPosition(float x, float y) {
    GetPattern()->SetPosition(x, y);
}

extern "C" void fn_803659FC(CScene *scene, float x, float y) {
    scene->GetPattern()->SetScale(x, y);
}

extern "C" void fn_803659F0(CScene *scene) {
    scene->m_end = 1;
}

extern "C" void fn_803659E4(CScene *scene) {
    scene->m_end = 0;
}

extern "C" void fn_803659DC(CScene *scene, s32 mode) {
    scene->m_playMode.m_endMode = mode;
}

s32 CScene::GetEndMode() {
    return m_playMode.m_endMode;
}

extern "C" void fn_803659CC(CScene *scene, s32 mode) {
    scene->m_playMode.m_startMode = mode;
}

/* (Not used by the game; the linker drops it.) */
#pragma dont_inline on
void CScene::Unused(CSceneObserver *observer, CNode *node, CNodeObserver *nodeObserver, const SCastIndex &index) {
    Detach(observer);
    m_nodes.clear();
    m_nodes.insert(std::pair<SCastIndex, RCPtr<CNode> >(index, m_parent));
    SCastIndex other(1, 2);
    NodeMap::iterator it = m_nodes.begin();
    NodeMap::const_iterator copy;
    copy = it;
    m_parent.Set(node, s_deleteTag);
    node->CSubject<CNode, CNodeObserver>::GetSubject();
    nodeObserver->m_subjects.remove(node);
    CSubject<CScene, CSceneObserver>::GetSubject();
    Notify(&CObserver<CScene>::RemoveSubject, this);
    observer->RemoveSubject(this);
}
#pragma dont_inline reset

float CScene::GetFrameCount() {
    return m_frameCount;
}

/* (Not used by the game; the linker drops it.) */
CScene::CScene(const CScene &other) : m_parent(other.m_parent), m_pool(0) {
    Reset();
    CopyFrom(other);
}

void CScene::SetFrameTime(float time) {
    s_frameTime = time;
}

void CScene::Initialize(SSceneInfo *info, const RCPtr<unsigned char> &data, void *owner, CProject *project,
                        const CFontList *fontList) {
    Reset();
    Set(info, data);
    m_owner = owner;
    if (fontList != 0) {
        m_fontList = new (m_pool->m_fontList) CFontList(fontList);
    }
    InitState();
    InitFrames();
    m_frameCount = info->m_frameCount + 1.0f;
    m_project = project;
}

void CScene::SetFontList(const CFontList *fontList) {
    if (m_fontList != 0) {
        m_fontList->~CFontList();
        m_fontList = 0;
    }
    NodeMap::iterator it;
    for (it = m_nodes.begin(); it != m_nodes.end(); ++it) {
        (*it).second->SetFontList(fontList);
    }
    if (fontList != 0) {
        m_fontList = new (m_pool->m_fontList) CFontList(fontList);
        InitLayers(m_palette->m_resource, m_resource, m_fontList->m_resource);
    }
}

/* Rebuilds the run-time state's layers. */
void CScene::RebuildState() {
    m_palette->Rebuild(this, m_fontList);
}

float CScene::GetMotionStartFrame(s32 index) {
    if (m_resource->m_version >= 2) {
        return m_resource->m_motionFrames[index].m_x;
    }
    return m_startFrame;
}

/* The start frame of the motion named name (the scene's when there is none). */
extern "C" float fn_803644A0(CScene *scene, const char *name) {
    s32 index = scene->FindMotion(name);
    if (index >= 0) {
        return scene->GetMotionStartFrame(index);
    }
    return scene->m_startFrame;
}

float CScene::GetMotionEndFrame(s32 index) {
    if (m_resource->m_version >= 2) {
        return m_resource->m_motionFrames[index].m_y;
    }
    return m_endFrame;
}

/* The end frame of the motion named name (the scene's when there is none). */
extern "C" float fn_80364430(CScene *scene, const char *name) {
    s32 index = scene->FindMotion(name);
    if (index >= 0) {
        return scene->GetMotionEndFrame(index);
    }
    return scene->m_endFrame;
}

/* The scene, for its observers. */
CScene *CScene::GetSubject() {
    return this;
}

} // namespace CSD
} // namespace Chao

#include <map_impl>

