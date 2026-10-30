/* Player::Control::UserInput (class name from RTTI): the control that steers a player from its
 * pad (control 0 of the players' control factories; fn_8009F9FC makes one). Every frame
 * (vf_0x14) it turns the pad's stick into AccelerateInputCommand and its buttons into the
 * player's commands: for each of four buttons it picks the command a press and a release send
 * (CollectCommands, from the player's state and what it touches: a jump or homing attack, an
 * attack, a weapon's use, a light dash, a vehicle, a rail, a weapon to pick up, a target to
 * approach) and the character that takes them, sends those of the buttons pressed and released
 * and deletes the others, and shows the commands on the button hints of the player's HUD record.
 * Among them StartUseWeaponCommand, EndUseWeaponCommand and JumpDashCommand (class names from
 * RTTI) have their vtables here. While it runs it keeps a VehicleArrow (Enter makes it) that
 * points at a vehicle or a gadget the player can use. File, member and function names are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
/* ControlBase's slots with their arguments (see Game/PlayerControl.h). */
#define PLAYER_CONTROL_SLOT_ARGS
/* The inline constructors of the commands the control sends (see Game/CharCommand.h). */
#define CHARCOMMAND_PLAYER_USER_INPUT
/* boost 1.32's shared_ptr operator!= (see boost/shared_ptr.hpp). */
#define BOOST_SP_NOT_EQUAL
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/Fn8040CopyVector3.h"
#include "Game/GadgetCharCommands.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerCharaParam.h"
#include "Game/PlayerControl.h"
#include "Game/PlayerPadInput.h"
#include "Game/PlayerSoundBase.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/TVehicleBase.h"
#include "Game/UnkCircuitTarget_800ABCE8.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Player::PlayerBase;

/* The arrow that points a vehicle's rider the way (class in Game/VehicleArrow.cpp). */
class VehicleArrow;

namespace Player {
class Controller;
} // namespace Player

namespace SonicteamUSA {
namespace System {
namespace CharaColli {
class CharaColliBaseCharInfo;
} // namespace CharaColli
} // namespace System
} // namespace SonicteamUSA

using SonicteamUSA::System::CharaColli::CharaColliBaseCharInfo;

/* Target (class name from RTTI; see Game/Target.h): only what this unit uses (names inferred). */
class Target {
  public:
    /* Flag bits 3, 5 and 6: a light dash follows it, the player picks its owner up, the player
     * turns it over (inline in the game: the shared_ptr's operator-> before them stays out of
     * line). */
    bool IsLightDashTarget() const {
        return m_flags & 8;
    }
    bool IsPickupTarget() const {
        return m_flags & 0x20;
    }
    bool IsTurnOverTarget() const {
        return m_flags & 0x40;
    }

    /* Where to aim at, seen from a character (slot 0x8). */
    virtual Vec3f GetPosition(CharInfo *viewer);

    /* 0x04 */ u8 field_0x4[0x20 - 0x4];
    /* 0x20 */ float m_radius; /* negative: any distance (inferred) */
    /* 0x24 */ u32 field_0x24;
    /* 0x28 */ boost::weak_ptr<CharInfo> m_owner;
    /* 0x30 */ u32 m_flags;
};

/* The mission manager (see Game/Mission.cpp). */
namespace Mission {
class MissionManager {
  public:
    void SetPlayer(int player);
    u8 IsFlagSet(int player);

    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ int m_currentPlayer;
};
} // namespace Mission

/* The game state (struct name unknown; see Game/TGi.h): two flags read here (meanings unknown). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xC8];
    /* 0x0C8 */ u8 field_0xC8; /* the controls are locked (inferred) */
    /* 0x0C9 */ u8 field_0xC9[0x125 - 0xC9];
    /* 0x125 */ u8 field_0x125; /* the players may not be switched (inferred) */
};
extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* What a mover stands on (as declared in Game/PlayerBehaviorEdgeHang.cpp). */
struct UnkColliMoveContact_803334A4 {
    bool IsValid() const;

    /* 0x00 */ u8 m_isCollider;
    /* 0x04 */ boost::weak_ptr<CharInfo> m_chara;
};

/* A line segment (layout unknown; built by fn_80020A20, as in Game/PlayerBehaviorLightDash.cpp). */
struct UnkSegment_80020A20 {
    /* 0x00 */ u8 field_0x0[0x18];
};

/* The player's collision handle (see Game/PlayerCharaColliImpl.cpp), the stage's records and a
 * player's record in them (see Game/BossCtrlUnit.cpp). */
struct UnkPlayerPart_80095BD4;
struct UnkStageData_80079808;
struct UnkPlayerInfo_80178B5C;

