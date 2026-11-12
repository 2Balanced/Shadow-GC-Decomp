/* The clump updater and the limb solvers (no class with RTTI; file name and all names inferred).
 * UnkClumpUpdater_80015A7C ties a clump's frames to the nodes of a motion: its constructor lists
 * every frame of the clump, with the motion's node for the frames of the skin hierarchy, and its
 * update (fn_800158C4) copies each node's matrix into its frame's LTM (or rebuilds the LTM from the
 * parent), optionally followed by a matrix or an offset, and then grows the atomics' world bounding
 * spheres (fn_80015354; fn_80015344 sets the flag that picks the scale). fn_80015B60, fn_80015DE0
 * and fn_80015EC8 aim a chain of two or three bones at a target (law of cosines). The unit ends
 * with small out-of-line helpers other units call: arc cosine, the angle of (x, z), negation and
 * conversions between Mtx34f and RenderWare's matrix. The next unit (.rodata "std::exception")
 * starts at 0x80016478. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include "Game/MathTypes.h"
#include <dolphin/types.h>
#include "size_assert.h"

struct RwV3d {
    float x;
    float y;
    float z;
};

/* RenderWare's matrix: the axes and the position, each followed by a word. */
struct RwMatrix {
    /* 0x00 */ RwV3d right;
    /* 0x0C */ u32 flags;
    /* 0x10 */ RwV3d up;
    /* 0x1C */ u32 pad1;
    /* 0x20 */ RwV3d at;
    /* 0x2C */ u32 pad2;
    /* 0x30 */ RwV3d pos;
    /* 0x3C */ u32 pad3;
};
SIZE_ASSERT(RwMatrix, 0x40);

/* A frame: its modelling matrix at 0x10 and its LTM at 0x50. */
struct RwFrame {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ RwMatrix modelling;
    /* 0x50 */ RwMatrix ltm;
};

struct RpClump {
    /* 0x00 */ u32 field_0x0;
    /* 0x04 */ RwFrame *frame;
};

struct RwSphere {
    RwV3d center;
    float radius;
};

struct RpAtomic {
    /* 0x00 */ u32 field_0x0;
    /* 0x04 */ RwFrame *frame;
    /* 0x08 */ u8 field_0x8[0x18 - 0x8];
    /* 0x18 */ void *geometry;
    /* 0x1C */ RwSphere boundingSphere;
    /* 0x2C */ RwSphere worldBoundingSphere;
};

/* A skin hierarchy: its flags, its node count and its node table. */
struct RpHAnimNodeInfo {
    /* 0x00 */ int nodeID;
    /* 0x04 */ int nodeIndex;
    /* 0x08 */ int flags;
    /* 0x0C */ RwFrame *frame;
};

struct RpHAnimHierarchy {
    /* 0x00 */ u32 flags;
    /* 0x04 */ int numNodes;
    /* 0x08 */ u8 field_0x8[0x10 - 0x8];
    /* 0x10 */ RpHAnimNodeInfo *nodeInfo;
};

struct _stMtnPlayWork;

/* A frame the updater keeps: the motion node whose matrix it takes, or the parent LTM it is
 * placed under. */
struct UnkClumpUpdaterNode_80015A7C {
    /* 0x00 */ RwFrame *m_frame;
    /* 0x04 */ Mtx34f *m_node;
    /* 0x08 */ RwMatrix *m_parent;
    /* 0x0C */ int m_id;
    /* 0x10 */ UnkClumpUpdaterNode_80015A7C *m_next;
};
SIZE_ASSERT(UnkClumpUpdaterNode_80015A7C, 0x14);

/* Ties a clump's frames to a motion (also declared in Game/DevilDoomCtrlUnit.h and
 * Game/BlackDoomCtrlUnit.cpp with its size only). */
class UnkClumpUpdater_80015A7C {
  public:
    UnkClumpUpdater_80015A7C(RpClump *clump, _stMtnPlayWork *motion);
    ~UnkClumpUpdater_80015A7C();

