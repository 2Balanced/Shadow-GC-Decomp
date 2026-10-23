/* AdvReal::Title::Phase (class name from RTTI): the front-end phase of the title screen and its
 * menus. Its screen loads the "csdFiles/honban" project, shows the title until a button is
 * pressed (or plays the demo when the title has run out), and then runs the main menu (1P, 2P,
 * options), the 1P menu (story, stage select, hard mode) and the hard mode menu (new or resume).
 * The cursor positions are kept across visits in s_cursor. The file name, the screen's class name
 * and all function and member names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "Game/AdvBase.h"
#include "Game/AdvCsdEffect.h"
#include "Game/CsdSceneSet.h"
#include "Game/StageSequence.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* The value a project returns from fn_8036D598 (class name unknown; see Game/TGi.h). */
class UnkCsdHandle_80042DF0 {
  public:
    UnkCsdHandle_80042DF0(const UnkCsdHandle_80042DF0 &other);
    ~UnkCsdHandle_80042DF0();

    /* 0x0 */ void *m_object;
};

/* The memory card object (class name unknown; see Game/AdvMemoryCheck.cpp). */
class UnkMemoryCard_802E472C {
  public:
    ~UnkMemoryCard_802E472C();

    /* 0x0 */ u8 field_0x0[0x8];
};

/* The save data (struct name unknown; see Game/GoalAction.cpp). */
struct UnkSaveData_800BB74C {
    /* 0x0000 */ u8 field_0x0[0x9C];
    /* 0x009C */ s32 field_0x9C; /* a hard mode game can be resumed when not 0 (meaning inferred) */
    /* 0x00A0 */ u8 field_0xA0[0x16C9 - 0xA0];
    /* 0x16C9 */ u8 m_allBest; /* every mission has the best rank: opens hard mode */
};

/* The game state (struct name unknown; see Game/AdvMemoryCheck.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x20];
    /* 0x020 */ float m_dt; /* (name inferred) */
    /* 0x024 */ u8 field_0x24[0x80 - 0x24];
    /* 0x080 */ u8 m_titleWait; /* the title waits m_timer before it takes a button (meaning inferred) */
    /* 0x081 */ u8 field_0x81[0xCC - 0x81];
    /* 0x0CC */ u8 m_record[0x148 - 0xCC]; /* (name inferred; see Game/StageSequencePlayOnce.cpp) */
    /* 0x148 */ u8 field_0x148[3]; /* meaning unknown */
    /* 0x14B */ u8 field_0x14B[0x150 - 0x14B];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The stages to play (struct name unknown; see Game/StageSequencePlayOnce.cpp). */
struct UnkStageInfo_80584720 {
    /* 0x000 */ u8 field_0x0[0x20];
    /* 0x020 */ s32 m_resume; /* (name inferred) */
    /* 0x024 */ u8 field_0x24[0x230 - 0x24];
};
SIZE_ASSERT(UnkStageInfo_80584720, 0x230);

extern UnkStageInfo_80584720 lbl_80584720;

/* The pads (struct name unknown; see Game/SaveCycleMessage.cpp). */
struct UnkPad_80051734 {
    /* 0x00 */ u8 field_0x0[0x28];
    /* 0x28 */ s32 m_state; /* 1 while the pad is missing (meaning inferred) */
};

struct UnkPads_80051734 {
    /* 0x000 */ u8 field_0x0[0x20];
    /* 0x020 */ UnkPad_80051734 *m_pads[2];
    /* 0x028 */ u8 field_0x28[0x11C - 0x28];
    /* 0x11C */ s32 field_0x11C; /* meaning unknown */
};

class UnkObject_802E5B2C;
class UnkManager_801E5E00;

/* A project's texture list (see Game/AdvOption.cpp), and the empty one. */
extern Chao::CSD::RCPtr<Chao::CSD::CTexList> lbl_805EE110;

/* Whether two texture lists are the same (name inferred; as in Game/AdvOption.cpp). */
static inline int SameTexList(const Chao::CSD::RCPtr<Chao::CSD::CTexList> &a,
                              const Chao::CSD::RCPtr<Chao::CSD::CTexList> &b) {
    if (a.m_object != 0 && b.m_object != 0) {
        return a.m_object->m_object == b.m_object->m_object;
    }
    return a.m_object == b.m_object;
}

