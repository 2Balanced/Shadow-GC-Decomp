#ifndef GAME_SETCAMERADATA_H
#define GAME_SETCAMERADATA_H

#include "Game/CameraModeUser.h"
#include "Game/CameraState.h"
#include "Game/CharaColli.h"
#include "Game/MathTypes.h"
#include "Game/PtrVector.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" bool fn_80014E34(const u32 *flags, int bit);

/* The stage's camera set data: the camera areas of stg%04d_cam.dat, each with a shape, the camera
 * mode to switch to inside it and its blend (Game/SetCameraDataManager.cpp; the class name of the
 * manager comes from its debug message, the other names are inferred). */

/* The flags of an area (struct name unknown; a 32-bit flag word, see Game/BKTankController.h).
 * Its copy constructor is out of line (0x801C7E80). */
struct SetCameraFlags {
    SetCameraFlags(const SetCameraFlags &flags);

    bool Test(int bit) const {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits;
};

/* The shape of an area (class name unknown): a sphere, a box, a capsule, a cylinder or a flat box
 * (kinds 0 to 4; meanings inferred from the colliders made for them). */
class SetCameraShape {
  public:
    SetCameraShape();
    ~SetCameraShape();

    Vec3f GetPosition() const;
    Vec3f GetRotation() const {
        return Vec3f(m_rotX, m_rotY, m_rotZ);
    }
    /* The collider of the shape. */
    boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> CreateCollision() const;

    /* 0x00 */ s32 m_kind;
    /* 0x04 */ float m_x;
    /* 0x08 */ float m_y;
    /* 0x0C */ float m_z;
    /* 0x10 */ float m_rotX;
    /* 0x14 */ float m_rotY;
    /* 0x18 */ float m_rotZ;
    /* 0x1C */ float m_size[3]; /* the radius first */
};
SIZE_ASSERT(SetCameraShape, 0x28);

/* One area of the camera set data (class name unknown; 0xDC bytes, also its size in the file). */
class SetCameraData {
  public:
    ~SetCameraData();

    /* 0x00 */ s32 field_0x0;
    /* 0x04 */ s32 m_modeKind; /* the camera mode made for the area */
    /* 0x08 */ s32 m_priority;
    /* 0x0C */ s32 m_blendKind;
    /* 0x10 */ SetCameraFlags m_flags;
    /* 0x14 */ s32 m_order;
    /* 0x18 */ s32 m_event;
    /* 0x1C */ SetCameraShape m_shape;
    /* 0x44 */ UnkCameraModeArgs_8000EBB0 m_args;
    /* 0xB0 */ UnkCameraBlend_8000B2C0 m_blend;
};
SIZE_ASSERT(SetCameraData, 0xDC);

/* The head of stg%04d_cam.dat; the areas follow it. */
struct SetCameraDataFile {
    /* 0x00 */ u32 m_version;
    /* 0x04 */ u32 m_release;
    /* 0x08 */ u8 field_0x8[0x14 - 0x8];
    /* 0x14 */ u32 m_count;
};

class CharInfo;

namespace PJSCamera {
class PJSCameraTargetInfo;
}

/* A camera area in play (class name unknown; 0xC bytes, Game/SetCameraArea.cpp): its data and the
 * collider of its shape. */
class SetCameraArea {
  public:
    SetCameraArea(SetCameraData *data);
    ~SetCameraArea();
    /* Whether chara is in the area (switch and up vector conditions first; a flat box area tests
     * the line from the player's part to the player, and only when line is set). */
    bool Check(boost::shared_ptr<CharInfo> chara, const PJSCamera::PJSCameraTargetInfo *target,
               bool line);

    /* 0x0 */ SetCameraData *m_data;
    /* 0x4 */ boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> m_collision;
};
SIZE_ASSERT(SetCameraArea, 0xC);

/* SetCameraDataManager (class name from its debug message): loads a stage's camera set data. */
class SetCameraDataManager {
  public:
    SetCameraDataManager(int stage);
    ~SetCameraDataManager();
    void RequestFileLoad();
    SetCameraData *GetData(u32 index);
    bool Load(const SetCameraDataFile *file);
    void Clear();

    /* The number of areas and their range (out of line where used: Game/StageCameraManager.cpp;
     * names inferred). */
    int GetNum() {
        return m_data.size();
    }
    SetCameraData **Begin() {
        return m_data.begin();
    }
    SetCameraData **End() {
        return m_data.end();
    }

    /* 0x00 */ s32 m_stage;
    /* 0x04 */ s32 m_state; /* 2 once the file is loaded */
    /* 0x08 */ char m_fileName[0x20];
    /* 0x28 */ std::vector<SetCameraData *> m_data;
};
SIZE_ASSERT(SetCameraDataManager, 0x34);

#endif
