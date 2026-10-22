/* ChaosSpear (class name from RTTI: Weapon::ChaosSpear::ChaosSpear): the weapon behind Shadow's
 * Chaos Spear, weapon type 0x47. It is not a weapon a character picks up: Player::Shadow creates it
 * through fn_802F0740 (see PlayerShadowSuper.cpp). Holding the button charges it, with a charging
 * effect and sound, and releasing it fires a Chaos Spear bullet (Game/BulletChaosSpear.cpp) with a
 * burst effect sized by the charge. File name inferred from the class name. Method and member names
 * are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Effect/System/EffectElement.h"
#include "Game/BulletChaosSpear.h"
#include "Game/CharInfo.h"
#include "Game/EffectPJSLauncher.h"
#include "Game/GadgetSound.h"
#include "Game/GadgetUtil.h"
#include "Game/Vibration.h"
#include "Game/WeaponBase.h"
#include "Game/WeaponBullet.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Effect::System::EffectElement;
using Weapon::ShotInfo;

extern "C" {
/* The owner of a weapon (see WeaponVacuumBase.cpp). */
boost::weak_ptr<CharInfo> fn_80065034(Weapon::WeaponBase *weapon);
/* The position and rotation of a weapon; either may be null (behaviour inferred). */
void fn_8005CF90(Weapon::WeaponBase *weapon, Vec3f *position, Vec3f *rotation);
/* A vector turned about Z, X and Y by an angle (behaviour inferred). */
Vec3f fn_8040D044(const Vec3f &vector, float angle);
Vec3f fn_8040D118(const Vec3f &vector, float angle);
Vec3f fn_8040D0AC(const Vec3f &vector, float angle);
/* m rotated by the Euler angles in order (behaviour inferred). */
Mtx34f fn_8040FFBC(const Mtx34f *m, const Vec3f *angles, int order);
/* Returns its argument (an empty conversion; meaning inferred). */
int fn_801DBD00(int value);
/* The smaller of a and b. */
const float &fn_800704AC(const float &a, const float &b);
/* The id effects launched for a character carry (behaviour inferred). */
u32 fn_8005E4B8(const boost::weak_ptr<CharInfo> &owner);
/* Launches a PJS effect following, or placed by, a matrix (see EffectPJS.cpp). */
boost::weak_ptr<EffectElement> fn_80305900(Effect::PJS::Launcher *launcher, const Mtx34f *matrix,
                                           s32 arg, u32 owner);
boost::weak_ptr<EffectElement> fn_803055F8(Effect::PJS::Launcher *launcher, const Mtx34f *matrix,
                                           s32 arg, u32 owner);
/* The launcher of effect set index, and taking and dropping a reference to the set (behaviour
 * inferred). */
boost::shared_ptr<Effect::PJS::Launcher> fn_801D2CA4(int index);
void fn_801D2DE8(int index);
void fn_801D2D14(int index);
/* Starts a sound for owner (see UpDownReel.cpp). */
UnkSoundHandle_801D3DC8 fn_801D3EF0(int soundId, void *owner);
/* Plays a sound (behaviour inferred). */
void fn_801D51A0(void *manager, int id, int arg2, int arg3);
/* Starts controller vibration pattern for player (see GadgetUtility.cpp). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int player);
}

/* The longest charge, in seconds. */
const float kMaxCharge = 3.0f;

namespace Weapon {
namespace ChaosSpear {

class ChaosSpear : public WeaponGunBase {
  public:
    ChaosSpear(const boost::weak_ptr<CharInfo> &owner);
    virtual ~ChaosSpear();
    virtual void Tick(float dt);
    virtual float vf_0x1C();
    virtual Vec3f vf_0x20();
    virtual float vf_0x24() {
        return 1.0f;
    }
    virtual float vf_0x28() {
        return 0.0f;
    }
    virtual float Fire(const Vec3f *aim);
    virtual float FireAt(const Vec3f *aim, const Vec3f *target);
    /* Releases the charge: fires the spear (returns the time until the next shot). */
    virtual float vf_0x38(const Vec3f *aim);
    /* Starts charging. */
    virtual void vf_0x3C();
    /* Cancels the charge. */
    virtual void vf_0x40();

    /* The charging effect. */
    void StartChargeEffect();
    void StopChargeEffect();
    /* Places m_matrix at the owner. */
    void UpdateMatrix();