extern "C" {
bool fn_80014E34(u32 *flags, int bit);
bool fn_8006CD78(u32 *bits, int bit);
/* The player's pad input (see Game/PlayerPadInput.h). */
UnkPlayerInput_800A85B0 *fn_8006F8D0(PlayerBase *player);
/* The mission manager (see Game/BASwitch.cpp). */
Mission::MissionManager *fn_8006D6A8(void);
/* The stage's records and a player's record; its four button hints (meanings inferred). */
UnkStageData_80079808 *fn_80079808(void);
UnkPlayerInfo_80178B5C *fn_80178B5C(UnkStageData_80079808 *data, int player);
void fn_8016EED8(UnkPlayerInfo_80178B5C *info, s32 value);
void fn_8016EEF0(UnkPlayerInfo_80178B5C *info, s32 value);
void fn_8016EEF8(UnkPlayerInfo_80178B5C *info, s32 value);
void fn_8016EF00(UnkPlayerInfo_80178B5C *info, s32 value);
void fn_8016EF08(UnkPlayerInfo_80178B5C *info, s32 value);
/* The kind of a vehicle, for its hint (behaviour inferred). */
s32 fn_801B6938(Vehicle::TVehicleBase *vehicle);
/* Whether an Npc player is busy, seen from a position (behaviour inferred). */
bool fn_801F42C8(const boost::weak_ptr<PlayerBase> *player, const Vec3f *position);
/* Switches the player's control (Game/PlayerController.cpp), whether the player holds a weapon of
 * a kind it uses, and the circuit path the player reached. */
bool fn_8009EBA4(Player::Controller *controller, const Player::Control::InitializeArgs *args);
bool fn_8009EB80(Player::Controller *controller);
UnkCircuitTarget_800ABCE8 *fn_8009EB50(Player::Controller *controller);
/* The kind of a circuit path, and looking for a rail (see Game/PlayerBehaviorGrind.cpp). */
int fn_800ABB48(UnkCircuitTarget_800ABCE8 *target);
int fn_800ABC0C(UnkCircuitTarget_800ABCE8 *target, boost::shared_ptr<PlayerBase> *player,
                const Vec3f *position, const Vec3f *direction, int arg4, float range);
/* The target nearest the pad's direction (see Game/PlayerTargetBase.cpp). */
boost::weak_ptr<Target> fn_80206814(Player::PlayerTargetBase *aim, const Vec3f *direction);
/* The character the player's collision found nearest (Game/PlayerCharaColliImpl.cpp), and what
 * the player stands on. */
boost::weak_ptr<CharInfo> fn_80095A10(UnkPlayerPart_80095BD4 *colli);
UnkColliMoveContact_803334A4 *fn_80078DA0(PlayerBase *player);
/* The character a collider belongs to (see Game/BulletHoming.cpp). */
boost::weak_ptr<CharInfo> fn_800586EC(CharaColliBaseCharInfo *collider);
/* The player's up axis, and its input lock for a time (behaviour inferred). */
Vec3f fn_80079698(PlayerBase *player);
void fn_800792D4(PlayerBase *player, float time);
/* vector turned by the angle of the pad's direction relative to the player (Game/PlayerUtil.cpp). */
Vec3f fn_80093DD0(const boost::shared_ptr<PlayerBase> *player, const Vec3f *vector);
/* Builds a segment from start to end, and casts lines through the world (see
 * Game/PlayerBehaviorEdgeHang.cpp and Game/PlayerBehaviorLightDash.cpp). */
void *fn_80020A20(UnkSegment_80020A20 *segment, const Vec3f *start, const Vec3f *end);
float fn_800979A0(boost::shared_ptr<PlayerBase> *player, const UnkSegment_80020A20 *segment, int arg2,
                  int arg3, int arg4, int arg5);
float fn_800979E8(boost::shared_ptr<PlayerBase> *player, void *arg1, void *arg2,
                  boost::weak_ptr<CharaColliBaseCharInfo> *hit, void *arg4, void *arg5,
                  UnkSegment_80020A20 *segment, void *arg7, u32 attributes, u32 mask, int arg10);
/* The vehicle arrow: showing it, and where it points (Game/VehicleArrow.cpp). */
void fn_8033AC24(VehicleArrow *arrow, bool visible);
void fn_8033ABFC(VehicleArrow *arrow, const Vec3f *position);
/* Makes a VehicleArrow (Game/VehicleArrow.cpp). */
boost::shared_ptr<VehicleArrow> fn_8033A9B8(void);

bool fn_8000A950(float value); /* nearly zero */
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Vec3f fn_8040CDA8(const Vec3f *vector, float scale); /* vector * scale */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);   /* a + b */
void fn_8040CE60(Vec3f *vector);                     /* vector = 0 */
float fn_8040CE74(Vec3f *vector);                    /* normalises it */
float fn_8040CF90(const Vec3f *a, const Vec3f *b);   /* dot product */
float fn_8040D180(const Vec3f *a, const Vec3f *b);   /* the squared distance between */
Vec3f fn_80410098(const Vec3f *vector, const Vec3f *rotation, int order);
}

