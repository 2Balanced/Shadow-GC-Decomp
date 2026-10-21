/* The collision move of the vehicles (file and class names unknown): sweeps a vehicle's sphere
 * through the collision world, slides it along what it touches, keeps it out of the ground and
 * records the ground it stands on (UnkVehicleColliMove_8033377C), and the record of what a mover
 * stands on (UnkColliMoveContact_803334A4; Vehicle::CharaColliMove and the player's collision move
 * use it too). Names marked "(name inferred)" are not original. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first).
 *
 * NonMatching: every function but three matches, and all data matches. The three differ only in
 * register allocation and scheduling, which no source shape tried changed (GC/1.3.2 to 2.7 give
 * the same code):
 * - Sweep (0x80333B1C): retail gives the locals the high saved registers and the parameters
 *   r15-r21; GC/2.7 gives the parameters the high ones.
 * - Settle (0x803337EC): retail keeps &filter in a register across both loops (and colours the
 *   locals as in Sweep); GC/2.7 recomputes it.
 * - AcceptHit (0x80334314): retail schedules two loads above stack stores (above the stmw, and
 *   above the iterator copy); GC/2.7 keeps them below. The same load-above-store pattern blocks
 *   fn_80334480 in Game/VehiclePathMove.cpp. */
#define BOOST_SP_NOT_EQUAL
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/GeometryLine.h"
#include "Game/TVehicleBase.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Sonicteam::System::Geometry::Line;
using SonicteamUSA::System::CharaColli::CharaColliBase;
using SonicteamUSA::System::CharaColli::CharaColliBaseCharInfo;
using Vehicle::TVehicleBase;

namespace std {
float fabs(float x);
} // namespace std

/* fn_8040CD74 called as returning a - b by value (see ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* vector = value through the out-of-line assignment (see Game/GadgetUtil.h), and the
 * out-of-line copy constructor. */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}
inline void CopyVector(Vec3f &vector, const Vec3f &value) {
    fn_800091EC(&vector, &value);
}

/* A vector copied through the out-of-line vector copy constructor (fn_800091EC; see
 * Game/WeaponBullet.h, whose Fn8040CopyVector3 this mirrors). */
struct UnkCopyVector3_800091EC {
    UnkCopyVector3_800091EC() {}
    UnkCopyVector3_800091EC(const UnkCopyVector3_800091EC &v) {
        fn_800091EC(reinterpret_cast<Vec3f *>(this), reinterpret_cast<const Vec3f *>(&v));
    }

    float x;
    float y;
    float z;
};

/* A collision object of the world (class name unknown): its position at 0x4, and its rotation
 * (slot 0x08; name inferred). */
class UnkColliObject_80028CA4 {
  public:
    virtual Vec3f GetRotation();

    /* 0x04 */ Vec3f m_position;
};

/* A length (name inferred). As a call it is not a constant expression, so the constants below are
 * stored by a static initializer, as in retail (see CityBombBig.cpp). */
inline float Span(float length) {
    return length;
}

/* How far the sweep pushes away from what it touched, and the radius of the vehicle's sphere
 * (names inferred). */
static float s_skin = Span(0.020000001f);
static float s_radius = Span(7.0f);

/* A list of colliders (struct name unknown; a std::list<boost::shared_ptr<CharaColliBase> > of the
 * game's library, whose instances are weak functions of the player's collision move around
 * 0x80095000). A node holds the value at 0x8. */
struct UnkColliListNode_80095198 {
    /* 0x00 */ UnkColliListNode_80095198 *m_next;
    /* 0x04 */ UnkColliListNode_80095198 *m_prev;
    /* 0x08 */ boost::shared_ptr<CharaColliBase> m_value;
};

struct UnkColliListIter_80095198 {
    UnkColliListIter_80095198(const UnkColliListIter_80095198 &other) : m_node(other.m_node) {}

    UnkColliListNode_80095198 *m_node;
};

