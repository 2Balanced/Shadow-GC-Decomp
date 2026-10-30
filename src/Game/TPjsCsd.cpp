/* TPjsCsd (class name from RTTI): a CSD project a task draws, with a queue of motion requests for
 * its scene. The unit also has TPjsSceneTour (class name from the RTTI of its shared_ptr's control
 * block), a scene with a queue of motions to play, and the set of named scenes the front end keeps
 * (UnkCsdSceneSet_8003D5D8, class name unknown). Names other than the class names are inferred;
 * the functions other units call keep their fn_ names. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first); the MSL vector, tree and string template
 * members the game emits after the unit's code come from <vector_impl>, <map_impl> and <string>
 * (MSL_STRING_REPLACE). The unit also emits the game's copies of CMemoryAlloc::Alloc and Free, of the
 * RCPtr members for CScene, CNode, CTexList and unsigned char, of CProject's constructor and of the
 * string copy constructor, assign, replace and compare.
 *
 * The unit stays NonMatching (249 of 255 functions match):
 * - fn_8003FAC4: retail loads csd.m_project above the prologue's stores (the later 2.7-era compiler
 *   of docs/research-notes.md, fourth difference).
 * - fn_8003CEF8: retail computes the scene comparison into a register before the GetScene
 *   temporary's destructor; GC/2.7 compares after it (one more saved register).
 * - allocator<Motion>::destroy: retail calls Motion's destructor out of line, GC/2.7 inlines it.
 * - list<SReorder>::erase (and its operator-> helper): retail calls operator-> out of line for this
 *   trivially destructible list only; <list>'s MSL_LIST_TRIVIAL_VALUES fixes it but breaks the
 *   list<RCPtr<CScene> > copy in the same unit.
 * - basic_string's copy constructor: the allocator and tag temporaries swap stack slots.
 * - Order: CResourceBase<SProject>::CopyFrom, string::data() const and the template members at the
 *   end of the unit (vector, list, tree and string) come out in a different order; the RTTI name
 *   string of Chao::CSD::CBase follows RCObject<unsigned char>'s instead of preceding it. */
#define CSD_OBJECT_LAYER
#define CSD_MEMORY_ALLOC_INLINE
#define CSD_FUNCTOR_DTOR_DECL_ONLY
#define MSL_STRING_DEFAULT_CTOR
#define MSL_STRING_COPY
#define MSL_STRING_APPEND
#define MSL_STRING_REPLACE
#define MSL_VECTOR_MIDDLE
#define MSL_VECTOR_DESTROY_ALL_LATE
#define MSL_VECTOR_DESTROY_ALL_OUTLINE
#define MSL_VECTOR_BACK
#define MSL_VECTOR_ERASE_ONE
#define MSL_ALGORITHM_COPY
#define MSL_MULTIMAP
#define MSL_TREE_END_NODE_PER_TREE
#define TPJS_SCENE_TOUR_IMPL
#define TPJS_CSD_IMPL
#include <string>
#include <vector>

#include "Game/CsdProject.h"
#include "Game/CsdSceneCtrl.h"
#include "Game/CsdSceneSet.h"
#include "Game/PjsCsd.h"
#include "boost/shared_ptr.hpp"

using Chao::CSD::CMemoryAlloc;
using Chao::CSD::CNode;
using Chao::CSD::CProject;
using Chao::CSD::CScene;
using Chao::CSD::CSceneList;
using Chao::CSD::CTexList;
using Chao::CSD::RCPtr;

namespace Sonicteam {
namespace System {

/* The singleton holder (class names from RTTI; as Game/EffectManagerPJS.h declares them). */
template <class T> class CreateStatic {};
template <class T, class Creator> class Singleton {
  public:
    /* The instance (name inferred). */
    static T *ms_instance;
};

template <class T, class Creator> T *Singleton<T, Creator>::ms_instance;

} // namespace System
} // namespace Sonicteam

