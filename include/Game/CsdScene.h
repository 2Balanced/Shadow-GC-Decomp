#ifndef GAME_CSDSCENE_H
#define GAME_CSDSCENE_H

/* The Chao CSD library's object layer (Game/csdmScene.cpp and the units after it: csdmMotionPalette,
 * csdmMotionPattern, csdmNode, csdmProject; file names from the allocator's records): the scenes,
 * nodes and projects the game holds through RCPtr, built on the library's run-time core
 * (Game/CsdLib.h). A unit includes this header with CSD_OBJECT_LAYER defined, which gives
 * Game/CsdRCPtr.h's RCPtr and CBase the library's inline members. The library has no RTTI, so
 * only the class names that game code's RTTI records name are known (CScene, CNode, CResourceBase,
 * RCPtr, CBase); the other names are inferred. */

#include <list>
#include <map>

#include <dolphin/types.h>

#include "Game/CsdLib.h"

namespace Chao {
namespace CSD {

struct SScene2;
struct SNameTable2Owner;

/* An entry of a sorted name table with two values (see Game/csdAccess.cpp): a node's layer and
 * cast. */
struct SNameIndex2 {
    /* 0x0 */ const char *m_name;
    /* 0x4 */ s32 m_index;
    /* 0x8 */ s32 m_value;
};
struct SFontList;

/* Game/csdAccess.cpp and Game/csdLoader.cpp */
SNameIndex2 *FindEntry(const char *name, const SNameTable2Owner *owner);
s32 FindMotionIndex(const SScene2 *owner, const char *name);
void ToLayout(float *x, float *y);
void FromLayout(float *x, float *y);
void SplitPatternFrame(float frame, s32 *index, s32 *pattern, float *frac);
float MakePatternFrame(float frac, s32 index, s32 pattern);
void InitLayers(SSceneState *state, const SSceneInfo *info, void *fontList);
void UpdateScene(float frame, SSceneState *state, const SSceneInfo *info, const SMotion *motion, void *fontList,
                 SCast *parent);
void DrawScene(SSceneInfo *info, const RCPtr<CTexList> &texList, void *fontList, SSceneState *state);

class CNode;

/* The value attr has in node's cast data (Game/csdmNode.cpp; name inferred). */
void GetNodeAttribute(CValue *out, const CNode *node, s32 attr);

/* The tag RCPtr's constructor takes to delete its object (each unit has its own; name inferred). */
static RCDeleteTag s_deleteTag;

/* CResourceBase (class name from game code's RTTI): a resource of the library, the T it points to
 * and the data block that holds it. */
template <class T>
class CResourceBase {
  public:
    CResourceBase() {
        Reset();
    }
    virtual ~CResourceBase() {
        m_resource = 0;
    }
    /* Shares other's resource. */
    virtual void CopyFrom(const CResourceBase &other) {
        m_data = other.m_data;
        m_resource = other.m_resource;
    }

    void Reset() {
        m_data = 0;
        m_resource = 0;
    }
    void Set(T *resource, const RCPtr<unsigned char> &data) {
        Reset();
        m_data = data;
        m_resource = resource;
    }

    /* 0x4 */ RCPtr<unsigned char> m_data;
    /* 0x8 */ T *m_resource;
};

/* The list implementation the game's lists of pointers share (Game/ElecSecurityCode.h names it
 * UnkPtrList_8001B5B8): its constructor, destructor, first and end positions, removing a value,
 * clearing and the element count. The library's lists of pointers are thin wrappers around it
 * (std::list<T *> in the original, by their shape). */
struct CPtrListLink {
    /* 0x0 */ CPtrListLink *m_prev;
    /* 0x4 */ CPtrListLink *m_next;
};

struct CPtrListPosition {
    CPtrListPosition(const CPtrListPosition &other) : m_node(other.m_node) {}