/* fn_8040CD74 returns lhs - rhs through its first argument, as a function returning a Vec3f does
 * (as in Game/PlayerBehaviorLightDash.cpp). */
typedef Vec3f (*VectorSubtract)(const Vec3f *lhs, const Vec3f *rhs);
#define Subtract(lhs, rhs) (reinterpret_cast<VectorSubtract>(fn_8040CD74)(lhs, rhs))

/* The player's members used here (struct name unknown; Player::PlayerBase leaves them
 * unmapped). */
struct UnkPlayerFields_8009FD64 {
    /* 0x000 */ u8 field_0x0[0x64];
    /* 0x064 */ s32 m_kind;
    /* 0x068 */ u8 field_0x68[0x11C - 0x68];
    /* 0x11C */ Mtx34f m_inputMatrix;
    /* 0x14C */ u8 field_0x14C[0x1E4 - 0x14C];
    /* 0x1E4 */ Mtx34f m_inputMatrix2; /* used when flag 0 is set */
    /* 0x214 */ u8 field_0x214[0x224 - 0x214];
    /* 0x224 */ Player::Controller *m_controller;
    /* 0x228 */ u8 field_0x228[0x248 - 0x228];
    /* 0x248 */ UnkPlayerPart_80095BD4 *m_colli;
};

#define Fields(player) reinterpret_cast<UnkPlayerFields_8009FD64 *>(player)

/* vector * scale (an inline wrapper, which orders the call as the game does; as in
 * Game/PlayerBehaviorGrind.cpp). */
inline Vec3f Scale(const Vec3f &vector, float scale) {
    return fn_8040CDA8(&vector, scale);
}

/* A distance (name and meaning inferred). As a call it is not a constant expression, so the
 * constant is stored by a static initializer, as in retail (see Game/VehicleColliMove.cpp). */
inline float Span(float length) {
    return length;
}

static float s_reach = Span(50.0f);

/* The control's name (in .sdata, as the gadgets' s_taskName). */
static const char *s_name = "UserInput";

namespace Player {

/* StartUseWeaponCommand, EndUseWeaponCommand, JumpDashCommand (class names from RTTI): commands
 * 0x115, 0x117 and 0x108 (inline constructors; their destructors are out of line here). */
class StartUseWeaponCommand : public PlayerCommand {
  public:
    StartUseWeaponCommand(int arg1) : PlayerCommand(UserInputCommandId(0x115), arg1) {}
    virtual ~StartUseWeaponCommand();
};
SIZE_ASSERT(StartUseWeaponCommand, 0x14);

class EndUseWeaponCommand : public PlayerCommand {
  public:
    EndUseWeaponCommand(int arg1) : PlayerCommand(UserInputCommandId(0x117), arg1) {}
    virtual ~EndUseWeaponCommand();
};
SIZE_ASSERT(EndUseWeaponCommand, 0x14);

class JumpDashCommand : public PlayerCommand {
  public:
    JumpDashCommand(int arg1) : PlayerCommand(UserInputCommandId(0x108), arg1) {}
    virtual ~JumpDashCommand();
};
SIZE_ASSERT(JumpDashCommand, 0x14);

namespace Control {

/* The commands a button sends when pressed and when released (name inferred). */
struct CommandPair {
    CharCommand *m_press;
    CharCommand *m_release;
};

class UserInput : public ControlBase {
  public:
    UserInput(const boost::shared_ptr<PlayerBase> &player);
    virtual ~UserInput();
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x14(float dt);

    bool CheckIdle();
    bool IsParallelMove(const UnkPlayerInput_800A85B0 *pad);
    bool CanHomingAttack(const boost::weak_ptr<Target> &target);
    bool CanLightDash(const Vec3f &move);
    Fn8040CopyVector3 GetMove(const UnkPlayerInput_800A85B0 *pad);
    void CollectCommands(const UnkPlayerInput_800A85B0 *pad, const Vec3f &move,
                         boost::shared_ptr<CharInfo> &bReceiver, boost::shared_ptr<CharInfo> &cReceiver,
                         CommandPair *a, CommandPair *b, CommandPair *c, CommandPair *d);
    void ShowHints(const CommandPair *a, const CommandPair *b, const CommandPair *c,
                   const CommandPair *d);
    void ClearHints();

