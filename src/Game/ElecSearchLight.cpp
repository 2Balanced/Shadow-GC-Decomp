/* ElecSearchLight (class name from RTTI): the ELEC_LIGHT gadget, a searchlight (LIGHT.BIN) that
 * sweeps "RotateRange" degrees back and forth at "RotateSpeed" degrees a second and draws a cone of
 * light "LightLength" long. Of type NORMAL it grabs a player that crosses the cone, pulls it in and
 * throws it up (HomingJumpCommand), then switches off, breaks into pieces and, once no player is
 * near, flickers back on. Of type ENEMY_ID it waits for its switch ("ENEMY_ID") to turn on, then
 * turns towards the player in its light before grabbing it. File name inferred from the class.
 * Names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). The unit's code reaches its .data
 * through a pooled base address, so the linker keeps that section whole: Game/TNumEdit.h and
 * Game/CharaColliBaseCharInfo.h supply the RTTI base lists it starts with. */
/* Keeps the dead copy of bad_alloc's vtable in .data, as retail has it (see <new>). */
#define MSL_NEW_INLINE_BAD_ALLOC
/* Retail's .data of this unit holds no copy of sp_counted_base's vtable (the other kept-whole units
 * end with one); declaring its destructor out of line keeps the vtable out (see
 * boost/detail/sp_counted_base_impl.hpp). */
#define BOOST_SP_EXTERN_BASE_DESTRUCTOR

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <cmath>

#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharCommands.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetDebris.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathConstants.h"
#include "Game/Render.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* The RenderWare types and functions used here, declared locally (layouts as in rw/rwcore.h and
 * rw/rpworld.h, reduced to the members used). */
extern "C" {
typedef struct RwV3d {
    float x;
    float y;
    float z;
} RwV3d;

typedef struct RwRGBA {
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
} RwRGBA;

typedef struct RwMatrix {
    /* 0x00 */ RwV3d right;
    /* 0x0C */ u32 flags;
    /* 0x10 */ RwV3d up;
    /* 0x1C */ u32 pad1;
    /* 0x20 */ RwV3d at;
    /* 0x2C */ u32 pad2;
    /* 0x30 */ RwV3d pos;
    /* 0x3C */ u32 pad3;
} RwMatrix;

typedef struct RwIm3DVertex {
    /* 0x00 */ RwV3d objVertex;
    /* 0x0C */ RwV3d objNormal;
    /* 0x18 */ RwRGBA color;
    /* 0x1C */ float u;
    /* 0x20 */ float v;
} RwIm3DVertex;

typedef int (*RwRenderStateSetFunction)(int state, void *value);
typedef int (*RwRenderStateGetFunction)(int state, void *value);

typedef struct RwGlobals {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ RwRenderStateSetFunction fpRenderStateSet; /* dOpenDevice.fpRenderStateSet */
    /* 0x24 */ RwRenderStateGetFunction fpRenderStateGet; /* dOpenDevice.fpRenderStateGet */
} RwGlobals;

/* the engine instance */
extern RwGlobals *lbl_805F15B4;

/* RwIm3DTransform, RwIm3DRenderPrimitive and RwIm3DEnd. */
void *fn_8049A82C(RwIm3DVertex *vertices, u32 count, RwMatrix *ltm, u32 flags);
int fn_8049AAF8(int prim);
int fn_8049A90C(void);
}

enum {
    rwRENDERSTATETEXTURERASTER = 1,
    rwRENDERSTATEZWRITEENABLE = 8,
    rwRENDERSTATESRCBLEND = 10,
    rwRENDERSTATEDESTBLEND = 11,
    rwRENDERSTATEVERTEXALPHAENABLE = 12,
    rwRENDERSTATEFOGENABLE = 14,
    rwRENDERSTATECULLMODE = 20,
    rwCULLMODECULLNONE = 1,
    rwBLENDONE = 2,
    rwBLENDSRCALPHA = 5,
    rwPRIMTYPETRISTRIP = 4,
    rwMATRIXTYPEORTHONORMAL = 0x3,
    rwMATRIXINTERNALIDENTITY = 0x20000
};

/* The scene's render state calls (rw/rwcore.h macros). */
#define RwRenderStateGet(state, value) (lbl_805F15B4->fpRenderStateGet((state), (void *)(value)))
#define RwRenderStateSet(state, value) (lbl_805F15B4->fpRenderStateSet((state), (void *)(value)))

/* A colour of four bytes; its constructor is out of line (as in ElecCristalWall.cpp). */
struct UnkColor_8040D2D8 {
    UnkColor_8040D2D8(u8 r, u8 g, u8 b, u8 a);

