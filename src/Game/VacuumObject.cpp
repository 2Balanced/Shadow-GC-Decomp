/* VacuumObject and VacuumObjectTask (class names from RTTI): an object a vacuum weapon pulls in,
 * and the task that drives it. The object keeps its state in an Impl (a pimpl; name inferred): the
 * vacuum weapon or, failing that, its owner, the matrix the object follows, the object's offset in
 * that matrix's space and its velocity. Each frame the offset spins about the vacuum while it is
 * pulled towards it; the object has arrived when it is close (or, without a weapon, when it has
 * passed the owner). The task tells the weapon about the object and plays a sound on arrival.
 * File name inferred from the class. Names other than the class names are inferred. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first). */
#include "Effect/System/EffectElement.h"
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/GadgetUtil.h"
#include "Game/VacuumObject.h"
#include "Game/WeaponVacuumBase.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Effect::System::EffectElement;
using Weapon::WeaponBase;
using Weapon::WeaponVacuumBase;

extern "C" {
/* The task tree's root and its layer (see Gadget.h). */
void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);
/* The fn_80014E04 flag word (see GiantLantern.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
/* Plays sound id at position (see Fan.cpp). */
void fn_801D54F4(void *manager, int id, const Vec3f *position, int arg3, int arg4);
/* Starts effect id (behaviour inferred; the other arguments are unknown). */
boost::weak_ptr<EffectElement> fn_801D2BB0(int arg0, int id, int arg2);
/* Moves an effect to position (see GiantLantern.cpp). */
void fn_80425E4C(EffectElement *effect, const Vec3f *position);
/* The weapon's owner (WeaponBase::m_owner; 0x80065034). */
boost::weak_ptr<CharInfo> fn_80065034(WeaponBase *weapon);

/* Matrix helpers (behaviour inferred from their bodies). */
Mtx34f fn_8040BEF8(const Mtx34f *matrix); /* inverse */
Mtx34f fn_8040FFBC(const Mtx34f *matrix, const Vec3f *angles, int order);
/* Vector helpers (behaviour inferred from their bodies). */
Vec3f fn_8040D0AC(const Vec3f *vector, float angle); /* turned about an axis */
Vec3f fn_8040D044(const Vec3f *vector, float angle); /* turned about y */
Vec3f fn_8040CEE8(const Vec3f *vector);              /* normalised */
Vec3f fn_80016284(const Vec3f *vector);              /* negated */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale); /* vector * scale */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
/* Moves position by velocity, which acceleration changes, over dt (see Bomb.cpp). */
void fn_804103E8(Vec3f *position, Vec3f *velocity,
                 const Vec3f *acceleration, float scale, float dt);
}

#define DEG2RAD(x) ((x) * (lbl_805FAD38 / 180.0f))

/* Degrees to radians (name inferred). */
inline float DegToRad(float degrees) {
    return DEG2RAD(degrees);
}

/* The matrix type's out-of-line copy (fn_8040C148), called the way the original's operator is
 * (name inferred). */
inline void AssignMatrix(Mtx34f &matrix, const Mtx34f &value) {
    fn_8040C148(&matrix, &value);
}

/* A weapon's type (name inferred). */
inline int GetWeaponType(WeaponBase *weapon) {
    return weapon->m_type;
}

/* A length (name inferred). As a call it is not a constant expression, so the values below are
 * stored by the static initializer, as in retail. */
inline float Span(float length) {
    return length;
}

/* The fn_80014E04 flag word (struct name inferred). */
struct VacuumObjectFlags {
    VacuumObjectFlags() {
        fn_80014E04(&m_bits);
    }

    u32 m_bits;
};

class VacuumObject::Impl {
  public:
    Impl(int kind);
    ~Impl();
    /* Places the object at position, given in world space (0x802388B8). */
    void SetPosition(const Vec3f &position);
    /* The object's position in world space (0x80238880). */
    Vec3f GetPosition();
    void Tick(float dt);
    bool IsValid();
    boost::weak_ptr<CharInfo> GetOwner();

