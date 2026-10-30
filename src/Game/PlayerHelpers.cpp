/* Player helpers (file name inferred; the unit has no class of its own): small functions other
 * units call by their addresses, which keep their fn_ names. They test a player's state bits,
 * drop its rings, make it throw what it carries, warp it to a position and angle, reset it, and
 * turn it super. The unit follows Game/PlayerShadowTarget.cpp: its .sdata2 has a 0.0 of its own
 * (0x805F2D90, after that unit's 0.0 at 0x805F2D68), and its .rodata starts 8-aligned at
 * 0x804B3D80. It ends with the dynamic_cast from CharInfo to Player::PlayerObject (function
 * templates come last). All names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first).
 *
 * NonMatching: 22 of 24 functions match. fn_800B7D48 and fn_800B7BCC differ by two swapped
 * instructions: retail loads the player pointer (lwz rN, 0(r3)) above the copy of the second
 * argument or the address of the call's result, which GC/2.7 does not do for any form tried (as
 * in Game/PlayerUtil.cpp). */
#define CHARCOMMAND_SET_ANGLE_INLINE
#define PLAYER_OBJECT_MEMBERS
#define PLAYER_PICKUP_SLOT
#include "Game/CharCommand.h"
#include "Game/PickupCharInfo.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerSoundBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using boost::shared_ptr;
using boost::weak_ptr;
using Player::PlayerBase;

/* What ScatteringRing::Instantiate scatters (as in Game/ContainerContent.cpp; its implicit
 * constructor is the out-of-line copy at 0x800B7E40). */
struct UnkScatterRequest_800B7E40 {
    /* 0x0 */ Vec3f position;
    /* 0xC */ s32 count;
};

namespace ScatteringRing {
class ScatteringRing {
  public:
    static void Instantiate(const UnkScatterRequest_800B7E40 &request, bool flag);
};
} // namespace ScatteringRing

/* A player's record (struct name unknown; see Game/ContinueAction.cpp): its ring counter at 0x28. */
struct UnkPlayerInfo_80178B5C {
    /* 0x00 */ u8 field_0x0[0x28];
    /* 0x28 */ u8 m_rings[4];
};
struct UnkStageData_80079808;
class UnkCharaList_80074194;

extern "C" {
/* A bit array: test bit. */
bool fn_8006CD78(const u32 *bits, int bit);
/* A 32-bit flag word: test bit (see Game/ScatteringRing.cpp). */
bool fn_80014E34(const u32 *flags, int bit);
/* The dot product (behaviour inferred). */
float fn_8040CF90(const Vec3f *a, const Vec3f *b);
/* The characters, and the character of a player number. */
UnkCharaList_80074194 *fn_80074194(void);
weak_ptr<CharInfo> fn_800A8B74(UnkCharaList_80074194 *list, int id);
/* The entry index of a player's character slots (behaviour inferred). */
weak_ptr<CharInfo> fn_800AADA4(Player::UnkCharaSlots_800AADA4 *slots, int index);
/* Switches the player's behavior. */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);
/* The stage data, a player's record in it, a ring counter and adding to it (behaviour
 * inferred). */
UnkStageData_80079808 *fn_80079808(void);
UnkPlayerInfo_80178B5C *fn_80178B5C(UnkStageData_80079808 *data, int player);
int fn_8016F280(u8 *counter);
void fn_80178B20(UnkPlayerInfo_80178B5C *info, int count);
/* Drops the rings of the player's ring bag (meaning unknown). */
void fn_800A28EC(u32 bag);
}