    /* 0x14 */ boost::shared_ptr<VehicleArrow> m_arrow;
};
SIZE_ASSERT(UserInput, 0x1C);

} // namespace Control

JumpDashCommand::~JumpDashCommand() {}

EndUseWeaponCommand::~EndUseWeaponCommand() {}

StartUseWeaponCommand::~StartUseWeaponCommand() {}

namespace Control {

UserInput::UserInput(const boost::shared_ptr<PlayerBase> &player) : ControlBase(player, s_name, 0) {}

UserInput::~UserInput() {}

void UserInput::Enter(int prevId, const InitializeArgs &args) {
    m_arrow = fn_8033A9B8();
}

void UserInput::Leave(int nextId) {}

void UserInput::vf_0x14(float dt) {
    if (m_player->field_0x5C == 2) {
        if (fn_801F42C8(&m_player->m_self, &m_player->m_position)) {
            return;
        }
        if (!CheckIdle()) {
            return;
        }
    }
    UnkPlayerInput_800A85B0 *pad = fn_8006F8D0(m_player.get());
    if (m_arrow) {
        fn_8033AC24(m_arrow.get(), false);
    }
    if (lbl_8057D6A0.field_0xC8 || fn_8006CD78(m_player->m_stateBits, 0x2A)) {
        if (fn_8006CD78(m_player->m_stateBits, 0x19)) {
            m_player->PushCommand(new EndUseWeaponCommand(2));
        }
        if (fn_8006CD78(m_player->m_stateBits, 0x10) && fn_80014E34(&m_player->m_flags, 8)) {
            m_player->PushCommand(new ControlVehicleCommand(7, 2));
        }
        ClearHints();
        return;
    }
    if (IsParallelMove(pad)) {
        if (!fn_8006CD78(m_player->m_stateBits, 0x3C)) {
            m_player->PushCommand(new StartParallelMoveCommand(1));
        }
    } else if (fn_8006CD78(m_player->m_stateBits, 0x3C)) {
        m_player->PushCommand(new EndParallelMoveCommand(1));
    }
    if (!lbl_8057D6A0.field_0x125) {
        Mission::MissionManager *missions = fn_8006D6A8();
        u32 pressed = pad->m_pressed;
        int current = missions->m_currentPlayer;
        int player = current;
        if (pressed & 0x200) {
            player = 0;
        }
        if (pressed & 0x400) {
            player = 1;
        }
        if (pressed & 0x800) {
            player = 2;
        }
        if (player != current) {
            if (missions->IsFlagSet(player)) {
                missions->SetPlayer(player);
            } else {
                m_player->m_sound->StopVoice(0);
            }
        }
    }
    if (pad->m_released & 0x20) {
        m_player->PushCommand(new EndAttackCommand(2));
    }
    if (pad->m_pressed & 0x40) {
        m_player->PushCommand(new StartActionCommand(2));
    }
    if (pad->m_released & 0x40) {
        m_player->PushCommand(new EndActionCommand(2));
    }
    Vec3f move;
    fn_800091EC(&move, &(const Vec3f &)GetMove(pad));
    if (!fn_8006CD78(m_player->m_stateBits, 0x2E) &&
        !(move.x * move.x + move.y * move.y + move.z * move.z < 0.0001f)) {
        m_player->PushCommand(new AccelerateInputCommand(move, 1));
    }
    CommandPair a = {0, 0};
    CommandPair b = {0, 0};
    CommandPair c = {0, 0};
    CommandPair d = {0, 0};
    boost::shared_ptr<CharInfo> bReceiver(m_player);
    boost::shared_ptr<CharInfo> cReceiver(m_player);
    CollectCommands(pad, move, bReceiver, cReceiver, &a, &b, &c, &d);
    ShowHints(&a, &b, &c, &d);
    bool taken = false;
    if (c.m_press) {
        if (pad->m_pressed & 0x40) {
            cReceiver->PushCommand(c.m_press);
            if (cReceiver != m_player) {
                taken = true;
            }
        } else {
            delete c.m_press;
        }
    }
    if (c.m_release) {
        if (pad->m_released & 0x40) {
            cReceiver->PushCommand(c.m_release);
        } else {
            delete c.m_release;
        }
    }
    if (b.m_press) {
        if ((pad->m_pressed & 0x10) && !taken) {
            bReceiver->PushCommand(b.m_press);
            if (bReceiver != m_player) {
                taken = true;
            }
        } else {
            delete b.m_press;
        }
    }
    if (b.m_release) {
        if (pad->m_released & 0x10) {
            bReceiver->PushCommand(b.m_release);
        } else {
            delete b.m_release;
        }
    }
    if (a.m_press) {
        if ((pad->m_pressed & 0x20) && !taken) {
            m_player->PushCommand(a.m_press);
        } else {
            delete a.m_press;
        }
    }
    if (a.m_release) {
        if (pad->m_released & 0x20) {
            m_player->PushCommand(a.m_release);
        } else {
            delete a.m_release;
        }
    }
    if (d.m_press) {
        if ((pad->m_pressed & 0x80) && !taken) {
            m_player->PushCommand(d.m_press);
        } else {
            delete d.m_press;
        }
    }
    if (d.m_release) {
        if (pad->m_released & 0x80) {
            m_player->PushCommand(d.m_release);
        } else {
            delete d.m_release;
        }
    }
}

/* Hands an idle Npc player to control 2 after ten seconds without input (false then). */
bool UserInput::CheckIdle() {
    if (fn_8006F8D0(m_player.get())->GetIdleTime() >= 10.0f) {
        fn_8009EBA4(Fields(m_player.get())->m_controller, &InitializeArgs(2));
        return false;
    }
    return true;
}

/* Not called anywhere: the linker strips it from the game. The original pools vf_0x14's float
 * literal before CheckIdle's, which is what compiling a function that uses it at this point of the
 * file does (inferred from the order of the literals; see docs/research-notes.md). */
bool UserInputMoveLiteralOutOfLine(const Vec3f &move) {
    return move.x < 0.0001f;
}

/* Whether the player moves sideways (button 0x100 held). */
bool UserInput::IsParallelMove(const UnkPlayerInput_800A85B0 *pad) {
    if (m_player->field_0x5C == 2) {
        return false;
    }
    if (!fn_8006CD78(m_player->m_stateBits, 0x2E) && (pad->m_held & 0x100)) {
        return true;
    }
    return false;
}

/* Whether the player can reach target with a homing attack. */
bool UserInput::CanHomingAttack(const boost::weak_ptr<Target> &target) {
    if (target.expired()) {
        return false;
    }
    boost::shared_ptr<Target> locked = target.lock();
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)locked->GetPosition(m_player.get()));
    float distance = fn_8040D180(&position, &m_player->m_position);
    UnkSegment_80020A20 segment;
    fn_80020A20(&segment, &m_player->m_position, &position);
    boost::weak_ptr<CharaColliBaseCharInfo> hit;
    float clear = fn_800979E8(&m_player, 0, 0, &hit, 0, 0, &segment, 0, 0x1A93, 0x40000, 2);
    if (clear <= 0.0f || clear >= distance - 1.0f) {
        return true;
    }
    if (!hit.expired()) {
        boost::shared_ptr<CharaColliBaseCharInfo> collider = hit.lock();
        if (fn_800586EC(collider.get()).lock() == locked->m_owner.lock()) {
            return true;
        }
    }
    return false;
}

