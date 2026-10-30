/* Player::Attack (class name from RTTI; base Task) and Player::AttackImpl (class name from the RTTI of
 * its callback binder): a player character's attack collision. The task keeps the implementation in
 * a boost::scoped_ptr; the implementation makes an attack collider of one of twelve kinds (a table
 * of the motion node it follows, its radius, the attack kind and power, and the character slot that
 * owns it), keeps it on its node every frame, and calls back the behavior that started the attack
 * when it hits. Other units call the task's functions by their addresses. File name from the class;
 * other names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). The collider set-up reaches the unit's .data through a pooled base
 * address, so the linker keeps that section whole: Game/CharaColliBaseCharInfo.h supplies the RTTI
 * base list it starts with. */
#define BOOST_SP_STATIC_POINTER_CAST
#define MSL_RUNTIME_ERROR_STRING

#include "Game/CharaColliBaseCharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/Task.h"
#include "Effect/System/EffectElement.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <dolphin/types.h>
#include "size_assert.h"

using boost::shared_ptr;
using boost::weak_ptr;
using Effect::System::EffectElement;
using Player::PlayerBase;
using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;
using SonicteamUSA::System::CharaColli::CharaColliBaseCharInfo;

namespace Vibration {
class RequestHandle;
}

/* A player's attack colliders (class unknown; see Game/BossSonic.cpp). */
struct UnkPlayerPart_80095BD4;

/* A node of a player's motion (struct name unknown; see Game/EmbeddedWeapon.cpp). */
struct UnkMotionNode_800AA760 {
    /* 0x00 */ const Mtx34f *m_matrix;
    /* 0x04 */ u8 m_chara[4]; /* passed to the collider (meaning unknown) */
};

/* The player members used here (names inferred). */
struct UnkPlayerFields_80077B1C {
    /* 0x000 */ u8 field_0x0[0x54];
    /* 0x054 */ s32 m_pad;
    /* 0x058 */ u8 field_0x58[0xA8 - 0x58];
    /* 0x0A8 */ u32 m_stateBits[2];
    /* 0x0B0 */ u8 field_0xB0[0x230 - 0xB0];
    /* 0x230 */ Player::PlayerMotion *m_motion;
    /* 0x234 */ Task *m_attack;
    /* 0x238 */ u8 field_0x238[0x248 - 0x238];
    /* 0x248 */ UnkPlayerPart_80095BD4 *m_part;
    /* 0x24C */ u8 field_0x24C[0x254 - 0x24C];
    /* 0x254 */ Player::UnkCharaSlots_800AADA4 *m_charaSlots;
};

#define Fields(player) reinterpret_cast<UnkPlayerFields_80077B1C *>(player)

extern "C" {
/* The collider manager, a sphere collider, and its attack collision (see Game/AttackObject.cpp). */
void *fn_80021B08(void);
shared_ptr<CharaColliBase> fn_800C7EA8(void *manager, const Vec3f *position, u32 flags, int arg3, int arg4,
                                       const weak_ptr<CharInfo> &owner, const Vec3f *offset, float radius);
weak_ptr<CharaColliAttack> fn_800C4374(void *manager, shared_ptr<CharaColliBase> collider);
/* Sets the attack collision's callbacks (see Game/AttackObject.cpp). */
void fn_80054DD0(CharaColliAttack *attack, int arg1, float power, int kind, const AttackCallback &onHit,
                 const AttackCallback &onEnd);
/* Makes attack ignore other, and other's ignored ones (Game/AttackIgnoreList.h). */
void fn_800532F8(CharaColliAttack *attack, weak_ptr<CharaColliAttack> other);
void fn_80054C6C(CharaColliAttack *attack, weak_ptr<CharaColliAttack> other);
/* The character in a slot of a player (see Game/PlayerManageUnit.cpp). */
weak_ptr<CharInfo> fn_800AADA4(Player::UnkCharaSlots_800AADA4 *slots, int index);
/* The player's attack colliders: turning them on and off, and their attack collision. */
void fn_80095AAC(UnkPlayerPart_80095BD4 *part, bool attacking);
weak_ptr<CharaColliAttack> fn_800959B0(UnkPlayerPart_80095BD4 *part);
/* Sets and clears a bit of a bit array. */
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* A node of a motion. */
UnkMotionNode_800AA760 *fn_800AA760(Player::PlayerMotion *motion, int index);
/* Makes the effect follow a matrix. */
void fn_80425E68(EffectElement *effect, const Mtx34f *matrix);
/* Rumbles a pad (see Game/Vibration.h). */
weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int pad);
Vec3f fn_8040CEE8(const Vec3f *v);
}

