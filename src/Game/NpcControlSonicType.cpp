/* @unnamed@NpcControlSonicType_cpp@::NpcSonicType (class name from RTTI; base
 * Player::Control::ControlBase; its name is "NpcSonicType"): control 2 of a Sonic-type NPC, its
 * own AI. It follows the player it partners (keeping beside it, replaying its moves when it is
 * close), or walks back to where it was put; it picks targets near it and attacks them with a jump
 * (AttackJump, a homing attack or the weapon's attack), and jumps along when the player jumps
 * (WatchJump). Three timers (one per kind of target) decide when a target is taken. The unit also
 * holds the NPC managers' accessor (fn_8006F754) and the clamp, min and max of floats that other
 * units call by their addresses. File name from the RTTI name; function and member names are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). NonMatching: 85 of 87 functions match (UpdateTargets and UpdateAttack differ
 * in registers only; see there); the rest of the unit links byte for byte (checked as Matching). */
#define CHARCOMMAND_IMPLEMENTATION
#define CHARCOMMAND_PLAYER_COMMANDS
#define CHARCOMMAND_INLINE_BASE
#define MSL_VECTOR_MIDDLE
#define MSL_VECTOR_INDEX
#define MSL_ALGORITHM_OUTLINE
#include <string>
#include <vector>
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/Npc.h"
#include "Game/Player.h"
#include "Game/PlayerCharaParam.h"
#include "Game/PlayerControl.h"
#include "Game/PlayerPadInput.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/Target.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using boost::shared_ptr;
using boost::weak_ptr;
using Player::PlayerBase;
using Player::PlayerTargetBase;
using Player::Behavior::UnkBehaviorExecuter_800A7F14;
using Player::Control::ControlBase;
using Player::Npc::Npc;

/* One of a player's recent states (Game/PlayerHistory.cpp has the layout): the input direction,
 * a velocity and the state bits are used here. */
struct UnkPlayerRecord_800A849C {
    /* 0x00 */ Vec3f field_0x0;
    /* 0x0C */ u8 field_0xC[0x24 - 0xC];
    /* 0x24 */ Vec3f field_0x24;
    /* 0x30 */ u8 field_0x30[0x58 - 0x30];
    /* 0x58 */ u32 m_stateBits[3];
};

/* A player's recent states (Game/PlayerHistory.cpp). */
class UnkPlayerHistory_800A82C8 {
  public:
    UnkPlayerRecord_800A849C *Get(int back);
};

/* The NPC managers (class name unknown; constructor 0x80075070, destructor 0x80074FD4): its byte
 * at 0xC is set every frame an NPC runs this control (meaning unknown). */
struct UnkCharManager_8006F754 {
    UnkCharManager_8006F754();
    ~UnkCharManager_8006F754();
    void SetActive(bool active) {
        field_0xC = active;
    }

    /* 0x00 */ u8 field_0x0[0xC];
    /* 0x0C */ bool field_0xC;
    /* 0x0D */ u8 field_0xD[0x14 - 0xD];
};

/* The game state (struct name unknown; see Game/TGi.h): a flag at 0xC8 that stops the NPCs
 * (meaning unknown). */
struct UnkGameState_8057D6A0 {
    /* 0x00 */ u8 field_0x0[0xC8];
    /* 0xC8 */ u8 field_0xC8;
};
extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The player members used here (names inferred). */
struct UnkPlayerFields_8006DE34 {
    /* 0x000 */ u8 field_0x0[0x64];
    /* 0x064 */ s32 m_kind;
    /* 0x068 */ u8 field_0x68[0xA0 - 0x68];
    /* 0x0A0 */ u32 m_flags;
    /* 0x0A4 */ u32 field_0xA4;
    /* 0x0A8 */ u32 m_stateBits[3];
    /* 0x0B4 */ u8 field_0xB4[0x100 - 0xB4];
    /* 0x100 */ Vec3f field_0x100;
    /* 0x10C */ u8 field_0x10C[0x218 - 0x10C];
    /* 0x218 */ UnkPlayerInput_800A85B0 *m_input;
    /* 0x21C */ u8 field_0x21C[0x24C - 0x21C];
    /* 0x24C */ PlayerTargetBase *m_aim;
    /* 0x250 */ UnkPlayerHistory_800A82C8 *m_history;
};

#define Fields(player) reinterpret_cast<UnkPlayerFields_8006DE34 *>(player)