    /* 0x0 */ u8 r;
    /* 0x1 */ u8 g;
    /* 0x2 */ u8 b;
    /* 0x3 */ u8 a;
};
SIZE_ASSERT(UnkColor_8040D2D8, 0x4);

/* The material animation of a model fn_801621F0 finds by name (as in ElecCristalWall.cpp). */
struct UnkMaterialAnim_801621F0 {
    /* 0x0 */ u8 field_0x0[0x8];
    /* 0x8 */ UnkColor_8040D2D8 color;
};

/* What fn_8015FA78 passes for each part of a model (see ElecFireWall.cpp). */
struct UnkModelPart_8015FA78 {
    /* 0x0 */ void *m_object;
    /* 0x4 */ void *field_0x4;
    /* 0x8 */ s32 m_index;
};

/* A model part as fn_8015F7BC returns it (see ElecFireWall.cpp). */
struct UnkModelPart_8015F7BC {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ void *field_0x4;
};

/* The line segment fn_80020A20 sets up (see GadgetUtil.h; names inferred). */
struct UnkSegment_80020A20 {
    /* 0x00 */ Vec3f start;
    /* 0x0C */ Vec3f end;
};

/* The fields of the game state used here (layout from its users). */
struct UnkGameState_8057D6A0 {
    /* 0x00 */ u8 field_0x0[0x28];
    /* 0x28 */ float m_dt; /* the frame time */
};

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* A vector constant (meaning unknown; see CatchStick.cpp). */
extern const Vec3f lbl_805E07B8;

class UnkPieceManager_800122D4;

extern "C" {
/* Calls callback for each part of model, with mask: bit n selects part n, bit 0x8000 shows the
 * selected parts and its absence hides them (behaviour inferred). */
void fn_8015FA78(GadgetModelInfo *model,
                 UnkModelPart_8015FA78 *(*callback)(UnkModelPart_8015FA78 *part, u32 mask), u32 mask);
/* Shows and hides a part's object (behaviour inferred). */
void fn_80038F94(void *object);
void fn_80038FC0(void *object);
/* Returns the part of model whose id is id, and the pieces' manager and lookup (see
 * ElecFireWall.cpp). */
UnkModelPart_8015F7BC *fn_8015F7BC(GadgetModelInfo *model, int id);
UnkPieceManager_800122D4 *fn_800122D4(void);
u32 fn_80012700(UnkPieceManager_800122D4 *manager, int arg1, void *arg2);
/* Turns the attack collision of a model's part off and on (behaviour inferred). */
void fn_80161230(GadgetModelInfo *model, int index, int arg2);
void fn_8016111C(GadgetModelInfo *model, int index, int arg2);
/* GadgetModelInfo helpers (see ElecCristalWall.cpp): find the material animation named name, and
 * select the material named name. */
UnkMaterialAnim_801621F0 *fn_801621F0(GadgetModelInfo *model, const char *name);
void fn_8016239C(GadgetModelInfo *model, const char *name);
/* Sets the contact callback of a model's collision (see HintRing.cpp). */
void fn_800D0B64(void *collision, const boost::function1<void, ColliDetectArgs &> &callback);
/* Creates a Render (see HintRing.cpp). */
boost::shared_ptr<Render> fn_8004E24C(void *arg0);
/* The character nearest to position, and the character on a line segment (behaviour inferred). */
boost::weak_ptr<CharInfo> fn_800A8810(UnkCharaList_80074194 *manager, void *segment);
/* Calls callback with every character in the collision event (behaviour inferred). */
void fn_8015AA84(ColliDetectArgs &args, void (*callback)(boost::shared_ptr<CharInfo> hit, void *user),
                 void *user);
/* Plays sound id for source (behaviour inferred; the other arguments are unknown). */
void fn_801D53C4(void *manager, int id, GadgetCharInfo *charInfo, void *source, int arg4, int arg5,
                 int arg6);
/* The angles of a rotation matrix (behaviour inferred). */
void fn_800494F8(Vec3f *rotation, const Mtx34f *matrix);
/* The pitch and yaw of a direction (behaviour inferred). */
void fn_8003B9E8(const Vec3f *direction, float *pitch, float *yaw);
/* matrix * vector without the translation (behaviour inferred). */
Vec3f fn_8040B618(const Mtx34f *matrix, const Vec3f *vector);
/* vector * scale, a + b, direction normalised, a . b, and the squared distance of two points
 * (behaviour inferred). */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CEE8(const Vec3f *vector);
float fn_8040CF90(const Vec3f *a, const Vec3f *b);
float fn_8040D180(const Vec3f *a, const Vec3f *b);
}

