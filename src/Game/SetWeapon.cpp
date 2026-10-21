/* SetWeapon (classes SetWeapon::SetWeaponTask, SetWeapon::Arms and SetWeapon::WeaponTask from
 * RTTI): the SET_WEAPON gadget, a weapon lying in the stage. Arms is the pick-up: a character info
 * with a model and the weapon's Weapon::Item; a character that uses it (command 0xD) receives an
 * AddWeaponCommand for the weapon. SetWeaponTask places one for the weapon type of its "weapon"
 * parameter. fn_8011E938 and fn_8011E630 drop a weapon at run time (on the ground below a point,
 * or at a point facing a direction); WeaponTask then keeps it for 30 seconds, blinking for the last
 * 3. File name inferred from the namespace. Names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <cstring>

#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharCommands.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/WeaponItem.h"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::CharaColliBase;

/* boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out of line; this unit
 * emits its own copy (0x8011EE78), so it defines it here (boost 1.32's definition). Inline, as in
 * the class body of boost 1.32, so that it is emitted with its first user (ReloadPlacement) rather
 * than at the end of the unit; it still stays out of line. */
namespace boost {
template<class T> inline void shared_ptr<T>::reset() {
    this_type().swap(*this);
}
} // namespace boost

/* The cursor handed to a parameter's label callback (layout from fn_800CBC20 and fn_8011EB88;
 * names inferred). */
class UnkLabelQuery_800CBC20 {
  public:
    /* 0x0 */ const GadgetParamDesc *params;
    /* 0x4 */ s32 index; /* the parameter being shown */
    /* 0x8 */ s32 value; /* its current value */
};

/* A word of flag bits; the functions below construct it, clear all bits, and set, test and clear
 * one bit (behaviour inferred from their bodies). */
struct UnkFlags_80014E04 {
    /* 0x0 */ u32 m_bits;
};

/* What the ground probe below a dropped weapon hits (class name unknown; only the members used
 * here are named, names inferred). */
struct UnkCollision_80021E24 {
    /* 0x00 */ u8 field_0x0[0xC];
    /* 0x0C */ u32 m_flags;
    /* 0x10 */ u32 field_0x10;
    /* 0x14 */ void *m_object;
};

extern "C" {
void fn_80014E04(UnkFlags_80014E04 *flags);
void fn_80014E10(UnkFlags_80014E04 *flags);
void fn_80014E1C(UnkFlags_80014E04 *flags, int bit);
bool fn_80014E34(const UnkFlags_80014E04 *flags, int bit);

/* Weapon types and items (behaviour inferred): fn_8005BF14 reports whether a type exists,
 * fn_8005BEEC returns its name and fn_8005B898 its pick-up size; fn_8005F09C makes an item blink
 * and fn_8005F274 places it. */
bool fn_8005BF14(int type);
const char *fn_8005BEEC(int type);
Vec2f fn_8005B898(int type);
void fn_8005F09C(Weapon::Item *item);
void fn_8005F274(Weapon::Item *item, const Vec3f *position, const Vec3f *rotation);

/* The rotation angles about x and z that face along direction (behaviour inferred). */
void fn_8003B8F4(const Vec3f *direction, float *x, float *z);

/* The collision world, a probe along segment returning the first object for which filter is
 * true, the hit object's position, and releasing the hit (behaviour inferred). */
void *fn_80020D40(void);
UnkCollision_80021E24 *fn_80021E24(void *world, void *segment, int arg2, int arg3,
                                   bool (*filter)(UnkCollision_80021E24 *hit), int arg5);
const Vec3f *fn_8001E9BC(void *object);
void fn_800265A4(UnkCollision_80021E24 *hit, int arg1);

/* Model helpers (behaviour inferred): fn_80161FC8 attaches a character info to a model variant's
 * collision, fn_80161614 returns that collision and fn_80161A24 hides the model. */
bool fn_80161FC8(GadgetModelInfo *model, int index, const boost::weak_ptr<CharInfo> &info);
boost::weak_ptr<CharaColliBase> fn_80161614(GadgetModelInfo *model, int index);
void fn_80161A24(GadgetModelInfo *model, int arg1);

/* Reports whether position is out of range (meaning inferred). */
bool fn_80169BE4(const Vec3f *position, float distanceSquared);

/* Tells the owner that the gadget is done, in two ways (behaviour inferred). */
void fn_800C9B34(UnkPlacement_800C9CD0 *owner);
void fn_8015A794(Gadget *gadget);
}

