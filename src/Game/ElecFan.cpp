/* ElecFan (class name from RTTI): the ELEC_FAN gadget of the electric circuit area, an updraft of
 * the "height" and "radius" parameters. While its switch is on (or when it has none), the nearest
 * character inside the column is lifted with a Player::StreamCommand and an AccelerateCommand of
 * the "power" parameter, weakening towards the top; it is let go once it leaves the column. Every
 * frame it also spawns an ElecFanDraw (class name from RTTI), a short-lived task that draws one
 * rising streak (DE0403DENSIRYUU.DFF) at a random point of the column's rim and fades it out. File
 * name inferred from the class. Method names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/CharCommand.h"
#include "Game/GadgetCharCommands.h"
#include "Game/CharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetPickup.h"
#include "Game/GadgetClassDesc.h"
#include "Game/Player.h"
#include "Game/Render.h"
#include "Game/MathConstants.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/FileQueue.h"
#include "Game/Random.h"
#include "boost/bind.hpp"
#include "boost/function.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"
#include <cmath>

/* The RenderWare types and functions used here, declared locally because the RenderWare headers
 * need the C library headers (layouts as in rw/rwcore.h and rw/rpworld.h, reduced to the members
 * used; see ElecCoreWall.cpp). */
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

typedef struct RwRGBA {
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
} RwRGBA;

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

typedef int (*RwRenderStateSetFunction)(int state, void *value);

typedef struct RwGlobals {
    /* 0x00 */ RwCamera *curCamera;
    /* 0x04 */ u8 field_0x4[0x20 - 0x4];
    /* 0x20 */ RwRenderStateSetFunction fpRenderStateSet; /* dOpenDevice.fpRenderStateSet */
} RwGlobals;

/* the engine instance */
extern RwGlobals *lbl_805F15B4;

/* whether a sphere is in a camera's view frustum (inferred) */
int fn_804856C8(const RwCamera *camera, const RwSphere *sphere);
/* translates a frame */
RwFrame *fn_804875B0(RwFrame *frame, const RwV3d *translation, int combine);
/* Renders a clump. */
RpClump *fn_80456BC4(RpClump *clump);
}

enum {
    rwRENDERSTATECULLMODE = 20,
    rwCULLMODECULLNONE = 1,
    rwCULLMODECULLBACK = 2
};

extern "C" {
/* Loads a file of the model loader (behaviour inferred). */
void *fn_800122D4(void);
void *fn_80012700(void *loader, int arg1, const char *fileName);
/* Turns a frame about its y axis to angle (behaviour inferred). */
void fn_800E9CEC(RwFrame *frame, float angle, int combine);
/* Sets the material colour of every atomic of a clump (behaviour inferred). */
void fn_80037B84(RpClump *clump, const RwRGBA *color);
/* Square root (behaviour inferred). */
float fn_8000FEE4(float value);
/* Builds the character info of a gadget type, the sound manager, vector *= scale and a debug line
 * from start to end (segment is 0x18 bytes of caller stack); see Game/GadgetUtil.h, which declares
 * pi without const. */
boost::shared_ptr<GadgetCharInfo> fn_8015CD90(int arg0, int arg1);
void *fn_80053164(void);
void fn_8040CE38(Vec3f *vector, float scale);
void *fn_80020A20(void *segment, const Vec3f *start, const Vec3f *end);
/* Clears bit of a flag word (see Elevator.cpp). */
void fn_8006127C(u32 *flags, int bit);
/* A test on a character (behaviour unknown; see FireRing.cpp), and another one that this gadget
 * makes before it lifts the character (behaviour unknown). */
bool fn_800B7AD0(const boost::shared_ptr<CharInfo> &character);
bool fn_800B7938(const boost::shared_ptr<CharInfo> &character);
/* The distance of point from the debug line segment (behaviour inferred). */
float fn_8003C63C(const Vec3f *point, const void *segment, int arg2);
/* Plays sound id for source (behaviour inferred; the other arguments are unknown). */
void fn_801D53C4(void *sound, int id, int arg2, void *source, int arg4, int arg5, int arg6);
/* vector * scale, vector += value and a + b (behaviour inferred). */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
void fn_8040CDD0(Vec3f *destination, const Vec3f *a, const Vec3f *b);
/* A debug cylinder at position, turned by rotation (behaviour inferred; see Elevator.cpp). */
void fn_802E0C60(const Vec3f *position, const Vec3f *rotation, int arg2,
                 float radius, float length);
}

