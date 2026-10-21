/* ScatteringWeapon (file name from the RTTI of its class, @unnamed@ScatteringWeapon_cpp@::Impl): a
 * weapon thrown out of something (an enemy or a container) that flies under gravity, spinning, and
 * lands on the ground. fn_80221BA8 throws one from a point; fn_80221B08 first moves the start point
 * out of the ground along the line from another point. While it flies the weapon item follows it; a
 * loaded weapon is dropped where it lands, an empty one blinks and disappears. Names marked "(name
 * inferred)" are not original. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include "Game/GeometryLine.h"
#include "Game/Task.h"
#include "Game/VectorDot.h"
#include "Game/WeaponItem.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

using Sonicteam::System::Geometry::Line;

/* A constant vector next to the zero vector (see MathTypes.h; meaning unknown). */
extern const Vec3f lbl_805E07B8;

/* The hits of a line test (see ScatteringRing.cpp; class name unknown). */
struct UnkLineHits_80021FE4;

/* One hit of a line test: the object hit is at 0x14 (layout from the code below; names
 * inferred). */
struct UnkLineHit_8002735C {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ void *m_object;
};

/* What the line test passes its filter for each candidate (layout from the filter). */
struct UnkLineCandidate_80021FE4 {
    /* 0x00 */ u8 field_0x0[0xC];
    /* 0x0C */ u32 m_attributes;
};

extern "C" {
/* The task tree's root lookup and the task layer under it (see Gadget.h). */
void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);

/* The collision world, the line test against it, its first hit, the hit object's position and
 * normal, and the release of the result (behaviour inferred; see ScatteringRing.cpp). */
void *fn_80020D40(void);
UnkLineHits_80021FE4 *fn_80021FE4(void *world, const Line *line, int arg2, int arg3,
                                  bool (*filter)(UnkLineCandidate_80021FE4 *candidate), int arg5);
UnkLineHit_8002735C *fn_8002735C(UnkLineHits_80021FE4 *hits);
const Vec3f *fn_8001E9BC(void *object);
const Vec3f *fn_800266D4(void *object);
void fn_80026FB4(UnkLineHits_80021FE4 *hits, int arg1);
/* Scales a distance by the current frame rate (behaviour inferred). */
float fn_8005AE20(float value);

/* Weapon items (behaviour inferred; see SetWeapon.cpp): makes an item blink, places it, and
 * computes where an item dropped at position lands. */
void fn_8005F09C(Weapon::Item *item);
void fn_8005F274(Weapon::Item *item, const Vec3f *position, const Vec3f *rotation);
void fn_8005F0F4(Weapon::Item *item, Vec3f *outPosition, Vec3f *outRotation,
                 const Vec3f *position, const Vec3f *direction);
/* Drops item at position, facing along direction (see SetWeapon.cpp). */
void fn_8011E630(const boost::shared_ptr<Weapon::Item> &item, const Vec3f *position,
                 const Vec3f *direction);
/* Moves a sphere from position along delta through the collision world; on a hit it stores the
 * contact and its normal and sets bit 0 of flags (behaviour inferred). */
void fn_801BFC68(Vec3f *hitPosition, Vec3f *hitNormal, int arg2, Vec3f *position,
                 const Vec3f *delta, u32 *flags, int mask, float radius);

/* The flag word: fn_80014E04 constructs it cleared and fn_80014E34 tests bit (see SetWeapon.cpp
 * and ArkCannon.cpp). */
u32 *fn_80014E04(u32 *flags);
bool fn_80014E34(u32 *flags, int bit);

/* Vector helpers (behaviour inferred from their bodies). */
Vec3f fn_800091A4(const Vec3f &vector, float scale);       /* vector * scale */
Vec3f fn_80093204(const Vec3f &a, const Vec3f &b); /* a + b */
Vec3f fn_8040CEE8(const Vec3f &vector);                    /* normalised */
Vec3f fn_8040CDA8(const Vec3f &vector, float scale);       /* vector * scale */
Vec3f fn_8040CDD0(const Vec3f &a, const Vec3f &b); /* a + b */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);       /* vector += value */
void fn_8040CE04(Vec3f *vector, const Vec3f *value);       /* vector -= value */
float fn_8040CE74(Vec3f *vector); /* normalises; returns the former length */
void fn_8040F990(Vec3f *along, Vec3f *across, const Vec3f *vector,
                 const Vec3f *normal);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
