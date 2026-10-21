/* EnergyCore (classes EnergyCore::SetEnergyCoreTask, EnergyCore::EnergyCoreTask and
 * EnergyCore::EnergyCore from RTTI): the ENERGY_CORE gadget. SetEnergyCoreTask is the stage object
 * and EnergyCoreTask a free-standing task other code creates through EnergyCoreTask::Instantiate; both
 * keep an EnergyCore, which owns the core's character info, a collision handle and the effect of
 * its "type" (0 or 1). The first character the collision reports picks the core up: the hit
 * function hands it the core's item, and the owning task then finishes (the stage object after
 * launching a pick-up effect). EnergyCoreTask also finishes once fn_80169BE4 reports its position
 * against its radius (meaning inferred). File name inferred from the namespace. Names marked
 * "(name inferred)" are not original. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetColliDetectArgs.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetClassDesc.h"
#include "boost/scoped_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

extern "C" {
/* A 32-bit flag word: fn_80014E04 clears it, fn_80014E1C sets bit and fn_80014E34 tests bit
 * (behaviour inferred from their bodies). */
void fn_80014E04(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* The collider manager (behaviour inferred). */
void *fn_80021B08(void);
/* Creates a collision for owner (behaviour inferred; the meaning of the arguments is unknown). */
boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliBase>
fn_800C7DEC(void *manager, const boost::weak_ptr<CharInfo> &owner, int flags, int arg3, int arg4,
            const Vec3f *offset, float radius);
/* Wraps a collision in the handle gadgets keep (behaviour inferred). */
boost::shared_ptr<GadgetCharCollision>
fn_8015B8C4(boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> collision);
/* Draws the handle's shape for debugging, and advances the handle by dt (behaviour inferred). */
void fn_8015BE64(GadgetCharCollision *handle);
void fn_8015BE9C(GadgetCharCollision *handle, float dt);

/* Launches effect id at position (behaviour inferred). */
boost::weak_ptr<Effect::System::EffectElement> fn_8015D794(const Vec3f *position, int id);
/* Moves a running effect to position (behaviour inferred). */
void fn_80425E4C(Effect::System::EffectElement *effect, const Vec3f *position);

/* Calls callback with every character in the collision event (behaviour inferred). */
void fn_8015AA84(ColliDetectArgs &args,
                 void (*callback)(boost::shared_ptr<CharInfo> hit, void *user), void *user);
/* Gives target the item id (behaviour inferred; the meaning of the other arguments is unknown). */
void fn_8015F6EC(boost::shared_ptr<CharInfo> target, int id, int arg2, int arg3);

/* Reports whether the task at position may finish, given a squared radius (behaviour
 * inferred). */
bool fn_80169BE4(const Vec3f *position, float radiusSquared);
/* Tells the owner that the gadget is done (behaviour inferred). */
void fn_8015A794(Gadget *gadget);
}

/* Passed (converted to an integer) with each item (meaning unknown). */
extern const float lbl_805F4BF0;

/* Copies a rotation one component at a time (name inferred). The source is not const: the original
 * reloads each component after the previous store, which a const source would let the scheduler
 * hoist. */
inline void CopyRotation(Vec3f &rotation, Vec3f &value) {
    rotation.x = value.x;
    rotation.y = value.y;
    rotation.z = value.z;
}

namespace EnergyCore {

/* Both tasks keep the core behind a boost::scoped_ptr (a pimpl); it is defined below them. */
class EnergyCore;

/* What the collision callback passes to the hit function (name inferred). */
struct EnergyCoreHitContext {
    EnergyCoreHitContext() : m_type(0), m_taken(false) {}

    /* 0x0 */ boost::weak_ptr<GadgetCharInfo> m_chara;
    /* 0x8 */ int m_type;
    /* 0xC */ bool m_taken;
};

/* The free-standing core (name from RTTI): created by other code with a type, a position, a
 * rotation and a radius. */
class EnergyCoreTask : public Task {
  public:
    EnergyCoreTask(int type, Task *parent, const Vec3f &position,
                   Vec3f &rotation, float radius);
    virtual ~EnergyCoreTask();
    virtual void Tick(float dt);

    /* Creates a core under task layer 11 (name inferred). */
    static void Instantiate(int type, const Vec3f &position, Vec3f &rotation,
                       float radius);

    static const char *s_taskName;

