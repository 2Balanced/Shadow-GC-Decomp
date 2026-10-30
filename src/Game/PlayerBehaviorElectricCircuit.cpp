/* Player::Behavior::ElectricCircuit (class name from RTTI; its name is "ElectricCircuit"): behavior
 * 0x21, riding an electric circuit. Entering it takes the circuit target the command found, then
 * each frame moves the player along the circuit, speeding up towards a top speed, until the circuit
 * ends; the player then jumps off in the direction it was moving. The unit also holds the base
 * class Player::Behavior::Behavior's own functions and vtable, the circuit target's helpers that
 * other behaviors call by their addresses (fn_8007646C, fn_8007684C), and the player's state-bit
 * setters (fn_80076CA8, fn_80076CC8). File name inferred from the class (as
 * Game/PlayerBehaviorAirAttack.cpp); all other names are inferred. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
#define PLAYER_BEHAVIOR_SELF
#define PLAYER_BEHAVIOR_ARGS_COMMANDS
#define PLAYER_BEHAVIOR_ARGS_JUMP_INLINE
#include "Game/CharCommand.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/UnkCircuitTarget_800ABCE8.h"
#include "Game/Vibration.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The sound manager (see Game/SecretDoor.cpp). */
class UnkSoundManager_80053164;

/* The player's members the behavior reads (names inferred). */
struct UnkPlayerFields_80076390 {
    /* 0x000 */ Vec3f m_position;
    /* 0x00C */ Vec3f m_rotation;
    /* 0x018 */ Vec3f m_velocity;
    /* 0x024 */ u8 field_0x24[0xD0 - 0x24];
    /* 0x0D0 */ Vec3f m_prevPosition; /* where the player was before this frame (inferred) */
};

extern "C" {
/* Tests a bit of a bit array (Game/NpcSonicMotion.cpp). */
bool fn_8006CD78(const u32 *bits, int bit);
/* The circuit target (see Game/PlayerBehaviorChaosControl.cpp). */
void fn_800ABAE8(UnkCircuitTarget_800ABCE8 *target, Vec3f *position, Vec3f *forward, Vec3f *up);
bool fn_800ABB14(UnkCircuitTarget_800ABCE8 *target, float distance);
void fn_800ABAA4(UnkCircuitTarget_800ABCE8 *target, u8 on);
/* Switches a player's behavior (behaviour inferred). */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);
/* The player's up axis, and two player helpers (behaviour inferred). */
Vec3f fn_80079698(Player::PlayerBase *player);
void fn_800B7D48(boost::shared_ptr<Player::PlayerBase> *player, int arg1);
void fn_80079028(Player::PlayerBase *player, float arg1);
/* Called with the player's object at 0x23C when the behavior starts and ends (behaviour
 * unknown). */
void fn_800A2AD8(void *object);
void fn_800A2A78(void *object);
/* Rumbles the controller (see Game/VibrationRequest.cpp). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int pad);
/* The sound manager, and playing a sound and a sound from a source (see Game/SecretDoor.cpp). */
UnkSoundManager_80053164 *fn_80053164(void);
void fn_801D560C(UnkSoundManager_80053164 *manager, int id, int arg2, int arg3);
void fn_801D53C4(UnkSoundManager_80053164 *manager, int id, int arg2, void *source, int arg4,
                 int arg5, int arg6);
/* Whether a value is about zero (see Game/ArkCannon.cpp). */
bool fn_8000A950(float value);

Vec3f fn_8040CDA8(const Vec3f *vector, float scale);  /* vector * scale */
Vec3f fn_8040CEE8(const Vec3f *vector);               /* the normalized vector */
float fn_8040CF50(const Vec3f *v);                    /* |v| */
void fn_8040CE38(Vec3f *vector, float scale);         /* vector *= scale */
Vec3f fn_8040F364(const Vec3f *forward, const Vec3f *up); /* Euler angles */
}

/* fn_8040CD74 (lhs - rhs, Game/MathTypes.h) called as returning the difference. */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *lhs, const Vec3f *rhs);

/* pi (see Game/BkSoldier.cpp). */
extern const float lbl_805FAD38;

/* Sets and clears a bit of a bit array (the player's state bits; other units call them by their
 * addresses). */
/* The word and the bit of a bit array (inline calls, so that the setters below are not inlined
 * themselves: deferred inlining is one level deep). */
