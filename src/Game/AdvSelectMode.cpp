/* AdvReal::SelectMode::Phase (class name from RTTI): the front-end phase of the stage select.
 * Its screen plays the "csdFiles/SelectMode" project: the stages of the two story maps and of the
 * boss list are icons the cursor moves over, the chosen stage shows its picture, mission ranks
 * and keys, and its missions' results are printed beside them; deciding twice starts the stage.
 * The cursor position is kept across visits in s_cursor. The file name, the screen's class name
 * and all function and member names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). The unit's code reaches its .rodata
 * through one base address, so the linker keeps that section whole (see s_rodataPool). */
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <cstdio>

#include "Game/AdvBase.h"
#include "Game/CsdSceneSet.h"
#include "Game/SavedataAnalyzer.h"
#include "Game/StageSequence.h"
#include "Game/StageTable.h"
#include <dolphin/types.h>
#include "size_assert.h"

using Chao::CSD::CNode;
using Chao::CSD::RCPtr;
using SavedataAnalyzer::StoryCheck;

/* The value a project returns from fn_8036D598 (class name unknown; see Game/TGi.h). */
class UnkCsdHandle_80042DF0 {
  public:
    UnkCsdHandle_80042DF0(const UnkCsdHandle_80042DF0 &other);
    ~UnkCsdHandle_80042DF0();

    /* 0x0 */ void *m_object;
};

/* A node's position (struct name unknown; see Game/SavePointSelect.cpp). */
struct UnkCsdPosition_8036AE98 {
    float x;
    float y;
};

/* The save data (struct name unknown; see Game/GoalAction.cpp and Game/AdvStageTable.cpp). */
struct UnkMissionSave_800BB74C {
    /* 0x00 */ u8 m_cleared;
    /* 0x04 */ s32 m_rank; /* 0 to 4 */
    /* 0x08 */ s32 m_minutes;
    /* 0x0C */ s8 m_seconds;
    /* 0x0D */ s8 m_frames;
    /* 0x0E */ u8 field_0xE[0x14 - 0xE];
    /* 0x14 */ s32 m_score;
};

struct UnkStageSave_800BB74C {
    /* 0x00 */ s32 m_plays;
    /* 0x04 */ UnkMissionSave_800BB74C m_missions[3];
    /* 0x4C */ s32 m_keys[5]; /* -1 where the key has not been found */
};
SIZE_ASSERT(UnkStageSave_800BB74C, 0x60);

struct UnkSaveData_800BB74C {
    /* 0x0000 */ u8 field_0x0[0xA8];
    /* 0x00A8 */ UnkStageSave_800BB74C m_stages[59];
    /* 0x1688 */ u8 field_0x1688[0x16CC - 0x1688];
    /* 0x16CC */ s32 m_keys; /* of the 71 */
};

/* The game state (struct name unknown; see Game/AdvTitle.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xCC];
    /* 0x0CC */ u8 m_record[0x148 - 0xCC]; /* (name inferred; see Game/StageSequencePlayOnce.cpp) */
    /* 0x148 */ u8 field_0x148[3]; /* meaning unknown */
    /* 0x14B */ u8 field_0x14B[0x150 - 0x14B];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The stages to play (struct name unknown; see Game/StageSequencePlayOnce.cpp). */
struct UnkStageInfo_80584720 {
    /* 0x000 */ s32 m_stage;
    /* 0x004 */ u8 field_0x4[0x230 - 0x4];
};
SIZE_ASSERT(UnkStageInfo_80584720, 0x230);

extern UnkStageInfo_80584720 lbl_80584720;

/* The pads (struct name unknown; see Game/AdvTitle.cpp). */
struct UnkPads_80051734 {
    /* 0x000 */ u8 field_0x0[0x11C];
    /* 0x11C */ s32 field_0x11C; /* meaning unknown */
};

/* The message source of the menus (class name unknown; see Game/AdvMessage.cpp). */
class UnkManager_801E5D48 {
  public:
    /* 0x0 */ u8 field_0x0[0x8];
    /* 0x8 */ float m_scale; /* of the messages */
};

/* TGiNumberDisplay (class name from RTTI; see Game/TGi.h): draws a string of digits. */
class TGiNumberDisplay {
  public:
    /* Scales the digits by x and y (name inferred). */
    void SetScale(float x, float y) {
        m_scaleX = x;
        m_scaleY = y;
    }

    /* 0x000 */ u8 field_0x0[0x8];
    /* 0x008 */ float m_scaleX;
    /* 0x00C */ float m_scaleY;
};

/* The front-end's number display (struct name unknown; fn_802B61F4 returns the one object). */
struct UnkNumbers_802B61F4 {
    /* 0x0 */ boost::shared_ptr<TGiNumberDisplay> m_display;
};

class UnkManager_801E5E00;

/* A project's texture list (see Game/AdvOption.cpp), and the empty one. */
extern RCPtr<Chao::CSD::CTexList> lbl_805EE110;

