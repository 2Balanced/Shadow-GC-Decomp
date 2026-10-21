/* BK_TANK_COLI (gadget type name): a CommandCollision::CommandCollision box that steers the Black
 * Arms tank (BKTank.cpp) when a character touches it. By its "type" parameter it sends the tank's
 * character info (id 0x92) a GadgetCommand::MoveStartCommand or MoveEndCommand with its time, an
 * AccelerateCommand or a SetPositionCommand, then stops reacting. File name inferred from the type
 * name; the other names are inferred. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). The unit's code reaches its .data through a pooled base
 * address, so the linker keeps that section whole: Game/TNumEdit.h and
 * Game/CharaColliBaseCharInfo.h supply the RTTI base lists it starts with, and the dead weak
 * vtables of the exceptions and of the gadget commands fill it zeroed. */
/* Keeps the dead copy of bad_alloc's vtable in .data, as retail has it (see <new>). */
#define MSL_NEW_INLINE_BAD_ALLOC

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

/* The cursor a parameter's label callback receives (see BAGunShip.cpp); the value is read as an
 * integer or a float by the parameter's type. */
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
}

/* The label of the type parameter and of the two parameters whose meaning depends on it. */
extern "C" const char *fn_803212B4(UnkLabelQuery_800CBC20 *cursor) {
    static s32 s_type = 0;
    static const char *s_typeLabels[4] = {"type : Move", "type : Stop", "type : Speed", "type : SetPos"};
    static const char *s_timeLabels[3] = {"no timer", "sec", "route"};
    static const char *s_rateLabels[3] = {"Not in Use", "speed", "rate(0.0 - 1.0)"};

    if (cursor->index == 3) {
        s_type = cursor->value;
        return s_typeLabels[cursor->value];
    }
    if (cursor->index == 4) {
        if (s_type == 3) {
            return s_timeLabels[2];
        }
        if (cursor->floatValue > 0.0f) {
            return s_timeLabels[1];
        }
        return s_timeLabels[0];
    }
    if (cursor->index == 5) {
        if (s_type == 2) {
            return s_rateLabels[1];
        }
        if (s_type == 3) {
            return s_rateLabels[2];
        }
        return s_rateLabels[0];
    }
    return 0;
}

extern "C" void fn_803212B0(void) {}

extern "C" void fn_803212AC(void) {}

/* The collision's callback: commands the tank once (name inferred). */
static void OnTouch(UnkCommandCollisionEvent_8026003C &event) {
    GadgetColliDetectArgs *args = event.m_args;
    UnkCommandCollisionParams_80260668 *params = event.m_params;
    u32 *flags = event.m_flags;
    bool hit = fn_8015AC84(*args);
    if (!hit) {
        hit = fn_8015B544(*args, 4, 0x91);
    }
    if (hit) {
        boost::weak_ptr<CharInfo> *tank = fn_801DCB14(fn_800DDF20(), 0x92);
        if (tank != 0) {
            if (boost::shared_ptr<CharInfo> info = tank->lock()) {
                switch (params->m_type) {
                case 0:
                    info->PushCommand(new GadgetCommand::MoveStartCommand(params->m_time, 4));
                    break;
                case 1:
                    info->PushCommand(new GadgetCommand::MoveEndCommand(params->m_time, 4));
                    break;
                case 2:
                    info->PushCommand(
                        new AccelerateCommand(Vec3f(params->m_rate, 0.0f, params->m_time), 4));
                    break;
                case 3: {
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
extern "C" void fn_80320EE4(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    CommandCollision::CommandCollision *collision = new CommandCollision::CommandCollision(parent, owner, 7);
    collision->m_callback = CommandCollisionCallback(boost::bind(OnTouch, _1));
}

/* Never called; the linker drops it. GC/2.7 lays out the dead weak vtables in .data in the order it
 * generates their functions, and this places sp_counted_base's after the gadget commands', as
 * retail has it (the unused-function trick of docs/research-notes.md). */
void BKTankColi_VtableOrder() {
    boost::shared_ptr<CharInfo> info;
}

static GadgetParamDesc lbl_8055C688[] = {
    {4, "X", 0, 0, 0, 40.0f, 1.0f, 10000.0f, fn_800CBC20, 0},
    {4, "Y", 0, 0, 0, 40.0f, 1.0f, 10000.0f, fn_800CBC20, 0},
    {4, "Z", 0, 0, 0, 40.0f, 1.0f, 10000.0f, fn_800CBC20, 0},
    {1, "type", 0, 0, 3, 0.0f, 0.0f, 0.0f, fn_803212B4, 0},
    {4, "sec", 0, 0, 0, 1.0f, 0.0f, 10000.0f, fn_803212B4, 0},
    {4, "Not in Use", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_803212B4, 0},
    {0},
};

GadgetClassDesc lbl_8055C7A0 = {
    "BK_TANK_COLI", fn_803212B0, fn_803212AC, fn_80320EE4, 0, 0x800, 0x899, 1, 0, 10, lbl_8055C688,
};
