/* EnemyAttack (class name from the RTTI of its bound callback; declared in Game/EnemyAttack.h): an
 * enemy's attack collider. Create makes a collider with the attack's shape for the owner, takes its
 * attack collision and binds the two callbacks to it with boost::bind: OnHit stores the direction
 * from the attacker to the hit point, OnHitEnd tells the attack collision about the other side.
 * File name inferred from the class. Names marked "(name inferred)" are not original. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/EnemyAttack.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

/* The shape a collider is made with (struct name unknown; layout inferred). */
struct UnkColliShape_800C86A8 {
    /* 0x00 */ u32 field_0x0;
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ u32 m_flags;
    /* 0x0C */ u32 field_0xC[5];
};
SIZE_ASSERT(UnkColliShape_800C86A8, 0x20);

extern const Vec3f lbl_805E07B8;

extern "C" {
/* The collider manager, a collider of a shape for owner, its attack collision, and releasing a
 * collider (see Game/DevilDoomCtrlUnitPsychoObject.cpp and Game/BlockChange.cpp). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C86A8(void *manager, const UnkColliShape_800C86A8 *shape,
                                              const Vec3f *position, const Vec3f *rotation,
                                              int arg4, int arg5, int arg6,
                                              boost::weak_ptr<CharInfo> owner);
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager,
                                              boost::shared_ptr<CharaColliBase> collider);
void fn_800C78B0(void *manager, boost::shared_ptr<CharaColliBase> *collider);
/* Sets up an attack collision and its callbacks (see Game/DiablonPartsBody.cpp), and tells it
 * about the other side of a hit (see Game/ThrowObject.cpp). */
void fn_80054DD0(CharaColliAttack *attack, int arg1, int arg2, float arg3,
                 const AttackCallback &onHit, const AttackCallback &onEnd);
void fn_800532F8(CharaColliAttack *attack, boost::weak_ptr<CharaColliAttack> other);
}

/* The default shape (name inferred). */
static const UnkColliShape_800C86A8 s_shape = {0, 0, 0x40000, {0, 0, 0, 0, 0}};

/* What AttackCallbackParam holds beyond its named members (names inferred): the other side's
 * attack collision, the hit point, a flag and the direction OnHit stores. */
inline boost::weak_ptr<CharaColliAttack> &GetOther(AttackCallbackParam &param) {
    return *reinterpret_cast<boost::weak_ptr<CharaColliAttack> *>(param.field_0x8);
}

inline Vec3f &GetHitPoint(AttackCallbackParam &param) {
    return *reinterpret_cast<Vec3f *>(param.field_0x8 + (0x18 - 0x8));
}

inline u8 &GetHitFlag(AttackCallbackParam &param) {
    return param.field_0x8[0x30 - 0x8];
}

inline Vec3f &GetHitDirection(AttackCallbackParam &param) {
    return *reinterpret_cast<Vec3f *>(param.field_0x8 + (0x34 - 0x8));
}

/* The attack collision's word at 0x3C (meaning unknown). */
inline s32 GetField3C(CharaColliAttack *attack) {
    return *reinterpret_cast<s32 *>(attack->field_0x3C);
}

EnemyAttack::EnemyAttack() : m_type(-1) {}

EnemyAttack::~EnemyAttack() {
    Release();
}

void EnemyAttack::Set(const Desc &desc, CharInfo *info) {
    if (m_type != desc.m_type) {
        m_type = desc.m_type;
        Release();
        Create(desc);
        SetCharInfo(info);
    } else {
        SetCharInfo(info);
    }
    m_set = 1;
}

void EnemyAttack::Update() {
    if (!m_set) {
        Reset();
    }
    m_set = 0;
}

void EnemyAttack::Reset() {
    m_type = -1;
    Release();
}

/* NonMatching (97%): retail loads the 1.0f argument of fn_80054DD0 before desc.m_damage; GC/2.7
 * schedules the two loads the other way round for every shape tried (locals, casts, an inline
 * wrapper, by-value callbacks). */
void EnemyAttack::Create(const Desc &desc) {
    Release();
    UnkColliShape_800C86A8 shape = s_shape;
    shape.field_0x0 = desc.field_0x8;
    shape.field_0x4 = desc.field_0xC;
    m_collider = fn_800C86A8(fn_80021B08(), &shape, &lbl_805E07A0, &lbl_805E07B8, 2, 6, 0,
                             desc.m_owner);
    m_attack = fn_800C4374(fn_80021B08(), m_collider);
    if (boost::shared_ptr<CharaColliAttack> attack = m_attack.lock()) {
        fn_80054DD0(attack.get(), 4, desc.m_damage, 1.0f,
                    AttackCallback(boost::bind(&EnemyAttack::OnHit, this, _1)),
                    AttackCallback(boost::bind(&EnemyAttack::OnHitEnd, this, _1)));
    }
}

void EnemyAttack::Release() {
    if (m_collider.get() != 0) {
        fn_800C78B0(fn_80021B08(), &m_collider);
    }
}

void EnemyAttack::SetCharInfo(CharInfo *info) {
    if (m_collider.get() != 0) {
        m_collider->SetCharInfo(info);
    }
}

void EnemyAttack::OnHit(AttackCallbackParam &param) {
    Vec3f direction;
    boost::shared_ptr<CharInfo> chara = param.m_attacker.lock()->m_owner.lock();
    fn_8040CD74(&direction, &GetHitPoint(param), &chara->m_position);
    fn_800091D0(&GetHitDirection(param), &direction);
    boost::shared_ptr<CharaColliAttack> other = GetOther(param).lock();
    if (GetField3C(other.get()) == 2) {
        GetHitFlag(param) = 1;
    }
}

void EnemyAttack::OnHitEnd(AttackCallbackParam &param) {
    if (param.m_result & 1) {
        if (boost::shared_ptr<CharaColliAttack> attack = m_attack.lock()) {
            fn_800532F8(attack.get(), GetOther(param));
        }
    }
}
