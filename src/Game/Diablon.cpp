/* Boss::Diablon::Diablon (class name from RTTI; declared in Game/Diablon.h): the Diablon boss's
 * character info. Create makes it with a stage's tuning table and sets up its controller,
 * executer and effects; its helpers measure the player's distance and direction and keep the
 * health and the charge level in range. The helpers other units call keep their fn_ names (C
 * functions taking the boss). File name inferred from the class. Names marked "(name inferred)"
 * are not original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/Diablon.h"
#include "Game/MathTypes.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

extern "C" {
/* value clamped to [min, max], and the smaller of a and b (behaviour from their bodies). */
const s32 &fn_8016F21C(const s32 &value, const s32 &min, const s32 &max);
const float &fn_800704AC(const float &a, const float &b);
/* Hands command to the command unit (see Game/GadgetCharInfo.cpp). */
void fn_801D9C0C(boost::shared_ptr<CharCommandUnit> unit, CharCommand *command);
/* Vector helpers (see Game/DiablonBehaviorMove.cpp). */
float fn_8040CF50(const Vec3f *v);                   /* |v| */
void fn_8040CE74(Vec3f *vector);                      /* normalise */
float fn_8040CF90(const Vec3f *a, const Vec3f *b);   /* dot product */
Vec3f fn_8040D0AC(const Vec3f *vector, float angle); /* turned about the y axis */
/* Where new tasks go (see Game/Gadget.h). */
void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);

/* The boss's helpers (see the file comment). */
void fn_802D9664(Boss::Diablon::Diablon *diablon, int amount);
void fn_802D96B0(Boss::Diablon::Diablon *diablon, int amount);
float fn_802D96FC(Boss::Diablon::Diablon *diablon);
}

typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* The highest charge level (name inferred). */
const s32 kMaxCharge = 5;

namespace Boss {
namespace Diablon {

/* The boss's controller (defined in Game/DiablonController.cpp); only its constructor is used
 * here. */
class Controller : public Task {
  public:
    Controller(Task *parent, const boost::shared_ptr<Diablon> &owner);

    /* 0x28 */ void *m_impl; /* a boost::scoped_ptr */
    /* 0x2C */ u32 field_0x2C;
};
SIZE_ASSERT(Controller, 0x30);

boost::shared_ptr<Diablon> Diablon::Create(Task *parent, int cameraSlot, DiablonParams *params) {
    boost::shared_ptr<Diablon> diablon = boost::shared_ptr<Diablon>(new Diablon);
    diablon->m_cameraSlot = cameraSlot;
    diablon->m_params = params;
    diablon->m_self = diablon;
    diablon->Setup(parent);
    return diablon;
}

void Diablon::Setup(Task *parent) {
    boost::shared_ptr<Diablon> self = m_self.lock();
    field_0xCC = m_params->field_0x0;
    field_0xD0 = 0;
    new Controller(fn_801EB304(fn_8001C4F8(), 7), self);
    new Executer(parent, self);
    new Effect(m_executer, self);
    m_fade.m_mode = 8;
    m_executer->Start();
}

Diablon::Diablon() : CharInfo(0xAC) {
    field_0x5C = 3;
    m_commandUnit = boost::shared_ptr<CharCommandUnit>(new CharCommandUnit);
    m_controller = 0;
    m_executer = 0;
    field_0x6C = 0;
    m_params = 0;
}

Diablon::~Diablon() {}

void Diablon::PushCommand(CharCommand *command) {
    if (m_commandUnit.px != 0) {
        fn_801D9C0C(m_commandUnit, command);
    } else {
        delete command;
    }
}

} // namespace Diablon
} // namespace Boss

using Boss::Diablon::Diablon;

/* The health as a share of the most (name inferred). */
extern "C" float fn_802D96FC(Diablon *diablon) {
    return fn_800704AC((float)diablon->field_0xCC / (float)diablon->m_params->field_0x0, 1.0f);
}

/* Changes the health by amount (name inferred). */
extern "C" void fn_802D96B0(Diablon *diablon, int amount) {
    diablon->field_0xCC += amount;
    diablon->field_0xCC = fn_8016F21C(diablon->field_0xCC, 0, diablon->m_params->field_0x0);
}

/* Changes the charge level by amount (name inferred). */
extern "C" void fn_802D9664(Diablon *diablon, int amount) {
    diablon->field_0xD0 += amount;
    diablon->field_0xD0 = fn_8016F21C(diablon->field_0xD0, 0, kMaxCharge);
}

/* The cosine of the angle between the boss's facing and the player, -1 without a player (name
 * inferred). */
extern "C" float fn_802D9570(Diablon *diablon) {
    boost::shared_ptr<CharInfo> target = diablon->m_target.lock();
    if (target.get() == 0) {
        return -1.0f;
    }
    Vec3f offset;
    fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&target->m_position,
                                                                          &diablon->m_position));
    offset.y = 0.0f;
    fn_8040CE74(&offset);
    Vec3f forward(0.0f, 0.0f, -1.0f);
    fn_800091D0(&forward, &(const Vec3f &)fn_8040D0AC(&forward, diablon->m_rotation.y));
    float facing = fn_8040CF90(&offset, &forward);
    return facing;
}

/* The player's distance across the ground, 0 without a player (name inferred). */
extern "C" float fn_802D94B8(Diablon *diablon) {
    boost::shared_ptr<CharInfo> target = diablon->m_target.lock();
    if (target.get() == 0) {
        return 0.0f;
    }
    Vec3f offset;
    fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&target->m_position,
                                                                          &diablon->m_position));
    offset.y = 0.0f;
    float distance = fn_8040CF50(&offset);
    return distance;
}