/* Copies a rotation one component at a time (name inferred; Game/EnergyCore.cpp has the same
 * helper). The source is not const: the original reloads each component after the previous store,
 * which a const source would let the scheduler hoist. */
inline void CopyRotation(Vec3f &vector, Vec3f &value) {
    vector.x = value.x;
    vector.y = value.y;
    vector.z = value.z;
}

namespace SetWeapon {

/* The weapon pick-up (class name from RTTI). Flag bit 0: picked up; bit 1: do not place the item
 * (names inferred). */
class Arms {
  public:
    Arms(const boost::shared_ptr<Weapon::Item> &item, u32 flag);
    ~Arms();

    /* Updates the model and the item (name inferred). */
    void Tick(float dt);
    /* True once the pick-up is out of range (name inferred). */
    bool IsOutOfRange(float distanceSquared);
    /* Moves the pick-up (name inferred). */
    void SetTransform(const Vec3f *position, Vec3f *rotation);
    /* Places the model and the item at the character info (name inferred). */
    void UpdateModel();
    /* The character info's command callback (name inferred). */
    void OnCommand(CharCommand &command);
    /* Starts the item's blinking (name inferred). */
    void StartBlink();

    bool IsPickedUp() const {
        return fn_80014E34(&m_flags, 0);
    }

    /* 0x00 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x08 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x10 */ boost::shared_ptr<Weapon::Item> m_item;
    /* 0x18 */ UnkFlags_80014E04 m_flags;
};
SIZE_ASSERT(Arms, 0x1C);

/* Deletes a pick-up (name inferred; see checked_delete below). */
inline void DeleteArms(Arms *arms) {
    delete arms;
}

} // namespace SetWeapon

/* Retail keeps boost::checked_delete<Arms> out of line (0x8011E8E0) and ~Arms at its own place
 * (0x8011F68C). GC/2.7 only keeps checked_delete out of line when its body calls an inline
 * function, and emits an inline ~Arms right after it (see docs/research-notes.md); this
 * specialization calls an inline helper instead, which gives retail's code and order (as in
 * Rocket.cpp). */
namespace boost {
template<> inline void checked_delete<SetWeapon::Arms>(SetWeapon::Arms *x) {
    SetWeapon::DeleteArms(x);
}
} // namespace boost

namespace SetWeapon {

/* Keeps a dropped weapon for a while (class name from RTTI). Flag bit 2: finished; bit 3: the time
 * is nearly up; bit 4: blinking started (names inferred). */
class WeaponTask : public Task {
  public:
    WeaponTask(Task *parent, boost::shared_ptr<Arms> arms);
    virtual ~WeaponTask();
    virtual void Tick(float dt);

    static const char *s_taskName;

    /* 0x28 */ boost::shared_ptr<Arms> m_arms;
    /* 0x30 */ float m_time; /* seconds left */
    /* 0x34 */ UnkFlags_80014E04 m_flags;
};
SIZE_ASSERT(WeaponTask, 0x38);

class SetWeaponTask : public Gadget {
  public:
    SetWeaponTask(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~SetWeaponTask();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position, rotation and weapon type (name inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    /* Moves the pick-up to the character info (name inferred). */
    void PlaceArms();

    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<Arms> m_arms;
    /* 0x38 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x40 */ s32 m_param; /* the "weapon" parameter */
    /* 0x44 */ s32 m_weapon;
};
SIZE_ASSERT(SetWeaponTask, 0x48);

Arms::Arms(const boost::shared_ptr<Weapon::Item> &item, u32 flag) {
    fn_80014E04(&m_flags);
    m_chara = fn_8015CD90(0x34, 4);
    m_chara->m_commandUnit = boost::shared_ptr<CharCommandUnit>(new CharCommandUnit);
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&Arms::OnCommand, this, _1)));
    m_model = fn_8015F9D8(0, 0xDF);
    m_model->fn_80162A68(0, 0);
    fn_80161FC8(m_model.get(), 0, m_chara);
    fn_80014E10(&m_flags);
    m_item = item;
    if (flag != 0) {
        fn_80014E1C(&m_flags, flag);
    }
    if (m_item.get() != 0) {
        Vec2f size = fn_8005B898(m_item->TypeId());
        Vec3f offset(0.0f, 5.0f, 0.0f);
        boost::weak_ptr<CharaColliBase> collider = fn_80161614(m_model.get(), 0);
        if (boost::shared_ptr<CharaColliBase> locked = collider.lock()) {
            locked->SetSize(size.x, 0.0f, size.y, -2);
            locked->SetOffset(&offset, -2);
        }
        m_chara->field_0x34 = m_item->m_type;
    }
}

