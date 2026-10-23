/* EggLastMechFeverRing::EggLastMechFeverRingGenerator and EggLastMechFeverRing::EggLastMechFeverRing
 * (class names from RTTI): the "fever" rings of the Egg Dealer (EggLastMech) fight. The AI state
 * that pays out (Game/EggLastMechAIState.cpp) starts a generator at a position (fn_803577D4); it
 * throws the rings in three waves of "count" rings (10 when the count is not positive), a few every
 * 0.03 seconds with 0.3 seconds between waves. Each ring is a free task much like ScatteringRing
 * (Game/ScatteringRing.cpp, whose constants it shares): it flies off in a random direction under
 * gravity, bounces on a fixed floor height, turns, and can be picked up by touching it once it
 * has landed; it blinks once 1.5 seconds of its 9-second life remain. File name from the classes.
 * Names other than the classes' are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first), except the static HitCharacter, which is defined
 * before the ring's constructor: that puts its code where the original has it. */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Effect/System/EffectElement.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetUtil.h"
#include "Game/MathTypes.h"
#include "Game/Random.h"
#include "Game/Render.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::CharaColliBase;

/* The RenderWare types and functions used here, declared locally because the RenderWare headers
 * need the C library headers (layouts as in rw/rwcore.h and rw/rpworld.h, reduced to the members
 * used; as in ScatteringRing.cpp). */
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

/* The renderer state the ring drawing sets up and restores (class name unknown; as in
 * ScatteringRing.cpp). */
struct UnkRenderer_800289B0 {
    /* 0x000 */ u8 field_0x0[0x456];
    /* 0x456 */ u8 field_0x456;
};

/* The game state (struct name unknown; see Game/TGi.h). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xAC];
    /* 0x0AC */ s32 m_stage;
};

extern UnkGameState_8057D6A0 lbl_8057D6A0;

extern "C" {
/* A 32-bit flag word: fn_80014E04 constructs it cleared, fn_80014E1C sets bit, fn_8006127C clears
 * bit and fn_80014E34 tests bit (behaviour from their bodies). */
void fn_80014E04(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);

/* Where new tasks go: the root of the task tree and its task of a layer (see Game/Gadget.h). */
void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);

/* Returns the file loader, and the clump id of it (behaviour inferred). */
void *fn_800122D4(void);
RpClump *fn_8001277C(void *loader, int arg1, int id);

/* Sets a vector to the constant at 0x8051A4E8 (behaviour from its body). */
void fn_8040E970(Vec3f *vector);
/* A random number in [0, 1) (behaviour inferred). */
float fn_8040EE90(void);
/* a + b (see Game/BAMine.cpp). */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
/* Moves position by velocity under acceleration with damping over dt (behaviour inferred). */
void fn_804103E8(Vec3f *position, Vec3f *velocity, const Vec3f *acceleration, float damping,
                 float dt);

/* The collider manager, a sphere collider at position, and the removal of a collider (behaviour
 * inferred; as in ScatteringRing.cpp). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7EA8(void *manager, const Vec3f *position,
                                              u32 flags, int arg3, int arg4,
                                              const boost::weak_ptr<CharInfo> &owner,
                                              const Vec3f *offset, float radius);
void fn_800C78B0(void *manager, boost::shared_ptr<CharaColliBase> *collider);

/* Gives target the item id, and sends message to the character (behaviour inferred). */
void fn_8015F6EC(boost::shared_ptr<CharInfo> target, int id, int arg2, int arg3);
int fn_8020642C(const boost::shared_ptr<CharInfo> &character, int message);
/* Plays the ring sound, and a sound at a position (behaviour inferred). */
void fn_801D5300(void *sound);
void fn_801D54F4(void *manager, int id, const Vec3f *position, int arg3, int arg4);
/* Launches effect id at position (behaviour inferred). */
boost::weak_ptr<Effect::System::EffectElement> fn_8015D794(const Vec3f *position, int id);

/* The renderer, and its state changes around the rings (behaviour inferred). */
UnkRenderer_800289B0 *fn_800289B0(void);
void fn_80048390(UnkRenderer_800289B0 *renderer, int mode);
void fn_800482E0(UnkRenderer_800289B0 *renderer, int value);
void fn_80047780(UnkRenderer_800289B0 *renderer);
}

/* The C library's pow (declared here rather than through <cmath>, as in Coaster.cpp). */
extern "C" double pow(double x, double y);

/* The empty character info (an .sbss shared_ptr; as in ScatteringRing.cpp). */
extern boost::shared_ptr<CharInfo> lbl_805EE698;

/* The constants of ScatteringRing.cpp the rings use: the default ring count, the turn speed in
 * degrees per frame and the frame rate, the floor height and the collider radius, and the time
 * left at which a ring blinks (meanings inferred). */
extern const s32 lbl_805F3CA8;
extern const float lbl_805F3CAC;
extern const float lbl_805F3CB0;
extern const float lbl_805F3CB4;
extern const float lbl_805F3CB8;