extern "C" {
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_8006CD78(u32 *bits, int bit);
/* NPC helpers (see Game/NpcControlRestartFollow.cpp): whether the NPC follows a player, the player
 * it follows, and its player number. */
bool fn_80073708(Npc *npc);
weak_ptr<PlayerBase> fn_8007363C(Npc *npc);
int fn_80073A08(Npc *npc);
/* The facing and the up vectors of a player. */
Vec3f fn_800796DC(PlayerBase *player);
Vec3f fn_80079698(PlayerBase *player);
/* Switches the control of the character task (Player::Controller). */
void fn_8009EBA4(Task *controller, const Player::Control::InitializeArgs *args);
/* Whether the player's behavior is id. */
bool fn_800A7EEC(UnkBehaviorExecuter_800A7F14 *executer, int id);
/* Whether a character stands on the ground (behaviour inferred). */
bool fn_800B7B58(const shared_ptr<PlayerBase> &player);
/* Turns the player towards direction (see Game/PlayerUtil.cpp). */
void fn_80093BA0(const shared_ptr<PlayerBase> *player, const Vec3f *direction, float rate);
/* Whether a target is still alive. */
bool fn_801E7094(const weak_ptr<Target> *target);
/* Whether the player's position is in an event's area (behaviour unknown). */
bool fn_801F42C8(weak_ptr<PlayerBase> *player, Vec3f *position);
/* The mode of a player number (meaning unknown). */
int fn_8020D2A8(int number);
float fn_8005AE20(float metres);
void fn_8040F990(Vec3f *along, Vec3f *across, const Vec3f *vector, const Vec3f *normal);
float fn_8040D180(const Vec3f *a, const Vec3f *b); /* |a - b|^2 */
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b); /* a x b */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* += */
void fn_8040CE04(Vec3f *vector, const Vec3f *value); /* -= */
Vec3f fn_800091A4(const Vec3f *vector, float scale);
Vec3f fn_8040CDA8(const Vec3f *v, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CEE8(const Vec3f *v);
float fn_8040CF90(const Vec3f *a, const Vec3f *b);
float fn_8040EE90(void);
void fn_8040CE38(Vec3f *v, float scale);
float fn_8040CE74(Vec3f *v);
}

extern "C" double pow(double x, double y);

/* fn_8040CD74 called as returning a - b by value (see Game/ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);
#define Sub(a, b) ((VectorSubtractFn)fn_8040CD74)(a, b)

/* v normalized (an inline call: inline calls in an expression are evaluated before it, right to
 * left). */
inline Vec3f Normalized(const Vec3f *v) {
    return fn_8040CEE8(v);
}

/* v * scale (inline, as Normalized). */
inline Vec3f Scaled(const Vec3f *v, float scale) {
    return fn_8040CDA8(v, scale);
}

/* The parameters of a player's character. */
inline const UnkCharaParam_804F1834 &CharaParam(PlayerBase *player) {
    return lbl_804F1834[Fields(player)->m_kind];
}

/* Returns x (an inline call, so that the accessors below are not inlined themselves; deferred
 * inlining is one level deep). */
template <class T> inline T &Ref(T &x) {
    return x;
}

/* Min, max and clamp of floats (other units call them by their addresses). */
extern "C" {
const float &fn_800704AC(const float &a, const float &b);
const float &fn_80070494(const float &a, const float &b);
const float &fn_80070460(const float &value, const float &low, const float &high);
}

/* The instance pointer of a class made on first use (as in Game/NpcControlRestartFollow.cpp). */
template <class T> struct Instance {
    static T *s_instance;
};

template <class T> T *Instance<T>::s_instance;


/* The pair made on first use by fn_801E5BD0 (class name unknown; Game/PlayerEffect.cpp). */
class UnkPair_800A26E0;

/* Never called: retail's .sbss holds the instance pointer of UnkPair_800A26E0 here, after this
 * unit's own, so the unit names it (the accessor that uses it is fn_801E5BD0). */
UnkPair_800A26E0 *fn_UnkPairInstance() {
    return Instance<UnkPair_800A26E0>::s_instance;
}

/* The NPC managers, made on first use (other units call it by its address). */
extern "C" inline UnkCharManager_8006F754 *fn_8006F754(void) {
    if (Instance<UnkCharManager_8006F754>::s_instance == 0) {
        static UnkCharManager_8006F754 s_manager;
        Instance<UnkCharManager_8006F754>::s_instance = &Ref(s_manager);
    }
    return Instance<UnkCharManager_8006F754>::s_instance;
}

