/* FireRing::FireRing (class name from RTTI): the FIRE_RING gadget, a burning ring (FIRERING.BIN)
 * that plays its fire sound while it stands. Two colliders 14 units in front of and behind the ring
 * report the characters that touch them: a character that touches one side and then the other has
 * passed through, so the ring tells it (fn_8015F6EC), spawns a task through fn_80136B5C and burns
 * out, finishing half a second later. An attack whose flag bit 4 is set ends it at once. File name
 * inferred from the class. Method names marked "(name inferred)" are not original. Definitions are
 * in reverse address order (deferred inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

/* The collision event a collider reports (name from the bound callback's RTTI). Gadget code only
 * passes it through, so its members are not mapped. */
struct ColliDetectArgs {};

/* The event type of a handle's callbacks (name from the RTTI of CatchStick's callback binder); its
 * use with a callback that takes ColliDetectArgs makes it a class derived from ColliDetectArgs. */
struct GadgetColliDetectArgs : public ColliDetectArgs {};

/* The function a handle calls back with a collision event (name inferred). */
typedef boost::function1<void, GadgetColliDetectArgs &> GadgetColliFunction;

/* The handle fn_8015B8C4 wraps a collider in (the class name is unknown; layout as in
 * ElecCircuitColli). It calls m_onContact with collision events (names and meanings inferred). */
class GadgetCharCollision {
  public:
    /* Sets flag bit 1, which stops the handle's collisions (name and meaning inferred). */
    void SetFlag2() {
        m_flags |= 2;
    }

    /* 0x00 */ GadgetColliFunction m_onContact;
    /* 0x0C */ u8 field_0xC[0x18 - 0xC];
    /* 0x18 */ GadgetColliFunction m_onRelease;
    /* 0x24 */ CharaColliBase *m_collider;
    /* 0x28 */ u32 field_0x28;
    /* 0x2C */ u32 m_flags;
};

/* The collider manager (behaviour inferred). */
class UnkColliderManager_80021B08;

extern "C" {
/* A 32-bit flag word: fn_80014E04 constructs it clear, fn_80014E10 clears it, fn_80014E1C sets
 * bit and fn_80014E34 tests bit; fn_8000AAEC constructs one from a value and fn_8000B21C copies
 * one (behaviour inferred from their bodies). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
u32 *fn_8000AAEC(u32 *flags, u32 value);
void fn_8000B21C(u32 *flags, const u32 *source);

/* Loads the model file fileName (behaviour inferred; arg0 unknown). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* Shows the model named name of the model file (behaviour inferred; arg2 unknown). */
void fn_801629D8(GadgetModelInfo *model, const char *name, int arg2);
/* Returns the attack collision of the model's collision node name (behaviour inferred). */
boost::weak_ptr<CharaColliAttack> fn_80161334(GadgetModelInfo *model, const char *name);
/* A model call taking a float (behaviour unknown; TurnOver passes the same -1). */
void fn_8016268C(GadgetModelInfo *model, float arg1);

/* Returns the collider manager, and creates a collider for charInfo with the given flags, offset,
 * rotation and size (behaviour and argument meanings inferred, as in DarkSpinEntrance). */
UnkColliderManager_80021B08 *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7A84(UnkColliderManager_80021B08 *manager,
                                              const boost::weak_ptr<CharInfo> &charInfo, int flags,
                                              int arg3, u16 arg4, const Vec3f *offset,
                                              const Vec3f *rotation, float height,
                                              float radius, int arg9);
/* Wraps collider in a new handle (behaviour inferred). */
boost::shared_ptr<GadgetCharCollision>
fn_8015B8C4(boost::shared_ptr<CharaColliBase> collider);
/* Assigns callback to one of a handle's callbacks (behaviour inferred; the callback's
 * operator=). */
void fn_800F5980(GadgetColliFunction *target, const GadgetColliFunction &callback);
/* Draws the handle's shape for debugging, and advances the handle by dt (behaviour inferred). */
void fn_8015BE64(GadgetCharCollision *handle);
void fn_8015BE9C(GadgetCharCollision *handle, float dt);
/* Calls callback with every character in the collision event (behaviour inferred). */
void fn_8015AE44(ColliDetectArgs &args,
                 void (*callback)(boost::shared_ptr<CharInfo> hit, void *user), void *user);

/* GadgetModelInfo particle helpers (behaviour inferred, as in Elevator and Stick). */
void fn_8015FA78(GadgetModelInfo *model,
                 void *(*callback)(void *part, UnkParticleAttach_80169D00 *param),
                 UnkParticleAttach_80169D00 *param);
void *fn_80169D00(void *part, UnkParticleAttach_80169D00 *param);
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_80166AF4(void);
void fn_80167180(GadgetParticle::GadgetParticleCtrl *ctrl, int id);

/* Plays sound id at charInfo for source (behaviour inferred; the other arguments are unknown). */
void fn_801D53C4(void *sound, int id, GadgetCharInfo *charInfo, void *source, int arg4, int arg5,
                 int arg6);
/* A test on the character that passed through (behaviour unknown). */
bool fn_800B7AD0(const boost::shared_ptr<CharInfo> &character);
/* Tells the character that it passed through the ring (behaviour inferred; the meanings of the
 * integer arguments are unknown). */
void fn_8015F6EC(boost::shared_ptr<CharInfo> character, int arg1, int arg2, int arg3);
/* Creates a task of the Gong unit for kind unless kind is 5 (behaviour inferred). */
void fn_80136B5C(int kind);
/* Marks the gadget finished (behaviour inferred). */
void fn_8015A794(Gadget *gadget);
/* A vector constant read by the collision setup (meaning unknown). */
extern Vec3f lbl_805E07B8;
}