    /* Stops the spin and sets its acceleration away from the side the object is on (name
     * inferred). */
    void ResetSpin() {
        m_spin = 0.0f;
        m_spinAcceleration = m_offset.x >= 0.0f ? -1750.0f : 1750.0f;
    }
    /* An unused out-of-line copy of ResetSpin (name inferred; see its definition). */
    void ResetSpinOutOfLine();

    /* 0x00 */ int m_kind;
    /* 0x04 */ boost::weak_ptr<WeaponVacuumBase> m_weapon;
    /* 0x0C */ boost::weak_ptr<CharInfo> m_owner;
    /* 0x14 */ s32 m_count; /* passed to the weapon's vf_0x38 and vf_0x3C */
    /* 0x18 */ Mtx34f m_matrix;
    /* 0x48 */ Vec3f m_offset;
    /* 0x54 */ Vec3f m_velocity;
    /* 0x60 */ float m_spin;
    /* 0x64 */ float m_spinAcceleration;
    /* 0x68 */ float m_distance;
    /* 0x6C */ Vec3f m_rotation;
    /* 0x78 */ VacuumObjectFlags m_flags;
    /* 0x7C */ boost::weak_ptr<EffectElement> m_effect;
};
SIZE_ASSERT(VacuumObject::Impl, 0x84);

/* Deletes the state (name inferred; see checked_delete below). */
inline void DeleteVacuumObjectImpl(VacuumObject::Impl *impl) {
    delete impl;
}

/* Retail keeps boost::checked_delete<VacuumObject::Impl> out of line (0x80238400) and ~Impl at its
 * own place (0x80238954); this specialization gives that code and order under GC/2.7 (see
 * docs/research-notes.md and EnergyCore.cpp). */
namespace boost {
template<> inline void checked_delete<VacuumObject::Impl>(VacuumObject::Impl *x) {
    DeleteVacuumObjectImpl(x);
}
} // namespace boost

const float lbl_805F72B0 = 3.5f;

const float lbl_805EFEE8 = DEG2RAD(-1080.0f);
/* How hard the object is pulled in with a weapon and with only an owner (names inferred). */
static float s_weaponPull = Span(1500.0f);
static float s_ownerPull = Span(6000.0f);

VacuumObject::Impl::Impl(int kind)
    : m_kind(kind), m_rotation(0.0f, 0.0f, 0.0f) {
    fn_8040CE60(&m_velocity);
    ResetSpin();
}

VacuumObject::Impl::~Impl() {}

void VacuumObject::Impl::SetPosition(const Vec3f &position) {
    Mtx34f inverse;
    AssignMatrix(inverse, fn_8040BEF8(&m_matrix));
    AssignVector(m_offset, fn_8040B67C(&inverse, &position));
    fn_8040CE60(&m_velocity);
    m_distance = fn_8040CF50(&m_offset);
    ResetSpin();
}

Vec3f VacuumObject::Impl::GetPosition() {
    return fn_8040B67C(&m_matrix, &m_offset);
}

void VacuumObject::Impl::Tick(float dt) {
    bool arrived = false;
    if (!m_owner.expired()) {
        m_spin += DegToRad(m_spinAcceleration * dt);
        AssignVector(m_offset, fn_8040D0AC(&m_offset, m_spin * dt));
        AssignVector(m_velocity, fn_8040D0AC(&m_velocity, m_spin * dt));
        Vec3f pull;
        fn_800091EC(&pull, &(const Vec3f &)fn_8040CDA8(
                               &(const Vec3f &)fn_80016284(
                                   &(const Vec3f &)fn_8040CEE8(&m_offset)),
                               s_weaponPull));
        fn_804103E8(&m_offset, &m_velocity, &pull, 20.0f, dt);
        if (fn_8040CF2C(&m_offset) < 4.0f) {
            arrived = true;
        }
    } else {
        m_spin += DegToRad(m_spinAcceleration * dt);
        AssignVector(m_offset, fn_8040D044(&m_offset, m_spin * dt));
        AssignVector(m_velocity, fn_8040D044(&m_velocity, m_spin * dt));
        Vec3f pull;
        fn_800091EC(
            &pull,
            &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)fn_80016284(&(const Vec3f &)fn_8040CEE8(
                            &(const Vec3f &)fn_8040CDD0(
                                &m_offset, &Vec3f(m_offset.x, m_offset.y, 0.0f)))),
                        s_ownerPull));
        fn_804103E8(&m_offset, &m_velocity, &pull, 20.0f, dt);
        if (m_offset.z < 0.0f) {
            arrived = true;
        }
    }
    if (arrived) {
        fn_8040CE60(&m_offset);
        fn_8040CE60(&m_velocity);
        fn_80014E1C(&m_flags.m_bits, 0);
    }
}

