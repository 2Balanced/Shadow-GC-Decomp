#ifndef EFFECT_SYSTEM_EFFECTCOMMANDEXEC_H
#define EFFECT_SYSTEM_EFFECTCOMMANDEXEC_H

/* The effect library's command executors (class names and bases from RTTI): an element reads its
 * description's command list and makes one executor per command, which then runs once per update
 * until Exec returns false. Executors live in the effect system's current pool. The base class is
 * in Game/EffectObject.cpp, the executors in Game/EffectCommandExec.cpp, the unit before
 * Game/EffectManager.cpp (the constructors' parameter types and all member names are inferred).
 * The executors' destructors are inline: Game/EffectObject.cpp emits EffCmdVector3Exec's
 * (0x80421C18) and Game/EffectCommandExec.cpp the others at its end, except those of the launch
 * and stop commands. */
#include "Effect/System/EffectSystem.h"
#include "Game/MathTypes.h"
#include <dolphin/types.h>

#ifdef EFFECT_COMMAND_LAUNCH
/* The launch command holds the launched effect (only Game/EffectCommandExec.cpp defines it, so
 * that the units including this header do not instantiate boost's templates). */
#include "boost/weak_ptr.hpp"
#endif

namespace Effect {
namespace System {

class EffectElement;
class EffectLaunchInfo;
class EffectManager;

/* The commands of a description (layouts from Game/EffectCommandExec.cpp). Commands are stored
 * back to back; their sizes depend on the opcode (EffectCommandExec::CountCommands). */
struct UnkEffectCommand_80421834 {
    /* 0x0 */ u8 m_opcode;
    /* 0x1 */ u8 m_hasNext;    /* whether another command follows */
    /* 0x2 */ u8 m_endsEffect; /* when the command is done, the effect ends */
    /* 0x3 */ u8 field_0x3;
};

/* The commands that change a value over time: the change (m_base, plus up to m_range times a
 * random number) is reached in m_frames frames, plus up to m_framesRange. Relative commands add
 * to the value, the others set it. */
struct UnkEffectTimedCommand_80421834 : UnkEffectCommand_80421834 {
    /* 0x4 */ u8 m_relative;
    /* 0x5 */ u8 m_sharedRandom; /* one random number for all components */
    /* 0x6 */ u8 field_0x6[2];
    /* 0x8 */ u16 m_frames;
    /* 0xA */ u16 m_framesRange;
};

struct UnkEffectVector3Command_804245CC : UnkEffectTimedCommand_80421834 {
    /* 0x0C */ Vec3f m_base;
    /* 0x18 */ Vec3f m_range;
};

struct UnkEffectVector2Command_804248D4 : UnkEffectTimedCommand_80421834 {
    /* 0x0C */ Vec2f m_base;
    /* 0x14 */ Vec2f m_range;
};

struct UnkEffectSingleCommand_80424B80 : UnkEffectTimedCommand_80421834 {
    /* 0x0C */ float m_base;
    /* 0x10 */ float m_range;
};

struct UnkEffectAlphaCommand_80423000 : UnkEffectTimedCommand_80421834 {
    /* 0x0C */ u8 m_base;
    /* 0x0D */ u8 m_range;
};

struct UnkEffectColorCommand_8042341C : UnkEffectTimedCommand_80421834 {
    /* 0x0C */ u8 m_base[3];
    /* 0x0F */ u8 m_range[3];
};

/* A loop's repeats (0: forever), or a wait's frames. */
struct UnkEffectCountCommand_80422D30 : UnkEffectCommand_80421834 {
    /* 0x4 */ u32 m_count;
};

struct UnkEffectSwitchCommand_80423FC0 : UnkEffectCommand_80421834 {
    /* 0x4 */ u8 m_on;
};

struct UnkEffectPatternCommand_8042425C : UnkEffectCommand_80421834 {
    /* 0x4 */ u8 m_grid; /* 0, 1, 2: 2, 4 or 8 cells per side */
    /* 0x5 */ u8 m_loop;
    /* 0x6 */ u8 m_firstCell;
    /* 0x7 */ u8 m_firstCellRange;
    /* 0x8 */ u8 m_lastCell;
    /* 0x9 */ u8 field_0x9[3];
    /* 0xC */ u16 m_frames; /* per cell */
    /* 0xE */ u16 m_framesRange;
};

struct UnkEffectLaunchCommand_80422750 : UnkEffectCommand_80421834 {
    /* 0x04 */ u8 field_0x4;
    /* 0x05 */ u8 m_sharedRandom;
    /* 0x06 */ u8 field_0x6[2];
    /* 0x08 */ u16 m_delay;
    /* 0x0A */ u16 m_delayRange;
    /* 0x0C */ int m_id; /* the effect of the launch info to launch */
    /* 0x10 */ Vec3f m_positionBase;
    /* 0x1C */ Vec3f m_positionRange;
    /* 0x28 */ Vec3f m_rotationBase;
    /* 0x34 */ Vec3f m_rotationRange;
    /* 0x40 */ Vec3f m_scaleBase;
    /* 0x4C */ Vec3f m_scaleRange;
    /* 0x58 */ u8 m_stopOnEnd; /* opcode 0x12 only */
};

class EffectCommandExec {
  public:
    EffectCommandExec(const u8 *data) : m_data(data), m_prev(0), m_next(0), field_0x10(0) {}
    /* 0x80421E98. */
    virtual ~EffectCommandExec();
    /* Runs the command for dt; false once it is done. */
    virtual bool Exec(float dt) = 0;

#ifdef EFFECT_SYSTEM_FULL
    /* The executors live in the current pool (EffectSystem::SetPool). */
    static void *operator new(unsigned long size) {
        return fn_8042A5D0()->PoolAlloc();
    }
    static void operator delete(void *p) {
        fn_8042A5D0()->PoolFree(p);
    }
#endif

