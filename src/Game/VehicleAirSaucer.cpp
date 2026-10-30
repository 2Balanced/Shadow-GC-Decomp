/* Vehicle::AirSaucer (class name from RTTI): the air saucer, a hovering vehicle (vehicle type 7)
 * that its rider steers (command 1). It floats at a height over the ground, tilts with the slope
 * and the steering, jumps (command 0x15, action 2: a hop, then a boost while the button is held)
 * and, when it finds a vehicle path below it, rides along the path (state 5 and 6) until the path
 * ends. Hits push it back and hurt it; it attacks what it rams. State 0 waits for a rider, 1
 * hovers, 2 and 3 jump, 4 falls, 5 and 6 follow a path. Its model is the weapon model type
 * (Weapon::UnkWeaponModel_80063428). File name from the class (prefixed like
 * Game/VehicleWalker.cpp); member and function names are inferred. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include <list>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Effect/System/EffectElement.h"
#include "Game/BirdBase.h"
#define CHARCOMMAND_PLAYER_COMMANDS
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetSound.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/Quaternion.h"
#include "Game/TVehicleBase.h"
#include "Game/VehicleEffect.h"
#include "Game/Vibration.h"
#include "Game/WeaponBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Effect::System::EffectElement;
using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;
using Vehicle::TVehicleBase;
using Vehicle::BirdBase::UnkBirdCollision_801B4EBC;
using Weapon::UnkWeaponEffectSet_80063428;
using Weapon::UnkWeaponEffectSub_80063428;
using Weapon::UnkWeaponEffect_80063428;
using Weapon::UnkWeaponModel_80063428;

extern "C" {
/* The C library's floor and arc cosine (double). */
double fn_803B1618(double x);
double fn_803B1BFC(double x);
}

/* The float overloads of <cmath> the unit uses: fabs and cos are linked from other units' copies;
 * floor and acos come out of line here (one level of deferred inlining) as the game's only copies,
 * which Game/BirdBase.cpp and Game/DiablonPartsBody.cpp call. */
namespace std {
float fabs(float x);
float cos(float x);

inline float FloorF(float x) {
    return (float)fn_803B1618(x);
}

inline float floor(float x) {
    return FloorF(x);
}

inline float AcosF(float x) {
    return (float)fn_803B1BFC(x);
}

inline float acos(float x) {
    return AcosF(x);
}
} // namespace std

/* A vehicle kind's entry in the vehicle table (see Game/GunLift.cpp). */
struct UnkVehicleEntry {
    const char *name;
    Vehicle::TVehicleBase *(*create)(void *arg);
};

/* What a mover stands on (see Game/VehicleCharaColliMove.cpp). */
struct UnkColliMoveContact_803334A4 {
    bool IsValid() const;

    /* 0x00 */ u8 m_isCollider;
    /* 0x04 */ boost::weak_ptr<CharInfo> m_chara;
    /* 0x0C */ boost::weak_ptr<CharaColliBase> m_collider;
    /* 0x14 */ void *m_object;
};

/* A placement taken from a contact (see Game/VehicleCharaColliMove.cpp). */
struct UnkColliMovePlacement_80333098 {
    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ Vec3f m_rotation;
    /* 0x18 */ u32 m_kind;
};

/* CharaColliMoveArgs (class name from RTTI; see Game/VehicleWalker.cpp). */
struct CharaColliMoveArgs {
    /* 0x00 */ UnkColliMoveContact_803334A4 *m_contact;
    /* 0x04 */ boost::shared_ptr<CharInfo> m_chara;
    /* 0x0C */ UnkColliMovePlacement_80333098 m_current;
    /* 0x28 */ UnkColliMovePlacement_80333098 m_previous;
};

typedef boost::function1<void, CharaColliMoveArgs &> CharaColliMoveCallback;

/* The list the collision move skips (see Game/VehicleWalker.cpp). */
struct UnkColliList_80095314;
typedef std::list<boost::shared_ptr<CharaColliBase> > ColliderList;

/* The vehicles' collision move (see Game/VehicleColliMove.cpp). */
class UnkVehicleColliMove_8033377C {
  public:
    UnkVehicleColliMove_8033377C(TVehicleBase *vehicle);
    ~UnkVehicleColliMove_8033377C();

    bool Move(Vec3f *position, Vec3f *velocity, bool ground, UnkColliList_80095314 *ignore);
    void TickGround(float dt);
    void SetTrackGround(u8 track);
    void ClearOnGround();
    bool IsOnGround();
    bool IsLanding();
    bool IsLineDrawn();

    /* 0x00 */ Vec3f m_velocity;
    /* 0x0C */ Vec3f m_normal; /* of the ground */
    /* 0x18 */ u8 field_0x18[0x28 - 0x18];
};

/* The path follower of the vehicles (see Game/VehiclePathMove.cpp). */
class UnkPathMoverImpl_803348C8;

class UnkPathMover_803345C4 {
  public:
    UnkPathMover_803345C4();
    ~UnkPathMover_803345C4();

    /* 0x0 */ UnkPathMoverImpl_803348C8 *m_impl;
};

/* A shadow cast on the ground (see Game/VehicleWalker.cpp). */
struct UnkShadow_8009368C {
    UnkShadow_8009368C();

    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ Vec3f m_normal;
    /* 0x18 */ u32 field_0x18;
    /* 0x1C */ u32 field_0x1C;
    /* 0x20 */ float field_0x20;
    /* 0x24 */ bool m_visible;
};

/* A node of a collision hit list: the hit's object at 0x14 (see Game/BirdBase.cpp). */
struct UnkColliContactNode_800587A0 {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ void *m_object;
};

/* The parts of AttackCallbackParam the saucer reads (see Game/BirdBase.cpp). */
struct SaucerAttackParam {
    /* 0x00 */ u8 field_0x0[0x18];
    /* 0x18 */ Vec3f m_position;
    /* 0x24 */ Vec3f m_direction;
    /* 0x30 */ u8 field_0x30[0x50 - 0x30];
    /* 0x50 */ float m_damage;
    /* 0x54 */ s32 m_result;
    /* 0x58 */ const Vec3f *m_normal;
};

#define Param(param) (*(SaucerAttackParam *)&(param))

/* The identity rotation (a quaternion). */
extern const Quatf lbl_805E0790;

extern "C" {
/* The 32-bit flag word (see Game/BAMine.cpp). */
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* Debug output and resource loading, the vehicle type in use, and the loader's model clumps (see
 * Game/VehicleWalker.cpp). */
