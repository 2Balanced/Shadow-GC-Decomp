/* GravityChangeSwitch (class name from RTTI; its s_taskName is "GravityChangeSwitch"): the gadget
 * type of the same name, a switch (GRAVITYCHANGESWITCH.BIN) that, when a character touches it,
 * makes its "GRAVITY" direction the stage's current one, with an effect, a sound and a rumble, and
 * turns on its stage switch when it has one. It shows look 1 (with its target enabled) while
 * another gravity is current and look 0 while its own is. File name from the class. Names marked
 * "(name inferred)" are not original. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetUtil.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/Vibration.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* Target (class name from RTTI): a point other objects can aim at (see Spring.cpp); bit 0 of the
 * word at 0x30 enables it (inferred). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);

    void TurnOn() {
        m_flags |= 1;
    }

    void TurnOff() {
        m_flags &= ~1;
    }

    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ u32 m_flags;
};

/* Four colour bytes built by an out-of-line constructor (see ElecBarrier.cpp). */
struct UnkColor_8040D2D8 {
    UnkColor_8040D2D8(u8 r, u8 g, u8 b, u8 a);

    /* 0x0 */ u8 r;
    /* 0x1 */ u8 g;
    /* 0x2 */ u8 b;
    /* 0x3 */ u8 a;
};
SIZE_ASSERT(UnkColor_8040D2D8, 0x4);

/* A debug line segment set up by fn_80020A20 (0x18 bytes; contents unknown). */
struct UnkDebugLine_80020A20 {
    u8 field_0x0[0x18];
};

extern "C" {
/* A 32-bit flag word: fn_8000AAEC constructs one from a value, fn_80014E1C sets bit, fn_8006127C
 * clears bit and fn_80014E34 tests bit (see ArkCannon.cpp). */
u32 *fn_8000AAEC(u32 *flags, u32 value);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* The stage's gravity (behaviour inferred): the current gravity, its direction for a gravity
 * number, making gravity current, and the set-up and clean-up the type's hooks run. */
int fn_80293AC8(void);
const Vec3f *fn_80293B60(int gravity);
void fn_80293928(int gravity);
void fn_80293AE4(void);
void fn_80293B1C(void);

/* Whether a collision's contacts include a character (behaviour inferred; see Coaster.cpp). */
bool fn_8015D2EC(UnkColliContacts_8001EE04 *contacts);

/* GadgetModelInfo helpers (behaviour inferred; see BASwitch.cpp, Catapult.cpp and
 * GoalRing.cpp). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
void fn_801612B8(GadgetModelInfo *model);
void fn_801612E0(GadgetModelInfo *model);

/* Creates a Target for owner (see ArkCannon.cpp). */
boost::shared_ptr<Target> fn_801E7128(const boost::weak_ptr<CharInfo> &owner, int flags);

/* Starts effect id at position, pointing along direction (behaviour inferred). */
boost::weak_ptr<Effect::System::EffectElement> fn_80168C48(const Vec3f *position,
                                                           const Vec3f *direction, int id);
/* Plays sound id at a character info (behaviour inferred). */
void fn_801D54F4(void *manager, int id, GadgetCharInfo *source, int arg3, int arg4);
/* Starts a controller vibration (see GunLift.cpp). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int arg1);

/* a + b by value, and the debug line draw call (see Meteor.cpp). */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
void fn_802E0AA0(UnkDebugLine_80020A20 *segment, const UnkColor_8040D2D8 &color, float width);
}

/* a + b (name inferred; as in ArkLaser.cpp). */
inline Vec3f AddVectors(const Vec3f &a, const Vec3f &b) {
    return fn_8040CDD0(&a, &b);
}

/* The direction the touch effect points in (name inferred). The original copy-constructs it from a
 * temporary through the vector type's out-of-line copy constructor, fn_800091EC (as in
 * ArkLaser.cpp). */
static struct GravityChangeSwitchEffectDirection {
    GravityChangeSwitchEffectDirection() {
        float value[3];

        value[0] = 0.0f;
        value[1] = 0.0f;
        value[2] = -1.0f;
        fn_800091EC((Vec3f *)m_value, (Vec3f *)value);
    }

    const Vec3f *Get() const {
        return (const Vec3f *)m_value;
    }

    float m_value[3];
} s_effectDirection;