extern "C" {
void *memset(void *dst, int value, unsigned long count);
/* The project's texture list (behaviour inferred; see Game/AdvOption.cpp). */
RCPtr<Chao::CSD::CTexList> fn_8036C628(void *project);

UnkSaveData_800BB74C *fn_800BB74C(void);
UnkPads_80051734 *fn_80051734(void);

/* The front-end's CSD folder: releasing what is loaded (behaviour inferred), loading a project,
 * and whether it is ready (see Game/AdvTitle.cpp). */
void *fn_801E5F0C(void);
void fn_802B57FC(void *folder);
void *fn_802B5504(void *folder, const char *name);
bool fn_802B5434(void *folder, void *project);

/* Projects and nodes (see Game/TGi.h): the root handle, a node by name, a node's pattern, hiding
 * it, its colours (fn_8036ACCC sets them, fn_8036ACA8 restores them), its position, its scale
 * along x and y, and the word fn_8036AEE0 sets (behaviour inferred). */
UnkCsdHandle_80042DF0 fn_8036D598(void *project, int arg1);
RCPtr<CNode> fn_803666B4(Chao::CSD::CScene *scene, const char *name);
void fn_8036AD14(CNode *node, int pattern);
void fn_8036AD78(CNode *node, int hidden);
void fn_8036ACCC(CNode *node, const u32 *color);
void fn_8036ACA8(CNode *node);
UnkCsdPosition_8036AE98 fn_8036AE98(CNode *node);
void fn_8036AE70(CNode *node, const UnkCsdPosition_8036AE98 *position);
void fn_8036A2CC(CNode *node, float scale);
void fn_8036A278(CNode *node, float scale);
void fn_8036AEE0(CNode *node, int value);

/* Menu input (see Game/AdvTitle.cpp; 5 is left and 6 right). */
bool fn_802B7D5C(int button, int pad);
bool fn_802B7CC0(void);
s32 fn_802B7CE0(void);

/* The stage tables (see Game/AdvStageTable.cpp). */
s32 fn_802BF3C0(int x, int y, bool cleared);
s32 fn_802BF2EC(int x, int y, bool cleared);
s32 fn_802BF244(int index, bool cleared);

/* The number of keys of a stage found (behaviour inferred). */
int fn_801E5700(UnkStageSave_800BB74C *stage);

/* The background music (see Game/AdvBgm.cpp). */
void *fn_801E5CA8(void);
void fn_802C839C(void *player, int track);

/* The phase manager (see Game/AdvMemoryCheck.cpp). */
UnkManager_801E5E00 *fn_801E5E00(void);
void fn_802B7528(UnkManager_801E5E00 *manager, int id);

/* The message source (see Game/AdvMessage.cpp). */
UnkManager_801E5D48 *fn_801E5D48(void);
bool fn_802BEEC8(UnkManager_801E5D48 *self, s32 id, s32 branch, float x, float y, bool right);
bool fn_802BEC5C(UnkManager_801E5D48 *self, int color, float alpha);

/* The number display, and drawing text at (x, y) (see Game/TGi.h). */
UnkNumbers_802B61F4 *fn_802B61F4(void);
void fn_800BC7BC(TGiNumberDisplay *display, const char *text, float x, int arg3, float y);

/* Sound effects (see Game/AdvTitle.cpp). */
void *fn_80053164(void);
void fn_801D560C(void *sound, int id, int arg2, int arg3);

/* Screen fades (see Game/ScreenFade.h). */
int fn_80044E10(void);
void fn_80173834(int arg0, float time);
void fn_80173878(int arg0, int arg1, float time);

/* Starting a game (see Game/AdvTitle.cpp). */
void *fn_801E2128(void);
void fn_801E5858(void *manager, int system);
void fn_802062FC(void *record);
void fn_800CBBFC(void *dest, int value, int count);
}

/* The CSD pointers' comparison (name inferred; see Game/AdvMessage.cpp). */
namespace Chao {
namespace CSD {
template <class T> inline int operator==(const RCPtr<T> &a, const RCPtr<T> &b) {
    if (a.m_object != 0 && b.m_object != 0) {
        return a.m_object->m_object == b.m_object->m_object;
    }
    return a.m_object == b.m_object;
}
} // namespace CSD
} // namespace Chao

/* The empty node pointer (see Game/AdvMessage.cpp). */
extern RCPtr<CNode> lbl_805EE11C;

/* Deletes the story check (name inferred); its destructor keeps its fn_ name. */
inline void DeleteStoryCheck(StoryCheck *check) {
    fn_800BBC04(check, 1);
}

/* Retail keeps checked_delete<StoryCheck> out of line (see checked_delete<Screen> below). */
namespace boost {
template <> inline void checked_delete<StoryCheck>(StoryCheck *x) {
    DeleteStoryCheck(x);
}
} // namespace boost

namespace AdvReal {
namespace SelectMode {

static const char *s_projectName = "csdFiles/SelectMode";

/* The colours of the story map's icons, the boss list's icons and the map's lines. */
static u32 s_storyColor = 0xFFFFFFFF;
static u32 s_bossColor = 0xFFFFFFFF;
static u32 s_linesColor = 0x007CB0FF;

/* The project's scenes: the frame, the icons of the first map and the boss list, and the stars,
 * icons of the second map and the cursor. */
static const char *const s_sceneNames[3] = {"SelectMode", "IconsBG", "Star"};

/* The picture of each stage (a pattern of mission_picture). */
static const s32 s_pictures[59] = {
    0,  0,  0,  0,  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 0,  22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
    34, 35, 36, 37, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
};

/* The rank of each mission (dark, normal, hero), and the keys. */
static const char *s_rankNames[3] = {"rank_dark", "rank_normal", "rank_hero"};
static const char *s_keys[5] = {"key1", "key2", "key3", "key4", "key5"};

/* The cursor position (struct name inferred): a place of the maps (the stage tables' first and
 * second index), on the second map, and an entry of the boss list. */
struct CursorValues {
    CursorValues &operator=(const CursorValues &other) {
        m_x = other.m_x;
        m_y = other.m_y;
        m_secondMap = other.m_secondMap;
        m_boss = other.m_boss;
        m_bossList = other.m_bossList;
        return *this;
    }

    /* 0x00 */ s32 m_x;
    /* 0x04 */ s32 m_y;
    /* 0x08 */ u8 m_secondMap;
    /* 0x0C */ s32 m_boss;
    /* 0x10 */ u8 m_bossList;
};

/* The cursor, kept across visits of the phase (struct name inferred; as AdvReal::Title's). */
struct Cursor {
    Cursor() {
        Clear();
    }

    void Clear() {
        memset(&m_values, 0, sizeof(m_values));
        Save();
        m_lastState = -1;
    }

    void Save() {
        m_saved = m_values;
        m_changed = 0;
    }