/* The attack collision of a collider (Game/BulletUtil.cpp). */
weak_ptr<CharaColliAttack> AttackOf(CharaColliBaseCharInfo *collider);

/* fn_8040CD74 as a function that returns its result (see Game/EnemyRender.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *lhs, const Vec3f *rhs);

/* The vector type's out-of-line assignment (see Game/JumpBoard.cpp). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* The size of the colliders (name inferred; folded, but a dynamic initializer stores it). */
inline float Radius(float radius) {
    return radius;
}

/* An attack kind (struct name unknown): the motion node the collider follows, its radius, the attack
 * kind and power passed to the collision, and the character slot that owns it (slot 0 also rumbles
 * the pad on a hit). */
struct UnkAttackParam_8052012C {
    /* 0x00 */ s32 m_node;
    /* 0x04 */ float m_radius;
    /* 0x08 */ s32 m_kind;
    /* 0x0C */ float m_power;
    /* 0x10 */ s32 m_slot;
};

static UnkAttackParam_8052012C s_params[12] = {
    {5, Radius(5.0f), 1, 1.0f, 0},     {3, Radius(5.0f), 1, 1.0f, 0},    {9, Radius(5.0f), 1, 1.0f, 0},
    {7, Radius(5.0f), 1, 1.0f, 0},     {0, Radius(7.0f), 2, 2.0f, 0},    {11, Radius(7.0f), 4, 2.0f, 0},
    {11, Radius(8.0f), 4, 2.0f, 0},    {11, Radius(8.0f), 8, 2.0f, 0},   {2, Radius(15.0f), 1, 1.0f, 0},
    {12, Radius(8.0f), 1, 1.0f, 0},    {11, Radius(8.0f), 0x10, 12.0f, 1}, {11, Radius(100.0f), 0x200, 2.0f, 1},
};

namespace Player {

/* The callback a behavior gives the attack (class name unknown). */
class UnkAttackCallback_800786CC {
  public:
    UnkAttackCallback_800786CC &Clear() {
        m_function.clear();
        return *this;
    }
    UnkAttackCallback_800786CC &operator=(const AttackCallback &f) {
        m_function = f;
        return *this;
    }
    bool IsSet() const {
        return HasTarget();
    }
    bool HasTarget() const {
        return !m_function.empty();
    }
    void operator()(AttackCallbackParam &param) {
        if (!m_function.empty()) {
            m_function(param);
        }
    }

    /* 0x0 */ AttackCallback m_function;
};

class AttackImpl {
  public:
    AttackImpl(const shared_ptr<PlayerBase> &player);
    ~AttackImpl();
    void Tick(float dt);
    /* Starts an attack of a kind, unless one of that kind runs. */
    void SetKind(int kind);
    void StopIfActive();
    void SetSize(float size);
    void SetCallback(const AttackCallback &callback);
    void ClearCallback();
    void OnAttack(AttackCallbackParam &param);
    void OnHit(AttackCallbackParam &param);
    void Update();
    void Start();
    void Stop();
    void SetNode(CharInfo *node);
    void SetPower(float power) {
        m_power = power;
    }

