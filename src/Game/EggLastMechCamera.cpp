/* EggLastMech::EggLastMechCamera (class name from the RTTI of its bound callbacks) and
 * EggLastMech::CameraTask (class name from RTTI): the boss's event camera. Start pushes a user
 * camera mode (CameraModeUser) that follows one of the boss's camera paths (resource slot 0x16)
 * from a position and rotation; a CameraTask updates it each frame, and while no path plays a
 * camera control object (UnkCameraCtrl_8031A268) frames the boss and the player, switching when
 * the player is far away. File name inferred from the class; names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CameraModeUser.h"
#include "Game/CharInfo.h"
#include "Game/EggLastMech.h"
#include "Game/ExternalCameraRequest.h"
#include "Game/Gadget.h"
#include "Game/Task.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using SonicteamUSA::System::Camera::CameraMode;
using SonicteamUSA::System::Camera::CameraModeUser;

/* The game state (see Coaster.cpp): the stage number at 0xAC. */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xAC];
    /* 0x0AC */ s32 m_stage;
    /* 0x0B0 */ u8 field_0xB0[0x150 - 0xB0];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

namespace EggLastMech {

extern "C" {
extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The resource loader and a camera path of a resource slot (behaviour inferred). */
void *fn_800122D4(void);
UnkCameraData_80012510 *fn_80012510(void *loader, int slot, int index);
/* Moves a camera path's time on; whether it ended (behaviour inferred). */
int fn_80011B7C(UnkCameraData_80012510 *data, float *time, const float *step);
/* Places a camera mode on a camera path (behaviour inferred). */
void fn_8000FBFC(const boost::shared_ptr<CameraMode> &mode, UnkCameraData_80012510 *data, float time,
                 const Mtx34f *matrix);

/* The camera control (behaviour inferred). */
void fn_8031A0A0(UnkCameraCtrl_8031A268 *ctrl, const Vec2f *value);
void fn_8031A0C4(UnkCameraCtrl_8031A268 *ctrl, const Vec3f *value);
void fn_8031A0E8(UnkCameraCtrl_8031A268 *ctrl, const Vec2f *value);
void fn_8031A10C(UnkCameraCtrl_8031A268 *ctrl, const Vec3f *value);
void fn_8031A130(UnkCameraCtrl_8031A268 *ctrl, float dt);
void fn_8031A164(UnkCameraCtrl_8031A268 *ctrl);
void fn_8031A188(UnkCameraCtrl_8031A268 *ctrl, const UnkCameraBlend_8000B2C0 *blend);
void fn_8031A1AC(UnkCameraCtrl_8031A268 *ctrl, int flags);

/* Whether the player stands on the ground (behaviour inferred). */
bool fn_800B7AD0(const boost::shared_ptr<CharInfo> &player);

/* Time and angle helpers (behaviour inferred). */
float fn_8005AE20(float value);
Vec2f fn_80272E70(float x, float y);

/* Task helpers (see Gadget.h). */
Task *fn_8001C4F8(void);
Task *fn_801EB304(Task *root, int layer);
}

/* CameraTask (class name from RTTI; vtable at 0x8055BE5C): updates its camera each frame. */
class CameraTask : public Task {
  public:
    CameraTask(EggLastMechCamera *camera);
    virtual ~CameraTask();
    virtual void Tick(float dt);

    /* Lets the camera go (name inferred). */
    void LetGo();

    /* Remembers where the owner keeps this task (name inferred). */
    void SetOwner(CameraTask **owner) {
        m_owner = owner;
        *m_owner = this;
    }

    /* 0x28 */ CameraTask **m_owner;
    /* 0x2C */ EggLastMechCamera *m_camera;
};
SIZE_ASSERT(CameraTask, 0x30);

/* A vector initialized at startup through the out-of-line vector copy (struct name unknown). */
struct UnkStaticVector3 {
    struct Raw {
        float x;
        float y;
        float z;
    };

    UnkStaticVector3(float x, float y, float z) {
        Raw value;
        value.x = x;
        value.y = y;
        value.z = z;
        fn_800091EC(reinterpret_cast<Vec3f *>(m_value), reinterpret_cast<Vec3f *>(&value));
    }

