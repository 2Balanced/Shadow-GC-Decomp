/* Player::Control::Bukuu (class name from RTTI; "bukuu", Japanese for the empty sky): control 5
 * of a player, a free flight steered with the pad. Entering it stops the player, hands it to an
 * outside control (Player::StartExternalControlCommand), plays motion 0x23 and, for a
 * Player::Player, asks for camera 3; every frame it moves the player along the pad (the two
 * buttons rise and sink) by SetPositionCommand. It hands control back (control 0) once behavior
 * 0x20 has run and ended; leaving it ends the outside control and drops the camera request.
 * Member and function names are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#define PLAYER_CONTROL_SLOT_ARGS
#define CHARCOMMAND_INLINE_CONSTRUCTORS
#define GADGETCHARCOMMANDS_INLINE_EXTERNAL_CONTROL
#define GADGETCHARCOMMANDS_INLINE_END_EXTERNAL_CONTROL
#include "Game/CharCommand.h"
#include "Game/GadgetCharCommands.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerControl.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Player::PlayerBase;

namespace SonicteamUSA {
namespace System {
namespace Camera {
class CameraMode;
class CameraRequest;
} // namespace Camera
} // namespace System
} // namespace SonicteamUSA

using SonicteamUSA::System::Camera::CameraMode;
using SonicteamUSA::System::Camera::CameraRequest;

/* The pad as a player reads it (struct name unknown): the stick and the buttons. */
struct UnkPlayerPad_8006F8D0 {
    /* 0x00 */ float m_x;
    /* 0x04 */ float m_y;
    /* 0x08 */ u8 field_0x8[0x18 - 0x8];
    /* 0x18 */ u32 m_buttons;
};

/* 1 while a button is held, else 0 (name inferred). */
inline float ButtonValue(const UnkPlayerPad_8006F8D0 *pad, u32 button) {
    return (pad->m_buttons & button) ? 1.0f : 0.0f;
}

extern "C" {
/* The player's pad (see Game/BossSonic.cpp). */
UnkPlayerPad_8006F8D0 *fn_8006F8D0(PlayerBase *player);
bool fn_8006CD78(u32 *bits, int bit);
void fn_8007939C(PlayerBase *player, bool on);
/* vector turned by the angle of the pad's direction relative to the player (Game/PlayerUtil.cpp). */
Vec3f fn_80093DD0(const boost::shared_ptr<PlayerBase> *player, const Vec3f *vector);
/* Switches the player's control (Player::Controller; see its unit). */
void fn_8009EBA4(Task *controller, const Player::Control::InitializeArgs *args);
/* Whether the player's behavior is id (behaviour inferred). */
bool fn_800A7EEC(void *behaviors, int id);
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
/* The camera mode of a kind for a pad, a request for it, and dropping a request (behaviour
 * inferred; see Game/BossCtrlUnit.cpp). */
boost::shared_ptr<CameraMode> fn_8026F6A4(int kind, void *args, int index);
boost::shared_ptr<CameraRequest> fn_8026F464(boost::shared_ptr<CameraMode> mode, int flags,
                                             int arg2, int arg3, int arg4);
void fn_8026F444(boost::shared_ptr<CameraRequest> *request);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b); /* a + b */
void fn_8040CE38(Vec3f *vector, float scale);
}

/* The player's members used here (struct name unknown; Player::PlayerBase leaves them
 * unmapped). */
struct UnkPlayerFields_8009E324 {
    /* 0x000 */ u8 field_0x0[0x54];
    /* 0x054 */ s32 m_pad;
    /* 0x058 */ u8 field_0x58[0xA8 - 0x58];
    /* 0x0A8 */ u32 m_stateBits[2];
    /* 0x0B0 */ u8 field_0xB0[0x224 - 0xB0];
    /* 0x224 */ Task *m_controller;
    /* 0x228 */ u32 field_0x228;
    /* 0x22C */ void *m_behaviors;
};

