/* @unnamed@PlayerPostManager_cpp@::PostManageTask (class name from RTTI): the task that, after
 * the players have moved, hands each player's position and state to the camera of the player's
 * screen as a PJSCamera::PJSCameraTargetInfo. The stage manager starts it (fn_8029773C). Member
 * and function names other than the class's are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#define MSL_LIST_ACCESSORS
#include "Game/CameraManagerUnit.h"
#include "Game/CharInfo.h"
#include "Game/GadgetUtil.h"
#include "Game/MathTypes.h"
#include "Game/PJSCameraTargetInfo.h"
#include "Game/Player.h"
#include "Game/PlayerPadInput.h"
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
/* A player's pad input (see Game/PlayerPadInput.h). */
UnkPlayerInput_800A85B0 *fn_8006F8D0(Player::PlayerBase *player);
}

/* Copies the second stick of the player's pad to the target (name inferred). Retail calls the
 * player pointer's operator-> out of line before fn_8006F8D0, which GC/2.7 does only when the
 * pointer is the argument of an inline call that is a statement of its own; an inline lookup used
 * as an initializer or inside an expression leaves operator-> inline. */
inline void SetAim(PJSCamera::PJSCameraTargetInfo *info, Player::PlayerBase *player) {
    UnkPlayerInput_800A85B0 *input = fn_8006F8D0(player);
    info->field_0x4C.x = input->m_subStick.x;
    info->field_0x4C.y = input->m_subStick.y;
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
    fn_8000D0F0(manager, index)->m_target->Assign(*info);
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
                fn_800091D0(&info.m_position, &player->m_position);
                fn_800091D0(&info.field_0xC, &player->field_0x18);
            } else {
                boost::shared_ptr<Vehicle::TVehicleBase> vehicle(player->m_vehicle.lock());
                fn_800091D0(&info.m_position, &vehicle->m_position);
                fn_800091D0(&info.field_0xC, &vehicle->field_0x18);
            }
            fn_800091D0(&info.field_0x18, &(const Vec3f &)fn_80079698(player.get()));
            fn_800091D0(&info.field_0x24, &(const Vec3f &)fn_800796DC(player.get()));
            fn_800091D0(&info.field_0x30, (Vec3f *)((u8 *)player.get() + 0xE8));
            info.field_0x40 = *reinterpret_cast<const UnkWords_8007A4D4 *>(player->m_stateBits);
            if (lbl_8057D6A0.field_0xC8 == 0 && !fn_8006CD78(player->m_stateBits, 0x2A)) {
                SetAim(&info, player.operator->());
            }
            UnkCameraManager_80009548 *manager = fn_80009548();
            SetCameraTarget(manager, player->field_0x54, &info);
        }
    }
}

extern "C" Task *fn_8029773C(Task *parent) {
    return new PostManageTask(parent);
}
