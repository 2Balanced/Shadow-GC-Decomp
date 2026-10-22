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
    /* 0x18 */ u8 field_0x18[0x34 - 0x18];
};
SIZE_ASSERT(UnkCameraState_8000AA70, 0x34);

/* The previous frame's placement of a camera unit (struct name unknown; its constructor and
 * destructor are out of line at 0x80359164 and 0x80359118). */
struct UnkCameraState_80359164 : public UnkCameraState_8000AA70 {
    UnkCameraState_80359164();
    ~UnkCameraState_80359164();
};

#endif
