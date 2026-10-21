/* GravityChangeZone (class name from RTTI; its s_taskName is "GravityChangeZone"): the gadget type
 * of the same name, a zone (GRAVITYCHANGEZONE.BIN, shown with GravityChangeSwitch's model) that,
 * when a character touches it, makes its "GRAVITY" direction the stage's current one, with an
 * effect, a sound and a rumble, and turns on its stage switch when it has one. Like
 * GravityChangeSwitch.cpp, whose code it repeats, it shows look 1 while another gravity is current
 * and look 2 while its own is. File name from the class. Names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetUtil.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/Vibration.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

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

/* The collider of a model part (see BKTank.cpp). */
boost::weak_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> fn_80161614(GadgetModelInfo *model,
                                                                             int variant);

/* Particle controller helpers (behaviour inferred; see BASwitch.cpp and BombServer.cpp). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void fn_80167180(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
void fn_8016761C(GadgetParticle::GadgetParticleCtrl *ctrl, float dt);
void fn_8015FA78(GadgetModelInfo *model, void *(*callback)(void *part, void *data), void *data);
void *fn_80169D00(void *part, void *data);

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
static struct GravityChangeZoneEffectDirection {
    GravityChangeZoneEffectDirection() {
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

/* The flag word at 0x60 (struct name unknown). */
struct GravityChangeZoneFlags {
    GravityChangeZoneFlags(u32 value) {
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

class GravityChangeZone : public Gadget {
  public:
    GravityChangeZone(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~GravityChangeZone();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Method names are inferred. */
    /* Copies the owner's position, rotation, gravity and switch id. */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the two models. */
    void RebuildTransform();
    /* Shows the look that matches the stage's current gravity. */
    void UpdateLook();
    /* Shows look 1 (another gravity is current) or 2 (this one is). */
    void SetLook(int look);
    /* The zone model's collision callback. */
    void OnColliDetect(ColliDetectArgs &args);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ s32 m_gravity;
    /* 0x34 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x3C */ boost::shared_ptr<GadgetModelInfo> m_model; /* GravityChangeSwitch's */
    /* 0x44 */ boost::shared_ptr<GadgetModelInfo> m_zone;
    /* 0x4C */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particles;
    /* 0x54 */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
    /* 0x5C */ s32 m_look;
    /* 0x60 */ GravityChangeZoneFlags m_flags;
    /* 0x64 */ u8 m_switchId; /* 0 for none */
};
SIZE_ASSERT(GravityChangeZone, 0x68);

const char *GravityChangeZone::s_taskName = "GravityChangeZone";

GravityChangeZone::GravityChangeZone(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner), m_gravity(0), m_flags(0) {
    m_name = s_taskName;
    m_effect.reset();
    m_chara = fn_8015CD90(0x67, 4);
    ReadPlacement(owner);
    m_model = fn_8015F800(1, "GRAVITYCHANGESWITCH.BIN");
    m_zone = fn_8015F800(1, "GRAVITYCHANGEZONE.BIN");
    m_zone->fn_80162A68(0, 0);
    fn_80161FC8(m_zone.get(), 0, m_chara);
    fn_800D0B64(m_zone->field_0x4, boost::bind(&GravityChangeZone::OnColliDetect, this, _1));
    {
        boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> collider =
            fn_80161614(m_zone.get(), 0).lock();
        if (collider) {
            collider->SetAttributes(0x140100, -1);
        }
    }
    UpdateLook();
    m_particles = fn_80166AF4();
    UnkParticleAttach_80169D00 ref;
    ref.ctrl = m_particles;
    fn_8015FA78(m_zone.get(), fn_80169D00, &ref);
    fn_80167180(m_particles.get(), -1);
}

GravityChangeZone::~GravityChangeZone() {
    if (!m_effect.expired()) {
        boost::shared_ptr<Effect::System::EffectElement> effect = m_effect.lock();
        if (effect.get() != 0) {
            effect->Retire();
        }
        m_effect.reset();
    }
}

extern "C" void fn_80247FA8(void) {
    fn_80293B1C();
    fn_8006B474("GRAVITYCHANGESWITCH.BIN", 1);
    fn_8006B474("GRAVITYCHANGEZONE.BIN", 1);
}

extern "C" void fn_80247F88(void) {
    fn_80293AE4();
}

void GravityChangeZone::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new GravityChangeZone(fn_801EB304(fn_8001C4F8(), 11), owner);
}

void GravityChangeZone::Behave(float dt) {
    m_model->fn_80162FB0(dt);
    m_zone->fn_80162FB0(dt);
    if (m_particles) {
        fn_8016761C(m_particles.get(), dt);
    }
    if (m_flags.Test(0)) {
        if (m_effect.expired()) {
            m_effect = fn_80168C48(&m_chara->m_position, s_effectDirection.Get(), 0xE);
        }
        fn_80293928(m_gravity);
        fn_8034DF48(5, -2);
        if (m_switchId != 0) {
            fn_801690D0(fn_800CB69C(), m_switchId, 1);
        }
        fn_801D54F4(fn_80053164(), 0x773B, m_chara.get(), 0, 1);
        m_flags.Clear(0);
    } else {
        UpdateLook();
    }
}

void GravityChangeZone::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    RebuildTransform();
    m_model->fn_80162EF4();
    m_zone->fn_80162EF4();
    m_zone->fn_8016108C();
    Vec3f direction;
    fn_800091EC(&direction, fn_80293B60(m_gravity));
    fn_8040CE38(&direction, -50.0f);
    UnkDebugLine_80020A20 segment;
    fn_80020A20(&segment, &m_chara->m_position, &(const Vec3f &)AddVectors(m_chara->m_position, direction));
    fn_802E0AA0(&segment, UnkColor_8040D2D8(0xC0, 0xFF, 0xFF, 0x00), 1.0f);
}

void GravityChangeZone::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_gravity, sizeof(m_gravity));
    fn_800C9E60(owner, &m_switchId);
    m_look = 0;
}

void GravityChangeZone::RebuildTransform() {
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_chara->m_position);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    fn_8040B884(&matrix, m_chara->m_rotation.x);
    fn_8040B80C(&matrix, m_chara->m_rotation.z);
    m_model->fn_801633DC(&matrix);
    m_zone->fn_801633DC(&matrix);
}

void GravityChangeZone::UpdateLook() {
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

void GravityChangeZone::SetLook(int look) {
    m_model->fn_80162804();
    switch (look) {
    case 1:
        m_model->fn_80162A68(0, 0);
        RebuildTransform();
        fn_801612B8(m_zone.get());
        break;
    case 2:
        m_model->fn_80162A68(1, 0);
        RebuildTransform();
        fn_801612E0(m_zone.get());
        break;
    }
    m_look = look;
}

void GravityChangeZone::OnColliDetect(ColliDetectArgs &args) {
    if (fn_8015D2EC(args.m_contacts)) {
        m_flags.Set(0);
    }
}

static GadgetParamDesc lbl_80548AA4[] = {
    {1, "GRAVITY", 0, 0, 5, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80548AF4 = {
    "GravityChangeZone", fn_80247FA8, fn_80247F88, GravityChangeZone::Instantiate, 0, 0, 0x14B5, 1, 0,
    10, lbl_80548AA4,
};
