/* PJSCamera::CameraModeGeneral (class name and bases from RTTI; mode 10 of the camera factory): the
 * camera that follows the player around the stage. The mode itself only holds its work object
 * (UnkGeneralCamera_801C4594, class name unknown), which keeps a point above the player to look at
 * and places the eye at a distance and height from it (five parameters, from the mode's arguments
 * or the defaults below). Each frame the work object follows the player's movement, turns around
 * walls the line of sight hits, pulls the eye in when it collides with the stage, reacts to the
 * camera stick and offsets the view on the screen; the mode then copies its placement. File name
 * from the class; other names are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include "Game/CameraModeGeneral.h"
#include "Game/CameraModeFixLookChar.h"
#include "Game/CameraState.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/PJSCameraMode.h"
#include "Game/PJSCameraTargetInfo.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using PJSCamera::CameraModeGeneral;
using PJSCamera::PJSCameraMode;
using PJSCamera::PJSCameraTargetInfo;
using SonicteamUSA::System::Camera::CameraMode;

namespace Player {
class Player;
}

namespace SonicteamUSA {
namespace System {
namespace Collision {
/* CollisionResult (class name from the RTTI of its shared_ptr control block): the first hit of a
 * line test (fn_801C7C70); the object hit is at 0x14. It is deleted through its out-of-line
 * destructor (fn_800265A4). */
class CollisionResult {
  public:
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ void *m_object;
};

/* CollisionResultList (class name from the RTTI of its shared_ptr control block; see
 * MeteorsHolder.cpp): the hits of a sweep or a line, the first one at 0x8. */
class CollisionResultList {
  public:
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ CollisionResult *m_first;
};
} // namespace Collision
} // namespace System
} // namespace SonicteamUSA

using SonicteamUSA::System::Collision::CollisionResult;
using SonicteamUSA::System::Collision::CollisionResultList;

/* The arguments a camera mode is made with (Game/CameraState.h); a mode receives them as its
 * owner. */
inline const UnkCameraModeArgs_8000EBB0 *ModeArgs(const UnkCameraOwner_8000E9BC *owner) {
    return reinterpret_cast<const UnkCameraModeArgs_8000EBB0 *>(owner);
}

/* The switch argument; the flags and fn_8000EC44 are used (layout as in
 * Game/CameraModeFixLookChar.cpp). */
class UnkCameraSwitchArg_8000996C {
  public:
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ u32 m_flags; /* bit 0: move the camera behind the player at once */
};

/* The RenderWare camera (see Game/ScreenHolder.h): its near clip plane. */
struct UnkScreen_8000DCC0 {
    /* 0x00 */ u8 field_0x0[0x80];
    /* 0x80 */ float m_nearPlane;
};

/* The game's screen (see Game/ScreenHolder.h; a reduced view as in Game/ScreenViewport.cpp). */
class UnkScreenHolder_8000DCC0 {
  public:
    int GetViewport(int player);

    /* 0x00 */ UnkScreen_8000DCC0 *m_camera;
};

/* A sphere and a line for the collision queries (see Game/EggMecaCtrlUnit.cpp). */
struct UnkSphere_80021C04 {
    /* 0x00 */ float center[3];
    /* 0x0C */ float radius;
};

struct UnkLine_80020A20 {
    u8 field_0x0[0x18];
};

/* An iterator over the hits (layout from fn_8001EDFC, its out-of-line constructor; see
 * Game/BlockChange.cpp). */
struct UnkHitIter_8001EDFC;
extern "C" void fn_8001EDFC(UnkHitIter_8001EDFC *it, CollisionResult *node);

struct UnkHitIter_8001EDFC {
    UnkHitIter_8001EDFC(CollisionResult *node) {
        fn_8001EDFC(this, node);
    }

    CollisionResult *m_node;
};

/* The camera manager (class name unknown; see Game/CameraManagerUnit.h). */
class UnkCameraManager_80009548;

extern "C" u32 *fn_8000AAEC(u32 *flags, u32 value);

/* A 32-bit flag word set up through its out-of-line constructor (fn_8000AAEC; name inferred). */
struct UnkFlags_8000AAEC {
    UnkFlags_8000AAEC(u32 value) {
        fn_8000AAEC(&m_bits, value);
    }

    u32 m_bits;
};

/* The filter a collision query calls for each candidate (see Game/MeteorsHolder.cpp). */
typedef bool (*UnkCollisionFilter_80021934)(void *candidate);

extern "C" {
/* The camera manager, the target a screen's camera follows and the placement of a screen's camera
 * (behaviour inferred). */
UnkCameraManager_80009548 *fn_80009548(void);
const PJSCameraTargetInfo *fn_801C0B98(UnkCameraManager_80009548 *manager, int index);
const UnkCameraState_8000AA70 *fn_80009370(UnkCameraManager_80009548 *manager, int index);
UnkScreenHolder_8000DCC0 *fn_8000DCC0(void);

/* The player as a Player::Player (boost::dynamic_pointer_cast's instance; see
 * Game/PlayerPostManager.cpp). */
boost::shared_ptr<Player::Player> fn_800DFAD4(const boost::shared_ptr<CharInfo> &chara);
/* The character of a screen (behaviour inferred). */
boost::weak_ptr<CharInfo> fn_800A8C54(UnkCharaList_80074194 *list, int index);

/* A 32-bit flag word: set up, clear all, set, test and clear bit, copy (see Game/ArkCannon.cpp). */
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
void fn_8000B21C(u32 *flags, const u32 *source);
/* A bit of a word array (see Game/PlayerPostManager.cpp). */
bool fn_8006CD78(const u32 *bits, int bit);

/* A camera placement's look vector (target - eye), setting its eye and point looked at, moving
 * both, putting the point looked at at a distance or at eye + look, its up vector, its side and up
 * axes (behaviour inferred from their bodies). */
Vec3f fn_8000A3BC(const UnkCameraState_8000AA70 *state);
void fn_8000A5D4(CameraMode *mode, const Vec3f *eye, const Vec3f *target);
void fn_8000A508(UnkCameraState_8000AA70 *state, const Vec3f *offset);
void fn_8000A430(UnkCameraState_8000AA70 *state, float distance);
void fn_8000A49C(UnkCameraState_8000AA70 *state, const Vec3f *look);
Vec3f fn_8000A770(const UnkCameraState_8000AA70 *state);
Vec3f fn_8000A7B8(const UnkCameraState_8000AA70 *state);
/* Whether two points are about the same (behaviour from its body). */
bool fn_8000AD08(const Vec3f *a, const Vec3f *b);
/* A point's screen position as seen by a camera placement in a viewport (behaviour inferred). */
Vec3f fn_8000F57C(const UnkCameraState_8000AA70 *state, const Vec3f *point, int viewport);
/* The scale of a viewport (see Game/ScreenViewport.cpp). */
void fn_80272DB0(Vec2f *scale, s32 viewport);
/* Cuts the blend into the mode (behaviour inferred). */
void fn_8000EC44(UnkCameraSwitchArg_8000996C *arg, int arg1, int arg2);

/* The tangent and the arc tangent (behaviour inferred). */
float fn_8000F3D4(float angle);
float fn_8000F3F8(float value);
/* value limited to [min, max], the larger and the smaller of two (behaviour from their bodies). */
const float &fn_80070460(const float &value, const float &min, const float &max);
const float &fn_80070494(const float &a, const float &b);
const float &fn_800704AC(const float &a, const float &b);

/* Vector routines (behaviour inferred). */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);       /* a + b */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);     /* vector * scale */
Vec3f fn_800091A4(const Vec3f *vector, float scale);     /* vector * scale */
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b);       /* a x b */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);     /* vector += value */
void fn_8040CE38(Vec3f *vector, float scale);            /* vector *= scale */
float fn_8040CE74(Vec3f *vector);                        /* normalises */
void fn_8040CE60(Vec3f *vector);                         /* zero */
float fn_8040CF90(const Vec3f *a, const Vec3f *b);       /* a . b */
/* Splits vector into its parts along and across normal (see Game/AdjustCosRot.cpp). */
void fn_8040F990(Vec3f *along, Vec3f *across, const Vec3f *vector, const Vec3f *normal);
float fn_8040F8FC(const Vec3f *a, const Vec3f *b); /* the angle between */
/* Rotations about an axis, and a point transformed by a matrix. */
Mtx34f fn_8040FD8C(const Vec3f *axis, float angle);
Mtx34f fn_80049778(const Vec3f *axis, float angle);
void fn_800498A0(Mtx34f *matrix, const Vec3f *axis, float angle);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
/* Two-float vectors: zero, a * scale, a - b and a += b. */
void fn_8040CC5C(Vec2f *vector);
Vec2f fn_80033934(const Vec2f *vector, float scale);
Vec2f fn_8040CBF8(const Vec2f *a, const Vec2f *b);
void fn_8040CD3C(Vec2f *vector, const Vec2f *value);

