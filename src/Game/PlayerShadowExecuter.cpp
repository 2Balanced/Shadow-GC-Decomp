/* Player::Shadow::ShadowExecuter (class name from RTTI): the executer (the task that runs the
 * behaviors) of Shadow (fn_800AF1D4 makes it, Game/PlayerShadow.cpp). It takes the player's
 * commands through a bound callback: a damage command (type 7) switches to the damage behavior
 * (or to behavior 0x1F where the player's slot 0x50 says so) unless he is guarding or already
 * damaged, type 8 switches to behavior 0x1F and 0x108 to 6, and 0x115-0x117 use the weapon.
 * Commands nobody took are rejected back to the sender. Each frame it also drops a carried
 * object that is gone, keeps the ring and life effects up to date and counts down the special
 * gauge (meanings inferred from the calls). File, member and function names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#define MSL_RUNTIME_ERROR_STRING
#define PLAYER_EXECUTER_MEMBERS
#define PLAYER_PICKUP_SLOT
#define PLAYER_PLAYER_IMPLEMENTATION
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerExecuter.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using boost::shared_ptr;
using Player::PlayerBase;

/* The stage's record of the players' rings and gauges (struct name unknown; names inferred). */
struct UnkStageData_80079808 {
    /* 0x000 */ u8 field_0x0[0x70C];
    /* 0x70C */ s32 m_rings;
    /* 0x710 */ s32 m_ringLimit;
    /* 0x714 */ u8 field_0x714[0x718 - 0x714];
    /* 0x718 */ s32 m_life;
    /* 0x71C */ s32 m_lifeLimit;
};

/* The player's effect task (Game/PlayerEffect.cpp; class name unknown here). */
class UnkPlayerEffect_800A29E4;
/* The special gauge fn_80178B5C finds (class name unknown): a counter at 0x28. */
struct UnkGauge_80178B5C {
    /* 0x00 */ u8 field_0x0[0x28];
    /* 0x28 */ u8 m_counter[4];
};

extern "C" {
/* Player::Executer's command handling, frame update and behavior switch (see
 * Game/PlayerExecuter.h). */
void fn_800A78FC(Player::Executer *executer, CharCommand *command);
void fn_800A7D78(Player::Executer *executer);
void fn_800A7F14(Player::Executer *executer, const Player::Behavior::InitializeArgs *args);
/* A command reached the player (behaviour inferred). */
void fn_80078D10(PlayerBase *player);
/* Tests a bit of the player's state bits, and of a flag word. */
bool fn_8006CD78(u32 *bits, int bit);
bool fn_80014E34(u32 *flags, int bit);
/* The stage's record and the gauge of a player in it (behaviour inferred). */
UnkStageData_80079808 *fn_80079808(void);
UnkGauge_80178B5C *fn_80178B5C(UnkStageData_80079808 *data, s32 player);
/* The gauge counter: count it down by one, and read it (behaviour inferred). */
void fn_8016F288(u8 *counter, int step);
int fn_8016F280(u8 *counter);
/* The player's effects (Game/PlayerEffect.cpp; behaviour inferred). */
void fn_800A29E4(UnkPlayerEffect_800A29E4 *effect);
void fn_800A2B44(UnkPlayerEffect_800A29E4 *effect);
void fn_800A2BD8(UnkPlayerEffect_800A29E4 *effect);
void fn_800A2C00(UnkPlayerEffect_800A29E4 *effect);
}

/* The player's members used here (names inferred). */
inline CharCommandUnit *GetCommandUnit(PlayerBase *player) {
    return *reinterpret_cast<CharCommandUnit **>(reinterpret_cast<u8 *>(player) + 0x21C);
}
inline UnkPlayerEffect_800A29E4 *GetEffect(PlayerBase *player) {
    return reinterpret_cast<UnkPlayerEffect_800A29E4 *>(player->field_0x23C);
}

namespace Player {
namespace Shadow {

class ShadowExecuter : public Executer {
  public:
    ShadowExecuter(Task *parent, const shared_ptr<PlayerBase> &player);
    virtual ~ShadowExecuter();
    virtual void Tick(float dt);

    void OnCommand(CharCommand &command);
    void UpdatePickup();
    void UpdateRingEffects();
    void UpdateGauge();
    void UpdateRingLoss();

