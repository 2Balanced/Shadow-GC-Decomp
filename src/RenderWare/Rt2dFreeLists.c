/* RenderWare 2D toolkit (rt2d), 0x8047726C..0x80477344: the close and open functions of a module
 * with two free lists (8- and 0x14-byte entries, kept at 0x174 and 0x178 of the toolkit's global
 * block). Its other functions are not in the binary (dead-stripped), so which rt2d module this
 * is the binary does not tell; the name is descriptive.
 *
 * One source file: its .bss is the two free lists (0x805E26F8 and 0x805E271C, 0x24 bytes apart:
 * one section, unlike the brush and path lists before them, each padded to 8 bytes) and its .sdata
 * their parameters (0x805EDE60..0x805EDE70).
 *
 * Written from the retail code; the fn_ symbols are kept because Rt2dOpen and Rt2dClose call them
 * by address. C, built with cflags_rw_r5 (-O4,p, no stmw/lmw, automatic inlining). */
#include "Game/rw/r5.h"

void *fn_8047FC04(s32 entrySize, s32 entriesPerBlock, s32 alignment, s32 preallocBlocks, void *inPlace,
                  u32 hint);                     /* RwFreeListCreateAndPreallocateSpace */
RwBool fn_8047FCDC(void *freeList);              /* RwFreeListDestroy */

/* The parameters of the two free lists (entries per block, blocks to preallocate). */
s32 lbl_805EDE60 = 32;
s32 lbl_805EDE64 = 1;
s32 lbl_805EDE68 = 32;
s32 lbl_805EDE6C = 1;

/* The two free lists. */
R5FreeList lbl_805E26F8;
R5FreeList lbl_805E271C;

/* Closes a module with two free lists (8- and 0x14-byte entries). */
void fn_8047726C(void) {
    fn_8047FCDC(lbl_805E1950.freeList174);
    fn_8047FCDC(lbl_805E1950.freeList178);
    lbl_805E1950.freeList174 = 0;
    lbl_805E1950.freeList178 = 0;
}

/* Opens it. */
RwBool fn_804772C4(void) {
    lbl_805E1950.freeList174 = fn_8047FC04(8, lbl_805EDE68, 4, lbl_805EDE6C, &lbl_805E26F8, 0x40190);
    lbl_805E1950.freeList178 = fn_8047FC04(0x14, lbl_805EDE60, 4, lbl_805EDE64, &lbl_805E271C, 0x40190);
    return 1;
}