/* The flag word at 0x50 (struct name unknown). */
struct GravityChangeSwitchFlags {
    GravityChangeSwitchFlags(u32 value) {
        fn_8000AAEC(&m_bits, value);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    void Clear(int bit) {
        fn_8006127C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits; /* bit 0: touched this frame */
};

class GravityChangeSwitch : public Gadget {
  public:
    GravityChangeSwitch(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~GravityChangeSwitch();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Method names are inferred. */
    /* Copies the owner's position, rotation, gravity and switch id. */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model and the target. */
    void RebuildTransform();
    /* Shows the look that matches the stage's current gravity. */
    void UpdateLook();
    /* Shows look 1 (another gravity is current) or 2 (this one is). */
    void SetLook(int look);
    /* The model's collision callback. */
    void OnColliDetect(ColliDetectArgs &args);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ s32 m_gravity;
    /* 0x34 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x3C */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x44 */ boost::shared_ptr<Target> m_target;
    /* 0x4C */ s32 m_look;
    /* 0x50 */ GravityChangeSwitchFlags m_flags;
    /* 0x54 */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
    /* 0x5C */ u8 m_switchId; /* 0 for none */
};
SIZE_ASSERT(GravityChangeSwitch, 0x60);

const char *GravityChangeSwitch::s_taskName = "GravityChangeSwitch";

GravityChangeSwitch::GravityChangeSwitch(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner), m_gravity(0), m_flags(0) {
    m_name = s_taskName;
    m_effect.reset();
    m_chara = fn_8015CD90(0x66, 4);
    ReadPlacement(owner);
    m_target = fn_801E7128(m_chara, 0xA400);
    m_model = fn_8015F800(1, "GRAVITYCHANGESWITCH.BIN");
    fn_80161FC8(m_model.get(), 0, m_chara);
    fn_800D0B64(m_model->field_0x4, boost::bind(&GravityChangeSwitch::OnColliDetect, this, _1));
    UpdateLook();
}

GravityChangeSwitch::~GravityChangeSwitch() {
    if (!m_effect.expired()) {
        boost::shared_ptr<Effect::System::EffectElement> effect = m_effect.lock();
        if (effect.get() != 0) {
            effect->Retire();
        }
        m_effect.reset();
    }
}

extern "C" void fn_80246DF4(void) {
    fn_80293B1C();
    fn_8006B474("GRAVITYCHANGESWITCH.BIN", 1);
}

extern "C" void fn_80246DD4(void) {
    fn_80293AE4();
}

void GravityChangeSwitch::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new GravityChangeSwitch(fn_801EB304(fn_8001C4F8(), 11), owner);
}

void GravityChangeSwitch::Behave(float dt) {
    m_model->fn_80162FB0(dt);
    if (m_flags.Test(0)) {
        if (m_effect.expired()) {
            m_effect = fn_80168C48(&m_chara->m_position, s_effectDirection.Get(), 0xE);
        }
        fn_80293928(m_gravity);
        fn_801D54F4(fn_80053164(), 0x773B, m_chara.get(), 0, 1);
        fn_8034DF48(5, -2);
        if (m_switchId != 0) {
            fn_801690D0(fn_800CB69C(), m_switchId, 1);
        }
        m_flags.Clear(0);
    } else {
        UpdateLook();
    }
}

void GravityChangeSwitch::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    RebuildTransform();
    m_model->fn_80162EF4();
    m_model->fn_8016108C();
    Vec3f direction;
    fn_800091EC(&direction, fn_80293B60(m_gravity));
    fn_8040CE38(&direction, -50.0f);
    UnkDebugLine_80020A20 segment;
    fn_80020A20(&segment, &m_chara->m_position, &(const Vec3f &)AddVectors(m_chara->m_position, direction));
    fn_802E0AA0(&segment, UnkColor_8040D2D8(0xC0, 0xFF, 0xFF, 0x00), 1.0f);
}

void GravityChangeSwitch::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_gravity, sizeof(m_gravity));
    fn_800C9E60(owner, &m_switchId);
    m_look = 0;
}

void GravityChangeSwitch::RebuildTransform() {
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_chara->m_position);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    fn_8040B884(&matrix, m_chara->m_rotation.x);
    fn_8040B80C(&matrix, m_chara->m_rotation.z);
    m_model->fn_801633DC(&matrix);
    m_target->fn_800CCE38(&m_chara->m_position);
}

void GravityChangeSwitch::UpdateLook() {
    if (m_gravity == fn_80293AC8()) {
        if (m_look != 2) {
            SetLook(2);
        }
    } else {
        if (m_look != 1) {
            SetLook(1);
        }
    }
}

void GravityChangeSwitch::SetLook(int look) {
    m_model->fn_80162804();
    switch (look) {
    case 1:
        m_model->fn_80162A68(0, 0);
        RebuildTransform();
        fn_801612B8(m_model.get());
        m_target->TurnOn();
        break;
    case 2:
        m_model->fn_80162A68(1, 0);
        RebuildTransform();
        fn_801612E0(m_model.get());
        m_target->TurnOff();
        break;
    }
    m_look = look;
}

void GravityChangeSwitch::OnColliDetect(ColliDetectArgs &args) {
    if (fn_8015D2EC(args.m_contacts)) {
        m_flags.Set(0);
    }
}

static GadgetParamDesc lbl_805488B4[] = {
    {1, "GRAVITY", 0, 0, 5, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80548904 = {
    "GravityChangeSwitch", fn_80246DF4, fn_80246DD4, GravityChangeSwitch::Instantiate, 0, 0, 0x14B4, 1, 0,
    10, lbl_805488B4,
};
