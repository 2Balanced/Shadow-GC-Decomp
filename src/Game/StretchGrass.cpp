/* StretchGrass::SetStretchGrass (class name from RTTI): the NOBI_KUSA gadget, a stretching grass
 * that launches a character. It draws a morphing RenderWare clump (JG0404STRETCH.DFF with the
 * morph JG0404STRETCH.DMA, or JG0404STRETCHONE.DFF while idle) through a Render callback. A
 * character that touches its collider while it is idle is held at its tip (a
 * StartExternalControlCommand) while the grass stretches to "length"; after "charge" seconds, or
 * when the character leaves (LeaveCommand), it springs back and launches the character along its
 * axis tilted by "angle", at the speed for "level", ignoring its input for "sec" seconds. Only one
 * grass holds a character at a time. File name inferred from the namespace. Names marked "(name
 * inferred)" are not original. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). The unit's code reaches its .data and .rodata through pooled base
 * addresses, so the linker keeps those sections whole. */
/* Keeps the dead copy of bad_alloc's vtable in .data, as retail has it (see <new>). */
#define MSL_NEW_INLINE_BAD_ALLOC

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <cmath>

#include "Game/TNumEdit.h"
/* This unit's .rodata is kept whole as well, and retail has two unreferenced constant blocks of the
 * original headers in it: 0x40 bytes after the TNumEdit names and a zero vector after the
 * CharaColliBase names (their meaning and types are unknown; the bytes are reproduced). */
static const float lbl_804C119C[16] = {
    1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
};
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
static const float lbl_804C124C[3] = {0.0f, 0.0f, 0.0f};
#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharCommands.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/Render.h"
#include "Game/FileQueue.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::CharaColliBase;

/* The RenderWare types and functions used here, declared locally because the RenderWare headers
 * need the C library headers (layouts as in rw/rwcore.h and rw/rpworld.h, reduced to the members
 * used; as in Door.cpp and Cage.cpp). */
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
typedef struct RpGeometry RpGeometry;

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

typedef struct RpAtomic {
    /* 0x00 */ u8 field_0x0[0x18];
    /* 0x18 */ RpGeometry *geometry;
    /* 0x1C */ RwSphere boundingSphere;
    /* 0x2C */ u8 field_0x2C[0x4C - 0x2C];
    /* 0x4C */ s32 interpolatorFlags; /* 0x2 (rpINTERPOLATORDIRTYSPHERE): the sphere is stale */
} RpAtomic;

typedef RpAtomic *(*RpAtomicCallBack)(RpAtomic *atomic, void *data);

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
/* Turn a frame about its z, x and y axes (behaviour inferred; the last argument is the combine
 * mode). */
void fn_80109AF8(RwFrame *frame, float angle, int combine);
void fn_800E9C64(RwFrame *frame, float angle, int combine);
void fn_800E9CEC(RwFrame *frame, float angle, int combine);
/* RpClumpForAllAtomics, and the update of an atomic's bounding sphere from its morph targets. */
RpClump *fn_80456C54(RpClump *clump, RpAtomicCallBack callBack, void *data);
void fn_80456560(RpAtomic *atomic);
/* The number of morph weights of a geometry, and an atomic's morph weights (behaviour
 * inferred). */
int fn_80443A54(RpGeometry *geometry);
float *fn_804442E4(RpAtomic *atomic);
/* Prepares a clump for drawing, and attaches the morph animation anim to it (behaviour
 * inferred). */
void fn_80038B18(RpClump *clump);
void fn_80037A80(RpClump *clump, void *anim);
}

/* Target (class name from RTTI): a point other objects can aim at (meaning inferred). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);
};

/* The contacts of a collider (the object holds them 0xC bytes in; name inferred). */
inline UnkColliContacts_8001EE04 *GetContacts(CharaColliBase *collider) {
    return reinterpret_cast<UnkColliContacts_8001EE04 *>(reinterpret_cast<u8 *>(collider) + 0xC);
}

