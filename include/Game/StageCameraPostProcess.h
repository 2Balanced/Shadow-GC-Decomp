#ifndef GAME_STAGECAMERAPOSTPROCESS_H
#define GAME_STAGECAMERAPOSTPROCESS_H

/* Units that include this header define the <vector> options of Game/StageCameraPostProcess.cpp
 * first when they use the vectors (see <vector>). */
#include <vector>

#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class CharInfo;

namespace PJSCamera {

class StageCameraManager;

/* StageCameraPostProcess (class name and base from RTTI; vtable 0x8053C318): a task the stage camera
 * makes for its player (Game/StageCameraPostProcess.cpp). While enabled it fades the player when
 * the camera comes close and makes the characters between the camera and the player transparent,
 * making them opaque again once they are out of the way. The stage camera enables it when it leaves
 * its first state or an external request that asks for it, and resets it when it changes to its
 * default mode. Member names are inferred. */
class StageCameraPostProcess : public Task {
  public:
    StageCameraPostProcess(Task *parent, StageCameraManager *manager);
    virtual ~StageCameraPostProcess();
    virtual void Tick(float dt);
    /* Makes everything opaque again and disables the task. */
    void Reset();
    /* Makes a transparent character opaque again unless it is still in the way; true when it is to
     * be dropped from the list. */
    bool Restore(boost::weak_ptr<CharInfo> hidden);

    static const char *s_taskName;

    /* 0x28 */ StageCameraManager *m_manager;
    /* 0x2C */ bool m_enabled;
    /* 0x2D */ bool m_playerFaded;
    /* 0x30 */ std::vector<boost::weak_ptr<CharInfo> > m_hidden;     /* made transparent */
    /* 0x3C */ std::vector<boost::shared_ptr<CharInfo> > m_blocking; /* in the way this frame */
};
SIZE_ASSERT(StageCameraPostProcess, 0x48);

} // namespace PJSCamera

#endif
