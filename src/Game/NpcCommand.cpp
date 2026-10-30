/* Player::Npc::NpcCommand (class name from RTTI; base CharCommand) and the commands derived from it
 * (Player::Npc::EnemyDeadByPlayerCommand, ShowCommand and HideCommand): the commands only NPCs
 * accept. Their Reject functions act on the NPC directly: the enemy count shows the partner's
 * level-up hint, Show switches the NPC's control (greeting the player first for kind 1) and Hide
 * hides it. File name from the base class's RTTI name; function and member names marked
 * "(inferred)" are not original. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include <stdio.h>
#include <string>

#define CHARCOMMAND_IMPLEMENTATION
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/Npc.h"
#include "Game/Player.h"
#include "Game/PlayerControl.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>

using boost::shared_ptr;
using boost::weak_ptr;

extern "C" {
/* a + b for the debug descriptions (see Game/CharCommands.cpp). */
std::string fn_8004F5DC(const std::string &a, const char *b);
/* NPC helpers (behaviour inferred: whether the NPC is shown and may be shown, showing or hiding
 * it, and resetting it). */
bool fn_80073708(Player::Npc::Npc *npc);
bool fn_800736E4(Player::Npc::Npc *npc);
void fn_80073730(Player::Npc::Npc *npc, int shown);
void fn_8007393C(Player::Npc::Npc *npc);
void fn_80073964(Player::Npc::Npc *npc);
/* Switches the control of the character task (see Game/BossSonicExecuter.cpp). */
void fn_8009EBA4(Task *task, const ::Player::Control::InitializeArgs *args);
}

/* The partner's hints (Game/NpcHint.cpp). */
void ShowGreeting(const weak_ptr<Player::PlayerBase> &player);
void ShowLevelUpHint(const shared_ptr<Player::Npc::Npc> &npc, int kind, int count);

namespace Player {
namespace Npc {

NpcCommand::NpcCommand(u16 type, int priority) : CharCommand(type, priority) {}

NpcCommand::~NpcCommand() {}

HideCommand::HideCommand() : NpcCommand(0x401, 4) {}

HideCommand::~HideCommand() {}

void HideCommand::Reject(const weak_ptr<CharInfo> &receiver) {
    if (shared_ptr<CharInfo> character = receiver.lock()) {
        shared_ptr<Npc> npc = boost::dynamic_pointer_cast<Npc>(character);
        if (fn_80073708(npc.get())) {
            fn_80073730(npc.get(), 0);
            fn_80073964(npc.get());
        }
        Flags() |= 2;
    }
}

const char *HideCommand::GetName() {
    return "Hide";
}

ShowCommand::ShowCommand(int arg0, int arg1) : NpcCommand(0x402, arg1) {
    field_0x14 = arg0;
}

ShowCommand::~ShowCommand() {}

void ShowCommand::Reject(const weak_ptr<CharInfo> &receiver) {
    if (shared_ptr<CharInfo> character = receiver.lock()) {
        shared_ptr<Npc> npc = boost::dynamic_pointer_cast<Npc>(character);
        switch (field_0x14) {
        case 0:
            fn_8009EBA4(npc->m_task, &::Player::Control::InitializeArgs(2));
            fn_8007393C(npc.get());
            break;
        case 1:
            if (!fn_80073708(npc.get()) && fn_800736E4(npc.get())) {
                fn_80073730(npc.get(), 1);
                fn_8009EBA4(npc->m_task, &::Player::Control::InitializeArgs(4));
                ShowGreeting(npc);
            }
            break;
        case 2:
            if (fn_800736E4(npc.get())) {
                fn_80073730(npc.get(), 1);
                fn_8009EBA4(npc->m_task, &::Player::Control::InitializeArgs(4));
            }
            break;
        }
        Flags() |= 2;
    }
}

const char *ShowCommand::GetName() {
    return "Show";
}

EnemyDeadByPlayerCommand::EnemyDeadByPlayerCommand(int kind, int count) : NpcCommand(0x403, 4) {
    m_kind = kind;
    m_count = count;
}

EnemyDeadByPlayerCommand::~EnemyDeadByPlayerCommand() {}

void EnemyDeadByPlayerCommand::Reject(const weak_ptr<CharInfo> &receiver) {
    if (shared_ptr<CharInfo> character = receiver.lock()) {
        shared_ptr<Npc> npc = boost::dynamic_pointer_cast<Npc>(character);
        ShowLevelUpHint(npc, m_kind, m_count);
        Flags() |= 2;
    }
}

const char *EnemyDeadByPlayerCommand::GetName() {
    return "EnemyDeadByPlayer";
}

std::string EnemyDeadByPlayerCommand::vf_0x10() const {
    char text[0x44];
    sprintf(text, " Group %d Count %d", m_kind, m_count);
    return fn_8004F5DC(CharCommand::vf_0x10(), text);
}

} // namespace Npc
} // namespace Player
