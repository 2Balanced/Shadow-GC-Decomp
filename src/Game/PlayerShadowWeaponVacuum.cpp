/* @unnamed@PlayerShadowWeaponVacuum_cpp@::ControlVacuum (class and file name from RTTI): how
 * Shadow handles a weapon that pulls objects in (Weapon::WeaponVacuumBase, e.g. the vacuum pod).
 * A state machine: wait until nothing is held, raise the arm when the fire button is pressed, pull
 * while it is held, stop when the object has arrived or the button is let go, and throw the object
 * along Shadow's motion. Method, member and state names are inferred. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include "Game/Player.h"
#include "Game/PlayerShadowWeapon.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/WeaponBase.h"
#include "Game/WeaponVacuumBase.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

extern "C" {
/* A bit array: tests, sets and clears bit. */
bool fn_8006CD78(u32 *bits, int bit);
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* Whether the motion holds the arm up to aim (behaviour inferred). */
bool fn_800AA640(Player::PlayerMotion *motion);
}

namespace {

using Player::Shadow::ControlWeaponBase;
using Player::Shadow::ShadowWeapon;

/* Weapon::WeaponVacuumBase's slot 0x48 takes the ammo, but its declaration in
 * Game/WeaponVacuumBase.h has no parameter and its symbol (0x80225CA0) is named after that
 * signature; the call goes through this view of the vtable instead (name inferred). */
class UnkVacuumVtableView {
  public:
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
    virtual void vf_0x40();
    virtual void vf_0x44();
    /* Tells the weapon the ammo left (behaviour inferred). */
    virtual void SetAmmo(int ammo);
};

class ControlVacuum : public ControlWeaponBase {
  public:
    enum State {
        STATE_EMPTY = 0,
        STATE_READY = 1,
        STATE_AIM = 100,
        STATE_PULL = 101,
        STATE_HOLD = 102,
        STATE_RELEASE = 103,
        STATE_RECOVER = 200,
    };

    ControlVacuum(ShadowWeapon *owner, const boost::shared_ptr<Weapon::WeaponBase> &weapon);
    virtual ~ControlVacuum();
    virtual void Update(float dt);

    /* 0x08 */ boost::shared_ptr<Weapon::WeaponVacuumBase> m_vacuum;
    /* 0x10 */ s32 m_state;
    /* 0x14 */ float m_time;
};
SIZE_ASSERT(ControlVacuum, 0x18);

ControlVacuum::ControlVacuum(ShadowWeapon *owner,
                             const boost::shared_ptr<Weapon::WeaponBase> &weapon)
    : ControlWeaponBase(owner) {
    m_vacuum = boost::dynamic_pointer_cast<Weapon::WeaponVacuumBase>(weapon);
    reinterpret_cast<UnkVacuumVtableView *>(m_vacuum.get())->SetAmmo(m_owner->GetAmmo());
    m_state = STATE_EMPTY;
}

ControlVacuum::~ControlVacuum() {
    m_owner->m_flags.Clear(2);
    m_owner->LowerArm();
    fn_80076CC8(m_owner->m_player->m_stateBits, 0x41);
}

void ControlVacuum::Update(float dt) {
    m_time += dt;
    switch (m_state) {
    case STATE_EMPTY:
        if (m_vacuum->fn_80239E8C() == 0) {
            m_owner->m_flags.Clear(2);
            m_owner->LowerArm();
            m_state++;
        }
        break;
    case STATE_READY:
        if (m_owner->m_flags.Test(0)) {
            m_owner->m_flags.Set(2);
            m_owner->RaiseArm();
            m_state = STATE_AIM;
        }
        break;
    case STATE_AIM:
        if (!m_owner->m_flags.Test(0)) {
            m_state = STATE_EMPTY;
        } else if (fn_800AA640(m_owner->m_player->m_motion)) {
            m_vacuum->StartVacuum();
            m_state++;
        }
        break;
    case STATE_PULL:
        if (m_vacuum->fn_80239DD4()) {
            m_owner->m_flags.Clear(2);
            m_vacuum->StopVacuum();
            m_state++;
        }
    case STATE_HOLD:
        if (!m_owner->m_flags.Test(0)) {
            m_vacuum->StopVacuum();
            m_state = STATE_RELEASE;
        }
        break;
    case STATE_RELEASE:
        if (m_vacuum->fn_80239DD4()) {
            m_vacuum->LetGo(m_owner->m_player->field_0x18);
            m_owner->m_flags.Set(3);
            m_time = 0.0f;
            m_state = STATE_RECOVER;
        } else if (!m_vacuum->HasObject()) {
            m_state = STATE_EMPTY;
        }
        break;
    case STATE_RECOVER:
        if (m_time > 0.3f) {
            m_state = STATE_EMPTY;
        }
        break;
    }
    m_vacuum->fn_80239D8C(fn_8006CD78(m_owner->m_player->m_stateBits, 0x3F));
    if (m_vacuum->fn_80239DD4()) {
        fn_80076CA8(m_owner->m_player->m_stateBits, 0x41);
    } else {
        fn_80076CC8(m_owner->m_player->m_stateBits, 0x41);
    }
    m_owner->AddAmmo(-m_vacuum->fn_80239DCC());
    if (m_owner->GetAmmo() <= 0) {
        m_vacuum->StopVacuum();
    }
    if (!m_owner->HasAmmo() && !m_vacuum->HasObject() && !m_vacuum->fn_80239DD4() &&
        m_vacuum->fn_80239E8C() == 0) {
        m_owner->DropWeapon();
    }
}

} // namespace

namespace Player {
namespace Shadow {

ControlWeaponBase *CreateControlVacuum(ShadowWeapon *owner,
                                       const boost::shared_ptr<Weapon::WeaponBase> &weapon) {
    return new ControlVacuum(owner, weapon);
}

} // namespace Shadow
} // namespace Player