/* The collision world, a sphere, a line, a sweep of a sphere and the hits of a line (see
 * Game/VehicleColliMove.cpp and Game/EggMecaCtrlUnit.cpp). */
void *fn_80020D40(void);
void fn_80021C04(UnkSphere_80021C04 *sphere, const Vec3f *center, float radius);
void *fn_80020A20(UnkLine_80020A20 *line, const Vec3f *start, const Vec3f *end);
CollisionResultList *fn_80021934(void *world, const UnkSphere_80021C04 *sphere,
                                 const Vec3f *velocity, int arg3, int arg4, int arg5,
                                 UnkCollisionFilter_80021934 filter, void *data, int arg8);
CollisionResultList *fn_80021FE4(void *world, const UnkLine_80020A20 *line, int arg2, int arg3,
                                 UnkCollisionFilter_80021934 filter, int arg5);
/* The hits' first node, an iterator over them (see Game/BlockChange.cpp), and a hit object's
 * position and normal. */
CollisionResult *fn_8001EE04(CollisionResultList *hits);
const Vec3f *fn_8001E9BC(void *object);
const Vec3f *fn_800266D4(void *object);
/* shared_ptr<CollisionResultList>'s pointer, out of line (behaviour from its body). */
CollisionResultList *fn_800CFDE4(const boost::shared_ptr<CollisionResultList> *hits);
/* Releases a hit (CollisionResult's destructor; see checked_delete below). */
void fn_800265A4(CollisionResult *hit, int arg1);

/* The camera's collision tests (the unit after this one): a sphere moved from one point to
 * another, whether the way between two points is blocked, the first hit on a line, and the filters
 * of the queries (behaviour inferred). */
bool fn_801C72B8(const Vec3f *from, const Vec3f *to, Vec3f *result, float radius);
bool fn_801C7920(const Vec3f *from, const Vec3f *to, float radius);
CollisionResult *fn_801C7C70(const Vec3f *from, const Vec3f *to);
bool fn_801C7D04(void *candidate);
bool fn_801C7D20(void *candidate);
}

/* fn_8040CD74 called as returning a - b by value (see Game/ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* Degrees to radians (out of line in the game). */
float DegToRad(float degrees);

extern const float lbl_805FAD38; /* pi */
/* A zero two-float vector (see Game/PJSCameraTargetInfo.cpp). */
extern Vec2f lbl_805F1358;

/* MSL's fabs for floats as the game has it: each level calls the next inline function, so (under
 * one-level deferred inlining) std::fabs and fabsf stay out of line (as in the stand-in <cmath>,
 * whose sqrtf statics this unit does not have). */
extern "C" inline double fabs(double x) {
    return __fabs(x);
}

extern "C" inline float fabsf(float x) {
    return (float)fabs((double)x);
}

namespace std {
inline float fabs(float x) {
    return fabsf(x);
}
} // namespace std

/* The tolerances of fn_8000A950 and fn_8000B044. */
extern const float lbl_805F1714;
extern const float lbl_805F1730;

/* Whether value is about zero (an inline function first emitted before this unit; it calls the
 * inline std::fabs, so it stays out of line). */
extern "C" inline bool fn_8000B044(float value) {
    return std::fabs(value) < lbl_805F1730;
}

/* Whether value is about zero (an inline function first emitted before this unit; it calls the
 * inline std::fabs, so it stays out of line). */
extern "C" inline bool fn_8000A950(float value) {
    return std::fabs(value) < lbl_805F1714;
}

/* Whether two floats are about equal (Game/DefenseShield.cpp and others call it by this name;
 * emitted here, out of line, after its first user). */
extern "C" inline bool fn_801C455C(float a, float b) {
    return fn_8000A950(a - b);
}

/* Retail releases a hit through its out-of-line destructor (as MeteorsHolder.cpp does with
 * fn_80026FB4). */
namespace boost {
template<> inline void checked_delete<CollisionResult>(CollisionResult *x) {
    fn_800265A4(x, 1);
}
} // namespace boost

/* The smaller and the larger of two floats. */
inline float MinFloat(float a, float b) {
    return a < b ? a : b;
}

inline float MaxFloat(float a, float b) {
    return a > b ? a : b;
}

/* Whether v is (nearly) the zero vector. */
inline bool IsNearZero(const Vec3f &v) {
    return v.x * v.x + v.y * v.y + v.z * v.z < 0.0001f;
}

/* A camera mode starts with its placement. */
inline UnkCameraState_8000AA70 *ModeState(CameraMode *mode) {
    return reinterpret_cast<UnkCameraState_8000AA70 *>(mode);
}

inline CameraMode *AsMode(UnkCameraState_8000AA70 *state) {
    return reinterpret_cast<CameraMode *>(state);
}

/* The game sets these in __sinit rather than in .sdata, as an initializer that is not a constant
 * expression does; the inline call reproduces that (as in Game/PlayerBehaviorSuper.cpp). */
inline float DynamicFloat(float value) {
    return value;
}

