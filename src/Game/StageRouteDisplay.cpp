/* StageRouteDisplay (class name from RTTI; see Game/StageSequence.h): the stage that shows the
 * story's route before a stage of the first story (stage ids below 700 that are a multiple of 100
 * plus 0 to 9). It loads the CSD project "CsdFiles/RouteDisplay", hides every stage picture and
 * route line, then shows the pictures of the stages the save data's route has passed, the lines
 * between them and the cursor on the current stage, plays the scenes IconsBG, Pics and Arrows and
 * fades out once all three have ended. The file name comes from the class name; the names marked
 * "(name inferred)" are not original. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <cstdio>

#include "Game/CsdRCPtr.h"
#include "Game/Render.h"
#include "Game/ScreenFade.h"
#include "Game/StageSequence.h"
#include "Game/StageTable.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Chao::CSD::CNode;
using Chao::CSD::CScene;
using Chao::CSD::RCPtr;

/* RCPtr's operator-> (as in Game/TGi.h; out of line where its result feeds an inline call, as in
 * scene->IsEnd()). */
namespace Chao {
namespace CSD {
template <class T> inline T *RCPtr<T>::operator->() const {
    return Get();
}
} // namespace CSD
} // namespace Chao

/* The loaded CSD project (class name unknown; declared as in Game/TGi.h). */
class UnkCsdResource_8003E808 {
  public:
    UnkCsdResource_8003E808();
    virtual ~UnkCsdResource_8003E808();

    /* 0x04 */ u8 field_0x4[0x4C - 0x4];

    static void *operator new(u32 size) {
        return fn_803605AC()->Alloc(size, "csdBase.h", 40);
    }
};

/* The value a project returns from fn_8036D598 (class name unknown; declared as in Game/TGi.h). */
class UnkCsdHandle_80042DF0 {
  public:
    UnkCsdHandle_80042DF0(const UnkCsdHandle_80042DF0 &other);
    ~UnkCsdHandle_80042DF0();

    /* 0x0 */ void *m_object;
};

/* A node's position (struct name unknown; see Game/AdvBattleMode.cpp). */
struct UnkCsdPosition_8036AE98 {
    float x;
    float y;
};

/* The game state (struct name unknown; see Game/AdvMemoryCheck.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x18];
    /* 0x018 */ float m_frameTime; /* the frame's duration (meaning inferred) */
    /* 0x01C */ u8 field_0x1C[0x150 - 0x1C];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The route through the story the save data records (see Game/StageSequencePlayStory.cpp). */
struct UnkStoryRoute_800BB74C {
    /* 0x00 */ s32 m_length; /* the stages played before the current one (name inferred) */
    /* 0x04 */ s32 m_stages[0x18]; /* their stage table indexes (name inferred) */
};

/* The save data fn_800BB74C returns (class unknown; see Game/StageSequencePlayStory.cpp). */
struct UnkSaveData_800BB74C {
    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ UnkStoryRoute_800BB74C m_route;
};

extern "C" {
UnkSaveData_800BB74C *fn_800BB74C(void);
/* Silences the stage's sounds (behaviour inferred; see Game/StageMovie.cpp). */
void fn_802BEB90(void);
/* Fades the screen in (Game/PjsEventActionManager.cpp; argument meanings inferred). */
void fn_80173834(int arg0, float time);
/* Creates a Render (behaviour inferred; see Game/StageMovie.cpp). */
boost::shared_ptr<Render> fn_8004E24C(void *arg0);
/* The resource manager, a file's path and its language-specific path (behaviour inferred). */
void *fn_8003F8D4(void);
const char *fn_800C02CC(const char *name);
const char *fn_800423B8(void *arg0, const char *name, int arg2);
/* CSD (behaviour inferred; see Game/TGi.h and Game/AdvCsdCache.cpp): loading a project's texture
 * archive and the project, advancing and drawing it, a scene by name, the scene calls, a node by
 * name, hiding a node, and a node's position. */
void fn_800422A8(UnkCsdResource_8003E808 *project, const char *file);
void fn_8036E114(UnkCsdResource_8003E808 *project, const char *file);
void fn_8036DAD4(UnkCsdResource_8003E808 *project);
void fn_8036D94C(UnkCsdResource_8003E808 *project);
UnkCsdHandle_80042DF0 fn_8036D598(UnkCsdResource_8003E808 *project, int arg1);
RCPtr<CScene> fn_8036D660(UnkCsdResource_8003E808 *project, const char *name);
void fn_80367238(CScene *scene, const char *motion);
void fn_80367140(CScene *scene, float speed);
void fn_803659DC(CScene *scene, int arg1);
RCPtr<CNode> fn_803666B4(CScene *scene, const char *name);
void fn_8036AD78(CNode *node, int hidden);
UnkCsdPosition_8036AE98 fn_8036AE98(CNode *node);
void fn_8036AE70(CNode *node, const UnkCsdPosition_8036AE98 *position);
}