extern "C" {
/* The file loader, and a file of it: a clump or a morph animation (behaviour inferred). */
void *fn_800122D4(void);
void *fn_80012700(void *loader, int arg1, const char *fileName);
/* The collider manager, a sphere collider for charInfo, and the removal of a collider (behaviour
 * inferred; the meanings of the integer arguments are unknown). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7DEC(void *manager, const boost::weak_ptr<CharInfo> &charInfo,
                                              int flags, int arg3, int arg4, const Vec3f *offset,
                                              float radius);
void fn_800C78B0(void *manager, boost::shared_ptr<CharaColliBase> *collider);
/* Creates a target with the given flags, and removes a gadget's target (behaviour inferred). */
boost::shared_ptr<Target> fn_801E72DC(int flags);
void fn_801E7108(boost::shared_ptr<Target> *target);
/* Calls callback with every character in the collision event (behaviour inferred). */
void fn_8015AE44(ColliDetectArgs &args, void (*callback)(boost::shared_ptr<CharInfo> hit, void *user),
                 void *user);
/* Plays sound id for owner (behaviour inferred; the other arguments are unknown). */
void fn_801D53C4(void *manager, int id, int arg2, Gadget *owner, int arg4, int arg5, int arg6);
}

namespace StretchGrass {

/* The parameters the owner supplies (names from the parameter table). */
struct StretchGrassParams {
    StretchGrassParams() : level(0.0f), angle(0.0f), sec(0.0f), length(0.0f), charge(0.0f) {}

    /* 0x00 */ float level;
    /* 0x04 */ float angle; /* degrees */
    /* 0x08 */ float sec;
    /* 0x0C */ float length;
    /* 0x10 */ float charge;
};
SIZE_ASSERT(StretchGrassParams, 0x14);

/* The state the gadget shares with its hit function and its drawing (names inferred). */
struct StretchGrassState {
    StretchGrassState()
        : m_state(0), m_stretch(0.0f), m_speed(0.0f), m_position(0.0f, 0.0f, 0.0f),
          m_rotation(0.0f, 0.0f, 0.0f), m_timer(0.0f), m_released(false) {}

    /* 0x00 */ s32 m_state; /* 0 new, 1 idle, 2 stretching, 3 springing back, 4 swinging, 5 held */
    /* 0x04 */ float m_stretch;
    /* 0x08 */ float m_speed;
    /* 0x0C */ Vec3f m_position;
    /* 0x18 */ Vec3f m_rotation;
    /* 0x24 */ float m_timer;
    /* 0x28 */ bool m_released; /* the character left */
};
SIZE_ASSERT(StretchGrassState, 0x2C);

class SetStretchGrass : public Gadget {
  public:
    SetStretchGrass(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~SetStretchGrass();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* The command unit's callback (name inferred). */
    void OnCommand(CharCommand &command);
    /* The Render callback (name inferred). */
    void Draw(RednerDrawArgs &args);
    /* Moves the target to the character info (name inferred). */
    void UpdateTarget();
    /* Copies the owner's position, rotation and parameters (name inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);
    /* Not called anywhere (see its definition). */
    void LaunchDirectionOutOfLine(Vec3f *direction);

    static const char *s_renderName; /* (name inferred) */
    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ StretchGrassParams m_params;
    /* 0x44 */ StretchGrassState m_state;
    /* 0x70 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x78 */ RpClump *m_clump;    /* JG0404STRETCH.DFF */
    /* 0x7C */ RpClump *m_clumpOne; /* JG0404STRETCHONE.DFF */
    /* 0x80 */ void *m_anim;        /* JG0404STRETCH.DMA */
    /* 0x84 */ float m_drawStretch;
    /* 0x88 */ StretchGrassState *m_drawState;
    /* 0x8C */ boost::shared_ptr<CharaColliBase> m_collider;
    /* 0x94 */ boost::shared_ptr<Target> m_target;
    /* 0x9C */ boost::shared_ptr<Render> m_render;
    /* 0xA4 */ Vec3f m_holdPosition;
    /* 0xB0 */ Vec3f m_holdRotation;
    /* 0xBC */ u32 pad_0xBC; /* the class size is a multiple of 8 */
};
SIZE_ASSERT(SetStretchGrass, 0xC0);

/* What Behave passes to the per-character hit function (name inferred). */
struct StretchGrassHitContext {
    StretchGrassHitContext() {}