    /* 0x00 */ CursorValues m_values;
    /* 0x14 */ CursorValues m_saved;
    /* 0x28 */ u8 m_changed;
    /* 0x2C */ s32 m_lastState;
};
SIZE_ASSERT(Cursor, 0x30);

/* What DrawTexts prints (struct name inferred). */
struct TextState {
    /* 0x0 */ s32 m_mode; /* 0 nothing, 1 a stage of the first map, 2 of the second map or the
                           * boss list, 3 the keys found */
    /* 0x4 */ s32 m_stage;
    /* 0x8 */ s32 m_wait; /* frames since the scenes stopped */
};

/* The phase's state (class name inferred). */
class Screen {
  public:
    Screen();
    ~Screen();

    bool Tick();
    bool Draw();

    void StartStage();
    void Setup();
    bool LoadProject();
    void Leave();
    void Back();
    void DrawTexts();
    void ShowIcons(bool plain, bool plainLines);
    void ShowKeys(int index);
    void ShowStage(int index, s32 id, bool selected, bool keys);
    void ShowStage2(int index, s32 id, bool selected, bool keys);
    void ShowBoss(int index, s32 id, int number, bool selected, bool keys);
    void UpdateMap(bool keys);
    void Select();
    void Open();
    void Next();
    void SetState(int state, bool start);

    /* Moves on to state once the scenes have played (name inferred; as AdvReal::Title's). */
    void Leave(int state, bool start) {
        m_nextState = state;
        m_nextStart = start;
        m_step = 2;
    }

    /* 0x00 */ s32 m_state; /* 0 open, 1 select, 2 back, 3 leave */
    /* 0x04 */ s32 m_nextState;
    /* 0x08 */ s32 m_step;
    /* 0x0C */ void *m_project;
    /* 0x10 */ UnkCsdSceneSet_8003D5D8 m_scenes;
    /* 0x1C */ u8 m_playing;
    /* 0x1D */ bool m_nextStart;
    /* 0x20 */ boost::shared_ptr<StoryCheck> m_check;
    /* 0x28 */ TextState m_text;
};
SIZE_ASSERT(Screen, 0x34);

/* Deletes the screen (name inferred; see checked_delete below). */
inline void DeleteScreen(Screen *screen) {
    delete screen;
}

} // namespace SelectMode
} // namespace AdvReal

/* Retail keeps boost::checked_delete<Screen> out of line and the destructor at its own place;
 * this specialization gives that under GC/2.7 (see docs/research-notes.md, EnergyCore). */
namespace boost {
template <>
inline void checked_delete<AdvReal::SelectMode::Screen>(AdvReal::SelectMode::Screen *x) {
    AdvReal::SelectMode::DeleteScreen(x);
}
} // namespace boost

