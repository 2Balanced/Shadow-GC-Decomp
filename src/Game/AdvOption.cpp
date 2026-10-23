/* AdvReal::Option::Phase (class name from RTTI): the front-end options menu. Its state lives in an
 * Option::Impl (class name inferred) that plays the CSD project "csdFiles/Options": a grid of six
 * items (sound test, vibration, sound output, subtitle language, loading the save data and voice
 * language), each with a list of values drawn through the text manager. The settings being
 * edited, with the last applied copy, are kept in the static s_data. Applying writes them to the
 * game state and the save data; a new subtitle language reloads the CSD project. The file name and
 * all names marked "(name inferred)" are not original; functions other units call keep their fn_
 * names. Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include <boost/scoped_ptr.hpp>
#include <string.h>

#include "Game/AdvBase.h"
#include "Game/AdvCsdEffect.h"
#include "Game/AdvPhaseManager.h"
#include "Game/CsdRCPtr.h"
#include "Game/CsdSceneCtrl.h"
#include "Game/CsdSceneSet.h"
#include "Game/Render.h"
#include "Game/SaveCycle.h"
#include "Game/Task.h"
#include "Game/UnkSingletons.h"
#include "Game/Vibration.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
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

/* The sound output and vibration settings of the game state (class names unknown; their
 * functions are at 0x80305AD8..0x80305BE8). */
class UnkSoundSetting_80305B34 {
  public:
    void Set(long mode);

    /* 0x0 */ s32 m_mode;
};

class UnkVibrationSetting_80305AD8 {
  public:
    void SetEnabled(bool enabled);

    /* 0x0 */ u8 m_enabled;
};

/* The game state (struct name unknown; see Game/AdvMemoryCheck.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x58];
    /* 0x058 */ s32 m_language;      /* the text language (name inferred) */
    /* 0x05C */ s32 m_voiceLanguage; /* (name inferred) */
    /* 0x060 */ u8 field_0x60[0x78 - 0x60];
    /* 0x078 */ UnkSoundSetting_80305B34 m_sound;
    /* 0x07C */ UnkVibrationSetting_80305AD8 m_vibration;
    /* 0x07D */ u8 m_cardUsable; /* (name inferred; see Game/AdvMemoryCheck.cpp) */
    /* 0x07E */ u8 field_0x7E[0x150 - 0x7E];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The save data (struct name unknown; see Game/GoalAction.cpp). The settings are saved at 0x18;
 * fn_801E5774 reads the unlocked sound test entries from 0x1714. */
struct UnkSaveData_800BB74C {
    /* 0x0000 */ u8 field_0x0[0x18];
    /* 0x0018 */ s32 m_language;
    /* 0x001C */ s32 m_voiceLanguage;
    /* 0x0020 */ s32 m_soundMode;
    /* 0x0024 */ u8 m_vibration;
    /* 0x0025 */ u8 field_0x25[0x1714 - 0x25];
    /* 0x1714 */ u8 m_soundTest[0x4];
};

/* The text language the CSD files are loaded in (meaning inferred). */
extern s32 lbl_805EE180;
/* An empty texture list (meaning inferred). */
extern RCPtr<CTexList> lbl_805EE110;

class UnkManager_801E5D48;