void fn_803A953C(const char *format, ...);
void fn_8006B50C(int id, int arg1);
void fn_801B4850(int type);
void *fn_800122D4(void);
void *fn_8001277C(void *loader, int arg1, int id);

/* The path follower: starting it from a position and a direction, moving it along by a distance
 * (false at the path's end) and its placement (any output may be null; see
 * Game/VehiclePathMove.cpp). */
bool fn_803344C8(UnkPathMover_803345C4 *mover, const Vec3f *start, const Vec3f *direction);
bool fn_803344A4(UnkPathMover_803345C4 *mover, float distance);
void fn_80334480(UnkPathMover_803345C4 *mover, Vec3f *position, Vec3f *forward, Vec3f *up);

/* The model's other functions (see Game/WeaponBase.h; behaviour inferred). */
bool fn_80063178(UnkWeaponModel_80063428 *model);
void fn_800631AC(UnkWeaponModel_80063428 *model);

/* TVehicleBase's other functions (see Game/VehicleWalker.cpp): the rider leaves, the vehicle's
 * life (set and get) and a change of it, its kind, the end of the ride, the default command
 * handling, a second model clump, its collision-move callback, its two colliders (get and set),
 * a line test, the collision move's velocity and the contact it stands on. */
void fn_801B6030(TVehicleBase *vehicle, int arg1);
void fn_801B6570(TVehicleBase *vehicle, float value);
float fn_801B64F8(TVehicleBase *vehicle);
void fn_801B6510(TVehicleBase *vehicle, float delta);
void fn_801B6944(TVehicleBase *vehicle);
void fn_801B6860(TVehicleBase *vehicle, CharCommand *command);
void fn_801B64B8(TVehicleBase *vehicle, void *clump);
void fn_801B61DC(TVehicleBase *vehicle, const CharaColliMoveCallback &callback);
boost::shared_ptr<CharaColliBase> fn_801B6350(TVehicleBase *vehicle);
boost::shared_ptr<CharaColliBase> fn_801B6380(TVehicleBase *vehicle);
void fn_801B63D4(TVehicleBase *vehicle, boost::shared_ptr<CharaColliBase> collider);
void fn_801B6470(TVehicleBase *vehicle, boost::shared_ptr<CharaColliBase> collider);
void fn_801B5FA4(TVehicleBase *vehicle);
void fn_801C067C(TVehicleBase *vehicle, const boost::shared_ptr<CharaColliBase> *collider);
Vec3f fn_801B6108(TVehicleBase *vehicle);
UnkColliMoveContact_803334A4 *fn_801B6150(TVehicleBase *vehicle);

/* The collision at 0xD0 (see Game/BirdBase.cpp): its owner, its collider (set and get), its hit
 * callback, two resets, and hiding and showing it (behaviour inferred). */
void fn_801B4DD0(UnkBirdCollision_801B4EBC *collision, TVehicleBase *vehicle);
void fn_801B4DAC(UnkBirdCollision_801B4EBC *collision,
                 const boost::shared_ptr<CharaColliBase> *collider);
boost::shared_ptr<CharaColliBase> fn_801B4D84(UnkBirdCollision_801B4EBC *collision);
void fn_801B4D18(UnkBirdCollision_801B4EBC *collision, const AttackCallback &callback);
void fn_801B4CF4(UnkBirdCollision_801B4EBC *collision);
void fn_801B4D60(UnkBirdCollision_801B4EBC *collision);
void fn_801B4C4C(UnkBirdCollision_801B4EBC *collision);
void fn_801B4CA0(UnkBirdCollision_801B4EBC *collision);

/* A body attack with its two callbacks (see Game/BirdBase.cpp). */
boost::shared_ptr<CharaColliAttack> fn_801C08F0(const boost::weak_ptr<CharInfo> &owner,
                                               const AttackCallback &onHit,
                                               const AttackCallback &onEnd);

/* The collider manager (see Game/BirdBase.cpp). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7DEC(void *manager,
                                              const boost::weak_ptr<CharInfo> &charInfo, int flags,
                                              int arg3, int arg4, float radius, const Vec3f *offset);
bool fn_800C3ED4(void *manager, UnkColliContactNode_800587A0 *hit);
void fn_800C40C4(void *manager, boost::weak_ptr<CharaColliAttack> attack,
                 UnkColliContactNode_800587A0 *hit, const Vec3f *normal);
const Vec3f *fn_800266D4(void *object);
const Vec3f *fn_8001E9BC(void *object);

/* The shadow (see Game/VehicleWalker.cpp). */
void fn_800935B4(UnkShadow_8009368C *shadow, const Vec3f *position, const Vec3f *direction,
                 ColliderList *ignore);
void fn_803563D0(UnkShadow_8009368C *shadow, float size);

/* Effects, sounds and rumble (see Game/BirdBase.cpp). */
boost::weak_ptr<EffectElement> fn_801D2AFC(int arg0, const Vec3f *position, const Vec3f *direction,
                                           int id, int arg4);
void *fn_80053164(void);
void fn_801D54F4(void *manager, int id, const void *source, int arg3, int arg4);
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int player);

/* Clamps (see Game/GunLift.cpp), and whether a value is zero. */
const float &fn_80070460(const float &value, const float &low, const float &high);
const float &fn_800704AC(const float &a, const float &b); /* the smaller */
bool fn_8000A950(float value);

/* Vector, quaternion and matrix helpers (behaviour inferred). */
Vec3f fn_800091A4(const Vec3f *vector, float scale);
Vec3f fn_80016284(const Vec3f *vector); /* the negated vector */
void fn_8040CE60(Vec3f *vector);        /* sets the vector to zero */
void fn_8040E970(Vec3f *angles);        /* sets the angles to zero */
void fn_8040CE38(Vec3f *vector, float scale);
float fn_8040CE74(Vec3f *vector); /* normalizes the vector */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b); /* a + b */
Vec3f fn_8040CEE8(const Vec3f *vector);           /* normalized */
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b); /* the cross product */
float fn_8040CF90(const Vec3f *a, const Vec3f *b); /* the dot product */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* vector += value */
float fn_8040E99C(float angle);
float fn_8040F314(float from, float to, float rate);
float fn_8040F8FC(const Vec3f *a, const Vec3f *b); /* the angle between */
Vec3f fn_8040F364(const Vec3f *forward, const Vec3f *up); /* Euler angles */
Vec2f fn_8040F664(const Vec3f *direction); /* the pitch and the heading */
void fn_8040F990(Vec3f *along, Vec3f *across, const Vec3f *vector, const Vec3f *axis);
void fn_800938CC(Vec3f *position, Vec3f *velocity, const Vec3f *acceleration, float drag,
                 float dt);