    /* The number of commands in a list (0x804220CC). */
    static int CountCommands(const u8 *data);

    /* 0x00 vtable */
    /* 0x04 */ const u8 *m_data; /* the command; its first byte is the opcode */
    /* 0x08 */ EffectCommandExec *m_prev;
    /* 0x0C */ EffectCommandExec *m_next;
    /* 0x10 */ u8 field_0x10; /* set once Exec has run (a timed command waits one update) */
};

/* Moves a vector of the element (0x804245CC). */
class EffCmdVector3Exec : public EffectCommandExec {
  public:
    EffCmdVector3Exec(const u8 *data, u32 *state, Vec3f *target);
    virtual ~EffCmdVector3Exec() {}
    virtual bool Exec(float dt);

    /* 0x14 */ u32 *m_state;
    /* 0x18 */ Vec3f *m_target;
    /* 0x1C */ float m_elapsed;
    /* 0x20 */ float m_duration;
    /* 0x24 */ Vec3f m_delta; /* per frame once started */
};

class EffCmd3DPosExec : public EffCmdVector3Exec {
  public:
    EffCmd3DPosExec(const u8 *data, u32 *state, Vec3f *target)
        : EffCmdVector3Exec(data, state, target) {}
    virtual ~EffCmd3DPosExec() {}
};

class EffCmd3DRotExec : public EffCmdVector3Exec {
  public:
    EffCmd3DRotExec(const u8 *data, u32 *state, Vec3f *target)
        : EffCmdVector3Exec(data, state, target) {}
    virtual ~EffCmd3DRotExec() {}
};

class EffCmd3DVelExec : public EffCmdVector3Exec {
  public:
    EffCmd3DVelExec(const u8 *data, u32 *state, Vec3f *target)
        : EffCmdVector3Exec(data, state, target) {}
    virtual ~EffCmd3DVelExec() {}
};

/* Scales a vector of the element (0x80423CF8). */
class EffCmd3DSclExec : public EffectCommandExec {
  public:
    EffCmd3DSclExec(const u8 *data, u32 *state, Vec3f *target);
    virtual ~EffCmd3DSclExec() {}
    virtual bool Exec(float dt);

    /* 0x14 */ u32 *m_state;
    /* 0x18 */ Vec3f *m_target;
    /* 0x1C */ float m_elapsed;
    /* 0x20 */ float m_duration;
    /* 0x24 */ Vec3f m_delta;
};

/* Scales the first two components of a vector (0x804239E0). */
class EffCmd2DSclExec : public EffectCommandExec {
  public:
    EffCmd2DSclExec(const u8 *data, u32 *state, Vec2f *target);
    virtual ~EffCmd2DSclExec() {}
    virtual bool Exec(float dt);

    /* 0x14 */ u32 *m_state;
    /* 0x18 */ Vec3f *m_target; /* changed through the Vec3f helpers */
    /* 0x1C */ float m_elapsed;
    /* 0x20 */ float m_duration;
    /* 0x24 */ Vec3f m_delta;
};

/* Holds the command list back until its time has passed, or until the command before it has
 * ended (0x80422E34). */
class EffCmdWaitExec : public EffectCommandExec {
  public:
    EffCmdWaitExec(const u8 *data, u32 *state, EffectCommandExec *previous);
    virtual ~EffCmdWaitExec() {}
    virtual bool Exec(float dt);