/* The game's setup of the CSD library (Game/TCsdSystem.cpp). */
class TCsdSystem : public Sonicteam::System::Singleton<TCsdSystem, Sonicteam::System::CreateStatic<TCsdSystem> >,
                   public CMemoryAlloc {
  public:
    TCsdSystem();
    virtual ~TCsdSystem();
    virtual void *Allocate(u32 size);
    virtual void Deallocate(void *p);
};

extern "C" {
/* Game heap: free and allocate. */
void fn_80049B34(void *block);
void *fn_80049B90(u32 size, int heap);
/* The project file name of name (Game/TCsdSystem.cpp). */
const char *fn_800423B8(TCsdSystem *system, const char *name, int arg2);
/* Loads a project's textures from a texture archive file (Game/TCsdSystem.cpp). */
bool fn_800422A8(CProject *project, const char *file);
/* Projects (Game/csdmProject.cpp): loading from data and from a file, the scene of a name, the
 * scene of a name set to a motion (by name and by index), advancing and drawing. */
int fn_8036DB70(CProject *project, void *data, u32 size, int arg3);
int fn_8036E114(CProject *project, const char *file);
RCPtr<CScene> fn_8036D660(CProject *project, const char *name);
RCPtr<CScene> fn_8036D7D4(CProject *project, const char *name, const char *motion, int arg4);
RCPtr<CScene> fn_8036D890(CProject *project, const char *name, s32 motion, int arg4);
void fn_8036DAD4(CProject *project);
void fn_8036D94C(CProject *project);
/* The scene of list named name (Game/csdmProject.cpp). */
RCPtr<CScene> fn_8036F470(CSceneList *list, const char *name);
}

typedef UnkCsdSceneSetEntry_8003D5D8 UnkCsdSceneSetEntry;
typedef TPjsSceneTour::Motion TPjsSceneTourMotion;

extern "C" {
TCsdSystem *fn_8003F8D4(void);
bool fn_8003E750(TPjsCsd &csd, void *data, u32 size);
void fn_8003E490(TPjsCsd &csd);
void fn_8003E414(TPjsCsd &csd, s32 index, bool loop);
void fn_8003E388(TPjsCsd &csd, const char *motion, bool loop);
bool fn_8003F938(TPjsCsd &csd, const char *name, const char *textures);
bool fn_8003FA08(TPjsCsd &csd, const char *name);
RCPtr<CScene> fn_8003FAC4(TPjsCsd &csd);
bool fn_8003FB10(TPjsCsd &csd);
void fn_8003FDF4(TPjsCsd &csd);
bool fn_8003FE2C(TPjsCsd &csd);
void fn_8003D6E8(TPjsSceneTour *tour, const char *motion, int mode);
bool fn_8003D7D8(TPjsSceneTour *tour);
bool fn_8003D870(TPjsSceneTour *tour);
void fn_8003D514(UnkCsdSceneSet_8003D5D8 *set);
void fn_8003D4D4(UnkCsdSceneSet_8003D5D8 *set);
bool fn_8003D080(UnkCsdSceneSet_8003D5D8 *set, TPjsSceneTour *tour, const char *name);
UnkCsdSceneSetEntry *fn_8003C79C(UnkCsdSceneSet_8003D5D8 *set, const char *name);
}

/* The handles the game uses where it has no texture list, scene list or scene
 * (Game/TCsdSystem.cpp). */
extern RCPtr<CTexList> lbl_805EE110;
extern UnkCsdHandle_80042DF0 lbl_805EE114;
extern RCPtr<CScene> lbl_805EE118;
extern RCPtr<CNode> lbl_805EE11C;

/* Whether two handles hold the same object (or both none). */
template <class T> inline int operator==(const RCPtr<T> &a, const RCPtr<T> &b) {
    if (a.m_object != 0 && b.m_object != 0) {
        return a.m_object->m_object == b.m_object->m_object;
    }
    return a.m_object == b.m_object;
}