    /* 0x00 */ boost::weak_ptr<GadgetCharInfo> m_chara;
    /* 0x08 */ StretchGrassParams *m_params;
    /* 0x0C */ StretchGrassState *m_state;
    /* 0x10 */ Vec3f *m_holdPosition;
    /* 0x14 */ Vec3f *m_holdRotation;
};
SIZE_ASSERT(StretchGrassHitContext, 0x18);

} // namespace StretchGrass

using StretchGrass::SetStretchGrass;
using StretchGrass::StretchGrassHitContext;
using StretchGrass::StretchGrassState;

const char *SetStretchGrass::s_renderName = "SetStretchGrass";
const char *SetStretchGrass::s_taskName = "SetStretchGrass";

/* Half a turn, in radians (name inferred). */
static const float s_halfTurn = 180.0f * (lbl_805FAD38 / 180.0f);

/* The grass that holds a character (name inferred). */
static StretchGrassState *s_holder;

/* Launch speed for each level. */
static const float lbl_804C1268[8] = {
    2000.0f, 2200.0f, 2400.0f, 2600.0f, 2800.0f, 3000.0f, 3200.0f, 3400.0f,
};

/* Sets the morph weights of an atomic to *data and the radius of its bounding sphere (name
 * inferred). */
static RpAtomic *SetMorphWeights(RpAtomic *atomic, void *data) {
    if (atomic->geometry != 0) {
        int count = fn_80443A54(atomic->geometry);
        if (count > 0) {
            float *weights = fn_804442E4(atomic);
            for (int i = 0; i < count; i++) {
                *weights++ = *(float *)data;
            }
        }
    }
    if (atomic->interpolatorFlags & 0x2) {
        fn_80456560(atomic);
    }
    RwSphere *sphere = &atomic->boundingSphere;
    if (sphere != 0) {
        sphere->radius = 500.0f;
    }
    return atomic;
}

/* Takes hold of an idle character, or launches the held one once the grass springs back (name
 * inferred). The level interpolation converts float to int and back; retail shares one stack slot
 * for both conversions, which GC/2.7 does not (see docs/research-notes.md). */
static void HitCharacter(boost::shared_ptr<CharInfo> hit, void *user) {
    StretchGrassHitContext *context = static_cast<StretchGrassHitContext *>(user);

    if (context == 0) {
        return;
    }

    boost::shared_ptr<GadgetCharInfo> self = context->m_chara.lock();
    if (self.get() == 0) {
        return;
    }

    if (context->m_state->m_state == 1) {
        context->m_state->m_state = 2;
        context->m_state->m_speed = 0.063f;
        context->m_state->m_released = false;
        s_holder = context->m_state;
        hit->PushCommand(new Player::StartExternalControlCommand(
            boost::weak_ptr<CharInfo>(context->m_chara), context->m_holdPosition,
            context->m_holdRotation, 1, 4));
        hit->PushCommand(new Player::SetMotionCommand(3, 4, 0.0f));
    } else if (context->m_state->m_state == 2) {
    } else if (context->m_state->m_state == 5) {
    } else if (context->m_state->m_state == 3) {
        if (!context->m_state->m_released) {
            hit->PushCommand(new Player::IgnoreInputCommand(0, context->m_params->sec));
            hit->PushCommand(new ClrVelocityCommand(4));
            hit->PushCommand(new SetPositionCommand(self->m_position, 4));

            int level = context->m_params->level;
            float speed = lbl_804C1268[level];
            if (level < 7) {
                speed += (lbl_804C1268[level + 1] - speed) * (context->m_params->level - level);
            }

            Mtx34f matrix;
            fn_8040C148(&matrix, &lbl_805E0720);
            fn_8040B848(&matrix, self->m_rotation.y);
            fn_8040B884(&matrix, self->m_rotation.x);
            fn_8040B80C(&matrix, self->m_rotation.z);
            fn_8040B884(&matrix, context->m_params->angle * (lbl_805FAD38 / 180.0f));

            Vec3f velocity(0.0f, 0.0f, 1.0f);
            AssignVector(velocity, fn_8040B67C(&matrix, &velocity));
            fn_8040CE38(&velocity, speed);
            hit->PushCommand(new SetVelocityCommand(velocity, 4));
            hit->PushCommand(new Player::LaunchCommand(0, 4));

            fn_80169BB0(boost::weak_ptr<CharInfo>(hit), 5);
            context->m_state->m_released = true;
        }
    }
}

namespace StretchGrass {

SetStretchGrass::SetStretchGrass(Task *parent, UnkPlacement_800C9CD0 *owner)
    : Gadget(parent, owner), m_clump(0), m_clumpOne(0), m_anim(0), m_drawStretch(0.0f), m_drawState(0),
      m_holdRotation(0.0f, 0.0f, 0.0f) {
    m_name = s_taskName;
    m_chara = fn_8015CD90(0x28, 4);
    m_chara->m_commandUnit = boost::shared_ptr<CharCommandUnit>(new CharCommandUnit());
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&SetStretchGrass::OnCommand, this, _1)));
    m_collider = fn_800C7DEC(fn_80021B08(), m_chara, 0x40000, 10, 0, &lbl_805E07A0, 10.0f);
    m_render = fn_8004DFB0(fn_80046178()->field_0x8, s_renderName);
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&SetStretchGrass::Draw, this, _1));
    m_clump = static_cast<RpClump *>(fn_80012700(fn_800122D4(), 1, "JG0404STRETCH.DFF"));
    m_clumpOne = static_cast<RpClump *>(fn_80012700(fn_800122D4(), 1, "JG0404STRETCHONE.DFF"));
    m_anim = fn_80012700(fn_800122D4(), 1, "JG0404STRETCH.DMA");
    m_drawState = &m_state;
    m_drawStretch = 0.0f;
    fn_80038B18(m_clump);
    fn_80038B18(m_clumpOne);
    fn_80037A80(m_clump, m_anim);
    m_target = fn_801E72DC(0x400);
    LoadFromOwner(m_owner);
    UpdateTarget();
}

