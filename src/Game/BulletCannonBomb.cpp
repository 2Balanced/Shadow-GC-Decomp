/* CannonBomb (class name from RTTI: Weapon::Bullet::CannonBomb): the blast a cannon shell
 * (Game/BulletCannon.cpp) or a gun bullet leaves where it hits. It stays where the bullet was, plays
 * its sound and effect once, and grows an attack collider (a sphere) to its radius over its time;
 * the collider ignores what the bullet's attack collision ignored. This unit is the first to emit
 * the shared inline functions that set up an attack collision (fn_80054DD0, which other units call
 * by its address) and copy its ignore list (fn_80054C6C). File name inferred from the class name;
 * other names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include <algorithm>
#include <boost/function/function_base.hpp>

/* std::swap for boost::function's functor pointer: the game has one shared copy (0x80019164), which
 * this overload, declared but not defined here, stands in for (see Game/CatchStick.cpp). */
namespace std {
void swap(boost::detail::function::any_pointer &a, boost::detail::function::any_pointer &b);
} // namespace std

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/AttackIgnoreList.h"
#include "Effect/System/EffectElement.h"
#include "Game/BulletCannonBomb.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/SoundManager.h"
#include "Game/WeaponBullet.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Effect::System::EffectElement;
using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;
using Weapon::Bullet::BulletBase;

extern "C" {
/* The sound manager and its play call (see Game/BulletCannon.cpp). */
UnkSound_80053164 *fn_80053164(void);
bool fn_801D54F4(UnkSound_80053164 *sound, u16 id, void *source, void *owner, u16 flags);
/* Starts effect id at position, facing direction, for owner (see Game/BulletLaser.cpp). */
boost::weak_ptr<EffectElement> fn_8005E32C(const Vec3f *position, const Vec3f *direction, int id,
                                           const boost::weak_ptr<CharInfo> &owner);
/* The collider manager, a sphere collider of radius at offset from position, and its attack
 * collision (see Game/BlackBull0412ModeCtrlUnit.cpp and Game/ArkLaser.cpp). The radius is declared
 * first here: it is the only floating-point argument, so it goes in f1 wherever it is declared, but
 * the order in which retail loads the arguments puts it ahead of the integer ones. */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7EA8(float radius, void *manager, const Vec3f *position, u32 flags,
                                              int kind, int arg4, boost::weak_ptr<CharInfo> owner,
                                              const Vec3f *offset);
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager, boost::shared_ptr<CharaColliBase> collider);
}

/* boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out of line; this unit
 * emits the copy for CharaColliBase (boost 1.32's definition). */
namespace boost {
template <> inline void shared_ptr<CharaColliBase>::reset() {
    this_type().swap(*this);
}
} // namespace boost

/* The assignment of an attack collision's callbacks (declared in Game/CharaColliAttack.h): an
 * inline function in the original, which this unit is the first to emit. */
inline UnkAttackCallback_80054E28 &UnkAttackCallback_80054E28::operator=(const AttackCallback &f) {
    AttackCallback::operator=(f);
    return *this;
}

/* Sets up an attack collision: its kind, flags and power, and its callbacks (other units call it by
 * its address). */
extern "C" inline void fn_80054DD0(CharaColliAttack *attack, int kind, u32 flags, float power,
                                   const AttackCallback &onHit, const AttackCallback &onEnd) {
    attack->field_0x54 = 1;
    attack->m_kind = kind;
    attack->m_power = power;
    attack->m_flags = flags;
    attack->m_hitCallback = onHit;
    /* A third callback at 0x28 (meaning inferred). */
    reinterpret_cast<UnkAttackCallback_80054E28 &>(attack->field_0x28) = onEnd;
}

/* Makes attack ignore what other ignores (name inferred; other units call it by its address). */
extern "C" inline void fn_80054C6C(CharaColliAttack *attack, boost::weak_ptr<CharaColliAttack> other) {
    if (fn_80053538(&other)) {
        return;
    }
    boost::shared_ptr<CharaColliAttack> source(other);
    if (source.get() == 0) {
        return;
    }
    for (UnkAttackIgnoreList_80053340::iterator it =
             reinterpret_cast<UnkAttackIgnoreList_80053340 *>(&source->field_0x44)->Head();
         it != reinterpret_cast<UnkAttackIgnoreList_80053340 *>(&source->field_0x44)->End(); it++) {
        fn_800532F8(attack, *it);
    }
}

/* Returns x unchanged (name inferred). Being initialised through a function call, the values below
 * are set up at startup (see Meteor.cpp). */
inline float Plain(float x) {
    return x;
}

