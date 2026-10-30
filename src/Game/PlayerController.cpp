/* Player::Controller (class name from RTTI): the task that runs a player's control (the pad's
 * Player::Control::UserInput or an AI) every frame. Its state (a separately allocated Impl; class
 * name unknown) holds the current control and the factory that makes the controls by id; every
 * frame it updates the pad, counts down the player's timers, turns the hits and deaths the
 * player's collision recorded into DamageCommand and DeadCommand, puts the player on the path of a
 * circuit it reached (SetLoopPathCommand, GrindCommand), stops a weapon the player may no longer
 * use (CancelUseWeaponCommand, class name from RTTI) and ticks the control. The player points back
 * at the task (word 0x224). File, member and function names are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
/* The inline constructors of the commands the controller sends (see Game/CharCommand.h). */
#define CHARCOMMAND_PLAYER_CONTROLLER
/* ControlBase's slots with the arguments the controller passes (see Game/PlayerControl.h). */
#define PLAYER_CONTROL_SLOT_ARGS

#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/Player.h"
#include "Game/PlayerControl.h"
#include "Game/PlayerFactory.h"
#include "Game/PlayerPadInput.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/Task.h"
#include "Game/UnkCircuitTarget_800ABCE8.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Player::PlayerBase;
using Player::Control::ControlBase;
using Player::Control::InitializeArgs;

/* The player's collision handle (see Game/PlayerCharaColliImpl.cpp). */
struct UnkPlayerPart_80095BD4;

extern "C" {
/* A 32-bit flag word, and the player's state bits (see Game/PlayerCharaColliImpl.cpp). */
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_8006CD78(u32 *bits, int bit);
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* The player's pad input (see Game/PlayerPadInput.h). */
UnkPlayerInput_800A85B0 *fn_8006F8D0(PlayerBase *player);
/* The character the player's collision found nearest (Game/PlayerCharaColliImpl.cpp). */
boost::weak_ptr<CharInfo> fn_80095A10(UnkPlayerPart_80095BD4 *colli);
/* Whether the player has landed (see Game/PlayerBehaviorDamage.cpp). */
bool fn_800B7B58(const boost::shared_ptr<PlayerBase> *player);
/* The ends of the player's states that the timers below run (behaviour inferred). */
void fn_80079074(PlayerBase *player);
void fn_800791BC(PlayerBase *player);
void fn_8007922C(PlayerBase *player);
/* Ends what a task acts out (see Game/PlayerCommands.cpp). */
void fn_80077360(Task *task);
/* The circuit path the player reached: its kind, and whether the player is on it (behaviour
 * inferred). */
int fn_800ABB48(UnkCircuitTarget_800ABCE8 *path);
int fn_800ABBD8(UnkCircuitTarget_800ABCE8 *path, const boost::shared_ptr<PlayerBase> *player);
void fn_8007684C(UnkCircuitTarget_800ABCE8 *path);
}

/* The player's members used here (struct name unknown; Player::PlayerBase leaves them
 * unmapped; names inferred). */
struct UnkPlayerFields_8009EB50 {
    /* 0x000 */ u8 field_0x0[0x8C];
    /* 0x08C */ float m_timer8C;
    /* 0x090 */ u8 field_0x90[0x94 - 0x90];
    /* 0x094 */ float m_timer94;
    /* 0x098 */ float m_timer98;
    /* 0x09C */ float m_timer9C;
    /* 0x0A0 */ u32 m_flags;
    /* 0x0A4 */ u32 m_touched;
    /* 0x0A8 */ u32 m_stateBits[2];
    /* 0x0B0 */ u8 field_0xB0[0x248 - 0xB0];
    /* 0x248 */ UnkPlayerPart_80095BD4 *m_colli;
};

#define Fields(player) reinterpret_cast<UnkPlayerFields_8009EB50 *>(player)

