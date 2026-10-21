/* VacuumRing: the ring a vacuum pulls in (class name from RTTI: @unnamed@VacuumRing_cpp@::Ring,
 * which also gives the file name). A ring gadget hands a collected ring to it (fn_80238A78, called
 * by Game/RegularRing.cpp); it draws the ring model through a Render callback while the ring flies
 * to the vacuum, and on arrival gives the ring to the vacuum's owner. Names other than the class
 * name are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/GadgetUtil.h"
#include "Game/Render.h"
#include "Game/VacuumObject.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

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

/* The renderer state the ring drawing sets up and restores (class name unknown; see
 * RegularRing.cpp). */
struct UnkRenderer_800289B0 {
    /* 0x000 */ u8 field_0x0[0x456];
    /* 0x456 */ u8 field_0x456;
};

extern "C" {
/* Returns the file loader, and the clump id of it (behaviour inferred). */
void *fn_800122D4(void);
RpClump *fn_8001277C(void *loader, int arg1, int id);
/* Gives target the item id, and sends message to the character (behaviour inferred; see
 * RegularRing.cpp). */
void fn_8015F6EC(boost::shared_ptr<CharInfo> target, int id, int arg2, int arg3);
int fn_8020642C(const boost::shared_ptr<CharInfo> &character, int message);
/* Plays the ring sound on the sound manager (behaviour inferred). */
void fn_801D5300(void *sound);
/* The renderer, and its state changes around the rings (behaviour inferred). */
UnkRenderer_800289B0 *fn_800289B0(void);
void fn_80048390(UnkRenderer_800289B0 *renderer, int mode);
void fn_800482E0(UnkRenderer_800289B0 *renderer, int value);
void fn_80047780(UnkRenderer_800289B0 *renderer);
}

namespace {

class Ring : public VacuumObject {
  public:
    Ring(VacuumCommand &command, bool reward);
    virtual ~Ring();
    virtual void fn_80238180();

    void Draw(RednerDrawArgs &args);

    /* Starts a ring at position and rotation flying to the command's vacuum, and a task for it. */
    static void Start(VacuumCommand &command, const Vec3f &position,
                      const Vec3f &rotation, bool reward);

    /* 0x08 */ boost::shared_ptr<Render> m_render;
    /* 0x10 */ RpClump *m_clump;
    /* 0x14 */ bool m_reward; /* whether the owner is sent message 0 on arrival (meaning unknown) */
};
SIZE_ASSERT(Ring, 0x18);

void Ring::Start(VacuumCommand &command, const Vec3f &position,
                 const Vec3f &rotation, bool reward) {
    boost::shared_ptr<VacuumObject> object =
        boost::shared_ptr<VacuumObject>(new Ring(command, reward));
    object->fn_802382E0(position);
    object->fn_80238268(rotation);
    new VacuumObjectTask(object);
}

Ring::Ring(VacuumCommand &command, bool reward) : VacuumObject(0, command) {
    m_clump = fn_8001277C(fn_800122D4(), 0, 0x7C);
    m_render = fn_8004DFB0(fn_80046178()->field_0x8, "VacuumRing");
    fn_80062CD0(&m_render->m_drawCallbacks, boost::bind(&Ring::Draw, this, _1));
    m_reward = reward;
}

Ring::~Ring() {}

void Ring::fn_80238180() {
    boost::shared_ptr<CharInfo> owner = fn_80238058(this).lock();
    if (owner) {
        fn_8015F6EC(owner, 0, 1, 0);
        fn_801D5300(fn_80053164());
        if (m_reward) {
            fn_8020642C(owner, 0);
        }
    }
}

void Ring::Draw(RednerDrawArgs &args) {
    fn_80048390(fn_800289B0(), 5);
    UnkRenderer_800289B0 *renderer = fn_800289B0();
    fn_800482E0(renderer, renderer->field_0x456);
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)fn_802382B0(this));
    Vec3f rotation = fn_80238248(this);
    RwSphere sphere;
    sphere.radius = 5.0f;
    sphere.center.x = position.x;
    sphere.center.y = position.y;
    sphere.center.z = position.z;
    if (fn_804856C8(lbl_805F15B4->curCamera, &sphere) != 0) {
        RwFrame *frame = (RwFrame *)m_clump->object.parent;
        fn_800E9CEC(frame, rotation.y, 0);
        fn_804875B0(frame, &sphere.center, 2);
        fn_80456BC4(m_clump);
    }
    fn_80047780(fn_800289B0());
}

} // namespace

/* Hands a collected ring at position and rotation to the command's vacuum (name unknown; called by
 * Game/RegularRing.cpp). */
extern "C" void fn_80238A78(VacuumCommand &command, const Vec3f &position,
                            const Vec3f &rotation, bool reward) {
    Ring::Start(command, position, rotation, reward);
}