    /* 0x28 */ boost::scoped_ptr<EnergyCore> m_core;
    /* 0x2C */ Vec3f m_position;
    /* 0x38 */ Vec3f m_rotation;
    /* 0x44 */ float m_radiusSquared;
};

/* The ENERGY_CORE stage object (name from RTTI). */
class SetEnergyCoreTask : public Gadget {
  public:
    SetEnergyCoreTask(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~SetEnergyCoreTask();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Passes the position and rotation to the core (name inferred). */
    void UpdateTransform();

    /* Copies the owner's position, rotation and type (name inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
        if (owner != 0) {
            fn_800C9F14(owner, &m_position);
            fn_800C9ED4(owner, &m_rotation);
            fn_800C9DBC(owner, &m_type, sizeof(m_type));
        }
    }

    static const char *s_taskName;

    /* 0x30 */ boost::scoped_ptr<EnergyCore> m_core;
    /* 0x34 */ Vec3f m_position;
    /* 0x40 */ Vec3f m_rotation;
    /* 0x4C */ int m_type; /* the "type" parameter */
};

/* The core shared by both tasks (class name from the RTTI of its bound collision callback). Method
 * and member names are inferred. */
class EnergyCore {
  public:
    EnergyCore();
    ~EnergyCore();

    /* The collision callback: hands the first character the core's item (name inferred). */
    void OnColliDetect(ColliDetectArgs &args);
    /* Lets the effect finish and forgets it (name inferred). */
    void StopEffect();
    /* Draws the collision handle for debugging (name inferred). */
    void DrawCollision();
    /* Sets the type and launches its effect (name inferred). */
    void SetType(int type);
    /* Places the character info and the effect (name inferred). */
    void SetTransform(const Vec3f *position, Vec3f *rotation);
    /* Advances the collision handle (name inferred). */
    void Tick(float dt);

    /* True once a character picked the core up (name inferred). */
    bool IsTaken() {
        return fn_80014E34(&m_flags, 0);
    }

    /* 0x00 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x08 */ boost::shared_ptr<GadgetCharCollision> m_handle;
    /* 0x10 */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
    /* 0x18 */ int m_type;
    /* 0x1C */ u32 m_flags; /* bit 0: taken */
};

/* Deletes a core (name inferred; see checked_delete below). */
inline void DeleteEnergyCore(EnergyCore *core) {
    delete core;
}

} // namespace EnergyCore

/* Retail keeps boost::checked_delete<EnergyCore> out of line (0x80101368) and ~EnergyCore at its
 * own place (0x80101A10). GC/2.7 only keeps checked_delete out of line when its body calls an
 * inline function, and emits an inline ~EnergyCore right after it (see docs/research-notes.md);
 * this specialization calls an inline helper instead, which gives retail's code and order (as in
 * Rocket.cpp). */
namespace boost {
template<> inline void checked_delete<EnergyCore::EnergyCore>(EnergyCore::EnergyCore *x) {
    EnergyCore::DeleteEnergyCore(x);
}
} // namespace boost

SIZE_ASSERT(EnergyCore::EnergyCore, 0x20);
SIZE_ASSERT(EnergyCore::EnergyCoreHitContext, 0x10);
SIZE_ASSERT(EnergyCore::EnergyCoreTask, 0x48);
SIZE_ASSERT(EnergyCore::SetEnergyCoreTask, 0x50);

namespace EnergyCore {

const char *SetEnergyCoreTask::s_taskName = "EnergyCore::SetEnergyCoreTask";
const char *EnergyCoreTask::s_taskName = "EnergyCore::EnergyCoreTask";

/* The effect of each type. */
static const int s_effects[2] = {0xAD, 0xB1};

/* Hands hit the core's item unless the core was already taken (name inferred). */
static void HitCharacter(boost::shared_ptr<CharInfo> hit, void *user) {
    EnergyCoreHitContext *context = static_cast<EnergyCoreHitContext *>(user);

    if (context != 0 && !context->m_taken) {
        static const int s_items[2] = {7, 8};
        fn_8015F6EC(hit, s_items[context->m_type], lbl_805F4BF0, 0);
        context->m_taken = true;
    }
}

EnergyCore::EnergyCore() {
    fn_80014E04(&m_flags);
    m_type = 0;
    m_chara = fn_8015CB88(0x1C, 4, "EnergyCore");
    boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> collision =
        fn_800C7DEC(fn_80021B08(), m_chara, 0, 10, 0, &lbl_805E07A0, 10.0f);
    m_handle = fn_8015B8C4(collision);
    fn_800F5980(m_handle.get(), boost::bind(&EnergyCore::OnColliDetect, this, _1));
}

EnergyCore::~EnergyCore() {
    StopEffect();
}

void EnergyCore::Tick(float dt) {
    fn_8015BE9C(m_handle.get(), dt);
}

void EnergyCore::SetTransform(const Vec3f *position, Vec3f *rotation) {
    fn_800091D0(&m_chara->m_position, position);
    CopyRotation(m_chara->m_rotation, *rotation);
    if (!m_effect.expired()) {
        boost::shared_ptr<Effect::System::EffectElement> effect = m_effect.lock();
        fn_80425E4C(effect.get(), &m_chara->m_position);
    }
}

void EnergyCore::SetType(int type) {
    m_type = type;
    m_effect = fn_8015D794(&m_chara->m_position, s_effects[m_type]);
}

void EnergyCore::DrawCollision() {
    fn_8015BE64(m_handle.get());
}

void EnergyCore::StopEffect() {
    if (!m_effect.expired()) {
        boost::shared_ptr<Effect::System::EffectElement> effect = m_effect.lock();
        effect->Retire();
    }
    m_effect.reset();
}

void EnergyCore::OnColliDetect(ColliDetectArgs &args) {
    EnergyCoreHitContext context;

    context.m_chara = m_chara;
    context.m_type = m_type;
    fn_8015AA84(args, HitCharacter, &context);
    if (context.m_taken) {
        fn_80014E1C(&m_flags, 0);
    }
}

SetEnergyCoreTask::SetEnergyCoreTask(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner), m_core(new EnergyCore()), m_rotation(0.0f, 0.0f, 0.0f), m_type(0) {
    m_name = s_taskName;
    LoadFromOwner(owner);
    m_core->SetType(m_type);
    UpdateTransform();
}

SetEnergyCoreTask::~SetEnergyCoreTask() {
    m_core->StopEffect();
}

void SetEnergyCoreTask::Behave(float dt) {
    m_core->Tick(dt);
    if (m_core->IsTaken()) {
        if (m_type == 0) {
            fn_8015D794(&m_position, 0x2A1);
        } else {
            fn_8015D794(&m_position, 0x2A5);
        }
        fn_8015A794(this);
        m_flags |= 1;
    }
}

void SetEnergyCoreTask::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    int type = m_type;

