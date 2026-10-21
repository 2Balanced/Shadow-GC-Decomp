#ifndef GAME_DIABLON_H
#define GAME_DIABLON_H

#include "Game/BossDesire.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The Diablon boss (Black Doom's mech; class names from RTTI). Its main code (0x802D3600-0x802DD000)
 * is not split yet; the classes are declared here as far as the split units use them. Member
 * names are inferred unless noted. */

namespace Boss {
namespace Diablon {

class Diablon;

/* Executer (class name from RTTI; a Task, vtable 0x80558CD0, constructor 0x802DA1B0): runs the
 * boss's behaviors and passes actions on to its parts. Its methods used here are declared as the
 * fn_ functions below. */
class Executer;

namespace Behavior {

/* InitializeArgs (class name from RTTI; vtable 0x805585AC): the behavior to switch to; id is the
 * index in the behavior table at 0x80558CA8 (1 Idle, 2 Move, 3 Beam, 4 Bomb, 5 Wipe, 6 Event). */
class InitializeArgs {
  public:
    InitializeArgs(int id) : m_id(id) {}
    virtual ~InitializeArgs() {}

    /* 0x4 */ s32 m_id;
};
SIZE_ASSERT(InitializeArgs, 0x8);

/* BehaviorBase (class name from RTTI; vtable 0x805584B0, constructor 0x802D36E4, destructor
 * 0x802D3638): one behavior of the boss. The executer starts, runs and ends it. */
class BehaviorBase {
  public:
    /* name is the behavior's name; the meaning of arg is unknown. */
    BehaviorBase(const boost::shared_ptr<Diablon> &owner, const char *name, int arg);
    virtual ~BehaviorBase();
    virtual void Begin() = 0;
    virtual void End() = 0;
    virtual void Tick(float dt) = 0;

    /* 0x04 */ boost::shared_ptr<Diablon> m_owner;
    /* 0x0C */ const char *m_name;
    /* 0x10 */ s32 field_0x10;
};
SIZE_ASSERT(BehaviorBase, 0x14);

/* An entry of the behavior table at 0x80558CA8 (struct name unknown): creates the behavior, and is
 * told when the executer is set up and torn down. */
struct Info {
    boost::shared_ptr<BehaviorBase> (*create)(const boost::shared_ptr<Diablon> &owner);
    void (*setup)(void *executer);
    void (*teardown)(void *executer);
};

} // namespace Behavior

namespace Parts {

/* Action (class name from RTTI; vtable 0x805585F8): a request to the boss's parts. Its constructor
 * and destructor are inline; derived actions are therefore not inlined and emit weak copies. */
class Action {
  public:
    Action(int type) : m_type(type) {}
    virtual ~Action() {}

    /* 0x4 */ s32 m_type;
};

/* ActionMove (class name from RTTI; vtable 0x805586A0, constructor 0x802D571C, destructor
 * 0x802D56C0): turns the parts for a move (direction 0 forward, 1 backward; meaning inferred). */
class ActionMove : public Action {
  public:
    ActionMove(int direction) : Action(1), m_direction(direction) {}
    virtual ~ActionMove() {}

    /* 0x8 */ s32 m_direction;
};
SIZE_ASSERT(ActionMove, 0xC);

/* PartsBase (class name from RTTI; vtable 0x805589C0, constructor 0x802D6180, destructor
 * 0x802D60B4): a part of the boss. The parts form a tree: the constructor appends the part to its
 * parent's children, the boss updates the tree and passes actions down it (0x802D5FB0 and
 * 0x802D5F24), and a part that sets m_finished is deleted by its parent. Member names are
 * inferred. */
class PartsBase {
  public:
    PartsBase(PartsBase *parent, const char *name, const boost::shared_ptr<Diablon> &owner);
    virtual ~PartsBase();
    virtual void Tick(float dt);               /* 0x802D5F1C */
    virtual void OnAction(const Action &action); /* 0x802D5F20 */

