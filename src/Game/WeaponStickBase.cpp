/* WeaponStickBase (class name from RTTI: Weapon::WeaponStickBase): the base of the swung weapons. It
 * keeps its swing in a WeaponStickBaseImpl (class name from the RTTI of its attack callbacks'
 * bind_t): while a swing is held the Impl owns an attack collision and sweeps it along the blade
 * from where it was a frame ago in segments of five units, makes a hit target ignore the collision
 * from then on and counts the hits, pushes hit targets away from the swinger, and, for a player's
 * stick, grows the model's part 0 to 1.5 times its length while swinging and back to its length
 * after. Other units call the functions without class names by their addresses. File name from
 * the class name; the other names are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first).
 *
 * NonMatching: everything matches but fn_80067918, whose two argument instructions after the
 * out-of-line scoped_ptr::operator-> (the flags' address and the bit) GC/2.7 emits in the opposite
 * order; about 25 source shapes (helpers, locals, operator*, pragmas) did not change it. */
#define WEAPON_STICK_BASE_IMPL_MEMBERS
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/AttackIgnoreList.h"
#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/GadgetUtil.h"
#include "Game/GeometryLine.h"
#include "Game/MathTypes.h"
#include "Game/WeaponBase.h"
#include "Game/WeaponStickBase.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using Sonicteam::System::Geometry::Line;
using Weapon::UnkWeaponModel_80063428;
using Weapon::WeaponStickBase;

class UnkColliderManager_80021B08;
struct UnkLineHits_80021FE4;