#define Fields(player) reinterpret_cast<UnkPlayerFields_8009E324 *>(player)

/* The cache of the stage tasks' accessor (fn_800D51EC; Game/StageCameraList.h): this unit's .sbss
 * holds it, before the unit's own object (inferred from the layout; nothing here uses it). */
struct UnkStageTasks_800D51EC;
UnkStageTasks_800D51EC *lbl_805EE558;

namespace Player {
namespace Control {

class Bukuu : public ControlBase {
  public:
    Bukuu(const boost::shared_ptr<PlayerBase> &player);
    virtual ~Bukuu();
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x14(float dt);

    static const char *s_name;

    /* 0x14 */ boost::shared_ptr<CameraRequest> m_request;
    /* 0x1C */ u32 m_flags; /* bit 0: behavior 0x20 has run */
};
SIZE_ASSERT(Bukuu, 0x20);

const char *Bukuu::s_name = "Bukuu";

Bukuu::Bukuu(const boost::shared_ptr<PlayerBase> &player) : ControlBase(player, s_name, 5) {
    fn_80014E04(&m_flags);
}

Bukuu::~Bukuu() {}

void Bukuu::Enter(int prevId, const InitializeArgs &args) {
    m_player->PushCommand(new ClrVelocityCommand(2));
    m_player->PushCommand(new StartExternalControlCommand(0, 2));
    m_player->PushCommand(new SetMotionCommand(0x23, 2, 0.0f));
    boost::shared_ptr<Player> player = boost::dynamic_pointer_cast<Player>(m_player);
    if (player) {
        boost::shared_ptr<CameraMode> mode = fn_8026F6A4(3, 0, Fields(player.get())->m_pad);
        m_request = fn_8026F464(mode, 0x401, 0, 0, 0);
    }
    fn_8007939C(m_player.get(), true);
    fn_80014E10(&m_flags);
}

void Bukuu::Leave(int nextId) {
    m_player->PushCommand(new EndExternalControlCommand(2));
    boost::shared_ptr<Player> player = boost::dynamic_pointer_cast<Player>(m_player);
    if (player) {
        fn_8026F444(&m_request);
    }
    fn_8007939C(m_player.get(), false);
}

void Bukuu::vf_0x14(float dt) {
    if (fn_8006CD78(Fields(m_player.get())->m_stateBits, 0x2A)) {
        return;
    }
    if (!fn_800A7EEC(Fields(m_player.get())->m_behaviors, 0x20)) {
        if (fn_80014E34(&m_flags, 0)) {
            fn_8009EBA4(Fields(m_player.get())->m_controller, &InitializeArgs(0));
        }
    } else {
        fn_80014E1C(&m_flags, 0);
    }
    UnkPlayerPad_8006F8D0 *pad = fn_8006F8D0(m_player.get());
    Vec3f move;
    move.x = 10.0f * pad->m_x;
    move.z = 10.0f * pad->m_y;
    /* The height is kept in rise as well as in move.y (the game reuses the register; names
     * inferred). */
    float rise = 0.0f;
    move.y = rise;
    float up = 10.0f * ((pad->m_buttons & 0x80) ? 1.0f : rise);
    move.y = rise = rise + up;
    float down = 10.0f * ButtonValue(pad, 0x10);
    move.y = rise - down;
    fn_800091D0(&move, &(const Vec3f &)fn_80093DD0(&m_player, &move));
    fn_8040CE38(&move, 60.0f * dt);
    m_player->PushCommand(
        new SetPositionCommand(fn_8040CDD0(&m_player->m_position, &move), 2));
}

} // namespace Control
} // namespace Player

/* Makes a player's Bukuu control (name inferred). */
extern "C" Player::Control::ControlBase *fn_8009E324(const boost::shared_ptr<PlayerBase> &player) {
    return new Player::Control::Bukuu(player);
}
