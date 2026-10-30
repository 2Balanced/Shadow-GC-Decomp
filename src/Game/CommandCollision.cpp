/* CommandCollision::CommandCollision (class name from RTTI; its s_taskName is "CommandCollision"):
 * a box collider of the size its owner's parameters give, made for another stage gadget type
 * (BK_TANK_COLI, E_B_COLI, E_P_COLI, ...), which sets m_callback. While a character touches the box
 * the contact handle's callback passes the event, with the parameters and the flag word, to
 * m_callback until bit 0 of the flag word is set; the gadget then reports itself finished. A
 * positive switch id sets that bit once the stage switch is on. File name from the class; method
 * and member names marked "(name inferred)" in Game/CommandCollision.h are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#define COMMANDCOLLISION_IMPL
/* boost::function's operator() puts a bad_function_call on the stack (see <stdexcept>). */
#define MSL_RUNTIME_ERROR_STRING
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CommandCollision.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharCollision.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetColliDetectArgs.h"
#include "Game/MathTypes.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::CharaColliBase;

/* A vector constant read by the collision setup (meaning unknown; see Weight.cpp). */
extern Vec3f lbl_805E07B8;

extern "C" {
/* The collider manager (see Weight.cpp). */
void *fn_80021B08(void);
/* Creates a box collider of the given half size for charInfo (see Weight.cpp). */
boost::shared_ptr<CharaColliBase> fn_800C7C34(void *manager, const boost::weak_ptr<CharInfo> &charInfo,
                                              const Vec3f *size, int flags, int arg4, int arg5,
                                              const Vec3f *offset, const Vec3f *rotation,
                                              int arg8);
/* Wraps collider in a new contact handle, re-registers the handle's collider, and advances the
 * handle by dt (see Weight.cpp). */
boost::shared_ptr<GadgetCharCollision> fn_8015B8C4(boost::shared_ptr<CharaColliBase> collider);
void fn_8015BE64(GadgetCharCollision *handle);
void fn_8015BE9C(GadgetCharCollision *handle, float dt);
/* Reads the owner's switch id (behaviour inferred). */
void fn_800C9E50(UnkPlacement_800C9CD0 *owner, s32 *id);
/* Tells the gadget's owner that the gadget is done (see ElecBarrier.cpp). */
void fn_8015A794(Gadget *gadget);
}

namespace CommandCollision {

/* The collider's size before the parameters set it (name inferred). */
static Vec3f s_size(30.0f, 15.0f, 20.0f);

const char *CommandCollision::s_taskName = "CommandCollision";

CommandCollision::CommandCollision(Task *parent, UnkPlacement_800C9CD0 *owner, int arg) : Gadget(parent, owner) {
    m_params.m_size[0] = 1.0f;
    m_params.m_size[1] = 1.0f;
    m_params.m_size[2] = 1.0f;
    m_params.m_type = 0;
    m_params.m_time = 1.0f;
    m_params.m_rate = 0.0f;
    m_flags.Init();
    m_name = s_taskName;
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_chara = fn_8015CB88(0x3E, 4, s_taskName);
    {
        Vec3f size;
        fn_800091EC(&size, &s_size);
        boost::shared_ptr<CharaColliBase> collider =
            fn_800C7C34(fn_80021B08(), m_chara, &size, 0x40000, arg, 0, &lbl_805E07A0, &lbl_805E07B8, 0);
        m_collision = fn_8015B8C4(collider);
        fn_800F5980(m_collision.get(), boost::bind(&CommandCollision::OnColliDetect, this, _1));
        m_collision->m_collider->SetSize(m_params.m_size[0], m_params.m_size[1], m_params.m_size[2], -2);
    }
    ReadPlacement(owner);
}

CommandCollision::~CommandCollision() {}

void CommandCollision::Behave(float dt) {
    if (m_collision.get() != 0) {
        fn_8015BE9C(m_collision.get(), dt);
    }
    switch (m_state) {
    case 0:
        m_state = 1;
        break;
    case 1:
        if (m_switchId > 0 && fn_8016904C(fn_800CB69C(), m_switchId) == 1) {
            m_flags.Set(0);
        }
        if (m_flags.Test(0)) {
            m_state = 2;
        }
        break;
    case 2:
        fn_8015A794(this);
        Task::m_flags |= 1;
        break;
    }
}

void CommandCollision::fn_800CBC1C() {
    if (m_collision.get() != 0) {
        fn_8015BE64(m_collision.get());
    }
}

void CommandCollision::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    if (m_collision.get() != 0) {
        m_collision->m_collider->SetSize(m_params.m_size[0], m_params.m_size[1], m_params.m_size[2], -2);
        fn_8015BE64(m_collision.get());
    }
}

void CommandCollision::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    fn_800C9E50(owner, &m_switchId);
    m_state = 0;
    m_flags.Clear();
}

void CommandCollision::OnColliDetect(ColliDetectArgs &args) {
    if (!m_callback.m_function) {
        return;
    }
    if (m_flags.Test(0)) {
        return;
    }
    Args event;
    event.m_args = static_cast<GadgetColliDetectArgs *>(&args);
    event.m_params = &m_params;
    event.m_flags = &m_flags.m_bits;
    m_callback.CallIfSet(event);
}

} // namespace CommandCollision
