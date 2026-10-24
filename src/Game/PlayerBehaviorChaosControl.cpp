/* Player::Behavior::ChaosControl (class name from RTTI): behavior 0x1D of Shadow, Chaos Control.
 * It may start only where a circuit path is found for the player (or when the game state allows
 * it anywhere, flag bit 3); it then plays its motion, listens to the player's motion events and
 * warps the player along the path, faster and faster, while it drains the gauge. It ends when the
 * path or the gauge runs out, or when the player lets go after two seconds, and switches to
 * behavior 2. File name inferred from the class (as Game/PlayerBehaviorSuper.cpp); all other names
 * are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
/* Declares Player::Behavior::Behavior's slots with their arguments (see Game/PlayerBehavior.h). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharCommand.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerMotionEvent.h"
#include "Game/PlayerSoundBase.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/UnkCircuitTarget_800ABCE8.h"
#include "Game/Vibration.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The game state (struct name unknown; as far as this unit reads it). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xA8];
    /* 0x0A8 */ u8 field_0xA8[0xB0 - 0xA8]; /* fn_802039E0 sets its byte at 0x21 */
    /* 0x0B0 */ u8 field_0xB0;              /* set: Chaos Control without a path (inferred) */
};

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The chaos gauge of the save data (struct name unknown): its value, and what fn_8016F1C0
 * returns (meaning inferred). */
struct UnkGauge_8016F1C0 {
    /* 0x0 */ s32 m_value;
    /* 0x4 */ s32 m_max;
};

/* The save data (see Game/BlackBullCtrlUnit.cpp): the record at 0x70C keeps the gauge. */
struct UnkObject_80079808 {
    /* 0x000 */ u8 field_0x0[0x70C];
    /* 0x70C */ UnkGauge_8016F1C0 m_gauge;
};

/* The player's members read here (struct name unknown; Player::PlayerBase leaves them
 * unmapped). */
struct UnkPlayerFields_8007BA58 {
    /* 0x000 */ u8 field_0x0[0xD0];
    /* 0x0D0 */ Vec3f m_prevPosition; /* where the player was before this frame (inferred) */
    /* 0x0DC */ u8 field_0xDC[0xF4 - 0xDC];
    /* 0x0F4 */ Vec3f m_up;
};

extern "C" {
/* A 32-bit flag word: fn_80014E04 constructs it cleared, fn_80014E10 clears it, fn_80014E1C sets
 * bit, fn_8006127C clears bit and fn_80014E34 tests bit (see Game/BAMine.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
/* The player's state bits (see Game/PlayerShadowSuper.cpp). */
bool fn_8006CD78(u32 *bits, int bit);
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* Counts the player's requests for state bit 0x17, and per player index for the word at 0x280
 * (see Game/PlayerBehaviorChaosBlast.cpp). */
void fn_8007939C(Player::PlayerBase *player, bool on);
void fn_80078F2C(Player::PlayerBase *player, int index, bool on);
/* Called with the player's object at 0x234 (behaviour unknown). */
void fn_8007792C(void *object);
void fn_80077950(void *object, int arg);
/* Called with the player's effects at 0x23C (behaviour unknown). */
void fn_800A2A08(void *effects);
void fn_800A2A30(void *effects);
void fn_800A2A54(void *effects);
void fn_800A2D7C(void *effects, int arg);
/* The player's up axis (behaviour inferred). */
Vec3f fn_80079698(Player::PlayerBase *player);
/* Player updates run before the behavior's own (behaviour unknown; as in FirstMeet). */
void fn_80079720(Player::PlayerBase *player);
void fn_8009728C(boost::shared_ptr<Player::PlayerBase> *player, int arg1);
/* Sets the player's up vector (behaviour inferred). */
void fn_8009339C(boost::shared_ptr<Player::PlayerBase> *player, const Vec3f *up);
/* Whether a point is free to stand at (behaviour inferred). */
bool fn_80097050(const Vec3f *position);
/* Switches a player's behavior (behaviour inferred). */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);
/* The circuit target: finds a path for the player, where it is (position and the two directions
 * of its frame; either may be 0), its state, whether it has a path, moving it on by a distance
 * (false at the path's end) and a flag (behaviour inferred; see
 * Game/UnkCircuitTarget_800ABCE8.h). */
