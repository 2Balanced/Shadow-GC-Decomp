/* GadgetCrash3D (namespace name from RTTI): the debris gadgets throw when they break. Two kinds
 * of debris task:
 * - TObjEffCrash3DR, for a model with a skeleton: one TObjEffCrash3DRChildObj per bone frame
 *   (the atomic hanging from it) flies off from the model's place, spins, bounces on the ground
 *   and blinks out (fn_8015D93C launches one from a UnkDebrisParam_8015F0B8);
 * - TObjEffCrash3D, for loose models: TObjEffCrash3DChildObj pieces, each a clone of a model,
 *   thrown out from a point (fn_8015D97C launches one from a UnkCrash3DParam_8015F4AC).
 * The parent task finishes once all its pieces have. The unit also holds the random generator and
 * the parameter tables the debris spawns start from, set up and released by the common gadget
 * data's hooks (fn_8015F018, fn_8015EF54). File name inferred from the namespace. Names other
 * than the class names are inferred. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/GadgetParticle.h"
#include "Game/Task.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

/* MSL's std::fabs for floats, which the game calls out of line (as in DevilDoomCtrlUnit.cpp). */
namespace std {
float fabs(float x);
} // namespace std

/* The RenderWare types used here, declared locally (layouts as in rw/rwcore.h, rw/rpworld.h and
 * rpanim/rphanim.h, reduced to the members used). */
extern "C" {
typedef struct RwV3d {
    float x;
    float y;
    float z;
} RwV3d;

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

typedef struct RwObject {
    u8 type;
    u8 subKind;
    u8 flags;
    u8 privateFlags;
    void *parent;
} RwObject;

typedef struct RwFrame {
    /* 0x00 */ RwObject object;
    /* 0x08 */ u8 inDirtyListLink[0x8];
    /* 0x10 */ RwMatrix modelling;
} RwFrame;

typedef struct RpClump {
    /* 0x00 */ RwObject object; /* the parent is the clump's frame */
} RpClump;

typedef struct RpAtomic {
    /* 0x00 */ RwObject object; /* flags bit 2: rendered; the parent is the atomic's frame */
} RpAtomic;

typedef struct RpHAnimNodeInfo {
    /* 0x0 */ s32 field_0x0;
    /* 0x4 */ s32 nodeID;
    /* 0x8 */ s32 flags;
    /* 0xC */ RwFrame *pFrame;
} RpHAnimNodeInfo;

typedef struct RpHAnimHierarchy {
    /* 0x00 */ s32 flags;
    /* 0x04 */ s32 numNodes;
    /* 0x08 */ u8 field_0x8[0x10 - 0x8];
    /* 0x10 */ RpHAnimNodeInfo *pNodeInfo;
} RpHAnimHierarchy;

typedef RwFrame *(*RwFrameCallBack)(RwFrame *frame, void *data);
typedef RpAtomic *(*RpAtomicCallBack)(RpAtomic *atomic, void *data);

/* Frames: translate, scale, and rotate about X, Y and Z (as in ElecCoreProgram.cpp), and the
 * children of a frame. */
RwFrame *fn_804875B0(RwFrame *frame, const RwV3d *translation, int combine);
RwFrame *fn_8048764C(RwFrame *frame, const RwV3d *scale, int combine);
void fn_800E9C64(RwFrame *frame, float angle, int combine);
void fn_800E9CEC(RwFrame *frame, float angle, int combine);
void fn_80109AF8(RwFrame *frame, float angle, int combine);
RwFrame *fn_8048752C(RwFrame *frame, RwFrameCallBack callBack, void *data);
/* The skeleton of a frame, and attaching a skeleton to its frames (behaviour inferred). */
RpHAnimHierarchy *fn_8044757C(RwFrame *frame);
void fn_804474D8(RpHAnimHierarchy *hierarchy);
/* Clumps: clone from loaded data, the world a clump is in, add to and remove from a world,
 * destroy, the atomics of a clump, and a setting of unknown meaning (behaviour inferred; see
 * HeavyDog0410CtrlUnit.cpp). */
RpClump *fn_80457090(void *data);
void *fn_8046256C(RpClump *clump);
void fn_804623EC(void *world, RpClump *clump);
void fn_804622FC(void *world, RpClump *clump);
void fn_80457468(RpClump *clump);
RpClump *fn_80456C54(RpClump *clump, RpAtomicCallBack callBack, void *data);
void fn_800389F8(RpClump *clump, s16 arg1);
/* The area a clump is in, and its loading (see HeavyDog0410CtrlUnit.cpp). */
void *fn_80169364(RpClump *clump);
int fn_80169350(void *frame);
void fn_8003A840(int area);

/* The task tree's root lookup and the task layer under it (see Gadget.h), and the number of
 * tasks still running under a task (behaviour inferred). */
void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);
void fn_8004EAEC(Task *task, int *count);

/* The game's random generator (behaviour inferred): its constructor and a float in [0, 1). */
void fn_8040CB2C(void *random, int seed);
float fn_8040CB58(void *random);

