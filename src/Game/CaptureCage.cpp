/* CaptureCage (class CaptureCage::CaptureCage from RTTI; its s_taskName is "CaptureCage"): the
 * CAPTURE_CAGE gadget, a cage shown with the CAPTURECAGE.BIN model. Its collision captures the
 * nearest character that touches it (a BindCommand on that character); attacks wear its health
 * down, and when it breaks it spawns the CAPTCAGECRSH01.DFF debris, gives the attacker reward 0x1F,
 * releases the captured character (a ReleaseCommand), shows the broken model and fades it out
 * before finishing. File name inferred from the class. Names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first); the static HitCharacter, emitted after the constructor and its boost
 * helpers, is defined before the constructor. The unit's code reaches its .data through a pooled
 * base address, so the linker keeps that section whole: Game/TNumEdit.h supplies the four RTTI
 * base lists it starts with, and the weak_ptr::lock copies it emits (boost/weak_ptr.hpp) the
 * std::exception and bad_weak_ptr data it ends with. */
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/FileQueue.h"
#include "boost/bind.hpp"
#include "boost/function.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::CharaColliAttack;

/* The parameters of a debris spawn (layout from its constructor at 0x8015F0B8, BreakObj::Break and
 * CaptureCage::Break; names and meanings inferred). */
struct UnkDebrisParam_8015F0B8 {
    /* Sets up the parameters at a character info's place from template index (behaviour
     * inferred). */
    UnkDebrisParam_8015F0B8(const boost::shared_ptr<CharInfo> &info, int index);

    /* 0x00 */ Task *parent;
    /* 0x04 */ Vec3f position;
    /* 0x10 */ Vec3f rotation;
    /* 0x1C */ u8 field_0x1C[0x34 - 0x1C];
    /* 0x34 */ float scale;
    /* 0x38 */ u8 field_0x38[0x40 - 0x38];
    /* 0x40 */ float field_0x40;
    /* 0x44 */ s32 model;
    /* 0x48 */ u8 field_0x48[0x4A - 0x48];
    /* 0x4A */ u8 field_0x4A;
    /* 0x4B */ u8 field_0x4B;
    /* 0x4C */ s32 field_0x4C;
    /* 0x50 */ u8 field_0x50[0x54 - 0x50];
    /* 0x54 */ u8 field_0x54;
    /* 0x55 */ u8 field_0x55[0x58 - 0x55];
    /* 0x58 */ s32 rotationOrder;
};
SIZE_ASSERT(UnkDebrisParam_8015F0B8, 0x5C);

/* BindCommand and ReleaseCommand are declared in Game/CharCommand.h. */

extern "C" {
/* A 32-bit flag word: fn_80014E04 clears it, fn_80014E1C sets bit, fn_8006127C clears bit and
 * fn_80014E34 tests bit (behaviour inferred from their bodies). */
void fn_80014E04(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);

/* Loads the model fileName (behaviour inferred; arg0 unknown). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* GadgetModelInfo methods that are not declared in its header yet (behaviour inferred):
 * fn_80161FC8 attaches a character info to the collision of a model variant, fn_80161428 returns
 * that collision's attack and fn_8016268C fades the model (the argument's meaning is unknown). */
bool fn_80161FC8(GadgetModelInfo *model, int variant, const boost::weak_ptr<CharInfo> &info);
boost::weak_ptr<CharaColliAttack> fn_80161428(GadgetModelInfo *model, int variant);
void fn_8016268C(GadgetModelInfo *model, float arg1);

/* Calls callback with every character in the collision event (behaviour inferred; the last
 * argument is unknown). */
void fn_8015B70C(ColliDetectArgs &args, void (*callback)(boost::shared_ptr<CharInfo> hit, void *user),
                 void *user, int arg3);

/* Spawns debris (behaviour inferred). */
void fn_8015D93C(UnkDebrisParam_8015F0B8 *param);
/* Returns the model manager, and the id of a model file in it (behaviour inferred). */
void *fn_800122D4(void);
s32 fn_80012700(void *manager, int arg1, const char *fileName);

/* Gives the attacker the reward for breaking the object (behaviour inferred). */
void fn_8020642C(const boost::shared_ptr<CharInfo> &attacker, int reward);
/* Tells the owner that the gadget is done (behaviour inferred). */
void fn_8015A794(Gadget *gadget);
}

