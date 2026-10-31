/* Player helpers (file name inferred; the unit has no class of its own): the player's movement
 * with drag, its turning towards a direction, the direction of the pad's input in the camera's
 * frame, and the shadow it casts on the ground (UnkShadow_8009368C). They take the player's shared
 * pointer. All names are inferred. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). The unit starts after Game/PlayerBehaviorWallJump.cpp and
 * ends with its __sinit (0x80094074, the .ctors entry at 0x804A9CDC).
 *
 * NonMatching: 23 of 24 functions match. fn_8009325C, fn_800936F4, fn_80093C60 and fn_80093FC0 read
 * the player pointer (lwz rN, 0(r3)) above the store of the link register or the copy of r3, as in
 * fn_80334480 of Game/VehiclePathMove.cpp; GC/2.7 does that with a const object parameter, so the
 * four are declared that way here (callers in other units may declare them either way). fn_8009339C
 * (89%) keeps the player pointer in r31 and reaches the tilt quaternion through a copy of it in
 * r4; GC/2.7 allocates the registers differently. */
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/Quaternion.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Player::PlayerBase;

namespace SonicteamUSA {
namespace System {
namespace Camera {
class CameraManagerUnit;
}
} // namespace System
} // namespace SonicteamUSA

/* The camera manager (see Game/CameraManagerUnit.h). */
class UnkCameraManager_80009548;

/* MSL's float overloads, which the game calls out of line. */
namespace std {
float cos(float x);
float fabs(float x);
} // namespace std

/* A shadow cast on the ground (as in Game/BossCtrlUnit.h): the ground's position and normal, what
 * the line cast down found there, its distance, and whether it is shown. */
struct UnkShadow_8009368C {
    UnkShadow_8009368C();
    ~UnkShadow_8009368C();

    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ Vec3f m_normal;
    /* 0x18 */ u32 field_0x18;
    /* 0x1C */ u32 field_0x1C;
    /* 0x20 */ float m_distance;
    /* 0x24 */ bool m_visible;
};
SIZE_ASSERT(UnkShadow_8009368C, 0x28);

extern "C" {
Vec3f *fn_800091D0(Vec3f *out, const Vec3f *from);
Vec3f *fn_800091EC(Vec3f *out, const Vec3f *from);
Vec3f fn_800091A4(const Vec3f *vector, float scale); /* vector * scale */
UnkCameraManager_80009548 *fn_80009548(void);
Vec3f fn_8000A3BC(const void *state);
bool fn_8000A950(float value); /* whether the value is zero */
boost::shared_ptr<SonicteamUSA::System::Camera::CameraManagerUnit>
fn_8000D0F0(UnkCameraManager_80009548 *manager, int index);
bool fn_80014E34(u32 *flags, int bit);
float fn_80015D68(float value); /* arc cosine */
float fn_80016260(float x, float z); /* the angle of (x, z) */
Vec3f fn_80016284(const Vec3f *v); /* -v */
void *fn_80020A20(void *line, const Vec3f *start, const Vec3f *end);
void fn_8003B9E8(const Vec3f *direction, float *pitch, float *yaw);
bool fn_8004962C(const Vec3f *vector); /* whether the vector is zero */
bool fn_8006CD78(u32 *bits, int bit);
const float &fn_800704AC(const float &a, const float &b); /* the smaller */
Vec3f fn_800796DC(PlayerBase *player); /* the player's front */
void fn_8007A34C(Quatf *rotation, const Quatf *value);
float fn_800979E8(boost::shared_ptr<PlayerBase> *player, void *position, void *normal, void *arg3,
                  void *arg4, void *arg5, void *segment, int arg7, u32 attributes, u32 mask,
                  int arg10);
Mtx34f fn_8040B228(const Quatf *rotation);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Mtx34f fn_8040BEF8(const Mtx34f *matrix); /* the inverse */
Quatf fn_8040C3EC(const Quatf *from, const Quatf *to, float t);
Quatf fn_8040C874(const Vec3f *axis, float angle);
Vec3f fn_8040CDA8(const Vec3f *vector, float scale); /* vector * scale */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);   /* a + b */
float fn_8040CE74(Vec3f *vector);                    /* normalizes */
void fn_8040CE60(Vec3f *vector);                     /* vector = 0 */
Vec3f fn_8040CEE8(const Vec3f *vector);              /* normalized */
float fn_8040CF90(const Vec3f *a, const Vec3f *b);   /* dot product */
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b);   /* cross product */
Vec3f fn_8040D0AC(const Vec3f *vector, float angle);
float fn_8040F314(float value, float target, float step);
Vec3f fn_8040F364(const Vec3f *forward, const Vec3f *up); /* Euler angles */
void fn_8040F990(Vec3f *along, Vec3f *across, const Vec3f *vector, const Vec3f *axis);
double pow(double x, double y);
}