/* Global state of the game; the float at 0x98 is the angle the updraft's top is scaled by (meaning
 * unknown). */
struct UnkGameState_8057D6A0 {
    /* 0x00 */ u8 field_0x0[0x98];
    /* 0x98 */ float field_0x98;
};
extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* vector += value through fn_8040CFF8 (see ElecCircuitEntrance.cpp). */
inline Vec3f &operator+=(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
    return vector;
}

/* Stores sin(angle) and cos(angle) scaled by radius through x and z (name inferred). The copy
 * emitted in ElecCircuitEntrance.cpp is the one linked. */
inline void GetCirclePoint(float *x, float *z, float angle, float radius) {
    *x = radius * std::sin(angle);
    *z = radius * std::cos(angle);
}

/* fn_8040CD74 called as returning a - b by value (it stores the result through its first
 * argument). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* The velocity of a player character (offset from this gadget's use; name inferred). */
inline Vec3f *GetPlayerVelocity(Player::PlayerBase *player) {
    return reinterpret_cast<Vec3f *>(reinterpret_cast<u8 *>(player) + 0xE8);
}

/* One streak of the updraft: rises from the column's rim for a third of a second, fading out over
 * its last twelfth (name from RTTI; member names inferred). */
class ElecFanDraw : public Task {
  public:
    /* The original probably takes position by value: the game's vector type has an out-of-line
     * copy constructor (fn_800091EC), so callers pass the copy that returns. */
    ElecFanDraw(Task *parent, const Vec3f &position, float angle, float radius,
                float halfHeight);
    virtual ~ElecFanDraw();
    virtual void Tick(float dt);

    /* The draw callback (name inferred). */
    void Draw(RednerDrawArgs &args);

    static const char *s_taskName;

    /* 0x28 */ boost::shared_ptr<Render> m_render;
    /* 0x30 */ RpClump *m_clump;
    /* 0x34 */ float m_time;
    /* 0x38 */ float m_radius;
    /* 0x3C */ float m_halfHeight;
    /* 0x40 */ float m_boundRadius;
    /* 0x44 */ Vec3f m_position;
    /* 0x50 */ float m_angle;
    /* 0x54 */ u32 pad_0x54; /* the class size is a multiple of 8 */
};
SIZE_ASSERT(ElecFanDraw, 0x58);

/* The parameters of the owner record. */
struct ElecFanParams {
    /* 0x0 */ float height;
    /* 0x4 */ float radius;
    /* 0x8 */ float power;
};

