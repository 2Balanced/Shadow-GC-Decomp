/* RenderWare: RpDMorph (plugin 0x122) on the GameCube, 0x804448B8..0x80445CF4: the delta morph
 * atomic pipeline: freeing and reserving an instance's resource block (waiting on its GX draw
 * token), the instance callback that blends the delta targets into the instanced vertex streams,
 * the render callback, the stream blend helpers, and creating, destroying and selecting the
 * pipeline. Its .sbss is the two pipelines (0x805F1438, 8 bytes) and its .sdata2 pool
 * 0x805FAFB0..0x805FAFE8 (the instance callback's format tables and constants).
 * Not written, because the originals are hand-written paired-single assembly (no compiler prologue,
 * psq_lux/psq_stux loops through GQR6): the stream helpers fn_804459C0, fn_80445AE8 and
 * fn_80445C00.
 * Built as C without lmw/stmw at -O4,p, inlining only what is declared inline, in definition order
 * (cflags_rw_c; cflags_rw_r3, _r5 and _r6 give the same code; the render callback
 * fn_80445934 matches as C, where the old C++ unit under the game's flags did not). Names are inferred from behavior. NonMatching: fn_80444A2C (draft), fn_80444E7C
 * (not written yet). */
#include <dolphin/types.h>
#include "Game/rw/r1.h"

extern u8 *lbl_805F15B4;
extern s32 lbl_805F1428;
extern u32 lbl_805F1578;
extern volatile u16 lbl_805EDF40;

/* The plugin's pipelines; [1] is the one fn_80445C38 creates. */
u32 lbl_805F1438[2];

typedef void (*R1Callback)(void);

void fn_8048071C(void *);
void fn_8039561C(u32);
s32 fn_8049A78C(u32);
void *fn_80480878(void *, void *, u32, void *);
s32 fn_80462C28(void);
void fn_80462C20(u32);
void fn_804639AC(u32 *, u32);
void *fn_80444E7C(void);
/* Hand-written paired-single assembly in the original (no compiler prologue, psq_lux/psq_stux
 * loops on GQR6): not written. */
void fn_804459C0(u8 *, u8 *, float *, float *, u32 *, u32, u32);
void fn_80445AE8(u8 *, u8 *, float *, float *, u32 *, u32, u32);
void fn_80445C00(u8 *, u8 *, u32, u32, s32);
void *fn_8046335C(u32, void *, R1Callback, R1Callback, R1Callback, R1Callback);
void fn_80463428(void);
void fn_80462914(void);
void fn_8049B800(void *);

void fn_804448B8(u32 *object) {
    u32 *owner;
    u32 *tree;
    if (object[9] != 0) {
        fn_8048071C(*(void **)(object[9] - 4));
    } else {
        if (*(u16 *)((u8 *)object + 0x18) == lbl_805EDF40) {
            s32 next;
            fn_8039561C(lbl_805EDF40);
            next = (s32)lbl_805EDF40 + 1;
            lbl_805EDF40 = (u16)(next % 0xE000);
        }
        while (fn_8049A78C(*(u16 *)((u8 *)object + 0x18)) == 0) {
        }
    }

    owner = (u32 *)object[3];
    if (*(u8 *)owner == 1) {
        owner = *(u32 **)((u8 *)owner + 0x18);
        tree = *(u32 **)((u8 *)owner + lbl_805F1428);
    } else {
        tree = *(u32 **)((u8 *)owner + lbl_805F1428);
    }
    object[9] = tree[3];
    if ((owner[2] & 0x10) != 0) {
        object[0xB] = tree[4];
    }
    tree[3] = 0;
}

u32 fn_804449A4(u8 *object) {
    s32 result;
    if (*(u16 *)(object + 0x18) == lbl_805EDF40) {
        s32 next;
        fn_8039561C(lbl_805EDF40);
        next = (s32)lbl_805EDF40 + 1;
        lbl_805EDF40 = (u16)(next % 0xE000);
    }
    do {
        result = fn_8049A78C(*(u16 *)(object + 0x18));
    } while (result == 0);
    return result;
}

static inline void RwCpp07MoveToFront(u32 *node) {
    u32 *next;
    u32 *previous;
    u32 *sentinel;
    if (node[0] == 0) return;
    next = (u32 *)node[0];
    previous = (u32 *)node[1];
    previous[0] = (u32)next;
    next[1] = (u32)previous;
    sentinel = *(u32 **)(lbl_805F15B4 + lbl_805F1578 + 0x24);
    node[0] = sentinel[0];
    node[1] = (u32)sentinel;
    *(u32 *)(sentinel[0] + 4) = (u32)node;
    sentinel[0] = (u32)node;
}