/* The type descriptor's load and unload hooks and the label callback (defined at the end). */
extern "C" {
void fn_801534FC(void);
void fn_801534F8(void);
const char *fn_80153478(UnkLabelQuery_800CBC20 *cursor);
}

extern "C" {
/* The sound manager and its play call, matrix * point, and a line segment from start to end
 * (see GadgetUtil.h, which this unit does not include: it declares pi as a variable). */
void *fn_80053164(void);
void fn_801D560C(void *manager, int soundId, int arg2, int arg3);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
void *fn_80020A20(void *segment, const Vec3f *start, const Vec3f *end);
}

/* Turns current towards target by at most step (behaviour inferred). */
float TurnAngleToward(float target, float current, float step);

/* fn_8040CD74 called as returning a - b by value (as in ChaoHolder.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* a + b (name inferred). */
inline Vec3f AddVectors(const Vec3f &a, const Vec3f &b) {
    return fn_8040CDD0(&a, &b);
}

/* a to b as t goes from 0 to 1 (name inferred). */
inline float Lerp(float a, float b, float t) {
    return (1.0f - t) * a + t * b;
}

/* Degrees to radians (name inferred). */
inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* Set a vertex's position and normal, reading all three components before storing them (macros in
 * the manner of RwIm3DVertexSetPos and RwIm3DVertexSetNormal; names inferred). */
#define SetVertexPosition(vertex, px, py, pz)     do {         float x_ = (px);         float y_ = (py);         float z_ = (pz);         (vertex)->objVertex.x = x_;         (vertex)->objVertex.y = y_;         (vertex)->objVertex.z = z_;     } while (0)
#define SetVertexNormal(vertex, px, py, pz)     do {         float x_ = (px);         float y_ = (py);         float z_ = (pz);         (vertex)->objNormal.x = x_;         (vertex)->objNormal.y = y_;         (vertex)->objNormal.z = z_;     } while (0)

/* The parameters the owner supplies (names from the parameter table). */
struct ElecSearchLightParams {
    /* 0x0 */ s32 type;        /* 0 NORMAL, 1 ENEMY_ID */
    /* 0x4 */ float rotateRange; /* degrees */
    /* 0x8 */ float rotateSpeed; /* degrees a second */
    /* 0xC */ float lightLength;
};
SIZE_ASSERT(ElecSearchLightParams, 0x10);

class ElecSearchLight : public Gadget {
  public:
    ElecSearchLight(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~ElecSearchLight();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Method names are inferred. */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    /* Places the model and the light at the gadget. */
    void UpdateMatrix();
    /* The collider's and the command unit's callbacks. */
    void OnColliDetect(ColliDetectArgs &args);
    void OnCommand(CharCommand &command);
    /* The squared distance of point from segment; the nearest point goes to nearest and its place
     * on the segment, measured from the end, to place. */
    float GetSegmentDistanceSq(const Vec3f *point, const UnkSegment_80020A20 *segment,
                               Vec3f *nearest, float *place);
    /* Grabs a player that crosses the light. */
    void Search();
    /* Takes hold of the target at position. */
    void Capture(const Vec3f *position);
    /* Pulls the target in and throws it up. */
    void Pull();
    /* The angles the light points along. */
    void GetLightAngles(Vec3f *angles);
    /* Sweeps the light. */
    void UpdateSweep();
    void SpawnDebris();
    /* Turns the light towards the target. */
    void Track(float dt);
    /* Builds the light's cone from the light's matrix. */
    void BuildCone(const Mtx34f *matrix);
    /* Fades the lamp and the light back in. */
    void Revive(float dt);
    /* The Render callback. */
    void Draw(RednerDrawArgs &args);

    /* Shows or hides the parts of the model mask selects. */
    static UnkModelPart_8015FA78 *ShowParts(UnkModelPart_8015FA78 *part, u32 mask);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x030 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x038 */ GadgetModelInfo *m_model;
    /* 0x03C */ ElecSearchLightParams m_params;
    /* 0x04C */ u8 m_switchId;
    /* 0x050 */ s32 m_switchState;
    /* 0x054 */ s32 m_state; /* 0 searching, 1 tracking, 2 pulling, 3 breaking, 4 broken, 5 reviving */
    /* 0x058 */ float m_pitch;
    /* 0x05C */ float m_yaw;
    /* 0x060 */ float m_timer;
    /* 0x064 */ Vec3f m_lightEnd;
    /* 0x070 */ boost::shared_ptr<CharInfo> m_target;
    /* 0x078 */ boost::shared_ptr<Render> m_render;
    /* 0x080 */ RwIm3DVertex m_vertices[26]; /* a strip of 12 sides, closed by repeating the first */
    /* 0x428 */ float m_alpha;
    /* 0x42C */ u32 pad_0x42C;
};
SIZE_ASSERT(ElecSearchLight, 0x430);

/* What the collision callback passes to the per-character hit function (name inferred). */
struct ElecSearchLightHitContext {
    ElecSearchLightHitContext() : m_params(0) {}

