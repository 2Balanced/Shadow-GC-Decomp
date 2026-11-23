/* RenderWare 3.7 world plugin: default pipelines, 0x804626F8-0x80462914 (wave 41 re-cut of the
 * size-cut Game/RwCpp25 tail). The setters of the default atomic and world-sector pipelines, the
 * open/close of the world's pipelines (they call the GameCube creators behind the render file),
 * the attach stub and the wait for the GPU before instance data is rewritten. The file has no data
 * of its own: the pipelines live in the pipeline module's globals (offset lbl_805F1690), the draw
 * sync token in .sdata (lbl_805EDF40) of the driver.
 *
 * Names are inferred from behaviour:
 *   fn_804626F8  RpWorldSetDefaultSectorPipeline   fn_80462734  RpAtomicSetDefaultPipeline
 *   fn_80462770 / fn_80462798  world pipeline close / open   fn_80462884  attach (returns TRUE)
 *   fn_8046288C  wait until the GPU is done with an instance buffer
 * Matching. The setters test the platform default in a local of its own (an if/else that retail
 * keeps as branches) and the pipeline open keeps its result in a flag that each creator updates.
 * Built with cflags_rw_r3 (configure.py). */
#include <dolphin/types.h>
#include <dolphin/gx.h>
#include "Game/rw/r3.h"

/* The untyped functions below still index the engine block by offset. */
#define RWENGINEBYTES ((u8 *)lbl_805F15B4)

/* Functions of this file. */

void fn_804626F8(u32 value);
void fn_80462734(u32 value);
void fn_80462770(void);
u32 fn_80462798(void);
u32 fn_80462884(void);
void fn_8046288C(RwResEntry *repEntry);
extern u32 lbl_805F1690;
s32 fn_804631CC(void);
s32 fn_80463298(void);
s32 fn_804631D8(void);
void fn_80463244(void);
void fn_80463308(void);
void fn_804631D4(void);

s32 fn_8049A78C(u32);
extern volatile u16 lbl_805EDF40; /* draw-sync token of the instance data built last */

void fn_804626F8(u32 value) {
    if (value == 0) {
        u32 platform = *(u32 *)(RWENGINEBYTES + lbl_805F1690 + 0x58);
        if (platform != 0) {
            value = platform;
        } else {
            value = 0;
        }
    }
    *(u32 *)(RWENGINEBYTES + lbl_805F1690 + 0x40) = value;
}

void fn_80462734(u32 value) {
    if (value == 0) {
        u32 platform = *(u32 *)(RWENGINEBYTES + lbl_805F1690 + 0x54);
        if (platform != 0) {
            value = platform;
        } else {
            value = 0;
        }
    }
    *(u32 *)(RWENGINEBYTES + lbl_805F1690 + 0x3C) = value;
}

void fn_80462770(void) {
    fn_80463244();
    fn_80463308();
    fn_804631D4();
}

u32 fn_80462798(void) {
    *(u32 *)(RWENGINEBYTES + lbl_805F1690 + 0x3C) = 0;
    *(u32 *)(RWENGINEBYTES + lbl_805F1690 + 0x40) = 0;
    *(u32 *)(RWENGINEBYTES + lbl_805F1690 + 0x44) = 0;
    *(u32 *)(RWENGINEBYTES + lbl_805F1690 + 0x48) = 0;
    *(u32 *)(RWENGINEBYTES + lbl_805F1690 + 0x4C) = 0;
    *(u32 *)(RWENGINEBYTES + lbl_805F1690 + 0x50) = 0;
    *(u32 *)(RWENGINEBYTES + lbl_805F1690 + 0x54) = 0;
    *(u32 *)(RWENGINEBYTES + lbl_805F1690 + 0x58) = 0;
    *(u32 *)(RWENGINEBYTES + lbl_805F1690 + 0x5C) = 0;
    {
        s32 ok = fn_804631CC();
        if (ok) {
            ok = fn_80463298();
        }
        if (ok) {
            ok = fn_804631D8();
        }
        if (ok) {
            return 1;
        }
    }
    fn_80463244();
    fn_80463308();
    fn_804631D4();
    return 0;
}

u32 fn_80462884(void) {
    return 1;
}

/* Before an instance buffer is rewritten or freed the GPU must be done with it: a buffer that
 * carries the current draw-sync token gets a draw sync of its own and the token moves on, then
 * the CPU waits until the GPU has passed the buffer's token. */
void fn_8046288C(RwResEntry *repEntry) {
    RxGCInstanceHeader *header = (RxGCInstanceHeader *)(repEntry + 1);

    if (header->serialNum == lbl_805EDF40) {
        GXSetDrawSync(lbl_805EDF40);
        lbl_805EDF40 = (lbl_805EDF40 + 1) % 0xE000;
    }
    while (!fn_8049A78C(header->serialNum)) {
    }
}
