/* The player commands (Player::*Command, class names from RTTI; declared in Game/CharCommand.h and
 * Game/GadgetCharCommands.h): their Reject functions, which apply a command to the player it was
 * sent to (switching the player's behavior or control, or calling its motion, weapon, aim or
 * effects) and mark it handled, the constructors that are not inline, the debug descriptions of
 * the commands that have values, and Player::PlayerCommand's name table. The InitializeArgs the
 * Reject functions build have their inline constructors and destructors here (out of line, as
 * they call other inline functions). The destructors the classes leave implicit come out at the
 * end of the unit with their vtables. Names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include <stdio.h>
#include <string>

#define CHARCOMMAND_IMPLEMENTATION
#define CHARCOMMAND_PLAYER_COMMANDS
#define CHARCOMMAND_SET_LOOP_PATH
#define PLAYER_BEHAVIOR_ARGS_COMMANDS
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/GadgetCharCommands.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerControl.h"
#include "Game/PlayerSoundBase.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/StageCameraList.h"
#include "Game/Vibration.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Player::PlayerBase;
using Player::Behavior::UnkBehaviorExecuter_800A7F14;

namespace Sonicteam {
namespace System {
namespace Geometry {
class Line;
} // namespace Geometry
} // namespace System
} // namespace Sonicteam

/* The command queues (class name unknown; Game/CharCommandManagerTask.cpp defines it). */
class UnkCharCommandManager_8009978C {
  public:
    UnkCharCommandManager_8009978C();
    ~UnkCharCommandManager_8009978C();

    /* 0x00 */ u8 field_0x0[0xB8];
};

namespace ItemIcon {

/* The item icons (class name from its functions' symbols; see Game/ItemIconManagerTask.cpp). */
class Manager {
  public:
    Manager();
    ~Manager();
    void ShowItem(int player, int kind, int count);

    /* 0x00 */ u8 field_0x0[0x10];
};

} // namespace ItemIcon

/* The player's effects (Player::Effect, at 0x23C of the player; see Game/PlayerEffect.cpp). */
class UnkPlayerEffects_800A28B0;
/* The player's controller (at 0x224; see Game/PlayerController.cpp). */
namespace Player {
class Controller;
} // namespace Player
/* The stage's records (see Game/BossCtrlUnit.cpp). */
class UnkStageData_80079808;
class UnkPlayerInfo_80178B5C;

extern "C" {
/* a + b for the debug descriptions (MSL's operator+ for a string and a C string, out of line in
 * the game; see Game/CharCommands.cpp). */
std::string fn_8004F5DC(const std::string &a, const char *b);
/* Deletes the queued commands of a level (behaviour inferred). */
void fn_801D9F00(UnkCharCommandManager_8009978C *manager, int level);
bool fn_8006CD78(u32 *bits, int bit);
void fn_8040CE60(Vec3f *vector); /* sets the vector to zero */
Vec3f fn_8040CEE8(const Vec3f *vector); /* normalized */
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
bool fn_80014E34(u32 *flags, int bit);
/* Whether the player's behavior is id, and switching it (see Game/PlayerBehavior.h). */
bool fn_800A7EEC(UnkBehaviorExecuter_800A7F14 *executer, int id);
void fn_800A7F14(UnkBehaviorExecuter_800A7F14 *executer, const Player::Behavior::InitializeArgs *args);
/* The controller: switching the control, ending the acting, and an acted acceleration (behaviour
 * inferred). */
void fn_8009EBA4(Player::Controller *controller, const Player::Control::InitializeArgs *args);
void fn_8009EBEC(Player::Controller *controller);
void fn_800773A4(Player::Controller *controller, const boost::weak_ptr<PlayerBase> &player,
                 float time, const Vec3f *velocity);
/* The player's effects (see Game/PlayerEffect.cpp). */
void fn_800A28B0(UnkPlayerEffects_800A28B0 *effects);
void fn_800A2920(UnkPlayerEffects_800A28B0 *effects, int kind);
void fn_800A2B44(UnkPlayerEffects_800A28B0 *effects);
void fn_800A2B78(UnkPlayerEffects_800A28B0 *effects);
/* The player's reset and invincibility, and the input lock modes (behaviour inferred). */
void fn_80078FEC(PlayerBase *player);
void fn_8007939C(PlayerBase *player, bool on);
void fn_800792D4(PlayerBase *player, float time);
void fn_80079284(PlayerBase *player);
void fn_8007922C(PlayerBase *player);
void fn_800791BC(PlayerBase *player);
void fn_8007914C(PlayerBase *player, float time);
void fn_800790EC(PlayerBase *player);
void fn_800790B4(PlayerBase *player);
void fn_80079074(PlayerBase *player);
/* The stage's records, a player's record, its score and rings (behaviour inferred; see
 * Game/BossCtrlUnit.cpp). */
UnkStageData_80079808 *fn_80079808(void);
UnkPlayerInfo_80178B5C *fn_80178B5C(UnkStageData_80079808 *data, int player);
void fn_80178B20(UnkPlayerInfo_80178B5C *info, int count);
void fn_8016F0CC(void *score, int kind, int points);
void fn_8016EF8C(void *lives, int count);
void fn_801B64E8(Vehicle::TVehicleBase *vehicle);
void *fn_80053164(void);
void fn_801D560C(void *manager, int soundId, int arg2, int arg3);
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int player);
}

