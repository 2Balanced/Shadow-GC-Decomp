#ifndef GAME_ENEMYRENDER_H
#define GAME_ENEMYRENDER_H

#include <boost/function.hpp>

#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* EnemyRender (class name from the RTTI of its shared_ptr control blocks, with a pimpl
 * EnemyRender::Impl): the model and motion of an enemy. Its methods around 0x802CD270 are declared
 * where they are used. */
class EnemyRender;

/* EnemyMotionEventArg (struct name from the RTTI of EggLastMech's callback binder): what an enemy's
 * motion passes to its event callback. Layout from EggLastMech's callback (names inferred). */
struct EnemyMotionEventArg {
    /* The data of an event (names inferred). */
    struct Data {
        /* 0x0 */ s32 kind;
        /* 0x4 */ s32 value;
    };

    /* 0x0 */ u16 m_type;
    /* 0x4 */ Data *m_data;
};
SIZE_ASSERT(EnemyMotionEventArg, 0x8);

/* The motion event callback (name inferred). */
typedef boost::function1<void, EnemyMotionEventArg &> EnemyMotionCallback;

/* The motion table EnemyRender is created with (layout from EggLastMech; names inferred). */
struct EnemyMotionEntry {
    /* 0x0 */ s16 id;
    /* 0x2 */ s16 value;
};

struct EnemyMotionTable {
    /* 0x0 */ const EnemyMotionEntry *entries;
    /* 0x4 */ s32 count;
};

#endif