extern "C" {
/* The CSD project cache (Game/AdvCsdCache.cpp): loading a project, whether it holds one, and
 * reloading one (behaviour inferred). */
void *fn_801E5F0C(void);
UnkCsdResource_8003E808 *fn_802B5504(void *cache, const char *file);
bool fn_802B5434(void *cache, UnkCsdResource_8003E808 *project);
void fn_802B5760(void *cache, UnkCsdResource_8003E808 *project, const char *file);
/* Projects (behaviour inferred; see Game/TGi.h). */
UnkCsdHandle_80042DF0 fn_8036D598(UnkCsdResource_8003E808 *project, int arg1);
RCPtr<CTexList> fn_8036C628(UnkCsdResource_8003E808 *project);
/* Scenes and nodes (behaviour inferred; see Game/TGi.h and Game/TGiScoreDisplay.cpp). */
RCPtr<CNode> fn_803666B4(CScene *scene, const char *name);
void fn_8036AD78(CNode *node, int hidden);
void fn_8036AD14(CNode *node, int pattern);

/* The text manager: the colour and alpha of the next text, drawing message id at a node, and
 * reloading the font (behaviour inferred; their code is at 0x802BEC5C..0x802BEFB4). */
UnkManager_801E5D48 *fn_801E5D48(void);
void fn_802BEC5C(UnkManager_801E5D48 *manager, float alpha, int color);
void fn_802BED7C(UnkManager_801E5D48 *manager, int message, int arg2, RCPtr<CNode> node, int arg4);
void fn_802BEF78(UnkManager_801E5D48 *manager);
void fn_802BEFB4(UnkManager_801E5D48 *manager);

/* The voice manager and muting it (behaviour inferred; see Game/AdvMovie.cpp). */
void *fn_801E5CA8(void);
void fn_802C839C(void *manager, int arg1);
/* Sets the voice language (behaviour inferred). */
void fn_801DF6A0(s32 *languages, int language, int arg2);
/* Starts a rumble pattern (see Game/BlackDoomModeCtrlUnit.cpp). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int arg1);

/* The save data, its number of sound test entries, whether entry index is unlocked, and an
 * entry's stream (behaviour inferred). */
UnkSaveData_800BB74C *fn_800BB74C(void);
int fn_801E5038(UnkSaveData_800BB74C *save);
bool fn_801E5774(u8 *unlocks, int index);
const char *fn_801E4FE0(UnkSaveData_800BB74C *save, int index);

/* The sound manager, playing a sound and a stream (see Game/GadgetUtil.h and Game/GoalAction.cpp). */
void *fn_80053164(void);
void fn_801D560C(void *manager, int soundId, int arg2, int arg3);
void fn_801D524C(void *manager, const char *name);

/* The screen fade: starting it, and its state (see Game/ScreenFade.h and Game/AdvMovie.cpp). */
void fn_80173834(int arg0, float time);
void fn_80173878(int arg0, int arg1, float time);
int fn_80044E10(void);
/* Whether the save message object lets the player act (see Game/AdvPadInput.cpp). */
bool fn_802E5810(UnkObject_802E5B2C *object);

/* Pad input (Game/AdvPadInput.cpp). */
bool fn_802B7D5C(int button, int pad);
bool fn_802B7D00(int button, int pad);
bool fn_802B7CC0(void);
}

/* memcmp (the symbol keeps its placeholder name; Game/MeteorsHolder.cpp calls it by it). */
extern "C" int fn_803A8DA8(const void *a, const void *b, u32 size);

namespace AdvReal {
namespace Option {

/* An item of the grid: its scene motion, title message, node, values and their number (names
 * inferred). The sound test's values are built from the save data. */
struct ItemInfo {
    /* 0x00 */ const char *m_motion;
    /* 0x04 */ s32 m_message;
    /* 0x08 */ const char *m_node;
    /* 0x0C */ const s32 *m_values;
    /* 0x10 */ s32 m_count;
};

/* The settings of the menu: the grid cursor, and per item the first value shown and the value
 * chosen (names inferred). */
struct Settings {
    /* 0x00 */ s32 m_row;
    /* 0x04 */ s32 m_column;
    /* 0x08 */ s32 m_top[6];
    /* 0x20 */ s32 m_value[6];
};

/* The settings being edited and the last applied copy (class name inferred). */
class OptionData {
  public:
    OptionData() {
        Reset();
    }

    void Reset() {
        memset(&m_current, 0, sizeof(m_current));
        Commit();
        field_0x74 = -1;
    }

    void Commit() {
        m_saved = m_current;
        m_changed = 0;
    }

