/* Container::ContainerNormal (class name from RTTI): the CONTAINER gadget, a breakable container
 * (model file 0xCA, variant "model") a character can pick up, carry and throw or drop. It is a
 * pickup object (GadgetPickup::PickupBase) with a PickupCharInfo of its own, written like the BOMB
 * gadget: resting, it follows a ContainerBehavior; carried, it follows the carrier's hand matrix;
 * dropped, it flies until it hits something. Thrown or pulled in by a vacuum, it hands its model
 * to a new object and is done. An attack or a hit after a drop breaks it: it leaves debris,
 * releases the content of its "select"/"value" parameters (an item, a weapon, rings, a healing unit
 * or an energy core) and rewards the attacker. File name inferred from the class. Names marked
 * "(name inferred)" are not original. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
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

/* SetTransparentCommand (class name from RTTI): fades a character's model; its command type is
 * 0x17. Only the alpha is used here (as in ContainerUnbreak.cpp). */
class SetTransparentCommand : public CharCommand {
  public:
    /* 0x14 */ u8 field_0x14;
    /* 0x18 */ float m_alpha; /* 1 or more: opaque (inferred) */
    /* 0x1C */ u32 field_0x1C;
};
SIZE_ASSERT(SetTransparentCommand, 0x20);

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

/* Set up at startup and never read (meaning unknown): 45 degrees in radians. */
static float s_angle = 45.0f * (lbl_805FAD38 / 180.0f);

namespace Container {

/* The parameters the owner supplies (names from the parameter table). */
struct ContainerNormalParams {
    ContainerNormalParams() : model(0), select(0), value(0) {}

    /* 0x0 */ int model;  /* the model variant */
    /* 0x4 */ int select; /* what it holds: none, item, weapon, rings, healing unit, energy core */
    /* 0x8 */ int value;  /* which item, weapon or core, or how many rings */
};
SIZE_ASSERT(ContainerNormalParams, 0xC);

class ContainerNormal : public GadgetPickup::PickupBase {
  public:
    ContainerNormal();
    virtual ~ContainerNormal();
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
    void ReleaseContent();
    void UpdateFlight(float dt);
    void OnAttack(AttackCallbackParam &param);
    void BreakModel();
    void UpdateCarried();
    void OnSetGrabPosition(PickupCharInfo::SetGrabPositionArgs &args);
    void OnCommand(CharCommand &command);

    /* The reward of breaking each model (name inferred). */
    int GetReward() {
        switch (m_params.model) {
        case 0:
            return 9;
        case 1:
            return 0xB;
        case 2:
            return 0xD;
        default:
            return 0x27;
        }
    }

    /* Member names are inferred. */
    /* 0x6C */ u32 field_0x6C;
    /* 0x70 */ boost::shared_ptr<PickupCharInfo> m_pickup;
    /* 0x78 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x80 */ boost::shared_ptr<ContainerBehavior> m_behavior;
    /* 0x88 */ s32 m_variant; /* the model variant shown */
    /* 0x8C */ ContainerNormalParams m_params;
    /* 0x98 */ boost::weak_ptr<CharInfo> m_attacker;
    /* 0xA0 */ boost::weak_ptr<CharInfo> m_carrier;
    /* 0xA8 */ float m_speed;
    /* 0xAC */ Vec3f m_direction;
    /* 0xB8 */ Vec3f m_lastPosition;
    /* 0xC4 */ Vec3f m_pickupPosition; /* where it was picked up */
    /* 0xD0 */ Vec3f m_pickupTarget;
    /* 0xDC */ Mtx34f m_handMatrix;
    /* 0x10C */ float field_0x10C;
    /* 0x110 */ float field_0x110;
    /* 0x114 */ GadgetPickup::PickupFlags m_flags; /* 0 flying, 1 picked up, 2 done, 3 dropped, 4 hit
                                                      test */
    /* 0x118 */ GadgetPickup::PickupFlags m_hitFlags;
    /* 0x11C */ s32 m_state; /* 0 new, 1 resting, 2 picked up, 3 carried, 4 thrown, 5 flying, 6
                                vacuumed, 7 broken */
};
SIZE_ASSERT(ContainerNormal, 0x120);

ContainerNormal::ContainerNormal() : PickupBase(4) {
    fn_801DC984(this, "ContainerNormal");
}

ContainerNormal::~ContainerNormal() {}

void ContainerNormal::Init(UnkPlacement_800C9CD0 *owner) {
    m_flags.Clear();
    m_hitFlags.Set(1);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_pickup = boost::shared_ptr<PickupCharInfo>(new PickupCharInfo(4));
    fn_800729B0(&m_pickup->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&ContainerNormal::OnCommand, this, _1)));
    fn_801E4160(m_pickup.get(), m_pickup, 0x80, 30.0f);
    fn_801DC984(m_pickup.get(), "ContainerNormal");
    m_pickup->SetKind(4);
    m_pickup->m_grabCallback = boost::function1<void, PickupCharInfo::SetGrabPositionArgs &>(
        boost::bind(&ContainerNormal::OnSetGrabPosition, this, _1));
    m_variant = m_params.model;
    SetupModel(m_variant);
    m_state = 0;
    SetPosition(m_pickup->m_position, m_pickup->m_rotation);
}