    /* 0x3C */ boost::shared_ptr<Effect::PJS::Launcher> m_launcher;
    /* 0x44 */ boost::weak_ptr<EffectElement> m_chargeEffect;
    /* 0x4C */ Mtx34f m_matrix;
    /* 0x7C */ float m_charge;
    /* 0x80 */ u8 m_charged;
    /* 0x81 */ u8 m_charging;
    /* 0x84 */ UnkSoundHandle_801D3DC8 m_sound;
};
SIZE_ASSERT(ChaosSpear, 0xA0);

ChaosSpear::ChaosSpear(const boost::weak_ptr<CharInfo> &owner) : WeaponGunBase(owner, 0x47) {
    m_charge = 0.0f;
    m_charged = false;
    m_charging = false;
    fn_801D2DE8(2);
    m_launcher = fn_801D2CA4(2);
}

ChaosSpear::~ChaosSpear() {
    fn_801D2D14(2);
    StopChargeEffect();
    m_sound.fn_801D455C();
}

void ChaosSpear::Tick(float dt) {
    WeaponGunBase::Tick(dt);
    if (m_charging) {
        m_charge = fn_800704AC(m_charge + dt, kMaxCharge);
        if (!m_charged && m_charge >= 2.4f) {
            m_charged = true;
            fn_801D51A0(fn_80053164(), 0xF, -1, -1);
        }
    }
    UpdateMatrix();
}

float ChaosSpear::vf_0x1C() {
    return Bullet::ChaosSpear::GetRange();
}

Vec3f ChaosSpear::vf_0x20() {
    return Vec3f(0.0f, 0.0f, -3.1875f);
}

float ChaosSpear::Fire(const Vec3f *aim) {
    return -1.0f;
}

float ChaosSpear::FireAt(const Vec3f *aim, const Vec3f *target) {
    return -1.0f;
}

float ChaosSpear::vf_0x38(const Vec3f *aim) {
    Vec3f position;
    Vec3f rotation(0.0f, 0.0f, 0.0f);
    fn_8005CF90(this, &position, &rotation);
    Vec3f direction(0.0f, 0.0f, 1.0f);
    AssignVector(direction, fn_8040D044(direction, rotation.z));
    AssignVector(direction, fn_8040D118(direction, rotation.x));
    AssignVector(direction, fn_8040D0AC(direction, rotation.y));
    boost::shared_ptr<CharInfo> owner = fn_80065034(this).lock();
    bool side;
    if (owner.get() != 0 && owner->field_0x5C == 1) {
        side = false;
    } else {
        side = true;
    }
    float charge = m_charge / 3.0f;
    Bullet::ChaosSpear::Fire(ShotInfo(boost::weak_ptr<CharInfo>(owner), side, position, direction, *aim),
                             m_launcher, charge);
    m_sound.fn_801D455C();
    int vibration;
    int sound;
    int effect;
    float scale;
    if (charge >= 1.0f) {
        scale = 1.0f;
        sound = 0x8130;
        effect = 0xF;
        vibration = 3;
        fn_801D51A0(fn_80053164(), 0x10, -1, -1);
    } else {
        scale = 1.0f + charge;
        sound = 0x812E;
        effect = 0;
        vibration = 0;
    }
    {
        boost::shared_ptr<CharInfo> chara = fn_80065034(this).lock();
        if (chara.get() != 0) {
            fn_8034DF48(vibration, chara->field_0x54);
        }
    }
    fn_801D560C(fn_80053164(), (u16)sound, 0, 0);
    Mtx34f matrix;
    Mtx34f place;
    SetTranslationMtx(&place, position);
    fn_8040C148(&matrix, &place);
    fn_8040B848(&matrix, rotation.y);
    fn_8040B884(&matrix, rotation.x);
    fn_8040B80C(&matrix, rotation.z);
    fn_8040B720(&matrix, scale, scale, scale);
    fn_803055F8(m_launcher.get(), &matrix, effect, fn_8005E4B8(boost::weak_ptr<CharInfo>(owner)));
    fn_8005EB94(1);
    m_charge = 0.0f;
    m_charged = false;
    m_charging = false;
    StopChargeEffect();
    return 0.0f;
}

void ChaosSpear::vf_0x3C() {
    m_charge = 0.0f;
    m_charged = false;
    m_charging = true;
    m_sound = fn_801D3EF0(0x812F, this);
    StopChargeEffect();
    StartChargeEffect();
}

void ChaosSpear::vf_0x40() {
    m_charge = 0.0f;
    m_charged = false;
    m_charging = false;
    m_sound.fn_801D455C();
    StopChargeEffect();
}

/* Rotates m by a character's Euler angles in order (see WeaponSpecialKatana.cpp). */
inline void RotateMtx(Mtx34f *m, const CharInfo *chara, int order) {
    fn_8040C148(m, &(const Mtx34f &)fn_8040FFBC(m, &chara->m_rotation, order));
}

void ChaosSpear::UpdateMatrix() {
    boost::shared_ptr<CharInfo> owner = fn_80065034(this).lock();
    if (owner.get() != 0) {
        Mtx34f place;
        SetTranslationMtx(&place, owner->m_position);
        fn_8040C148(&m_matrix, &place);
        RotateMtx(&m_matrix, owner.operator->(), fn_801DBD00(owner->field_0x30));
    }
}

void ChaosSpear::StartChargeEffect() {
    boost::shared_ptr<CharInfo> owner = fn_80065034(this).lock();
    if (owner.get() != 0) {
        m_chargeEffect = fn_80305900(m_launcher.get(), &m_matrix, 9,
                                     fn_8005E4B8(boost::weak_ptr<CharInfo>(owner)));
    }
}

void ChaosSpear::StopChargeEffect() {
    if (!m_chargeEffect.expired()) {
        m_chargeEffect.lock()->Stop();
        m_chargeEffect.reset();
    }
}

} // namespace ChaosSpear
} // namespace Weapon

/* Creates the weapon for owner (called by Player::Shadow; see PlayerShadowSuper.cpp). */
extern "C" boost::shared_ptr<Weapon::WeaponGunBase> fn_802F0740(const boost::weak_ptr<CharInfo> &owner) {
    return boost::shared_ptr<Weapon::WeaponGunBase>(new Weapon::ChaosSpear::ChaosSpear(owner));
}