struct UnkColliListConstIter_80097DA0 {
    UnkColliListConstIter_80097DA0(const UnkColliListConstIter_80097DA0 &other) : m_node(other.m_node) {}

    bool operator!=(const UnkColliListConstIter_80097DA0 &other) const {
        return m_node != other.m_node;
    }
    const boost::shared_ptr<CharaColliBase> &operator*() const {
        return m_node->m_value;
    }

    UnkColliListNode_80095198 *m_node;
};

/* The list's input iterator (struct name unknown). */
struct UnkColliListInputIter_803342C4 {
    UnkColliListInputIter_803342C4(const UnkColliListConstIter_80097DA0 &it) : m_node(it.m_node) {}
    UnkColliListInputIter_803342C4(const UnkColliListInputIter_803342C4 &it) : m_node(it.m_node) {}

    UnkColliListNode_80095198 *m_node;
};

/* The empty iterator-category argument of the list's range insertion (struct name unknown). */
struct UnkIterTag_80099328 {};

struct UnkColliList_80095314;

extern "C" {
/* The list's constructor and destructor, push_back, end, const begin and end, the const iterator's
 * post-increment, and the range insertion behind insert (behaviour inferred). */
void fn_80095314(UnkColliList_80095314 *list);
void fn_800951D4(UnkColliList_80095314 *list, int flag);
void fn_8009514C(UnkColliList_80095314 *list, const boost::shared_ptr<CharaColliBase> *value);
UnkColliListIter_80095198 fn_80095198(UnkColliList_80095314 *list);
UnkColliListConstIter_80097DA0 fn_80097DA0(const UnkColliList_80095314 *list);
UnkColliListConstIter_80097DA0 fn_80097D64(const UnkColliList_80095314 *list);
UnkColliListConstIter_80097DA0 fn_80099230(UnkColliListConstIter_80097DA0 *it, int);
void fn_80099328(UnkColliList_80095314 *list, UnkColliListIter_80095198 position,
                 UnkColliListInputIter_803342C4 first, UnkColliListInputIter_803342C4 last,
                 UnkIterTag_80099328 tag);
}

struct UnkColliList_80095314 {
    UnkColliListIter_80095198 end() {
        return fn_80095198(this);
    }
    void push_back(const boost::shared_ptr<CharaColliBase> &value) {
        fn_8009514C(this, &value);
    }
    /* A weak function of this unit (0x803342C4). */
    void insert(UnkColliListIter_80095198 position, UnkColliListInputIter_803342C4 first,
                UnkColliListInputIter_803342C4 last) {
        fn_80099328(this, position, first, last, UnkIterTag_80099328());
    }

    /* 0x00 */ u8 field_0x0[0xC];
};

/* What the sweep's filter skips and accepts (struct name unknown; a weak constructor and
 * destructor of this unit, 0x80333AE4 and 0x80333A88): hits on the colliders of m_owner and on
 * the colliders in m_ignore are skipped; other hits count when their attributes share a bit with
 * m_mask (names inferred). */
struct UnkColliMoveFilter_80333AE4 {
    UnkColliMoveFilter_80333AE4() {
        fn_80095314(&m_ignore);
    }
    ~UnkColliMoveFilter_80333AE4() {
        fn_800951D4(&m_ignore, -1);
    }

    /* 0x00 */ boost::weak_ptr<CharInfo> m_owner;
    /* 0x08 */ u32 m_mask;
    /* 0x0C */ UnkColliList_80095314 m_ignore;
};
SIZE_ASSERT(UnkColliMoveFilter_80333AE4, 0x18);

/* What the sweep passes its filter for each candidate hit (see MeteorsHolder.cpp). */
struct UnkCollisionCandidate_80021934 {
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ CharaColliBase *m_collider;
    /* 0x0C */ u32 m_attributes;
    /* 0x10 */ const UnkColliMoveFilter_80333AE4 *m_filter;
};

typedef bool (*UnkCollisionFilter_80021934)(UnkCollisionCandidate_80021934 *candidate);

