/* Bomb::Bomb (class name from RTTI): the BOMB gadget, a bomb (BOMB.BIN) a character can pick up,
 * carry and throw or drop. It is a pickup object (GadgetPickup::PickupBase) with a PickupCharInfo
 * of its own: resting, it follows a ContainerBehavior; carried, it follows the carrier's hand
 * matrix; thrown or dropped, it flies until it hits something. An attack, a hit after a throw, or
 * its switch turning on makes it explode: it leaves a Bomb::BombBlast (class name from RTTI), a
 * task whose attack collision grows for a moment, and rewards the character that set it off. Game
 * code can also spawn bombs (Bomb::Spawn). File name inferred from the class. Names marked
 * "(name inferred)" are not original. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
/* The weak copies of std::bad_alloc's functions, vtable and RTTI are in retail's .data (as in
 * BreakWall). */
#define MSL_NEW_INLINE_BAD_ALLOC
#include <algorithm>
#include <boost/function/function_base.hpp>

/* std::swap for boost::function's functor pointer. The game has one shared copy of that instance
 * (0x80019164), where GC/2.7 would emit a local copy of its own; this overload, declared but not
 * defined here (as in CatchStick.cpp), makes boost::function's swap call the shared one. */
namespace std {
void swap(boost::detail::function::any_pointer &a, boost::detail::function::any_pointer &b);
} // namespace std

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Effect/System/EffectElement.h"
#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/ContainerBehavior.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetPickup.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/PickupCharInfo.h"
#include "Game/Task.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

/* Target (class name from RTTI): a point characters aim at. Only the flag word is mapped here
 * (bit 0 makes the target active; names inferred). */
class Target {
  public:
    void TurnOff() {
        m_flags &= ~1;
    }

    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ u32 m_flags;
};

/* The handle fn_8015B8C4 wraps a collider in (class name unknown; only the collider is mapped). */
class GadgetCharCollision {
  public:
    /* 0x00 */ u8 field_0x0[0x24];
    /* 0x24 */ boost::shared_ptr<CharaColliBase> m_collider;
};

/* boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out of line; this unit
 * emits the copy for ContainerBehavior (0x800EED84; boost 1.32's definition). */
namespace boost {
template<> inline void shared_ptr<ContainerBehavior>::reset() {
    this_type().swap(*this);
}
} // namespace boost


extern "C" {
/* Sets a character info's name (behaviour inferred). */
void fn_801DC984(CharInfo *charInfo, const char *name);
/* Creates the pickup's target for owner with the given flags and radius, and moves the target to
 * the character info each frame (behaviours inferred). */
void fn_801E4160(PickupCharInfo *charInfo, const boost::weak_ptr<CharInfo> &owner, int flags,
                 float radius);
void fn_801E413C(PickupCharInfo *charInfo, CharInfo *position);
/* Loads the model fileName (behaviour inferred; arg0 unknown). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* Returns the attack of a model's collision (behaviour inferred). */
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int variant);
/* Turns the model's collision off (behaviour inferred; the arguments are unknown). */
void fn_80161230(GadgetModelInfo *model, int index, int arg2);
/* Sends message to the character (behaviour and meaning of message inferred). */
int fn_8020642C(const boost::shared_ptr<CharInfo> &character, int message);
/* The switch id the owner was placed with (behaviour inferred). */
void fn_800C9E50(UnkPlacement_800C9CD0 *owner, s32 *id);
/* Lets a vacuum pull the model in (behaviour inferred; arg2 unknown). */
void fn_8033F0F8(CharCommand *command, boost::shared_ptr<GadgetModelInfo> *model, int arg2,
                 CharInfo *charInfo, const Vec3f *rotation);
/* Vector helpers (behaviour inferred): vector * scale, vector += value, the translation and the
 * rotation of a matrix, and matrix * point. */