    /* 0x00 */ shared_ptr<PlayerBase> m_player;
    /* 0x08 */ shared_ptr<CharaColliBase> m_collider;
    /* 0x10 */ weak_ptr<CharaColliAttack> m_attack;
    /* 0x18 */ weak_ptr<EffectElement> m_effect;
    /* 0x20 */ s32 m_kind;
    /* 0x24 */ UnkAttackCallback_800786CC m_callback;
    /* 0x30 */ float m_power;
};
SIZE_ASSERT(AttackImpl, 0x34);

class Attack : public Task {
  public:
    Attack(Task *parent, const shared_ptr<PlayerBase> &player);
    virtual ~Attack();
    virtual void Tick(float dt);

    /* 0x28 */ boost::scoped_ptr<AttackImpl> m_impl;
    /* 0x2C */ u32 field_0x2C;
};
SIZE_ASSERT(Attack, 0x30);

/* Deletes an implementation (name inferred; see checked_delete below). */
inline void DeleteAttackImpl(AttackImpl *impl) {
    delete impl;
}

} // namespace Player

/* Retail keeps boost::checked_delete<Player::AttackImpl> out of line (0x80077AF8) and the
 * implementation's destructor at its own place (0x800787C8); this specialization calls an inline
 * helper, which gives retail's code and order (see docs/research-notes.md, EnergyCore). */
namespace boost {
template <> inline void checked_delete<Player::AttackImpl>(Player::AttackImpl *x) {
    Player::DeleteAttackImpl(x);
}
} // namespace boost

namespace Player {

AttackImpl::AttackImpl(const shared_ptr<PlayerBase> &player) : m_player(player) {}

AttackImpl::~AttackImpl() {
    Stop();
}

void AttackImpl::Tick(float dt) {
    Update();
}

void AttackImpl::SetKind(int kind) {
    if (!m_collider || m_kind != kind) {
        m_kind = kind;
        Start();
    }
}

void AttackImpl::StopIfActive() {
    if (m_collider) {
        Stop();
    }
}

void AttackImpl::SetSize(float size) {
    if (m_collider) {
        m_collider->SetSize(size, 0.0f, 0.0f, -2);
    }
}

void AttackImpl::SetCallback(const AttackCallback &callback) {
    m_callback = callback;
}

void AttackImpl::ClearCallback() {
    m_callback.Clear();
}

/* Gives the hit the attack's power, knocking the target away from the player. */
void AttackImpl::OnAttack(AttackCallbackParam &param) {
    param.m_damage = m_power;
    fn_800091D0(reinterpret_cast<Vec3f *>(&param.field_0x8[0x34 - 0x8]),
                &(const Vec3f &)fn_8040CEE8(&(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                    reinterpret_cast<Vec3f *>(&param.field_0x8[0x18 - 0x8]), &m_player->m_position)));
}

/* Ignores the target from now on, rumbles the pad and calls back the behavior. */
void AttackImpl::OnHit(AttackCallbackParam &param) {
    if (param.m_result & 1) {
        shared_ptr<CharaColliAttack> attack = m_attack.lock();
        if (attack) {
            fn_800532F8(attack.get(), *reinterpret_cast<weak_ptr<CharaColliAttack> *>(param.field_0x8));
        }
        if (s_params[m_kind].m_slot == 0) {
            fn_8034DF48(1, Fields(m_player.get())->m_pad);
        }
    }
    if (m_callback.IsSet()) {

        m_callback(param);
    }
}

/* Keeps the collider and the effect on the attack's node. */
void AttackImpl::Update() {
    if (m_collider && Fields(m_player.get())->m_motion) {
        UnkMotionNode_800AA760 *node = fn_800AA760(Fields(m_player.get())->m_motion, s_params[m_kind].m_node);
        SetNode(reinterpret_cast<CharInfo *>(node->m_chara));
        if (!m_effect.expired() && node->m_matrix) {
            fn_80425E68(m_effect.lock().get(), node->m_matrix);
        }
    }
}

void AttackImpl::Start() {
    void *manager = fn_80021B08();
    const UnkAttackParam_8052012C *param = &s_params[m_kind];
    shared_ptr<CharaColliBase> previous(m_collider);
    if (m_collider) {
        Stop();
    }
    m_collider = fn_800C7EA8(manager, &lbl_805E07A0, 0x40000, 3, 0,
                             fn_800AADA4(Fields(m_player.get())->m_charaSlots, param->m_slot), &lbl_805E07A0,
                             param->m_radius);
    m_collider->SetField2C(1);
    m_attack = fn_800C4374(manager, m_collider);
    shared_ptr<CharaColliAttack> attack = m_attack.lock();
    if (attack) {
        if (previous) {
            shared_ptr<CharaColliBaseCharInfo> info = boost::static_pointer_cast<CharaColliBaseCharInfo>(previous);
            fn_80054C6C(attack.get(), AttackOf(info.get()));
        }
        fn_80054DD0(attack.get(), 3, param->m_power, param->m_kind,
                    AttackCallback(boost::bind(&AttackImpl::OnAttack, this, _1)),
                    AttackCallback(boost::bind(&AttackImpl::OnHit, this, _1)));
        m_power = param->m_power;
        fn_800532F8(attack.get(), fn_800959B0(Fields(m_player.get())->m_part));
    }
    fn_80095AAC(Fields(m_player.get())->m_part, true);
    fn_80076CA8(Fields(m_player.get())->m_stateBits, 0x1C);
    Update();
}

void AttackImpl::Stop() {
    if (m_collider) {
        m_collider.reset();
    }
    if (Fields(m_player.get())->m_part) {
        fn_80095AAC(Fields(m_player.get())->m_part, false);
    }
    fn_80076CC8(Fields(m_player.get())->m_stateBits, 0x1C);
    if (!m_effect.expired()) {
        m_effect.lock()->Stop();
        m_effect.reset();
    }
}

void AttackImpl::SetNode(CharInfo *node) {
    if (m_collider) {
        m_collider->SetCharInfo(node);
    }
}

Attack::Attack(Task *parent, const shared_ptr<PlayerBase> &player) : Task(parent), m_impl(new AttackImpl(player)) {
    m_name = "Attack";
    Fields(m_impl->m_player.get())->m_attack = this;
}

Attack::~Attack() {
    Fields(m_impl->m_player.get())->m_attack = 0;
}

void Attack::Tick(float dt) {
    m_impl->Tick(dt);
}

} // namespace Player

