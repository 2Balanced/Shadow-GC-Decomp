/* ScatteringRing::ScatteringRing (class name from RTTI): a ring thrown out of a character, for
 * example when it is hit. ScatteringRing::Instantiate (name inferred) throws "count" rings (10 when the
 * count is not positive) from a position, spread evenly around a random direction; each one is a
 * free task with a ring model, a sphere collider, a Target and a character info. A ring flies under
 * gravity and air drag, bounces on the ground below it and turns. Once it has landed it can be
 * picked up by touching it (the character gets an item, and message 0 when the thrower's flag is
 * set) or through a VacuumCommand; it blinks during the last 1.5 seconds of its 5-second life and
 * disappears after at most 10 seconds. The ring is drawn through a Render callback. File name
 * inferred from the class. Names marked "(name inferred)" are not original. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first), except the static
 * HitCharacter, which is defined before the constructor: that puts its code where the original has
 * it. Instantiate, which is emitted after HitCharacter, is defined before it.
 *
 * Not yet matching (48 of 49 functions match; the data matches): in Instantiate the original loads
 * request.count only after storing the three components of the launch direction, while GC/2.7
 * (and 1.3.2 to 2.6) schedules that load above the stores; 3.0a3/3.0a5 keep it after them, as the
 * original does. No source shape tried (if/else, ternary, accessors, pointer or by-value request,
 * pointer/reference/aggregate initialisation of the direction, loops and labels) changes it, so it
 * looks like another trait of the newer compiler described in docs/research-notes.md. The other
 * 0x1F0 bytes of Instantiate match. */
/* Keeps the dead copy of bad_alloc's vtable in .data, as retail has it (see <new>). */
#define MSL_NEW_INLINE_BAD_ALLOC

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharaColliBaseCharInfo.h"
/* Before TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/TNumEdit.h"
#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Effect/System/EffectElement.h"
#include "Game/GadgetCharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetUtil.h"
#include "Game/Render.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::CharaColliBase;

/* The RenderWare types and functions used here, declared locally because the RenderWare headers
 * need the C library headers (layouts as in rw/rwcore.h and rw/rpworld.h, reduced to the members
 * used; as in RegularRing.cpp). */
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

/* Target (class name from RTTI): a point other objects can aim at (meaning inferred). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);
};

/* The renderer state the ring drawing sets up and restores (class name unknown; as in
 * RegularRing.cpp). */
struct UnkRenderer_800289B0 {
    /* 0x000 */ u8 field_0x0[0x456];
    /* 0x456 */ u8 field_0x456;
};

/* One hit of a line test and the list of them (class names unknown; layouts from fn_8001EE04,
 * fn_8001EDEC and fn_80027398, and from the members read here). */
struct UnkLineHit_80027398 {
    /* 0x00 */ u8 field_0x0[0xC];
    /* 0x0C */ void *field_0xC;
    /* 0x10 */ u32 field_0x10;
    /* 0x14 */ void *m_object; /* what was hit */
    /* 0x18 */ u32 field_0x18;
    /* 0x1C */ UnkLineHit_80027398 *m_next;
};

struct UnkLineHits_80021FE4 {
    /* 0x0 */ u8 field_0x0[0x8];
    /* 0x8 */ UnkLineHit_80027398 *m_first;
    /* 0xC */ UnkLineHit_80027398 *m_end;
};

/* What the throwers pass to ScatteringRing::Instantiate (class name unknown; built by fn_800B7E40 in
 * the callers; only the members read here are mapped). */
struct UnkScatterRequest_800B7E40 {
    /* 0x0 */ Vec3f position;
    /* 0xC */ s32 count;
};

