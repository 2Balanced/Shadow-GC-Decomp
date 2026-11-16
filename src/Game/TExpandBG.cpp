/* TExpandBG and TExpandNotice (class names from RTTI): a thread that expands compressed data in the
 * background and, once done, a task that reports the expanded size to a callback on the main
 * thread. The unit also holds the byte-order helpers other units call by their fn_ names. Names
 * marked "(name inferred)" are not original. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first).
 *
 * The byte-order helpers swap in C: GC/2.7 turns ((v >> 8) & 0xFF) | ((v << 8) & 0xFF00) and the
 * four-term 32-bit form into a load and sthbrx/stwbrx with the value in a free argument register,
 * as retail has it; the __sthbrx/__stwbrx intrinsics put the value in r0, and the 16-bit form
 * without the masks is not recognised (wave 39).
 *
 * NonMatching: the expander fn_80043FB8 (3 rows of 77). What matched the rest in wave 39: the
 * source pointer of a match is a signed integer that first holds the offset (retail forms the
 * offset in its register and adds dst to it there), the count is signed and compared unsigned,
 * and both are declared before the other locals. What is left: retail copies dst to start after
 * the three other set-up instructions, where GC/2.7 schedules the copy second, and retail has no
 * blr after the loop. GC/2.7 always emits that blr when the function has a return statement
 * (tried: for (;;), while (1), do/while, goto loops, a break with the return at the end, an inline
 * body, C instead of C++, scheduling, peephole and optimisation pragmas); it leaves it out only
 * when no return statement exists, as in Game/csdPlatform.cpp's fn_80361D18. Probably the later
 * compiler build of docs/research-notes.md. */
#include "Game/Task.h"
#include "Game/Thread.h"
#include <dolphin/types.h>
#include "size_assert.h"

struct UnkTaskRegistry_80014B90;
struct UnkThreadQueue_8032CCFC;

extern "C" {
void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);
/* The registry of the tasks on screen (see Game/StageEventList.cpp). */
UnkTaskRegistry_80014B90 *fn_80014B90(void);
bool fn_8025D1C4(UnkTaskRegistry_80014B90 *registry, void *task, int kind, const char *name);
bool fn_8025D144(UnkTaskRegistry_80014B90 *registry, void *task);
/* The disc error handler's queue, and OSSleepThread (see Game/DiscErrorHandler.cpp). */
UnkThreadQueue_8032CCFC *fn_8032BAE0(void);
void fn_8037847C(UnkThreadQueue_8032CCFC *queue);
}

/* Called with the expanded size and the caller's argument (type name inferred). */
typedef void (*ExpandCallback)(u32 size, void *arg);

class TExpandNotice : public Task {
  public:
    TExpandNotice(void *owner, Task *parent, ExpandCallback callback, u32 size, void *arg);
    virtual void Tick(float dt);

    static const char *s_taskName;

    /* 0x28 */ ExpandCallback m_callback;
    /* 0x2C */ u32 m_size;
    /* 0x30 */ void *m_arg;
    /* 0x34 */ void *m_owner; /* the TExpandBG */
};
SIZE_ASSERT(TExpandNotice, 0x38);

class TExpandBG : public SonicteamUSA::System::Thread {
  public:
    TExpandBG(const u8 *src, u8 *dst, ExpandCallback callback, void *arg);
    virtual ~TExpandBG();
    virtual bool Run();

    /* Starts the task that reports the size (name inferred). */
    void Notify();

    /* 0x32C */ u32 field_0x32C;
    /* 0x330 */ const u8 *m_src;
    /* 0x334 */ u8 *m_dst;
    /* 0x338 */ ExpandCallback m_callback;
    /* 0x33C */ u32 m_size;
    /* 0x340 */ void *m_arg;
    /* 0x344 */ u32 field_0x344;
};
SIZE_ASSERT(TExpandBG, 0x348);

const char *TExpandNotice::s_taskName = "TExpandNotice";

/* Does nothing (name inferred; nothing calls it). */
extern "C" void fn_800440EC(void) {}