/* Set up and never read (meaning unknown). */
static const float s_unused = DynamicFloat(2.0f);
/* The defaults of the five parameters: the height of the point looked at above the player, the
 * distance below which the eye may come down, the distance, the height and the lowest height of
 * the eye (names inferred). */
static const float s_lookHeight = DynamicFloat(7.0f);
static const float s_lowDistance = DynamicFloat(35.0f);
static const float s_distance = DynamicFloat(60.0f);
static const float s_height = DynamicFloat(20.0f);
static const float s_lowHeight = DynamicFloat(0.0f);
/* The largest screen offset of the point looked at, and how fast it moves (names inferred). */
static const Vec2f s_maxOffset(0.3f, 0.4f);
static const Vec2f s_offsetSpeed(0.5f, 0.5f);
static const float s_offsetMinSpeed = DynamicFloat(0.0f);
static const float s_offsetMaxSpeed = DynamicFloat(250.0f);
/* The distance limits of the eye, the height limits and the speeds that move it (names inferred). */
static const float s_minDistance = DynamicFloat(2.5f);
static const float s_maxDistance = DynamicFloat(140.0f);
static const float s_stickMinDistance = DynamicFloat(2.5f);
static const float s_stickMaxDistance = DynamicFloat(120.0f);
static const float s_stickMaxHeight = DynamicFloat(50.0f);
static const float s_maxHeight = DynamicFloat(100.0f);
static const float s_runMinSpeed = DynamicFloat(75.0f);
static const float s_runMaxSpeed = DynamicFloat(250.0f);
static const float s_runDistanceSpeed = DynamicFloat(115.0f);
static const float s_runHeightSpeed = DynamicFloat(50.0f);
static const float s_stickDistanceSpeed = DynamicFloat(100.0f);
static const float s_stickHeightSpeed = DynamicFloat(100.0f);
static const float s_stickHeightSpeed2 = DynamicFloat(50.0f);
static const float s_stillSpeed = DynamicFloat(500.0f);
static const float s_radius = DynamicFloat(2.5f);

/* The work object of CameraModeGeneral (class name unknown; names inferred). */
class UnkGeneralCamera_801C4594 {
  public:
    UnkGeneralCamera_801C4594(const UnkCameraOwner_8000E9BC *owner, CameraModeGeneral *mode,
                              int number);

    void OnSwitch(UnkCameraSwitchArg_8000996C *arg);
    void ResetOffset();
    void ReadStick();
    void Finish();
    void UpdateLookAt();
    void CheckUp();
    void SetUp();
    void Follow(float dt);
    void FollowEye(float dt, Vec3f *eye, const Vec3f *from, const Vec3f *look);
    void Stick(float dt);
    void StickEye(float dt, Vec3f *eye);
    bool MoveBehind(float minDistance, const Vec3f *offset, const Vec3f *center);
    bool TurnBehind();
    void TurnAround(float dt);
    void TurnSight(float dt);
    void Recover(float dt);
    void Measure();
    void ResetFree();
    /* An unused out-of-line copy of IsNearZero (see its definition). */
    bool IsTurnAxisZero();
    void Free(float dt);
    void ScreenOffset(float dt);
    void Frame(float dt);
    void Tick();

    void OnFlags(int flags) {
        if (flags & 1) {
            fn_80014E1C(&m_flags.m_bits, 1);
        }
        if (flags & 2) {
            fn_80014E1C(&m_flags.m_bits, 5);
            if (!fn_80014E34(&m_status.m_bits, 2)) {
                fn_80014E1C(&m_flags.m_bits, 6);
            }
        }
    }
    void OnArg(UnkCameraArg_800098E4 *arg) {}

    /* Stops turning around a wall. */
    void StopTurn() {
        m_turnSpeed = 0.0f;
        fn_8040CE60(&m_turnAxis);
        fn_8006127C(&m_status.m_bits, 0);
    }

    /* The vectors copied through the out-of-line copy constructor. */
    Vec3f &LookAt() {
        return *reinterpret_cast<Vec3f *>(&m_lookAt);
    }
    Vec3f &FreeDirection() {
        return *reinterpret_cast<Vec3f *>(&m_freeDirection);
    }
    const Vec3f &Up() const {
        return m_target->field_0x30;
    }

    /* 0x000 */ const float *m_lookHeight;
    /* 0x004 */ const float *m_lowDistance;
    /* 0x008 */ const float *m_distanceParam;
    /* 0x00C */ const float *m_heightParam;
    /* 0x010 */ const float *m_lowHeight;
    /* 0x014 */ int m_number;
    /* 0x018 */ const PJSCameraTargetInfo *m_target;
    /* 0x01C */ PJSCameraTargetInfo m_lastTarget;
    /* 0x070 */ CameraMode *m_mode;
    /* 0x074 */ UnkCameraState_8000AA70 m_state;
    /* 0x0A8 */ UnkCameraState_8000AA70 m_lastState;
    /* 0x0DC */ UnkFlags_8000AAEC m_status;
    /* 0x0E0 */ UnkFlags_8000AAEC m_flags;
    /* 0x0E4 */ UnkFlags_8000AAEC m_lastFlags;
    /* 0x0E8 */ UnkCopiedVector m_lookAt;
    /* 0x0F4 */ float m_distance;
    /* 0x0F8 */ float m_height;
    /* 0x0FC */ float m_minHeight;
    /* 0x100 */ Vec3f m_turnAxis;
    /* 0x10C */ float m_turnSpeed;
    /* 0x110 */ float m_blockedTime;
    /* 0x114 */ float m_backTime;
    /* 0x118 */ Vec2f m_offsetGoal;
    /* 0x120 */ Vec2f m_offset;
    /* 0x128 */ Vec2f m_screenPos;
    /* 0x130 */ float m_stickY;
    /* 0x134 */ float m_stickX;
    /* 0x138 */ Vec2f m_stickRate;
    /* 0x140 */ UnkCopiedVector m_freeDirection;
    /* 0x14C */ float m_freeDistance;
};
SIZE_ASSERT(UnkGeneralCamera_801C4594, 0x150);

/* The horizontal distance and the height of point from center, about up (name inferred). */
static void SplitOffset(float *distance, float *height, const Vec3f *point, const Vec3f *center,
                        const Vec3f *up) {
    Vec3f across;
    Vec3f along;
    fn_8040F990(&along, &across, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(point, center),
                up);
    *distance = fn_8040CF50(&across);
    *height = fn_8040CF90(&along, up);
}

/* SplitOffset that also gives the horizontal part (name inferred). */
static void SplitOffsetAcross(float *distance, float *height, Vec3f *across, const Vec3f *point,
                              const Vec3f *center, const Vec3f *up) {
    Vec3f along;
    fn_8040F990(&along, across, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(point, center),
                up);
    *distance = fn_8040CF50(across);
    *height = fn_8040CF90(&along, up);
}

