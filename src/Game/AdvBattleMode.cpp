/* AdvReal::BattleMode::Phase (class name from RTTI): the front-end phase of the 2P battle mode.
 * Its screen plays the "csdFiles/BattleMode" project: the first and then the second player pick
 * their characters on a 2 x 3 grid, then the arena is chosen (or the 1P mode's three-stage
 * match), and the phase starts the stage sequence. The file name, the screen's class name and all
 * function and member names are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). The unit's code reaches its .rodata through one
 * base address, so the linker keeps that section whole (see s_rodataPool). */
#include <boost/scoped_ptr.hpp>

#include "Game/AdvBase.h"
#include "Game/CsdRCPtr.h"
#include "Game/StageSequence.h"
#include <dolphin/types.h>
#include "size_assert.h"

using Chao::CSD::CScene;
using Chao::CSD::RCPtr;

/* A node's position (struct name unknown; see Game/SavePointSelect.cpp). */
struct UnkCsdPosition_8036AE98 {
    float x;
    float y;
};

namespace Chao {
namespace CSD {

/* RCPtr's operator-> (as in Game/TGi.h). Retail calls it out of line (CNode's at 0x802C19F8) to
 * pass a node to fn_8036AE98, which is probably a CNode member function. */
template <class T> inline T *RCPtr<T>::operator->() const {
    return Get();
}

} // namespace CSD
} // namespace Chao

using Chao::CSD::CNode;

/* The value a project returns from fn_8036D598 (class name unknown; see Game/TGi.h). */
class UnkCsdHandle_80042DF0 {
  public:
    UnkCsdHandle_80042DF0(const UnkCsdHandle_80042DF0 &other);
    ~UnkCsdHandle_80042DF0();

    /* 0x0 */ void *m_object;
};

/* The game state (struct name unknown; see Game/AdvMemoryCheck.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x18];
    /* 0x018 */ float m_dt; /* (name inferred) */
    /* 0x01C */ u8 field_0x1C[0x58 - 0x1C];
    /* 0x058 */ s32 m_language;
    /* 0x05C */ u8 field_0x5C[0xCC - 0x5C];
    /* 0x0CC */ u8 m_record[0x13C - 0xCC]; /* (see Game/AdvTitle.cpp) */
    /* 0x13C */ s32 m_characters[2]; /* the characters the players picked, from 0xE on */
    /* 0x144 */ u8 field_0x144[0x148 - 0x144];
    /* 0x148 */ u8 field_0x148[3];
    /* 0x14B */ u8 field_0x14B[0x150 - 0x14B];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The stages to play (struct name unknown; see Game/StageSequencePlayOnce.cpp). */
struct UnkStageInfo_80584720 {
    /* 0x000 */ u8 field_0x0[0xC];
    /* 0x00C */ s32 m_stage;     /* of a battle */
    /* 0x010 */ s32 m_stages[3]; /* of the three-stage match */
    /* 0x01C */ u8 field_0x1C[0x230 - 0x1C];
};
SIZE_ASSERT(UnkStageInfo_80584720, 0x230);

extern UnkStageInfo_80584720 lbl_80584720;

/* The pads (struct name unknown; see Game/AdvTitle.cpp). */
struct UnkPads_80051734 {
    /* 0x000 */ u8 field_0x0[0x11C];
    /* 0x11C */ s32 m_controllers[2]; /* meaning unknown */
};

class UnkManager_801E5E00;

