/* Container::ContainerWeapon (class name from RTTI): the WEAPON_CONTAINER gadget, a container
 * (model file 0xCE, variant "kind") that holds the weapon of its "weapon" parameter. Written like
 * Game/ContainerHard.cpp, from which it differs in its layout (two parameters), its reward (always
 * 0xF) and in always releasing a weapon (content kind 2). Resting, it follows a ContainerBehavior;
 * carried, it follows the carrier's hand matrix; dropped, it flies until it hits something. Thrown
 * or pulled in by a vacuum, it hands its model to a new object and is done; when broken it leaves
 * debris, releases its weapon and rewards the attacker. File name inferred from the class. Names
 * marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <cstring>

#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/ContainerBehavior.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetPickup.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/PickupCharInfo.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;

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

/* SetTransparentCommand (command type 0x17) is declared in Game/CharCommand.h. */

/* The cursor handed to a parameter's label callback (layout from fn_800CBC20; names inferred). */
class UnkLabelQuery_800CBC20 {
  public:
    /* 0x0 */ const GadgetParamDesc *params;
    /* 0x4 */ s32 index; /* the parameter being shown */
    /* 0x8 */ s32 value; /* its current value */
};

extern "C" {
/* Sets a character info's name (behaviour inferred). */
void fn_801DC984(CharInfo *charInfo, const char *name);
/* Creates the pickup's target for owner with the given flags and radius, and moves the target to
 * the character info each frame (behaviours inferred). */
void fn_801E4160(PickupCharInfo *charInfo, const boost::weak_ptr<CharInfo> &owner, int flags,
                 float radius);
void fn_801E413C(PickupCharInfo *charInfo, CharInfo *position);
/* Returns the attack of a model's collision (behaviour inferred). */
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int variant);
/* Turns the model's collision off (behaviour inferred; the arguments are unknown). */
void fn_80161230(GadgetModelInfo *model, int index, int arg2);
/* Fades the model to alpha, and makes it opaque again (behaviour inferred). */
void fn_8015FDD8(GadgetModelInfo *model, float alpha);
void fn_8015FD10(GadgetModelInfo *model);
/* Rewards the character (behaviour and meaning of reward inferred). */
int fn_8020642C(const boost::shared_ptr<CharInfo> &character, int reward);
/* Releases the content: item value of kind select at position (behaviour inferred). */
void fn_8033E828(int select, int value, const Vec3f *position,
                 const Vec3f *rotation);
/* Breaks the model into debris (behaviour inferred). */
void fn_8033E644(boost::shared_ptr<GadgetModelInfo> model, int kind, const Vec3f *position,
                 const Vec3f *rotation);
/* Hands the model to the object a throw creates, and to the vacuum that pulls it in (behaviours
 * inferred; the float arguments are unknown). */
void fn_8027B338(const boost::weak_ptr<CharInfo> *thrower, boost::shared_ptr<GadgetModelInfo> *model,
                 int kind, int select, int value, CharInfo *charInfo, const Vec3f *rotation,
                 const Vec3f *velocity, float arg8, float arg9, float arg10,
                 const Vec3f *direction, int reward);
void fn_80254384(VacuumCommand *command, boost::shared_ptr<GadgetModelInfo> *model, int kind,
                 int select, int value, CharInfo *charInfo, const Vec3f *rotation,
                 int reward);
/* The names of an item and of a weapon type, and whether the weapon type exists (behaviour
 * inferred). */
const char *fn_8010B7F8(int item);
bool fn_8005BF14(int type);
const char *fn_8005BEEC(int type);
/* Vector helpers (behaviour inferred): vector * scale, vector += value, a + b, the translation and
 * the rotation of a matrix, and matrix * point. */
Vec3f fn_800091A4(const Vec3f *vector, float scale);
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
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
}

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

namespace Container {

/* The parameters the owner supplies (names from the parameter table). */
struct ContainerWeaponParams {
    ContainerWeaponParams() : kind(0), weapon(0) {}

    /* 0x0 */ int kind;   /* the model variant */
    /* 0x4 */ int weapon; /* the weapon type it holds */
};
SIZE_ASSERT(ContainerWeaponParams, 0x8);

class ContainerWeapon : public GadgetPickup::PickupBase {
  public:
    ContainerWeapon();
    virtual ~ContainerWeapon();
    virtual void SetPosition(const Vec3f &position, const Vec3f &rotation);
    virtual void Init(UnkPlacement_800C9CD0 *owner);
    virtual void Tick(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);
    virtual bool IsDone(GadgetPickup::PickupFlags *request);

    /* Creates a container from an owner (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Method names are inferred. */
    void ReleaseModel();
    void SetupModel(int model);
    void UpdateFlight(float dt);
    void OnAttack(AttackCallbackParam &param);
    void BreakModel();
    void UpdateCarried();
    void OnSetGrabPosition(PickupCharInfo::SetGrabPositionArgs &args);
    void OnCommand(CharCommand &command);

