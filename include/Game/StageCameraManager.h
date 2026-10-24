#ifndef GAME_STAGECAMERAMANAGER_H
#define GAME_STAGECAMERAMANAGER_H

#include "Game/CameraModeUser.h"
#include "Game/ExternalCameraRequest.h"
#include "Game/Task.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

namespace SonicteamUSA {
namespace System {
namespace Camera {
class CameraRequest;
} // namespace Camera
} // namespace System
} // namespace SonicteamUSA

/* The state of a stage camera (class name unknown; 0xA8 bytes, constructor 0x801CBB30):
 * Game/StageCameraManager.cpp defines it. */
class UnkStageCamera_801CBB30;

namespace PJSCamera {

/* An object the stage camera deletes with itself (class name unknown; a word comes before its
 * vtable pointer; these units only delete it). */
class UnkStageCameraObject_801C96A4 {
  public:
    /* 0x0 */ u32 field_0x0;

    virtual ~UnkStageCameraObject_801C96A4();
};

/* StageCameraManager (class name and base from RTTI; vtable 0x8053C2EC): the camera of a player on
 * the stage (Game/StageCameraManager.cpp). Each frame it puts the camera mode of the camera area
 * the player is in, of the topmost external request, or the default one, on the player's camera
 * unit; all its state lives in a separate object it owns (UnkStageCamera_801CBB30, class name
 * unknown). Method names are inferred. */
class StageCameraManager : public Task {
  public:
    StageCameraManager(Task *parent, int player);
    virtual ~StageCameraManager();
    virtual void Tick(float dt);

    /* Takes an external mode off the camera (as set by SetExternalMode). */
    void ReleaseExternalMode(boost::shared_ptr<SonicteamUSA::System::Camera::CameraMode> mode);
    /* Puts a mode on the camera from outside the stage data. */
    void SetExternalMode(boost::shared_ptr<SonicteamUSA::System::Camera::CameraMode> mode,
                         int blendKind, const UnkCameraBlend_8000B2C0 *blend, u32 flags, int arg);
    void RemoveRequest(boost::shared_ptr<ExternalCameraRequest> request);
    void AddRequest(boost::shared_ptr<ExternalCameraRequest> request);
    int GetPlayer();
    /* Back to the stage's own camera. */
    void Reset();

    static const char *s_taskName;

    /* 0x28 */ boost::scoped_ptr<UnkStageCamera_801CBB30> m_impl;
    /* 0x2C */ UnkStageCameraObject_801C96A4 *field_0x2C;
};
SIZE_ASSERT(StageCameraManager, 0x30);

} // namespace PJSCamera

struct UnkCameraModeArgs_8000EBB0;

/* The kind of the stage's general camera mode (10), its arguments and its blend, and the default
 * blend kind (2) (names unknown; Game/StageCameraModeHolder.cpp). */
extern const int lbl_805F5D18;
extern UnkCameraModeArgs_8000EBB0 lbl_805812E4;
extern UnkCameraBlend_8000B2C0 lbl_8058135C;
extern int lbl_805E87D8;

/* The general camera mode of a player kept ready by the stage camera (class name unknown; its
 * constructor 0x801CF3C0 makes a mode of kind 10 with the stage's default parameters; constructor
 * and destructor are in Game/StageCameraModeHolder.cpp). */
class UnkCameraModeHolder_801CF3C0 {
  public:
    UnkCameraModeHolder_801CF3C0(int player);
    ~UnkCameraModeHolder_801CF3C0();

    boost::shared_ptr<SonicteamUSA::System::Camera::CameraMode> GetMode() {
        return m_mode;
    }

    /* 0x0 */ boost::shared_ptr<SonicteamUSA::System::Camera::CameraMode> m_mode;
};

/* A general camera mode requested over the stage's modes while the camera stick is used (class
 * name unknown; constructor 0x801CF664, destructor 0x801CF5F8; Release 0x801CF520 withdraws the
 * request, Request 0x801CF558 makes it; names and meanings inferred; Game/StageCameraHandle.cpp). */
class UnkCameraHandle_801CF664 {
  public:
    UnkCameraHandle_801CF664(int player);
    ~UnkCameraHandle_801CF664();
    void Release();
    void Request();

    bool IsRequested() {
        return m_request.get() != 0;
    }

    /* 0x00 */ bool m_active;
    /* 0x04 */ boost::shared_ptr<SonicteamUSA::System::Camera::CameraMode> m_mode;
    /* 0x0C */ boost::shared_ptr<SonicteamUSA::System::Camera::CameraRequest> m_request;
};

#endif
