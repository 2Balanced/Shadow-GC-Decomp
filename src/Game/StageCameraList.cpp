/* The camera manager of the player code (class name unknown; Game/StageCameraList.h): the stage
 * cameras of the screens and the stage's camera set data, and the entry points through which other
 * code puts camera modes and external requests on the stage camera of a mode's screen. No class of
 * the unit has RTTI; the file name is inferred from its contents, as are the other names.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#define MSL_ALGORITHM_FIND
#define MSL_LIST_ACCESSORS
#include <algorithm>

#include "Game/StageCameraList.h"
#include "Game/CameraManagerUnit.h"
#include "Game/CameraModeUser.h"
#include "Game/ExternalCameraRequest.h"
#include "Game/PtrVector.h"
#include "Game/SetCameraData.h"
#include "Game/StageCameraManager.h"
#include "Game/StageTable.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using PJSCamera::ExternalCameraRequest;
using PJSCamera::StageCameraManager;
using SonicteamUSA::System::Camera::CameraManagerUnit;
using SonicteamUSA::System::Camera::CameraMode;

/* boost/shared_ptr.hpp and boost/weak_ptr.hpp only declare reset, which the game has out of line;
 * this unit emits the copies for the external requests (boost 1.32's definitions). */
namespace boost {
template <> inline void weak_ptr<ExternalCameraRequest>::reset() {
    this_type().swap(*this);
}
template <> inline void shared_ptr<ExternalCameraRequest>::reset() {
    this_type().swap(*this);
}
} // namespace boost

/* The camera manager (class name unknown; see Game/CameraManagerUnit.h). */
class UnkCameraManager_80009548;

extern "C" {
/* The camera manager, the camera unit of a screen and turning on a screen's camera unit (see
 * Game/StageCameraManager.cpp; behaviour inferred). */
UnkCameraManager_80009548 *fn_80009548(void);
boost::shared_ptr<CameraManagerUnit> fn_8000D0F0(UnkCameraManager_80009548 *manager, int index);
void fn_8000D270(UnkCameraManager_80009548 *manager, int index);
}

/* Turns off the camera unit of a screen (out of line here; behaviour inferred). */
inline void DisableCameraUnit(UnkCameraManager_80009548 *manager, int index) {
    fn_8000D0F0(manager, index)->field_0x0 = 0;
}

UnkCameraManager_80099954::UnkCameraManager_80099954() : m_numScreens(-1), m_setCameraData(0) {
    m_cameras.clear();
}

UnkCameraManager_80099954::~UnkCameraManager_80099954() {}

void UnkCameraManager_80099954::ResetStageCamera(int screen) {
    StageCameraManager *camera = GetStageCamera(screen);
    if (camera != 0) {
        camera->Reset();
    }
}

extern "C" {

boost::weak_ptr<ExternalCameraRequest> fn_801CED24(const boost::shared_ptr<CameraMode> &mode,
                                                   int flags, int blendKindIn, int blendIn,
                                                   int blendKindOut, int blendOut,
                                                   int requestFlags) {
    boost::shared_ptr<ExternalCameraRequest> request(new ExternalCameraRequest(
        mode, flags, blendKindIn, (const UnkCameraBlend_8000B2C0 *)blendIn, blendKindOut,
        (const UnkCameraBlend_8000B2C0 *)blendOut, requestFlags));
    StageCameraManager *camera = fn_80099954()->GetStageCamera(mode->GetScreen());
    if (camera != 0) {
        camera->AddRequest(request);
        return request;
    }
    request.reset();
    return request;
}

void fn_801CEAD4(boost::weak_ptr<ExternalCameraRequest> *request) {
    boost::shared_ptr<ExternalCameraRequest> locked = request->lock();
    if (locked.get() != 0) {
        StageCameraManager *camera =
            fn_80099954()->GetStageCamera(locked->GetMode()->GetScreen());
        if (camera != 0) {
            camera->RemoveRequest(locked);
        }
    }
    request->reset();
}

void fn_801CEA54(UnkCameraManager_80099954 *manager, boost::shared_ptr<CameraMode> mode,
                 int blendKind, const UnkCameraBlend_8000B2C0 &blend, int flags, int arg) {
    StageCameraManager *camera = manager->GetStageCamera(mode->GetScreen());
    if (camera != 0) {
        camera->SetExternalMode(mode, blendKind, &blend, flags, arg);
    }
}

void fn_801CE9F4(UnkCameraManager_80099954 *manager, const boost::shared_ptr<CameraMode> &mode) {
    StageCameraManager *camera = manager->GetStageCamera(mode->GetScreen());
    if (camera != 0) {
        camera->ReleaseExternalMode(mode);
    }
}

void fn_801CE8CC(UnkCameraManager_80099954 *manager, s32 stageId) {
    Task *parent = fn_800D51EC()->m_preTask;
    if (fn_80176C3C(fn_800B74B4(), -1)->m_flags & 4) {
        manager->m_numScreens = 2;
        fn_8000D270(fn_80009548(), 1);
    } else {
        manager->m_numScreens = 1;
    }
    if (manager->m_setCameraData == 0) {
        manager->m_setCameraData = new SetCameraDataManager(stageId);
        manager->m_setCameraData->RequestFileLoad();
    }
    for (int i = 0; i < manager->m_numScreens; i++) {
        manager->m_cameras.push_back(new StageCameraManager(parent, i));
    }
}

void fn_801CE760(UnkCameraManager_80099954 *manager, StageCameraManager *camera) {
    if (!manager->m_cameras.empty()) {
        std::vector<StageCameraManager *>::iterator it =
            std::find(manager->m_cameras.begin(), manager->m_cameras.end(), camera);
        if (it != manager->m_cameras.end()) {
            manager->m_cameras.erase(it);
            if (manager->m_cameras.empty()) {
                if (manager->m_setCameraData != 0) {
                    delete manager->m_setCameraData;
                }
                manager->m_setCameraData = 0;
                if (manager->m_numScreens == 2) {
                    DisableCameraUnit(fn_80009548(), 1);
                }
                manager->m_numScreens = -1;
            }
        }
    }
}

} // extern "C"

/* Not called in the game (the linker drops it); the game emits the vector's begin and end with it,
 * before fn_801CE760, which uses them. */
void UnkCameraManager_80099954::ResetStageCameras() {
    for (std::vector<StageCameraManager *>::iterator it = m_cameras.begin(); it != m_cameras.end();
         ++it) {
        (*it)->Reset();
    }
}

StageCameraManager *UnkCameraManager_80099954::GetStageCamera(int screen) {
    if (m_cameras.size() > screen) {
        return m_cameras.at(screen);
    }
    return 0;
}
