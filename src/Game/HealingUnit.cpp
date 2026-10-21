/* HealingUnit::HealingUnit (class name from RTTI): the HEALING_UNIT gadget, a healing capsule
 * (HEALINGUNIT.BIN) a character can pick up, carry and throw or drop. Like the BOMB gadget it is a
 * pickup object (GadgetPickup::PickupBase) with a PickupCharInfo of its own: carried, it follows the
 * carrier's hand matrix; thrown or dropped, it flies until it hits something, and then leaves a
 * HealingUnit::HealingBlast (class name from RTTI), a task whose collision sends every character
 * it touches a RecoverCommand for a moment. Game code can also spawn healing units (Spawn). File
 * name inferred from the class. Names marked "(name inferred)" are not original. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Effect/System/EffectElement.h"
#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetColliDetectArgs.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetPickup.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/PickupCharInfo.h"
#include "Game/Task.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"
#include "Game/GadgetCharCollision.h"

using SonicteamUSA::System::CharaColli::CharaColliBase;

/* Target (class name from RTTI): a point characters aim at. Only the flag word is mapped here
 * (bit 0 makes the target active; names inferred, as in Bomb.cpp). */
class Target {
  public:
    void TurnOff() {
        m_flags &= ~1;
    }

    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ u32 m_flags;
};

extern "C" {
/* Sets a character info's name (behaviour inferred). */
void fn_801DC984(CharInfo *charInfo, const char *name);
/* Creates the pickup's target for owner with the given flags and radius, and moves the target to
 * a position each frame (behaviours inferred). */
void fn_801E4160(PickupCharInfo *charInfo, const boost::weak_ptr<CharInfo> &owner, int flags,
                 float radius);
void fn_801E413C(PickupCharInfo *charInfo, const Vec3f *position);
/* Loads the model fileName (behaviour inferred; arg0 unknown). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* Attaches a character info to the model's collision (behaviour inferred). */
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
/* Turns the model's collision off (behaviour inferred; the arguments are unknown). */
void fn_80161230(GadgetModelInfo *model, int index, int arg2);
/* Sends message to the character (behaviour and meaning of message inferred). */
int fn_8020642C(const boost::shared_ptr<CharInfo> &character, int message);
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
/* Wraps collider in a new handle, and advances the handle by dt (behaviour inferred). */
boost::shared_ptr<GadgetCharCollision> fn_8015B8C4(boost::shared_ptr<CharaColliBase> collider);
void fn_8015BE9C(GadgetCharCollision *handle, float dt);
/* Calls callback with every character in the collision event (behaviour inferred). */
void fn_8015B384(ColliDetectArgs &args,
                 void (*callback)(boost::shared_ptr<CharInfo> hit, void *user), void *user);
/* Starts effect id at position, facing up (behaviour inferred). */
boost::weak_ptr<Effect::System::EffectElement> fn_8015D7DC(const Vec3f *position,
                                                           const Vec3f &up, int id);
}

/* The empty character info (an .sbss shared_ptr; meaning inferred). */
extern boost::shared_ptr<CharInfo> lbl_805EE698;

/* vector += value (name inferred). */
inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

/* result = a - b (name inferred), returned through the caller's temporary. */
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

namespace HealingUnit {

/* What the blast hands its collision callback (names inferred). */
struct HealData {
    HealData() : m_amount(0.0f) {}

    /* 0x0 */ boost::weak_ptr<CharInfo> m_owner;
    /* 0x8 */ float m_amount;
};

/* The blast a healing unit leaves (class name from RTTI; names inferred). */
class HealingBlast : public Task {
  public:
    HealingBlast(Task *parent, const boost::weak_ptr<CharInfo> &owner,
                 const Vec3f &position, int kind, float amount);
    virtual ~HealingBlast();
    virtual void Tick(float dt);

    /* Creates the blast under task layer 11 (name inferred). */
    static void Instantiate(const boost::weak_ptr<CharInfo> &owner, const Vec3f &position,
                       int kind);

