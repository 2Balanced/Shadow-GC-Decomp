/* Weight::Weight (class name from RTTI): the WEIGHT gadget, a heavy block (WEIGHT.BIN) that drops
 * and rises again. Of the "Piston" type it cycles on its own: it waits "intv upper(sec)" at the
 * top, falls by "height", waits "intv lower(sec)" at the bottom and rises again. Of the "Fall" type
 * it waits at the top until a character touches its trigger collider, then falls. Characters it
 * lands on are hurt and held under it (StartExternalControlCommand) until it is back at the top.
 * With "sw" enabled it only runs once the switch whose id the owner supplies is on. Attacks wear it down; a
 * worn-down weight pauses for "intv damage(sec)". The model and both colliders are scaled by the
 * "scale X/Y/Z" parameters. File name inferred from the class. Names marked "(name inferred)" are
 * not original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). The unit's code reaches its .data and .bss through pooled base addresses,
 * so the linker keeps those sections whole. */
/* Retail's .data keeps the RTTI base lists of EndExternalControlCommand's weak vtable, which an
 * in-class constructor emits (see Game/GadgetCharCommands.h). */
#define GADGETCHARCOMMANDS_INLINE_END_EXTERNAL_CONTROL
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharCommands.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetColliDetectArgs.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

/* The handle fn_8015B8C4 wraps a collider in (class name unknown; only the collider is mapped, as
 * in ElecCircuitColli). */
class GadgetCharCollision {
  public:
    /* 0x00 */ u8 field_0x0[0x24];
    /* 0x24 */ CharaColliBase *m_collider;
};

extern "C" {
/* A 32-bit flag word: fn_80014E04 constructs it cleared, fn_80014E10 clears it, fn_80014E1C sets
 * bit, fn_8006127C clears bit and fn_80014E34 tests bit (behaviour from their bodies). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
/* Loads the model file fileName for a gadget (behaviour inferred; arg0 unknown). */
boost::shared_ptr<GadgetModelInfo> fn_8015F800(int arg0, const char *fileName);
/* Returns the attack collision of a model's node (behaviour inferred). */
boost::weak_ptr<CharaColliAttack> fn_80161334(GadgetModelInfo *model, const char *nodeName);
/* Passes offset to each part of the model whose id equals arg1 (behaviour inferred; meanings of
 * the integer arguments unknown). */
void fn_80161740(GadgetModelInfo *model, int arg1, const Vec3f *offset, int arg3);
/* Scales the matrix's three rotation columns by the vector's components (behaviour inferred). */
void fn_8040B790(Mtx34f *matrix, const Vec3f *scale);
/* The collider manager (behaviour inferred). */
void *fn_80021B08(void);
/* Creates a box collider of the given half size for charInfo (behaviour inferred; the meanings of
 * the integer arguments are unknown). */
boost::shared_ptr<CharaColliBase> fn_800C7C34(void *manager, const boost::weak_ptr<CharInfo> &charInfo,
                                              const Vec3f *size, int flags, int arg4, int arg5,
                                              const Vec3f *offset, const Vec3f *rotation,
                                              int arg8);
/* Creates a box collider at a position (behaviour inferred, as in FallRoad; the meanings of the
 * integer arguments are unknown). */
boost::shared_ptr<CharaColliBase>
fn_800C7D04(void *manager, GadgetCharInfo *charInfo, const Vec3f &size,
            const Vec3f &rotation, int arg4, int arg5, int arg6, int arg7,
            const boost::weak_ptr<CharInfo> &owner, const Vec3f &offset,
            const Vec3f &arg10, int arg11);
/* A vector constant read by the collision setup (meaning unknown). */
extern Vec3f lbl_805E07B8;
/* Wraps collider in a new handle (behaviour inferred). */
boost::shared_ptr<GadgetCharCollision> fn_8015B8C4(boost::shared_ptr<CharaColliBase> collider);
/* Re-registers the handle's collider, and advances the handle by dt (behaviour inferred). */
void fn_8015BE64(GadgetCharCollision *handle);
void fn_8015BE9C(GadgetCharCollision *handle, float dt);
/* Calls callback with every character in the collision event (behaviour inferred). */
void fn_8015AA84(ColliDetectArgs &args, void (*callback)(boost::shared_ptr<CharInfo> hit, void *user),
                 void *user);
/* Whether a character is in the collision event (behaviour inferred). */
bool fn_8015AC84(ColliDetectArgs &args);
/* Plays sound id at the character info's position (behaviour inferred; the meanings of the last
 * two arguments are unknown). */
void fn_801D54F4(void *manager, int id, GadgetCharInfo *charInfo, int arg3, int arg4);
}