namespace AdvReal {
namespace SelectMode {

static Cursor s_cursor;

/* Plays the menu sound id (name inferred). */
inline void PlaySound(int id) {
    fn_801D560C(fn_80053164(), id, 0, 0);
}

/* Builds the story check from the save data (name inferred). */
inline StoryCheck *NewStoryCheck() {
    StoryCheck *check = (StoryCheck *)operator new(sizeof(StoryCheck));
    if (check != 0) {
        check = fn_801AC1B4(check, fn_800BB74C());
    }
    return check;
}

/* Puts the cursor on the first stage. */
static void ResetCursor() {
    s_cursor.m_values.m_x = 0;
    s_cursor.m_values.m_y = 2;
    s_cursor.m_values.m_secondMap = 0;
    s_cursor.m_values.m_boss = 5;
    s_cursor.m_values.m_bossList = 0;
}

Screen::Screen() : m_state(0), m_check(boost::shared_ptr<StoryCheck>(NewStoryCheck())) {
    memset(&m_text, 0, sizeof(m_text));
    fn_80173834(1, 0.0f);
    fn_801ABDB8(m_check.get());
}

Screen::~Screen() {}

bool Screen::Tick() {
    switch (m_state) {
    case 0:
        Open();
        break;
    case 1:
        Select();
        break;
    case 2:
        Back();
        break;
    case 3:
        Leave();
        break;
    }
    m_playing = fn_8003CDF4(&m_scenes);
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
    if (m_state == 3) {
        fn_80173878(0, 1, 1.0f);
    }
}

void Screen::Next() {
    SetState(m_nextState, m_nextStart);
}

void Screen::Open() {
    if (LoadProject()) {
        Setup();
        SetState(1, true);
        UpdateMap(true);
        fn_802C839C(fn_801E5CA8(), 3);
    } else {
        SetState(4, true);
    }
}

void Screen::Select() {
    switch (m_step) {
    case 0:
        fn_8003C8D8(&m_scenes, s_sceneNames[0], 1);
        fn_8003C8D8(&m_scenes, s_sceneNames[1], 1);
        fn_8003C8D8(&m_scenes, s_sceneNames[2], 1);
        fn_8003CEAC(&m_scenes, s_sceneNames[0])->Play("HiLiteOpen", 1);
        fn_8003CEAC(&m_scenes, s_sceneNames[1])->Play("HiLiteOpen", 1);
        fn_8003CEAC(&m_scenes, s_sceneNames[2])->Play("HiLiteOpen", 1);
        ShowIcons(true, true);
        m_step = 3;
        break;
    case 3:
        if (!m_playing) {
            s32 x = s_cursor.m_values.m_x;
            s32 y = s_cursor.m_values.m_y;
            u8 secondMap = s_cursor.m_values.m_secondMap;
            s32 boss = s_cursor.m_values.m_boss;
            u8 bossList = s_cursor.m_values.m_bossList;
            if (fn_802B7D5C(3, 4)) {
                if (bossList) {
                    if (--boss < 0) {
                        boss = 0;
                    }
                } else if (secondMap) {
                    secondMap = false;
                } else {
                    y--;
                    if (fn_802BF2EC(x, y, true) != -1) {
                        secondMap = true;
                    } else {
                        secondMap = false;
                    }
                }
            } else if (fn_802B7D5C(4, 4)) {
                if (bossList) {
                    if (++boss > 9) {
                        boss = 9;
                    }
                } else if (secondMap) {
                    y++;
                    if (fn_802BF3C0(x, y, false) != -1) {
                        secondMap = false;
                    }
                } else if (fn_802BF2EC(x, y, true) != -1) {
                    secondMap = true;
                } else {
                    y++;
                }
            } else if (fn_802B7D5C(5, 4)) {
                if (bossList) {
                    bossList = false;
                } else {
                    x--;
                    if (secondMap && fn_802BF3C0(x, y, false) != -1) {
                        secondMap = false;
                    }
                }
            } else if (fn_802B7D5C(6, 4)) {
                if (!bossList && x == 5) {
                    bossList = true;
                } else {
                    x++;
                    if (secondMap && fn_802BF3C0(x, y, false) != -1) {
                        secondMap = false;
                    }
                }
            } else if (fn_802B7D5C(1, 4)) {
                UnkSaveData_800BB74C *save = fn_800BB74C();
                bool played = false;
                if (bossList) {
                    if (save->m_stages[fn_802BF244(boss, false)].m_plays > 0) {
                        played = true;
                    }
                } else if (save->m_stages[fn_802BF3C0(x, y, false)].m_plays > 0) {
                    played = true;
                }
                if (played) {
                    fn_8003CEAC(&m_scenes, s_sceneNames[0])->Play("SelectOpen", 1);
                    fn_8003CEAC(&m_scenes, s_sceneNames[1])->Play("SelectOpen", 1);
                    fn_8003CEAC(&m_scenes, s_sceneNames[2])->Play("SelectOpen", 1);
                    ShowIcons(true, false);
                    m_step = 4;
                    PlaySound(0xE003);
                } else {
                    PlaySound(0xE005);
                }
            } else if (fn_802B7D5C(2, 4)) {
                Leave(2, true);
                PlaySound(0xE004);
            }
            if (x != s_cursor.m_values.m_x || y != s_cursor.m_values.m_y ||
                secondMap != s_cursor.m_values.m_secondMap || boss != s_cursor.m_values.m_boss ||
                bossList != s_cursor.m_values.m_bossList) {
                bool moved = false;
                if (fn_802BF3C0(x, y, false) != -1) {
                    s_cursor.m_values.m_x = x;
                    s_cursor.m_values.m_y = y;
                    s_cursor.m_values.m_secondMap = secondMap;
                    moved = true;
                }
                if (bossList != s_cursor.m_values.m_bossList) {
                    if (bossList) {
                        boss = s_cursor.m_values.m_boss = s_cursor.m_values.m_y * 2;
                    } else {
                        s_cursor.m_values.m_y = s_cursor.m_values.m_boss / 2;
                        boss = s_cursor.m_values.m_boss;
                    }
                    s_cursor.m_values.m_bossList = bossList;
                    moved = true;
                }
                if (boss != s_cursor.m_values.m_boss) {
                    s_cursor.m_values.m_boss = boss;
                    moved = true;
                }
                if (moved) {
                    PlaySound(0xE002);
                }
            }
        }
        UpdateMap(false);
        break;
    case 4:
        if (!m_playing) {
            if (fn_802B7D5C(1, 4)) {
                PlaySound(0xE003);
                StartStage();
                SetState(3, true);
                fn_80051734()->field_0x11C = fn_802B7CE0();
            } else if (fn_802B7D5C(2, 4)) {
                m_step = 3;
                fn_8003CEAC(&m_scenes, s_sceneNames[0])->Play("SelectClose", 1);
                fn_8003CEAC(&m_scenes, s_sceneNames[1])->Play("SelectClose", 1);
                fn_8003CEAC(&m_scenes, s_sceneNames[2])->Play("SelectClose", 1);
                ShowIcons(false, false);
                PlaySound(0xE004);
                m_text.m_mode = 0;
            }
        }
        UpdateMap(true);
        break;
    case 2:
        fn_8036AEE0(fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[2])->GetScene().Get(), "cursor")
                        .Get(),
                    0);
        fn_8003CEAC(&m_scenes, s_sceneNames[0])->Play("HiLiteClose", 1);
        fn_8003CEAC(&m_scenes, s_sceneNames[1])->Play("HiLiteClose", 1);
        fn_8003CEAC(&m_scenes, s_sceneNames[2])->Play("HiLiteClose", 1);
        ShowIcons(true, true);
        m_text.m_mode = 0;
        Next();
        break;
    }
}

/* Shows every stage of the maps and the boss list, the one under the cursor chosen. */
void Screen::UpdateMap(bool keys) {
    m_text.m_mode = 3;
    for (int x = 0; x < 6; x++) {
        for (int y = 0; y < 5; y++) {
            int index = fn_802BF2EC(x, y, false);
            if (index != -1) {
                char name[32];
                fn_800B74B4();
                sprintf(name, "%04d", lbl_804C4A18[index].m_id);
                RCPtr<CNode> icon =
                    fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[2])->GetScene().Get(), name);
                if (icon == lbl_805EE11C) {
                    continue;
                }
                if (fn_802BF2EC(x, y, true) == -1) {
                    fn_8036AD78(icon.Get(), 1);
                } else {
                    fn_8036AD78(icon.Get(), 0);
                }
            }
            index = fn_802BF3C0(x, y, false);
            if (index != -1) {
                fn_800B74B4();
                const UnkStageInfo_804C4A18 *info = &lbl_804C4A18[index];
                if (info != 0) {
                    bool selected = false;
                    if (x == s_cursor.m_values.m_x && y == s_cursor.m_values.m_y) {
                        selected = true;
                    }
                    bool chosen = selected;
                    if (s_cursor.m_values.m_secondMap || s_cursor.m_values.m_bossList) {
                        chosen = false;
                    }
                    ShowStage(index, info->m_id, chosen, keys);
                }
            }
            index = fn_802BF2EC(x, y, true);
            if (index != -1) {
                fn_800B74B4();
                const UnkStageInfo_804C4A18 *info = &lbl_804C4A18[index];
                if (info != 0) {
                    bool selected = false;
                    if (x == s_cursor.m_values.m_x && y == s_cursor.m_values.m_y) {
                        selected = true;
                    }
                    bool chosen = selected;
                    if (!s_cursor.m_values.m_secondMap) {
                        chosen = false;
                    }
                    ShowStage2(index, info->m_id, chosen, keys);
                }
            }
        }
    }
    for (int i = 0; i < 10; i++) {
        int index = fn_802BF244(i, false);
        if (index != -1) {
            fn_800B74B4();
            const UnkStageInfo_804C4A18 *info = &lbl_804C4A18[index];
            if (info != 0) {
                ShowBoss(index, info->m_id, i,
                         s_cursor.m_values.m_bossList && i == s_cursor.m_values.m_boss, keys);
            }
        }
    }
}