/* Moves eye away from center by distance and up by height (name inferred). */
static void MoveEye(Vec3f *eye, const Vec3f *center, const Vec3f *up, float distance,
                    float height) {
    Vec3f offset;
    fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(center, eye));
    Vec3f across;
    fn_8040F990(0, &across, &offset, up);
    fn_8040CE74(&across);
    fn_8040CE38(&across, -distance);
    fn_800091D0(&offset, &(const Vec3f &)fn_8040CDD0(&across,
                                                    &(const Vec3f &)fn_8040CDA8(up, height)));
    fn_8040CFF8(eye, &offset);
}

/* Keeps eye within the largest distance and height from center (name inferred). */
static void LimitEye(Vec3f *eye, const Vec3f *center, const Vec3f *up) {
    float distance;
    float height;
    Vec3f across;
    SplitOffsetAcross(&distance, &height, &across, eye, center, up);
    distance = fn_800704AC(distance, s_maxDistance);
    height = fn_800704AC(height, s_maxHeight);
    fn_8040CE74(&across);
    fn_8040CE38(&across, distance);
    fn_8040CFF8(&across, &(const Vec3f &)fn_800091A4(up, height));
    fn_800091D0(eye, &(const Vec3f &)fn_8040CDD0(center, &across));
}

/* The unit normal of the plane of a and b (name inferred). */
static void PlaneNormal(Vec3f *normal, const Vec3f *a, const Vec3f *b) {
    fn_800091D0(normal, &(const Vec3f &)fn_8040CFB8(b, a));
    fn_8040CE74(normal);
}

/* How far change goes towards the limit the stick pushes to (name inferred). */
static float StickRate(float change, float current, float min, float max, float stick) {
    if (!fn_8000B044(change)) {
        if (stick < 0.0f) {
            float range = current - min;
            return !fn_8000B044(range) ? change / range : 0.0f;
        } else {
            float range = max - current;
            return !fn_8000B044(range) ? change / range : 0.0f;
        }
    }
    return 0.0f;
}

/* Whether point is in front of the camera and within limit of the screen's centre (name
 * inferred). */
static bool IsOnScreen(const UnkCameraState_8000AA70 *state, const Vec3f *point,
                       const Vec2f *limit, int viewport) {
    Vec3f screen;
    fn_800091EC(&screen, &(const Vec3f &)fn_8000F57C(state, point, viewport));
    UnkScreenHolder_8000DCC0 *holder = fn_8000DCC0();
    if (-screen.z > holder->m_camera->m_nearPlane && std::fabs(screen.x) < limit->x &&
        std::fabs(screen.y) < limit->y) {
        return true;
    }
    return false;
}

UnkGeneralCamera_801C4594::UnkGeneralCamera_801C4594(const UnkCameraOwner_8000E9BC *owner,
                                                     CameraModeGeneral *mode, int number)
    : m_lookHeight((ModeArgs(owner)->m_flags & 1) ? &s_lookHeight : &ModeArgs(owner)->m_distance),
      m_lowDistance((ModeArgs(owner)->m_flags & 1) ? &s_lowDistance : &ModeArgs(owner)->m_height),
      m_distanceParam((ModeArgs(owner)->m_flags & 1) ? &s_distance : &ModeArgs(owner)->field_0x54),
      m_heightParam((ModeArgs(owner)->m_flags & 1) ? &s_height : &ModeArgs(owner)->field_0x58),
      m_lowHeight((ModeArgs(owner)->m_flags & 1) ? &s_lowHeight : &ModeArgs(owner)->field_0x5C),
      m_number(number), m_target(0), m_mode(mode), m_status(0), m_flags(0), m_lastFlags(0),
      m_lookAt(lbl_805E07A0), m_distance(0.0f), m_height(0.0f), m_minHeight(*m_lowHeight),
      m_turnAxis(0.0f, 1.0f, 0.0f), m_turnSpeed(0.0f), m_blockedTime(0.0f), m_backTime(0.0f),
      m_offsetGoal(s_maxOffset), m_offset(s_maxOffset), m_screenPos(lbl_805F1358), m_stickY(0.0f),
      m_stickX(0.0f), m_stickRate(lbl_805F1358), m_freeDirection(lbl_805E07A0),
      m_freeDistance(0.0f) {
    m_target = fn_801C0B98(fn_80009548(), m_number);
}

void UnkGeneralCamera_801C4594::OnSwitch(UnkCameraSwitchArg_8000996C *arg) {
    m_lastTarget = *m_target;
    SetUp();
    m_blockedTime = m_backTime = 0.0f;
    ResetFree();
    m_offset = m_offsetGoal = s_maxOffset;
    fn_80014E10(&m_status.m_bits);
    fn_80014E10(&m_flags.m_bits);
    UpdateLookAt();
    fn_80014E1C(&m_flags.m_bits, 0);
    if (!(arg->m_flags & 1) || !TurnBehind()) {
        const UnkCameraState_8000AA70 *camera = fn_80009370(fn_80009548(), m_number);
        fn_8000A5D4(AsMode(&m_state), &camera->m_eye, &camera->m_target);
        SplitOffset(&m_distance, &m_height, &m_state.m_eye, &LookAt(), &Up());
        if (m_distance < *m_lowDistance) {
            float rate = (m_distance - s_minDistance) / (*m_lowDistance - s_minDistance);
            m_minHeight = rate * *m_lowHeight;
        } else {
            m_minHeight = *m_lowHeight;
        }
        if (m_distance > s_stickMaxDistance || m_distance < s_stickMinDistance) {
            Vec3f offset;
            fn_800091EC(&offset,
                        &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&m_state.m_eye, &LookAt()));
            MoveBehind(s_minDistance, &offset, &LookAt());
        } else if (m_height > m_minHeight && m_height < s_maxHeight) {
            if (fn_8040CF50(&m_target->field_0xC) < s_stillSpeed &&
                fn_801C455C(m_state.m_fov, camera->m_fov) &&
                fn_8000AD08(&m_state.m_up, &camera->m_up)) {
                if (IsOnScreen(&m_state, &LookAt(), &m_offset,
                               fn_8000DCC0()->GetViewport(m_number))) {
                    fn_8000EC44(arg, 0, 0);
                }
            }
        }
    }
}

void UnkGeneralCamera_801C4594::ResetFree() {
    fn_800091D0(&FreeDirection(), &lbl_805E07A0);
    m_freeDistance = 0.0f;
}

void UnkGeneralCamera_801C4594::ReadStick() {
    m_stickY = m_target->field_0x4C.y;
    m_stickX = m_target->field_0x4C.x;
    if (fn_80014E34(&m_flags.m_bits, 6)) {
        fn_80014E1C(&m_status.m_bits, 2);
        ResetFree();
    } else if (fn_80014E34(&m_status.m_bits, 2) && !fn_80014E34(&m_flags.m_bits, 5)) {
        fn_80014E1C(&m_flags.m_bits, 7);
        fn_8006127C(&m_status.m_bits, 2);
        m_blockedTime = m_backTime = 0.0f;
    }
}

