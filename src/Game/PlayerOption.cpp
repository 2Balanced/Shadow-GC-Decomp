/* Player::ExternalOption and Player::InternalOption (class names from RTTI): two characters a
 * player keeps beside itself (Player::UnkCharaSlots_800AADA4, at 0x254 of the player). They follow
 * the player's position and motion each frame and pass the commands they get on to it. File name
 * inferred from the classes; other names marked "(inferred)" are not original. Functions other
 * units call by address keep their fn_ names (C functions taking the object). Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#define PLAYER_OBJECT_MEMBERS
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

namespace Player {
class Executer;
}

/* The player's members used here (struct name unknown; Player::PlayerBase leaves them unmapped). */
struct UnkPlayerFields_800AB304 {
    /* 0x000 */ Vec3f m_position;
    /* 0x00C */ Vec3f m_rotation;
    /* 0x018 */ Vec3f m_velocity;
    /* 0x024 */ Vec3f field_0x24;
    /* 0x030 */ s32 field_0x30;
    /* 0x034 */ u8 field_0x34[0x3C - 0x34];
    /* 0x03C */ char m_name[0x10];
    /* 0x04C */ u8 field_0x4C[0x5C - 0x4C];
    /* 0x05C */ s32 m_isPlayer;
    /* 0x060 */ const float *m_frameTime;
    /* 0x064 */ s32 m_kind;
    /* 0x068 */ u8 field_0x68[0x22C - 0x68];
    /* 0x22C */ Player::Executer *m_executer;
};

#define Fields(player) reinterpret_cast<UnkPlayerFields_800AB304 *>(player)

/* The player's character kind and frame time: inline functions in the game (as in
 * Game/PlayerBehaviorJump.cpp, whose out-of-line copies are the ones linked). */
inline s32 CharaKind(Player::PlayerBase *player) {
    return Fields(player)->m_kind;
}
inline const float &FrameTime(Player::PlayerBase *player) {
    return *Fields(player)->m_frameTime;
}

/* Whether a character is a player (the word at 0x5C; name inferred). An inline function in the
 * game, whose copy this unit emits. */
inline s32 IsPlayerChara(Player::PlayerBase *player) {
    return Fields(player)->m_isPlayer;
}

extern "C" {
/* Executer's command handling (Game/PlayerExecuter.cpp). */
void fn_800A78FC(Player::Executer *executer, CharCommand *command);
/* Names a character (behaviour inferred). */
void fn_801DC984(CharInfo *chara, const char *name);
}

/* The slots of a character's vtable the options call with the arguments their bodies take (a view:
 * CharInfo declares them without arguments). */
class UnkCharaSlots_800AAD44 {
  public:
    /* 0x00 */ u8 field_0x0[0x38];
    virtual ~UnkCharaSlots_800AAD44();
    virtual void vf_0x0C(void *arg);
    virtual void PushCommand(CharCommand *command);
    virtual void vf_0x14(CharCommand *command);
};

#define Slots(chara) reinterpret_cast<UnkCharaSlots_800AAD44 *>(chara)

namespace Player {

/* ExternalOption (class name from RTTI; vtable 0x80521AE8): the option a player keeps in slot 1
 * (meaning inferred). */
class ExternalOption : public PlayerObject {
  public:
    ExternalOption(const boost::shared_ptr<PlayerBase> &player, int type);
    virtual ~ExternalOption();
    virtual void PushCommand(CharCommand *command);
    virtual void vf_0x14();
    virtual bool vf_0x18();
    virtual bool vf_0x1C();
    virtual bool vf_0x20();
    virtual bool vf_0x24();
    virtual bool vf_0x28();

    /* 0x68 */ boost::shared_ptr<PlayerBase> m_player;
};
SIZE_ASSERT(ExternalOption, 0x70);

/* InternalOption (class name from RTTI; vtable 0x80521B74): the option a player keeps in slot 0
 * (meaning inferred). */
class InternalOption : public PlayerObject {
  public:
    InternalOption(const boost::shared_ptr<PlayerBase> &player, int type);
    virtual ~InternalOption();
    virtual void PushCommand(CharCommand *command);
    virtual void vf_0x14();
    virtual bool vf_0x18() {
        return true;
    }
    virtual bool vf_0x1C() {
        return true;
    }
    virtual bool vf_0x20() {
        return true;
    }
    virtual bool vf_0x24() {
        return IsPlayerChara(m_player.get()) == 1;
    }
    virtual bool vf_0x28() {
        return IsPlayerChara(m_player.get()) == 1;
    }

    /* 0x68 */ boost::shared_ptr<PlayerBase> m_player;
};
SIZE_ASSERT(InternalOption, 0x70);

/* The options of a player (class name unknown; at 0x254 of the player): slot 0 holds its
 * InternalOption, slot 1 its ExternalOption. */
class UnkCharaSlots_800AADA4 {
  public:
    UnkCharaSlots_800AADA4(const boost::shared_ptr<PlayerBase> &player);
    ~UnkCharaSlots_800AADA4();

