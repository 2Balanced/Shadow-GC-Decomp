/* AdvReal::StoryLibrary::Phase (class name from RTTI): the story menu of the front end and its
 * library. Its state lives in a StoryLibrary::Impl (class name inferred) that plays the CSD project
 * "csdFiles/Story_Library". The menu offers a new story, continuing the last one (its route is shown
 * on the story map), the library of finished stories and the last story; the library pages through
 * the endings unlocked in the save data, eight to a page, and shows each one's route on the map.
 * Choosing an entry sets up the stage sequence and asks the phase manager for the next phase. The
 * file name and all names marked "(name inferred)" are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include <boost/scoped_ptr.hpp>
#include <stdio.h>

#include "Game/AdvBase.h"
#include "Game/AdvCsdEffect.h"
#include "Game/AdvPhaseManager.h"
#include "Game/CsdRCPtr.h"
#include "Game/CsdSceneCtrl.h"
#include "Game/CsdSceneSet.h"
#include "Game/StageSequence.h"
#include "Game/StageTable.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Chao::CSD::CNode;
using Chao::CSD::CScene;
using Chao::CSD::CTexList;
using Chao::CSD::RCPtr;

/* The loaded CSD project (class name unknown; declared as in TGi.h). */
class UnkCsdResource_8003E808;

/* The value a project returns from fn_8036D598 (class name unknown; declared as in TGi.h). */
class UnkCsdHandle_80042DF0 {
  public:
    UnkCsdHandle_80042DF0(const UnkCsdHandle_80042DF0 &other);
    ~UnkCsdHandle_80042DF0();

    /* 0x0 */ void *m_object;
};

/* The save data (struct names unknown; see Game/GoalAction.cpp). Only the members read here are
 * named. */
struct UnkMissionSave_800BB74C {
    /* 0x00 */ u8 m_cleared;
    /* 0x04 */ s32 m_rank;
    /* 0x08 */ u8 field_0x8[0x18 - 0x8];
};

struct UnkStageSave_800BB74C {
    /* 0x00 */ s32 m_plays;
    /* 0x04 */ UnkMissionSave_800BB74C m_missions[3];
    /* 0x4C */ u8 field_0x4C[0x60 - 0x4C];
};

struct UnkSaveData_800BB74C {
    /* 0x0000 */ u8 field_0x0[0x30];
    /* 0x0030 */ s32 m_historyCount;  /* the stages of the last story played (meaning inferred) */
    /* 0x0034 */ s32 m_history[29];
    /* 0x00A8 */ UnkStageSave_800BB74C m_stages[59];
    /* 0x16C8 */ bool m_hardMode; /* (meaning inferred) */
    /* 0x16C9 */ u8 field_0x16C9[0x16D0 - 0x16C9];
    /* 0x16D0 */ u8 m_endings[0x40];  /* the endings seen, read by fn_801E5774 (meaning inferred) */
    /* 0x1710 */ u8 m_keys[0x4];      /* read by fn_800BBDD8 (meaning inferred) */
};

/* The game state (struct name unknown; see Game/AdvMemoryCheck.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xCC];
    /* 0x0CC */ u8 m_record[0x148 - 0xCC]; /* (see Game/ScoreData.cpp) */
    /* 0x148 */ u8 field_0x148[3];
    /* 0x14B */ u8 field_0x14B[0x150 - 0x14B];
};

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The stage sequence's choices (struct name unknown; see Game/StageSequencePlay2POnce.cpp). */
struct UnkStageInfo_80584720 {
    /* 0x000 */ u8 field_0x0[0x4];
    /* 0x004 */ s32 m_storyMode; /* (meaning inferred) */
    /* 0x008 */ u8 field_0x8[0x228 - 0x8];
    /* 0x228 */ s32 m_ending; /* the library entry to play (meaning inferred) */
    /* 0x22C */ u8 field_0x22C[0x230 - 0x22C];
};

extern UnkStageInfo_80584720 lbl_80584720;

/* An empty texture list (meaning inferred; see Game/AdvOption.cpp). */
extern RCPtr<CTexList> lbl_805EE110;

/* The pad state (struct name unknown; see Game/SaveCycleMessage.cpp). */
struct UnkPads_80051734 {
    /* 0x000 */ u8 field_0x0[0x11C];
    /* 0x11C */ s32 field_0x11C; /* meaning unknown */
};

class UnkManager_801E5D48;