extern "C" {
UnkPads_80051734 *fn_80051734(void);

/* The front-end's CSD folder: loading a project, and whether it is ready (see
 * Game/AdvTitle.cpp). */
void *fn_801E5F0C(void);
void *fn_802B5504(void *folder, const char *name);
bool fn_802B5434(void *folder, void *project);

/* Projects, scenes and nodes (behaviour inferred; see Game/TGi.h). */
UnkCsdHandle_80042DF0 fn_8036D598(void *project, int arg1);
RCPtr<CScene> fn_8036D660(void *project, const char *name);
void fn_8036D94C(void *project);
void fn_8036DAD4(void *project);
void fn_8036C248(void *project);
void fn_80367238(CScene *scene, const char *motion);
float fn_803644A0(CScene *scene, const char *motion);
float fn_80364430(CScene *scene, const char *motion);
void fn_80366740(CScene *scene, float frame);
void fn_80367140(CScene *scene, float speed);
void fn_803659E4(CScene *scene);
void fn_803659DC(CScene *scene, int arg1);
void fn_803659CC(CScene *scene, int arg1);
RCPtr<CNode> fn_803666B4(CScene *scene, const char *name);
UnkCsdPosition_8036AE98 fn_8036AE98(CNode *node);
void fn_8036AE70(CNode *node, const UnkCsdPosition_8036AE98 *position);
void fn_8036AD14(CNode *node, int pattern);
void fn_8036AD78(CNode *node, int hidden);
void fn_8036AC60(CNode *node, int corner, const u32 *color);
void fn_8036ACA8(CNode *node);
void fn_8036ACCC(CNode *node, const u32 *color);

/* Menu input (see Game/AdvTitle.cpp). */
bool fn_802B7D5C(int button, int pad);
bool fn_802B7CC0(void);
s32 fn_802B7CE0(void);

/* The background music (see Game/AdvBgm.cpp). */
void *fn_801E5CA8(void);
void fn_802C839C(void *player, int track);

/* The phase manager (see Game/AdvMemoryCheck.cpp). */
UnkManager_801E5E00 *fn_801E5E00(void);
void fn_802B7528(UnkManager_801E5E00 *manager, int id);

/* Sounds and voices (see Game/AdvTitle.cpp and Game/GiGameOver.cpp). */
void *fn_80053164(void);
void fn_801D560C(void *sound, int id, int arg2, int arg3);
void fn_801D51A0(void *sound, int id, int arg2, int arg3);
void fn_801D515C(void *sound);

/* Screen fades (see Game/ScreenFade.h). */
int fn_80044E10(void);
void fn_80173834(int arg0, float time);
void fn_80173878(int arg0, int arg1, float time);

/* Starting a game (see Game/AdvTitle.cpp). */
void *fn_801E2128(void);
void fn_801E5858(void *manager, int system);
void fn_802062FC(void *record);
void fn_800CBBFC(void *dest, int value, int count);

void *memset(void *dst, int value, unsigned long count);
}

/* Not original: an unused table that pools the unit's .rodata strings in retail's order (see
 * Game/GiGameOver.cpp; the linker drops the table). The .rodata is kept whole, so it also holds
 * the RTTI names of the CSD and boost headers' inline classes, which this unit does not
 * reconstruct. */
static const char *s_projectName = "csdFiles/BattleMode";

/* The stages of the three-stage match (name inferred). */
static const s32 s_matchStages[3] = {0x320, 0x321, 0x322};

static const char *s_rodataPool[] = {
    "shadow_1p",
    "shadow_2p",
    "AdvReal::BattleMode::Phase",
    "AdvReal::AdvBase",
    "Null_box",
    "box_chara",
    "Cursor1P",
    "1PCharaSelect",
    "Cursor2P",
    "2PCharaSelect",
    "tx_3match",
    "tx_1match",
    "Not inititalized m_eDealloctor\n",
    "csdRCObject.h",
    "ArenaOpen",
    "CharaOpen",
    "select_button",
    "back_button",
    "ButtonsOpen",
    "ButtonOpen",
    "Chao::CSD::RCPtr<Chao::CSD::CSceneList>::RCObject<Chao::CSD::CSceneList>::CFunctorBase",
    "Chao::CSD::CBase",
    "Chao::CSD::RCPtr<Chao::CSD::CSceneList>::RCObject<Chao::CSD::CSceneList>",
    "boost::detail::sp_counted_base",
    "Chao::CSD::RCPtr<Chao::CSD::CNode>::RCObject<Chao::CSD::CNode>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CNode>::RCObject<Chao::CSD::CNode>",
    "Chao::CSD::RCPtr<Chao::CSD::CScene>::RCObject<Chao::CSD::CScene>::CFunctorBase",
    "Chao::CSD::RCPtr<Chao::CSD::CScene>::RCObject<Chao::CSD::CScene>",
};

/* The colours of the screen's texts and shadows: grey (0x646464FF), white and dark red (names
 * inferred, in retail's order). */
static u32 s_select2pOff = 0x646464FF;
static u32 s_select2pShadowOff = 0x646464FF;
static u32 s_chosen1pOff = 0x646464FF;
static u32 s_chosen2pOn = 0xFFFFFFFF;
static u32 s_chosen1pShadowOff = 0x646464FF;
static u32 s_chosen2pShadowOn = 0xFFFFFFFF;
static u32 s_cancel1pOn = 0xFFFFFFFF;
static u32 s_cancel2pOff = 0x646464FF;
static u32 s_cancel1pShadowOn = 0xFFFFFFFF;
static u32 s_cancel2pShadowOff = 0x646464FF;
static u32 s_done1pOn = 0xFFFFFFFF;
static u32 s_done2pOn = 0xFFFFFFFF;
static u32 s_done1pShadowOn = 0xFFFFFFFF;
static u32 s_done2pShadowOn = 0xFFFFFFFF;
static u32 s_arena1MatchOn = 0xFFFFFFFF;
static u32 s_arena3MatchOn = 0xFFFFFFFF;

