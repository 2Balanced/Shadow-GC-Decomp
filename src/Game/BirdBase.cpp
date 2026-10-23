/* Vehicle::BirdBase::BirdBase (class name from RTTI; declared in Game/BirdBase.h): a flying vehicle
 * (the AirBolt and the AirWing derive from it). Once a rider gets on it takes off onto its path
 * (state 1), flies along it at a speed the rider can raise (state 2) and lands when the rider gets
 * off (state 3); state 4 ends the ride. The rider steers the bird around the path or, in free
 * flight (flag bit 10), turns it. Hits slow it down, push it away and hurt it; a collision of its
 * body collider with a character of kind 0x74 (flag bit 9) blocks the steering. Member and function
 * names are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include "Game/BirdBase.h"
#include "Game/BirdModelBase.h"
#include "Game/CharCommand.h"
#include "Game/GadgetCollision.h"
#include "Game/MathConstants.h"
#include "Game/Vibration.h"
#include "Game/VehicleEffect.h"
#include "Effect/System/EffectElement.h"
#include "boost/weak_ptr.hpp"

using Effect::System::EffectElement;
using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;
using Vehicle::BirdBase::UnkBirdCollision_801B4EBC;
using Vehicle::BirdBase::UnkBirdPath_801B446C;
using Vehicle::BirdBase::UnkBirdTrail_801B3148;

/* A node of a collision hit list: the hit's object at 0x14 (see Game/BlockChange.cpp and
 * Game/BlackBullCtrlUnit.cpp). */
struct UnkColliContactNode_800587A0 {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ void *m_object;
};

/* An iterator over a contact list (see Game/BombServer.cpp). */
struct UnkColliContactIter_8001EDFC {
    UnkColliContactIter_8001EDFC(UnkColliContactNode_800587A0 *node) : m_node(node) {}

    UnkColliContactNode_800587A0 *m_node;
};

/* The parts of AttackCallbackParam the bird reads (layout as in Game/CharaColliAttack.h; the
 * members between are named here, meanings inferred; see Game/DevilDoomCtrlUnit.cpp). */
struct BirdAttackParam {
    /* 0x00 */ u8 field_0x0[0x18];
    /* 0x18 */ Vec3f m_position;
    /* 0x24 */ Vec3f m_direction;
    /* 0x30 */ u8 field_0x30[0x50 - 0x30];
    /* 0x50 */ float m_damage;
    /* 0x54 */ s32 m_result;
    /* 0x58 */ const Vec3f *m_normal;
};

#define Param(param) (*(BirdAttackParam *)&(param))

/* The bird's commands (class names unknown; layouts from their use). */
class UnkVectorCommand_BirdBase : public CharCommand {
  public:
    /* 0x14 */ Vec3f m_vector;
};

class UnkDamageCommand_BirdBase : public CharCommand {
  public:
    /* 0x14 */ float m_power;
    /* 0x18 */ Vec3f m_direction;
};

class UnkActionCommand_BirdBase : public CharCommand {
  public:
    /* 0x14 */ s32 m_action;
};

