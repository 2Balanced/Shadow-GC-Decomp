/* RegularRing::Ring (class name from RTTI): the RING gadget, a set of "num" rings laid out from the
 * gadget's position by "type": all at the position (normal), in a line of "param0" length (liner),
 * on a circle of "param0" radius (circle) or along an arc of "param0" length and "param1" height
 * (parabola). Each ring has a sphere collider and a Target; a character that touches a ring, or
 * a VacuumCommand or UseCommand naming its target, collects it. With "ghost" set, collected rings
 * stay as ghosts (drawn with the second model) instead of disappearing. A positive switch id keeps
 * the rings hidden until that switch turns on. The rings are drawn through a Render callback.
 * When the gadget goes away it leaves a RegularRing::RingState (class name from RTTI) with its
 * owner recording which rings are left, which the next gadget for that owner reads back. File
 * name from the namespace. Names marked "(name inferred)" are not original. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first).
 *
 * Not yet matching (64 of 66 functions match; the data matches):
 * - OnCommand: GC/2.7 hoists the address of the command's target (command + 0x14 / 0x1C) out of
 *   both ring loops into a sixth saved register; the original computes it inside the loops. No
 *   source shape tried (references, pointer casts, &&, continue, a local) stops the hoist, so the
 *   function is 8 bytes longer.
 * - LayoutRings: same code, but the "liner" step lives in f31 instead of the original's f30.
 * Both look like the code-generation differences of the original compiler described in
 * docs/research-notes.md. */
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <cmath>

#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Effect/System/EffectElement.h"
#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/Render.h"
#include "Game/SetAdapterState.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::CharaColliBase;

/* The RenderWare types and functions used here, declared locally because the RenderWare headers
 * need the C library headers (layouts as in rw/rwcore.h and rw/rpworld.h, reduced to the members
 * used; as in Door.cpp). */
extern "C" {
typedef struct RwV3d {
    float x;
    float y;
    float z;
} RwV3d;

typedef struct RwSphere {
    RwV3d center;
    float radius;
} RwSphere;

typedef struct RwFrame RwFrame;
typedef struct RwCamera RwCamera;

typedef struct RwObject {
    u8 type;
    u8 subKind;
    u8 flags;
    u8 privateFlags;
    void *parent;
} RwObject;

typedef struct RpClump {
    /* 0x00 */ RwObject object; /* the parent is the clump's frame */
} RpClump;

typedef struct RwGlobals {
    /* 0x000 */ RwCamera *curCamera;
} RwGlobals;

/* the engine instance */
extern RwGlobals *lbl_805F15B4;

/* whether a sphere is in a camera's view frustum (inferred) */
int fn_804856C8(const RwCamera *camera, const RwSphere *sphere);
/* translates a frame */
RwFrame *fn_804875B0(RwFrame *frame, const RwV3d *translation, int combine);
/* Renders a clump. */
RpClump *fn_80456BC4(RpClump *clump);
/* Turns a frame about its y axis (behaviour inferred; the last argument is the combine mode). */
void fn_800E9CEC(RwFrame *frame, float angle, int combine);
}

/* Target (class name from RTTI): a point other objects can aim at (meaning inferred). Only the
 * flag word is mapped here. */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);

    /* Flag bit 0 makes the target active (names and meaning inferred). */
    void TurnOn() {
        m_flags |= 1;
    }
    void TurnOff() {
        m_flags &= ~1;
    }
    void SetFlags(u32 flags) {
        m_flags = flags;
    }

    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ u32 m_flags;
};

/* The renderer state the ring drawing sets up and restores (class name unknown). */
struct UnkRenderer_800289B0 {
    /* 0x000 */ u8 field_0x0[0x456];
    /* 0x456 */ u8 field_0x456;
};