    /* 0x00 */ boost::shared_ptr<PlayerBase> m_player;
    /* 0x08 */ boost::shared_ptr<CharInfo> m_options[2];
};
SIZE_ASSERT(UnkCharaSlots_800AADA4, 0x18);

InternalOption::InternalOption(const boost::shared_ptr<PlayerBase> &player, int type)
    : PlayerObject(type), m_player(player) {
    SetPlayerFlag(IsPlayerChara(m_player.operator->()));
    fn_801DC984(this, Fields(m_player.get())->m_name);
    SetKind(CharaKind(m_player.operator->()));
}

InternalOption::~InternalOption() {}

void InternalOption::PushCommand(CharCommand *command) {
    Slots(m_player.get())->PushCommand(command);
}

} // namespace Player

/* Slot 0x14: a command nobody took (behaviour inferred); the player's executer may take a score
 * command, the player gets the rest. */
extern "C" void vf_0x14__Q26Player14InternalOptionFv(Player::InternalOption *option,
                                                     CharCommand *command) {
    if (Fields(option->m_player.get())->m_executer != 0) {
        if (command->TypeId() == 0x1C) {
            fn_800A78FC(Fields(option->m_player.get())->m_executer, command);
        }
        if (command->Flags() & 2) {
            return;
        }
    }
    Slots(option->m_player.get())->vf_0x14(command);
}

namespace Player {

ExternalOption::ExternalOption(const boost::shared_ptr<PlayerBase> &player, int type)
    : PlayerObject(type), m_player(player) {
    SetPlayerFlag(IsPlayerChara(m_player.operator->()));
    fn_801DC984(this, Fields(m_player.get())->m_name);
    SetKind(CharaKind(m_player.operator->()));
}

ExternalOption::~ExternalOption() {}

void ExternalOption::PushCommand(CharCommand *command) {
    Slots(m_player.get())->PushCommand(command);
}

} // namespace Player

/* Slot 0x14 (see InternalOption's): what neither takes is rejected back to the player. */
extern "C" void vf_0x14__Q26Player14ExternalOptionFv(Player::ExternalOption *option,
                                                     CharCommand *command) {
    if (Fields(option->m_player.get())->m_executer != 0) {
        if (command->TypeId() == 0x1C) {
            fn_800A78FC(Fields(option->m_player.get())->m_executer, command);
        }
        if (command->Flags() & 2) {
            return;
        }
    }
    command->Reject(boost::weak_ptr<CharInfo>(option->m_player));
}

namespace Player {

UnkCharaSlots_800AADA4::UnkCharaSlots_800AADA4(const boost::shared_ptr<PlayerBase> &player)
    : m_player(player) {
    m_options[0] = boost::shared_ptr<CharInfo>(new InternalOption(m_player, 2));
    m_options[1] = boost::shared_ptr<CharInfo>(new ExternalOption(m_player, 3));
}

UnkCharaSlots_800AADA4::~UnkCharaSlots_800AADA4() {}

} // namespace Player

extern "C" {

/* Moves the options with the player. */
void fn_800AAE14(Player::UnkCharaSlots_800AADA4 *slots) {
    for (int i = 0; i < 2; i++) {
        boost::shared_ptr<CharInfo> option = slots->m_options[i];
        fn_800091D0(&option->m_position, &slots->m_player->m_position);
        option->m_rotation = slots->m_player->m_rotation;
        fn_800091D0(&option->field_0x18, &slots->m_player->field_0x18);
        fn_800091D0(&option->field_0x24, &slots->m_player->field_0x24);
        option->field_0x30 = slots->m_player->field_0x30;
        static_cast<Player::PlayerObject *>(option.operator->())
            ->SetFrameTime(&FrameTime(slots->m_player.operator->()));
    }
}

/* The option of a slot. */
boost::weak_ptr<CharInfo> fn_800AADA4(Player::UnkCharaSlots_800AADA4 *slots, int index) {
    return boost::weak_ptr<CharInfo>(slots->m_options[index]);
}

/* Passes slot 0x0C on to the options. */
void fn_800AAD44(Player::UnkCharaSlots_800AADA4 *slots, void *arg) {
    for (int i = 0; i < 2; i++) {
        Slots(slots->m_options[i].get())->vf_0x0C(arg);
    }
}

} // extern "C"

namespace Player {

bool ExternalOption::vf_0x28() {
    return false;
}

bool ExternalOption::vf_0x24() {
    return false;
}

bool ExternalOption::vf_0x20() {
    return false;
}

bool ExternalOption::vf_0x1C() {
    return true;
}

bool ExternalOption::vf_0x18() {
    return false;
}

} // namespace Player