/* The player fields this unit reads (names inferred). */
struct UnkPlayerFields_800B75DC {
    /* 0x000 */ Vec3f m_position;
    /* 0x00C */ Vec3f m_rotation;
    /* 0x018 */ Vec3f m_front;
    /* 0x024 */ u8 field_0x24[0x54 - 0x24];
    /* 0x054 */ s32 m_number;
    /* 0x058 */ u8 field_0x58[0xE8 - 0x58];
    /* 0x0E8 */ Vec3f m_up;
    /* 0x0F4 */ u8 field_0xF4[0x214 - 0xF4];
    /* 0x214 */ u32 m_flags2;
    /* 0x218 */ u8 field_0x218[0x23C - 0x218];
    /* 0x23C */ u32 m_ringBag;
};

#define Fields(player) reinterpret_cast<UnkPlayerFields_800B75DC *>(player)

/* Vector assignment through the game's out-of-line copy. */
#define Assign(to, from) fn_800091D0(&(to), &(const Vec3f &)(from))

namespace Player {

/* Resets the stage camera of the player's number (class name from RTTI; declared in
 * Game/CharCommand.h only where CHARCOMMAND_PLAYER_COMMANDS is defined). Its constructor is
 * inline in the game (the copy linked is this unit's, 0x800B81B4). */
class ResetCameraCommand : public PlayerCommand {
  public:
    static int Id() {
        return 0x136;
    }

    ResetCameraCommand(int arg1) : PlayerCommand(Id(), arg1) {}
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
};

/* Commands 0x137, 0x135 and 0x12F (class names from RTTI; as in Game/CharCommand.h). */
class ResetCommand : public PlayerCommand {
  public:
    ResetCommand();
    virtual ~ResetCommand();
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
};

class CancelActingCommand : public PlayerCommand {
  public:
    CancelActingCommand(int arg1);
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
};

class ToSuperCommand : public PlayerCommand {
  public:
    ToSuperCommand(bool on);
    virtual ~ToSuperCommand();
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);

    /* 0x14 */ s32 m_on;
};

} // namespace Player

extern "C" void fn_800B8000(const weak_ptr<CharInfo> &character, const Vec3f &position,
                            const Vec3f &angle) {
    shared_ptr<PlayerBase> player = boost::dynamic_pointer_cast<PlayerBase>(character.lock());
    if (player) {
        fn_800A7F14(player->m_behaviors,
                    &(const Player::Behavior::InitializeArgs &)Player::Behavior::InitializeArgs(0));
        player->PushCommand(new ClrVelocityCommand(0));
        player->PushCommand(new SetPositionCommand(position, 0));
        player->PushCommand(new SetAngleCommand(angle, 2, 0));
        player->PushCommand(new Player::ResetCameraCommand(0));
        player->PushCommand(new Player::CancelActingCommand(0));
    }
}

extern "C" void fn_800B7F88(int id, const Vec3f &position, const Vec3f &angle) {
    weak_ptr<CharInfo> character = fn_800A8B74(fn_80074194(), id);
    fn_800B8000(character, position, angle);
}

extern "C" void fn_800B7EF8(const weak_ptr<CharInfo> &character) {
    if (!character.expired()) {
        shared_ptr<CharInfo> chara = character.lock();
        chara->PushCommand(new Player::ResetCommand);
    }
}

extern "C" void fn_800B7E44(int id) {
    shared_ptr<CharInfo> chara = fn_800A8B74(fn_80074194(), id).lock();
    if (chara) {
        chara->PushCommand(new Player::ToSuperCommand(true));
    }
}

extern "C" void fn_800B7D48(shared_ptr<PlayerBase> &player, int count) {
    int number = Fields(player.get())->m_number;
    if (number >= 0) {
        if (fn_8006CD78(player.get()->m_stateBits, 0x20)) {
            fn_800A28EC(Fields(player.get())->m_ringBag);
        } else if (!fn_8006CD78(player.get()->m_stateBits, 0x24) &&
                   !fn_80014E34(&Fields(player.get())->m_flags2, 1)) {
            UnkPlayerInfo_80178B5C *info = fn_80178B5C(fn_80079808(), number);
            if (info != 0) {
                if (count > fn_8016F280(info->m_rings)) {
                    count = fn_8016F280(info->m_rings);
                }
                if (count > 0) {
                    UnkScatterRequest_800B7E40 request;
                    Assign(request.position, Fields(player.get())->m_position);
                    request.count = count;
                    ScatteringRing::ScatteringRing::Instantiate(request, false);
                }
                fn_80178B20(info, -count);
            }
        }
    }
}