/* The player's pad input (other units call it by its address). */
extern "C" inline UnkPlayerInput_800A85B0 *fn_8006F8D0(PlayerBase *player) {
    return Ref(Fields(player)->m_input);
}

/* The timers of the three kinds of target (class name unknown): a vector of floats. */
class UnkTimers_8006FC34 {
  public:
    UnkTimers_8006FC34();
    ~UnkTimers_8006FC34();
    /* Adds count timers at 0. */
    void Add(int count);
    float &operator[](int index);
    /* Never called (it emits the vector's element access before operator[]). */
    float &Get(int index);

    /* 0x0 */ std::vector<float> m_values;
};

namespace {

/* The jump of an attack: the NPC jumps, then (homing) attacks its target once it is in the air,
 * or ends the jump when its time runs out (time < 0: no limit). */
class AttackJump : public Task {
  public:
    AttackJump(AttackJump **slot, const shared_ptr<Npc> &npc, const weak_ptr<Target> &target,
               bool homing, float time);
    virtual ~AttackJump();
    virtual void Tick(float dt);

    /* 0x28 */ AttackJump **m_slot;
    /* 0x2C */ shared_ptr<Npc> m_npc;
    /* 0x34 */ weak_ptr<Target> m_target;
    /* 0x3C */ float m_time;
    /* 0x40 */ bool m_homing;
    /* 0x44 */ s32 m_phase;
};
SIZE_ASSERT(AttackJump, 0x48);

/* A jump after the player's: after delay, the NPC jumps, and holds the jump for as long as the
 * player held its jump button. */
class WatchJump : public Task {
  public:
    WatchJump(const shared_ptr<Npc> &npc, const weak_ptr<PlayerBase> &leader, float delay);
    virtual ~WatchJump();
    virtual void Tick(float dt);

    /* 0x28 */ shared_ptr<Npc> m_npc;
    /* 0x30 */ weak_ptr<PlayerBase> m_leader;
    /* 0x38 */ float m_delay;
    /* 0x3C */ float m_time;
    /* 0x40 */ bool m_holding;
    /* 0x44 */ s32 m_phase;
};
SIZE_ASSERT(WatchJump, 0x48);

class NpcSonicType : public ControlBase {
  public:
    NpcSonicType(const shared_ptr<PlayerBase> &player);
    virtual ~NpcSonicType();
    virtual void vf_0x0C();
    virtual void vf_0x10();
    virtual void vf_0x14(float dt);

    /* Turns the NPC towards the player it follows. */
    void FaceLeader(const shared_ptr<PlayerBase> &leader);
    /* Moves the NPC along with the player, replaying its moves. */
    void FollowMove(float dt);
    /* Walks the NPC towards its destination. */
    void Walk();
    void WalkIfIdle();
    void Move(float dt);
    void UpdateIdle(float dt);
    void UpdateFollow(float dt);
    void UpdateAttack(float dt);
    void UpdateState(float dt);
    void UpdateDestination();
    void UpdateWatch(float dt);
    void DecayTimers(float dt);
    void UpdateTargets(float dt);

    static const char *s_taskName;

    /* 0x14 */ UnkTimers_8006FC34 m_timers;
    /* 0x20 */ s32 m_targetKind;
    /* 0x24 */ weak_ptr<Target> m_target;
    /* 0x2C */ shared_ptr<Npc> m_npc;
    /* 0x34 */ u32 m_flags;
    /* 0x38 */ s32 m_state;
    /* 0x3C */ AttackJump *m_attackJump;
    /* 0x40 */ Vec3f m_destination;
    /* 0x4C */ Vec3f m_home;
};
SIZE_ASSERT(NpcSonicType, 0x58);

} // namespace

extern "C" const float &fn_800704AC(const float &a, const float &b) {
    return a < b ? a : b;
}

extern "C" const float &fn_80070494(const float &a, const float &b) {
    return a < b ? b : a;
}

extern "C" const float &fn_80070460(const float &value, const float &low, const float &high) {
    return fn_800704AC(fn_80070494(value, low), high);
}

/* Never called: it emits EndJumpCommand's destructor here, where retail has it (the vtable alone
 * would put it after these functions). */
void fn_DestroyEndJumpCommand(Player::EndJumpCommand *command) {
    command->Player::EndJumpCommand::~EndJumpCommand();
}

