#ifndef GAME_UNKHINTMANAGER_H
#define GAME_UNKHINTMANAGER_H

#include <algorithm>
#include <memory>

#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The hint message manager (class name unknown; its code is in Game/MessageTask.cpp, 0x801FAAA4).
 * Gadgets such as HINTRING queue a message (an id, a branch per character and a page index) and a
 * MessageTask shows the queued pages one by one from the message source the manager is attached
 * to. Names are inferred. */

/* One queued page (names inferred). A hint is known to callers as id * 1000 + branch * 100. */
struct UnkHintEntry_801FB180 {
    UnkHintEntry_801FB180() {}
    UnkHintEntry_801FB180(s32 id_, s32 branch_, s32 index_)
        : id(id_), branch(branch_), index(index_) {}

    /* 0x0 */ s32 id;
    /* 0x4 */ s32 branch;
    /* 0x8 */ s32 index;
};

/* The message source the manager is attached to (class name unknown). */
class UnkHintTarget_801FB180;

/* The members only Game/MessageTask.cpp uses (it defines UNK_HINT_LIST_MEMBERS) are hidden from the
 * other units: another template at namespace scope renumbers the compiler's @N local symbols in
 * every unit that includes this header. */
#ifdef UNK_HINT_LIST_MEMBERS
/* Returns x. The accessors below go through it, so that, inlining it, they are not inlined
 * themselves (deferred inlining is one level deep) and come out as small out-of-line copies (as
 * std::__vec_ref in <vector>). */
template <class T> inline T &UnkHintRef(T &x) {
    return x;
}

template <class T> inline const T &UnkHintRef(const T &x) {
    return x;
}
#endif

/* The capacity stored with the (empty) allocator, in two levels (as std::__vec_cap_pair). */
template <class A, class S> struct UnkHintCapStorage : public A {
    UnkHintCapStorage(S n) : second_(n) {}
    UnkHintCapStorage(const A &a, S n) : A(a), second_(n) {}
    void swap(UnkHintCapStorage &x) {
        std::swap(second_, x.second_);
    }
    A &Allocator() {
        return *this;
    }
    S &Capacity() {
        return second_;
    }

    S second_;
};

template <class A, class S> struct UnkHintCapPair : public UnkHintCapStorage<A, S> {
    UnkHintCapPair(S n) : UnkHintCapStorage<A, S>(n) {}
    UnkHintCapPair(const A &a, S n) : UnkHintCapStorage<A, S>(a, n) {}

    const A &first() const {
        return UnkHintRef<A>(*this);
    }
    const S &second() const {
        return UnkHintRef(this->second_);
    }
    void swap(UnkHintCapPair &x) {
        UnkHintCapStorage<A, S>::swap(x);
    }
};

#ifdef UNK_HINT_LIST_MEMBERS
template <class A, class S>
inline void UnkHintSwap(UnkHintCapPair<A, S> &a, UnkHintCapPair<A, S> &b) {
    a.swap(b);
}
#endif

/* A list of hints (names inferred): a vector-like container (capacity, size, storage) in three
 * class levels, as retail's destructors show, written like the <vector> stand-in. The base
 * destructor is a template member defined out of the class, so it is instantiated, with its
 * helpers, at the end of a unit (0x8010B200 in HintRing.cpp); the middle level's inline destructor
 * is expanded, with its null test, into the list's (0x80109E2C). The order in which the members
 * defined out of the class are declared sets the order the game emits them in. */
template <class T> class UnkHintListBase {
  public:
    typedef T *iterator;
    typedef std::allocator<T> allocator_type;
    typedef u32 size_type;

    UnkHintListBase() : m_cap(0), m_size(0), m_data(0) {}
    UnkHintListBase(const allocator_type &a) : m_cap(a, 0), m_size(0), m_data(0) {}
    ~UnkHintListBase();

    /* The accessors call an inline function, so that they are not inlined themselves (deferred
     * inlining is one level deep) and come out as small out-of-line copies. */
    allocator_type &Alloc() {
        return m_cap.Allocator();
    }
    T *&Data() {
        return DataRef();
    }
    u32 &Capacity() {
        return m_cap.Capacity();
    }
    T *&DataRef() {
        return m_data;
    }

    void Clear() {
        T *first = m_data;
        T *last = m_data + m_size;
        while (last > first) {
            Alloc().destroy(--last);
        }
        m_size = 0;
    }

    iterator begin() {
        return UnkHintRef(m_data);
    }
    iterator end() {
        return UnkHintRef(m_data) + m_size;
    }
    bool empty() const {
        return UnkHintRef(m_size) == 0;
    }
    size_type capacity() const {
        return m_cap.second();
    }
    size_type max_size() const {
        return m_cap.first().max_size();
    }
    void reserve(size_type n);
    void push_back(const T &x) {
        InsertN(end(), 1, x);
    }
    void Swap(UnkHintListBase &x) {
        if (this != &x) {
            UnkHintSwap(m_cap, x.m_cap);
            std::swap(m_data, x.m_data);
            std::swap(m_size, x.m_size);
        }
    }
    void InsertN(iterator position, size_type n, const T &x);
    iterator erase(iterator position);

    /* 0x0 */ UnkHintCapPair<allocator_type, u32> m_cap;
    /* 0x4 */ u32 m_size;
    /* 0x8 */ T *m_data;
};