/* A sphere: centre and radius (built by fn_80021C04; see MeteorsHolder.cpp). */
struct UnkSphere_80021C04 {
    /* 0x00 */ float center[3];
    /* 0x0C */ float radius;
};

/* A line segment set up by fn_80020A20 (0x18 bytes; see DiablonPartsBeam.cpp). */
struct UnkLine_80020A20 {
    u8 field_0x0[0x18];
};

/* A node of the sweep's hits (see MeteorsHolder.cpp): its collision object at 0xC and the world
 * object at 0x14 (names inferred). */
struct UnkColliContactNode_800587A0;
extern "C" u32 fn_80026588(UnkColliContactNode_800587A0 *node);
struct UnkColliContactNode_800587A0 {
    /* The hit's attributes and its kind, the low 28 and the high 4 bits of its flags (names
     * inferred). */
    u32 GetAttributes() {
        return fn_80026588(this) & 0x0FFFFFFF;
    }
    u32 GetKind() {
        return fn_80026588(this) >> 28;
    }

    /* 0x00 */ u8 field_0x0[0xC];
    /* 0x0C */ UnkColliObject_80028CA4 *m_collider;
    /* 0x10 */ u8 field_0x10[0x14 - 0x10];
    /* 0x14 */ void *m_object;
};

/* An iterator over the hits (layout from fn_8001EDFC; see BlockChange.cpp). */
struct UnkColliContactIter_8001EDFC {
    UnkColliContactIter_8001EDFC(UnkColliContactNode_800587A0 *node) : m_node(node) {}

    UnkColliContactNode_800587A0 *m_node;
};

/* The hits of a sweep (see MeteorsHolder.cpp), and the hit list a line test fills (constructor
 * 0x800270CC; struct name unknown). */
struct CollisionResultList;
struct UnkHitList_800270CC;

extern "C" {
UnkHitList_800270CC *fn_800270CC(UnkHitList_800270CC *list);
}

struct UnkHitList_800270CC {
    UnkHitList_800270CC() {
        fn_800270CC(this);
    }
    ~UnkHitList_800270CC();

    /* 0x00 */ u8 field_0x0[0x1C];
};

/* The first hit of a line test (see DiablonPartsBeam.cpp). */
struct UnkLineHit_8002735C {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ void *m_object;
};

/* The vehicle as the collision move calls it: TVehicleBase's slot 0x40 takes the hit the vehicle
 * touched (Vehicle/TVehicleBase.h declares the slot without its argument; meaning inferred). */
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
/* The collision world, a sphere from a centre and a radius, and the sweep of a sphere along a
 * velocity; it returns the hits that filter accepts, or null (see MeteorsHolder.cpp). */
void *fn_80020D40(void);
void fn_80021C04(UnkSphere_80021C04 *sphere, const Vec3f *center, float radius);
CollisionResultList *fn_80021934(void *world, const UnkSphere_80021C04 *sphere, const Vec3f *velocity,
                                 int arg3, int arg4, CollisionResultList *previous,
                                 UnkCollisionFilter_80021934 filter, const UnkColliMoveFilter_80333AE4 *self,
                                 int arg8);
/* The hits of a sphere and of a line against the world (behaviour inferred). */
CollisionResultList *fn_80021CA4(void *world, const UnkSphere_80021C04 *sphere, int arg2, int arg3,
                                 UnkCollisionFilter_80021934 filter, const UnkColliMoveFilter_80333AE4 *self,
                                 UnkHitList_800270CC *list);
CollisionResultList *fn_80021E24(void *world, const Line *line, int arg2, int arg3,
                                 UnkCollisionFilter_80021934 filter, const UnkColliMoveFilter_80333AE4 *self);
void fn_800265A4(CollisionResultList *hits, int arg1);
/* A debug line from start to end (see DiablonPartsBeam.cpp), and whether it is drawn (behaviour
 * inferred). */