/* Vector helpers (behaviour inferred): vector += value, vector *= scale, vector = 0. */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
void fn_8040CE38(Vec3f *vector, float scale);
void fn_8040CE60(Vec3f *vector);
/* Matrix helpers (behaviour inferred): the matrix of a RenderWare matrix, matrix *= other, and
 * matrix * point. */
void fn_800635A4(Mtx34f *matrix, const RwMatrix *source);
void fn_8040BAC4(Mtx34f *matrix, const Mtx34f *other);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);

/* The line test (see ScatteringWeapon.cpp). */
struct UnkLine_80020A20 {
    u8 field_0x0[0x18];
};
struct UnkLineHits_80021FE4 {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ s32 m_count;
};
struct UnkLineHit_8002735C {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ void *m_object;
};
void *fn_80020A20(UnkLine_80020A20 *line, const Vec3f *start, const Vec3f *end);
void *fn_80020D40(void);
UnkLineHits_80021FE4 *fn_80021FE4(void *world, const UnkLine_80020A20 *line, int arg2, int arg3,
                                  void *filter, int arg5);
UnkLineHit_8002735C *fn_8002735C(UnkLineHits_80021FE4 *hits);
const Vec3f *fn_8001E9BC(void *object);
void fn_80026FB4(UnkLineHits_80021FE4 *hits, int arg1);

/* Particle controllers (as in BreakWall.cpp). */
boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> fn_8016690C(void *part);
void fn_80167448(GadgetParticle::GadgetParticleCtrl *ctrl, int id);
}

/* The worlds of the stage areas (see HeavyDog0410CtrlUnit.cpp). */
struct UnkWorldEntry_8051D208 {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ void *m_world;
    /* 0x14 */ u8 field_0x14[0x18 - 0x14];
};
extern UnkWorldEntry_8051D208 lbl_8051D208[];

extern float lbl_805FAD38; /* pi */

/* fn_8040CD74 called as returning a - b by value (it stores the result through its first argument;
 * as in ChaoHolder.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);
extern "C" void fn_8040B790(Mtx34f *matrix, const Vec3f *scale); /* scale */

/* RenderWare's RwMatrixSetIdentity macro. */
#define RW_MATRIX_SET_IDENTITY(m)                                                                  \
    {                                                                                              \
        (m)->right.x = (m)->up.y = (m)->at.z = 1.0f;                                               \
        (m)->right.y = (m)->right.z = (m)->up.x = 0.0f;                                            \
        (m)->up.z = (m)->at.x = (m)->at.y = 0.0f;                                                  \
        (m)->pos.x = (m)->pos.y = (m)->pos.z = 0.0f;                                               \
        (m)->flags |= 0x20003;                                                                     \
    }

/* The game's random generator (class name unknown; 0x10 bytes). */
struct UnkRandom_8040CB2C {
    UnkRandom_8040CB2C(int seed) { fn_8040CB2C(this, seed); }

    u8 field_0x0[0x10];
};

/* The parameters of a skeleton model's debris (layout from its constructors; names inferred). */
struct UnkDebrisParam_8015F0B8 {
    UnkDebrisParam_8015F0B8();
    UnkDebrisParam_8015F0B8(const boost::shared_ptr<CharInfo> &info);
    UnkDebrisParam_8015F0B8(const boost::shared_ptr<CharInfo> &info, int index);
    UnkDebrisParam_8015F0B8 &operator=(const UnkDebrisParam_8015F0B8 &other);

    /* 0x00 */ Task *parent;
    /* 0x04 */ Vec3f position;
    /* 0x10 */ Vec3f rotation;
    /* 0x1C */ Vec3f velocity;
    /* 0x28 */ Vec3f scale;
    /* 0x34 */ float speed;
    /* 0x38 */ s32 field_0x38;
    /* 0x3C */ float field_0x3C;
    /* 0x40 */ float ground;
    /* 0x44 */ void *model;
    /* 0x48 */ s16 field_0x48;
    /* 0x4A */ u8 bounce;
    /* 0x4C */ s32 life;
    /* 0x50 */ s32 field_0x50;
    /* 0x54 */ u8 particle;
    /* 0x58 */ s32 order;
};
SIZE_ASSERT(UnkDebrisParam_8015F0B8, 0x5C);

/* The parameters of loose debris (layout from its constructors; names inferred). */
struct UnkCrash3DParam_8015F4AC {
    UnkCrash3DParam_8015F4AC();
    UnkCrash3DParam_8015F4AC(int index);
    UnkCrash3DParam_8015F4AC &operator=(const UnkCrash3DParam_8015F4AC &other);
    /* Drops ground onto the ground below position. */
    void FindGround();

    /* An entry of the piece table: the number of pieces of a model. */
    struct Entry {
        u32 field_0x0;
        s32 count;
    };