    /* 0x0 */ boost::weak_ptr<GadgetCharInfo> m_chara;
    /* 0x8 */ ElecSearchLightParams *m_params;
};
SIZE_ASSERT(ElecSearchLightHitContext, 0xC);

/* The light's turning speed, in radians a second (name inferred). */
static const float s_turnSpeed = 2.0f * lbl_805FAD38;

/* The lamp's colours while it comes back on (names inferred). */
static UnkColor_8040D2D8 s_black(0, 0, 0, 0);
static UnkColor_8040D2D8 s_yellow(0xFF, 0xFF, 0x33, 0);
static UnkColor_8040D2D8 s_white(0xFF, 0xFF, 0xFF, 0xFF);

const char *ElecSearchLight::s_taskName = "ElecSearchLight";

/* Does nothing (name inferred). Retail emits it after the constructor and its template instances,
 * so it is defined before the constructor. */
static void HitCharacter(boost::shared_ptr<CharInfo> hit, void *user) {}

ElecSearchLight::ElecSearchLight(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_chara = fn_8015CB88(5, 4, "ElecSearchLight");
    m_chara->m_commandUnit = boost::shared_ptr<CharCommandUnit>(new CharCommandUnit());
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&ElecSearchLight::OnCommand, this, _1)));
    m_model = new GadgetModelInfo(1, "LIGHT.BIN");
    m_model->fn_80162A68(0, 0);
    fn_8016239C(m_model, "mat1");
    fn_8016239C(m_model, "mat2");
    fn_8016239C(m_model, "mat3");
    fn_8016239C(m_model, "mat4");
    m_model->fn_80161F08("col", m_chara);
    fn_800D0B64(m_model->field_0x4, boost::bind(&ElecSearchLight::OnColliDetect, this, _1));
    LoadFromOwner(m_owner);
    m_timer = 0.0f;
    m_state = 0;
    m_pitch = 0.0f;
    m_yaw = 0.0f;
    m_switchState = fn_8016904C(fn_800CB69C(), m_switchId);
    fn_8015FA78(m_model, ShowParts, 0x8007);
    m_alpha = 1.0f;
    UpdateMatrix();
    m_render = fn_8004E24C(fn_80046178()->field_0x28);
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&ElecSearchLight::Draw, this, _1));
}

ElecSearchLight::~ElecSearchLight() {
    if (m_model != 0) {
        delete m_model;
        m_model = 0;
    }
}

UnkModelPart_8015FA78 *ElecSearchLight::ShowParts(UnkModelPart_8015FA78 *part, u32 mask) {
    if ((1 << part->m_index) & mask) {
        if (mask & 0x8000) {
            fn_80038F94(part->m_object);
        } else {
            fn_80038FC0(part->m_object);
        }
        return part;
    }
    return part;
}

/* Blends animation's colour from from to to as t goes from t0 to t1 (name inferred). Other units
 * call this copy. */
extern "C" void fn_80154C6C(UnkMaterialAnim_801621F0 *animation, const UnkColor_8040D2D8 *from,
                            const UnkColor_8040D2D8 *to, float t, float t0, float t1) {
    float ratio = (t - t0) / (t1 - t0);
    animation->color.r = ratio * (to->r - from->r) + from->r;
    animation->color.g = ratio * (to->g - from->g) + from->g;
    animation->color.b = ratio * (to->b - from->b) + from->b;
    animation->color.a = ratio * (to->a - from->a) + from->a;
}