/* The player's members used here (struct name unknown; Player::PlayerBase leaves them
 * unmapped). */
struct UnkPlayerFields_80099658 {
    /* 0x000 */ u8 field_0x0[0x54];
    /* 0x054 */ s32 m_number; /* the player's number */
    /* 0x058 */ u8 field_0x58[0xA0 - 0x58];
    /* 0x0A0 */ u32 m_flags;
    /* 0x0A4 */ u32 field_0xA4;
    /* 0x0A8 */ u32 m_stateBits[2];
    /* 0x0B0 */ u8 field_0xB0[0xE8 - 0xB0];
    /* 0x0E8 */ Vec3f m_gravity;
    /* 0x0F4 */ u8 field_0xF4[0x224 - 0xF4];
    /* 0x224 */ Player::Controller *m_controller;
    /* 0x228 */ u8 field_0x228[0x23C - 0x228];
    /* 0x23C */ UnkPlayerEffects_800A28B0 *m_effects;
};

#define Fields(player) reinterpret_cast<UnkPlayerFields_80099658 *>(player)

/* Slots of the player's motion and aim with the arguments the commands pass (the shared headers
 * declare them without; class names unknown). */
class UnkPlayerMotionSlots_8009A65C {
  public:
    /* 0x00 */ u8 field_0x0[0x18];
    virtual void vf_0x08();
    virtual void vf_0x0C();
    virtual void SetMotion(int motion, float time);
    virtual void vf_0x14();
    virtual void vf_0x18();
    virtual void SetExternalMotion(const s32 *motions, float time);
};

class UnkPlayerTargetSlots_80099C60 {
  public:
    virtual void vf_0x08();
    virtual void vf_0x0C();
    virtual void vf_0x10();
    virtual void vf_0x14();
    virtual void SetLookAt(const Vec3f *position, float time);
};

inline UnkPlayerMotionSlots_8009A65C *MotionSlots(PlayerBase *player) {
    return reinterpret_cast<UnkPlayerMotionSlots_8009A65C *>(player->m_motion);
}

/* The singletons the commands use: each accessor keeps a pointer to the static object it builds
 * on first use (struct and names inferred). The pointers are defined after the accessors, as the
 * game has them after the objects' guards in .sbss. */
struct UnkSingletons_8009978C {
    static ItemIcon::Manager *s_itemIcons;
    static UnkCameraManager_80099954 *s_cameras;
    static UnkCharCommandManager_8009978C *s_commands;
};

/* The static objects (inline functions, so that the accessors call one and stay out of line). */
inline ItemIcon::Manager &ItemIconsObject() {
    static ItemIcon::Manager s_instance;
    return s_instance;
}

inline UnkCharCommandManager_8009978C &CommandsObject() {
    static UnkCharCommandManager_8009978C s_instance;
    return s_instance;
}

inline UnkCameraManager_80099954 &CamerasObject() {
    static UnkCameraManager_80099954 s_instance;
    return s_instance;
}

/* The item icons, the command queues (see Game/CharCommandManagerTask.cpp) and the camera manager
 * of the player code (see Game/StageCameraList.h); other units call them by their fn_ names. */
