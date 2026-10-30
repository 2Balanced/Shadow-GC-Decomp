/* The vehicles' collision tests (no class of their own; file name inferred): a sweep of a sphere
 * through the collision world that slides it along what it touches (and, for a vehicle, ignores
 * the vehicle's colliders and reports the hits to it), a line test, the sweep and line test that
 * other units call to move a body (Bomb, the containers, ScatteringWeapon, ...), and whether a line
 * is long enough for the debug line test. A sibling of Game/VehicleColliMove.cpp's sweep. The
 * functions keep their placeholder names because other units call them by those names; their
 * behaviour is inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first).
 *
 * NonMatching: every function but the sweep (fn_801BFFD8) matches, and all data matches. The sweep
 * has retail's instructions and stack layout; it differs only in register allocation: retail keeps
 * &filter in the mask parameter's register across the loop and gives the loop's locals the high
 * saved registers, which GC/2.7 does not (the blocker of Game/VehicleColliMove.cpp's Sweep and
 * Settle; about ten source shapes tried). */
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/MathTypes.h"
#include "Game/TVehicleBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::CharaColliBase;
using SonicteamUSA::System::CharaColli::CharaColliBaseCharInfo;
using Vehicle::TVehicleBase;

namespace Player {
class PlayerBase;
} // namespace Player

/* fn_8040CD74 called as returning a - b by value (see Game/ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* vector = value through the out-of-line assignment, and the out-of-line copy constructor (see
 * Game/VehicleColliMove.cpp). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}
inline void CopyVector(Vec3f &vector, const Vec3f &value) {
    fn_800091EC(&vector, &value);
}

/* A length (see Game/VehicleColliMove.cpp): as a call it is not a constant expression, so the
 * constant below is stored by a static initializer, as in retail. */
inline float Span(float length) {
    return length;
}

/* How far the sweep pushes away from what it touched (name inferred). */
static float s_skin = Span(0.020000001f);

/* A line from start to end (set up by fn_80020A20; see Game/VehicleColliMove.cpp). */
struct UnkLine_80020A20 {
    /* 0x00 */ float m_start[3];
    /* 0x0C */ float m_end[3];
};

/* What the tests' filter skips and accepts (struct name unknown; a weak constructor and destructor
 * of this unit, 0x801BFF9C and 0x801BFF40): hits on the vehicle's two colliders are skipped; other
 * hits count when their attributes share a bit with m_mask (names inferred). */
struct UnkSweepFilter_801BFF9C {
    /* 0x00 */ u32 m_mask;
    /* 0x04 */ boost::shared_ptr<CharaColliBase> m_body;
    /* 0x0C */ boost::shared_ptr<CharaColliBase> m_other;
};
SIZE_ASSERT(UnkSweepFilter_801BFF9C, 0x14);

/* What the tests pass the filter for each candidate hit (see Game/VehicleColliMove.cpp). */
struct UnkCollisionCandidate_80021934 {
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ CharaColliBase *m_collider;
    /* 0x0C */ u32 m_attributes;
    /* 0x10 */ const UnkSweepFilter_801BFF9C *m_filter;
};

typedef bool (*UnkCollisionFilter_80021934)(UnkCollisionCandidate_80021934 *candidate);

/* A sphere: centre and radius (see Game/VehicleColliMove.cpp). */
struct UnkSphere_80021C04 {
    /* 0x00 */ float center[3];
    /* 0x0C */ float radius;
};

/* A node of the hits: its world object at 0x14 (see Game/VehicleColliMove.cpp). */
struct UnkColliContactNode_800587A0 {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ void *m_object;
};

struct UnkColliContactIter_8001EDFC {
    UnkColliContactIter_8001EDFC(UnkColliContactNode_800587A0 *node) : m_node(node) {}

    UnkColliContactNode_800587A0 *m_node;
};

/* The hits of a test (see Game/VehicleColliMove.cpp) and the first of them. */
struct CollisionResultList;
struct UnkLineHit_8002735C {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ void *m_object;
};

/* The vehicle as the sweep calls it: TVehicleBase's slot 0x40 takes the hit the vehicle touched
 * (see Game/VehicleColliMove.cpp). */
class UnkVehicleHitView_80333B1C {
  public:
    /* 0x00 */ u8 field_0x0[0x38];

    /* 0x38: vtable pointer */
    virtual void vf_0x08();
    virtual void vf_0x0C();
    virtual void vf_0x10();
    virtual void vf_0x14();
    virtual void vf_0x18();
    virtual void vf_0x1C();
    virtual void vf_0x20();
    virtual void vf_0x24();
    virtual void vf_0x28();
    virtual void vf_0x2C();
    virtual void vf_0x30();
    virtual void vf_0x34();
    virtual void vf_0x38();
    virtual void vf_0x3C();
    virtual void OnHit(const UnkColliContactIter_8001EDFC &hit);
};

