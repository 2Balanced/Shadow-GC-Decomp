#ifndef GAME_ENEMYHINT_H
#define GAME_ENEMYHINT_H

#include <dolphin/types.h>
#include "size_assert.h"

/* EnemyHint (class name inferred; Game/EnemyHint.cpp has no RTTI): the one-time hint messages of
 * enemies. GunSoldier keeps one (at 0x35C) and calls Show for its situations 0-4; ShowBossHint
 * shows a hint of a boss stage (EggLastMech calls it). */
class EnemyHint {
  public:
    EnemyHint();
    ~EnemyHint();

    /* Shows hint index unless it was shown before (name inferred). */
    void Show(int index);

    /* Shows hint index of the boss stage, or closes the shown hint for -2 (name inferred). */
    static void ShowBossHint(int index);

    /* 0x0 */ u32 m_shown; /* one bit per index */
};
SIZE_ASSERT(EnemyHint, 0x4);

#endif