extern "C" {
/* The CSD project cache (Game/AdvCsdCache.cpp; behaviour inferred). */
void *fn_801E5F0C(void);
UnkCsdResource_8003E808 *fn_802B5504(void *cache, const char *file);
bool fn_802B5434(void *cache, UnkCsdResource_8003E808 *project);
void fn_802B57FC(void *cache);
/* Projects (behaviour inferred; see Game/TGi.h). */
UnkCsdHandle_80042DF0 fn_8036D598(UnkCsdResource_8003E808 *project, int arg1);
RCPtr<CTexList> fn_8036C628(UnkCsdResource_8003E808 *project);
/* Scenes and nodes (behaviour inferred): a node by name, hiding a node, its pattern, the colour of
 * one of its corners, restoring that colour, and three calls on its colour as a whole. */
RCPtr<CNode> fn_803666B4(CScene *scene, const char *name);
void fn_8036AD78(CNode *node, int hidden);
void fn_8036AD14(CNode *node, int pattern);
void fn_8036AC60(CNode *node, int corner, const u32 *color);
void fn_8036AC3C(CNode *node, int corner);
void fn_8036ACA8(CNode *node);
void fn_8036ACCC(CNode *node, const u32 *color);
void fn_8036A8BC(CNode *node);

/* The text manager: the colour and alpha of the next text, and drawing message id at a position
 * (behaviour inferred; see Game/AdvOption.cpp). */
UnkManager_801E5D48 *fn_801E5D48(void);
void fn_802BEC5C(UnkManager_801E5D48 *manager, float alpha, int color);
void fn_802BEEC8(UnkManager_801E5D48 *manager, float x, float y, int message, int arg4, int arg5);

/* The voice manager and muting it (behaviour inferred; see Game/AdvMovie.cpp). */
void *fn_801E5CA8(void);
void fn_802C839C(void *manager, int arg1);

/* The save data, its ending flags, the route of an ending, and its key flags (behaviour inferred). */
UnkSaveData_800BB74C *fn_800BB74C(void);
bool fn_801E5774(u8 *flags, int index);
void fn_801E515C(UnkSaveData_800BB74C *save, s8 *route, int ending);
bool fn_800BBDD8(u8 *flags, int index);

/* Starting a story (behaviour inferred): the object fn_801E2128 returns and fn_801E5858 on it,
 * resetting a record (Game/ScoreData.cpp) and filling bytes (see Game/StageSequencePlay2POnce.cpp). */
void *fn_801E2128(void);
void fn_801E5858(void *object, int arg1);
void fn_802062FC(void *record);
void fn_800CBBFC(void *dest, int value, int count);

/* The sound manager and playing a sound (see Game/GadgetUtil.h). */
void *fn_80053164(void);
void fn_801D560C(void *manager, int soundId, int arg2, int arg3);
/* The screen fade (see Game/AdvOption.cpp). */
void fn_80173834(int arg0, float time);
void fn_80173878(int arg0, int arg1, float time);
int fn_80044E10(void);
/* The pad state (see Game/SaveCycleMessage.cpp). */
UnkPads_80051734 *fn_80051734(void);

/* Pad input (Game/AdvPadInput.cpp). */
bool fn_802B7D5C(int button, int pad);
bool fn_802B7D00(int button, int pad);
bool fn_802B7CC0(void);
int fn_802B7CE0(void);
}

/* The CSD project of the menu. */
static const char *s_csdFile = "csdFiles/Story_Library";

/* The scenes of the project. */
static const char *const s_sceneNames[7] = {
    "Background", "StoryRoute", "SRIconsBG", "Window", "PageNumber", "WindowLib", "Buttons",
};

/* Whether any of the 0x200 flags is set (name inferred). */
extern "C" bool fn_802BCABC(u8 *flags) {
    for (int i = 0; i < 0x200; i++) {
        if (fn_801E5774(flags, i)) {
            return true;
        }
    }
    return false;
}

namespace AdvReal {
namespace StoryLibrary {

/* The menu's state (class name inferred). */
class Impl {
  public:
    Impl(int library);
    ~Impl();

    bool Tick();
    bool Draw();

    void Start();
    void UpdateMenu();
    void UpdateLibrary();
    void StartStory();
    void Close();
    void ShowSynopsis();
    bool Load();
    void SetupNodes();
    void StartLastStory();
    void StartEnding();
    void StartNewStory();
    bool HasLastStory();
    bool HasLibrary();
    bool CanContinue();
    void ShowMenu();
    void ShowRouteMap();
    void ShowLastRoute();
    void ShowEndingRoute();
    void CountEndings();
    void ShowPageNumbers();
    void DrawEndings();

    /* Member names are inferred. */
    /* 0x000 */ s32 m_state; /* 0 opening, 1 menu, 2 library, 3 starting, 4 closing, 5 synopsis */
    /* 0x004 */ s32 field_0x4;
    /* 0x008 */ s32 m_step;
    /* 0x00C */ UnkCsdResource_8003E808 *m_project;
    /* 0x010 */ UnkCsdSceneSet_8003D5D8 m_scenes;
    /* 0x01C */ u8 m_busy;
    /* 0x01D */ u8 m_library;
    /* 0x020 */ RCPtr<CNode> m_routeLines[55];
    /* 0x0FC */ boost::shared_ptr<AdvCsdEffect> m_effect;
    /* 0x104 */ s32 m_cursor;
    /* 0x108 */ s32 m_ending;
    /* 0x10C */ s32 m_page;
    /* 0x110 */ s32 m_pageCount;
    /* 0x114 */ s32 m_endingCount;
    /* 0x118 */ s32 m_repeatWait;
};
SIZE_ASSERT(Impl, 0x11C);

class Phase : public AdvBase {
  public:
    Phase(int library);
    virtual ~Phase();
    virtual bool Tick();
    virtual bool Draw();

    /* 0x4 */ boost::scoped_ptr<Impl> m_impl;
};
SIZE_ASSERT(Phase, 0x8);

/* Deletes the Impl (name inferred; see checked_delete below). */
inline void DeleteImpl(Impl *impl) {
    delete impl;
}

} // namespace StoryLibrary
} // namespace AdvReal