/* The empty character info (an .sbss shared_ptr; meaning inferred). */
extern boost::shared_ptr<CharInfo> lbl_805EE698;

namespace Weight {

/* The parameters the owner supplies (names from the parameter table). */
struct WeightParams {
    WeightParams()
        : type(0), height(0.0f), intervalUpper(0.0f), intervalLower(0.0f), intervalDamage(0.0f),
          sw(0), scaleX(1.0f), scaleY(1.0f), scaleZ(1.0f) {}

    /* 0x00 */ int type; /* 0 piston, 1 fall */
    /* 0x04 */ float height;
    /* 0x08 */ float intervalUpper;  /* seconds */
    /* 0x0C */ float intervalLower;  /* seconds */
    /* 0x10 */ float intervalDamage; /* seconds */
    /* 0x14 */ int sw;               /* 0 disable, 1 enable */
    /* 0x18 */ float scaleX;
    /* 0x1C */ float scaleY;
    /* 0x20 */ float scaleZ;
};
SIZE_ASSERT(WeightParams, 0x24);

/* A 32-bit flag word (class name unknown; bit 0: a character is held). */
struct WeightFlags {
    WeightFlags() {
        fn_80014E04(&m_bits);
    }
    void Clear() {
        fn_80014E10(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    void Reset(int bit) {
        fn_8006127C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits;
};

class Weight : public Gadget {
  public:
    Weight(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~Weight();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the weight under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Method names are inferred. */
    /* The model's attack callback. */
    void OnAttack(AttackCallbackParam &param);
    /* The callbacks of the press collider and of the trigger collider. */
    void OnPressDetect(ColliDetectArgs &args);
    void OnTriggerDetect(ColliDetectArgs &args);
    /* Scales the model and the colliders by the scale parameters. */
    void UpdateScale();
    /* Places the model and the trigger collider. */
    void RebuildTransform();
    /* Copies the owner's position, rotation, parameters and switch id. */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetModelInfo> m_model;
    /* 0x38 */ WeightParams m_params;
    /* 0x5C */ s32 m_state; /* 0 start, 1/2 waiting for the switch, 3 up, 4 down, 5 rising,
                               6 falling, 7 damaged, 8 still */
    /* 0x60 */ s32 m_prevState;
    /* 0x64 */ float m_timer;
    /* 0x68 */ float m_baseY;
    /* 0x6C */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x74 */ boost::shared_ptr<GadgetCharCollision> m_pressHandle;
    /* 0x7C */ boost::shared_ptr<GadgetCharCollision> m_triggerHandle;
    /* 0x84 */ boost::weak_ptr<CharInfo> m_held;
    /* 0x8C */ float m_durability;
    /* 0x90 */ u8 m_switchId;
    /* 0x94 */ WeightFlags m_flags;
};

/* What the press callback passes to the per-character hit function (name inferred). */
struct WeightHitContext {
    WeightHitContext() : m_height(0.0f), m_hit(false) {}

    /* 0x00 */ boost::weak_ptr<GadgetCharInfo> m_chara;
    /* 0x08 */ float m_height; /* the owner's height */
    /* 0x0C */ boost::weak_ptr<CharInfo> m_target;
    /* 0x14 */ bool m_hit;
};
SIZE_ASSERT(WeightHitContext, 0x18);

} // namespace Weight

SIZE_ASSERT(Weight::Weight, 0x98);

/* The half sizes of the model, of the trigger collider and of the press collider (names
 * inferred). The unit's .bss is kept whole (the startup code reaches it through one base), so it
 * also holds two objects nothing uses: 0xC bytes before the sizes (as in Helicopter.cpp) and 0xC8
 * bytes after them (their types are unknown). */
static Vec3f s_unused;
static Vec3f s_modelSize(30.0f, 15.0f, 20.0f);
static Vec3f s_triggerSize(30.0f, 15.0f, 20.0f);
static Vec3f s_pressSize(28.0f, 15.0f, 18.0f);
static u8 s_unusedBlock[0xC8];

namespace Weight {

const char *Weight::s_taskName = "Weight";

/* Holds a character the weight lands on under it (name inferred). Retail emits it after the
 * constructor and its template instances, so it is defined before the constructor. */
static void HitCharacter(boost::shared_ptr<CharInfo> hit, void *user) {
    WeightHitContext *context = static_cast<WeightHitContext *>(user);
    boost::shared_ptr<GadgetCharInfo> self = context->m_chara.lock();

    if (self.get() == 0) {
        return;
    }
    if (hit.get() != 0) {
        Vec3f position;
        fn_800091EC(&position, &hit->m_position);
        position.y = 1.0f + context->m_height;
        hit->PushCommand(new SetPositionCommand(position, 4));
        hit->PushCommand(new DamageCommand(4, 1.0f));
        hit->PushCommand(new Player::StartExternalControlCommand(0, 4));
        hit->PushCommand(new ClrVelocityCommand(4));
        context->m_target = hit;
        context->m_hit = true;
    }
}

Weight::Weight(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x27, 4);
    LoadFromOwner(m_owner);
    m_model = fn_8015F800(0, "WEIGHT.BIN");
    m_model->fn_80162A68(0, 0);
    m_model->fn_80161F08("col", m_chara);
    boost::weak_ptr<CharaColliAttack> attack = fn_80161334(m_model.get(), "col");
    if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
        collision->field_0x54 = 0;
        collision->field_0x55 = 1;
        collision->m_attackCallback = AttackCallback(boost::bind(&Weight::OnAttack, this, _1));
    }
    {
        Vec3f size;
        ConstructVector(&size, s_pressSize);
        boost::shared_ptr<CharaColliBase> collider =
            fn_800C7C34(fn_80021B08(), m_chara, &size, 4, 7, 0, &lbl_805E07A0, &lbl_805E07B8, 0);
        m_pressHandle = fn_8015B8C4(collider);
        fn_800F5980(m_pressHandle.get(), boost::bind(&Weight::OnPressDetect, this, _1));
    }
    {
        Vec3f size;
        ConstructVector(&size, s_triggerSize);
        boost::shared_ptr<CharaColliBase> collider =
            fn_800C7D04(fn_80021B08(), m_chara.get(), size, m_chara->m_rotation, 2, 0x40000, 7,
                        0, lbl_805EE698, lbl_805E07A0, lbl_805E07B8, 0);
        m_triggerHandle = fn_8015B8C4(collider);
        fn_800F5980(m_triggerHandle.get(), boost::bind(&Weight::OnTriggerDetect, this, _1));
    }
    UpdateScale();
    m_flags.Clear();
    RebuildTransform();
}

Weight::~Weight() {}

void Weight::Behave(float dt) {
    switch (m_state) {
    case 0:
        if (m_params.sw == 0) {
            m_state = 3;
        } else {
            m_chara->m_position.y = m_baseY;
            m_state = 2;
        }
        break;
    case 1:
        if (fn_8016904C(fn_800CB69C(), m_switchId) == 1) {
            m_timer = 0.0f;
            m_state = 3;
        }
        break;
    case 2:
        if (fn_8016904C(fn_800CB69C(), m_switchId) == 1) {
            m_timer = 0.0f;
            m_state = 4;
        }
        break;
    case 3:
        if (m_params.type == 0) {
            m_timer -= dt;
            if (m_timer <= 0.0f) {
                m_state = 6;
            }
        }
        break;
    case 4:
        m_timer -= dt;
        if (m_timer <= 0.0f) {
            m_state = 5;
            if (m_pressHandle.get() != 0) {
                m_pressHandle->m_collider->TurnOff(-1);
            }
        }
        break;
    case 5: {
        m_chara->m_position.y += 1.0f;
        float top = m_baseY + m_params.height;
        if (m_chara->m_position.y >= top) {
            m_chara->m_position.y = top;
            m_state = 3;
            m_timer = m_params.intervalUpper;
            if (m_flags.Test(0)) {
                if (boost::shared_ptr<CharInfo> held = m_held.lock()) {
                    held->PushCommand(new Player::EndExternalControlCommand(4));
                }
                m_flags.Reset(0);
            }
            if (m_params.sw == 1 && fn_8016904C(fn_800CB69C(), m_switchId) != 1) {
                m_state = 1;
            }
            if (m_pressHandle.get() != 0) {
                m_pressHandle->m_collider->TurnOn(-1);
            }
        }
        break;
    }
    case 6:
        m_chara->m_position.y -= 2.5f;
        if (m_chara->m_position.y <= m_baseY) {
            m_chara->m_position.y = m_baseY;
            m_state = 4;
            m_timer = m_params.intervalLower;
            fn_801D54F4(fn_80053164(), 0x1019, m_chara.get(), 0, 1);
        }
        break;
    case 7:
        m_timer -= dt;
        if (m_timer <= 0.0f) {
            m_state = m_prevState;
            if (m_state == 4) {
                m_timer = m_params.intervalLower;
            } else if (m_state == 3) {
                m_timer = m_params.intervalUpper;
            }
        }
        break;
    case 8:
        break;
    }

    RebuildTransform();
    if (m_model.get() != 0) {
        m_model->fn_80162FB0(dt);
    }
    if (m_pressHandle.get() != 0) {
        fn_8015BE9C(m_pressHandle.get(), dt);
    }
    if (m_triggerHandle.get() != 0) {
        fn_8015BE9C(m_triggerHandle.get(), dt);
    }
}

void Weight::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    RebuildTransform();
    if (m_model.get() != 0) {
        UpdateScale();
        m_model->fn_80162EF4();
        m_model->fn_8016108C();
    }
    if (m_pressHandle.get() != 0) {
        fn_8015BE64(m_pressHandle.get());
    }
    if (m_triggerHandle.get() != 0) {
        fn_8015BE64(m_triggerHandle.get());
    }
}

void Weight::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    fn_800C9E60(owner, &m_switchId);
    m_baseY = m_chara->m_position.y;
    m_chara->m_position.y += m_params.height;
    m_timer = 0.0f;
    m_durability = 10.0f;
    m_flags.Clear();
    if (m_params.height <= 0.0f) {
        m_state = 8;
        m_prevState = 8;
    } else {
        m_state = 0;
        m_prevState = 0;
    }
}

void Weight::RebuildTransform() {
    if (m_model.get() != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B790(&matrix, &(const Vec3f &)Vec3f(m_params.scaleX, m_params.scaleY,
                                                                    m_params.scaleZ));
        m_model->fn_801633DC(&matrix);
    }
    if (m_triggerHandle.get() != 0) {
        Vec3f position;
        Vec3f rotation(0.0f, 0.0f, 0.0f);

        if (m_owner != 0) {
            fn_800C9F14(m_owner, &position);
            fn_800C9ED4(m_owner, &rotation);
        }
        m_triggerHandle->m_collider->SetCharInfo(reinterpret_cast<CharInfo *>(&position));
        m_triggerHandle->m_collider->SetRotation(&rotation);
    }
}

void Weight::UpdateScale() {
    if (m_model.get() != 0) {
        Vec3f size;
        ConstructVector(&size, s_modelSize);
        size.x *= m_params.scaleX;
        size.y *= m_params.scaleY;
        size.z *= m_params.scaleZ;
        m_model->fn_801617D0(0, -2, size.x, size.y, size.z);
        Vec3f offset(0.0f, size.y, 0.0f);
        fn_80161740(m_model.get(), 0, &offset, -2);
    }
    if (m_pressHandle.get() != 0) {
        Vec3f size;
        ConstructVector(&size, s_pressSize);
        size.x *= m_params.scaleX;
        size.z *= m_params.scaleZ;
        m_pressHandle->m_collider->SetSize(size.x, size.y, size.z, -2);
    }
    if (m_triggerHandle.get() != 0) {
        Vec3f size;
        ConstructVector(&size, s_triggerSize);
        size.x *= m_params.scaleX;
        size.y = 0.5f * m_params.height;
        size.z *= m_params.scaleZ;
        m_triggerHandle->m_collider->SetSize(size.x, size.y, size.z, -2);
        Vec3f offset(0.0f, size.y, 0.0f);
        m_triggerHandle->m_collider->SetOffset(&offset, -2);
    }
}

void Weight::OnPressDetect(ColliDetectArgs &args) {
    if (m_state == 6) {
        WeightHitContext context;
        Vec3f position;

        context.m_chara = m_chara;
        fn_800C9F14(m_owner, &position);
        context.m_height = position.y;
        fn_8015AA84(args, HitCharacter, &context);
        if (context.m_hit) {
            m_flags.Set(0);
            m_held = context.m_target;
        }
    }
}

void Weight::OnTriggerDetect(ColliDetectArgs &args) {
    if (m_params.type == 1 && m_state == 3) {
        if (fn_8015AC84(args)) {
            m_state = 6;
        }
    }
}

void Weight::OnAttack(AttackCallbackParam &param) {
    if (m_state != 7) {
        if (boost::shared_ptr<CharaColliAttack> attacker = param.m_attacker.lock()) {
            if (attacker->m_flags & 2) {
                param.m_result = 5;
                return;
            }
        }
        param.m_result = 1;
        m_durability -= param.m_damage;
        if (m_durability <= 0.0f) {
            m_prevState = m_state;
            m_state = 7;
            m_durability = 10.0f;
            m_timer = m_params.intervalDamage;
        }
    } else {
        param.m_result = 5;
    }
}

} // namespace Weight