namespace {

WatchJump::WatchJump(const shared_ptr<Npc> &npc, const weak_ptr<PlayerBase> &leader, float delay)
    : Task(npc->m_task), m_npc(npc), m_leader(leader), m_delay(delay) {
    m_name = "WatchJump";
    m_time = 0.0f;
    m_holding = true;
    m_phase = 0;
}

WatchJump::~WatchJump() {}

void WatchJump::Tick(float dt) {
    shared_ptr<PlayerBase> leader = m_leader.lock();
    if (leader) {
        UnkPlayerInput_800A85B0 *input = fn_8006F8D0(leader.get());
        if (m_holding) {
            if (input->m_held & 0x10) {
                m_time += dt;
            } else {
                m_holding = false;
            }
        }
    } else {
        m_time = 1.0f + dt;
    }
    switch (m_phase) {
    case 0:
        m_delay -= dt;
        if (m_delay <= 0.0f) {
            m_delay = 0.0f;
            if (fn_80014E34(&Fields(m_npc.get())->m_flags, 1)) {
                m_npc->PushCommand(new Player::StartJumpCommand(4));
                m_phase++;
            } else {
                m_flags |= 1;
            }
        }
        break;
    case 1:
        m_time -= dt;
        if (m_time <= 0.0f) {
            m_time = 0.0f;
            m_npc->PushCommand(new Player::EndJumpCommand(4));
            m_flags |= 1;
        }
        if (fn_8006CD78(Fields(m_npc.get())->m_stateBits, 2)) {
            m_flags |= 1;
        }
        break;
    }
}

AttackJump::AttackJump(AttackJump **slot, const shared_ptr<Npc> &npc, const weak_ptr<Target> &target,
                       bool homing, float time)
    : Task(npc->m_task), m_slot(slot), m_npc(npc), m_target(target), m_time(time), m_homing(homing) {
    m_name = "AttackJump";
    if (m_slot) {
        *m_slot = this;
    }
    m_phase = 0;
    if (!fn_80014E34(&Fields(m_npc.get())->m_flags, 2) || !m_homing) {
        if (fn_80014E34(&Fields(m_npc.get())->m_flags, 1)) {
            m_npc->PushCommand(new Player::StartJumpCommand(4));
        } else {
            m_flags |= 1;
        }
    }
}

AttackJump::~AttackJump() {
    if (m_slot) {
        *m_slot = 0;
    }
}

void AttackJump::Tick(float dt) {
    if (!fn_801E7094(&m_target)) {
        m_flags |= 1;
        return;
    }
    if (m_time >= 0.0f) {
        m_time -= dt;
        if (m_time <= 0.0f) {
            m_npc->PushCommand(new Player::EndJumpCommand(4));
            m_flags |= 1;
            return;
        }
    }
    if (fn_800B7B58(m_npc)) {
        if (m_homing) {
            m_npc->PushCommand(new Player::HomingAttackCommand(m_target, 4));
        }
        m_flags |= 1;
    }
}

} // namespace

UnkTimers_8006FC34::UnkTimers_8006FC34() {
    m_values.reserve(20);
}

UnkTimers_8006FC34::~UnkTimers_8006FC34() {}

void UnkTimers_8006FC34::Add(int count) {
    while (count-- > 0) {
        float zero = 0.0f;
        m_values.push_back(zero);
    }
}

float &UnkTimers_8006FC34::operator[](int index) {
    return m_values[index];
}

float &UnkTimers_8006FC34::Get(int index) {
    return m_values[index];
}