    /* 0x14 */ u32 *m_state;
    /* 0x18 */ float m_elapsed;
    /* 0x1C */ float m_duration;
    /* 0x20 */ EffectCommandExec *m_previous;
};

/* 0x80422D30. */
class EffCmdLoopStartExec : public EffectCommandExec {
  public:
    EffCmdLoopStartExec(const u8 *data, u32 *state);
    virtual ~EffCmdLoopStartExec() {}
    virtual bool Exec(float dt);

    /* 0x14 */ u32 *m_state;
    /* 0x18 */ u32 m_count;
    /* 0x1C */ u32 m_remaining;
    /* 0x20 */ bool m_done;
};

/* Sets *jump to the command to continue at, if it loops back (0x80422BA8). */
class EffCmdLoopEndExec : public EffectCommandExec {
  public:
    EffCmdLoopEndExec(const u8 *data, u32 *state, EffectCommandExec *previous,
                      const u8 **jump);
    virtual ~EffCmdLoopEndExec() {}
    virtual bool Exec(float dt);

    /* 0x14 */ u32 *m_state;
    /* 0x18 */ bool m_closed; /* no open loop was found */
};

/* 0x80422B40. */
class EffCmdCallbackExec : public EffectCommandExec {
  public:
    EffCmdCallbackExec(const u8 *data, u32 *state);
    virtual ~EffCmdCallbackExec() {}
    virtual bool Exec(float dt);

    /* 0x14 */ u32 *m_state;
};

#ifdef EFFECT_COMMAND_LAUNCH
/* Launches a child effect (0x80422750). */
class EffCmdLaunchExec : public EffectCommandExec {
  public:
    EffCmdLaunchExec(const u8 *data, u32 *state, EffectManager *manager, EffectLaunchInfo *info,
                     EffectElement *parent);
    virtual ~EffCmdLaunchExec();
    virtual bool Exec(float dt);

    /* 0x14 */ u32 *m_state;
    /* 0x18 */ float m_elapsed;
    /* 0x1C */ float m_delay; /* frames after which the child is stopped (0: never) */
    /* 0x20 */ boost::weak_ptr<EffectElement> m_child;
    /* 0x28 */ EffectElement *m_parent; /* an EffectArranger */
    /* 0x2C */ Vec3f m_offset;
    /* 0x38 */ Vec3f m_angles;
    /* 0x44 */ Vec3f m_scale;
    /* 0x50 */ Mtx34f m_matrix;
};
#else
class EffCmdLaunchExec : public EffectCommandExec {
  public:
    EffCmdLaunchExec(const u8 *data, u32 *state, EffectManager *manager, EffectLaunchInfo *info,
                     EffectElement *parent);
    virtual ~EffCmdLaunchExec();
    virtual bool Exec(float dt);
};
#endif

/* 0x80422254. */
class EffCmdRequestStopExec : public EffectCommandExec {
  public:
    EffCmdRequestStopExec(const u8 *data, u32 *state, EffectElement *element);
    virtual ~EffCmdRequestStopExec();
    virtual bool Exec(float dt);

    /* 0x14 */ u32 *m_state;
};

/* Moves one float of the element (0x80424B80). */
class EffCmdSingleExec : public EffectCommandExec {
  public:
    EffCmdSingleExec(const u8 *data, u32 *state, float *target);
    virtual ~EffCmdSingleExec() {}
    virtual bool Exec(float dt);

    /* 0x14 */ u32 *m_state;
    /* 0x18 */ float *m_target;
    /* 0x1C */ float m_elapsed;
    /* 0x20 */ float m_duration;
    /* 0x24 */ float m_delta;
};

/* Moves two floats of the element (0x804248D4). */
class EffCmdVector2Exec : public EffectCommandExec {
  public:
    EffCmdVector2Exec(const u8 *data, u32 *state, Vec2f *target);
    virtual ~EffCmdVector2Exec() {}
    virtual bool Exec(float dt);

    /* 0x14 */ u32 *m_state;
    /* 0x18 */ Vec3f *m_target; /* changed through the Vec3f helpers */
    /* 0x1C */ float m_elapsed;
    /* 0x20 */ float m_duration;
    /* 0x24 */ Vec3f m_delta;
};

/* Fades a packed alpha of the element (0x80423000): 8 bits from bit 22. */
class EffCmdMatAlphaExec : public EffectCommandExec {
  public:
    EffCmdMatAlphaExec(const u8 *data, u32 *state, u32 *target);
    virtual ~EffCmdMatAlphaExec() {}
    virtual bool Exec(float dt);