    /* 0x00 */ Task *parent;
    /* 0x04 */ Vec3f position;
    /* 0x10 */ Vec3f rotation;
    /* 0x1C */ float speed;
    /* 0x20 */ s32 type;
    /* 0x24 */ float height;
    /* 0x28 */ float ground;
    /* 0x2C */ void **models;
    /* 0x30 */ Entry *entries;
    /* 0x34 */ s16 modelCount;
    /* 0x36 */ s16 field_0x36;
    /* 0x38 */ s32 order;
};
SIZE_ASSERT(UnkCrash3DParam_8015F4AC, 0x3C);

namespace GadgetCrash3D {

/* An object the unit releases at shutdown (class unknown). */
class UnkObject {
  public:
    virtual ~UnkObject();
};

class TObjEffCrash3D : public Task {
  public:
    TObjEffCrash3D(UnkCrash3DParam_8015F4AC &param);
    virtual ~TObjEffCrash3D();
    virtual void Tick(float dt);

    static const char *s_taskName;

    /* 0x28 */ Vec3f m_position;
    /* 0x34 */ Vec3f m_rotation;
};
SIZE_ASSERT(TObjEffCrash3D, 0x40);

class TObjEffCrash3DChildObj : public Task {
  public:
    TObjEffCrash3DChildObj(Task *parent, const Vec3f *position,
                           const Vec3f *rotation, void *model, float speed, int type,
                           float height, float ground, int arg9, int order);
    virtual ~TObjEffCrash3DChildObj();
    virtual void Tick(float dt);

    void UpdateFrame();

    static const char *s_taskName;

    /* 0x28 */ Vec3f m_position;
    /* 0x34 */ Vec3f m_velocity;
    /* 0x40 */ Vec3f m_rotation;
    /* 0x4C */ Vec3f m_spin;
    /* 0x58 */ float m_health;
    /* 0x5C */ float m_ground;
    /* 0x60 */ RpClump *m_clump;
    /* 0x64 */ s32 m_state; /* 1 flying, 2 settled, 3 done */
    /* 0x68 */ s16 m_life;
    /* 0x6C */ s32 m_order;
};
SIZE_ASSERT(TObjEffCrash3DChildObj, 0x70);

class TObjEffCrash3DR : public Task {
  public:
    TObjEffCrash3DR(UnkDebrisParam_8015F0B8 &param);
    virtual ~TObjEffCrash3DR();
    virtual void Tick(float dt);

    /* The frame of the skeleton node with the given id. */
    RwFrame *GetNodeFrame(int id) {
        int index = -1;
        if (m_hierarchy != 0) {
            for (int i = 0; i < m_hierarchy->numNodes; i++) {
                if (id == m_hierarchy->pNodeInfo[i].nodeID) {
                    index = i;
                    break;
                }
            }
        }
        if (index == -1) {
            return 0;
        }
        return m_hierarchy->pNodeInfo[index].pFrame;
    }

    /* The atomic of the clump hanging from frame. */
    RpAtomic *GetFrameAtomic(RwFrame *frame);

    static const char *s_taskName;

    /* 0x28 */ Vec3f m_position;
    /* 0x34 */ Vec3f m_rotation;
    /* 0x40 */ RpHAnimHierarchy *m_hierarchy;
    /* 0x44 */ RpClump *m_clump;
    /* 0x48 */ s32 field_0x48;
    /* 0x4C */ boost::shared_ptr<GadgetParticle::GadgetParticleCtrl> m_particle;
    /* 0x54 */ u32 field_0x54;
};
SIZE_ASSERT(TObjEffCrash3DR, 0x58);

class TObjEffCrash3DRChildObj : public Task {
  public:
    TObjEffCrash3DRChildObj(Task *parent, const Vec3f *position,
                            const Vec3f *rotation, Vec3f *velocity,
                            const Vec3f *scale, RwFrame *frame, RpAtomic *atomic,
                            u8 bounce, s16 life, int order, float speed, float ground);
    virtual ~TObjEffCrash3DRChildObj();
    virtual void Tick(float dt);

    void UpdateFrame();
    /* Unused (see its definition; name inferred). */
    void ResetHealth();

    static const char *s_taskName;