extern "C" {
/* A 32-bit flag word: fn_80014E04 constructs it cleared, fn_80014E10 clears it, fn_80014E1C sets
 * bit, fn_8006127C clears bit and fn_80014E34 tests bit (behaviour from their bodies). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);

/* The runtime's array construction (MSL); RingState builds its flag words through it with their
 * out-of-line constructor fn_80014E04. */
void __construct_array(void *block, void *ctor, void *dtor, size_t size, size_t n);

/* Returns the file loader, and the clump id of it (behaviour inferred). */
void *fn_800122D4(void);
RpClump *fn_8001277C(void *loader, int arg1, int id);

/* Sets a vector to the constant at 0x8051A4E8 (behaviour from its body). */
void fn_8040E970(Vec3f *vector);

/* Gives the character info a command handler unless it has one (behaviour inferred). */
void fn_8015CF7C(GadgetCharInfo *charInfo);

/* The collider manager, a sphere collider at position, and the removal of a collider (behaviour
 * inferred; the meanings of the integer arguments are unknown). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7EA8(void *manager, const Vec3f *position,
                                              u32 flags, int arg3, int arg4,
                                              const boost::weak_ptr<CharInfo> &owner,
                                              const Vec3f *offset, float radius);
void fn_800C78B0(void *manager, boost::shared_ptr<CharaColliBase> *collider);

/* Creates a target for owner with the given flags, and removes a gadget's target (behaviour
 * inferred). */
boost::shared_ptr<Target> fn_801E7128(const boost::weak_ptr<CharInfo> &owner, int flags);
void fn_801E7108(boost::shared_ptr<Target> *target);

/* Reports a collected ring at position and rotation to the command's sender (behaviour
 * inferred). */
void fn_80238A78(CharCommand *command, const Vec3f *position,
                 const Vec3f *rotation, int count);
/* Gives target the item id, and sends message to the character (behaviour inferred). */
void fn_8015F6EC(boost::shared_ptr<CharInfo> target, int id, int arg2, int arg3);
int fn_8020642C(const boost::shared_ptr<CharInfo> &character, int message);
/* Plays the ring sound on the sound manager (behaviour inferred). */
void fn_801D5300(void *sound);
/* Launches effect id at position (behaviour inferred). */
boost::weak_ptr<Effect::System::EffectElement> fn_8015D794(const Vec3f *position, int id);

/* Owner helpers (behaviour inferred): the switch id the owner was placed with, and telling the
 * owner that the gadget is done (fn_8015A7C0 keeping it for the ghosts). */
void fn_800C9E50(UnkPlacement_800C9CD0 *owner, s32 *id);
void fn_8015A794(Gadget *gadget);
void fn_8015A7C0(Gadget *gadget);

/* The renderer, and its state changes around the rings (behaviour inferred). */
UnkRenderer_800289B0 *fn_800289B0(void);
void fn_80048390(UnkRenderer_800289B0 *renderer, int mode);
void fn_800482E0(UnkRenderer_800289B0 *renderer, int value);
void fn_80047780(UnkRenderer_800289B0 *renderer);
}

/* The empty character info (an .sbss shared_ptr; meaning inferred). */
extern boost::shared_ptr<CharInfo> lbl_805EE698;

/* The cursor handed to a parameter's label callback (layout as in SetVehicle and Switch). */
class UnkLabelQuery_800CBC20 {
  public:
    /* 0x0 */ const GadgetParamDesc *params;
    /* 0x4 */ s32 index; /* the parameter being shown */
    /* 0x8 */ s32 value; /* its current value */
};

/* The contacts of a collider (the object holds them 0xC bytes in; as in Door.cpp). */
inline UnkColliContacts_8001EE04 *GetContacts(CharaColliBase *collider) {
    return reinterpret_cast<UnkColliContacts_8001EE04 *>(reinterpret_cast<u8 *>(collider) + 0xC);
}