    /* 0x04 */ boost::shared_ptr<Diablon> m_owner;
    /* 0x0C */ const char *m_name;
    /* 0x10 */ PartsBase *m_parent;
    /* 0x14 */ PartsBase *m_child; /* the first child */
    /* 0x18 */ PartsBase *m_next;  /* the next sibling */
    /* 0x1C */ u32 field_0x1C;
    /* 0x20 */ bool m_finished;
};
SIZE_ASSERT(PartsBase, 0x24);

} // namespace Parts

/* The boss's tuning values (struct name unknown; only the members used so far). */
struct DiablonParams {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ float m_moveDesireTime; /* the move desire grows by dt over it */
    /* 0x24 */ float m_moveAccel;      /* the move behavior's acceleration */
    /* 0x28 */ float m_moveFriction;   /* passed to the integration with the time step */
    /* 0x2C */ float m_moveStopFront;
    /* 0x30 */ float m_moveBackLimit; /* beyond it, the boss moves forward */
    /* 0x34 */ float m_moveStopBack;
    /* 0x38 */ float m_moveFacingAngle; /* degrees */
    /* 0x3C */ float m_moveMinDistance;
    /* 0x40 */ float m_moveMaxDistance;
    /* 0x44 */ u8 field_0x44[0x4C - 0x44];
    /* 0x4C */ float m_beamDelay;     /* before the beam first aims at the player (inferred) */
    /* 0x50 */ float m_beamAngleLimit; /* degrees; the beam's highest pitch (inferred) */
    /* 0x54 */ float m_beamSweepAngle; /* degrees the beam turns while it fires (inferred) */
    /* 0x58 */ float m_beamTime;       /* how long the beam fires */
    /* 0x5C */ u8 field_0x5C[0x70 - 0x5C];
    /* 0x70 */ float m_bombMaxRadius; /* the bomb blast's final radius (inferred) */
    /* 0x74 */ float m_bombGrowTime;  /* how long the blast takes to reach it (inferred) */
};

/* Diablon (class name from RTTI; vtable 0x80558C54): the boss's character info. */
class Diablon : public CharInfo {
  public:
    /* 0x64 */ u32 field_0x64;
    /* 0x68 */ Executer *m_executer;
    /* 0x6C */ u32 field_0x6C;
    /* 0x70 */ boost::weak_ptr<CharInfo> m_target; /* the player */
    /* 0x78 */ u8 field_0x78[0x88 - 0x78];
    /* 0x88 */ u32 m_flags; /* a bit set; see fn_802D4ED4 */
    /* 0x8C */ DiablonParams *m_params;
};

namespace Desire {

/* DiablonDesireBase (class name from RTTI; vtable 0x805586FC, constructor 0x802D5EC8, destructor
 * 0x802D5E60): a desire of the Diablon boss. */
class DiablonDesireBase : public Boss::Desire::DesireBase {
  public:
    DiablonDesireBase(const boost::shared_ptr<Diablon> &owner, const char *name, int arg);
    virtual ~DiablonDesireBase();
    virtual void Begin(); /* 0x802D5E5C */

    /* 0x14 */ boost::shared_ptr<Diablon> m_owner;
};
SIZE_ASSERT(DiablonDesireBase, 0x1C);

} // namespace Desire

extern "C" {
/* Sets, clears and tests bit index of flags (behaviour from their bodies). */
void fn_802D4ED4(u32 *flags, int index);
void fn_802D4EF4(u32 *flags, int index);
bool fn_802D58FC(u32 *flags, int index);
/* The distance to the target, and the cosine of the angle between the boss's facing and the
 * target (behaviour inferred). */
float fn_802D94B8(Diablon *diablon);
float fn_802D9570(Diablon *diablon);
/* Has the parts perform action, and switches to the behavior args names (behaviour inferred). */
void fn_802D9E4C(Executer *executer, const Parts::Action &action);
void fn_802D9ED8(Executer *executer, const Behavior::InitializeArgs &args);
}

} // namespace Diablon
} // namespace Boss

#endif