extern "C" {
/* A 32-bit flag word: fn_80014E04 constructs it cleared, fn_80014E10 clears it, fn_80014E1C sets
 * bit, fn_8006127C clears bit and fn_80014E34 tests bit (behaviour from their bodies). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);

/* Returns the file loader, and the clump id of it (behaviour inferred). */
void *fn_800122D4(void);
RpClump *fn_8001277C(void *loader, int arg1, int id);

/* Sets a vector to the constant at 0x8051A4E8 (behaviour from its body). */
void fn_8040E970(Vec3f *vector);
/* vector += value (behaviour inferred). */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
/* A random number in [0, 1) (behaviour inferred). */
float fn_8040EE90(void);

/* The collision world, the line test against it, the first and end hits of the result, the step
 * to the next hit, the hit object's position and the release of the result (behaviour
 * inferred). */
void *fn_80020D40(void);
UnkLineHits_80021FE4 *fn_80021FE4(void *world, void *segment, int arg2, int arg3, int arg4, int arg5);
UnkLineHit_80027398 *fn_8001EE04(UnkLineHits_80021FE4 *hits);
UnkLineHit_80027398 *fn_8001EDEC(UnkLineHits_80021FE4 *hits);
void fn_80027398(UnkLineHit_80027398 **it);
const Vec3f *fn_8001E9BC(void *object);
void fn_80026FB4(UnkLineHits_80021FE4 *hits, int arg1);

/* The collider manager, a sphere collider at position, and the removal of a collider (behaviour
 * inferred; the meanings of the integer arguments are unknown). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7EA8(void *manager, const Vec3f *position,
                                              u32 flags, int arg3, int arg4,
                                              const boost::weak_ptr<CharInfo> &owner,
                                              const Vec3f *offset, float radius);
void fn_800C78B0(void *manager, boost::shared_ptr<CharaColliBase> *collider);

/* Creates a target for owner with the given flags, and removes a target (behaviour inferred). */
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

/* The renderer, and its state changes around the rings (behaviour inferred). */
UnkRenderer_800289B0 *fn_800289B0(void);
void fn_80048390(UnkRenderer_800289B0 *renderer, int mode);
void fn_800482E0(UnkRenderer_800289B0 *renderer, int value);
void fn_80047780(UnkRenderer_800289B0 *renderer);
}

/* The empty character info (an .sbss shared_ptr; meaning inferred). */
extern boost::shared_ptr<CharInfo> lbl_805EE698;

/* Constants of the ring loss that the player code around 0x80357AA0 reads (meanings unknown;
 * this unit does not use them). They lead the unit's .sdata2. */
extern const s32 lbl_805F3CA8 = 10;
extern const float lbl_805F3CAC = 1.5f;
extern const float lbl_805F3CB0 = 5.0f;
extern const float lbl_805F3CB4 = 60.0f;
extern const float lbl_805F3CB8 = 1.5f;

/* The contacts of a collider (the object holds them 0xC bytes in; as in RegularRing.cpp). */
inline UnkColliContacts_8001EE04 *GetContacts(CharaColliBase *collider) {
    return reinterpret_cast<UnkColliContacts_8001EE04 *>(reinterpret_cast<u8 *>(collider) + 0xC);
}

/* Converts degrees to radians (name inferred). */
inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

namespace ScatteringRing {

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

/* A vector seen as a plain block: assigning one copies it word by word, as the game's own
 * assignment of the rotation below does (name inferred). */
struct VectorBlock {
    Vec3f value;
};

/* The state of a thrown ring, which Instantiate fills in and the constructor copies (name and member
 * names inferred). */
struct ScatterParams {
    ScatterParams() : rotation(0.0f, 0.0f, 0.0f) {}

    /* The position and velocity go through the vectors' out-of-line assignment (fn_800091D0), the
     * rotation is copied as a block. */
    ScatterParams &operator=(const ScatterParams &other) {
        fn_800091D0(&position, &other.position);
        fn_800091D0(&velocity, &other.velocity);
        (VectorBlock &)rotation = (const VectorBlock &)other.rotation;
        field_0x24 = other.field_0x24;
        lifeTime = other.lifeTime;
        groundY = other.groundY;
        clump = other.clump;
        flag = other.flag;
        return *this;
    }

    /* 0x00 */ Vec3f position;
    /* 0x0C */ Vec3f velocity;
    /* 0x18 */ Vec3f rotation;
    /* 0x24 */ float field_0x24;
    /* 0x28 */ float lifeTime; /* counted down once the ring has landed */
    /* 0x2C */ float groundY;  /* the height the ring bounces at */
    /* 0x30 */ RpClump *clump;
    /* 0x34 */ bool flag; /* passed on with the ring (meaning unknown) */
};
SIZE_ASSERT(ScatterParams, 0x38);

/* What the collision test passes to the hit function (name inferred). */
struct ScatterHitContext {
    ScatterHitContext() : m_hit(false), m_flag(false) {}