namespace RegularRing {

/* A 32-bit flag word (the class name is unknown; its out-of-line members are the fn_80014E04
 * family above). */
struct RingFlags {
    RingFlags() {
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

/* One ring (name and member names inferred). Flag bit 1: the ring is there; bit 2: it is a ghost;
 * bit 3: it is hidden for m_hideTimer seconds. */
struct RingElement {
    RingElement();

    /* Hides the ring and its target and collider for time seconds. */
    void Hide(float time);
    /* Shows the ring again once its hide time is over. */
    void Tick(float dt);

    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ Vec3f field_0xC;
    /* 0x18 */ Vec3f m_rotation;
    /* 0x24 */ float m_phase; /* of the bobbing turn */
    /* 0x28 */ float field_0x28;
    /* 0x2C */ float field_0x2C;
    /* 0x30 */ RpClump *m_clump;
    /* 0x34 */ RpClump *m_ghostClump;
    /* 0x38 */ boost::shared_ptr<Target> m_target;
    /* 0x40 */ boost::shared_ptr<CharaColliBase> m_collider;
    /* 0x48 */ float m_hideTimer;
    /* 0x4C */ RingFlags m_flags;
};
SIZE_ASSERT(RingElement, 0x50);

/* The state the owner keeps: one bit per ring that is left (class name from RTTI). */
class RingState : public SetAdapterState {
  public:
    RingState(RingElement *elements, int count);
    virtual ~RingState() {}

    /* Marks the rings that were collected: removed, or ghosts when ghost is set (name
     * inferred). */
    void Apply(RingElement *elements, int count, bool ghost);

    /* 0x04 */ u32 m_bits[4];
};
SIZE_ASSERT(RingState, 0x14);

/* The parameters the owner supplies (names from the parameter table). */
struct RingParams {
    RingParams() : type(0), num(1), param0(0.0f), param1(0.0f), ghost(0) {}

    /* 0x00 */ s32 type; /* 0 normal, 1 liner, 2 circle, 3 parabola */
    /* 0x04 */ s32 num;
    /* 0x08 */ float param0; /* length or radius */
    /* 0x0C */ float param1; /* height */
    /* 0x10 */ s32 ghost;
};
SIZE_ASSERT(RingParams, 0x14);

/* What the collision callback passes to the hit function (name inferred). */
struct RingHitContext {
    RingHitContext() : m_ghost(false), m_hit(false) {}

    /* 0x0 */ bool m_ghost;
    /* 0x1 */ bool m_hit;
};

class Ring : public Gadget {
  public:
    Ring(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~Ring();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Method names are inferred. */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    bool CheckHit(ColliDetectArgs &args, bool ghost);
    void CreateRings(const RingParams *params);
    void FreeRings();
    void LayoutRings(const RingParams *params);
    void CheckContacts();
    void UpdateTargets();
    void ResetColliders();
    void Draw(RednerDrawArgs &args);
    void OnCommand(CharCommand &command);
    void EnableTargets();
    void DisableTargets();

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ RingElement *m_elements;
    /* 0x34 */ s32 m_count;
    /* 0x38 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x40 */ RingParams m_params;
    /* 0x54 */ boost::shared_ptr<Render> m_render;
    /* 0x5C */ s32 m_switchId;
    /* 0x60 */ s32 m_state; /* 0 new, 1 waiting for the switch, 2 shown */
    /* 0x64 */ RingFlags m_status; /* bit 0: hidden */
};
SIZE_ASSERT(Ring, 0x68);

const char *Ring::s_taskName = "Ring";

RingElement::RingElement()
    : m_rotation(0.0f, 0.0f, 0.0f), m_phase(0.0f), field_0x28(0.0f), field_0x2C(0.0f), m_clump(0),
      m_ghostClump(0), m_hideTimer(0.0f) {
    m_flags.Set(1);
}

void RingElement::Tick(float dt) {
    if (m_flags.Test(3)) {
        m_hideTimer -= dt;
        if (m_hideTimer <= 0.0f) {
            if (m_target.get() != 0) {
                m_target->TurnOn();
            }
            if (m_collider.get() != 0) {
                m_collider->TurnOn(-1);
            }
            m_flags.Reset(3);
        }
    }
}

void RingElement::Hide(float time) {
    m_flags.Set(3);
    m_hideTimer = time;
    if (m_target.get() != 0) {
        m_target->TurnOff();
    }
    if (m_collider.get() != 0) {
        m_collider->TurnOff(-1);
    }
}

/* Collects the ring for a character that may take it, unless it is a ghost or was already taken
 * in this check (name inferred). */
static void HitCharacter(boost::shared_ptr<CharInfo> hit, void *user) {
    if (hit->vf_0x1C()) {
        RingHitContext *context = static_cast<RingHitContext *>(user);
        if (context != 0) {
            if (!context->m_ghost && !context->m_hit) {
                fn_8015F6EC(hit, 0, 1, 0);
                fn_8020642C(hit, 0);
                fn_801D5300(fn_80053164());
            }
            context->m_hit = true;
        }
    }
}

Ring::Ring(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_elements = 0;
    m_count = 0;
    m_chara = fn_8015CD90(0x26, 4);
    fn_8015CF7C(m_chara.get());
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&Ring::OnCommand, this, _1)));
    LoadFromOwner(m_owner);
    CreateRings(&m_params);
    LayoutRings(&m_params);
    CheckContacts();
    UpdateTargets();
}

Ring::~Ring() {
    if (!fn_800C9870(m_owner)) {
        fn_800C9894(m_owner);
        fn_800C9930(m_owner, new RingState(m_elements, m_count));
    }
    FreeRings();
}

void Ring::Behave(float dt) {
    switch (m_state) {
    case 0:
        if (m_switchId > 0) {
            m_state = 1;
            m_status.Set(0);
            DisableTargets();
        } else {
            m_state = 2;
            m_status.Reset(0);
        }
        break;
    case 1:
        if (fn_8016904C(fn_800CB69C(), m_switchId) == 1) {
            m_state = 2;
            m_status.Reset(0);
            EnableTargets();
        }
        break;
    case 2:
        if (m_elements != 0) {
            bool left = false;
            for (int i = 0; i < m_count; i++) {
                if (m_elements[i].m_flags.Test(1)) {
                    m_elements[i].Tick(dt);
                    m_elements[i].m_phase += 0.01f;
                    m_elements[i].m_rotation.y += 0.05f * std::sin(m_elements[i].m_phase);
                    left = true;
                }
            }
            if (!left) {
                if (m_params.ghost == 1) {
                    fn_8015A7C0(this);
                } else {
                    fn_8015A794(this);
                }
                m_flags |= 1;
            } else {
                CheckContacts();
            }
        }
        break;
    case 3:
        break;
    }
}

void Ring::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    if (m_params.num != m_count) {
        if (m_params.num > 0) {
            FreeRings();
            CreateRings(&m_params);
            LayoutRings(&m_params);
        }
    } else if (m_elements != 0) {
        LayoutRings(&m_params);
    }
    CheckContacts();
    UpdateTargets();
    ResetColliders();
}