    /* 0x0 */ CPtrListLink *m_node;
};

extern "C" {
void fn_8001BE04(void *list);
void fn_8001B5B8(void *list, short flag);
CPtrListPosition fn_8001B6B0(void *list);
CPtrListPosition fn_8001B674(void *list);
void fn_8001B42C(void *list, void *const &value);
void fn_8001E404(void *list);
u32 fn_80058F00(void *list);
}

/* Does nothing (an inline call that keeps the list's members out of line, as the library's are). */
inline void __csd_list_nothing() {}

template <class T>
class CPtrList {
  public:
    struct Node : public CPtrListLink {
        /* 0x8 */ T *m_value;
    };

    class iterator {
      public:
        iterator() {
            __csd_list_nothing();
        }
        iterator(const CPtrListPosition &position) : m_node(position.m_node) {
            __csd_list_nothing();
        }
#ifdef CSD_PTRLIST_ITERATOR_COPY
        /* (Only where CSD_PTRLIST_ITERATOR_COPY is defined, Game/csdmSceneObserver.cpp: the copy the
         * postfix increment makes is called out of line there.) */
        iterator(const iterator &other) : m_node(other.m_node) {
            __csd_list_nothing();
        }
#endif

        T *&operator*() const {
            __csd_list_nothing();
            return static_cast<Node *>(m_node)->m_value;
        }
        iterator &operator++() {
            m_node = m_node->m_next;
            __csd_list_nothing();
            return *this;
        }
#ifdef CSD_PTRLIST_ITERATOR_COPY
        iterator operator++(int) {
            iterator old(*this);
            m_node = m_node->m_next;
            return old;
        }
#else
        iterator operator++(int) {
            iterator old(*this);
            ++*this;
            return old;
        }
#endif
        bool operator!=(const iterator &other) const {
            __csd_list_nothing();
            return m_node != other.m_node;
        }

        /* 0x0 */ CPtrListLink *m_node;
    };

    CPtrList() {
        __csd_list_nothing();
        fn_8001BE04(this);
    }
    ~CPtrList() {
        __csd_list_nothing();
        fn_8001B5B8(this, 0);
    }

    iterator begin() {
        __csd_list_nothing();
        return iterator(fn_8001B6B0(this));
    }
    iterator end() {
        __csd_list_nothing();
        return iterator(fn_8001B674(this));
    }
    void remove(T *const &value) {
        __csd_list_nothing();
        fn_8001B42C(this, reinterpret_cast<void *const &>(value));
    }
    void clear() {
        fn_8001E404(this);
    }
    u32 size() {
        return fn_80058F00(this);
    }

    /* 0x0 */ u32 m_size;
    /* 0x4 */ CPtrListLink m_end;
};

/* An object that observers watch (S, of which this is a base, is what they are told about). The
 * observers are told by a pointer to one of their member functions. */
template <class S, class O>
class CSubject {
  public:
    CSubject() {}
    virtual ~CSubject() = 0;
    virtual S *GetSubject() {
        __csd_nothing();
        return 0;
    }

    void Notify(void (O::*func)(S *), S *subject) {
        typename CPtrList<O>::iterator it;
        for (it = m_observers.begin(); it != m_observers.end(); ++it) {
            ((*it)->*func)(subject);
        }
    }
    void Detach(O *observer) {
        observer->RemoveSubject(GetSubject());
        m_observers.remove(observer);
    }
    u32 GetObserverCount() {
        return m_observers.size();
    }

    /* 0x4 */ CPtrList<O> m_observers;
};


/* What observes subjects S. */
template <class S>
class CObserver {
  public:
    CObserver() {}
    virtual ~CObserver() = 0;

    void RemoveSubject(S *subject) {
        m_subjects.remove(subject);
    }
    /* Told that subject is going away. */
    void OnSubjectDestroyed(S *subject);

