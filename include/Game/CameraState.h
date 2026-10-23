#ifndef GAME_CAMERASTATE_H
#define GAME_CAMERASTATE_H

#include "Game/MathTypes.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* A camera's placement (struct name unknown; 0x34 bytes, from CameraManagerUnit's layout): the eye
 * and the point looked at come first; a CameraMode starts with one. Its constructor, destructor
 * and assignment are out of line (0x8000AA70, 0x8000A9AC, 0x8000B074). */
struct UnkCameraState_8000AA70 {
    UnkCameraState_8000AA70();
    ~UnkCameraState_8000AA70();
    UnkCameraState_8000AA70 &operator=(const UnkCameraState_8000AA70 &state);

    /* 0x00 */ Vec3f m_eye;
    /* 0x0C */ Vec3f m_target;
    /* 0x18 */ float m_fov; /* (meaning inferred; AdjustCosRot blends it) */
    /* 0x1C */ u8 field_0x1C[0x24 - 0x1C];
    /* 0x24 */ Vec3f m_up; /* (meaning inferred; AdjustCosRot turns about it) */
    /* 0x30 */ u8 field_0x30[0x34 - 0x30];
};
SIZE_ASSERT(UnkCameraState_8000AA70, 0x34);

/* The previous frame's placement of a camera unit, with the eye's velocity and the position
 * followed (struct name unknown; its constructor and destructor are out of line at 0x80359164 and
 * 0x80359118, in Game/AdjustCosRot.cpp). The placement is a member: the destructor destroys it as
 * one. */
struct UnkCameraState_80359164 {
    UnkCameraState_80359164();
    ~UnkCameraState_80359164();

    /* 0x00 */ UnkCameraState_8000AA70 m_state;
    /* 0x34 */ Vec3f m_velocity;
    /* 0x40 */ Vec3f m_targetPosition;
};

/* The parameters a camera mode is made with (struct name unknown; 0x6C bytes, from its constructor
 * 0x8000EBB0, which clears the parameters at 0x4C; destructor 0x8000EAE8). A mode copies the
 * parameters from 0x34 on; CameraModeFixLookChar takes its distance and height from 0x4C and 0x50
 * (names inferred). */
struct UnkCameraModeArgs_8000EBB0 {
    UnkCameraModeArgs_8000EBB0();
    ~UnkCameraModeArgs_8000EBB0();

    /* 0x00 */ u8 field_0x0[0x4C];
    /* 0x4C */ float m_distance;
    /* 0x50 */ float m_height;
    /* 0x54 */ u8 field_0x54[0x6C - 0x54];
};

#endif
