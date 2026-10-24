/* Player::Behavior::PickupObject (class name from RTTI): behavior 0x15 of Shadow, picking up an
 * object (Player::Behavior::InitializeArgsPickupObject names the target the object publishes).
 * Entering it plays the pickup motion, listens to the motion's events and hands the object to the
 * player (Player::Player keeps it). When the motion reaches the grab (event 0x5000's value 1) the
 * player sends the object a PickupCommand and calls out; command 0xE (the object is held) rumbles
 * the controller, and command 0xF, losing the object or fn_800A9E1C ends it (behavior 0). File name
 * inferred from the class (as Game/PlayerBehaviorSuper.cpp); all other names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). Where the game reloads the player pointer in a call's arguments after another argument's
 * call, the code reads m_player.px (as in Game/PlayerBehaviorGrind.cpp). */
/* Declares Player::Behavior::Behavior's slots with their arguments (see Game/PlayerBehavior.h). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
/* shared_ptr's header first: CodeWarrior emits the unit's function template instances (here
 * dynamic_pointer_cast before bind) in the order their templates are declared. */
#include "boost/shared_ptr.hpp"

#include <boost/bind.hpp>
#include <boost/function.hpp>

/* Declares Player::PlayerBase's slot 0x40 with its parameter (see Game/Player.h). */
#define PLAYER_PICKUP_SLOT

#include "Game/CharCommand.h"
#include "Game/MathTypes.h"
#include "Game/PickupCharInfo.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerMotionEvent.h"
#include "Game/PlayerSoundBase.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/Vibration.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* Target (class name from RTTI; see Game/CharCommand.h): what a character aims at to pick an object
 * up. Only what this unit uses is declared (as in Game/PlayerBehaviorHomingAttack.cpp). */
class Target {
  public:
    /* The point to aim at, as seen by viewer (vtable slot 0x8). */
    virtual Vec3f GetPosition(CharInfo *viewer);

    /* 0x04 */ u8 field_0x4[0x28 - 0x4];
    /* 0x28 */ boost::weak_ptr<CharInfo> m_owner; /* the character the target belongs to */
    /* 0x30 */ u32 m_flags;                       /* bit 0: it can be locked on to */
};

/* The player's members read here (struct name unknown; Player::PlayerBase leaves them
 * unmapped). */
struct UnkPlayerFields_8008BF30 {
    /* 0x000 */ u8 field_0x0[0xE8];
    /* 0x0E8 */ Vec3f m_jumpUp; /* the direction jumps go (as in Game/PlayerBehaviorHomingAttack.cpp) */
};

extern "C" {
/* A 32-bit flag word: fn_80014E04 constructs it cleared, fn_80014E10 clears it, fn_80014E1C sets
 * bit and fn_80014E34 tests bit (see Game/BAMine.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
/* The player's state bits (see Game/PlayerShadowSuper.cpp). */
void fn_80076CA8(u32 *bits, int bit);
/* Counted holds on the player: fn_8007922C raises the counters at 0x88 and 0x90 and sets state bits
 * 0x2A and 0x2E, fn_800791BC lowers them and clears each bit when its counter reaches 0 (behaviour
 * from their bodies; meaning unknown). */
void fn_8007922C(Player::PlayerBase *player);
void fn_800791BC(Player::PlayerBase *player);
/* offset turned by the player's rotation (behaviour from its body). */
Vec3f fn_80079628(Player::PlayerBase *player, const Vec3f *offset);
/* Switches a player's behavior (behaviour inferred). */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);
/* Tests bit 3 of the player's word at 0xB4 (meaning unknown; as in AirAttack). */
bool fn_800A9E1C(boost::shared_ptr<Player::PlayerBase> *player);
/* Rumbles player's controller with pattern (see Game/PlayerBehaviorHomingJump.cpp). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int player);

Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b); /* a + b */
}

/* The player's members above (a macro, as in Game/PlayerBehaviorSuper.cpp). */
#define FieldsOf(player) reinterpret_cast<UnkPlayerFields_8008BF30 *>(player)

/* a + b (an inline wrapper, which orders the command's arguments as the game does). */
inline Vec3f Add(const Vec3f &a, const Vec3f &b) {
    return fn_8040CDD0(&a, &b);
}

namespace Player {
namespace Behavior {

/* The behavior's flag word (struct name unknown): bit 0 the grab (event 0x5000's value 1), bit 1
 * the object held (command 0xE), bit 2 command 0xF, bit 3 the end. */
struct PickupObjectFlags {
    PickupObjectFlags() {
        fn_80014E04(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }
    void Reset() {
        fn_80014E10(&m_bits);
    }

    u32 m_bits;
};

class PickupObject : public Behavior {
  public:
    PickupObject(const boost::shared_ptr<PlayerBase> &player);
    virtual ~PickupObject();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x18(CharCommand *command);
    virtual void Update(float dt);
    virtual void vf_0x20();

