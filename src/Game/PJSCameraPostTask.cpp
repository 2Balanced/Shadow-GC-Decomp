/* PJSCamera::PJSCameraPostTask and PJSCamera::PJSCameraPreTask (class names and base from RTTI):
 * the tasks that run after and before the camera manager's update, and the object that owns them
 * (UnkStageTasks_800D51EC, class name unknown; Game/StageCameraList.h). File name from the first
 * class; other names are inferred. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#define MSL_LIST_ACCESSORS
#include <list>

#include "Game/StageCameraList.h"
#include "Game/CameraManagerUnit.h"
#include "Game/PJSCameraFactory.h"
#include "Game/StageCameraManager.h"
#include "Game/Task.h"
#include <dolphin/types.h>
#include "size_assert.h"

using PJSCamera::PJSCameraFactory;
using SonicteamUSA::System::Camera::CameraFactory;

/* The camera manager (class name unknown; see Game/CameraManagerUnit.h). */
class UnkCameraManager_80009548;

extern "C" {
/* The camera manager, its update, its factory set and a call whose meaning is unknown (see
 * Game/StageCameraManager.cpp; behaviour inferred). */
UnkCameraManager_80009548 *fn_80009548(void);
void fn_8000D170(UnkCameraManager_80009548 *manager, float dt);
void fn_8000D1E0(UnkCameraManager_80009548 *manager, CameraFactory *factory);
void fn_8000D534(UnkCameraManager_80009548 *manager, int arg1);
/* The task tree and the parent task of a layer (see Game/AdvStaffRoll.cpp). */
void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);
}

namespace PJSCamera {

/* PJSCameraPreTask (class name and base from RTTI; vtable 0x8053C518): runs before the camera
 * manager's update; the stage cameras hang below it. */
class PJSCameraPreTask : public Task {
  public:
    PJSCameraPreTask(Task *parent);
    virtual ~PJSCameraPreTask();
    virtual void Tick(float dt);

    static const char *s_taskName;
};
SIZE_ASSERT(PJSCameraPreTask, 0x28);

/* PJSCameraPostTask (class name and base from RTTI; vtable 0x8053C4FC): updates the camera
 * manager; the stage cameras' post processes hang below it. */
class PJSCameraPostTask : public Task {
  public:
    PJSCameraPostTask(Task *parent);
    virtual ~PJSCameraPostTask();
    virtual void Tick(float dt);

    static const char *s_taskName;
};
SIZE_ASSERT(PJSCameraPostTask, 0x28);

} // namespace PJSCamera

using PJSCamera::PJSCameraPostTask;
using PJSCamera::PJSCameraPreTask;

const char *PJSCameraPreTask::s_taskName = "PJSCameraPreTask";
const char *PJSCameraPostTask::s_taskName = "PJSCameraPostTask";

extern "C" void fn_801D0AD8(void) {
    fn_800D51EC();
    fn_80099954();
}

UnkStageTasks_800D51EC::UnkStageTasks_800D51EC() : m_preTask(0), m_postTask(0), m_numUsers(0) {
    fn_8000D1E0(fn_80009548(), new PJSCameraFactory());
}

UnkStageTasks_800D51EC::~UnkStageTasks_800D51EC() {
    if (m_preTask != 0) {
        m_preTask->m_flags |= 1;
        m_preTask = 0;
    }
    if (m_postTask != 0) {
        m_postTask->m_flags |= 1;
        m_postTask = 0;
    }
}

void UnkStageTasks_800D51EC::Start() {
    if (m_numUsers == 0 && m_preTask == 0 && m_postTask == 0) {
        m_preTask = new PJSCameraPreTask(fn_801EB304(fn_8001C4F8(), 0x12));
        m_postTask = new PJSCameraPostTask(fn_801EB304(fn_8001C4F8(), 0x12));
    }
    m_numUsers++;
}

PJSCameraPreTask::PJSCameraPreTask(Task *parent) : Task(parent) {
    m_name = s_taskName;
    m_flags |= 0x100;
}

PJSCameraPreTask::~PJSCameraPreTask() {
    UnkStageTasks_800D51EC *tasks = fn_800D51EC();
    if (tasks->m_preTask != 0) {
        tasks->m_preTask = 0;
    }
}

void PJSCameraPreTask::Tick(float dt) {}

PJSCameraPostTask::PJSCameraPostTask(Task *parent) : Task(parent) {
    m_name = s_taskName;
    m_flags |= 0x100;
}

PJSCameraPostTask::~PJSCameraPostTask() {
    UnkStageTasks_800D51EC *tasks = fn_800D51EC();
    if (tasks->m_postTask != 0) {
        tasks->m_postTask = 0;
    }
    tasks->m_numUsers = 0;
    fn_8000D534(fn_80009548(), 0);
}

void PJSCameraPostTask::Tick(float dt) {
    fn_8000D170(fn_80009548(), dt);
}