void Ring::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, 0x14);
    fn_800C9E50(owner, &m_switchId);
    m_state = 0;
    m_status.Clear();
}

bool Ring::CheckHit(ColliDetectArgs &args, bool ghost) {
    RingHitContext context;

    context.m_ghost = ghost;
    fn_8015B004(args, HitCharacter, &context);
    return context.m_hit;
}

void Ring::CreateRings(const RingParams *params) {
    if (params->num > 0) {
        m_count = params->num;
        m_elements = new RingElement[m_count];
        RpClump *clump = fn_8001277C(fn_800122D4(), 0, 0x7C);
        RpClump *ghostClump = fn_8001277C(fn_800122D4(), 0, 0x5E);
        for (int i = 0; i < m_count; i++) {
            fn_8040CE60(&m_elements[i].m_position);
            fn_8040CE60(&m_elements[i].field_0xC);
            fn_8040E970(&m_elements[i].m_rotation);
            m_elements[i].m_phase = 0.0f;
            m_elements[i].m_clump = clump;
            m_elements[i].m_ghostClump = ghostClump;
        }

        RingState *state = dynamic_cast<RingState *>(m_owner->m_adapterState);
        if (state != 0) {
            if (m_params.ghost == 0) {
                state->Apply(m_elements, m_count, false);
            } else {
                state->Apply(m_elements, m_count, true);
            }
        }

        for (int i = 0; i < m_count; i++) {
            if (m_elements[i].m_flags.Test(1)) {
                if (m_elements[i].m_flags.Test(2)) {
                    m_elements[i].m_target = fn_801E7128(m_chara, 8);
                } else {
                    m_elements[i].m_target = fn_801E7128(m_chara, 0x188);
                }
                m_elements[i].m_collider =
                    fn_800C7EA8(fn_80021B08(), &m_elements[i].m_position, 0x40000, 10, 0,
                                lbl_805EE698, &lbl_805E07A0, 5.0f);
            }
        }

        if (m_render.get() == 0) {
            m_render = fn_8004DFB0(fn_80046178()->field_0x8, "SetRing");
            fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&Ring::Draw, this, _1));
        }
    }
}