    /* 0x28 */ Vec3f m_position;
    /* 0x34 */ Vec3f m_velocity;
    /* 0x40 */ Vec3f m_rotation;
    /* 0x4C */ Vec3f m_scale;
    /* 0x58 */ Vec3f m_spin;
    /* 0x64 */ float m_health;
    /* 0x68 */ float m_ground;
    /* 0x6C */ RwFrame *m_frame;
    /* 0x70 */ RpAtomic *m_atomic;
    /* 0x74 */ u8 field_0x74;
    /* 0x75 */ u8 m_bounce;
    /* 0x78 */ s32 m_state; /* 1 flying, 2 settled, 3 done */
    /* 0x7C */ s16 m_life;
    /* 0x7E */ s16 m_blinkTime;
    /* 0x80 */ s32 m_order;
    /* 0x84 */ u32 field_0x84;
};
SIZE_ASSERT(TObjEffCrash3DRChildObj, 0x88);

const char *TObjEffCrash3D::s_taskName = "TObjEffCrash3D";
const char *TObjEffCrash3DChildObj::s_taskName = "TObjEffCrash3DChildObj";
const char *TObjEffCrash3DR::s_taskName = "TObjEffCrash3DR";
const char *TObjEffCrash3DRChildObj::s_taskName = "TObjEffCrash3DRChildObj";

/* The random generator, the life of a piece (a base plus a random part), the time a settled piece
 * blinks, two objects released at shutdown, the parameter tables, and the atomic FindAtomic
 * found. */
UnkRandom_8040CB2C *s_random;
s32 s_lifeBase;
s32 s_blinkTime;
float s_lifeRandom;
UnkObject *s_object0;
UnkObject *s_object1;
UnkCrash3DParam_8015F4AC *s_params;
UnkDebrisParam_8015F0B8 *s_debrisParams;
RpAtomic *s_foundAtomic;

/* A random float in [0, 1). */
inline float Random() {
    return fn_8040CB58(s_random);
}

inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

inline void CopyToRwV3d(RwV3d &result, const Vec3f &vector) {
    result.x = vector.x;
    result.y = vector.y;
    result.z = vector.z;
}

/* The world of the stage area clump is in, loading the area (see HeavyDog0410CtrlUnit.cpp). */
inline void *GetAreaWorld(RpClump *clump) {
    int area = fn_80169350(fn_80169364(clump));
    fn_8003A840(area);
    return lbl_8051D208[area].m_world;
}

/* Moves clump into world. */
inline void SetClumpWorld(RpClump *clump, void *world) {
    void *current = fn_8046256C(clump);
    if (current != world) {
        if (current != 0) {
            fn_804623EC(current, clump);
        }
        fn_804622FC(world, clump);
    }
}

} // namespace GadgetCrash3D

using namespace GadgetCrash3D;

TObjEffCrash3D::~TObjEffCrash3D() {}

UnkCrash3DParam_8015F4AC::UnkCrash3DParam_8015F4AC() {
    parent = 0;
    fn_8040CE60(&position);
    fn_8040CE60(&rotation);
    speed = 0.0f;
    type = 0;
    height = 0.0f;
    ground = 0.0f;
    models = 0;
    entries = 0;
    modelCount = 0;
    field_0x36 = 0;
    order = 0;
}

UnkCrash3DParam_8015F4AC::UnkCrash3DParam_8015F4AC(int index) {
    if (index < 32) {
        *this = s_params[index];
    }
    parent = fn_801EB304(fn_8001C4F8(), 11);
    fn_8040CE60(&position);
    fn_8040CE60(&rotation);
}

UnkCrash3DParam_8015F4AC &UnkCrash3DParam_8015F4AC::operator=(const UnkCrash3DParam_8015F4AC &other) {
    parent = other.parent;
    fn_800091D0(&position, &other.position);
    fn_800091D0(&rotation, &other.rotation);
    speed = other.speed;
    type = other.type;
    height = other.height;
    ground = other.ground;
    models = other.models;
    entries = other.entries;
    modelCount = other.modelCount;
    field_0x36 = other.field_0x36;
    order = other.order;
    return *this;
}

void UnkCrash3DParam_8015F4AC::FindGround() {
    UnkLine_80020A20 line;
    Vec3f start;
    Vec3f end;
    fn_800091D0(&start, fn_800091D0(&end, &position));
    end.y -= 1000.0f;
    fn_80020A20(&line, &start, &end);
    UnkLineHits_80021FE4 *hits = fn_80021FE4(fn_80020D40(), &line, 1, 0, 0, 0);
    if (hits != 0 && hits->m_count > 0) {
        Vec3f point;
        fn_800091EC(&point, fn_8001E9BC(fn_8002735C(hits)->m_object));
        ground = point.y;
        fn_80026FB4(hits, 1);
    }
}

UnkDebrisParam_8015F0B8::UnkDebrisParam_8015F0B8() {
    parent = 0;
    fn_8040CE60(&position);
    fn_8040CE60(&rotation);
    fn_8040CE60(&velocity);
    scale.x = scale.y = scale.z = 1.0f;
    speed = 0.0f;
    field_0x38 = 0;
    field_0x3C = 0.0f;
    ground = 0.0f;
    model = 0;
    field_0x48 = 0;
    bounce = 0;
    life = 300;
    field_0x50 = 0;
    particle = 0;
    order = 0;
}

UnkDebrisParam_8015F0B8::UnkDebrisParam_8015F0B8(const boost::shared_ptr<CharInfo> &info) {
    parent = fn_801EB304(fn_8001C4F8(), 11);
    fn_800091D0(&position, &info->m_position);
    rotation.x = info->m_rotation.x;
    rotation.y = info->m_rotation.y;
    rotation.z = info->m_rotation.z;
    fn_8040CE60(&velocity);
    scale.x = scale.y = scale.z = 1.0f;
    speed = 0.0f;
    field_0x38 = 0;
    field_0x3C = 0.0f;
    ground = info->m_position.y;
    model = 0;
    field_0x48 = 0;
    bounce = 0;
    life = 300;
    field_0x50 = 0;
    particle = 0;
    order = info->field_0x30;
}