using Player::Attack;

extern "C" {

/* Starts an attack of a motion kind, with power (kinds 4 and 7 to 9 map to attack kinds 4, 8 and 9;
 * others to 4). */
void fn_80077974(Attack *attack, int kind, float power) {
    int attackKind;
    switch (kind) {
    case 0:
        attackKind = 0;
        break;
    case 1:
        attackKind = 1;
        break;
    case 2:
        attackKind = 2;
        break;
    case 3:
        attackKind = 3;
        break;
    case 5:
        attackKind = 8;
        break;
    case 6:
        attackKind = 9;
        break;
    default:
        attackKind = 4;
        break;
    }
    attack->m_impl->SetKind(attackKind);
    attack->m_impl->SetPower(power);
}

void fn_80077950(Attack *attack, int kind) {
    attack->m_impl->SetKind(kind);
}

void fn_8007792C(Attack *attack) {
    attack->m_impl->StopIfActive();
}

void fn_80077908(Attack *attack, float size) {
    attack->m_impl->SetSize(size);
}

/* Never called (the linker drops it): retail emits the out-of-line operator-> of the task's
 * implementation pointer (used by fn_80077974) here, after fn_800778DC. */
void fn_AttackSetPowerUnused(Attack *attack, float power) {
    attack->m_impl->SetPower(power);
}

void fn_800778DC(Attack *attack, const AttackCallback &callback) {

    attack->m_impl->SetCallback(callback);
}

void fn_800778B8(Attack *attack) {
    attack->m_impl->ClearCallback();
}

} // extern "C"