/* Retail keeps boost::checked_delete<Impl> out of line; this specialization gives that under
 * GC/2.7 (see docs/research-notes.md and Game/EnergyCore.cpp). */
namespace boost {
template <> inline void checked_delete<AdvReal::StoryLibrary::Impl>(AdvReal::StoryLibrary::Impl *x) {
    AdvReal::StoryLibrary::DeleteImpl(x);
}
} // namespace boost

/* Whether two texture lists are the same (name inferred; as in Game/AdvOption.cpp). */
static inline int SameTexList(const RCPtr<CTexList> &a, const RCPtr<CTexList> &b) {
    if (a.m_object != 0 && b.m_object != 0) {
        return a.m_object->m_object == b.m_object->m_object;
    }
    return a.m_object == b.m_object;
}

namespace AdvReal {
namespace StoryLibrary {

/* Not called anywhere: the linker strips it from the game. This unit's .rodata is kept whole (its
 * functions address it through one base), so retail keeps the RTTI names of the CSD and boost
 * headers' inline classes the original instantiated here even though their records are stripped.
 * Compiled last (deferred inlining compiles the first definition last), this function creates
 * them in retail's order, and the RTTI records created afterwards reuse them. */
void PoolStoryLibraryRttiNames(const char **names) {
    names[0] = "Chao::CSD::RCPtr<Chao::CSD::CTexList>::RCObject<Chao::CSD::CTexList>::CFunctorBase";
    names[1] = "Chao::CSD::CBase";
    names[2] = "Chao::CSD::RCPtr<Chao::CSD::CTexList>::RCObject<Chao::CSD::CTexList>";
    names[3] = "Chao::CSD::RCPtr<Chao::CSD::CSceneList>::RCObject<Chao::CSD::CSceneList>::CFunctorBase";
    names[4] = "Chao::CSD::RCPtr<Chao::CSD::CSceneList>::RCObject<Chao::CSD::CSceneList>";
    names[5] = "boost::detail::sp_counted_base_impl<AdvReal::AdvCsdEffect *, boost::checked_deleter<AdvReal::AdvCsdEffect>>";
    names[6] = "boost::detail::sp_counted_base";
    names[7] = "std::exception";
    names[8] = "exception";
    names[9] = "std::bad_alloc";
    names[10] = "bad_alloc";
    names[11] = "Chao::CSD::RCPtr<Chao::CSD::CScene>::RCObject<Chao::CSD::CScene>::CFunctorBase";
    names[12] = "Chao::CSD::RCPtr<Chao::CSD::CScene>::RCObject<Chao::CSD::CScene>";
    names[13] = "Chao::CSD::RCPtr<Chao::CSD::CNode>::RCObject<Chao::CSD::CNode>::CFunctorBase";
    names[14] = "Chao::CSD::RCPtr<Chao::CSD::CNode>::RCObject<Chao::CSD::CNode>";
    names[15] = "boost::checked_deleter<AdvReal::AdvCsdEffect>";
}

Impl::Impl(int library) : m_state(0), m_library(library), m_cursor(0), m_ending(0), m_page(0) {
    fn_80173834(1, 0.0f);
}

Impl::~Impl() {}

bool Impl::Tick() {
    switch (m_state) {
    case 0:
        Start();
        break;
    case 1:
        UpdateMenu();
        break;
    case 2:
        UpdateLibrary();
        break;
    case 3:
        StartStory();
        break;
    case 4:
        Close();
        break;
    case 5:
        ShowSynopsis();
        break;
    }
    m_busy = fn_8003CDF4(&m_scenes);
    fn_80355F54(m_effect.get());
    return true;
}

void Impl::Start() {
    if (Load()) {
        SetupNodes();
        if (m_library) {
            m_state = 2;
            m_step = 0;
        } else {
            m_state = 1;
            m_step = 0;
            fn_802C839C(fn_801E5CA8(), 3);
        }
    } else {
        m_state = 6;
        m_step = 0;
    }
}

void Impl::UpdateMenu() {
    switch (m_step) {
    case 0:
        fn_8003CEAC(&m_scenes, s_sceneNames[3])->Play("Open", 1);
        fn_8003C8D8(&m_scenes, s_sceneNames[3], true);
        fn_8003CEAC(&m_scenes, s_sceneNames[0])->Play("StoryOpen", 1);
        fn_8003C8D8(&m_scenes, s_sceneNames[0], true);
        fn_8003CEAC(&m_scenes, s_sceneNames[1])->Play("StoryOpen", 1);
        fn_8003CEAC(&m_scenes, s_sceneNames[2])->Play("StoryOpen", 1);
        fn_8003CEAC(&m_scenes, s_sceneNames[6])->Play("DefaultAnim", 1);
        fn_8003C8D8(&m_scenes, s_sceneNames[6], true);
        if (CanContinue()) {
            m_cursor = 1;
        }
        m_step = 1;
        break;
    case 1:
        if (fn_8003DBB0(fn_8003CEAC(&m_scenes, s_sceneNames[3]))) {
            fn_8003CEAC(&m_scenes, s_sceneNames[3])->Play("Select", 2);
            m_step = 2;
        }
        break;
    case 2:
        if (fn_802B7D5C(3, 4) || fn_802B7D5C(4, 4)) {
            int previous = m_cursor;
            int step = 1;
            if (fn_802B7D5C(3, 4)) {
                step = -1;
            }
            while (step != 0) {
                m_cursor += step;
                if (m_cursor > 3) {
                    m_cursor = 0;
                } else if (m_cursor < 0) {
                    m_cursor = 3;
                }
                if (m_cursor == previous) {
                    break;
                }
                switch (m_cursor) {
                case 1:
                    if (CanContinue()) {
                        step = 0;
                    }
                    break;
                case 2:
                    if (HasLibrary()) {
                        step = 0;
                    }
                    break;
                case 3:
                    if (HasLastStory()) {
                        step = 0;
                    }
                    break;
                default:
                    step = 0;
                    break;
                }
            }
            if (m_cursor != previous) {
                fn_801D560C(fn_80053164(), 0xE002, 0, 0);
            }
        } else if (fn_802B7D5C(1, 4)) {
            switch (m_cursor) {
            case 0:
                StartNewStory();
                m_state = 3;
                m_step = 0;
                fn_801D560C(fn_80053164(), 0xE003, 0, 0);
                fn_80051734()->field_0x11C = fn_802B7CE0();
                break;
            case 1:
                m_state = 5;
                m_step = 0;
                fn_801D560C(fn_80053164(), 0xE003, 0, 0);
                fn_80051734()->field_0x11C = fn_802B7CE0();
                break;
            case 2:
                StartEnding();
                m_state = 2;
                m_step = 0;
                fn_801D560C(fn_80053164(), 0xE003, 0, 0);
                break;
            case 3:
                StartLastStory();
                m_state = 3;
                m_step = 0;
                fn_801D560C(fn_80053164(), 0xE003, 0, 0);
                fn_80051734()->field_0x11C = fn_802B7CE0();
                break;
            }
        } else if (fn_802B7D5C(2, 4)) {
            m_step = 3;
            fn_801D560C(fn_80053164(), 0xE004, 0, 0);
        }
        break;
    case 3:
        m_state = 4;
        m_step = 0;
        break;
    }
    ShowMenu();
    ShowLastRoute();
}

void Impl::UpdateLibrary() {
    CountEndings();
    switch (m_step) {
    case 0:
        if (m_library) {
            m_cursor = 2;
            ShowMenu();
            fn_8003CEAC(&m_scenes, s_sceneNames[0])->Play("LibraryOpen", 1);
            fn_8003C8D8(&m_scenes, s_sceneNames[0], true);
            fn_8003CEAC(&m_scenes, s_sceneNames[6])->Play("DefaultAnim", 1);
            fn_8003C8D8(&m_scenes, s_sceneNames[6], true);
            fn_802C839C(fn_801E5CA8(), 4);
        } else {
            fn_8003CEAC(&m_scenes, s_sceneNames[0])->Play("LibraryOpen", 1);
            fn_8003CEAC(&m_scenes, s_sceneNames[3])->Play("Close", 1);
        }
        fn_8003CEAC(&m_scenes, s_sceneNames[5])->Play("Open", 1);
        fn_8003C8D8(&m_scenes, s_sceneNames[5], true);
        fn_8003CEAC(&m_scenes, s_sceneNames[4])->Play("Open", 1);
        fn_8003C8D8(&m_scenes, s_sceneNames[4], true);
        fn_8036AD14(fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[4])->GetScene().Get(), "R2").Get(), 1);
        fn_8036AD14(fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[4])->GetScene().Get(), "L1").Get(), 1);
        fn_8003CEAC(&m_scenes, s_sceneNames[1])->Play("LibraryOpen", 1);
        fn_8003C8D8(&m_scenes, s_sceneNames[1], true);
        fn_8003CEAC(&m_scenes, s_sceneNames[2])->Play("LibraryOpen", 1);
        fn_8003C8D8(&m_scenes, s_sceneNames[2], true);
        m_library = 0;
        m_repeatWait = 0;
        m_step = 1;
        break;
    case 1:
        if (fn_8003DBB0(fn_8003CEAC(&m_scenes, s_sceneNames[5]))) {
            m_step = 2;
        }
        break;
    case 2:
        if (m_repeatWait != 0) {
            m_repeatWait--;
        } else if (fn_802B7D5C(1, 4)) {
            StartEnding();
            m_state = 3;
            m_step = 0;
            fn_801D560C(fn_80053164(), 0xE003, 0, 0);
            fn_80051734()->field_0x11C = fn_802B7CE0();
        } else if (fn_802B7D00(3, 4)) {
            if (m_ending > 1) {
                m_ending--;
                m_page = m_ending / 8 + (m_ending % 8 ? 1 : 0);
                m_repeatWait = 2;
                fn_801D560C(fn_80053164(), 0xE002, 0, 0);
            }
        } else if (fn_802B7D00(4, 4)) {
            if (m_ending < m_endingCount) {
                m_ending++;
                m_page = m_ending / 8 + (m_ending % 8 ? 1 : 0);
                m_repeatWait = 2;
                fn_801D560C(fn_80053164(), 0xE002, 0, 0);
            }
        } else if (fn_802B7D00(9, 4)) {
            if (m_page > 1) {
                m_page--;
                m_ending -= 8;
                m_repeatWait = 2;
                fn_801D560C(fn_80053164(), 0xE002, 0, 0);
                fn_8003CEAC(&m_scenes, s_sceneNames[4])->Reset();
                fn_8003CEAC(&m_scenes, s_sceneNames[4])->Play("arrowL", 1);
            }
        } else if (fn_802B7D00(8, 4)) {
            if (m_page < m_pageCount) {
                m_page++;
                m_ending += 8;
                if (m_ending > m_endingCount) {
                    m_ending = m_endingCount;
                }
                m_repeatWait = 2;
                fn_801D560C(fn_80053164(), 0xE002, 0, 0);
                fn_8003CEAC(&m_scenes, s_sceneNames[4])->Reset();
                fn_8003CEAC(&m_scenes, s_sceneNames[4])->Play("arrowR", 1);
            }
        } else if (fn_802B7D5C(2, 4)) {
            fn_8003CEAC(&m_scenes, s_sceneNames[5])->Play("Close", 1);
            fn_8003CEAC(&m_scenes, s_sceneNames[4])->Play("Close", 1);
            fn_8003CEAC(&m_scenes, s_sceneNames[1])->Play("LibraryClose", 1);
            fn_8003CEAC(&m_scenes, s_sceneNames[2])->Play("LibraryClose", 1);
            fn_8003CEAC(&m_scenes, s_sceneNames[0])->Play("StoryOpen", 1);
            fn_8003CEAC(&m_scenes, s_sceneNames[3])->Play("Open", 1);
            fn_8003C8D8(&m_scenes, s_sceneNames[3], true);
            m_step = 3;
            fn_801D560C(fn_80053164(), 0xE004, 0, 0);
        }
        break;
    case 3:
        if (!m_busy) {
            fn_8003C8D8(&m_scenes, s_sceneNames[5], false);
            fn_8003C8D8(&m_scenes, s_sceneNames[4], false);
            fn_8003C8D8(&m_scenes, s_sceneNames[1], false);
            fn_8003C8D8(&m_scenes, s_sceneNames[2], false);
            fn_8003CEAC(&m_scenes, s_sceneNames[1])->Play("StoryOpen", 1);
            fn_8003CEAC(&m_scenes, s_sceneNames[2])->Play("StoryOpen", 1);
            fn_8003CEAC(&m_scenes, s_sceneNames[3])->Play("Select", 2);
            m_state = 1;
            m_step = 2;
        }
        break;
    }
    ShowPageNumbers();
    ShowEndingRoute();
}