bool fn_800ABC68(UnkCircuitTarget_800ABCE8 *target, boost::shared_ptr<Player::PlayerBase> *player);
void fn_800ABAE8(UnkCircuitTarget_800ABCE8 *target, Vec3f *position, Vec3f *forward, Vec3f *up);
int fn_800ABB48(UnkCircuitTarget_800ABCE8 *target);
bool fn_8007646C(UnkCircuitTarget_800ABCE8 *target);
bool fn_800ABB14(UnkCircuitTarget_800ABCE8 *target, float distance);
void fn_800ABAA4(UnkCircuitTarget_800ABCE8 *target, bool on);
/* Sets the byte at 0x21 (behaviour from its body). */
void fn_802039E0(void *object, bool on);
/* Plays a jingle (see Game/ActStageController.cpp). */
void fn_801D3888(int jingle);
/* The save data, a gauge's maximum and adding to a gauge (see Game/BlackBullCtrlUnit.cpp). */
UnkObject_80079808 *fn_80079808(void);
int fn_8016F1C0(UnkGauge_8016F1C0 *gauge);
void fn_8016F0CC(UnkGauge_8016F1C0 *gauge, int arg1, int points);
/* Rumbles the controller, and stops a rumble (see Game/VibrationRequest.cpp). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DEE4(int pattern, int pad);
void fn_8034DB60(boost::weak_ptr<Vibration::RequestHandle> *handle);
/* Whether a value is about zero (see Game/ArkCannon.cpp). */
bool fn_8000A950(float value);

void fn_8040CE60(Vec3f *vector);                     /* vector = 0 */
Vec3f fn_800091A4(const Vec3f *vector, float scale); /* vector * scale */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale); /* vector * scale */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* vector += value */
Vec3f fn_8040F364(const Vec3f *forward, const Vec3f *up); /* Euler angles */
}

/* The player's members above (a macro, as in Game/PlayerBehaviorSuper.cpp). */
#define FieldsOf(player) reinterpret_cast<UnkPlayerFields_8007BA58 *>(player)

/* fn_8040CD74 returns lhs - rhs through its first argument, as a function returning a Vec3f does;
 * called this way the result is a temporary, as in the game (see Game/PlayerBehaviorSuper.cpp). */
typedef Vec3f (*VectorSubtract)(const Vec3f *lhs, const Vec3f *rhs);
#define Subtract(lhs, rhs) (reinterpret_cast<VectorSubtract>(fn_8040CD74)(lhs, rhs))

inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

namespace Player {
namespace Behavior {

/* The behavior's flag word (struct name unknown): bit 0 the player keeps the button held, 1 the
 * player asked to stop, 2 the motion's event 0x5000 came, 3 Chaos Control without a path. */
struct ChaosControlFlags {
    ChaosControlFlags() {
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
    void Reset() {
        fn_80014E10(&m_bits);
    }

    u32 m_bits;
};

class ChaosControl : public Behavior {
  public:
    ChaosControl(const boost::shared_ptr<PlayerBase> &player);
    virtual ~ChaosControl();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x18(CharCommand *command);
    virtual void Update(float dt);
    virtual void vf_0x20();