/* Shows entry number of the boss list, and its picture, rank and keys when it is chosen. */
void Screen::ShowBoss(int index, s32 id, int number, bool selected, bool keys) {
    char name[32];
    sprintf(name, "%d", number + 1);
    RCPtr<CNode> icon =
        fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[1])->GetScene().Get(), name);
    if (icon == lbl_805EE11C) {
        return;
    }
    RCPtr<CNode> cursor =
        fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[2])->GetScene().Get(), "cursor");
    if (selected) {
        RCPtr<CNode> picture =
            fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(),
                        "mission_picture");
        fn_8036AD14(picture.Get(), s_pictures[index]);
        fn_8036AD78(cursor.Get(), 0);
        fn_8036AE98(icon.Get());
        UnkCsdPosition_8036AE98 position = fn_8036AE98(icon.Get());
        fn_8036AE70(cursor.Get(), &position);
        fn_8036A2CC(cursor.Get(), 0.5f);
        fn_8036A278(cursor.Get(), 0.5f);
        RCPtr<CNode> text =
            fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(), "text");
        fn_8036AD14(text.Get(), 1);
        fn_8036AD78(fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(), "key_no")
                        .Get(),
                    1);
        for (int i = 0; i < 3; i++) {
            fn_8036AD78(
                fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(),
                            s_rankNames[i])
                    .Get(),
                1);
        }
        UnkStageSave_800BB74C *stage = &fn_800BB74C()->m_stages[index];
        fn_8036AD78(
            fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(),
                        s_rankNames[2]).Get(),
            0);
        if (stage->m_missions[1].m_cleared) {
            switch (stage->m_missions[1].m_rank) {
            case 0:
                fn_8036AD14(fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(),
                                        s_rankNames[2])
                                .Get(),
                            0);
                break;
            case 1:
                fn_8036AD14(fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(),
                                        s_rankNames[2])
                                .Get(),
                            1);
                break;
            case 2:
                fn_8036AD14(fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(),
                                        s_rankNames[2])
                                .Get(),
                            2);
                break;
            case 3:
                fn_8036AD14(fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(),
                                        s_rankNames[2])
                                .Get(),
                            3);
                break;
            case 4:
                fn_8036AD14(fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(),
                                        s_rankNames[2])
                                .Get(),
                            4);
                break;
            }
        } else {
            fn_8036AD14(
                fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(),
                            s_rankNames[2])
                    .Get(),
                5);
        }
        if (keys) {
            ShowKeys(index);
            m_text.m_mode = 2;
            m_text.m_stage = index;
        }
    }
}

/* Shows a stage of the second map, and its picture, rank and keys when it is chosen. */
void Screen::ShowStage2(int index, s32 id, bool selected, bool keys) {
    char name[32];
    sprintf(name, "%04d", id);
    RCPtr<CNode> icon =
        fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[2])->GetScene().Get(), name);
    if (icon == lbl_805EE11C) {
        return;
    }
    RCPtr<CNode> cursor =
        fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[2])->GetScene().Get(), "cursor");
    if (selected) {
        RCPtr<CNode> picture =
            fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(),
                        "mission_picture");
        fn_8036AD14(picture.Get(), s_pictures[index]);
        fn_8036AD78(cursor.Get(), 0);
        fn_8036AE98(icon.Get());
        UnkCsdPosition_8036AE98 position = fn_8036AE98(icon.Get());
        fn_8036AE70(cursor.Get(), &position);
        fn_8036A2CC(cursor.Get(), 0.5f);
        fn_8036A278(cursor.Get(), 0.5f);
        RCPtr<CNode> text =
            fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(), "text");
        fn_8036AD14(text.Get(), 1);
        fn_8036AD78(fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(), "key_no")
                        .Get(),
                    1);
        for (int i = 0; i < 3; i++) {
            fn_8036AD78(
                fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(),
                            s_rankNames[i])
                    .Get(),
                1);
        }
        UnkStageSave_800BB74C *stage = &fn_800BB74C()->m_stages[index];
        fn_8036AD78(
            fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(),
                        s_rankNames[2]).Get(),
            0);
        if (stage->m_missions[1].m_cleared) {
            switch (stage->m_missions[1].m_rank) {
            case 0:
                fn_8036AD14(fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(),
                                        s_rankNames[2])
                                .Get(),
                            0);
                break;
            case 1:
                fn_8036AD14(fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(),
                                        s_rankNames[2])
                                .Get(),
                            1);
                break;
            case 2:
                fn_8036AD14(fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(),
                                        s_rankNames[2])
                                .Get(),
                            2);
                break;
            case 3:
                fn_8036AD14(fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(),
                                        s_rankNames[2])
                                .Get(),
                            3);
                break;
            case 4:
                fn_8036AD14(fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(),
                                        s_rankNames[2])
                                .Get(),
                            4);
                break;
            }
        } else {
            fn_8036AD14(
                fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(),
                            s_rankNames[2])
                    .Get(),
                5);
        }
        if (keys) {
            ShowKeys(index);
            m_text.m_mode = 2;
            m_text.m_stage = index;
        }
    }
}

/* Shows a stage of the first map: its star once every mission has the best rank, its picture
 * once fn_801AC1A4 says so, and its picture, ranks and keys when it is chosen. */