/* Whether the player's light dash target is in reach. */
bool UserInput::CanLightDash(const Vec3f &move) {
    if (!fn_80014E34(&m_player->m_flags, 3)) {
        return false;
    }
    boost::shared_ptr<Target> target = m_player->m_target->vf_0x28().lock();
    if (target && target->IsLightDashTarget()) {
        Vec3f position;
        fn_800091EC(&position, &(const Vec3f &)target->GetPosition(m_player.get()));
        float distance = fn_8040D180(&position, &m_player->m_position);
        if (distance < s_reach * s_reach) {
            UnkSegment_80020A20 segment;
            fn_80020A20(&segment, &m_player->m_position, &position);
            float clear = fn_800979A0(&m_player, &segment, 0, 0x1A93, 0x40000, 2);
            if (clear <= 0.0f || clear >= distance - 1.0f) {
                return true;
            }
        }
    }
    return false;
}

/* The stick as a move in the world. */
Fn8040CopyVector3 UserInput::GetMove(const UnkPlayerInput_800A85B0 *pad) {
    Vec3f move;
    fn_8040CE60(&move);
    if (!fn_8000A950(pad->m_stick.x) || !fn_8000A950(pad->m_stick.y)) {
        move.x = pad->m_stick.x;
        move.z = pad->m_stick.y;
    }
    if (!fn_8006CD78(m_player->m_stateBits, 0x3A)) {
        fn_800091D0(&move, &(const Vec3f &)fn_80093DD0(&m_player, &move));
    }
    if (fn_8040CF2C(&move) > 1.0f) {
        fn_8040CE74(&move);
    }
    if (fn_80014E34(&m_player->m_flags, 0)) {
        fn_800091D0(&move, &(const Vec3f &)fn_8040B67C(&Fields(m_player.get())->m_inputMatrix2, &move));
    } else {
        fn_800091D0(&move, &(const Vec3f &)fn_8040B67C(&Fields(m_player.get())->m_inputMatrix, &move));
    }
    return Fn8040CopyVector3(move);
}