namespace FireRing {

class FireRing : public Gadget {
  public:
    FireRing(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~FireRing();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the ring under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position and rotation and rearms the ring (name inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    /* Places the model at the character info's translation and rotation (name inferred). */
    void RebuildTransform();
    /* The attack callback of the model's collision (name inferred). */
    void OnAttack(AttackCallbackParam &param);
    /* The collision callbacks of the front and back colliders (names inferred). */
    void OnFrontColliDetect(ColliDetectArgs &args);
    void OnBackColliDetect(ColliDetectArgs &args);

    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::shared_ptr<GadgetCharCollision> m_front; /* the collider at +14 */
    /* 0x48 */ boost::shared_ptr<GadgetCharCollision> m_back;  /* the collider at -14 */
    /* 0x50 */ boost::weak_ptr<CharInfo> m_passer; /* the character passing through (inferred) */
    /* 0x58 */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particle;
    /* 0x60 */ s32 m_state; /* 0 new, 1 burning, 2 passed through, 3 burning out (inferred) */
    /* 0x64 */ float m_timer;
    /* 0x68 */ u32 m_touchFlags; /* bit 0 front touched, bit 1 back touched, bit 2 entered
                                  * (inferred) */
    /* 0x6C */ u32 pad_0x6C;      /* the class size is a multiple of 8 */
};

/* What the collision callbacks pass to the per-character hit function (name inferred). */
struct FireRingHitContext {
    FireRingHitContext() : m_hit(false) {}

    /* 0x0 */ boost::weak_ptr<CharInfo> m_character;
    /* 0x8 */ bool m_hit;
};

} // namespace FireRing

SIZE_ASSERT(FireRing::FireRing, 0x70);
SIZE_ASSERT(FireRing::FireRingHitContext, 0xC);

namespace FireRing {

const char *FireRing::s_taskName = "FireRing";

/* Records the character a collider touched (name inferred). Retail emits it after the constructor
 * and its template instances, so it is defined before the constructor. */
static void HitCharacter(boost::shared_ptr<CharInfo> hit, void *user) {
    FireRingHitContext *context = static_cast<FireRingHitContext *>(user);

    if (context != 0) {
        context->m_character = hit;
        context->m_hit = true;
    }
}

FireRing::FireRing(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    fn_80014E04(&m_touchFlags);
    m_name = s_taskName;
    m_chara = fn_8015CB88(0x1A, 4, "FireRing");
    LoadFromOwner(m_owner);
    m_model = fn_8015F800(1, "FIRERING.BIN");
    fn_801629D8(m_model.get(), "ring", 0);
    m_model->fn_80161F08("ring", m_chara);
    boost::weak_ptr<CharaColliAttack> attack = fn_80161334(m_model.get(), "ring");
    if (boost::shared_ptr<CharaColliAttack> target = attack.lock()) {
        target->field_0x54 = 0;
        target->field_0x55 = 1;
        target->m_attackCallback = AttackCallback(boost::bind(&FireRing::OnAttack, this, _1));
    }

    {
        Vec3f offset(0.0f, 0.0f, 14.0f);
        Vec3f rotation = lbl_805E07B8;
        boost::shared_ptr<CharaColliBase> collider = fn_800C7A84(
            fn_80021B08(), m_chara, 0x80000, 10, 0, &offset, &rotation, 3.0f, 18.0f, 1);
        m_front = fn_8015B8C4(collider);
        fn_800F5980(&m_front->m_onContact, boost::bind(&FireRing::OnFrontColliDetect, this, _1));
    }

    {
        Vec3f offset(0.0f, 0.0f, -14.0f);
        Vec3f rotation = lbl_805E07B8;
        boost::shared_ptr<CharaColliBase> collider = fn_800C7A84(
            fn_80021B08(), m_chara, 0x80000, 10, 0, &offset, &rotation, 3.0f, 18.0f, 1);
        m_back = fn_8015B8C4(collider);
        fn_800F5980(&m_back->m_onContact, boost::bind(&FireRing::OnBackColliDetect, this, _1));
    }

    RebuildTransform();
}

FireRing::~FireRing() {}

void FireRing::Behave(float dt) {
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }
    fn_8015BE9C(m_front.get(), dt);
    fn_8015BE9C(m_back.get(), dt);
    fn_801D53C4(fn_80053164(), 0x7444, m_chara.get(), this, 3, 0, 2);