/* The path of a file (name inferred; the lookup ignores the manager it is called on). */
inline const char *FilePath(void *manager, const char *name) {
    return fn_800C02CC(name);
}

/* The stage table entry at index (name inferred; the lookup ignores the object it is called on). */
inline const UnkStageInfo_804C4A18 *StageEntry(int index) {
    fn_800B74B4();
    return &lbl_804C4A18[index];
}

SIZE_ASSERT(StageRouteDisplay, 0x1C);

/* Not called anywhere: the linker strips it from the game. This unit's .rodata is kept whole (the
 * constructor addresses it through one base), so retail keeps the RTTI names of the CSD and boost
 * headers' inline classes the original instantiated here even though their records are stripped.
 * Compiled last (deferred inlining compiles the first definition last), this function creates them
 * in retail's order, as Game/AdvOption.cpp does. */
void PoolRouteDisplayRttiNames(const char **names) {
    names[0] = "Chao::CSD::RCPtr<unsigned char>::RCObject<unsigned char>::CFunctorBase";
    names[1] = "Chao::CSD::CBase";
    names[2] = "Chao::CSD::RCPtr<unsigned char>::RCObject<unsigned char>";
    names[3] = "Chao::CSD::CResourceBase<Chao::CSD::Project>";
    names[4] = "Chao::CSD::RCPtr<Chao::CSD::CTexList>::RCObject<Chao::CSD::CTexList>::CFunctorBase";
    names[5] = "Chao::CSD::RCPtr<Chao::CSD::CTexList>::RCObject<Chao::CSD::CTexList>";
    names[6] = "Chao::CSD::RCPtr<Chao::CSD::CSceneList>::RCObject<Chao::CSD::CSceneList>::CFunctorBase";
    names[7] = "Chao::CSD::RCPtr<Chao::CSD::CSceneList>::RCObject<Chao::CSD::CSceneList>";
    names[8] = "boost::detail::sp_counted_base";
    names[9] = "Chao::CSD::RCPtr<Chao::CSD::CScene>::RCObject<Chao::CSD::CScene>::CFunctorBase";
    names[10] = "Chao::CSD::RCPtr<Chao::CSD::CScene>::RCObject<Chao::CSD::CScene>";
    names[11] = "Chao::CSD::RCPtr<Chao::CSD::CNode>::RCObject<Chao::CSD::CNode>::CFunctorBase";
    names[12] = "Chao::CSD::RCPtr<Chao::CSD::CNode>::RCObject<Chao::CSD::CNode>";
}

/* NonMatching (6 instructions): retail gives the render pointer temporary the stack slot above the
 * node position's and GC/2.7 the one below; the shapes tried for the position (a named local, a
 * temporary, a reference, a nested block) do not change it. */