void UserInput::CollectCommands(const UnkPlayerInput_800A85B0 *pad, const Vec3f &move,
                                boost::shared_ptr<CharInfo> &bReceiver,
                                boost::shared_ptr<CharInfo> &cReceiver, CommandPair *a, CommandPair *b,
                                CommandPair *c, CommandPair *d) {
    if (fn_8006CD78(m_player->m_stateBits, 0x16) || fn_8006CD78(m_player->m_stateBits, 0x14)) {
        return;
    }
    if (fn_8006CD78(m_player->m_stateBits, 0x10)) {
        if (fn_80014E34(&m_player->m_flags, 8)) {
            a->m_press = new ControlVehicleCommand(6, 2);
            a->m_release = new ControlVehicleCommand(7, 2);
        } else if (fn_8006CD78(m_player->m_stateBits, 0x1F) &&
                   fn_8009EB80(Fields(m_player.get())->m_controller)) {
            a->m_press = new StartUseWeaponCommand(2);
            a->m_release = new EndUseWeaponCommand(2);
        }
        b->m_press = new ControlVehicleCommand(2, 2);
        b->m_release = new ControlVehicleCommand(3, 2);
        c->m_press = new ControlVehicleCommand(4, 2);
        c->m_release = new ControlVehicleCommand(5, 2);
        return;
    }

    if (fn_8006CD78(m_player->m_stateBits, 0x23)) {
        d->m_press = new ChaosBlastCommand(2);
    } else if (fn_8006CD78(m_player->m_stateBits, 0x22)) {
        d->m_press = new ChaosControlCommand(2);
    } else if (!fn_8006CD78(m_player->m_stateBits, 0x33) && fn_8006CD78(m_player->m_stateBits, 0x1F) &&
               !fn_8006CD78(m_player->m_stateBits, 0xB)) {
        d->m_press = new CastWeaponCommand(2);
    }

    if (fn_8006CD78(m_player->m_stateBits, 0x1E)) {
        c->m_press = new ThrowCommand(boost::weak_ptr<PickupCharInfo>(), 2);
    } else if (fn_800ABB48(fn_8009EB50(Fields(m_player.get())->m_controller)) == 4) {
        c->m_press = new DarkSpinCommand(*fn_8009EB50(Fields(m_player.get())->m_controller), 2);
    } else if (CanLightDash(move)) {
        c->m_press = new LightDashCommand(2);
    } else if (fn_80014E34(&m_player->m_flags, 4) && m_player->field_0x5C == 1) {
        c->m_press = new SlidingCommand(2);
    } else if (fn_80014E34(&m_player->m_flags, 5)) {
        c->m_press = new SpinDashCommand(2);
    }

    if (fn_8006CD78(m_player->m_stateBits, 0x1F) && fn_80014E34(&m_player->m_flags, 7) &&
        fn_8009EB80(Fields(m_player.get())->m_controller)) {
        a->m_press = new StartUseWeaponCommand(2);
        a->m_release = new EndUseWeaponCommand(2);
    } else if (fn_80014E34(&m_player->m_flags, 4) && m_player->field_0x5C == 1) {
        a->m_press = new SlidingCommand(2);
    } else if (fn_80014E34(&m_player->m_flags, 6)) {
        a->m_press = new StartAttackCommand(2);
    }

    if (fn_80014E34(&m_player->m_flags, 2)) {
        boost::weak_ptr<Target> target;
        if (lbl_804F1834[Fields(m_player.get())->m_kind].m_canHomingAttack) {
            target = fn_80206814(m_player->m_target, &move);
        }
        if (CanHomingAttack(target)) {
            b->m_press = new HomingAttackCommand(target, 2);
        } else {
            b->m_press = new JumpDashCommand(2);
        }
    } else if (fn_80014E34(&m_player->m_flags, 1)) {
        b->m_press = new StartJumpCommand(2);
    }
    if (fn_8006CD78(m_player->m_stateBits, 4)) {
        b->m_release = new EndJumpCommand(2);
    }

    if (m_player->field_0x5C == 1) {
        boost::weak_ptr<Target> target = m_player->m_target->vf_0x28();
        boost::shared_ptr<Target> locked = target.lock();
        if (!fn_8006CD78(m_player->m_stateBits, 0x1E) && locked) {
            float radius = locked->m_radius;
            if (radius < 0.0f ||
                radius * radius > fn_8040D180(&(const Vec3f &)locked->GetPosition(m_player.get()),
                                              &m_player->m_position)) {
                Vec3f up;
                fn_800091EC(&up, &(const Vec3f &)Scale(fn_80079698(m_player.get()), m_player->field_0x274));
                Vec3f bottom;
                fn_800091EC(&bottom, &(const Vec3f &)Subtract(&m_player->m_position, &up));
                Vec3f top;
                fn_800091EC(&top, &(const Vec3f &)fn_8040CDD0(&m_player.px->m_position,
                                                              &(const Vec3f &)fn_8040CDA8(&up, 2.0f)));
                Vec3f position;
                fn_800091EC(&position, &(const Vec3f &)locked->GetPosition(m_player.get()));
                if (fn_8040CF90(&(const Vec3f &)Subtract(&position, &bottom), &up) >= 0.0f &&
                    fn_8040CF90(&(const Vec3f &)Subtract(&top, &position), &up) >= 0.0f) {
                    if (locked->IsPickupTarget()) {
                        if (c->m_press) {
                            delete c->m_press;
                        }
                        c->m_press = new ApproachControlCommand(1, target, 2);
                    } else if (locked->IsTurnOverTarget()) {
                        if (c->m_press) {
                            delete c->m_press;
                        }
                        c->m_press = new ApproachControlCommand(2, target, 2);
                    }
                }
            }
        }
    }

    if (m_player->field_0x5C == 1) {
        int kind = 0;
        boost::shared_ptr<CharInfo> chara = fn_80095A10(Fields(m_player.get())->m_colli).lock();
        if (chara) {
            kind = chara.get()->GetKind();
        }
        switch (kind) {
        case 0:
            break;
        case 0x1B:
            chara->PushCommand(new UseCommand(boost::weak_ptr<CharInfo>(m_player->m_self)));
            fn_800792D4(m_player.get(), 0.25f);
            if (a->m_press) {
                delete a->m_press;
                a->m_press = 0;
            }
            if (a->m_release) {
                delete a->m_release;
                a->m_release = 0;
            }
            if (b->m_press) {
                delete b->m_press;
                b->m_press = 0;
            }
            if (b->m_release) {
                delete b->m_release;
                b->m_release = 0;
            }
            if (c->m_press) {
                delete c->m_press;
                c->m_press = 0;
            }
            if (c->m_release) {
                delete c->m_release;
                c->m_release = 0;
            }
            return;
        case 0x34:
            if (!fn_8006CD78(m_player->m_stateBits, 0x33) && m_player->m_weapon) {
                if (static_cast<s32>(chara->field_0x34) == m_player->m_weapon->GetWeaponType() &&
                    m_player->m_weapon->CanUse()) {
                    chara->PushCommand(new UseCommand(boost::weak_ptr<CharInfo>(m_player->m_self)));
                } else if (fn_8006CD78(m_player->m_stateBits, 0x1F)) {
                    if (!fn_8006CD78(m_player->m_stateBits, 0x1E)) {
                        if (c->m_press) {
                            delete c->m_press;
                        }
                        c->m_press = new PickupWeaponCommand(boost::weak_ptr<CharInfo>(chara), 2);
                    }
                } else if (m_player->field_0x98 == 0.0f) {
                    m_player->PushCommand(new PickupWeaponCommand(boost::weak_ptr<CharInfo>(chara), 2));
                    m_player->field_0x98 = 0.001f;
                    return;
                }
            }
            break;
        case 0x33:
            if (!fn_8006CD78(m_player->m_stateBits, 0xB) && !fn_8006CD78(m_player->m_stateBits, 0x1E) &&
                !fn_8006CD78(m_player->m_stateBits, 0x10)) {
                boost::shared_ptr<Vehicle::TVehicleBase> vehicle =
                    boost::dynamic_pointer_cast<Vehicle::TVehicleBase>(chara);
                if (vehicle && vehicle->GetRider().expired()) {
                    if (c->m_press) {
                        delete c->m_press;
                    }
                    c->m_press = new GetOnVehicleCommand(boost::weak_ptr<Vehicle::TVehicleBase>(vehicle), 2);
                    if (m_arrow) {
                        Vec3f seat;
                        Vec3f rotation(0.0f, 0.0f, 0.0f);
                        vehicle->GetSeat(&seat, &rotation);
                        fn_8033AC24(m_arrow.get(), true);
                        fn_8033ABFC(m_arrow.get(), &seat);
                    }
                }
            }
            break;
        case 0x86:
            if (!fn_8006CD78(m_player->m_stateBits, 0xB) && !fn_8006CD78(m_player->m_stateBits, 0x1E)) {
                Vec3f direction;
                fn_800091EC(&direction, &(const Vec3f &)fn_80410098(&(const Vec3f &)Vec3f(0.0f, 0.0f, -1.0f),
                                                                    &chara->m_rotation, chara->field_0x30));
                UnkCircuitTarget_800ABCE8 rail;
                if (fn_800ABC0C(&rail, &m_player, &m_player->m_position, &direction, 1, 40.0f) == 4) {
                    if (c->m_press) {
                        delete c->m_press;
                    }
                    c->m_press = new DarkSpinCommand(rail, 2);
                }
            }
            break;
        }
    }

    if (m_player->field_0x5C == 1) {
        UnkColliMoveContact_803334A4 *contact = fn_80078DA0(m_player.get());
        if (contact && contact->IsValid()) {
            boost::shared_ptr<CharInfo> chara = contact->m_chara.lock();
            switch (chara->GetKind()) {
            case 0xC:
                if (!fn_8006CD78(m_player->m_stateBits, 0x1E) && fn_8040CF2C(&m_player->field_0x18) < 900.0f) {
                    if (c->m_press) {
                        delete c->m_press;
                    }
                    c->m_press = new UseCommand(boost::weak_ptr<CharInfo>(m_player->m_self));
                    cReceiver = chara;
                    fn_8033AC24(m_arrow.get(), true);
                    fn_8033ABFC(m_arrow.get(), &chara->m_position);
                }
                break;
            case 0x37:
                if (b->m_press) {
                    delete b->m_press;
                }
                b->m_press = new UseCommand(boost::weak_ptr<CharInfo>(m_player->m_self));
                bReceiver = chara;
                break;
            }
        }
    }

    if (fn_8006CD78(m_player->m_stateBits, 0x15)) {
        if (c->m_press) {
            delete c->m_press;
        }
        c->m_press = 0;
    }
}