/* The players' shadows and cursor names, and the grid's boxes. */
static const char *s_shadowNames[2] = {"shadow_1p", "shadow_2p"};
static const char *s_cursorNames[2] = {"1p", "2p"};
static const char *s_boxNames[6] = {"box1", "box2", "box3", "box4", "box5", "box6"};

static u32 s_matchShown = 0xFFFFFFFF;
static u32 s_matchHidden = 0x660000FF;
static u32 s_arenaHidden = 0x660000FF;
static u32 s_arenaShown = 0xFFFFFFFF;

namespace AdvReal {
namespace BattleMode {

/* The menus' cursors (struct name inferred; as AdvReal::Title::Cursor). Its members are defined
 * out of the class, so they are called out of line. */
template <int N> struct Cursor {
    Cursor();
    void Clear();
    void Save();
    /* Value i, and all the values. Values calls an inline function, so it is not inlined. */
    s32 &Value(int i) {
        return m_values[i];
    }
    s32 *Values() {
        return &Value(0);
    }

    /* 0x00 */ s32 m_values[N];
    /* 0x0C */ s32 m_saved[N];
    /* 0x18 */ u8 m_changed;
    /* 0x1C */ s32 m_lastState;
};

template <int N> Cursor<N>::Cursor() {
    Clear();
}

template <int N> void Cursor<N>::Clear() {
    memset(m_values, 0, sizeof(m_values));
    Save();
    m_lastState = -1;
}

template <int N> void Cursor<N>::Save() {
    __memcpy(m_saved, m_values, sizeof(m_values));
    m_changed = 0;
}

/* The phase's state (class name inferred). */
class Screen {
  public:
    Screen();
    ~Screen();

    bool Tick();
    bool Draw();

    void StartMatch();
    void StartBattle();
    void UpdateDisplay();
    bool MoveCursor(s32 *cursor, int pad);
    bool Leave();
    bool SelectArena();
    bool SelectCharacters();
    bool Open();
    void Setup();
    bool LoadProject();
    void SetState(int state, bool start);

    /* 0x00 */ s32 m_state; /* 0 open, 1 characters, 2 arena, 3 leave */
    /* 0x04 */ s32 m_nextState;
    /* 0x08 */ s32 m_step;
    /* 0x0C */ Cursor<3> m_cursor; /* the characters of the two players, and the arena */
    /* 0x2C */ void *m_project;
    /* 0x30 */ u32 field_0x30;
    /* 0x34 */ s32 m_nextPhase; /* 0 the stage sequence, 1 back */
    /* 0x38 */ s32 m_player;    /* the player choosing */
    /* 0x3C */ u8 m_shown;
    /* 0x40 */ float m_timer; /* until the voice has played */
    /* 0x44 */ u32 field_0x44;
    /* 0x48 */ u32 field_0x48;
};
SIZE_ASSERT(Screen, 0x4C);

/* Deletes the screen (name inferred; see checked_delete below). */
inline void DeleteScreen(Screen *screen) {
    delete screen;
}

} // namespace BattleMode
} // namespace AdvReal

/* Retail keeps boost::checked_delete<Screen> out of line and the destructor at its own place;
 * this specialization gives that under GC/2.7 (see docs/research-notes.md, EnergyCore). */
namespace boost {
template <> inline void checked_delete<AdvReal::BattleMode::Screen>(AdvReal::BattleMode::Screen *x) {
    AdvReal::BattleMode::DeleteScreen(x);
}
} // namespace boost