void UnkGeneralCamera_801C4594::Finish() {
    m_lastTarget = *m_target;
    fn_8000B21C(&m_lastFlags.m_bits, &m_flags.m_bits);
    fn_80014E10(&m_flags.m_bits);
}

void UnkGeneralCamera_801C4594::UpdateLookAt() {
    Vec3f up;
    fn_800091EC(&up, &Up());
    fn_8040CE38(&up, *m_lookHeight);
    fn_800091D0(&LookAt(), &(const Vec3f &)fn_8040CDD0(&m_target->m_position, &up));
}

void UnkGeneralCamera_801C4594::CheckUp() {
    if (!fn_8000AD08(&Up(), &m_lastTarget.field_0x30)) {
        SetUp();
        TurnBehind();
    }
}

void UnkGeneralCamera_801C4594::SetUp() {
    fn_8000ACE4(reinterpret_cast<SonicteamUSA::System::Camera::CameraModeUser *>(&m_state), &Up());
}

void UnkGeneralCamera_801C4594::Follow(float dt) {
    CheckUp();
    CameraMode *mode = m_mode;
    Vec3f eye;
    FollowEye(dt, &eye, &mode->m_eye, &(const Vec3f &)fn_8000A3BC(ModeState(mode)));
    if (!fn_80014E34(&m_flags.m_bits, 0)) {
        Vec3f to;
        fn_800091EC(&to, &eye);
        if (fn_801C72B8(&m_mode->m_eye, &to, &eye, s_radius)) {
            fn_80014E1C(&m_flags.m_bits, 2);
        }
    }
    Vec3f move;
    fn_800091EC(&move, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&eye, &m_mode->m_eye));
    Vec3f toCenter;
    fn_800091EC(&toCenter, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&LookAt(), &eye));
    fn_8000A508(&m_state, &move);
    fn_8000A430(&m_state, fn_8040CF50(&toCenter));
    SplitOffset(&m_distance, &m_height, &m_state.m_eye, &LookAt(), &Up());
    m_lastState = m_state;
}

void UnkGeneralCamera_801C4594::FollowEye(float dt, Vec3f *eye, const Vec3f *from,
                                          const Vec3f *look) {
    fn_800091D0(eye, from);
    Vec3f move;
    fn_800091EC(&move, &(const Vec3f &)fn_8040CDA8(&m_target->field_0xC, dt));
    if (!IsNearZero(move)) {
        Vec3f moveAcross;
        fn_8040F990(0, &moveAcross, &move, &Up());
        Vec3f lookAcross;
        fn_8040F990(0, &lookAcross, look, &Up());
        float angle = fn_8040F8FC(&moveAcross, &lookAcross);
        if (angle > 125.0f * (lbl_805FAD38 / 180.0f)) {
            float rate =
                (angle - 125.0f * (lbl_805FAD38 / 180.0f)) / (40.0f * (lbl_805FAD38 / 180.0f));
            rate = fn_80070460(rate, 0.0f, 1.0f);
            fn_8040CFF8(eye, &(const Vec3f &)fn_800091A4(&moveAcross, rate));
        }
    }
    float distance;
    float height;
    Vec3f across;
    SplitOffsetAcross(&distance, &height, &across, eye, &LookAt(), &Up());
    float maxDistance;
    if (m_distance > s_maxDistance) {
        maxDistance = s_maxDistance;
    } else if (m_distance > *m_distanceParam) {
        maxDistance = m_distance;
    } else {
        maxDistance = *m_distanceParam;
    }
    float minDistance;
    if (m_distance < s_minDistance) {
        minDistance = s_minDistance;
    } else if (m_distance < *m_lowDistance) {
        minDistance = m_distance;
    } else {
        minDistance = *m_lowDistance;
    }
    if (distance > maxDistance) {
        distance = maxDistance;
    } else if (distance < minDistance) {
        distance = minDistance;
    }
    float maxHeight;
    if (m_height > s_maxHeight) {
        maxHeight = s_maxHeight;
    } else if (m_height > *m_heightParam) {
        maxHeight = m_height;
    } else {
        maxHeight = *m_heightParam;
    }
    if (distance < *m_lowDistance) {
        float low = (distance - s_minDistance) / (*m_lowDistance - s_minDistance);
        m_minHeight = low * *m_lowHeight;
    } else {
        m_minHeight = *m_lowHeight;
    }
    if (height > maxHeight) {
        height = maxHeight;
    } else if (height < m_minHeight) {
        height = m_minHeight;
    }
    float speed = fn_8040CF50(&m_target->field_0xC);
    if (speed > s_runMinSpeed && fn_8006CD78(m_target->field_0x40.m_words, 2)) {
        float run = (speed - s_runMinSpeed) / (s_runMaxSpeed - s_runMinSpeed);
        run = fn_800704AC(run, 1.0f);
        if (distance > *m_distanceParam) {
            float excess = distance - *m_distanceParam;
            float step = dt * (s_runDistanceSpeed * run);
            excess = excess - step;
            excess = fn_80070494(excess, 0.0f);
            distance = excess + *m_distanceParam;
        } else if (distance < *m_distanceParam) {
            float lack = *m_distanceParam - distance;
            float step = dt * (s_runDistanceSpeed * run);
            lack = lack - step;
            lack = fn_80070494(lack, 0.0f);
            distance = *m_distanceParam - lack;
        }
        float step = dt * (s_runHeightSpeed * run);
        float rise = height - *m_heightParam;
        if (rise > 0.0f) {
            rise = rise - step;
            rise = fn_80070494(rise, 0.0f);
        } else {
            rise = rise + step;
            rise = fn_800704AC(rise, 0.0f);
        }
        height = rise + *m_heightParam;
    }
    fn_8040CE74(&across);
    fn_8040CE38(&across, distance);
    fn_8040CFF8(&across, &(const Vec3f &)fn_800091A4(&Up(), height));
    fn_800091D0(eye, &(const Vec3f &)fn_8040CDD0(&LookAt(), &across));
}

void UnkGeneralCamera_801C4594::Stick(float dt) {
    if (fn_80014E34(&m_flags.m_bits, 1)) {
        Vec3f eye;
        fn_800091EC(&eye, &m_state.m_eye);
        StickEye(dt, &eye);
        Vec3f to;
        fn_800091EC(&to, &eye);
        fn_801C72B8(&m_state.m_eye, &to, &eye, s_radius);
        float distance;
        float height;
        SplitOffset(&distance, &height, &eye, &LookAt(), &Up());
        if (s_stickMinDistance < distance && distance < s_stickMaxDistance &&
            m_minHeight < height && height < s_stickMaxHeight) {
            float distanceChange = distance - m_distance;
            float heightChange = height - m_height;
            m_stickRate.x = StickRate(distanceChange, m_distance, s_stickMinDistance,
                                      s_stickMaxDistance, m_stickY);
            m_stickRate.y = StickRate(heightChange, m_height, m_minHeight, s_stickMaxHeight,
                                      m_stickY);
            fn_8000A5D4(AsMode(&m_state), &eye, &m_state.m_target);
            SplitOffset(&m_distance, &m_height, &m_state.m_eye, &LookAt(), &Up());
            return;
        }
        fn_80014E1C(&m_flags.m_bits, 4);
    }
    m_stickRate = lbl_805F1358;
}