    /* The collision callback (name inferred). */
    void OnColliDetect(ColliDetectArgs &args);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x28 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x30 */ boost::shared_ptr<GadgetCharCollision> m_handle;
    /* 0x38 */ boost::weak_ptr<CharInfo> m_owner;
    /* 0x40 */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
    /* 0x48 */ s32 m_state; /* 0 new, 2 active, 3 done */
    /* 0x4C */ float m_amount;
    /* 0x50 */ float m_timer;
    /* 0x54 */ u32 field_0x54;
};
SIZE_ASSERT(HealingBlast, 0x58);

class HealingUnit : public GadgetPickup::PickupBase {
  public:
    HealingUnit();
    HealingUnit(const boost::weak_ptr<CharInfo> &owner);
    virtual ~HealingUnit();
    virtual void SetPosition(const Vec3f &position, const Vec3f &rotation);
    virtual void Init(UnkPlacement_800C9CD0 *owner);
    virtual void Tick(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);
    virtual void SetPlacement(const Vec3f &position, const Vec3f &rotation);
    virtual bool IsDone(GadgetPickup::PickupFlags *request);

    /* Creates a healing unit from an owner (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);
    /* Spawns a healing unit at position, with or without an owner, finished once out of range
     * (names inferred). */
    static void Spawn(const Vec3f &position, const Vec3f &rotation, float range);
    static void Spawn(const Vec3f &position, const Vec3f &rotation,
                      const boost::weak_ptr<CharInfo> &owner, float range);
    /* Places a spawned pickup and starts its task (name inferred). */
    static void StartTask(boost::shared_ptr<GadgetPickup::PickupBase> &pickup,
                          const Vec3f &position, const Vec3f &rotation,
                          float range);

    /* Method names are inferred. */
    void UpdateFlight(float dt);
    void UpdateCarried();
    void OnSetGrabPosition(PickupCharInfo::SetGrabPositionArgs &args);
    void OnCommand(CharCommand &command);

    /* Member names are inferred. */
    /* 0x6C */ u32 field_0x6C;
    /* 0x70 */ s32 m_state; /* 0 new, 1 resting, 2 picked up, 3 carried, 4 flying, 5 bursting,
                               6 vacuumed */
    /* 0x74 */ boost::shared_ptr<PickupCharInfo> m_pickup;
    /* 0x7C */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x84 */ boost::weak_ptr<CharInfo> m_carrier;
    /* 0x8C */ float m_speed;
    /* 0x90 */ Vec3f m_direction;
    /* 0x9C */ Vec3f m_lastPosition;
    /* 0xA8 */ Mtx34f m_handMatrix;
    /* 0xD8 */ boost::weak_ptr<CharInfo> m_owner;
    /* 0xE0 */ GadgetPickup::PickupFlags m_flags; /* 0 flying, 1 picked up, 2 done, 3 thrown, 4 hit
                                                     test */
    /* 0xE4 */ GadgetPickup::PickupFlags m_hitFlags;
};
SIZE_ASSERT(HealingUnit, 0xE8);

const char *HealingBlast::s_taskName = "HealingBlast";

HealingUnit::HealingUnit() : PickupBase(0x4E) {
    fn_801DC984(this, "HealingUnit");
}

HealingUnit::HealingUnit(const boost::weak_ptr<CharInfo> &owner)
    : PickupBase(0x4E), m_owner(owner) {
    fn_801DC984(this, "HealingUnit");
}

HealingUnit::~HealingUnit() {
    if (!m_owner.expired()) {
        boost::shared_ptr<CharInfo> owner = m_owner.lock();
        if (owner.get() != 0) {
            owner->PushCommand(new UseCommand(m_pickup));
        }
    }
}

void HealingUnit::Init(UnkPlacement_800C9CD0 *owner) {
    m_flags.Clear();
    m_hitFlags.Set(1);
    m_pickup = boost::shared_ptr<PickupCharInfo>(new PickupCharInfo(0x4E));
    fn_800729B0(&m_pickup->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&HealingUnit::OnCommand, this, _1)));
    fn_801E4160(m_pickup.get(), m_pickup, 0x80, 30.0f);
    fn_801DC984(m_pickup.get(), "HealingUnit");
    m_pickup->m_grabCallback = boost::function1<void, PickupCharInfo::SetGrabPositionArgs &>(
        boost::bind(&HealingUnit::OnSetGrabPosition, this, _1));
    m_model = fn_8015F800(0, "HEALINGUNIT.BIN");
    m_model->fn_80162A68(0, 0);
    fn_80161FC8(m_model.get(), 0, m_pickup);
}