void Screen::ShowStage(int index, s32 id, bool selected, bool keys) {
    char star[32];
    char picture[32];
    sprintf(star, "star%04d", id);
    sprintf(picture, "pic%04d", id);
    RCPtr<CNode> starNode =
        fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[2])->GetScene().Get(), star);
    if (starNode == lbl_805EE11C) {
        return;
    }
    RCPtr<CNode> pictureNode =
        fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[2])->GetScene().Get(), picture);
    if (pictureNode == lbl_805EE11C) {
        return;
    }
    if (fn_801AC1A4(m_check.get(), index) & 1) {
        fn_8036AD78(pictureNode.Get(), 0);
    } else {
        fn_8036AD78(pictureNode.Get(), 1);
    }
    UnkSaveData_800BB74C *save = fn_800BB74C();
    fn_800B74B4();
    const UnkStageInfo_804C4A18 *info = &lbl_804C4A18[index];
    if (!(save == 0 ||
          (info->m_missions[0].m_id != -1 && save->m_stages[index].m_missions[0].m_rank != 0) ||
          (info->m_missions[1].m_id != -1 && save->m_stages[index].m_missions[1].m_rank != 0) ||
          (info->m_missions[2].m_id != -1 && save->m_stages[index].m_missions[2].m_rank != 0))) {
        fn_8036AD78(starNode.Get(), 0);
    } else {
        fn_8036AD78(starNode.Get(), 1);
    }
    RCPtr<CNode> cursor =
        fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[2])->GetScene().Get(), "cursor");
    if (selected) {
        RCPtr<CNode> missionPicture =
            fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(),
                        "mission_picture");
        fn_8036AD14(missionPicture.Get(), s_pictures[index]);
        fn_8036AD78(cursor.Get(), 0);
        fn_8036AE98(starNode.Get());
        UnkCsdPosition_8036AE98 position = fn_8036AE98(starNode.Get());
        fn_8036AE70(cursor.Get(), &position);
        fn_8036A2CC(cursor.Get(), 1.0f);
        fn_8036A278(cursor.Get(), 1.0f);
        UnkStageSave_800BB74C *stage = &fn_800BB74C()->m_stages[index];
        fn_8036AD14(fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(), "key_no")
                        .Get(),
                    fn_801E5700(stage));
        fn_8036AD78(fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(), "key_no")
                        .Get(),
                    0);
        RCPtr<CNode> text =
            fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(), "text");
        fn_8036AD14(text.Get(), 0);
        for (int i = 0; i < 3; i++) {
            fn_8036AD78(
                fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(),
                            s_rankNames[i])
                    .Get(),
                0);
            if (!stage->m_missions[i].m_cleared) {
                fn_8036AD14(fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(),
                                        s_rankNames[i])
                                .Get(),
                            5);
            } else {
                switch (stage->m_missions[i].m_rank) {
                case 0:
                    fn_8036AD14(
                        fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(),
                                    s_rankNames[i])
                            .Get(),
                        0);
                    break;
                case 1:
                    fn_8036AD14(
                        fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(),
                                    s_rankNames[i])
                            .Get(),
                        1);
                    break;
                case 2:
                    fn_8036AD14(
                        fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(),
                                    s_rankNames[i])
                            .Get(),
                        2);
                    break;
                case 3:
                    fn_8036AD14(
                        fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(),
                                    s_rankNames[i])
                            .Get(),
                        3);
                    break;
                case 4:
                    fn_8036AD14(
                        fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(),
                                    s_rankNames[i])
                            .Get(),
                        4);
                    break;
                }
            }
        }
        if (keys) {
            ShowKeys(index);
            m_text.m_mode = 1;
            m_text.m_stage = index;
        }
        fn_8036A2CC(pictureNode.Get(), 1.0f);
        fn_8036A278(pictureNode.Get(), 1.0f);
    } else {
        fn_8036A2CC(pictureNode.Get(), 0.5f);
        fn_8036A278(pictureNode.Get(), 0.5f);
    }
}

/* Shows which keys of stage index have been found (none past the first map's stages). */
void Screen::ShowKeys(int index) {
    UnkStageSave_800BB74C *stage = &fn_800BB74C()->m_stages[index];
    if (index < 28) {
        for (int i = 0; i < 5; i++) {
            fn_8036AEE0(
                fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(),
                            s_keys[i]).Get(),
                i < fn_801E5700(stage));
        }
    } else {
        for (int i = 0; i < 5; i++) {
            fn_8036AEE0(
                fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(),
                            s_keys[i]).Get(),
                0);
        }
    }
}

/* Colours the icons of the first map and the boss list, and the map's lines, or restores them. */
void Screen::ShowIcons(bool plain, bool plainLines) {
    char name[32];
    int i;
    for (int x = 0; x < 6; x++) {
        for (int y = 0; y < 5; y++) {
            int index = fn_802BF3C0(x, y, false);
            if (index != -1) {
                fn_800B74B4();
                const UnkStageInfo_804C4A18 *info = &lbl_804C4A18[index];
                if (info != 0) {
                    sprintf(name, "%04d", info->m_id);
                    if (plain) {
                        fn_8036ACA8(
                            fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[1])->GetScene().Get(),
                                        name)
                                .Get());
                    } else {
                        fn_8036ACCC(
                            fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[1])->GetScene().Get(),
                                        name)
                                .Get(),
                            &s_storyColor);
                    }
                }
            }
        }
    }
    for (i = 0; i < 10; i++) {
        int index = fn_802BF244(i, false);
        if (index != -1) {
            fn_800B74B4();
            const UnkStageInfo_804C4A18 *info = &lbl_804C4A18[index];
            if (info != 0) {
                sprintf(name, "%d", i + 1);
                if (plain) {
                    fn_8036ACA8(
                        fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[1])->GetScene().Get(), name)
                            .Get());
                } else {
                    fn_8036ACCC(
                        fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[1])->GetScene().Get(), name)
                            .Get(),
                        &s_bossColor);
                }
            }
        }
    }
    if (plainLines) {
        fn_8036ACA8(
            fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(),
                        "Null_lines").Get());
    } else {
        fn_8036ACCC(
            fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(),
                        "Null_lines").Get(),
            &s_linesColor);
    }
}

/* A value in screen units (name inferred). As a call it is not a constant expression, so the
 * statics below are set when DrawTexts first runs, and the messages' heights keep their
 * multiplication by Pixels(1.0f), as in retail. */