extern "C" {
/* The collision world, a sphere, the sweep of a sphere and the test of a line (see
 * Game/VehicleColliMove.cpp and Game/BulletUtil.cpp). */
void *fn_80020D40(void);
void fn_80021C04(UnkSphere_80021C04 *sphere, const Vec3f *center, float radius);
CollisionResultList *fn_80021934(void *world, const UnkSphere_80021C04 *sphere, const Vec3f *velocity,
                                 int arg3, int arg4, CollisionResultList *previous,
                                 UnkCollisionFilter_80021934 filter, const UnkSweepFilter_801BFF9C *self,
                                 int arg8);
CollisionResultList *fn_80021FE4(void *world, const UnkLine_80020A20 *line, int arg2, int arg3,
                                 UnkCollisionFilter_80021934 filter, const UnkSweepFilter_801BFF9C *self);
void *fn_80020A20(UnkLine_80020A20 *line, const Vec3f *start, const Vec3f *end);
/* The hits: the first, their release, iteration (see Game/VehicleColliMove.cpp). */
UnkLineHit_8002735C *fn_8002735C(CollisionResultList *hits);
void fn_80026FB4(CollisionResultList *hits, int arg1);
UnkColliContactNode_800587A0 *fn_8001EE04(CollisionResultList *hits);
UnkColliContactNode_800587A0 *fn_8001EDEC(CollisionResultList *hits);
UnkColliContactIter_8001EDFC fn_80027374(UnkColliContactIter_8001EDFC *it, int);
/* A world object's position, normal, a second point (behaviour inferred), its distance and its
 * kind. */
const Vec3f *fn_8001E9BC(void *object);
const Vec3f *fn_800266D4(void *object);
const Vec3f *fn_8001E9C4(void *object);
float fn_80097D0C(void *object);
int fn_80099080(void *object);
/* A hit's collider, and the character info a collider is attached to (see
 * Game/VehicleColliMove.cpp). */
boost::weak_ptr<CharaColliBase> fn_800587A0(UnkColliContactNode_800587A0 *node);
boost::weak_ptr<CharInfo> fn_800586EC(CharaColliBaseCharInfo *collider);
/* The vehicle's body and second collider (see Game/VehicleColliMove.cpp). */
boost::shared_ptr<CharaColliBase> fn_801B6350(TVehicleBase *vehicle);
boost::shared_ptr<CharaColliBase> fn_801B6380(TVehicleBase *vehicle);
/* The player's line test (see Game/PlayerCharaColliImpl.cpp). */
float fn_800979E8(boost::shared_ptr<Player::PlayerBase> *player, Vec3f *position, Vec3f *normal,
                  void *collider, u32 *attributes, u32 *kind, const UnkLine_80020A20 *line, void *ignore, u32 mask,
                  u32 exclude, int arg10);
/* A 32-bit flag word (see Game/VehicleColliMove.cpp). */
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
/* Vector helpers (see Game/VehicleColliMove.cpp). */
Vec3f fn_800091A4(const Vec3f *vector, float scale);
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
void fn_8040CE04(Vec3f *vector, const Vec3f *value); /* -= */
void fn_8040CE60(Vec3f *vector);
Vec3f fn_8040CEE8(const Vec3f *vector);
float fn_8040CF90(const Vec3f *a, const Vec3f *b);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* += */
void fn_8040F990(Vec3f *along, Vec3f *across, const Vec3f *vector, const Vec3f *normal);
Vec3f fn_80093230(const Vec3f *direction);
Vec3f fn_80016284(const Vec3f *vector);
}

/* a += b, a -= b and their dot product (through the vector helpers). */
inline void AddVector(Vec3f &a, const Vec3f &b) {
    fn_8040CFF8(&a, &b);
}
inline void SubVector(Vec3f &a, const Vec3f &b) {
    fn_8040CE04(&a, &b);
}
inline float Dot(const Vec3f &a, const Vec3f &b) {
    return fn_8040CF90(&a, &b);
}

/* Accepts a candidate hit of the tests (name inferred). */
static bool AcceptHit(UnkCollisionCandidate_80021934 *candidate) {
    const UnkSweepFilter_801BFF9C *filter = candidate->m_filter;
    u32 mask = filter->m_mask;
    if (candidate->m_collider != 0) {
        if (candidate->m_collider == filter->m_other.get()) {
            return false;
        }
        if (candidate->m_collider == filter->m_body.get()) {
            return false;
        }
    }
    return (candidate->m_attributes & mask) != 0;
}