void Ring::FreeRings() {
    for (int i = 0; i < m_count; i++) {
        if (m_elements[i].m_target.get() != 0) {
            fn_801E7108(&m_elements[i].m_target);
        }
        if (m_elements[i].m_collider.get() != 0) {
            fn_800C78B0(fn_80021B08(), &m_elements[i].m_collider);
        }
    }
    if (m_elements != 0) {
        delete[] m_elements;
        m_elements = 0;
    }
    m_count = 0;
}

void Ring::LayoutRings(const RingParams *params) {
    if (m_elements == 0) {
        return;
    }

    switch (params->type) {
    case 0:
        for (int i = 0; i < m_count; i++) {
            if (m_elements[i].m_flags.Test(1)) {
                fn_800091D0(&m_elements[i].m_position, &m_chara->m_position);
                m_elements[i].m_rotation.y = m_chara->m_rotation.y;
            }
        }
        break;
    case 1: {
        float step = params->param0 / m_count;
        Mtx34f matrix;
        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B884(&matrix, m_chara->m_rotation.x);
        fn_8040B80C(&matrix, m_chara->m_rotation.z);
        Vec3f offset(0.0f, 0.0f, 0.0f);
        for (int i = 0; i < m_count; i++) {
            if (m_elements[i].m_flags.Test(1)) {
                offset.z = -step * i;
                AssignVector(m_elements[i].m_position, fn_8040B67C(&matrix, &offset));
                m_elements[i].m_rotation.y = m_chara->m_rotation.y;
            }
        }
        break;
    }
    case 2: {
        float angleStep = (360.0f / m_count) * (lbl_805FAD38 / 180.0f);
        float radius = params->param0;
        Mtx34f matrix;
        fn_8040C148(&matrix, &lbl_805E0720);
        Vec3f offset(0.0f, 0.0f, radius);
        for (int i = 0; i < m_count; i++) {
            if (m_elements[i].m_flags.Test(1)) {
                fn_8040B6F8(&matrix);
                fn_8040B59C(&matrix, &m_chara->m_position);
                fn_8040B848(&matrix, m_chara->m_rotation.y);
                fn_8040B884(&matrix, m_chara->m_rotation.x);
                fn_8040B80C(&matrix, m_chara->m_rotation.z);
                fn_8040B848(&matrix, angleStep * i);
                AssignVector(m_elements[i].m_position, fn_8040B67C(&matrix, &offset));
                m_elements[i].m_rotation.y = m_chara->m_rotation.y;
            }
        }
        break;
    }
    case 3: {
        float angleStep;
        float step = params->param0 / m_count;
        angleStep = (180.0f / m_count) * (lbl_805FAD38 / 180.0f);
        float height = params->param1;
        Mtx34f matrix;
        fn_8040C148(&matrix, &lbl_805E0720);
        Vec3f offset(0.0f, 0.0f, 0.0f);
        for (int i = 0; i < m_count; i++) {
            if (m_elements[i].m_flags.Test(1)) {
                offset.x = height * std::sin(angleStep * i);
                offset.z = step * i;
                fn_8040B6F8(&matrix);
                fn_8040B59C(&matrix, &m_chara->m_position);
                fn_8040B848(&matrix, m_chara->m_rotation.y);
                fn_8040B884(&matrix, m_chara->m_rotation.x);
                fn_8040B80C(&matrix, m_chara->m_rotation.z);
                AssignVector(m_elements[i].m_position, fn_8040B67C(&matrix, &offset));
                m_elements[i].m_rotation.y = m_chara->m_rotation.y;
            }
        }
        break;
    }
    }
}