    /* 0x00 */ RpClump *m_clump;
    /* 0x04 */ _stMtnPlayWork *m_motion;
    /* 0x08 */ UnkClumpUpdaterNode_80015A7C *m_head;
    /* 0x0C */ UnkClumpUpdaterNode_80015A7C *m_cursor;
    /* 0x10 */ RwMatrix *m_parent;
    /* 0x14 */ const Vec3f *m_offset;
    /* 0x18 */ const Mtx34f *m_matrix;
};
SIZE_ASSERT(UnkClumpUpdater_80015A7C, 0x1C);

typedef RpAtomic *(*RpAtomicCallBack)(RpAtomic *atomic, void *data);
typedef RwFrame *(*RwFrameCallBack)(RwFrame *frame, void *data);

extern "C" {
/* RenderWare: a frame's LTM, its root, its children, a clump's atomics, multiplying matrices,
 * transforming a point and a square root (behaviour inferred). */
RwMatrix *RwFrameGetLTM(RwFrame *frame);
RwFrame *fn_804872D8(RwFrame *frame);
RwFrame *fn_8048752C(RwFrame *frame, RwFrameCallBack callBack, void *data);
RpClump *fn_80456C54(RpClump *clump, RpAtomicCallBack callBack, void *data);
RwMatrix *fn_8047EBB8(RwMatrix *dst, const RwMatrix *a, const RwMatrix *b);
RwV3d *fn_80482458(RwV3d *out, const RwV3d *in, const RwMatrix *matrix);
float fn_80482340(float x);
/* Skin hierarchies: a frame's hierarchy, a geometry's skin, setting an atomic's hierarchy and
 * updating its matrices (behaviour inferred). */
RpHAnimHierarchy *fn_8044757C(RwFrame *frame);
void *fn_8044CCAC(void *geometry);
RpAtomic *fn_8044CC98(RpAtomic *atomic, RpHAnimHierarchy *hierarchy);
void fn_804474D8(RpHAnimHierarchy *hierarchy);
/* A motion's node matrix (behaviour inferred). */
Mtx34f *fn_80415E70(_stMtnPlayWork *work, int id);
/* Matrix routines: invert, transform a vector, translate (behaviour inferred). */
void fn_8040B4D0(Mtx34f *out, const Mtx34f *m);
Vec3f fn_8040B618(const Mtx34f *m, const Vec3f *v);
void fn_8040B52C(Mtx34f *m, float x, float y, float z);
/* MSL: arc cosine and arc tangent of y / x (double). */
double fn_803B1BFC(double x);
double fn_803B1C3C(double y, double x);
/* Float square root and arc tangent (inline copies in Game/CameraMotionPlay.cpp and
 * Game/CameraProjection.cpp). */
float fn_8000FEE4(float x);
float fn_8000F3F8(float x);

float fn_80016260(float y, float x);
Vec3f fn_80016284(const Vec3f *v);
float fn_800162AC(float a, float b, float c, int negate);
float fn_80016104(Mtx34f *out, const Vec3f *target, const Mtx34f *base, bool flip);
float fn_80015EC8(Mtx34f *a, Mtx34f *b, Mtx34f *c, const Vec3f *target, const Mtx34f *base,
                  float length0, float length1, float length2, bool flip);
float fn_80015D68(float x);
void fn_80016390(RwMatrix *out, const Mtx34f *m);
void fn_80015740(UnkClumpUpdater_80015A7C *updater);
void fn_800158C4(UnkClumpUpdater_80015A7C *updater);
RpAtomic *fn_80015354(RpAtomic *atomic, void *data);
RwFrame *fn_80015470(RwFrame *frame, UnkClumpUpdater_80015A7C *updater);
RwFrame *fn_80015570(RwFrame *frame, void *data);
RpAtomic *fn_80015698(RpAtomic *atomic, void *data);
RwFrame *fn_800156E4(RwFrame *frame, void *data);
}

extern const float lbl_805FAD38; /* pi */

/* std::sin(float), kept out of line as in Game/CameraMotionPlay.cpp (<cmath> would add sqrtf's
 * constants to .sdata2). */
extern "C" double sin(double x);

namespace std {
inline float sinf_(float x) {
    return (float)::sin(x);
}

inline float sin(float x) {
    return sinf_(x);
}
} // namespace std

/* Whether the world bounding spheres get the large scale. */
static bool s_largeBounds;

inline float Square(float x) {
    return x * x;
}