namespace CaptureCage {

/* The three parameters the owner supplies; all are named "N/A" in the parameter table and the cage
 * does not read them (names inferred). */
struct CaptureCageParams {
    CaptureCageParams() : field_0x0(1.0f), field_0x4(1.0f), field_0x8(1.0f) {}

    /* 0x0 */ float field_0x0;
    /* 0x4 */ float field_0x4;
    /* 0x8 */ float field_0x8;
};
SIZE_ASSERT(CaptureCageParams, 0xC);

class CaptureCage : public Gadget {
  public:
    CaptureCage(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~CaptureCage();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the cage under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position, rotation and parameters and restarts the cage (name
     * inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Places the model at the character info's translation and Y rotation (name inferred). */
    void RebuildTransform();
    /* The attack callback of the model's collision (name inferred). */
    void OnAttack(AttackCallbackParam &param);
    /* Spawns the debris of the broken cage (name inferred). */
    void Break();
    /* The model's collision callback: captures the nearest character (name inferred). */
    void OnColliDetect(ColliDetectArgs &args);

    static const char *s_fileName;       /* (name inferred) */
    static const char *s_debrisFileName; /* (name inferred) */
    static const char *s_taskName;

    /* Member names and meanings are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x40 */ boost::weak_ptr<CharInfo> m_captured;
    /* 0x48 */ CaptureCageParams m_params;
    /* 0x54 */ s32 m_state; /* 0 new, 1 intact, 2 broken, 3 fading out, 4 done */
    /* 0x58 */ float m_health;
    /* 0x5C */ float m_timer; /* time left to fade out */
    /* 0x60 */ u32 m_cageFlags; /* bit 0: holding a character, 1: broken, 2: fading */
    /* 0x64 */ u32 pad_0x64; /* the class size is a multiple of 8 */
};
SIZE_ASSERT(CaptureCage, 0x68);

/* What the collision callback passes to the per-character hit function (name inferred). */
struct CaptureCageHitContext {
    CaptureCageHitContext() : m_nearestDistance(100000000.0f), m_found(false) {}

    /* 0x00 */ Vec3f m_position;          /* the cage's position */
    /* 0x0C */ boost::weak_ptr<CharInfo> m_nearest;
    /* 0x14 */ float m_nearestDistance;           /* squared */
    /* 0x18 */ bool m_found;
};
SIZE_ASSERT(CaptureCageHitContext, 0x1C);

const char *CaptureCage::s_fileName = "CAPTURECAGE.BIN";
const char *CaptureCage::s_debrisFileName = "CAPTCAGECRSH01.DFF";
const char *CaptureCage::s_taskName = "CaptureCage";

/* The hit function of OnColliDetect: keeps the nearest character hit (name inferred). */
static void HitCharacter(boost::shared_ptr<CharInfo> hit, void *user) {
    CaptureCageHitContext *context = static_cast<CaptureCageHitContext *>(user);

    if (context != 0) {
        Vec3f offset;
        fn_8040CD74(&offset, &context->m_position, &hit->m_position);
        float distance = fn_8040CF2C(&offset);
        if (distance < context->m_nearestDistance) {
            context->m_nearestDistance = distance;
            context->m_nearest = hit;
            context->m_found = true;
        }
    }
}

CaptureCage::CaptureCage(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    fn_80014E04(&m_cageFlags);
    m_name = s_taskName;
    m_chara = fn_8015CB88(0x16, 4, "CaptureCage");
    ReadPlacement(owner);
    m_model = fn_8015F800(0, s_fileName);
    m_model->fn_80162A68(0, 0);
    if (fn_80161FC8(m_model.get(), 0, m_chara)) {
        boost::weak_ptr<CharaColliAttack> attack = fn_80161428(m_model.get(), 0);
        if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
            collision->field_0x54 = 0;
            collision->field_0x55 = 1;
            collision->m_attackCallback =
                AttackCallback(boost::bind(&CaptureCage::OnAttack, this, _1));
        }
    }
    fn_800D0B64(m_model->field_0x4, boost::bind(&CaptureCage::OnColliDetect, this, _1));
    fn_80161FC8(m_model.get(), 1, m_chara);
    RebuildTransform();
}

CaptureCage::~CaptureCage() {}

void CaptureCage::Behave(float dt) {
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }

    switch (m_state) {
    case 0:
        m_state = 1;
        break;
    case 2:
        m_model->fn_80161880();
        m_model->fn_80162804();
        Break();
        m_model->fn_80162A68(1, 0);
        fn_80161FC8(m_model.get(), 1, m_chara);
        RebuildTransform();
        m_timer = 10.0f;
        m_state = 3;
        if (!m_captured.expired()) {
            if (boost::shared_ptr<CharInfo> captured = m_captured.lock()) {
                captured->PushCommand(new ReleaseCommand(boost::weak_ptr<CharInfo>(m_chara)));
            }
            m_captured.reset();
            fn_8006127C(&m_cageFlags, 0);
        }
        fn_80014E1C(&m_cageFlags, 1);
        break;
    case 3:
        m_timer -= dt;
        if (!fn_80014E34(&m_cageFlags, 2) && m_timer <= 2.0f) {
            fn_8016268C(m_model.get(), -1.0f);
            fn_80014E1C(&m_cageFlags, 2);
        }
        if (m_timer <= 0.0f) {
            m_state = 4;
        }
        break;
    case 4:
        fn_8015A794(this);
        m_flags |= 1;
        break;
    }
}

void CaptureCage::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
    RebuildTransform();
    if (m_model.get() != 0) {
        m_model->fn_8016108C();
    }
}

void CaptureCage::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_state = 0;
    m_health = 5.0f;
    m_timer = 0.0f;
}

void CaptureCage::RebuildTransform() {
    if (m_model.get() != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        m_model->fn_801633DC(&matrix);
    }
}

void CaptureCage::OnAttack(AttackCallbackParam &param) {
    param.m_result = 5;
    if (m_state == 1) {
        if (boost::shared_ptr<CharaColliAttack> collision = param.m_attacker.lock()) {
            if (collision->m_flags & 3) {
                param.m_result = 5;
                return;
            }
            m_health -= param.m_damage;
            if (m_health <= 0.0f) {
                param.m_result = 3;
                m_state = 2;
                if (boost::shared_ptr<CharInfo> attacker = collision->m_owner.lock()) {
                    fn_8020642C(attacker, 0x1F);
                }
            } else {
                param.m_result = 1;
            }
            return;
        }
    }
}

void CaptureCage::Break() {
    s32 model = fn_80012700(fn_800122D4(), 0, s_debrisFileName);
    if (model != 0) {
        UnkDebrisParam_8015F0B8 debris(m_chara, 0);

        debris.position.y += 20.0f;
        debris.model = model;
        debris.scale = 1.3f;
        debris.field_0x54 = 1;
        debris.field_0x4C = 0x96;
        debris.field_0x40 = m_chara->m_position.y;
        debris.field_0x4A = 1;
        fn_8015D93C(&debris);
    }
}

void CaptureCage::OnColliDetect(ColliDetectArgs &args) {
    if (fn_80014E34(&m_cageFlags, 1)) {
        return;
    }
    if (fn_80014E34(&m_cageFlags, 0)) {
        return;
    }

    CaptureCageHitContext context;
    fn_800091D0(&context.m_position, &m_chara->m_position);
    fn_8015B70C(args, HitCharacter, &context, 3);
    if (context.m_found) {
        m_captured = context.m_nearest;
        if (boost::shared_ptr<CharInfo> captured = m_captured.lock()) {
            captured->PushCommand(new BindCommand(boost::weak_ptr<CharInfo>(m_chara)));
        }
    }
}

} // namespace CaptureCage

extern "C" void fn_800F34E0(void) {
    fn_8006B474(CaptureCage::CaptureCage::s_fileName, 1);
}

extern "C" void fn_800F34DC(void) {}

namespace CaptureCage {

void CaptureCage::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new CaptureCage(parent, owner);
}

} // namespace CaptureCage

static GadgetParamDesc lbl_80525078[] = {
    {4, "N/A", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "N/A", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "N/A", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80525118 = {
    "CAPTURE_CAGE", fn_800F34E0, fn_800F34DC, CaptureCage::CaptureCage::Instantiate, 0, 0, 0x37, 1, 0, 10,
    lbl_80525078,
};
