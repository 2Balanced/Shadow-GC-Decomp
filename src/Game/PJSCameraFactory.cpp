/* PJSCamera::PJSCameraFactory (class name and base from RTTI; Game/PJSCameraFactory.h): the game's
 * camera factory, which registers the game's camera modes (mode kinds -1 to 14) and adjusts
 * (types -1 to 4). File name from the class; other names are inferred. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). The unit's .rodata is
 * reached through one base and so kept whole. */
#define MSL_LIST_ACCESSORS
#include <list>

/* The unreferenced constant block of the original headers before the CharaColliBase names (as in
 * Gong.cpp; the bytes are reproduced). */
static const float s_zeros[7] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
#include "Game/CharaColliBaseCharInfo.h"

#include "Game/PJSCameraFactory.h"
#include "Game/CameraManagerUnit.h"
#include "Game/CameraModeFixEye.h"
#include "Game/CameraModeFixEye2.h"
#include "Game/CameraModeFixLook.h"
#include "Game/CameraModeFixLook2.h"
#include "Game/CameraModeFixLookChar.h"
#include "Game/CameraModeFree.h"
#include "Game/CameraModeGeneral.h"
#include "Game/CameraModeLookFront.h"
#include "Game/CameraModeSetCamEdit.h"
#include "Game/CameraModeSetEditor.h"
#include "Game/CameraModeTestPlayer.h"
#include "Game/CameraModeTestPlayerNoRot.h"
#include "Game/PJSCameraAdjust.h"
#include "Game/PJSCameraTargetInfo.h"
#include "size_assert.h"

using PJSCamera::PJSCameraFactory;
using SonicteamUSA::System::Camera::CameraMode;
using SonicteamUSA::System::Camera::CameraTargetInfo;

PJSCameraFactory::PJSCameraFactory() {
    AddMode(-1, "Null", 0, 0);
    AddMode(0, "Command", 0, 0);
    AddMode(1, "Free", PJSCamera::CameraModeFree::Create, 0);
    AddMode(2, "TestPlayer", PJSCamera::CameraModeTestPlayer::Create, 0);
    AddMode(3, "TestPlayerNoRot", PJSCamera::CameraModeTestPlayerNoRot::Create, 0);
    AddMode(5, "SetEditor", PJSCamera::CameraModeSetEditor::Create, 0);
    AddMode(6, "SetCamEdit", PJSCamera::CameraModeSetCamEdit::Create, 0);
    AddMode(7, "FixEye", PJSCamera::CameraModeFixEye::Create, 2);
    AddMode(8, "FixLook", PJSCamera::CameraModeFixLook::Create, 0);
    AddMode(10, "General", PJSCamera::CameraModeGeneral::Create, 0);
    AddMode(11, "LookFront", PJSCamera::CameraModeLookFront::Create, 0);
    AddMode(12, "FixEye2", PJSCamera::CameraModeFixEye2::Create, 3);
    AddMode(13, "FixLook2", PJSCamera::CameraModeFixLook2::Create, 1);
    AddMode(14, "FixLookChar", PJSCamera::CameraModeFixLookChar::Create, 0);
    AddAdjust(-1, "Void", 0);
    AddAdjust(0, "None", 0);
    AddAdjust(1, "LinerWorld", PJSCamera::AdjustLinerWorld::Create);
    AddAdjust(2, "LinerLocal", PJSCamera::AdjustLinerLocal::Create);
    AddAdjust(3, "OnlyAov", PJSCamera::AdjustOnlyAov::Create);
    AddAdjust(4, "CosRot", PJSCamera::AdjustCosRot::Create);
}

PJSCameraFactory::~PJSCameraFactory() {}

CameraTargetInfo *PJSCameraFactory::CreateTarget() {
    return new PJSCamera::PJSCameraTargetInfo();
}