    /* 0x4 */ CPtrList<S> m_subjects;
};


class CScene;
class CNode;
class CProject;

/* What observes a scene: told before and after it updates and draws, and when its motion reaches
 * its end or its start. */
class CSceneObserver : public CObserver<CScene>, public CBase {
  public:
    CSceneObserver() {}
    virtual ~CSceneObserver();
#ifdef CSD_SCENE_OBSERVER
    /* (Defined in Game/csdmSceneObserver.cpp, which defines CSD_SCENE_OBSERVER.) */
    virtual void OnEnd(CScene *scene);
    virtual void OnStart(CScene *scene);
#else
    virtual void OnEnd(CScene *scene) {}
    virtual void OnStart(CScene *scene) {}
#endif
    virtual void OnUpdate(CScene *scene) {}
    virtual void OnPostUpdate(CScene *scene) {}
    virtual void OnDraw(CScene *scene) {}
    virtual void OnPostDraw(CScene *scene) {}
};

/* The scene's subject and observer base (Game/csdmScene.cpp defines these). */
template <>
CSubject<CScene, CSceneObserver>::~CSubject();
template <>
CObserver<CScene>::~CObserver();
template <>
void CObserver<CScene>::OnSubjectDestroyed(CScene *subject);

/* What a scene does when its motion reaches its end or its start (0 stops, 1 loops, 2 reverses,
 * 3 tells the project). */
class CScenePlayMode : public CSceneObserver {
  public:
    CScenePlayMode() : m_endMode(1), m_startMode(1) {}
    virtual ~CScenePlayMode() {}
    CScenePlayMode &operator=(const CScenePlayMode &other) {
        m_endMode = other.m_endMode;
        m_startMode = other.m_startMode;
        __csd_nothing();
        return *this;
    }
    virtual void OnEnd(CScene *scene);
    virtual void OnStart(CScene *scene);

    /* 0x10 */ s32 m_endMode;
    /* 0x14 */ s32 m_startMode;
};

/* A function a scene calls before or after it updates or draws; returning false removes it. */
class CSceneFunctor {
  public:
    virtual bool operator()(CScene *scene) = 0;
    virtual ~CSceneFunctor() {}
};

/* What observes a node: told when it updates. */
class CNodeObserver : public CObserver<CNode> {
  public:
    virtual ~CNodeObserver();
    virtual void OnUpdate(CNode *node) = 0;
};

/* A function a node calls when it updates; returning false removes it. */
class CNodeFunctor {
  public:
    virtual bool operator()(CNode *node) = 0;
    virtual ~CNodeFunctor();
};

/* A scene's font list (a project's, shared). */
class CFontList : public CResourceBase<SFontList>, public CBase {
  public:
    CFontList(const CResourceBase<SFontList> *other) {
        CopyFrom(*other);
    }
#ifdef CSD_PROJECT
    /* The font list of a project's data (Game/csdmProject.cpp). */
    CFontList(SFontList *fontList, const RCPtr<unsigned char> &data) {
        Set(fontList, data);
    }
#endif
    virtual ~CFontList() {}
};

/* A scene's motion. */
class CMotion : public CResourceBase<SMotion>, public CBase {
  public:
    CMotion(SMotion *motion, const RCPtr<unsigned char> &data) {
        Set(motion, data);
    }
    CMotion(const CResourceBase<SMotion> *other) {
        CopyFrom(*other);
    }
    virtual ~CMotion() {}
};

class CMotionPattern;

/* A scene's run-time state (Game/csdmMotionPalette.cpp). */
class CMotionPalette : public CResourceBase<SSceneState>, public CBase {
  public:
    CMotionPalette(const SSceneInfo *info, void *fontList);
    virtual ~CMotionPalette() {}
    void Rebuild(CScene *scene, CFontList *fontList);
    CMotionPattern *CreatePattern(void *memory, const SNameIndex2 *entry);
};

/* The cast that places a whole scene (Game/csdmMotionPattern.cpp). Its attributes can be
 * overridden one by one; what is not overridden comes from the cast or from its node. */
class CMotionPattern : public CResourceBase<SCast>, public CBase {
  public:
    CMotionPattern(float aspect);
    CMotionPattern(SCast *cast, const RCPtr<unsigned char> &data) {
        Set(cast, data);
    }
    CMotionPattern(const CResourceBase<SCast> *other) {
        CopyFrom(*other);
    }
    virtual ~CMotionPattern() {}
    void Update();
    virtual void CopyFrom(const CResourceBase<SCast> &other);