void ElecSearchLight::Revive(float dt) {
    UnkMaterialAnim_801621F0 *material1 = fn_801621F0(m_model, "mat1");
    UnkMaterialAnim_801621F0 *material2 = fn_801621F0(m_model, "mat2");
    UnkMaterialAnim_801621F0 *material3 = fn_801621F0(m_model, "mat3");
    UnkMaterialAnim_801621F0 *material4 = fn_801621F0(m_model, "mat4");
    float previous = m_timer;

    m_timer = previous + dt;
    if (previous < 0.666667f && 0.666667f <= m_timer) {
        fn_8015FA78(m_model, ShowParts, 0x8001);
    } else if (previous < 1.0f && 1.0f <= m_timer) {
        fn_8015FA78(m_model, ShowParts, 0x8006);
        fn_8015FA78(m_model, ShowParts, 8);
    }

    if (m_timer < 0.666667f) {
        fn_80154C6C(material4, &s_black, &s_yellow, m_timer, 0.0f, 0.666667f);
    } else if (m_timer < 1.0f) {
        fn_80154C6C(material4, &s_yellow, &s_black, m_timer, 0.666667f, 1.0f);
        fn_80154C6C(material1, &s_black, &s_white, m_timer, 0.666667f, 1.0f);
    } else if (m_timer < 1.5f) {
        fn_80154C6C(material2, &s_black, &s_white, m_timer, 1.0f, 1.5f);
        fn_80154C6C(material3, &s_black, &s_white, m_timer, 1.0f, 1.5f);
        m_alpha = 1.0f - (1.5f - m_timer) / 0.5f;
    } else {
        m_state = 0;
        m_alpha = 1.0f;
        m_timer = 0.0f;
    }
}

void ElecSearchLight::Behave(float dt) {
    if (m_model != 0) {
        m_model->fn_80162FB0(dt);
    }

    switch (m_params.type) {
    case 0:
        switch (m_state) {
        case 0:
            UpdateSweep();
            Search();
            break;
        case 1:
            break;
        case 2:
            Pull();
            break;
        case 3:
            fn_8015FA78(m_model, ShowParts, 7);
            m_alpha = 0.0f;
            m_state = 4;
            m_timer = 0.0f;
            SpawnDebris();
            fn_80161230(m_model, 0, -1);
            break;
        case 4:
            m_timer += dt;
            if (m_timer >= 3.0f) {
                boost::shared_ptr<CharInfo> nearest(fn_800A8A7C(fn_80074194(), &m_chara->m_position));
                if (nearest.get() != 0) {
                    Vec3f delta;
                    Vec3f difference;
                    fn_8040CD74(&difference, &m_chara->m_position, &nearest->m_position);
                    if (fn_8040CF2C(fn_800091EC(&delta, &difference)) > 10000.0f) {
                        m_state = 5;
                        m_timer = 0.0f;
                        m_yaw = 0.0f;
                        fn_8015FA78(m_model, ShowParts, 0x8008);
                    }
                }
            }
            break;
        case 5:
            Revive(dt);
            fn_8016111C(m_model, 0, -1);
            break;
        }
        break;
    case 1:
        switch (m_state) {
        case 0:
            UpdateSweep();
            if (fn_8016904C(fn_800CB69C(), m_switchId) == 1 && m_switchState != 1) {
                u8 segment[0x18];
                boost::shared_ptr<CharInfo> hit(fn_800A8810(
                    fn_80074194(), fn_80020A20(segment, &m_chara->m_position, &m_lightEnd)));
                if (hit.get() != 0) {
                    m_target = hit;
                    m_state = 1;
                }
            }
            m_switchState = fn_8016904C(fn_800CB69C(), m_switchId);
            break;
        case 1:
            Track(dt);
            break;
        case 2:
            Pull();
            break;
        case 3:
            fn_8015FA78(m_model, ShowParts, 7);
            m_alpha = 0.0f;
            m_state = 4;
            m_timer = 0.0f;
            SpawnDebris();
            fn_80161230(m_model, 0, -1);
            break;
        case 4:
            m_timer += dt;
            if (m_timer >= 3.0f) {
                boost::shared_ptr<CharInfo> nearest(fn_800A8A7C(fn_80074194(), &m_chara->m_position));
                if (nearest.get() != 0) {
                    Vec3f delta;
                    Vec3f difference;
                    fn_8040CD74(&difference, &m_chara->m_position, &nearest->m_position);
                    if (fn_8040CF2C(fn_800091EC(&delta, &difference)) > 10000.0f) {
                        m_state = 5;
                        m_timer = 0.0f;
                        m_yaw = 0.0f;
                        fn_8015FA78(m_model, ShowParts, 0x8008);
                    }
                }
            }
            break;
        case 5:
            Revive(dt);
            fn_8016111C(m_model, 0, -1);
            break;
        }
        break;
    }

    UpdateMatrix();
}

void ElecSearchLight::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    UpdateSweep();
    UpdateMatrix();
    if (m_model != 0) {
        m_model->fn_80162EF4();
        m_model->fn_8016108C();
    }
}