    void Revert() {
        m_current = m_saved;
        m_changed = 0;
    }

    bool IsDirty() {
        if (m_changed) {
            return true;
        }
        return fn_803A8DA8(&m_current, &m_saved, sizeof(m_current)) != 0;
    }

    void Load(bool languageOffset);
    void Finish();

    /* 0x00 */ Settings m_current;
    /* 0x38 */ Settings m_saved;
    /* 0x70 */ u8 m_changed;
    /* 0x74 */ s32 field_0x74;
};
SIZE_ASSERT(OptionData, 0x78);

} // namespace Option
} // namespace AdvReal

using AdvReal::Option::ItemInfo;
using AdvReal::Option::OptionData;

/* The CSD project of the menu. */
static const char *s_csdFile = "csdFiles/Options";

/* The scenes of the project. */
static const char *const s_sceneNames[3] = {"backgound", "menu", "fontnull"};

/* The messages of the items' values. */
static const s32 s_voiceValues[2] = {0x151F, 0x1520};
static const s32 s_vibrationValues[2] = {0x1526, 0x1527};
static const s32 s_languageValues[11] = {
    0x151F, 0x1520, 0x1521, 0x1523, 0x1522, 0x1524, 0x1520, 0x1521, 0x1523, 0x1522, 0x1524,
};
static const s32 s_soundValues[3] = {0x1529, 0x152A, 0x152B};
static const s32 s_loadValues[1] = {0x1528};

static ItemInfo s_items[6] = {
    {"Sound", 0x138B, "Null_box", 0, 0},
    {"Vibe", 0x1647, "Null_box", s_vibrationValues, 2},
    {"System", 0x138C, "Null_box", s_soundValues, 3},
    {"Subtitle", 0x138A, "Null_box", s_languageValues, 6},
    {"Load", 0x138E, "Null_box", s_loadValues, 1},
    {"Audio", 0x1389, "Null_box", s_voiceValues, 2},
};

/* The item at each grid position. */
static const s32 s_itemGrid[3][2] = {{0, 1}, {2, 3}, {4, 5}};

/* The nodes of the seven lines of a list. */
static const char *s_textNodes[7] = {"text1", "text2", "text3", "text4", "text5", "text6", "text7"};

static OptionData s_data;

namespace AdvReal {
namespace Option {

/* The menu's state (class name inferred). */
class Impl {
  public:
    Impl();
    ~Impl();

    bool Tick();
    bool Draw();

    void SetState(int state, bool restart);
    void Start();
    void UpdateGrid();
    void UpdateList();
    void ChangeLanguage();
    void LoadData();
    void Exit();
    void DrawList(int item, bool active);
    bool Load();
    /* Whether a setting that is applied changed (name inferred). */
    u8 IsChanged();
    void Apply();
    void SetupMenu();

    /* Whether the cursor may stop at a grid position (name inferred). */
    bool IsSelectable(int column, int row) {
        if (m_hideVoice && column + row * 2 == 5) {
            return false;
        }
        if (m_hideSubtitle && column + row * 2 == 3) {
            return false;
        }
        if (s_itemGrid[row][column] == -1) {
            return false;
        }
        return true;
    }