template <class T> UnkHintListBase<T>::~UnkHintListBase() {
    Clear();
    if (m_data != 0) {
        Alloc().deallocate(Data(), Capacity());
    }
}

#ifdef UNK_HINT_LIST_MEMBERS
template <class T> inline void UnkHintSwap(UnkHintListBase<T> &a, UnkHintListBase<T> &b) {
    a.Swap(b);
}

template <class T> void UnkHintListBase<T>::reserve(size_type n) {
    if (n > capacity()) {
        if (n > max_size()) {
            std::__msl_error("vector::reserve length error\n");
        }
        UnkHintListBase tmp(Alloc());
        tmp.m_data = tmp.Alloc().allocate(n, 0);
        tmp.Capacity() = n;
        T *dest = tmp.m_data;
        T *src = begin();
        T *last = end();
        for (; src < last; ++src, ++dest) {
            tmp.Alloc().construct(dest, *src);
            ++tmp.m_size;
        }
        UnkHintSwap(tmp, *this);
    }
}

/* Inserts n copies of x before position. */
template <class T> void UnkHintListBase<T>::InsertN(iterator position, size_type n, const T &x) {
    if (n != 0) {
        size_type ms = max_size();
        if (n > ms || m_size > ms - n) {
            std::__msl_error("vector::insert length error\n");
        }
        if (m_size + n <= capacity()) {
            const T *xp = &x;
            T *old_end = end();
            size_type after = old_end - position;
            if (n > after) {
                T *dest = old_end;
                for (; n > after; ++dest, --n) {
                    Alloc().construct(dest, *xp);
                    ++m_size;
                }
                for (T *src = position; src < old_end; ++src, ++dest) {
                    Alloc().construct(dest, *src);
                    ++m_size;
                }
            } else {
                T *src = old_end - n;
                for (T *dest = old_end; src < old_end; ++src, ++dest) {
                    Alloc().construct(dest, *src);
                    ++m_size;
                }
                size_type moved = after - n;
                if (old_end - moved <= xp && xp < old_end) {
                    xp += n;
                }
                std::copy_backward(position, position + moved, old_end);
            }
            std::fill_n(position, n, *xp);
        } else {
            UnkHintListBase tmp(Alloc());
            size_type new_size = m_size + n;
            size_type cap = Capacity() != 0 ? Capacity() : 1;
            const size_type limit = ms;
            while (new_size > cap) {
                if (cap < limit / 2) {
                    cap *= 2;
                } else {
                    cap = limit;
                }
            }
            tmp.m_data = tmp.Alloc().allocate(cap, 0);
            tmp.Capacity() = cap;
            T *dest = tmp.m_data;
            T *src = begin();
            T *last = end();
            for (; src < position; ++src, ++dest) {
                tmp.Alloc().construct(dest, *src);
                ++tmp.m_size;
            }
            for (; n != 0; ++dest, --n) {
                tmp.Alloc().construct(dest, x);
                ++tmp.m_size;
            }
            for (; src < last; ++src, ++dest) {
                tmp.Alloc().construct(dest, *src);
                ++tmp.m_size;
            }
            UnkHintSwap(tmp, *this);
        }
    }
}

template <class T>
typename UnkHintListBase<T>::iterator UnkHintListBase<T>::erase(iterator position) {
    iterator last = end();
    if (last - position - 1 != 0) {
        std::copy(position + 1, last, position);
    }
    Alloc().destroy(Data() + --m_size);
    return position;
}
#endif

template <class T> class UnkHintListMid : public UnkHintListBase<T> {
  public:
    ~UnkHintListMid() {}
};

template <class T> class UnkHintList : public UnkHintListMid<T> {
  public:
    typedef UnkHintListBase<T> base;
    typedef typename base::iterator iterator;

    iterator begin() {
        return base::begin();
    }
    iterator end() {
        return base::end();
    }
    void push_back(const T &x) {
        base::push_back(x);
    }
    void clear() {
        base::Clear();
    }
};

class MessageTask;

/* The hint manager (class name unknown): it shows hint messages. GetHintManager (defined inline in
 * HintRing.cpp, 0x80109D64) returns the one instance. */
class UnkHintManager_801FB180 {
  public:
    UnkHintManager_801FB180();

    static UnkHintManager_801FB180 *s_cached;

    void SetTaskActive() {
        m_taskActive = 1;
    }

    /* 0x00 */ UnkHintList<UnkHintEntry_801FB180> m_hints; /* the pages still to show */
    /* 0x0C */ boost::weak_ptr<UnkHintTarget_801FB180> m_target;
    /* 0x14 */ UnkHintEntry_801FB180 m_current; /* the page shown; id -1 for none */
    /* 0x20 */ s32 m_voice; /* the voice of the page shown */
    /* 0x24 */ s32 m_lock; /* nonzero while new hints are refused */
    /* 0x28 */ s32 m_priority; /* the priority of the page shown, or -1 */
    /* 0x2C */ u8 m_taskActive; /* set by MessageTask; pages then show through fn_80049BC0 */
    /* 0x2D */ u8 m_altLayout; /* show pages in the third layout (cleared on attaching) */
    /* 0x30 */ MessageTask *m_task;
    /* 0x34 */ u8 field_0x34[0x38 - 0x34];
};
SIZE_ASSERT(UnkHintManager_801FB180, 0x38);

#endif