void Impl::StartStory() {
    switch (m_step) {
    case 0:
        fn_80173878(1, 1, 1.0f);
        m_step = 2;
        break;
    case 1:
        break;
    case 2:
        if (fn_80044E10() == 1) {
            fn_802C839C(fn_801E5CA8(), 0);
            fn_802B7528(fn_801E5E00(), 0);
        }
        break;
    }
}

void Impl::Close() {
    switch (m_step) {
    case 0:
        fn_8003CEAC(&m_scenes, s_sceneNames[3])->Play("Close", 1);
        fn_8003CEAC(&m_scenes, s_sceneNames[0])->Play("StoryClose", 1);
        fn_8003CEAC(&m_scenes, s_sceneNames[1])->Play("StoryClose", 1);
        fn_8003CEAC(&m_scenes, s_sceneNames[2])->Play("StoryClose", 1);
        m_step = 2;
        break;
    case 1:
        break;
    case 2:
        if (!m_busy) {
            fn_802B7528(fn_801E5E00(), 1);
        }
        break;
    }
}

void Impl::ShowSynopsis() {
    switch (m_step) {
    case 0:
        fn_80173878(1, 1, 1.0f);
        m_step = 2;
        break;
    case 1:
        break;
    case 2:
        if (fn_80044E10() == 1) {
            fn_802B57FC(fn_801E5F0C());
            fn_802B7528(fn_801E5E00(), 5);
        }
        break;
    }
}