void Ring::CheckContacts() {
    ColliDetectArgs args;

    for (int i = 0; i < m_count; i++) {
        if (m_elements[i].m_collider != 0) {
            UnkColliContacts_8001EE04 *contacts = GetContacts(m_elements[i].m_collider.get());
            if (contacts != 0 && contacts->m_count > 0) {
                args.m_contacts = contacts;
                args.m_index = 0;
                bool ghost = false;
                if (m_elements[i].m_flags.Test(2)) {
                    ghost = true;
                }
                if (CheckHit(args, ghost)) {
                    if (m_params.ghost == 1) {
                        m_elements[i].m_flags.Set(2);
                        m_elements[i].Hide(1.0f);
                        m_elements[i].m_target->SetFlags(8);
                    } else {
                        fn_800C78B0(fn_80021B08(), &m_elements[i].m_collider);
                        fn_801E7108(&m_elements[i].m_target);
                        m_elements[i].m_flags.Reset(1);
                        fn_8015D794(&m_elements[i].m_position, 0x8A);
                    }
                }
            }
        }
    }
}

void Ring::UpdateTargets() {
    for (int i = 0; i < m_count; i++) {
        if (m_elements[i].m_target.get() != 0) {
            m_elements[i].m_target->fn_800CCE38(&m_elements[i].m_position);
        }
        if (m_elements[i].m_collider.get() != 0) {
            m_elements[i].m_collider->SetCharInfo(
                reinterpret_cast<CharInfo *>(&m_elements[i].m_position));
        }
    }
}

void Ring::ResetColliders() {
    for (int i = 0; i < m_count; i++) {
        if (m_elements[i].m_collider.get() != 0) {
            m_elements[i].m_collider->Reset();
        }
    }
}

void Ring::Draw(RednerDrawArgs &args) {
    if (!m_status.Test(0)) {
        RwSphere sphere;

        sphere.radius = 5.0f;
        fn_80048390(fn_800289B0(), 5);
        UnkRenderer_800289B0 *renderer = fn_800289B0();
        fn_800482E0(renderer, renderer->field_0x456);
        for (int i = 0; i < m_count; i++) {
            if (m_elements[i].m_flags.Test(1)) {
                sphere.center.x = m_elements[i].m_position.x;
                sphere.center.y = m_elements[i].m_position.y;
                sphere.center.z = m_elements[i].m_position.z;
                if (fn_804856C8(lbl_805F15B4->curCamera, &sphere) != 0) {
                    if (m_elements[i].m_flags.Test(2)) {
                        RwFrame *frame = (RwFrame *)m_elements[i].m_ghostClump->object.parent;
                        fn_800E9CEC(frame, m_elements[i].m_rotation.y, 0);
                        fn_804875B0(frame, &sphere.center, 2);
                        fn_80456BC4(m_elements[i].m_ghostClump);
                    } else {
                        RwFrame *frame = (RwFrame *)m_elements[i].m_clump->object.parent;
                        fn_800E9CEC(frame, m_elements[i].m_rotation.y, 0);
                        fn_804875B0(frame, &sphere.center, 2);
                        fn_80456BC4(m_elements[i].m_clump);
                    }
                }
            }
        }
        fn_80047780(fn_800289B0());
    }
}

void Ring::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0x14: /* VacuumCommand */
        for (int i = 0; i < m_count; i++) {
            if (m_elements[i].m_flags.Test(1)) {
                if (m_elements[i].m_target ==
                    static_cast<VacuumCommand &>(command).m_target.lock()) {
                    if (m_params.ghost == 1) {
                        m_elements[i].m_flags.Set(2);
                        m_elements[i].Hide(1.0f);
                        m_elements[i].m_target->SetFlags(8);
                    } else {
                        fn_800C78B0(fn_80021B08(), &m_elements[i].m_collider);
                        fn_801E7108(&m_elements[i].m_target);
                        m_elements[i].m_flags.Reset(1);
                    }
                    fn_80238A78(&command, &m_elements[i].m_position, &m_elements[i].m_rotation, 1);
                    command.field_0x4[3] |= 2;
                    return;
                }
            }
        }
        break;
    case 0xD: /* UseCommand */
        for (int i = 0; i < m_count; i++) {
            if (m_elements[i].m_flags.Test(1)) {
                if (m_elements[i].m_target == static_cast<UseCommand &>(command).m_target.lock()) {
                    if (m_params.ghost == 1) {
                        if (!m_elements[i].m_flags.Test(2)) {
                            if (boost::shared_ptr<CharInfo> user =
                                    static_cast<UseCommand &>(command).m_user.lock()) {
                                fn_8015F6EC(user, 0, 1, 0);
                                fn_8020642C(user, 0);
                            }
                        }
                        m_elements[i].m_flags.Set(2);
                        m_elements[i].Hide(1.0f);
                        m_elements[i].m_target->SetFlags(8);
                    } else {
                        fn_800C78B0(fn_80021B08(), &m_elements[i].m_collider);
                        fn_801E7108(&m_elements[i].m_target);
                        m_elements[i].m_flags.Reset(1);
                        if (boost::shared_ptr<CharInfo> user =
                                static_cast<UseCommand &>(command).m_user.lock()) {
                            fn_8015F6EC(user, 0, 1, 0);
                            fn_8020642C(user, 0);
                        }
                    }
                    command.field_0x4[3] |= 2;
                    return;
                }
            }
        }
        break;
    }
}