void *fn_80020A20(UnkLine_80020A20 *line, const Vec3f *start, const Vec3f *end);
bool fn_801BFB70(void *line);
/* The first hit, the release of a hit list, and the hits as a list (see DiablonPartsBeam.cpp). */
UnkLineHit_8002735C *fn_8002735C(CollisionResultList *hits);
void fn_80026FB4(CollisionResultList *hits, int arg1);
void fn_80026A40(UnkHitList_800270CC *list);
UnkColliContactNode_800587A0 *fn_8001EE04(void *hits);
UnkColliContactNode_800587A0 *fn_8001EDEC(void *hits);
UnkColliContactIter_8001EDFC fn_80027374(UnkColliContactIter_8001EDFC *it, int);
/* A world object's position and normal, its distance (behaviour inferred) and its kind. */
const Vec3f *fn_8001E9BC(void *object);
const Vec3f *fn_800266D4(void *object);
float fn_80097D0C(void *object);
int fn_80099080(void *object);
/* A hit's attributes and its collider (see BlockChange.cpp). */
u32 fn_80026588(UnkColliContactNode_800587A0 *node);
boost::weak_ptr<CharaColliBase> fn_800587A0(UnkColliContactNode_800587A0 *node);
/* The character info a collider is attached to (a weak member function of CharaColliBaseCharInfo;
 * behaviour from its body). */
boost::weak_ptr<CharInfo> fn_800586EC(CharaColliBaseCharInfo *collider);
/* A collision object's update and its kind (behaviour inferred). */
void fn_80028CA4(UnkColliObject_80028CA4 *object);
u32 fn_80028ECC(UnkColliObject_80028CA4 *object);

/* The vehicle's body and second collider, its ground hooks (behaviour inferred). */
boost::shared_ptr<CharaColliBase> fn_801B6350(TVehicleBase *vehicle);
boost::shared_ptr<CharaColliBase> fn_801B6380(TVehicleBase *vehicle);
void fn_801B6184(TVehicleBase *vehicle);

/* A 32-bit flag word (see ArkCannon.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* Vector helpers (behaviour inferred from their bodies; see ArkCannon.cpp and Bomb.cpp). */
Vec3f fn_800091A4(const Vec3f *vector, float scale);
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
void fn_8040CE04(Vec3f *vector, const Vec3f *value); /* -= */
void fn_8040CE60(Vec3f *vector);
float fn_8040CE74(Vec3f *vector);
Vec3f fn_8040CEE8(const Vec3f *vector);
float fn_8040CF90(const Vec3f *a, const Vec3f *b);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* += */
void fn_8040F990(Vec3f *along, Vec3f *across, const Vec3f *vector,
                 const Vec3f *normal);
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

/* What a mover stands on (struct name unknown; constructor 0x803334A4): a character (m_chara)
 * or, when m_isCollider is set, a collider (m_collider) and its collision object (names
 * inferred). */
struct UnkColliMoveContact_803334A4 {
    UnkColliMoveContact_803334A4();
    ~UnkColliMoveContact_803334A4();

    bool IsValid() const;
    void SetCharInfo(const boost::weak_ptr<CharInfo> &charInfo);
    void Set(const boost::weak_ptr<CharInfo> &charInfo, const boost::weak_ptr<CharaColliBase> &collider,
             UnkColliObject_80028CA4 *object);
    void Reset();

    /* 0x00 */ u8 m_isCollider;
    /* 0x04 */ boost::weak_ptr<CharInfo> m_chara;
    /* 0x0C */ boost::weak_ptr<CharaColliBase> m_collider;
    /* 0x14 */ UnkColliObject_80028CA4 *m_object;
};
SIZE_ASSERT(UnkColliMoveContact_803334A4, 0x18);

/* The placement taken from a contact (struct name unknown; names inferred). */
struct UnkColliMovePlacement_80333098 {
    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ Vec3f m_rotation;
    /* 0x18 */ u32 m_kind;
};

/* The ground tracker of the collision move (struct name unknown; constructor 0x80344608,
 * destructor 0x8034454C). */
struct UnkGroundTracker_80344608;