/* Whether two handles hold different objects. */
template <class T> inline bool operator!=(const RCPtr<T> &a, const RCPtr<T> &b) {
    T *pa = a.m_object != 0 ? a.m_object->m_object : 0;
    T *pb = b.m_object != 0 ? b.m_object->m_object : 0;
    return pa != pb;
}

inline int operator==(const UnkCsdHandle_80042DF0 &a, const UnkCsdHandle_80042DF0 &b) {
    if (a.m_object != 0 && b.m_object != 0) {
        return a.m_object->m_object == b.m_object->m_object;
    }
    return a.m_object == b.m_object;
}

/* A scene as game code calls it (Game/CsdRCPtr.h's view without CSD_OBJECT_LAYER): the virtual
 * function at 0x24 hands it a project's texture list. */
class UnkCsdSceneView {
  public:
    virtual ~UnkCsdSceneView();
    virtual void vf_0x0C();
    virtual void vf_0x10();
    virtual void vf_0x14();
    virtual void vf_0x18();
    virtual void vf_0x1C();
    virtual void vf_0x20();
    virtual void SetTexList(const void *texList);
};

/* The draw switch of the projects (meaning inferred). */
u8 lbl_805E3780 = 1;

typedef Sonicteam::System::Singleton<TCsdSystem, Sonicteam::System::CreateStatic<TCsdSystem> > TCsdSystemSingleton;

TPjsCsd::TPjsCsd() {
    m_project = 0;
}

TPjsCsd::~TPjsCsd() {
    fn_8003E490(*this);
}

/* Advances the scene; whether a project is loaded (fn_8003FE2C). */
extern "C" bool fn_8003FE2C(TPjsCsd &csd) {
    if (csd.m_project != 0) {
        fn_8036DAD4(csd.m_project);
        RCPtr<CScene> scene(fn_8003FAC4(csd));
        if (scene == lbl_805EE118) {
            csd.m_playing = false;
            return false;
        }
        CScene *s = scene.Get();
        float frame = fn_80366728(scene.Get());
        if (fn_80366738(s) >= frame) {
            if (csd.m_requests != 0) {
                fn_8003FB10(csd);
                csd.m_playing = true;
            } else {
                csd.m_playing = false;
            }
        } else {
            csd.m_playing = true;
        }
        return true;
    }
    return false;
}

/* Draws the project (fn_8003FDF4). */
extern "C" void fn_8003FDF4(TPjsCsd &csd) {
    if (lbl_805E3780 && csd.m_project != 0) {
        fn_8036D94C(csd.m_project);
    }
}

/* Starts the first queued request on the scene (fn_8003FB10). */
extern "C" bool fn_8003FB10(TPjsCsd &csd) {
    if (csd.m_requests == 0) {
        return false;
    }
    RCPtr<CScene> scene(fn_8003FAC4(csd));
    TPjsCsdRequest *request = csd.m_requests;
    if (request->m_index >= 0) {
        if (scene != lbl_805EE118) {
            fn_80367274(scene.Get(), request->m_index);
        } else {
            scene = fn_8036D890(csd.m_project, csd.m_sceneName, request->m_index, 0);
        }
    } else {
        if (scene != lbl_805EE118) {
            fn_80367238(scene.Get(), request->m_motion);
        } else {
            scene = fn_8036D7D4(csd.m_project, csd.m_sceneName, request->m_motion, 0);
        }
    }
    if (scene != lbl_805EE118) {
        if (csd.m_requests->m_loop) {
            if (scene->GetEndMode() != 0) {
                fn_803659DC(scene.Get(), 0);
            }
        } else {
            if (scene->GetEndMode() != 1) {
                fn_803659DC(scene.Get(), 1);
            }
        }
        fn_80366740(scene.Get(), 0.0f);
        fn_803659E4(scene.Get());
        TPjsCsdRequest *first = csd.m_requests;
        if (first != 0) {
            TPjsCsdRequest *next = first->m_next;
            fn_80049B34(first);
            csd.m_requests = next;
        }
    }
    return true;
}

