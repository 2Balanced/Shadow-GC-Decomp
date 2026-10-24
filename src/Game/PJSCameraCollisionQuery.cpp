/* The PJS camera's collision queries (a unit without classes; the file name is inferred): moving a
 * sphere from one point to another and sliding it along what it hits, whether the way between two
 * points is blocked, pulling a mode's eye in to where the line from a point to it hits the stage,
 * the first hit on a line, and the two filters the queries pass the collision system. Other units
 * call them by address (Game/CameraModeGeneral.cpp, Game/PJSCameraCollision.cpp, the stage camera
 * post process). Names are inferred. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#include "Game/CameraModeUser.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::Camera::CameraMode;

namespace SonicteamUSA {
namespace System {
namespace Collision {
/* CollisionResult (class name from the RTTI of its shared_ptr control block): a hit, the object
 * hit at 0x14 (see Game/CameraModeGeneral.cpp). */
class CollisionResult {
  public:
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ void *m_object;
};

/* CollisionResultList (class name from the RTTI of its shared_ptr control block): the hits of a
 * sweep (see Game/CameraModeGeneral.cpp). */
class CollisionResultList {
  public:
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ CollisionResult *m_first;
};
} // namespace Collision
} // namespace System
} // namespace SonicteamUSA

using SonicteamUSA::System::Collision::CollisionResult;
using SonicteamUSA::System::Collision::CollisionResultList;

/* A sphere and a line for the collision queries (see Game/CameraModeGeneral.cpp). */
struct UnkSphere_80021C04 {
    /* 0x00 */ float center[3];
    /* 0x0C */ float radius;
};

struct UnkLine_80020A20 {
    u8 field_0x0[0x18];
};

/* A candidate a query's filter looks at (layout from the filters). */
struct UnkCollisionCandidate {
    /* 0x00 */ u8 field_0x0[0xC];
    /* 0x0C */ u32 m_flags;
};

/* The filter a collision query calls for each candidate (see Game/MeteorsHolder.cpp). */
typedef bool (*UnkCollisionFilter_80021934)(UnkCollisionCandidate *candidate);

/* An iterator over the hits (see Game/BirdBase.cpp). */
struct UnkHitIter_80027374 {
    UnkHitIter_80027374(CollisionResult *node) : m_node(node) {}

    CollisionResult *m_node;
};

extern "C" {
/* The collision world, a sphere, a line, a sweep of a sphere and the first hit of a line (see
 * Game/CameraModeGeneral.cpp and Game/AttackObject.cpp). */
void *fn_80020D40(void);
void fn_80021C04(UnkSphere_80021C04 *sphere, const Vec3f *center, float radius);
void *fn_80020A20(UnkLine_80020A20 *line, const Vec3f *start, const Vec3f *end);
CollisionResultList *fn_80021934(void *world, const UnkSphere_80021C04 *sphere,
                                 const Vec3f *velocity, int arg3, int arg4, int arg5,
                                 UnkCollisionFilter_80021934 filter, void *data, int arg8);
CollisionResult *fn_80021E24(void *world, const UnkLine_80020A20 *line, int arg2, int arg3,
                             UnkCollisionFilter_80021934 filter, int arg5);
/* The hits' first node and end, and an iterator's increment (see Game/BirdBase.cpp); a hit
 * object's position, normal and kind (behaviour inferred). */
CollisionResult *fn_8001EE04(CollisionResultList *hits);
CollisionResult *fn_8001EDEC(CollisionResultList *hits);
UnkHitIter_80027374 fn_80027374(UnkHitIter_80027374 *it, int);
const Vec3f *fn_8001E9BC(void *object);
const Vec3f *fn_800266D4(void *object);
int fn_80099080(void *object);
/* shared_ptr<CollisionResultList>'s pointer, out of line (see Game/CameraModeGeneral.cpp). */
CollisionResultList *fn_800CFDE4(const boost::shared_ptr<CollisionResultList> *hits);
/* Releases a hit (CollisionResult's destructor; see checked_delete below). */
void fn_800265A4(CollisionResult *hit, int arg1);

/* Sets a camera mode's eye and point looked at (see Game/CameraModeUser.h). */
void fn_8000A5D4(CameraMode *mode, const Vec3f *eye, const Vec3f *target);

/* Vector routines (behaviour inferred). */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);   /* a + b */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale); /* vector * scale */
Vec3f fn_800091A4(const Vec3f *vector, float scale); /* vector * scale */
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b);   /* a x b */
Vec3f fn_8040CEE8(const Vec3f *vector);              /* normalized */
Vec3f fn_80016284(const Vec3f *vector);              /* -vector */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* vector += value */
void fn_8040CE04(Vec3f *vector, const Vec3f *value); /* vector -= value */
void fn_8040CE38(Vec3f *vector, float scale);        /* vector *= scale */
float fn_8040CE74(Vec3f *vector);                    /* normalises */
float fn_8040CF90(const Vec3f *a, const Vec3f *b);   /* a . b */
}

