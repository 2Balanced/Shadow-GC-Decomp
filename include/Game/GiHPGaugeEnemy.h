#ifndef GAME_GIHPGAUGEENEMY_H
#define GAME_GIHPGAUGEENEMY_H

#include "Game/MathTypes.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* GiHPGaugeEnemy (class name from the RTTI of its shared_ptr control block,
 * boost::detail::sp_counted_base_impl<GiHPGaugeEnemy *, ...>, vtable at 0x80522650): an enemy's
 * health gauge on screen. The 4-byte object points to the gauge's data; its setters (fn_800BEFD0
 * and the functions after it) write that data. Names and meanings are inferred. */
class GiHPGaugeEnemy {
  public:
    GiHPGaugeEnemy();
    ~GiHPGaugeEnemy();

    /* 0x0 */ void *m_data;
};
SIZE_ASSERT(GiHPGaugeEnemy, 0x4);

extern "C" {
/* The gauge's setters (behaviour inferred from their bodies: each stores its argument in the
 * gauge's data). */
void fn_800BEFD0(GiHPGaugeEnemy *gauge, float value);
void fn_800BF008(GiHPGaugeEnemy *gauge, float value);
void fn_800BF038(GiHPGaugeEnemy *gauge, float value);
void fn_800BF068(GiHPGaugeEnemy *gauge, const Vec3f *position);
void fn_800BF0B0(GiHPGaugeEnemy *gauge, float value);
void fn_800BF0EC(GiHPGaugeEnemy *gauge, int visible);
}

#endif