#define MAX(a, b) ((a) >= (b) ? (a) : (b))

inline float LengthSquared(const RwV3d &v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

/* Copies value into vector through the game's out-of-line assignment. */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* The skin hierarchy of a clump's frame tree (see fn_800156E4). */
inline RpHAnimHierarchy *FindHierarchy(RpClump *clump) {
    RpHAnimHierarchy *hierarchy = 0;
    fn_8048752C(clump->frame, fn_800156E4, &hierarchy);
    return hierarchy;
}

typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* Mtx34f's columns as RenderWare's axes. */
inline RwMatrix ToRwMatrix(const Mtx34f &m) {
    RwMatrix result;
    result.right.x = m.values[0][0];
    result.right.y = m.values[1][0];
    result.right.z = m.values[2][0];
    result.up.x = m.values[0][1];
    result.up.y = m.values[1][1];
    result.up.z = m.values[2][1];
    result.at.x = m.values[0][2];
    result.at.y = m.values[1][2];
    result.at.z = m.values[2][2];
    result.pos.x = m.values[0][3];
    result.pos.y = m.values[1][3];
    result.pos.z = m.values[2][3];
    result.flags = result.pad1 = result.pad2 = result.pad3 = 0;
    return result;
}

extern "C" {

void fn_80016414(Mtx34f *out, const RwMatrix *m) {
    out->values[0][0] = m->right.x;
    out->values[1][0] = m->right.y;
    out->values[2][0] = m->right.z;
    out->values[0][1] = m->up.x;
    out->values[1][1] = m->up.y;
    out->values[2][1] = m->up.z;
    out->values[0][2] = m->at.x;
    out->values[1][2] = m->at.y;
    out->values[2][2] = m->at.z;
    out->values[0][3] = m->pos.x;
    out->values[1][3] = m->pos.y;
    out->values[2][3] = m->pos.z;
}

void fn_800163F4(Mtx34f *out, const Mtx34f *m) {
    fn_8040C148(out, m);
}

void fn_80016390(RwMatrix *out, const Mtx34f *m) {
    out->right.x = m->values[0][0];
    out->right.y = m->values[1][0];
    out->right.z = m->values[2][0];
    out->up.x = m->values[0][1];
    out->up.y = m->values[1][1];
    out->up.z = m->values[2][1];
    out->at.x = m->values[0][2];
    out->at.y = m->values[1][2];
    out->at.z = m->values[2][2];
    out->pos.x = m->values[0][3];
    out->pos.y = m->values[1][3];
    out->pos.z = m->values[2][3];
}

void fn_80016370(Mtx34f *out, const Mtx34f *m) {
    fn_8040C148(out, m);
}

void fn_80016350(Vec3f *out, const Vec3f *v) {
    fn_800091D0(out, v);
}

/* The angle at the joint between sides a and b of a triangle with third side c, as an angle for
 * the rotation about X (negated if negate). */
float fn_800162AC(float a, float b, float c, int negate) {
    float cosine = (Square(a) + Square(b) - Square(c)) / (2.0f * (a * b));
    if (cosine > 1.0f) {
        cosine = 1.0f;
    } else if (cosine < -1.0f) {
        cosine = -1.0f;
    }
    float sine = fn_8000FEE4(1.0f - cosine * cosine);
    if (negate) {
        sine = -sine;
        cosine = -cosine;
    }
    return fn_80016260(sine, cosine);
}

Vec3f fn_80016284(const Vec3f *v) {
    return Vec3f(-v->x, -v->y, -v->z);
}

float fn_80016260(float y, float x) {
    return (float)fn_803B1C3C(y, x);
}

/* Turns out (base) to face target; returns the distance to it, or 0 when it is too close. */
float fn_80016104(Mtx34f *out, const Vec3f *target, const Mtx34f *base, bool flip) {
    Vec3f position;
    position.x = base->values[0][3];
    position.y = base->values[1][3];
    position.z = base->values[2][3];
    Vec3f direction;
    AssignVector(direction, ((VectorSubtractFn)fn_8040CD74)(target, &position));
    Mtx34f inverse;
    fn_8040B4D0(&inverse, base);
    AssignVector(direction, fn_8040B618(&inverse, &direction));
    if (flip) {
        AssignVector(direction, fn_80016284(&direction));
    }
    float yy = direction.y * direction.y;
    float zz = direction.z * direction.z;
    float xx = direction.x * direction.x;
    float distance = fn_8000FEE4(xx + yy + zz);
    if (distance < 0.0001f) {
        return 0.0f;
    }
    fn_8040C148(out, base);
    fn_8040B884(out, fn_80016260(-direction.y, direction.z));
    float across = fn_8000FEE4(yy + zz);
    fn_8040B848(out, fn_80016260(direction.x, across));
    return distance;
}

float fn_80015EC8(Mtx34f *a, Mtx34f *b, Mtx34f *c, const Vec3f *target, const Mtx34f *base,
                  float length0, float length1, float length2, bool flip) {
    float distance = fn_80016104(a, target, base, false);
    if (distance < 0.0001f) {
        return 0.0f;
    }
    if (distance > length2 + (length0 + length1)) {
        fn_8040C148(b, a);
        fn_8040B52C(b, 0.0f, 0.0f, length0);
        fn_8040C148(c, b);
        fn_8040B52C(c, 0.0f, 0.0f, length1);
        return distance;
    }
    float diff = length2 - length0;
    float dd = diff;
    dd *= dd;
    float sum = length0 + length2;
    float qa = 4.0f * length0 * length2;
    float qb = length1 * sum;
    dd += length1 * length1 - distance * distance;
    float disc = qb * qb - qa * dd;
    if (disc < 0.0f) {
        return 0.0f;
    }
    float t = (fn_8000FEE4(disc) - qb) / qa;
    float angle = fn_80015D68(t);
    float offset = fn_8000F3F8(diff * std::sin(angle) / (t * sum + length1));
    fn_8040B884(a, flip ? offset + angle : -(offset + angle));
    fn_8040C148(b, a);
    fn_8040B52C(b, 0.0f, 0.0f, length0);
    fn_8040B884(b, flip ? -angle : angle);
    fn_8040C148(c, b);
    fn_8040B52C(c, 0.0f, 0.0f, length1);
    fn_8040B884(c, flip ? -angle : angle);
    return distance;
}

float fn_80015DE0(Mtx34f *a, Mtx34f *b, const Vec3f *target, const Mtx34f *base, float length0,
                  float length1) {
    float distance = fn_80016104(a, target, base, false);
    if (distance < 0.0001f) {
        return 0.0f;
    }
    fn_8040B884(a, fn_800162AC(length0, distance, length1, 0));
    fn_8040C148(b, a);
    fn_8040B52C(b, 0.0f, 0.0f, length0);
    fn_8040B884(b, fn_800162AC(length0, length1, distance, 1));
    return distance;
}

float fn_80015DBC(Mtx34f *a, Mtx34f *b, Mtx34f *c, const Vec3f *target, const Mtx34f *base,
                  float length0, float length1, float length2) {
    return fn_80015EC8(a, b, c, target, base, length0, length1, length2, false);
}

/* Arc cosine, clamped. */
float fn_80015D68(float x) {
    if (x > 1.0f) {
        return 0.0f;
    }
    if (x < -1.0f) {
        return 0.5f * lbl_805FAD38;
    }
    return (float)fn_803B1BFC(x);
}

float fn_80015B60(Mtx34f *a, Mtx34f *b, Mtx34f *c, const Vec3f *target, const Mtx34f *base,
                  float length0, float length1, float length2) {
    float distance = fn_80016104(a, target, base, true);
    if (distance < 0.0001f) {
        return 0.0f;
    }
    if (distance > length2 + (length0 + length1)) {
        fn_8040C148(b, a);
        fn_8040B52C(b, 0.0f, 0.0f, -length0);
        fn_8040C148(c, b);
        fn_8040B52C(c, 0.0f, 0.0f, -length1);
        return distance;
    }
    float diff = length2 - length0;
    float dd = diff;
    dd *= dd;
    float sum = length0 + length2;
    float qa = 4.0f * length0 * length2;
    float qb = length1 * sum;
    dd += length1 * length1 - distance * distance;
    float disc = qb * qb - qa * dd;
    if (disc < 0.0f) {
        return 0.0f;
    }
    float t = (fn_8000FEE4(disc) - qb) / qa;
    float angle = fn_80015D68(t);
    float offset = fn_8000F3F8(diff * std::sin(angle) / (t * sum + length1));
    offset += angle;
    fn_8040B884(a, offset);
    fn_8040C148(b, a);
    fn_8040B52C(b, 0.0f, 0.0f, -length0);
    fn_8040B884(b, -angle);
    fn_8040C148(c, b);
    fn_8040B52C(c, 0.0f, 0.0f, -length1);
    fn_8040B884(c, -angle);
    return distance;
}

}

UnkClumpUpdater_80015A7C::UnkClumpUpdater_80015A7C(RpClump *clump, _stMtnPlayWork *motion)
    : m_clump(clump), m_motion(motion), m_head(0), m_cursor(0) {
    if (m_clump == 0 || m_motion == 0) {
        delete this;
        return;
    }
    RpHAnimHierarchy *hierarchy = FindHierarchy(m_clump);
    if (hierarchy != 0) {
        fn_80456C54(m_clump, fn_80015698, hierarchy);
        hierarchy->flags |= 2;
        fn_804474D8(hierarchy);
    }
    RwFrame *root = fn_804872D8(m_clump->frame);
    fn_80015570(root, this);
    m_parent = 0;
    fn_80015470(root, this);
}

UnkClumpUpdater_80015A7C::~UnkClumpUpdater_80015A7C() {
    while (m_head != 0) {
        m_cursor = m_head->m_next;
        delete m_head;
        m_head = m_cursor;
    }
    RpHAnimHierarchy *hierarchy = FindHierarchy(m_clump);
    if (hierarchy != 0) {
        hierarchy->flags &= ~2;
    }
}

extern "C" {

void fn_800159AC(UnkClumpUpdater_80015A7C *updater) {
    updater->m_offset = 0;
    updater->m_matrix = 0;
    fn_800158C4(updater);
}

void fn_80015980(UnkClumpUpdater_80015A7C *updater, const Vec3f *offset) {
    updater->m_offset = offset;
    updater->m_matrix = 0;
    fn_800158C4(updater);
}

void fn_80015954(UnkClumpUpdater_80015A7C *updater, const Mtx34f *matrix) {
    updater->m_offset = 0;
    updater->m_matrix = matrix;
    fn_800158C4(updater);
}

void fn_800158C4(UnkClumpUpdater_80015A7C *updater) {
    if (updater->m_head != 0) {
        for (updater->m_cursor = updater->m_head; updater->m_cursor != 0;
             updater->m_cursor = updater->m_cursor->m_next) {
            fn_80015740(updater);
        }
    }
    fn_80456C54(updater->m_clump, fn_80015354, FindHierarchy(updater->m_clump));
}

void fn_80015740(UnkClumpUpdater_80015A7C *updater) {
    RwMatrix *ltm = RwFrameGetLTM(updater->m_cursor->m_frame);
    UnkClumpUpdaterNode_80015A7C *node = updater->m_cursor;
    if (node->m_node != 0) {
        fn_80016390(ltm, node->m_node);
    } else if (node->m_parent != 0) {
        fn_8047EBB8(ltm, &node->m_frame->modelling, node->m_parent);
    }
    if (updater->m_matrix != 0) {
        RwMatrix transform = ToRwMatrix(*updater->m_matrix);
        RwMatrix local = *ltm;
        fn_8047EBB8(ltm, &local, &transform);
    }
    if (updater->m_offset != 0) {
        ltm->pos.x += updater->m_offset->x;
        ltm->pos.y += updater->m_offset->y;
        ltm->pos.z += updater->m_offset->z;
    }
}

/* Finds the skin hierarchy of a frame tree. */
RwFrame *fn_800156E4(RwFrame *frame, void *data) {
    RpHAnimHierarchy *hierarchy = fn_8044757C(frame);
    if (hierarchy == 0) {
        fn_8048752C(frame, fn_800156E4, data);
        return frame;
    }
    *static_cast<RpHAnimHierarchy **>(data) = hierarchy;
    return 0;
}

RpAtomic *fn_80015698(RpAtomic *atomic, void *data) {
    if (fn_8044CCAC(atomic->geometry) != 0) {
        fn_8044CC98(atomic, static_cast<RpHAnimHierarchy *>(data));
    }
    return atomic;
}

/* Lists the frames of the skin hierarchy that the motion has nodes for.
 *
 * NonMatching (one instruction): retail copies the zeroed loop counter into the byte offset with
 * `mr r27,r30`; GC/2.7 emits an independent `li r27,0`. Explicit byte offsets and cached node
 * pointers change the saved-register set; chained initialization, declaration order, and the
 * opt_propagation/opt_strength_reduction pragmas leave the same residual. */
RwFrame *fn_80015570(RwFrame *frame, void *data) {
    UnkClumpUpdater_80015A7C *updater = static_cast<UnkClumpUpdater_80015A7C *>(data);
    if (updater->m_motion != 0) {
        RpHAnimHierarchy *hierarchy = fn_8044757C(frame);
        if (hierarchy != 0) {
            for (int i = 0; i < hierarchy->numNodes; i++) {
                if (hierarchy->nodeInfo[i].frame != 0) {
                    Mtx34f *node = fn_80415E70(updater->m_motion, hierarchy->nodeInfo[i].nodeID);
                    if (node != 0) {
                        if (updater->m_head == 0) {
                            updater->m_head = updater->m_cursor = new UnkClumpUpdaterNode_80015A7C;
                        } else {
                            updater->m_cursor->m_next = new UnkClumpUpdaterNode_80015A7C;
                            updater->m_cursor = updater->m_cursor->m_next;
                        }
                        if (updater->m_cursor != 0) {
                            updater->m_cursor->m_frame = hierarchy->nodeInfo[i].frame;
                            updater->m_cursor->m_id = hierarchy->nodeInfo[i].nodeID;
                            updater->m_cursor->m_node = node;
                            updater->m_cursor->m_parent = 0;
                            updater->m_cursor->m_next = 0;
                        }
                    }
                }
            }
        }
    }
    fn_8048752C(frame, fn_80015570, data);
    return frame;
}

/* Lists the frames not listed yet, under their parent's LTM. */
RwFrame *fn_80015470(RwFrame *frame, UnkClumpUpdater_80015A7C *updater) {
    if (updater->m_motion != 0) {
        UnkClumpUpdaterNode_80015A7C *node;
        UnkClumpUpdaterNode_80015A7C *head = updater->m_head;
        if (head != 0) {
            for (node = head; node != 0; node = node->m_next) {
                if (frame == node->m_frame) {
                    goto children;
                }
            }
        }
        if (head == 0) {
            updater->m_head = updater->m_cursor = new UnkClumpUpdaterNode_80015A7C;
        } else {
            updater->m_cursor->m_next = new UnkClumpUpdaterNode_80015A7C;
            updater->m_cursor = updater->m_cursor->m_next;
        }
        if (updater->m_cursor != 0) {
            updater->m_cursor->m_frame = frame;
            updater->m_cursor->m_id = -1;
            updater->m_cursor->m_node = 0;
            updater->m_cursor->m_parent = updater->m_parent;
            updater->m_cursor->m_next = 0;
        }
    }
children:
    RwMatrix *parent = updater->m_parent;
    updater->m_parent = &frame->ltm;
    fn_8048752C(frame, (RwFrameCallBack)fn_80015470, updater);
    updater->m_parent = parent;
    return frame;
}

/* Moves an atomic's world bounding sphere with its frame and scales it by the frame's largest
 * axis. */
RpAtomic *fn_80015354(RpAtomic *atomic, void *data) {
    RwMatrix *ltm = RwFrameGetLTM(atomic->frame);
    fn_80482458(&atomic->worldBoundingSphere.center, &atomic->boundingSphere.center, ltm);
    float right = LengthSquared(ltm->right);
    float up = LengthSquared(ltm->up);
    float at = LengthSquared(ltm->at);
    float scale = fn_80482340(MAX(right, MAX(up, at)));
    if (s_largeBounds) {
        scale *= 100.0f;
    } else {
        scale *= 1.4f;
    }
    atomic->worldBoundingSphere.radius = atomic->boundingSphere.radius * scale;
    return atomic;
}

bool fn_80015344(bool large) {
    bool previous = s_largeBounds;
    s_largeBounds = large;
    return previous;
}

}
