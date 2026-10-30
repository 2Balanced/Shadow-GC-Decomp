/* Player::Npc::FlyType::FlyTypeExecuter (class name from RTTI): the executer (the task that runs
 * the behaviors) of the flying NPCs. It takes the NPC's commands through a bound callback: command
 * 0x11B is handed back to the player the NPC follows, while there is one; a damage command (type 7)
 * switches to the damage behavior, and types 8 and 0x101 are taken without effect. Commands nobody
 * took are rejected back to the sender. File, member and function names are inferred. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first). */
#define MSL_RUNTIME_ERROR_STRING
#define PLAYER_EXECUTER_MEMBERS
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
/* The NPC's frame update (behaviour inferred). */
void fn_80073AD0(Player::Npc::Npc *npc);
}

/* The player the NPC follows (see Game/NpcControlRestartFollow.cpp). */
extern "C" boost::weak_ptr<Player::PlayerBase> fn_8007363C(Player::Npc::Npc *npc);

namespace Player {
namespace Npc {
namespace FlyType {

using boost::shared_ptr;

/* The player's command unit (at 0x21C) and its weak pointer to itself (at 0x260). */
inline CharCommandUnit *GetCommandUnit(PlayerBase *player) {
    return *reinterpret_cast<CharCommandUnit **>(reinterpret_cast<u8 *>(player) + 0x21C);
}
inline const boost::weak_ptr<PlayerBase> &GetSelf(PlayerBase *player) {
    return *reinterpret_cast<boost::weak_ptr<PlayerBase> *>(reinterpret_cast<u8 *>(player) + 0x260);
}

class FlyTypeExecuter : public Executer {
  public:
    FlyTypeExecuter(Task *parent, const shared_ptr<Npc> &npc);
    virtual ~FlyTypeExecuter();
    virtual void Tick(float dt);

    void OnCommand(CharCommand &command);

    static FlyTypeExecuter *Create(Task *parent, const shared_ptr<Npc> &npc);

    /* 0x50 */ shared_ptr<Npc> m_npc;
};
SIZE_ASSERT(FlyTypeExecuter, 0x58);

FlyTypeExecuter::FlyTypeExecuter(Task *parent, const shared_ptr<Npc> &npc)
    : Executer(parent, shared_ptr<PlayerBase>(npc)), m_npc(npc) {
    m_name = "Npc::FlyTypeExecuter";
    fn_800729B0(&GetCommandUnit(m_player.get())->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(boost::bind(&FlyTypeExecuter::OnCommand, this, _1)));
}

FlyTypeExecuter::~FlyTypeExecuter() {}

void FlyTypeExecuter::Tick(float dt) {
    fn_80073AD0(m_npc.get());
    fn_800A7D78(this);
}

void FlyTypeExecuter::OnCommand(CharCommand &command) {
    u32 type = command.TypeId();
    if (type != 1 && type != 6) {
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
    case 7: {
        Behavior::InitializeArgsDamage args(dynamic_cast<DamageCommand &>(command).m_direction);
        fn_800A7F14(this, &args);
    }
        command.Flags() |= 2;
        break;
    case 8:
        command.Flags() |= 2;
        break;
    }
    if (!(command.Flags() & 2)) {
        command.Reject(boost::weak_ptr<CharInfo>(GetSelf(m_player.get())));
        /* Retail tests the flag once more here, with nothing left to skip. */
        if (command.Flags() & 2) {
            return;
        }
    }
}

FlyTypeExecuter *FlyTypeExecuter::Create(Task *parent, const shared_ptr<Npc> &npc) {
    return new FlyTypeExecuter(parent, npc);
}

} // namespace FlyType
} // namespace Npc
} // namespace Player