SetStretchGrass::~SetStretchGrass() {
    fn_801E7108(&m_target);
    if (m_collider.get() != 0) {
        fn_800C78B0(fn_80021B08(), &m_collider);
    }
}

void SetStretchGrass::Behave(float dt) {
    bool moved = false;

    switch (m_state.m_state) {
    case 0:
        m_state.m_state = 1;
        break;
    case 1:
        break;
    case 2:
        m_state.m_speed += 0.005f;
        m_state.m_stretch += m_state.m_speed;
        if (m_state.m_stretch > m_params.length) {
            m_state.m_stretch = m_params.length;
            m_state.m_state = 5;
            m_state.m_timer = m_params.charge;
        } else {
            fn_801D53C4(fn_80053164(), 0x7531, 0, this, 2, 0, 2);
        }
        moved = true;
        break;
    case 3:
        m_state.m_stretch -= 0.42f;
        if (m_state.m_stretch < -0.36f) {
            m_state.m_stretch = -0.36f;
            m_state.m_state = 4;
        }
        moved = true;
        break;
    case 4:
        m_state.m_stretch *= 0.93f;
        m_state.m_stretch *= -1.0f;
        if (std::fabs(m_state.m_stretch) < 0.001f) {
            m_state.m_stretch = 0.0f;
            m_state.m_state = 1;
        }
        moved = true;
        break;
    case 5:
        m_state.m_timer -= dt;
        if (m_state.m_released) {
            fn_801D560C(fn_80053164(), 0x7532, 0, 0);
            m_state.m_state = 3;
        } else if (m_state.m_timer <= 0.0f) {
            fn_801D560C(fn_80053164(), 0x7532, 0, 0);
            m_state.m_state = 3;
        }
        moved = true;
        break;
    }

    if (moved && m_render.get() != 0) {
        m_drawStretch = m_state.m_stretch;
    }

    if (m_chara.get() != 0) {
        Mtx34f matrix;

        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_state.m_position);
        fn_8040B848(&matrix, m_state.m_rotation.y);
        fn_8040B884(&matrix, m_state.m_rotation.x);
        fn_8040B80C(&matrix, m_state.m_rotation.z);

        Vec3f tip(0.0f, 0.0f, -50.0f * m_state.m_stretch);
        fn_800091D0(&m_chara->m_position, &(const Vec3f &)fn_8040B67C(&matrix, &tip));
        Vec3f hold(0.0f, 0.0f, -50.0f * m_state.m_stretch - 5.0f);
        AssignVector(m_holdPosition, fn_8040B67C(&matrix, &hold));
        m_holdRotation = m_state.m_rotation;
        m_holdRotation.y += s_halfTurn;
    }

    if (m_collider.get() != 0) {
        UnkColliContacts_8001EE04 *contacts = GetContacts(m_collider.get());
        if (contacts != 0 && contacts->m_count > 0) {
            ColliDetectArgs args;
            args.m_contacts = contacts;
            args.m_index = 0;

            StretchGrassHitContext context;
            context.m_chara = m_chara;
            context.m_params = &m_params;
            context.m_state = &m_state;
            context.m_holdPosition = &m_holdPosition;
            context.m_holdRotation = &m_holdRotation;
            fn_8015AE44(args, HitCharacter, &context);
        }
    }
}