bool Impl::Draw() {
    if (!fn_802B5434(fn_801E5F0C(), m_project)) {
        return false;
    }
    if (SameTexList(fn_8036C628(m_project), lbl_805EE110)) {
        return false;
    }
    fn_80355EB0(m_effect.get(), fn_8036C628(m_project));
    fn_8003C92C(&m_scenes, fn_8036C628(m_project));
    switch (m_state) {
    case 2:
        switch (m_step) {
        case 2:
            DrawEndings();
            break;
        }
        break;
    }
    return true;
}

bool Impl::Load() {
    m_project = fn_802B5504(fn_801E5F0C(), s_csdFile);
    if (m_project != 0) {
        UnkCsdHandle_80042DF0 project = fn_8036D598(m_project, 0);
        fn_8003CEF8(&m_scenes, project, s_sceneNames, 7);
        m_effect = boost::shared_ptr<AdvCsdEffect>(new AdvCsdEffect(project));
        return true;
    }
    return false;
}

void Impl::SetupNodes() {
    fn_8003CE5C(&m_scenes);
    const char *lines[55] = {
        "hsline10",    "hsline9",     "hsline8",     "hsline7",     "hsline6",     "hsline5",
        "hsline4",     "hsline3",     "hsline2",     "hsline1",     "line100-200", "line100-201",
        "line100-202", "line200-300", "line200-301", "line201-300", "line201-301", "line201-302",
        "line202-301", "line202-302", "line300-400", "line300-401", "line300-402", "line301-401",
        "line301-402", "line301-403", "line302-402", "line302-403", "line302-404", "line400-500",
        "line400-501", "line401-500", "line401-501", "line401-502", "line402-501", "line402-502",
        "line402-503", "line403-502", "line403-503", "line403-504", "line404-503", "line404-504",
        "line500-600", "line500-601", "line501-600", "line501-601", "line501-602", "line502-601",
        "line502-602", "line502-603", "line503-602", "line503-603", "line503-604", "line504-603",
        "line504-604",
    };
    for (int i = 0; i < 55; i++) {
        m_routeLines[i] = fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[1])->GetScene().Get(), lines[i]);
    }
    if (fn_802B7CC0()) {
        RCPtr<CNode> node;
        node = fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[6])->GetScene().Get(), "select_btn");
        fn_8036AD14(node.Get(), 3);
        node = fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[6])->GetScene().Get(), "exit_btn");
        fn_8036AD14(node.Get(), 3);
    }
}