    /* Member names are inferred. */
    /* 0x00 */ s32 m_state; /* 0 opening, 1 grid, 2 list, 3 new language, 4 loading, 5 closing */
    /* 0x04 */ s32 field_0x4;
    /* 0x08 */ s32 m_step;
    /* 0x0C */ UnkCsdResource_8003E808 *m_project;
    /* 0x10 */ UnkCsdSceneSet_8003D5D8 m_scenes;
    /* 0x1C */ u8 m_busy;
    /* 0x1D */ u8 field_0x1D;
    /* 0x1E */ u8 m_active;
    /* 0x20 */ SaveCycle::CycleTask *m_saveTask;
    /* 0x24 */ u8 m_hideSubtitle;
    /* 0x25 */ bool m_languageOffset;
    /* 0x26 */ u8 m_hideVoice;
    /* 0x28 */ boost::shared_ptr<AdvCsdEffect> m_effect;
    /* 0x30 */ u8 m_playing;
    /* 0x31 */ u8 m_applied;
    /* 0x34 */ s32 m_repeatWait;
};
SIZE_ASSERT(Impl, 0x38);

class Phase : public AdvBase {
  public:
    Phase();
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

} // namespace Option
} // namespace AdvReal

/* Retail keeps boost::checked_delete<Impl> out of line; this specialization gives that under
 * GC/2.7 (see docs/research-notes.md and Game/EnergyCore.cpp). */
namespace boost {
template <> inline void checked_delete<AdvReal::Option::Impl>(AdvReal::Option::Impl *x) {
    AdvReal::Option::DeleteImpl(x);
}
} // namespace boost

/* Whether a cycle task has finished (name unknown; other units call it). */
extern "C" bool fn_802B8DF0(SaveCycle::CycleTask *task);

/* Whether two texture lists are the same (name inferred). */
static inline int SameTexList(const RCPtr<CTexList> &a, const RCPtr<CTexList> &b) {
    if (a.m_object != 0 && b.m_object != 0) {
        return a.m_object->m_object == b.m_object->m_object;
    }
    return a.m_object == b.m_object;
}

namespace AdvReal {
namespace Option {

/* Not called anywhere: the linker strips it from the game. This unit's .rodata is kept whole (its
 * functions address it through one base), so retail keeps the RTTI names of the CSD and boost
 * headers' inline classes the original instantiated here even though their records are stripped.
 * Compiled last (deferred inlining compiles the first definition last), this function creates
 * them in retail's order, and the RTTI records created afterwards reuse them. */
void PoolOptionRttiNames(const char **names) {
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

Impl::Impl()
    : m_state(0), m_active(0), m_hideSubtitle(0), m_languageOffset(0), m_hideVoice(0), m_playing(0),
      m_applied(0) {
    fn_80173834(1, 0.0f);
    m_hideVoice = 1;
    s_data.Load(m_languageOffset);
    s_data.Commit();
}

void OptionData::Finish() {}

Impl::~Impl() {
    s_data.Finish();
    if (s_items[0].m_values != 0) {
        delete[] s_items[0].m_values;
        s_items[0].m_values = 0;
    }
}

bool Impl::Tick() {
    m_active = 0;
    switch (m_state) {
    case 0:
        Start();
        break;
    case 1:
        UpdateGrid();
        break;
    case 2:
        UpdateList();
        break;
    case 3:
        ChangeLanguage();
        break;
    case 4:
        LoadData();
        break;
    case 5:
        Exit();
        break;
    }
    m_busy = fn_8003CDF4(&m_scenes);
    fn_80355F54(m_effect.get());
    return true;
}

void Impl::SetState(int state, bool restart) {
    m_state = state;
    if (restart) {
        m_step = 0;
    } else {
        m_step = 1;
    }
    s_data.m_changed = 1;
}

void Impl::Start() {
    if (Load()) {
        SetupMenu();
        fn_8003CEAC(&m_scenes, s_sceneNames[0])->Play("BGOpen", 1);
        fn_8003CEAC(&m_scenes, s_sceneNames[1])->Play("MenuOpen", 1);
        SetState(1, true);
    } else {
        SetState(6, true);
    }
}

void Impl::UpdateGrid() {
    if (fn_8003DBB0(fn_8003CEAC(&m_scenes, s_sceneNames[1])) && fn_802E5810(fn_80117514())) {
        m_active = 1;
        if (fn_802B7D5C(3, 4)) {
            if (s_data.m_current.m_row > 0 && IsSelectable(s_data.m_current.m_column, s_data.m_current.m_row - 1)) {
                s_data.m_current.m_row--;
                fn_801D560C(fn_80053164(), 0xE002, 0, 0);
            }
        } else if (fn_802B7D5C(4, 4)) {
            if (2 > s_data.m_current.m_row && IsSelectable(s_data.m_current.m_column, s_data.m_current.m_row + 1)) {
                s_data.m_current.m_row++;
                fn_801D560C(fn_80053164(), 0xE002, 0, 0);
            }
        } else if (fn_802B7D5C(5, 4)) {
            if (s_data.m_current.m_column > 0 && IsSelectable(s_data.m_current.m_column - 1, s_data.m_current.m_row)) {
                s_data.m_current.m_column--;
                fn_801D560C(fn_80053164(), 0xE002, 0, 0);
            }
        } else if (fn_802B7D5C(6, 4)) {
            if (s_data.m_current.m_column < 1 && IsSelectable(s_data.m_current.m_column + 1, s_data.m_current.m_row)) {
                s_data.m_current.m_column++;
                fn_801D560C(fn_80053164(), 0xE002, 0, 0);
            }
        } else if (fn_802B7D5C(1, 4)) {
            SetState(2, true);
            fn_801D560C(fn_80053164(), 0xE003, 0, 0);
        } else if (fn_802B7D5C(2, 4)) {
            SetState(5, true);
            fn_801D560C(fn_80053164(), 0xE004, 0, 0);
        }
        if (s_data.IsDirty()) {
            int item = s_itemGrid[s_data.m_current.m_row][s_data.m_current.m_column];
            fn_8003CEAC(&m_scenes, s_sceneNames[1])->Reset();
            fn_8003CEAC(&m_scenes, s_sceneNames[1])->Play(s_items[item].m_motion, 1);
            s_data.Commit();
        }
    }
}

void Impl::UpdateList() {
    int item = s_itemGrid[s_data.m_current.m_row][s_data.m_current.m_column];
    if (fn_8003DBB0(fn_8003CEAC(&m_scenes, s_sceneNames[1]))) {
        m_active = 1;
        if (m_repeatWait != 0) {
            m_repeatWait--;
            return;
        }
        if (fn_802B7D00(3, 4)) {
            if (s_data.m_current.m_value[item] > 0) {
                s_data.m_current.m_value[item]--;
                m_repeatWait = 2;
                fn_801D560C(fn_80053164(), 0xE002, 0, 0);
                if (s_data.m_current.m_value[item] < s_data.m_current.m_top[item]) {
                    s_data.m_current.m_top[item] = s_data.m_current.m_value[item];
                }
            }
        } else if (fn_802B7D00(4, 4)) {
            if (s_data.m_current.m_value[item] < s_items[item].m_count - 1) {
                s_data.m_current.m_value[item]++;
                m_repeatWait = 2;
                fn_801D560C(fn_80053164(), 0xE002, 0, 0);
                if (s_data.m_current.m_value[item] > s_data.m_current.m_top[item] + 6) {
                    s_data.m_current.m_top[item] = s_data.m_current.m_value[item] - 6;
                }
            }
        } else if (fn_802B7D5C(1, 4)) {
            switch (item) {
            case 4:
                SetState(4, true);
                fn_801D560C(fn_80053164(), 0xE003, 0, 0);
                break;
            case 0: {
                int i;
                int entry = 0;
                UnkSaveData_800BB74C *save = fn_800BB74C();
                for (i = 0; i < fn_801E5038(fn_800BB74C()); i++) {
                    if (fn_801E5774(save->m_soundTest, i)) {
                        if (entry == s_data.m_current.m_value[item]) {
                            fn_802C839C(fn_801E5CA8(), 0);
                            fn_801D524C(fn_80053164(), fn_801E4FE0(fn_800BB74C(), i));
                            m_playing = 1;
                        }
                        entry++;
                    }
                }
                break;
            }
            case 1:
            case 2:
            case 5:
                if (IsChanged()) {
                    Apply();
                    fn_801D560C(fn_80053164(), 0xE003, 0, 0);
                    SetState(1, true);
                }
                break;
            case 3:
                if (IsChanged()) {
                    SetState(3, true);
                    fn_80173878(0, 1, 1.0f);
                    fn_801D560C(fn_80053164(), 0xE003, 0, 0);
                }
                break;
            }
        } else if (fn_802B7D5C(2, 4)) {
            if (m_playing) {
                fn_802C839C(fn_801E5CA8(), 3);
                m_playing = 0;
            }
            s_data.Revert();
            SetState(1, true);
            fn_801D560C(fn_80053164(), 0xE004, 0, 0);
        }
    }
}

void OptionData::Load(bool languageOffset) {
    m_current.m_value[5] = lbl_8057D6A0.m_voiceLanguage;
    m_current.m_value[1] = lbl_8057D6A0.m_vibration.m_enabled == 0;
    m_current.m_value[0] = 0;
    if (languageOffset) {
        m_current.m_value[3] = lbl_8057D6A0.m_language - 1;
    } else {
        m_current.m_value[3] = lbl_8057D6A0.m_language;
    }
    m_current.m_value[4] = 0;
    switch (lbl_8057D6A0.m_sound.m_mode) {
    case 0:
        m_current.m_value[2] = 0;
        break;
    case 1:
        m_current.m_value[2] = 1;
        break;
    case 2:
        m_current.m_value[2] = 2;
        break;
    }
    m_current.m_top[5] = 0;
    m_current.m_top[1] = 0;
    m_current.m_top[0] = 0;
    m_current.m_top[3] = 0;
    m_current.m_top[4] = 0;
    m_current.m_top[2] = 0;

    UnkSaveData_800BB74C *save = fn_800BB74C();
    int i;
    s_items[0].m_count = 0;
    for (i = 0; i < fn_801E5038(fn_800BB74C()); i++) {
        if (fn_801E5774(save->m_soundTest, i)) {
            s_items[0].m_count++;
        }
    }
    if (s_items[0].m_values != 0) {
        delete[] s_items[0].m_values;
        s_items[0].m_values = 0;
    }
    if (s_items[0].m_count != 0) {
        s32 *values = new s32[s_items[0].m_count];
        int n;
        for (i = 0, n = 0; i < fn_801E5038(fn_800BB74C()); i++) {
            if (fn_801E5774(save->m_soundTest, i)) {
                values[n++] = i + 0x17D4;
            }
        }
        s_items[0].m_values = values;
    }
}

void Impl::LoadData() {
    switch (m_step) {
    case 0:
        m_saveTask = new SaveCycle::CycleTask((Task *)fn_80046178()->field_0x3C, 1);
        m_step = 1;
        break;
    case 1:
        if (fn_802B8DF0(m_saveTask)) {
            m_saveTask->m_flags |= 1;
            fn_802C839C(fn_801E5CA8(), 3);
            s_data.Load(m_languageOffset);
            if (s_data.m_current.m_value[3] != s_data.m_saved.m_value[3]) {
                SetState(3, true);
                fn_80173878(0, 1, 1.0f);
            } else {
                s_data.Commit();
                SetState(1, true);
            }
        }
        break;
    }
}

void Impl::ChangeLanguage() {
    m_active = 1;
    switch (m_step) {
    case 0:
        if (fn_80044E10() == 1 && fn_802E5810(fn_80117514())) {
            m_step = 1;
        }
        break;
    case 1:
        Apply();
        s_data.Commit();
        m_step = 3;
        fn_80173834(1, 0.0f);
        break;
    case 2:
        if (fn_802B8DF0(m_saveTask)) {
            m_saveTask->m_flags |= 1;
            fn_80173834(1, 0.0f);
            m_step = 3;
        }
        break;
    case 3:
        if (fn_80044E10() == 0) {
            SetState(1, true);
        }
        break;
    }
}

} // namespace Option
} // namespace AdvReal

extern "C" bool fn_802B8DF0(SaveCycle::CycleTask *task) {
    return fn_802E3E34(task->m_cycle.get());
}

namespace AdvReal {
namespace Option {

void Impl::Exit() {
    switch (m_step) {
    case 0:
        fn_8003CEAC(&m_scenes, s_sceneNames[0])->Play("BGClose", 1);
        fn_8003CEAC(&m_scenes, s_sceneNames[1])->Play("MenuClose", 1);
        m_step = 1;
        break;
    case 1:
        if (!m_busy) {
            if (lbl_8057D6A0.m_cardUsable) {
                m_saveTask = new SaveCycle::CycleTask((Task *)fn_80046178()->field_0x3C, 2);
                m_step = 2;
            } else {
                m_step = 3;
                fn_80173834(1, 0.0f);
            }
        }
        break;
    case 2:
        if (fn_802B8DF0(m_saveTask)) {
            m_saveTask->m_flags |= 1;
            fn_80173834(1, 0.0f);
            m_step = 3;
        }
        break;
    case 3:
        fn_802B7528(fn_801E5E00(), 1);
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
    case 1: {
        int item = s_itemGrid[s_data.m_current.m_row][s_data.m_current.m_column];
        RCPtr<CNode> node = fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[1])->GetScene().Get(), s_items[item].m_node);
        fn_802BEC5C(fn_801E5D48(), 1.0f, 0);
        fn_802BED7C(fn_801E5D48(), s_items[item].m_message, 1, node, 0);
        DrawList(item, false);
        break;
    }
    case 2:
    case 3: {
        int item = s_itemGrid[s_data.m_current.m_row][s_data.m_current.m_column];
        RCPtr<CNode> node = fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[1])->GetScene().Get(), s_items[item].m_node);
        fn_802BEC5C(fn_801E5D48(), 1.0f, 0);
        fn_802BED7C(fn_801E5D48(), s_items[item].m_message, 1, node, 0);
        DrawList(item, true);
        break;
    }
    }
    return true;
}