    /* 0x0 */ bool m_hit;
    /* 0x1 */ bool m_flag;
};

class ScatteringRing : public Task {
  public:
    ScatteringRing(Task *parent, const ScatterParams &params);
    virtual ~ScatteringRing();
    virtual void Tick(float dt);

    /* Throws request.count rings from request.position (name inferred). */
    static void Instantiate(const UnkScatterRequest_800B7E40 &request, bool flag);

    /* Method names are inferred. */
    bool TestHit(ColliDetectArgs &args);
    float GetGroundHeight(const Vec3f &position);
    void Draw(RednerDrawArgs &args);
    void OnCommand(CharCommand &command);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x28 */ boost::shared_ptr<Render> m_render;
    /* 0x30 */ boost::shared_ptr<CharaColliBase> m_collider;
    /* 0x38 */ boost::shared_ptr<Target> m_target;
    /* 0x40 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x48 */ ScatterParams m_params;
    /* 0x80 */ float m_timer; /* the longest time left */
    /* 0x84 */ RingFlags m_status; /* bit 0: landed, bit 1: blinking, bit 2: hidden this frame */
};
SIZE_ASSERT(ScatteringRing, 0x88);

const char *ScatteringRing::s_taskName = "ScatteringRing";

void ScatteringRing::Instantiate(const UnkScatterRequest_800B7E40 &request, bool flag) {
    ScatterParams params;
    Mtx34f matrix;
    Vec3f velocity;
    Vec3f direction(0.0f, 5.109375f, 1.15625f);
    int count;
    if (request.count > 0) {
        count = request.count;
    } else {
        count = 10;
    }
    float step = DegToRad(360.0f / count);

    fn_8040C148(&matrix, &lbl_805E0720);
    RpClump *clump = fn_8001277C(fn_800122D4(), 0, 0x7C);
    fn_8040B6F8(&matrix);
    fn_8040B848(&matrix, DegToRad(360.0f * fn_8040EE90()));
    for (int i = 0; i < count; i++) {
        fn_8040B848(&matrix, step);
        AssignVector(velocity, fn_8040B67C(&matrix, &direction));
        fn_800091D0(&params.position, &request.position);
        fn_800091D0(&params.velocity, &velocity);
        fn_8040E970(&params.rotation);
        params.field_0x24 = 0.0f;
        params.lifeTime = 5.0f;
        params.groundY = 6.0f;
        params.clump = clump;
        params.flag = flag;
        new ScatteringRing(fn_801EB304(fn_8001C4F8(), 11), params);
    }
    fn_801D560C(fn_80053164(), 0xE00C, 0, 0);
}

/* Gives the character the ring unless the ring was already taken in this test (name
 * inferred). */
static void HitCharacter(boost::shared_ptr<CharInfo> hit, void *user) {
    if (hit->vf_0x1C()) {
        ScatterHitContext *context = static_cast<ScatterHitContext *>(user);
        if (context != 0 && !context->m_hit) {
            fn_8015F6EC(hit, 0, 1, 0);
            if (context->m_flag) {
                fn_8020642C(hit, 0);
            }
            context->m_hit = true;
        }
    }
}

ScatteringRing::ScatteringRing(Task *parent, const ScatterParams &params) : Task(parent) {
    m_name = s_taskName;
    m_params = params;
    m_chara = fn_8015CD90(0x26, 4);
    m_chara->m_commandUnit = boost::shared_ptr<CharCommandUnit>(new CharCommandUnit());
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&ScatteringRing::OnCommand, this, _1)));
    m_render = fn_8004DFB0(fn_80046178()->field_0x8, "ScatteringRing");
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&ScatteringRing::Draw, this, _1));
    m_collider = fn_800C7EA8(fn_80021B08(), &m_params.position, 0x40000, 10, 0, lbl_805EE698,
                             &lbl_805E07A0, 5.0f);
    m_target = fn_801E7128(m_chara, 0x180);
    m_status.Clear();
    m_timer = 10.0f;
}

ScatteringRing::~ScatteringRing() {
    fn_801E7108(&m_target);
    if (m_collider.get() != 0) {
        fn_800C78B0(fn_80021B08(), &m_collider);
    }
}