    u64 GetPosition() const;
    void OverridePosition(float x, float y);
    void OverrideHide(s32 hide);
    void ClearHideOverride();
    s32 GetHide() const;
    float GetRotation() const;
    void OverrideRotation(float rotation);
    u64 GetScale() const;
    u32 GetColor() const;
    void OverrideColor(const u32 &color);
    void ClearColorOverride();
    float GetPattern() const;
    void OverridePattern(float pattern);
    u32 GetCornerColor(s32 corner) const;
    void OverrideCornerColor(s32 corner, const u32 &color);
    void ClearCornerColorOverride(s32 corner);
    s32 GetParam(s32 index) const;
    void OverrideParam(s32 index, s32 value);
    void OverrideX(float x);
    void OverrideY(float y);
    void OverrideScaleX(float x);
    void OverrideScaleY(float y);

    void SetScale(float x, float y) {
        m_resource->m_scaleX = x;
        m_resource->m_scaleY = y;
        m_resource->m_changed &= ~0x10;
        m_resource->m_changed &= ~0x20;
        m_resource->m_changed &= ~0x80000;
        __csd_nothing();
    }
    void SetPosition(float x, float y) {
        m_resource->m_x = x;
        m_resource->m_y = y;
        m_resource->m_changed &= ~0x2;
        m_resource->m_changed &= ~0x4;
        m_resource->m_changed &= ~0x80000;
        __csd_nothing();
    }
    void SetHide(s32 hide) {
        m_resource->m_attrHide = hide;
        m_resource->m_changed &= ~0x1;
        m_resource->m_changed &= ~0x80000;
        __csd_nothing();
    }

    /* 0x0C */ CNode *m_node;
    /* 0x10 */ f32 m_aspect;
    /* 0x14 */ RCPtr<SCast> m_matrices;
};

/* The storage a node places its pattern and font list in. */
struct SNodePool {
    /* 0x00 */ u8 m_pattern[0x18];
    /* 0x18 */ u8 m_fontList[0xC];
};

/* Unknown data a scene and a node keep a counted pointer to. */
struct SUserData;

/* A node of a scene: one cast and what is attached to it (Game/csdmNode.cpp). A node can follow
 * other nodes: each link copies one attribute (0 hide, 1 x, 2 y, 3 rotation, 4 and 5 scale, 6
 * pattern, 7 colour, 8 to 11 corner colours, 12 to 14 parameters) from the linked node when the
 * node updates. */
class CNode : public CResourceBase<SCastInfo>, public CSubject<CNode, CNodeObserver>, public CBase {
  public:
    typedef std::list<RCPtr<CNodeFunctor> > FunctorList;
    typedef std::map<s32, RCPtr<CNode> > LinkMap;

    CNode(SCastInfo *info, const RCPtr<unsigned char> *data, CMotionPalette *palette, const SNameIndex2 *entry,
          CScene *scene, CFontList *fontList);
    virtual ~CNode();
    virtual CNode *GetSubject();