    switch (m_state) {
    case 0:
        m_state = 1;
        if (m_model.get() != 0) {
            m_particle = fn_80166AF4();

            UnkParticleAttach_80169D00 attach;
            attach.ctrl = m_particle;
            fn_8015FA78(m_model.get(), fn_80169D00, &attach);
        }
        if (m_particle.get() != 0) {
            fn_80167180(m_particle.get(), -1);
        }
        break;
    case 1:
        if (fn_80014E34(&m_touchFlags, 2)) {
            if (!m_passer.expired()) {
                if (fn_800B7AD0(m_passer.lock())) {
                    m_passer.reset();
                    fn_80014E10(&m_touchFlags);
                }
            } else {
                fn_80014E10(&m_touchFlags);
            }
        }
        break;
    case 2:
        if (!m_passer.expired()) {
            boost::shared_ptr<CharInfo> passer = m_passer.lock();
            fn_8015F6EC(passer, 0, 5, 0);
            fn_80136B5C(1);
            m_passer.reset();
        }
        m_timer = 0.5f;
        m_state = 3;
        m_front->SetFlag2();
        m_back->SetFlag2();
        fn_8016268C(m_model.get(), -1.0f);
        if (m_particle.get() != 0) {
            m_particle.reset();
        }
        break;
    case 3:
        m_timer -= dt;
        if (m_timer <= 0.0f) {
            fn_8015A794(this);
            m_flags |= 1;
        }
        break;
    }
}

void FireRing::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    RebuildTransform();
    if (m_model.get() != 0) {
        m_model->fn_8016108C();
        fn_8015BE64(m_front.get());
        fn_8015BE64(m_back.get());
    }
}

void FireRing::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    m_state = 0;
    m_timer = 0.0f;
    fn_80014E10(&m_touchFlags);
}

void FireRing::RebuildTransform() {
    if (m_model.get() != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B884(&matrix, m_chara->m_rotation.x);
        fn_8040B80C(&matrix, m_chara->m_rotation.z);
        m_model->fn_801633DC(&matrix);
    }
}

void FireRing::OnAttack(AttackCallbackParam &param) {
    if (m_state == 1) {
        if (boost::shared_ptr<CharaColliAttack> attacker = param.m_attacker.lock()) {
            if ((attacker->m_flags & 0x10) == 0) {
                param.m_result = 5;
                return;
            }
        }
        param.m_result = 3;
        m_state = 3;
    } else {
        param.m_result = 5;
    }
}

void FireRing::OnFrontColliDetect(ColliDetectArgs &args) {
    FireRingHitContext context;

    fn_8015AE44(args, HitCharacter, &context);
    if (context.m_hit) {
        if (fn_80014E34(&m_touchFlags, 2)) {
            if (fn_80014E34(&m_touchFlags, 1)) {
                m_state = 2;
                fn_80014E10(&m_touchFlags);
            }
        } else {
            fn_80014E1C(&m_touchFlags, 0);
            fn_80014E1C(&m_touchFlags, 2);
            m_passer = context.m_character;
        }
    }
}

void FireRing::OnBackColliDetect(ColliDetectArgs &args) {
    FireRingHitContext context;

    fn_8015AE44(args, HitCharacter, &context);
    if (context.m_hit) {
        if (fn_80014E34(&m_touchFlags, 2)) {
            if (fn_80014E34(&m_touchFlags, 0)) {
                m_state = 2;
                u32 cleared;
                fn_8000B21C(&m_touchFlags, fn_8000AAEC(&cleared, 0));
            }
        } else {
            fn_80014E1C(&m_touchFlags, 1);
            fn_80014E1C(&m_touchFlags, 2);
            m_passer = context.m_character;
        }
    }
}

} // namespace FireRing

extern "C" void fn_801352FC(void) {
    fn_8006B474("FIRERING.BIN", 1);
}

extern "C" void fn_801352F8(void) {}

void FireRing::FireRing::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new FireRing(fn_801EB304(fn_8001C4F8(), 11), owner);
}

GadgetClassDesc lbl_80528B74 = {
    "FIRE_RING", fn_801352FC, fn_801352F8, FireRing::FireRing::Instantiate, 0, 0, 0xC85, 1, 0, 10, 0,
};
