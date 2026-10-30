#ifndef GAME_TGIPAUSE_H
#define GAME_TGIPAUSE_H

/* TGiPause (class name from RTTI) and the three panels of the pause screen it owns: folderPause,
 * folderStoryRoute and folderTargetWindow (class names from the RTTI of their shared_ptr control
 * blocks). TGiPause loads the CSD projects of the pause menu and of the target window and creates
 * the panels on them; each panel is its own unit after Game/TGiPause.cpp. Member names are inferred
 * unless the RTTI gives them. */

#include "Game/CsdSceneCtrl.h"
#include "Game/SavedataAnalyzer.h"
#include "Game/TGi.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class Render;
struct RednerDrawArgs;

/* The pause manager (Game/TPauseManager.cpp; reduced to the members the pause screen reads). */
class TPauseManager {
  public:
    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ s32 m_state; /* 2 and 3 while the menu is open */
    /* 0x34 */ s32 m_player; /* the player whose mission the menu shows */
    /* 0x38 */ s32 m_cursor;
    /* 0x3C */ u8 m_disabled;
    /* 0x3D */ u8 m_canRestart;
    /* 0x3E */ u8 field_0x3E[0x4D - 0x3E];
    /* 0x4D */ u8 field_0x4D; /* cleared while the pause screen's panels are open */
};

/* The one pause manager (Game/TPauseManager.cpp), and folderPause's accessor to it. */
extern TPauseManager *lbl_805EF9F4;
extern "C" TPauseManager *fn_800BADE8(void);

/* The pause menu proper: the mission board, the player icons, the cursor and the resume/restart/
 * quit choices (fn_800BACF0 constructs it, fn_800BAC34 destroys it). The scenes are those of the
 * project's scene list Root/Pause. */
class folderPause {
  public:
    folderPause(Chao::CSD::CProject *project);
    ~folderPause();

    /* Advances the menu; true while a scene is still playing (name inferred). */
    bool Update();
    /* Draws the mission's text (name inferred). */
    void Draw();
    /* Finds the scenes and hides the missing missions' icons (name inferred). */
    void InitScenes(Chao::CSD::CProject *project);
    /* Opens the menu (name inferred). */
    void Open();
    /* Follows the manager's cursor and player (names inferred). */
    void UpdateCursor();
    void UpdatePlayer();

    /* 0x00 */ s32 m_state;
    /* 0x04 */ u8 m_showStoryRoute; /* whether the story route is shown (meaning inferred) */
    /* 0x08 */ UnkCsdHandle_80042DF0 m_sceneList;
    /* 0x0C */ TPjsSceneTour m_mission;
    /* 0x20 */ TPjsSceneTour m_heroIcon;
    /* 0x34 */ TPjsSceneTour m_normalIcon;
    /* 0x48 */ TPjsSceneTour m_darkIcon;
    /* 0x5C */ TPjsSceneTour m_missionBoard;
    /* 0x70 */ TPjsSceneTour m_changeMission;
    /* 0x84 */ TPjsSceneTour m_pause;
    /* 0x98 */ TPjsSceneTour m_text;
    /* 0xAC */ TPjsSceneTour m_storyRouteText;
    /* 0xC0 */ s32 m_managerState;
    /* 0xC4 */ s32 m_player;
    /* 0xC8 */ s32 m_cursor;
};
SIZE_ASSERT(folderPause, 0xCC);

/* The story route chart of the pause screen (Game/folderStoryRoute.cpp). The scenes are those of
 * the project's scene list Root/StoryRoute. */
class folderStoryRoute {
  public:
    folderStoryRoute(Chao::CSD::CProject *project);
    ~folderStoryRoute();

    /* Advances the chart; true while a scene is still playing (name inferred). */
    bool Update();
    /* Finds the scenes (name inferred). */
    void InitScenes(Chao::CSD::CProject *project);
    /* Shows the stages and endings reached, the lines gone along and the player icons (names
     * inferred). */
    void UpdateStages();
    void UpdateLines();
    void UpdateIcons();

    /* 0x00 */ s32 m_state;
    /* 0x04 */ UnkCsdHandle_80042DF0 m_sceneList;
    /* 0x08 */ TPjsSceneTour m_route;  /* StoryRoute */
    /* 0x1C */ TPjsSceneTour m_stages; /* SRIconsBG */
    /* 0x30 */ TPjsSceneTour m_icons;  /* SRIcons */
    /* 0x44 */ s32 m_managerState;
    /* 0x48 */ s32 m_player;
    /* 0x4C */ s32 m_cursor;
    /* 0x50 */ SavedataAnalyzer::StoryCheck m_check;
};
SIZE_ASSERT(folderStoryRoute, 0x150);

/* The window of the mission's targets (Game/folderTargetWindow.cpp): a panel for the dark and for
 * the hero mission, which shows the mission's count while the pause menu shows that mission. The
 * scenes are those of the project's scene list Root/TargetWindow. */
class folderTargetWindow {
  public:
    /* A panel (struct name and member names inferred). */
    struct Panel {
        /* 0x00 */ s32 m_state;
        /* 0x04 */ u8 m_selected; /* the pause menu shows this panel's player */
        /* 0x05 */ u8 m_counting; /* the player's mission counts something */
        /* 0x08 */ TPjsSceneTour m_scene;
    };

    folderTargetWindow(Chao::CSD::CProject *project);
    ~folderTargetWindow();

    /* Advances the window; true while a scene is still playing (name inferred). */
    bool Update();
    /* Draws the counts (name inferred). */
    void Draw();
    /* Finds the scenes and loads the font (name inferred). */
    void InitScenes(Chao::CSD::CProject *project);
    /* Draws the count of panel index's mission (name inferred). */
    void DrawCount(int index);
    /* Sets up the number display, and scales its digits (names inferred). */
    void InitNumbers();
    void SetNumberScale(float scale);

    /* 0x000 */ UnkCsdHandle_80042DF0 m_sceneList;
    /* 0x004 */ Panel m_panels[2];
    /* 0x03C */ TGiNumberDisplay m_numbers;
};
SIZE_ASSERT(folderTargetWindow, 0x51C);

/* The CSD projects of the pause screen (struct name unknown). */
struct TGiPauseProjects {
    TGiPauseProjects();
    ~TGiPauseProjects() {}

    /* 0x00 */ s32 m_loaded;
    /* 0x04 */ Chao::CSD::CProject m_pause;  /* CsdFiles/pause/pause... */
    /* 0x50 */ Chao::CSD::CProject m_window; /* CsdFiles/TWindow */
};

class TGiPause : public Task {
  public:
    TGiPause(Task *parent);
    virtual ~TGiPause();
    virtual void Tick(float dt);

    /* The path of the pause menu's project (name inferred). */
    static const char *GetProjectPath();
    /* Loads the projects and the textures; true on success (name inferred). */
    bool Load();
    /* Loads the texture dictionary in file path (name inferred). */
    bool LoadTextures(const char *path);
    /* The Render's draw callback (name inferred). */
    void Draw(RednerDrawArgs &args);

    /* 0x28 */ TGiPauseProjects m_projects;
    /* 0xC4 */ boost::shared_ptr<folderPause> m_pause;
    /* 0xCC */ boost::shared_ptr<folderStoryRoute> m_storyRoute;
    /* 0xD4 */ boost::shared_ptr<folderTargetWindow> m_targetWindow;
    /* 0xDC */ boost::shared_ptr<Render> m_render;
    /* 0xE4 */ void *m_textures; /* a RenderWare texture dictionary */
};
SIZE_ASSERT(TGiPause, 0xE8);

#endif