void ElecSearchLight::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    fn_800C9E60(owner, &m_switchId);
}

void ElecSearchLight::UpdateMatrix() {
    if (m_model != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_chara->m_position);
        fn_8040B848(&matrix, m_chara->m_rotation.y);
        fn_8040B884(&matrix, m_chara->m_rotation.x);
        fn_8040B80C(&matrix, m_chara->m_rotation.z);
        fn_8040B848(&matrix, m_yaw);
        fn_8040B884(&matrix, m_pitch);
        fn_800091D0(&m_lightEnd, &(const Vec3f &)fn_8040B67C(
                                     &matrix, &Vec3f(0.0f, 0.0f, 10.0f * -m_params.lightLength)));
        m_model->fn_801633DC(&matrix);
        BuildCone(&matrix);
    }
}

void ElecSearchLight::OnColliDetect(ColliDetectArgs &args) {
    ElecSearchLightHitContext context;

    context.m_chara = m_chara;
    context.m_params = &m_params;
    fn_8015AA84(args, HitCharacter, &context);
}

void ElecSearchLight::OnCommand(CharCommand &command) {}

float ElecSearchLight::GetSegmentDistanceSq(const Vec3f *point,
                                            const UnkSegment_80020A20 *segment,
                                            Vec3f *nearest, float *place) {
    Vec3f direction;
    fn_800091EC(&direction,
                &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&segment->end, &segment->start));
    Vec3f offset;
    fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(point, &segment->start));
    float dot = fn_8040CF90(&offset, &direction);
    if (dot > 0.0f) {
        float dirLenSq = fn_8040CF90(&direction, &direction);
        if (dot >= dirLenSq) {
            if (nearest != 0) {
                fn_800091D0(nearest, &segment->end);
            }
            if (place != 0) {
                *place = 0.0f;
            }
            return fn_8040D180(point, &segment->end);
        }
        dot /= dirLenSq;
        Vec3f position;
        fn_800091EC(&position, &(const Vec3f &)fn_8040CDD0(
                                   &segment->start, &(const Vec3f &)fn_8040CDA8(&direction, dot)));
        if (nearest != 0) {
            fn_800091D0(nearest, &position);
        }
        if (place != 0) {
            *place = dot;
        }
        return fn_8040D180(point, &position);
    }
    if (nearest != 0) {
        fn_800091D0(nearest, &segment->start);
    }
    if (place != 0) {
        *place = 1.0f;
    }
    return fn_8040CF2C(&offset);
}

void ElecSearchLight::Search() {
    u8 segment[0x18];

    fn_80020A20(segment, &m_chara->m_position, &m_lightEnd);
    boost::shared_ptr<CharInfo> hit(fn_800A8810(fn_80074194(), segment));
    if (hit.get() == 0) {
        return;
    }
    Vec3f nearest;
    float place;
    float distanceSq = GetSegmentDistanceSq(&hit->m_position, (const UnkSegment_80020A20 *)segment,
                                            &nearest, &place);
    if (place < 0.0f || 1.0f < place) {
        return;
    }
    float radius = Lerp(25.0f, 50.0f, place);
    if (distanceSq < radius * radius) {
        m_target = hit;
        Capture(&nearest);
    }
}

void ElecSearchLight::Capture(const Vec3f *position) {
    Vec3f angles(0.0f, 0.0f, 0.0f);

    GetLightAngles(&angles);
    m_target->PushCommand(new ClrVelocityCommand(4));
    m_target->PushCommand(new Player::StartExternalControlCommand(0, 2));
    m_target->PushCommand(new Player::SetMotionCommand(0x26, 2, 0.0f));
    m_target->PushCommand(new SetPositionCommand(*position, 4));
    m_target->PushCommand(new SetAngleCommand(angles, 2, 4));
    m_state = 2;
}

void ElecSearchLight::Pull() {
    Vec3f delta;
    fn_800091EC(&delta, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                            &m_chara->m_position, &m_target->m_position));
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)AddVectors(
                               m_target->m_position,
                               fn_8040CDA8(&(const Vec3f &)fn_8040CEE8(&delta), 20.0f)));
    m_target->PushCommand(new SetPositionCommand(position, 4));
    fn_801D53C4(fn_80053164(), 0x7230, 0, this, 2, 0, 5);
    if (fn_8040CF2C(&delta) < 2500.0f) {
        m_target->PushCommand(new Player::EndExternalControlCommand(4));
        m_target->PushCommand(new ClrVelocityCommand(4));
        m_target->PushCommand(new Player::HomingJumpCommand(Vec3f(0.0f, 1.5f, 0.0f), 4));
        m_state = 3;
    }
}

