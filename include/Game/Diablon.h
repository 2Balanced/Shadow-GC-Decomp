#ifndef GAME_DIABLON_H
#define GAME_DIABLON_H

#include "Game/BossDesire.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/Task.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The Diablon boss (Black Doom's mech; class names from RTTI). Its main code (0x802D3600-0x802DD000)
 * is not split yet; the classes are declared here as far as the split units use them. Member
 * names are inferred unless noted. */

/* A set of N bits (class name unknown). Its members are out-of-line template functions, so each
 * unit that uses them carries weak copies; retail keeps the first ones linked: Set at 0x802D4ED4,
 * Reset at 0x802D4EF4 (Game/DiablonBehaviorEvent.cpp) and Test at 0x802D58FC. */
template <int N> class UnkBitFlags_802D4ED4 {
  public:
    UnkBitFlags_802D4ED4(); /* 0x802D9DCC */
    void Set(u32 index);
    void Reset(u32 index);
    bool Test(u32 index) const;

    u32 m_words[(N + 31) / 32];
};

template <int N> UnkBitFlags_802D4ED4<N>::UnkBitFlags_802D4ED4() {
    for (int i = 0; i < (N + 31) / 32; i++) {
        m_words[i] = 0;
    }
}

template <int N> void UnkBitFlags_802D4ED4<N>::Set(u32 index) {
    m_words[index >> 5] |= 1 << (index & 31);
}

template <int N> void UnkBitFlags_802D4ED4<N>::Reset(u32 index) {
    m_words[index >> 5] &= ~(1 << (index & 31));
}

template <int N> bool UnkBitFlags_802D4ED4<N>::Test(u32 index) const {
    return (m_words[index >> 5] & (1 << (index & 31))) != 0;
}

class CharCommandUnit;

/* A clump's colour fade (struct name unknown; as in Game/BlackDoomCtrlUnit.cpp and
 * Game/BossRender.cpp); its constructor is out of line (0x80027D5C). */
struct UnkClumpFade_80027D5C;
extern "C" UnkClumpFade_80027D5C *fn_80027D5C(UnkClumpFade_80027D5C *fade);
struct UnkClumpFade_80027D5C {
    UnkClumpFade_80027D5C() {
        fn_80027D5C(this);
    }

    /* 0x00 */ float m_color[4];
    /* 0x10 */ u8 field_0x10[0x24 - 0x10];
    /* 0x24 */ u8 m_mode;
    /* 0x25 */ u8 field_0x25[0x2C - 0x25];
};

namespace Boss {
namespace Diablon {

class Diablon;

/* Controller (class name from RTTI; a Task, vtable 0x805586DC; Game/DiablonController.cpp): runs
 * the boss's desires. */
class Controller;

/* Executer (class name from RTTI; a Task, vtable 0x80558CD0; Game/DiablonExecuter.cpp): runs the
 * boss's behaviors and its parts. The methods other units call keep their fn_ names (C functions
 * taking the executer; see the end of this header). */
class Executer : public Task {
  public:
    /* Its state (class name inferred; defined in Game/DiablonExecuter.cpp). */
    class Impl;

    Executer(Task *parent, const boost::shared_ptr<Diablon> &owner);
    virtual ~Executer();
    virtual void Tick(float dt);

    /* Makes the boss's body, health gauge and camera control (name inferred). */
    void Start();

    /* 0x28 */ boost::scoped_ptr<Impl> m_impl;
    /* 0x2C */ u32 field_0x2C; /* never touched (size from the allocation) */
};

/* Effect (class name from RTTI; a Task, vtable 0x80558DAC, constructor 0x802DCA60): the boss's
 * effects; only its size is known so far. */
class Effect : public Task {
  public:
    Effect(Executer *executer, const boost::shared_ptr<Diablon> &owner);