Vec3f fn_800091A4(const Vec3f *vector, float scale);
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
Vec3f fn_800496F8(const Mtx34f *matrix);
void fn_800494F8(Vec3f *rotation, const Mtx34f *matrix);
Vec3f fn_8040B618(const Mtx34f *matrix, const Vec3f *point);
/* matrix translated by (x, y, z) (behaviour inferred). */
void fn_8040B52C(Mtx34f *matrix, float x, float y, float z);
/* Moves position by velocity, which acceleration changes, over dt (behaviour inferred). */
void fn_804103E8(Vec3f *position, Vec3f *velocity,
                 const Vec3f *acceleration, float scale, float dt);
/* The collision manager, and a sweep test from start along delta that stops start at a hit
 * (behaviour inferred; the other arguments are unknown). */
void *fn_80021B08(void);
bool fn_801BFC28(int arg0, Vec3f *start, Vec3f *delta, u32 *flags, int arg4,
                 float radius);
/* A sphere collider at position (behaviour inferred). */
boost::shared_ptr<CharaColliBase> fn_800C7EA8(void *manager, const Vec3f *position,
                                              u32 flags, int arg3, int arg4,
                                              const boost::weak_ptr<CharInfo> &owner,
                                              const Vec3f *offset, float radius);
/* The attack collision of a collider (behaviour inferred). */
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager,
                                              boost::shared_ptr<CharaColliBase> collider);
/* Wraps collider in a new handle, and advances the handle by dt (behaviour inferred). */
boost::shared_ptr<GadgetCharCollision> fn_8015B8C4(boost::shared_ptr<CharaColliBase> collider);
void fn_8015BE9C(GadgetCharCollision *handle, float dt);
/* Starts effect id at position, facing up (behaviour inferred). */
boost::weak_ptr<Effect::System::EffectElement> fn_8015D7DC(const Vec3f *position,
                                                           const Vec3f &up, int id);
/* Plays sound id at position (behaviour inferred; the other arguments are unknown). */
void fn_801D54F4(void *manager, int id, const Vec3f *position, int arg3, int arg4);
}

/* The empty character info (an .sbss shared_ptr; meaning inferred). */
extern boost::shared_ptr<CharInfo> lbl_805EE698;

/* vector += value (name inferred). */
inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

/* result = a - b (name inferred). Retail's a - b returns its result through the caller's
 * temporary, which this reproduces. */
inline const Vec3f &Subtract(Vec3f *result, const Vec3f *a,
                                     const Vec3f *b) {
    fn_8040CD74(result, a, b);
    return *result;
}

/* Copies a rotation one angle at a time (name inferred). */
inline void CopyRotation(Vec3f &rotation, const Vec3f &value) {
    rotation.x = value.x;
    rotation.y = value.y;
    rotation.z = value.z;
}

namespace Bomb {

/* The blast a bomb leaves (class name from RTTI; names inferred). */
class BombBlast : public Task {
  public:
    BombBlast(Task *parent, const Vec3f &position);
    virtual ~BombBlast();
    virtual void Tick(float dt);

    /* Creates the blast under task layer 11 (name inferred). */
    static void Instantiate(const Vec3f &position);

    /* The callback of the blast's attack collision (name inferred). */
    void OnAttack(AttackCallbackParam &param);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x28 */ Vec3f m_position;
    /* 0x34 */ boost::shared_ptr<GadgetCharCollision> m_handle;
    /* 0x3C */ float m_radius;
    /* 0x40 */ float m_timer;
    /* 0x44 */ s32 m_state; /* 0 new, 1 starting, 2 growing, 3 done */
};
SIZE_ASSERT(BombBlast, 0x48);

class Bomb : public GadgetPickup::PickupBase {
  public:
    Bomb();
    Bomb(const boost::weak_ptr<CharInfo> &owner);
    virtual ~Bomb();
    virtual void SetPosition(const Vec3f &position, const Vec3f &rotation);
    virtual void Init(UnkPlacement_800C9CD0 *owner);
    virtual void Tick(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);
    virtual void SetPlacement(const Vec3f &position, const Vec3f &rotation);
    virtual bool IsDone(GadgetPickup::PickupFlags *request);

