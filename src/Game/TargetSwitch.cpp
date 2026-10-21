/* TargetSwitch (class name from RTTI): the TARGETSWITCH gadget, a target that swings between the
 * owner's position and the "X/Y/Z" offset, one way in "sec" seconds and resting "pause" seconds at
 * each end. Its collider's attack callback takes the attack's damage from its "hitpoint" points and
 * recolours the model by the points left; when they run out the gadget sets the owner's switch,
 * starts an effect and finishes. File name inferred from the class. Method names marked "(name
 * inferred)" are not original. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <cmath>

#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* Target (class name from RTTI): a point other objects can aim at (meaning inferred). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);
};

extern "C" {
/* Loads the model file fileName for a gadget (behaviour inferred; arg0 unknown). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* Returns the collider of the model's part number index (behaviour inferred). */
boost::weak_ptr<SonicteamUSA::System::CharaColli::CharaColliAttack> fn_80161428(GadgetModelInfo *model, int index);
/* Creates a target with the given flags (behaviour inferred). */
boost::shared_ptr<Target> fn_801E72DC(int flags);
/* Restores the model's material colours, and colours it by the colour named name (behaviour
 * inferred). */
void fn_8016232C(GadgetModelInfo *model);
void fn_8016239C(GadgetModelInfo *model, const char *name);
/* Plays sound id at the character info's position (behaviour inferred; the meanings of the last
 * two arguments are unknown). */
void fn_801D54F4(void *manager, int id, GadgetCharInfo *charInfo, int arg3, int arg4);
/* Launches effect number at charInfo's position (behaviour inferred). */
boost::weak_ptr<Effect::System::EffectElement> fn_8015D794(GadgetCharInfo *charInfo, int number);
/* Tells the owner the gadget is done (behaviour inferred). */
void fn_800C9B34(UnkPlacement_800C9CD0 *owner);
/* result += value. */
void fn_8040CFF8(Vec3f *result, const Vec3f *value);
/* Returns vector normalised (behaviour inferred). */
Vec3f fn_8040CEE8(const Vec3f &vector);
/* Returns a + b (behaviour inferred). The result goes through the hidden result pointer, so this
 * is the fn_8040CDD0(destination, a, b) that other units declare. */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
}

/* Pi, and degrees to radians (names inferred). Retail calls a shared out-of-line copy of the
 * conversion (0x8000AB90) that each unit emits, as an inline function whose body calls another
 * inline one is; the copy loads pi and 180 and multiplies. */
inline float Pi() {
    return lbl_805FAD38;
}

inline float DegToRad(float degrees) {
    return degrees * (Pi() / 180.0f);
}

/* The six parameters the owner supplies (names from the parameter table). */
struct TargetSwitchParams {
    TargetSwitchParams() : x(0.0f), y(0.0f), z(0.0f), sec(0.0f), pause(0.0f), hitpoint(2) {}

    /* 0x00 */ float x; /* the far end, relative to the owner */
    /* 0x04 */ float y;
    /* 0x08 */ float z;
    /* 0x0C */ float sec;   /* seconds for one way */
    /* 0x10 */ float pause; /* seconds of rest at each end */
    /* 0x14 */ int hitpoint;
};
SIZE_ASSERT(TargetSwitchParams, 0x18);

class TargetSwitch : public Gadget {
  public:
    TargetSwitch(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~TargetSwitch();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the target under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Colours the model by the hit points left (name inferred). */
    void UpdateColor(float hitPoints);
    /* The collider's attack callback (name inferred). */
    void OnAttack(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    /* Places the character info angle degrees along its swing (name inferred). */
    void UpdatePosition(float angle);
    /* Places the model at the character info's translation and rotation, and moves the target
     * (name inferred). */
    void RebuildTransform();
    /* Copies the owner's position, rotation and parameters and restarts the swing (name
     * inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);

    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ TargetSwitchParams m_params;
    /* 0x58 */ float m_time;      /* seconds along the swing (name inferred) */
    /* 0x5C */ float m_speed;     /* degrees per second (name inferred) */
    /* 0x60 */ float m_pauseTime; /* rest left (name inferred) */
    /* 0x64 */ float m_hitPoints;
    /* 0x68 */ int m_state; /* 0 start, 1 out, 2 back, 3 resting, 4 destroyed (inferred) */
    /* 0x6C */ boost::shared_ptr<Target> m_target;
    /* 0x74 */ u32 field_0x74; /* not used by this unit */
};
SIZE_ASSERT(TargetSwitch, 0x78);

const char *TargetSwitch::s_taskName = "TargetSwitch";

TargetSwitch::TargetSwitch(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(4, 4);
    m_model = fn_8015F800(0, "TARGETSWITCH.BIN");
    m_model->fn_80162A68(0, 0);
    m_model->fn_80161F08("col", m_chara);

    boost::weak_ptr<SonicteamUSA::System::CharaColli::CharaColliAttack> collider = fn_80161428(m_model.get(), 0);
    if (boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliAttack> locked = collider.lock()) {
        locked->field_0x54 = 0;
        locked->field_0x55 = 1;
        locked->m_attackCallback =
            SonicteamUSA::System::CharaColli::AttackCallback(boost::bind(&TargetSwitch::OnAttack, this, _1));
    }

    m_model->fn_80160BA0("mtn");
    m_model->fn_8015FF60();
    m_target = fn_801E72DC(0x800);
    m_state = 0;
    LoadFromOwner(m_owner);
    RebuildTransform();
    UpdateColor(m_hitPoints);
}

TargetSwitch::~TargetSwitch() {}

void TargetSwitch::Behave(float dt) {
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }

    switch (m_state) {
    case 0:
        m_state = 1;
        break;
    case 1: {
        m_time += dt;
        float angle = m_speed * m_time;
        if (angle >= 180.0f) {
            m_state = 3;
            m_pauseTime = m_params.pause;
        }
        UpdatePosition(angle);
        break;
    }
    case 2: {
        m_time += dt;
        float angle = m_speed * m_time;
        if (m_time > m_params.sec) {
            m_time = 0.0f;
            m_state = 3;
            m_pauseTime = m_params.pause;
        }
        UpdatePosition(angle);
        break;
    }
    case 3:
        m_pauseTime -= dt;
        if (m_pauseTime <= 0.0f) {
            if (m_time > 0.0f) {
                m_state = 2;
            } else {
                m_state = 1;
            }
        }
        break;
    case 4:
        if (m_owner != 0) {
            u8 switchId = 0;

            fn_800C9E60(m_owner, &switchId);
            fn_801690D0(fn_800CB69C(), switchId, 1);
            fn_800C9B34(m_owner);
        }
        fn_8015D794(m_chara.get(), 0xA9);
        m_flags |= 1;
        break;
    }

    RebuildTransform();
}

void TargetSwitch::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    RebuildTransform();
    if (m_model.get() != 0) {
        m_model->fn_80162EF4();
        m_model->fn_8016108C();

        u8 segment[0x18];
        Vec3f start;
        fn_800091EC(&start, &m_chara->m_position);
        Vec3f end(m_params.x, m_params.y, m_params.z);
        fn_8040CFF8(&end, &start);
        fn_802E0B58(fn_80020A20(segment, &start, &end), &lbl_805F06D8);
    }
}

