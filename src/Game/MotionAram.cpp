/* The motion library's ARAM memory (no class with RTTI; file name and all names inferred): one
 * 6 MB block of ARAM, handed out in 32-byte-aligned pieces kept in a list sorted by address, and
 * blocking transfers between main memory and ARAM through the ARAM queue. Game/MotionLib.cpp's
 * start-up and shut-down set the block up and release it; the library's loaders (0x804129D4 to
 * 0x80413538) use the rest. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include <dolphin/types.h>
#include "size_assert.h"

/* One piece of the ARAM block. The first piece of the list keeps the last one in m_prev. */
struct UnkAramBlock_80421368 {
    /* 0x00 */ UnkAramBlock_80421368 *m_next;
    /* 0x04 */ UnkAramBlock_80421368 *m_prev;
    /* 0x08 */ u32 m_addr; /* its ARAM address */
    /* 0x0C */ u32 m_size;
};
SIZE_ASSERT(UnkAramBlock_80421368, 0x10);

/* A request of the ARAM queue (the SDK's ARQRequest). */
struct UnkAramRequest_804211E4 {
    /* 0x00 */ u32 field_0x0[8];
};
SIZE_ASSERT(UnkAramRequest_804211E4, 0x20);

typedef void (*UnkAramCallback_804211E4)(u32 request);

extern "C" {
void DCInvalidateRange(void *addr, u32 nBytes);
void DCFlushRange(void *addr, u32 nBytes);
u32 fn_80381A78(u32 length); /* ARAlloc */
u32 fn_80381AE0(u32 *length); /* ARFree */
void fn_803836F4(UnkAramRequest_804211E4 *request, u32 owner, u32 type, u32 priority, u32 source,
                 u32 dest, u32 length, UnkAramCallback_804211E4 callback); /* ARQPostRequest */
/* The motion library's memory (Game/MotionLib.cpp). */
void *fn_80411134(int kind, u32 size);
void fn_804110FC(void *data);

UnkAramBlock_80421368 *fn_804215C4(UnkAramBlock_80421368 *block, short release);
void fn_80421560(void);
void fn_80421500(void);
u32 fn_80421368(u32 size);
void fn_80421300(u32 addr);
void fn_804212F4(u32 request);
void fn_80421270(u32 aram, void *mram, u32 length);
void fn_804211E4(void *mram, u32 aram, u32 length);
}

/* The size of the library's ARAM block. */
static const u32 kAramSize = 0x600000;

/* The ARAM address of the block. */
u32 lbl_805F13F0;
/* The list of pieces (the first one marks the start of the block). */
UnkAramBlock_80421368 *lbl_805F13F4;
/* Set when the last transfer has finished. */
volatile int lbl_805F13F8;

/* A new piece of size bytes after after, or the first piece of the block (inferred). */
inline UnkAramBlock_80421368 *NewAramBlock(UnkAramBlock_80421368 *after, u32 size) {
    UnkAramBlock_80421368 *block =
        (UnkAramBlock_80421368 *)fn_80411134(1, sizeof(UnkAramBlock_80421368));
    if (block != 0) {
        if (after != 0) {
            block->m_next = after->m_next;
            if (block->m_next != 0) {
                block->m_next->m_prev = block;
            }
            after->m_next = block;
            block->m_prev = after;
            block->m_addr = after->m_addr + after->m_size;
            block->m_size = size;
        } else {
            block->m_next = 0;
            block->m_prev = block;
            block->m_addr = lbl_805F13F0;
            block->m_size = size;
        }
    }
    return block;
}

/* Takes block out of the list and frees it if release is positive. */
UnkAramBlock_80421368 *fn_804215C4(UnkAramBlock_80421368 *block, short release) {
    if (block != 0) {
        if (lbl_805F13F4 == block) {
            block->m_prev = 0;
            block->m_next = 0;
            block->m_addr = 0;
            block->m_size = 0;
            lbl_805F13F4 = 0;
        } else {
            if (block->m_next != 0) {
                block->m_next->m_prev = block->m_prev;
            } else {
                lbl_805F13F4->m_prev = block->m_prev;
            }
            block->m_prev->m_next = block->m_next;
            block->m_next = 0;
            block->m_prev = 0;
            block->m_addr = 0;
            block->m_size = 0;
        }
        if (release > 0) {
            fn_804110FC(block);
        }
    }
    return block;
}

void fn_80421560(void) {
    if (lbl_805F13F0 == 0) {
        lbl_805F13F0 = fn_80381A78(kAramSize);
    }
    lbl_805F13F4 = NewAramBlock(0, 0);
}

void fn_80421500(void) {
    if (lbl_805F13F4 != 0) {
        UnkAramBlock_80421368 *last;
        while (lbl_805F13F4 != 0 && (last = lbl_805F13F4->m_prev) != 0) {
            fn_804215C4(last, 1);
        }
        fn_804215C4(lbl_805F13F4, 1);
    }
    fn_80381AE0(0);
}

/* Allocates size bytes of ARAM; returns their address, or 0. */
u32 fn_80421368(u32 size) {
    u32 aligned;
    UnkAramBlock_80421368 *block;
    UnkAramBlock_80421368 *next;
    if (size == 0) {
        return 0;
    }
    if (lbl_805F13F4 == 0) {
        return 0;
    }
    aligned = (size + 31) & ~31;
    for (block = lbl_805F13F4; block != 0 && (next = block->m_next) != 0; block = next) {
        if (next->m_addr - (block->m_addr + block->m_size) >= aligned) {
            return NewAramBlock(block, aligned)->m_addr;
        }
    }
    if (block->m_addr + block->m_size + size <= lbl_805F13F0 + kAramSize) {
        return NewAramBlock(block, aligned)->m_addr;
    }
    return 0;
}

/* Frees the ARAM at addr. */
void fn_80421300(u32 addr) {
    if (addr == 0) {
        return;
    }
    if (lbl_805F13F4 == 0) {
        return;
    }
    UnkAramBlock_80421368 *block = lbl_805F13F4;
    while (block->m_addr != addr || block->m_size == 0) {
        block = block->m_next;
        if (block == 0) {
            return;
        }
    }
    fn_804215C4(block, 1);
}

/* The transfers' completion callback. */
void fn_804212F4(u32 request) {
    lbl_805F13F8 = 1;
}

/* Copies length bytes from main memory to ARAM and waits for it. */
void fn_80421270(u32 aram, void *mram, u32 length) {
    UnkAramRequest_804211E4 request;
    DCFlushRange(mram, length);
    lbl_805F13F8 = 0;
    fn_803836F4(&request, 0, 0, 1, (u32)mram, aram, length, fn_804212F4);
    while (lbl_805F13F8 != 1) {
    }
    DCInvalidateRange(mram, length);
}

/* Copies length bytes from ARAM to main memory and waits for it. */
void fn_804211E4(void *mram, u32 aram, u32 length) {
    UnkAramRequest_804211E4 request;
    DCInvalidateRange(mram, length);
    lbl_805F13F8 = 0;
    fn_803836F4(&request, 0, 1, 1, aram, (u32)mram, length, fn_804212F4);
    DCInvalidateRange(mram, length);
    while (lbl_805F13F8 != 1) {
    }
    DCInvalidateRange(mram, length);
}
