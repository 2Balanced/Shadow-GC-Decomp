/* NpcRangeCheck (file and function names inferred; the unit has no RTTI or data): the check the
 * NPC controls (Game/NpcControlFlyType.cpp, Game/NpcControlSonicType.cpp) and the player's input
 * control (Game/PlayerControlUserInput.cpp) make each frame on a character. If the character is an
 * NPC that has strayed too far, from the player it follows or, with none, from the camera, it is
 * brought back (control 4, or fn_80073964) and the check returns true. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include "Game/MathTypes.h"
#include "Game/Npc.h"
#include "Game/Player.h"
#include "Game/PlayerControl.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

/* The camera manager (see Game/CameraManagerUnit.h). */
class UnkCameraManager_80009548;

extern "C" {
/* NPC helpers (Game/Npc.cpp; behaviours inferred): whether the NPC is being brought back, the
 * distance it may stray, whether it follows a player, the player it follows, and brings it back
 * to the camera. */
bool fn_80073914(Player::Npc::Npc *npc);
float fn_8007382C(Player::Npc::Npc *npc);
bool fn_80073708(Player::Npc::Npc *npc);
boost::weak_ptr<Player::PlayerBase> fn_8007363C(Player::Npc::Npc *npc);
void fn_80073964(Player::Npc::Npc *npc);
/* The squared distance between two points (behaviour inferred). */
float fn_8040D180(const Vec3f *a, const Vec3f *b);
/* Switches the control of the character task (see Game/BossSonicExecuter.cpp). */
void fn_8009EBA4(Task *task, const Player::Control::InitializeArgs *args);
/* The camera manager and the position of its camera index (see Game/AudioChannel.cpp). */
UnkCameraManager_80009548 *fn_80009548(void);
const Vec3f *fn_80010244(UnkCameraManager_80009548 *manager, int index);
}

/* Whether the character of player is an NPC that had to be brought back (name inferred).
 * position is where the caller keeps the character. */
extern "C" bool fn_801F42C8(const boost::weak_ptr<Player::PlayerBase> *player,
                            const Vec3f *position) {
    boost::shared_ptr<Player::Npc::Npc> npc =
        boost::dynamic_pointer_cast<Player::Npc::Npc>(player->lock());
    if (npc.get() != 0) {
        if (fn_80073914(npc.get())) {
            return true;
        }
        float range = fn_8007382C(npc.get());
        if (fn_80073708(npc.get())) {
            boost::shared_ptr<Player::PlayerBase> leader = fn_8007363C(npc.get()).lock();
            if (leader.get() != 0 &&
                fn_8040D180(&npc->m_position, &leader->m_position) > range * range) {
                fn_8009EBA4(npc->m_task, &Player::Control::InitializeArgs(4));
                return true;
            }
        } else if (fn_8040D180(position, fn_80010244(fn_80009548(), 0)) > range * range) {
            fn_80073964(npc.get());
            return true;
        }
    }
    return false;
}