/* Shows the commands of buttons a, c and d on the player's button hints. */
void UserInput::ShowHints(const CommandPair *a, const CommandPair *b, const CommandPair *c,
                          const CommandPair *d) {
    UnkPlayerInfo_80178B5C *info = fn_80178B5C(fn_80079808(), m_player->field_0x54);
    int hintA = 0;
    int hintB = 0;
    int hintC = 0;
    int hintD = 0;
    if (a->m_press) {
        switch (a->m_press->TypeId()) {
        case 0x15:
            break;
        case 0x123:
            hintA = 2;
            break;
        case 0x102:
            if (fn_8006CD78(m_player->m_stateBits, 0x1F)) {
                hintA = 1;
            }
            break;
        case 0x115:
            hintA = 1;
            break;
        case 0x109:
            hintA = 3;
            break;
        }
    }
    if (c->m_press) {
        switch (c->m_press->TypeId()) {
        case 0x110:
            hintC = 1;
            break;
        case 0x119:
            hintC = 3;
            break;
        case 0x118:
            if (fn_8006CD78(m_player->m_stateBits, 0x1F)) {
                hintC = 4;
            } else {
                hintC = 2;
            }
            break;
        case 0x10F:
            hintC = 7;
            break;
        case 0x11E:
            hintC = 5;
            break;
        case 0x109:
            hintC = 8;
            break;
        case 0x10E:
            hintC = 9;
            break;
        case 0x11C:
            switch (static_cast<ApproachControlCommand *>(c->m_press)->m_kind) {
            case 1:
                hintC = 2;
                break;
            case 2:
                hintC = 6;
                break;
            }
            break;
        case 0x123: {
            boost::shared_ptr<Vehicle::TVehicleBase> vehicle =
                static_cast<GetOnVehicleCommand *>(c->m_press)->m_vehicle.lock();
            if (vehicle) {
                hintC = 0xB;
                if (info) {
                    fn_8016EED8(info, fn_801B6938(vehicle.get()));
                }
            }
            break;
        }
        case 0x15:
            hintC = 0xC;
            break;
        case 0x11D:
            break;
        case 0xF:
            hintC = 0xA;
            break;
        }
    }
    if (d->m_press) {
        switch (d->m_press->TypeId()) {
        case 0x119:
            hintD = 3;
            break;
        case 0x113:
            hintD = 1;
            break;
        case 0x114:
            hintD = 2;
            break;
        }
    }
    if (info) {
        fn_8016EF08(info, hintA);
        fn_8016EF00(info, hintB);
        fn_8016EEF8(info, hintC);
        fn_8016EEF0(info, hintD);
    }
}

/* Clears the hint of button d. */
void UserInput::ClearHints() {
    UnkPlayerInfo_80178B5C *info = fn_80178B5C(fn_80079808(), m_player->field_0x54);
    if (info) {
        fn_8016EEF0(info, 0);
    }
}

} // namespace Control
} // namespace Player

/* Makes the pad control of a player (called by the control factories). */
extern "C" Player::Control::ControlBase *fn_8009F9FC(const boost::shared_ptr<PlayerBase> &player) {
    return new Player::Control::UserInput(player);
}