/* fn_8040CD74 (a - b) called as returning its result by value. */
typedef Vec3f (*SubtractFn)(const Vec3f *a, const Vec3f *b);
#define Subtract ((SubtractFn)fn_8040CD74)

/* A matrix copied with fn_8040C148 (name inferred). */
struct UnkPlayerMtx_800930AC : public Mtx34f {
    UnkPlayerMtx_800930AC() {}
    UnkPlayerMtx_800930AC(const Mtx34f &matrix) {
        fn_8040C148(this, &matrix);
    }
    UnkPlayerMtx_800930AC &operator=(const Mtx34f &matrix) {
        fn_8040C148(this, &matrix);
        return *this;
    }
};

/* The player's members used here (struct name unknown; Player::PlayerBase leaves them unmapped). */
struct UnkPlayerFields_800930AC {
    /* 0x000 */ Vec3f m_position;
    /* 0x00C */ Vec3f m_rotation;
    /* 0x018 */ Vec3f m_velocity;
    /* 0x024 */ Vec3f m_acceleration;
    /* 0x030 */ u8 field_0x30[0x54 - 0x30];
    /* 0x054 */ s32 m_pad;
    /* 0x058 */ u8 field_0x58[0x5C - 0x58];
    /* 0x05C */ s32 field_0x5C;
    /* 0x060 */ u8 field_0x60[0xA0 - 0x60];
    /* 0x0A0 */ u32 m_flags;
    /* 0x0A4 */ u32 field_0xA4;
    /* 0x0A8 */ u32 m_stateBits[2];
    /* 0x0B0 */ u8 field_0xB0[0xB8 - 0xB0];
    /* 0x0B8 */ Vec3f field_0xB8;
    /* 0x0C4 */ u8 field_0xC4[0xE8 - 0xC4];
    /* 0x0E8 */ Vec3f field_0xE8;
    /* 0x0F4 */ Vec3f m_up;
    /* 0x100 */ u8 field_0x100[0x11C - 0x100];
    /* 0x11C */ UnkPlayerMtx_800930AC field_0x11C;
    /* 0x14C */ u8 field_0x14C[0x17C - 0x14C];
    /* 0x17C */ Vec3f field_0x17C;
    /* 0x188 */ Quatf field_0x188;
    /* 0x198 */ UnkPlayerMtx_800930AC field_0x198;
    /* 0x1C8 */ Vec3f m_groundUp;
    /* 0x1D4 */ Quatf field_0x1D4;
    /* 0x1E4 */ UnkPlayerMtx_800930AC field_0x1E4;
    /* 0x214 */ u8 field_0x214[0x278 - 0x214];
    /* 0x278 */ UnkShadow_8009368C *m_shadow;

};

#define Fields(player) reinterpret_cast<UnkPlayerFields_800930AC *>(player)




static inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}


/* Drag across and along the up axis, and in some states (read by fn_80093728); the steepest
 * ground the player stands up on (fn_8009330C); the slowest the player turns to its motion
 * (fn_80093AF8); and the most it tilts towards its ground in a frame (fn_8009339C). */
static float s_dragAcross = 3.0f;
static float s_dragAlong = 1.0f;
static float s_dragFrozen = 10.0f;
static float s_dragState2 = 20.0f;
static float s_dragState7 = 1.0f;
static float s_maxSlope = 30.0f;
static float s_minTurnSpeed = 10.0f;
static float s_maxTilt = 0.2f;

/* The game sets these in __sinit rather than in initial values, as an initializer that is not a
 * constant expression does; the inline call reproduces that (as in Game/CameraModeFixLook2.cpp). */