UnkDebrisParam_8015F0B8 &UnkDebrisParam_8015F0B8::operator=(const UnkDebrisParam_8015F0B8 &other) {
    parent = other.parent;
    fn_800091D0(&position, &other.position);
    fn_800091D0(&rotation, &other.rotation);
    fn_800091D0(&velocity, &other.velocity);
    fn_800091D0(&scale, &other.scale);
    speed = other.speed;
    field_0x38 = other.field_0x38;
    field_0x3C = other.field_0x3C;
    ground = other.ground;
    model = other.model;
    field_0x48 = other.field_0x48;
    bounce = other.bounce;
    life = other.life;
    field_0x50 = other.field_0x50;
    particle = other.particle;
    order = other.order;
    return *this;
}

UnkDebrisParam_8015F0B8::UnkDebrisParam_8015F0B8(const boost::shared_ptr<CharInfo> &info,
                                                 int index) {
    if (index < 32) {
        *this = s_debrisParams[index];
    }
    parent = fn_801EB304(fn_8001C4F8(), 11);
    fn_800091D0(&position, &info->m_position);
    rotation.x = info->m_rotation.x;
    rotation.y = info->m_rotation.y;
    rotation.z = info->m_rotation.z;
    order = info->field_0x30;
}

/* Sets up the random generator and the parameter tables (the common gadget data's load hook). */
extern "C" void fn_8015F018(void) {
    s_random = new UnkRandom_8040CB2C(0);
    s_lifeBase = 160;
    s_blinkTime = 45;
    s_lifeRandom = 55.0f;
    s_params = new UnkCrash3DParam_8015F4AC[32];
    s_debrisParams = new UnkDebrisParam_8015F0B8[32];
}

/* Releases what fn_8015F018 set up (the common gadget data's unload hook). */
extern "C" void fn_8015EF54(void) {
    if (s_object1 != 0) {
        delete s_object1;
        s_object1 = 0;
    }
    delete[] s_debrisParams;
    s_debrisParams = 0;
    if (s_object0 != 0) {
        delete s_object0;
        s_object0 = 0;
    }
    delete[] s_params;
    s_params = 0;
    if (s_random != 0) {
        delete s_random;
        s_random = 0;
    }
}

TObjEffCrash3D::TObjEffCrash3D(UnkCrash3DParam_8015F4AC &param) : Task(param.parent) {
    m_name = s_taskName;
    fn_800091D0(&m_position, &param.position);
    fn_800091D0(&m_rotation, &param.rotation);
    for (int i = 0; i < param.modelCount; i++) {
        if (param.models[i] != 0) {
            for (int j = 0; j < param.entries[i].count; j++) {
                new TObjEffCrash3DChildObj(this, &m_position, &m_rotation, param.models[i],
                                           param.speed, param.type, param.height, param.ground,
                                           param.field_0x36, param.order);
            }
        }
    }
}

void TObjEffCrash3D::Tick(float dt) {
    int count = 0;
    fn_8004EAEC(this, &count);
    if (count == 0) {
        m_flags |= 1;
    }
}