Arms::~Arms() {}

void Arms::Tick(float dt) {
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }
    if (m_item.get() != 0) {
        m_item->Tick(dt);
    }
}

bool Arms::IsOutOfRange(float distanceSquared) {
    return fn_80169BE4(&m_chara->m_position, distanceSquared);
}

void Arms::SetTransform(const Vec3f *position, Vec3f *rotation) {
    fn_800091D0(&m_chara->m_position, position);
    CopyRotation(m_chara->m_rotation, *rotation);
    UpdateModel();
}

void Arms::UpdateModel() {
    if (m_model.get() != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B884(&matrix, m_chara->m_rotation.x);
        fn_8040B80C(&matrix, m_chara->m_rotation.z);
        m_model->fn_801633DC(&matrix);
    }
    if (m_item.get() != 0 && !fn_80014E34(&m_flags, 1)) {
        Vec3f position;

        fn_800091EC(&position, &m_chara->m_position);
        position.y += 5.0f;
        fn_8005F274(m_item.get(), &position, &m_chara->m_rotation);
    }
}

void Arms::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0xD: /* UseCommand */
        if (!fn_80014E34(&m_flags, 0) && m_item.get() != 0) {
            if (boost::shared_ptr<CharInfo> user =
                    static_cast<UseCommand &>(command).m_user.lock()) {
                user->PushCommand(
                    new Player::AddWeaponCommand(m_item->m_type, m_item->m_ammo, 4));
                if (m_model.get() != 0) {
                    fn_80161A24(m_model.get(), 0);
                }
                fn_80014E1C(&m_flags, 0);
            }
        }
        break;
    }
}

void Arms::StartBlink() {
    if (m_item.get() != 0) {
        fn_8005F09C(m_item.get());
    }
}

const char *WeaponTask::s_taskName = "WeaponTask";

WeaponTask::WeaponTask(Task *parent, boost::shared_ptr<Arms> arms) : Task(parent) {
    fn_80014E04(&m_flags);
    m_name = s_taskName;
    m_arms = arms;
    m_time = 30.0f;
    fn_80014E10(&m_flags);
}

WeaponTask::~WeaponTask() {}

void WeaponTask::Tick(float dt) {
    m_time -= dt;
    if (m_time <= 0.0f) {
        fn_80014E1C(&m_flags, 2);
    } else if (m_time <= 3.0f) {
        fn_80014E1C(&m_flags, 3);
    }
    if (!fn_80014E34(&m_flags, 4) && fn_80014E34(&m_flags, 3)) {
        m_arms->StartBlink();
        fn_80014E1C(&m_flags, 4);
    }
    if (m_arms.get() != 0) {
        if (m_arms->IsOutOfRange(1000000.0f)) {
            fn_80014E1C(&m_flags, 2);
        } else if (m_arms->IsPickedUp()) {
            fn_80014E1C(&m_flags, 2);
        }
    }
    if (fn_80014E34(&m_flags, 2)) {
        Task::m_flags |= 1;
    } else {
        m_arms->Tick(dt);
    }
}

const char *SetWeaponTask::s_taskName = "SetWeaponTask";

SetWeaponTask::SetWeaponTask(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner), m_param(1) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x34, 4);
    LoadFromOwner(owner);
    boost::shared_ptr<Weapon::Item> item = fn_8005B9D4(m_weapon, -1);
    if (item.get() != 0) {
        m_arms = boost::shared_ptr<Arms>(new Arms(item, 0));
        PlaceArms();
    } else {
        fn_8015A794(this);
        m_flags |= 1;
    }
}

SetWeaponTask::~SetWeaponTask() {}

void SetWeaponTask::Behave(float dt) {
    if (m_arms.get() != 0) {
        if (m_arms->IsPickedUp()) {
            if (m_owner != 0) {
                fn_800C9B34(m_owner);
            }
            m_flags |= 1;
        } else {
            m_arms->Tick(dt);
        }
    }
}