void UnkGeneralCamera_801C4594::StickEye(float dt, Vec3f *eye) {
    if (!fn_8000B044(m_stickY)) {
        Vec3f start;
        fn_800091EC(&start, eye);
        float rise;
        float distance = m_distance + dt * (m_stickY * s_stickDistanceSpeed);
        distance = fn_80070460(distance, s_stickMinDistance, s_stickMaxDistance);
        float change = distance - m_distance;
        m_stickRate.x =
            StickRate(change, m_distance, s_stickMinDistance, s_stickMaxDistance, m_stickY);
        if (!fn_8000B044(m_stickRate.x)) {
            rise = m_stickRate.x < 0.0f ? m_stickRate.x * (m_height - m_minHeight)
                                        : m_stickRate.x * (s_stickMaxHeight - m_height);
            rise = fn_80070460(rise, -s_stickHeightSpeed * dt, s_stickHeightSpeed * dt);
        } else {
            rise = dt * (m_stickY * s_stickHeightSpeed2);
        }
        float height = rise + m_height;
        height = fn_80070460(height, m_minHeight, s_stickMaxHeight);
        rise = height - m_height;
        m_stickRate.y = StickRate(rise, m_height, m_minHeight, s_stickMaxHeight, m_stickY);
        if (s_stickMinDistance < distance && distance < s_stickMaxDistance &&
            m_minHeight < height && height < s_stickMaxHeight) {
            MoveEye(eye, &LookAt(), &Up(), change, rise);
        } else {
            fn_8040CC5C(&m_stickRate);
        }
    }
    if (!fn_8000B044(m_stickX)) {
        Vec3f offset;
        fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(eye, &LookAt()));
        float turn = dt * (150.0f * m_stickX);
        Mtx34f rotation;
        fn_8040C148(&rotation,
                    &(const Mtx34f &)fn_8040FD8C(&Up(), turn * (lbl_805FAD38 / 180.0f)));
        fn_800091D0(&offset, &(const Vec3f &)fn_8040B67C(&rotation, &offset));
        fn_800091D0(eye, &(const Vec3f &)fn_8040CDD0(&offset, &LookAt()));
    }
}

bool UnkGeneralCamera_801C4594::MoveBehind(float minDistance, const Vec3f *offset,
                                           const Vec3f *center) {
    Vec3f direction;
    fn_800091EC(&direction, offset);
    fn_8040F990(0, &direction, &direction, &Up());
    fn_8040CE74(&direction);
    fn_8040CE38(&direction, *m_distanceParam);
    fn_8040CFF8(&direction, &(const Vec3f &)fn_800091A4(&Up(), *m_heightParam));
    Vec3f eye;
    fn_800091EC(&eye, &(const Vec3f &)fn_8040CDD0(center, &direction));
    UnkSphere_80021C04 sphere;
    fn_80021C04(&sphere, center, s_radius);
    Vec3f move;
    fn_800091EC(&move, &direction);
    boost::shared_ptr<CollisionResultList> hits(
        fn_80021934(fn_80020D40(), &sphere, &move, 3, 0xD, 0, fn_801C7D20, 0, 0));
    if (hits.get() != 0) {
        fn_8001EE04(fn_800CFDE4(&hits));
        fn_800091D0(&eye, &(const Vec3f &)fn_8040CDD0(center, &move));
        float distance;
        float height;
        SplitOffset(&distance, &height, &eye, center, &Up());
        if (distance < minDistance) {
            return false;
        }
    }
    fn_8000A5D4(AsMode(&m_state), &eye, &m_state.m_target);
    fn_8000A5D4(AsMode(&m_state), &m_state.m_eye, center);
    fn_80014E1C(&m_flags.m_bits, 0);
    fn_80014E1C(&m_flags.m_bits, 3);
    return true;
}

bool UnkGeneralCamera_801C4594::TurnBehind() {
    Vec3f front;
    fn_800091EC(&front, &m_target->field_0x24);
    Vec3f back;
    fn_800091EC(&back, &(const Vec3f &)fn_8040CDA8(&front, -1.0f));
    if (MoveBehind(s_minDistance, &back, &LookAt())) {
        return true;
    }
    float step = 30.0f * (lbl_805FAD38 / 180.0f);
    for (float angle = step; angle < lbl_805FAD38; angle += step) {
        for (int i = 0; i < 2; i++) {
            Mtx34f rotation;
            fn_8040C148(&rotation, &(const Mtx34f &)fn_8040FD8C(
                                       &Up(), angle * (i == 0 ? 1.0f : -1.0f)));
            Vec3f direction;
            fn_800091EC(&direction, &(const Vec3f &)fn_8040B67C(&rotation, &back));
            if (MoveBehind(s_minDistance, &direction, &LookAt())) {
                return true;
            }
        }
    }
    return MoveBehind(s_minDistance, &front, &LookAt());
}

void UnkGeneralCamera_801C4594::TurnAround(float dt) {
    Vec3f last;
    fn_800091EC(&last, &m_state.m_eye);
    TurnSight(dt);
    if (fn_801C7920(&last, &m_state.m_eye, s_radius)) {
        Vec3f offset;
        fn_800091EC(&offset,
                    &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&m_state.m_eye, &LookAt()));
        MoveBehind(0.0f, &offset, &LookAt());
        StopTurn();
    }
}