void HealingUnit::Tick(float dt) {
    if (m_flags.Test(0)) {
        UpdateFlight(dt);
    }
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }

    switch (m_state) {
    case 0:
        m_state = 1;
        break;
    case 1:
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
        HealingBlast::Instantiate(m_carrier, m_pickup->m_position, 0);
        if (boost::shared_ptr<CharInfo> carrier = m_carrier.lock()) {
            fn_8020642C(carrier, 0x20);
        }
        m_flags.Set(2);
        break;
    case 6:
        m_flags.Set(2);
        break;
    }
    Vec3f position;
    fn_800091EC(&position, &m_pickup->m_position);
    position.y += 8.0f;
    fn_801E413C(m_pickup.get(), &position);
}

bool HealingUnit::IsDone(GadgetPickup::PickupFlags *request) {
    if (request->Test(0) && m_flags.Test(1)) {
        return true;
    }
    if (request->Test(1) && m_flags.Test(2)) {
        return true;
    }
    return false;
}

void HealingUnit::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    if (m_pickup.get() != 0) {
        fn_800C9F14(owner, &m_pickup->m_position);
        fn_800C9ED4(owner, &m_pickup->m_rotation);
    }
    m_state = 0;
    SetPosition(m_pickup->m_position, m_pickup->m_rotation);
}

void HealingUnit::SetPlacement(const Vec3f &position, const Vec3f &rotation) {
    if (m_pickup.get() != 0) {
        fn_800091D0(&m_pickup->m_position, &position);
        CopyRotation(m_pickup->m_rotation, rotation);
    }
    m_state = 0;
    SetPosition(m_pickup->m_position, m_pickup->m_rotation);
}

void HealingUnit::SetPosition(const Vec3f &position, const Vec3f &rotation) {
    PickupBase::SetPosition(position, rotation);
    if (m_model.get() != 0) {
        Mtx34f matrix;

        fn_8040B6F8(&matrix);
        fn_8040B59C(&matrix, &position);
        fn_8040B848(&matrix, rotation.y);
        m_model->fn_801633DC(&matrix);
    }
}

void HealingUnit::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0x11:
        if (m_state == 1 && !m_flags.Test(1)) {
            m_carrier = static_cast<PickupCommand &>(command).m_character;
            m_pickup->m_target->TurnOff();
            fn_80161230(m_model.get(), 0, -1);
            m_state = 2;
            m_flags.Set(1);
            m_flags.Set(0);
        }
        break;
    case 0x13:
        if (m_state == 3) {
            AssignVector(m_direction, static_cast<ThrowCommand &>(command).m_direction);
            m_speed = -320.0f;
            fn_8040CE60(&m_pickup->field_0x24);
            fn_800494F8(&m_pickup->m_rotation, &m_handMatrix);
            fn_800091D0(&m_pickup->field_0x18,
                        &(const Vec3f &)fn_8040B618(&m_handMatrix,
                                                            &Vec3f(0.0f, 100.0f, -80.0f)));
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
            fn_8033F0F8(&command, &m_model, 1, this, &m_rotation);
            m_model.reset();
            m_state = 6;
        }
        break;
    }
    command.field_0x4[3] |= 2;
}

void HealingUnit::OnSetGrabPosition(PickupCharInfo::SetGrabPositionArgs &args) {
    fn_8040C148(&m_handMatrix, args.m_matrix);
    if (m_state == 3) {
        UpdateCarried();
    }
}

void HealingUnit::UpdateCarried() {
    fn_800091D0(&m_pickup->m_position, &(const Vec3f &)fn_800496F8(&m_handMatrix));
    m_model->fn_801633DC(&m_handMatrix);
    PickupBase::SetPosition(m_pickup->m_position, m_pickup->m_rotation);
}