    LoadFromOwner(owner);
    if (m_type != type) {
        m_core->StopEffect();
        m_core->SetType(m_type);
    }
    UpdateTransform();
    m_core->DrawCollision();
}

void SetEnergyCoreTask::UpdateTransform() {
    m_core->SetTransform(&m_position, &m_rotation);
}

EnergyCoreTask::EnergyCoreTask(int type, Task *parent, const Vec3f &position,
                               Vec3f &rotation, float radius)
    : Task(parent), m_core(new EnergyCore()), m_rotation(0.0f, 0.0f, 0.0f) {
    m_name = s_taskName;
    fn_800091D0(&m_position, &position);
    CopyRotation(m_rotation, rotation);
    m_radiusSquared = radius * radius;
    m_core->SetType(type);
    m_core->SetTransform(&m_position, &m_rotation);
}

EnergyCoreTask::~EnergyCoreTask() {
    m_core->StopEffect();
}

void EnergyCoreTask::Tick(float dt) {
    m_core->Tick(dt);
    if (m_core->IsTaken()) {
        m_flags |= 1;
    } else if (fn_80169BE4(&m_position, m_radiusSquared)) {
        m_flags |= 1;
    }
}

void EnergyCoreTask::Instantiate(int type, const Vec3f &position,
                            Vec3f &rotation, float radius) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new EnergyCoreTask(type, parent, position, rotation, radius);
}

} // namespace EnergyCore

extern "C" void fn_801011AC(void) {}

extern "C" void fn_801011A8(void) {}

void EnergyCore::SetEnergyCoreTask::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new SetEnergyCoreTask(parent, owner);
}

static GadgetParamDesc lbl_80525B6C[] = {
    {1, "type", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80525BBC = {
    "ENERGY_CORE", fn_801011AC, fn_801011A8, EnergyCore::SetEnergyCoreTask::Instantiate, 0, 0, 0x33, 1, 0,
    10, lbl_80525B6C,
};