    /* The reward of breaking each model (name inferred). */
    int GetReward() {
        return 0xF;
    }

    /* Member names are inferred. */
    /* 0x6C */ u32 field_0x6C;
    /* 0x70 */ boost::shared_ptr<PickupCharInfo> m_pickup;
    /* 0x78 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x80 */ boost::shared_ptr<ContainerBehavior> m_behavior;
    /* 0x88 */ boost::shared_ptr<Target> m_target; /* never set by this unit */
    /* 0x90 */ s32 m_variant; /* the model variant shown */
    /* 0x94 */ s32 m_state; /* 0 new, 1 resting, 2 picked up, 3 carried, 4 thrown, 5 flying, 6
                               vacuumed, 7 broken */
    /* 0x98 */ ContainerWeaponParams m_params;
    /* 0xA0 */ boost::weak_ptr<CharInfo> m_carrier;
    /* 0xA8 */ boost::weak_ptr<CharInfo> m_attacker;
    /* 0xB0 */ float m_speed;
    /* 0xB4 */ Vec3f m_direction;
    /* 0xC0 */ Vec3f m_lastPosition;
    /* 0xCC */ Mtx34f m_handMatrix;
    /* 0xFC */ GadgetPickup::PickupFlags m_flags; /* 0 flying, 1 picked up, 2 done, 3 dropped, 4 hit
                                                     test */
    /* 0x100 */ GadgetPickup::PickupFlags m_hitFlags;
};
SIZE_ASSERT(ContainerWeapon, 0x104);

ContainerWeapon::ContainerWeapon() : PickupBase(4) {
    fn_801DC984(this, "ContainerWeapon");
}

ContainerWeapon::~ContainerWeapon() {}

void ContainerWeapon::Init(UnkPlacement_800C9CD0 *owner) {
    m_flags.Clear();
    m_hitFlags.Set(1);
    m_pickup = boost::shared_ptr<PickupCharInfo>(new PickupCharInfo(4));
    fn_800729B0(&m_pickup->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&ContainerWeapon::OnCommand, this, _1)));
    fn_801E4160(m_pickup.get(), m_pickup, 0x80, 30.0f);
    fn_801DC984(m_pickup.get(), "ContainerWeapon");
    m_pickup->SetKind(4);
    m_pickup->m_grabCallback = boost::function1<void, PickupCharInfo::SetGrabPositionArgs &>(
        boost::bind(&ContainerWeapon::OnSetGrabPosition, this, _1));
    m_variant = m_params.kind;
    SetupModel(m_variant);
    m_state = 0;
    SetPosition(m_pickup->m_position, m_pickup->m_rotation);
}

void ContainerWeapon::Tick(float dt) {
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
        m_flags.Set(2);
        break;
    case 5:
        SetPosition(m_pickup->m_position, m_pickup->m_rotation);
        break;
    case 7:
        BreakModel();
        fn_8033E828(2, m_params.weapon, &m_pickup->m_position, &m_pickup->m_rotation);
        if (boost::shared_ptr<CharInfo> attacker = m_attacker.lock()) {
            fn_8020642C(attacker, GetReward());
        }
        m_flags.Set(2);
        break;
    case 6:
        m_flags.Set(2);
        break;
    }
    fn_801E413C(m_pickup.get(), m_pickup.get());
}

bool ContainerWeapon::IsDone(GadgetPickup::PickupFlags *request) {
    if (request->Test(0) && m_flags.Test(1)) {
        return true;
    }
    if (request->Test(1) && m_flags.Test(2)) {
        return true;
    }
    return false;
}

void ContainerWeapon::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    int model = m_params.kind;

    if (m_pickup.get() != 0) {
        fn_800C9F14(owner, &m_pickup->m_position);
        fn_800C9ED4(owner, &m_pickup->m_rotation);
        m_pickup->m_position.y += 10.0f;
    }
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    if (model != m_params.kind) {
        ReleaseModel();
        m_variant = m_params.kind;
        SetupModel(m_variant);
    }
    SetPosition(m_pickup->m_position, m_pickup->m_rotation);
    m_state = 0;
}

void ContainerWeapon::SetPosition(const Vec3f &position, const Vec3f &rotation) {
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
    if (m_behavior.get() != 0) {
        fn_800EF474(m_behavior.get(), &position, &rotation);
    }
}

