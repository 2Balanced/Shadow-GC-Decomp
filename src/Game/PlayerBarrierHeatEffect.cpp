/* @unnamed@PlayerBarrierHeatEffect_cpp@::HeatEffect (class name from RTTI; its task name is
 * "HeatEffect"): the task of a player's heat barrier (Player::Barrier kind 2, Game/PlayerBarrier.cpp).
 * It puts an attack sphere on the player's node 1 that hits each character in it, counting the
 * characters it is touching in a hit list, and sets the player's state bit 0x21 while it lives.
 * The file name comes from the RTTI name. Function and member names are inferred. Definitions are
 * in reverse address order (deferred inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerBarrier.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

extern "C" {
/* The collider manager, a sphere collider for owner, and the attack collision of a collider
 * (behaviour inferred; see Game/BlockChange.cpp and Game/WeaponTorch.cpp). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7EA8(void *manager, const Vec3f *position,
                                              u32 flags, int arg3, int arg4,
                                              const boost::weak_ptr<CharInfo> &owner,
                                              const Vec3f *offset, float radius);
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager,
                                              boost::shared_ptr<CharaColliBase> collider);
/* Sets the two hit callbacks of an attack collision (see Game/WeaponTorch.cpp). */
void fn_80054DD0(CharaColliAttack *attack, int arg1, int arg2, float arg3, const AttackCallback &onHit,
                 const AttackCallback &onEnd);
/* Makes an attack collision ignore another (see Game/BulletLaser.cpp), and the attack collision
 * of the player's weapon state (behaviour inferred). */
void fn_800532F8(CharaColliAttack *attack, boost::weak_ptr<CharaColliAttack> other);
boost::weak_ptr<CharaColliAttack> fn_800959B0(void *weapons);
/* The player's character slot index (see Game/PlayerShadowSuper.cpp) and motion node index. */
boost::weak_ptr<CharInfo> fn_800AADA4(Player::UnkCharaSlots_800AADA4 *slots, int index);
Player::UnkMotionNode_800AA760 *fn_800AA760(Player::PlayerMotion *motion, int index);
/* Set and clear a bit of the player's state bits (see Game/Player.h). */
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* Whether a hit's target is gone (behaviour inferred). */
bool fn_80053538(const void *target);
/* The hit list (class unknown, 0x10 bytes): constructor and destructor, whether it holds target,
 * counting target in, ageing the entries by count and dropping the stale ones (behaviour inferred
 * from their bodies). */
void *fn_803555CC(void *list);
void fn_80355478(void *list, int);
bool fn_803553E8(void *list, const void *target);
void fn_803552DC(void *list, const void *target);
void fn_80355260(void *list, int count);
void fn_8035511C(void *list);
}

/* A value (name inferred). As a call it is not a constant expression, so the value is stored by
 * the static initializer, as in retail. */
inline float Plain(float value) {
    return value;
}

/* The radius of the attack sphere (name inferred). */
static float s_radius = Plain(20.0f);

namespace {

class HeatEffect : public Task {
  public:
    HeatEffect(Task *parent, Player::BarrierImpl *barrier);
    virtual ~HeatEffect();
    virtual void Tick(float dt);

    void OnAttack(AttackCallbackParam &param);
    void OnAttackEnd(AttackCallbackParam &param);
    void Setup();
    void RemoveCollider();
    void UpdatePosition();

    /* 0x28 */ boost::shared_ptr<Player::PlayerBase> m_player;
    /* 0x30 */ boost::shared_ptr<CharaColliBase> m_collider;
    /* 0x38 */ boost::weak_ptr<CharaColliAttack> m_attack;
    /* 0x40 */ u8 m_hits[0x10]; /* the hit list (see fn_803555CC) */
};
SIZE_ASSERT(HeatEffect, 0x50);

HeatEffect::HeatEffect(Task *parent, Player::BarrierImpl *barrier)
    : Task(parent), m_player(barrier->m_player) {
    fn_803555CC(m_hits);
    m_name = "HeatEffect";
    Setup();
    fn_80076CA8(m_player->m_stateBits, 0x21);
}

HeatEffect::~HeatEffect() {
    fn_80076CC8(m_player->m_stateBits, 0x21);
    RemoveCollider();
    fn_80355478(m_hits, -1);
}

void HeatEffect::Tick(float dt) {
    UpdatePosition();
    fn_80355260(m_hits, 1);
    fn_8035511C(m_hits);
}

void HeatEffect::OnAttack(AttackCallbackParam &param) {
    if (!fn_80053538(param.field_0x8)) {
        if (fn_803553E8(m_hits, param.field_0x8)) {
            param.field_0x8[0x30 - 0x8] = 1;
        }
        fn_803552DC(m_hits, param.field_0x8);
    }
}

void HeatEffect::OnAttackEnd(AttackCallbackParam &param) {}

void HeatEffect::Setup() {
    void *manager = fn_80021B08();
    m_collider = fn_800C7EA8(manager, &lbl_805E07A0, 0x40000, 3, 0,
                             fn_800AADA4(m_player->m_charaSlots, 1), &lbl_805E07A0, s_radius);
    m_collider->SetField2C(1);
    m_attack = fn_800C4374(manager, m_collider).lock();
    boost::shared_ptr<CharaColliAttack> attack = m_attack.lock();
    if (attack) {
        fn_80054DD0(attack.get(), 3, 0x200, 1.0f,
                    AttackCallback(boost::bind(&HeatEffect::OnAttack, this, _1)),
                    AttackCallback(boost::bind(&HeatEffect::OnAttackEnd, this, _1)));
        fn_800532F8(attack.get(),
                    fn_800959B0(*reinterpret_cast<void **>(m_player->field_0x244 + (0x248 - 0x244))));
    }
}

void HeatEffect::RemoveCollider() {
    if (m_collider.get() != 0) {
        m_collider.reset();
    }
}

void HeatEffect::UpdatePosition() {
    if (m_player->m_motion != 0 && m_collider.get() != 0) {
        Player::UnkMotionNode_800AA760 *node = fn_800AA760(m_player->m_motion, 1);
        m_collider->SetCharInfo((CharInfo *)node->field_0x4);
    }
}

} // namespace

/* Creates the heat barrier's task under parent (called through Player::Barrier's kind table). */
extern "C" Task *fn_8033FBA0(Task *parent, Player::BarrierImpl *barrier) {
    return new HeatEffect(parent, barrier);
}