    float m_value[3];
};

/* The control's framing (meanings inferred). */
static UnkStaticVector3 s_offset(0.0f, 5.0f, 0.0f);
static Vec2f s_range = fn_80272E70(fn_8005AE20(1.0f), fn_8005AE20(2.0f));
static UnkStaticVector3 s_awayOffset(0.0f, 40.0f, 0.0f);
static Vec2f s_awayRange = fn_80272E70(fn_8005AE20(6.0f), fn_8005AE20(1.0f));

CameraTask::CameraTask(EggLastMechCamera *camera)
    : Task(fn_801EB304(fn_8001C4F8(), 0xF)), m_owner(0), m_camera(camera) {
    m_name = "EggLastMechCameraTask";
}

CameraTask::~CameraTask() {
    if (m_owner != 0 && *m_owner == this) {
        *m_owner = 0;
    }
}

void CameraTask::LetGo() {
    m_owner = 0;
    m_camera = 0;
    m_flags |= 1;
}

void CameraTask::Tick(float dt) {
    if (m_camera != 0) {
        m_camera->Tick(dt);
    }
}

EggLastMechCamera::EggLastMechCamera(EggLastMech *mech) : m_mech(mech), m_ctrl(0) {
    m_data = 0;
    m_finished = true;
    m_playerAway = 0;
    m_paused = 0;
    m_ctrl = new UnkCameraCtrl_8031A268(mech->GetPlayer(), mech->GetWeakPtr());
    if (m_ctrl != 0) {
        fn_8031A10C(m_ctrl, reinterpret_cast<const Vec3f *>(s_offset.m_value));
        fn_8031A0E8(m_ctrl, &s_range);
        fn_8031A0C4(m_ctrl, reinterpret_cast<const Vec3f *>(s_awayOffset.m_value));
        fn_8031A0A0(m_ctrl, &s_awayRange);
    }
    fn_8031A188(m_ctrl, 0);
    if (lbl_8057D6A0.m_stage == 0x28) {
        m_awayDistanceSquared = 62500.0f;
    } else {
        m_awayDistanceSquared = 90000.0f;
    }
    CameraTask *task = new CameraTask(this);
    if (task != 0) {
        task->SetOwner(&m_task);
    }
}

EggLastMechCamera::~EggLastMechCamera() {
    if (m_task != 0) {
        m_task->LetGo();
        m_task = 0;
    }
    End();
    if (m_ctrl != 0) {
        fn_8031A164(m_ctrl);
        fn_8031A1AC(m_ctrl, 1);
        m_ctrl = 0;
    }
}

void EggLastMechCamera::Start(int mode, const Vec3f *position, const Vec3f *rotation) {
    m_data = fn_80012510(fn_800122D4(), 0x16, mode);
    if (m_data == 0) {
        return;
    }
    m_mode = fn_80009704(0, 0);
    if (m_mode.get() != 0) {
        fn_800A6A24(&m_mode.get()->m_onSwitch,
                    &(const CameraModeUser::SwitchCallback &)CameraModeUser::SwitchCallback(
                        boost::bind(&EggLastMechCamera::OnSwitch, this, _1)));
        fn_800A673C(&m_mode.get()->m_onUpdate,
                    &(const CameraModeUser::UpdateCallback &)CameraModeUser::UpdateCallback(
                        boost::bind(&EggLastMechCamera::OnUpdate, this, _1)));
    }
    m_time = m_data->m_startTime;
    m_handle = fn_801CED24(boost::shared_ptr<CameraMode>(m_mode), 0, 0, 0, 0, 0, 1);
    Mtx34f matrix;
    fn_8040B034(&matrix, position);
    fn_8040C148(&m_matrix, &matrix);
    fn_8040B848(&m_matrix, rotation->y);
    fn_8040B884(&m_matrix, rotation->x);
    fn_8040B80C(&m_matrix, rotation->z);
    m_finished = false;
}

void EggLastMechCamera::Tick(float dt) {
    if (m_paused) {
        return;
    }
    if (m_data != 0 && !m_finished) {
        float step = 60.0f * dt;
        m_finished = fn_80011B7C(m_data, &m_time, &step) != 0;
        return;
    }
    if (IsPlayerAway()) {
        if (!m_playerAway) {
            m_playerAway = 1;
            fn_8031A164(m_ctrl);
        }
    } else if (m_playerAway) {
        m_playerAway = 0;
        static UnkCameraBlend_8000B2C0 s_blend(1.0f);
        fn_8031A188(m_ctrl, &s_blend);
    }
    fn_8031A130(m_ctrl, dt);
}

bool EggLastMechCamera::IsPlayerAway() {
    if (!m_mech->GetPlayer().expired()) {
        boost::shared_ptr<CharInfo> player = m_mech->GetPlayer().lock();
        Vec3f offset;
        fn_800091EC(&offset, &player.get()->m_position);
        offset.y = 0.0f;
        if (!m_playerAway) {
            if (fn_8040CF2C(&offset) < m_awayDistanceSquared) {
                return true;
            }
            return false;
        }
        if (fn_8040CF2C(&offset) > m_awayDistanceSquared && fn_800B7AD0(player)) {
            return false;
        }
        return true;
    }
    return true;
}

void EggLastMechCamera::End() {
    if (!m_handle.expired()) {
        fn_801CEAD4(&m_handle);
        m_playerAway = 1;
        fn_8031A164(m_ctrl);
        if (!IsPlayerAway()) {
            m_playerAway = 0;
            fn_8031A188(m_ctrl, 0);
        }
    }
}

bool EggLastMechCamera::HasEnded() {
    return m_finished;
}

float EggLastMechCamera::GetTimeLeft() {
    if (m_data != 0 && !m_finished) {
        float left = m_data->m_endTime - m_time;
        if (left < 0.0f) {
            left = 0.0f;
        }
        return left;
    }
    return 0.0f;
}

void EggLastMechCamera::OnSwitch(CameraModeUser::SwitchEventArgs &args) {
    Apply();
}

void EggLastMechCamera::OnUpdate(CameraModeUser::UpdateEventArgs &args) {
    Apply();
}

/* Places the camera mode on the current path (name inferred). */
void EggLastMechCamera::Apply() {
    if (m_data != 0) {
        fn_8000FBFC(boost::shared_ptr<CameraMode>(m_mode), m_data, m_time, &m_matrix);
    }
}

} // namespace EggLastMech