/* Expands the compressed data src into dst and returns the expanded size (name inferred). */
extern "C" u32 fn_80043FB8(const u8 *src, u8 *dst) {
    s32 from;
    s32 count;
    u8 *start;
    const u8 *in;
    u32 flags;
    int bits;
    flags = *src;
    in = src + 1;
    bits = 9;
    start = dst;
    for (;;) {
        u32 bit;
        do {
            if (--bits == 0) {
                flags = *in++;
                bits = 8;
            }
            bit = flags & 1;
            flags >>= 1;
            if (bit == 1) {
                *dst++ = *in++;
            }
        } while (bit == 1);
        if (--bits == 0) {
            flags = *in++;
            bits = 8;
        }
        bit = flags & 1;
        flags >>= 1;
        if (bit == 1) {
            u32 lo = in[0];
            u32 hi = in[1];
            in += 2;
            if ((lo | ((hi << 8) & 0xFF00)) == 0) {
                return dst - start;
            }
            count = lo & 7;
            from = ((lo | ((hi << 8) & 0xFF00)) >> 3) | 0xFFFFE000;
            if (count == 0) {
                count = *in++ + 1;
            } else {
                count += 2;
            }
            from += (u32)dst;
        } else {
            count = 0;
            for (int i = 0; i < 2; i++) {
                if (--bits == 0) {
                    flags = *in++;
                    bits = 8;
                }
                bit = flags & 1;
                flags >>= 1;
                count = (count << 1) | bit;
            }
            count += 2;
            from = (*in++ | 0xFFFFFF00) + (u32)dst;
        }
        for (; count != 0u; count--) {
            *dst++ = *(u8 *)from;
            from++;
        }
    }
}


TExpandNotice::TExpandNotice(void *owner, Task *parent, ExpandCallback callback, u32 size, void *arg)
    : Task(parent) {
    m_name = s_taskName;
    m_flags |= 0x100;
    m_callback = callback;
    m_arg = arg;
    m_size = size;
    m_owner = owner;
    m_flags |= 0x8000;
}

void TExpandNotice::Tick(float dt) {
    if (m_flags & 0x8000) {
        if (m_owner != 0) {
            m_owner = 0;
        }
        m_callback(m_size, m_arg);
        m_flags |= 1;
    }
}

TExpandBG::TExpandBG(const u8 *src, u8 *dst, ExpandCallback callback, void *arg) : Thread(0) {
    m_src = src;
    m_dst = dst;
    m_callback = callback;
    m_arg = arg;
    m_size = 0;
    fn_8025D1C4(fn_80014B90(), this, 1, "Expanding");
}

TExpandBG::~TExpandBG() {
    fn_8025D144(fn_80014B90(), this);
}

void TExpandBG::Notify() {
    new TExpandNotice(this, fn_801EB304(fn_8001C4F8(), 0), m_callback, m_size, m_arg);
}

bool TExpandBG::Run() {
    UnkThreadQueue_8032CCFC *queue = fn_8032BAE0();
    if (queue != 0) {
        fn_8037847C(queue);
    }
    m_size = fn_80043FB8(m_src, m_dst);
    Notify();
    return false;
}

/* Swaps the byte order of size bytes of 32-bit values (name inferred). */
extern "C" void fn_80043D0C(void *value, int size) {
    u32 *p = (u32 *)value;
    for (; size > 0; size -= 4) {
        *p = (*p >> 24) | ((*p >> 8) & 0xFF00) | ((*p << 8) & 0xFF0000) | (*p << 24);
        p++;
    }
}

/* Swaps the byte order of size bytes of 16-bit values (name inferred). */
extern "C" void fn_80043CE4(void *value, int size) {
    u16 *p = (u16 *)value;
    for (int i = 0; i < size; i += 2) {
        *p = ((*p >> 8) & 0xFF) | ((*p << 8) & 0xFF00);
        p++;
    }
}

/* Swaps the 16-bit or 32-bit value at data, aligned to its size, and returns the address after it
 * (names inferred). Inline functions: written in fn_80043C58's switch, the pointer and the value
 * get the registers after the two loop variables' instead of r6 and r7. */
inline u8 *SwapNext16(u8 *data) {
    u16 *p = (u16 *)(((u32)data + 1) & ~1);
    data = (u8 *)(p + 1);
    *p = ((*p >> 8) & 0xFF) | ((*p << 8) & 0xFF00);
    return data;
}
inline u8 *SwapNext32(u8 *data) {
    u32 *p = (u32 *)(((u32)data + 3) & ~3);
    data = (u8 *)(p + 1);
    *p = (*p >> 24) | ((*p >> 8) & 0xFF00) | ((*p << 8) & 0xFF0000) | (*p << 24);
    return data;
}
/* Swaps the byte order of count records laid out as format says ('C' a byte, 'S' a 16-bit value,
 * 'I', 'F' or 'X' a 32-bit value, each aligned to its size; lower case alike) and returns the end
 * of the data (name inferred). */
extern "C" u8 *fn_80043C58(u8 *data, const char *format, int count) {
    for (int i = 0; i < count; i++) {
        for (const char *f = format; *f != 0; f++) {
            switch (*f) {
            case 'C':
            case 'c':
                data++;
                break;
            case 'S':
            case 's':
                data = SwapNext16(data);
                break;
            case 'I':
            case 'i':
            case 'F':
            case 'f':
            case 'X':
            case 'x':
                data = SwapNext32(data);
                break;
            }
        }
    }
    return data;
}

