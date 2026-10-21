/* ElecKyuu (class name from RTTI): the ELEC_KYUU gadget of the electric circuit area. It turns its
 * character info about the vertical axis by a third of a turn per unit of dt and draws the
 * DE0403KYUU1.DFF clump sixteen times around that axis from a draw callback it registers with its
 * Render, while a collision built for its character info follows it. File name inferred from the
 * class. Method names marked "(name inferred)" are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/MathConstants.h"
#include "Game/FileQueue.h"
#include "boost/bind.hpp"
#include "boost/function.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

class GadgetCharCollision;

/* The RenderWare types and functions used here, declared locally because the RenderWare headers
 * need the C library headers (layouts as in rw/rwcore.h and rw/rpworld.h, reduced to the members
 * used). */
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
}

/* The argument a Render passes to its draw callbacks (struct name from the game's RTTI, which
 * spells it "Redner"). */
struct RednerDrawArgs;

/* Render (class name from the RTTI of its shared_ptr control block): a 0x28-byte drawing hook. Only
 * the list of draw callbacks at 0x14 is known (name inferred). */
class Render {
  public:
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ u8 m_drawCallbacks[0x28 - 0x14];
};
SIZE_ASSERT(Render, 0x28);

/* The object fn_80046178 returns; the word at 0x8 is passed when creating a Render (meaning
 * unknown). */
struct UnkRenderOwner_80046178 {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ u32 field_0x4;
    /* 0x8 */ void *field_0x8;
};

extern "C" {
UnkRenderOwner_80046178 *fn_80046178(void);
/* Creates a Render named name (behaviour inferred). */
boost::shared_ptr<Render> fn_8004DFB0(void *arg0, const char *name);
/* Adds a draw callback to a Render's list (behaviour inferred). */
void fn_80062CD0(void *callbacks, const boost::function1<void, RednerDrawArgs &> &callback);

/* Loads the clump of a model file (behaviour inferred). */
void *fn_800122D4(void);
RpClump *fn_80012700(void *loader, int arg1, const char *fileName);
/* Renders a clump. */
RpClump *fn_80456BC4(RpClump *clump);
/* Turns a frame about its y axis to angle (behaviour inferred). */
void fn_800E9CEC(RwFrame *frame, float angle, int arg2);

/* The collision manager (behaviour inferred). */
void *fn_80021B08(void);
/* Creates a collision for owner (behaviour inferred; the meaning of the arguments is unknown). */
boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliBase>
fn_800C7DEC(void *manager, const boost::weak_ptr<CharInfo> &owner, int flags, int arg3, int arg4,
            const Vec3f *offset, float radius);
/* Wraps a collision in the handle gadgets keep (behaviour inferred). */
boost::shared_ptr<GadgetCharCollision>
fn_8015B8C4(boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> collision);
}

class ElecKyuu : public Gadget {
  public:
    ElecKyuu(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~ElecKyuu();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position and rotation into the gadget (name inferred). */
    void ReadPlacement(UnkPlacement_800C9CD0 *owner);
    /* Turns the character info about its y axis by a third of a turn per unit of dt (name
     * inferred). */
    void Spin(float dt);
    /* The draw callback: draws the clump sixteen times around the vertical axis when the character
     * info is in view (name inferred). */
    void Draw(RednerDrawArgs &args);

    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<Render> m_render;
    /* 0x38 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x40 */ boost::shared_ptr<GadgetCharCollision> m_collision;
    /* 0x48 */ RpClump *m_clump;
    /* 0x4C */ u32 field_0x4C;
};
SIZE_ASSERT(ElecKyuu, 0x50);

const char *ElecKyuu::s_taskName = "ElecKyuu";

ElecKyuu::ElecKyuu(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CB88(0x58, 4, "ElecKyuu");
    m_clump = fn_80012700(fn_800122D4(), 1, "DE0403KYUU1.DFF");
    m_render = fn_8004DFB0(fn_80046178()->field_0x8, "ElecKyuu");
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&ElecKyuu::Draw, this, _1));

    boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> collision =
        fn_800C7DEC(fn_80021B08(), m_chara, 0x20202, 10, 0, &lbl_805E07A0, 302.5f);
    m_collision = fn_8015B8C4(collision);
    ReadPlacement(m_owner);
}

ElecKyuu::~ElecKyuu() {}

void ElecKyuu::Draw(RednerDrawArgs &args) {
    RwV3d position = {0.0f, 0.0f, 0.0f};
    RwSphere sphere;

    position.x = m_chara->m_position.x;
    position.y = m_chara->m_position.y;
    position.z = m_chara->m_position.z;
    sphere.radius = 302.5f;
    sphere.center.x = position.x;
    sphere.center.y = position.y;
    sphere.center.z = position.z;
    if (fn_804856C8(lbl_805F15B4->curCamera, &sphere) != 0) {
        RwFrame *frame = (RwFrame *)m_clump->object.parent;

        float step = 2.0f * lbl_805FAD38 / 16.0f;
        for (int i = 0; i < 16; i++) {
            fn_800E9CEC(frame, i * step + m_chara->m_rotation.y, 0);
            fn_804875B0(frame, &position, 2);
            fn_80456BC4(m_clump);
        }
    }
}

void ElecKyuu::Behave(float dt) {
    Spin(dt);
}

void ElecKyuu::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    ReadPlacement(owner);
}

void ElecKyuu::ReadPlacement(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
}

void ElecKyuu::Spin(float dt) {
    m_chara->m_rotation.y += dt * (2.0f * lbl_805FAD38 / 3.0f);
}

extern "C" void fn_80230594(void) {
    fn_8006B474("DE0403KYUU1.DFF", 0);
}

extern "C" void fn_80230590(void) {}

void ElecKyuu::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new ElecKyuu(fn_801EB304(fn_8001C4F8(), 11), owner);
}

GadgetClassDesc lbl_80546F34 = {
    "ELEC_KYUU", fn_80230594, fn_80230590, ElecKyuu::Instantiate, 0, 0, 0x84A, 1, 0, 10, 0,
};