/* fn_8040CD74 called as returning a - b by value (see Game/ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* Retail releases a hit through its out-of-line destructor (as Game/CameraModeGeneral.cpp does). */
namespace boost {
template<> inline void checked_delete<CollisionResult>(CollisionResult *x) {
    fn_800265A4(x, 1);
}
} // namespace boost

/* Whether two points differ. */
inline bool operator!=(const Vec3f &a, const Vec3f &b) {
    return a.x != b.x || a.y != b.y || a.z != b.z;
}

extern "C" {

/* The filter of the camera's queries: candidates without these flags. */
bool fn_801C7D20(UnkCollisionCandidate *candidate) {
    return (candidate->m_flags & 0x100108) == 0;
}

/* Another filter (used by the stage camera post process). */
bool fn_801C7D04(UnkCollisionCandidate *candidate) {
    return (candidate->m_flags & 0x50008) == 0;
}

/* The first hit on the line from one point to another, or 0. */
CollisionResult *fn_801C7C70(const Vec3f *from, const Vec3f *to) {
    if (*from != *to) {
        UnkLine_80020A20 line;
        fn_80020A20(&line, from, to);
        return fn_80021E24(fn_80020D40(), &line, 1, 0xD, fn_801C7D20, 0);
    }
    return 0;
}

/* Pulls a mode's eye in to where the line from point to it (stretched by radius) hits the stage,
 * then turns it about the character; whether it did. */
bool fn_801C7A20(CameraMode *mode, Vec3f *point, CharInfo *chara, float radius) {
    Vec3f direction;
    fn_800091EC(&direction, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&mode->m_eye, point));
    Vec3f unit;
    fn_800091EC(&unit, &(const Vec3f &)fn_8040CEE8(&direction));
    Vec3f far;
    fn_800091EC(&far, &(const Vec3f &)fn_8040CDD0(&mode->m_eye,
                                                    &(const Vec3f &)fn_8040CDA8(&unit, radius)));
    boost::shared_ptr<CollisionResult> hit(fn_801C7C70(point, &far));
    if (hit.get() == 0) {
        return false;
    }
    const Vec3f *position = fn_8001E9BC(hit->m_object);
    float length = fn_8040CF50(&direction);
    Vec3f eye;
    fn_800091EC(&eye, &(const Vec3f &)fn_8040CDD0(
                          &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)fn_80016284(&unit), radius),
                          position));
    float distance = fn_8040CF50(&(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&eye, point));
    Vec3f offset;
    fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(point, &chara->m_position));
    fn_8040CE38(&offset, distance / length);
    Vec3f turned;
    fn_800091EC(&turned, &(const Vec3f &)fn_8040CDD0(&chara->m_position, &offset));
    Vec3f back;
    fn_800091EC(&back, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&turned, &mode->m_target));
    fn_8040CE74(&back);
    fn_8040CE38(&back, distance);
    Vec3f result;
    fn_800091EC(&result, &(const Vec3f &)fn_8040CDD0(&turned, &back));
    {
        boost::shared_ptr<CollisionResult> blocked(fn_801C7C70(&eye, &result));
        if (blocked.get() != 0) {
            fn_800091D0(&result, fn_8001E9BC(blocked->m_object));
        }
    }
    fn_8000A5D4(mode, &result, &mode->m_target);
    return true;
}

