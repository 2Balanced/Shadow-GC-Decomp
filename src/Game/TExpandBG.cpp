/* TExpandBG and TExpandNotice (class names from RTTI): a thread that expands compressed data in the
 * background and, once done, a task that reports the expanded size to a callback on the main
 * thread. The unit also holds the byte-order helpers other units call by their fn_ names. Names
 * marked "(name inferred)" are not original. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first).
 *
 * NonMatching: the byte-order helpers fn_80043CE4, fn_80043D0C (99%) and fn_80043C58 (98%), where
 * retail loads the value into a free argument register (r4, r7) and GC/2.7 into r0 whatever the
 * source shape, and the expander fn_80043FB8 (93%: register numbering and one rlwimi). The rest
 * matches. */
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
    int bits = 9;
    u32 count;
    u8 *from;
    u8 *start = dst;
    const u8 *in = src + 1;
    u32 flags = *src;
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
            if ((lo | (hi << 8)) == 0) {
                return dst - start;
            }
            count = lo & 7;
            int offset = ((lo | (hi << 8)) >> 3) | 0xFFFFE000;
            if (count == 0) {
                count = *in++ + 1;
            } else {
                count += 2;
            }
            from = dst + offset;
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
            from = dst + (int)(*in++ | 0xFFFFFF00);
        }
        for (; count != 0; count--) {
            *dst++ = *from++;
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
        __stwbrx(*p, p, 0);
        p++;
    }
}

/* Swaps the byte order of size bytes of 16-bit values (name inferred). */
extern "C" void fn_80043CE4(void *value, int size) {
    u16 *p = (u16 *)value;
    for (int i = 0; i < size; i += 2) {
        __sthbrx(*p, p, 0);
        p++;
    }
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
            case 's': {
                u16 *p = (u16 *)(((u32)data + 1) & ~1);
                data = (u8 *)(p + 1);
                __sthbrx(*p, p, 0);
                break;
            }
            case 'I':
            case 'i':
            case 'F':
            case 'f':
            case 'X':
            case 'x': {
                u32 *p = (u32 *)(((u32)data + 3) & ~3);
                data = (u8 *)(p + 1);
                __stwbrx(*p, p, 0);
                break;
            }
            }
        }
    }
    return data;
}