/* The rotation matrix of Euler angles in the given axis order, applied to matrix. */
Mtx34f fn_8040FFBC(const Mtx34f *matrix, const Vec3f *angles, int order);
/* Interpolates from rotation a towards b by t (behaviour inferred). */
Vec3f fn_8040F16C(const Vec3f *a, const Vec3f *b, int order, float t);
/* Converts a direction into two angles, and two angles into a rotation (behaviour inferred). */
Vec2f fn_8040F760(const Vec3f &direction);
Vec3f fn_800576CC(const Vec2f &angles);
/* The smaller of two values. */
const float &fn_800704AC(const float &a, const float &b);
}

/* fn_8040CD74 called as returning a - b by value (see ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f &a, const Vec3f &b);

/* vector = value through the out-of-line assignment (see Game/GadgetUtil.h). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* vector += value (name inferred; see Bomb.cpp). */
inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

/* vector -= value (name inferred). */
inline void SubtractVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CE04(&vector, &value);
}

/* Returns x unchanged (name inferred). Being initialised through a function call, the value below
 * is set up at startup (see Meteor.cpp). */
inline float Plain(float x) {
    return x;
}

/* The gravity of a thrown weapon (meaning inferred). */
static const float lbl_805EFDC0 = Plain(-60.0f);

namespace {

/* The flag word at 0x30 (struct name unknown). */
struct ImplFlags {
    ImplFlags() {
        fn_80014E04(&m_bits);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits; /* bit 0: landed */
};

class Impl : public Task {
  public:
    Impl(Task *parent, const Vec3f *position, const Vec3f *direction,
         const Vec3f *velocity, int type, int arg);
    virtual ~Impl();
    virtual void Tick(float dt);

    /* Drops the weapon where it landed (name inferred). */
    void Drop();
    /* Whether the weapon has flown for too long (name inferred). */
    bool IsExpired() {
        return m_time > 10.0f;
    }
    /* An unused out-of-line copy of IsExpired (name inferred; see its definition). */
    bool IsExpiredOutOfLine();
    /* Turns the weapon towards its spin (name inferred). */
    void AdvanceSpin(float dt);
    /* Moves the weapon under gravity and tests what it hits (name inferred). */
    void UpdateMotion(float dt);