    /* 0x50 */ float m_gaugeTime;
    /* 0x54 */ u32 field_0x54;
};
SIZE_ASSERT(ShadowExecuter, 0x58);

ShadowExecuter::ShadowExecuter(Task *parent, const shared_ptr<PlayerBase> &player)
    : Executer(parent, player) {
    m_name = "ShadowExecuter";
    fn_800729B0(&GetCommandUnit(m_player.get())->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(boost::bind(&ShadowExecuter::OnCommand, this, _1)));
    m_gaugeTime = 0.0f;
}

ShadowExecuter::~ShadowExecuter() {}

void ShadowExecuter::Tick(float dt) {
    UpdatePickup();
    UpdateRingEffects();
    UpdateGauge();
    UpdateRingLoss();
    fn_800A7D78(this);
}

void ShadowExecuter::OnCommand(CharCommand &command) {
    u32 type = command.TypeId();
    if (type != 1 && type != 0x17) {
        fn_80078D10(m_player.get());
    }
    switch (command.TypeId()) {
    case 0x106:
        if (fn_8006CD78(m_player->m_stateBits, 0x15)) {
            fn_800A29E4(GetEffect(m_player.get()));
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
    case 7:
        if (!fn_8006CD78(m_player->m_stateBits, 0x17) && !fn_8006CD78(m_player->m_stateBits, 0x18) &&
            m_behavior->m_id != 0x1E) {
            if (m_player->vf_0x50()) {
                Behavior::InitializeArgs args(0x1F);
                fn_800A7F14(this, &args);
            } else {
                Behavior::InitializeArgsDamage args(dynamic_cast<DamageCommand &>(command).m_direction);
                fn_800A7F14(this, &args);
            }
        }
        command.Flags() |= 2;
        break;
    case 8:
        fn_800A7F14(this, &(const Behavior::InitializeArgs &)Behavior::InitializeArgs(0x1F));
        command.Flags() |= 2;
        break;
    case 0x108:
        fn_800A7F14(this, &(const Behavior::InitializeArgs &)Behavior::InitializeArgs(6));
        command.Flags() |= 2;
        break;
    case 0x115:
        if (fn_8006CD78(m_player->m_stateBits, 0x1F)) {
            m_player->m_weapon->BeginUse();
        }
        command.Flags() |= 2;
        break;
    case 0x116:
        if (fn_8006CD78(m_player->m_stateBits, 0x1F)) {
            m_player->m_weapon->BeginAltUse();
        }
        command.Flags() |= 2;
        break;
    case 0x117:
        if (fn_8006CD78(m_player->m_stateBits, 0x1F)) {
            m_player->m_weapon->EndUse();
        }
        command.Flags() |= 2;
        break;
    }
    if (command.Flags() & 2) {
        return;
    }
    command.Reject(boost::weak_ptr<CharInfo>(m_player->m_self));
    if (command.Flags() & 2) {
        return;
    }
}

void ShadowExecuter::UpdatePickup() {
    shared_ptr<Player> player = boost::dynamic_pointer_cast<Player>(m_player);
    if (fn_8006CD78(player->m_stateBits, 0x1E)) {
        if (player->vf_0x4C().expired()) {
            player->vf_0x44();
        }
    }
}

void ShadowExecuter::UpdateRingEffects() {
    if (!fn_8006CD78(m_player->m_stateBits, 0x10) && !fn_8006CD78(m_player->m_stateBits, 0x23) &&
        !fn_8006CD78(m_player->m_stateBits, 0x22)) {
        UnkStageData_80079808 *data = fn_80079808();
        if (data->m_life >= data->m_lifeLimit) {
            fn_800A2C00(GetEffect(m_player.get()));
        } else if (data->m_rings >= data->m_ringLimit) {
            fn_800A2BD8(GetEffect(m_player.get()));
        }
    }
}

void ShadowExecuter::UpdateGauge() {
    if (fn_8006CD78(m_player->m_stateBits, 0x24)) {
        UnkGauge_80178B5C *gauge = fn_80178B5C(fn_80079808(), m_player->field_0x54);
        if (gauge != 0) {
            m_gaugeTime += **reinterpret_cast<float **>(m_player->field_0x60);
            if (m_gaugeTime >= 1.0f) {
                m_gaugeTime -= 1.0f;
                if (!fn_80014E34(reinterpret_cast<u32 *>(reinterpret_cast<u8 *>(m_player.get()) + 0x214), 1)) {
                    fn_8016F288(gauge->m_counter, -1);
                }
            }
            if (fn_8016F280(gauge->m_counter) <= 0) {
                if (fn_8006CD78(m_player->m_stateBits, 0x25)) {
                    Behavior::InitializeArgs args(0x1F);
                    fn_800A7F14(this, &args);
                } else {
                    fn_800A2B44(GetEffect(m_player.get()));
                }
            }
        }
    }
}

void ShadowExecuter::UpdateRingLoss() {
    if (fn_8006CD78(m_player->m_stateBits, 0x15)) {
        if (fn_80079808()->m_rings <= 0.0f || !fn_8006CD78(m_player->m_stateBits, 0x22)) {
            fn_800A29E4(GetEffect(m_player.get()));
        }
    }
}

} // namespace Shadow
} // namespace Player

/* Creates Shadow's executer (called by Game/PlayerShadow.cpp; name inferred). */
extern "C" Player::Shadow::ShadowExecuter *fn_800AF1D4(Task *parent, const shared_ptr<PlayerBase> &player) {
    return new Player::Shadow::ShadowExecuter(parent, player);
}