namespace {

/* The limits of the timers (retail keeps the zero in .sdata, hence explicit_zero_data). */
#pragma explicit_zero_data on
float s_one = 1.0f;
float s_zero = 0.0f;
#pragma explicit_zero_data reset

const char *NpcSonicType::s_taskName = "NpcSonicType";

NpcSonicType::NpcSonicType(const shared_ptr<PlayerBase> &player) : ControlBase(player, s_taskName, 2) {
    fn_80014E04(&m_flags);
    m_npc = boost::dynamic_pointer_cast<Npc>(m_player);
    m_timers.Add(3);
    m_targetKind = 0;
    m_attackJump = 0;
}

NpcSonicType::~NpcSonicType() {
    if (m_attackJump) {
        m_attackJump->m_slot = 0;
        m_attackJump->m_flags |= 1;
    }
}

void NpcSonicType::vf_0x0C() {
    fn_800091D0(&m_destination, &m_player.get()->m_position);
    fn_800091D0(&m_home, &m_player.get()->m_position);
    fn_80014E10(&m_flags);
    m_state = 0;
}

void NpcSonicType::vf_0x10() {}

void NpcSonicType::vf_0x14(float dt) {
    if (fn_801F42C8(&m_npc->m_self, &m_home)) {
        return;
    }
    if (lbl_8057D6A0.field_0xC8) {
        return;
    }
    if (fn_80073708(m_npc.get()) && fn_8006F8D0(m_npc.get())->GetIdleTime() == 0.0f) {
        fn_8009EBA4(m_npc->m_task, &Player::Control::InitializeArgs(0));
        return;
    }
    UpdateTargets(dt);
    UpdateWatch(dt);
    UpdateDestination();
    UpdateState(dt);
    Move(dt);
    fn_8006F754()->SetActive(true);
}

/* NonMatching (28 instructions): with kind set at the top, GC/2.7 gives retail's registers but
 * keeps the li of 0 at the top; retail has it after the target's flags are loaded (in the block
 * that uses it). Setting it there instead gives other registers. Probably the newer compiler's
 * scheduling (see docs/research-notes.md). */
void NpcSonicType::UpdateTargets(float dt) {
    int kind = 0;
    shared_ptr<Target> target = Fields(m_npc.get())->m_aim->vf_0x28().lock();
    fn_8006127C(&m_flags, 2);
    if (target && m_state != 3) {
        if (target->m_flags & 0x18) {
            float distance =
                fn_8040D180(&(const Vec3f &)target->GetPosition(&m_npc->m_position), &m_npc->m_position);
            if (distance < 2500.0f) {
                kind = 2;
            }
        } else if (target->m_flags & 0x600000) {
            float distance =
                fn_8040D180(&(const Vec3f &)target->GetPosition(&m_npc->m_position), &m_npc->m_position);
            if (distance < target->m_radius * target->m_radius) {
                kind = 2;
            }
        } else if (target->m_flags & 0x70000) {
            if (Fields(m_npc.get())->m_kind != 9 && fn_8006F754()->field_0xC &&
                fn_8020D2A8(fn_80073A08(m_npc.get())) != 1) {
                float distance =
                    fn_8040D180(&(const Vec3f &)target->GetPosition(&m_npc->m_position), &m_npc->m_position);
                if (distance < 10000.0f) {
                    kind = 1;
                    fn_80014E1C(&m_flags, 2);
                }
            }
        }
        for (int i = 0; i < 3; i++) {
            if (i != kind) {
                float &timer = m_timers[i];
                timer -= dt;
                timer = fn_80070494(timer, s_zero);
            }
        }
        if (kind != 0) {
            float &timer = m_timers[kind];
            timer += dt;
            timer = fn_800704AC(timer, s_one);
            if (timer >= 1.0f) {
                m_target = target;
                m_targetKind = kind;
            }
        }
    } else {
        DecayTimers(dt);
    }
}

void NpcSonicType::DecayTimers(float dt) {
    for (int i = 0; i < 3; i++) {
        float &timer = m_timers[i];
        timer -= dt;
        timer = fn_80070494(timer, s_zero);
    }
}

void NpcSonicType::UpdateWatch(float dt) {
    shared_ptr<PlayerBase> leader = fn_8007363C(m_npc.get()).lock();
    if (fn_80073708(m_npc.get()) && leader) {
        UnkPlayerRecord_800A849C *now = Fields(leader.get())->m_history->Get(0);
        UnkPlayerRecord_800A849C *before = Fields(leader.get())->m_history->Get(1);
        if (fn_8006CD78(now->m_stateBits, 4) && !fn_8006CD78(before->m_stateBits, 4)) {
            float delay;
            switch (m_state) {
            case 2:
                delay = 0.5f;
                break;
            case 3:
                delay = 0.05f;
                break;
            default:
                delay = -1.0f;
                break;
            }
            if (delay >= 0.0f) {
                new WatchJump(m_npc, leader, delay);
            }
        }
        if (!fn_801E7094(&m_target) && fn_8006CD78(Fields(leader.get())->m_stateBits, 3)) {
            m_state = 3;
        }
    }
}

void NpcSonicType::UpdateDestination() {
    if (fn_801E7094(&m_target)) {
        fn_800091D0(&m_destination, &(const Vec3f &)m_target.lock()->GetPosition(&m_npc->m_position));
        return;
    }
    if (fn_80073708(m_npc.get())) {
        shared_ptr<PlayerBase> leader = fn_8007363C(m_npc.get()).lock();
        if (!leader) {
            return;
        }
        Vec3f up;
        fn_800091EC(&up, &(const Vec3f &)fn_80079698(leader.get()));
        Vec3f front;
        fn_800091EC(&front, &(const Vec3f &)fn_800796DC(leader.get()));
        Vec3f side;
        fn_800091EC(&side, &(const Vec3f &)fn_8040CEE8(&(const Vec3f &)fn_8040CFB8(&front, &up)));
        Vec3f base;
        fn_800091EC(&base, &(const Vec3f &)fn_8040CDD0(&leader->m_position, &(const Vec3f &)Scaled(&up, 0.0f)));
        if (fn_8040CF90(&side, &(const Vec3f &)Sub(&m_npc->m_position, &leader->m_position)) >= 0.0f) {
            fn_8040CFF8(&base, &(const Vec3f &)fn_800091A4(&side, 10.0f));
        } else {
            fn_8040CE04(&base, &(const Vec3f &)fn_800091A4(&side, 10.0f));
        }
        Vec3f offset;
        fn_800091EC(&offset, &(const Vec3f &)Sub(&m_destination, &base));
        float height = fn_8040CF90(&offset, &up);
        if (height < 0.0f) {
            fn_8040CE04(&offset, &(const Vec3f &)fn_800091A4(&up, height));
        } else if (height > 10.0f) {
            fn_8040CE04(&offset, &(const Vec3f &)fn_800091A4(&up, height - fn_8005AE20(1.0f)));
        }
        if (fn_8040CF2C(&offset) > 625.0f) {
            fn_800091D0(&offset, &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)fn_8040CEE8(&offset), 25.0f));
        }
        fn_800091D0(&m_destination, &(const Vec3f &)fn_8040CDD0(&base, &offset));
        fn_800091D0(&offset, &(const Vec3f &)Sub(&m_destination, &leader->m_position));
        if (fn_8040CF2C(&offset) < 400.0f) {
            fn_800091D0(&offset, &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)fn_8040CEE8(&offset), 20.0f));
            fn_800091D0(&m_destination, &(const Vec3f &)fn_8040CDD0(&leader->m_position, &offset));
        }
        return;
    }
    fn_800091D0(&m_destination, &m_home);
}