extern "C" {
/* A 32-bit flag word (see Game/BKTankController.h): construct cleared, clear all, set, clear and
 * test a bit. */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
/* Plays sound id at position (behaviour inferred; see Game/WeaponWormShooter.cpp). */
void fn_801D54F4(void *sound, u16 id, const Vec3f *position, int arg3, int arg4);
/* The owner of a weapon (Game/WeaponRocketLauncher.cpp). */
boost::weak_ptr<CharInfo> fn_80065034(const Weapon::WeaponBase *weapon);
/* Sets the scale of a weapon model's part (Game/WeaponModel.cpp). */
void fn_800631FC(UnkWeaponModel_80063428 *model, int part, const Vec3f *scale);
/* Sets the two hit callbacks of an attack collision (see Game/ArkLaser.cpp). */
void fn_80054DD0(CharaColliAttack *attack, int arg1, int arg2, float arg3, const AttackCallback &onHit,
                 const AttackCallback &onEnd);
/* The collider manager (see Game/ArkLaser.cpp), the colliders along a line that an attack
 * collision of kind may hit (behaviour inferred), and freeing the list it returns (see
 * Game/BkWingLarge.cpp). */
UnkColliderManager_80021B08 *fn_80021B08(void);
UnkLineHits_80021FE4 *fn_800C5D74(UnkColliderManager_80021B08 *manager, const Line *line, int kind, bool arg3,
                                  int arg4, int arg5, int arg6,
                                  boost::weak_ptr<CharaColliAttack> attack);
void fn_80026FB4(UnkLineHits_80021FE4 *hits, int arg1);
/* Copies a line (behaviour inferred). */
Line *fn_8002CCF0(Line *line, const Line *other);
/* Vector helpers (behaviour inferred from their bodies; see Game/BirdBase.cpp and
 * Game/BulletCannon.cpp): set to zero, add in place, scale in place, length, the distance between
 * two points, and normalised, scaled and summed vectors returned by value. */
void fn_8040CE60(Vec3f *vector);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
void fn_8040CE38(Vec3f *vector, float scale);
float fn_8040D1B4(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CEE8(const Vec3f &vector);
Vec3f fn_8040CDA8(const Vec3f &vector, float scale);
Vec3f fn_8040CDD0(const Vec3f &a, const Vec3f &b);
}

/* fn_8040CD74 called as returning a - b by value (see Game/ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* A flag word (see Game/BKTankController.h; struct name unknown, as in
 * Game/WeaponLockonBase.cpp). */
struct UnkFlags_80014E04 {
    UnkFlags_80014E04() {
        fn_80014E04(&m_bits);
    }
    void ClearAll() {
        fn_80014E10(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    void Clear(int bit) {
        fn_8006127C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits;
};

namespace Weapon {

/* The swing. Flag bit 0: a swing is held. */
class WeaponStickBaseImpl {
  public:
    WeaponStickBaseImpl(const boost::weak_ptr<CharInfo> &owner, float reach);
    ~WeaponStickBaseImpl();

    /* Sweeps the held swing and grows or shrinks a player's stick. */
    void Update(float dt);
    /* Starts and ends a swing. */
    void Hold();
    void Release();
    /* Moves the blade to tip and root, keeping where it was. */
    void SetBlade(const Vec3f *tip, const Vec3f *root);
    /* Tests the attack collision along the blade's way since the last frame. */
    void Sweep();
    /* Creates and frees the attack collision. */
    void StartSwing();
    void StopSwing();
    /* The attack collision's callbacks: pushes the target away from the swinger, and makes a
     * target hit ignore the collision and counts it. */
    void PushTarget(AttackCallbackParam &param);
    void CountHit(AttackCallbackParam &param);

    void SetWeapon(WeaponStickBase *weapon) {
        m_weapon = weapon;
    }
    int HitCount() {
        return m_hits;
    }

    /* 0x00 */ boost::shared_ptr<CharaColliAttack> m_attack;
    /* 0x08 */ WeaponStickBase *m_weapon;
    /* 0x0C */ Line m_lines[2]; /* the blade (tip, root) now and a frame ago */
    /* 0x3C */ UnkFlags_80014E04 m_flags;
    /* 0x40 */ s32 m_hits; /* this frame */
    /* 0x44 */ float m_scale;
    /* 0x48 */ float m_reach;
    /* 0x4C */ s32 m_hitKind;    /* 3 for a player's stick, 6 otherwise (meanings unknown) */
    /* 0x50 */ s32 m_attackKind; /* 3 for a player's stick, 4 otherwise */
};
SIZE_ASSERT(WeaponStickBaseImpl, 0x54);

} // namespace Weapon

using Weapon::WeaponStickBaseImpl;

/* Deletes the Impl (see checked_delete below). */
inline void DeleteImpl(WeaponStickBaseImpl *impl) {
    delete impl;
}

/* Retail keeps boost::checked_delete of the Impl out of line and its destructor at its own place;
 * this specialization gives that order under GC/2.7 (see docs/research-notes.md and
 * Game/EnergyCore.cpp). */
namespace boost {
template <>
inline void checked_delete<WeaponStickBaseImpl>(WeaponStickBaseImpl *x) {
    DeleteImpl(x);
}
} // namespace boost

/* Returns x unchanged (name inferred). As a call it is not a constant expression, so the value below
 * is stored by the static initializer, as in retail. */
inline float Value(float x) {
    return x;
}

/* The length of the segments the blade's way is tested in. */
static float s_segmentLength = Value(5.0f);

namespace Weapon {

WeaponStickBaseImpl::WeaponStickBaseImpl(const boost::weak_ptr<CharInfo> &owner, float reach) : m_reach(reach) {
    boost::shared_ptr<CharInfo> chara = owner.lock();
    if (chara.get() != 0 && chara->field_0x5C == 1) {
        m_hitKind = 3;
        m_attackKind = 3;
    } else {
        m_hitKind = 6;
        m_attackKind = 4;
    }
    fn_8040CE60(&m_lines[0].m_start);
    fn_8040CE60(&m_lines[0].m_end);
    fn_8040CE60(&m_lines[1].m_start);
    fn_8040CE60(&m_lines[1].m_end);
    m_scale = 1.0f;
    m_flags.ClearAll();
}

WeaponStickBaseImpl::~WeaponStickBaseImpl() {
    StopSwing();
}

void WeaponStickBaseImpl::Update(float dt) {
    m_hits = 0;
    bool swung = false;
    if (m_flags.Test(0) && m_attack.get() != 0) {
        Sweep();
        swung = true;
    }
    boost::shared_ptr<CharInfo> chara = fn_80065034(m_weapon).lock();
    if (chara.get() != 0 && chara->field_0x5C == 1) {
        if (swung) {
            if (m_scale < 1.5f) {
                m_scale += 0.5f * dt / 0.1f;
                if (m_scale > 1.5f) {
                    m_scale = 1.5f;
                }
            }
        } else if (m_scale > 1.0f) {
            m_scale += -0.5f * dt / 0.1f;
            if (m_scale < 1.0f) {
                m_scale = 1.0f;
            }
        }
        if (m_weapon->m_model != 0) {
            Vec3f scale(1.0f, m_scale, 1.0f);
            fn_800631FC(m_weapon->m_model, 0, &scale);
        }
    }
}

void WeaponStickBaseImpl::Hold() {
    if (!m_flags.Test(0)) {
        StartSwing();
    }
}

void WeaponStickBaseImpl::Release() {
    if (m_flags.Test(0)) {
        StopSwing();
    }
}

void WeaponStickBaseImpl::SetBlade(const Vec3f *tip, const Vec3f *root) {
    fn_8002CCF0(&m_lines[1], &m_lines[0]);
    fn_800091D0(&m_lines[0].m_start, tip);
    fn_800091D0(&m_lines[0].m_end, root);
}

void WeaponStickBaseImpl::Sweep() {
    Vec3f tipStep;
    fn_800091EC(&tipStep, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&m_lines[0].m_start, &m_lines[1].m_start));
    Vec3f rootStep;
    fn_800091EC(&rootStep, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&m_lines[0].m_end, &m_lines[1].m_end));
    int steps = (int)(fn_8040CF50(&rootStep) / s_segmentLength) + 1;
    float t = 1.0f / steps;
    fn_8040CE38(&tipStep, t);
    fn_8040CE38(&rootStep, t);
    float current = fn_8040D1B4(&m_lines[0].m_end, &m_lines[0].m_start);
    float length = fn_8040D1B4(&m_lines[1].m_end, &m_lines[1].m_start);
    float lengthStep = t * (current - length);
    Vec3f tip;
    fn_800091EC(&tip, &m_lines[1].m_start);
    Vec3f root;
    fn_800091EC(&root, &m_lines[1].m_end);
    for (int i = 0; i < steps; i++) {
        Line line;
        fn_800091D0(&line.m_start, &tip);
        fn_800091D0(&line.m_end,
                    &(const Vec3f &)fn_8040CDD0(
                        fn_8040CDA8(fn_8040CEE8(((VectorSubtractFn)fn_8040CD74)(&root, &tip)), length), tip));
        UnkLineHits_80021FE4 *hits = fn_800C5D74(fn_80021B08(), &line, m_hitKind, 1, 0, 0, 0,
                                                 boost::weak_ptr<CharaColliAttack>(m_attack));
        if (hits != 0) {
            fn_80026FB4(hits, 1);
        }
        fn_8040CFF8(&tip, &tipStep);
        fn_8040CFF8(&root, &rootStep);
        length += lengthStep;
    }
}

void WeaponStickBaseImpl::StartSwing() {
    m_attack = boost::shared_ptr<CharaColliAttack>(new CharaColliAttack);
    if (m_attack.get() != 0) {
        fn_80054DD0(m_attack.get(), m_attackKind, 0x80, m_reach,
                    boost::bind(&WeaponStickBaseImpl::PushTarget, this, _1),
                    boost::bind(&WeaponStickBaseImpl::CountHit, this, _1));
        m_attack->m_owner = fn_80065034(m_weapon);
        m_flags.Set(0);
    }
}

void WeaponStickBaseImpl::StopSwing() {
    if (m_attack.get() != 0) {
        fn_80053B40(&m_attack);
        m_flags.Clear(0);
    }
}

void WeaponStickBaseImpl::PushTarget(AttackCallbackParam &param) {
    boost::shared_ptr<CharInfo> chara = param.m_attacker.lock()->m_owner.lock();
    if (chara.get() != 0) {
        fn_800091D0(reinterpret_cast<Vec3f *>(param.field_0x8 + (0x34 - 0x8)),
                    &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                        reinterpret_cast<Vec3f *>(param.field_0x8 + (0x18 - 0x8)), &chara->m_position));
    }
}

void WeaponStickBaseImpl::CountHit(AttackCallbackParam &param) {
    if (param.m_result & 1) {
        if (m_attack.get() != 0) {
            fn_800532F8(m_attack.get(), *reinterpret_cast<boost::weak_ptr<CharaColliAttack> *>(param.field_0x8));
        }
        m_hits++;
    }
}

WeaponStickBase::WeaponStickBase(const boost::weak_ptr<CharInfo> &owner, int type, float reach)
    : WeaponBase(owner, type, 5), m_swing(new WeaponStickBaseImpl(owner, reach)) {
    m_swing->SetWeapon(this);
}

WeaponStickBase::~WeaponStickBase() {}

void WeaponStickBase::Tick(float dt) {
    m_swing->Update(dt);
}

void WeaponStickBase::SwingHeld() {
    m_swing->Hold();
}

void WeaponStickBase::SwingReleased() {
    m_swing->Release();
}

bool WeaponStickBase::fn_80067918() {
    return m_swing->m_flags.Test(0);
}

float WeaponStickBase::fn_8006790C() {
    return m_swing->m_scale;
}

void WeaponStickBase::vf_0x38(int n) {
    fn_801D54F4(fn_80053164(), n + 0x2005, &m_position, 0, 1);
}

} // namespace Weapon

/* The hits of the swing this frame (other units call it by its address). */
extern "C" int fn_80067894(WeaponStickBase *weapon) {
    return weapon->m_swing->HitCount();
}

namespace Weapon {

void WeaponStickBase::fn_80067870(const Vec3f *tip, const Vec3f *root) {
    m_swing->SetBlade(tip, root);
}

} // namespace Weapon