extern "C" {
void fn_80344608(UnkGroundTracker_80344608 *tracker);
void fn_8034454C(UnkGroundTracker_80344608 *tracker, int flag);
void fn_80344528(UnkGroundTracker_80344608 *tracker);
void fn_803444E0(UnkGroundTracker_80344608 *tracker, const Vec3f *position,
                 const Vec3f *normal, u32 attributes, u32 kind);
void fn_801B61B0(TVehicleBase *vehicle, const UnkColliMoveContact_803334A4 *contact);
}

struct UnkGroundTracker_80344608 {
    /* 0x00 */ u32 field_0x0;
};

/* The collision move of a vehicle (class name unknown; constructor 0x8033377C). m_flags: bit 0 on
 * the ground, bit 1 the debug line is drawn, bit 2 just landed (meanings inferred). */
class UnkVehicleColliMove_8033377C {
  public:
    UnkVehicleColliMove_8033377C(TVehicleBase *vehicle);
    ~UnkVehicleColliMove_8033377C();

    bool Move(Vec3f *position, Vec3f *velocity, bool ground, UnkColliList_80095314 *ignore);
    void SetTrackGround(u8 track);
    void ResetGround();
    void ClearOnGround();
    bool IsLanding();
    bool IsOnGround();
    bool IsLineDrawn();

    /* 0x00 */ Vec3f m_velocity;
    /* 0x0C */ Vec3f m_normal;
    /* 0x18 */ TVehicleBase *m_vehicle;
    /* 0x1C */ u32 m_flags;
    /* 0x20 */ UnkGroundTracker_80344608 m_ground;
    /* 0x24 */ u8 m_trackGround;
};
SIZE_ASSERT(UnkVehicleColliMove_8033377C, 0x28);

/* Accepts a candidate hit of the sweep (name inferred). */
static bool AcceptHit(UnkCollisionCandidate_80021934 *candidate) {
    const UnkColliMoveFilter_80333AE4 *filter = candidate->m_filter;
    if (candidate->m_collider != 0) {
        if (!filter->m_owner.expired()) {
            CharaColliBaseCharInfo *charCollider = dynamic_cast<CharaColliBaseCharInfo *>(candidate->m_collider);
            if (charCollider != 0 && fn_800586EC(charCollider).lock() == filter->m_owner.lock()) {
                return false;
            }
        }
        UnkColliListConstIter_80097DA0 it = fn_80097DA0(&filter->m_ignore);
        CharaColliBase *collider = candidate->m_collider;
        for (; it != fn_80097D64(&filter->m_ignore); fn_80099230(&it, 0)) {
            if (collider == (*it).get()) {
                return false;
            }
        }
    }
    return (candidate->m_attributes & filter->m_mask) != 0;
}

/* Sweeps the sphere around position along velocity through the world, sliding along what it
 * touches; returns whether it stands on the ground (name inferred). */
