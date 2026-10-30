/* folderTargetWindow (class name from the RTTI of its shared_ptr control block in
 * Game/TGiPause.cpp): the target window of the pause screen. It has a panel for the dark mission
 * and one for the hero mission (scenes TargetDark and TargetHero of the scene list
 * Root/TargetWindow); a panel opens while the pause menu shows its player and that player's
 * mission counts something, and prints the count with a number display. The file name and the
 * names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first).
 *
 * NonMatching: 8 of 10 functions and all the data match. In the constructor and in Update GC/2.7
 * assigns the zeroes that start a loop in another order (which register gets the li and which the
 * copy), and in Update it swaps the registers of the two words the panel's player check compares;
 * the loop shapes, declaration orders and operand orders tried did not change it. */
#define TGI_NO_NUMBER_COLORS
#define TGI_HANDLE_ASSIGN
#include <dolphin/types.h>
#include <stdio.h>
#include "Game/CsdRCPtr.h"
#include "Game/CsdSceneCtrl.h"
#include "Game/Mission.h"
#include "Game/TGi.h"
#include "Game/TGiPause.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Chao::CSD::CScene;
using Chao::CSD::RCPtr;

/* The player manager (as far as this unit reads it; see Game/BDHologram.cpp). */
class UnkPlayerManager_8006D6A8 {
  public:
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ s32 m_pausePlayer; /* the player the pause menu shows (meaning inferred) */
};

extern "C" {
/* The scene list at path of project, and a scene of a scene list by name (see
 * Game/folderPause.cpp). */
UnkCsdHandle_80042DF0 fn_8036D2FC(Chao::CSD::CProject *project, const char *path, void *factory);
RCPtr<CScene> fn_8036F470(void *object, const char *name);
/* Whether a scene's queued motions have all been played (see Game/CsdSceneSet.h). */
bool fn_8003DBB0(TPjsSceneTour *scene);
/* The player manager, and a player's mission (see Game/BASwitch.cpp). */
UnkPlayerManager_8006D6A8 *fn_8006D6A8(void);
boost::weak_ptr<Mission::Mission> fn_800740E4(UnkPlayerManager_8006D6A8 *manager, int player);
}

/* Handle comparison (name inferred; as in Game/folderPause.cpp). */
inline bool operator!=(const UnkCsdHandle_80042DF0 &a, const UnkCsdHandle_80042DF0 &b) {
    void *pa = a.m_object != 0 ? *(void **)a.m_object : 0;
    void *pb = b.m_object != 0 ? *(void **)b.m_object : 0;
    if (pa != pb) {
        return true;
    }
    return false;
}

/* How a panel prints its count (struct name and member names inferred). */
struct UnkTargetNumbers_805224F0 {
    /* 0x00 */ s32 m_x;
    /* 0x04 */ s32 m_y;
    /* 0x08 */ float m_scaleX;
    /* 0x0C */ float m_scaleY;
    /* 0x10 */ TGiColorSet m_colors;
    /* 0x50 */ float m_depth;
    /* 0x54 */ float m_spacing;
    /* 0x58 */ s32 field_0x58;
};

/* A panel's scene and motions, and its count (struct name and member names inferred). */
struct UnkTargetPanel_805224F0 {
    /* 0x00 */ const char *m_scene;
    /* 0x04 */ const char *m_open;
    /* 0x08 */ const char *m_close;
    /* 0x0C */ UnkTargetNumbers_805224F0 m_numbers;
};

static UnkTargetPanel_805224F0 s_panels[2] = {
    {"TargetDark", "DarkOpen", "DarkClose",
     {24, 128, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}},
      0.0f, -2.7f, 1}},
    {"TargetHero", "HeroOpen", "HeroClose",
     {545, 128, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}},
      0.0f, -2.7f, 1}},
};

/* The digits' scale for counts of three digits (meaning inferred). */
static float s_smallScale = 0.8f;

/* The players of the panels: dark and hero. */
static s32 s_players[2] = {0, 2};

/* The player of panel index (name inferred). */
inline s32 PanelPlayer(int index) {
    return s_players[index];
}

folderTargetWindow::folderTargetWindow(Chao::CSD::CProject *project) {
    InitScenes(project);
    for (int i = 0; i < 2; i++) {
        m_panels[i].m_scene.Reset();
        m_panels[i].m_state = 0;
        m_panels[i].m_selected = 0;
    }
}

folderTargetWindow::~folderTargetWindow() {}

