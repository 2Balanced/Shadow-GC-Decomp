/* RenderWare 3.7 GameCube driver: draw-sync tokens (0x8049A78C..0x8049A82B). Every raster records
 * the token that was current when it was last drawn; the device puts a draw-sync token into the
 * GX FIFO after each frame, and a raster's memory may be touched again only once GX has read its
 * token back. The file owns the token counter (.sdata lbl_805EDF40, starting at 1) and the last
 * token read back (.sbss lbl_805F1678): both are 8-aligned groups of their own between the
 * texture file's and the Im3D file's data. Built as C with the driver's flags (-O4,p, no automatic
 * inlining, writable strings). */
#include <dolphin/types.h>
#include <dolphin/gx.h>
#include "Game/rw/r7.h"

/* .sbss: the last token read back from GX. */
volatile u16 lbl_805F1678;

/* .sdata: the next token to send. */
volatile u16 lbl_805EDF40 = 1;

/* Has the graphics processor passed the draw-sync token? Tokens count up to 0xE000 and wrap, so
 * the test is whether the token lies outside the range still to be drawn. */
RwBool fn_8049A78C(u16 token) {
    lbl_805F1678 = GXReadDrawSync();
    if (lbl_805F1678 >= 0xE000) {
        return 0;
    }
    if (lbl_805EDF40 >= lbl_805F1678) {
        RwBool passed = 0;
        if (token <= lbl_805F1678 || token > lbl_805EDF40) {
            passed = 1;
        }
        return passed;
    } else {
        RwBool passed = 0;
        if (token > lbl_805EDF40 && token <= lbl_805F1678) {
            passed = 1;
        }
        return passed;
    }
}