extern "C" inline ItemIcon::Manager *fn_8009B6A0(void) {
    if (UnkSingletons_8009978C::s_itemIcons == 0) {
        UnkSingletons_8009978C::s_itemIcons = &ItemIconsObject();
    }
    return UnkSingletons_8009978C::s_itemIcons;
}

extern "C" inline UnkCharCommandManager_8009978C *fn_8009978C(void) {
    if (UnkSingletons_8009978C::s_commands == 0) {
        UnkSingletons_8009978C::s_commands = &CommandsObject();
    }
    return UnkSingletons_8009978C::s_commands;
}

extern "C" inline UnkCameraManager_80099954 *fn_80099954(void) {
    if (UnkSingletons_8009978C::s_cameras == 0) {
        UnkSingletons_8009978C::s_cameras = &CamerasObject();
    }
    return UnkSingletons_8009978C::s_cameras;
}

ItemIcon::Manager *UnkSingletons_8009978C::s_itemIcons;
UnkCameraManager_80099954 *UnkSingletons_8009978C::s_cameras;
UnkCharCommandManager_8009978C *UnkSingletons_8009978C::s_commands;


namespace Player {

/* The player commands' names (struct name unknown). */
struct UnkCommandName_804B18DC {
    s32 id;
    const char *name;
};

static const UnkCommandName_804B18DC s_names[0x38] = {
    {0x100, "Dummy"},
    {0x101, "CancelBehavior"},
    {0x102, "StartAttack"},
    {0x103, "EndAttack"},
    {0x104, "StartJump"},
    {0x105, "EndJump"},
    {0x106, "StartAction"},
    {0x107, "EndAction"},
    {0x108, "JumpDash"},
    {0x109, "Sliding"},
    {0x10A, "HomingAttack"},
    {0x10B, "HomingJump"},
    {0x10C, "SetLoopPath"},
    {0x10D, "Grind"},
    {0x10E, "SpinDash"},
    {0x10F, "DarkSpin"},
    {0x110, "LightDash"},
    {0x111, "TouchLine"},
    {0x112, "TouchHang"},
    {0x114, "ChaosControl"},
    {0x113, "ChaosBlast"},
    {0x115, "StartUseWeapon"},
    {0x116, "CancelUseWeapon"},
    {0x117, "EndUseWeapon"},
    {0x118, "PickupWeapon"},
    {0x119, "CastWeapon"},
    {0x11A, "AddWeapon"},
    {0x11B, "GetItem"},
    {0x11C, "ApproachControl"},
    {0x11D, "PickupObject"},
    {0x11E, "Throw"},
    {0x11F, "TurnOver"},
    {0x120, "Dash"},
    {0x121, "Launch"},
    {0x122, "Stream"},
    {0x123, "GetOnVehicle"},
    {0x124, "StartParallelMove"},
    {0x125, "EndParallelMove"},
    {0x126, "Invincible"},
    {0x127, "IgnoreInput"},
    {0x128, "IgnoreSearchPath"},
    {0x129, "SetMotion"},
    {0x12A, "SetExternalMotion"},
    {0x12B, "StartExternalControl"},
    {0x12C, "EndExternalControl"},
    {0x12D, "SetGravity"},
    {0x12E, "PressGround"},
    {0x12F, "ToSuper"},
    {0x130, "ToElectric"},
    {0x131, "ElectricCircuit"},
    {0x132, "WaitTalking"},
    {0x133, "SetLookAtPosition"},
    {0x134, "ActingAccelerate"},
    {0x135, "CancelActing"},
    {0x136, "ResetCamera"},
    {0x137, "Reset"},
};

/* Never called (the linker drops it): with -str reuse, Player::Behavior::InitializeArgs's RTTI
 * name shares this literal, which puts it before the derived classes' names in .rodata, where the
 * game has it (a stand-in for whatever emitted the base class's RTTI first in the original; see
 * the unused-function trick in docs/research-notes.md). */
const char *UnusedInitializeArgsName() {
    return "Player::Behavior::InitializeArgs";
}

const char *PlayerCommand::GetName() {
    u16 id = m_type;
    for (int i = 0; i < 0x38; i++) {
        if (id == s_names[i].id) {
            return s_names[i].name;
        }
    }
    return "Unknown PlayerCommand ?";
}

boost::shared_ptr<PlayerBase> PlayerCommand::GetPlayer(const boost::weak_ptr<CharInfo> &receiver) {
    if (!receiver.expired()) {
        boost::shared_ptr<PlayerBase> player = boost::dynamic_pointer_cast<PlayerBase>(receiver.lock());
        if (player.get() != 0) {
            return player;
        }
    }
    return boost::shared_ptr<PlayerBase>();
}

void CancelBehaviorCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        fn_800A7F14(player->m_behaviors, &Behavior::InitializeArgs(0));
    }
    Flags() |= 2;
}

void StartAttackCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        if (fn_8006CD78(Fields(player.get())->m_stateBits, 2)) {
            if (fn_8006CD78(Fields(player.get())->m_stateBits, 0x1F)) {
                switch (player->m_weapon->GetWeaponKind()) {
                case 1:
                case 3:
                case 6:
                    player->m_weapon->BeginUse();
                    break;
                case 5: {
                    fn_800A7F14(player->m_behaviors, &Behavior::InitializeArgs(0x19));
                    break;
                }
                }
            } else {
                fn_800A7F14(player->m_behaviors, &Behavior::InitializeArgs(0x19));
            }
        } else {
            fn_800A7F14(player->m_behaviors, &Behavior::InitializeArgs(0x1A));
        }
        Flags() |= 2;
    }
}

void EndAttackCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    Flags() |= 2;
}

StartJumpCommand::StartJumpCommand(int arg0) : PlayerCommand(0x104, arg0) {
    fn_8040CE60(&field_0x14);
}

StartJumpCommand::StartJumpCommand(const Vec3f &direction, int arg1) : PlayerCommand(0x104, arg1) {
    if (!(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z < 1e-4f)) {
        fn_800091D0(&field_0x14, &(const Vec3f &)fn_8040CEE8(&direction));
    } else {
        fn_8040CE60(&field_0x14);
    }
}

void StartJumpCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0 && fn_80014E34(&Fields(player.get())->m_flags, 1)) {
        fn_800A7F14(player->m_behaviors, &Behavior::InitializeArgsJump(field_0x14));
        Flags() |= 2;
    }
}

std::string StartJumpCommand::vf_0x10() const {
    char text[0x44];
    sprintf(text, " Dir%6.3f % 6.3f % 6.3f", field_0x14.x, field_0x14.y, field_0x14.z);
    return fn_8004F5DC(CharCommand::vf_0x10(), text);
}

void StartActionCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    Flags() |= 2;
}

void EndActionCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    Flags() |= 2;
}

void SlidingCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        fn_800A7F14(player->m_behaviors, &Behavior::InitializeArgs(7));
        Flags() |= 2;
    }
}

void HomingAttackCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        fn_800A7F14(player->m_behaviors, &Behavior::InitializeArgsHomingAttack(m_target));
        Flags() |= 2;
    }
}

void HomingJumpCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        fn_800A7F14(player->m_behaviors, &Behavior::InitializeArgsHomingJump(m_velocity, false));
        Flags() |= 2;
    }
}

void SetLoopPathCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    Flags() |= 2;
}

void GrindCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        fn_800A7F14(player->m_behaviors, &Behavior::InitializeArgsGrind(m_rail));
        Flags() |= 2;
    }
}

void SpinDashCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        fn_800A7F14(player->m_behaviors, &Behavior::InitializeArgs(0x12));
        Flags() |= 2;
    }
}

void DarkSpinCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        fn_800A7F14(player->m_behaviors, &Behavior::InitializeArgsDarkSpin(m_target));
        Flags() |= 2;
    }
}

void LightDashCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        fn_800A7F14(player->m_behaviors, &Behavior::InitializeArgs(0x14));
        Flags() |= 2;
    }
}

void TouchLineCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        fn_800A7F14(player->m_behaviors, &Behavior::InitializeArgsLineHang(m_target, m_line));
        Flags() |= 2;
    }
}

void TouchHangCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        fn_800A7F14(player->m_behaviors, &Behavior::InitializeArgsSomethingHang(
                                             m_target, m_position, m_rotation, field_0x24));
        Flags() |= 2;
    }
}

void ChaosBlastCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        fn_800A7F14(player->m_behaviors, &Behavior::InitializeArgs(0x1C));
        Flags() |= 2;
    }
}

void ChaosControlCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        fn_800A7F14(player->m_behaviors, &Behavior::InitializeArgs(0x1D));
        Flags() |= 2;
    }
}

void PickupWeaponCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        if (fn_8006CD78(Fields(player.get())->m_stateBits, 2) &&
            !fn_8006CD78(Fields(player.get())->m_stateBits, 3)) {
            fn_800A7F14(player->m_behaviors, &Behavior::InitializeArgsPickupWeapon(m_weapon));
        } else {
            if (fn_8006CD78(Fields(player.get())->m_stateBits, 0x1F)) {
                player->m_weapon->DropWeapon();
            }
            boost::shared_ptr<CharInfo> weapon = m_weapon.lock();
            if (weapon.get() != 0) {
                weapon->PushCommand(new UseCommand(boost::weak_ptr<CharInfo>(player->m_self)));
            }
        }
        Flags() |= 2;
    }
}

void CastWeaponCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        if (fn_8006CD78(Fields(player.get())->m_stateBits, 0x1F)) {
            if (fn_8006CD78(Fields(player.get())->m_stateBits, 2) &&
                !fn_8006CD78(Fields(player.get())->m_stateBits, 3)) {
                fn_800A7F14(player->m_behaviors, &Behavior::InitializeArgsPickupWeapon());
            } else {
                player->m_weapon->DropWeapon();
            }
        }
        Flags() |= 2;
    }
}

void AddWeaponCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        if (fn_8006CD78(Fields(player.get())->m_stateBits, 0x1F) &&
            m_type != player->m_weapon->GetWeaponType()) {
            player->m_weapon->DropWeapon();
        }
        player->m_weapon->GiveWeaponType(m_type, m_ammo);
        fn_8034DF48(6, Fields(player.get())->m_number);
        Flags() |= 2;
    }
}

} // namespace Player


namespace Player {

void GetItemCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        int number = Fields(player.get())->m_number;
        bool voice = true;
        if (number < 0) {
            number = 0;
            voice = false;
        }
        UnkStageData_80079808 *stage = fn_80079808();
        UnkPlayerInfo_80178B5C *info = fn_80178B5C(fn_80079808(), number);
        if (info != 0) {
            switch (m_item) {
            case 7:
                fn_8016F0CC(reinterpret_cast<u8 *>(stage) + 0x70C, 0, m_value);
                break;
            case 8:
                fn_8016F0CC(reinterpret_cast<u8 *>(stage) + 0x70C, 1, m_value);
                break;
            case 0:
                fn_80178B20(info, m_value);
                voice = false;
                break;
            case 5:
                if (!player->m_vehicle.expired()) {
                    fn_801B64E8(player->m_vehicle.lock().get());
                    fn_801D560C(fn_80053164(), 0xE020, 0, 0);
                }
                break;
            case 6:
                fn_8016EF8C(reinterpret_cast<u8 *>(info) + 0x20, 1);
                break;
            case 9:
                fn_801D560C(fn_80053164(), 0xE01F, 0, 0);
                break;
            }
            if (m_showIcon) {
                fn_8009B6A0()->ShowItem(number, m_item, m_value);
            }
            fn_800A2920(Fields(player.get())->m_effects, m_item);
        }
        if (voice) {
            player->m_sound->StopVoice(0x18);
        }
        Flags() |= 2;
    }
}

void ApproachControlCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        fn_8009EBA4(Fields(player.get())->m_controller, &Control::InitializeArgsApproach(m_kind, m_target));
        Flags() |= 2;
    }
}

void PickupObjectCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        fn_800A7F14(player->m_behaviors, &Behavior::InitializeArgsPickupObject(m_target));
        Flags() |= 2;
    }
}

void ThrowCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        fn_800A7F14(player->m_behaviors, &Behavior::InitializeArgsThrow(m_object));
        Flags() |= 2;
    }
}

void TurnOverCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        fn_800A7F14(player->m_behaviors, &Behavior::InitializeArgsTurnOver(m_target));
        Flags() |= 2;
    }
}

LaunchCommand::LaunchCommand(int arg0, int arg1) : PlayerCommand(0x121, arg1), field_0x14(arg0) {}

void LaunchCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        fn_800A7F14(player->m_behaviors, &Behavior::InitializeArgsLaunch(field_0x14));
        Flags() |= 2;
    }
}