    /* Creates a bomb from an owner (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);
    /* Spawns a bomb for owner at position, finished once out of range (name inferred). */
    static void Spawn(const Vec3f &position, const Vec3f &rotation,
                      const boost::weak_ptr<CharInfo> &owner, float range);

    /* Method names are inferred. */
    void UpdateFlight(float dt);
    void OnAttack(AttackCallbackParam &param);
    void UpdateCarried();
    void OnSetGrabPosition(PickupCharInfo::SetGrabPositionArgs &args);
    void OnCommand(CharCommand &command);

    /* Member names are inferred. */
    /* 0x6C */ u32 field_0x6C;
    /* 0x70 */ boost::shared_ptr<PickupCharInfo> m_pickup;
    /* 0x78 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x80 */ boost::shared_ptr<ContainerBehavior> m_behavior;
    /* 0x88 */ s32 m_state; /* 0 new, 1 resting, 2 picked up, 3 carried, 4 flying, 5 exploding,
                               6 vacuumed */
    /* 0x8C */ s32 m_switchId;
    /* 0x90 */ boost::weak_ptr<CharInfo> m_carrier;
    /* 0x98 */ boost::weak_ptr<CharInfo> m_attacker;
    /* 0xA0 */ float m_speed;
    /* 0xA4 */ Vec3f m_direction;
    /* 0xB0 */ Vec3f m_lastPosition;
    /* 0xBC */ Mtx34f m_handMatrix;
    /* 0xEC */ boost::weak_ptr<CharInfo> m_owner;
    /* 0xF4 */ GadgetPickup::PickupFlags m_flags; /* 0 flying, 1 picked up, 2 done, 3 thrown, 4 hit
                                                     test */
    /* 0xF8 */ GadgetPickup::PickupFlags m_hitFlags;
};
SIZE_ASSERT(Bomb, 0xFC);

const char *BombBlast::s_taskName = "BombBlast";

Bomb::Bomb() : PickupBase(0xE) {
    fn_801DC984(this, "Bomb");
}

Bomb::Bomb(const boost::weak_ptr<CharInfo> &owner) : PickupBase(0xE), m_owner(owner) {
    fn_801DC984(this, "Bomb");
}

Bomb::~Bomb() {
    if (!m_owner.expired()) {
        boost::shared_ptr<CharInfo> owner = m_owner.lock();
        if (owner.get() != 0) {
            owner->PushCommand(new UseCommand(m_pickup));
        }
    }
}

void Bomb::Init(UnkPlacement_800C9CD0 *owner) {
    m_flags.Clear();
    m_hitFlags.Set(1);
    m_pickup = boost::shared_ptr<PickupCharInfo>(new PickupCharInfo(0xE));
    fn_800729B0(&m_pickup->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&Bomb::OnCommand, this, _1)));
    fn_801E4160(m_pickup.get(), m_pickup, 0x880, 30.0f);
    fn_801DC984(m_pickup.get(), "Bomb");
    m_pickup->SetKind(4);
    m_pickup->m_grabCallback = boost::function1<void, PickupCharInfo::SetGrabPositionArgs &>(
        boost::bind(&Bomb::OnSetGrabPosition, this, _1));
    m_model = fn_8015F800(0, "BOMB.BIN");
    m_model->fn_80162A68(0, 0);
    m_model->fn_80161F08("col", m_pickup);
    boost::weak_ptr<CharaColliAttack> attack = fn_80161428(m_model.get(), 0);
    if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
        collision->field_0x54 = 0;
        collision->field_0x55 = 1;
        collision->m_attackCallback = AttackCallback(boost::bind(&Bomb::OnAttack, this, _1));
    }
    m_state = 0;
    m_switchId = 0;
    SetPosition(m_pickup->m_position, m_pickup->m_rotation);
}

