#ifndef GAME_CAMERAMANAGERUNIT_H
#define GAME_CAMERAMANAGERUNIT_H

/* Units that include this header define MSL_LIST_ACCESSORS before it (see <list>). */
#include <list>

#include "Game/CameraModeUser.h"
#include "Game/CameraState.h"
#include "Game/CameraTargetInfo.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The camera manager (class name unknown; fn_80009548 returns it). Its first member is the camera
 * factory (see SonicteamUSA::System::Camera::CameraFactory below). */
class UnkCameraManager_80009548;

/* A 3x4 matrix member copied from another by fn_8040C148. */
struct UnkCameraMtx_8040C148 {
    UnkCameraMtx_8040C148(const Mtx34f &m) {
        fn_8040C148(&value, &m);
    }

    Mtx34f value;
};

namespace SonicteamUSA {
namespace System {
namespace Camera {

/* CameraFactory (class name from RTTI; PJSCamera::PJSCameraFactory derives from it): the camera
 * manager's first member. Slot 0x08 makes a camera unit's target (a CameraTargetInfo,
 * Game/CameraTargetInfo.h; name inferred). */
class CameraFactory {
  public:
    virtual CameraTargetInfo *CreateTarget();
};

/* CameraShake (class name from the RTTI of its shared_ptr control block): a camera unit's shake
 * (contents unknown). */
class CameraShake;

/* CameraRequest (class name from the RTTI of its shared_ptr control block,
 * sp_counted_base_impl<CameraRequest *, void (*)(CameraRequest *)>): a request that a camera mode
 * drive one camera unit. Its constructor (0x8000ED58) takes the mode by value; its destructor is
 * out of line (0x8000ECDC). Field names are inferred. */
class CameraRequest {
  public:
    CameraRequest(boost::shared_ptr<CameraMode> mode, int arg1, int arg2, int arg3, int arg4);
    ~CameraRequest();

    /* 0x00 */ boost::shared_ptr<CameraMode> m_mode;
    /* 0x08 */ s32 m_priority;
    /* 0x0C */ u32 field_0xC;
    /* 0x10 */ void *m_blend;
    /* 0x14 */ u32 m_flags; /* bit 1: stays active behind another request (meaning inferred) */
};
SIZE_ASSERT(CameraRequest, 0x18);

/* CameraManagerUnit (class name from the RTTI of its shared_ptr control block): the camera of one
 * screen (fn_8000D0F0 returns the one of an index). It keeps the camera modes made for it and the
 * requests on it, the first request's mode placing the camera (meaning of most members inferred;
 * Game/CameraManagerUnit.cpp defines its members). */
class CameraManagerUnit {
  public:
    CameraManagerUnit(int index);
    ~CameraManagerUnit();

    bool HasRequest() {
        return !m_requests.empty();
    }
    void RemoveRequest(CameraRequest *request);
    void AddRequest(boost::shared_ptr<CameraRequest> request);
    void PruneModes();
    void AddMode(boost::shared_ptr<CameraMode> mode);
    void Update(float dt);
    boost::shared_ptr<CameraRequest> GetCurrentRequest();
    boost::shared_ptr<CameraMode> GetCurrentMode();

    /* 0x000 */ u8 field_0x0;
    /* 0x004 */ int m_index;
    /* 0x008 */ UnkCameraState_8000AA70 m_state;
    /* 0x03C */ UnkCameraMtx_8040C148 m_viewMtx;
    /* 0x06C */ UnkCameraMtx_8040C148 m_worldMtx;
    /* 0x09C */ UnkCameraState_8000AA70 m_state2;
    /* 0x0D0 */ u32 field_0xD0;
    /* 0x0D4 */ std::list<boost::weak_ptr<CameraMode> > m_modes;
    /* 0x0E0 */ std::list<boost::weak_ptr<CameraRequest> > m_requests;
    /* 0x0EC */ boost::shared_ptr<CameraShake> m_shake;
    /* 0x0F4 */ CameraTargetInfo *m_target;
    /* 0x0F8 */ UnkCameraState_80359164 m_lastState;
    /* 0x12C */ Vec3f m_velocity;
    /* 0x138 */ Vec3f m_targetPosition;
};
SIZE_ASSERT(CameraManagerUnit, 0x144);

} // namespace Camera
} // namespace System
} // namespace SonicteamUSA

#endif
