/* Vehicle::AttackImpl (class name from the RTTI of its bound callbacks): a vehicle's body collider
 * and the attack collision made from it, with the callbacks the vehicle sets for a hit, for the end
 * of an attack, and for being attacked. Being attacked by a character of the vehicle's own side
 * (field 0x54) or by a player (kind 2 at 0x5C) is ignored. Its owner is the pimpl wrapper
 * Vehicle::BirdBase::UnkBirdCollision_801B4EBC (Game/BirdBase.h; probably Vehicle::Attack in the
 * original), which the birds, the AirSaucer and the Walker keep. The wrapper's methods keep their
 * placeholder names, as free functions over the wrapper, because those units call them by those
 * names. File name from the class (prefixed like Game/VehicleAirWing.cpp); member names are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#define VEHICLE_ATTACK_IMPL
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>

#include "Game/BirdBase.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/TVehicleBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;
using Vehicle::TVehicleBase;
using Vehicle::BirdBase::UnkBirdCollision_801B4EBC;

extern "C" {
/* The collider manager and the attack collision of a collider (see Game/AttackObject.cpp). */
void *fn_80021B08(void);
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager,
                                              boost::shared_ptr<CharaColliBase> collider);
/* Sets an attack collision's callbacks (see Game/BKTank.cpp), and its callback for being
 * attacked (Game/PlayerCharaColliImpl.cpp). */
void fn_80054DD0(CharaColliAttack *attack, int arg1, int arg2, float arg3,
                 const AttackCallback &onHit, const AttackCallback &onEnd);
void fn_80096B64(CharaColliAttack *attack, int mode, const AttackCallback &callback);
/* Whether a callback is empty, and calling it (behaviour inferred). */
bool fn_800C402C(const UnkAttackCallback_80054E28 *callback);
void fn_800C6538(const UnkAttackCallback_80054E28 *callback, AttackCallbackParam *param);
}

/* Clears a callback (name inferred). An inline function in the original: retail calls it out of
 * line. */
inline void ClearCallback(UnkAttackCallback_80054E28 *callback) {
    callback->clear();
}

/* The side a character is on (CharInfo field 0x54; name inferred). An inline function in the
 * original: retail compares through it. */
inline s32 GetSide(const CharInfo *chara) {
    return chara->field_0x54;
}

namespace Vehicle {

class AttackImpl {
  public:
    AttackImpl();
    ~AttackImpl();

    void OnAttacked(AttackCallbackParam &param);
    void OnEnd(AttackCallbackParam &param);
    void OnHit(AttackCallbackParam &param);
    void ClearAttackedCallback();
    void SetAttackedCallback(const AttackCallback &callback);
    void CreateAttack(const AttackCallback &onHit, const AttackCallback &onEnd);
    void ResetCollider();
    void SetCollider(const boost::shared_ptr<CharaColliBase> &collider);

    void ClearCallbacks();
    void SetVehicle(TVehicleBase *vehicle) {
        m_vehicle = vehicle;
    }

    /* 0x00 */ TVehicleBase *m_vehicle;
    /* 0x04 */ boost::shared_ptr<CharaColliBase> m_collider;
    /* 0x0C */ boost::weak_ptr<CharaColliAttack> m_attack;
    /* 0x14 */ UnkAttackCallback_80054E28 m_onHit;
    /* 0x20 */ UnkAttackCallback_80054E28 m_onEnd;
    /* 0x2C */ UnkAttackCallback_80054E28 m_onAttacked;
};
SIZE_ASSERT(AttackImpl, 0x38);

} // namespace Vehicle

using Vehicle::AttackImpl;

/* Deletes the state (name inferred; see checked_delete below). */
inline void DeleteAttackImpl(AttackImpl *impl) {
    delete impl;
}

/* Retail keeps boost::checked_delete<AttackImpl> out of line (0x801B4E98) and the destructor at its
 * definition; see Game/Rocket.cpp and docs/research-notes.md. */
namespace boost {
template <> inline void checked_delete<AttackImpl>(AttackImpl *x) {
    DeleteAttackImpl(x);
}
} // namespace boost

