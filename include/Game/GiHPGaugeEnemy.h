#ifndef GAME_GIHPGAUGEENEMY_H
#define GAME_GIHPGAUGEENEMY_H

#include "Game/MathTypes.h"
#include <dolphin/types.h>
#include "size_assert.h"

#ifdef GIHPGAUGEENEMY_IMPL
#include "boost/scoped_ptr.hpp"
class GiHPGaugeEnemyImpl;
#endif

/* GiHPGaugeEnemy (class name from the RTTI of its shared_ptr control block,
 * boost::detail::sp_counted_base_impl<GiHPGaugeEnemy *, ...>, vtable at 0x80522650): an enemy's
 * health gauge on screen. The 4-byte object points to the gauge's data; its setters (fn_800BEFD0
 * and the functions after it) write that data. Names and meanings are inferred. The data is a
 * GiHPGaugeEnemyImpl (class name from the RTTI of its draw callback's bind_t) held by a
 * boost::scoped_ptr; only Game/GiHPGaugeEnemy.cpp, which defines GIHPGAUGEENEMY_IMPL, sees that
 * type, so that the other units are unchanged. */
class GiHPGaugeEnemy {
  public:
    GiHPGaugeEnemy();
    ~GiHPGaugeEnemy();

#ifdef GIHPGAUGEENEMY_IMPL
    /* 0x0 */ boost::scoped_ptr<GiHPGaugeEnemyImpl> m_impl;
#else
    /* 0x0 */ void *m_data;
#endif
};
SIZE_ASSERT(GiHPGaugeEnemy, 0x4);

extern "C" {
/* The gauge's setters (behaviour inferred from their bodies: each stores its argument in the
 * gauge's data; fn_800BF0B0 and fn_800BF0EC return the previous value, Game/GiHPGaugeEnemy.cpp). */
void fn_800BEFD0(GiHPGaugeEnemy *gauge, float value);
void fn_800BF008(GiHPGaugeEnemy *gauge, float value);
void fn_800BF038(GiHPGaugeEnemy *gauge, float value);
void fn_800BF068(GiHPGaugeEnemy *gauge, const Vec3f *position);
float fn_800BF0B0(GiHPGaugeEnemy *gauge, float value);
bool fn_800BF0EC(GiHPGaugeEnemy *gauge, bool visible);
}

#endif
