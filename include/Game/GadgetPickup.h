#ifndef GAME_GADGETPICKUP_H
#define GAME_GADGETPICKUP_H

#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/Task.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" {
/* A 32-bit flag word: fn_80014E04 constructs it cleared, fn_80014E10 clears it, fn_80014E1C sets
 * bit and fn_80014E34 tests bit (behaviour from their bodies). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
}

namespace GadgetPickup {

/* A 32-bit flag word (class name unknown; its out-of-line members are the fn_80014E04 family). */
struct PickupFlags {
    PickupFlags() {
        fn_80014E04(&m_bits);
    }
    void Clear() {
        fn_80014E10(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits;
};

/* PickupBase (class name from RTTI; vtable at 0x8052B62C): the character info of a gadget a
 * character can pick up (BOMB, the containers, HEALING_UNIT). A SetPickupBaseTask creates it from
 * an owner, or a PickupBaseTask runs one spawned by game code. Names are inferred from the BOMB
 * gadget and the tasks' calls. */
class PickupBase : public GadgetCharInfo {
  public:
    PickupBase(int type) : GadgetCharInfo(type) {}
    virtual ~PickupBase() {}

    /* Moves the character info to position and rotation (0x801689FC). */
    virtual void SetPosition(const Vec3f &position, const Vec3f &rotation);
    /* Sets the object up (owner is null when game code spawns it). */
    virtual void Init(UnkPlacement_800C9CD0 *owner) {}
    virtual void Tick(float dt) = 0;
    /* Re-reads the owner's placement. */
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner) = 0;
    /* Places a spawned object. */
    virtual void SetPlacement(const Vec3f &position, const Vec3f &rotation) {}
    /* Whether the object is done, given the task's request flags (bit 0: always, bit 1: only when
     * out of range); the task then finishes. */
    virtual bool IsDone(PickupFlags *request) = 0;
};
SIZE_ASSERT(PickupBase, 0x6C);

/* PickupBaseTask (class name from RTTI; vtable at 0x8052B5FC): the task that updates a pickup
 * object, finishing it once the object is done or, with a range set, out of range. */
class PickupBaseTask : public Task {
  public:
    PickupBaseTask(Task *parent, const boost::shared_ptr<PickupBase> &pickup);
    virtual ~PickupBaseTask();
    virtual void Tick(float dt);

    /* Sets the range (stored squared; 0x80168798). */
    void SetRange(float range);
    /* Whether the object is done whatever the range (0x801687A4; name inferred). */
    bool IsPickupDone();
    /* Passes the owner's placement on to the object (0x80168804; name inferred). */
    void ResetPickup(UnkPlacement_800C9CD0 *owner);

    /* 0x28 */ boost::shared_ptr<PickupBase> m_pickup;
    /* 0x30 */ float m_rangeSq;
    /* 0x34 */ u32 field_0x34;
};
SIZE_ASSERT(PickupBaseTask, 0x38);

/* SetPickupBaseTask (class name from RTTI; vtable at 0x8052B5D0): the gadget that runs a pickup
 * object placed in a stage, through a PickupBaseTask of its own. */
class SetPickupBaseTask : public Gadget {
  public:
    SetPickupBaseTask(Task *parent, UnkPlacement_800C9CD0 *owner,
                      const boost::shared_ptr<PickupBase> &pickup);
    virtual ~SetPickupBaseTask();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    static const char *s_taskName;

    /* 0x30 */ PickupBaseTask *m_task;
    /* 0x34 */ u32 field_0x34;
};
SIZE_ASSERT(SetPickupBaseTask, 0x38);

} // namespace GadgetPickup

#endif
