/* RenderWare intersection toolkit (RtIntersection), 0x8047B720..0x8047CCF8: two triangle tests
 * (fn_8047B720, called by the collision plugin's sphere test, and fn_8047BDD0, by its line
 * test). Ordinary compiled C (standard prologues, the sign of a float read through its bits).
 *
 * One source file: its .sdata2 is one constant (0.0f at 0x805FB298, padded to 0x805FB2A0), which
 * both functions use.
 * Not written yet: fn_8047B720 (0x6B0 bytes) and fn_8047BDD0 (0xF28 bytes), so this unit has no
 * code yet; its split gives the file its .text and .sdata2.
 *
 * C, to be built with cflags_rw_r5 like the rest of the range. */
#include "Game/rw/r5.h"