void Impl::DrawList(int item, bool active) {
    if (m_active) {
        s32 *top;
        s32 *value;
        ItemInfo &info = s_items[item];
        float alpha = active ? 1.0f : 0.5f;
        top = &s_data.m_current.m_top[item];
        value = &s_data.m_current.m_value[item];
        for (int i = 0; i < info.m_count; i++) {
            if (i < *top) {
                continue;
            }
            if (i > *top + 6) {
                break;
            }
            RCPtr<CNode> node = fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[2])->GetScene().Get(), s_textNodes[i - *top]);
            if (i == *value) {
                fn_802BEC5C(fn_801E5D48(), alpha, 2);
            } else {
                fn_802BEC5C(fn_801E5D48(), alpha, 0);
            }
            fn_802BED7C(fn_801E5D48(), info.m_values[i], 1, node, 0);
            fn_802BEC5C(fn_801E5D48(), 1.0f, 0);
        }
        fn_802BEC5C(fn_801E5D48(), 1.0f, 0);
    }
}

bool Impl::Load() {
    m_project = fn_802B5504(fn_801E5F0C(), s_csdFile);
    if (m_project != 0) {
        UnkCsdHandle_80042DF0 project = fn_8036D598(m_project, 0);
        fn_8003CEF8(&m_scenes, project, s_sceneNames, 3);
        fn_8003C8D8(&m_scenes, s_sceneNames[0], true);
        fn_8003C8D8(&m_scenes, s_sceneNames[1], true);
        m_effect = boost::shared_ptr<AdvCsdEffect>(new AdvCsdEffect(project));
        return true;
    }
    return false;
}