    /* 0x28 */ u8 field_0x28[0x38 - 0x28];
};

namespace Behavior {

/* InitializeArgs (class name from RTTI; vtable 0x805585AC): the behavior to switch to; id is the
 * index in the behavior table at 0x80558CA8 (1 Idle, 2 Move, 3 Bomb, 4 Beam, 5 Wipe, 6 Event). */
class InitializeArgs {
  public:
    InitializeArgs(int id) : m_id(id) {}
    virtual ~InitializeArgs() {}

    /* 0x4 */ s32 m_id;
};
SIZE_ASSERT(InitializeArgs, 0x8);

/* InitializeArgsEvent (class name from RTTI; vtable 0x80559688): switches to the event behavior
 * (id 6) with the event to play (0-2, in the order Desire::Event wants them). Its constructor and
 * destructor are inline; as they call InitializeArgs's, retail keeps weak copies of them. */
class InitializeArgsEvent : public InitializeArgs {
  public:
    InitializeArgsEvent(int event) : InitializeArgs(6), m_event(event) {}
    virtual ~InitializeArgsEvent() {}

    /* 0x8 */ s32 m_event;
};
SIZE_ASSERT(InitializeArgsEvent, 0xC);

/* BehaviorBase (class name from RTTI; vtable 0x805584B0, constructor 0x802D36E4, destructor
 * 0x802D3638): one behavior of the boss. The executer starts, runs and ends it. */
class BehaviorBase {
  public:
    /* name is the behavior's name; the meaning of arg is unknown. */
    BehaviorBase(const boost::shared_ptr<Diablon> &owner, const char *name, int arg);
    virtual ~BehaviorBase();
    /* The executer starts a behavior with the id of the behavior before it and the arguments it
     * was switched with (see fn_802DA29C), and ends it with the id of the next one. */
    virtual void Begin(int previous, const InitializeArgs &args) = 0;
    virtual void End(int next) = 0;
    virtual void Tick(float dt) = 0;

    /* The behavior's id (name inferred). */
    int GetId() const {
        return field_0x10;
    }

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

/* ActionDamage (class name from RTTI; vtable 0x80558A9C): tells the parts the boss was hit for
 * m_damage (type 11; meaning inferred). Its constructor and destructor are inline; retail keeps
 * weak copies of them (0x802D6414, 0x802D63B8). */
class ActionDamage : public Action {
  public:
    ActionDamage(int damage) : Action(11), m_damage(damage) {}
    virtual ~ActionDamage() {}

    /* 0x8 */ s32 m_damage;
};
SIZE_ASSERT(ActionDamage, 0xC);

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

    /* Passes action to every part below this one (0x802D5F24), and ticks them, deleting those
     * that finished (0x802D5FB0) (names inferred). */
    void SendAction(const Action &action);
    void TickChildren(float dt);
    /* Links child in as the last child, and unlinks it (names inferred; Game/DiablonPartsBase.cpp). */
    inline void AddChild(PartsBase *child);
    inline void RemoveChild(PartsBase *child);

    /* 0x04 */ boost::shared_ptr<Diablon> m_owner;
    /* 0x0C */ const char *m_name;
    /* 0x10 */ PartsBase *m_parent;
    /* 0x14 */ PartsBase *m_child; /* the first child */
    /* 0x18 */ PartsBase *m_next;  /* the next sibling */
    /* 0x1C */ u32 field_0x1C;
    /* 0x20 */ bool m_finished;
};
SIZE_ASSERT(PartsBase, 0x24);

/* A node of a part's model and the motions it plays (struct names unknown; members inferred). */
struct ModelNode {
    /* 0x0 */ s32 m_id;
    /* 0x4 */ s32 field_0x4;
};

struct ModelDesc {
    /* 0x00 */ s32 field_0x0;
    /* 0x04 */ s32 field_0x4;
    /* 0x08 */ const ModelNode *m_nodes;
    /* 0x0C */ s32 m_nodeCount;
    /* 0x10 */ const s32 *m_motions;
    /* 0x14 */ s32 m_motionCount;
};

/* The model a ModelBase owns (class name unknown; 0x38 bytes, constructor at 0x802DB4D0). */
class UnkModel_802DB4D0;

/* ModelBase (class name from RTTI; vtable 0x80558D0C, constructor 0x802DAB6C, destructor
 * 0x802DAA94): a part with a model of its own, which it ticks and whose motions it plays. Its
 * code is at 0x802DA7AC-0x802DABEC; method names are inferred. */
class ModelBase : public PartsBase {
  public:
    ModelBase(PartsBase *parent, const char *name, const boost::shared_ptr<Diablon> &owner,
              const ModelDesc *desc, const Mtx34f *parentMatrix);
    virtual ~ModelBase();
    virtual void Tick(float dt);