void NpcSonicType::UpdateState(float dt) {
    if (fn_801E7094(&m_target)) {
        UpdateAttack(dt);
    } else if (fn_80073708(m_npc.get())) {
        UpdateFollow(dt);
    } else {
        UpdateIdle(dt);
    }
}

/* NonMatching (8 instructions): retail computes the character table's address in r3 and the
 * offset in r0 (both homing-attack checks); GC/2.7 swaps them (about 15 source shapes tried). */
void NpcSonicType::UpdateAttack(float dt) {
    Vec3f offset;
    fn_800091EC(&offset, &(const Vec3f &)Sub(&m_destination, &m_npc->m_position));
    Vec3f across;
    Vec3f along;
    fn_8040F990(&along, &across, &offset, &(const Vec3f &)fn_80079698(m_npc.get()));
    float distance = fn_8040CF50(&offset);
    float horizontal = fn_8040CF50(&across);
    float vertical = fn_8040CF50(&along);
    if (m_timers[m_targetKind] <= 0.0f) {
        m_state = 0;
        m_targetKind = 0;
        m_target.reset();
        return;
    }
    switch (m_state) {
    case 0:
        if (distance > 1.0f) {
            m_state = 2;
        }
        break;
    case 1:
        if (m_attackJump == 0) {
            if (fn_80014E34(&Fields(m_npc.get())->m_flags, 2) &&
                CharaParam(m_npc.get()).m_canHomingAttack) {
                new AttackJump(&m_attackJump, m_npc, m_target, true, -1.0f);
            }
            if (fn_800A7EEC(m_npc->m_behaviors, 1) || fn_800A7EEC(m_npc->m_behaviors, 3) ||
                fn_800A7EEC(m_npc->m_behaviors, 0)) {
                if (Fields(m_npc.get())->m_kind == 9 || fn_8040EE90() < 0.5f) {
                    fn_80014E1C(&m_flags, 3);
                } else {
                    fn_8006127C(&m_flags, 3);
                }
                m_state = 2;
            }
        }
        break;
    case 2:
        if (fn_80014E34(&m_flags, 2)) {
            if (horizontal < 70.0f && CharaParam(m_npc.get()).m_canHomingAttack) {
                new AttackJump(&m_attackJump, m_npc, m_target, true, -1.0f);
                m_state = 1;
            } else if (fn_80014E34(&m_flags, 3)) {
                if (fn_8006CD78(Fields(m_npc.get())->m_stateBits, 2)) {
                    if (vertical >= 20.0f) {
                        fn_8006127C(&m_flags, 3);
                    } else if (horizontal < 30.0f) {
                        m_npc->PushCommand(new Player::StartAttackCommand(4));
                        m_state = 1;
                    }
                }
            } else if (horizontal < 30.0f) {
                float time = 0.25f * (vertical / 20.0f);
                new AttackJump(&m_attackJump, m_npc, m_target, false, time);
                m_state = 1;
            }
        } else if (horizontal < 1.0f) {
            if (vertical > 10.0f) {
                new WatchJump(m_npc, weak_ptr<PlayerBase>(), 0.0f);
            }
            m_state = 0;
        }
        break;
    case 3:
    default:
        m_state = 0;
        break;
    }
}