void Bomb::Tick(float dt) {
    if (m_flags.Test(0)) {
        UpdateFlight(dt);
    }
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }

    switch (m_state) {
    case 0:
        if (m_behavior.get() == 0) {
            m_behavior = boost::shared_ptr<ContainerBehavior>(
                new ContainerBehavior((GadgetCharInfo *)m_pickup.get(), m_model));
        } else {
            fn_800F8FC4(m_behavior.get(), &m_pickup->m_position, &m_pickup->m_rotation);
        }
        m_state = 1;
        break;
    case 1:
        if (m_behavior.get() != 0) {
            m_pickup->m_position.y -= 10.0f;
            fn_800EF474(m_behavior.get(), &m_pickup->m_position, &m_pickup->m_rotation);
            fn_800F9000(m_behavior.get());
            fn_800EF448(m_behavior.get(), &m_pickup->m_position);
            m_pickup->m_position.y += 10.0f;
        }
        if (m_switchId > 0 && fn_8016904C(fn_800CB69C(), m_switchId) == 1) {
            m_state = 5;
        }
        SetPosition(m_pickup->m_position, m_pickup->m_rotation);
        break;
    case 2: {
        boost::shared_ptr<CharInfo> carrier = m_carrier.lock();
        if (carrier.get() != 0) {
            carrier->PushCommand(new ReadyCommand(m_pickup));
        }
        m_state = 3;
        break;
    }
    case 3:
        UpdateCarried();
        break;
    case 4:
        SetPosition(m_pickup->m_position, m_pickup->m_rotation);
        break;
    case 5:
        BombBlast::Instantiate(m_pickup->m_position);
        if (boost::shared_ptr<CharInfo> attacker = m_attacker.lock()) {
            fn_8020642C(attacker, 0x11);
        }
        m_flags.Set(2);
        break;
    case 6:
        if (boost::shared_ptr<CharInfo> attacker = m_attacker.lock()) {
            fn_8020642C(attacker, 0x11);
        }
        m_flags.Set(2);
        break;
    }
    fn_801E413C(m_pickup.get(), m_pickup.get());
}

bool Bomb::IsDone(GadgetPickup::PickupFlags *request) {
    if (request->Test(0) && m_flags.Test(1)) {
        return true;
    }
    if (request->Test(1) && m_flags.Test(2)) {
        return true;
    }
    return false;
}

void Bomb::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    if (m_pickup.get() != 0) {
        fn_800C9F14(owner, &m_pickup->m_position);
        fn_800C9ED4(owner, &m_pickup->m_rotation);
        fn_800C9E50(owner, &m_switchId);
        m_pickup->m_position.y += 10.0f;
    }
    m_state = 0;
    SetPosition(m_pickup->m_position, m_pickup->m_rotation);
}

void Bomb::SetPlacement(const Vec3f &position, const Vec3f &rotation) {
    if (m_pickup.get() != 0) {
        fn_800091D0(&m_pickup->m_position, &position);
        CopyRotation(m_pickup->m_rotation, rotation);
        m_pickup->m_position.y += 10.0f;
    }
    m_state = 0;
    m_switchId = 0;
    SetPosition(m_pickup->m_position, m_pickup->m_rotation);
}

void Bomb::SetPosition(const Vec3f &position, const Vec3f &rotation) {
    PickupBase::SetPosition(position, rotation);
    if (m_model.get() != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &position);
        fn_8040B848(&matrix, rotation.y);
        fn_8040B884(&matrix, rotation.x);
        fn_8040B80C(&matrix, rotation.z);
        m_model->fn_801633DC(&matrix);
    }
}