namespace Player {

/* CancelUseWeaponCommand (class name from RTTI): command 0x116, which makes the player stop
 * using its weapon. Its constructor is inline (the type accessor keeps it out of line). */
class CancelUseWeaponCommand : public PlayerCommand {
  public:
    CancelUseWeaponCommand(int arg1) : PlayerCommand(CommandId(0x116), arg1) {}
    virtual ~CancelUseWeaponCommand();

    static int CommandId(int id) {
        return id;
    }
};
SIZE_ASSERT(CancelUseWeaponCommand, 0x14);

class Controller : public Task {
  public:
    class Impl;

    Controller(Task *parent, const boost::shared_ptr<PlayerBase> &player);
    virtual ~Controller();
    virtual void Tick(float dt);

    /* 0x28 */ boost::scoped_ptr<Impl> m_impl;
};
SIZE_ASSERT(Controller, 0x2C);

class Controller::Impl {
  public:
    Impl(const boost::shared_ptr<PlayerBase> &player);
    ~Impl();

    void Update(float dt);
    void SetFactory(const boost::shared_ptr<Control::Factory> &factory);
    bool SwitchControl(const InitializeArgs &args);
    boost::shared_ptr<ControlBase> GetControl() const;
    bool OnPath();
    bool IsUsingWeapon();
    void UpdateTimers(float dt);
    void CheckDamage();
    void CheckDead();

    UnkCircuitTarget_800ABCE8 *GetPath() {
        return &m_path;
    }

    /* 0x00 */ boost::shared_ptr<PlayerBase> m_player;
    /* 0x08 */ boost::shared_ptr<ControlBase> m_control;
    /* 0x10 */ UnkCircuitTarget_800ABCE8 m_path; /* the circuit path the player reached */
    /* 0x18 */ boost::shared_ptr<Control::Factory> m_factory;
};
SIZE_ASSERT(Controller::Impl, 0x20);

} // namespace Player

using Player::Controller;

/* boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out of line; this unit
 * emits the copy for ControlBase (boost 1.32's definition; as in Game/AdvFolder.cpp). */
namespace boost {
template <> inline void shared_ptr<ControlBase>::reset() {
    this_type().swap(*this);
}
} // namespace boost

/* Deletes the state (name inferred; see checked_delete below). */
inline void DeleteImpl(Controller::Impl *impl) {
    delete impl;
}

/* Retail keeps boost::checked_delete<Impl> out of line and ~Impl at its own place: the
 * specialization calls an inline helper (as in Game/PlayerCharaColliMove.cpp; see
 * docs/research-notes.md). */
namespace boost {
template <> inline void checked_delete<Controller::Impl>(Controller::Impl *x) {
    DeleteImpl(x);
}
} // namespace boost