extern "C" {
void *memset(void *dst, int value, unsigned long count);
/* The project's texture list (behaviour inferred; see Game/AdvOption.cpp). */
Chao::CSD::RCPtr<Chao::CSD::CTexList> fn_8036C628(void *project);
int fn_803A8DA8(const void *a, const void *b, unsigned long size); /* memcmp */

UnkPads_80051734 *fn_80051734(void);
UnkSaveData_800BB74C *fn_800BB74C(void);

/* The front-end's CSD folder: loading a project, and whether it is ready (behaviour inferred). */
void *fn_801E5F0C(void);
void *fn_802B5504(void *folder, const char *name);
bool fn_802B5434(void *folder, void *project);

/* Projects, scenes and nodes (see Game/TGi.h): the root handle, a node by name, and showing a
 * node's pattern (behaviour inferred). */
UnkCsdHandle_80042DF0 fn_8036D598(void *project, int arg1);
Chao::CSD::RCPtr<Chao::CSD::CNode> fn_803666B4(Chao::CSD::CScene *scene, const char *name);
void fn_8036AD14(Chao::CSD::CNode *node, int pattern);
void fn_8036AD78(Chao::CSD::CNode *node, int hidden);
/* Stops the scene's motion (behaviour inferred). */
void fn_8003D870(UnkCsdSceneCtrl_8003DD20 *ctrl);

/* Menu input: whether button (1 decide, 2 cancel, 3 up, 4 down, 0 start) was pressed, and two
 * values of the input (behaviour inferred). */
bool fn_802B7D5C(int button, int arg1);
bool fn_802B7CC0(void);
s32 fn_802B7CE0(void);

/* The background music: the player, and choosing a track (behaviour inferred). */
void *fn_801E5CA8(void);
void fn_802C839C(void *player, int track);

/* The phase manager and the request for phase id (see Game/AdvMemoryCheck.cpp). */
UnkManager_801E5E00 *fn_801E5E00(void);
void fn_802B7528(UnkManager_801E5E00 *manager, int id);

/* The message window (see Game/SaveCycleMessage.cpp). */
UnkObject_802E5B2C *fn_80117514(void);
bool fn_802E5810(UnkObject_802E5B2C *object);
void fn_802E585C(UnkObject_802E5B2C *object, int id, int pad, int quick);
int fn_802E58B4(UnkObject_802E5B2C *object, int id, int pad, int quick);

/* Sound effects (see Game/BlackDoomCtrlUnit.cpp). */
void *fn_80053164(void);
void fn_801D560C(void *sound, int id, int arg2, int arg3);

/* Screen fades (see Game/ScreenFade.h). */
void *fn_80044254(void);
void fn_80044648(void *fader, float level);
int fn_80044E10(void);
void fn_80173834(int arg0, float time);
void fn_80173878(int arg0, int arg1, float time);

/* Starting a game: the system manager and its next system, the record set-up and a byte fill
 * (behaviour inferred; see Game/StageSequencePlayOnce.cpp). */
void *fn_801E2128(void);
void fn_801E5858(void *manager, int system);
void fn_802062FC(void *record);
void fn_800CBBFC(void *dest, int value, int count);
}

namespace AdvReal {
namespace Title {

/* The cursor positions of the menus (struct name inferred). */
struct CursorValues {
    void Clear() {
        memset(this, 0, sizeof(*this));
    }

    /* 0x0 */ s32 m_title;
    /* 0x4 */ s32 m_main;  /* Menu_1P, Menu_2P, Menu_Options */
    /* 0x8 */ s32 m_story; /* 1P_Story, 1P_Selct, 1P_Hard */
    /* 0xC */ s32 m_hard;  /* Hard_new, Hard_Resume */
};

/* The menus' cursors, kept across visits of the title phase (struct name inferred). */
struct Cursor {
    Cursor() {
        Clear();
    }

    void Clear() {
        m_values.Clear();
        Save();
        m_lastState = -1;
    }

    /* Whether the cursor moved since Save (name inferred). */
    bool Changed() {
        if (m_changed) {
            return true;
        }
        return fn_803A8DA8(&m_values, &m_saved, sizeof(m_values)) != 0;
    }

    void Save();