void TargetSwitch::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_time = 0.0f;
    m_speed = m_params.sec > 0.0f ? 360.0f / m_params.sec : 0.0f;
    m_hitPoints = m_params.hitpoint;
}

void TargetSwitch::RebuildTransform() {
    if (m_model.get() != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B884(&matrix, m_chara->m_rotation.x);
        fn_8040B80C(&matrix, m_chara->m_rotation.z);
        m_model->fn_801633DC(&matrix);
    }
    if (m_target.get() != 0) {
        m_target->fn_800CCE38(&m_chara->m_position);
    }
}

void TargetSwitch::UpdatePosition(float angle) {
    Vec3f origin;
    Vec3f end(m_params.x, m_params.y, m_params.z);

    fn_800C9F14(m_owner, &origin);
    fn_8040CFF8(&end, &origin);
    Vec3f direction(m_params.x, m_params.y, m_params.z);
    float halfLength = 0.5f * fn_8040CF50(&direction);
    AssignVector(direction, fn_8040CEE8(direction));
    fn_8040CE38(&direction, halfLength - halfLength * std::cos(DegToRad(angle)));
    const Vec3f &position = fn_8040CDD0(&direction, &origin);
    fn_800091D0(&m_chara->m_position, &position);
}

void TargetSwitch::OnAttack(SonicteamUSA::System::CharaColli::AttackCallbackParam &param) {
    if (m_state != 4) {
        if (boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliAttack> attacker = param.m_attacker.lock()) {
            if (attacker->m_flags & 2) {
                param.m_result = 5;
                return;
            }
        }

        m_hitPoints -= param.m_damage;
        if (m_hitPoints <= 0.0f) {
            param.m_result = 3;
            m_state = 4;
            fn_801D54F4(fn_80053164(), 0x100F, m_chara.get(), 0, 1);
        } else {
            param.m_result = 1;
            fn_801D54F4(fn_80053164(), 0x100E, m_chara.get(), 0, 1);
        }
        if (m_model.get() != 0) {
            m_model->fn_80160B18(0);
        }
        UpdateColor(m_hitPoints);
    } else {
        param.m_result = 5;
    }
}

void TargetSwitch::UpdateColor(float hitPoints) {
    if (m_model.get() != 0) {
        fn_8016232C(m_model.get());
        if (hitPoints >= 8.0f) {
            fn_8016239C(m_model.get(), "purple");
        } else if (hitPoints >= 6.0f) {
            fn_8016239C(m_model.get(), "blue");
        } else if (hitPoints >= 4.0f) {
            fn_8016239C(m_model.get(), "green");
        } else if (hitPoints >= 2.0f) {
            fn_8016239C(m_model.get(), "yellow");
        } else {
            fn_8016239C(m_model.get(), "red");
        }
    }
}

extern "C" void fn_801245F4(void) {
    fn_8006B474("TARGETSWITCH.BIN", 1);
}

extern "C" void fn_801245F0(void) {}

void TargetSwitch::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new TargetSwitch(fn_801EB304(fn_8001C4F8(), 11), owner);
}

static GadgetParamDesc lbl_80527BF4[] = {
    {4, "X", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "Y", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "Z", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "sec", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "pause", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {1, "hitpoint", 2, 2, 10, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80527D0C = {
    "TARGETSWITCH", fn_801245F4, fn_801245F0, TargetSwitch::Instantiate, 0, 0, 0x16, 1, 0, 10,
    lbl_80527BF4,
};
