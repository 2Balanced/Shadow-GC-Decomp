/* RenderWare: RpPTank (particle tank, plugin 0x12F), GameCube rendering: the pipeline's render
 * callback and the vertex writers it calls.
 *
 * 0x80448D04-0x8044BAB4 (.rodata 0x8051BCF0-0x8051BD10, .sdata 0x805EDD78-0x805EDD80,
 * .sdata2 0x805FB078-0x805FB0A8). A file of its own: its .sdata2 starts on the 8-byte group at
 * 0x805FB078, behind a 4-byte hole after RpPTankGameCube.c's last constant (-1.0f at 0x805FB070),
 * which one translation unit would not leave (GC/2.7 packs a 4-byte GXColor initializer straight
 * behind a float constant).
 *
 * Not written:
 * - fn_80448D04, the render callback, sets GQR6/GQR7 in the middle of compiled code; its constants
 *   are the first 0x20 bytes of this file's .sdata2 (white and black GXColor initializers, two u8[5]
 *   GX-type tables, 0.5f and 255.0f).
 * - fn_8044920C, fn_804495D0, fn_80449994, fn_80449D5C, fn_8044A168, fn_8044A540, fn_8044A93C,
 *   fn_8044ADA8, fn_8044B210 and fn_8044B654: hand-written paired-single assembly (they save r14-r31,
 *   f14-f31 and the condition register themselves), which stays unwritten. The data below is theirs:
 *   they reach it through the two .sdata pointers.
 *
 * Built as C with cflags_rw_c (configure.py), like RpPTankGameCube.c.
 */
#include <dolphin/types.h>

/* {0, pi, 0, -pi/2} and {1, 1, 4/pi^2, 1}: the coefficients of the writers' parabolic sine. */
const f32 PTankSineCoefficients[8] = {
    0.0f, 3.1415927f, 0.0f, -1.5707964f, 1.0f, 1.0f, 0.4052847f, 1.0f,
};

/* The quad corner offsets, as one paired single. */
const f32 PTankHalfOffsets[2] = {0.5f, -0.5f};

/* The GX FIFO's write-gather address. */
volatile u32 *const PTankGXFifo = (volatile u32 *)0xCC008000;

const f32 *PTankHalfOffsetsPtr = PTankHalfOffsets;
const f32 *PTankSineCoefficientsPtr = PTankSineCoefficients;
