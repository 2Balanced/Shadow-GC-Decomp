#ifndef GAME_PJSCAMERAMODE_H
#define GAME_PJSCAMERAMODE_H

#include "Game/CameraModeUser.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"

/* The camera manager a mode is created for (class name unknown; the first argument of a mode's
 * constructor, handed on to CameraMode's, which copies it to 0x34 like a
 * UnkCameraModeArgs_8000EBB0 when it is not null). */
class UnkCameraOwner_8000E9BC;

namespace PJSCamera {

/* PJSCameraMode (class name and base from RTTI; Game/PJSCameraMode.cpp): the base of the game's
 * own camera modes (CameraModeFixEye, CameraModeFixLook, CameraModeFixLookChar, ...). It adds no
 * members of its own; its constructor (0x801D0730) and destructor (0x801D06D8) only set the
 * vtable. */
class PJSCameraMode : public SonicteamUSA::System::Camera::CameraMode {
  public:
    PJSCameraMode(UnkCameraOwner_8000E9BC *owner, int arg);
    virtual ~PJSCameraMode();
};

namespace Collision {

/* ProcessBase (class name from RTTI; vtable 0x8053C284): how a camera mode keeps its eye clear
 * of the stage. Its derived classes ProcessDeltaMoveNoLook and ProcessRay2Eye fill in the two
 * pure virtual slots; slot 0x10 adjusts eye for a mode looking at chara (name and meaning
 * inferred from CameraModeFixLookChar). */
class ProcessBase {
  public:
    virtual ~ProcessBase();
    virtual void vf_0x0C() = 0;
    virtual bool Process(SonicteamUSA::System::Camera::CameraMode *mode, Vec3f *eye,
                         CharInfo *chara) = 0;
};

} // namespace Collision
} // namespace PJSCamera

extern "C" {
/* Create a 0x14-byte and a 4-byte collision process, each owned by the returned pointer (which
 * ProcessBase subclasses they are has not been checked). */
boost::shared_ptr<PJSCamera::Collision::ProcessBase> fn_801C68E8(void);
boost::shared_ptr<PJSCamera::Collision::ProcessBase> fn_801C6EB0(void);
}

#endif