    /* The factory record's functions (names inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);
    static void Release();

    void OnMotion(MotionEventArgs &args);
    void Move(float dt);

    static const char *s_name;

    /* 0x1C */ s32 m_state; /* 0 start, 1 waiting for the motion, 2 warping, 3 done, 4 in place */
    /* 0x20 */ ChaosControlFlags m_flags;
    /* 0x24 */ UnkCircuitTarget_800ABCE8 m_target;
    /* 0x2C */ float m_speed;
    /* 0x30 */ float m_time;
    /* 0x34 */ boost::weak_ptr<Vibration::RequestHandle> m_vibration;
};
SIZE_ASSERT(ChaosControl, 0x3C);

const char *ChaosControl::s_name = "ChaosControl";

/* The speed the warp eases towards (name inferred). The game sets it in __sinit rather than in
 * .sdata, as an initializer that is not a constant expression does; the inline call reproduces
 * that (as in Game/PlayerBehaviorSuper.cpp). */
inline float DynamicFloat(float value) {
    return value;
}
static float s_maxSpeed = DynamicFloat(900.0f);

} // namespace Behavior
} // namespace Player

using namespace Player::Behavior;

/* Marks the commands the chaos behaviors take as handled (name unknown; called by
 * Player::Behavior::ChaosBlast too, Game/PlayerBehaviorChaosBlast.cpp). */
extern "C" void fn_8007C810(CharCommand *command) {
    switch (command->TypeId()) {
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 0x101:
    case 0x10B:
    case 0x10D:
    case 0x111:
    case 0x112:
    case 0x121:
    case 0x122:
    case 0x129:
    case 0x12A:
    case 0x12B:
    case 0x12C:
    case 0x12E:
    case 0x131:
    case 0x134:
        command->Flags() |= 2;
        break;
    }
}

namespace Player {
namespace Behavior {

ChaosControl::ChaosControl(const boost::shared_ptr<PlayerBase> &player)
    : Behavior(player, s_name, 0x1D) {}

ChaosControl::~ChaosControl() {}

bool ChaosControl::vf_0x0C(int prevId, const InitializeArgs &args) {
    if (prevId == 0x1D || fn_8006CD78(m_player.get()->m_stateBits, 0x15)) {
        return false;
    }
    if (!fn_8006CD78(m_player.get()->m_stateBits, 0x34)) {
        m_flags.Reset();
        if (lbl_8057D6A0.field_0xB0) {
            m_flags.Set(3);
            return true;
        }
        if (fn_800ABC68(&m_target, &m_player)) {
            Vec3f position;
            fn_800ABAE8(&m_target, &position, 0, 0);
            if (fn_80097050(&position)) {
                return true;
            }
        }
    }
    m_player.get()->m_sound->StopVoice(0x1B);
    return false;
}

void ChaosControl::Enter(int prevId, const InitializeArgs &args) {
    fn_80076CA8(m_player.get()->m_stateBits, 0x38);
    fn_80076CC8(m_player.get()->m_stateBits, 2);
    fn_80076CA8(m_player.get()->m_stateBits, 0x14);
    fn_8007939C(m_player.get(), 1);
    fn_802039E0(lbl_8057D6A0.field_0xA8, 1);
    m_player.get()->m_motion->vf_0x10(0x25, 0.0f);
    PlayerMotionEventCallback(m_player.get()) =
        MotionEventCallback(boost::bind(&ChaosControl::OnMotion, this, _1));
    fn_8040CE60(&m_player.get()->field_0x18);
    m_speed = 0.0f;
    m_time = 0.0f;
    if (m_flags.Test(3)) {
        m_state = 4;
        fn_801D3888(2);
    } else {
        fn_800ABAA4(&m_target, 1);
        m_state = 0;
        fn_801D3888(1);
    }
    fn_800A2D7C(reinterpret_cast<void *>(m_player.get()->field_0x23C), 1);
    fn_80078F2C(m_player.get(), m_player.get()->field_0x54, 1);
}

void ChaosControl::Leave(int nextId) {
    fn_80076CC8(m_player.get()->m_stateBits, 0x38);
    fn_80076CC8(m_player.get()->m_stateBits, 0x14);
    fn_8007939C(m_player.get(), 0);
    fn_802039E0(lbl_8057D6A0.field_0xA8, 0);
    PlayerMotionEventCallback(m_player.get()).Clear();
    fn_80076CC8(m_player.get()->m_stateBits, 0x35);
    fn_8007792C(reinterpret_cast<void *>(m_player.get()->field_0x234));
    fn_8034DB60(&m_vibration);
    if (m_flags.Test(3)) {
        fn_800A2A08(reinterpret_cast<void *>(m_player.get()->field_0x23C));
    } else {
        fn_800A2A30(reinterpret_cast<void *>(m_player.get()->field_0x23C));
    }
    fn_80078F2C(m_player.get(), m_player.get()->field_0x54, 0);
}

void ChaosControl::vf_0x18(CharCommand *command) {
    fn_8007C810(command);
    if (!(command->Flags() & 2)) {
        switch (command->TypeId()) {
        case 0x106:
            m_flags.Set(1);
            command->Flags() |= 2;
            break;
        }
    }
}

void ChaosControl::Update(float dt) {
    if (!m_flags.Test(3) && fn_800ABB48(&m_target) != 5) {
        fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(2));
        return;
    }
    UnkObject_80079808 *save = fn_80079808();
    switch (m_state) {
    case 0:
        m_state = 1;
        break;
    case 1:
        if (m_flags.Test(2)) {
            fn_80076CA8(m_player.get()->m_stateBits, 0x35);
            fn_80077950(reinterpret_cast<void *>(m_player.get()->field_0x234), 0xB);
            fn_800A2A54(reinterpret_cast<void *>(m_player.get()->field_0x23C));
            m_vibration = fn_8034DEE4(8, m_player.get()->field_0x54);
            m_state = 2;
        }
        break;
    case 2: {
        m_time += dt;
        int cost = dt * fn_8016F1C0(&save->m_gauge) / 4.5f;
        if (cost <= 0) {
            cost = 1;
        }
        fn_8016F0CC(&save->m_gauge, 0, -cost);
        if (save->m_gauge.m_value <= 0.0f) {
            m_flags.Set(1);
        }
        m_speed += 0.03f * (s_maxSpeed - m_speed);
        if (!fn_8007646C(&m_target) || !fn_800ABB14(&m_target, dt * m_speed)) {
            m_state = 3;
        }
        break;
    }
    case 3:
        m_player.get()->m_sound->StopVoice(0x1C);
        fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(2));
        fn_8034DB60(&m_vibration);
        break;
    case 4:
        AddVector(m_player->field_0x24,
                  fn_800091A4(&(const Vec3f &)fn_80079698(m_player.operator->()), 12.5f));
        if (m_flags.Test(2)) {
            fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(2));
            return;
        }
        break;
    }
    if (m_flags.Test(3)) {
        fn_80079720(m_player.get());
        fn_8009728C(&m_player, 0);
        return;
    }
    if (fn_80014E34(&m_player.get()->field_0xA4, 6)) {
        m_flags.Clear(0);
    }
    if (fn_80014E34(&m_player.get()->field_0xA4, 5)) {
        m_flags.Set(0);
    }
    if (fn_80014E34(&m_player.get()->field_0xA4, 7)) {
        m_state = 3;
    } else if (m_flags.Test(1)) {
        if (m_flags.Test(0) || m_time <= 2.0f) {
            m_flags.Clear(1);
        } else {
            m_state = 3;
        }
    }
    Move(dt);
    if (!fn_8000A950(dt)) {
        const Vec3f &velocity =
            fn_8040CDA8(&(const Vec3f &)Subtract(&m_player.get()->m_position,
                                                 &FieldsOf(m_player.get())->m_prevPosition),
                        1.0f / dt);
        fn_800091D0(&m_player.get()->field_0x18, &velocity);
    }
    fn_8009339C(&m_player, &FieldsOf(m_player.get())->m_up);
}

void ChaosControl::OnMotion(MotionEventArgs &args) {
    if (args.m_event == 0x5000) {
        m_flags.Set(2);
    }
}

/* Places the player where the target is on its path, turned along it. */
void ChaosControl::Move(float dt) {
    Vec3f position;
    Vec3f forward;
    Vec3f up;
    fn_800ABAE8(&m_target, &position, &forward, &up);
    fn_800091D0(&m_player.get()->m_position, &position);
    m_player->m_rotation = fn_8040F364(&forward, &up);
}

void ChaosControl::vf_0x20() {}

Behavior *ChaosControl::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new ChaosControl(player);
}

void ChaosControl::Release() {}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds ChaosControl by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804AF9CC = {
    Player::Behavior::ChaosControl::Create, Player::Behavior::ChaosControl::Release, 0};
