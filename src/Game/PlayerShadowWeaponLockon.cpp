/* @unnamed@PlayerShadowWeaponLockon_cpp@::ControlLockon (class and file name from RTTI): how
 * Shadow handles a lock-on weapon (Weapon::WeaponLockonBase). A small state machine: wait for the
 * reload and the fire button, raise the arm, start locking on, lock on while the button is held
 * (as long as there is ammo for another target), fire when it is let go and wait until the shots
 * are out. Method, member and state names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/Player.h"
#include "Game/PlayerShadowWeapon.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/WeaponBase.h"
#include "Game/WeaponLockonBase.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

/* The player's record in the game state (struct names unknown; see Game/PlayerShadowWeapon.cpp). */
struct UnkObject_80079808;
struct UnkRecord_80178B5C {
    /* 0x00 */ u8 field_0x0[0x1C];
    /* 0x1C */ s32 m_ammo;
};

extern "C" {
UnkObject_80079808 *fn_80079808(void);
UnkRecord_80178B5C *fn_80178B5C(UnkObject_80079808 *data, int player);
s32 fn_8016EF30(s32 *ammo);
/* Whether the motion holds the arm up to aim (behaviour inferred). */
bool fn_800AA640(Player::PlayerMotion *motion);

/* Weapon::WeaponLockonBase methods (behaviour inferred): start locking on, the targets locked
 * on, stop locking on, cancel, the time until the next round, and the shots fired this frame. */
void fn_8005FA68(Weapon::WeaponLockonBase *weapon);
void fn_8005FA38(Weapon::WeaponLockonBase *weapon);
void fn_8005FA14(Weapon::WeaponLockonBase *weapon);
float fn_8005FA98(Weapon::WeaponLockonBase *weapon);
int fn_8005F9A0(Weapon::WeaponLockonBase *weapon);
}

namespace {

using Player::Shadow::ControlWeaponBase;
using Player::Shadow::ShadowWeapon;

class ControlLockon : public ControlWeaponBase {
  public:
    enum State {
        STATE_WAIT = 0,
        STATE_RAISE = 1,
        STATE_LOCK = 2,
        STATE_HOLD = 3,
        STATE_FIRE = 4,
        STATE_END = 100,
    };

    ControlLockon(ShadowWeapon *owner, const boost::shared_ptr<Weapon::WeaponBase> &weapon);
    virtual ~ControlLockon();
    virtual void Update(float dt);

    /* 0x08 */ boost::shared_ptr<Weapon::WeaponLockonBase> m_lockon;
    /* 0x10 */ s32 m_state;
    /* 0x14 */ float m_reload;
};
SIZE_ASSERT(ControlLockon, 0x18);

ControlLockon::ControlLockon(ShadowWeapon *owner,
                             const boost::shared_ptr<Weapon::WeaponBase> &weapon)
    : ControlWeaponBase(owner) {
    m_lockon = boost::dynamic_pointer_cast<Weapon::WeaponLockonBase>(weapon);
    m_state = STATE_WAIT;
    m_reload = 0.0f;
}

ControlLockon::~ControlLockon() {
    m_owner->m_flags.Clear(2);
    m_owner->LowerArm();
}

void ControlLockon::Update(float dt) {
    UnkRecord_80178B5C *record = fn_80178B5C(fn_80079808(), m_owner->m_player->field_0x54);
    switch (m_state) {
    case STATE_WAIT:
        if (m_reload > 0.0f) {
            m_reload -= dt;
            if (m_reload <= 0.0f) {
                m_reload = 0.0f;
            }
        } else if (m_owner->m_flags.Test(0)) {
            m_owner->m_flags.Set(2);
            m_owner->RaiseArm();
            m_state++;
        }
        break;
    case STATE_RAISE:
        if (fn_800AA640(m_owner->m_player->m_motion)) {
            fn_8005FA68(m_lockon.get());
            m_state++;
        }
        break;
    case STATE_LOCK:
        if (record == 0 || fn_8016EF30(&record->m_ammo) <= m_lockon.get()->fn_8005F9C8()) {
            fn_8005FA38(m_lockon.get());
            m_state++;
        }
    case STATE_HOLD:
        if (m_owner->m_flags.Test(1)) {
            fn_8005FA14(m_lockon.get());
            m_state = STATE_END;
        } else if (!m_owner->m_flags.Test(0)) {
            fn_8005FA38(m_lockon.get());
            m_reload = fn_8005FA98(m_lockon.get());
            m_state++;
        }
        break;
    case STATE_FIRE:
        if (m_lockon.get()->fn_8005F9C8() <= 0) {
            m_state = STATE_END;
        }
        break;
    case STATE_END:
        m_state = STATE_WAIT;
        m_owner->m_flags.Clear(2);
        m_owner->LowerArm();
        break;
    }
    int shots = fn_8005F9A0(m_lockon.get());
    if (shots) {
        m_owner->m_flags.Set(3);
        m_owner->AddAmmo(-shots);
    }
    if (!m_owner->HasAmmo()) {
        m_owner->DropWeapon();
    }
}

} // namespace

namespace Player {
namespace Shadow {

ControlWeaponBase *CreateControlLockon(ShadowWeapon *owner,
                                       const boost::shared_ptr<Weapon::WeaponBase> &weapon) {
    return new ControlLockon(owner, weapon);
}

} // namespace Shadow
} // namespace Player