StageRouteDisplay::StageRouteDisplay(int stageId) : StageBase::Base(0), m_project(0) {
    if (stageId >= 700) {
        return;
    }
    stageId = (stageId % 100) / 10;
    if (stageId != 0) {
        return;
    }
    m_render = fn_8004E24C(fn_80046178()->field_0x54);
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&StageRouteDisplay::Draw, this, _1));
    m_project = new UnkCsdResource_8003E808;
    fn_800422A8(m_project, FilePath(fn_8003F8D4(), "CsdFiles/RouteDisplay"));
    fn_8036E114(m_project, fn_800423B8(fn_8003F8D4(), "CsdFiles/RouteDisplay", 0));
    fn_8036D598(m_project, 0);
    RCPtr<CScene> scene;
    scene = fn_8036D660(m_project, "IconsBG");
    fn_80367238(scene.Get(), "Open");
    fn_803659DC(scene.Get(), 0);
    fn_80367140(scene.Get(), 0.0f);
    scene = fn_8036D660(m_project, "Pics");
    fn_80367238(scene.Get(), "Open");
    fn_803659DC(scene.Get(), 0);
    fn_80367140(scene.Get(), 0.0f);
    scene = fn_8036D660(m_project, "Arrows");
    fn_80367238(scene.Get(), "Open");
    fn_803659DC(scene.Get(), 0);
    fn_80367140(scene.Get(), 0.0f);
    const char *pics[22] = {"0100", "0200", "0201", "0202", "0300", "0301", "0302", "0400",
                            "0401", "0402", "0403", "0404", "0500", "0501", "0502", "0503",
                            "0504", "0600", "0601", "0602", "0603", "0604"};
    const char *lines[45] = {
        "line100-200", "line100-201", "line100-202", "line200-300", "line200-301", "line201-300",
        "line201-301", "line201-302", "line202-301", "line202-302", "line300-400", "line300-401",
        "line300-402", "line301-401", "line301-402", "line301-403", "line302-402", "line302-403",
        "line302-404", "line400-500", "line400-501", "line401-500", "line401-501", "line401-502",
        "line402-501", "line402-502", "line402-503", "line403-502", "line403-503", "line403-504",
        "line404-503", "line404-504", "line500-600", "line500-601", "line501-600", "line501-601",
        "line501-602", "line502-601", "line502-602", "line502-603", "line503-602", "line503-603",
        "line503-604", "line504-603", "line504-604"};
    RCPtr<CNode> node;
    char name[0x10];
    scene = fn_8036D660(m_project, "Pics");
    for (int i = 0; i < 22; i++) {
        sprintf(name, "pic%s", pics[i]);
        node = fn_803666B4(scene.Get(), name);
        fn_8036AD78(node.Get(), 1);
    }
    node = fn_803666B4(scene.Get(), "cursor");
    fn_8036AD78(node.Get(), 1);
    scene = fn_8036D660(m_project, "Arrows");
    for (int i = 0; i < 45; i++) {
        node = fn_803666B4(scene.Get(), lines[i]);
        fn_8036AD78(node.Get(), 1);
    }
    int i = 0;
    int id;
    int prev = 0;
    UnkSaveData_800BB74C *save = fn_800BB74C();
    while (i <= save->m_route.m_length) {
        id = StageEntry(save->m_route.m_stages[i])->m_id;
        scene = fn_8036D660(m_project, "Pics");
        sprintf(name, "pic%04d", id);
        node = fn_803666B4(scene.Get(), name);
        if (node.Get() == 0) {
            i++;
            continue;
        }
        if (i < save->m_route.m_length) {
            fn_8036AD78(node.Get(), 0);
        } else {
            RCPtr<CNode> cursor = fn_803666B4(scene.Get(), "cursor");
            UnkCsdPosition_8036AE98 position = fn_8036AE98(node.Get());
            fn_8036AE70(cursor.Get(), &position);
        }
        if (i != 0) {
            scene = fn_8036D660(m_project, "Arrows");
            sprintf(name, "line%03d-%03d", prev, id);
            node = fn_803666B4(scene.Get(), name);
            if (node.Get() != 0) {
                fn_8036AD78(node.Get(), 0);
            }
        }
        prev = id;
        i++;
    }
    fn_802BEB90();
    fn_80173834(1, 0.0f);
    m_state = 0;
}

/* Not called anywhere (stripped by the linker): retail compiled the CSD header's inline RCPtr code
 * here; this places its strings where that code put them (see Game/AdvOption.cpp). */
void PoolRouteDisplayRCPtrStrings(const char **names) {
    names[0] = "Not inititalized m_eDealloctor\n";
    names[1] = "csdRCObject.h";
}

StageRouteDisplay::~StageRouteDisplay() {
    if (m_project != 0) {
        delete m_project;
    }
}

int StageRouteDisplay::Tick() {
    if (m_project == 0) {
        return 2;
    }
    fn_8036DAD4(m_project);
    m_time += lbl_8057D6A0.m_frameTime;
    RCPtr<CScene> scene;
    switch (m_state) {
    case 0:
        if (fn_80044E10() == 0) {
            scene = fn_8036D660(m_project, "IconsBG");
            fn_80367140(scene.Get(), 1.0f);
            scene = fn_8036D660(m_project, "Pics");
            fn_80367140(scene.Get(), 1.0f);
            scene = fn_8036D660(m_project, "Arrows");
            fn_80367140(scene.Get(), 1.0f);
            m_time = 0.0f;
            m_state = 1;
        }
        break;
    case 1:
        scene = fn_8036D660(m_project, "Pics");
        if (m_time > 2.0f) {
            fn_8036AD78(fn_803666B4(scene.Get(), "cursor").Get(), 0);
        }
        if (scene->IsEnd()) {
            scene = fn_8036D660(m_project, "IconsBG");
            if (scene->IsEnd()) {
                scene = fn_8036D660(m_project, "Arrows");
                if (scene->IsEnd()) {
                    fn_80173878(0, 1, 1.0f);
                    m_state = 2;
                }
            }
        }
        break;
    case 2:
        if (fn_80044E10() == 1) {
            if (m_project != 0) {
                delete m_project;
                m_project = 0;
            }
            return 2;
        }
        break;
    }
    return 1;
}

void StageRouteDisplay::Draw(RednerDrawArgs &args) {
    if (m_project != 0) {
        fn_8036D94C(m_project);
    }
}
