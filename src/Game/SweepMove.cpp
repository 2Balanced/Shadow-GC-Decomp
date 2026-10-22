/* A sphere's move through the collision world (no RTTI; the file and every name here are
 * inferred). fn_8026EE9C moves a sphere from where it was towards where it is now in up to five
 * steps, pushing it out of what it touches and sliding the rest of the move, and the velocity,
 * along the surfaces (see ElecSecurityCode.cpp, its caller). Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#include <dolphin/types.h>

struct SweepVector;

extern "C" {
/* The vector copy constructor and assignment the game keeps out of line (see MathTypes.h). */
SweepVector *fn_800091EC(SweepVector *out, const SweepVector *from);
SweepVector *fn_800091D0(SweepVector *out, const SweepVector *from);
}

/* A three-float vector (SweepVector in MathTypes.h, whose declarations of the vector routines take
 * their result by pointer) whose copy constructor and assignment are the out-of-line ones. */
struct SweepVector {
    float x;
    float y;
    float z;
};

/* A node of the sweep's hits: the world object it hit at 0x14 (see VehicleColliMove.cpp). */
struct UnkColliContactNode_800587A0 {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ u8 *m_object;
};

/* An iterator over the hits (layout from fn_8001EDFC; see BlockChange.cpp). */
struct UnkColliContactIter_8001EDFC {
    struct Unset {};

    UnkColliContactIter_8001EDFC() : m_node(0) {}
    UnkColliContactIter_8001EDFC(Unset) {}

    UnkColliContactNode_800587A0 *m_node;
};

/* The hits of a sweep (see MeteorsHolder.cpp): their number at 0x4. */
struct CollisionResultList {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ s32 m_count;
};

/* What the sweep passes its filter for each candidate hit (see ElecSecurityCode.cpp). */
struct UnkSweepHit_8026EE9C;
typedef bool (*UnkSweepFilter_8026EE9C)(UnkSweepHit_8026EE9C *hit);

/* A sphere: centre and radius (see VehicleColliMove.cpp). */
struct UnkSphere_80021C04 {
    /* 0x00 */ SweepVector center;
    /* 0x0C */ float radius;
};

extern "C" {
/* The collision world, a sphere, and the sweep of a sphere along a velocity (see
 * VehicleColliMove.cpp). */
void *fn_80020D40(void);
void fn_80021C04(UnkSphere_80021C04 *sphere, const SweepVector *center, float radius);
CollisionResultList *fn_80021934(void *world, const UnkSphere_80021C04 *sphere,
                                 const SweepVector *velocity, int arg3, int arg4,
                                 CollisionResultList *previous, UnkSweepFilter_8026EE9C filter,
                                 void *user, int arg8);
void fn_80026FB4(CollisionResultList *hits, int arg1);
UnkColliContactNode_800587A0 *fn_8001EE04(CollisionResultList *hits);
UnkColliContactNode_800587A0 *fn_8001EDEC(CollisionResultList *hits);
void fn_8001EDFC(UnkColliContactIter_8001EDFC *it, UnkColliContactNode_800587A0 *node);
UnkColliContactIter_8001EDFC fn_80027374(UnkColliContactIter_8001EDFC *it, int);
/* A world object's normal and kind (see VehicleColliMove.cpp), and a plane's distance (behaviour
 * inferred). */
const SweepVector *fn_800266D4(void *object);
int fn_80099080(void *object);
float fn_8001E9E8(void *plane);
/* Vector routines (behaviour inferred from their bodies). */
void fn_8040CD74(SweepVector *out, const SweepVector *a, const SweepVector *b); /* a - b */
void fn_8040CDD0(SweepVector *out, const SweepVector *a, const SweepVector *b); /* a + b */
void fn_800091A4(SweepVector *out, const SweepVector *v, float scale);         /* v * scale */
void fn_8040CFF8(SweepVector *v, const SweepVector *value); /* += */
void fn_8040CE04(SweepVector *v, const SweepVector *value);       /* -= */
void fn_8040CE38(SweepVector *v, float scale);              /* *= */
float fn_8040CF90(const SweepVector *a, const SweepVector *b);    /* a . b */
s32 fn_8026F0B4(CollisionResultList *hits);
}

/* A zero vector (see MathTypes.h). */
extern const SweepVector lbl_805E07A0;

static void Slide(SweepVector *result, SweepVector *velocity, SweepVector *move,
                  CollisionResultList *hits, bool *hit);
static void PushOut(SweepVector *result, CollisionResultList *hits);

/* Moves position by move in up to five steps (see fn_8026EE9C); position and move receive where
 * it ended and how far it went. */