/* The scene (fn_8003FAC4). */
extern "C" RCPtr<CScene> fn_8003FAC4(TPjsCsd &csd) {
    CProject *project = csd.m_project;
    if (project != 0) {
        return fn_8036D660(project, csd.m_sceneName);
    }
    return lbl_805EE118;
}

/* Loads the project file of name (fn_8003FA08). */
extern "C" bool fn_8003FA08(TPjsCsd &csd, const char *name) {
    if (csd.m_project != 0) {
        return false;
    }
    csd.m_project = new CProject;
    if (fn_8036E114(csd.m_project, fn_800423B8(fn_8003F8D4(), name, 0)) == 0) {
        fn_8003E490(csd);
        return false;
    }
    csd.m_playing = false;
    csd.m_requests = 0;
    return true;
}

/* Loads the project file of name and its textures from textures (fn_8003F938). */
extern "C" bool fn_8003F938(TPjsCsd &csd, const char *name, const char *textures) {
    if (csd.m_project != 0) {
        return false;
    }
    csd.m_project = new CProject;
    if (fn_8036E114(csd.m_project, fn_800423B8(fn_8003F8D4(), name, 0)) == 0) {
        fn_8003E490(csd);
        return false;
    }
    fn_8003F8D4();
    fn_800422A8(csd.m_project, textures);
    csd.m_playing = false;
    csd.m_requests = 0;
    return true;
}

/* The TCsdSystem singleton (fn_8003F8D4). */
extern "C" TCsdSystem *fn_8003F8D4(void) {
    if (TCsdSystemSingleton::ms_instance == 0) {
        static TCsdSystem s_instance;
        TCsdSystemSingleton::ms_instance = &s_instance;
    }
    return TCsdSystemSingleton::ms_instance;
}

/* Loads the project from data (fn_8003E750). */
extern "C" bool fn_8003E750(TPjsCsd &csd, void *data, u32 size) {
    if (csd.m_project != 0) {
        return false;
    }
    csd.m_project = new CProject;
    if (fn_8036DB70(csd.m_project, data, size, 0) == 0) {
        fn_8003E490(csd);
        return false;
    }
    csd.m_playing = false;
    csd.m_requests = 0;
    return true;
}

/* Frees the project and the queued requests (fn_8003E490). */
extern "C" void fn_8003E490(TPjsCsd &csd) {
    if (csd.m_project == 0) {
        return;
    }
    while (csd.m_requests != 0) {
        TPjsCsdRequest *next = csd.m_requests->m_next;
        fn_80049B34(csd.m_requests);
        csd.m_requests = next;
    }
    csd.m_texList = lbl_805EE110;
    delete csd.m_project;
    csd.m_project = 0;
}

/* Queues motion index of the scene (fn_8003E414). */
extern "C" void fn_8003E414(TPjsCsd &csd, s32 index, bool loop) {
    TPjsCsdRequest *request = (TPjsCsdRequest *)fn_80049B90(sizeof(TPjsCsdRequest), 2);
    request->m_next = 0;
    request->m_index = index;
    request->m_loop = loop;
    request->m_motion[0] = 0;
    TPjsCsdRequest *last = csd.m_requests;
    if (last == 0) {
        csd.m_requests = request;
    } else {
        while (last->m_next != 0) {
            last = last->m_next;
        }
        last->m_next = request;
    }
}

/* Queues the motion named motion of the scene (fn_8003E388). */
extern "C" void fn_8003E388(TPjsCsd &csd, const char *motion, bool loop) {
    TPjsCsdRequest *request = (TPjsCsdRequest *)fn_80049B90(sizeof(TPjsCsdRequest), 2);
    request->m_next = 0;
    request->m_index = -1;
    request->m_loop = loop;
    strcpy(request->m_motion, motion);
    TPjsCsdRequest *last = csd.m_requests;
    if (last == 0) {
        csd.m_requests = request;
    } else {
        while (last->m_next != 0) {
            last = last->m_next;
        }
        last->m_next = request;
    }
}