inline float Pixels(float value) {
    return value;
}

/* A message and where it is printed (struct name inferred). */
struct TextPlace {
    /* 0x0 */ s32 m_id;
    /* 0x4 */ float m_x;
    /* 0x8 */ float m_y;
};

/* Prints the chosen stage's name and its missions' ranks, scores and times, or the number of keys
 * found. */
void Screen::DrawTexts() {
    static float s_messageScale = Pixels(0.03f);
    static float s_numberScaleX = Pixels(0.45f);
    static float s_numberScaleY = Pixels(0.8f);
    static float s_numberOffset = Pixels(2.0f);
    static float s_left = Pixels(160.0f);
    static float s_top = Pixels(310.0f);
    static float s_width = Pixels(110.0f);
    static float s_height = Pixels(25.0f);
    static float s_keysX = Pixels(440.0f);
    static float s_keysY = Pixels(80.0f);
    TextPlace places[16] = {
        {0x170C, s_left - s_width, s_top},
        {0x16A9, s_left + s_width, s_top},
        {0x16AA, s_left + s_width * 2.0f, s_top},
        {0x16A8, s_left + s_width * 3.0f, s_top},
        {0x16B4, s_left, s_top + s_height},
        {0x16B0, s_left + s_width, s_top + s_height},
        {0x16B0, s_left + s_width * 2.0f, s_top + s_height},
        {0x16B0, s_left + s_width * 3.0f, s_top + s_height},
        {0x16B1, s_left, s_top + s_height * 2.0f},
        {0x16B0, s_left + s_width, s_top + s_height * 2.0f},
        {0x16B0, s_left + s_width * 2.0f, s_top + s_height * 2.0f},
        {0x16B0, s_left + s_width * 3.0f, s_top + s_height * 2.0f},
        {0x16B2, s_left, s_top + s_height * 3.0f},
        {0x16B0, s_left + s_width, s_top + s_height * 3.0f},
        {0x16B0, s_left + s_width * 2.0f, s_top + s_height * 3.0f},
        {0x16B0, s_left + s_width * 3.0f, s_top + s_height * 3.0f},
    };
    TextPlace *place;
    char score[32];
    char time[32];
    char time2[32];
    char keys[32];
    fn_802BEC5C(fn_801E5D48(), 0, 1.0f);
    fn_801E5D48()->m_scale = s_messageScale;
    fn_802B61F4()->m_display->SetScale(s_numberScaleX, s_numberScaleY);
    UnkSaveData_800BB74C *save = fn_800BB74C();
    UnkStageSave_800BB74C *stage = &save->m_stages[m_text.m_stage];
    switch (m_text.m_mode) {
    case 1:
        fn_802BEEC8(fn_801E5D48(), places[0].m_id + m_text.m_stage, 1, places[0].m_x,
                    places[0].m_y * Pixels(1.0f), false);
        fn_802BEEC8(fn_801E5D48(), places[4].m_id, 1, places[4].m_x,
                    places[4].m_y * Pixels(1.0f), false);
        fn_802BEEC8(fn_801E5D48(), places[8].m_id, 1, places[8].m_x,
                    places[8].m_y * Pixels(1.0f), false);
        fn_802BEEC8(fn_801E5D48(), places[12].m_id, 1, places[12].m_x,
                    places[12].m_y * Pixels(1.0f), false);
        for (int i = 0; i < 3; i++) {
            place = &places[i + 1];
            fn_802BEEC8(fn_801E5D48(), place->m_id, 1, place->m_x,
                        place->m_y * Pixels(1.0f), false);
            if (stage->m_missions[i].m_cleared) {
                fn_802BEEC8(fn_801E5D48(), stage->m_missions[i].m_rank + 0x16AB, 1, place[4].m_x,
                            place[4].m_y * Pixels(1.0f), false);
                sprintf(score, "%8d", stage->m_missions[i].m_score);
                fn_800BC7BC(fn_802B61F4()->m_display.get(), score, place[8].m_x, 0,
                            s_numberOffset + (place[8].m_y - 18.0f * s_numberScaleY));
                if (stage->m_missions[i].m_minutes == 99 && stage->m_missions[i].m_seconds == 59 &&
                    stage->m_missions[i].m_frames == 98) {
                    sprintf(time, "99:59:99");
                } else {
                    sprintf(time, "%02d:%02d:%02d", stage->m_missions[i].m_minutes,
                            stage->m_missions[i].m_seconds,
                            stage->m_missions[i].m_frames);
                }
                fn_800BC7BC(fn_802B61F4()->m_display.get(), time, place[12].m_x, 0,
                            s_numberOffset + (place[12].m_y - 18.0f * s_numberScaleY));
            } else {
                fn_802BEEC8(fn_801E5D48(), 0x16B0, 1, place[4].m_x,
                            place[4].m_y * Pixels(1.0f), false);
                fn_802BEEC8(fn_801E5D48(), 0x16B0, 1, place[8].m_x,
                            place[8].m_y * Pixels(1.0f), false);
                fn_802BEEC8(fn_801E5D48(), 0x16B0, 1, place[12].m_x,
                            place[12].m_y * Pixels(1.0f), false);
            }
        }
        break;
    case 2: {
        fn_802BEEC8(fn_801E5D48(), places[0].m_id + m_text.m_stage, 1, places[0].m_x,
                    places[0].m_y * Pixels(1.0f), false);
        fn_802BEEC8(fn_801E5D48(), places[4].m_id, 1, places[4].m_x,
                    places[4].m_y * Pixels(1.0f), false);
        fn_802BEEC8(fn_801E5D48(), places[8].m_id, 1, places[8].m_x,
                    places[8].m_y * Pixels(1.0f), false);
        fn_802BEEC8(fn_801E5D48(), places[12].m_id, 1, places[12].m_x,
                    places[12].m_y * Pixels(1.0f), false);
        UnkMissionSave_800BB74C *mission = &stage->m_missions[1];
        fn_802BEEC8(fn_801E5D48(), mission->m_rank + 0x16AB, 1, places[5].m_x,
                    places[5].m_y * Pixels(1.0f), false);
        fn_802BEEC8(fn_801E5D48(), 0x16B0, 1, places[9].m_x, places[9].m_y * Pixels(1.0f), false);
        if (mission->m_minutes == 99 && mission->m_seconds == 59 && mission->m_frames == 98) {
            sprintf(time2, "99:59:99");
        } else {
            sprintf(time2, "%02d:%02d:%02d", mission->m_minutes, mission->m_seconds,
                    mission->m_frames);
        }
        fn_800BC7BC(fn_802B61F4()->m_display.get(), time2, places[13].m_x, 0,
                    s_numberOffset + (places[13].m_y - 18.0f * s_numberScaleY));
        break;
    }
    case 3:
        sprintf(keys, "%02d/71", save->m_keys);
        fn_802B61F4()->m_display->SetScale(1.0f, 1.0f);
        fn_800BC7BC(fn_802B61F4()->m_display.get(), keys, s_keysX, 0, s_keysY);
        break;
    }
    fn_801E5D48()->m_scale = 0.04f;
}

