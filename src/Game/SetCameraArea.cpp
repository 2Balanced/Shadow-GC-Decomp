/* The camera areas in play (class name unknown; Game/SetCameraData.h): an area holds its set data
 * and the collider of its shape, and tests whether a character is inside it. File and other names
 * are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/SetCameraData.h"
#include "Game/CharaColli.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/PJSCameraTargetInfo.h"
#include "Game/Player.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using PJSCamera::PJSCameraTargetInfo;
using SonicteamUSA::System::CharaColli::CharaColliBase;

/* A sphere and a line for the collision queries (see Game/CameraModeGeneral.cpp). */
struct UnkSphere_80021C04 {
    /* 0x00 */ float center[3];
    /* 0x0C */ float radius;
};

struct UnkLine_80020A20 {
    u8 field_0x0[0x18];
};

/* A part of a player character fn_80078CEC returns (see Game/CatchStick.cpp). */
struct UnkPlayerPart_80078CEC {
    /* 0x00 */ u8 field_0x0[0xC];
    /* 0x0C */ Vec3f m_position;
};

extern "C" {
/* The player as a Player::Player (see Game/CameraModeGeneral.cpp) and a part of it (see
 * Game/CatchStick.cpp). */
boost::shared_ptr<Player::Player> fn_800DFAD4(const boost::shared_ptr<CharInfo> &chara);
UnkPlayerPart_80078CEC *fn_80078CEC(Player::PlayerBase *player, int index);
/* The up vector of a gravity number (behaviour inferred), and whether two vectors are about the
 * same (see Game/CameraModeGeneral.cpp). */
const Vec3f *fn_80293B60(u32 gravity);
bool fn_8000AD08(const Vec3f *a, const Vec3f *b);
/* A sphere and a line, and whether a collider is hit by them (behaviour inferred). */
void fn_80021C04(UnkSphere_80021C04 *sphere, const Vec3f *center, float radius);
void *fn_80020A20(UnkLine_80020A20 *line, const Vec3f *start, const Vec3f *end);
void *fn_80021B08(void);
bool fn_800C6174(void *manager, const UnkLine_80020A20 *line,
                 const boost::weak_ptr<CharaColliBase> &collider, int arg3);
bool fn_800C5878(void *manager, const UnkSphere_80021C04 *sphere,
                 const boost::weak_ptr<CharaColliBase> &collider, int arg3);
}

SetCameraArea::SetCameraArea(SetCameraData *data) : m_data(data) {
    m_collision = m_data->m_shape.CreateCollision();
}

SetCameraArea::~SetCameraArea() {}

bool SetCameraArea::Check(boost::shared_ptr<CharInfo> chara, const PJSCameraTargetInfo *target,
                          bool line) {
    if (m_data->m_flags.Test(2) && fn_8016904C(fn_800CB69C(), m_data->m_event) != 1) {
        return false;
    }
    if (m_data->m_flags.Test(6) &&
        !fn_8000AD08(fn_80293B60(m_data->m_args.field_0x28), &target->field_0x30)) {
        return false;
    }
    if (m_data->m_shape.m_kind == 4) {
        if (line) {
            boost::shared_ptr<Player::Player> player = fn_800DFAD4(chara);
            UnkLine_80020A20 segment;
            fn_80020A20(&segment, &fn_80078CEC(player.get(), 1)->m_position, &player->m_position);
            return fn_800C6174(fn_80021B08(), &segment, m_collision, 0);
        }
    } else {
        UnkSphere_80021C04 sphere;
        fn_80021C04(&sphere, &chara->m_position, 0.0f);
        return fn_800C5878(fn_80021B08(), &sphere, m_collision, 0);
    }
    return false;
}
