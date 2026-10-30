/* Player::Npc::SonicType::SonicTypeExecuter (class name from RTTI; bases Task and
 * Player::Executer; its name is "Npc::SonicTypeExecuter"): the executer (the task that runs the
 * behaviors) of a Sonic-type NPC. It takes the NPC's commands through a bound callback: command
 * 0x11B is rejected back to the player the NPC follows, a damage command (type 7) switches to the
 * damage behavior unless the NPC is guarding or already damaged, type 0x108 switches to behavior
 * 6, and types 8 and 0x101 are dropped. Commands nobody took are rejected back to the sender. The
 * unit also holds the first copies of shared helpers: the behavior arguments' destructors,
 * Behavior's empty command slot, and the command unit's callback assignment (fn_800729B0). File,
 * member and function names are inferred (as in Game/BossSonicExecuter.cpp). Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#define MSL_RUNTIME_ERROR_STRING
#define CHARCOMMAND_IMPLEMENTATION
#include <string>
#define PLAYER_EXECUTER_MEMBERS
#define PLAYER_BEHAVIOR_ARGS_DAMAGE_INLINE
#define PLAYER_BEHAVIOR_COMMAND_SLOT_INLINE
#include <algorithm>
#include <boost/function/function_base.hpp>

/* std::swap for boost::function's functor pointer: the game's one shared copy (0x80019164) is
 * called (as in Game/Bomb.cpp). */
namespace std {
void swap(boost::detail::function::any_pointer &a, boost::detail::function::any_pointer &b);
} // namespace std

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/MathTypes.h"
#include "Game/Npc.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerExecuter.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

extern "C" {
/* Player::Executer's command handling, frame update and behavior switch (see
 * Game/PlayerExecuter.h). */
void fn_800A78FC(Player::Executer *executer, CharCommand *command);
void fn_800A7D78(Player::Executer *executer);
void fn_800A7F14(Player::Executer *executer, const Player::Behavior::InitializeArgs *args);
/* A command reached the player (behaviour inferred). */
void fn_80078D10(Player::PlayerBase *player);
/* The NPC's frame update and the player it follows (see Game/NpcControlRestartFollow.cpp). */
void fn_80073AD0(Player::Npc::Npc *npc);
boost::weak_ptr<Player::PlayerBase> fn_8007363C(Player::Npc::Npc *npc);
/* Tests a bit of the player's state bits (see Game/Player.h). */
bool fn_8006CD78(u32 *bits, int bit);
}

/* Assigns source to callback (other units call it by its address; see Game/CharCommandUnit.h). */
extern "C" inline UnkCharCommandCallback_801DA7C0 *fn_800729B0(UnkCharCommandCallback_801DA7C0 *callback,
                                                               const CharCommandCallback *source) {
    callback->m_function = *source;
    return callback;
}

namespace Player {

namespace Npc {
namespace SonicType {

using boost::shared_ptr;

/* The player's command unit (at 0x21C) and its weak pointer to itself (at 0x260). */
inline CharCommandUnit *GetCommandUnit(PlayerBase *player) {
    return *reinterpret_cast<CharCommandUnit **>(reinterpret_cast<u8 *>(player) + 0x21C);
}
inline const boost::weak_ptr<PlayerBase> &GetSelf(PlayerBase *player) {
    return *reinterpret_cast<boost::weak_ptr<PlayerBase> *>(reinterpret_cast<u8 *>(player) + 0x260);
}

class SonicTypeExecuter : public Executer {
  public:
    SonicTypeExecuter(Task *parent, const shared_ptr<Npc> &npc);
    virtual ~SonicTypeExecuter();
    virtual void Tick(float dt);

    void OnCommand(CharCommand &command);

    static SonicTypeExecuter *Create(Task *parent, const shared_ptr<Npc> &npc);

    /* 0x50 */ shared_ptr<Npc> m_npc;
};
SIZE_ASSERT(SonicTypeExecuter, 0x58);

SonicTypeExecuter::SonicTypeExecuter(Task *parent, const shared_ptr<Npc> &npc)
    : Executer(parent, shared_ptr<PlayerBase>(npc)), m_npc(npc) {
    m_name = "Npc::SonicTypeExecuter";
    fn_800729B0(&GetCommandUnit(m_player.get())->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(boost::bind(&SonicTypeExecuter::OnCommand, this, _1)));
}

SonicTypeExecuter::~SonicTypeExecuter() {}

void SonicTypeExecuter::Tick(float dt) {
    fn_80073AD0(m_npc.get());
    fn_800A7D78(this);
}

void SonicTypeExecuter::OnCommand(CharCommand &command) {
    if (command.m_type != 1 && command.m_type != 6) {
        fn_80078D10(m_player.get());
    }
    switch (command.TypeId()) {
    case 0x11B:
        if (!fn_8007363C(m_npc.get()).expired()) {
            command.Reject(boost::weak_ptr<CharInfo>(fn_8007363C(m_npc.get())));
            command.Flags() |= 2;
        }
        break;
    }
    if (command.Flags() & 2) {
        return;
    }
    if (m_behavior) {
        shared_ptr<Behavior::Behavior> behavior = m_behavior;
        behavior->vf_0x18(&command);
    }
    if (command.Flags() & 2) {
        return;
    }
    fn_800A78FC(this, &command);
    if (command.Flags() & 2) {
        return;
    }
    switch (command.TypeId()) {
    case 0x101:
        command.Flags() |= 2;
        break;
    case 7:
        if (!fn_8006CD78(m_player->m_stateBits, 0x17) && !fn_8006CD78(m_player->m_stateBits, 0x18) &&
            m_behavior->m_id != 0x1E) {
            Behavior::InitializeArgsDamage args(dynamic_cast<DamageCommand &>(command).m_direction);
            fn_800A7F14(this, &args);
        }
        command.Flags() |= 2;
        break;
    case 8:
        command.Flags() |= 2;
        break;
    case 0x108:
        fn_800A7F14(this, &(const Behavior::InitializeArgs &)Behavior::InitializeArgs(6));
        command.Flags() |= 2;
        break;
    }
    if (command.Flags() & 2) {
        return;
    }
    command.Reject(boost::weak_ptr<CharInfo>(GetSelf(m_player.get())));
    if (command.Flags() & 2) {
        return;
    }
}

SonicTypeExecuter *SonicTypeExecuter::Create(Task *parent, const shared_ptr<Npc> &npc) {
    return new SonicTypeExecuter(parent, npc);
}

} // namespace SonicType
} // namespace Npc
} // namespace Player