extern "C" void fn_800B7BCC(shared_ptr<PlayerBase> &player) {
    shared_ptr<PickupCharInfo> object = player.get()->vf_0x4C().lock();
    if (object) {
        object->PushCommand(new ThrowCommand(weak_ptr<CharInfo>(player.get()->m_self),
                                             Fields(player.get())->m_up));
        player.get()->vf_0x44();
        player.get()->m_sound->StopVoice(0x17);
    }
}

extern "C" bool fn_800B7B58(shared_ptr<PlayerBase> &player) {
    if (fn_8006CD78(player.get()->m_stateBits, 2)) {
        return false;
    }
    return !(fn_8040CF90(&Fields(player.get())->m_front, &Fields(player.get())->m_up) >= 0.0f);
}

extern "C" bool fn_800B7AD0(const shared_ptr<CharInfo> &character) {
    shared_ptr<PlayerBase> player = boost::dynamic_pointer_cast<PlayerBase>(character);
    if (player) {
        return fn_8006CD78(player->m_stateBits, 2);
    }
    return false;
}

extern "C" bool fn_800B7A48(const shared_ptr<CharInfo> &character) {
    shared_ptr<PlayerBase> player = boost::dynamic_pointer_cast<PlayerBase>(character);
    if (player) {
        return fn_8006CD78(player->m_stateBits, 6);
    }
    return false;
}

extern "C" bool fn_800B79C0(const shared_ptr<CharInfo> &character) {
    shared_ptr<PlayerBase> player = boost::dynamic_pointer_cast<PlayerBase>(character);
    if (player) {
        return fn_8006CD78(player->m_stateBits, 8);
    }
    return false;
}

extern "C" bool fn_800B7938(const shared_ptr<CharInfo> &character) {
    shared_ptr<PlayerBase> player = boost::dynamic_pointer_cast<PlayerBase>(character);
    if (player) {
        return fn_8006CD78(player->m_stateBits, 0xF);
    }
    return false;
}

extern "C" bool fn_800B78B0(const shared_ptr<CharInfo> &character) {
    shared_ptr<PlayerBase> player = boost::dynamic_pointer_cast<PlayerBase>(character);
    if (player) {
        return fn_8006CD78(player->m_stateBits, 0xA);
    }
    return false;
}

extern "C" bool fn_800B7828(const shared_ptr<CharInfo> &character) {
    shared_ptr<PlayerBase> player = boost::dynamic_pointer_cast<PlayerBase>(character);
    if (player) {
        return fn_8006CD78(player->m_stateBits, 0x29);
    }
    return false;
}

extern "C" bool fn_800B77A0(const shared_ptr<CharInfo> &character) {
    shared_ptr<PlayerBase> player = boost::dynamic_pointer_cast<PlayerBase>(character);
    if (player) {
        return fn_8006CD78(player->m_stateBits, 0x12);
    }
    return false;
}

extern "C" bool fn_800B7694(const shared_ptr<CharInfo> &character) {
    shared_ptr<Player::PlayerObject> object =
        boost::dynamic_pointer_cast<Player::PlayerObject>(character);
    if (object && object->m_kind < 0x10) {
        return true;
    }
    return false;
}

extern "C" weak_ptr<CharInfo> fn_800B75DC(const weak_ptr<CharInfo> &character) {
    if (!character.expired()) {
        shared_ptr<PlayerBase> player = boost::dynamic_pointer_cast<PlayerBase>(character.lock());
        if (player) {
            return fn_800AADA4(player.get()->m_charaSlots, 1);
        }
    }
    return weak_ptr<CharInfo>();
}