StreamCommand::StreamCommand(bool on, int arg1) : PlayerCommand(0x122, arg1), m_on(on) {}

void StreamCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0 && m_on != 0) {
        fn_800A7F14(player->m_behaviors, &Behavior::InitializeArgs(0x18));
        Flags() |= 2;
    }
}

GetOnVehicleCommand::GetOnVehicleCommand(const boost::weak_ptr<Vehicle::TVehicleBase> &vehicle, int arg1)
    : PlayerCommand(0x123, arg1) {
    m_vehicle = vehicle;
}

GetOnVehicleCommand::~GetOnVehicleCommand() {}

void GetOnVehicleCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        fn_800A7F14(player->m_behaviors, &Behavior::InitializeArgsDriveVehicle(m_vehicle));
        Flags() |= 2;
    }
}

void StartParallelMoveCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        fn_80076CA8(Fields(player.get())->m_stateBits, 0x3C);
        Flags() |= 2;
    }
}

void EndParallelMoveCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        fn_80076CC8(Fields(player.get())->m_stateBits, 0x3C);
        Flags() |= 2;
    }
}

void InvincibleCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        fn_8007939C(player.get(), m_on);
        Flags() |= 2;
    }
}

void IgnoreInputCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        bool cleared = false;
        switch (m_mode) {
        case 0:
            switch (m_kind) {
            case 0:
                fn_800792D4(player.get(), m_time);
                cleared = true;
                break;
            case 1:
                fn_80079284(player.get());
                cleared = true;
                break;
            case 2:
                fn_8007922C(player.get());
                cleared = true;
                break;
            case 3:
                fn_800791BC(player.get());
                break;
            }
            if (cleared) {
                fn_801D9F00(fn_8009978C(), 1);
                fn_801D9F00(fn_8009978C(), 2);
            }
            break;
        case 1:
            switch (m_kind) {
            case 0:
                fn_8007914C(player.get(), m_time);
                cleared = true;
                break;
            case 1:
                fn_800790EC(player.get());
                cleared = true;
                break;
            case 2:
                fn_800790B4(player.get());
                cleared = true;
                break;
            case 3:
                fn_80079074(player.get());
                break;
            }
            if (cleared) {
                fn_801D9F00(fn_8009978C(), 1);
            }
            break;
        }
        Flags() |= 2;
    }
}

void SetMotionCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        MotionSlots(player.get())->SetMotion(m_motion, field_0x18);
        Flags() |= 2;
    }
}

void SetExternalMotionCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        MotionSlots(player.get())->SetExternalMotion(m_motions, m_time);
        Flags() |= 2;
    }
}

std::string SetExternalMotionCommand::vf_0x10() const {
    char text[0x44];
    sprintf(text, " %4d %4d %4d T%6.3f", m_motions[0], m_motions[1], m_motions[2], m_time);
    return fn_8004F5DC(CharCommand::vf_0x10(), text);
}

void StartExternalControlCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        if (!field_0x14.expired()) {
            field_0x24 |= 2;
        }
        if (field_0x1C != 0) {
            field_0x24 |= 4;
        }
        if (field_0x20 != 0) {
            field_0x24 |= 8;
        }
        fn_800A7F14(player->m_behaviors,
                    &Behavior::InitializeArgsExternalControl(field_0x14, reinterpret_cast<const Vec3f *>(field_0x1C),
                                                             reinterpret_cast<const Vec3f *>(field_0x20),
                                                             field_0x24));
        Flags() |= 2;
    }
}

SetGravityCommand::SetGravityCommand(const Vec3f &direction, int arg1) : PlayerCommand(0x12D, arg1) {
    if (!(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z < 1e-4f)) {
        fn_800091D0(&m_direction, &(const Vec3f &)fn_8040CEE8(&direction));
    } else {
        fn_800091D0(&m_direction, &Vec3f(0.0f, 1.0f, 0.0f));
    }
}

SetGravityCommand::~SetGravityCommand() {}

void SetGravityCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        fn_800091D0(&Fields(player.get())->m_gravity, &m_direction);
    }
}

std::string SetGravityCommand::vf_0x10() const {
    char text[0x44];
    sprintf(text, " %4d %4d %4d", m_direction.x, m_direction.y, m_direction.z);
    return fn_8004F5DC(CharCommand::vf_0x10(), text);
}

