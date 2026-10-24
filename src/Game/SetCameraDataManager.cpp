/* SetCameraDataManager (class name from its debug message) and the camera areas it loads (class
 * names unknown; Game/SetCameraData.h): the manager reads stg%04d/stg%04d_cam.dat and keeps a copy
 * of each area in it; an area's shape makes its own collider. File name from the class; other
 * names are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/SetCameraData.h"
#include "Game/CameraModeUser.h"
#include "Game/CameraState.h"
#include "Game/CharaColli.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::CharaColliBase;

extern "C" {
void *memset(void *dest, int c, unsigned long count);
int sprintf(char *buffer, const char *format, ...);
}

/* The file system fn_800371D8 returns (class name unknown; see Game/ActStageController.cpp). */
class UnkFileSystem_800371D8 {
  public:
    /* Whether a file exists (name inferred). */
    bool Exists(const char *path) {
        return exists(path);
    }

    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ bool (*exists)(const char *path);
};

/* The player whose colliders the camera areas belong to (see Game/AttackObject.cpp). */
extern boost::shared_ptr<CharInfo> lbl_805EE698;
/* Vector constants the collider setup reads (see Game/ContainerBehavior.cpp). */
extern Vec3f lbl_805E07B8;

extern "C" {
/* The debug print (see Game/BkLarva.cpp). */
void fn_803A953C(const char *format, ...);
/* The file system, a whole file read into memory, its bytes swapped and its release (see
 * Game/AudioDataBank.cpp and Game/FogTable.cpp). */
UnkFileSystem_800371D8 *fn_800371D8(void);
void *fn_8004509C(UnkFileSystem_800371D8 *fileSystem, const char *path, u32 *size);
void fn_80043D0C(void *value, int size);
void fn_80049B34(void *data);

/* The collider manager and its colliders: a sphere, a box, a capsule and a cylinder (see
 * Game/BlackBullCtrlUnit.cpp and Game/ContainerBehavior.cpp; the integer arguments' meanings are
 * unknown). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7EA8(void *manager, const Vec3f *position, u32 flags,
                                              int arg3, int arg4,
                                              const boost::weak_ptr<CharInfo> &owner,
                                              const Vec3f *offset, float radius);
boost::shared_ptr<CharaColliBase>
fn_800C7D04(void *manager, const Vec3f &position, const Vec3f &size, const Vec3f &rotation,
            int arg4, int arg5, int arg6, int arg7, const boost::weak_ptr<CharInfo> &owner,
            const Vec3f &offset, const Vec3f &arg10, int arg11);
boost::shared_ptr<CharaColliBase>
fn_800C799C(void *manager, const Vec3f &position, const Vec3f &rotation, int arg4, int arg5,
            int arg6, int arg7, const boost::weak_ptr<CharInfo> &owner, const Vec3f &offset,
            const Vec3f &arg10, int arg11, float height, float radius);
boost::shared_ptr<CharaColliBase>
fn_800C7B4C(void *manager, const Vec3f &position, const Vec3f &rotation, int arg4, int arg5,
            int arg6, int arg7, const boost::weak_ptr<CharInfo> &owner, const Vec3f &offset,
            const Vec3f &arg10, int arg11, float height, float radius);
}

SetCameraShape::~SetCameraShape() {}

boost::shared_ptr<CharaColliBase> SetCameraShape::CreateCollision() const {
    s32 kind = m_kind;
    boost::shared_ptr<CharaColliBase> collision;
    switch (kind) {
    case 0:
        collision = fn_800C7EA8(fn_80021B08(), &(const Vec3f &)GetPosition(), 0, 0xE, 0,
                                lbl_805EE698, &lbl_805E07A0, m_size[0]);
        break;
    case 1:
        collision = fn_800C7D04(fn_80021B08(), GetPosition(), Vec3f(m_size[0], m_size[1], m_size[2]),
                                GetRotation(), 1, 0, 0xE, 0, lbl_805EE698, lbl_805E07A0,
                                lbl_805E07B8, 0);
        break;
    case 3:
        collision = fn_800C7B4C(fn_80021B08(), GetPosition(), GetRotation(), 1, 0, 0xE, 0,
                                lbl_805EE698, lbl_805E07A0, lbl_805E07B8, 0, m_size[1], m_size[0]);
        break;
    case 2:
        collision = fn_800C799C(fn_80021B08(), GetPosition(), GetRotation(), 1, 0, 0xE, 0,
                                lbl_805EE698, lbl_805E07A0, lbl_805E07B8, 0, m_size[1], m_size[0]);
        break;
    case 4:
        collision = fn_800C7D04(fn_80021B08(), GetPosition(), Vec3f(m_size[0], m_size[1], 0.0f),
                                GetRotation(), 1, 0, 0xE, 0, lbl_805EE698, lbl_805E07A0,
                                lbl_805E07B8, 0);
        break;
    }
    return collision;
}

SetCameraDataManager::SetCameraDataManager(int stage) : m_stage(stage), m_state(0) {
    m_data.clear();
    memset(m_fileName, 0, sizeof(m_fileName));
    sprintf(m_fileName, "stg%04d/stg%04d_cam.dat", m_stage, m_stage);
}

SetCameraDataManager::~SetCameraDataManager() {
    Clear();
}

void SetCameraDataManager::RequestFileLoad() {
    void *data = 0;
    if (fn_800371D8()->Exists(m_fileName)) {
        u32 size;
        data = fn_8004509C(fn_800371D8(), m_fileName, &size);
        fn_80043D0C(data, size);
    } else {
        fn_803A953C("SetCameraDataManager::RequestFileLoad() %s File not exist, Camera Set Data is empty.\n",
                    m_fileName);
    }
    Load(static_cast<const SetCameraDataFile *>(data));
    if (data != 0) {
        fn_80049B34(data);
    }
    m_state = 2;
}

/* The area at index, or 0 (never called; the linker drops it, keeping the out-of-line size
 * accessors it reaches first in the game's order). */
SetCameraData *SetCameraDataManager::GetData(u32 index) {
    if (index < m_data.size()) {
        return m_data.begin()[index];
    }
    return 0;
}

bool SetCameraDataManager::Load(const SetCameraDataFile *file) {
    m_data.clear();
    if (file != 0) {
        if (file->m_version != 3) {
            fn_803A953C("WORNING: SetDataVersion is invalid. DataVersion:%d-Release:%d, CurrentVersion:%d-Release:%d\n",
                        file->m_version, file->m_release, 3, 20041107);
        }
        u32 count = file->m_count;
        m_data.reserve(count);
        const SetCameraData *data = reinterpret_cast<const SetCameraData *>(file + 1);
        for (u32 i = 0; i < count; i++) {
            m_data.push_back(new SetCameraData(*data));
            data++;
        }
    }
    return true;
}

void SetCameraDataManager::Clear() {
    for (std::vector<SetCameraData *>::iterator it = m_data.begin(); it != m_data.end(); ++it) {
        delete *it;
    }
    m_data.clear();
    m_state = 0;
}

SetCameraFlags::SetCameraFlags(const SetCameraFlags &flags) : m_bits(flags.m_bits) {}

/* UnkCameraModeArgs_8000EBB0's copy (Game/CameraState.h): member by member, the parameters from
 * 0x4C as one block. */
UnkCameraModeArgs_8000EBB0::UnkCameraModeArgs_8000EBB0(const UnkCameraModeArgs_8000EBB0 &args)
    : m_view(args.m_view), m_flags(args.m_flags), field_0x28(args.field_0x28),
      field_0x2C(args.field_0x2C), field_0x34(args.field_0x34), field_0x38(args.field_0x38),
      field_0x3C(args.field_0x3C), field_0x40(args.field_0x40), field_0x44(args.field_0x44),
      field_0x48(args.field_0x48) {
    *reinterpret_cast<UnkCameraModeParams_8000EBB0 *>(&m_distance) =
        *reinterpret_cast<const UnkCameraModeParams_8000EBB0 *>(&args.m_distance);
}

SetCameraData::~SetCameraData() {}

Vec3f SetCameraShape::GetPosition() const {
    return Vec3f(m_x, m_y, m_z);
}