void NpcSonicType::UpdateFollow(float dt) {
    shared_ptr<PlayerBase> leader = fn_8007363C(m_npc.get()).lock();
    if (!leader) {
        return;
    }
    switch (m_state) {
    case 0: {
        Vec3f offset;
        fn_800091EC(&offset, &(const Vec3f &)Sub(&leader->m_position, &m_npc->m_position));
        fn_8040F990(0, &offset, &offset, &(const Vec3f &)fn_80079698(m_npc.get()));
        float distance = fn_8040CF50(&offset);
        if (distance > 40.0f) {
            m_state = 2;
        }
        if (distance < 15.0f) {
            m_state = 2;
            fn_80014E1C(&m_flags, 1);
        }
        FaceLeader(leader);
        break;
    }
    case 1:
        m_state = 0;
        break;
    case 2: {
        Vec3f offset;
        fn_800091EC(&offset, &(const Vec3f &)Sub(&m_destination, &m_npc->m_position));
        Vec3f along;
        fn_8040F990(&along, &offset, &offset, &(const Vec3f &)fn_80079698(m_npc.get()));
        float distance = fn_8040CF50(&offset);
        if (fn_80014E34(&m_flags, 1)) {
            if (distance < 1.0f) {
                fn_8006127C(&m_flags, 1);
            }
        } else if (distance < 5.0f && distance > 0.0f) {
            m_state = 0;
        }
        break;
    }
    case 3:
    default:
        if (fn_8040CF2C(&Fields(leader.get())->m_history->Get(0)->field_0x0) < 0.9f) {
            m_state = 2;
        }
        break;
    }
}

void NpcSonicType::UpdateIdle(float dt) {
    Vec3f offset;
    fn_800091EC(&offset, &(const Vec3f &)Sub(&m_destination, &m_npc->m_position));
    fn_8040F990(0, &offset, &offset, &(const Vec3f &)fn_80079698(m_npc.get()));
    float distance = fn_8040CF50(&offset);
    switch (m_state) {
    case 0:
        if (distance > 40.0f) {
            m_state = 2;
        }
        break;
    case 2:
        if (distance < 1.0f) {
            m_state = 0;
        }
        break;
    case 1:
    case 3:
    default:
        m_state = 0;
        break;
    }
}

void NpcSonicType::Move(float dt) {
    switch (m_state) {
    case 0:
        break;
    case 1:
        WalkIfIdle();
        break;
    case 2:
        Walk();
        break;
    case 3:
        FollowMove(dt);
        break;
    }
}

void NpcSonicType::WalkIfIdle() {
    if (m_npc && !m_attackJump) {
        Walk();
    }
}

void NpcSonicType::Walk() {
    if (!m_npc) {
        return;
    }
    Vec3f direction;
    fn_800091EC(&direction, &(const Vec3f &)Sub(&m_destination, &m_npc->m_position));
    fn_8040F990(0, &direction, &direction, &(const Vec3f &)fn_80079698(m_npc.get()));
    float distance = fn_8040CF50(&direction);
    fn_8040CE74(&direction);
    if (distance < 40.0f) {
        fn_8040CE38(&direction, distance / 40.0f);
    }
    float speed = 0.152344f;
    if (fn_8006CD78(Fields(m_npc.get())->m_stateBits, 2)) {
        if (fn_80014E34(&m_flags, 2)) {
            speed = 1.0f;
        } else if (fn_801E7094(&m_target)) {
            speed = 0.85f;
        }
    }
    if (fn_8040CF2C(&direction) < speed * speed) {
        fn_800091D0(&direction, &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)fn_8040CEE8(&direction), speed));
    }
    m_npc->PushCommand(new AccelerateInputCommand(direction, 4));
}