void ContainerNormal::Tick(float dt) {
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
        ReleaseContent();
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

bool ContainerNormal::IsDone(GadgetPickup::PickupFlags *request) {
    if (request->Test(0) && m_flags.Test(1)) {
        return true;
    }
    if (request->Test(1) && m_flags.Test(2)) {
        return true;
    }
    return false;
}

void ContainerNormal::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    int model = m_params.model;

    if (m_pickup.get() != 0) {
        fn_800C9F14(owner, &m_pickup->m_position);
        fn_800C9ED4(owner, &m_pickup->m_rotation);
        m_pickup->m_position.y += 10.0f;
    }
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    if (model != m_params.model) {
        ReleaseModel();
        m_variant = m_params.model;
        SetupModel(m_variant);
    }
    SetPosition(m_pickup->m_position, m_pickup->m_rotation);
    m_state = 0;
}

void ContainerNormal::SetPosition(const Vec3f &position, const Vec3f &rotation) {
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

void ContainerNormal::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0x11:
        if (m_state == 1 && !m_flags.Test(1)) {
            PickupCommand &pickup = static_cast<PickupCommand &>(command);
            field_0x10C = 0.0f;
            AssignVector(m_direction, pickup.field_0x2C);
            m_speed = -320.0f;
            field_0x110 = pickup.field_0x20;
            fn_800091D0(&m_pickupPosition, &m_pickup->m_position);
            fn_800091D0(&m_pickupTarget,
                        &(const Vec3f &)fn_8040CDD0(
                            &pickup.field_0x14,
                            &(const Vec3f &)fn_8040CDA8(&pickup.field_0x2C, 10.0f)));
            m_carrier = pickup.m_character;
            m_pickup->m_target->TurnOff();
            fn_80161230(m_model.get(), 0, -1);
            m_behavior.reset();
            m_state = 2;
            m_flags.Set(1);
            m_flags.Set(0);
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
            m_model->fn_80161880();
            Vec3f rotation(0.0f, 0.0f, 0.0f);
            fn_800494F8(&rotation, &m_handMatrix);
            Vec3f velocity;
            fn_800091EC(&velocity, &(const Vec3f &)fn_8040CDA8(&m_direction, m_speed));
            fn_8027B338(&throwCommand.m_character, &m_model, m_params.model, m_params.select,
                        m_params.value, m_pickup.get(), &rotation, &m_pickup->field_0x18, 0.0f,
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
            fn_80254384(&vacuum, &m_model, m_params.model, m_params.select, m_params.value, this,
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

void ContainerNormal::OnSetGrabPosition(PickupCharInfo::SetGrabPositionArgs &args) {
    fn_8040C148(&m_handMatrix, args.m_matrix);
    if (m_state == 3) {
        UpdateCarried();
    }
}

void ContainerNormal::UpdateCarried() {
    fn_800091D0(&m_pickup->m_position, &(const Vec3f &)fn_800496F8(&m_handMatrix));
    PickupBase::SetPosition(m_pickup->m_position, m_pickup->m_rotation);
    Mtx34f matrix;
    fn_8040C148(&matrix, &m_handMatrix);
    fn_8040B52C(&matrix, 0.0f, 10.0f, 0.0f);
    m_model->fn_801633DC(&matrix);
}

void ContainerNormal::BreakModel() {
    fn_8033E644(m_model, m_params.model, &m_pickup->m_position, &m_pickup->m_rotation);
}

void ContainerNormal::OnAttack(AttackCallbackParam &param) {
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

void ContainerNormal::UpdateFlight(float dt) {
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

void ContainerNormal::ReleaseContent() {
    fn_8033E828(m_params.select, m_params.value, &m_pickup->m_position, &m_pickup->m_rotation);
}

void ContainerNormal::SetupModel(int model) {
    m_model = fn_8015F9D8(0, 0xCA);
    m_model->fn_80162A68(model, 1);
    m_model->fn_80161F08("col", m_pickup);
    boost::weak_ptr<CharaColliAttack> attack = fn_80161428(m_model.get(), 0);
    if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
        collision->field_0x54 = 0;
        collision->field_0x55 = 1;
        collision->m_attackCallback =
            AttackCallback(boost::bind(&ContainerNormal::OnAttack, this, _1));
    }
}

void ContainerNormal::ReleaseModel() {
    m_behavior.reset();
    if (m_model.get() != 0) {
        m_model->fn_80161880();
        m_model->fn_80162804();
    }
}

} // namespace Container

extern "C" void fn_800F688C(void) {
    fn_8006B50C(0xCA, 1);
}

extern "C" void fn_800F6888(void) {}

void Container::ContainerNormal::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    boost::shared_ptr<GadgetPickup::PickupBase> pickup =
        boost::shared_ptr<GadgetPickup::PickupBase>(new ContainerNormal());
    pickup->Init(owner);
    pickup->ReloadPlacement(owner);
    new GadgetPickup::SetPickupBaseTask(parent, owner, pickup);
}

/* The labels of the "select" and "value" parameters; value is labelled by the select last shown
 * (the item or weapon name, or the core's side). */
extern "C" const char *fn_800F64D0(UnkLabelQuery_800CBC20 *cursor) {
    static const char *s_selectLabels[6] = {"None", "Item", "Weapon", "Ring", "HealUnit",
                                            "EnergyCore"};
    static const char *s_coreLabels[3] = {"Hero", "Dark", "N/A"};
    static const char *s_ringNum = "Ring Num";
    static const char *s_notAvailable = "N/A";
    static int s_select = 0;

    if (cursor->index == 1) {
        int select = cursor->value;
        s_select = select;
        return s_selectLabels[select];
    } else if (cursor->index == 2) {
        if (s_select == 0) {
        } else if (s_select == 1) {
            const char *name = fn_8010B7F8(cursor->value);
            if (name != 0 && strcmp(name, "") != 0) {
                return name;
            }
        } else if (s_select == 2) {
            int type = cursor->value;
            if (fn_8005BF14(type)) {
                const char *name = fn_8005BEEC(type);
                if (name != 0 && strcmp(name, "") != 0) {
                    return name;
                }
            }
        } else if (s_select == 3) {
            return s_ringNum;
        } else if (s_select == 4) {
        } else if (s_select == 5) {
            int core = cursor->value;
            if (core >= 0 && core <= 1) {
                return s_coreLabels[core];
            }
        }
        return s_notAvailable;
    }
    return s_notAvailable;
}

static GadgetParamDesc lbl_805253D0[] = {
    {1, "model", 0, 0, 2, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "select", 0, 0, 6, 0.0f, 0.0f, 0.0f, fn_800F64D0, 0},
    {1, "value", 0, 0, 3600, 0.0f, 0.0f, 0.0f, fn_800F64D0, 0},
    {0},
};

GadgetClassDesc lbl_80525470 = {
    "CONTAINER", fn_800F688C, fn_800F6888, Container::ContainerNormal::Instantiate, 0, 0, 9, 1, 0, 10,
    lbl_805253D0,
};
