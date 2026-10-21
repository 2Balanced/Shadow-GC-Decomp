/* Boss::Diablon::Parts::CameraControl (class name from RTTI): the Diablon boss's camera, a part of
 * the boss. Once the player exists it creates a Lamborghini boss camera (EggLastMech's
 * UnkCameraCtrl_8031A268) between the player and the boss, and a task (UpdateTask, class name from
 * RTTI) that steps it every frame. Each update it lowers the camera's pitch limits the higher the
 * player stands. Action 16 moves the camera's boss point up. File name from the RTTI name of
 * UpdateTask's unnamed namespace. Names marked "(name inferred)" are not original. Definitions are
 * in reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/Diablon.h"
#include "Game/EggLastMech.h"
#include "Game/Gadget.h"
#include "Game/Task.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using EggLastMech::UnkCameraCtrl_8031A268;

struct UnkCameraBlend_8000B2C0;

extern const float lbl_805FAD38; /* pi */

extern "C" {
/* The camera control (see EggLastMechCamera.cpp; behaviour inferred). */
void fn_8031A0A0(UnkCameraCtrl_8031A268 *ctrl, const Vec2f *value);
void fn_8031A0C4(UnkCameraCtrl_8031A268 *ctrl, const Vec3f *value);
void fn_8031A0E8(UnkCameraCtrl_8031A268 *ctrl, const Vec2f *value);
void fn_8031A10C(UnkCameraCtrl_8031A268 *ctrl, const Vec3f *value);
void fn_8031A130(UnkCameraCtrl_8031A268 *ctrl, float dt);
void fn_8031A164(UnkCameraCtrl_8031A268 *ctrl);
void fn_8031A188(UnkCameraCtrl_8031A268 *ctrl, const UnkCameraBlend_8000B2C0 *blend);
UnkCameraCtrl_8031A268 *fn_8031A1AC(UnkCameraCtrl_8031A268 *ctrl, int flags);
/* value limited to [min, max] (behaviour from its body). */
const float &fn_80070460(const float &value, const float &min, const float &max);
}

/* The range the player's height is limited to (names inferred). Retail keeps the zero in .sdata. */
#pragma explicit_zero_data on
static float s_heightMin = 0.0f;
#pragma explicit_zero_data reset
static float s_heightMax = 1.0f;

namespace {

/* UpdateTask (class name from RTTI): steps the camera control each frame. */
class UpdateTask : public Task {
  public:
    UpdateTask(UpdateTask **owner, UnkCameraCtrl_8031A268 *ctrl);
    virtual ~UpdateTask();
    virtual void Tick(float dt);

    /* Lets the camera control go (name inferred). */
    void LetGo() {
        m_ctrl = 0;
        m_owner = 0;
    }

    /* 0x28 */ UpdateTask **m_owner;
    /* 0x2C */ UnkCameraCtrl_8031A268 *m_ctrl;
};
SIZE_ASSERT(UpdateTask, 0x30);

UpdateTask::UpdateTask(UpdateTask **owner, UnkCameraCtrl_8031A268 *ctrl)
    : Task(fn_801EB304(fn_8001C4F8(), 0xF)), m_owner(owner), m_ctrl(ctrl) {
    m_name = "LamborghiniUpdate";
    if (m_owner != 0) {
        *m_owner = this;
    }
}

UpdateTask::~UpdateTask() {
    if (m_owner != 0) {
        *m_owner = 0;
    }
}

void UpdateTask::Tick(float dt) {
    if (m_ctrl != 0) {
        fn_8031A130(m_ctrl, dt);
    }
}

} // namespace

namespace Boss {
namespace Diablon {
namespace Parts {

class CameraControl : public PartsBase {
  public:
    CameraControl(PartsBase *parent, const boost::shared_ptr<Diablon> &owner)
        : PartsBase(parent, "CameraControl", owner) {
        m_ctrl = 0;
        m_state = 0;
    }
    virtual ~CameraControl();
    virtual void Tick(float dt);
    virtual void OnAction(const Action &action);

    static PartsBase *Instantiate(PartsBase *parent, const boost::shared_ptr<Diablon> &owner);

    /* 0x24 */ s32 m_state;
    /* 0x28 */ UnkCameraCtrl_8031A268 *m_ctrl;
    /* 0x2C */ UpdateTask *m_task;
};
SIZE_ASSERT(CameraControl, 0x30);

CameraControl::~CameraControl() {
    if (m_ctrl != 0) {
        fn_8031A164(m_ctrl);
        fn_8031A1AC(m_ctrl, 1);
    }
    if (m_task != 0) {
        m_task->LetGo();
        m_task->m_flags |= 1;
    }
}

void CameraControl::Tick(float dt) {
    switch (m_state) {
    case 0:
        if (!m_owner->m_target.expired()) {
            m_ctrl = new UnkCameraCtrl_8031A268(m_owner->m_target, boost::weak_ptr<CharInfo>(m_owner));
            new UpdateTask(&m_task, m_ctrl);
            fn_8031A10C(m_ctrl, &Vec3f(0.0f, 5.0f, 0.0f));
            fn_8031A0E8(m_ctrl, &Vec2f(10.0f, 20.0f));
            fn_8031A0C4(m_ctrl, &Vec3f(0.0f, 40.0f, 0.0f));
            fn_8031A0A0(m_ctrl, &Vec2f(60.0f, 10.0f));
            fn_8031A188(m_ctrl, 0);
            m_state++;
        }
        break;
    }
    if (m_ctrl != 0) {
        boost::shared_ptr<CharInfo> player = m_owner->m_target.lock();
        if (player.get() != 0) {
            float height = (player->m_position.y - 5.0f) / 100.0f;
            height = fn_80070460(height, s_heightMin, s_heightMax);
            float degToRad = lbl_805FAD38 / 180.0f;
            float low = -5.0f * degToRad;
            float range = -40.0f * degToRad;
            m_ctrl->SetPitchLimits(height * range + low, 15.0f * degToRad);
        }
    }
}

void CameraControl::OnAction(const Action &action) {
    switch (action.m_type) {
    case 15:
        break;
    case 16:
        if (m_ctrl != 0) {
            fn_8031A0C4(m_ctrl, &Vec3f(0.0f, 10.0f, 0.0f));
            fn_8031A0A0(m_ctrl, &Vec2f(80.0f, 10.0f));
        }
        break;
    case 17:
        break;
    }
}

PartsBase *CameraControl::Instantiate(PartsBase *parent, const boost::shared_ptr<Diablon> &owner) {
    return new CameraControl(parent, owner);
}

} // namespace Parts
} // namespace Diablon
} // namespace Boss