void HealingUnit::UpdateFlight(float dt) {
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

/* Heals a character the blast touches (name inferred). */
static void HealCharacter(boost::shared_ptr<CharInfo> hit, void *user) {
    HealData *data = static_cast<HealData *>(user);

    if (data != 0) {
        hit->PushCommand(new RecoverCommand(data->m_owner, data->m_amount));
    }
}

HealingBlast::HealingBlast(Task *parent, const boost::weak_ptr<CharInfo> &owner,
                           const Vec3f &position, int kind, float amount)
    : Task(parent) {
    m_name = s_taskName;
    m_chara = fn_8015CB88(0x4E, 4, "HealingBlast");
    fn_800091D0(&m_chara->m_position, &position);

    float radius = 0.0f;
    int effect = 0;
    switch (kind) {
    case 0:
        radius = 20.0f;
        effect = 0xC5;
        break;
    case 1:
        radius = 40.0f;
        effect = 0x2DA;
        break;
    }

    boost::shared_ptr<CharaColliBase> collider =
        fn_800C7EA8(fn_80021B08(), &m_chara->m_position, 0x40000, 3, 0, lbl_805EE698,
                    &lbl_805E07A0, radius);
    m_handle = fn_8015B8C4(collider);
    fn_800F5980(m_handle.get(), boost::bind(&HealingBlast::OnColliDetect, this, _1));
    Vec3f up(0.0f, 1.0f, 0.0f);
    m_effect = fn_8015D7DC(&m_chara->m_position, up, effect);
    m_amount = amount;
    m_state = 0;
    m_timer = 1.0f;
    m_owner = owner;
}

HealingBlast::~HealingBlast() {
    if (!m_effect.expired()) {
        boost::shared_ptr<Effect::System::EffectElement> effect = m_effect.lock();
        effect->Retire();
    }
}

void HealingBlast::Tick(float dt) {
    switch (m_state) {
    case 0:
        m_state = 2;
        break;
    case 2:
        m_timer -= dt;
        if (m_timer <= 0.0f) {
            m_state = 3;
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

void HealingBlast::OnColliDetect(ColliDetectArgs &args) {
    HealData data;
    data.m_owner = m_owner;
    data.m_amount = m_amount;
    fn_8015B384(args, HealCharacter, &data);
    m_handle->TurnOff();
}

extern "C" void fn_801074E0(void) {
    fn_8006B474("HEALINGUNIT.BIN", 1);
}

extern "C" void fn_801074DC(void) {}

void HealingUnit::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    boost::shared_ptr<GadgetPickup::PickupBase> pickup =
        boost::shared_ptr<GadgetPickup::PickupBase>(new HealingUnit());
    pickup->Init(owner);
    pickup->ReloadPlacement(owner);
    new GadgetPickup::SetPickupBaseTask(parent, owner, pickup);
}

void HealingUnit::StartTask(boost::shared_ptr<GadgetPickup::PickupBase> &pickup,
                            const Vec3f &position, const Vec3f &rotation,
                            float range) {
    pickup->Init(0);
    pickup->SetPlacement(position, rotation);
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    GadgetPickup::PickupBaseTask *task = new GadgetPickup::PickupBaseTask(parent, pickup);
    task->SetRange(range);
}

void HealingUnit::Spawn(const Vec3f &position, const Vec3f &rotation,
                        const boost::weak_ptr<CharInfo> &owner, float range) {
    boost::shared_ptr<GadgetPickup::PickupBase> pickup =
        boost::shared_ptr<GadgetPickup::PickupBase>(new HealingUnit(owner));
    StartTask(pickup, position, rotation, range);
}

void HealingUnit::Spawn(const Vec3f &position, const Vec3f &rotation,
                        float range) {
    boost::shared_ptr<GadgetPickup::PickupBase> pickup =
        boost::shared_ptr<GadgetPickup::PickupBase>(new HealingUnit());
    StartTask(pickup, position, rotation, range);
}

void HealingBlast::Instantiate(const boost::weak_ptr<CharInfo> &owner, const Vec3f &position,
                          int kind) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new HealingBlast(parent, owner, position, kind, 1.0f);
}

} // namespace HealingUnit

GadgetClassDesc lbl_80526234 = {
    "HEALING_UNIT", HealingUnit::fn_801074E0, HealingUnit::fn_801074DC,
    HealingUnit::HealingUnit::Instantiate, 0, 0, 0x38, 1, 0, 10, 0,
};
