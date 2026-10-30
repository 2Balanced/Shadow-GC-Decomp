/* SonicteamUSA::System::CharaColli::CharaColliManageTask (class name from RTTI): the task that
 * updates the collider manager (Game/CharaColliManager.cpp) every frame. Its constructor sets up the
 * game's collision kinds (fn_801DDDD4). Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#include "Game/Task.h"
#include "dolphin/types.h"
#include "size_assert.h"

extern "C" {
/* The collider manager (an inline singleton of an earlier unit) and its update. */
class UnkColliderManager_80021B08;
UnkColliderManager_80021B08 *fn_80021B08(void);
void fn_800C6C80(UnkColliderManager_80021B08 *manager);
/* The collision kinds the game uses (Game/DeltaMorphAnimManagerTask.cpp). */
void fn_801DDDD4(void);
}

namespace SonicteamUSA {
namespace System {
namespace CharaColli {

class CharaColliManageTask : public Task {
  public:
    CharaColliManageTask(Task *parent);
    virtual ~CharaColliManageTask();
    virtual void Tick(float dt);

    static const char *s_taskName;
};
SIZE_ASSERT(CharaColliManageTask, 0x28);

const char *CharaColliManageTask::s_taskName = "CharaColliManageTask";

CharaColliManageTask::CharaColliManageTask(Task *parent) : Task(parent) {
    m_name = s_taskName;
    fn_801DDDD4();
}

CharaColliManageTask::~CharaColliManageTask() {}

void CharaColliManageTask::Tick(float dt) {
    fn_800C6C80(fn_80021B08());
}

} // namespace CharaColli
} // namespace System
} // namespace SonicteamUSA