void ScatteringRing::Tick(float dt) {
    if (m_status.Test(0) && m_collider.get() != 0) {
        UnkColliContacts_8001EE04 *contacts = GetContacts(m_collider.get());
        if (contacts != 0 && contacts->m_count > 0) {
            ColliDetectArgs args;
            args.m_contacts = contacts;
            args.m_index = 0;
            if (TestHit(args)) {
                fn_801D5300(fn_80053164());
                fn_8015D794(&m_params.position, 0x8A);
                m_flags |= 1;
                return;
            }
        }
    }

    if (m_status.Test(1)) {
        if (m_status.Test(2)) {
            m_status.Reset(2);
        } else {
            m_status.Set(2);
        }
    } else if (m_params.lifeTime <= 1.5f) {
        m_status.Set(1);
    }

    if (m_status.Test(0)) {
        m_params.lifeTime -= dt;
        if (m_params.lifeTime <= 0.0f) {
            m_flags |= 1;
            return;
        }
    }
    m_timer -= dt;
    if (m_timer <= 0.0f) {
        m_flags |= 1;
        return;
    }

    m_params.groundY = 5.0f + GetGroundHeight(m_params.position);
    m_params.velocity.x += 0.015627f * -m_params.velocity.x;
    m_params.velocity.y += 0.1f * -m_params.velocity.y - 0.12138f;
    m_params.velocity.z += 0.015627f * -m_params.velocity.z;
    fn_8040CFF8(&m_params.position, &m_params.velocity);
    if (m_params.position.y < m_params.groundY && m_params.velocity.y < 0.0f) {
        m_params.position.y = m_params.groundY;
        m_params.velocity.x *= 0.9f;
        m_params.velocity.y *= -1.11879f;
        m_params.velocity.z *= 0.9f;
        m_status.Set(0);
    }
    m_params.rotation.y += DegToRad(1.5f);
    if (m_collider.get() != 0) {
        m_collider->SetCharInfo(reinterpret_cast<CharInfo *>(&m_params.position));
    }
    if (m_target.get() != 0) {
        m_target->fn_800CCE38(&m_params.position);
    }
}

bool ScatteringRing::TestHit(ColliDetectArgs &args) {
    ScatterHitContext context;

    context.m_flag = m_params.flag;
    fn_8015B004(args, HitCharacter, &context);
    return context.m_hit;
}

float ScatteringRing::GetGroundHeight(const Vec3f &position) {
    u8 segment[0x18];
    Vec3f start;
    fn_800091EC(&start, &position);
    Vec3f end;
    fn_800091EC(&end, &position);
    end.y += -10000.0f;
    fn_80020A20(segment, &start, &end);

    UnkLineHits_80021FE4 *hits = fn_80021FE4(fn_80020D40(), segment, 1, 0, 0, 0);
    if (hits != 0) {
        for (UnkLineHit_80027398 *it = fn_8001EE04(hits); it != fn_8001EDEC(hits); fn_80027398(&it)) {
            if (it != 0 && it->field_0xC != 0 && it->m_object != 0) {
                Vec3f ground;
                fn_800091EC(&ground, fn_8001E9BC(it->m_object));
                fn_80026FB4(hits, 1);
                return ground.y;
            }
        }
    }
    fn_80026FB4(hits, 1);
    return 0.0f;
}

void ScatteringRing::Draw(RednerDrawArgs &args) {
    if (!m_status.Test(2)) {
        RwSphere sphere;
        sphere.radius = 5.0f;
        fn_80048390(fn_800289B0(), 5);
        UnkRenderer_800289B0 *renderer = fn_800289B0();
        fn_800482E0(renderer, renderer->field_0x456);
        if (m_params.clump != 0) {
            sphere.center.x = m_params.position.x;
            sphere.center.y = m_params.position.y;
            sphere.center.z = m_params.position.z;
            if (fn_804856C8(lbl_805F15B4->curCamera, &sphere) != 0) {
                RwFrame *frame = (RwFrame *)m_params.clump->object.parent;
                fn_800E9CEC(frame, m_params.rotation.y, 0);
                fn_804875B0(frame, &sphere.center, 2);
                fn_80456BC4(m_params.clump);
            }
        }
        fn_80047780(fn_800289B0());
    }
}

void ScatteringRing::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0x14: { /* VacuumCommand */
        Vec3f rotation(m_params.rotation.x, m_params.rotation.y, m_params.rotation.z);
        fn_80238A78(&command, &m_params.position, &rotation, m_params.flag);
        m_flags |= 1;
        command.field_0x4[3] |= 2;
        break;
    }
    }
}

} // namespace ScatteringRing