namespace AdvReal {
namespace BattleMode {

/* Plays the menu sound id (name inferred). */
inline void PlaySound(int id) {
    fn_801D560C(fn_80053164(), id, 0, 0);
}

Screen::Screen() : m_state(0) {
    m_project = 0;
    fn_80173834(1, 0.0f);
    field_0x48 = 0;
    field_0x44 = 0;
}

Screen::~Screen() {
    fn_802C839C(fn_801E5CA8(), 0);
    if (fn_802B5434(fn_801E5F0C(), m_project)) {
        fn_8036C248(m_project);
        fn_8036DAD4(m_project);
    }
}

bool Screen::Tick() {
    bool result = false;
    switch (m_state) {
    case 0:
        result = Open();
        break;
    case 1:
        result = SelectCharacters();
        break;
    case 2:
        result = SelectArena();
        break;
    case 3:
        result = Leave();
        break;
    }
    if (fn_802B5434(fn_801E5F0C(), m_project)) {
        fn_8036DAD4(m_project);
    }
    return result;
}

bool Screen::Draw() {
    if (fn_802B5434(fn_801E5F0C(), m_project)) {
        fn_8036D94C(m_project);
    }
    return 1;
}

void Screen::SetState(int state, bool start) {
    m_state = state;
    if (start) {
        m_step = 0;
    } else {
        m_step = 1;
    }
    m_cursor.m_changed = 1;
}

bool Screen::LoadProject() {
    m_project = fn_802B5504(fn_801E5F0C(), s_projectName);
    if (m_project != 0) {
        UnkCsdHandle_80042DF0 handle = fn_8036D598(m_project, 0);
        return true;
    }
    return false;
}

void Screen::Setup() {
    m_cursor.m_lastState = -1;
    m_cursor.m_values[0] = lbl_8057D6A0.m_characters[0] - 0xE;
    m_cursor.m_values[1] = lbl_8057D6A0.m_characters[1] - 0xE;
    m_cursor.m_values[2] = 0;
    RCPtr<CScene> scene = fn_8036D660(m_project, "Kazari");
    fn_80367238(scene.Get(), "CharaOpen");
    CScene *s = scene.Get();
    fn_80366740(s, fn_803644A0(scene.Get(), "CharaOpen"));
    fn_80367140(scene.Get(), 1.0f);
    fn_803659DC(scene.Get(), 0);
    fn_803659CC(scene.Get(), 0);
    fn_803659E4(scene.Get());
    scene = fn_8036D660(m_project, "Buttons");
    if (fn_802B7CC0()) {
        RCPtr<CNode> node;
        node = fn_803666B4(scene.Get(), "select_button");
        fn_8036AD14(node.Get(), 3);
        node = fn_803666B4(scene.Get(), "back_button");
        fn_8036AD14(node.Get(), 3);
    }
    fn_80367238(scene.Get(), "ButtonsOpen");
    s = scene.Get();
    fn_80366740(s, fn_803644A0(scene.Get(), "ButtonOpen"));
    fn_80367140(scene.Get(), 1.0f);
    fn_803659DC(scene.Get(), 0);
    fn_803659E4(scene.Get());
    scene = fn_8036D660(m_project, "Arena");
    fn_80367238(scene.Get(), "ArenaOpen");
    s = scene.Get();
    fn_80366740(s, fn_803644A0(scene.Get(), "ArenaOpen"));
    fn_80367140(scene.Get(), -1.0f);
    fn_803659CC(scene.Get(), 0);
    scene = fn_8036D660(m_project, "1PCharaSelect");
    fn_80367238(scene.Get(), "1PClose");
    s = scene.Get();
    fn_80366740(s, fn_80364430(scene.Get(), "1PClose"));
    fn_80367140(scene.Get(), 1.0f);
    fn_803659DC(scene.Get(), 0);
    fn_803659E4(scene.Get());
    scene = fn_8036D660(m_project, "2PCharaSelect");
    fn_80367238(scene.Get(), "2PClose");
    s = scene.Get();
    fn_80366740(s, fn_80364430(scene.Get(), "2PClose"));
    fn_80367140(scene.Get(), 1.0f);
    fn_803659DC(scene.Get(), 0);
    fn_803659E4(scene.Get());
    scene = fn_8036D660(m_project, "Cursor1P");
    fn_80367238(scene.Get(), "Close");
    s = scene.Get();
    fn_80366740(s, fn_80364430(scene.Get(), "Close"));
    fn_80367140(scene.Get(), 1.0f);
    fn_803659DC(scene.Get(), 0);
    fn_803659E4(scene.Get());
    scene = fn_8036D660(m_project, "Cursor2P");
    fn_80367238(scene.Get(), "Close");
    s = scene.Get();
    fn_80366740(s, fn_80364430(scene.Get(), "Close"));
    fn_80367140(scene.Get(), 1.0f);
    fn_803659DC(scene.Get(), 0);
    fn_803659E4(scene.Get());
}

bool Screen::Open() {
    if (LoadProject()) {
        Setup();
        SetState(1, true);
        fn_802C839C(fn_801E5CA8(), 5);
    } else {
        SetState(4, true);
    }
    return 1;
}

bool Screen::SelectCharacters() {
    RCPtr<CScene> scene;
    RCPtr<CNode> node;
    switch (m_step) {
    case 0: {
        scene = fn_8036D660(m_project, "Kazari");
        node = fn_803666B4(scene.Get(), "tx_2p");
        fn_8036ACCC(node.Get(), &s_select2pOff);
        scene = fn_8036D660(m_project, "1PCharaSelect");
        fn_80367238(scene.Get(), "1POpen");
        CScene *s = scene.Get();
        fn_80366740(s, fn_803644A0(scene.Get(), "1POpen"));
        fn_803659E4(scene.Get());
        scene = fn_8036D660(m_project, "Cursor1P");
        fn_80367238(scene.Get(), "Open");
        s = scene.Get();
        fn_80366740(s, fn_803644A0(scene.Get(), "Open"));
        fn_803659E4(scene.Get());
        scene = fn_8036D660(m_project, "2PCharaSelect");
        fn_80367238(scene.Get(), "2POpen");
        s = scene.Get();
        fn_80366740(s, fn_803644A0(scene.Get(), "2POpen"));
        fn_803659E4(scene.Get());
        node = fn_803666B4(scene.Get(), "shadow_2p");
        fn_8036ACCC(node.Get(), &s_select2pShadowOff);
        m_player = 0;
        m_shown = 0;
        m_step = 1;
        break;
    }
    case 1:
        if (fn_8036D660(m_project, "Kazari").Get()->m_end) {
            if (!m_shown) {
                scene = fn_8036D660(m_project, "Cursor1P");
                if (scene->IsEnd()) {
                    fn_80367238(scene.Get(), "Select");
                    CScene *s = scene.Get();
                    fn_80366740(s, fn_803644A0(scene.Get(), "Select"));
                    fn_803659DC(scene.Get(), 1);
                    fn_803659E4(scene.Get());
                    m_shown = 1;
                }
            }
            if (fn_802B7D5C(1, 4) || fn_802B7D5C(0, 4)) {
                PlaySound(0xE003);
                lbl_8057D6A0.m_characters[0] = m_cursor.m_values[0] + 0xE;
                fn_80051734()->m_controllers[0] = fn_802B7CE0();
                scene = fn_8036D660(m_project, "Cursor1P");
                fn_80367238(scene.Get(), "On");
                CScene *s = scene.Get();
                fn_80366740(s, fn_803644A0(scene.Get(), "On"));
                fn_803659DC(scene.Get(), 0);
                fn_803659E4(scene.Get());
                fn_801D51A0(fn_80053164(), 0x18, -1, -1);
                if (lbl_8057D6A0.m_language == 0) {
                    m_timer = 2.39f;
                } else {
                    m_timer = 3.56f;
                }
                m_step = 2;
            } else if (fn_802B7D5C(2, 4)) {
                PlaySound(0xE004);
                m_nextPhase = 1;
                SetState(3, true);
            } else if (MoveCursor(&m_cursor.m_values[0], 4)) {
                PlaySound(0xE002);
            }
        }
        break;
    case 2:
        m_timer -= lbl_8057D6A0.m_dt;
        if (m_timer > 0.0f) {
            break;
        }
        {
            scene = fn_8036D660(m_project, "Cursor2P");
            fn_80367238(scene.Get(), "Open");
            CScene *s = scene.Get();
            fn_80366740(s, fn_803644A0(scene.Get(), "Open"));
            fn_803659DC(scene.Get(), 0);
            fn_803659E4(scene.Get());
            scene = fn_8036D660(m_project, "Kazari");
            node = fn_803666B4(scene.Get(), "tx_1p");
            fn_8036ACCC(node.Get(), &s_chosen1pOff);
            node = fn_803666B4(scene.Get(), "tx_2p");
            fn_8036ACCC(node.Get(), &s_chosen2pOn);
            scene = fn_8036D660(m_project, "1PCharaSelect");
            node = fn_803666B4(scene.Get(), "shadow_1p");
            fn_8036ACCC(node.Get(), &s_chosen1pShadowOff);
            scene = fn_8036D660(m_project, "2PCharaSelect");
            node = fn_803666B4(scene.Get(), "shadow_2p");
            fn_8036ACCC(node.Get(), &s_chosen2pShadowOn);
            m_player = 1;
            m_step = 3;
            m_shown = 0;
        }
        break;
    case 3:
        if (!m_shown) {
            scene = fn_8036D660(m_project, "Cursor2P");
            if (scene->IsEnd()) {
                fn_80367238(scene.Get(), "Select");
                CScene *s = scene.Get();
                fn_80366740(s, fn_803644A0(scene.Get(), "Select"));
                fn_803659DC(scene.Get(), 1);
                fn_803659E4(scene.Get());
                m_shown = 1;
            }
        }
        if (fn_802B7D5C(1, 5) || fn_802B7D5C(0, 5)) {
            PlaySound(0xE003);
            lbl_8057D6A0.m_characters[1] = m_cursor.m_values[1] + 0xE;
            fn_80051734()->m_controllers[1] = fn_802B7CE0();
            scene = fn_8036D660(m_project, "Cursor2P");
            fn_80367238(scene.Get(), "On");
            CScene *s = scene.Get();
            fn_80366740(s, fn_803644A0(scene.Get(), "On"));
            fn_803659DC(scene.Get(), 0);
            fn_803659E4(scene.Get());
            fn_801D51A0(fn_80053164(), 0x1A, -1, -1);
            if (lbl_8057D6A0.m_language == 0) {
                m_timer = 2.1f;
            } else {
                m_timer = 2.43f;
            }
            m_step = 4;
        } else if (fn_802B7D5C(2, 4) || fn_802B7D5C(2, 5)) {
            fn_801D515C(fn_80053164());
            PlaySound(0xE004);
            scene = fn_8036D660(m_project, "Cursor2P");
            fn_80367238(scene.Get(), "Close");
            CScene *s = scene.Get();
            fn_80366740(s, fn_803644A0(scene.Get(), "Close"));
            fn_803659DC(scene.Get(), 0);
            fn_803659E4(scene.Get());
            scene = fn_8036D660(m_project, "Kazari");
            node = fn_803666B4(scene.Get(), "tx_1p");
            fn_8036ACCC(node.Get(), &s_cancel1pOn);
            node = fn_803666B4(scene.Get(), "tx_2p");
            fn_8036ACCC(node.Get(), &s_cancel2pOff);
            scene = fn_8036D660(m_project, "1PCharaSelect");
            node = fn_803666B4(scene.Get(), "shadow_1p");
            fn_8036ACCC(node.Get(), &s_cancel1pShadowOn);
            scene = fn_8036D660(m_project, "2PCharaSelect");
            node = fn_803666B4(scene.Get(), "shadow_2p");
            fn_8036ACCC(node.Get(), &s_cancel2pShadowOff);
            m_step = 1;
        } else if (MoveCursor(&m_cursor.m_values[1], 5)) {
            PlaySound(0xE002);
        }
        break;
    case 4:
        m_timer -= lbl_8057D6A0.m_dt;
        if (m_timer > 0.0f) {
            break;
        }
        scene = fn_8036D660(m_project, "Kazari");
        node = fn_803666B4(scene.Get(), "tx_1p");
        fn_8036ACCC(node.Get(), &s_done1pOn);
        node = fn_803666B4(scene.Get(), "tx_2p");
        fn_8036ACCC(node.Get(), &s_done2pOn);
        scene = fn_8036D660(m_project, "1PCharaSelect");
        node = fn_803666B4(scene.Get(), "shadow_1p");
        fn_8036ACCC(node.Get(), &s_done1pShadowOn);
        scene = fn_8036D660(m_project, "2PCharaSelect");
        node = fn_803666B4(scene.Get(), "shadow_2p");
        fn_8036ACCC(node.Get(), &s_done2pShadowOn);
        m_player = 0;
        m_shown = 0;
        m_step = 5;
        break;
    case 5: {
        scene = fn_8036D660(m_project, "Kazari");
        fn_80367238(scene.Get(), "ArenaOpen");
        CScene *s = scene.Get();
        fn_80366740(s, fn_803644A0(scene.Get(), "ArenaOpen"));
        fn_80367140(scene.Get(), 1.0f);
        fn_803659E4(scene.Get());
        scene = fn_8036D660(m_project, "1PCharaSelect");
        fn_80367238(scene.Get(), "1PClose");
        s = scene.Get();
        fn_80366740(s, fn_803644A0(scene.Get(), "1PClose"));
        fn_803659E4(scene.Get());
        scene = fn_8036D660(m_project, "Cursor1P");
        fn_80367238(scene.Get(), "Close");
        s = scene.Get();
        fn_80366740(s, fn_803644A0(scene.Get(), "Close"));
        fn_803659DC(scene.Get(), 0);
        fn_803659E4(scene.Get());
        scene = fn_8036D660(m_project, "2PCharaSelect");
        fn_80367238(scene.Get(), "2PClose");
        s = scene.Get();
        fn_80366740(s, fn_803644A0(scene.Get(), "2PClose"));
        fn_803659E4(scene.Get());
        scene = fn_8036D660(m_project, "Cursor2P");
        fn_80367238(scene.Get(), "Close");
        s = scene.Get();
        fn_80366740(s, fn_803644A0(scene.Get(), "Close"));
        fn_803659DC(scene.Get(), 0);
        fn_803659E4(scene.Get());
        SetState(2, true);
        break;
    }
    }
    UpdateDisplay();
    return 1;
}

bool Screen::SelectArena() {
    RCPtr<CScene> scene;
    RCPtr<CNode> node;
    switch (m_step) {
    case 0: {
        scene = fn_8036D660(m_project, "Arena");
        fn_80367238(scene.Get(), "ArenaOpen");
        CScene *s = scene.Get();
        fn_80366740(s, fn_803644A0(scene.Get(), "ArenaOpen"));
        fn_80367140(scene.Get(), 1.0f);
        fn_803659DC(scene.Get(), 0);
        fn_803659E4(scene.Get());
        m_shown = 0;
        m_step = 1;
        break;
    }
    case 1:
        if (!m_shown) {
            scene = fn_8036D660(m_project, "Arena");
            if (!scene->IsEnd()) {
                break;
            }
            fn_80367238(scene.Get(), "Select");
            CScene *s = scene.Get();
            fn_80366740(s, fn_803644A0(scene.Get(), "Select"));
            fn_803659DC(scene.Get(), 1);
            fn_803659E4(scene.Get());
            m_shown = 1;
        } else {
            scene = fn_8036D660(m_project, "Arena");
            if (m_cursor.m_values[2] == 0) {
                fn_8036ACA8(fn_803666B4(scene.Get(), "tx_3match").Get());
                fn_8036ACCC(fn_803666B4(scene.Get(), "tx_1match").Get(), &s_arena1MatchOn);
            } else {
                fn_8036ACCC(fn_803666B4(scene.Get(), "tx_3match").Get(), &s_arena3MatchOn);
                fn_8036ACA8(fn_803666B4(scene.Get(), "tx_1match").Get());
            }
        }
        if (fn_802B7D5C(1, 4) || fn_802B7D5C(1, 5)) {
            PlaySound(0xE003);
            if (m_cursor.m_values[2] == 0) {
                StartMatch();
            } else {
                StartBattle();
            }
            m_cursor.m_lastState = m_state;
            m_nextPhase = 0;
            SetState(3, true);
        } else if (fn_802B7D5C(2, 4) || fn_802B7D5C(2, 5)) {
            PlaySound(0xE004);
            scene = fn_8036D660(m_project, "Arena");
            fn_80367238(scene.Get(), "ArenaOpen");
            CScene *s = scene.Get();
            fn_80366740(s, fn_80364430(scene.Get(), "ArenaOpen"));
            fn_80367140(scene.Get(), -1.0f);
            fn_803659CC(scene.Get(), 0);
            fn_803659E4(scene.Get());
            scene = fn_8036D660(m_project, "Kazari");
            fn_80367140(scene.Get(), -1.0f);
            fn_803659E4(scene.Get());
            SetState(1, true);
        } else if (fn_802B7D5C(3, 4) || fn_802B7D5C(3, 5)) {
            if (m_cursor.m_values[2] != 0) {
                m_cursor.m_values[2]--;
                PlaySound(0xE002);
            }
        } else if (fn_802B7D5C(4, 4) || fn_802B7D5C(4, 5)) {
            if (m_cursor.m_values[2] < 3) {
                m_cursor.m_values[2]++;
                PlaySound(0xE002);
            }
        }
        break;
    }
    UpdateDisplay();
    return 1;
}

bool Screen::Leave() {
    switch (m_step) {
    case 0:
        fn_80173878(1, 1, 1.0f);
        m_step = 1;
        break;
    case 1:
        if (fn_80044E10() == 1) {
            fn_802B7528(fn_801E5E00(), m_nextPhase);
        }
        break;
    }
    return 1;
}

/* Moves cursor about the 2 x 3 grid with pad; returns whether it moved (name inferred). */
bool Screen::MoveCursor(s32 *cursor, int pad) {
    int column = *cursor % 2;
    int row = *cursor / 2;
    bool moved = false;
    if (fn_802B7D5C(3, pad)) {
        if (--row < 0) {
            row = 0;
        } else {
            moved = true;
        }
    } else if (fn_802B7D5C(4, pad)) {
        if (++row > 2) {
            row = 2;
        } else {
            moved = true;
        }
    } else if (fn_802B7D5C(5, pad)) {
        if (--column < 0) {
            column = 0;
        } else {
            moved = true;
        }
    } else if (fn_802B7D5C(6, pad)) {
        if (++column > 1) {
            column = 1;
        } else {
            moved = true;
        }
    }
    *cursor = column + row * 2;
    return moved;
}

/* Places the cursors and shadows on the picked characters and shows the arena choice. */
void Screen::UpdateDisplay() {
    RCPtr<CScene> kazari;
    RCPtr<CScene> cursor;
    RCPtr<CScene> select;
    RCPtr<CNode> node;
    UnkCsdPosition_8036AE98 position;
    kazari = fn_8036D660(m_project, "Kazari");
    node = fn_803666B4(kazari.Get(), "Null_box");
    UnkCsdPosition_8036AE98 origin = fn_8036AE98(node.operator->());
    float x = origin.x;
    float y = origin.y;
    node = fn_803666B4(kazari.Get(), "box_chara");
    position = fn_8036AE98(node.operator->());
    x += position.x;
    y += position.y;
    for (int i = 0; i < 6; i++) {
        fn_8036AD78(fn_803666B4(kazari.Get(), s_boxNames[i]).Get(), 0);
    }
    int character;
    for (int i = 0; i < 2; i++) {
        if (i == 0) {
            cursor = fn_8036D660(m_project, "Cursor1P");
            select = fn_8036D660(m_project, "1PCharaSelect");
        } else {
            cursor = fn_8036D660(m_project, "Cursor2P");
            select = fn_8036D660(m_project, "2PCharaSelect");
        }
        character = m_cursor.m_values[i];
        if (i <= m_player) {
            node = fn_803666B4(kazari.Get(), s_boxNames[character]);
            fn_8036AD78(node.Get(), 1);
        }
        position = fn_8036AE98(node.operator->());
        position.x += x;
        position.y += y;
        node = fn_803666B4(cursor.Get(), s_cursorNames[i]);
        fn_8036AE70(node.Get(), &position);
        node = fn_803666B4(select.Get(), s_shadowNames[i]);
        fn_8036AD14(node.Get(), character);
    }
    RCPtr<CScene> arena = fn_8036D660(m_project, "Arena");
    if (m_cursor.m_values[2] == 0) {
        for (int i = 0; i < 4; i++) {
            fn_8036AC60(fn_803666B4(arena.Get(), "tx_3match").Get(), i, &s_matchShown);
            fn_8036AC60(fn_803666B4(arena.Get(), "tx_1match").Get(), i, &s_matchHidden);
        }
        fn_8036AD14(fn_803666B4(arena.Get(), "pic1").Get(), 1);
        fn_8036AD14(fn_803666B4(arena.Get(), "pic2").Get(), 1);
        fn_8036AD14(fn_803666B4(arena.Get(), "pic3").Get(), 1);
        fn_8036AD78(fn_803666B4(arena.Get(), "pic1_on").Get(), 1);
        fn_8036AD78(fn_803666B4(arena.Get(), "pic2_on").Get(), 1);
        fn_8036AD78(fn_803666B4(arena.Get(), "pic3_on").Get(), 1);
    } else {
        for (int i = 0; i < 4; i++) {
            fn_8036AC60(fn_803666B4(arena.Get(), "tx_3match").Get(), i, &s_arenaHidden);
            fn_8036AC60(fn_803666B4(arena.Get(), "tx_1match").Get(), i, &s_arenaShown);
        }
        fn_8036AD14(fn_803666B4(arena.Get(), "pic1").Get(), 0);
        fn_8036AD14(fn_803666B4(arena.Get(), "pic2").Get(), 0);
        fn_8036AD14(fn_803666B4(arena.Get(), "pic3").Get(), 0);
        fn_8036AD78(fn_803666B4(arena.Get(), "pic1_on").Get(), m_cursor.m_values[2] != 1);
        fn_8036AD78(fn_803666B4(arena.Get(), "pic2_on").Get(), m_cursor.m_values[2] != 2);
        fn_8036AD78(fn_803666B4(arena.Get(), "pic3_on").Get(), m_cursor.m_values[2] != 3);
    }
}

/* Starts the three-stage match. */
void Screen::StartBattle() {
    fn_801E5858(fn_801E2128(), 5);
    StageSequence::Manager::SetNext(3);
    lbl_80584720.m_stage = s_matchStages[m_cursor.Values()[2] - 1];
    fn_802062FC(lbl_8057D6A0.m_record);
    fn_800CBBFC(lbl_8057D6A0.field_0x148, 1, 3);
}

/* Starts a battle on the chosen arena. */
void Screen::StartMatch() {
    fn_801E5858(fn_801E2128(), 5);
    StageSequence::Manager::SetNext(4);
    for (int i = 0; i < 3; i++) {
        lbl_80584720.m_stages[i] = s_matchStages[i];
    }
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

} // namespace BattleMode
} // namespace AdvReal