extern "C" {
/* The 32-bit flag word (see Game/BAMine.cpp). */
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* The members at 0x104 and 0x108 (behaviour inferred). The path: whether it is set, the point
 * nearest the bird and its frame (any output may be null), how far the bird follows it, moving it
 * along by a distance (false at its end), and whether the bird reached its start. */
bool fn_801B42E0(UnkBirdPath_801B446C *path);
void fn_801B4304(UnkBirdPath_801B446C *path, Vec3f *position, Vec3f *forward, Vec3f *up);
bool fn_801B4328(UnkBirdPath_801B446C *path, float distance);
void fn_801B434C(UnkBirdPath_801B446C *path, Vehicle::TVehicleBase *vehicle);
bool fn_801B4370(UnkBirdPath_801B446C *path, Vehicle::TVehicleBase *vehicle);
/* The collision: its owner, its collider (set and get), its hit callback, and two resets. */
void fn_801B4DD0(UnkBirdCollision_801B4EBC *collision, Vehicle::TVehicleBase *vehicle);
void fn_801B4DAC(UnkBirdCollision_801B4EBC *collision,
                 const boost::shared_ptr<CharaColliBase> *collider);
boost::shared_ptr<CharaColliBase> fn_801B4D84(UnkBirdCollision_801B4EBC *collision);
void fn_801B4D18(UnkBirdCollision_801B4EBC *collision, const AttackCallback &callback);
void fn_801B4CF4(UnkBirdCollision_801B4EBC *collision);
void fn_801B4D60(UnkBirdCollision_801B4EBC *collision);
/* The object at 0x100 (behaviour unknown). */
UnkBirdTrail_801B3148 *fn_801B3148(UnkBirdTrail_801B3148 *trail, Vehicle::TVehicleBase *vehicle,
                                   UnkBirdPath_801B446C *path);
void fn_801B308C(UnkBirdTrail_801B3148 *trail, int deleteFlag);

/* TVehicleBase's other functions (behaviour inferred): the rider leaves, the vehicle's life and a
 * change of it, whether the rider steers, the end of the ride, and the default command handling. */
void fn_801B6030(Vehicle::TVehicleBase *vehicle, int arg1);
float fn_801B64F8(Vehicle::TVehicleBase *vehicle);
void fn_801B6510(Vehicle::TVehicleBase *vehicle, float delta);
bool fn_801B6684(Vehicle::TVehicleBase *vehicle);
void fn_801B6944(Vehicle::TVehicleBase *vehicle);
void fn_801B6860(Vehicle::TVehicleBase *vehicle, CharCommand *command);
/* Moves start by delta against the world (false when blocked; see Game/Spring.cpp), a collider's
 * update, and a body attack with its two callbacks (behaviour inferred). */
bool fn_801BFC28(Vehicle::TVehicleBase *vehicle, Vec3f *start, Vec3f *delta, u32 *flags, int arg4,
                 float radius);
void fn_801C067C(Vehicle::TVehicleBase *vehicle, const boost::shared_ptr<CharaColliBase> *collider);
boost::shared_ptr<CharaColliAttack> fn_801C08F0(const boost::weak_ptr<CharInfo> &owner,
                                               const AttackCallback &onHit,
                                               const AttackCallback &onEnd);

/* The collider manager (see Game/BlockChange.cpp): the character info a collider belongs to, a
 * sphere collider, whether a hit is an attack target, and an attack on it. */
void *fn_80021B08(void);
boost::weak_ptr<CharInfo> fn_800C4568(void *manager, boost::weak_ptr<CharaColliBase> collider);
/* (The radius is declared before the offset: the game evaluates it first. Floats are passed in
 * their own registers, so the order does not change the call.) */
boost::shared_ptr<CharaColliBase> fn_800C7DEC(void *manager,
                                              const boost::weak_ptr<CharInfo> &charInfo, int flags,
                                              int arg3, int arg4, float radius, const Vec3f *offset);
bool fn_800C3ED4(void *manager, UnkColliContactNode_800587A0 *hit);
void fn_800C40C4(void *manager, boost::weak_ptr<CharaColliAttack> attack,
                 UnkColliContactNode_800587A0 *hit, const Vec3f *normal);
/* The contact list (see Game/BombServer.cpp). */
UnkColliContactNode_800587A0 *fn_8001EE04(UnkColliContacts_8001EE04 *contacts);
UnkColliContactNode_800587A0 *fn_8001EDEC(UnkColliContacts_8001EE04 *contacts);
UnkColliContactIter_8001EDFC fn_80027374(UnkColliContactIter_8001EDFC *it, int);
boost::weak_ptr<CharaColliBase> fn_800587A0(UnkColliContactNode_800587A0 *node);
/* A hit object's normal and position (see Game/DiablonPartsBeam.cpp). */
const Vec3f *fn_800266D4(void *object);
const Vec3f *fn_8001E9BC(void *object);

/* An effect at position (behaviour inferred), sounds and rumble (see Game/GunLift.cpp). */
boost::weak_ptr<EffectElement> fn_801D2AFC(int arg0, const Vec3f *position, const Vec3f *direction,
                                           int id, int arg4);
void *fn_80053164(void);
void fn_801D54F4(void *manager, int id, const void *source, int arg3, int arg4);
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int player);
/* The damage of a hit of the given power (behaviour inferred). */
float fn_801AF9F8(float power);

/* Clamps (see Game/GunLift.cpp), and whether a value is zero. */
const float &fn_80070460(const float &value, const float &low, const float &high);
const float &fn_800704AC(const float &a, const float &b); /* the smaller */
bool fn_8000A950(float value);
/* Vector and matrix helpers (behaviour inferred). */
void fn_8040CE60(Vec3f *vector);                     /* sets the vector to zero */
void fn_8040E970(Vec3f *angles);                     /* sets the angles to zero */
void fn_8040E988(Vec2f *angles);                     /* sets the angles to zero */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* vector += value */
float fn_8040CF90(const Vec3f *a, const Vec3f *b);   /* a . b */
float fn_8040D1B4(const Vec3f *a, const Vec3f *b);   /* distance */
Vec3f fn_8040CEE8(const Vec3f *vector);              /* normalized */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
Vec3f fn_800091A4(const Vec3f *vector, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b); /* a + b */
Vec3f fn_8040D044(const Vec3f *vector, float angle); /* turned about z */
Vec3f fn_8040D118(const Vec3f *vector, float angle); /* turned about x */
Vec3f fn_8040D0AC(const Vec3f *vector, float angle); /* turned about y */
Vec3f fn_8040F364(const Vec3f *forward, const Vec3f *up); /* Euler angles */
Vec3f fn_8040F16C(const Vec3f *a, const Vec3f *b, int order, float t);
void fn_8040F990(Vec3f *along, Vec3f *across, const Vec3f *vector, const Vec3f *normal);
void fn_804103E8(Vec3f *position, Vec3f *velocity, const Vec3f *acceleration, float scale,
                 float dt);
float fn_80015D68(float value); /* arc cosine */
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Vec3f fn_8007E4E8(const Mtx34f *matrix, const Vec3f *point);
Mtx34f fn_8040BEF8(const Mtx34f *matrix);              /* inverse */
Mtx34f fn_8001E044(const Mtx34f *a, const Mtx34f *b); /* product */
}