/* Converts degrees to radians (as in ScatteringRing.cpp). */
inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* Where and how many rings to throw (struct name as in Game/EggLastMechAIState.cpp, which builds
 * it; names inferred). Its copies go through the vector's out-of-line copy constructor. */
struct UnkParam_803577D4 {
    UnkParam_803577D4(const UnkParam_803577D4 &other) {
        ConstructVector(&m_position, other.m_position);
        m_value = other.m_value;
    }

    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ s32 m_value; /* the rings per wave */
};

namespace EggLastMechFeverRing {

/* A 32-bit flag word (as in ScatteringRing.cpp). */
struct RingFlags {
    RingFlags() {
        fn_80014E04(&m_bits);
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

/* A vector seen as a plain block: assigning one copies it word by word (as in
 * ScatteringRing.cpp). */
struct VectorBlock {
    Vec3f value;
};

/* The state of a thrown ring, which the generator fills in and the ring copies. */
struct RingParams {
    RingParams() : rotation(0.0f, 0.0f, 0.0f) {}

    /* The position and velocity go through the vectors' out-of-line assignment (fn_800091D0), the
     * rotation is copied as a block. */
    RingParams &operator=(const RingParams &other) {
        fn_800091D0(&position, &other.position);
        fn_800091D0(&velocity, &other.velocity);
        (VectorBlock &)rotation = (const VectorBlock &)other.rotation;
        field_0x24 = other.field_0x24;
        lifeTime = other.lifeTime;
        groundY = other.groundY;
        clump = other.clump;
        return *this;
    }

    /* 0x00 */ Vec3f position;
    /* 0x0C */ Vec3f velocity;
    /* 0x18 */ Vec3f rotation;
    /* 0x24 */ float field_0x24;
    /* 0x28 */ float lifeTime;
    /* 0x2C */ float groundY; /* the height the ring bounces at */
    /* 0x30 */ RpClump *clump;
};
SIZE_ASSERT(RingParams, 0x34);

class EggLastMechFeverRing : public Task {
  public:
    EggLastMechFeverRing(Task *parent, const RingParams &params);
    virtual ~EggLastMechFeverRing();
    virtual void Tick(float dt);

    bool TestHit(ColliDetectArgs &args);
    void Draw(RednerDrawArgs &args);

    static const char *s_taskName;

    /* 0x28 */ boost::shared_ptr<Render> m_render;
    /* 0x30 */ boost::shared_ptr<CharaColliBase> m_collider;
    /* 0x38 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x40 */ RingParams m_params;
    /* 0x74 */ RingFlags m_status; /* bit 0: landed, bit 1: blinking, bit 2: hidden this frame */
};
SIZE_ASSERT(EggLastMechFeverRing, 0x78);

class EggLastMechFeverRingGenerator : public Task {
  public:
    EggLastMechFeverRingGenerator(Task *parent, const UnkParam_803577D4 &params);
    virtual ~EggLastMechFeverRingGenerator();
    virtual void Tick(float dt);

    /* Throws one ring. */
    void Spawn();

    /* 0x28 */ UnkParam_803577D4 m_params;
    /* 0x38 */ s32 m_remaining; /* rings left in this wave */
    /* 0x3C */ float m_interval;
    /* 0x40 */ s32 m_waves;
    /* 0x44 */ float m_delay; /* before the next wave */
    /* 0x48 */ bool m_playSound;
    /* 0x4C */ u32 field_0x4C;
};
SIZE_ASSERT(EggLastMechFeverRingGenerator, 0x50);

const char *EggLastMechFeverRing::s_taskName = "EggLastMechFeverRing";

/* Gives the character the ring unless the ring was already taken in this test. */
static void HitCharacter(boost::shared_ptr<CharInfo> hit, void *user) {
    if (hit->vf_0x1C()) {
        bool *taken = static_cast<bool *>(user);
        if (taken != 0 && !*taken) {
            fn_8015F6EC(hit, 0, 1, 0);
            fn_8020642C(hit, 0);
            *taken = true;
        }
    }
}

EggLastMechFeverRing::EggLastMechFeverRing(Task *parent, const RingParams &params) : Task(parent) {
    m_name = s_taskName;
    m_params = params;
    m_chara = fn_8015CD90(0x26, 4);
    m_render = fn_8004DFB0(fn_80046178()->field_0x8, "EggLastMechFeverRing");
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&EggLastMechFeverRing::Draw, this, _1));
    m_collider = fn_800C7EA8(fn_80021B08(), &m_params.position, 0x40000, 10, 0, lbl_805EE698,
                             &lbl_805E07A0, lbl_805F3CB0);
    m_status.Set(0);
}

EggLastMechFeverRing::~EggLastMechFeverRing() {
    if (m_collider.get() != 0) {
        fn_800C78B0(fn_80021B08(), &m_collider);
    }
}

void EggLastMechFeverRing::Tick(float dt) {
    if (m_status.Test(0) && m_collider.get() != 0) {
        UnkColliContacts_8001EE04 *contacts = reinterpret_cast<UnkColliContacts_8001EE04 *>(
            reinterpret_cast<u8 *>(m_collider.get()) + 0xC);
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
    } else if (m_params.lifeTime <= lbl_805F3CB8) {
        m_status.Set(1);
    }

    m_params.lifeTime -= dt;
    if (m_params.lifeTime < 0.0f) {
        m_flags |= 1;
        return;
    }

    Vec3f horizontal;
    fn_800091EC(&horizontal, &m_params.velocity);
    horizontal.y = 0.0f;
    fn_804103E8(&m_params.position, &horizontal, &lbl_805E07A0, 0.3f, dt);
    Vec3f vertical;
    fn_800091EC(&vertical, &lbl_805E07A0);
    vertical.y = m_params.velocity.y;
    Vec3f gravity(0.0f, -100.0f, 0.0f);
    fn_804103E8(&m_params.position, &vertical, &gravity, 0.0f, dt);
    AssignVector(m_params.velocity, fn_8040CDD0(&horizontal, &vertical));
    if (m_params.position.y < m_params.groundY && m_params.velocity.y < 0.0f) {
        m_params.position.y = m_params.groundY;
        float damping = 1.0f / (float)pow(2.7182817f, 0.9f * dt);
        m_params.velocity.x *= damping;
        m_params.velocity.y *= -0.45f;
        m_params.velocity.z *= damping;
    }
    m_params.rotation.y += DegToRad(dt * (lbl_805F3CAC * lbl_805F3CB4));
    if (m_collider.get() != 0) {
        m_collider->SetCharInfo(reinterpret_cast<CharInfo *>(&m_params.position));
    }
}

bool EggLastMechFeverRing::TestHit(ColliDetectArgs &args) {
    bool taken = false;
    fn_8015B004(args, HitCharacter, &taken);
    return taken;
}

void EggLastMechFeverRing::Draw(RednerDrawArgs &args) {
    if (!m_status.Test(2)) {
        RwSphere sphere;
        sphere.radius = lbl_805F3CB0;
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

EggLastMechFeverRingGenerator::EggLastMechFeverRingGenerator(Task *parent,
                                                             const UnkParam_803577D4 &params)
    : Task(parent), m_params(params) {
    m_name = "EggLastMechFeverRingGenerator";
    if (params.m_value <= 0) {
        m_params.m_value = lbl_805F3CA8;
    }
    m_remaining = m_params.m_value;
    m_interval = 0.03f;
    m_waves = 3;
    m_delay = 0.0f;
    m_playSound = true;
}

void EggLastMechFeverRingGenerator::Spawn() {
    Mtx34f matrix;
    Vec3f velocity;
    float forward = 30.0f * fn_8040EC80(0.3f, 1.0f);
    Vec3f direction(0.0f, 80.0f * fn_8040EC80(1.5f, 2.0f), forward);
    float angle = DegToRad(360.0f * fn_8040EE90());
    fn_8040C148(&matrix, &lbl_805E0720);
    RpClump *clump = fn_8001277C(fn_800122D4(), 0, 0x7C);
    fn_8040B6F8(&matrix);
    fn_8040B848(&matrix, DegToRad(360.0f * fn_8040EE90()));
    fn_8040B848(&matrix, angle);
    AssignVector(velocity, fn_8040B67C(&matrix, &direction));
    RingParams params;
    fn_800091D0(&params.position, &m_params.m_position);
    fn_800091D0(&params.velocity, &velocity);
    fn_8040E970(&params.rotation);
    params.field_0x24 = 0.0f;
    params.lifeTime = 9.0f;
    params.groundY = lbl_805F3CB0;
    if (lbl_8057D6A0.m_stage == 0x28) {
        params.groundY += 20.0f;
    }
    params.clump = clump;
    new EggLastMechFeverRing(fn_801EB304(fn_8001C4F8(), 11), params);
}

void EggLastMechFeverRingGenerator::Tick(float dt) {
    if ((m_delay -= dt) > 0.0f) {
        return;
    }
    m_delay = 0.0f;
    if (m_playSound) {
        m_playSound = false;
        fn_801D54F4(fn_80053164(), 0xE00C, &m_params.m_position, 0, 1);
    }

    int count = 0;
    if ((m_interval -= dt) < 0.0f) {
        count = 1;
        while (true) {
            if ((m_interval += 0.03f) >= 0.0f) {
                break;
            }
            count++;
        }
        m_interval = 0.03f;
    }
    if (m_remaining < count) {
        count = m_remaining;
    }
    for (int i = 0; i < count; i++) {
        Spawn();
        m_remaining--;
    }

    if (m_remaining <= 0) {
        if (--m_waves != 0) {
            m_delay = 0.3f;
            m_playSound = true;
            m_remaining = m_params.m_value;
            m_interval = 0.03f;
        } else {
            m_flags |= 1;
        }
    }
}

} // namespace EggLastMechFeverRing

/* Starts throwing fever rings (name unknown). */
extern "C" void fn_803577D4(UnkParam_803577D4 *params) {
    new EggLastMechFeverRing::EggLastMechFeverRingGenerator(fn_801EB304(fn_8001C4F8(), 11),
                                                            *params);
}

EggLastMechFeverRing::EggLastMechFeverRingGenerator::~EggLastMechFeverRingGenerator() {}