namespace Weapon {
namespace Bullet {

/* What each blast type does (struct name unknown). */
struct CannonBombParam {
    /* 0x00 */ float damage;
    /* 0x04 */ float radius;
    /* 0x08 */ u32 sound;
    /* 0x0C */ s32 effect;
    /* 0x10 */ float duration;
};

static CannonBombParam s_params[] = {
    {4.0f, Plain(30.0f), 0x2014, 0x100, 0.5f},  {8.0f, Plain(50.0f), 0x2016, 0x106, 0.5f},
    {14.0f, Plain(100.0f), 0x2018, 0x135, 0.5f}, {4.0f, Plain(40.0f), 0x201A, 0x106, 0.5f},
    {8.0f, Plain(70.0f), 0x201C, 0x106, 0.5f},  {8.0f, Plain(50.0f), 0x2016, 0x106, 0.5f},
    {4.0f, Plain(40.0f), 0x201A, 0x106, 0.5f},  {0.0f, Plain(0.0f), 0x201A, 0x106, 0.5f},
    {0.0f, Plain(0.0f), 0x201A, 0x106, 0.5f},
};

class CannonBomb : public BulletBase {
  public:
    CannonBomb(const BulletBase &bullet, int type, const boost::weak_ptr<CharaColliAttack> &attack);
    virtual ~CannonBomb();
    virtual void Tick(float dt);

    /* Creates the attack collider, which ignores what previous ignored. */
    void SetupAttack(float damage, const BulletBase &bullet, const boost::weak_ptr<CharaColliAttack> &previous);
    void ReleaseCollider();
    /* Places the collider and grows it with the blast's time. */
    void UpdateCollider();
    /* The callbacks of the attack collision. */
    void OnHit(AttackCallbackParam &param);
    void OnAttackEnd(AttackCallbackParam &param);

    /* 0x3C */ float m_time;
    /* 0x40 */ float m_radius;
    /* 0x44 */ s32 m_type;
    /* 0x48 */ boost::shared_ptr<CharaColliBase> m_collider;
    /* 0x50 */ boost::weak_ptr<CharaColliAttack> m_attack;
};
SIZE_ASSERT(CannonBomb, 0x58);

CannonBomb::CannonBomb(const BulletBase &bullet, int type, const boost::weak_ptr<CharaColliAttack> &attack)
    : BulletBase(bullet), m_type(type) {
    m_radius = s_params[m_type].radius;
    m_time = 0.0f;
    fn_801D54F4(fn_80053164(), s_params[m_type].sound, &m_state.m_position, 0, 1);
    Vec3f up(0.0f, 1.0f, 0.0f);
    fn_8005E32C(&m_state.m_position, &up, s_params[m_type].effect, bullet.m_owner);
    SetupAttack(s_params[m_type].damage, bullet, attack);
}

CannonBomb::~CannonBomb() {
    ReleaseCollider();
}

void CannonBomb::Tick(float dt) {
    m_time += dt;
    if (m_time < s_params[m_type].duration) {
        UpdateCollider();
    } else {
        delete this;
    }
}

void CannonBomb::SetupAttack(float damage, const BulletBase &bullet,
                             const boost::weak_ptr<CharaColliAttack> &previous) {
    void *manager = fn_80021B08();
    if (m_collider.get() != 0) {
        ReleaseCollider();
    }
    Vec3f position;
    m_collider = fn_800C7EA8(0.0f, manager, &position, 0, bullet.m_attackKind, 0, bullet.m_owner, &lbl_805E07A0);
    m_attack = fn_800C4374(manager, m_collider);
    boost::shared_ptr<CharaColliAttack> attack = m_attack.lock();
    if (attack.get() != 0) {
        fn_80054DD0(attack.get(), 5, 0x200, damage, boost::bind(&CannonBomb::OnHit, this, _1),
                    boost::bind(&CannonBomb::OnAttackEnd, this, _1));
        if (!fn_80053538(&previous)) {
            fn_80054C6C(attack.get(), previous);
        }
    }
    UpdateCollider();
}

void CannonBomb::ReleaseCollider() {
    if (m_collider.get() != 0) {
        m_collider.reset();
    }
}

void CannonBomb::UpdateCollider() {
    if (m_collider.get() != 0) {
        /* Slot 0x3C, which takes the position here. */
        m_collider->SetCharInfo(reinterpret_cast<CharInfo *>(&m_state.m_position));
        m_collider->SetSize(m_radius * m_time / s_params[m_type].duration, 0.0f, 0.0f, -2);
    }
}

void CannonBomb::OnHit(AttackCallbackParam &param) {}

void CannonBomb::OnAttackEnd(AttackCallbackParam &param) {
    boost::shared_ptr<CharaColliAttack> attack = m_attack.lock();
    if (attack.get() != 0) {
        /* What the attack hit at 0x8 (meaning inferred). */
        fn_800532F8(attack.get(), *reinterpret_cast<const boost::weak_ptr<CharaColliAttack> *>(param.field_0x8));
    }
}

void StartCannonBomb(const BulletBase &bullet, int type, const boost::weak_ptr<CharaColliAttack> &attack) {
    new CannonBomb(bullet, type, attack);
}

} // namespace Bullet
} // namespace Weapon