void Impl::StartNewStory() {
    fn_801E5858(fn_801E2128(), 5);
    StageSequence::Manager::SetNext(1);
    lbl_80584720.m_storyMode = 0;
    fn_802062FC(lbl_8057D6A0.m_record);
    fn_800CBBFC(lbl_8057D6A0.field_0x148, 1, 3);
}

void Impl::StartEnding() {
    fn_801E5858(fn_801E2128(), 5);
    StageSequence::Manager::SetNext(7);
    int count = 0;
    int page = 1;
    int line = 0;
    UnkSaveData_800BB74C *save = fn_800BB74C();
    int i;
    for (i = 0; i < 0x146; i++) {
        if (fn_801E5774(save->m_endings, i)) {
            line++;
            count++;
            if (line > 8) {
                line -= 8;
                page++;
            }
            if (page == m_page && count == m_ending) {
                break;
            }
        }
    }
    lbl_80584720.m_ending = i;
    fn_802062FC(lbl_8057D6A0.m_record);
}

void Impl::StartLastStory() {
    fn_801E5858(fn_801E2128(), 5);
    StageSequence::Manager::SetNext(1);
    lbl_80584720.m_storyMode = 2;
    fn_802062FC(lbl_8057D6A0.m_record);
    fn_800CBBFC(lbl_8057D6A0.field_0x148, 1, 3);
}

bool Impl::CanContinue() {
    return fn_800BB74C()->m_historyCount != 0;
}

bool Impl::HasLibrary() {
    return fn_802BCABC(fn_800BB74C()->m_endings);
}

bool Impl::HasLastStory() {
    return fn_800BB74C()->m_hardMode;
}

/* The colours of the menu entries (names inferred). These and the colours below are file statics:
 * declared as function-local statics, this compiler setup initializes them at run time behind a
 * guard, which retail does not. */
static u32 s_disabledColor = 0x555555FF;
static u32 s_color = 0x990000FF;
static u32 s_frameColor = 0xFFFFFFFF;
static const char *s_menuNodes[4] = {"new", "continue", "library", "laststory"};

void Impl::ShowMenu() {
    for (int i = 0; i < 4; i++) {
        RCPtr<CNode> node = fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[3])->GetScene().Get(), s_menuNodes[i]);
        bool disabled = false;
        switch (i) {
        case 1:
            if (!CanContinue()) {
                disabled = true;
            }
            break;
        case 2:
            if (!HasLibrary()) {
                disabled = true;
            }
            break;
        case 3:
            if (HasLastStory()) {
                fn_8036A8BC(node.Get());
            } else {
                fn_8036AD78(node.Get(), 1);
            }
            break;
        }
        for (int corner = 0; corner < 4; corner++) {
            if (disabled) {
                fn_8036AC60(node.Get(), corner, &s_disabledColor);
            } else if (i != m_cursor) {
                fn_8036AC60(node.Get(), corner, &s_color);
            } else {
                fn_8036AC3C(node.Get(), corner);
            }
        }
        if (i != m_cursor && m_step == 2) {
            fn_8036ACCC(node.Get(), &s_frameColor);
        } else {
            fn_8036ACA8(node.Get());
        }
    }
}

/* The colours of the route map (names inferred). */
static u32 s_lineColor = 0x969696FF;
static u32 s_stageColor = 0x969696FF;
static u32 s_endingColor = 0x969696FF;
static u32 s_routeColor = 0xFFFFFFFF;
static u32 s_lastLineColor = 0xFFFFFFFF;
static u32 s_clearedColor = 0xFFFFFFFF;
static u32 s_keyColor = 0xFFFFFFFF;