void ElecSearchLight::GetLightAngles(Vec3f *angles) {
    Mtx34f matrix;

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B848(&matrix, m_chara->m_rotation.y);
    fn_8040B884(&matrix, m_chara->m_rotation.x);
    fn_8040B80C(&matrix, m_chara->m_rotation.z);
    fn_8040B848(&matrix, m_yaw);
    fn_8040B884(&matrix, m_pitch);
    fn_8040B848(&matrix, lbl_805FAD38);
    fn_800494F8(angles, &matrix);
}

void ElecSearchLight::UpdateSweep() {
    m_timer += lbl_8057D6A0.m_dt;
    m_yaw = std::sin(m_timer * DegToRad(m_params.rotateSpeed)) * DegToRad(m_params.rotateRange);
}

void ElecSearchLight::SpawnDebris() {
    if (m_model != 0) {
        UnkModelPart_8015F7BC *part = fn_8015F7BC(m_model, 4);
        if (part != 0) {
            UnkPieceManager_800122D4 *manager = fn_800122D4();
            UnkDebrisParam_8015F0B8 pieces(m_chara, 0);

            pieces.scale = 1.3f;
            pieces.model = fn_80012700(manager, 1, part->field_0x4);
            pieces.field_0x54 = 0;
            pieces.field_0x4C = 150;
            fn_8015D93C(&pieces);
        }
    }
    fn_801D560C(fn_80053164(), 0x7231, 0, 0);
}

void ElecSearchLight::Track(float dt) {
    Vec3f direction;
    fn_800091EC(&direction, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                &m_chara->m_position, &m_target->m_position));
    Vec3f angles = lbl_805E07B8;
    fn_8003B9E8(&direction, &angles.x, &angles.y);

    Mtx34f matrix;
    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B80C(&matrix, -m_chara->m_rotation.z);
    fn_8040B884(&matrix, -m_chara->m_rotation.x);
    fn_8040B848(&matrix, -m_chara->m_rotation.y);
    fn_8040B848(&matrix, angles.y);
    fn_8040B884(&matrix, angles.x);
    fn_800494F8(&angles, &matrix);

    m_pitch = TurnAngleToward(angles.x, m_pitch, s_turnSpeed * dt);
    m_yaw = TurnAngleToward(angles.y, m_yaw, s_turnSpeed * dt);
    if (m_pitch == angles.x && m_yaw == angles.y) {
        Capture(&m_target->m_position);
    }
}

void ElecSearchLight::BuildCone(const Mtx34f *matrix) {
    float step = 2.0f * lbl_805FAD38 / 12.0f;
    Mtx34f rotation;

    fn_8040C148(&rotation, matrix);
    for (int i = 0; i < 12; i++) {
        Vec3f point;
        fn_800091D0(&point, &(const Vec3f &)fn_8040B67C(&rotation, &Vec3f(0.0f, 25.0f, -40.0f)));
        SetVertexPosition(&m_vertices[i * 2], point.x, point.y, point.z);
        fn_800091D0(&point, &(const Vec3f &)fn_8040B67C(
                                 &rotation, &Vec3f(0.0f, 50.0f, 10.0f * -m_params.lightLength)));
        SetVertexPosition(&m_vertices[i * 2 + 1], point.x, point.y, point.z);
        Vec3f normal;
        fn_800091D0(&normal, &(const Vec3f &)fn_8040B618(&rotation, &Vec3f(0.0f, 1.0f, 0.0f)));
        SetVertexNormal(&m_vertices[i * 2], normal.x, normal.y, normal.z);
        SetVertexNormal(&m_vertices[i * 2 + 1], normal.x, normal.y, normal.z);
        if (i == 0) {
            fn_800091D0(&point, &(const Vec3f &)fn_8040B67C(&rotation, &Vec3f(0.0f, 25.0f, -40.0f)));
            SetVertexPosition(&m_vertices[24], point.x, point.y, point.z);
            fn_800091D0(&point, &(const Vec3f &)fn_8040B67C(
                                     &rotation, &Vec3f(0.0f, 50.0f, 10.0f * -m_params.lightLength)));
            SetVertexPosition(&m_vertices[25], point.x, point.y, point.z);
            SetVertexNormal(&m_vertices[24], normal.x, normal.y, normal.z);
            SetVertexNormal(&m_vertices[25], normal.x, normal.y, normal.z);
        }
        fn_8040B80C(&rotation, step);
    }

    u8 innerRed = 128.0f * m_alpha;
    u8 innerGreen = 96.0f * m_alpha;
    u8 innerBlue = 64.0f * m_alpha;
    u8 innerAlpha = 32.0f * m_alpha;
    u8 outerRed = 96.0f * m_alpha;
    u8 outerGreen = 64.0f * m_alpha;
    u8 outerBlue = 48.0f * m_alpha;
    u8 outerAlpha = 48.0f * m_alpha;
    for (int i = 0; i < 12; i++) {
        m_vertices[i * 2].color.red = innerRed;
        m_vertices[i * 2].color.green = innerGreen;
        m_vertices[i * 2].color.blue = innerBlue;
        m_vertices[i * 2].color.alpha = innerAlpha;
        m_vertices[i * 2 + 1].color.red = outerRed;
        m_vertices[i * 2 + 1].color.green = outerGreen;
        m_vertices[i * 2 + 1].color.blue = outerBlue;
        m_vertices[i * 2 + 1].color.alpha = outerAlpha;
        if (i == 0) {
            m_vertices[24].color.red = innerRed;
            m_vertices[24].color.green = innerGreen;
            m_vertices[24].color.blue = innerBlue;
            m_vertices[24].color.alpha = innerAlpha;
            m_vertices[25].color.red = outerRed;
            m_vertices[25].color.green = outerGreen;
            m_vertices[25].color.blue = outerBlue;
            m_vertices[25].color.alpha = outerAlpha;
        }
    }
}

