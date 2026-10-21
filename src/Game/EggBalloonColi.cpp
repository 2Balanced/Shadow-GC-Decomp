/* E_B_COLI (gadget type name): a CommandCollision::CommandCollision box that steers the egg balloon
 * (EggBalloon.cpp, run by EggBalloonController.cpp) when a character touches it, as BK_TANK_COLI
 * (BKTankColi.cpp) steers the Black Arms tank. By its "type" parameter it sends the controller's
 * character info (id 0x3F) a GadgetCommand::MoveStartCommand or MoveEndCommand with its time, a
 * GadgetCommand::AttackStartCommand that starts one of the balloon's four attacks, an
 * AttackEndCommand that stops it, an AccelerateCommand or a SetPositionCommand, then stops
 * reacting. The unit also holds the four stage callbacks that create, delete, recreate and poke
 * the balloon. File name inferred from the type name; the other names are inferred. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first). The unit's
 * code reaches its .data through a pooled base address, so the linker keeps that section whole:
 * Game/TNumEdit.h and Game/CharaColliBaseCharInfo.h supply the RTTI base lists it starts with,
 * and the dead weak vtables of the exceptions and of the gadget commands fill it zeroed. */
/* Keeps the dead copy of bad_alloc's vtable in .data, as retail has it (see <new>). */
#define MSL_NEW_INLINE_BAD_ALLOC
/* The gadget commands' constructors call GadgetCommand's out of line (see Game/GadgetCommand.h). */
#define GADGETCOMMAND_CTOR_NOT_INLINED

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/CommandCollision.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharCommands.h"
#include "Game/GadgetColliDetectArgs.h"
#include "Game/GadgetCommand.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

namespace GadgetCommand {

/* GadgetCommand::AttackStartCommand (class name from RTTI): command 0x303, which starts attack
 * m_kind of the balloon for m_time seconds, firing every m_interval seconds (the balloon reads it
 * as UnkPathObjectMessage; meanings inferred). Its constructor is defined in the class but, as
 * it calls GadgetCommand's inline one, not inlined. */
class AttackStartCommand : public GadgetCommand {
  public:
    AttackStartCommand(float time, int kind, float interval, int arg)
        : GadgetCommand(0x303, arg), m_kind(kind), m_time(time), m_interval(interval) {}
    virtual ~AttackStartCommand() {}

    /* 0x14 */ s32 m_kind; /* 0 right gun, 1 left gun, 2 bomb, 3 fan */
    /* 0x18 */ float m_time;
    /* 0x1C */ float m_interval;
};
SIZE_ASSERT(AttackStartCommand, 0x20);

/* GadgetCommand::AttackEndCommand (class name from RTTI): command 0x304, which stops the balloon's
 * attack (member name inferred). */
class AttackEndCommand : public GadgetCommand {
  public:
    AttackEndCommand(float time, int arg) : GadgetCommand(0x304, arg), m_time(time) {}
    virtual ~AttackEndCommand() {}

    /* 0x14 */ float m_time;
};
SIZE_ASSERT(AttackEndCommand, 0x18);

} // namespace GadgetCommand

/* The cursor a parameter's label callback receives (see BKTankColi.cpp). */
class UnkLabelQuery_800CBC20 {
  public:
    /* 0x0 */ const GadgetParamDesc *params;
    /* 0x4 */ s32 index; /* the parameter being shown */
    /* 0x8 */ s32 value; /* its current value */
    /* 0xC */ float floatValue;
};

/* The character manager (see Monster.cpp). */
class UnkCharManager_800DDF20;

extern "C" {
/* The character manager and the character info registered under id (behaviour inferred). */
UnkCharManager_800DDF20 *fn_800DDF20(void);
boost::weak_ptr<CharInfo> *fn_801DCB14(UnkCharManager_800DDF20 *manager, int id);
/* Whether the collision is with the player, and with a character of kind kind and id id
 * (behaviour inferred). */
bool fn_8015AC84(ColliDetectArgs &args);
bool fn_8015B544(ColliDetectArgs &args, int kind, int id);
/* Sets a bit of a 32-bit flag word (see EggBalloon.cpp). */
void fn_80014E1C(u32 *flags, int bit);

/* The balloon's stage callbacks (EggBalloon.cpp): create it with its controller, delete the
 * controller, poke the balloon, and delete and create it again. */
void fn_8014565C(void);
void fn_80145614(void);
void fn_801455E8(void);
void fn_8014559C(void);
}