void UnkGeneralCamera_801C4594::TurnSight(float dt) {
    Vec3f *eye = &m_state.m_eye;
    const PJSCameraTargetInfo *target = m_target;
    Vec3f sight;
    fn_800091EC(&sight, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(eye, &target->m_position));
    UnkLine_80020A20 line;
    fn_80020A20(&line, eye, &target->m_position);
    boost::shared_ptr<CollisionResultList> hits(
        fn_80021FE4(fn_80020D40(), &line, 1, 0xD, fn_801C7D04, 0));
    if (hits.get() != 0) {
        UnkHitIter_8001EDFC(fn_8001EE04(fn_800CFDE4(&hits)));
        void *object = hits.get()->m_first->m_object;
        Vec3f point;
        fn_800091EC(&point, fn_8001E9BC(object));
        const Vec3f *normal = fn_800266D4(object);
        if (fn_80014E34(&m_flags.m_bits, 1)) {
            Vec3f offset;
            fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&m_state.m_eye, &LookAt()));
            if (!MoveBehind(s_stickMinDistance, &offset, &LookAt())) {
                m_state = m_lastState;
                StopTurn();
            } else {
                StopTurn();
            }
        } else {
            if (fn_80014E34(&m_status.m_bits, 0) && !fn_80014E34(&m_status.m_bits, 1)) {
                Vec3f axis;
                PlaneNormal(&axis, &sight, normal);
                if (fn_8040F8FC(&axis, &m_turnAxis) > 105.0f * (lbl_805FAD38 / 180.0f)) {
                    StopTurn();
                    Vec3f offset;
                    fn_800091EC(&offset,
                                &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&m_state.m_eye, &LookAt()));
                    if (!MoveBehind(s_minDistance, &offset, &LookAt())) {
                        fn_800091D0(&offset, &m_target->field_0x24);
                        fn_8040CE38(&offset, -1.0f);
                        MoveBehind(0.0f, &offset, &LookAt());
                    }
                }
            } else if (!fn_80014E34(&m_status.m_bits, 0)) {
                PlaneNormal(&m_turnAxis, &sight, normal);
                fn_80014E1C(&m_status.m_bits, 0);
            }
            if (fn_80014E34(&m_status.m_bits, 0)) {
                m_turnSpeed = 900.0f * dt + m_turnSpeed;
                m_turnSpeed = m_turnSpeed < 240.0f ? m_turnSpeed : 240.0f;
            }
        }
        fn_80014E1C(&m_status.m_bits, 1);
    } else {
        fn_8006127C(&m_status.m_bits, 1);
        m_turnSpeed = m_turnSpeed - 540.0f * dt;
        if (m_turnSpeed < 0.0f || fn_8000B044(m_turnSpeed)) {
            StopTurn();
        }
    }
    if (fn_80014E34(&m_status.m_bits, 0)) {
        Mtx34f rotation;
        fn_8040C148(&rotation,
                    &(const Mtx34f &)fn_80049778(&m_turnAxis, -DegToRad(m_turnSpeed * dt)));
        Vec3f newEye;
        fn_800091EC(&newEye, &(const Vec3f &)fn_8040CDD0(
                                 &(const Vec3f &)fn_8040B67C(&rotation, &sight),
                                 &target->m_position));
        LimitEye(&newEye, &LookAt(), &Up());
        float height;
        float distance;
        SplitOffset(&distance, &height, eye, &LookAt(), &Up());
        if (height < *m_heightParam) {
            float newHeight;
            float newDistance;
            SplitOffset(&newDistance, &newHeight, &newEye, &LookAt(), &Up());
            fn_8040CFF8(&newEye, &(const Vec3f &)fn_800091A4(&Up(), -(newHeight - height)));
        }
        fn_8000A5D4(AsMode(&m_state), &newEye, &m_state.m_target);
    }
}

void UnkGeneralCamera_801C4594::Recover(float dt) {
    if (fn_80014E34(&m_flags.m_bits, 2)) {
        m_blockedTime += dt;
        float distance;
        float height;
        SplitOffset(&distance, &height, &m_state.m_eye, &LookAt(), &Up());
        if (distance > s_maxDistance || height > s_maxHeight) {
            Vec3f offset;
            fn_800091EC(&offset,
                        &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&m_state.m_eye, &LookAt()));
            if (MoveBehind(s_minDistance, &offset, &LookAt())) {
                m_blockedTime = 0.0f;
            }
        }
    } else {
        m_blockedTime = 0.0f;
    }
    if (fn_80014E34(&m_status.m_bits, 1)) {
        m_backTime += dt;
        if (m_backTime > 1.0f) {
            Vec3f offset;
            fn_800091EC(&offset,
                        &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&m_state.m_eye, &LookAt()));
            if (MoveBehind(s_minDistance, &offset, &LookAt())) {
                m_backTime = 0.0f;
            } else {
                fn_800091D0(&offset, &m_target->field_0x24);
                fn_8040CE38(&offset, -1.0f);
                if (MoveBehind(s_minDistance, &offset, &LookAt())) {
                    m_backTime = 0.0f;
                }
            }
        }
    } else {
        m_backTime = 0.0f;
    }
}

void UnkGeneralCamera_801C4594::Measure() {
    SplitOffset(&m_distance, &m_height, &m_state.m_eye, &LookAt(), &Up());
}

void UnkGeneralCamera_801C4594::Free(float dt) {
    m_state = *ModeState(m_mode);
    if (fn_80014E34(&m_flags.m_bits, 6)) {
        Vec3f eye;
        fn_800091EC(&eye, &m_state.m_eye);
        fn_8040CFF8(&eye, &(const Vec3f &)fn_800091A4(&m_target->field_0xC, dt));
        fn_800091D0(&FreeDirection(),
                    &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&eye, &LookAt()));
        m_freeDistance = fn_8040CF50(&FreeDirection());
        fn_8040CE74(&FreeDirection());
    }
    Vec3f eye;
    fn_800091EC(&eye, &(const Vec3f &)fn_8040CDD0(
                          &LookAt(), &(const Vec3f &)fn_8040CDA8(&FreeDirection(), m_freeDistance)));
    Vec3f move;
    fn_800091EC(&move, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&eye, &m_state.m_eye));
    fn_8000A508(&m_state, &move);
    Vec3f direction;
    fn_800091EC(&direction,
                &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&m_state.m_eye, &LookAt()));
    fn_8040CE74(&direction);
    {
        boost::shared_ptr<CollisionResult> hit(fn_801C7C70(
            &LookAt(), &(const Vec3f &)fn_8040CDD0(
                           &m_state.m_eye, &(const Vec3f &)fn_8040CDA8(&direction, s_radius))));
        if (hit.get() != 0) {
            Vec3f point;
            fn_800091EC(&point, fn_8001E9BC(hit->m_object));
            fn_8040CFF8(&point, &(const Vec3f &)fn_800091A4(&direction, -s_radius));
            Vec3f back;
            fn_800091EC(&back, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&point,
                                                                               &m_state.m_eye));
            fn_8000A508(&m_state, &back);
            fn_80014E1C(&m_flags.m_bits, 2);
        }
    }
    fn_800091D0(&FreeDirection(),
                &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&m_state.m_eye, &LookAt()));
    fn_8040CE74(&FreeDirection());
    m_lastState = m_state;
}

/* a - b for two-float vectors (Game/CameraScreenSet.cpp calls it by this name). */
extern "C" Vec2f fn_801C2800(const Vec2f *a, const Vec2f *b) {
    return fn_8040CBF8(a, b);
}