/* Whether a sphere of radius moved from one point to another hits the stage. */
bool fn_801C7920(const Vec3f *from, const Vec3f *to, float radius) {
    if (*from != *to) {
        UnkSphere_80021C04 sphere;
        fn_80021C04(&sphere, from, radius);
        Vec3f move;
        fn_800091EC(&move, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(to, from));
        boost::shared_ptr<CollisionResultList> hits(
            fn_80021934(fn_80020D40(), &sphere, &move, 3, 0xD, 0, fn_801C7D20, 0, 0));
        if (hits.get() != 0) {
            return true;
        }
    }
    return false;
}

/* Moves a sphere of radius from one point towards another; where it hits the stage, slides it
 * along the surfaces hit. Stores where it ends in result; whether it hit anything. */
bool fn_801C72B8(const Vec3f *from, const Vec3f *to, Vec3f *result, float radius) {
    if (*from != *to) {
        UnkSphere_80021C04 sphere;
        fn_80021C04(&sphere, from, radius);
        Vec3f move;
        fn_800091EC(&move, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(to, from));
        boost::shared_ptr<CollisionResultList> hits(
            fn_80021934(fn_80020D40(), &sphere, &move, 3, 0xD, 0, fn_801C7D20, 0, 0));
        if (hits.get() != 0) {
            void *object = fn_8001EE04(fn_800CFDE4(&hits))->m_object;
            Vec3f contact;
            fn_800091EC(&contact, fn_8001E9BC(object));
            const Vec3f *normal = fn_800266D4(object);
            Vec3f back;
            fn_800091EC(&back, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(from, to));
            float distance = fn_8040CF50(&back);
            fn_8040CE74(&back);
            Vec3f side;
            fn_800091EC(&side, &(const Vec3f &)fn_8040CFB8(normal, &back));
            fn_8040CE74(&side);
            Vec3f slide;
            fn_800091EC(&slide, &(const Vec3f &)fn_8040CFB8(normal, &side));
            fn_8040CE74(&slide);
            Vec3f position;
            fn_800091EC(&position, &(const Vec3f &)fn_8040CDD0(from, &move));
            Vec3f push(0.0f, 0.0f, 0.0f);
            float count = 0.0f;
            for (UnkHitIter_80027374 it(fn_8001EE04(fn_800CFDE4(&hits)));
                 it.m_node != fn_8001EDEC(fn_800CFDE4(&hits)); fn_80027374(&it, 0)) {
                void *hitObject = it.m_node->m_object;
                Vec3f hitNormal;
                fn_800091EC(&hitNormal, fn_800266D4(hitObject));
                fn_8040CFF8(&position, &(const Vec3f &)fn_800091A4(&hitNormal, 0.0002f));
                if (fn_80099080(hitObject) == 1) {
                    Vec3f offset;
                    fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                             &position, fn_8001E9BC(hitObject)));
                    float depth = fn_8040CF90(&offset, &hitNormal);
                    if (depth >= 0.0f && fn_8040CF2C(&offset) < radius * radius) {
                        Vec3f out;
                        fn_800091EC(&out, &(const Vec3f &)fn_8040CDA8(&hitNormal, radius - depth));
                        fn_8040CFF8(&position, &out);
                        fn_8040CFF8(&push, &out);
                        count += 1.0f;
                    }
                }
            }
            if (count > 1.0f) {
                Vec3f average;
                fn_800091EC(&average, &(const Vec3f &)fn_8040CDA8(&push, 1.0f / count));
                fn_8040CE04(&position, &average);
            }
            Vec3f end;
            fn_800091EC(&end, &(const Vec3f &)fn_8040CDD0(
                                  &position, &(const Vec3f &)fn_8040CDA8(
                                                 &slide, distance - fn_8040CF50(&move))));
            UnkLine_80020A20 line;
            fn_80020A20(&line, &position,
                        &(const Vec3f &)fn_8040CDD0(
                            &end, &(const Vec3f &)fn_8040CDA8(&slide, sphere.radius)));
            boost::shared_ptr<CollisionResult> hit(
                fn_80021E24(fn_80020D40(), &line, 3, 0xD, fn_801C7D20, 0));
            if (hit.get() != 0) {
                void *hitObject = hit->m_object;
                fn_800091D0(result, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                        fn_8001E9BC(hitObject),
                                        &(const Vec3f &)fn_8040CDA8(&slide, sphere.radius)));
            } else {
                fn_800091D0(result, &end);
            }
            return true;
        }
    }
    fn_800091D0(result, to);
    return false;
}

} // extern "C"