void folderTargetWindow::InitScenes(Chao::CSD::CProject *project) {
    m_sceneList = fn_8036D2FC(project, "Root/TargetWindow", 0);
    if (m_sceneList != UnkCsdHandle_80042DF0(0)) {
        for (int i = 0; i < 2; i++) {
            m_panels[i].m_scene.SetScene(fn_8036F470(m_sceneList.Get(), s_panels[i].m_scene));
        }
    }
    fn_800BCEA8(&m_numbers, lbl_805EE68C, "ad_hpasc1");
    fn_800BCC54(&m_numbers, 64, 64, 16, 18, 4);
    InitNumbers();
}

void folderTargetWindow::DrawCount(int index) {
    boost::shared_ptr<Mission::Mission> mission = fn_800740E4(fn_8006D6A8(), PanelPlayer(index)).lock();
    if (mission) {
        char text[16];
        sprintf(text, "%d/%d", mission->GetCount(0), mission->GetTarget());
        if (mission->GetTarget() >= 100) {
            SetNumberScale(s_smallScale);
        } else {
            SetNumberScale(1.0f);
        }
        fn_800BC7BC(&m_numbers, text, s_panels[index].m_numbers.m_x, 0, s_panels[index].m_numbers.m_y);
    }
}

bool folderTargetWindow::Update() {
    int i;
    for (i = 0; i < 2; i++) {
        /* Not original: the panel's address written out, so that it is computed as retail does
         * (the index's offset plus the array's, then this). */
        Panel *panel = (Panel *)((u8 *)this + 4 + i * sizeof(Panel));
        boost::shared_ptr<Mission::Mission> mission = fn_800740E4(fn_8006D6A8(), PanelPlayer(i)).lock();
        if (mission) {
            if (mission->GetFlag()) {
                switch (mission->GetId()) {
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 17:
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                case 26:
                case 27:
                case 28:
                case 29:
                case 30:
                case 31:
                case 32:
                case 34:
                case 35:
                    panel->m_counting = 1;
                    break;
                default:
                    panel->m_counting = 0;
                    break;
                }
            }
        } else {
            panel->m_counting = 0;
        }
        if (mission) {
            switch (panel->m_state) {
            case 0:
                {
                    UnkPlayerManager_8006D6A8 *manager = fn_8006D6A8();
                    if (manager->m_pausePlayer == mission->m_player) {
                        panel->m_selected = 1;
                    } else {
                        panel->m_selected = 0;
                    }
                }
                if (panel->m_selected && panel->m_counting) {
                    m_panels[i].m_scene.Play(s_panels[i].m_open, 1);
                    m_panels[i].m_state = 1;
                }
                break;
            case 1:
                if (fn_8003DBB0(&panel->m_scene)) {
                    panel->m_state = 2;
                }
                break;
            case 2:
                {
                    UnkPlayerManager_8006D6A8 *manager = fn_8006D6A8();
                    if (manager->m_pausePlayer != mission->m_player) {
                        panel->m_selected = 0;
                    } else {
                        panel->m_selected = 1;
                    }
                }
                if (!panel->m_selected || !panel->m_counting) {
                    m_panels[i].m_scene.Play(s_panels[i].m_close, 1);
                    m_panels[i].m_state = 3;
                }
                break;
            case 3:
                if (fn_8003DBB0(&panel->m_scene)) {
                    panel->m_state = 0;
                }
                break;
            }
        }
    }
    int busy = 0;
    for (i = 0; i < 2; i++) {
        busy += m_panels[i].m_scene.Update(false);
    }
    return busy != 0;
}

void folderTargetWindow::InitNumbers() {
    for (int i = 0; i < 2; i++) {
        const UnkTargetNumbers_805224F0 *numbers = &s_panels[i].m_numbers;
        fn_800BCB7C(&m_numbers, &numbers->m_colors);
        fn_800BCB2C(&m_numbers, numbers->m_depth);
        m_numbers.SetScale(numbers->m_scaleX, numbers->m_scaleY);
        m_numbers.SetField14(numbers->m_spacing);
    }
}

void folderTargetWindow::SetNumberScale(float scale) {
    for (int i = 0; i < 2; i++) {
        const UnkTargetNumbers_805224F0 *numbers = &s_panels[i].m_numbers;
        m_numbers.SetScale(numbers->m_scaleX * scale, numbers->m_scaleY);
    }
}

void folderTargetWindow::Draw() {
    for (int i = 0; i < 2; i++) {
        switch (m_panels[i].m_state) {
        case 0:
            break;
        case 1:
            break;
        case 2:
            DrawCount(i);
            break;
        case 3:
            break;
        }
    }
}
