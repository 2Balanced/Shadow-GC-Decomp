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

/* The placement a camera mode is made with (struct name unknown; 0x24 bytes): the eye, the point
 * looked at, the field of view and the view window. Its constructors (0x8000B4AC, which takes the
 * defaults, and 0x8000B464) and destructor (0x8000B428) are out of line. Copies go member by
 * member, so the parts are single floats (names inferred). */
struct UnkCameraView_8000B4AC {
    UnkCameraView_8000B4AC();
    ~UnkCameraView_8000B4AC();

    /* 0x00 */ float m_eyeX;
    /* 0x04 */ float m_eyeY;
    /* 0x08 */ float m_eyeZ;
    /* 0x0C */ float m_targetX;
    /* 0x10 */ float m_targetY;
    /* 0x14 */ float m_targetZ;
    /* 0x18 */ float m_fov;
    /* 0x1C */ float m_windowX; /* (CameraModeLookFront copies the window to the mode) */
    /* 0x20 */ float m_windowY;
};

/* Two words of UnkCameraModeArgs_8000EBB0 that its constructors clear together (struct name
 * unknown; copied as one). */
struct UnkCameraModeWords_8000EBB0 {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ u32 field_0x4;
};

/* The parameters from 0x4C of UnkCameraModeArgs_8000EBB0 as one block (the copy constructor
 * copies them so). */
struct UnkCameraModeParams_8000EBB0 {
    float m_values[8];
};

/* The parameters a camera mode is made with (struct name unknown; 0x6C bytes, from its constructor
 * 0x8000EBB0, which clears the parameters at 0x4C; destructor 0x8000EAE8; another constructor at
 * 0x8000EB34 takes the placement and the flags; copy constructor 0x801C7DC0, defined in
 * Game/SetCameraDataManager.cpp). A mode copies the parameters from 0x34 on;
 * CameraModeFixLookChar takes its distance and height from 0x4C and 0x50 (names inferred).
 * CameraModeGeneral reads the five parameters from 0x4C on unless bit 0 of the flags at 0x24 asks
 * for its defaults; CameraModeLookFront takes six from 0x4C on. */
struct UnkCameraModeArgs_8000EBB0 {
    UnkCameraModeArgs_8000EBB0();
    /* With a placement (eye, point looked at, field of view, window) and the flags (0x8000EB34;
     * Game/StageCameraManager.cpp makes one). */
    UnkCameraModeArgs_8000EBB0(const Vec3f *eye, const Vec3f *target, float fov, const Vec2f *window,
                               u32 flags);
    UnkCameraModeArgs_8000EBB0(const UnkCameraModeArgs_8000EBB0 &args);
    ~UnkCameraModeArgs_8000EBB0();

    /* 0x00 */ UnkCameraView_8000B4AC m_view;
    /* 0x24 */ u32 m_flags;
    /* 0x28 */ u32 field_0x28;
    /* 0x2C */ UnkCameraModeWords_8000EBB0 field_0x2C;
    /* 0x34 */ float field_0x34;
    /* 0x38 */ float field_0x38;
    /* 0x3C */ float field_0x3C;
    /* 0x40 */ float field_0x40;
    /* 0x44 */ float field_0x44;
    /* 0x48 */ float field_0x48;
    /* 0x4C */ float m_distance;
    /* 0x50 */ float m_height;
    /* 0x54 */ float field_0x54;
    /* 0x58 */ float field_0x58;
    /* 0x5C */ float field_0x5C;
    /* 0x60 */ float field_0x60;
    /* 0x64 */ float field_0x64;
    /* 0x68 */ float field_0x68;
};
SIZE_ASSERT(UnkCameraModeArgs_8000EBB0, 0x6C);

#endif