static bool Sweep(Vec3f *normal, TVehicleBase *vehicle, Vec3f *position, Vec3f *velocity,
                  UnkGroundTracker_80344608 *ground, UnkColliMoveContact_803334A4 *contact,
                  const UnkColliList_80095314 *ignore, int count) {
    void *world = fn_80020D40();
    CollisionResultList *previous = 0;
    UnkSphere_80021C04 sphere;
    fn_80021C04(&sphere, position, s_radius);
    Vec3f move;
    fn_800091EC(&move, velocity);
    int mode = 1;
    u32 attributes = 0;
    u32 kind = 0;
    UnkColliMoveFilter_80333AE4 filter;
    filter.m_mask = 0x3A93;
    if (vehicle != 0) {
        boost::shared_ptr<CharaColliBase> collider;
        collider = fn_801B6350(vehicle);
        if (collider.get() != 0) {
            filter.m_ignore.push_back(collider);
        }
        collider = fn_801B6380(vehicle);
        if (collider.get() != 0) {
            filter.m_ignore.push_back(collider);
        }
        if (ignore != 0) {
            filter.m_ignore.insert(filter.m_ignore.end(), fn_80097DA0(ignore), fn_80097D64(ignore));
        }
        filter.m_owner = vehicle->GetRider();
        if (fn_80014E34(&vehicle->m_vehicleFlags, 0)) {
            mode = 2;
        }
    }
    float grounds = 0.0f;
    bool onGround = false;
    Vec3f groundPosition;
    CollisionResultList *hits;
    bool again;
    do {
        again = false;
        Vec3f start;
        fn_800091EC(&start, &move);
        hits = fn_80021934(world, &sphere, &move, 3, mode, previous, AcceptHit, &filter, 0);
        if (previous != 0) {
            fn_80026FB4(previous, 1);
            previous = 0;
        }
        fn_8040CFF8(reinterpret_cast<Vec3f *>(sphere.center), &move);
        if (hits != 0) {
            if (contact != 0) {
                contact->Reset();
            }
            fn_8040CE60(normal);
            grounds = 0.0f;
            Vec3f remain;
            fn_800091EC(&remain, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&start, &move));
            Vec3f push(0.0f, 0.0f, 0.0f);
            float pushes = 0.0f;
            UnkColliContactNode_800587A0 *node;
            for (UnkColliContactIter_8001EDFC it(fn_8001EE04(hits)); (node = it.m_node) != fn_8001EDEC(hits);
                 fn_80027374(&it, 0)) {
                void *object = node->m_object;
                Vec3f hitNormal;
                fn_800091EC(&hitNormal, fn_800266D4(object));
                if (fn_8040CF90(&hitNormal, &remain) <= 0.0f) {
                    SubVector(remain, fn_80093230(&hitNormal));
                }
                if (fn_8040CF90(&hitNormal, velocity) <= 0.0f) {
                    Vec3f along;
                    Vec3f across;
                    fn_8040F990(&along, &across, velocity, &hitNormal);
                    if (vehicle != 0) {
                        UnkColliContactIter_8001EDFC hit(node);
                        reinterpret_cast<UnkVehicleHitView_80333B1C *>(vehicle)->OnHit(hit);
                    }
                    if (!(across.x * across.x + across.y * across.y + across.z * across.z < 0.0001f) &&
                        fn_8040CF90(&across, velocity) <= 0.0f) {
                        fn_8040CE60(velocity);
                    } else {
                        float slope = Dot(hitNormal, Vec3f(0.0f, 1.0f, 0.0f));
                        if (std::fabs(slope) < 0.0871557427f) {
                            slope = Dot(fn_8040CEE8(velocity), fn_8040CEE8(&along));
                            float half = 0.5f * slope;
                            Vec3f slide;
                            CopyVector(slide, fn_8040CDA8(&fn_80016284(&along), 1.0f + half));
                            fn_8040CFF8(velocity, &slide);
                        } else {
                            AssignVector(*velocity, across);
                            if (slope > 0.57357645f) {
                                fn_8040CFF8(normal, &hitNormal);
                                grounds += 1.0f;
                                AssignVector(groundPosition, *fn_8001E9BC(object));
                                attributes = node->GetAttributes();
                                kind = node->GetKind();
                                onGround = true;
                                boost::shared_ptr<CharaColliBase> collider = fn_800587A0(node).lock();
                                boost::weak_ptr<CharInfo> owner;
                                if (collider.get() != 0) {
                                    boost::shared_ptr<CharaColliBaseCharInfo> charCollider =
                                        boost::dynamic_pointer_cast<CharaColliBaseCharInfo>(collider);
                                    owner = fn_800586EC(charCollider.get());
                                    if (contact != 0 && !owner.expired()) {
                                        contact->Set(owner, fn_800587A0(node), node->m_collider);
                                    }
                                }
                            }
                        }
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
    if (grounds >= 1.0f) {
        fn_8040CE74(normal);
        if (ground != 0) {
            fn_803444E0(ground, &groundPosition, normal, attributes, kind);
        }
    } else {
        fn_800091D0(normal, &Vec3f(0.0f, 1.0f, 0.0f));
    }
    AssignVector(*position, reinterpret_cast<Vec3f &>(sphere.center));
    return onGround;
}

/* Moves position out of the world objects within distance below it (name inferred; direction is
 * unused). */
static UnkCopyVector3_800091EC Settle(const Vec3f &position, const Vec3f &direction,
                                      float distance) {
    void *world = fn_80020D40();
    UnkColliMoveFilter_80333AE4 filter;
    filter.m_mask = 0x3A93;
    UnkCopyVector3_800091EC point;
    fn_800091EC(reinterpret_cast<Vec3f *>(&point), &position);
    float reach = distance + 1.0f;
    float lift = distance + s_skin;
    const UnkColliMoveFilter_80333AE4 *self = &filter;
    for (int i = 3; i > 0; i--) {
        UnkHitList_800270CC list;
        UnkSphere_80021C04 sphere;
        fn_80021C04(&sphere, reinterpret_cast<Vec3f *>(&point), reach);
        CollisionResultList *hits = fn_80021CA4(world, &sphere, 1, 2, AcceptHit, &filter, &list);
        if (hits != 0) {
            void *object = fn_8002735C(hits)->m_object;
            if (fn_80097D0C(object) < distance) {
                AssignVector(reinterpret_cast<Vec3f &>(point), fn_8040CDD0(fn_8001E9BC(object),
                                                &fn_8040CDA8(fn_800266D4(object), lift)));
                fn_80026FB4(hits, 1);
                fn_80026A40(&list);
                continue;
            }
            fn_80026FB4(hits, 1);
        }
        bool clear = true;
        UnkColliContactNode_800587A0 *node;
        for (UnkColliContactIter_8001EDFC it(fn_8001EE04(&list)); (node = it.m_node) != fn_8001EDEC(&list);
             fn_80027374(&it, 0)) {
            void *object = node->m_object;
            Line line;
            AssignVector(line.m_start, reinterpret_cast<Vec3f &>(point));
            AssignVector(line.m_end, *fn_8001E9BC(object));
            CollisionResultList *blocked = fn_80021E24(world, &line, 1, 2, AcceptHit, &filter);
            if (blocked != 0) {
                fn_800265A4(blocked, 1);
            } else {
                AssignVector(reinterpret_cast<Vec3f &>(point), fn_8040CDD0(fn_8001E9BC(object),
                                                &fn_8040CDA8(fn_800266D4(object), lift)));
                clear = false;
                break;
            }
        }
        fn_80026A40(&list);
        if (clear) {
            break;
        }
    }
    return point;
}

UnkVehicleColliMove_8033377C::UnkVehicleColliMove_8033377C(TVehicleBase *vehicle) : m_vehicle(vehicle) {
    fn_80014E04(&m_flags);
    fn_80344608(&m_ground);
    fn_8040CE60(&m_velocity);
    fn_800091D0(&m_normal, &Vec3f(0.0f, 1.0f, 0.0f));
    m_trackGround = 0;
}

UnkVehicleColliMove_8033377C::~UnkVehicleColliMove_8033377C() {
    fn_8034454C(&m_ground, -1);
}

void UnkVehicleColliMove_8033377C::ResetGround() {
    fn_80344528(&m_ground);
}

void UnkVehicleColliMove_8033377C::SetTrackGround(u8 track) {
    m_trackGround = track;
}

bool UnkVehicleColliMove_8033377C::Move(Vec3f *position, Vec3f *velocity, bool ground,
                                        UnkColliList_80095314 *ignore) {
    UnkColliMoveContact_803334A4 contact;
    Vec3f previous;
    CopyVector(previous, *position);
    bool onGround = Sweep(&m_normal, m_vehicle, position, velocity, m_trackGround ? &m_ground : 0, &contact,
                          ignore, 8);
    fn_8006127C(&m_flags, 2);
    if (ground) {
        if (onGround) {
            if (!fn_80014E34(&m_flags, 0)) {
                fn_80014E1C(&m_flags, 2);
            }
            fn_80014E1C(&m_flags, 0);
            fn_801B61B0(m_vehicle, &contact);
        } else {
            fn_8006127C(&m_flags, 0);
            fn_801B6184(m_vehicle);
        }
        fn_8006127C(&m_flags, 1);
        UnkLine_80020A20 line;
        if (fn_801BFB70(fn_80020A20(&line, &previous, position))) {
            fn_80014E1C(&m_flags, 1);
        }
    }
    fn_800091D0(position, reinterpret_cast<const Vec3f *>(&Settle(*position, Vec3f(0.0f, 1.0f, 0.0f), s_radius)));
    return onGround;
}

void UnkVehicleColliMove_8033377C::ClearOnGround() {
    fn_8006127C(&m_flags, 0);
}

bool UnkVehicleColliMove_8033377C::IsLanding() {
    return fn_80014E34(&m_flags, 2);
}

bool UnkVehicleColliMove_8033377C::IsOnGround() {
    return fn_80014E34(&m_flags, 0);
}

bool UnkVehicleColliMove_8033377C::IsLineDrawn() {
    return fn_80014E34(&m_flags, 1);
}

UnkColliMoveContact_803334A4::UnkColliMoveContact_803334A4() : m_isCollider(0) {
    m_object = 0;
}

UnkColliMoveContact_803334A4::~UnkColliMoveContact_803334A4() {}

bool UnkColliMoveContact_803334A4::IsValid() const {
    if (m_isCollider && m_collider.expired()) {
        return false;
    }
    return !m_chara.expired();
}

void UnkColliMoveContact_803334A4::SetCharInfo(const boost::weak_ptr<CharInfo> &charInfo) {
    m_chara = charInfo;
    m_collider.reset();
    m_object = 0;
    m_isCollider = 0;
}

void UnkColliMoveContact_803334A4::Set(const boost::weak_ptr<CharInfo> &charInfo,
                                       const boost::weak_ptr<CharaColliBase> &collider,
                                       UnkColliObject_80028CA4 *object) {
    m_chara = charInfo;
    m_collider = collider;
    m_object = object;
    m_isCollider = 1;
}

void UnkColliMoveContact_803334A4::Reset() {
    m_chara.reset();
    m_object = 0;
    m_isCollider = 1;
}

/* Whether two contacts are the same (name inferred). */
bool operator==(const UnkColliMoveContact_803334A4 &a, const UnkColliMoveContact_803334A4 &b) {
    if (a.m_chara.lock() != b.m_chara.lock()) {
        return false;
    }
    if (a.m_isCollider != b.m_isCollider) {
        return false;
    }
    if (a.m_isCollider) {
        if (a.m_collider.lock() != b.m_collider.lock()) {
            return false;
        }
        if (a.m_object != b.m_object) {
            return false;
        }
    }
    return true;
}

bool operator!=(const UnkColliMoveContact_803334A4 &a, const UnkColliMoveContact_803334A4 &b) {
    return !(a == b);
}

/* Takes the placement of what contact stands on (name inferred). */
extern "C" void fn_80333098(UnkColliMovePlacement_80333098 *placement, const UnkColliMoveContact_803334A4 *contact) {
    if (contact->m_isCollider) {
        if (!contact->m_collider.expired()) {
            fn_80028CA4(contact->m_object);
            AssignVector(placement->m_position, contact->m_object->m_position);
            placement->m_rotation = contact->m_object->GetRotation();
            placement->m_kind = fn_80028ECC(contact->m_object);
        }
    } else if (!contact->m_chara.expired()) {
        boost::shared_ptr<CharInfo> charInfo = contact->m_chara.lock();
        fn_800091D0(&placement->m_position, &charInfo.get()->m_position);
        placement->m_rotation = charInfo->m_rotation;
        placement->m_kind = charInfo.get()->field_0x30;
    }
}
