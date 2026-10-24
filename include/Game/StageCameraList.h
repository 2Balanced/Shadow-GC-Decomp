#ifndef GAME_STAGECAMERALIST_H
#define GAME_STAGECAMERALIST_H

#include "Game/PtrVector.h"
#include <dolphin/types.h>
#include "size_assert.h"

class SetCameraDataManager;
class Task;

namespace PJSCamera {
class StageCameraManager;
} // namespace PJSCamera

/* The camera manager of the player code (class name unknown; see Game/CameraModeUser.h): a
 * function-local static object behind fn_80099954 (constructor 0x801CF160, destructor 0x801CF078;
 * Game/StageCameraList.cpp). For a stage it loads the stage's camera set data and makes a stage
 * camera for each screen; other code puts camera modes and requests on the stage camera of the
 * mode's screen through it (fn_801CE9F4, fn_801CEA54, fn_801CED24, fn_801CEAD4). Member names are
 * inferred. */
class UnkCameraManager_80099954 {
  public:
    UnkCameraManager_80099954();
    ~UnkCameraManager_80099954();

    SetCameraDataManager *GetSetCameraData() {
        return m_setCameraData;
    }
    /* The stage camera of a screen, or null. */
    PJSCamera::StageCameraManager *GetStageCamera(int screen);
    /* Unused in the game (see Game/StageCameraList.cpp). */
    void ResetStageCameras();
    void ResetStageCamera(int screen);

    /* 0x00 */ s32 m_numScreens; /* -1 without a stage */
    /* 0x04 */ SetCameraDataManager *m_setCameraData;
    /* 0x08 */ std::vector<PJSCamera::StageCameraManager *> m_cameras;
};
SIZE_ASSERT(UnkCameraManager_80099954, 0x14);

/* The PJS camera tasks (class name unknown): a function-local static object behind fn_800D51EC
 * (Game/PJSCameraPostTask.cpp). The task that runs before the camera manager's update
 * (PJSCamera::PJSCameraPreTask) is the parent of the stage cameras, the one that runs after it and
 * updates the camera manager (PJSCamera::PJSCameraPostTask) the parent of their post processes;
 * Start makes both for the first user and counts the users. The constructor gives the camera
 * manager the game's camera factory; the destructor ends both tasks. Member names are inferred. */
struct UnkStageTasks_800D51EC {
    UnkStageTasks_800D51EC();
    ~UnkStageTasks_800D51EC();
    /* Not called in the game. */
    void Start();

    /* 0x0 */ Task *m_preTask;
    /* 0x4 */ Task *m_postTask;
    /* 0x8 */ u32 m_numUsers;
};

extern "C" {
UnkStageTasks_800D51EC *fn_800D51EC(void);
/* Makes the stage cameras of a stage, and takes one off the list (behaviour inferred;
 * Game/StageCameraList.cpp). */
void fn_801CE8CC(UnkCameraManager_80099954 *manager, s32 stageId);
void fn_801CE760(UnkCameraManager_80099954 *manager, PJSCamera::StageCameraManager *camera);
}

#endif