Vec3f fn_800930AC(const Vec3f *from, const Vec3f *to, const Vec3f *up, float t);
float fn_80015D68(float value); /* arc cosine */
void fn_8040C908(Quatf *q);    /* sets the quaternion to the identity */
void fn_8007A34C(Quatf *q, const Quatf *value);
Quatf fn_8040C874(const Vec3f *axis, float angle);
Quatf fn_8040C3EC(const Quatf *from, const Quatf *to, float t);
void fn_8040B8C0(Mtx34f *matrix, const Quatf *rotation);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Vec3f fn_8007E4E8(const Mtx34f *matrix, const Vec3f *point);
Mtx34f fn_8040FFBC(const Mtx34f *matrix, const Vec3f *angles, int order);
Mtx34f fn_8040FEBC(const Mtx34f *matrix, const Vec3f *angles, int order);
Mtx34f fn_8040FD8C(const Vec3f *axis, float angle);
}

/* fn_8040CD74 called as returning a - b by value (see Game/GunLift.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* fn_800091EC called as returning a copy of from by value (it constructs its first argument). */
typedef Vec3f (*VectorCopyFn)(const Vec3f *from);

/* The vector type's out-of-line subtraction (0x8000AD64; see Game/CameraAdjust.cpp). */
Vec3f operator-(const Vec3f &a, const Vec3f &b);

/* The vector type's out-of-line copy constructor and assignment (see Game/GunLift.cpp). */
inline void ConstructVector(Vec3f *vector, const Vec3f &value) {
    fn_800091EC(vector, &value);
}

inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* Uninitialized storage for a vector built through the out-of-line constructor fn_80008E1C (see
 * Game/BirdBase.cpp). */
struct UnkVectorStorage {
    UnkVectorStorage &operator=(const UnkVectorStorage &other);

    float m_values[3];
};

#define NewVector(x, y, z) \
    fn_80008E1C((Vec3f *)&(const UnkVectorStorage &)UnkVectorStorage(), x, y, z)

/* A vector scaled (name inferred). */
inline Vec3f Scale(const Vec3f &vector, float scale) {
    return fn_8040CDA8(&vector, scale);
}

/* Degrees to radians (see Game/GunLift.cpp). */
inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* The dot product (name inferred). */
inline float Dot(const Vec3f *a, const Vec3f *b) {
    return fn_8040CF90(a, b);
}

/* Whether a vector is (nearly) zero (name inferred). */
inline bool IsSmall(const Vec3f &v) {
    return v.x * v.x + v.y * v.y + v.z * v.z < 0.0001f;
}

/* A length (see Game/VehicleWalker.cpp): stored by the static initializer. */
inline float Span(float length) {
    return length;
}