/* The label of the type parameter and of the two parameters whose meaning depends on it. */
extern "C" const char *fn_80273A0C(UnkLabelQuery_800CBC20 *cursor) {
    static s32 s_type = 0;
    static const char *s_typeLabels[9] = {
        "type : Move",       "type : Stop",       "type : RightGun Attack",
        "type : BombAttack", "type : FanAttack",  "type : StopAttack",
        "type : Accelerate", "type : SetPos",     "type : LeftGun Attack",
    };
    static const char *s_timeLabels[3] = {"no timer", "sec", "route"};
    static const char *s_rateLabels[3] = {"attack timer", "speed", "rate(0.0 - 1.0)"};

    if (cursor->index == 3) {
        s_type = cursor->value;
        return s_typeLabels[cursor->value];
    }
    if (cursor->index == 4) {
        if (s_type == 7) {
            return s_timeLabels[2];
        }
        if (cursor->floatValue > 0.0f) {
            return s_timeLabels[1];
        }
        return s_timeLabels[0];
    }
    if (cursor->index == 5) {
        if (s_type == 6) {
            return s_rateLabels[1];
        }
        if (s_type == 7) {
            return s_rateLabels[2];
        }
        return s_rateLabels[0];
    }
    return 0;
}

extern "C" void fn_80273A08(void) {}

extern "C" void fn_80273A04(void) {}

/* The collision's callback: commands the balloon once (name inferred). */
static void OnTouch(UnkCommandCollisionEvent_8026003C &event) {
    GadgetColliDetectArgs *args = event.m_args;
    UnkCommandCollisionParams_80260668 *params = event.m_params;
    u32 *flags = event.m_flags;
    bool hit = fn_8015AC84(*args);
    if (!hit) {
        hit = fn_8015B544(*args, 4, 0x3D);
    }
    if (hit) {
        boost::weak_ptr<CharInfo> *balloon = fn_801DCB14(fn_800DDF20(), 0x3F);
        if (balloon != 0) {
            if (boost::shared_ptr<CharInfo> info = balloon->lock()) {
                switch (params->m_type) {
                case 0:
                    info->PushCommand(new GadgetCommand::MoveStartCommand(params->m_time, 4));
                    break;
                case 1:
                    info->PushCommand(new GadgetCommand::MoveEndCommand(params->m_time, 4));
                    break;
                case 2:
                    info->PushCommand(
                        new GadgetCommand::AttackStartCommand(params->m_time, 0, params->m_rate, 4));
                    break;
                case 8:
                    info->PushCommand(
                        new GadgetCommand::AttackStartCommand(params->m_time, 1, params->m_rate, 4));
                    break;
                case 3:
                    info->PushCommand(
                        new GadgetCommand::AttackStartCommand(params->m_time, 2, params->m_rate, 4));
                    break;
                case 4:
                    info->PushCommand(
                        new GadgetCommand::AttackStartCommand(params->m_time, 3, params->m_rate, 4));
                    break;
                case 5:
                    info->PushCommand(new GadgetCommand::AttackEndCommand(0.0f, 4));
                    break;
                case 6:
                    info->PushCommand(
                        new AccelerateCommand(Vec3f(params->m_rate, 0.0f, 0.0f), 4));
                    break;
                case 7: {
                    Vec3f position;
                    position.x = params->m_time;
                    position.y = params->m_rate;
                    position.z = 0.0f;
                    info->PushCommand(new SetPositionCommand(position, 4));
                    break;
                }
                }
            }
        }
        fn_80014E1C(flags, 0);
    }
}

/* Creates the collision under task layer 11 (name inferred). */
extern "C" void fn_80273454(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    CommandCollision::CommandCollision *collision = new CommandCollision::CommandCollision(parent, owner, 7);
    collision->m_callback = CommandCollisionCallback(boost::bind(OnTouch, _1));
}

/* The stage callbacks (names inferred). */
extern "C" bool fn_80273430(void) {
    fn_8014565C();
    return true;
}

extern "C" bool fn_8027340C(void) {
    fn_801455E8();
    return true;
}

extern "C" bool fn_802733E8(void) {
    fn_8014559C();
    return true;
}

extern "C" bool fn_802733C4(void) {
    fn_80145614();
    return true;
}

/* Never called; the linker drops it. It places sp_counted_base's dead vtable after the gadget
 * commands', as retail has it (see BKTankColi.cpp). */
void EggBalloonColi_VtableOrder() {
    boost::shared_ptr<CharInfo> info;
}

static GadgetParamDesc lbl_8054CD3C[] = {
    {4, "X", 0, 0, 0, 100.0f, 1.0f, 10000.0f, fn_800CBC20, 0},
    {4, "Y", 0, 0, 0, 100.0f, 1.0f, 10000.0f, fn_800CBC20, 0},
    {4, "Z", 0, 0, 0, 100.0f, 1.0f, 10000.0f, fn_800CBC20, 0},
    {1, "type", 0, 0, 8, 0.0f, 0.0f, 0.0f, fn_80273A0C, 0},
    {4, "sec", 0, 0, 0, 1.0f, 0.0f, 10000.0f, fn_80273A0C, 0},
    {4, "N/A", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_80273A0C, 0},
    {0},
};

GadgetClassDesc lbl_8054CE54 = {
    "E_B_COLI", fn_80273A08, fn_80273A04, fn_80273454, 0, 0x800, 0x1451, 1, 0, 10, lbl_8054CD3C,
};