TObjEffCrash3DChildObj::TObjEffCrash3DChildObj(Task *parent, const Vec3f *position,
                                               const Vec3f *rotation, void *model,
                                               float speed, int type, float height, float ground,
                                               int arg9, int order)
    : Task(parent) {
    m_name = s_taskName;
    fn_800091D0(&m_position, position);
    fn_800091D0(&m_rotation, rotation);
    m_order = order;
    float random = Random();
    m_life = s_lifeBase + (int)(random * s_lifeRandom);
    m_health = 1.0f;
    m_ground = ground;
    m_spin.x = DegToRad(0.1f * (Random() - 0.5f));
    m_spin.y = DegToRad(0.06f * (Random() - 0.5f));
    m_spin.z = DegToRad(0.05f * (Random() - 0.5f));
    switch (type) {
    case 1: {
        m_rotation.x = DegToRad(90.0f * (36.0f * (2.0f * (Random() - 0.5f))));
        m_rotation.y = rotation->y + DegToRad(90.0f * (36.0f * (2.0f * (Random() - 0.5f))));
        m_rotation.z = 0.0f;
        Mtx34f matrix;
        fn_8040B6F8(&matrix);
        fn_8040B59C(&matrix, &m_position);
        fn_8040B848(&matrix, rotation->y);
        float angle = DegToRad(30.0f * (2.0f * (Random() - 0.5f)));
        fn_8040B848(&matrix, angle + 16384.0f * (180.0f * (Random() - 0.5f)));
        angle = DegToRad(30.0f * (2.0f * (Random() - 0.5f)));
        fn_8040B884(&matrix, angle + 16384.0f * (180.0f * (Random() - 0.5f)));
        Vec3f offset(0.0f, height, 0.0f);
        fn_800091D0(&m_position, &(const Vec3f &)fn_8040B67C(&matrix, &offset));
        break;
    }
    case 0: {
        m_rotation.x = DegToRad(360.0f * (2.0f * (Random() - 0.5f)));
        m_rotation.y = DegToRad(360.0f * (2.0f * (Random() - 0.5f)));
        m_rotation.z = 0.0f;
        Mtx34f matrix;
        Mtx34f translation;
        fn_8040B034(&translation, &m_position);
        fn_8040C148(&matrix, &translation);
        fn_8040B848(&matrix, rotation->y);
        fn_8040B848(&matrix, DegToRad(360.0f * (2.0f * (Random() - 0.5f))));
        fn_8040B884(&matrix, DegToRad(360.0f * (2.0f * (Random() - 0.5f))));
        Vec3f offset(0.0f, height, 0.0f);
        fn_800091D0(&m_position, &(const Vec3f &)fn_8040B67C(&matrix, &offset));
        break;
    }
    }
    Vec3f direction;
    fn_800091EC(&direction,
                &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&m_position, position));
    float length = fn_8040CF50(&direction);
    fn_8040CE38(&direction, length > 0.0f ? speed / length : 0.0f);
    fn_800091D0(&m_velocity, &direction);
    m_state = 1;
    m_clump = fn_80457090(model);
    if (m_clump == 0) {
        m_flags |= 1;
        return;
    }
    void *world = GetAreaWorld(m_clump);
    SetClumpWorld(m_clump, world);
    fn_800389F8(m_clump, arg9);
    UpdateFrame();
}

TObjEffCrash3DChildObj::~TObjEffCrash3DChildObj() {
    if (m_clump != 0) {
        void *world = fn_8046256C(m_clump);
        if (world != 0) {
            fn_804623EC(world, m_clump);
        }
        fn_80457468(m_clump);
        m_clump = 0;
    }
}

void TObjEffCrash3DChildObj::UpdateFrame() {
    if (m_clump != 0) {
        RwV3d v;
        CopyToRwV3d(v, m_position);
        RwFrame *frame = (RwFrame *)m_clump->object.parent;
        fn_804875B0(frame, &v, 0);
        switch (m_order) {
        case 0:
            fn_800E9C64(frame, m_rotation.x, 1);
            fn_800E9CEC(frame, m_rotation.y, 1);
            fn_80109AF8(frame, m_rotation.z, 1);
            break;
        case 1:
            fn_800E9CEC(frame, m_rotation.y, 1);
            fn_800E9C64(frame, m_rotation.x, 1);
            fn_80109AF8(frame, m_rotation.z, 1);
            break;
        case 2:
            fn_80109AF8(frame, m_rotation.z, 1);
            fn_800E9C64(frame, m_rotation.x, 1);
            fn_800E9CEC(frame, m_rotation.y, 1);
            break;
        }
    }
}

void TObjEffCrash3DChildObj::Tick(float dt) {
    fn_8040CFF8(&m_position, &m_velocity);
    if (m_ground >= m_position.y) {
        m_velocity.x *= 0.989f;
        m_velocity.z *= 0.989f;
        m_position.y = m_ground;
        if (std::fabs(m_velocity.y) < 0.445f) {
            m_velocity.y = 0.0f;
            m_velocity.x *= 0.6f;
            m_velocity.z *= 0.6f;
            fn_8040CE60(&m_spin);
        } else {
            m_velocity.y *= -0.6f;
            fn_8040CE38(&m_spin, 0.6f);
        }
    } else {
        m_velocity.x *= 0.989f;
        m_velocity.y = 0.989f * m_velocity.y - 0.089f;
        m_velocity.z *= 0.989f;
    }
    fn_8040CFF8(&m_rotation, &m_spin);
    fn_8040CE38(&m_spin, 0.989f);
    switch (m_state) {
    case 1:
    case 2:
        m_life--;
        if (m_state != 2 && std::fabs(fn_8040CF50(&m_velocity)) < 0.01f) {
            m_state = 2;
            if (m_life >= s_blinkTime) {
                m_life = s_blinkTime;
            }
        }
        if (m_health <= 0.0f || m_life <= 0) {
            m_state = 3;
            m_life = 0;
        }
        UpdateFrame();
        break;
    case 3:
        m_flags |= 1;
        break;
    }
}

static RpAtomic *FindAtomic(RpAtomic *atomic, void *data);
static RwFrame *FindHierarchy(RwFrame *frame, void *data);

inline RpAtomic *TObjEffCrash3DR::GetFrameAtomic(RwFrame *frame) {
    s_foundAtomic = 0;
    fn_80456C54(m_clump, FindAtomic, frame);
    return s_foundAtomic;
}

