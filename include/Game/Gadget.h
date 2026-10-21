#ifndef SHADOW_GAME_GADGET_H
#define SHADOW_GAME_GADGET_H

#include "Game/MathTypes.h"
#include "Game/Task.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* The placement record a stage object is spawned from: where it stands, its rotation and its
 * parameter block. The gadget keeps a pointer to it and checks in with it every frame (see
 * fn_800C9CD0). Its real name is unknown and its layout is only spelled out by units that read it
 * directly. */
class UnkPlacement_800C9CD0;

/* Gadget, named by the game's RTTI: the Task base of every stage object.
 *
 *   0x8015AA1C  constructor: keeps `owner` and registers with it
 *   0x8015A9A8  destructor: unregisters
 *   0x8015A944  Tick: runs Behave, unless HasEnded says the record is gone
 *   0x8015A898  HasEnded: true once the record went away (the task is then flagged done)
 *
 * Names of the virtual functions after Tick are ours. */
class Gadget : public Task {
  public:
    Gadget(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~Gadget();
    virtual void Tick(float dt);

    /* The object's own frame update, called while its record exists. */
    virtual void Behave(float dt) = 0;

    /* An empty slot in Gadget itself (0x800CBC1C). Most overrides take nothing; ELEC_BAR's
     * (0x80152810) uses a float argument, the frame time. A unit with such an override defines
     * GADGET_SLOT_14_TAKES_DT before including this header. */
#ifdef GADGET_SLOT_14_TAKES_DT
    virtual void fn_800CBC1C(float dt);
#else
    virtual void fn_800CBC1C();
#endif

    /* Picks up parameters that were changed on the record (for example by the stage editor). */
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner) = 0;

    virtual bool HasEnded();

    /* "Gadget", the task name the constructor stores (the pointer is at 0x805E6598). */
    static const char *s_taskName;

    /* 0x28 */ UnkPlacement_800C9CD0 *m_owner;
    /* 0x2C */ u32 field_0x2C;
};
SIZE_ASSERT(Gadget, 0x30);

extern "C" {
/* Where new gadgets go: fn_8001C4F8 returns the root of the task tree and fn_801EB304 the task
 * layer below it with the given number. */
void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);

/* Reads from the placement record: the position, the rotation, and the parameter block (copied
 * only if `size` agrees with the record's), and the switch number it was placed with. */
void fn_800C9F14(UnkPlacement_800C9CD0 *owner, Vec3f *position);
void fn_800C9ED4(UnkPlacement_800C9CD0 *owner, Vec3f *rotation);
void fn_800C9DBC(UnkPlacement_800C9CD0 *owner, void *params, int size);
void fn_800C9E60(UnkPlacement_800C9CD0 *owner, u8 *id);
}

#endif