/* The limits of the follow rate (in .sdata, as above). */
#pragma explicit_zero_data on
float s_clampLow = 0.0f;
float s_clampHigh = 1.0f;
#pragma explicit_zero_data reset

void NpcSonicType::FollowMove(float dt) {
    shared_ptr<PlayerBase> leader = fn_8007363C(m_npc.get()).lock();
    if (!m_npc->IsWeaponHidden() && leader) {
    Vec3f offset;
    fn_800091EC(&offset, &(const Vec3f &)Sub(&leader->m_position, &m_npc->m_position));
    float facing =
        fn_8040CF90(&(const Vec3f &)fn_800796DC(leader.get()), &(const Vec3f &)Normalized(&offset));
    if (facing > 0.26f) {
        float rate = (facing - 0.26f) / 0.74f;
        rate += 0.5f;
        rate = fn_80070460(rate, s_clampLow, s_clampHigh);
        rate = pow(rate, 0.5);
        UnkPlayerRecord_800A849C *record = Fields(leader.get())->m_history->Get(0);
        if (fn_8006CD78(Fields(m_npc.get())->m_stateBits, 2) && fn_8006CD78(record->m_stateBits, 2)) {
            Vec3f velocity;
            fn_800091EC(&velocity,
                        &(const Vec3f &)Sub(&record->field_0x24, &Fields(leader.get())->field_0x100));
            fn_8040CE38(&velocity, rate);
            m_npc->PushCommand(new AccelerateCommand(velocity, 4));
            fn_80093BA0(&shared_ptr<PlayerBase>(m_npc), &(const Vec3f &)fn_8040CEE8(&velocity), 10.0f * dt);
        } else {
            m_npc->PushCommand(new AccelerateInputCommand(record->field_0x0, 4));
        }
        if (fn_8006CD78(Fields(m_npc.get())->m_stateBits, 2) &&
            !fn_80014E34(&Fields(m_npc.get())->field_0xA4, 0)) {
            Vec3f toDestination;
            fn_800091EC(&toDestination, &(const Vec3f &)Sub(&m_destination, &m_npc->m_position));
            fn_8040F990(0, &toDestination, &toDestination, &(const Vec3f &)fn_80079698(m_npc.get()));
            m_npc->PushCommand(new AccelerateCommand(fn_8040CDA8(&toDestination, 25.0f), 4));
        }
    }
    }
}

void NpcSonicType::FaceLeader(const shared_ptr<PlayerBase> &leader) {
    if (!m_npc->IsWeaponHidden() && leader) {
    Vec3f direction;
    fn_800091EC(&direction,
                &(const Vec3f &)fn_8040CEE8(&(const Vec3f &)Sub(&leader->m_position, &m_npc->m_position)));
    Vec3f leaderFront;
    fn_800091EC(&leaderFront, &(const Vec3f &)fn_800796DC(leader.get()));
    Vec3f front;
    fn_800091EC(&front, &(const Vec3f &)fn_800796DC(m_npc.get()));
    float facing = fn_8040CF90(&direction, &(const Vec3f &)fn_800796DC(m_npc.get()));
    if (facing < 0.342f) {
        fn_80014E1C(&m_flags, 0);
    } else if (facing > 0.996f) {
        fn_8006127C(&m_flags, 0);
    }
    if (fn_8040CF90(&front, &direction) < 0.0f && fn_8040CF90(&front, &leaderFront) >= 0.0f) {
        fn_8006127C(&m_flags, 0);
    }
    if (fn_80014E34(&m_flags, 0)) {
        Vec3f turn;
        fn_800091EC(&turn, &(const Vec3f &)fn_8040CDA8(&direction, 0.1f));
        m_npc->PushCommand(new AccelerateInputCommand(turn, 4));
    }
    }
}

} // namespace

/* Makes the control (called by the NPC control factories by its address). */
extern "C" ControlBase *fn_8006DE34(const shared_ptr<PlayerBase> &player) {
    return new NpcSonicType(player);
}