void ContainerWeapon::OnCommand(CharCommand &command) {
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
            fn_800091D0(&m_pickup->field_0x18,
                        &(const Vec3f &)fn_8040B618(&m_handMatrix,
                                                            &Vec3f(0.0f, 100.0f, -150.0f)));
            Vec3f rotation(0.0f, 0.0f, 0.0f);
            fn_800494F8(&rotation, &m_handMatrix);
            Vec3f velocity;
            fn_800091EC(&velocity, &(const Vec3f &)fn_8040CDA8(&m_direction, m_speed));
            fn_8027B338(&throwCommand.m_character, &m_model, m_params.kind, 2, m_params.weapon, m_pickup.get(), &rotation, &m_pickup->field_0x18, 0.0f,
                        1.0f, 600.0f, &velocity, GetReward());
            m_model.reset();
            m_state = 4;
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
            m_state = 5;
            m_flags.Set(3);
            m_flags.Set(0);
            m_flags.Set(4);
        }
        break;
    case 0x14:
        if (m_state == 1) {
            VacuumCommand &vacuum = static_cast<VacuumCommand &>(command);
            m_model->fn_80161880();
            fn_80254384(&vacuum, &m_model, m_params.kind, 2, m_params.weapon, this,
                        &m_rotation, GetReward());
            m_model.reset();
            m_state = 6;
        }
        break;
    case 0x17:
        if (static_cast<SetTransparentCommand &>(command).m_alpha < 1.0f) {
            if (m_model.get() != 0) {
                fn_8015FDD8(m_model.get(), static_cast<SetTransparentCommand &>(command).m_alpha);
            }
        } else {
            if (m_model.get() != 0) {
                fn_8015FD10(m_model.get());
            }
        }
        break;
    }
    command.field_0x4[3] |= 2;
}

void ContainerWeapon::OnSetGrabPosition(PickupCharInfo::SetGrabPositionArgs &args) {
    fn_8040C148(&m_handMatrix, args.m_matrix);
    if (m_state == 3) {
        UpdateCarried();
    }
}

void ContainerWeapon::UpdateCarried() {
    fn_800091D0(&m_pickup->m_position, &(const Vec3f &)fn_800496F8(&m_handMatrix));
    PickupBase::SetPosition(m_pickup->m_position, m_pickup->m_rotation);
    Mtx34f matrix;
    fn_8040C148(&matrix, &m_handMatrix);
    fn_8040B52C(&matrix, 0.0f, 10.0f, 0.0f);
    m_model->fn_801633DC(&matrix);
}

void ContainerWeapon::BreakModel() {
    fn_8033E644(m_model, m_params.kind, &m_pickup->m_position, &m_pickup->m_rotation);
}

void ContainerWeapon::OnAttack(AttackCallbackParam &param) {
    if (m_state == 1) {
        if (boost::shared_ptr<CharaColliAttack> collision = param.m_attacker.lock()) {
            if (collision->m_flags & 2) {
                param.m_result = 5;
                return;
            }
            m_attacker = collision->m_owner;
            m_state = 7;
            param.m_result = 3;
            return;
        }
    }
    param.m_result = 5;
}

void ContainerWeapon::UpdateFlight(float dt) {
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
            m_state = 7;
        }
    }
    fn_800091D0(&m_lastPosition, &m_pickup->m_position);
    fn_8040CE60(&m_pickup->field_0x24);
}

void ContainerWeapon::SetupModel(int model) {
    m_model = fn_8015F9D8(0, 0xCE);
    m_model->fn_80162A68(model, 1);
    m_model->fn_80161F08("col", m_pickup);
    boost::weak_ptr<CharaColliAttack> attack = fn_80161428(m_model.get(), 0);
    if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
        collision->field_0x54 = 0;
        collision->field_0x55 = 1;
        collision->m_attackCallback =
            AttackCallback(boost::bind(&ContainerWeapon::OnAttack, this, _1));
    }
}

void ContainerWeapon::ReleaseModel() {
    m_behavior.reset();
    m_model->fn_80161880();
    m_model->fn_80162804();
}

} // namespace Container

extern "C" void fn_800FCCC4(void) {
    fn_8006B50C(0xCE, 1);
}

extern "C" void fn_800FCCC0(void) {}

void Container::ContainerWeapon::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    boost::shared_ptr<GadgetPickup::PickupBase> pickup =
        boost::shared_ptr<GadgetPickup::PickupBase>(new ContainerWeapon());
    pickup->Init(owner);
    pickup->ReloadPlacement(owner);
    new GadgetPickup::SetPickupBaseTask(parent, owner, pickup);
}

/* The label of the "weapon" parameter: the weapon's name (name inferred). */
extern "C" const char *fn_800FC9E8(UnkLabelQuery_800CBC20 *cursor) {
    static const char *s_notUsed = "Not Used";

    if (cursor->index == 1) {
        int type = cursor->value;
        if (fn_8005BF14(type)) {
            const char *name = fn_8005BEEC(type);
            if (name != 0 && strcmp(name, "") != 0) {
                return name;
            }
            return s_notUsed;
        }
        return s_notUsed;
    }
    return 0;
}

static GadgetParamDesc lbl_8052582C[] = {
    {1, "kind", 0, 0, 2, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "weapon", 1, 1, 0x47, 0.0f, 0.0f, 0.0f, fn_800FC9E8, 0},
    {0},
};

GadgetClassDesc lbl_805258A4 = {
    "WEAPON_CONTAINER", fn_800FCCC4, fn_800FCCC0, Container::ContainerWeapon::Instantiate, 0, 0, 0xC, 1,
    0, 10, lbl_8052582C,
};