    void SetPosition(float x, float y);
    s32 GetHide() const;
    float GetRotation() const;
    u32 GetColor() const;
    u32 GetCornerColor(s32 corner) const;
    s32 GetParam(s32 index) const;
    void NotifyObservers();
    void SetFontList(const CFontList *fontList);
    void Unused(s32 attr, const RCPtr<CNode> &node);
    void Update();
    void ApplyLink(LinkMap::iterator &link);
    float GetX() const;
    void SetX(float x);
    float GetY() const;
    void SetY(float y);
    float GetScaleX() const;
    float GetScaleY() const;
    void Refresh();
    void DetachScene() {
        m_scene = 0;
    }
    void SetHide(s32 hide) {
        m_pattern->OverrideHide(hide);
    }
    void SetRotation(float rotation) {
        m_pattern->OverrideRotation(rotation);
    }
    void SetScaleX(float scale) {
        m_pattern->OverrideScaleX(scale);
    }
    void SetScaleY(float scale) {
        m_pattern->OverrideScaleY(scale);
    }
    void SetPatternFrame(float frame) {
        m_pattern->OverridePattern(frame);
    }
    /* Takes frame's pattern and fraction (as a link copies them). */
    void CopyPatternFrame(float frame) {
        float frac;
        s32 pattern;
        s32 index;
        SplitPatternFrame(frame, &index, &pattern, &frac);
        m_pattern->OverridePattern(MakePatternFrame(frac, index, pattern));
    }
    void SetColor(u32 color) {
        m_pattern->OverrideColor(color);
    }
    void SetCornerColor(s32 corner, u32 color) {
        m_pattern->OverrideCornerColor(corner, color);
    }
    void SetParam(s32 index, s32 value) {
        m_pattern->OverrideParam(index, value);
    }

    /* 0x1C */ FunctorList m_functors;
    /* 0x28 */ CMotionPattern *m_pattern;
    /* 0x2C */ CFontList *m_fontList;
    /* 0x30 */ const char *m_name;
    /* 0x34 */ CScene *m_scene;
    /* 0x38 */ RCPtr<SUserData> m_userData;
    /* 0x3C */ RCPtr<SUserData> m_userData2;
    /* 0x40 */ LinkMap m_links;
    /* 0x50 */ SNodePool *m_pool;
};

/* The position of a cast in a scene: its layer and its index there. */
struct SCastIndex {
    SCastIndex(s32 layer, s32 cast) : m_layer(layer), m_cast(cast) {
        __csd_nothing();
    }
    SCastIndex(const SCastIndex &other) : m_layer(other.m_layer), m_cast(other.m_cast) {
        __csd_nothing();
    }
    bool operator<(const SCastIndex &other) const {
        if (m_layer == other.m_layer) {
            return m_cast < other.m_cast;
        }
        return m_layer < other.m_layer;
    }

    /* 0x0 */ s32 m_layer;
    /* 0x4 */ s32 m_cast;
};

/* The storage a scene places its font list, run-time state and motion in. */
struct SScenePool {
    /* 0x00 */ u8 m_fontList[0xC];
    /* 0x0C */ u8 m_palette[0xC];
    /* 0x18 */ u8 m_motion[0xC];
};

/* CScene (class name from the RTTI of RCPtr<CScene>::RCObject<CScene> in game code): a scene of a
 * project, its motion and its nodes. */
class CScene : public CResourceBase<SSceneInfo>, public CSubject<CScene, CSceneObserver>, public CBase {
  public:
    typedef std::list<RCPtr<CSceneFunctor> > FunctorList;
    typedef std::map<SCastIndex, RCPtr<CNode> > NodeMap;

    CScene() : m_pool(0) {
        Reset();
    }
    CScene(const CScene &other);
    virtual ~CScene();
    virtual void CopyFrom(const CResourceBase<SSceneInfo> &other);
    virtual void Update(float dt);
    virtual void Render(const RCPtr<CTexList> &texList);
    virtual CScene *GetSubject();

    static void SetFrameTime(float time);

    float GetMotionEndFrame(s32 index);
    float GetMotionStartFrame(s32 index);
    void RebuildState();
    void SetFontList(const CFontList *fontList);
    void Initialize(SSceneInfo *info, const RCPtr<unsigned char> &data, void *owner, CProject *project,
                    const CFontList *fontList);
    float GetFrameCount();
    void Unused(CSceneObserver *observer, CNode *node, CNodeObserver *nodeObserver, const SCastIndex &index);
    s32 GetEndMode();
    void SetPosition(float x, float y);
    float GetAspect();
    CMotionPattern *GetPattern();
    void SetPattern(CMotionPattern *pattern);
    s32 IsParentVisible();
    RCPtr<CNode> GetNode(const SNameIndex2 *entry);
    SNameIndex2 FindNodeEntry(const char *name);
    float GetStartFrame();
    void InitFrames();
    SSceneInfo *GetInfo() {
        __csd_nothing();
        return m_resource;
    }
    void InitState();
    s32 FindMotion(const char *name);
    void SetMotion(const CMotion *motion);
    void ClearMotion();
    void *GetOwner();
    void Reset();
#ifdef CSD_PROJECT
    /* The project that made the scene (Game/csdmProject.cpp). */
    CProject *GetProject() {
        return m_project;
    }
    void SetProject(CProject *project) {
        m_project = project;
    }
#endif