    u32 *GetModelFlags();                     /* 0x802DA7AC */
    const Mtx34f *GetNodeMatrix(int index);   /* 0x802DA7B8 */
    const Mtx34f *GetParentMatrix();          /* 0x802DA7E8 */
    void *GetMotion();                        /* 0x802DA7F4 */
    int GetMotionIndex();                     /* 0x802DA800 */
    void SetMotionSpeed(float speed);         /* 0x802DA80C */
    void PlayMotions(int motion, int next, float blend); /* 0x802DA838 */
    void SetMotionNode(int node, int motion); /* 0x802DA85C */
    void PlayMotion(int motion, float blend); /* 0x802DA880 */

    /* 0x24 */ UnkModel_802DB4D0 *m_model; /* a boost::scoped_ptr in the original (inferred) */
};
SIZE_ASSERT(ModelBase, 0x28);

} // namespace Parts

/* The boss's tuning values (struct name unknown; the tables are in Game/DiablonParam.cpp). */
struct DiablonParams {
    /* 0x00 */ s32 field_0x0;
    /* 0x04 */ float field_0x4;
    /* 0x08 */ float m_barrierDownTime; /* how long the barrier stays down once flag 3 is set (inferred) */
    /* 0x0C */ float m_idleTime;    /* how long the idle behavior keeps flag 8 set (inferred) */
    /* 0x10 */ float m_idleMinTime; /* ... at least, once flag 5 is set (inferred) */
    /* 0x14 */ float m_idleFacingAngle;  /* degrees; the idle behavior slows its turn within it */
    /* 0x18 */ float m_idleTurnAccel;    /* degrees */
    /* 0x1C */ float m_idleMaxTurnSpeed; /* degrees */
    /* 0x20 */ float m_moveDesireTime; /* the move desire grows by dt over it */
    /* 0x24 */ float m_moveAccel;      /* the move behavior's acceleration */
    /* 0x28 */ float m_moveFriction;   /* passed to the integration with the time step */
    /* 0x2C */ float m_moveStopFront;
    /* 0x30 */ float m_moveBackLimit; /* beyond it, the boss moves forward */
    /* 0x34 */ float m_moveStopBack;
    /* 0x38 */ float m_moveFacingAngle; /* degrees */
    /* 0x3C */ float m_moveMinDistance;
    /* 0x40 */ float m_moveMaxDistance;
    /* 0x44 */ float m_beamDesireTime;      /* the beam desire grows by dt over it while flag 0 is set */
    /* 0x48 */ float m_beamDesireTimeStill; /* ... and over this one otherwise (inferred) */
    /* 0x4C */ float m_beamDelay;     /* before the beam first aims at the player (inferred) */
    /* 0x50 */ float m_beamAngleLimit; /* degrees; the beam's highest pitch (inferred) */
    /* 0x54 */ float m_beamSweepAngle; /* degrees the beam turns while it fires (inferred) */
    /* 0x58 */ float m_beamTime;       /* how long the beam fires */
    /* 0x5C */ float m_beamFacingAngle;    /* degrees */
    /* 0x60 */ float m_beamMinDistance;    /* checked while flag 0 is set */
    /* 0x64 */ float m_beamMaxDistance;
    /* 0x68 */ float m_beamMinTargetHeight; /* checked while flag 0 is clear (inferred) */
    /* 0x6C */ float m_bombDesireTime;
    /* 0x70 */ float m_bombMaxRadius; /* the bomb blast's final radius (inferred) */
    /* 0x74 */ float m_bombGrowTime;  /* how long the blast takes to reach it (inferred) */
    /* 0x78 */ float m_bombMinDistance;
    /* 0x7C */ float m_bombMaxDistance;
    /* 0x80 */ float m_wipeDesireTime;
    /* 0x84 */ float m_wipeFacingAngle; /* degrees */
    /* 0x88 */ float m_wipeMinDistance;
    /* 0x8C */ float m_wipeMaxDistance;
    /* 0x90 */ s32 m_eventStartHintId; /* hint messages of the event behavior (inferred) */
    /* 0x94 */ s32 field_0x94;
    /* 0x98 */ s32 field_0x98;
    /* 0x9C */ s32 m_eventEndHintId1[2];   /* shown as the second event ends */
    /* 0xA4 */ s32 m_eventStartHintId2[2]; /* shown as the third event starts */
    /* 0xAC */ s32 m_wipeHintId; /* hint messages shown as each attack starts (inferred) */
    /* 0xB0 */ s32 m_beamHintId;
    /* 0xB4 */ s32 m_bombHintId;
    /* 0xB8 */ s32 field_0xB8;
    /* 0xBC */ s32 field_0xBC;
    /* 0xC0 */ float field_0xC0;
    /* 0xC4 */ float field_0xC4;
    /* 0xC8 */ float field_0xC8;
    /* 0xCC */ float field_0xCC;
    /* 0xD0 */ float field_0xD0;
    /* 0xD4 */ float field_0xD4;
    /* 0xD8 */ float field_0xD8;
    /* 0xDC */ float field_0xDC;
    /* 0xE0 */ float field_0xE0;
};
SIZE_ASSERT(DiablonParams, 0xE4);

/* The tuning tables of the boss's stages (Game/DiablonParam.cpp). */
extern DiablonParams g_params611;
extern DiablonParams g_params613;
extern DiablonParams g_paramsDefault;

/* Diablon (class name from RTTI; vtable 0x80558C54, Game/Diablon.cpp): the boss's character
 * info. Its methods that other units call keep their fn_ names (see the end of this header). */
class Diablon : public CharInfo {
  public:
    Diablon();
    virtual ~Diablon();
    virtual void PushCommand(CharCommand *command);