namespace Vehicle {

AttackImpl::AttackImpl() {}

AttackImpl::~AttackImpl() {
    ClearCallbacks();
    ClearAttackedCallback();
    ResetCollider();
}

void AttackImpl::SetCollider(const boost::shared_ptr<CharaColliBase> &collider) {
    void *manager = fn_80021B08();
    m_collider = collider;
    m_attack = fn_800C4374(manager, m_collider);
}

void AttackImpl::ResetCollider() {
    if (m_collider) {
        m_collider.reset();
    }
}

void AttackImpl::CreateAttack(const AttackCallback &onHit, const AttackCallback &onEnd) {
    boost::shared_ptr<CharaColliAttack> attack = m_attack.lock();
    if (attack) {
        fn_80054DD0(attack.get(), 3, 1, 0.0f,
                    AttackCallback(boost::bind(&AttackImpl::OnHit, this, _1)),
                    AttackCallback(boost::bind(&AttackImpl::OnEnd, this, _1)));
    }
    m_onHit = onHit;
    m_onEnd = onEnd;
}

void AttackImpl::ClearCallbacks() {
    ClearCallback(&m_onHit);
    ClearCallback(&m_onEnd);
}

void AttackImpl::SetAttackedCallback(const AttackCallback &callback) {
    boost::shared_ptr<CharaColliAttack> attack = m_attack.lock();
    if (attack) {
        fn_80096B64(attack.get(), 0,
                    AttackCallback(boost::bind(&AttackImpl::OnAttacked, this, _1)));
    }
    m_onAttacked = callback;
}

void AttackImpl::ClearAttackedCallback() {
    ClearCallback(&m_onAttacked);
}

void AttackImpl::OnHit(AttackCallbackParam &param) {
    if (!fn_800C402C(&m_onHit)) {
        fn_800C6538(&m_onHit, &param);
    }
}

void AttackImpl::OnEnd(AttackCallbackParam &param) {
    if (!fn_800C402C(&m_onEnd)) {
        fn_800C6538(&m_onEnd, &param);
    }
}

void AttackImpl::OnAttacked(AttackCallbackParam &param) {
    boost::shared_ptr<CharaColliAttack> attacker = param.m_attacker.lock();
    if (attacker && m_vehicle != 0) {
        boost::shared_ptr<CharInfo> owner = attacker->m_owner.lock();
        if (owner) {
            if (GetSide(owner.get()) == GetSide(m_vehicle)) {
                param.m_result = 8;
                return;
            }
            if (owner->field_0x5C == 2) {
                param.m_result = 8;
                return;
            }
        }
    }
    if (!fn_800C402C(&m_onAttacked)) {
        fn_800C6538(&m_onAttacked, &param);
    }
}

namespace BirdBase {

UnkBirdCollision_801B4EBC::UnkBirdCollision_801B4EBC() : m_impl(new AttackImpl) {}

UnkBirdCollision_801B4EBC::~UnkBirdCollision_801B4EBC() {}

} // namespace BirdBase
} // namespace Vehicle

/* The wrapper's methods (names keep their placeholders; see above). */
extern "C" {

void fn_801B4DD0(UnkBirdCollision_801B4EBC *collision, TVehicleBase *vehicle) {
    collision->m_impl->SetVehicle(vehicle);
}

void fn_801B4DAC(UnkBirdCollision_801B4EBC *collision,
                 const boost::shared_ptr<CharaColliBase> &collider) {
    collision->m_impl->SetCollider(collider);
}

boost::shared_ptr<CharaColliBase> fn_801B4D84(UnkBirdCollision_801B4EBC *collision) {
    return collision->m_impl->m_collider;
}

void fn_801B4D60(UnkBirdCollision_801B4EBC *collision) {
    collision->m_impl->ResetCollider();
}

void fn_801B4D3C(UnkBirdCollision_801B4EBC *collision, const AttackCallback &onHit,
                 const AttackCallback &onEnd) {
    collision->m_impl->CreateAttack(onHit, onEnd);
}

void fn_801B4D18(UnkBirdCollision_801B4EBC *collision, const AttackCallback &callback) {
    collision->m_impl->SetAttackedCallback(callback);
}

void fn_801B4CF4(UnkBirdCollision_801B4EBC *collision) {
    collision->m_impl->ClearAttackedCallback();
}

void fn_801B4CA0(UnkBirdCollision_801B4EBC *collision) {
    if (collision->m_impl->m_collider) {
        collision->m_impl->m_collider->TurnOn(-1);
    }
}

void fn_801B4C4C(UnkBirdCollision_801B4EBC *collision) {
    if (collision->m_impl->m_collider) {
        collision->m_impl->m_collider->TurnOff(-1);
    }
}

} // extern "C"

/* Not called anywhere: the linker strips it. Retail emits the scoped_ptr's out-of-line operator->
 * (0x801B4C44) ahead of the wrapper's methods; compiling this function last makes GC/2.7 emit it
 * there too (compare "the order of float constants" in docs/research-notes.md). */
void VehicleAttackInstanceOrder(UnkBirdCollision_801B4EBC *collision) {
    collision->m_impl->SetVehicle(0);
}
