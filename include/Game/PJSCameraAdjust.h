#ifndef GAME_PJSCAMERAADJUST_H
#define GAME_PJSCAMERAADJUST_H

#include "Game/CameraAdjust.h"
#include "Game/CameraModeUser.h"
#include "Game/CameraState.h"
#include "Game/MathTypes.h"
#include <dolphin/types.h>
#include "size_assert.h"

namespace PJSCamera {

/* AdjustCosRot (class name from RTTI; adjust type 4 of PJSCamera::PJSCameraFactory;
 * Game/AdjustCosRot.cpp): eases in and out along a cosine and swings the eye about the player.
 * Member names are inferred. */
class AdjustCosRot : public SonicteamUSA::System::Camera::CameraAdjust {
  public:
    AdjustCosRot(const UnkCameraBlend_8000B2C0 &blend);
    virtual s32 GetType();
    virtual void Start(const UnkCameraState_8000AA70 *from,
                       SonicteamUSA::System::Camera::CameraMode *mode);
    virtual void UpdateEye(SonicteamUSA::System::Camera::CameraMode *mode, float dt);
    virtual float GetEyeRate(float time, float duration);
    virtual ~AdjustCosRot();

    /* The camera factory's maker of this adjust. */
    static SonicteamUSA::System::Camera::CameraAdjust *Create(const UnkCameraBlend_8000B2C0 &blend);

    /* 0xB0 */ Vec3f m_startOffset;    /* the eye from the player */
    /* 0xBC */ Vec3f m_startDirection; /* looked in, normalised */
    /* 0xC8 */ float m_startDistance;  /* to the point looked at */
};
SIZE_ASSERT(AdjustCosRot, 0xCC);

/* AdjustLinerLocal (class name from RTTI; vtable 0x8053C35C; adjust type 2 of
 * PJSCamera::PJSCameraFactory; Game/AdjustLinerLocal.cpp): blends the eye's offset from the player
 * and the direction looked in linearly, as AdjustCosRot does with a swing. Member names are
 * inferred. */
class AdjustLinerLocal : public SonicteamUSA::System::Camera::CameraAdjust {
  public:
    AdjustLinerLocal(const UnkCameraBlend_8000B2C0 &blend);
    virtual s32 GetType();
    virtual void Start(const UnkCameraState_8000AA70 *from,
                       SonicteamUSA::System::Camera::CameraMode *mode);
    virtual void UpdateEye(SonicteamUSA::System::Camera::CameraMode *mode, float dt);
    virtual ~AdjustLinerLocal();

    /* The camera factory's maker of this adjust. */
    static SonicteamUSA::System::Camera::CameraAdjust *Create(const UnkCameraBlend_8000B2C0 &blend);

    /* 0xB0 */ Vec3f m_startOffset;    /* the eye from the player */
    /* 0xBC */ Vec3f m_startDirection; /* looked in, normalised */
    /* 0xC8 */ float m_startDistance;  /* to the point looked at */
};
SIZE_ASSERT(AdjustLinerLocal, 0xCC);

/* AdjustLinerWorld (class name from RTTI; vtable 0x8053C38C; adjust type 1 of
 * PJSCamera::PJSCameraFactory; Game/AdjustLinerWorld.cpp): moves the eye and the point looked at in
 * straight lines. */
class AdjustLinerWorld : public SonicteamUSA::System::Camera::CameraAdjust {
  public:
    AdjustLinerWorld(const UnkCameraBlend_8000B2C0 &blend) : CameraAdjust(blend) {}
    virtual s32 GetType();
    virtual void Start(const UnkCameraState_8000AA70 *from,
                       SonicteamUSA::System::Camera::CameraMode *mode);
    virtual void UpdateEye(SonicteamUSA::System::Camera::CameraMode *mode, float dt);
    virtual ~AdjustLinerWorld();

    /* The camera factory's maker of this adjust. */
    static SonicteamUSA::System::Camera::CameraAdjust *Create(const UnkCameraBlend_8000B2C0 &blend);
};
SIZE_ASSERT(AdjustLinerWorld, 0xB0);

/* AdjustOnlyAov (class name from RTTI; vtable 0x8053C3BC; adjust type 3 of
 * PJSCamera::PJSCameraFactory; Game/AdjustOnlyAov.cpp): takes the new mode's placement at once. */
class AdjustOnlyAov : public SonicteamUSA::System::Camera::CameraAdjust {
  public:
    AdjustOnlyAov(const UnkCameraBlend_8000B2C0 &blend) : CameraAdjust(blend) {}
    virtual s32 GetType();
    virtual void Start(const UnkCameraState_8000AA70 *from,
                       SonicteamUSA::System::Camera::CameraMode *mode);
    virtual void UpdateEye(SonicteamUSA::System::Camera::CameraMode *mode, float dt);
    virtual ~AdjustOnlyAov();

    /* The camera factory's maker of this adjust. */
    static SonicteamUSA::System::Camera::CameraAdjust *Create(const UnkCameraBlend_8000B2C0 &blend);
};
SIZE_ASSERT(AdjustOnlyAov, 0xB0);

} // namespace PJSCamera

#endif