namespace Player {

CancelUseWeaponCommand::~CancelUseWeaponCommand() {}

Controller::Impl::Impl(const boost::shared_ptr<PlayerBase> &player) : m_player(player) {}

Controller::Impl::~Impl() {}

/* Updates the pad, the player's timers, hits and deaths, the circuit path and the weapon, then
 * the control (name inferred). */
void Controller::Impl::Update(float dt) {
    float time = **reinterpret_cast<float **>(m_player->field_0x60);
    fn_8006F8D0(m_player.get())->Update(dt);
    UpdateTimers(time);
    CheckDamage();
    CheckDead();
    if (OnPath()) {
        switch (fn_800ABB48(&m_path)) {
        case 1:
            m_player->PushCommand(new SetLoopPathCommand(m_path, 2));
            break;
        case 2:
        case 3:
            m_player->PushCommand(new GrindCommand(m_path, 2));
            break;
        }
    }
    if (!fn_80014E34(&Fields(m_player.get())->m_flags, 7) && fn_8006CD78(Fields(m_player.get())->m_stateBits, 0x19) &&
        IsUsingWeapon()) {
        m_player->PushCommand(new CancelUseWeaponCommand(2));
    }
    if (m_control.get() != 0) {
        boost::shared_ptr<ControlBase> control = m_control;
        control->vf_0x14(time);
    }
}

void Controller::Impl::SetFactory(const boost::shared_ptr<Control::Factory> &factory) {
    m_factory = factory;
}

/* Leaves the current control and enters the one args names (name inferred). */
bool Controller::Impl::SwitchControl(const InitializeArgs &args) {
    int previous = 0;
    boost::shared_ptr<ControlBase> control = m_factory->Create(args.m_id);
    if (m_control.get() != 0) {
        previous = m_control->field_0x10;
        m_control->Leave(args.m_id);
        m_control.reset();
    }
    m_control = control;
    if (m_control.get() != 0) {
        m_control->Enter(previous, args);
    }
    return true;
}

/* The current control (name inferred). Nothing calls it, and the linker drops it; it is emitted
 * before SwitchControl, and with it the shared_ptr copy constructor, which retail has there. */
boost::shared_ptr<ControlBase> Controller::Impl::GetControl() const {
    return m_control;
}

/* Whether the player has reached a circuit path it should take (name inferred). */
bool Controller::Impl::OnPath() {
    if (fn_8006CD78(Fields(m_player.get())->m_stateBits, 0x32)) {
        if (fn_800ABBD8(&m_path, &m_player) == 0) {
            fn_80076CC8(Fields(m_player.get())->m_stateBits, 0x32);
        }
        return false;
    }
    if (fn_8006CD78(Fields(m_player.get())->m_stateBits, 0x31)) {
        fn_8007684C(&m_path);
    } else {
        switch (fn_800ABBD8(&m_path, &m_player)) {
        case 1:
        case 2:
        case 3:
        case 4:
            return true;
        }
    }
    return false;
}

/* Whether the player holds a weapon of a kind it uses (name inferred). */
bool Controller::Impl::IsUsingWeapon() {
    WeaponBase *weapon = m_player.get()->m_weapon;
    if (weapon != 0) {
        switch (weapon->GetWeaponKind()) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 6:
            return true;
        }
    }
    return false;
}

/* Counts the player's timers down by dt and ends the states they ran (names inferred). */
void Controller::Impl::UpdateTimers(float dt) {
    if (Fields(m_player.get())->m_timer8C > 0.0f) {
        Fields(m_player.get())->m_timer8C -= dt;
        if (Fields(m_player.get())->m_timer8C < 0.0f) {
            Fields(m_player.get())->m_timer8C = 0.0f;
        }
    } else if (fn_8006CD78(Fields(m_player.get())->m_stateBits, 0x2B)) {
        fn_80076CC8(Fields(m_player.get())->m_stateBits, 0x2B);
        fn_800791BC(m_player.get());
    }
    if (fn_80014E34(&Fields(m_player.get())->m_touched, 9)) {
        if (!fn_8006CD78(Fields(m_player.get())->m_stateBits, 0x2C)) {
            fn_80076CA8(Fields(m_player.get())->m_stateBits, 0x2C);
            fn_8007922C(m_player.get());
        }
    } else if (fn_8006CD78(Fields(m_player.get())->m_stateBits, 0x2C)) {
        fn_80076CC8(Fields(m_player.get())->m_stateBits, 0x2C);
        fn_800791BC(m_player.get());
    }
    if (Fields(m_player.get())->m_timer94 > 0.0f) {
        Fields(m_player.get())->m_timer94 -= dt;
        if (Fields(m_player.get())->m_timer94 < 0.0f) {
            Fields(m_player.get())->m_timer94 = 0.0f;
        }
    } else if (fn_8006CD78(Fields(m_player.get())->m_stateBits, 0x2F)) {
        fn_80076CC8(Fields(m_player.get())->m_stateBits, 0x2F);
        fn_80079074(m_player.get());
    }
    if (fn_800B7B58(&m_player) || fn_8006CD78(Fields(m_player.get())->m_stateBits, 2)) {
        if (fn_8006CD78(Fields(m_player.get())->m_stateBits, 0x2D)) {
            fn_80076CC8(Fields(m_player.get())->m_stateBits, 0x2D);
            fn_800791BC(m_player.get());
        }
        if (fn_8006CD78(Fields(m_player.get())->m_stateBits, 0x30)) {
            fn_80076CC8(Fields(m_player.get())->m_stateBits, 0x30);
            fn_80079074(m_player.get());
        }
    }
    {
        s32 kind = 0;
        boost::shared_ptr<CharInfo> nearest = fn_80095A10(Fields(m_player.get())->m_colli).lock();
        if (nearest.get() != 0) {
            kind = nearest.get()->GetKind();
        }
        if (kind != 0x34 && Fields(m_player.get())->m_timer98 > 0.0f) {
            Fields(m_player.get())->m_timer98 -= dt;
            if (Fields(m_player.get())->m_timer98 <= 0.0f) {
                Fields(m_player.get())->m_timer98 = 0.0f;
            }
        }
    }
    if (Fields(m_player.get())->m_timer9C > 0.0f) {
        Fields(m_player.get())->m_timer9C -= dt;
        if (Fields(m_player.get())->m_timer9C <= 0.0f) {
            Fields(m_player.get())->m_timer9C = 0.0f;
        }
    }
}