inline float DynamicFloat(float value) {
    return value;
}

/* The shadow's fade (read by fn_8009325C). */
static float s_shadowScale = DynamicFloat(15.0f);
static float s_shadowRange = DynamicFloat(100.0f);

extern "C" void fn_80094070(void) {
}

/* The direction the pad points in the camera's frame, flat on the player's ground (the front when
 * the player has no pad). */
extern "C" Vec3f fn_80093F74(UnkCameraManager_80009548 *manager, int index);

extern "C" Vec3f fn_80093FC0(const boost::shared_ptr<PlayerBase> *player) {
    if (Fields(player->px)->m_pad != -1) {
        UnkCameraManager_80009548 *manager = fn_80009548();
        Vec3f input;
        fn_800091EC(&input, &(const Vec3f &)fn_80093F74(manager, Fields(player->px)->m_pad));
        fn_8040CE74(&input);
        Vec3f side;
        fn_800091EC(&side, &(const Vec3f &)fn_8040CFB8(&Fields(player->px)->m_groundUp, &input));
        return fn_8040CEE8(&(const Vec3f &)fn_8040CFB8(&side, &Fields(player->px)->m_groundUp));
    }
    return fn_800796DC(player->px);
}

extern "C" Vec3f fn_80093F74(UnkCameraManager_80009548 *manager, int index) {
    return fn_8000A3BC(reinterpret_cast<const u8 *>(fn_8000D0F0(manager, index).get()) + 0x9C);
}

/* vector turned by the angle of the pad's direction relative to the player. */
extern "C" Vec3f fn_80093DD0(boost::shared_ptr<PlayerBase> *player, const Vec3f *vector) {
    UnkCameraManager_80009548 *manager = fn_80009548();
    int index = Fields(player->px)->m_pad;
    if (index == -1) {
        index = 0;
    }
    if (Fields(player->px)->field_0x5C == 2) {
        index = 0;
    }
    Vec3f input;
    fn_800091EC(&input, &(const Vec3f &)fn_80093F74(manager, index));
    fn_8040CE74(&input);
    if (fn_8000A950(fn_8040CF2C(&input))) {
        fn_800091D0(&input, &(const Vec3f &)Vec3f(0.0f, 0.0f, -1.0f));
    }
    float angle;
    if (fn_80014E34(&Fields(player->px)->m_flags, 0)) {
        UnkPlayerMtx_800930AC matrix(fn_8040BEF8(&Fields(player->operator->())->field_0x1E4));
        fn_800091D0(&input, &(const Vec3f &)fn_8040B67C(&matrix, &input));
        angle = fn_80016260(input.x, input.z) - DegToRad(180.0f);
    } else {
        UnkPlayerMtx_800930AC matrix(fn_8040BEF8(&Fields(player->operator->())->field_0x11C));
        fn_800091D0(&input, &(const Vec3f &)fn_8040B67C(&matrix, &input));
        angle = fn_80016260(input.x, input.z) - DegToRad(180.0f);
    }
    return fn_8040D0AC(vector, angle);
}

/* The player's Euler angles for a front and an up given in its frame. */
extern "C" Vec3f fn_80093D34(boost::shared_ptr<PlayerBase> *player, const Vec3f *forward,
                             const Vec3f *up) {
    UnkPlayerMtx_800930AC matrix(fn_8040BEF8(&Fields(player->operator->())->field_0x11C));
    Vec3f front;
    fn_800091EC(&front, &(const Vec3f &)fn_8040B67C(&matrix, forward));
    Vec3f top;
    fn_800091EC(&top, &(const Vec3f &)fn_8040B67C(&matrix, up));
    return fn_8040F364(&front, &top);
}

/* Where direction points relative to the player: 0 ahead, 3 behind, 1 and 2 to the sides. */
extern "C" int fn_80093C60(const boost::shared_ptr<PlayerBase> *player, const Vec3f *direction) {
    Vec3f front;
    fn_800091EC(&front, &(const Vec3f &)fn_800796DC((*player).px));
    if (fn_8040CF90(&front, direction) > 0.70710677f) {
        return 0;
    }
    if (fn_8040CF90(&front, direction) < -0.70710677f) {
        return 3;
    }
    Vec3f side;
    fn_800091EC(&side, &(const Vec3f &)fn_8040CEE8(
                           &(const Vec3f &)fn_8040CFB8(&front, &Fields(player->px)->m_groundUp)));
    if (fn_8040CF90(direction, &side) > 0.0f) {
        return 1;
    }
    return 2;
}

