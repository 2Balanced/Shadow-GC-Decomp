/* ElecCircuitColli (class name from RTTI): the ELEC_CIRCUITCOLLI gadget, a box collider of the
 * electric circuit puzzle at the owner's position. It wraps the collider in a handle and binds two
 * collision callbacks to it: on contact it changes the collider's attributes unless the character
 * that touched it passes fn_800B78B0, and on the second event it sets them back. File name inferred
 * from the class. Method names marked "(name inferred)" are not original. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "boost/bind.hpp"
#include "boost/function.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::CharaColliBase;

/* The collision event a collider reports (name from the bound callback's RTTI). Gadget code only
 * passes it through, so its members are not mapped. */
struct ColliDetectArgs {};

/* The event type of a handle's callbacks (name from the RTTI of CatchStick's callback binder). Its
 * use with a callback that takes ColliDetectArgs makes it a class derived from ColliDetectArgs. */
struct GadgetColliDetectArgs : public ColliDetectArgs {};

/* The function a handle calls back with a collision event (name inferred). */
typedef boost::function1<void, GadgetColliDetectArgs &> GadgetColliFunction;

/* The handle fn_8015B8C4 wraps a collider in (the class name is unknown). It calls m_onContact and
 * m_onRelease with collision events (names and meanings inferred). */
class GadgetCharCollision {
  public:
    /* Clears flag bit 1 (name and meaning inferred). */
    void ClearFlag2() {
        m_flags &= ~2;
    }

    /* 0x00 */ GadgetColliFunction m_onContact;
    /* 0x0C */ u8 field_0xC[0x18 - 0xC];
    /* 0x18 */ GadgetColliFunction m_onRelease;
    /* 0x24 */ CharaColliBase *m_collider;
    /* 0x28 */ u32 field_0x28;
    /* 0x2C */ u32 m_flags;
};

extern "C" {
/* The collider manager (behaviour inferred). */
void *fn_80021B08(void);
/* Creates a box collider of the given half size for charInfo (behaviour inferred; the meanings of
 * the integer arguments are unknown). */
boost::shared_ptr<CharaColliBase> fn_800C7C34(void *manager, const boost::weak_ptr<CharInfo> &charInfo,
                                              const Vec3f *size, int flags, int arg4, int arg5,
                                              const Vec3f *offset, const Vec3f *rotation,
                                              int arg8);
/* A vector constant read by the collision setup (meaning unknown). */
extern Vec3f lbl_805E07B8;
/* Wraps collider in a new handle (behaviour inferred). */
boost::shared_ptr<GadgetCharCollision> fn_8015B8C4(boost::shared_ptr<CharaColliBase> collider);
/* Assigns callback to one of a handle's callbacks (behaviour inferred; the callback's operator=). */
void fn_800F5980(GadgetColliFunction *target, const GadgetColliFunction &callback);
/* Draws the handle's shape for debugging, and advances the handle by dt (behaviour inferred). */
void fn_8015BE64(GadgetCharCollision *handle);
void fn_8015BE9C(GadgetCharCollision *handle, float dt);
/* Calls callback with every character in the collision event (behaviour inferred). */
void fn_8015AA84(ColliDetectArgs &args, void (*callback)(boost::shared_ptr<CharInfo> hit, void *user),
                 void *user);
/* A test on the character that touched the collider (behaviour unknown). */
bool fn_800B78B0(const boost::shared_ptr<CharInfo> &hit);
}

class ElecCircuitColli : public Gadget {
  public:
    ElecCircuitColli(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~ElecCircuitColli();
    virtual void Behave(float dt);
    virtual void fn_800CBC1C();
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* The handle's callbacks (names inferred). */
    void OnColliDetect(ColliDetectArgs &args);
    void OnColliRelease(ColliDetectArgs &args);
    /* Copies the owner's position and rotation (name inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    /* Draws the handle's shape for debugging (name inferred). */
    void DrawDebugShape();

    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetCharCollision> m_handle;
};
SIZE_ASSERT(ElecCircuitColli, 0x40);

/* What the collision callback passes to the per-character hit function (name inferred). */
struct ElecCircuitColliHitContext {
    /* 0x0 */ boost::weak_ptr<GadgetCharInfo> m_chara;
    /* 0x8 */ ElecCircuitColli *m_colli;
};
SIZE_ASSERT(ElecCircuitColliHitContext, 0xC);

const char *ElecCircuitColli::s_taskName = "ElecCircuitColli";

/* The collider's half size, built at startup. */
static Vec3f lbl_80585778(100.0f, 100.0f, 45.0f);

/* Switches the collider's attributes unless hit passes fn_800B78B0 (name inferred). Retail emits
 * it after the constructor and its template instances, so it is defined before the constructor
 * (defining it next to OnColliDetect emits it right after OnColliDetect instead). */
static void HitCharacter(boost::shared_ptr<CharInfo> hit, void *user) {
    ElecCircuitColliHitContext *context = static_cast<ElecCircuitColliHitContext *>(user);

    if (hit.get() != 0 && context != 0) {
        if (!fn_800B78B0(hit)) {
            context->m_colli->m_handle->m_collider->SetAttributes(0x202, -1);
        }
    }
}

ElecCircuitColli::ElecCircuitColli(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CB88(0x5A, 4, "ElecCircuitColli");
    boost::shared_ptr<CharaColliBase> collider = fn_800C7C34(fn_80021B08(), m_chara, &lbl_80585778,
                                                             0x40000, 10, 0, &lbl_805E07A0, &lbl_805E07B8, 0);
    m_handle = fn_8015B8C4(collider);
    fn_800F5980(&m_handle->m_onContact, boost::bind(&ElecCircuitColli::OnColliDetect, this, _1));
    fn_800F5980(&m_handle->m_onRelease, boost::bind(&ElecCircuitColli::OnColliRelease, this, _1));
    m_handle->ClearFlag2();
    LoadFromOwner(m_owner);
}

ElecCircuitColli::~ElecCircuitColli() {}

void ElecCircuitColli::Behave(float dt) {
    fn_8015BE9C(m_handle.get(), dt);
}

void ElecCircuitColli::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
}

void ElecCircuitColli::fn_800CBC1C() {
    DrawDebugShape();
}

void ElecCircuitColli::DrawDebugShape() {
    if (m_handle.get() != 0) {
        fn_8015BE64(m_handle.get());
    }
}

void ElecCircuitColli::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
}

void ElecCircuitColli::OnColliDetect(ColliDetectArgs &args) {
    ElecCircuitColliHitContext context;

    context.m_chara = m_chara;
    context.m_colli = this;
    fn_8015AA84(args, HitCharacter, &context);
}

void ElecCircuitColli::OnColliRelease(ColliDetectArgs &args) {
    m_handle->m_collider->SetAttributes(0x40000, -1);
}

extern "C" void fn_8033BAD0(void) {}

extern "C" void fn_8033BACC(void) {}

void ElecCircuitColli::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new ElecCircuitColli(fn_801EB304(fn_8001C4F8(), 11), owner);
}

GadgetClassDesc lbl_8055D638 = {
    "ELEC_CIRCUITCOLLI", fn_8033BAD0, fn_8033BACC, ElecCircuitColli::Instantiate, 0, 0, 0x7EC, 1, 0, 10, 0,
};
