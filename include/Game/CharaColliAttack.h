#ifndef GAME_CHARACOLLIATTACK_H
#define GAME_CHARACOLLIATTACK_H

#include <boost/function.hpp>

#include "boost/weak_ptr.hpp"
#include "dolphin/types.h"
#include "size_assert.h"

class CharInfo;

namespace SonicteamUSA {
namespace System {
namespace CharaColli {

class CharaColliAttack;

/* AttackCallbackParam (struct name from the RTTI of the gadgets' callback binders): what an attack
 * collision passes to its callbacks. The attacker comes first; the callback stores its answer at
 * 0x54 (names and meanings inferred). */
struct AttackCallbackParam {
    /* 0x00 */ boost::weak_ptr<CharaColliAttack> m_attacker;
    /* 0x08 */ u8 field_0x8[0x50 - 0x8];
    /* 0x50 */ float m_damage;
    /* 0x54 */ s32 m_result; /* 1 hit, 3 destroyed, 5 ignored (inferred) */
};

/* The function an attack collision calls back. */
typedef boost::function1<void, AttackCallbackParam &> AttackCallback;

} // namespace CharaColli
} // namespace System
} // namespace SonicteamUSA

/* The type of an attack collision's callback member: an AttackCallback whose assignment from an
 * AttackCallback is an out-of-line function of its own at 0x80054E28 (class name unknown). */
#ifdef CHARA_COLLI_ATTACK_MEMBERS
/* The same class as Game/BulletUtil.cpp sees it: its destructor (0x8005A5C4, first emitted there)
 * destroys the function as a member, not as a base class. */
class UnkAttackCallback_80054E28 {
  public:
    UnkAttackCallback_80054E28 &operator=(const SonicteamUSA::System::CharaColli::AttackCallback &f);

    /* 0x0 */ SonicteamUSA::System::CharaColli::AttackCallback m_function;
};
#else
class UnkAttackCallback_80054E28 : public SonicteamUSA::System::CharaColli::AttackCallback {
  public:
    UnkAttackCallback_80054E28 &operator=(const SonicteamUSA::System::CharaColli::AttackCallback &f);
};
#endif

namespace SonicteamUSA {
namespace System {
namespace CharaColli {

/* CharaColliAttack (class name from the RTTI of its shared_ptr control block): a 0x58-byte attack
 * collision, the collider GadgetModelInfo's fn_80161428 returns. Only the members gadgets use are
 * mapped (meanings inferred). */
#ifdef CHARA_COLLI_ATTACK_MEMBERS
class CharaColliBase;
#endif

class CharaColliAttack {
  public:
#ifdef CHARA_COLLI_ATTACK_MEMBERS
    /* The class as Game/BulletUtil.cpp, the first unit to emit its inline constructor (0x8005A840)
     * and destructor (0x8005A538), sees it: the members the other units keep as bytes have their
     * types here (it includes Game/AttackIgnoreList.h first, with ATTACK_IGNORE_LIST_MEMBERS
     * defined). */
    CharaColliAttack()
        : m_power(0.0f), m_kind(3), field_0x3C(0), m_flags(0), field_0x54(0), field_0x55(0) {}

    /* 0x00 */ boost::weak_ptr<CharaColliBase> field_0x0; /* the collider (meaning inferred) */
    /* 0x08 */ boost::weak_ptr<CharInfo> m_owner;
    /* 0x10 */ UnkAttackCallback_80054E28 m_hitCallback;
    /* 0x1C */ UnkAttackCallback_80054E28 m_attackCallback;
    /* 0x28 */ UnkAttackCallback_80054E28 field_0x28;
    /* 0x34 */ float m_power;
    /* 0x38 */ s32 m_kind;
    /* 0x3C */ u32 field_0x3C;
    /* 0x40 */ u32 m_flags;
    /* 0x44 */ UnkAttackIgnoreList_80053340 field_0x44;
    /* 0x54 */ u8 field_0x54;
    /* 0x55 */ u8 field_0x55;
#else
    CharaColliAttack();  /* 0x8005A840 */
    ~CharaColliAttack(); /* 0x8005A538 */

    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ boost::weak_ptr<CharInfo> m_owner; /* the attacker */
    /* 0x10 */ UnkAttackCallback_80054E28 m_hitCallback; /* a second callback (BombBlast sets it) */
    /* 0x1C */ UnkAttackCallback_80054E28 m_attackCallback;
    /* 0x28 */ u8 field_0x28[0x34 - 0x28];
    /* 0x34 */ float m_power; /* 4 for a bomb blast (meaning inferred) */
    /* 0x38 */ s32 m_kind;    /* 3 for a bomb blast (meaning inferred) */
    /* 0x3C */ u8 field_0x3C[0x40 - 0x3C];
    /* 0x40 */ u32 m_flags; /* bit 1: its attacks cannot break or hit targets; bit 4 unknown */
    /* 0x44 */ u8 field_0x44[0x54 - 0x44];
    /* 0x54 */ u8 field_0x54;
    /* 0x55 */ u8 field_0x55;
    /* 0x56 */ u8 field_0x56[0x58 - 0x56];
#endif
};

} // namespace CharaColli
} // namespace System
} // namespace SonicteamUSA

SIZE_ASSERT(SonicteamUSA::System::CharaColli::AttackCallbackParam, 0x58);
SIZE_ASSERT(SonicteamUSA::System::CharaColli::CharaColliAttack, 0x58);

#endif