/* Turns the player towards direction by up to rate. */
extern "C" void fn_80093BA0(boost::shared_ptr<PlayerBase> *player, const Vec3f *direction,
                            float rate) {
    Vec3f local;
    fn_800091EC(&local, &(const Vec3f &)fn_8040B67C(
                            &(const Mtx34f &)fn_8040BEF8(&Fields(player->px)->field_0x1E4),
                            direction));
    float pitch;
    float yaw;
    fn_8003B9E8(&local, &pitch, &yaw);
    yaw += DegToRad(180.0f);
    if (rate > 1.0f) {
        rate = 1.0f;
    }
    UnkPlayerFields_800930AC *fields = Fields(player->px);
    fields->m_rotation.y = fn_8040F314(fields->m_rotation.y, yaw, rate);
}

/* Turns the player towards where it moves (or towards field_0xB8). */
extern "C" void fn_80093AF8(boost::shared_ptr<PlayerBase> *player, float rate) {
    Vec3f direction;
    if (fn_8006CD78(Fields(player->px)->m_stateBits, 0x1B)) {
        fn_800091D0(&direction, &Fields(player->px)->field_0xB8);
    } else {
        fn_8040F990(0, &direction, &Fields(player->px)->m_velocity,
                    &Fields(player->px)->m_groundUp);
        if (fn_8040CF2C(&direction) < s_minTurnSpeed * s_minTurnSpeed) {
            return;
        }
    }
    fn_80093BA0(player, &direction, rate);
}

/* Moves position and velocity by acceleration over dt, with drag (none when drag is 0). */
extern "C" void fn_800938CC(Vec3f *position, Vec3f *velocity, const Vec3f *acceleration,
                            float drag, float dt) {
    if (drag <= 0.0f) {
        fn_800091D0(position,
                    &(const Vec3f &)fn_8040CDD0(
                        &(const Vec3f &)fn_8040CDD0(
                            position, &(const Vec3f &)fn_8040CDA8(velocity, dt)),
                        &(const Vec3f &)fn_8040CDA8(
                            &(const Vec3f &)fn_8040CDA8(
                                &(const Vec3f &)fn_8040CDA8(acceleration, dt), dt),
                            0.5f)));
        fn_800091D0(velocity, &(const Vec3f &)fn_8040CDD0(
                                  velocity, &(const Vec3f &)fn_8040CDA8(acceleration, dt)));
    } else {
        float invDecay = 1.0f / (float)pow(2.7182817f, drag * dt);
        float invDrag = 1.0f / drag;
        fn_800091D0(position,
                    &(const Vec3f &)fn_8040CDD0(
                        &(const Vec3f &)fn_8040CDD0(
                            position, &(const Vec3f &)fn_8040CDA8(
                                          &(const Vec3f &)fn_8040CDA8(acceleration, invDrag), dt)),
                        &(const Vec3f &)fn_8040CDA8(
                            &(const Vec3f &)fn_8040CDA8(
                                &(const Vec3f &)fn_8040CDA8(
                                    &(const Vec3f &)Subtract(
                                        &(const Vec3f &)fn_8040CDA8(velocity, drag), acceleration),
                                    invDrag),
                                invDrag),
                            1.0f - invDecay)));
        fn_800091D0(velocity,
                    &(const Vec3f &)fn_8040CDD0(
                        &(const Vec3f &)fn_8040CDA8(acceleration, invDrag),
                        &(const Vec3f &)fn_8040CDA8(
                            &(const Vec3f &)Subtract(
                                velocity, &(const Vec3f &)fn_8040CDA8(acceleration, invDrag)),
                            invDecay)));
    }
}

/* The constructor of a member of the player at 0x270 (nothing to do). */
extern "C" void fn_800938C8(void *member) {
}

UnkShadow_8009368C::~UnkShadow_8009368C() {
}


