#ifndef GAME_BOSSSONIC_H
#define GAME_BOSSSONIC_H

/* Boss Sonic's AI (class names from RTTI: Player::Npc::BossSonic::BossSonicWork, the
 * Player::Npc::BossSonic::Desire classes, Player::Npc::BossSonic::SubControlJump and
 * SubControlDodge). The AI's control (@unnamed@BossSonicControlAI_cpp@::ControlAI, around
 * 0x80294E40) keeps a BossSonicWork: the NPC it drives, the player it fights, the desire manager
 * whose desires pick the next behaviour, and the behaviour (mode) it runs. Member names are
 * inferred unless stated. */

#include "Game/BossDesire.h"
#include "Game/BossDesireManager.h"
#include "Game/CharInfo.h"
#include "Game/Npc.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" {
/* A 32-bit flag word (see Game/EggLastMech.h): construct it cleared, test and clear a bit. */
void fn_80014E04(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
}

namespace Player {
namespace Npc {
namespace BossSonic {

/* The AI's flag word (struct name unknown): bit 0 the boss keeps its distance (Desire::Move),
 * bit 1 a counter attack is wanted, bit 2 a dodge (meanings inferred from the desires). */
struct BossSonicFlags {
    BossSonicFlags() {
        fn_80014E04(&m_bits);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    void Clear(int bit) {
        fn_8006127C(&m_bits, bit);
    }

    /* 0x0 */ u32 m_bits;
};

/* The behaviour the AI runs and the one asked for next (struct name unknown; its functions are
 * in Game/BossSonicWork.cpp). While a sub control (SubControlJump) runs, requests are refused.
 * Modes: 1 move, 2 the move desire's, 3 jump, 4 counter attack, 5 dodge (inferred from the
 * desires and ControlAI). */
struct BossSonicMode {
    BossSonicMode() : m_locked(0), m_mode(1), m_next(0) {}
    void Apply();
    bool HasRequest();
    void Request(int mode);
    void Lock() {
        m_locked = 1;
    }
    void Unlock() {
        m_locked = 0;
    }

    /* 0x0 */ u8 m_locked;
    /* 0x4 */ s32 m_mode;
    /* 0x8 */ s32 m_next; /* 0: none */
};
SIZE_ASSERT(BossSonicMode, 0xC);

/* The AI's tuning of a stage (struct name unknown; three tables at 0x80558710, 0x805587F4 and
 * 0x805588D8, picked by the stage number). Only the members the AI reads are named. */
struct BossSonicParams {
    /* 0x00 */ u8 field_0x0[0xC0];
    /* 0xC0 */ float m_moveTime;     /* the move desire fills in this time */
    /* 0xC4 */ float m_keepDistance; /* Desire::Move: how close the boss lets the player come */
    /* 0xC8 */ float m_circleSpeed;  /* Desire::Move: how fast it circles the player */
    /* 0xCC */ float m_sideStep;     /* Desire::Move: how far it steps aside */
    /* 0xD0 */ float field_0xD0;
    /* 0xD4 */ float m_counterAttackTime; /* the counter attack desire fills in this time */
    /* 0xD8 */ float m_attackTime;  /* the attack desire fills in this time */
    /* 0xDC */ float m_attackRange; /* how close the target must be to attack */
    /* 0xE0 */ float m_attackAngle; /* degrees off the boss's facing the target may be */
};
SIZE_ASSERT(BossSonicParams, 0xE4);

/* An empty object BossSonicWork owns (class name unknown; constructor 0x8033A8B4). Its inline
 * destructor keeps boost::checked_delete for it out of line (0x8033A768), as in retail. */
struct UnkBossSonicPart_8033A8B4 {
    UnkBossSonicPart_8033A8B4();
    ~UnkBossSonicPart_8033A8B4() {}
};

/* BossSonicWork (class name from the RTTI of its shared_ptr control block): the AI's state.
 * Constructor 0x8033A790, destructor 0x8033A69C (Game/BossSonicWork.cpp). */
class BossSonicWork {
  public:
    BossSonicWork(const boost::weak_ptr<Npc> &npc);
    ~BossSonicWork();
    /* Takes the character closest to the NPC as the target (0x8033A5D4). */
    void UpdateTarget();

    /* 0x00 */ BossSonicFlags m_flags;
    /* 0x04 */ BossSonicMode m_mode;
    /* 0x10 */ boost::weak_ptr<Npc> m_npc;
    /* 0x18 */ boost::weak_ptr<CharInfo> m_target; /* the character the boss watches */
    /* 0x20 */ boost::weak_ptr<CharInfo> m_player;
    /* 0x28 */ Boss::Desire::DesireManager m_desires;
    /* 0x2C */ BossSonicParams *m_params;
    /* 0x30 */ Vec3f m_destination; /* where Desire::Move wants the boss to go */
    /* 0x3C */ boost::scoped_ptr<UnkBossSonicPart_8033A8B4> m_part;
};
SIZE_ASSERT(BossSonicWork, 0x40);

namespace Desire {

/* BossSonicDesireBase (class name from RTTI; vtable 0x8055D3EC): a desire of Boss Sonic. Its
 * functions are in Game/BossSonicDesireCounterAttack.cpp (constructor 0x80337BA4, destructor
 * 0x80337B3C). */
class BossSonicDesireBase : public Boss::Desire::DesireBase {
  public:
    BossSonicDesireBase(const boost::shared_ptr<BossSonicWork> &work, const char *name, int arg);
    virtual ~BossSonicDesireBase();
    virtual void Begin();

    /* 0x14 */ boost::shared_ptr<BossSonicWork> m_work;
};
SIZE_ASSERT(BossSonicDesireBase, 0x1C);

} // namespace Desire
} // namespace BossSonic
} // namespace Npc
} // namespace Player

#endif