void SetStretchGrass::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    UpdateTarget();

    Mtx34f matrix;
    Vec3f start(0.0f, 0.0f, 0.0f);
    Vec3f end(0.0f, 0.0f, 1000000.0f);

    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_state.m_position);
    fn_8040B848(&matrix, m_state.m_rotation.y);
    fn_8040B884(&matrix, m_state.m_rotation.x);
    fn_8040B80C(&matrix, m_state.m_rotation.z);
    fn_8040B884(&matrix, m_params.angle * (lbl_805FAD38 / 180.0f));
    AssignVector(start, fn_8040B67C(&matrix, &start));
    AssignVector(end, fn_8040B67C(&matrix, &end));
    u8 segment[0x18];
    fn_802E0B58(fn_80020A20(segment, &start, &end), &lbl_805F06D8);

    if (m_collider.get() != 0) {
        m_collider->Reset();
    }
}

/* Not called anywhere: the linker strips it from the game. Retail pools the 1.0 of HitCharacter's
 * launch direction between the literals of Draw and those of ReloadPlacement, which compiling a
 * function that builds that direction at this point of the file reproduces (inferred, as in
 * ElecSmallCube). */
void SetStretchGrass::LaunchDirectionOutOfLine(Vec3f *direction) {
    *direction = Vec3f(0.0f, 0.0f, 1.0f);
}

void SetStretchGrass::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_state.m_position);
    fn_800C9ED4(owner, &m_state.m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    if (m_chara.get() != 0) {
        fn_800091D0(&m_chara->m_position, &m_state.m_position);
        m_chara->m_rotation = m_state.m_rotation;
    }
}

void SetStretchGrass::UpdateTarget() {
    if (m_target.get() != 0) {
        m_target->fn_800CCE38(&m_chara->m_position);
    }
}

void SetStretchGrass::Draw(RednerDrawArgs &args) {
    RwSphere sphere;

    sphere.radius = 1000.0f;
    sphere.center.x = m_drawState->m_position.x;
    sphere.center.y = m_drawState->m_position.y;
    sphere.center.z = m_drawState->m_position.z;
    if (fn_804856C8(lbl_805F15B4->curCamera, &sphere) != 0) {
        if (m_drawState == s_holder && m_drawStretch != 0.0f) {
            float stretch = m_drawStretch;
            fn_80456C54(m_clump, SetMorphWeights, &stretch);

            RwFrame *frame = (RwFrame *)m_clump->object.parent;
            fn_80109AF8(frame, m_drawState->m_rotation.z, 0);
            fn_800E9C64(frame, m_drawState->m_rotation.x, 2);
            fn_800E9CEC(frame, m_drawState->m_rotation.y, 2);
            fn_804875B0(frame, &sphere.center, 2);
            fn_80456BC4(m_clump);
        } else {
            RwFrame *frame = (RwFrame *)m_clumpOne->object.parent;
            fn_80109AF8(frame, m_drawState->m_rotation.z, 0);
            fn_800E9C64(frame, m_drawState->m_rotation.x, 2);
            fn_800E9CEC(frame, m_drawState->m_rotation.y, 2);
            fn_804875B0(frame, &sphere.center, 2);
            fn_80456BC4(m_clumpOne);
        }
    }
}

void SetStretchGrass::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0xC: /* LeaveCommand */
        m_state.m_released = true;
        break;
    }
}

} // namespace StretchGrass

extern "C" void fn_8014BD6C(void) {
    fn_8006B474("JG0404STRETCH.DFF", 0);
    fn_8006B474("JG0404STRETCHONE.DFF", 0);
    fn_8006B474("JG0404STRETCH.DMA", 0);
    s_holder = 0;
}

extern "C" void fn_8014BD68(void) {}

namespace StretchGrass {

void SetStretchGrass::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new SetStretchGrass(fn_801EB304(fn_8001C4F8(), 11), owner);
}

} // namespace StretchGrass

static GadgetParamDesc lbl_8052A200[] = {
    {4, "level", 0, 0, 0, 0.0f, 0.0f, 7.0f, fn_800CBC20, 0},
    {4, "angle", 0, 0, 0, 0.0f, 0.0f, 360.0f, fn_800CBC20, 0},
    {4, "sec", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "length", 0, 0, 0, 4.0f, 1.0f, 65536.0f, fn_800CBC20, 0},
    {4, "charge", 0, 0, 0, 0.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_8052A2F0 = {
    "NOBI_KUSA", fn_8014BD6C, fn_8014BD68, StretchGrass::SetStretchGrass::Instantiate, 0, 0, 0x1137, 1, 0,
    10, lbl_8052A200,
};