/* Moves a body by its velocity and acceleration, with the drag of its state. */
extern "C" void fn_80093728(Vec3f *position, Vec3f *velocity, const Vec3f *acceleration,
                            const Vec3f *up, u32 *stateBits, float dt) {
    if (fn_8006CD78(stateBits, 7) || fn_8006CD78(stateBits, 9)) {
        fn_800938CC(position, velocity, acceleration, s_dragState7, dt);
    } else if (fn_8006CD78(stateBits, 2)) {
        fn_800938CC(position, velocity, acceleration, s_dragState2, dt);
    } else if (fn_8006CD78(stateBits, 0x1D)) {
        fn_800938CC(position, velocity, acceleration, s_dragFrozen, dt);
    } else {
        Vec3f velocityAcross;
        Vec3f velocityAlong;
        fn_8040F990(&velocityAlong, &velocityAcross, velocity, up);
        Vec3f accelerationAcross;
        Vec3f accelerationAlong;
        fn_8040F990(&accelerationAlong, &accelerationAcross, acceleration, up);
        fn_800938CC(position, &velocityAcross, &accelerationAcross, s_dragAcross, dt);
        fn_800938CC(position, &velocityAlong, &accelerationAlong, s_dragAlong, dt);
        fn_800091D0(velocity, &(const Vec3f &)fn_8040CDD0(&velocityAcross, &velocityAlong));
    }
}

/* Moves the player. */
extern "C" void fn_800936F4(const boost::shared_ptr<PlayerBase> *player, float dt) {
    UnkPlayerFields_800930AC *fields = Fields(player->px);
    fn_80093728(&fields->m_position, &fields->m_velocity, &fields->m_acceleration, &fields->m_up,
                fields->m_stateBits, dt);
}

UnkShadow_8009368C::UnkShadow_8009368C() {
    fn_8040CE60(&m_position);
    fn_800091D0(&m_normal, &(const Vec3f &)Vec3f(0.0f, 1.0f, 0.0f));
    field_0x18 = 0;
    field_0x1C = 0;
    m_distance = 10000000.0f;
    m_visible = false;
}

/* Casts the shadow from position along direction. */
extern "C" void fn_800935B4(UnkShadow_8009368C *shadow, const Vec3f *position,
                            const Vec3f *direction, int arg) {
    u8 segment[0x18];
    fn_80020A20(segment, position, &(const Vec3f &)fn_8040CDD0(position, direction));
    float distance = fn_800979E8(&boost::shared_ptr<PlayerBase>(), shadow, &shadow->m_normal, 0,
                                 &shadow->field_0x18, &shadow->field_0x1C, segment, arg, 0x1A93,
                                 0x40000, 0xC);
    if (distance < 0.0f) {
        shadow->m_visible = false;
        shadow->m_distance = 10000000.0f;
    } else {
        shadow->m_visible = true;
        shadow->m_distance = distance;
    }
}

/* Not called anywhere: the linker strips it from the game. The original pools the -1.0f of
 * fn_80093DD0's default input direction between the constants of fn_8009339C and fn_800935B4,
 * which is what compiling a function that uses it at this point of the file does (inferred from
 * the order of the literals). */
void PlayerUtilDefaultInputOutOfLine(Vec3f *input) {
    fn_800091D0(input, &(const Vec3f &)Vec3f(0.0f, 0.0f, -1.0f));
}

/* The rotation that turns the world's up to up. */
extern "C" Quatf fn_800934FC(const Vec3f *up) {
    Vec3f axis;
    Vec3f worldUp(0.0f, 1.0f, 0.0f);
    Vec3f to;
    fn_800091EC(&to, &(const Vec3f &)fn_8040CEE8(up));
    fn_800091D0(&axis, &(const Vec3f &)fn_8040CFB8(&worldUp, &to));
    if (fn_8004962C(&axis)) {
        axis.x = 1.0f;
        axis.y = 0.0f;
        axis.z = 0.0f;
    } else {
        fn_8040CE74(&axis);
    }
    return fn_8040C874(&axis, fn_80015D68(fn_8040CF90(&worldUp, &to)));
}