void Impl::ShowRouteMap() {
    int i;
    int corner;
    for (i = 0; i < 55; i++) {
        for (corner = 0; corner < 4; corner++) {
            if (m_routeLines[i].Get() != 0) {
                fn_8036AC60(m_routeLines[i].Get(), corner, &s_lineColor);
            }
        }
    }
    RCPtr<CNode> node;
    char name[16];
    for (i = 5; i < 27; i++) {
        fn_800B74B4();
        sprintf(name, "%04d", lbl_804C4A18[i].m_id);
        node = fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[2])->GetScene().Get(), name);
        for (corner = 0; corner < 4; corner++) {
            fn_8036AC60(node.Get(), corner, &s_stageColor);
        }
        fn_8036AD14(node.Get(), 0);
    }
    for (i = 0; i < 10; i++) {
        sprintf(name, "%d", i + 1);
        node = fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[2])->GetScene().Get(), name);
        for (corner = 0; corner < 4; corner++) {
            fn_8036AC60(node.Get(), corner, &s_endingColor);
        }
        fn_8036AD14(node.Get(), 0);
    }
    UnkSaveData_800BB74C *save = fn_800BB74C();
    const s32 routeLines[5][5][5] = {
        {{10, 11, 12, -1, -1}, {-1, -1, -1, -1, -1}, {-1, -1, -1, -1, -1}, {-1, -1, -1, -1, -1}, {-1, -1, -1, -1, -1}},
        {{13, 14, -1, -1, -1}, {15, 16, 17, -1, -1}, {-1, 18, 19, -1, -1}, {-1, -1, -1, -1, -1}, {-1, -1, -1, -1, -1}},
        {{20, 21, 22, -1, -1}, {-1, 23, 24, 25, -1}, {-1, -1, 26, 27, 28}, {-1, -1, -1, -1, -1}, {-1, -1, -1, -1, -1}},
        {{29, 30, -1, -1, -1}, {31, 32, 33, -1, -1}, {-1, 34, 35, 36, -1}, {-1, -1, 37, 38, 39}, {-1, -1, -1, 40, 41}},
        {{42, 43, -1, -1, -1}, {44, 45, 46, -1, -1}, {-1, 47, 48, 49, -1}, {-1, -1, 50, 51, 52}, {-1, -1, -1, 53, 54}},
    };
    for (i = 0; i < 0x146; i++) {
        if (fn_801E5774(save->m_endings, i)) {
            s8 route[7];
            fn_801E515C(fn_800BB74C(), route, i);
            for (int j = 1; j < 6; j++) {
                for (corner = 0; corner < 4; corner++) {
                    fn_8036AC60(m_routeLines[routeLines[j - 1][route[j - 1]][route[j]]].Get(), corner, &s_routeColor);
                }
            }
            for (corner = 0; corner < 4; corner++) {
                fn_8036AC60(m_routeLines[9 - route[6]].Get(), corner, &s_lastLineColor);
            }
        }
    }
    for (i = 5; i < 27; i++) {
        if (save->m_stages[i].m_missions[0].m_cleared || save->m_stages[i].m_missions[1].m_cleared ||
            save->m_stages[i].m_missions[2].m_cleared) {
            fn_800B74B4();
            sprintf(name, "%04d", lbl_804C4A18[i].m_id);
            node = fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[2])->GetScene().Get(), name);
            for (corner = 0; corner < 4; corner++) {
                fn_8036AC60(node.Get(), corner, &s_clearedColor);
            }
            fn_8036AD14(node.Get(), 0);
        }
    }
    for (i = 0; i < 10; i++) {
        if (fn_800BBDD8(save->m_keys, i)) {
            sprintf(name, "%d", i + 1);
            node = fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[2])->GetScene().Get(), name);
            for (corner = 0; corner < 4; corner++) {
                fn_8036AC60(node.Get(), corner, &s_keyColor);
            }
            fn_8036AD14(node.Get(), 1);
        }
    }
}

void Impl::ShowLastRoute() {
    if (m_cursor == 1) {
        fn_8003C8D8(&m_scenes, s_sceneNames[1], true);
        fn_8003C8D8(&m_scenes, s_sceneNames[2], true);
    } else {
        fn_8003C8D8(&m_scenes, s_sceneNames[1], false);
        fn_8003C8D8(&m_scenes, s_sceneNames[2], false);
        return;
    }
    ShowRouteMap();
    int stage;
    int i = 0;
    UnkSaveData_800BB74C *save = fn_800BB74C();
    RCPtr<CNode> node;
    for (; i <= save->m_historyCount; i++) {
        stage = save->m_history[i];
        char name[12];
        fn_800B74B4();
        sprintf(name, "%04d", lbl_804C4A18[stage].m_id);
        node = fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[2])->GetScene().Get(), name);
        if (node.Get() != 0) {
            fn_8036AD14(node.Get(), 1);
        }
    }
}

void Impl::ShowEndingRoute() {
    ShowRouteMap();
    int count = 0;
    UnkSaveData_800BB74C *save = fn_800BB74C();
    for (int i = 0; i < 0x146; i++) {
        if (fn_801E5774(save->m_endings, i)) {
            count++;
            if (count == m_ending) {
                s8 route[7];
                fn_801E515C(fn_800BB74C(), route, i);
                RCPtr<CNode> node;
                char name[12];
                for (int j = 0; j < 6; j++) {
                    sprintf(name, "%04d", route[j] + (j + 1) * 100);
                    node = fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[2])->GetScene().Get(), name);
                    fn_8036AD14(node.Get(), 1);
                }
                sprintf(name, "%d", route[6] + 1);
                node = fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[2])->GetScene().Get(), name);
                fn_8036AD14(node.Get(), 2);
                break;
            }
        }
    }
}

