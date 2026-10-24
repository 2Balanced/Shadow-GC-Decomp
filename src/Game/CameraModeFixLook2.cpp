/* PJSCamera::CameraModeFixLook2 (class name and bases from RTTI; mode 13 of the camera factory;
 * Game/CameraModeFixLook2.h): a camera that finds how far the position the screen's camera follows
 * is along a rail from the eye of the arguments it was made with, looks at the point that far
 * between the arguments' point looked at and the rail's second point, and puts the eye beyond the
 * followed position at the distance and height blended the same way; a collision process keeps
 * the eye clear of the stage. File name from the class; other names are inferred. Definitions are
 * in reverse address order (deferred inlining emits functions last-defined first). */
#define MSL_LIST_ACCESSORS
#include "Game/CameraModeFixLook2.h"
#include "Game/CameraManagerUnit.h"
#include "Game/CameraModeUser.h"
#include "Game/CameraState.h"
#include "Game/CameraTargetInfo.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/PJSCameraMode.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

using PJSCamera::CameraModeFixLook2;
using PJSCamera::PJSCameraMode;
using PJSCamera::Collision::ProcessBase;
using SonicteamUSA::System::Camera::CameraMode;
using SonicteamUSA::System::Camera::CameraModeUser;
using SonicteamUSA::System::Camera::CameraTargetInfo;

/* The camera manager (class name unknown; see Game/CameraManagerUnit.h). */
class UnkCameraManager_80009548;

/* The switch argument; only the flags are read (see Game/CameraModeFixLookChar.cpp). */
class UnkCameraSwitchArg_8000996C {
  public:
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ u32 m_flags; /* bit 2: no collision process; bit 3: the 0x14-byte process */
};

/* A line between two points (layout unknown; see Game/StageCameraPostProcess.cpp). */
struct UnkLine_80020A20 {
    u8 field_0x0[0x18];
};

/* Vectors the rail starts from (see Game/StageCameraModeHolder.cpp). */
extern Vec3f lbl_8056EE28;
extern Vec3f lbl_8056EE34;