/* Sets the player's ground up and turns the player towards it. */
extern "C" void fn_8009339C(boost::shared_ptr<PlayerBase> *player, const Vec3f *up) {
    fn_800091D0(&Fields(player->px)->m_groundUp, up);
    UnkPlayerFields_800930AC *fields = Fields(player->px);
    fn_8007A34C(&fields->field_0x1D4, &(const Quatf &)fn_800934FC(&fields->m_groundUp));
    Fields(player->operator->())->field_0x1E4 =
        fn_8040B228(&Fields(player->operator->())->field_0x1D4);
    float rate = 15.0f * (lbl_805FAD38 / 180.0f) /
                 fn_80015D68(fn_8040CF90(&fields->field_0x17C, &fields->m_groundUp));
    rate = fn_800704AC(rate, s_maxTilt);
    fn_8007A34C(&fields->field_0x188,
                &(const Quatf &)fn_8040C3EC(&fields->field_0x188, &fields->field_0x1D4, rate));
    Fields(player->operator->())->field_0x198 =
        fn_8040B228(&Fields(player->operator->())->field_0x188);
    fn_800091D0(&Fields(player->px)->field_0x17C,
                &(const Vec3f &)fn_8040B67C(&Fields(player->px)->field_0x198,
                                            &(const Vec3f &)Vec3f(0.0f, 1.0f, 0.0f)));
    fn_8040C958(&Fields(player->px)->field_0x188);
    fn_8040C958(&Fields(player->px)->field_0x1D4);
}

/* Whether the player is on ground no steeper than s_maxSlope. */
extern "C" bool fn_8009330C(boost::shared_ptr<PlayerBase> *player) {
    bool flat = false;
    if (!fn_8006CD78(Fields(player->px)->m_stateBits, 0x1B)) {
        float cosine = fn_8040CF90(&Fields(player->px)->field_0xE8, &Fields(player->px)->m_groundUp);
        if (cosine > std::cos(DegToRad(s_maxSlope))) {
            flat = true;
        }
    }
    return flat;
}

/* The player's up: its ground's when the ground is flat enough. */
extern "C" void fn_800932B4(boost::shared_ptr<PlayerBase> *player) {
    if (fn_8009330C(player)) {
        fn_800091D0(&Fields(player->px)->m_up, &Fields(player->px)->m_groundUp);
    } else {
        fn_800091D0(&Fields(player->px)->m_up, &Fields(player->px)->field_0xE8);
    }
}

/* How strongly the shadow shows. */
extern "C" float fn_8009325C(const boost::shared_ptr<PlayerBase> *player) {
    UnkShadow_8009368C *shadow = Fields(player->px)->m_shadow;
    float strength;
    if (shadow->m_visible) {
        strength = std::fabs(fn_8040CF90(&Fields(player->px)->m_velocity, &shadow->m_normal)) /
                   s_shadowRange;
    } else {
        strength = 0.0f;
    }
    return s_shadowScale * strength;
}

extern "C" Vec3f fn_80093230(const Vec3f *vector, float scale) {
    return fn_8040CDA8(vector, scale);
}

extern "C" Vec3f fn_80093204(const Vec3f *a, const Vec3f *b) {
    return fn_8040CDD0(a, b);
}

/* Where a body starting at from and arriving at to after a time t passes at time t, falling along
 * up (behaviour inferred). */
extern "C" Vec3f fn_800930AC(const Vec3f *from, const Vec3f *to, const Vec3f *up, float t) {
    Vec3f along;
    Vec3f across;
    Vec3f axis;
    Vec3f lift;
    fn_800091EC(&axis, &(const Vec3f &)fn_80016284(&(const Vec3f &)fn_8040CEE8(up)));
    float height = fn_8040CF50(up);
    fn_8040F990(&along, &across, &(const Vec3f &)Subtract(to, from), &axis);
    fn_800091EC(&lift,
                &(const Vec3f &)fn_8040CDA8(&axis, 0.5f * height + fn_8040CF90(&along, &axis)));
    return fn_8040CDD0(
        &(const Vec3f &)fn_80093204(from, &(const Vec3f &)fn_800091A4(&across, t)),
        &(const Vec3f &)fn_80093204(
            &(const Vec3f &)fn_800091A4(&lift, t),
            &(const Vec3f &)fn_800091A4(
                &(const Vec3f &)fn_800091A4(&(const Vec3f &)fn_80093230(up, 0.5f), t), t)));
}