bool VacuumObject::Impl::IsValid() {
    if (!m_weapon.expired()) {
        return true;
    }
    if (!m_owner.expired()) {
        return true;
    }
    return false;
}

boost::weak_ptr<CharInfo> VacuumObject::Impl::GetOwner() {
    if (!m_owner.expired()) {
        return m_owner;
    }
    if (!m_weapon.expired()) {
        boost::shared_ptr<WeaponBase> weapon = m_weapon.lock();
        return fn_80065034(weapon.operator->());
    }
    return boost::weak_ptr<CharInfo>();
}

/* Not called anywhere: the linker strips it from the game. The original pools ResetSpin's float
 * literals after GetProgress's and before Tick's, which is what compiling an out-of-line function
 * that inlines it at this point of the file does (see docs/research-notes.md). */
void VacuumObject::Impl::ResetSpinOutOfLine() {
    ResetSpin();
}

VacuumObject::VacuumObject(int kind, VacuumCommand &command) : m_impl(new Impl(kind)) {
    m_impl->m_weapon = command.m_weapon;
    m_impl->m_owner = command.field_0x24;
    m_impl->m_count = command.field_0x2C;
    AssignMatrix(m_impl->m_matrix, fn_80237EE4());
}

VacuumObject::~VacuumObject() {
    if (!m_impl->m_effect.expired()) {
        m_impl->m_effect.lock()->Stop();
    }
}

void VacuumObject::fn_802382E0(const Vec3f &position) {
    m_impl->SetPosition(position);
}

extern "C" Vec3f fn_802382B0(VacuumObject *object) {
    return object->m_impl->GetPosition();
}

/* Sets vector to value (name inferred). */
inline void SetVector(Vec3f &vector, const Vec3f &value) {
    vector.x = value.x;
    vector.y = value.y;
    vector.z = value.z;
}

void VacuumObject::fn_80238268(const Vec3f &rotation) {
    SetVector(m_impl->m_rotation, rotation);
}

extern "C" Vec3f fn_80238248(VacuumObject *object) {
    return object->m_impl->m_rotation;
}

void VacuumObject::Tick(float dt) {
    AssignMatrix(m_impl->m_matrix, fn_80237EE4());
    m_impl->Tick(dt);
    if (!m_impl->m_effect.expired()) {
        fn_80425E4C(m_impl->m_effect.lock().get(),
                    &(const Vec3f &)fn_802382B0(this));
    }
}

void VacuumObject::fn_80238184(const Vec3f &velocity) {}

void VacuumObject::fn_80238180() {}

void VacuumObject::StartEffect(int id) {
    if (!m_impl->m_effect.expired()) {
        m_impl->m_effect.lock()->Stop();
        m_impl->m_effect.reset();
    }
    m_impl->m_effect = fn_801D2BB0(0, id, 0);
}

u32 *VacuumObject::GetFlags() {
    return &m_impl->m_flags.m_bits;
}

int VacuumObject::GetKind() {
    return m_impl->m_kind;
}

bool VacuumObject::fn_80238088() {
    return m_impl->IsValid();
}

extern "C" boost::weak_ptr<CharInfo> fn_80238058(VacuumObject *object) {
    return object->m_impl->GetOwner();
}

boost::weak_ptr<WeaponVacuumBase> &VacuumObject::GetWeapon() {
    return m_impl->m_weapon;
}

float VacuumObject::GetProgress() {
    return 1.0f - fn_8040CF50(&m_impl->m_offset) / m_impl->m_distance;
}