void Bomb::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0x11:
        if (m_state == 1 && !m_flags.Test(1)) {
            m_carrier = static_cast<PickupCommand &>(command).m_character;
            m_pickup->m_target->TurnOff();
            fn_80161230(m_model.get(), 0, -1);
            m_behavior.reset();
            m_state = 2;
            m_flags.Set(0);
            m_flags.Set(1);
        }
        break;
    case 0x13:
        if (m_state == 3) {
            ThrowCommand &throwCommand = static_cast<ThrowCommand &>(command);
            m_attacker = throwCommand.m_character;
            AssignVector(m_direction, throwCommand.m_direction);
            m_speed = -320.0f;
            fn_8040CE60(&m_pickup->field_0x24);
            m_pickup->m_position.y += 10.0f;
            fn_800494F8(&m_pickup->m_rotation, &m_handMatrix);
            fn_800091D0(&m_pickup->field_0x18,
                        &(const Vec3f &)fn_8040B618(&m_handMatrix,
                                                            &Vec3f(0.0f, 100.0f, -150.0f)));
            m_state = 4;
            m_flags.Set(3);
            m_flags.Set(0);
            m_flags.Set(4);
        }
        break;
    case 0x12:
        if (m_state == 3) {
            AssignVector(m_direction, static_cast<CastCommand &>(command).m_direction);
            m_speed = -320.0f;
            fn_8040CE60(&m_pickup->field_0x24);
            m_pickup->m_position.y += 10.0f;
            fn_800091D0(&m_pickup->field_0x18,
                        &(const Vec3f &)fn_8040B618(&m_handMatrix,
                                                            &Vec3f(0.0f, 100.0f, 0.0f)));
            m_state = 4;
            m_flags.Set(3);
            m_flags.Set(0);
            m_flags.Set(4);
        }
        break;
    case 0x14:
        if (m_state == 1) {
            m_model->fn_80161880();
            fn_8033F0F8(&command, &m_model, 0, this, &m_rotation);
            m_model.reset();
            m_state = 6;
        }
        break;
    }
    command.field_0x4[3] |= 2;
}

void Bomb::OnSetGrabPosition(PickupCharInfo::SetGrabPositionArgs &args) {
    fn_8040C148(&m_handMatrix, args.m_matrix);
    if (m_state == 3) {
        UpdateCarried();
    }
}

void Bomb::UpdateCarried() {
    fn_800091D0(&m_pickup->m_position, &(const Vec3f &)fn_800496F8(&m_handMatrix));
    PickupBase::SetPosition(m_pickup->m_position, m_pickup->m_rotation);
    Mtx34f matrix;
    fn_8040C148(&matrix, &m_handMatrix);
    fn_8040B52C(&matrix, 0.0f, 10.0f, 0.0f);
    m_model->fn_801633DC(&matrix);
}

void Bomb::OnAttack(AttackCallbackParam &param) {
    if (m_state == 1) {
        if (boost::shared_ptr<CharaColliAttack> collision = param.m_attacker.lock()) {
            if (collision->m_flags & 2) {
                param.m_result = 5;
                return;
            }
            m_attacker = collision->m_owner;
            m_state = 5;
            param.m_result = 3;
            return;
        }
    }
    param.m_result = 5;
}

void Bomb::UpdateFlight(float dt) {
    if (m_flags.Test(3)) {
        AddVector(m_pickup->field_0x24, fn_800091A4(&m_direction, m_speed));
    }
    fn_804103E8(&m_pickup->m_position, &m_pickup->field_0x18, &m_pickup->field_0x24, 1.0f, dt);
    if (m_flags.Test(4)) {
        Vec3f delta;
        fn_800091EC(&delta, &Subtract(&Vec3f(), &m_pickup.px->m_position, &m_lastPosition));
        Vec3f start;
        fn_800091EC(&start, &m_lastPosition);
        fn_80021B08();
        if (fn_801BFC28(0, &start, &delta, &m_hitFlags.m_bits, 0, 10.0f)) {
            fn_800091D0(&m_pickup->m_position, &start);
            AssignVector(m_pickup.px->field_0x18, fn_8040CDA8(&delta, 1.0f / dt));
            m_state = 5;
        }
    }
    fn_800091D0(&m_lastPosition, &m_pickup->m_position);
    fn_8040CE60(&m_pickup->field_0x24);
}