u8 Impl::IsChanged() {
    if (s_data.m_current.m_value[5] != s_data.m_saved.m_value[5]) {
        return true;
    }
    if (s_data.m_current.m_value[1] != s_data.m_saved.m_value[1]) {
        return true;
    }
    if (s_data.m_current.m_value[3] != s_data.m_saved.m_value[3]) {
        return true;
    }
    return s_data.m_current.m_value[2] != s_data.m_saved.m_value[2];
}

void Impl::Apply() {
    OptionData &data = s_data;
    if (data.m_current.m_value[5] != data.m_saved.m_value[5]) {
        fn_802C839C(fn_801E5CA8(), 0);
        fn_801DF6A0(&lbl_8057D6A0.m_language, data.m_current.m_value[5], 0);
        fn_802C839C(fn_801E5CA8(), 3);
    }
    if (data.m_current.m_value[1] != data.m_saved.m_value[1]) {
        lbl_8057D6A0.m_vibration.SetEnabled(data.m_current.m_value[1] == 0);
        fn_8034DF48(4, -2);
    }
    if (data.m_current.m_value[3] != data.m_saved.m_value[3]) {
        if (m_languageOffset) {
            lbl_8057D6A0.m_language = lbl_805EE180 = data.m_current.m_value[3] + 1;
        } else {
            lbl_8057D6A0.m_language = lbl_805EE180 = data.m_current.m_value[3];
        }
        fn_802B5760(fn_801E5F0C(), m_project, s_csdFile);
        fn_802BEF78(fn_801E5D48());
        fn_802BEFB4(fn_801E5D48());
    }
    if (data.m_current.m_value[2] != data.m_saved.m_value[2]) {
        fn_802C839C(fn_801E5CA8(), 0);
        switch (data.m_current.m_value[2]) {
        case 0:
            lbl_8057D6A0.m_sound.Set(0);
            break;
        case 1:
            lbl_8057D6A0.m_sound.Set(1);
            break;
        case 2:
            lbl_8057D6A0.m_sound.Set(2);
            break;
        }
        fn_802C839C(fn_801E5CA8(), 3);
    }
    UnkSaveData_800BB74C *save = fn_800BB74C();
    if (save != 0) {
        save->m_language = lbl_8057D6A0.m_language;
        save->m_voiceLanguage = lbl_8057D6A0.m_voiceLanguage;
        save->m_soundMode = lbl_8057D6A0.m_sound.m_mode;
        save->m_vibration = lbl_8057D6A0.m_vibration.m_enabled;
    }
    data.m_saved = data.m_current;
    m_applied = 1;
}