void Ring::EnableTargets() {
    for (int i = 0; i < m_count; i++) {
        if (m_elements[i].m_flags.Test(1)) {
            if (m_elements[i].m_target.get() != 0) {
                m_elements[i].m_target->TurnOn();
            }
        }
    }
}

void Ring::DisableTargets() {
    for (int i = 0; i < m_count; i++) {
        if (m_elements[i].m_flags.Test(1)) {
            if (m_elements[i].m_target.get() != 0) {
                m_elements[i].m_target->TurnOff();
            }
        }
    }
}

} // namespace RegularRing

extern "C" void fn_801123A8(void) {
    fn_8006B50C(0xDA, 1);
}

extern "C" void fn_801123A4(void) {}

namespace RegularRing {

void Ring::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new Ring(fn_801EB304(fn_8001C4F8(), 11), owner);
}

} // namespace RegularRing

/* The labels of the "type", "param0", "param1" and "ghost" parameters; param0 and param1 are
 * labelled by the type last shown. */
extern "C" const char *fn_801122A8(UnkLabelQuery_800CBC20 *cursor) {
    static const char *s_typeLabels[4] = {
        "type : normal", "type : liner", "type : circle", "type : parabola"};
    static const char *s_param0Labels[4] = {"", "length", "radius", "length"};
    static const char *s_param1Labels[4] = {"", "", "", "height"};
    static const char *s_ghostLabels[2] = {"ghost off", "ghost on"};
    static int s_type = 0;

    if (cursor->index == 0) {
        s_type = cursor->value;
        return s_typeLabels[cursor->value];
    } else if (cursor->index == 2) {
        return s_param0Labels[s_type];
    } else if (cursor->index == 3) {
        return s_param1Labels[s_type];
    } else if (cursor->index == 4) {
        return s_ghostLabels[cursor->value];
    }
    return 0;
}

namespace RegularRing {

RingState::RingState(RingElement *elements, int count) {
    __construct_array(m_bits, (void *)fn_80014E04, 0, sizeof(u32), 4);
    for (int i = 0; i < 4; i++) {
        fn_80014E10(&m_bits[i]);
    }
    for (int i = 0; i < count; i++) {
        if (elements[i].m_flags.Test(1) && !elements[i].m_flags.Test(2)) {
            fn_80014E1C(&m_bits[i / 32], i % 32);
        }
    }
}

void RingState::Apply(RingElement *elements, int count, bool ghost) {
    for (int i = 0; i < count; i++) {
        if (fn_80014E34(&m_bits[i / 32], i % 32)) {
            elements[i].m_flags.Set(1);
        } else if (ghost) {
            elements[i].m_flags.Set(2);
        } else {
            elements[i].m_flags.Reset(1);
        }
    }
}

} // namespace RegularRing

static GadgetParamDesc lbl_80526AF4[] = {
    {1, "type", 0, 0, 3, 0.0f, 0.0f, 0.0f, fn_801122A8, 0},
    {1, "num", 1, 1, 128, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {4, "param0", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_801122A8, 0},
    {4, "param1", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_801122A8, 0},
    {1, "ghost", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_801122A8, 0},
    {0},
};

GadgetClassDesc lbl_80526BE4 = {
    "RING", fn_801123A8, fn_801123A4, RegularRing::Ring::Instantiate, 0, 0, 0x10, 1, 0, 10,
    lbl_80526AF4,
};