void Impl::CountEndings() {
    UnkSaveData_800BB74C *save = fn_800BB74C();
    m_endingCount = 0;
    for (int i = 0; i < 0x146; i++) {
        if (fn_801E5774(save->m_endings, i)) {
            m_endingCount++;
        }
    }
    m_pageCount = m_endingCount / 8;
    if (m_endingCount % 8 != 0) {
        m_pageCount++;
    }
    if (m_ending > m_endingCount) {
        m_ending = m_endingCount;
    }
    if (m_ending < 1) {
        m_ending = 1;
    }
    m_page = m_ending / 8;
    if (m_ending % 8 != 0) {
        m_page++;
    }
}

/* Not called anywhere: the linker strips it from the game. Retail compiled the CSD header's inline
 * RCPtr code here, after ShowPageNumbers; this places its strings where that code put them. */
void PoolStoryLibraryRCPtrStrings(const char **names) {
    names[0] = "Not inititalized m_eDealloctor\n";
    names[1] = "csdRCObject.h";
}

/* The colours of the page arrows (names inferred). */
static u32 s_firstLeft = 0x924649FF;
static u32 s_firstRight = 0x924649FF;
static u32 s_lastLeft = 0x924649FF;
static u32 s_lastRight = 0xFFFFFFFF;
static u32 s_middleLeft = 0xFFFFFFFF;
static u32 s_middleRight = 0x924649FF;
static u32 s_onlyLeft = 0xFFFFFFFF;
static u32 s_onlyRight = 0xFFFFFFFF;

void Impl::ShowPageNumbers() {
    RCPtr<CNode> node;
    node = fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[4])->GetScene().Get(), "number_ll");
    fn_8036AD14(node.Get(), m_page / 10);
    node = fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[4])->GetScene().Get(), "number_lr");
    fn_8036AD14(node.Get(), m_page % 10);
    node = fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[4])->GetScene().Get(), "number_rl");
    fn_8036AD14(node.Get(), m_pageCount / 10);
    node = fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[4])->GetScene().Get(), "number_rr");
    fn_8036AD14(node.Get(), m_pageCount % 10);
    RCPtr<CNode> right;
    RCPtr<CNode> left;
    left = fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[4])->GetScene().Get(), "arrow_left");
    right = fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[4])->GetScene().Get(), "arrow_right");
    if (left.Get() != 0 && right.Get() != 0) {
        int corner;
        if (m_pageCount == 1) {
            for (corner = 0; corner < 4; corner++) {
                fn_8036AC60(left.Get(), corner, &s_firstLeft);
            }
            for (corner = 0; corner < 4; corner++) {
                fn_8036AC60(right.Get(), corner, &s_firstRight);
            }
        } else if (m_page == 1) {
            for (corner = 0; corner < 4; corner++) {
                fn_8036AC60(left.Get(), corner, &s_lastLeft);
            }
            for (corner = 0; corner < 4; corner++) {
                fn_8036AC60(right.Get(), corner, &s_lastRight);
            }
        } else if (m_page == m_pageCount) {
            for (corner = 0; corner < 4; corner++) {
                fn_8036AC60(left.Get(), corner, &s_middleLeft);
            }
            for (corner = 0; corner < 4; corner++) {
                fn_8036AC60(right.Get(), corner, &s_middleRight);
            }
        } else {
            for (corner = 0; corner < 4; corner++) {
                fn_8036AC60(left.Get(), corner, &s_onlyLeft);
            }
            for (corner = 0; corner < 4; corner++) {
                fn_8036AC60(right.Get(), corner, &s_onlyRight);
            }
        }
    }
}

void Impl::DrawEndings() {
    const float positions[8][2] = {
        {100.0f, 110.0f}, {100.0f, 145.0f}, {100.0f, 180.0f}, {100.0f, 215.0f},
        {100.0f, 250.0f}, {100.0f, 285.0f}, {100.0f, 320.0f}, {100.0f, 355.0f},
    };
    UnkSaveData_800BB74C *save = fn_800BB74C();
    int count = 0;
    int page = 1;
    int line = 0;
    for (int i = 0; i < 0x146; i++) {
        if (fn_801E5774(save->m_endings, i)) {
            line++;
            count++;
            if (line > 8) {
                line -= 8;
                page++;
            }
            if (page == m_page) {
                if (count == m_ending) {
                    fn_802BEC5C(fn_801E5D48(), 1.0f, 0);
                } else {
                    fn_802BEC5C(fn_801E5D48(), 0.6f, 3);
                }
                float scale = 1.0f; /* retail multiplies the y position by 1 (name inferred) */
                fn_802BEEC8(fn_801E5D48(), positions[line - 1][0], scale * positions[line - 1][1], i + 0x138F, 1, 0);
            }
            if (page > m_page) {
                break;
            }
        }
    }
}

Phase::Phase(int library) : m_impl(new Impl(library)) {}

Phase::~Phase() {}

bool Phase::Tick() {
    return m_impl->Tick();
}

bool Phase::Draw() {
    return m_impl->Draw();
}

} // namespace StoryLibrary
} // namespace AdvReal