void ElecSearchLight::Draw(RednerDrawArgs &args) {
    RwMatrix matrix;
    u32 savedZWrite;
    u32 fog;
    u32 cull;

    RwRenderStateGet(rwRENDERSTATEZWRITEENABLE, &savedZWrite);
    RwRenderStateGet(rwRENDERSTATEFOGENABLE, &fog);
    RwRenderStateGet(rwRENDERSTATECULLMODE, &cull);
    RwRenderStateSet(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, 0);
    RwRenderStateSet(rwRENDERSTATEFOGENABLE, 0);
    RwRenderStateSet(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, rwBLENDONE);
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, 1);
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, 0);

    matrix.right.x = matrix.up.y = matrix.at.z = 1.0f;
    matrix.right.y = matrix.right.z = matrix.up.x = 0.0f;
    matrix.up.z = matrix.at.x = matrix.at.y = 0.0f;
    matrix.pos.x = matrix.pos.y = matrix.pos.z = 0.0f;
    matrix.flags |= rwMATRIXINTERNALIDENTITY | rwMATRIXTYPEORTHONORMAL;
    if (fn_8049A82C(m_vertices, 26, &matrix, 0x18) != 0) {
        fn_8049AAF8(rwPRIMTYPETRISTRIP);
        fn_8049A90C();
    }

    RwRenderStateSet(rwRENDERSTATECULLMODE, cull);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, savedZWrite);
    RwRenderStateSet(rwRENDERSTATEFOGENABLE, fog);
}

/* The labels of the Type parameter. */
static const char *s_typeLabels[2] = {"NORMAL", "ENEMY_ID"};

static GadgetParamDesc lbl_8052A7F4[] = {
    {1, "Type", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_80153478, 0},
    {4, "RotateRange", 0, 0, 0, 0.0f, -65536.0f, 65536.0f, fn_800CBC20, 0},
    {4, "RotateSpeed", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "LightLength", 0, 0, 0, 100.0f, 40.0f, 1000.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_8052A8BC = {
    "ELEC_LIGHT", fn_801534FC, fn_801534F8, ElecSearchLight::Instantiate, 0, 0, 0x7D1, 1, 0, 10,
    lbl_8052A7F4,
};

extern "C" void fn_801534FC(void) {
    fn_8006B474("LIGHT.BIN", 1);
}

extern "C" void fn_801534F8(void) {}

void ElecSearchLight::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new ElecSearchLight(fn_801EB304(fn_8001C4F8(), 11), owner);
}

/* The fields of the cursor that the label callback reads (layout from its accesses). */
struct ElecSearchLightParamCursor {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ int index; /* the parameter */
    /* 0x8 */ int value;
};

/* The label callback of the Type parameter. */
extern "C" const char *fn_80153478(UnkLabelQuery_800CBC20 *cursor) {
    const ElecSearchLightParamCursor *fields = (const ElecSearchLightParamCursor *)cursor;

    switch (fields->index) {
    case 0:
        return s_typeLabels[fields->value];
    }
    return 0;
}