/* (The linker dropped these two functions, which used a node handle; their out-of-line copies of
 * RCPtr<CNode>'s members are what remains. Names and bodies are inferred.) */
void DroppedNodeTemp(TPjsSceneTour *tour, const char *name) {
    fn_803666B4(tour->m_scene.Get(), name);
}

RCPtr<CNode> DroppedNodeCopy() {
    return lbl_805EE11C;
}

TPjsSceneTour::TPjsSceneTour() {}

/* A tour of scene (fn_8003DC00). */
TPjsSceneTour::TPjsSceneTour(const RCPtr<CScene> &scene) : m_scene(scene) {
    m_ended = true;
}

/* Whether the queue is empty and the motion has ended (fn_8003DBB0). */
extern "C" bool fn_8003DBB0(TPjsSceneTour *tour) {
    if (tour->m_queue.empty() && fn_8003D7D8(tour)) {
        return true;
    }
    return false;
}

void TPjsSceneTour::Play(const char *motion, int mode) {
    TPjsSceneTourMotion entry;
    entry.m_name = motion;
    entry.m_mode = mode;
    m_queue.push_back(entry);
}

void TPjsSceneTour::Reset() {
    m_queue.clear();
    CScene *scene = m_scene.Get();
    fn_80366740(scene, fn_80366728(m_scene.Get()));
    fn_803659F0(m_scene.Get());
    m_ended = true;
}

bool TPjsSceneTour::Update(bool loop) {
    if (fn_8003D7D8(this)) {
        if (m_queue.empty()) {
            if (loop) {
                fn_80366F9C(m_scene.Get());
            }
            return false;
        }
        fn_8003D870(this);
    }
    fn_80366F9C(m_scene.Get());
    return true;
}

/* Plays the first queued motion; false when there is none (fn_8003D870). */
extern "C" bool fn_8003D870(TPjsSceneTour *tour) {
    if (tour->m_queue.empty()) {
        return false;
    }
    TPjsSceneTour::MotionQueue::iterator it = tour->m_queue.begin();
    fn_8003D6E8(tour, it->m_name.c_str(), it->m_mode);
    tour->m_queue.erase(it);
    return true;
}

/* Whether the motion has ended (fn_8003D7D8). */
extern "C" bool fn_8003D7D8(TPjsSceneTour *tour) {
    if (tour->m_ended) {
        return true;
    }
    CScene *scene = tour->m_scene.Get();
    return fn_80366738(scene) >= fn_80366728(tour->m_scene.Get()) - 0.001f;
}

/* Starts motion on the scene; mode 1 plays it once, 2 loops it (fn_8003D6E8). */
extern "C" void fn_8003D6E8(TPjsSceneTour *tour, const char *motion, int mode) {
    fn_80367238(tour->m_scene.Get(), motion);
    fn_80366740(tour->m_scene.Get(), 0.0f);
    fn_803659E4(tour->m_scene.Get());
    switch (mode) {
    case 1:
        fn_803659DC(tour->m_scene.Get(), 0);
        tour->m_ended = false;
        break;
    case 2:
        fn_803659DC(tour->m_scene.Get(), 1);
        tour->m_ended = false;
        break;
    }
}

UnkCsdSceneSet_8003D5D8::UnkCsdSceneSet_8003D5D8() {
    fn_8003D514(this);
}

UnkCsdSceneSet_8003D5D8::~UnkCsdSceneSet_8003D5D8() {
    fn_8003D4D4(this);
}

/* Removes every scene (fn_8003D514). */
extern "C" void fn_8003D514(UnkCsdSceneSet_8003D5D8 *set) {
    set->m_entries.clear();
}