/* The 32-bit flag word at 0x144 (struct name unknown; see Game/VehicleWalker.cpp). */
struct SaucerFlags {
    SaucerFlags() {
        fn_80014E04(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    void Clear(int bit) {
        fn_8006127C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    /* bit 0: gravity, 1: on a path, 2: on the path's ground, 3: placed by a carrier, 4: jump
     * requested, 5: boost requested, 6: just placed */
    u32 m_bits;
};

namespace Vehicle {

class AirSaucer : public TVehicleBase {
  public:
    AirSaucer(void *arg);
    virtual ~AirSaucer();
    virtual void SetPlacement(const Vec3f *position, const Vec3f *rotation);
    virtual void Setup();
    virtual void OnCommand(CharCommand *command);
    virtual void vf_0x40(UnkColliContactNode_800587A0 *const &hit);
    virtual void Tick(float dt);
    virtual void OnGetOn();
    virtual void OnGetOff();
    virtual bool vf_0x5C();
    virtual void vf_0x68();
    virtual void vf_0x6C();
    virtual bool vf_0x70();
    virtual void vf_0x74();
    virtual void vf_0x78();
    virtual void vf_0x7C(void *arg);

    void Reset();
    void CreateBodyCollider();
    void Move(float dt);
    void OnColliMove(CharaColliMoveArgs &args);
    void Accelerate(float dt);
    void Hover(float dt);
    void Turn(float dt, const Vec3f *steer);
    void TiltToTarget(float dt);
    void TiltToGround(float dt);
    /* An unused function (see its definition). */
    void GetDown(Vec3f *down);
    bool FindPath(float dt);
    void FollowPath(float dt);
    void UpdatePathSpeed(float dt);
    bool Idle(float dt);
    void CreateModel();
    void CreateAttack();
    void Damage(float power, const Vec3f *position, const Vec3f *direction);
    void OnAttack(AttackCallbackParam &param);
    void OnAttackEnd(AttackCallbackParam &param);
    void Push(const Vec3f *position, const Vec3f *normal, float power);
    void OnHit(AttackCallbackParam &param);

    /* 0x09C */ UnkVehicleColliMove_8033377C *m_colliMove;
    /* 0x0A0 */ UnkPathMover_803345C4 m_path;
    /* 0x0A4 */ UnkShadow_8009368C m_shadow;
    /* 0x0CC */ UnkWeaponModel_80063428 *m_model;
    /* 0x0D0 */ UnkBirdCollision_801B4EBC m_collision;
    /* 0x0D4 */ boost::shared_ptr<CharaColliAttack> m_attack;
    /* 0x0DC */ Vec3f m_up;      /* of gravity */
    /* 0x0E8 */ Vec3f m_forward; /* on a path */
    /* 0x0F4 */ Vec3f m_pathUp;  /* on a path */
    /* 0x100 */ float m_pathSpeed;
    /* 0x104 */ float m_pathDelay;
    /* 0x108 */ float m_boostTime;
    /* 0x10C */ Vec3f m_steer;
    /* 0x118 */ Vec3f m_angles;
    /* 0x124 */ Quatf m_targetTilt;
    /* 0x134 */ Quatf m_groundTilt;
    /* 0x144 */ SaucerFlags m_flags;
    /* 0x148 */ Vec3f m_target; /* the point the saucer floats toward */
    /* 0x154 */ float m_drift;
    /* 0x158 */ u32 field_0x158;
    /* 0x15C */ float m_flinchTime;
    /* 0x160 */ UnkSoundHandle_801D3DC8 m_sound;
    /* 0x17C */ s32 m_state;
};
SIZE_ASSERT(AirSaucer, 0x180);

} // namespace Vehicle

using Vehicle::AirSaucer;

static const char *s_taskName = "AirSaucer";

/* The saucer's height above the ground (unused), the body's height, and how near a path has to
 * be (names inferred). */
static const float s_hoverHeight = Span(7.25f);
static const float s_height = Span(7.0f);
static const float s_pathRange = Span(8.0f);

/* The clamps' limits (names inferred). */
const float kMaxPushRatio = 3.0f;
const float kMaxDamage = 2.0f;
const float kMaxDrift = 0.96875f;

/* The model's effects and sounds (meanings inferred; see Game/WeaponBase.h). */
static const UnkWeaponEffectSub_80063428 s_effectSubs[] = {
    {0x5C, 0, 0},
};

static const UnkWeaponEffect_80063428 s_effects[] = {
    {-1, -1, 0x16, 3, s_effectSubs, 1},
    {0x2329, 0x232B, 0x3E, -1, 0, 0},
};

static const u32 s_sounds[] = {0x2328};

static const UnkWeaponEffectSet_80063428 s_effectSet = {s_effects, 2, s_sounds, 1};

/* Turns from toward to by at most maxAngle (name inferred). */
static Vec3f TurnToward(const Vec3f *from, const Vec3f *to, float maxAngle) {
    Vec3f axis;
    AssignVector(axis, fn_8040CFB8(from, to));
    if (IsSmall(axis)) {
        return ((VectorCopyFn)fn_800091EC)(to);
    }
    float angle = fn_8040F8FC(from, to);
    if (angle > maxAngle) {
        angle = maxAngle;
    }
    Mtx34f matrix;
    fn_8040C148(&matrix, &(const Mtx34f &)fn_8040FD8C(&(const Vec3f &)fn_8040CEE8(&axis),
                                                      0.5f * angle));
    return fn_8040B67C(&matrix, from);
}

AirSaucer::AirSaucer(void *arg) : TVehicleBase(arg), m_model(0), m_angles(0.0f, 0.0f, 0.0f) {
    fn_801B4DD0(&m_collision, this);
    fn_8040CE60(&field_0x18);
    fn_8040E970(&m_angles);
    fn_8040C908(&m_targetTilt);
    fn_8040C908(&m_groundTilt);
    fn_8040CE60(&m_steer);
    m_drift = 0.0f;
    m_up.x = 0.0f;
    m_up.y = 1.0f;
    m_up.z = 0.0f;
    m_pathDelay = 0.0f;
    m_flinchTime = 0.0f;
    m_colliMove = new UnkVehicleColliMove_8033377C(this);
    m_state = 0;
}

AirSaucer::~AirSaucer() {
    if (m_model != 0) {
        delete m_model;
        m_model = 0;
    }
    if (m_colliMove != 0) {
        delete m_colliMove;
    }
    m_sound.fn_801D455C();
}

void AirSaucer::Setup() {
    fn_801B6570(this, 10.0f);
    SetField98Float(10.0f);
    CreateModel();
    CreateAttack();
    Reset();
    fn_801B61DC(this, CharaColliMoveCallback(boost::bind(&AirSaucer::OnColliMove, this, _1)));
}

void AirSaucer::OnCommand(CharCommand *command) {
    if (m_flags.Test(3)) {
        fn_8040CE60(&field_0x18);
        m_flags.Clear(0);
    } else {
        m_flags.Set(0);
    }
    switch (command->TypeId()) {
    case 1:
        fn_8040CFF8(&m_steer, &static_cast<AccelerateInputCommand *>(command)->m_direction);
        command->Flags() |= 2;
        break;
    case 2: {
        SetPositionCommand *move = static_cast<SetPositionCommand *>(command);
        AssignVector(*reinterpret_cast<Vec3f *>(field_0x64), move->m_position);
        AssignVector(m_position, move->m_position);
        m_flags.Set(6);
        command->Flags() |= 2;
        break;
    }
    case 3:
        m_rotation.y = static_cast<SetAngleCommand *>(command)->m_angles.y;
        command->Flags() |= 2;
        break;
    case 4:
    case 5:
        command->Reject(GetSelf());
        m_colliMove->ClearOnGround();
        command->Flags() |= 2;
        break;
    case 7:
        Damage(static_cast<DamageCommand *>(command)->m_amount, &m_position,
               &static_cast<DamageCommand *>(command)->m_direction);
        command->Flags() |= 2;
        break;
    case 0x15:
        switch (static_cast<ControlVehicleCommand *>(command)->m_action) {
        case 1:
            fn_801B6030(this, 0);
            command->Flags() |= 2;
            break;
        case 2: {
            UnkColliMoveContact_803334A4 *contact = fn_801B6150(this);
            if (contact != 0 && contact->IsValid()) {
                boost::shared_ptr<CharInfo> chara = contact->m_chara.lock();
                if (*reinterpret_cast<const s32 *>(&chara->field_0x3C[0x50 - 0x3C]) == 0x37) {
                    chara->PushCommand(new UseCommand(GetSelf()));
                    command->Flags() |= 2;
                    break;
                }
            }
            m_flags.Set(4);
            command->Flags() |= 2;
            break;
        }
        case 3:
            break;
        case 4:
            if (m_flags.Test(1)) {
                m_flags.Set(5);
            } else {
                fn_801B6030(this, 0);
            }
            command->Flags() |= 2;
            break;
        }
        break;
    }
    if (!(command->Flags() & 2)) {
        fn_801B6860(this, command);
    }
}

void AirSaucer::vf_0x40(UnkColliContactNode_800587A0 *const &hit) {
    void *object;
    void *manager = fn_80021B08();
    object = hit->m_object;
    Vec3f normal;
    fn_800091EC(&normal, fn_800266D4(object));
    Vec3f position;
    fn_800091EC(&position, fn_8001E9BC(object));
    if (m_attack.get() != 0 && fn_800C3ED4(manager, hit) && !GetRider().expired()) {
        fn_800C40C4(manager, boost::weak_ptr<CharaColliAttack>(m_attack), hit, &normal);
    } else {
        Push(&position, &normal, 2.0f);
    }
}

void AirSaucer::SetPlacement(const Vec3f *position, const Vec3f *rotation) {
    TVehicleBase::SetPlacement(position, rotation);
    m_model->fn_800632E8(position, rotation);
    fn_8040CE60(&field_0x18);
    AssignVector(*reinterpret_cast<Vec3f *>(field_0x64), m_position);
    m_angles = m_rotation;
    fn_8040C908(&m_targetTilt);
    fn_8040C908(&m_groundTilt);
    fn_800091D0(&m_target, &(const Vec3f &)fn_8040CDD0(&m_position, &Vec3f(0.0f, 5.0f, 0.0f)));
    m_flags.Set(6);
}

Quatf::Quatf(const Quatf &q) : x(q.x), y(q.y), z(q.z), w(q.w) {}

void AirSaucer::Tick(float dt) {
    if (m_flinchTime > 0.0f) {
        m_model->fn_800631D0();
        m_flinchTime -= dt;
        if (m_flinchTime <= 0.0f) {
            m_flinchTime = 0.0f;
            fn_800631AC(m_model);
        }
    }
    if (m_colliMove != 0) {
        m_colliMove->SetTrackGround(fn_8040CF2C(&field_0x18) > 100.0f);
        m_colliMove->TickGround(dt);
    }
    switch (m_state) {
    case 0:
        Accelerate(dt);
        Move(dt);
        Hover(dt);
        Turn(dt, &m_steer);
        TiltToTarget(dt);
        TiltToGround(dt);
        Idle(dt);
        if (!GetRider().expired()) {
            m_state = 1;
        }
        break;
    case 1:
        if (GetRider().expired()) {
            m_state = 0;
            break;
        }
        Accelerate(dt);
        Move(dt);
        Hover(dt);
        Turn(dt, &m_steer);
        TiltToTarget(dt);
        TiltToGround(dt);
        if (m_colliMove->IsOnGround() && m_flags.Test(4)) {
            m_state = 2;
            break;
        }
        if (FindPath(dt)) {
            m_state = 5;
        }
        break;
    case 2:
        fn_8040F990(0, &field_0x18, &field_0x18, &m_up);
        fn_8040CFF8(&field_0x18, &(const Vec3f &)fn_800091A4(&m_colliMove->m_normal, 100.0f));
        m_colliMove->ClearOnGround();
        fn_801D54F4(fn_80053164(), 0x2044, this, 0, 1);
        m_state = 3;
    case 3:
        if (m_flags.Test(4)) {
            fn_8040F990(0, &field_0x18, &field_0x18, &m_up);
            fn_8040CFF8(&field_0x18, &(const Vec3f &)fn_800091A4(&m_up, 100.0f));
            fn_801D54F4(fn_80053164(), 0x2045, this, 0, 1);
            m_state = 4;
        }
    case 4:
        Accelerate(dt);
        Move(dt);
        Hover(dt);
        Turn(dt, &m_steer);
        TiltToTarget(dt);
        TiltToGround(dt);
        if (FindPath(dt)) {
            m_state = 5;
            break;
        }
        if (m_colliMove->IsOnGround()) {
            m_state = 1;
        }
        break;
    case 5:
        m_pathSpeed = fn_8040CF50(&field_0x18);
        fn_800091D0(&m_pathUp, &Vec3f(0.0f, 1.0f, 0.0f));
        fn_80334480(&m_path, 0, &m_forward, 0);
        m_pathDelay = 0.25f;
        m_boostTime = 0.0f;
        m_flags.Set(1);
        m_flags.Set(2);
        m_state = 6;
    case 6:
        if (m_flags.Test(5) && 0.0f == m_boostTime) {
            m_pathSpeed += 50.0f;
            m_boostTime = 0.5f;
        }
        if (fn_803344A4(&m_path, dt * m_pathSpeed)) {
            FollowPath(dt);
            UpdatePathSpeed(dt);
        } else {
            m_flags.Clear(1);
            m_state = 1;
            break;
        }
        Hover(dt);
        Turn(dt, &m_forward);
        TiltToTarget(dt);
        TiltToGround(dt);
        if (m_flags.Test(4)) {
            m_flags.Clear(1);
            m_state = 2;
            break;
        }
        if (!fn_8000A950(dt)) {
            AssignVector(field_0x18,
                         fn_8040CDA8(&(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                         &m_position, reinterpret_cast<Vec3f *>(field_0x64)),
                                     1.0f / dt));
        }
        break;
    }
    Mtx34f matrix;
    fn_8040C148(&matrix, &lbl_805E0720);
    Quatf tilt(MultiplyQuat(m_groundTilt, m_targetTilt));
    fn_8040B8C0(&matrix, &tilt);
    Vec3f up;
    fn_800091EC(&up, &(const Vec3f &)fn_8040B67C(&matrix, &Vec3f(0.0f, 1.0f, 0.0f)));
    float cosine = Dot(&up, &Vec3f(0.0f, 1.0f, 0.0f));
    if (cosine < std::cos(DegToRad(50.0f))) {
        float angle = std::acos(cosine);
        fn_8007A34C(&tilt,
                    &(const Quatf &)fn_8040C3EC(&lbl_805E0790, &tilt, DegToRad(50.0f) / angle));
        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B8C0(&matrix, &tilt);
    }
    fn_8040B848(&matrix, m_angles.y);
    fn_8040B884(&matrix, m_angles.x);
    fn_8040B80C(&matrix, m_angles.z);
    m_rotation = fn_8040F364(&(const Vec3f &)fn_8007E4E8(&matrix, NewVector(0.0f, 0.0f, -1.0f)),
                             &(const Vec3f &)fn_8007E4E8(&matrix, NewVector(0.0f, 1.0f, 0.0f)));
    m_model->fn_800632E8(&m_position, &m_rotation);
    m_model->fn_800632A0(dt);
    {
        ColliderList ignore;
        ignore.push_back(fn_801B6380(this));
        ignore.push_back(fn_801B6350(this));
        fn_800935B4(&m_shadow, &m_position,
                    &(const Vec3f &)fn_8040CDA8(&Vec3f(0.0f, -1.0f, 0.0f), 100.0f), &ignore);
        m_flags.Clear(6);
    }
    fn_803563D0(&m_shadow, 6.0f);
    if (fn_801B64F8(this) <= 0.0f) {
        fn_801D54F4(fn_80053164(), 0x2049, this, 0, 1);
        if (field_0x74 != 0) {
            reinterpret_cast<Vehicle::Effect *>(field_0x74)
                ->StartBurst((void *)m_model->fn_80063234(0));
        }
        fn_801B6030(this, 0);
        fn_801B6944(this);
    }
    fn_801C067C(this, &fn_801B4D84(&m_collision));
    if (!m_flags.Test(1) && m_colliMove->IsLanding() && !GetRider().expired()) {
        fn_8034DF48(10, GetRider().lock()->field_0x54);
    }
    if (m_colliMove->IsLineDrawn()) {
        fn_801B5FA4(this);
    }
    fn_8040CE60(&field_0x24);
    fn_8040CE60(&m_steer);
    AssignVector(*reinterpret_cast<Vec3f *>(field_0x64), m_position);
    m_flags.Clear(4);
    m_flags.Clear(5);
}

void AirSaucer::OnGetOn() {
    TVehicleBase::OnGetOn();
    m_sound.fn_801D455C();
    m_sound = fn_801D3DC8(0x2043, reinterpret_cast<GadgetCharInfo *>(this), this);
    CreateBodyCollider();
}

void AirSaucer::OnGetOff() {
    TVehicleBase::OnGetOff();
    m_sound.fn_801D455C();
    Reset();
}

void AirSaucer::vf_0x68() {
    m_model->fn_800631D0();
}

void AirSaucer::vf_0x6C() {
    fn_800631AC(m_model);
}

bool AirSaucer::vf_0x70() {
    return fn_80063178(m_model);
}

void AirSaucer::vf_0x74() {
    m_model->fn_80063154();
    fn_801B4C4C(&m_collision);
}

void AirSaucer::vf_0x78() {
    m_model->fn_80063130();
    fn_801B4CA0(&m_collision);
}

void AirSaucer::vf_0x7C(void *arg) {
    m_model->fn_8006310C(arg);
}

void AirSaucer::Reset() {
    void *loader = fn_800122D4();
    void *clump = fn_8001277C(loader, 3, 0x51);
    SetClump(clump);
    fn_801B64B8(this, fn_8001277C(loader, 3, 0x52));
    fn_801B4CF4(&m_collision);
    fn_801B4D60(&m_collision);
}

void AirSaucer::CreateBodyCollider() {
    void *manager = fn_80021B08();
    boost::shared_ptr<CharaColliBase> collider = fn_800C7DEC(
        manager, boost::weak_ptr<CharInfo>(GetSelf()), 0x302, 2, 0, 0.51f + s_height, &lbl_805E07A0);
    fn_801B6470(this, collider);
    fn_801B63D4(this, boost::shared_ptr<CharaColliBase>());
    fn_801B4DAC(&m_collision, &collider);
    fn_801B4D18(&m_collision, AttackCallback(boost::bind(&AirSaucer::OnHit, this, _1)));
}

void AirSaucer::Move(float dt) {
    if (!m_flags.Test(3)) {
        Vec3f delta;
        ConstructVector(&delta, ((VectorSubtractFn)fn_8040CD74)(
                                    &m_position, reinterpret_cast<Vec3f *>(field_0x64)));
        Vec3f position;
        ConstructVector(&position, *reinterpret_cast<Vec3f *>(field_0x64));
        m_colliMove->Move(&position, &delta, true, 0);
        if (m_colliMove->IsOnGround() && !fn_8000A950(dt)) {
            AssignVector(field_0x18, fn_8040CDA8(&delta, 1.0f / dt));
        }
        AssignVector(m_position, position);
    }
}

void AirSaucer::OnColliMove(CharaColliMoveArgs &args) {
    Mtx34f matrix;
    Mtx34f translation;
    fn_8040B034(&translation, &args.m_current.m_position);
    fn_8040C148(&matrix, &translation);
    fn_8040C148(&matrix, &(const Mtx34f &)fn_8040FFBC(&matrix, &args.m_current.m_rotation,
                                                      args.m_current.m_kind));
    fn_8040C148(&matrix, &(const Mtx34f &)fn_8040FEBC(&matrix, &args.m_previous.m_rotation,
                                                      args.m_previous.m_kind));
    fn_8040B59C(&matrix, &(const Vec3f &)fn_80016284(&args.m_previous.m_position));
    const Vec3f &moved = fn_8040B67C(&matrix, &args.m_chara->m_position);
    fn_800091D0(&args.m_chara->m_position, &moved);
    args.m_chara->m_rotation.y += args.m_current.m_rotation.y - args.m_previous.m_rotation.y;
    if (m_flags.Test(3)) {
        AssignVector(*reinterpret_cast<Vec3f *>(field_0x64), m_position);
        return;
    }
    Vec3f delta;
    ConstructVector(&delta, ((VectorSubtractFn)fn_8040CD74)(&m_position,
                                                            reinterpret_cast<Vec3f *>(field_0x64)));
    Vec3f position;
    ConstructVector(&position, *reinterpret_cast<Vec3f *>(field_0x64));
    ColliderList ignore;
    if (args.m_contact != 0 && args.m_contact->IsValid() && !args.m_contact->m_collider.expired()) {
        ignore.push_back(args.m_contact->m_collider.lock());
    }
    m_colliMove->Move(&position, &delta, false, (UnkColliList_80095314 *)&ignore);
    AssignVector(m_position, position);
    AssignVector(*reinterpret_cast<Vec3f *>(field_0x64), position);
}

void AirSaucer::Accelerate(float dt) {
    float gravity;
    float dragAcross;
    float dragAlong;
    float steerAccel;
    if (m_colliMove->IsOnGround()) {
        steerAccel = 270.0f;
        gravity = -1300.0f;
        dragAcross = 0.98f;
        dragAlong = 0.75f;
    } else {
        steerAccel = 220.0f;
        gravity = -320.0f;
        dragAcross = 3.0f;
        dragAlong = 1.0f;
    }
    if (m_flags.Test(0)) {
        fn_8040CFF8(&field_0x24, &(const Vec3f &)fn_800091A4(&m_up, gravity));
    }
    fn_8040CFF8(&field_0x24, &(const Vec3f &)fn_800091A4(&m_steer, steerAccel));
    Vec3f velocityAcross;
    Vec3f velocityAlong;
    fn_8040F990(&velocityAlong, &velocityAcross, &field_0x18, &m_up);
    Vec3f accelAcross;
    Vec3f accelAlong;
    fn_8040F990(&accelAlong, &accelAcross, &field_0x24, &m_up);
    fn_800938CC(&m_position, &velocityAcross, &accelAcross, dragAcross, dt);
    fn_800938CC(&m_position, &velocityAlong, &accelAlong, dragAlong, dt);
    AssignVector(field_0x18, fn_8040CDD0(&velocityAcross, &velocityAlong));
}

void AirSaucer::Hover(float dt) {
    fn_8040CFF8(&m_target, &(const Vec3f &)fn_800091A4(&(const Vec3f &)fn_801B6108(this), dt));
    float drift = kMaxDrift * (1.0f - fn_8040CF50(&field_0x18) / 100.0f);
    if (!IsSmall(m_steer) && drift > m_drift) {
        drift = m_drift;
    }
    m_drift += 0.1f * (drift - m_drift);
    m_drift = fn_80070460(m_drift, 0.0f, kMaxDrift);
    Vec3f target;
    fn_800091EC(&target,
                &(const Vec3f &)fn_8040CDD0(
                    &(const Vec3f &)fn_8040CDD0(&m_position, &Vec3f(0.0f, 5.0f, 0.0f)),
                    &(const Vec3f &)Scale(field_0x18, 0.0078125f)));
    float rate = (1.0f - m_drift) * (60.0f * dt);
    rate = fn_80070460(rate, 0.0f, 1.0f);
    fn_8040CFF8(&m_target, &(const Vec3f &)fn_800091A4(&(const Vec3f &)(target - m_target), rate));
    float height = Dot(&(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&target, &m_target),
                       &Vec3f(0.0f, 1.0f, 0.0f));
    if (height > 2.0f) {
        fn_8040CFF8(&m_target, &(const Vec3f &)fn_800091A4(NewVector(0.0f, 1.0f, 0.0f),
                                                           height - 2.0f));
    } else if (height < -2.0f) {
        fn_8040CFF8(&m_target, &(const Vec3f &)fn_800091A4(NewVector(0.0f, 1.0f, 0.0f),
                                                           2.0f + height));
    }
}

void AirSaucer::Turn(float dt, const Vec3f *steer) {
    if (!IsSmall(*steer)) {
        Vec2f angles = fn_8040F664(&(const Vec3f &)fn_80016284(steer));
        float yaw = angles.y;
        float step = dt * DegToRad(250.0f);
        float rate = step / std::fabs(fn_8040E99C(m_angles.y - yaw));
        rate *= 1.0f - m_drift;
        rate = fn_80070460(rate, 0.0f, 0.1f);
        m_angles.y = fn_8040F314(m_angles.y, yaw, rate);
        m_angles.y = fn_8040E99C(m_angles.y);
        if (!GetRider().expired()) {
            boost::shared_ptr<CharInfo> rider = GetRider().lock();
            if (rider->field_0x5C == 1) {
                Vec3f target;
                fn_800091EC(&target,
                            &(const Vec3f &)fn_8040CDD0(
                                &m_position,
                                &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)fn_8040CEE8(steer),
                                                            300.0f)));
                rider->PushCommand(new Player::SetLookAtPositionCommand(target, 0.05f));
            }
        }
    }
}

void AirSaucer::TiltToTarget(float dt) {
    Vec3f up;
    fn_800091D0(&up, &(const Vec3f &)fn_8040CEE8(&Vec3f(0.0f, 5.0f, 0.0f)));
    Vec3f direction;
    AssignVector(direction, fn_8040CEE8(&(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                &m_target, &m_position)));
    Vec3f axis;
    ConstructVector(&axis, fn_8040CEE8(&(const Vec3f &)fn_8040CFB8(&up, &direction)));
    Quatf tilt;
    float cosine = Dot(&up, &direction);
    if (cosine >= 0.9998f) {
        fn_8007A34C(&tilt, &lbl_805E0790);
    } else {
        fn_8007A34C(&tilt, &(const Quatf &)fn_8040C874(&axis, fn_80015D68(cosine)));
    }
    float rate = 15.0f * dt;
    rate = fn_800704AC(rate, 1.0f);
    fn_8007A34C(&m_targetTilt, &(const Quatf &)fn_8040C3EC(&m_targetTilt, &tilt, rate));
}

/* Not called anywhere: the linker strips it from the game. The original pools the -1.0f of the
 * downward vectors in Tick between the literals of TiltToGround and TiltToTarget, which is what
 * compiling a function that uses it at this point of the file does (inferred from the order of
 * the literals). */
void AirSaucer::GetDown(Vec3f *down) {
    *down = Vec3f(0.0f, -1.0f, 0.0f);
}

void AirSaucer::TiltToGround(float dt) {
    Vec3f up;
    fn_800091D0(&up, &Vec3f(0.0f, 1.0f, 0.0f));
    Vec3f normal;
    if (m_colliMove->IsOnGround()) {
        AssignVector(normal, m_colliMove->m_normal);
    } else {
        fn_800091D0(&normal, &Vec3f(0.0f, 1.0f, 0.0f));
    }
    Vec3f axis;
    ConstructVector(&axis, fn_8040CEE8(&(const Vec3f &)fn_8040CFB8(&up, &normal)));
    Quatf tilt;
    float cosine = Dot(&up, &normal);
    if (cosine >= 0.9998f) {
        fn_8007A34C(&tilt, &lbl_805E0790);
    } else {
        fn_8007A34C(&tilt, &(const Quatf &)fn_8040C874(&axis, fn_80015D68(cosine)));
    }
    float rate = 7.5f * dt;
    rate = fn_800704AC(rate, 1.0f);
    fn_8007A34C(&m_groundTilt, &(const Quatf &)fn_8040C3EC(&m_groundTilt, &tilt, rate));
}

bool AirSaucer::FindPath(float dt) {
    if (m_pathDelay >= 0.0f) {
        m_pathDelay -= dt;
        return false;
    }
    if (fn_803344C8(&m_path, &m_position, &field_0x18)) {
        Vec3f position;
        Vec3f forward;
        Vec3f up;
        fn_80334480(&m_path, &position, &forward, &up);
        Vec3f offset;
        ConstructVector(&offset, ((VectorSubtractFn)fn_8040CD74)(&position, &m_position));
        if (Dot(&up, &offset) >= 0.0f) {
            return false;
        }
        if (Dot(&field_0x18, &up) > 0.01f) {
            return false;
        }
        return !(fn_8040CF2C(&offset) > s_pathRange * s_pathRange);
    }
    return false;
}

void AirSaucer::FollowPath(float dt) {
    bool settled = true;
    Vec3f position;
    Vec3f forward;
    Vec3f up;
    fn_80334480(&m_path, &position, &forward, &up);
    float turn = dt * DegToRad(360.0f);
    AssignVector(m_pathUp, TurnToward(&m_pathUp, &up, turn));
    AssignVector(m_forward, TurnToward(&m_forward, &forward, turn));
    Vec3f side;
    ConstructVector(&side, fn_8040CFB8(&m_forward, &m_pathUp));
    fn_8040CE74(&m_pathUp);
    AssignVector(m_forward, fn_8040CEE8(&(const Vec3f &)fn_8040CFB8(&m_pathUp, &side)));
    fn_8040CFF8(&position, &(const Vec3f &)fn_800091A4(&m_pathUp, s_height));
    AssignVector(m_position, position);
    if (m_boostTime > 0.0f) {
        AssignVector(m_position,
                     fn_800930AC(&position, &position,
                                 &(const Vec3f &)fn_8040CDA8(&m_pathUp, -100.0f),
                                 m_boostTime / 0.5f));
        m_boostTime -= dt;
        if (m_boostTime <= 0.0f) {
            m_boostTime = 0.0f;
        }
        settled = false;
    }
    m_rotation = fn_8040F364(&m_forward, &m_pathUp);
    if (settled) {
        if (!m_flags.Test(2)) {
            m_flags.Set(2);
            if (!GetRider().expired()) {
                fn_8034DF48(10, GetRider().lock()->field_0x54);
            }
        }
    } else {
        m_flags.Clear(2);
    }
}

void AirSaucer::UpdatePathSpeed(float dt) {
    float slope = Dot(&Vec3f(0.0f, 1.0f, 0.0f), &m_forward);
    if (slope >= 0.0f) {
        m_pathSpeed -= dt * (100.0f * slope);
    } else {
        m_pathSpeed -= dt * (40.0f * slope);
    }
    if (m_pathSpeed > 360.0f) {
        m_pathSpeed -= 100.0f * dt;
        m_pathSpeed = m_pathSpeed > 360.0f ? m_pathSpeed : 360.0f;
    }
    m_pathSpeed = m_pathSpeed > 10.0f ? m_pathSpeed : 10.0f;
}

bool AirSaucer::Idle(float dt) {
    if (GetRider().expired() && m_colliMove->IsOnGround()) {
        fn_8040CE38(&field_0x18, 0.7f);
    }
    return false;
}

void AirSaucer::CreateModel() {
    if (m_model != 0) {
        if (m_model != 0) {
            delete m_model;
            m_model = 0;
        }
    }
    m_model = new UnkWeaponModel_80063428((void *)3, &s_effectSet);
    if (field_0x74 != 0) {
        reinterpret_cast<Vehicle::Effect *>(field_0x74)->SetNode((void *)m_model->fn_80063234(0));
    }
}

void AirSaucer::CreateAttack() {
    m_attack = fn_801C08F0(boost::weak_ptr<CharInfo>(GetSelf()),
                           AttackCallback(boost::bind(&AirSaucer::OnAttack, this, _1)),
                           AttackCallback(boost::bind(&AirSaucer::OnAttackEnd, this, _1)));
}

void AirSaucer::Damage(float power, const Vec3f *position, const Vec3f *direction) {
    int pattern = -1;
    if (power >= 1.0f) {
        fn_801D2AFC(0, position, direction, 0x17, 0);
        if (m_flinchTime <= 0.0f) {
            fn_801B6510(this, -std::floor(power));
            m_flinchTime = 2.0f;
            fn_801D54F4(fn_80053164(), 0x2048, this, 0, 1);
            pattern = 4;
        }
    } else if (power > 0.3f) {
        pattern = 1;
    }
    if (pattern != -1 && !GetRider().expired()) {
        fn_8034DF48(pattern, GetRider().lock()->field_0x54);
    }
}

void AirSaucer::OnAttack(AttackCallbackParam &param) {
    float speed = fn_8040CF50(&field_0x18);
    float ratio = 0.0f;
    ratio = speed / 100.0f;
    ratio = fn_800704AC(ratio, kMaxDamage);
    Param(param).m_damage = ratio;
    const Vec3f *normal = Param(param).m_normal;
    if (normal != 0) {
        float facing = -fn_8040CF90(&(const Vec3f &)fn_8040CEE8(&field_0x18), normal);
        if (facing <= 0.0f) {
            Param(param).m_damage = 0.0f;
        } else {
            Param(param).m_damage *= facing;
        }
    }
    Param(param).m_damage = std::floor(Param(param).m_damage);
}

void AirSaucer::OnAttackEnd(AttackCallbackParam &param) {
    float power = 1.0f;
    switch (Param(param).m_result) {
    case 2:
        power = 1.0f;
        break;
    case 1:
        power = 2.0f;
        break;
    }
    if (Param(param).m_normal != 0) {
        Push(&Param(param).m_position, Param(param).m_normal, power);
    }
}

void AirSaucer::Push(const Vec3f *position, const Vec3f *normal, float power) {
    if (std::fabs(Dot(normal, &Vec3f(0.0f, 1.0f, 0.0f))) > 0.087f) {
        return;
    }
    Vec3f across;
    fn_8040F990(0, &across, &(const Vec3f &)fn_8040CEE8(&field_0x18), &Vec3f(0.0f, 1.0f, 0.0f));
    if (fn_8040CF2C(&across) < 0.1f) {
        return;
    }
    float speed = fn_8040CF50(&field_0x18);
    float ratio = 0.0f;
    ratio = speed / 200.0f;
    ratio = fn_800704AC(ratio, kMaxPushRatio);
    power *= ratio;
    float facing = -fn_8040CF90(&across, normal);
    if (facing <= 0.0f) {
        power = 0.0f;
    } else {
        power *= facing;
    }
    Damage(power, position, normal);
}

void AirSaucer::OnHit(AttackCallbackParam &param) {
    Damage(Param(param).m_damage, &Param(param).m_position, &Param(param).m_direction);
    if (fn_801B64F8(this) <= 0.0f) {
        Param(param).m_result = 3;
    } else {
        Param(param).m_result = 1;
    }
}

/* Loading and dropping the saucer's files, creating one, the task record and the vehicle table's
 * entry (it refers to them by address); the static initializer stores their names (s_taskName is
 * defined at the top of the file). */
extern "C" void fn_801AF6C4(void) {
    fn_803A953C("%s::Initialize\n", s_taskName);
    fn_801B4850(7);
    fn_8006B50C(0x16, 0);
    fn_8006B50C(3, 0);
    fn_8006B50C(0x5C, 0);
    fn_8006B50C(0x3F, 0);
    fn_8006B50C(0x3E, 0);
    fn_8006B50C(0x51, 0);
    fn_8006B50C(0x52, 0);
}

extern "C" void fn_801AF694(void) {
    fn_803A953C("%s::Finalize\n", s_taskName);
}

static TVehicleBase *Create(void *arg) {
    return new AirSaucer(arg);
}

GadgetClassDesc lbl_8053B4B0 = {
    s_taskName, fn_801AF6C4, fn_801AF694, 0, 0, 0x80, 0x004C, 4, 0, 0, 0,
};

UnkVehicleEntry lbl_805E8440 = {s_taskName, Create};

bool AirSaucer::vf_0x5C() {
    return true;
}