void UnkGeneralCamera_801C4594::ScreenOffset(float dt) {
    float speed = fn_8040CF50(&m_target->field_0xC);
    if (m_distance < s_minDistance) {
        fn_8040CC5C(&m_offsetGoal);
        fn_8040CC5C(&m_offset);
    } else {
        if (m_distance < s_stickMinDistance) {
            m_offsetGoal = fn_80033934(&s_maxOffset, (m_distance - s_minDistance) /
                                                         (s_stickMinDistance - s_minDistance));
        } else if (speed < s_offsetMinSpeed || !fn_8006CD78(m_target->field_0x40.m_words, 2)) {
            m_offsetGoal = s_maxOffset;
        } else if (speed < s_offsetMaxSpeed) {
            m_offsetGoal = fn_80033934(&s_maxOffset, 1.0f - (speed - s_offsetMinSpeed) /
                                                                (s_offsetMaxSpeed - s_offsetMinSpeed));
        } else {
            fn_8040CC5C(&m_offsetGoal);
        }
        Vec2f change(fn_801C2800(&m_offsetGoal, &m_offset));
        Vec2f limit(fn_80033934(&s_offsetSpeed, dt));
        change.x = fn_80070460(change.x, -limit.x, limit.x);
        change.y = fn_80070460(change.y, -limit.y, limit.y);
        fn_8040CD3C(&m_offset, &change);
    }
    if (fn_80014E34(&m_flags.m_bits, 1) && m_stickY < 0.0f) {
        Vec3f screen;
        fn_800091EC(&screen, &(const Vec3f &)fn_8000F57C(&m_state, &LookAt(),
                                                          fn_8000DCC0()->GetViewport(m_number)));
        float scale = 0.0f;
        if (m_stickRate.x < scale && m_stickRate.y < scale) {
            scale = fn_80070494(m_stickRate.x, m_stickRate.y);
        } else if (fn_8000B044(m_stickRate.x) || m_stickRate.x > 0.0f) {
            scale = m_stickRate.y;
        } else if (fn_8000B044(m_stickRate.y) || m_stickRate.y > 0.0f) {
            scale = m_stickRate.x;
        }
        scale = 1.0f + scale;
        m_offset.x = fn_800704AC(scale * std::fabs(screen.x), m_offset.x);
        m_offset.y = fn_800704AC(scale * std::fabs(screen.y), m_offset.y);
    }
}

/* Not called anywhere: the linker strips it from the game. The original pools IsNearZero's float
 * literal right after those of Frame, which is what compiling an out-of-line function that inlines
 * it at this point of the file does (inferred from the order of the literals; see
 * docs/research-notes.md). */
bool UnkGeneralCamera_801C4594::IsTurnAxisZero() {
    return IsNearZero(m_turnAxis);
}

/* A camera placement's view window (Game/CameraScreenSet.cpp calls it by this name). */
extern "C" const Vec2f *fn_801C2508(const UnkCameraState_8000AA70 *state) {
    return reinterpret_cast<const Vec2f *>(state->field_0x1C);
}

void UnkGeneralCamera_801C4594::Frame(float dt) {
    int viewport = fn_8000DCC0()->GetViewport(m_number);
    Vec3f screen;
    fn_800091EC(&screen, &(const Vec3f &)fn_8000F57C(&m_state, &LookAt(), viewport));
    Vec2f position(screen.x, screen.y);
    if (fn_8000B044(m_offset.x) && fn_8000B044(m_offset.y)) {
        fn_8000A5D4(AsMode(&m_state), &m_state.m_eye, &LookAt());
        fn_8040CC5C(&position);
    } else {
        UnkScreenHolder_8000DCC0 *holder = fn_8000DCC0();
        if (-screen.z < holder->m_camera->m_nearPlane) {
            fn_8000A5D4(AsMode(&m_state), &m_state.m_eye, &LookAt());
            fn_8040CC5C(&position);
        } else {
            const Vec2f *view = fn_801C2508(&m_state);
            float windowX = view->x;
            float windowY = view->y;
            Vec2f scale;
            fn_80272DB0(&scale, viewport);
            windowX *= scale.x;
            windowY *= scale.y;
            Vec3f look;
            fn_800091EC(&look, &(const Vec3f &)fn_8000A3BC(&m_state));
            float length = fn_8040CF50(&look);
            Vec2f delta(0.0f, 0.0f);
            if (std::fabs(screen.x) > m_offset.x) {
                position.x = MinFloat(position.x, m_offset.x);
                position.x = MaxFloat(position.x, -m_offset.x);
                delta.x = screen.x - position.x;
            }
            if (std::fabs(screen.y) > m_offset.y) {
                position.y = MinFloat(position.y, m_offset.y);
                position.y = MaxFloat(position.y, -m_offset.y);
                delta.y = screen.y - position.y;
            }
            if (!fn_8000B044(delta.x)) {
                float side = length * fn_8000F3D4(0.5f * windowX);
                delta.x *= side;
                delta.x = -fn_8000F3F8(delta.x / length);
                Mtx34f rotation;
                fn_800498A0(&rotation, &(const Vec3f &)fn_8000A7B8(&m_state), delta.x);
                fn_800091D0(&look, &(const Vec3f &)fn_8040B67C(&rotation, &look));
                fn_8000A49C(&m_state, &look);
            }
            if (!fn_8000B044(delta.y)) {
                float side = length * fn_8000F3D4(0.5f * windowY);
                delta.y *= side;
                delta.y = fn_8000F3F8(delta.y / length);
                Mtx34f rotation;
                fn_800498A0(&rotation, &(const Vec3f &)fn_8000A770(&m_state), delta.y);
                fn_800091D0(&look, &(const Vec3f &)fn_8040B67C(&rotation, &look));
                fn_8000A49C(&m_state, &look);
            }

        }
    }
    m_screenPos = position;
}

void UnkGeneralCamera_801C4594::Tick() {}

namespace PJSCamera {

CameraModeGeneral::CameraModeGeneral(UnkCameraOwner_8000E9BC *owner, int number)
    : PJSCameraMode(owner, number), m_impl(new UnkGeneralCamera_801C4594(owner, this, number)) {}

CameraModeGeneral::~CameraModeGeneral() {}

CameraMode *CameraModeGeneral::Create(UnkCameraOwner_8000E9BC *owner, int number) {
    return new CameraModeGeneral(owner, number);
}

void CameraModeGeneral::Update(float dt, UnkCameraUpdateArg_800099B8 *arg) {
    boost::shared_ptr<CharInfo> chara(fn_800A8C54(fn_80074194(), field_0xA0).lock());
    boost::shared_ptr<Player::Player> player(fn_800DFAD4(chara));
    if (player.get() == 0) {
        return;
    }
    m_impl->ReadStick();
    m_impl->UpdateLookAt();
    if (fn_80014E34(&m_impl->m_status.m_bits, 2)) {
        m_impl->Free(dt);
    } else {
        m_impl->Follow(dt);
        m_impl->Stick(dt);
        m_impl->TurnAround(dt);
        m_impl->Recover(dt);
    }
    m_impl->Measure();
    m_impl->ScreenOffset(dt);
    m_impl->Frame(dt);
    m_impl->Tick();
    m_impl->Finish();
    *ModeState(this) = m_impl->m_state;
}

void CameraModeGeneral::OnSwitch(UnkCameraSwitchArg_8000996C *arg) {
    m_impl->OnSwitch(arg);
    *ModeState(this) = m_impl->m_state;
}

void CameraModeGeneral::vf_0x14(UnkCameraArg_800098E4 *arg) {
    m_impl->OnArg(arg);
}

void CameraModeGeneral::vf_0x18(int flags) {
    m_impl->OnFlags(flags);
}

int CameraModeGeneral::GetModeId() {
    return 10;
}

} // namespace PJSCamera
