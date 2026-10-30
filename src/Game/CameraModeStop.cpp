/* SonicteamUSA::System::Camera::CameraModeStop (class name and base from RTTI; mode -0x400): a
 * camera that stays where the first screen's camera was when it was switched to (behaviour
 * inferred). The unit also emits the first copies of the camera manager's accessor and of the
 * lookup of a screen camera's placement, which many camera units call, with the shared_ptr
 * instances they need. File name from the class; other names are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#define MSL_LIST_ACCESSORS
#include "Game/CameraManagerUnit.h"
#include "Game/CameraModeUser.h"
#include "Game/CameraState.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::Camera::CameraManagerUnit;
using SonicteamUSA::System::Camera::CameraMode;

/* The camera manager (class name unknown; see Game/CameraManagerUnit.cpp). */
class UnkCameraManager_80009548;

extern "C" {
/* The camera manager's constructor and destructor (Game/CameraManager.cpp), called on its
 * storage by their linker names. */
void *__ct__25UnkCameraManager_80009548Fv(void *object);
void __dt__25UnkCameraManager_80009548Fv(void *object, int flags);
void *__register_global_object(void *object, void *destructor, void *record);
/* The camera unit of a screen (see Game/StageCameraManager.cpp). */
boost::shared_ptr<CameraManagerUnit> fn_8000D0F0(UnkCameraManager_80009548 *manager, int index);
/* Copies a camera placement's eye, point looked at and field of view (Game/CameraState.cpp). */
void fn_8000A544(UnkCameraState_8000AA70 *state, const UnkCameraState_8000AA70 *from);
}

namespace SonicteamUSA {
namespace System {
namespace Camera {

class CameraModeStop : public CameraMode {
  public:
    CameraModeStop(UnkCameraOwner_8000E9BC *owner, int screen) : CameraMode(owner, screen) {}

    static CameraMode *Create(UnkCameraOwner_8000E9BC *owner, int screen);

    virtual int GetModeId();
    virtual void Update(float dt, UnkCameraUpdateArg_800099B8 *arg);
    virtual void OnSwitch(UnkCameraSwitchArg_8000996C *arg);
    virtual void vf_0x14(UnkCameraArg_800098E4 *arg);
    virtual ~CameraModeStop();
};
SIZE_ASSERT(CameraModeStop, 0xAC);

} // namespace Camera
} // namespace System
} // namespace SonicteamUSA

using SonicteamUSA::System::Camera::CameraModeStop;

/* The pointer to the camera manager and the manager's function-local static. The unit's .sbss
 * (0x805EDF94..0x805EDFA0) is left outside its split: it starts on a 4-byte boundary, and the
 * compiler aligns a unit's .sbss to 8 bytes, so the pointer and the guard are declared extern. */
extern UnkCameraManager_80009548 *lbl_805EDF94;
extern s8 lbl_805EDF98;
static u32 s_cameraManagerRecord[3];
static u32 s_cameraManagerObject[5];

/* A camera mode starts with a placement. */
inline UnkCameraState_8000AA70 *ModeState(CameraMode *mode) {
    return reinterpret_cast<UnkCameraState_8000AA70 *>(mode);
}

CameraModeStop::~CameraModeStop() {}

void CameraModeStop::Update(float dt, UnkCameraUpdateArg_800099B8 *arg) {}

extern "C" {

/* The camera manager (a function-local static object). */
UnkCameraManager_80009548 *fn_80009548(void) {
    if (lbl_805EDF94 == 0) {
        if (!lbl_805EDF98) {
            __register_global_object(__ct__25UnkCameraManager_80009548Fv(s_cameraManagerObject),
                                     __dt__25UnkCameraManager_80009548Fv, s_cameraManagerRecord);
            lbl_805EDF98 = 1;
        }
        lbl_805EDF94 = (UnkCameraManager_80009548 *)s_cameraManagerObject;
    }
    return lbl_805EDF94;
}

/* The placement of a screen's camera. */
inline const UnkCameraState_8000AA70 *fn_80009370(UnkCameraManager_80009548 *manager, int index) {
    CameraManagerUnit *unit = fn_8000D0F0(manager, index).get();
    return &unit->m_state;
}

} // extern "C"

void CameraModeStop::OnSwitch(UnkCameraSwitchArg_8000996C *arg) {
    fn_8000A544(ModeState(this), fn_80009370(fn_80009548(), 0));
}

void CameraModeStop::vf_0x14(UnkCameraArg_800098E4 *arg) {}

CameraMode *CameraModeStop::Create(UnkCameraOwner_8000E9BC *owner, int screen) {
    return new CameraModeStop(owner, screen);
}

int CameraModeStop::GetModeId() {
    return -0x400;
}