PressGroundCommand::PressGroundCommand(int arg) : PlayerCommand(0x12E, arg) {}

PressGroundCommand::~PressGroundCommand() {}

void PressGroundCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        fn_80076CA8(Fields(player.get())->m_stateBits, 2);
    }
}

ToSuperCommand::ToSuperCommand(bool on) : PlayerCommand(0x12F, 4), m_on(on) {}

ToSuperCommand::~ToSuperCommand() {}

void ToSuperCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        if (m_on != 0) {
            if (!fn_8006CD78(Fields(player.get())->m_stateBits, 0x24)) {
                fn_800A2B78(Fields(player.get())->m_effects);
            }
            if (!fn_800A7EEC(player->m_behaviors, 0x22)) {
                fn_800A7F14(player->m_behaviors, &Behavior::InitializeArgs(0));
            }
        } else {
            if (fn_8006CD78(Fields(player.get())->m_stateBits, 0x24)) {
                fn_800A2B44(Fields(player.get())->m_effects);
            }
            if (fn_800A7EEC(player->m_behaviors, 0x22)) {
                fn_800A7F14(player->m_behaviors, &Behavior::InitializeArgs(2));
            }
        }
        Flags() |= 2;
    }
}

void ElectricCircuitCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        fn_800A7F14(player->m_behaviors, &Behavior::InitializeArgsElectricCircuit(m_target, field_0x1C));
        Flags() |= 2;
    }
}

SetLookAtPositionCommand::SetLookAtPositionCommand(const Vec3f &position, float time)
    : PlayerCommand(0x133, 4) {
    fn_800091EC(&m_position, &position);
    m_time = time;
}

SetLookAtPositionCommand::~SetLookAtPositionCommand() {}

void SetLookAtPositionCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        reinterpret_cast<UnkPlayerTargetSlots_80099C60 *>(player->m_target)->SetLookAt(&m_position, m_time);
        Flags() |= 2;
    }
}

ActingAccelerateCommand::ActingAccelerateCommand(const Vec3f &velocity, float time)
    : PlayerCommand(0x134, 4), m_time(time) {
    fn_800091EC(&m_velocity, &velocity);
}

void ActingAccelerateCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        fn_800773A4(Fields(player.get())->m_controller, boost::weak_ptr<PlayerBase>(player), m_time,
                    &m_velocity);
        Flags() |= 2;
    }
}

CancelActingCommand::CancelActingCommand(int arg1) : PlayerCommand(0x135, arg1) {}

void CancelActingCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        if (Fields(player.get())->m_controller != 0) {
            fn_8009EBEC(Fields(player.get())->m_controller);
        }
        Flags() |= 2;
    }
}

} // namespace Player


namespace Player {

void ResetCameraCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    if (boost::shared_ptr<CharInfo> chara = receiver.lock()) {
        boost::shared_ptr<Player> player = boost::dynamic_pointer_cast<Player>(chara);
        if (player.get() != 0) {
            fn_80099954()->ResetStageCamera(Fields(player.get())->m_number);
        }
        Flags() |= 2;
    }
}

ResetCommand::ResetCommand() : PlayerCommand(0x137, 0) {}

ResetCommand::~ResetCommand() {}

} // namespace Player


namespace Player {

void ResetCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    boost::shared_ptr<PlayerBase> player = GetPlayer(receiver);
    if (player.get() != 0) {
        if (player->m_weapon != 0 && !fn_8006CD78(Fields(player.get())->m_stateBits, 0x33)) {
            player->m_weapon->DiscardWeapon();
        }
        player->vf_0x48();
        if (Fields(player.get())->m_effects != 0) {
            fn_800A28B0(Fields(player.get())->m_effects);
        }
        fn_80076CC8(Fields(player.get())->m_stateBits, 0x3C);
        fn_80078FEC(player.get());
        Flags() |= 2;
    }
    fn_801D9F00(fn_8009978C(), 1);
    fn_801D9F00(fn_8009978C(), 2);
    fn_801D9F00(fn_8009978C(), 3);
    fn_801D9F00(fn_8009978C(), 4);
    fn_801D9F00(fn_8009978C(), 5);
    fn_801D9F00(fn_8009978C(), 6);
}

} // namespace Player