extern "C" {
UnkCameraManager_80009548 *fn_80009548(void);
/* The target a screen's camera follows (Game/CameraModeFixEye.cpp). */
CameraTargetInfo *fn_801C0B98(UnkCameraManager_80009548 *manager, int index);
/* The placement in a mode's arguments, its eye and its point looked at (behaviour inferred). */
UnkCameraState_8000AA70 fn_8000B3B4(const UnkCameraModeArgs_8000EBB0 *args);
Vec3f fn_8000B398(const UnkCameraModeArgs_8000EBB0 *args);
Vec3f fn_8000B37C(const UnkCameraModeArgs_8000EBB0 *args);
/* A line between two points, and the point of a line nearest to point with its parameter (see
 * Game/SetSeLoop.cpp). */
void *fn_80020A20(UnkLine_80020A20 *line, const Vec3f *start, const Vec3f *end);
Vec3f fn_8040FC78(float *t, const UnkLine_80020A20 *line, const Vec3f *point);
/* v * scale, a + b, v normalised in place (returns the former length), v *= scale, and the vector
 * routine Game/CameraModeFixLookChar.cpp calls the same way (behaviour inferred). */
Vec3f fn_8040CDA8(const Vec3f *v, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
float fn_8040CE74(Vec3f *v);
void fn_8040CE38(Vec3f *v, float scale);
Vec3f fn_80093204(const Vec3f *a, const Vec3f *b);

/* Three floats as a vector, and the vectors of a mode's arguments at 0x34 and 0x40 (the game links
 * the first two from Game/CameraModeFixEye2.cpp; the third is out of line here). */
inline Vec3f fn_801C0F74(const float *xyz) {
    return Vec3f(xyz[0], xyz[1], xyz[2]);
}

inline Vec3f fn_801C0F44(const UnkCameraModeArgs_8000EBB0 *args) {
    return fn_801C0F74(&args->field_0x34);
}

inline Vec3f fn_801C17D4(const UnkCameraModeArgs_8000EBB0 *args) {
    return fn_801C0F74(&args->field_0x40);
}
}

/* fn_8040CD74 called as returning a - b by value (see Game/ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* The game sets these in __sinit rather than in initial values, as an initializer that is not a
 * constant expression does; the inline call reproduces that (as in Game/CameraModeLookFront.cpp). */
inline float DynamicFloat(float value) {
    return value;
}

/* The rail's default distance and height (names inferred). */
static const float s_distance = DynamicFloat(60.0f);
static const float s_height = DynamicFloat(10.0f);

/* The rail (struct name unknown; 0x2C bytes): the distance and height of the eye at both ends,
 * the rail's second point and the second point looked at, and where the followed position was on
 * it last (names inferred). */
struct UnkLookRail_801C1804 {
    UnkLookRail_801C1804()
        : m_startDistance(s_distance), m_startHeight(s_height), m_endDistance(s_distance),
          m_endHeight(s_height) {
        CopyVector(m_end, lbl_8056EE28);
        CopyVector(m_lookEnd, lbl_8056EE34);
        m_t = 0.0f;
    }
    ~UnkLookRail_801C1804() {}
    static void CopyVector(Vec3f &vector, const Vec3f &value) {
        fn_800091EC(&vector, &value);
    }

    /* 0x00 */ float m_startDistance;
    /* 0x04 */ float m_startHeight;
    /* 0x08 */ float m_endDistance;
    /* 0x0C */ float m_endHeight;
    /* 0x10 */ Vec3f m_end;
    /* 0x1C */ Vec3f m_lookEnd;
    /* 0x28 */ float m_t;
};

/* A camera mode starts with a placement and keeps its arguments at 0x34. */
inline UnkCameraState_8000AA70 *ModeState(CameraMode *mode) {
    return reinterpret_cast<UnkCameraState_8000AA70 *>(mode);
}

inline const UnkCameraModeArgs_8000EBB0 *ModeArgs(CameraMode *mode) {
    return reinterpret_cast<const UnkCameraModeArgs_8000EBB0 *>(mode->field_0x34);
}

namespace PJSCamera {

CameraModeFixLook2::CameraModeFixLook2(UnkCameraOwner_8000E9BC *owner, int number)
    : PJSCameraMode(owner, number), m_rail(new UnkLookRail_801C1804) {
    *ModeState(this) = fn_8000B3B4(ModeArgs(this));
    m_rail->m_startDistance = field_0x80;
    m_rail->m_startHeight = field_0x84;
    m_rail->m_endDistance = *reinterpret_cast<float *>(field_0x88);
    m_rail->m_endHeight = *reinterpret_cast<float *>(field_0x88 + 4);
    fn_800091D0(&m_rail->m_end, &(const Vec3f &)fn_801C0F44(ModeArgs(this)));
    fn_800091D0(&m_rail->m_lookEnd, &(const Vec3f &)fn_801C17D4(ModeArgs(this)));
}

CameraModeFixLook2::~CameraModeFixLook2() {}

void CameraModeFixLook2::Update(float dt, UnkCameraUpdateArg_800099B8 *arg) {
    CameraTargetInfo *target = fn_801C0B98(fn_80009548(), field_0xA0);
    UnkLine_80020A20 line;
    Vec3f look;
    Vec3f position;
    Vec3f direction;
    const Vec3f &start = fn_8000B398((const UnkCameraModeArgs_8000EBB0 *)field_0x34);
    const Vec3f &lookStart = fn_8000B37C((const UnkCameraModeArgs_8000EBB0 *)field_0x34);
    float t;
    const Vec3f *lookEnd = &m_rail->m_lookEnd;
    fn_80020A20(&line, &start, &m_rail->m_end);
    fn_8040FC78(&t, &line, &target->m_position);
    if (t < 0.0f) {
        t = 0.0f;
    } else if (t > 1.0f) {
        t = 1.0f;
    }
    float blend = t;
    fn_800091EC(&look,
                &(const Vec3f &)fn_8040CDD0(
                    &(const Vec3f &)fn_8040CDA8(
                        &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(lookEnd, &lookStart),
                        blend),
                    &lookStart));
    fn_8000A5D4(this, &m_eye, &look);
    float height = t * (m_rail->m_endHeight - m_rail->m_startHeight) + m_rail->m_startHeight;
    float distance =
        t * (m_rail->m_endDistance - m_rail->m_startDistance) + m_rail->m_startDistance;
    fn_800091EC(&position,
                &(const Vec3f &)fn_8040CDD0(
                    &target->m_position,
                    &(const Vec3f &)fn_8040CDA8(&target->field_0x30, height)));
    fn_800091EC(&direction,
                &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&position, &look));
    fn_8040CE74(&direction);
    fn_8040CE38(&direction, distance);
    fn_8000A5D4(this, &(const Vec3f &)fn_80093204(&position, &direction), &m_target);
    fn_8000ACE4(reinterpret_cast<CameraModeUser *>(this), &target->field_0x30);
    if (m_process.get() != 0) {
        m_process->Process(this, &position, reinterpret_cast<CharInfo *>(target));
    }
    m_rail->m_t = t;
}

void CameraModeFixLook2::OnSwitch(UnkCameraSwitchArg_8000996C *arg) {
    u32 flags = arg->m_flags;
    if (flags & 4) {
        m_process.reset();
    } else if (flags & 8) {
        m_process = fn_801C68E8();
    } else {
        m_process = fn_801C6EB0();
    }
}

void CameraModeFixLook2::vf_0x14(UnkCameraArg_800098E4 *arg) {}

CameraMode *CameraModeFixLook2::Create(UnkCameraOwner_8000E9BC *owner, int number) {
    return new CameraModeFixLook2(owner, number);
}

int CameraModeFixLook2::GetModeId() {
    return 13;
}

} // namespace PJSCamera