/* Sweeps a sphere of radius around position along velocity through the world, sliding along what
 * it touches (along the slope when slide is set) at most count times; chara gets the character
 * last touched. Whether it touched anything. */
extern "C" bool fn_801BFFD8(TVehicleBase *vehicle, u32 mask, Vec3f *position, Vec3f *velocity, bool slide,
                            boost::weak_ptr<CharInfo> *chara, int count, float radius) {
    void *world = fn_80020D40();
    CollisionResultList *previous = 0;
    UnkSphere_80021C04 sphere;
    fn_80021C04(&sphere, position, radius);
    Vec3f move;
    fn_800091EC(&move, velocity);
    UnkSweepFilter_801BFF9C filter;
    filter.m_mask = mask;
    if (vehicle != 0) {
        filter.m_body = fn_801B6350(vehicle);
        filter.m_other = fn_801B6380(vehicle);
    }
    bool again;
    CollisionResultList *hits;
    bool hit = false;
    const UnkSweepFilter_801BFF9C *self = &filter;
    do {
        again = false;
        Vec3f start;
        fn_800091EC(&start, &move);
        hits = fn_80021934(world, &sphere, &move, 3, 1, previous, AcceptHit, self, 0);
        if (previous != 0) {
            fn_80026FB4(previous, 1);
            previous = 0;
        }
        fn_8040CFF8(reinterpret_cast<Vec3f *>(sphere.center), &move);
        if (chara != 0) {
            chara->reset();
        }
        if (hits != 0) {
            hit = true;
            Vec3f remain;
            fn_800091EC(&remain, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&start, &move));
            Vec3f push(0.0f, 0.0f, 0.0f);
            float pushes = 0.0f;
            for (UnkColliContactIter_8001EDFC it(fn_8001EE04(hits)); it.m_node != fn_8001EDEC(hits);
                 fn_80027374(&it, 0)) {
                UnkColliContactNode_800587A0 *node = it.m_node;
                void *object = node->m_object;
                Vec3f hitNormal;
                fn_800091EC(&hitNormal, fn_800266D4(object));
                Vec3f hitPoint;
                fn_800091EC(&hitPoint, fn_8001E9C4(object));
                if (fn_8040CF90(&hitNormal, &remain) <= 0.0f) {
                    SubVector(remain, fn_80093230(&hitNormal));
                }
                if (fn_8040CF90(&hitNormal, velocity) <= 0.0f) {
                    Vec3f along;
                    Vec3f across;
                    fn_8040F990(&along, &across, velocity, &hitNormal);
                    if (vehicle != 0) {
                        UnkColliContactIter_8001EDFC touched(node);
                        reinterpret_cast<UnkVehicleHitView_80333B1C *>(vehicle)->OnHit(touched);
                    }
                    if (fn_8040CF90(&across, velocity) <= 0.0f) {
                        fn_8040CE60(velocity);
                    } else if (slide) {
                        float slope = Dot(fn_8040CEE8(velocity), fn_8040CEE8(&along));
                        Vec3f turn;
                        float half = 0.5f * slope;
                        CopyVector(turn, fn_8040CDA8(&fn_80016284(&along), 1.0f + half));
                        fn_8040CFF8(velocity, &turn);
                    } else {
                        AssignVector(*velocity, across);
                    }
                }
                AddVector(reinterpret_cast<Vec3f &>(sphere.center), fn_800091A4(&hitNormal, s_skin));
                if (fn_80099080(object) == 1) {
                    Vec3f offset;
                    fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                             reinterpret_cast<Vec3f *>(sphere.center), fn_8001E9BC(object)));
                    float distance = fn_8040CF90(&offset, &hitNormal);
                    if (distance >= 0.0f && fn_8040CF2C(&offset) < sphere.radius * sphere.radius) {
                        Vec3f out;
                        CopyVector(out, fn_8040CDA8(&hitNormal, sphere.radius - distance));
                        fn_8040CFF8(reinterpret_cast<Vec3f *>(sphere.center), &out);
                        fn_8040CFF8(&push, &out);
                        pushes += 1.0f;
                    }
                }
                count--;
                boost::shared_ptr<CharaColliBase> collider = fn_800587A0(node).lock();
                boost::weak_ptr<CharInfo> owner;
                if (collider.get() != 0) {
                    boost::shared_ptr<CharaColliBaseCharInfo> charCollider =
                        boost::dynamic_pointer_cast<CharaColliBaseCharInfo>(collider);
                    owner = fn_800586EC(charCollider.get());
                }
                if (chara != 0) {
                    *chara = owner;
                }
            }
            if (pushes > 1.0f) {
                Vec3f average;
                CopyVector(average, fn_8040CDA8(&push, 1.0f / pushes));
                fn_8040CE04(reinterpret_cast<Vec3f *>(sphere.center), &average);
            }
            if (fn_8040CF2C(&remain) > 0.001f) {
                again = true;
                AssignVector(move, remain);
            }
        }
        if (again) {
            previous = hits;
            hits = 0;
        }
    } while (again && count > 0);
    if (hits != 0) {
        fn_80026FB4(hits, 1);
    }
    if (previous != 0) {
        fn_80026FB4(previous, 1);
    }
    AssignVector(*position, reinterpret_cast<Vec3f &>(sphere.center));
    return hit;
}