class ElecFan : public Gadget {
  public:
    ElecFan(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~ElecFan();
    virtual void Behave(float dt);
    virtual void fn_800CBC1C();
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Catches the nearest character in the column and lifts it (name inferred). */
    void Blow();
    /* Copies the owner's position, switch id and parameters (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::weak_ptr<CharInfo> m_target;
    /* 0x40 */ ElecFanParams m_params;
    /* 0x4C */ u8 m_switchId;
    /* 0x50 */ GadgetPickup::PickupFlags m_flags; /* bit 0 caught this frame, bit 2 on */
    /* 0x54 */ u32 pad_0x54; /* the class size is a multiple of 8 */
};
SIZE_ASSERT(ElecFan, 0x58);

const char *ElecFan::s_taskName = "ElecFan";

ElecFan::ElecFan(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_chara = fn_8015CD90(0x10, 4);
    ReadPlacement(m_owner);
}

ElecFan::~ElecFan() {
    boost::shared_ptr<CharInfo> target = m_target.lock();
    if (target.get() != 0 && fn_800B7938(target)) {
        target->PushCommand(new Player::StreamCommand(false, 4));
    }
}

void ElecFan::Behave(float dt) {
    fn_8006127C(&m_flags.m_bits, 0);
    u8 id = m_switchId;
    if (id != 0) {
        if (fn_8016904C(fn_800CB69C(), id) == 1) {
            m_flags.Set(2);
        } else {
            fn_8006127C(&m_flags.m_bits, 2);
        }
    } else {
        m_flags.Set(2);
    }
    if (m_flags.Test(2)) {
        Blow();
    }

    boost::shared_ptr<CharInfo> target = m_target.lock();
    if (target.get() != 0 && fn_800B7938(target)) {
        Vec3f offset;

        fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                 &target->m_position, &m_chara->m_position));
        float distanceSq = offset.x * offset.x + offset.z * offset.z;
        if (distanceSq > m_params.radius * m_params.radius ||
            ((offset.y < 0.0f || m_params.height < offset.y) && fn_800B7AD0(target))) {
            target->PushCommand(new Player::StreamCommand(false, 4));
            m_target.reset();
        } else if (m_params.height < offset.y) {
            Player::PlayerBase *player = dynamic_cast<Player::PlayerBase *>(target.get());
            if (player != 0) {
                target->PushCommand(
                    new AccelerateCommand(fn_8040CDA8(GetPlayerVelocity(player), -30.0f), 4));
            }
        }
    }

    if (m_flags.Test(2)) {
        float angle = fn_8040EC80(-lbl_805FAD38, lbl_805FAD38);
        Vec3f position;

        GetCirclePoint(&position.x, &position.z, angle, m_params.radius);
        position += m_chara->m_position;
        new ElecFanDraw(fn_801EB304(fn_8001C4F8(), 11), *fn_800091EC(&Vec3f(), &position),
                        angle, m_params.radius, 0.5f * m_params.height);
    }
}

void ElecFan::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
}

void ElecFan::fn_800CBC1C() {
    Vec3f center;
    Vec3f offset;
    Vec3f rotation(0.5f * lbl_805FAD38, 0.0f, 0.0f);

    offset.y = 0.5f * m_params.height;
    offset.x = 0.0f;
    offset.z = 0.0f;

    fn_8040CDD0(&center, &m_chara->m_position, &offset);
    fn_802E0C60(&center, &rotation, 0, 0.5f * m_params.height, m_params.radius);
}

void ElecFan::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9E60(owner, &m_switchId);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
}

void ElecFan::Blow() {
    if (m_flags.Test(2)) {
        boost::shared_ptr<CharInfo> target(fn_800A8A7C(fn_80074194(), &m_chara->m_position));
        if (target.get() != 0) {
            float height = target->m_position.y - m_chara->m_position.y;
            if (0.0f <= height && height < m_params.height) {
                float dz = target->m_position.z - m_chara->m_position.z;
                float dx = target->m_position.x - m_chara->m_position.x;
                if (dx * dx + dz * dz <= m_params.radius * m_params.radius) {
                    if (!fn_800B7938(target)) {
                        target->PushCommand(new ClrVelocityCommand(4));
                        target->PushCommand(new Player::StreamCommand(true, 4));
                    }
                    Vec3f acceleration(0.0f, 1.0f, 0.0f);
                    fn_8040CE38(&acceleration, m_params.power);
                    float above = target->m_position.y - m_chara->m_position.y;
                    if (above < 0.0f) {
                        above = 0.0f;
                    }
                    float top = 5.0f * std::cos(lbl_8057D6A0.field_0x98) + m_params.height;
                    fn_8040CE38(&acceleration, (top - above) / top);
                    target->PushCommand(new AccelerateCommand(acceleration, 4));
                    m_target = target;
                    m_flags.Set(0);
                    fn_801D53C4(fn_80053164(), 0x7245, 0, this, 2, 0, 5);
                }
            }
            if (!m_flags.Test(0)) {
                Vec3f top;
                u8 segment[0x18];

                fn_800091EC(&top, &target->m_position);
                top.y += m_params.height;
                fn_80020A20(segment, &target->m_position, &top);
                float distance = fn_8003C63C(&target->m_position, segment, 0);
                if (distance < 10.0f + m_params.radius) {
                    fn_801D53C4(fn_80053164(), 0x7245, 0, this, 2,
                                (s8)(-127.0f * ((distance - m_params.radius) / 10.0f)), 5);
                }
            }
        }
    }
}

