#ifndef GAME_PLAYERSHADOWMOTIONEYE_H
#define GAME_PLAYERSHADOWMOTIONEYE_H

#include "Game/Task.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>

struct UnkMotionFiles_800ADE78;

namespace Player {

class PlayerBase;
/* What a motion's vf_0x14 hands the clumps to (see Game/PlayerWeaponBase.h). */
class UnkClumpList_8006CAE4;

namespace Shadow {

class MotionEyeImpl;

/* MotionEye (class name from RTTI; vtable 0x80522334; Game/PlayerShadowMotionEye.cpp): the task
 * that shows Shadow's eyes. Player::Shadow::ShadowMotion keeps it at its word 0x7E4 and shows,
 * hides and hands on its model with its own. 0x30 bytes. Method names are inferred. */
class MotionEye : public Task {
  public:
    MotionEye(Task *parent, const boost::shared_ptr<PlayerBase> &player,
              const UnkMotionFiles_800ADE78 &files);
    virtual ~MotionEye();
    virtual void Tick(float dt);

    /* The eyes' current blink frame, which the motion's slot 0x38 returns (0x800B5B90). */
    int GetBlinkFrame();
    void Hide();
    void Show();
    void AddTo(UnkClumpList_8006CAE4 *list);
    /* Empty (0x800B5C30; the eyes follow the head in Tick). */
    void Update();

    /* 0x28 */ boost::scoped_ptr<MotionEyeImpl> m_impl;
    /* 0x2C */ u32 pad_0x2C; /* the class size is a multiple of 8 */
};

} // namespace Shadow
} // namespace Player

#endif