    /* Makes the boss and its tasks (names inferred). */
    static boost::shared_ptr<Diablon> Create(Task *parent, int cameraSlot, DiablonParams *params);
    void Setup(Task *parent);

    /* 0x64 */ Controller *m_controller; /* set while its controller exists */
    /* 0x68 */ Executer *m_executer;
    /* 0x6C */ u32 field_0x6C;
    /* 0x70 */ boost::weak_ptr<CharInfo> m_target; /* the player */
    /* 0x78 */ boost::weak_ptr<CharInfo> field_0x78; /* another character, which the event behavior
                                                     * places and sends a ToUnderCommand */
    /* 0x80 */ Parts::PartsBase *m_rootPart; /* the root of the parts tree (the executer's) */
    /* 0x84 */ s32 m_cameraSlot; /* the resource slot of the boss's camera paths (name inferred) */
    /* 0x88 */ UnkBitFlags_802D4ED4<32> m_flags;
    /* 0x8C */ DiablonParams *m_params;
    /* 0x90 */ boost::weak_ptr<Diablon> m_self;
    /* 0x98 */ UnkClumpFade_80027D5C m_fade; /* passed with the controller when the boss is hit */
    /* 0xC4 */ boost::shared_ptr<CharCommandUnit> m_commandUnit;
    /* 0xCC */ s32 field_0xCC; /* the health; the event desire's last stage waits for 0 */
    /* 0xD0 */ s32 field_0xD0; /* the charge level; the beam is wanted while it is 1-4, the bomb
                                * from 5 on */
};
SIZE_ASSERT(Diablon, 0xD4);

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
