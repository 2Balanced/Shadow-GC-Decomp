/* Player::Npc::BossSonic::BossSonicExecuter (class name from RTTI): the executer (the task that
 * runs the behaviors) of Boss Sonic. It takes the NPC's commands through a bound callback: a
 * damage command (type 7) switches to the damage behavior unless the NPC is guarding or already
 * damaged, type 0x108 switches to behavior 6, type 8 switches the control to 4, and the AI's
 * commands (0x501, and 0x16 with 0 or 4) set the AI's flag bits 0, 1 and 2. Commands nobody took
 * are rejected back to the sender. It ends the NPC's task when the player is gone. File, member
 * and function names are inferred. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#define MSL_RUNTIME_ERROR_STRING
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/BossSonic.h"
#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/MathTypes.h"
#include "Game/Npc.h"
#include "Game/NpcPartModel.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerExecuter.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

namespace Player {

namespace Behavior {

/* InitializeArgsDamage (class name from RTTI; vtable 0x8051FDEC): switches to the damage
 * behavior (0x1E), knocked along direction. */
class InitializeArgsDamage : public InitializeArgs {
  public:
    InitializeArgsDamage(const Vec3f &direction);
    virtual ~InitializeArgsDamage();

    /* 0x8 */ Vec3f m_direction;
};
SIZE_ASSERT(InitializeArgsDamage, 0x14);

} // namespace Behavior

namespace Control {

/* InitializeArgs (class name from RTTI; vtable 0x8051FC94): the control to switch to. */
class InitializeArgs {
  public:
    InitializeArgs(int id) : m_id(id) {}
    virtual ~InitializeArgs() {}

    /* 0x4 */ s32 m_id;
};
SIZE_ASSERT(InitializeArgs, 0x8);

} // namespace Control
} // namespace Player

extern "C" {
/* Player::Executer's command handling, frame update and behavior switch (see
 * Game/PlayerExecuter.h). */
void fn_800A78FC(Player::Executer *executer, CharCommand *command);
void fn_800A7D78(Player::Executer *executer);
void fn_800A7F14(Player::Executer *executer, const Player::Behavior::InitializeArgs *args);
/* A command reached the player (behaviour inferred). */
void fn_80078D10(Player::PlayerBase *player);
/* The NPC's frame update (behaviour inferred). */
void fn_80073AD0(Player::Npc::Npc *npc);
/* Switches the control of the character task (behaviour inferred). */
void fn_8009EBA4(Task *task, const Player::Control::InitializeArgs *args);
/* Tests a bit of the player's state bits (see Game/Player.h). */
bool fn_8006CD78(u32 *bits, int bit);
}

namespace Player {
namespace Npc {
namespace BossSonic {

using boost::shared_ptr;

/* The player's command unit (at 0x21C) and its weak pointer to itself (at 0x260). */
inline CharCommandUnit *GetCommandUnit(PlayerBase *player) {
    return *reinterpret_cast<CharCommandUnit **>(reinterpret_cast<u8 *>(player) + 0x21C);
}
inline const boost::weak_ptr<PlayerBase> &GetSelf(PlayerBase *player) {
    return *reinterpret_cast<boost::weak_ptr<PlayerBase> *>(reinterpret_cast<u8 *>(player) + 0x260);
}

class BossSonicExecuter : public Executer {
  public:
    BossSonicExecuter(Task *parent, const shared_ptr<Npc> &npc, const shared_ptr<BossSonicWork> &work);
    virtual ~BossSonicExecuter();
    virtual void Tick(float dt);

    void OnCommand(CharCommand &command);

    static BossSonicExecuter *Create(Task *parent, const shared_ptr<Npc> &npc,
                                     const shared_ptr<BossSonicWork> &work);

    /* 0x50 */ shared_ptr<Npc> m_npc;
    /* 0x58 */ shared_ptr<BossSonicWork> m_work;
};
SIZE_ASSERT(BossSonicExecuter, 0x60);

BossSonicExecuter::BossSonicExecuter(Task *parent, const shared_ptr<Npc> &npc, const shared_ptr<BossSonicWork> &work)
    : Executer(parent, shared_ptr<PlayerBase>(npc)), m_npc(npc), m_work(work) {
    m_name = "BossSonicExecuter";
    fn_800729B0(&GetCommandUnit(m_player.get())->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(boost::bind(&BossSonicExecuter::OnCommand, this, _1)));
}

BossSonicExecuter::~BossSonicExecuter() {}

void BossSonicExecuter::Tick(float dt) {
    fn_80073AD0(m_npc.get());
    fn_800A7D78(this);
    if (!m_work || m_work->m_player.expired()) {
        Task *task = *reinterpret_cast<Task **>(reinterpret_cast<u8 *>(fn_8006CA70(&m_npc)) + 0x268);
        task->m_flags |= 1;
    }
}

void BossSonicExecuter::OnCommand(CharCommand &command) {
    if (command.TypeId() != 1) {
        fn_80078D10(m_player.get());
    }
    if (m_behavior != 0) {
        m_behavior->vf_0x18(&command);
    }
    if (command.Flags() & 2) {
        return;
    }
    fn_800A78FC(this, &command);
    if (command.Flags() & 2) {
        return;
    }
    switch (command.TypeId()) {
    case 0x501:
        m_work->m_flags.Set(0);
        break;
    case 0x101:
        command.Flags() |= 2;
        break;
    case 7:
        if (!fn_8006CD78(m_player->m_stateBits, 0x17) && !fn_8006CD78(m_player->m_stateBits, 0x18) &&
            m_behavior->m_id != 0x1E) {
            if (command.TypeId() == 7) {
                Behavior::InitializeArgsDamage args(static_cast<DamageCommand &>(command).m_direction);
                fn_800A7F14(this, &args);
            } else {
                Behavior::InitializeArgsDamage args(Vec3f(0.0f, 1.0f, 0.0f));
                fn_800A7F14(this, &args);
            }
        }
        command.Flags() |= 2;
        break;
    case 8: {
        Control::InitializeArgs args(4);
        fn_8009EBA4(m_player->m_task, &args);
        break;
    }
    case 0x108:
        fn_800A7F14(this, &(const Behavior::InitializeArgs &)Behavior::InitializeArgs(6));
        command.Flags() |= 2;
        break;
    case 0x16:
        switch (*reinterpret_cast<s32 *>(reinterpret_cast<u8 *>(&command) + 0x1C)) {
        case 0:
            m_work->m_flags.Set(1);
            break;
        case 4:
            m_work->m_flags.Set(2);
            break;
        }
        command.Flags() |= 2;
        break;
    }
    if (!(command.Flags() & 2)) {
        command.Reject(boost::weak_ptr<CharInfo>(GetSelf(m_player.get())));
    }
}

BossSonicExecuter *BossSonicExecuter::Create(Task *parent, const shared_ptr<Npc> &npc,
                                             const shared_ptr<BossSonicWork> &work) {
    return new BossSonicExecuter(parent, npc, work);
}

} // namespace BossSonic
} // namespace Npc
} // namespace Player