    /* 0x14 */ u32 *m_state;
    /* 0x18 */ int *m_target;
    /* 0x1C */ float m_elapsed;
    /* 0x20 */ float m_duration;
    /* 0x24 */ int m_delta;
};

/* Fades the packed red, green and blue of the element (0x8042341C). */
class EffCmdMatColorExec : public EffectCommandExec {
  public:
    EffCmdMatColorExec(const u8 *data, u32 *state, u32 *target);
    virtual ~EffCmdMatColorExec() {}
    virtual bool Exec(float dt);

    /* 0x14 */ u32 *m_state;
    /* 0x18 */ int *m_target;
    /* 0x1C */ float m_elapsed;
    /* 0x20 */ float m_duration;
    /* 0x24 */ int m_delta[3];
};

/* Switches the glare: sets state bit 1, and bit 2 to the command's (0x80423FC0). */
class EffCmdGlareSWExec : public EffectCommandExec {
  public:
    EffCmdGlareSWExec(const u8 *data, u32 *state);
    virtual ~EffCmdGlareSWExec() {}
    virtual bool Exec(float dt);

    /* 0x14 */ u32 *m_state;
};

/* Plays a texture pattern animation: picks the cells of a grid in turn (0x8042425C). */
class EffCmdPtnAnimExec : public EffectCommandExec {
  public:
    EffCmdPtnAnimExec(const u8 *data, u32 *state, Vec2f *uvMin, Vec2f *uvMax);
    virtual ~EffCmdPtnAnimExec() {}
    virtual bool Exec(float dt);

    /* Sets the texture coordinates of the current cell. */
    void SetCell() {
        u8 row = m_cell / m_grid;
        u8 column = m_cell % m_grid;
        float size = 1.0f / m_grid;
        m_uvMin->x = column * size;
        m_uvMin->y = row * size;
        m_uvMax->x = (column + 1) * size;
        m_uvMax->y = (row + 1) * size;
    }

    /* 0x14 */ u32 *m_state;
    /* 0x18 */ Vec2f *m_uvMin;
    /* 0x1C */ Vec2f *m_uvMax;
    /* 0x20 */ u8 m_cell;
    /* 0x21 */ u8 m_firstCell;
    /* 0x22 */ u8 m_lastCell;
    /* 0x23 */ u8 m_grid; /* cells per side; 0 once done */
    /* 0x24 */ float m_elapsed;
    /* 0x28 */ float m_frames;
};

/* Single-float commands the particle and ray elements share (Game/EffectParticle.cpp and
 * Game/EffectRay.cpp; retail keeps their vtables and destructors in the first). */
class EffCmdGenQuantityExec : public EffCmdSingleExec {
  public:
    EffCmdGenQuantityExec(const u8 *data, u32 *state, float *target)
        : EffCmdSingleExec(data, state, target) {}
};

class EffCmdFrictionExec : public EffCmdSingleExec {
  public:
    EffCmdFrictionExec(const u8 *data, u32 *state, float *target)
        : EffCmdSingleExec(data, state, target) {}
};

/* The colour and alpha executors Game/EffectLocus.cpp also starts (constructors and vtables in
 * the units before; colours are four words, 8 bits each from bit 22). */
class EffCmdTopColorExec : public EffCmdMatColorExec {
  public:
    EffCmdTopColorExec(const u8 *data, u32 *state, u32 *target)
        : EffCmdMatColorExec(data, state, target) {}
    virtual ~EffCmdTopColorExec();
};

class EffCmdLastColorExec : public EffCmdMatColorExec {
  public:
    EffCmdLastColorExec(const u8 *data, u32 *state, u32 *target)
        : EffCmdMatColorExec(data, state, target) {}
    virtual ~EffCmdLastColorExec();
};

class EffCmdTopAlphaExec : public EffCmdMatAlphaExec {
  public:
    EffCmdTopAlphaExec(const u8 *data, u32 *state, u32 *target)
        : EffCmdMatAlphaExec(data, state, target) {}
    virtual ~EffCmdTopAlphaExec();
};

class EffCmdLastAlphaExec : public EffCmdMatAlphaExec {
  public:
    EffCmdLastAlphaExec(const u8 *data, u32 *state, u32 *target)
        : EffCmdMatAlphaExec(data, state, target) {}
    virtual ~EffCmdLastAlphaExec();
};

} // namespace System
} // namespace Effect

#endif
