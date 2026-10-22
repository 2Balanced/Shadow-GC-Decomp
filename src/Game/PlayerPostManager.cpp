/* @unnamed@PlayerPostManager_cpp@::PostManageTask (class name from RTTI): the task that, after
 * the players have moved, hands each player's position and state to the camera of the player's
 * screen as a PJSCamera::PJSCameraTargetInfo. The stage manager starts it (fn_8029773C). Member
 * and function names other than the class's are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first).
 *
 * NonMatching: 4 of 5 functions match, and the data. PostManage (0x80297798) differs from the aim
 * block on: retail calls the player pointer's operator-> out of line before fn_8006F8D0 (as when
 * its result feeds an inline call), which no form tried here reproduces; the rest of the function
 * then shifts by one instruction. */
#define MSL_LIST_ACCESSORS
#include "Game/CameraManagerUnit.h"
#include "Game/CharInfo.h"
#include "Game/GadgetUtil.h"
#include "Game/MathTypes.h"
#include "Game/CameraTargetInfo.h"
#include "Game/Player.h"
#include "Game/TVehicleBase.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using SonicteamUSA::System::Camera::CameraManagerUnit;

/* The game state (struct name unknown; see Game/TGi.h). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xC8];
    /* 0x0C8 */ u8 field_0xC8; /* meaning unknown */
    /* 0x0C9 */ u8 field_0xC9[0x150 - 0xC9];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* What fn_8006F8D0 returns for a player (struct name unknown). */
struct UnkPlayerAim_8006F8D0 {
    /* 0x0 */ u8 field_0x0[0x8];
    /* 0x8 */ float field_0x8;
    /* 0xC */ float field_0xC;
};

extern "C" {
/* A character of the list by index (see Game/CityBombSmall.cpp). */
boost::weak_ptr<CharInfo> fn_800A8C54(UnkCharaList_80074194 *list, int index);
/* The camera manager and the camera of a screen (see Game/CameraManagerUnit.cpp). */
UnkCameraManager_80009548 *fn_80009548(void);
boost::shared_ptr<CameraManagerUnit> fn_8000D0F0(UnkCameraManager_80009548 *manager, int index);
/* A player's look-at and up vectors (behaviour inferred; see Game/PlayerShadowWeapon.cpp). */
Vec3f fn_80079698(Player::PlayerBase *player);
Vec3f fn_800796DC(Player::PlayerBase *player);
/* Tests a state bit (see Game/PlayerShadowWeapon.cpp). */
bool fn_8006CD78(u32 *bits, int bit);
/* Copies a player's state bits (three words). */
void fn_80095364(u32 *to, const u32 *from);
/* PlayerBase's word at 0x218 (behaviour inferred). */
UnkPlayerAim_8006F8D0 *fn_8006F8D0(Player::PlayerBase *player);
}

namespace {

class PostManageTask : public Task {
  public:
    PostManageTask(Task *parent) : Task(parent) {
        m_name = "PlayerPostManager";
    }
    virtual ~PostManageTask();
    virtual void Tick(float dt);
};

} // namespace

PostManageTask::~PostManageTask() {}

static void PostManage();

void PostManageTask::Tick(float dt) {
    PostManage();
}

/* Hands the target to the camera of a screen. */
static void SetCameraTarget(UnkCameraManager_80009548 *manager, int index,
                            PJSCamera::PJSCameraTargetInfo *info) {
    fn_8000D0F0(manager, index)->m_target->vf_0x0C((const UnkCameraTarget_8026FCFC *)info);
}

static void PostManage() {
    UnkCharaList_80074194 *list = fn_80074194();
    int i;
    for (i = 0; i != 2; i++) {
        boost::weak_ptr<CharInfo> chara(fn_800A8C54(list, i));
        if (!chara.expired()) {
            boost::shared_ptr<CharInfo> charaPtr(chara.lock());
            boost::shared_ptr<Player::PlayerBase> player(
                boost::dynamic_pointer_cast<Player::PlayerBase>(charaPtr));
            PJSCamera::PJSCameraTargetInfo info;
            if (player->m_vehicle.expired()) {
                fn_800091D0(&info.field_0x0, &player->m_position);
                fn_800091D0(&info.field_0xC, &player->field_0x18);
            } else {
                boost::shared_ptr<Vehicle::TVehicleBase> vehicle(player->m_vehicle.lock());
                fn_800091D0(&info.field_0x0, &vehicle->m_position);
                fn_800091D0(&info.field_0xC, &vehicle->field_0x18);
            }
            fn_800091D0(&info.field_0x18, &(const Vec3f &)fn_80079698(player.get()));
            fn_800091D0(&info.field_0x24, &(const Vec3f &)fn_800796DC(player.get()));
            fn_800091D0(&info.field_0x30, (Vec3f *)((u8 *)player.get() + 0xE8));
            fn_80095364(info.field_0x40.m_words, player->m_stateBits);
            if (lbl_8057D6A0.field_0xC8 == 0 && !fn_8006CD78(player->m_stateBits, 0x2A)) {
                UnkPlayerAim_8006F8D0 *aim = fn_8006F8D0(player.get());
                info.field_0x4C.x = aim->field_0x8;
                info.field_0x4C.y = aim->field_0xC;
            }
            UnkCameraManager_80009548 *manager = fn_80009548();
            SetCameraTarget(manager, player->field_0x54, &info);
        }
    }
}

extern "C" Task *fn_8029773C(Task *parent) {
    return new PostManageTask(parent);
}