/* Reserves (or reuses) the instance's resource block for the blended streams. NonMatching draft. */
u32 fn_80444A2C(u32 *object, u32 *params, u32 *nodeOut, u32 *links, u32 flagsA, u32 flagsB) {
    u32 *owner = (u32 *)object[6];
    u32 stride = owner[5];
    u32 *node = (u32 *)*nodeOut;
    u32 *result;
    RwCpp07MoveToFront(node);
    node[5] = (u32)fn_804448B8;

    if (links[0] == 0) {
        u32 index;
        links[0] = params[3];
        params[3] = 0;
        index = 1;
        if ((flagsA & 0x10) != 0) {
            if ((flagsB & 0x10) != 0) links[index] = params[index * 2 + 3];
            index++;
        }
        if ((flagsA & 0x20) != 0) {
            if ((flagsB & 0x20) != 0) links[index] = params[index * 2 + 3];
            index++;
        }
        if ((flagsA & 0x84) != 0 && (flagsB & 0x84) != 0) {
            links[index] = params[index * 2 + 3];
        }
    }

    if (params[3] != 0) {
        u32 *existing = (u32 *)(params[3] - 4);
        if (fn_8049A78C(*(u16 *)((u8 *)existing + 0x18)) == 0) {
            existing[4] = 0;
            params[3] = 0;
        }
    }

    if (params[3] == 0) {
        u32 sizes[4];
        u32 total;
        u32 index;
        void *memory;
        u32 ownerValue;
        sizes[0] = 0;
        sizes[1] = 0;
        sizes[2] = 0;
        sizes[3] = 0;
        sizes[0] = (((u8 *)params)[0x11] * stride + 0x1F) & ~0x1F;
        total = sizes[0] + 6;
        index = 1;
        if ((flagsA & 0x10) != 0) {
            if ((flagsB & 0x10) != 0) {
                sizes[index] = (((u8 *)params)[index * 8 + 0x11] * stride + 0x1F) & ~0x1F;
                total += sizes[index];
            }
            index++;
        }
        if ((flagsA & 0x20) != 0) {
            if ((flagsB & 0x20) != 0) {
                sizes[index] = (((u8 *)params)[index * 8 + 0x11] * stride + 0x1F) & ~0x1F;
                total += sizes[index];
            }
            index++;
        }
        if ((flagsA & 0x84) != 0 && (flagsB & 0x84) != 0) {
            sizes[index] = (((u8 *)params)[index * 8 + 0x11] * stride + 0x1F) & ~0x1F;
            total += sizes[index];
        }

        memory = fn_80480878(object, params + 3, total, fn_804449A4);
        ownerValue = owner[6] == 1 ? owner[0x16] : object[5];
        if (memory == NULL || ownerValue == 0) {
            if (memory != NULL) fn_8048071C(memory);
            return 0;
        }
        params[3] = (params[3] + 0x3D) & ~0x1F;

        index = 1;
        if ((flagsA & 0x10) != 0) {
            if ((flagsB & 0x10) != 0) params[index * 2 + 3] = params[index * 2 + 1] + sizes[index];
            index++;
        }
        if ((flagsA & 0x20) != 0) {
            if ((flagsB & 0x20) != 0) params[index * 2 + 3] = params[index * 2 + 1] + sizes[index];
            index++;
        }
        if ((flagsA & 0x84) != 0 && (flagsB & 0x84) != 0) {
            params[index * 2 + 3] = params[index * 2 + 1] + sizes[index];
        }
        *(u32 *)(params[3] - 4) = (u32)memory;
    } else {
        RwCpp07MoveToFront((u32 *)(params[3] - 4));
    }

    result = (u32 *)(params[3] - 4);
    *(u16 *)((u8 *)result + 0x18) = lbl_805EDF40;
    return 1;
}

/* The pipeline's render callback. */
void *fn_80445934(u32 *object, u32 argument) {
    u32 *owner = (u32 *)object[6];
    if ((owner[2] & 0x02000000) != 0) {
        s32 mode = fn_80462C28();
        if (mode == 1) fn_80462C20(0);
        fn_804639AC(object, argument);
        if (mode == 1) fn_80462C20(1);
    } else {
        fn_804639AC(object, argument);
    }
    return object;
}

u32 fn_80445C38(void *object) {
    void *result = fn_8046335C(0x122, object, (R1Callback)fn_80445934,
                               (R1Callback)fn_80444E7C, (R1Callback)fn_80463428,
                               (R1Callback)fn_80462914);
    if (result == NULL) return 0;
    lbl_805F1438[1] = (u32)result;
    return 1;
}

u32 fn_80445C9C(void) {
    if (lbl_805F1438[1] != 0) {
        fn_8049B800((void *)lbl_805F1438[1]);
        lbl_805F1438[1] = 0;
    }
    return 1;
}

void fn_80445CE0(u32 *object, u32 index) {
    object[0x1B] = lbl_805F1438[index];
}