/* Removes every scene (fn_8003D4D4). */
extern "C" void fn_8003D4D4(UnkCsdSceneSet_8003D5D8 *set) {
    set->m_entries.clear();
}

/* Adds tour as the scene named name (fn_8003D080). */
extern "C" bool fn_8003D080(UnkCsdSceneSet_8003D5D8 *set, TPjsSceneTour *tour, const char *name) {
    if (tour == 0) {
        return false;
    }
    UnkCsdSceneSetEntry entry;
    entry.m_tour = boost::shared_ptr<TPjsSceneTour>(tour);
    entry.m_name = name;
    entry.m_visible = false;
    set->m_entries.push_back(entry);
    return true;
}

/* Adds the scenes of list named names (fn_8003CEF8); false when one is missing. */
extern "C" bool fn_8003CEF8(UnkCsdSceneSet_8003D5D8 *set, UnkCsdHandle_80042DF0 list, const char *const *names,
                            int count) {
    if (list == lbl_805EE114) {
        return false;
    }
    bool ok = true;
    for (int i = 0; i < count; i++) {
        TPjsSceneTour *tour = new TPjsSceneTour(fn_8036F470(list.Get(), names[i]));
        bool valid = tour->GetScene() != lbl_805EE118;
        if (valid) {
            ok = ok & fn_8003D080(set, tour, names[i]);
        } else {
            ok = false;
        }
    }
    return ok;
}

/* The scene named name, or 0 (fn_8003CEAC). */
extern "C" TPjsSceneTour *fn_8003CEAC(UnkCsdSceneSet_8003D5D8 *set, const char *name) {
    UnkCsdSceneSetEntry *entry = fn_8003C79C(set, name);
    if (entry != set->m_entries.end()) {
        return entry->m_tour.get();
    }
    return 0;
}

/* Rewinds every scene (fn_8003CE5C). */
extern "C" void fn_8003CE5C(UnkCsdSceneSet_8003D5D8 *set) {
    for (UnkCsdSceneSetEntry *it = set->m_entries.begin(); it != set->m_entries.end(); ++it) {
        it->m_tour->Reset();
    }
}

/* Plays every scene's queued motions; whether any is still busy (fn_8003CDF4). */
extern "C" bool fn_8003CDF4(UnkCsdSceneSet_8003D5D8 *set) {
    int busy = 0;
    for (UnkCsdSceneSetEntry *it = set->m_entries.begin(); it != set->m_entries.end(); ++it) {
        busy += it->m_tour->Update(true);
    }
    return busy != 0;
}

/* Hands texList to the shown scenes (fn_8003C92C). */
extern "C" bool fn_8003C92C(UnkCsdSceneSet_8003D5D8 *set, const RCPtr<CTexList> &texList) {
    for (UnkCsdSceneSetEntry *it = set->m_entries.begin(); it != set->m_entries.end(); ++it) {
        if (it->m_visible) {
            ((UnkCsdSceneView *)it->m_tour->GetScene().Get())->SetTexList(&texList);
        }
    }
    return true;
}

/* Shows or hides the scene named name (fn_8003C8D8). */
extern "C" bool fn_8003C8D8(UnkCsdSceneSet_8003D5D8 *set, const char *name, bool visible) {
    UnkCsdSceneSetEntry *entry = fn_8003C79C(set, name);
    if (entry != set->m_entries.end()) {
        entry->m_visible = visible;
        return true;
    }
    return false;
}

/* The entry of the scene named name, or the end (fn_8003C79C). */
extern "C" UnkCsdSceneSetEntry *fn_8003C79C(UnkCsdSceneSet_8003D5D8 *set, const char *name) {
    UnkCsdSceneSetEntry *it;
    for (it = set->m_entries.begin(); it != set->m_entries.end(); ++it) {
        if (it->m_name == name) {
            return it;
        }
    }
    return it;
}

/* The template members the game emits after the unit's own code. */
#include <vector_impl>
#include <map_impl>