BombBlast::BombBlast(Task *parent, const Vec3f &position) : Task(parent) {
    m_name = s_taskName;
    AssignVector(m_position, position);
    boost::shared_ptr<CharaColliBase> collider = fn_800C7EA8(
        fn_80021B08(), &m_position, 0x40000, 0xF, 0, lbl_805EE698, &lbl_805E07A0, 1.0f);
    m_handle = fn_8015B8C4(collider);
    fn_8015D7DC(&m_position, Vec3f(0.0f, 1.0f, 0.0f), 0x19);
    m_state = 0;
    m_timer = 0.0f;
    m_radius = 0.0f;
}

BombBlast::~BombBlast() {}

void BombBlast::Tick(float dt) {
    switch (m_state) {
    case 0:
        m_timer = 0.0f;
        m_state = 1;
        m_radius = 5.0f;
        fn_801D54F4(fn_80053164(), 0x1008, &m_position, 0, 1);
        break;
    case 1:
        m_timer += dt;
        if (m_timer >= 0.13f) {
            m_state = 2;
            m_timer = 0.0f;
            if (m_handle.get() != 0) {
                boost::weak_ptr<CharaColliAttack> attack =
                    fn_800C4374(fn_80021B08(), m_handle->m_collider);
                if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
                    collision->field_0x54 = 1;
                    collision->m_kind = 3;
                    collision->m_power = 4.0f;
                    collision->m_flags = 0x200;
                    collision->m_hitCallback =
                        AttackCallback(boost::bind(&BombBlast::OnAttack, this, _1));
                }
                m_handle->m_collider->SetSize(m_radius, 0.0f, 0.0f, -2);
            }
        }
        break;
    case 2:
        m_timer += dt;
        if (m_timer >= 0.87f) {
            m_timer = 0.87f;
            m_state = 3;
        }
        m_radius = 51.724136f * m_timer + 5.0f;
        if (m_radius >= 50.0f) {
            m_radius = 50.0f;
        }
        if (m_handle.get() != 0) {
            m_handle->m_collider->SetSize(m_radius, 0.0f, 0.0f, -2);
        }
        break;
    case 3:
        m_flags |= 1;
        break;
    }
    if (m_handle.get() != 0) {
        fn_8015BE9C(m_handle.get(), dt);
    }
}

void BombBlast::OnAttack(AttackCallbackParam &param) {}

extern "C" void fn_800EDE08(void) {
    fn_8006B474("BOMB.BIN", 1);
}

extern "C" void fn_800EDE04(void) {}

void Bomb::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    boost::shared_ptr<GadgetPickup::PickupBase> pickup =
        boost::shared_ptr<GadgetPickup::PickupBase>(new Bomb());
    pickup->Init(owner);
    pickup->ReloadPlacement(owner);
    new GadgetPickup::SetPickupBaseTask(parent, owner, pickup);
}

void Bomb::Spawn(const Vec3f &position, const Vec3f &rotation,
                 const boost::weak_ptr<CharInfo> &owner, float range) {
    boost::shared_ptr<GadgetPickup::PickupBase> pickup =
        boost::shared_ptr<GadgetPickup::PickupBase>(new Bomb(owner));
    pickup->Init(0);
    pickup->SetPlacement(position, rotation);
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    GadgetPickup::PickupBaseTask *task = new GadgetPickup::PickupBaseTask(parent, pickup);
    task->SetRange(range);
}

void BombBlast::Instantiate(const Vec3f &position) {
    new BombBlast(fn_801EB304(fn_8001C4F8(), 11), position);
}

} // namespace Bomb

GadgetClassDesc lbl_80524C78 = {
    "BOMB", Bomb::fn_800EDE08, Bomb::fn_800EDE04, Bomb::Bomb::Instantiate, 0, 0, 0xD, 1, 0, 10, 0,
};
