/* Sonicteam::System::Heap (class name and base from RTTI): a first-fit heap over one block of
 * memory, kept as an address-ordered list of free blocks. Each block starts with an 8-byte header
 * (next free block, size including the header); the list head is a zero-sized block at the start of
 * the memory. The unit also holds the tick timer of Game/Task.cpp and pi, the .sdata2 constant
 * many units load (Game/MathConstants.h). Method and helper names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined first). */
#define TASK_TIME
#include "Game/IHeap.h"
#include "Game/Task.h"
#include "Game/MathConstants.h"
#include <algorithm>
#include <string.h>
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" u32 OSGetTick(void);

const float lbl_805FAD38 = 3.1415927f;

namespace Sonicteam {
namespace System {

class Heap : public IHeap {
  public:
    struct Block {
        /* 0x0 */ Block *next;
        /* 0x4 */ u32 size;
    };

    Heap(void *memory, u32 size);
    virtual void *Allocate(u32 size);
    virtual void *Allocate(u32 alignment, u32 size);
    virtual void *AllocateZeroed(u32 size);
    virtual void Virtual14();
    virtual void Free(void *memory);
    virtual u32 GetLargestFreeSize();
    virtual void *Reallocate(void *memory, u32 size);
    virtual u32 GetFreeSize();

    /* 0x4 */ Block *m_head;
};
SIZE_ASSERT(Heap, 0x8);

} // namespace System
} // namespace Sonicteam

/* The timer Game/Task.cpp times task ticks with (an empty class; name unknown). Task.cpp calls its
 * constructor, destructor and current time by their fn_ names. */
struct UnkTimer_8040ABC8 {};

extern "C" {
UnkTimer_8040ABC8 *fn_8040AC3C(UnkTimer_8040ABC8 *timer);
UnkTimer_8040ABC8 *fn_8040AC00(UnkTimer_8040ABC8 *timer, short free);
TaskTime fn_8040ABC8(UnkTimer_8040ABC8 *timer);
}

using Sonicteam::System::Heap;

Heap::Heap(void *memory, u32 size) {
    m_head = (Block *)memory;
    Block *first = (Block *)((u8 *)m_head + 8);
    m_head->next = first;
    m_head->size = 0;
    first->next = 0;
    first->size = size - 8;
}

void *Heap::Allocate(u32 size) {
    size = fn_80045FA0(size + 8, 8);
    Block *prev = m_head;
    Block *block;
    while ((block = prev->next) != 0) {
        if (block->size >= size) {
            if (size == block->size) {
                prev->next = block->next;
            } else {
                Block *rest = (Block *)((u8 *)block + size);
                rest->next = block->next;
                rest->size = block->size - size;
                block->size = size;
                prev->next = rest;
            }
            return block + 1;
        }
        prev = block;
    }
    return 0;
}

void *Heap::Allocate(u32 alignment, u32 size) {
    size = alignment + fn_80045FA0(size + 8, 8);
    Block *prev = m_head;
    Block *block;
    while ((block = prev->next) != 0) {
        if (block->size >= size) {
            Block *rest;
            Block *used = (Block *)(fn_80045FA0((u32)block + 8, alignment) - 8);
            if (size == block->size) {
                if (used == block) {
                    prev->next = block->next;
                } else {
                    block->size = (u32)used - (u32)block;
                    used->size = size - block->size;
                }
            } else {
                rest = (Block *)((u8 *)block + size);
                rest->next = block->next;
                rest->size = block->size - size;
                if (used == block) {
                    block->size = size;
                    prev->next = rest;
                } else {
                    block->size = (u32)used - (u32)block;
                    block->next = rest;
                    used->size = size - block->size;
                }
            }
            return used + 1;
        }
        prev = block;
    }
    return 0;
}

void *Heap::AllocateZeroed(u32 size) {
    void *memory = Allocate(size);
    if (memory) {
        memset(memory, 0, size);
    }
    return memory;
}

void Heap::Virtual14() {}

void Heap::Free(void *memory) {
    if (memory == 0) {
        return;
    }
    Block *prev = m_head;
    Block *block = (Block *)memory - 1;
    Block *next;
    while ((next = prev->next) < block && next != 0) {
        prev = next;
    }
    if (next != 0) {
        if ((u8 *)block + block->size == (u8 *)next) {
            block->next = next->next;
            block->size = block->size + next->size;
        } else {
            block->next = next;
        }
    } else {
        block->next = 0;
    }
    if ((u8 *)prev + prev->size == (u8 *)block) {
        prev->next = block->next;
        prev->size = prev->size + block->size;
    } else {
        prev->next = block;
    }
}

u32 Heap::GetLargestFreeSize() {
    u32 largest = 0;
    for (Block *block = m_head->next; block != 0; block = block->next) {
        if (largest < block->size) {
            largest = block->size;
        }
    }
    return largest;
}

void *Heap::Reallocate(void *memory, u32 size) {
    if (memory == 0) {
        return Allocate(size);
    }
    if (((Block *)memory - 1)->size == size) {
        return memory;
    }
    void *moved = Allocate(size);
    if (moved == 0) {
        return 0;
    }
    u32 old = ((Block *)memory - 1)->size - 8;
    memcpy(moved, memory, std::min(old, size));
    Free(memory);
    return moved;
}

u32 Heap::GetFreeSize() {
    u32 total = 0;
    for (Block *block = m_head->next; block != 0; block = block->next) {
        total = total + block->size;
    }
    return total;
}

UnkTimer_8040ABC8 *fn_8040AC3C(UnkTimer_8040ABC8 *timer) {
    return timer;
}

UnkTimer_8040ABC8 *fn_8040AC00(UnkTimer_8040ABC8 *timer, short free) {
    if (timer != 0 && free > 0) {
        delete timer;
    }
    return timer;
}

TaskTime fn_8040ABC8(UnkTimer_8040ABC8 *timer) {
    TaskTime now;
    now.m_value = (u32)OSGetTick();
    return TaskTime(now);
}