    /* 0x00 */ CursorValues m_values;
    /* 0x10 */ CursorValues m_saved;
    /* 0x20 */ u8 m_changed;
    /* 0x24 */ s32 m_lastState; /* the screen to return to, or -1 */
};
SIZE_ASSERT(Cursor, 0x28);

/* The screens of the title phase (names inferred). */
enum State {
    STATE_WAIT_CARD = 0,
    STATE_LOAD = 1,
    STATE_TITLE = 2,
    STATE_MAIN = 3,
    STATE_STORY = 4,
    STATE_HARD = 5,
    STATE_DEMO = 6,
    STATE_ADVERTISE = 7,
    STATE_2P = 8,
    STATE_RESTART = 9,
    STATE_ERROR = 10
};

/* The phase's state (class name inferred). */
class Screen {
  public:
    Screen();
    ~Screen();

    bool Tick();
    bool Draw();

    bool CanPlay2P();
    void Refresh();
    void Setup();
    bool LoadProject();
    void StartHard(bool resume);
    void Play2P();
    void Demo();
    void HardMenu();
    void StoryMenu();
    void MainMenu();
    void Advertise();
    void TitleScreen();
    void Load();
    void WaitCard();
    void Next();
    void SetState(int state, bool start);

    /* Moves on to state once the scenes have played (name inferred). */
    void Leave(int state, bool start) {
        m_nextState = state;
        m_nextStart = start;
        m_step = 2;
    }

    /* 0x00 */ s32 m_state;
    /* 0x04 */ s32 m_nextState;
    /* 0x08 */ s32 m_step;
    /* 0x0C */ void *m_project;
    /* 0x10 */ UnkCsdSceneSet_8003D5D8 m_scenes;
    /* 0x1C */ u8 m_playing;
    /* 0x1D */ bool m_nextStart;
    /* 0x1E */ u8 m_hardOpen;
    /* 0x1F */ u8 m_canResume;
    /* 0x20 */ u8 m_started;
    /* 0x24 */ boost::shared_ptr<AdvCsdEffect> m_effect;
    /* 0x2C */ u8 m_loaded;
    /* 0x30 */ UnkMemoryCard_802E472C *m_card;
    /* 0x34 */ float m_timer;
};
SIZE_ASSERT(Screen, 0x38);

/* Deletes the screen (name inferred; see checked_delete below). */
inline void DeleteScreen(Screen *screen) {
    delete screen;
}

} // namespace Title
} // namespace AdvReal

/* Retail keeps boost::checked_delete<Screen> out of line and the destructor at its own place;
 * this specialization gives that under GC/2.7 (see docs/research-notes.md, EnergyCore). */
namespace boost {
template <> inline void checked_delete<AdvReal::Title::Screen>(AdvReal::Title::Screen *x) {
    AdvReal::Title::DeleteScreen(x);
}
} // namespace boost