    /* 0x1C */ FunctorList m_onEnd;
    /* 0x28 */ FunctorList m_onStart;
    /* 0x34 */ FunctorList m_onUpdate;
    /* 0x40 */ FunctorList m_onPostUpdate;
    /* 0x4C */ FunctorList m_onDraw;
    /* 0x58 */ FunctorList m_onPostDraw;
    /* 0x64 */ CMotion *m_motion;
    /* 0x68 */ CFontList *m_fontList;
    /* 0x6C */ CMotionPalette *m_palette;
    /* 0x70 */ f32 m_prevFrame;
    /* 0x74 */ f32 m_frame;
    /* 0x78 */ f32 m_speed;
    /* 0x7C */ f32 m_startFrame;
    /* 0x80 */ f32 m_endFrame;
    /* 0x84 */ RCPtr<CNode> m_parent;
    /* 0x88 */ CMotionPattern *m_pattern;
    /* 0x8C */ s32 m_end;
    /* 0x90 */ CScenePlayMode m_playMode;
    /* 0xA8 */ s32 m_motionIndex;
    /* 0xAC */ f32 m_frameCount;
    /* 0xB0 */ NodeMap m_nodes;
    /* 0xC0 */ void *m_owner;
    /* 0xC4 */ RCPtr<SUserData> m_userData;
    /* 0xC8 */ CProject *m_project;
    /* 0xCC */ SScenePool *m_pool;
};


/* CScene's functions that game code calls by these names (Game/csdmScene.cpp). */
extern "C" {
void fn_803659CC(CScene *scene, s32 mode);
void fn_803659DC(CScene *scene, s32 mode);
void fn_803659E4(CScene *scene);
void fn_803659F0(CScene *scene);
void fn_803659FC(CScene *scene, float x, float y);
void fn_80365AC8(CScene *scene, float x, float y);
void fn_80365B18(CScene *scene, const SVec2 &position);
void fn_80365B40(CScene *scene, s32 hide);
void fn_803663C0(CScene *scene, const RCPtr<CNode> &parent);
RCPtr<CNode> fn_803666B4(CScene *scene, const char *name);
float fn_80366728(CScene *scene);
float fn_80366738(CScene *scene);
void fn_80366740(CScene *scene, float frame);
void fn_80366F9C(CScene *scene);
float fn_80367138(CScene *scene);
void fn_80367140(CScene *scene, float speed);
void fn_80367238(CScene *scene, const char *name);
void fn_80367274(CScene *scene, s32 index);
float fn_80364430(CScene *scene, const char *name);
float fn_803644A0(CScene *scene, const char *name);
}

/* CNode's functions that game code calls by these names (Game/csdmNode.cpp). */
extern "C" {
void fn_8036A278(CNode *node, float scale);
void fn_8036A2CC(CNode *node, float scale);
void fn_8036A8BC(CNode *node);
void fn_8036AC3C(CNode *node, s32 corner);
void fn_8036AC60(CNode *node, s32 corner, const u32 *color);
void fn_8036ACA8(CNode *node);
void fn_8036ACCC(CNode *node, const u32 *color);
void fn_8036AD14(CNode *node, s32 pattern);
void fn_8036AD78(CNode *node, s32 hide);
void fn_8036AE70(CNode *node, const SVec2 *position);
SVec2 fn_8036AE98(const CNode *node);
void fn_8036AEE0(CNode *node, s32 value);
}

} // namespace CSD
} // namespace Chao

#endif
