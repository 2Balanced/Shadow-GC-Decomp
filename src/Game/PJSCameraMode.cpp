/* PJSCamera::PJSCameraMode (class name and base from RTTI; Game/PJSCameraMode.h): the base of the
 * game's own camera modes. Its constructor and destructor only set the vtable. File name from the
 * class. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/PJSCameraMode.h"
#include "Game/CameraModeUser.h"

using PJSCamera::PJSCameraMode;
using SonicteamUSA::System::Camera::CameraMode;

PJSCameraMode::PJSCameraMode(UnkCameraOwner_8000E9BC *owner, int arg) : CameraMode(owner, arg) {}

PJSCameraMode::~PJSCameraMode() {}