/* Not original: an unused table that pools the unit's later .rodata strings in retail's order
 * (see Game/GiGameOver.cpp; the linker drops the table). The .rodata is kept whole, so it also
 * holds the RTTI names of the CSD, std and boost headers' inline classes, which this unit does
 * not reconstruct. */
static const char *s_rodataPool[] = {
    "AdvReal::SelectMode::Phase",
    "AdvReal::AdvBase",
    "select_button",
    "exit_button",
    "Not inititalized m_eDealloctor\n",
    "csdRCObject.h",
    "99:59:99",
    "%02d:%02d:%02d",
    "Null_lines",
    "star%04d",
    "mission_picture",
    "HiLiteOpen",
    "SelectOpen",
    "SelectClose",
    "HiLiteClose",
    "boost::detail::sp_counted_base_impl<SavedataAnalyzer::StoryCheck *, boost::checked_deleter<SavedataAnalyzer::StoryCheck>>",
    "boost::detail::sp_counted_base",
    "std::exception",
    "exception",
    "std::bad_alloc",
    "bad_alloc",
    "Chao::CSD::RCPtr<Chao::CSD::CTexList>::RCObject<Chao::CSD::CTexList>::CFunctorBase",
    "Chao::CSD::CBase",
    "Chao::CSD::RCPtr<Chao::CSD::CTexList>::RCObject<Chao::CSD::CTexList>",
    "Chao::CSD::RCPtr<Chao::CSD::CSceneList>::RCObject<Chao::CSD::CSceneList>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CSceneList>::RCObject<Chao::CSD::CSceneList>",
    "Chao::CSD::RCPtr<Chao::CSD::CScene>::RCObject<Chao::CSD::CScene>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CScene>::RCObject<Chao::CSD::CScene>",
    "Chao::CSD::RCPtr<Chao::CSD::CNode>::RCObject<Chao::CSD::CNode>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CNode>::RCObject<Chao::CSD::CNode>",
    "boost::checked_deleter<SavedataAnalyzer::StoryCheck>",
};

void Screen::Back() {
    if (!m_playing) {
        fn_802B7528(fn_801E5E00(), 1);
    }
}

void Screen::Leave() {
    if (fn_80044E10() == 1) {
        fn_802C839C(fn_801E5CA8(), 0);
        fn_802B7528(fn_801E5E00(), 0);
    }
}

bool Screen::Draw() {
    if (!fn_802B5434(fn_801E5F0C(), m_project)) {
        return 0;
    }
    if (fn_8036C628(m_project) == lbl_805EE110) {
        return 0;
    }
    fn_8003C92C(&m_scenes, fn_8036C628(m_project));
    if (m_playing) {
        m_text.m_wait = 0;
    }
    if (m_text.m_wait > 10) {
        DrawTexts();
    } else {
        m_text.m_wait++;
    }
    return 1;
}

bool Screen::LoadProject() {
    fn_802B57FC(fn_801E5F0C());
    m_project = fn_802B5504(fn_801E5F0C(), s_projectName);
    if (m_project != 0) {
        UnkCsdHandle_80042DF0 handle = fn_8036D598(m_project, 0);
        fn_8003CEF8(&m_scenes, handle, s_sceneNames, 3);
        return true;
    }
    return false;
}

void Screen::Setup() {
    fn_8003CE5C(&m_scenes);
    ResetCursor();
    fn_8036AD78(
        fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[2])->GetScene().Get(), "cursor").Get(), 1);
    m_text.m_mode = 0;
    m_text.m_wait = 0;
    if (fn_802B7CC0()) {
        RCPtr<CNode> node;
        node = fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(),
                           "select_button");
        fn_8036AD14(node.Get(), 3);
        node = fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[0])->GetScene().Get(),
                           "exit_button");
        fn_8036AD14(node.Get(), 3);
    }
}

/* Starts the stage under the cursor. */
void Screen::StartStage() {
    CursorValues &values = s_cursor.m_values;
    fn_801E5858(fn_801E2128(), 5);
    StageSequence::Manager::SetNext(0);
    const UnkStageInfo_804C4A18 *info;
    if (s_cursor.m_values.m_secondMap) {
        int index = fn_802BF2EC(values.m_x, values.m_y, false);
        fn_800B74B4();
        info = &lbl_804C4A18[index];
    } else if (s_cursor.m_values.m_bossList) {
        int index = fn_802BF244(s_cursor.m_values.m_boss, false);
        fn_800B74B4();
        info = &lbl_804C4A18[index];
    } else {
        int index = fn_802BF3C0(values.m_x, values.m_y, false);
        fn_800B74B4();
        info = &lbl_804C4A18[index];
    }
    lbl_80584720.m_stage = info->m_id;
    fn_802062FC(lbl_8057D6A0.m_record);
    fn_800CBBFC(lbl_8057D6A0.field_0x148, 1, 3);
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

} // namespace SelectMode
} // namespace AdvReal