static bool Sweep(SweepVector *velocity, SweepVector *position, SweepVector *move,
                  UnkSweepFilter_8026EE9C filter, void *user, float radius) {
    bool hit = false;
    void *world = fn_80020D40();
    UnkSphere_80021C04 sphere;
    CollisionResultList *hits = 0;
    fn_80021C04(&sphere, position, radius);
    SweepVector start;
    fn_800091EC(&start, position);
    SweepVector current;
    fn_800091EC(&current, position);
    SweepVector previous;
    fn_800091EC(&previous, &current);
    SweepVector rest;
    fn_800091EC(&rest, move);
    SweepVector saved;
    fn_800091EC(&saved, &rest);
    CollisionResultList *last;
    for (int i = 0; i < 5; i++) {
        fn_800091D0(&saved, &rest);
        SweepVector step;
        fn_800091EC(&step, &rest);
        SweepVector push;
        SweepVector arg;
        SweepVector out;
        SweepVector left;
        SweepVector done;
        SweepVector slid;
        fn_800091D0(&previous, &current);
        fn_800091D0(&sphere.center, &current);
        last = hits;
        hits = fn_80021934(world, &sphere, &step, 3, 5, last, filter, user, 1);
        if (last != 0) {
            fn_80026FB4(last, 1);
        }
        fn_8040CFF8(&current, &step);
        if (hits != 0) {
            PushOut(&out, hits);
            fn_800091EC(&push, &out);
            fn_8040CFF8(&current, &push);
        }
        if (hits == 0) {
            break;
        }
        fn_8040CD74(&done, &current, &previous);
        fn_8040CD74(&left, &rest, &done);
        fn_800091EC(&arg, &left);
        Slide(&slid, velocity, &arg, hits, &hit);
        fn_800091D0(&rest, &slid);
        if (rest.x * rest.x + rest.y * rest.y + rest.z * rest.z < 0.0001f) {
            fn_800091D0(&rest, &saved);
            break;
        }
    }
    SweepVector moved;
    fn_8040CD74(&moved, &current, &start);
    fn_800091D0(move, &moved);
    fn_800091D0(position, &current);
    if (hits != 0) {
        fn_80026FB4(hits, 1);
    }
    return hit;
}

/* Slides move along every surface it was hit by, and velocity with it. */
static void Slide(SweepVector *result, SweepVector *velocity, SweepVector *move, CollisionResultList *hits,
                  bool *hit) {
    SweepVector out;
    fn_800091EC(&out, move);
    UnkColliContactIter_8001EDFC it;
    {
        UnkColliContactIter_8001EDFC first((UnkColliContactIter_8001EDFC::Unset()));
        fn_8001EDFC(&first, fn_8001EE04(hits));
        it = first;
    }
    for (; it.m_node != fn_8001EDEC(hits); fn_80027374(&it, 0)) {
        SweepVector normal;
        fn_800091EC(&normal, fn_800266D4(it.m_node->m_object));
        float along = fn_8040CF90(&out, &normal);
        float speed = fn_8040CF90(velocity, &normal);
        if (!(along > 0.0f)) {
            *hit = true;
            SweepVector a;
            fn_800091A4(&a, &normal, along);
            fn_8040CE04(&out, &a);
            SweepVector b;
            fn_800091A4(&b, &normal, speed);
            fn_8040CE04(velocity, &b);
        }
    }
    fn_800091EC(result, &out);
}

extern "C" s32 fn_8026F0B4(CollisionResultList *hits) {
    return hits->m_count;
}

/* How far to push the sphere out of what it touches. */
static void PushOut(SweepVector *result, CollisionResultList *hits) {
    if (hits == 0 || hits->m_count == 0) {
        fn_800091EC(result, &lbl_805E07A0);
        return;
    }
    UnkColliContactIter_8001EDFC it;
    SweepVector push;
    fn_800091EC(&push, &lbl_805E07A0);
    SweepVector depth;
    fn_800091EC(&depth, &lbl_805E07A0);
    {
        UnkColliContactIter_8001EDFC first((UnkColliContactIter_8001EDFC::Unset()));
        fn_8001EDFC(&first, fn_8001EE04(hits));
        it = first;
    }
    for (; it.m_node != fn_8001EDEC(hits); fn_80027374(&it, 0)) {
        u8 *object = it.m_node->m_object;
        u8 *plane = object + 0xC;
        SweepVector normal;
        fn_800091EC(&normal, fn_800266D4(object));
        if (fn_80099080(object) == 1 && *reinterpret_cast<float *>(plane + 0x10) < 0.0f) {
            SweepVector d;
            fn_800091A4(&d, &normal, -fn_8001E9E8(plane));
            fn_8040CFF8(&depth, &d);
        }
        SweepVector p;
        fn_800091A4(&p, &normal, 0.02f);
        fn_8040CFF8(&push, &p);
    }
    fn_8040CE38(&push, 1.0f / fn_8026F0B4(hits));
    fn_8040CDD0(result, &depth, &push);
}

/* Moves position from previous towards where it is now, stopping at the colliders filter accepts
 * (user is handed to the filter); returns whether it slid along anything. */
extern "C" bool fn_8026EE9C(SweepVector *position, const SweepVector *previous, SweepVector *velocity,
                            UnkSweepFilter_8026EE9C filter, void *user, float radius) {
    SweepVector from;
    fn_800091EC(&from, previous);
    SweepVector move;
    {
        SweepVector delta;
        fn_8040CD74(&delta, position, previous);
        fn_800091EC(&move, &delta);
    }
    bool hit = Sweep(velocity, &from, &move, filter, user, radius);
    fn_800091D0(position, &from);
    return hit;
}