    /* The factory record's function (name inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);

    void OnMotion(MotionEventArgs &args);

    static const char *s_name;

    /* 0x1C */ boost::weak_ptr<PickupCharInfo> m_object;
    /* 0x24 */ PickupObjectFlags m_flags;
    /* 0x28 */ s32 m_step; /* 0 until the grab, 1 until the object is held */
};
SIZE_ASSERT(PickupObject, 0x2C);

const char *PickupObject::s_name = "PickupObject";

/* Where the player holds the object, before turning it by the player's rotation. */
static Vec3f s_holdOffset(1.68f, 3.394f, 0.117f);

PickupObject::PickupObject(const boost::shared_ptr<PlayerBase> &player)
    : Behavior(player, s_name, 0x15) {}

PickupObject::~PickupObject() {}

bool PickupObject::vf_0x0C(int prevId, const InitializeArgs &args) {
    const InitializeArgsPickupObject &pickup =
        dynamic_cast<const InitializeArgsPickupObject &>(args);
    if (pickup.m_target.expired()) {
        return false;
    }
    boost::shared_ptr<Target> target = pickup.m_target.lock();
    boost::shared_ptr<CharInfo> owner = target->m_owner.lock();
    boost::shared_ptr<PickupCharInfo> object = boost::dynamic_pointer_cast<PickupCharInfo>(owner);
    if (!object) {
        return false;
    }
    return true;
}

void PickupObject::Enter(int prevId, const InitializeArgs &args) {
    const InitializeArgsPickupObject &pickup =
        dynamic_cast<const InitializeArgsPickupObject &>(args);
    fn_8007922C(m_player.get());
    m_player.get()->m_motion->vf_0x10(0x19, 5.0f);
    PlayerMotionEventCallback(m_player.get()) =
        MotionEventCallback(boost::bind(&PickupObject::OnMotion, this, _1));
    boost::shared_ptr<Target> target = pickup.m_target.lock();
    boost::shared_ptr<CharInfo> owner = target->m_owner.lock();
    m_object = boost::dynamic_pointer_cast<PickupCharInfo>(owner);
    m_flags.Reset();
    m_step = 0;
    m_player.get()->vf_0x40(m_object);
}

void PickupObject::Leave(int nextId) {
    fn_800791BC(m_player.get());
    PlayerMotionEventCallback(m_player.get()).Clear();
    m_object.reset();
    if (!m_flags.Test(1)) {
        m_player.get()->vf_0x44();
    }
}

void PickupObject::vf_0x18(CharCommand *command) {
    switch (command->TypeId()) {
    case 0xE:
        m_flags.Set(1);
        command->Flags() |= 2;
        break;
    case 0xF:
        m_flags.Set(2);
        command->Flags() |= 2;
        break;
    }
}

void PickupObject::Update(float dt) {
    if (m_object.expired()) {
        m_flags.Set(3);
    } else {
        boost::shared_ptr<CharInfo> object = m_object.lock();
        switch (m_step) {
        case 0:
            if (m_flags.Test(0)) {
                object->PushCommand(new PickupCommand(
                    m_player.px->m_self,
                    Add(m_player.px->m_position, fn_80079628(m_player.get(), &s_holdOffset)),
                    0.4f, FieldsOf(m_player.get())->m_jumpUp));
                m_player.get()->m_sound->StopVoice(0x16);
                m_step++;
            }
            break;
        case 1:
            if (m_flags.Test(1)) {
                fn_8034DF48(6, m_player.get()->field_0x54);
                m_step++;
            }
            break;
        }
        if (fn_800A9E1C(&m_player)) {
            fn_80076CA8(m_player.get()->m_stateBits, 0x3D);
            m_flags.Set(3);
        }
    }
    if (m_flags.Test(2) || m_flags.Test(3)) {
        fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(0));
    }
}

void PickupObject::OnMotion(MotionEventArgs &args) {
    switch (args.m_event) {
    case 0x5000:
        switch (*args.m_value) {
        case 1:
            m_flags.Set(0);
            break;
        }
        break;
    }
}

void PickupObject::vf_0x20() {}

Behavior *PickupObject::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new PickupObject(player);
}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds PickupObject by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804B0AF8 = {
    Player::Behavior::PickupObject::Create, 0, 0};