void SetWeaponTask::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    s32 previous = m_weapon;

    LoadFromOwner(owner);
    PlaceArms();
    if (previous != m_weapon && fn_8005BF14(m_weapon)) {
        const char *name = fn_8005BEEC(m_weapon);
        if (name != 0 && strcmp(name, "") != 0) {
            m_arms.reset();
            m_arms = boost::shared_ptr<Arms>(new Arms(fn_8005B9D4(m_weapon, -1), 0));
        }
    }
}

void SetWeaponTask::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_param, sizeof(m_param));
    m_weapon = m_param;
    m_chara->field_0x34 = m_param;
}

void SetWeaponTask::PlaceArms() {
    if (m_arms.get() != 0) {
        m_arms->SetTransform(&m_chara->m_position, &m_chara->m_rotation);
    }
}

} // namespace SetWeapon

extern "C" void fn_8011EC80(void) {
    fn_8006B50C(0xDF, 1);
}

extern "C" void fn_8011EC7C(void) {}

namespace SetWeapon {

void SetWeaponTask::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new SetWeaponTask(fn_801EB304(fn_8001C4F8(), 11), owner);
}

} // namespace SetWeapon

/* The label of the "weapon" parameter: the weapon type's name, or "Not Used" for a type without
 * one (behaviour inferred). */
extern "C" const char *fn_8011EB88(UnkLabelQuery_800CBC20 *cursor) {
    static const char *notUsed = "Not Used";

    if (cursor->index == 0) {
        int type = cursor->value;
        if (fn_8005BF14(type)) {
            const char *name = fn_8005BEEC(type);
            if (name != 0 && strcmp(name, "") != 0) {
                return name;
            }
            return notUsed;
        }
        return notUsed;
    }
    return 0;
}

/* The ground probe's filter: objects with bit 0 of their flags set (meaning inferred). */
static bool IsGround(UnkCollision_80021E24 *hit) {
    return hit->m_flags & 1;
}

/* Drops a weapon of type weapon on the ground below position (name unknown; Game/Stick.cpp calls
 * it by this name). */
extern "C" void fn_8011E938(int weapon, const Vec3f *position, Vec3f *rotation) {
    u8 segment[0x18];
    Vec3f ground;
    Vec3f start;
    Vec3f end;

    fn_800091EC(&ground, position);
    fn_800091D0(&start, fn_800091D0(&end, position));
    end.y -= 1000.0f;
    fn_80020A20(segment, &start, &end);
    UnkCollision_80021E24 *hit = fn_80021E24(fn_80020D40(), segment, 3, 8, IsGround, 0);
    if (hit != 0) {
        if (hit->m_object != 0) {
            fn_800091D0(&ground, fn_8001E9BC(hit->m_object));
        }
        fn_800265A4(hit, 1);
    }

    boost::shared_ptr<Weapon::Item> item = fn_8005B9D4(weapon, -1);
    if (item.get() != 0) {
        boost::shared_ptr<SetWeapon::Arms> arms =
            boost::shared_ptr<SetWeapon::Arms>(new SetWeapon::Arms(item, 0));
        arms->SetTransform(&ground, rotation);
        Task *parent = fn_801EB304(fn_8001C4F8(), 11);
        new SetWeapon::WeaponTask(parent, arms);
    }
}

/* Drops item at position, facing along direction, without placing the item itself (name
 * unknown). */
extern "C" void fn_8011E630(const boost::shared_ptr<Weapon::Item> &item,
                            const Vec3f *position, const Vec3f *direction) {
    Vec3f rotation(0.0f, 0.0f, 0.0f);

    fn_8003B8F4(direction, &rotation.x, &rotation.z);
    boost::shared_ptr<SetWeapon::Arms> arms =
        boost::shared_ptr<SetWeapon::Arms>(new SetWeapon::Arms(item, 1));
    arms->SetTransform(position, &rotation);
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new SetWeapon::WeaponTask(parent, arms);
}

static GadgetParamDesc lbl_805276E4[] = {
    {1, "weapon", 1, 1, 0x47, 0.0f, 0.0f, 0.0f, fn_8011EB88, 0},
    {0},
};

GadgetClassDesc lbl_80527734 = {
    "SET_WEAPON", fn_8011EC80, fn_8011EC7C, SetWeapon::SetWeaponTask::Instantiate, 0, 0, 0x20, 1, 0,
    10, lbl_805276E4,
};