inline u32 &BitWord(u32 *bits, int bit) {
    return bits[(u32)bit >> 5];
}

extern "C" void fn_80076CC8(u32 *bits, int bit) {
    BitWord(bits, bit) &= ~(1 << (bit & 0x1F));
}

extern "C" void fn_80076CA8(u32 *bits, int bit) {
    BitWord(bits, bit) |= 1 << (bit & 0x1F);
}

/* The player's members above (a macro, as in Game/PlayerBehaviorChaosControl.cpp). */
#define FieldsOf(player) reinterpret_cast<UnkPlayerFields_80076390 *>(player)

/* Copies a rotation one angle at a time (as in Game/CharCommands.cpp). */
inline void CopyRotation(Vec3f &rotation, const Vec3f &value) {
    rotation.x = value.x;
    rotation.y = value.y;
    rotation.z = value.z;
}

/* vector *= scale (the arguments the other way round from Game/PlayerBehaviorGround.cpp's:
 * evaluated right to left, the vector comes first; name inferred). */
inline void ScaleVector(float scale, Vec3f &vector) {
    fn_8040CE38(&vector, scale);
}

/* Degrees to radians (name inferred). */
inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

namespace Player {
namespace Behavior {

class ElectricCircuit : public Behavior {
  public:
    ElectricCircuit(const boost::shared_ptr<PlayerBase> &player);
    virtual ~ElectricCircuit();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x18(CharCommand *command);
    virtual void Update(float dt);
    virtual void vf_0x20();

    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);
    static void Release();

    /* Moves along the circuit; false once it has ended. */
    bool Advance(float dt);
    /* Puts the player where the circuit target is. */
    void MoveAlong(float dt);

    static const char *s_name;

    /* 0x1C */ Vec3f m_up;
    /* 0x28 */ Vec3f m_forward;
    /* 0x34 */ UnkCircuitTarget_800ABCE8 m_target;
    /* 0x3C */ float m_speed;
    /* 0x40 */ s32 m_state;
};
SIZE_ASSERT(ElectricCircuit, 0x44);

const char *ElectricCircuit::s_name = "ElectricCircuit";

/* The speed the ride eases towards, and the speed it jumps off at (names inferred; set in __sinit,
 * see Game/PlayerBehaviorChaosControl.cpp). */
inline float DynamicFloat(float value) {
    return value;
}
static float s_maxSpeed = DynamicFloat(800.0f);
static float s_jumpSpeed = DynamicFloat(250.0f);

inline Behavior::Behavior(const boost::shared_ptr<PlayerBase> &player, const char *name, int id)
    : m_player(player), m_name(name), m_id(id) {}

void Behavior::vf_0x20() {}

ElectricCircuit::ElectricCircuit(const boost::shared_ptr<PlayerBase> &player)
    : Behavior(player, s_name, 0x21) {}

inline Behavior::~Behavior() {}

ElectricCircuit::~ElectricCircuit() {}

bool ElectricCircuit::vf_0x0C(int prevId, const InitializeArgs &args) {
    return true;
}

void ElectricCircuit::Enter(int prevId, const InitializeArgs &args) {
    m_player.get()->vf_0x48();
    m_target = static_cast<const InitializeArgsElectricCircuit &>(args).m_target;
    fn_800ABAA4(&m_target, static_cast<const InitializeArgsElectricCircuit &>(args).field_0x10);
    m_speed = fn_8040CF50(&FieldsOf(m_player.get())->m_velocity);
    fn_800091D0(&m_up, &(const Vec3f &)fn_80079698(m_player.get()));
    fn_80076CA8(m_player.get()->m_stateBits, 0xA);
    fn_80076CA8(m_player.get()->m_stateBits, 0x31);
    fn_80076CA8(m_player.get()->m_stateBits, 0x34);
    fn_80076CA8(m_player.get()->m_stateBits, 0x38);
    fn_80076CC8(m_player.get()->m_stateBits, 2);
    m_player.get()->m_motion->vf_0x10(6, 0.0f);
    fn_800A2AD8(reinterpret_cast<void *>(m_player.get()->field_0x23C));
    fn_8034DF48(1, -2);
    m_state = 0;
}

} // namespace Behavior
} // namespace Player

/* boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out of line (boost
 * 1.32's definition; as in Game/AdvMessage.cpp). */
namespace boost {
template <class T> inline void shared_ptr<T>::reset() {
    this_type().swap(*this);
}
} // namespace boost

/* Drops a circuit target's path (other behaviors call it by its address). */
extern "C" void fn_8007684C(UnkCircuitTarget_800ABCE8 *target) {
    target->m_object.reset();
}

namespace Player {
namespace Behavior {

void ElectricCircuit::Leave(int nextId) {
    fn_80076CC8(m_player.get()->m_stateBits, 0xA);
    fn_80076CC8(m_player.get()->m_stateBits, 0x31);
    fn_80076CC8(m_player.get()->m_stateBits, 0x34);
    fn_80076CC8(m_player.get()->m_stateBits, 0x38);
    fn_8007684C(&m_target);
    float angle = DegToRad(0.0f);
    m_player.get()->m_rotation.x = angle;
    m_player.get()->m_rotation.z = angle;
    fn_800A2A78(reinterpret_cast<void *>(m_player.get()->field_0x23C));
    fn_8034DF48(5, -2);
}

void ElectricCircuit::vf_0x18(CharCommand *command) {
    switch (command->TypeId()) {
    case 7:
        if (!fn_8006CD78(m_player.get()->m_stateBits, 0x17) &&
            !fn_8006CD78(m_player.get()->m_stateBits, 0x18)) {
            if (m_player.get()->vf_0x50()) {
                fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(0x1F));
            } else {
                fn_800B7D48(&m_player, 0xA);
                fn_80079028(m_player.get(), 2.0f);
            }
        }
        command->Flags() |= 2;
        break;
    }
}

void ElectricCircuit::Update(float dt) {
    switch (m_state) {
    case 0:
        m_state = 1;
        fn_801D560C(fn_80053164(), 0x722F, 0, 0);
        break;
    case 1:
        if (!Advance(dt)) {
            m_state = 2;
        } else {
            fn_801D53C4(fn_80053164(), 0x7230, 0, this, 2, 0, 5);
        }
        break;
    case 2: {
        Vec3f direction;
        fn_800091D0(&direction, &(const Vec3f &)fn_8040CEE8(&FieldsOf(m_player.get())->m_velocity));
        fn_800A7F14(m_player.get()->m_behaviors, &InitializeArgsJump(direction));
        float speed = fn_8040CF50(&FieldsOf(m_player.get())->m_velocity);
        if (0.0f != speed) {
            ScaleVector(s_jumpSpeed / speed, FieldsOf(m_player.operator->())->m_velocity);
        }
        break;
    }
    }
}

} // namespace Behavior
} // namespace Player

/* Whether a circuit target has a path (other behaviors call it by its address). */
extern "C" bool fn_8007646C(UnkCircuitTarget_800ABCE8 *target) {
    return target->m_object.get() != 0;
}

namespace Player {
namespace Behavior {

bool ElectricCircuit::Advance(float dt) {
    m_speed += 0.05f * (s_maxSpeed - m_speed);
    if (fn_8007646C(&m_target) && fn_800ABB14(&m_target, dt * m_speed)) {
        MoveAlong(dt);
    } else {
        return false;
    }
    if (!fn_8000A950(dt)) {
        const Vec3f &velocity = fn_8040CDA8(
            &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&FieldsOf(m_player.get())->m_position,
                                                             &FieldsOf(m_player.get())->m_prevPosition),
            1.0f / dt);
        fn_800091D0(&FieldsOf(m_player.get())->m_velocity, &velocity);
    }
    return true;
}

void ElectricCircuit::MoveAlong(float dt) {
    Vec3f position;
    fn_800ABAE8(&m_target, &position, &m_forward, &m_up);
    fn_800091D0(&FieldsOf(m_player.get())->m_position, &position);
    CopyRotation(m_player->m_rotation, fn_8040F364(&m_forward, &m_up));
}

void ElectricCircuit::vf_0x20() {}

Behavior *ElectricCircuit::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new ElectricCircuit(player);
}

void ElectricCircuit::Release() {}

} // namespace Behavior
} // namespace Player

/* The behavior's record in the behavior factories' tables. */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804AF420 = {
    Player::Behavior::ElectricCircuit::Create, Player::Behavior::ElectricCircuit::Release, 0};