namespace AdvReal {
namespace Title {

/* The project's two scenes, and the items of the menus. */
static const char *const s_sceneNames[2] = {"Title", "Menu"};
static const char *s_projectName = "csdFiles/honban";
static const char *s_mainItems[3] = {"Menu_1P", "Menu_2P", "Menu_Options"};
static const char *s_storyItems[3] = {"1P_Story", "1P_Selct", "1P_Hard"};
static const char *s_hardItems[2] = {"Hard_new", "Hard_Resume"};

static Cursor s_cursor;

/* Plays the menu sound id (name inferred). */
inline void PlaySound(int id) {
    fn_801D560C(fn_80053164(), id, 0, 0);
}

Screen::Screen() : m_state(0) {
    m_loaded = 0;
    m_card = 0;
    m_timer = 4.2f;
    fn_80044648(fn_80044254(), 1.0f);
}

Screen::~Screen() {
    if (m_card != 0) {
        delete m_card;
        m_card = 0;
    }
    if (m_started) {
        fn_802C839C(fn_801E5CA8(), 0);
    }
}

bool Screen::Tick() {
    switch (m_state) {
    case STATE_WAIT_CARD:
        WaitCard();
        return 1;
    case STATE_LOAD:
        Load();
        break;
    case STATE_TITLE:
        TitleScreen();
        break;
    case STATE_MAIN:
        MainMenu();
        break;
    case STATE_STORY:
        StoryMenu();
        break;
    case STATE_HARD:
        HardMenu();
        break;
    case STATE_DEMO:
        Demo();
        break;
    case STATE_ADVERTISE:
        Advertise();
        break;
    case STATE_2P:
        Play2P();
        break;
    case STATE_RESTART:
        SetState(STATE_TITLE, true);
        break;
    }
    m_playing = fn_8003CDF4(&m_scenes);
    fn_80355F54(m_effect.get());
    return 1;
}

void Screen::SetState(int state, bool start) {
    m_state = state;
    if (start) {
        m_step = 0;
    } else {
        m_step = 1;
    }
    s_cursor.m_changed = 1;
}

void Screen::Next() {
    SetState(m_nextState, m_nextStart);
}

void Screen::WaitCard() {
    if (fn_802E5810(fn_80117514())) {
        m_state = STATE_LOAD;
    }
}

void Screen::Load() {
    if (LoadProject()) {
        Setup();
        fn_80173834(1, 0.0f);
        m_loaded = 1;
    } else {
        SetState(STATE_ERROR, true);
    }
}

void Screen::TitleScreen() {
    switch (m_step) {
    case 0:
        fn_8003CE5C(&m_scenes);
        fn_8003CEAC(&m_scenes, s_sceneNames[0])->Play("TitleOpen", 1);
        fn_8003C8D8(&m_scenes, s_sceneNames[0], 1);
        fn_8003C8D8(&m_scenes, s_sceneNames[1], 0);
        m_step = 1;
        fn_802C839C(fn_801E5CA8(), 1);
        break;
    case 1:
        if (m_playing) {
            if (lbl_8057D6A0.m_titleWait) {
                m_timer -= lbl_8057D6A0.m_dt;
                if (m_timer > 0.0f) {
                    break;
                }
                lbl_8057D6A0.m_titleWait = 0;
            }
            if (fn_802B7D5C(0, 4)) {
                fn_8003CEAC(&m_scenes, s_sceneNames[0])->Play("TitleClose", 1);
                fn_8003D870(fn_8003CEAC(&m_scenes, s_sceneNames[0]));
                Leave(STATE_MAIN, true);
                PlaySound(0xE003);
            }
        } else {
            Leave(STATE_DEMO, true);
        }
        break;
    case 2:
        if (!m_playing) {
            Next();
        }
        break;
    }
}

void Screen::Advertise() {
    fn_80173878(0, 1, 1.0f);
    if (fn_80044E10() == 1) {
        fn_802C839C(fn_801E5CA8(), 0);
        fn_802B7528(fn_801E5E00(), 0);
    }
}

void Screen::MainMenu() {
    CursorValues &values = s_cursor.m_values;
    switch (m_step) {
    case 0:
        fn_8003CE5C(&m_scenes);
        fn_8003CEAC(&m_scenes, s_sceneNames[1])->Play("MenuOpen", 1);
        fn_8003C8D8(&m_scenes, s_sceneNames[0], 0);
        fn_8003C8D8(&m_scenes, s_sceneNames[1], 1);
        m_step = 1;
        fn_802C839C(fn_801E5CA8(), 3);
        break;
    case 1:
        if (fn_802E5810(fn_80117514())) {
            if (fn_802B7D5C(1, 4)) {
                if (values.m_main == 1 && !CanPlay2P()) {
                    m_step = 3;
                    PlaySound(0xE004);
                } else {
                    m_step = 2;
                    PlaySound(0xE003);
                }
            } else if (fn_802B7D5C(2, 4)) {
                SetState(STATE_TITLE, true);
                PlaySound(0xE004);
            } else if (fn_802B7D5C(3, 4)) {
                if (values.m_main > 0) {
                    values.m_main--;
                    PlaySound(0xE002);
                }
            } else if (fn_802B7D5C(4, 4)) {
                if (values.m_main < 2) {
                    values.m_main++;
                    PlaySound(0xE002);
                }
            }
        }
        if (s_cursor.Changed()) {
            fn_8003CEAC(&m_scenes, s_sceneNames[1])->Play(s_mainItems[values.m_main], 1);
            fn_8003C8D8(&m_scenes, s_sceneNames[0], 0);
            fn_8003C8D8(&m_scenes, s_sceneNames[1], 1);
            s_cursor.Save();
        }
        break;
    case 2:
        switch (values.m_main) {
        case 0:
            SetState(STATE_STORY, true);
            break;
        case 1:
            s_cursor.m_lastState = m_state;
            fn_802B7528(fn_801E5E00(), 7);
            break;
        case 2:
            s_cursor.m_lastState = m_state;
            fn_802B7528(fn_801E5E00(), 2);
            break;
        }
        break;
    case 3:
        if (fn_802E58B4(fn_80117514(), 0x4A, 0, 0) == 2) {
            m_step = 1;
        }
        break;
    }
}

void Screen::StoryMenu() {
    CursorValues &values = s_cursor.m_values;
    int last = m_hardOpen ? 1 : 0;
    switch (m_step) {
    case 0:
        fn_8003CE5C(&m_scenes);
        fn_8003CEAC(&m_scenes, s_sceneNames[1])->Play("1POpen", 1);
        fn_8003C8D8(&m_scenes, s_sceneNames[0], 0);
        fn_8003C8D8(&m_scenes, s_sceneNames[1], 1);
        fn_802C839C(fn_801E5CA8(), 3);
        m_step = 1;
        break;
    case 1:
        if (fn_802B7D5C(1, 4)) {
            m_step = 4;
            PlaySound(0xE003);
        } else if (fn_802B7D5C(2, 4)) {
            fn_8003CEAC(&m_scenes, s_sceneNames[1])->Play("1PClose", 1);
            Leave(STATE_MAIN, false);
            PlaySound(0xE004);
        } else if (fn_802B7D5C(3, 4)) {
            if (values.m_story > 0) {
                values.m_story--;
                PlaySound(0xE002);
            }
        } else if (fn_802B7D5C(4, 4)) {
            if (values.m_story < last + 1) {
                values.m_story++;
                PlaySound(0xE002);
            }
        }
        if (s_cursor.Changed()) {
            fn_8003CEAC(&m_scenes, s_sceneNames[1])->Play(s_storyItems[values.m_story], 1);
            fn_8003C8D8(&m_scenes, s_sceneNames[0], 0);
            fn_8003C8D8(&m_scenes, s_sceneNames[1], 1);
            s_cursor.Save();
        }
        break;
    case 4:
        switch (values.m_story) {
        case 0:
            fn_802B7528(fn_801E5E00(), 3);
            s_cursor.m_lastState = m_state;
            break;
        case 1:
            fn_802B7528(fn_801E5E00(), 6);
            s_cursor.m_lastState = m_state;
            break;
        case 2:
            m_nextState = STATE_HARD;
            m_nextStart = 1;
            break;
        }
        m_step = 2;
        break;
    case 2:
        if (!m_playing) {
            Next();
        }
        break;
    }
}

/* The copy of the last two positions is a block copy: that gives retail's load order. */
void Cursor::Save() {
    m_saved.m_title = m_values.m_title;
    m_saved.m_main = m_values.m_main;
    __memcpy(&m_saved.m_story, &m_values.m_story, 2 * sizeof(s32));
    m_changed = 0;
}

void Screen::HardMenu() {
    CursorValues &values = s_cursor.m_values;
    switch (m_step) {
    case 0:
        fn_8003CE5C(&m_scenes);
        fn_8003CEAC(&m_scenes, s_sceneNames[1])->Play("HardOpen", 1);
        fn_8003C8D8(&m_scenes, s_sceneNames[0], 0);
        fn_8003C8D8(&m_scenes, s_sceneNames[1], 1);
        fn_802C839C(fn_801E5CA8(), 3);
        m_step = 1;
        break;
    case 1:
        if (fn_802B7D5C(1, 4)) {
            s_cursor.m_lastState = m_state;
            PlaySound(0xE003);
            StartHard(values.m_hard != 0);
            Leave(STATE_ADVERTISE, false);
            fn_80051734()->field_0x11C = fn_802B7CE0();
        } else if (fn_802B7D5C(2, 4)) {
            fn_8003CEAC(&m_scenes, s_sceneNames[1])->Play("HardClose", 1);
            Leave(STATE_STORY, false);
            PlaySound(0xE004);
        } else if (fn_802B7D5C(3, 4)) {
            if (values.m_hard > 0) {
                values.m_hard--;
                PlaySound(0xE002);
            }
        } else if (fn_802B7D5C(4, 4)) {
            if (m_canResume && values.m_hard < 1) {
                values.m_hard++;
                PlaySound(0xE002);
            }
        }
        if (s_cursor.Changed()) {
            fn_8003CEAC(&m_scenes, s_sceneNames[1])->Play(s_hardItems[values.m_hard], 1);
            fn_8003C8D8(&m_scenes, s_sceneNames[0], 0);
            fn_8003C8D8(&m_scenes, s_sceneNames[1], 1);
            s_cursor.Save();
        }
        break;
    case 2:
        if (!m_playing) {
            Next();
        }
        break;
    }
}

void Screen::Demo() {
    fn_802B7528(fn_801E5E00(), 10);
}

void Screen::Play2P() {
    switch (m_step) {
    case 0:
    case 1:
        if (CanPlay2P() == true) {
            m_step = 2;
        } else {
            fn_802E585C(fn_80117514(), 0x4A, 0, 0);
        }
        break;
    case 2:
        fn_802B7528(fn_801E5E00(), 7);
        break;
    }
}

bool Screen::Draw() {
    if (m_loaded) {
        if (!fn_802B5434(fn_801E5F0C(), m_project)) {
            return 0;
        }
        if (SameTexList(fn_8036C628(m_project), lbl_805EE110)) {
            return 0;
        }
        fn_80355EB0(m_effect.get(), fn_8036C628(m_project));
        fn_8003C92C(&m_scenes, fn_8036C628(m_project));
    }
    return 1;
}

void Screen::StartHard(bool resume) {
    fn_801E5858(fn_801E2128(), 5);
    StageSequence::Manager::SetNext(6);
    if (resume) {
        lbl_80584720.m_resume = 1;
    } else {
        lbl_80584720.m_resume = 0;
    }
    fn_802062FC(lbl_8057D6A0.m_record);
    fn_800CBBFC(lbl_8057D6A0.field_0x148, 1, 3);
    m_started = 1;
}

bool Screen::LoadProject() {
    m_project = fn_802B5504(fn_801E5F0C(), s_projectName);
    if (m_project != 0) {
        UnkCsdHandle_80042DF0 handle = fn_8036D598(m_project, 0);
        fn_8003CEF8(&m_scenes, handle, s_sceneNames, 2);
        m_effect = boost::shared_ptr<AdvCsdEffect>(new AdvCsdEffect(handle));
        return true;
    }
    return false;
}

void Screen::Setup() {
    fn_8003CE5C(&m_scenes);
    if (s_cursor.m_lastState > 0) {
        SetState(s_cursor.m_lastState, true);
    } else {
        SetState(STATE_TITLE, true);
    }
    s_cursor.m_lastState = -1;
    Refresh();
    if (fn_802B7CC0()) {
        Chao::CSD::RCPtr<Chao::CSD::CNode> node;
        node = fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[1])->GetScene().Get(), "select_button");
        fn_8036AD14(node.Get(), 3);
        node = fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[1])->GetScene().Get(), "back_button");
        fn_8036AD14(node.Get(), 3);
    }
}

