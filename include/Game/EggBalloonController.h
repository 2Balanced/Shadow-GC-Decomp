#ifndef GAME_EGGBALLOONCONTROLLER_H
#define GAME_EGGBALLOONCONTROLLER_H

#include "Game/PathObject.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class CharCommand;
class GadgetCharInfo;

extern "C" {
/* A 32-bit flag word (see BASwitch.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
}

namespace EggBalloonController {

class BalloonPath;

/* The flag word at 0x44 (struct name unknown; bit 0: the balloon waits for its timer, bit 1: it
 * is stopped). */
struct ControllerFlags {
    ControllerFlags() {
        fn_80014E04(&m_bits);
    }
    void Clear() {
        fn_80014E10(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    void Clear(int bit) {
        fn_8006127C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits;
};

/* EggBalloonController::Controller (class name from RTTI; vtable at 0x8054D060): the task that
 * runs the egg balloon (a PathObject) along its paths (Game/EggBalloonController.cpp). Its
 * constructor is at 0x80274718. Member names are inferred. */
class Controller : public Task {
  public:
    Controller(Task *parent, boost::shared_ptr<PathObject> object);
    virtual ~Controller();
    virtual void Tick(float dt);
    /* The command callback of the controller's character info. */
    void OnCommand(CharCommand &command);

    /* 0x28 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x30 */ boost::shared_ptr<PathObject> m_object;
    /* 0x38 */ BalloonPath *m_path;
    /* 0x3C */ s32 m_state;
    /* 0x40 */ float m_timer; /* the balloon waits while it runs */
    /* 0x44 */ ControllerFlags m_flags;

    static const char *s_taskName;
};
SIZE_ASSERT(Controller, 0x48);

} // namespace EggBalloonController

extern "C" {
/* Calls the object's vf_0x28 (behaviour from its body). */
void fn_802742D4(EggBalloonController::Controller *controller);
}

#endif