/* Not called anywhere: the linker strips it from the game. Retail compiled the CSD header's inline
 * RCPtr code here, after SetupMenu; this places its strings where that code put them. */
void PoolOptionRCPtrStrings(const char **names) {
    names[0] = "Not inititalized m_eDealloctor\n";
    names[1] = "csdRCObject.h";
}

void Impl::SetupMenu() {
    fn_8003CE5C(&m_scenes);
    if (m_hideVoice) {
        RCPtr<CNode> node = fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[1])->GetScene().Get(), "icn_audio");
        fn_8036AD78(node.Get(), 1);
    }
    if (m_hideSubtitle) {
        RCPtr<CNode> node = fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[1])->GetScene().Get(), "icn_subtitle");
        fn_8036AD78(node.Get(), 1);
    }
    if (fn_802B7CC0()) {
        RCPtr<CNode> node;
        node = fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[1])->GetScene().Get(), "select_btn");
        fn_8036AD14(node.Get(), 3);
        node = fn_803666B4(fn_8003CEAC(&m_scenes, s_sceneNames[1])->GetScene().Get(), "exit_btn");
        fn_8036AD14(node.Get(), 3);
    }
    m_repeatWait = 0;
}

Phase::Phase() : m_impl(new Impl) {}

Phase::~Phase() {}

bool Phase::Tick() {
    return m_impl->Tick();
}

bool Phase::Draw() {
    return m_impl->Draw();
}

} // namespace Option
} // namespace AdvReal