/* Tests line against the world (skipping the vehicle's colliders); the distance to the first hit,
 * whose position and normal it stores, or -1. */
extern "C" float fn_801BFDD0(TVehicleBase *vehicle, Vec3f *hitPosition, Vec3f *hitNormal, const UnkLine_80020A20 *line,
                             u32 mask, int arg5) {
    void *object;
    void *world = fn_80020D40();
    UnkSweepFilter_801BFF9C filter;
    filter.m_mask = mask;
    if (vehicle != 0) {
        filter.m_body = fn_801B6350(vehicle);
        filter.m_other = fn_801B6380(vehicle);
    }
    CollisionResultList *hits = fn_80021FE4(world, line, 3, arg5, AcceptHit, &filter);
    if (hits != 0) {
        object = fn_8002735C(hits)->m_object;
        float distance = fn_80097D0C(object);
        if (distance <= 0.0f) {
            distance = -1.0f;
        } else {
            if (hitPosition != 0) {
                AssignVector(*hitPosition, *fn_8001E9BC(object));
            }
            if (hitNormal != 0) {
                AssignVector(*hitNormal, *fn_800266D4(object));
            }
        }
        fn_80026FB4(hits, 1);
        return distance;
    }
    return -1.0f;
}

/* Moves a sphere of radius from position along delta through the world (the hits of mask and the
 * usual kinds); bit 0 of flags is set when it lands on ground below it (bit 1 makes it slide), and
 * the ground's position and normal are stored. Whether it touched anything. */
extern "C" bool fn_801BFC68(Vec3f *hitPosition, Vec3f *hitNormal, TVehicleBase *vehicle, Vec3f *position,
                            Vec3f *delta, u32 *flags, u32 mask, float radius) {
    u32 kinds = mask | 0x1A93;
    boost::weak_ptr<CharInfo> chara;
    fn_8006127C(flags, 0);
    bool hit = fn_801BFFD8(vehicle, kinds, position, delta, fn_80014E34(flags, 1), &chara, 8, radius);
    UnkLine_80020A20 line;
    Vec3f up(0.0f, 1.0f, 0.0f);
    Vec3f down;
    fn_800091EC(&down, &(const Vec3f &)fn_8040CDA8(&up, -(1.5f * radius)));
    fn_80020A20(&line, position, &(const Vec3f &)fn_8040CDD0(position, &down));
    if (fn_801BFDD0(vehicle, hitPosition, hitNormal, &line, kinds, 1) >= 0.0f) {
        if (fn_8040CF90(delta, &Vec3f(0.0f, 1.0f, 0.0f)) <= 0.1f) {
            fn_80014E1C(flags, 0);
        }
    }
    return hit;
}

/* The same without the ground's position and normal. */
extern "C" bool fn_801BFC28(TVehicleBase *vehicle, Vec3f *position, Vec3f *delta, u32 *flags, u32 mask,
                            float radius) {
    return fn_801BFC68(0, 0, vehicle, position, delta, flags, mask, radius);
}

/* Whether the player's line test of line (when it is long enough) meets attribute bit 5 (meaning
 * unknown). */
extern "C" bool fn_801BFB70(const UnkLine_80020A20 *line) {
    Vec3f position;
    Vec3f normal;
    if (fn_8040CF2C(&(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(reinterpret_cast<const Vec3f *>(line->m_start),
                                                                  reinterpret_cast<const Vec3f *>(line->m_end))) < 0.0001f) {
        return false;
    }
    u32 attributes = 0;
    u32 kind = 0;
    {
        boost::shared_ptr<Player::PlayerBase> player;
        fn_800979E8(&player, &position, &normal, 0, &attributes, &kind, line, 0, 0x20, 0x40000, 2);
    }
    return (attributes >> 5) & 1;
}