extern "C" void fn_8012AC28(void) {
    fn_8006B474("WEIGHT.BIN", 1);
}

extern "C" void fn_8012AC24(void) {}

void Weight::Weight::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new Weight(parent, owner);
}

/* The fields of the cursor that the label callback reads (layout from its accesses). */
struct WeightParamCursor {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ int index; /* the parameter */
    /* 0x8 */ int value;
};

/* The labels of the type and sw values. */
static const char *lbl_805E5DF4[] = {
    "type : Piston",
    "type : Fall",
};

static const char *lbl_805E5DFC[] = {
    "switch : disable",
    "switch : enable",
};

/* The label callback of the type and sw parameters. */
extern "C" const char *fn_8012AB88(UnkLabelQuery_800CBC20 *cursor) {
    const WeightParamCursor *fields = (const WeightParamCursor *)cursor;

    if (fields->index == 0) {
        return lbl_805E5DF4[fields->value];
    }
    if (fields->index == 5) {
        return lbl_805E5DFC[fields->value];
    }
    return 0;
}

static GadgetParamDesc lbl_805281B4[] = {
    {1, "type", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_8012AB88, 0},
    {4, "height", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "intv upper(sec)", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "intv lower(sec)", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "intv damage(sec)", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {1, "sw", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_8012AB88, 0},
    {4, "scale X", 0, 0, 0, 1.0f, 1.0f, 65536.0f, fn_800CBC20, 0},
    {4, "scale Y", 0, 0, 0, 1.0f, 1.0f, 65536.0f, fn_800CBC20, 0},
    {4, "scale Z", 0, 0, 0, 1.0f, 1.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80528344 = {
    "WEIGHT", fn_8012AC28, fn_8012AC24, Weight::Weight::Instantiate, 0, 0, 0x19, 1, 0, 10, lbl_805281B4,
};