void Screen::Refresh() {
    m_started = 0;
    UnkSaveData_800BB74C *save = fn_800BB74C();
    Chao::CSD::RCPtr<Chao::CSD::CNode> hard =
        fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[1])->GetScene().Get(), "hardmode");
    if (save->m_allBest) {
        m_hardOpen = 1;
        fn_8036AD78(hard.Get(), 0);
    } else {
        m_hardOpen = 0;
        fn_8036AD78(hard.Get(), 1);
        if (s_cursor.m_values.m_story > 1) {
            s_cursor.m_values.m_story = 0;
        }
    }
    Chao::CSD::RCPtr<Chao::CSD::CNode> resume =
        fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[1])->GetScene().Get(), "resume");
    if (save->field_0x9C == 0) {
        fn_8036AD78(resume.Get(), 1);
        s_cursor.m_values.m_hard = 0;
        m_canResume = 0;
        if (s_cursor.m_values.m_hard > 0) {
            s_cursor.m_values.m_hard = 0;
        }
    } else {
        fn_8036AD78(resume.Get(), 0);
        s_cursor.m_values.m_hard = 1;
        m_canResume = 1;
    }
}

bool Screen::CanPlay2P() {
    u32 count = 0;
    for (u32 i = 0; i < 2; i++) {
        UnkPad_80051734 *pad = fn_80051734()->m_pads[i];
        if (pad != 0 && pad->m_state != 1) {
            count++;
        }
    }
    return count >= 2;
}

class Phase : public AdvBase {
  public:
    Phase();
    virtual ~Phase();
    virtual bool Tick();
    virtual bool Draw();

    /* 0x4 */ boost::scoped_ptr<Screen> m_screen;
};
SIZE_ASSERT(Phase, 0x8);

Phase::Phase() : m_screen(new Screen) {}

Phase::~Phase() {}

bool Phase::Tick() {
    return m_screen->Tick();
}

bool Phase::Draw() {
    return m_screen->Draw();
}

} // namespace Title
} // namespace AdvReal