    /* Member names are inferred. */
    /* 0x28 */ boost::shared_ptr<Weapon::Item> m_item;
    /* 0x30 */ ImplFlags m_status;
    /* 0x34 */ Vec3f m_hitPosition;
    /* 0x40 */ Vec3f m_hitNormal;
    /* 0x4C */ Vec3f m_position;
    /* 0x58 */ Vec3f m_velocity;
    /* 0x64 */ Vec3f m_rotation;
    /* 0x70 */ Vec3f m_spin;
    /* 0x7C */ float m_time;
};
SIZE_ASSERT(Impl, 0x80);

Impl::Impl(Task *parent, const Vec3f *position, const Vec3f *direction,
           const Vec3f *velocity, int type, int arg)
    : Task(parent), m_rotation(0.0f, 0.0f, 0.0f), m_spin(0.0f, 0.0f, 0.0f) {
    m_name = "ScatteringWeapon";
    m_status.Test(1); /* its result is unused in the original too */
    AssignVector(m_velocity, *velocity);
    m_time = 0.0f;
    m_item = fn_8005B9D4(type, arg);
    if (m_item.get() == 0) {
        m_flags |= 1;
        return;
    }
    fn_8005F274(m_item.get(), &lbl_805E07A0, &lbl_805E07B8);
    fn_8005F0F4(m_item.get(), &m_position, &m_rotation, position, direction);
    fn_8005F274(m_item.get(), &m_position, &m_rotation);
    Mtx34f matrix;
    fn_8040C148(&matrix, &(const Mtx34f &)fn_8040FFBC(&lbl_805E0720, &m_rotation, 2));
    Vec3f axis;
    fn_800091EC(&axis, &(const Vec3f &)fn_8040B67C(
                           &matrix, &(const Vec3f &)Vec3f(-1.0f, 0.0f, 0.0f)));
    fn_8040F990(0, &axis, &axis, &(const Vec3f &)Vec3f(0.0f, 1.0f, 0.0f));
    if (axis.x * axis.x + axis.y * axis.y + axis.z * axis.z < 0.0001f) {
        fn_800091D0(&axis, &(const Vec3f &)Vec3f(1.0f, 0.0f, 0.0f));
    } else {
        fn_8040CE74(&axis);
    }
    m_spin = fn_800576CC(fn_8040F760(axis));
}

Impl::~Impl() {}

void Impl::Tick(float dt) {
    m_time += dt;
    UpdateMotion(dt);
    AdvanceSpin(dt);
    if (m_item->m_ammo == 0) {
        if (m_time > 0.4f && m_item.get() != 0) {
            fn_8005F09C(m_item.get());
        }
        if (m_status.Test(0) || m_time > 0.8f) {
            m_flags |= 1;
        }
    } else if (m_status.Test(0)) {
        Drop();
        m_flags |= 1;
    }
    if (IsExpired()) {
        m_flags |= 1;
    }
    if (m_item.get() != 0) {
        fn_8005F274(m_item.get(), &m_position, &m_rotation);
        m_item->Tick(dt);
    }
}

void Impl::UpdateMotion(float dt) {
    Vec3f gravity(0.0f, lbl_805EFDC0, 0.0f);
    Vec3f previous;
    fn_800091EC(&previous, &m_position);
    AddVector(m_position, fn_80093204(fn_800091A4(m_velocity, dt),
                                      fn_800091A4(fn_800091A4(fn_800091A4(gravity, 0.5f), dt), dt)));
    AddVector(m_velocity, fn_800091A4(gravity, dt));
    Vec3f delta;
    fn_800091EC(&delta, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(m_position, previous));
    fn_801BFC68(&m_hitPosition, &m_hitNormal, 0, &m_position, &delta, &m_status.m_bits, 0x2000, 5.0f);
}

void Impl::AdvanceSpin(float dt) {
    float t = 4.0f * dt;
    t = fn_800704AC(t, 1.0f);
    m_rotation = fn_8040F16C(&m_rotation, &m_spin, 2, t);
}

/* Not called anywhere: the linker strips it from the game. The original pools IsExpired's float
 * literal between those of PushOutOfGround and AdvanceSpin, which is what compiling an
 * out-of-line function that inlines it at this point of the file does (see ElecSmallCube.cpp and
 * docs/research-notes.md). */
bool Impl::IsExpiredOutOfLine() {
    return IsExpired();
}

void Impl::Drop() {
    if (m_item->m_ammo != 0) {
        fn_8011E630(m_item, &m_hitPosition, &m_hitNormal);
    }
}

} // namespace

/* Accepts the ground hits of the line test (name and meaning inferred). */
static bool AcceptGround(UnkLineCandidate_80021FE4 *candidate) {
    return (candidate->m_attributes & 0xC0018) == 0;
}

/* Moves position out of the ground along the line from from, and reflects velocity off the ground
 * there (name inferred). */
static void PushOutOfGround(Vec3f *position, Vec3f *velocity, const Vec3f *from) {
    void *object;
    Line line;
    AssignVector(line.m_start, *from);
    AssignVector(line.m_end, fn_8040CDD0(*position, fn_8040CDA8(fn_8040CEE8(((VectorSubtractFn)fn_8040CD74)(*position, *from)), 2.0f)));
    if (UnkLineHits_80021FE4 *hits = fn_80021FE4(fn_80020D40(), &line, 1, 1, AcceptGround, 0)) {
        object = fn_8002735C(hits)->m_object;
        AssignVector(*position, *fn_8001E9BC(object));
        Vec3f normal;
        fn_800091EC(&normal, fn_800266D4(object));
        float offset = fn_8005AE20(0.1f);
        AddVector(*position, fn_800091A4(*fn_800266D4(object), offset));
        if (Fn8040Dot(*velocity, normal) < 0.0f) {
            SubtractVector(*velocity, fn_800091A4(normal, 2.0f * DotProduct(normal, *velocity)));
        }
        fn_80026FB4(hits, 1);
    }
}

/* Throws a weapon of type type from position with velocity (name unknown). */
extern "C" void fn_80221BA8(const Vec3f *position, const Vec3f *direction,
                            const Vec3f *velocity, int type, int arg) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new Impl(parent, position, direction, velocity, type, arg);
}

/* Throws a weapon of type type from position with velocity, first moving the start out of the
 * ground along the line from from (name unknown). */
extern "C" void fn_80221B08(const Vec3f *from, const Vec3f *position,
                            const Vec3f *direction, const Vec3f *velocity, int type,
                            int arg) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    Vec3f start;
    fn_800091EC(&start, position);
    Vec3f initialVel;
    fn_800091EC(&initialVel, velocity);
    PushOutOfGround(&start, &initialVel, from);
    new Impl(parent, &start, direction, &initialVel, type, arg);
}