Mtx34f *VacuumObject::GetMatrix() {
    return &m_impl->m_matrix;
}

/* fn_8040C148 called as returning a copy of source by value (it stores the copy through its first
 * argument). */
typedef Mtx34f (*MatrixCopyFn)(const Mtx34f *source);

/* A copy of matrix (name inferred). */
inline Mtx34f CopyMatrix(const Mtx34f *matrix) {
    return ((MatrixCopyFn)fn_8040C148)(matrix);
}

Mtx34f VacuumObject::CalcMatrix() {
    if (!m_impl->m_weapon.expired()) {
        return CopyMatrix(m_impl->m_weapon.lock()->GetMuzzle());
    }
    if (!m_impl->m_owner.expired()) {
        boost::shared_ptr<CharInfo> owner = m_impl->m_owner.lock();
        CharInfo *character = owner.get();
        Mtx34f translation;
        fn_8040B034(&translation, &character->m_position);
        return fn_8040FFBC(&translation, &character->m_rotation, character->field_0x30);
    }
}

Mtx34f VacuumObject::fn_80237EE4() {
    return CalcMatrix();
}

extern "C" int fn_80237EDC(VacuumCommand *command) {
    return 1;
}

extern "C" int fn_80237E34(VacuumCommand *command) {
    if (!command->m_weapon.expired()) {
        boost::shared_ptr<WeaponVacuumBase> weapon = command->m_weapon.lock();
        switch (GetWeaponType(weapon.operator->())) {
        case 0x3D:
        case 0x3E:
            return 1;
        }
    }
    return 2;
}

VacuumObjectTask::VacuumObjectTask(const boost::shared_ptr<VacuumObject> &object)
    : Task(fn_801EB304(fn_8001C4F8(), 0xD)), m_object(object) {
    m_name = "VacuumObject";
    boost::weak_ptr<WeaponVacuumBase> vacuum(m_object->GetWeapon());
    if (!vacuum.expired()) {
        boost::shared_ptr<WeaponVacuumBase> weapon = vacuum.lock();
        switch (m_object->GetKind()) {
        case 0:
            break;
        case 1:
            weapon->fn_80239E94(1);
            weapon->vf_0x38(m_object->m_impl->m_count);
            break;
        case 2:
            weapon->SetObject(boost::weak_ptr<VacuumObject>(m_object));
            weapon->vf_0x38(m_object->m_impl->m_count);
            break;
        }
    }
}

VacuumObjectTask::~VacuumObjectTask() {
    boost::weak_ptr<WeaponVacuumBase> vacuum(m_object->GetWeapon());
    if (!vacuum.expired()) {
        boost::shared_ptr<WeaponVacuumBase> weapon = vacuum.lock();
        switch (m_object->GetKind()) {
        case 0:
            break;
        case 1:
            weapon->fn_80239E94(-1);
            weapon->vf_0x3C(m_object->m_impl->m_count);
            break;
        case 2:
            weapon->SetObject(boost::weak_ptr<VacuumObject>());
            weapon->vf_0x3C(m_object->m_impl->m_count);
            break;
        }
    }
}

void VacuumObjectTask::Tick(float dt) {
    if (!m_object->fn_80238088()) {
        m_flags |= 1;
        return;
    }
    switch (m_object->GetKind()) {
    case 0:
    case 1:
        if (fn_80014E34(m_object->GetFlags(), 0)) {
            m_object->fn_80238180();
            m_flags |= 1;
            return;
        }
        break;
    case 2:
        if (fn_80014E34(m_object->GetFlags(), 0) && !fn_80014E34(m_object->GetFlags(), 2)) {
            fn_80014E1C(m_object->GetFlags(), 2);
            Vec3f position;
            fn_800091EC(&position, &(const Vec3f &)fn_802382B0(m_object.get()));
            fn_801D54F4(fn_80053164(), 0x2026, &position, 0, 1);
        }
        if (fn_80014E34(m_object->GetFlags(), 1)) {
            m_flags |= 1;
            return;
        }
        break;
    }
    m_object->Tick(dt);
}