/* Hurts the player when its collision recorded a hit (name inferred). */
void Controller::Impl::CheckDamage() {
    bool hit = false;
    if (fn_80014E34(&Fields(m_player.get())->m_touched, 1)) {
        fn_8006127C(&Fields(m_player.get())->m_touched, 1);
        hit = true;
    }
    if (hit) {
        m_player->PushCommand(new DamageCommand(4, 10.0f));
    }
}

/* Tells the player it died when its collision recorded a fall or a crush (name inferred). */
void Controller::Impl::CheckDead() {
    bool dead = false;
    if (fn_80014E34(&Fields(m_player.get())->m_touched, 2)) {
        fn_8006127C(&Fields(m_player.get())->m_touched, 2);
        fn_80076CC8(Fields(m_player.get())->m_stateBits, 0x13);
        dead = true;
    }
    if (fn_80014E34(&Fields(m_player.get())->m_touched, 3)) {
        fn_8006127C(&Fields(m_player.get())->m_touched, 3);
        fn_80076CA8(Fields(m_player.get())->m_stateBits, 0x13);
        dead = true;
    }
    if (dead) {
        m_player->PushCommand(new DeadCommand(4));
    }
}

Controller::Controller(Task *parent, const boost::shared_ptr<PlayerBase> &player)
    : Task(parent), m_impl(new Impl(player)) {
    m_name = "Controller";
    m_impl->m_player->m_task = this;
}

Controller::~Controller() {
    m_impl->m_player->m_task = 0;
}

void Controller::Tick(float dt) {
    m_impl->Update(dt);
}

} // namespace Player

extern "C" {

/* Ends what the controller's tasks act out (behaviour inferred; see Game/PlayerCommands.cpp). */
void fn_8009EBEC(Controller *controller) {
    for (Task *task = controller->m_firstChild; task != 0; task = task->m_nextSibling) {
        fn_80077360(task);
    }
}

/* Gives the controller the factory of its controls (behaviour inferred). */
void fn_8009EBC8(Controller *controller, const boost::shared_ptr<Player::Control::Factory> &factory) {
    controller->m_impl->SetFactory(factory);
}

/* Switches the player's control (behaviour inferred). */
bool fn_8009EBA4(Controller *controller, const InitializeArgs *args) {
    return controller->m_impl->SwitchControl(*args);
}

/* Whether the player holds a weapon of a kind it uses (behaviour inferred). */
bool fn_8009EB80(Controller *controller) {
    return controller->m_impl->IsUsingWeapon();
}

/* The circuit path the player reached (behaviour inferred). */
UnkCircuitTarget_800ABCE8 *fn_8009EB50(Controller *controller) {
    return controller->m_impl->GetPath();
}
}