/* The skeleton of a clump: the one on its frame, or the first found among the frame's children. */
inline RpHAnimHierarchy *GetHierarchy(RpClump *clump) {
    RpHAnimHierarchy *hierarchy = 0;
    hierarchy = fn_8044757C((RwFrame *)clump->object.parent);
    if (hierarchy == 0) {
        fn_8048752C((RwFrame *)clump->object.parent, FindHierarchy, &hierarchy);
    }
    return hierarchy;
}

TObjEffCrash3DR::TObjEffCrash3DR(UnkDebrisParam_8015F0B8 &param) : Task(param.parent) {
    m_name = s_taskName;
    fn_800091D0(&m_position, &param.position);
    fn_800091D0(&m_rotation, &param.rotation);
    field_0x48 = param.field_0x50;
    m_clump = fn_80457090(param.model);
    if (m_clump == 0) {
        m_flags |= 1;
        return;
    }
    RwFrame *frame = (RwFrame *)m_clump->object.parent;
    RW_MATRIX_SET_IDENTITY(&frame->modelling);
    void *world = GetAreaWorld(m_clump);
    SetClumpWorld(m_clump, world);
    fn_800389F8(m_clump, param.field_0x48);
    m_hierarchy = GetHierarchy(m_clump);
    if (m_hierarchy != 0) {
        fn_804474D8(m_hierarchy);
    }
    if (m_hierarchy != 0) {
        for (int i = 1; i < m_hierarchy->numNodes; i++) {
            RwFrame *node = GetNodeFrame(i);
            if (node == 0) {
                break;
            }
            RpAtomic *atomic = GetFrameAtomic(node);
            if (atomic != 0) {
                new TObjEffCrash3DRChildObj(this, &m_position, &m_rotation, &param.velocity,
                                            &param.scale, node, atomic, param.bounce, param.life,
                                            param.order, param.speed, param.ground);
            }
        }
    }
    if (param.particle) {
        m_particle = fn_8016690C(m_clump);
        fn_80167448(m_particle.get(), -1);
    }
}

TObjEffCrash3DR::~TObjEffCrash3DR() {
    if (m_clump != 0) {
        void *world = fn_8046256C(m_clump);
        if (world != 0) {
            fn_804623EC(world, m_clump);
        }
        fn_80457468(m_clump);
        m_clump = 0;
    }
}

void TObjEffCrash3DR::Tick(float dt) {
    int count = 0;
    fn_8004EAEC(this, &count);
    if (count == 0) {
        m_flags |= 1;
    }
}

/* Stops at the atomic hanging from frame data (an RpClumpForAllAtomics callback). */
static RpAtomic *FindAtomic(RpAtomic *atomic, void *data) {
    if (atomic->object.parent == data) {
        s_foundAtomic = atomic;
        return 0;
    }
    return atomic;
}

TObjEffCrash3DRChildObj::TObjEffCrash3DRChildObj(Task *parent, const Vec3f *position,
                                                 const Vec3f *rotation,
                                                 Vec3f *velocity,
                                                 const Vec3f *scale, RwFrame *frame,
                                                 RpAtomic *atomic, u8 bounce, s16 life, int order,
                                                 float speed, float ground)
    : Task(parent) {
    m_name = s_taskName;
    fn_800091D0(&m_scale, scale);
    m_bounce = bounce;
    m_order = order;
    m_state = 1;
    if (life == 0) {
        float random = Random();
        m_life = s_lifeBase + (int)(random * s_lifeRandom);
    } else {
        m_life = life;
    }
    if (m_life >= s_blinkTime * 4) {
        m_blinkTime = s_blinkTime;
    } else {
        m_blinkTime = 0.25f * m_life;
    }
    m_health = 1.0f;
    m_ground = ground;
    m_spin.x = DegToRad(2.0f * Random() - 1.0f);
    m_spin.y = DegToRad(2.0f * Random() - 1.0f);
    m_spin.z = DegToRad(2.0f * Random() - 1.0f);
    Mtx34f matrix;
    Mtx34f translation;
    fn_8040B034(&translation, position);
    fn_8040C148(&matrix, &translation);
    switch (m_order) {
    case 0:
        fn_8040B80C(&matrix, rotation->z);
        fn_8040B848(&matrix, rotation->y);
        fn_8040B884(&matrix, rotation->x);
        break;
    case 1:
        fn_8040B80C(&matrix, rotation->z);
        fn_8040B884(&matrix, rotation->x);
        fn_8040B848(&matrix, rotation->y);
        break;
    case 2:
        fn_8040B848(&matrix, rotation->y);
        fn_8040B884(&matrix, rotation->x);
        fn_8040B80C(&matrix, rotation->z);
        break;
    }
    fn_8040B790(&matrix, &m_scale);
    Mtx34f frameLtm;
    fn_800635A4(&frameLtm, &frame->modelling);
    fn_8040BAC4(&matrix, &frameLtm);
    Vec3f origin(0.0f, 0.0f, 0.0f);
    fn_800091D0(&m_position, &(const Vec3f &)fn_8040B67C(&matrix, &origin));
    fn_800091D0(&m_rotation, rotation);
    Vec3f direction;
    fn_800091EC(&direction,
                &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&m_position, position));
    float length = fn_8040CF50(&direction);
    if (length <= 0.01f) {
        direction.x = velocity->x;
        direction.y = speed + velocity->y;
        direction.z = velocity->z;
    } else {
        fn_8040CE38(&direction, speed / length);
        fn_8040CFF8(&direction, velocity);
    }
    fn_800091D0(&m_velocity, &direction);
    m_frame = frame;
    m_atomic = atomic;
    RW_MATRIX_SET_IDENTITY(&m_frame->modelling);
    UpdateFrame();
    field_0x74 = 0;
}