extern "C" void fn_80266708(void) {
    fn_8006B474("DE0403DENSIRYUU.DFF", 0);
}

extern "C" void fn_80266704(void) {}

void ElecFan::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new ElecFan(fn_801EB304(fn_8001C4F8(), 11), owner);
}

/* Defined before ElecFanDraw's functions: retail places this table and the type record before
 * their data. */
static GadgetParamDesc lbl_8054C120[] = {
    {4, "height", 0, 0, 0, 15.0f, 0.1f, 65536.0f, fn_800CBC20, 0},
    {4, "radius", 0, 0, 0, 25.0f, 0.1f, 65536.0f, fn_800CBC20, 0},
    {4, "power", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_8054C1C0 = {
    "ELEC_FAN", fn_80266708, fn_80266704, ElecFan::Instantiate, 0, 0, 0x7DA, 1, 0, 10, lbl_8054C120,
};

/* The streak's lifetime and the time its fading starts. */
const float kStreakLife = 0.333333f;
const float kStreakFade = 0.25f;

const char *ElecFanDraw::s_taskName = "ElecFanDraw";

ElecFanDraw::ElecFanDraw(Task *parent, const Vec3f &position, float angle, float radius,
                         float halfHeight)
    : Task(parent) {
    m_radius = radius;
    m_halfHeight = halfHeight;
    fn_800091EC(&m_position, &position);
    m_angle = angle;
    m_name = s_taskName;
    m_clump = (RpClump *)fn_80012700(fn_800122D4(), 1, "DE0403DENSIRYUU.DFF");
    m_render = fn_8004DFB0(fn_80046178()->field_0x28, "ElecFan");
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&ElecFanDraw::Draw, this, _1));
    m_time = 0.0f;
    float top = 300.0f + halfHeight;
    m_boundRadius = fn_8000FEE4(radius * radius + top * top);
}

ElecFanDraw::~ElecFanDraw() {}

void ElecFanDraw::Tick(float dt) {
    m_time += dt;
    if (m_time >= kStreakLife) {
        m_flags |= 1;
    }
}

void ElecFanDraw::Draw(RednerDrawArgs &args) {
    RwSphere sphere;

    sphere.radius = m_boundRadius;
    sphere.center.x = m_position.x;
    sphere.center.y = m_position.y + m_halfHeight;
    sphere.center.z = m_position.z;
    if (fn_804856C8(lbl_805F15B4->curCamera, &sphere) != 0) {
        RwRGBA color = {255, 255, 255, 255};

        if (m_time >= kStreakFade) {
            float brightness = 1.0f - (m_time - kStreakFade) / (kStreakLife - kStreakFade);
            if (brightness < 0.0f) {
                brightness = 0.0f;
            } else if (brightness > 1.0f) {
                brightness = 1.0f;
            }
            color.red = color.green = color.blue = 255.0f * brightness;
        }
        fn_80037B84(m_clump, &color);

        RwV3d position = {0.0f, 0.0f, 0.0f};
        position.x = m_position.x;
        position.y = (600.0f / kStreakLife) * m_time + m_position.y;
        position.z = m_position.z;
        RwFrame *frame = (RwFrame *)m_clump->object.parent;
        fn_800E9CEC(frame, m_angle, 0);
        fn_804875B0(frame, &position, 2);
        lbl_805F15B4->fpRenderStateSet(rwRENDERSTATECULLMODE, (void *)rwCULLMODECULLNONE);
        fn_80456BC4(m_clump);
        lbl_805F15B4->fpRenderStateSet(rwRENDERSTATECULLMODE, (void *)rwCULLMODECULLBACK);
    }
}