/* Degrees to radians (out of line in the game). */
float DegToRad(float degrees);

/* fn_8040CD74 called as returning a - b by value (it stores the result through its first
 * argument). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* The vector type's out-of-line assignment (see Game/GadgetUtil.h). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* Uninitialized storage for a vector: the game builds the unit vectors it turns by a matrix in
 * temporaries through the out-of-line vector constructor fn_80008E1C, and turns them through
 * fn_8007E4E8, the out-of-line copy of an inline function that calls fn_8040B67C. A temporary of
 * this type (it has no constructor, and its assignment operator makes it non-POD so it is not
 * zeroed) stands for the constructed temporary. */
struct UnkVectorStorage {
    UnkVectorStorage &operator=(const UnkVectorStorage &other);

    float m_values[3];
};

#define NewVector(x, y, z) \
    fn_80008E1C((Vec3f *)&(const UnkVectorStorage &)UnkVectorStorage(), x, y, z)

/* Sets a matrix to the rotation of Euler angles (name inferred). */
inline void SetRotation(Mtx34f *matrix, float x, float y, float z) {
    fn_8040C148(matrix, &lbl_805E0720);
    fn_8040B848(matrix, y);
    fn_8040B884(matrix, x);
    fn_8040B80C(matrix, z);
}

namespace Vehicle {
namespace BirdBase {

BirdBase::BirdBase(void *arg, const BirdParam *param)
    : TVehicleBase(arg), m_angles(0.0f, 0.0f, 0.0f), m_param(param) {
    m_model = 0;
    m_trail = 0;
    fn_801B4DD0(&m_collision, this);
    fn_8040E970(&m_angles);
    fn_8040CE60(&m_offset);
    fn_8040CE60(&m_offsetVelocity);
    fn_8040CE60(&m_offsetForce);
    m_returnTimer = 0.0f;
    m_flinchTimer = 0.0f;
    m_state = 0;
    m_step = 0;
}

BirdBase::~BirdBase() {
    if (m_trail != 0) {
        fn_801B308C(m_trail, 1);
        m_trail = 0;
    }
}

void BirdBase::OnCommand(CharCommand *command) {
    if (command->Flags() & 2) {
        return;
    }
    switch (command->TypeId()) {
    case 1:
        if (fn_80014E34(&m_flags.m_bits, 3) && !fn_80014E34(&m_flags.m_bits, 9)) {
            fn_8040CFF8(&m_input, &static_cast<UnkVectorCommand_BirdBase *>(command)->m_vector);
            fn_80014E1C(&m_flags.m_bits, 1);
            fn_8006127C(&m_flags.m_bits, 7);
            command->Flags() |= 2;
        }
        break;
    case 4:
        fn_8040CE60(&m_offsetVelocity);
        command->Flags() |= 2;
        break;
    case 5:
        AssignVector(m_offsetVelocity,
                     fn_8040B67C(&(const Mtx34f &)GetPathMatrix(),
                                 &static_cast<UnkVectorCommand_BirdBase *>(command)->m_vector));
        command->Flags() |= 2;
        break;
    case 7: {
        UnkDamageCommand_BirdBase *damage = static_cast<UnkDamageCommand_BirdBase *>(command);
        Damage(damage->m_power, &m_position, &damage->m_direction);
        command->Flags() |= 2;
        break;
    }
    case 0x15:
        switch (static_cast<UnkActionCommand_BirdBase *>(command)->m_action) {
        case 1:
            fn_801B6030(this, 0);
            command->Flags() |= 2;
            break;
        case 2:
            fn_80014E1C(&m_flags.m_bits, 2);
            command->Flags() |= 2;
            break;
        case 3:
            fn_8006127C(&m_flags.m_bits, 2);
            command->Flags() |= 2;
            break;
        case 4:
            fn_801B6030(this, 0);
            command->Flags() |= 2;
            break;
        }
        break;
    }
    if (!(command->Flags() & 2)) {
        fn_801B6860(this, command);
    }
}

void BirdBase::vf_0x40(UnkColliContactNode_800587A0 *const &hit) {
    void *object;
    void *manager = fn_80021B08();
    object = hit->m_object;
    Vec3f normal;
    fn_800091EC(&normal, fn_800266D4(object));
    Vec3f position;
    fn_800091EC(&position, fn_8001E9BC(object));
    if (m_attack.get() != 0 && fn_800C3ED4(manager, hit)) {
        fn_800C40C4(manager, boost::weak_ptr<CharaColliAttack>(m_attack), hit, &normal);
    } else {
        Push(&position, &normal, m_param->m_damage);
    }
}

void BirdBase::SetPlacement(const Vec3f *position, const Vec3f *rotation) {
    TVehicleBase::SetPlacement(position, rotation);
    m_model->SetPlacement(position, rotation);
    AssignVector(m_prevPosition, m_position);
}

void BirdBase::Tick(float dt) {
    if (m_flinchTimer > 0.0f) {
        m_model->RequestPause();
        m_flinchTimer -= dt;
        if (m_flinchTimer <= 0.0f) {
            m_flinchTimer = 0.0f;
            m_model->Stop();
        }
    }
    switch (m_state) {
    case 0:
        fn_8006127C(&m_flags.m_bits, 7);
        break;
    case 1:
        UpdateLaunch(dt);
        break;
    case 2:
        UpdateFlight(dt);
        break;
    case 3:
        UpdateLanding(dt);
        break;
    case 4:
        switch (m_step) {
        case 0:
            if (field_0x74 != 0) {
                reinterpret_cast<Vehicle::Effect *>(field_0x74)->StartBurst(m_model->GetFrame(0));
            }
            fn_8006127C(&m_flags.m_bits, 0);
            fn_8006127C(&m_flags.m_bits, 5);
            m_step++;
            break;
        case 1:
            fn_801B6944(this);
            break;
        }
        break;
    }
    CheckContacts();
    if (fn_80014E34(&m_flags.m_bits, 5)) {
        UpdateDrift(dt);
    }
    UpdateRotation(dt);
    if (fn_80014E34(&m_flags.m_bits, 10)) {
        UpdatePathMove(dt);
    } else {
        UpdateMove(dt);
    }
    if (fn_80014E34(&m_flags.m_bits, 0)) {
        Vec3f position;
        fn_800091EC(&position, &m_prevPosition);
        Vec3f delta;
        fn_800091EC(&delta, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&m_position,
                                                                           &m_prevPosition));
        if (fn_801BFC28(this, &position, &delta, &m_moveFlags.m_bits, 0, m_param->m_radius)) {
            fn_800091D0(&m_position, &position);
            if (!fn_8000A950(dt)) {
                AssignVector(field_0x18, fn_8040CDA8(&delta, 1.0f / dt));
            }
            if (fn_80014E34(&m_flags.m_bits, 10)) {
                AlignToPath();
            }
            if (fn_80014E34(&m_flags.m_bits, 8)) {
                m_state = 4;
                m_step = 0;
            }
        } else {
            fn_800091D0(&m_position, &position);
        }
    }
    m_model->SetPlacement(&m_position, &m_rotation);
    m_model->Update(dt);
    fn_801C067C(this, &fn_801B4D84(&m_collision));
    fn_8040CE60(&m_offsetForce);
    fn_8040CE60(&field_0x24);
    fn_8040CE60(&m_input);
    AssignVector(m_prevPosition, m_position);
    fn_8006127C(&m_flags.m_bits, 1);
    fn_80014E1C(&m_flags.m_bits, 7);
}

void BirdBase::OnGetOn() {
    TVehicleBase::OnGetOn();
    m_state = 1;
    m_step = 0;
    fn_80014E1C(&m_flags.m_bits, 6);
    fn_80014E1C(&m_flags.m_bits, 0);
    void *manager = fn_80021B08();
    boost::shared_ptr<CharaColliBase> collider;
    collider = fn_800C7DEC(manager, boost::weak_ptr<CharInfo>(GetSelf()), 0, 1, 0,
                           m_param->m_bodyRadius, &lbl_805E07A0);
    fn_801B4DAC(&m_collision, &collider);
    fn_801B4D18(&m_collision, AttackCallback(boost::bind(&BirdBase::OnHit, this, _1)));
}

void BirdBase::OnGetOff() {
    TVehicleBase::OnGetOff();
    fn_801B4CF4(&m_collision);
    fn_801B4D60(&m_collision);
    fn_8006127C(&m_flags.m_bits, 10);
    fn_8006127C(&m_flags.m_bits, 3);
    fn_8006127C(&m_flags.m_bits, 4);
    fn_8006127C(&m_flags.m_bits, 6);
    if (m_state == 1 || m_state == 2) {
        m_state = 3;
        m_step = 0;
    }
    if (m_trail != 0) {
        fn_801B308C(m_trail, 1);
        m_trail = 0;
    }
}

bool BirdBase::vf_0x54() {
    return true;
}

bool BirdBase::vf_0x60() {
    return true;
}

float BirdBase::vf_0x64() {
    return m_model->GetMotionTime();
}

void BirdBase::vf_0x7C(void *arg) {
    m_model->SetMotionArg(arg);
}

void BirdBase::UpdateLaunch(float dt) {
    fn_80014E1C(&m_flags.m_bits, 7);
    if (fn_801B42E0(&m_path) && m_trail == 0) {
        UnkBirdTrail_801B3148 *trail = (UnkBirdTrail_801B3148 *)operator new(4);
        if (trail != 0) {
            trail = fn_801B3148(trail, this, &m_path);
        }
        m_trail = trail;
    }
    switch (m_step) {
    case 0:
        fn_8006127C(&m_flags.m_bits, 8);
        m_returnPower = m_param->m_minSpeed;
        if (fn_801B4370(&m_path, this)) {
            m_model->SetState(1);
            fn_80014E1C(&m_flags.m_bits, 5);
            m_step++;
        }
        break;
    case 1: {
        fn_801B434C(&m_path, this);
        Vec3f position;
        fn_801B4304(&m_path, &position, 0, 0);
        if (fn_8040D1B4(&m_position, &position) < m_param->m_pathRange && m_trail != 0) {
            m_step++;
        }
        break;
    }
    case 2:
        AlignToPath();
        fn_80014E1C(&m_flags.m_bits, 3);
        fn_80014E1C(&m_flags.m_bits, 4);
        fn_8006127C(&m_flags.m_bits, 5);
        m_state = 2;
        m_step = 0;
        break;
    }
}

void BirdBase::UpdateFlight(float dt) {
    if (fn_80014E34(&m_flags.m_bits, 2)) {
        m_speed += dt * (m_param->m_maxSpeed - m_param->m_minSpeed) / m_param->m_accelTime;
    } else {
        m_speed += dt * (m_param->m_minSpeed - m_param->m_maxSpeed) / m_param->m_decelTime;
    }
    m_speed = fn_80070460(m_speed, m_param->m_minSpeed, m_param->m_maxSpeed);
    m_returnPower = m_param->m_returnPower;
    if (fn_80014E34(&m_flags.m_bits, 1) || fn_80014E34(&m_flags.m_bits, 2)) {
        fn_8006127C(&m_flags.m_bits, 5);
        m_returnTimer = 0.0f;
    } else if (m_param->m_returnTime >= 0.0f) {
        Vec3f position;
        fn_801B4304(&m_path, &position, 0, 0);
        if (fn_8040D1B4(&m_position, &position) < m_param->m_pathRange) {
            fn_8006127C(&m_flags.m_bits, 5);
        } else {
            m_returnTimer += dt;
            if (m_returnTimer >= m_param->m_returnTime) {
                m_returnTimer = m_param->m_returnTime;
                fn_80014E1C(&m_flags.m_bits, 5);
            }
        }
    }
    if (fn_801B64F8(this) <= 0.0f) {
        fn_801D54F4(fn_80053164(), 0x205B, this, 0, 1);
        fn_801B6030(this, 0);
        m_state = 4;
        m_step = 0;
    }
}

void BirdBase::UpdateLanding(float dt) {
    switch (m_step) {
    case 0:
        fn_8006127C(&m_flags.m_bits, 10);
        fn_80014E1C(&m_flags.m_bits, 8);
        fn_8006127C(&m_flags.m_bits, 5);
        m_step++;
        break;
    case 1:
        break;
    }
    m_speed = m_param->m_minSpeed;
}

void BirdBase::CreateAttack() {
    m_attack = fn_801C08F0(boost::weak_ptr<CharInfo>(GetSelf()),
                           AttackCallback(boost::bind(&BirdBase::OnAttackHit, this, _1)),
                           AttackCallback(boost::bind(&BirdBase::OnAttackEnd, this, _1)));
}

void BirdBase::Release() {}

void BirdBase::Damage(float power, const Vec3f *position, const Vec3f *direction) {
    int pattern = -1;
    if (power >= 1.0f) {
        fn_801D2AFC(0, position, direction, 0x17, 0);
        if (m_flinchTimer <= 0.0f) {
            m_model->SetState(3);
            fn_801B6510(this, -fn_801AF9F8(power));
            m_flinchTimer = m_param->m_flinchTime;
            fn_801D54F4(fn_80053164(), 0x205A, this, 0, 1);
            pattern = 4;
        }
    } else if (power >= 0.3f) {
        pattern = 1;
    }
    if (pattern != -1 && !GetRider().expired()) {
        fn_8034DF48(pattern, GetRider().lock()->field_0x54);
    }
}

void BirdBase::OnAttackHit(AttackCallbackParam &param) {
    float speed = fn_8040CF50(&field_0x18);
    float ratio = 0.0f;
    ratio = speed / m_param->m_hitSpeed;
    ratio = fn_800704AC(ratio, m_param->m_maxHitRatio);
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
    Param(param).m_damage = fn_801AF9F8(Param(param).m_damage);
}

void BirdBase::OnAttackEnd(AttackCallbackParam &param) {
    float power = 1.0f;
    switch (Param(param).m_result) {
    case 2:
        /* (Redundant; retail keeps the branch it leaves.) */
        power = 1.0f;
        break;
    case 1:
        power = m_param->m_damage;
        break;
    }
    if (Param(param).m_normal != 0) {
        Push(&Param(param).m_position, Param(param).m_normal, power);
    }
}

void BirdBase::Push(const Vec3f *position, const Vec3f *normal, float power) {
    Vec3f along;
    fn_8040F990(&along, 0, &(const Vec3f &)fn_8040CEE8(&field_0x18), normal);
    if (fn_8040CF2C(&along) < 0.1f) {
        return;
    }
    float speed = fn_8040CF50(&field_0x18);
    float ratio = 0.0f;
    ratio = speed / m_param->m_pushSpeed;
    /* Retail keeps the clamped ratio in a register as well as storing it back. */
    float scale = fn_800704AC(ratio, m_param->m_maxPushRatio);
    ratio = scale;
    float facing = -fn_8040CF90(&along, normal);
    float damage = scale * power;
    if (facing <= 0.0f) {
        damage = 0.0f;
    } else {
        damage *= facing;
    }
    Damage(damage, position, normal);
}

void BirdBase::OnHit(AttackCallbackParam &param) {
    Damage(Param(param).m_damage, &Param(param).m_position, &Param(param).m_direction);
    if (fn_801B64F8(this) <= 0.0f) {
        Param(param).m_result = 3;
    } else {
        Param(param).m_result = 1;
    }
}

void BirdBase::UpdateMove(float dt) {
    Vec3f forward(0.0f, 0.0f, -1.0f);
    AssignVector(forward, fn_8040D044(&forward, m_rotation.z));
    AssignVector(forward, fn_8040D118(&forward, m_rotation.x));
    AssignVector(forward, fn_8040D0AC(&forward, m_rotation.y));
    fn_8040CFF8(&field_0x24, &(const Vec3f &)fn_800091A4(&forward, m_speed));
    fn_804103E8(&m_position, &field_0x18, &field_0x24, m_param->m_drag, dt);
    if (fn_80014E34(&m_flags.m_bits, 4)) {
        ClampToPath();
    }
}

void BirdBase::UpdatePathMove(float dt) {
    Vec3f forward(0.0f, 0.0f, -1.0f);
    AssignVector(forward, fn_8040D044(&forward, m_angles.z));
    AssignVector(forward, fn_8040D118(&forward, m_angles.x));
    AssignVector(forward, fn_8040D0AC(&forward, m_angles.y));
    fn_8040CFF8(&m_offsetForce, &(const Vec3f &)fn_800091A4(&forward, m_speed));
    if (fn_801B42E0(&m_path)) {
        float speed = -1.0f;
        if (fn_801B6684(this)) {
            speed = fn_8040CF50(&m_offsetVelocity);
        }
        fn_804103E8(&m_offset, &m_offsetVelocity, &m_offsetForce, m_param->m_drag, dt);
        if (speed >= 0.0f) {
            AssignVector(m_offsetVelocity,
                         fn_8040CDA8(&(const Vec3f &)fn_8040CEE8(&m_offsetVelocity), speed));
        }
        if (fn_80014E34(&m_flags.m_bits, 4)) {
            if (m_offset.x < -m_param->m_maxOffset) {
                m_offset.x = -m_param->m_maxOffset;
            } else if (m_offset.x > m_param->m_maxOffset) {
                m_offset.x = m_param->m_maxOffset;
            }
            if (m_offset.y < -m_param->m_maxOffset) {
                m_offset.y = -m_param->m_maxOffset;
            } else if (m_offset.y > m_param->m_maxOffset) {
                m_offset.y = m_param->m_maxOffset;
            }
        }
        bool moved = fn_801B4328(&m_path, -m_offset.z * m_param->m_pathScale);
        Vec3f position;
        Vec3f pathForward;
        Vec3f pathUp;
        fn_801B4304(&m_path, &position, &pathForward, &pathUp);
        Vec3f angles = fn_8040F364(&pathForward, &pathUp);
        float x = angles.x;
        float y = angles.y;
        Vec3f offset(m_offset.x, m_offset.y, 0.0f);
        AssignVector(offset, fn_8040D044(&offset, angles.z));
        AssignVector(offset, fn_8040D118(&offset, x));
        AssignVector(offset, fn_8040D0AC(&offset, y));
        m_offset.z = 0.0f;
        AssignVector(m_position, fn_8040CDD0(&position, &offset));
        if (!moved) {
            fn_801B6030(this, 0);
        } else if (!fn_8000A950(dt)) {
            AssignVector(field_0x18, fn_8040CDA8(&(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                                     &m_position, &m_prevPosition),
                                                 1.0f / dt));
        }
    }
}

Mtx34f BirdBase::GetPathMatrix() {
    Mtx34f matrix;
    fn_801B434C(&m_path, this);
    Vec3f position;
    Vec3f forward;
    Vec3f up;
    fn_801B4304(&m_path, &position, &forward, &up);
    Vec3f angles = fn_8040F364(&forward, &up);
    SetRotation(&matrix, angles.x, angles.y, angles.z);
    return fn_8040BEF8(&matrix);
}

void BirdBase::AlignToPath() {
    fn_80014E1C(&m_flags.m_bits, 10);
    Mtx34f matrix;
    fn_8040C148(&matrix, &(const Mtx34f &)GetPathMatrix());
    Vec3f position;
    fn_801B4304(&m_path, &position, 0, 0);
    AssignVector(m_offset, ((VectorSubtractFn)fn_8040CD74)(&m_position, &position));
    AssignVector(m_offset, fn_8040B67C(&matrix, &m_offset));
    AssignVector(m_offsetVelocity, fn_8040B67C(&matrix, &field_0x18));
    fn_8040B848(&matrix, m_rotation.y);
    fn_8040B884(&matrix, m_rotation.x);
    fn_8040B80C(&matrix, m_rotation.z);
    m_angles = fn_8040F364(&(const Vec3f &)fn_8007E4E8(&matrix, NewVector(0.0f, 0.0f, -1.0f)),
                           &(const Vec3f &)fn_8007E4E8(&matrix, NewVector(0.0f, 1.0f, 0.0f)));
}

void BirdBase::ClampToPath() {
    Vec3f position;
    fn_801B4304(&m_path, &position, 0, 0);
    Vec3f offset;
    AssignVector(offset, ((VectorSubtractFn)fn_8040CD74)(&m_position, &position));
    float length = fn_8040CF50(&offset);
    float maxOffset = m_param->m_maxOffset;
    if (length > maxOffset) {
        AssignVector(m_position,
                     fn_8040CDD0(&(const Vec3f &)fn_8040CDA8(&(const Vec3f &)fn_8040CEE8(&offset),
                                                             maxOffset),
                                 &position));
    }
}

/* The volatile reads of m_returnPower below: retail reads the member before the normalization call
 * and keeps it in f31, while GC/2.7 moves a plain read below the call (into f1); see
 * docs/research-notes.md for the other differences between the game's compiler and GC/2.7. The
 * volatile read stops the move. It reproduces retail's output; it says nothing about how the
 * original was written. */
void BirdBase::UpdateDrift(float dt) {
    if (fn_80014E34(&m_flags.m_bits, 10)) {
        Vec3f force(-m_offset.x, -m_offset.y, 0.0f);
        float power = *(const volatile float *)&m_returnPower;
        AssignVector(force, fn_8040CDA8(&(const Vec3f &)fn_8040CEE8(&force), power));
        fn_8040CFF8(&m_offsetForce, &force);
        float lengthSq = fn_8040CF2C(&m_offsetForce);
        float maxForce = m_param->m_minSpeed;
        if (lengthSq > maxForce * maxForce) {
            AssignVector(m_offsetForce,
                         fn_8040CDA8(&(const Vec3f &)fn_8040CEE8(&m_offsetForce), maxForce));
        }
    } else {
        fn_801B434C(&m_path, this);
        Vec3f position;
        fn_801B4304(&m_path, &position, 0, 0);
        Vec3f force;
        AssignVector(force, ((VectorSubtractFn)fn_8040CD74)(&position, &m_position));
        float power = *(const volatile float *)&m_returnPower;
        AssignVector(force, fn_8040CDA8(&(const Vec3f &)fn_8040CEE8(&force), power));
        fn_8040CFF8(&field_0x24, &force);
        float lengthSq = fn_8040CF2C(&field_0x24);
        float maxForce = m_param->m_minSpeed;
        if (lengthSq > maxForce * maxForce) {
            AssignVector(field_0x24,
                         fn_8040CDA8(&(const Vec3f &)fn_8040CEE8(&field_0x24), maxForce));
        }
    }
}

void BirdBase::UpdateRotation(float dt) {
    if (fn_80014E34(&m_flags.m_bits, 10)) {
        Mtx34f matrix;
        Vec3f forward;
        Vec3f up;
        fn_801B4304(&m_path, 0, &forward, &up);
        Vec3f pathAngles = fn_8040F364(&forward, &up);
        SetRotation(&matrix, pathAngles.x, pathAngles.y, pathAngles.z);
        if (fn_80014E34(&m_flags.m_bits, 1)) {
            if (!fn_8000A950(m_input.x)) {
                m_angles.y += dt * (-m_input.x * m_param->m_yawSpeed);
            }
            if (!fn_8000A950(m_input.z)) {
                m_angles.x += dt * (m_input.z * m_param->m_pitchSpeed);
            }
        }
        if (fn_80014E34(&m_flags.m_bits, 7)) {
            Vec3f level(0.0f, 0.0f, 0.0f);
            fn_8040E970(&level);
            Vec3f heading(0.0f, 0.0f, -1.0f);
            AssignVector(heading, fn_8040D118(&heading, m_angles.x));
            AssignVector(heading, fn_8040D0AC(&heading, m_angles.y));
            float dot = fn_8040CF90(&heading, &(const Vec3f &)Vec3f(0.0f, 0.0f, -1.0f));
            float angle = fn_80015D68(dot);
            float t = m_param->m_levelSpeed * (lbl_805FAD38 / 180.0f) / angle;
            if (t > 0.5f) {
                t = 0.5f;
            }
            m_angles = fn_8040F16C(&m_angles, &level, 2, t);
        }
        if (fn_80014E34(&m_flags.m_bits, 6)) {
            float maxYaw = DegToRad(m_param->m_maxYaw);
            float maxPitch = DegToRad(m_param->m_maxPitch);
            Vec2f over(0.0f, 0.0f);
            fn_8040E988(&over);
            if (m_angles.x > maxPitch) {
                m_angles.x = maxPitch;
            } else if (m_angles.x < -maxPitch) {
                m_angles.x = -maxPitch;
            }
            if (m_angles.y > maxYaw) {
                m_angles.y = maxYaw;
            } else if (m_angles.y < -maxYaw) {
                m_angles.y = -maxYaw;
            }
        }
        fn_8040B848(&matrix, m_angles.y);
        fn_8040B884(&matrix, m_angles.x);
        fn_8040B80C(&matrix, m_angles.z);
        m_rotation = fn_8040F364(&(const Vec3f &)fn_8007E4E8(&matrix, NewVector(0.0f, 0.0f, -1.0f)),
                                 &(const Vec3f &)fn_8007E4E8(&matrix, NewVector(0.0f, 1.0f, 0.0f)));
    } else if (fn_801B42E0(&m_path)) {
        Mtx34f pathMatrix;
        Mtx34f matrix;
        Mtx34f world;
        Vec3f forward;
        Vec3f up;
        fn_801B4304(&m_path, 0, &forward, &up);
        Vec3f pathAngles = fn_8040F364(&forward, &up);
        SetRotation(&pathMatrix, pathAngles.x, pathAngles.y, pathAngles.z);
        fn_8040C148(&matrix, &(const Mtx34f &)fn_8040BEF8(&pathMatrix));
        fn_8040B848(&matrix, m_rotation.y);
        fn_8040B884(&matrix, m_rotation.x);
        fn_8040B80C(&matrix, m_rotation.z);
        Vec3f angles(0.0f, 0.0f, 0.0f);
        angles = fn_8040F364(&(const Vec3f &)fn_8007E4E8(&matrix, NewVector(0.0f, 0.0f, -1.0f)),
                             &(const Vec3f &)fn_8007E4E8(&matrix, NewVector(0.0f, 1.0f, 0.0f)));
        if (fn_80014E34(&m_flags.m_bits, 1)) {
            Vec2f turn(0.0f, 0.0f);
            fn_8040E988(&turn);
            if (!fn_8000A950(m_input.x)) {
                turn.y = dt * (-m_input.x * m_param->m_yawSpeed);
            }
            if (!fn_8000A950(m_input.z)) {
                turn.x = dt * (m_input.z * m_param->m_pitchSpeed);
            }
            if (0.0f != turn.x) {
                fn_8040B884(&matrix, turn.x);
            }
            if (0.0f != turn.y) {
                fn_8040B848(&matrix, turn.y);
            }
            angles = fn_8040F364(&(const Vec3f &)fn_8007E4E8(&matrix, NewVector(0.0f, 0.0f, -1.0f)),
                                 &(const Vec3f &)fn_8007E4E8(&matrix, NewVector(0.0f, 1.0f, 0.0f)));
        }
        if (fn_80014E34(&m_flags.m_bits, 7)) {
            Vec3f level(0.0f, 0.0f, 0.0f);
            fn_8040E970(&level);
            Vec3f heading;
            fn_800091EC(&heading, &(const Vec3f &)fn_8040B67C(
                                      &matrix, &(const Vec3f &)Vec3f(0.0f, 0.0f, -1.0f)));
            float dot = fn_8040CF90(&heading, &(const Vec3f &)Vec3f(0.0f, 0.0f, -1.0f));
            float angle = fn_80015D68(dot);
            float t = m_param->m_levelSpeed * (lbl_805FAD38 / 180.0f) / angle;
            if (t > 0.5f) {
                t = 0.5f;
            }
            angles = fn_8040F16C(&angles, &level, 2, t);
            fn_8040B6F8(&matrix);
            fn_8040B848(&matrix, angles.y);
            fn_8040B884(&matrix, angles.x);
            fn_8040B80C(&matrix, angles.z);
        }
        if (fn_80014E34(&m_flags.m_bits, 6)) {
            float maxYaw = DegToRad(m_param->m_maxYaw);
            float maxPitch = DegToRad(m_param->m_maxPitch);
            Vec2f over(0.0f, 0.0f);
            fn_8040E988(&over);
            if (angles.x > maxPitch) {
                over.x = -(angles.x - maxPitch);
            } else if (angles.x < -maxPitch) {
                over.x = -(angles.x + maxPitch);
            }
            if (angles.y > maxYaw) {
                over.y = -(angles.y - maxYaw);
            } else if (angles.y < -maxYaw) {
                over.y = -(angles.y + maxYaw);
            }
            if (0.0f != over.x) {
                fn_8040B884(&matrix, over.x);
            }
            if (0.0f != over.y) {
                fn_8040B848(&matrix, over.y);
            }
        }
        fn_8040C148(&world, &(const Mtx34f &)fn_8001E044(&pathMatrix, &matrix));
        m_rotation = fn_8040F364(&(const Vec3f &)fn_8007E4E8(&world, NewVector(0.0f, 0.0f, -1.0f)),
                                 &(const Vec3f &)fn_8007E4E8(&world, NewVector(0.0f, 1.0f, 0.0f)));
    }
}

void BirdBase::CheckContacts() {
    UnkColliContacts_8001EE04 *contacts;
    void *manager = fn_80021B08();
    boost::shared_ptr<CharaColliBase> collider = fn_801B4D84(&m_collision);
    fn_8006127C(&m_flags.m_bits, 9);
    if (collider.get() != 0) {
        contacts = reinterpret_cast<UnkColliContacts_8001EE04 *>(
            reinterpret_cast<u8 *>(collider.get()) + 0xC);
        if (contacts != 0) {
            for (UnkColliContactIter_8001EDFC it(fn_8001EE04(contacts));
                 it.m_node != fn_8001EDEC(contacts); fn_80027374(&it, 0)) {
                boost::weak_ptr<CharaColliBase> hit = fn_800587A0(it.m_node);
                boost::shared_ptr<CharInfo> owner = fn_800C4568(manager, hit).lock();
                if (owner.get() != 0) {
                    switch (owner->GetKind()) {
                    case 0x74:
                        fn_80014E1C(&m_flags.m_bits, 9);
                        break;
                    }
                }
            }
        }
    }
}

} // namespace BirdBase
} // namespace Vehicle