/* Not called anywhere: the linker strips it from the game. Retail pools 1.0f ahead of the
 * constructor's 0.25f; GC/2.7 pools the literals in the order it emits the functions, so a function
 * that uses 1.0f here gives retail's order (inferred; the original function is unknown). */
void TObjEffCrash3DRChildObj::ResetHealth() {
    m_health = 1.0f;
}

void TObjEffCrash3DRChildObj::UpdateFrame() {
    if (m_frame != 0) {
        RwV3d v;
        CopyToRwV3d(v, m_position);
        fn_804875B0(m_frame, &v, 0);
        switch (m_order) {
        case 0:
            fn_800E9C64(m_frame, m_rotation.x, 1);
            fn_800E9CEC(m_frame, m_rotation.y, 1);
            fn_80109AF8(m_frame, m_rotation.z, 1);
            break;
        case 1:
            fn_800E9CEC(m_frame, m_rotation.y, 1);
            fn_800E9C64(m_frame, m_rotation.x, 1);
            fn_80109AF8(m_frame, m_rotation.z, 1);
            break;
        case 2:
            fn_80109AF8(m_frame, m_rotation.z, 1);
            fn_800E9C64(m_frame, m_rotation.x, 1);
            fn_800E9CEC(m_frame, m_rotation.y, 1);
            break;
        }
        CopyToRwV3d(v, m_scale);
        fn_8048764C(m_frame, &v, 1);
    }
}

void TObjEffCrash3DRChildObj::Tick(float dt) {
    fn_8040CFF8(&m_position, &m_velocity);
    if (m_bounce && m_ground >= m_position.y) {
        m_velocity.x *= 0.989f;
        m_velocity.z *= 0.989f;
        m_position.y = m_ground;
        if (std::fabs(m_velocity.y) < 0.445f) {
            m_velocity.y = 0.0f;
            m_velocity.x *= 0.6f;
            m_velocity.z *= 0.6f;
            fn_8040CE60(&m_spin);
        } else {
            m_velocity.y *= -0.6f;
            fn_8040CE38(&m_spin, 0.6f);
        }
    } else {
        m_velocity.x *= 0.989f;
        m_velocity.y = 0.989f * m_velocity.y - 0.089f;
        m_velocity.z *= 0.989f;
    }
    fn_8040CFF8(&m_rotation, &m_spin);
    fn_8040CE38(&m_spin, 0.989f);
    switch (m_state) {
    case 1:
    case 2:
        m_life--;
        if (m_bounce) {
            if (m_state != 2 && std::fabs(fn_8040CF50(&m_velocity)) < 0.01f) {
                m_state = 2;
                if (m_life >= m_blinkTime) {
                    m_life = m_blinkTime;
                }
            }
        } else if (m_state != 2 && m_life <= m_blinkTime) {
            m_state = 2;
        }
        if (m_health <= 0.0f || m_life <= 0) {
            m_state = 3;
            m_atomic->object.flags &= ~4;
            m_life = 0;
        }
        if (m_state == 2) {
            if (m_life & 2) {
                m_atomic->object.flags &= ~4;
            } else {
                m_atomic->object.flags |= 4;
            }
        }
        UpdateFrame();
        break;
    case 3:
        m_flags |= 1;
        break;
    }
}

/* Finds the skeleton among the children of frame (an RwFrameForAllChildren callback). */
static RwFrame *FindHierarchy(RwFrame *frame, void *data) {
    RpHAnimHierarchy *hierarchy = fn_8044757C(frame);
    if (hierarchy == 0) {
        fn_8048752C(frame, FindHierarchy, data);
        return frame;
    }
    *(RpHAnimHierarchy **)data = hierarchy;
    return 0;
}

/* Throws the pieces param describes. */
extern "C" void fn_8015D97C(UnkCrash3DParam_8015F4AC *param) {
    new TObjEffCrash3D(*param);
}

/* Throws the skeleton pieces param describes. */
extern "C" void fn_8015D93C(UnkDebrisParam_8015F0B8 *param) {
    new TObjEffCrash3DR(*param);
}

TObjEffCrash3DRChildObj::~TObjEffCrash3DRChildObj() {}
