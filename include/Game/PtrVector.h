#ifndef GAME_PTRVECTOR_H
#define GAME_PTRVECTOR_H

#include <vector>
#ifdef PTRVECTOR_ASSIGN
#include <iterator>
#endif

/* MSL builds std::vector<T *> on std::vector<void *>, whose out-of-line functions the game shares
 * (constructor 0x80019A60, destructor 0x80019890, begin 0x8006D008, end 0x8006CB68, push_back
 * 0x8006CB24, reserve 0x800D15D8, erase 0x800DF450; see also Game/csdMatrix.cpp). In the camera
 * units (Game/SetCameraDataManager.cpp, Game/StageCameraManager.cpp, Game/StageCameraList.cpp) a
 * vector<T *> reaches them through a layer of its own, so each operation is two small out-of-line
 * copies per element type; this model reproduces that call structure. Bodies of vector<void *> are
 * placeholders: the game's copies are the ones linked. The class names below the vector are
 * invented; the original MSL source is not used. */
namespace std {

/* An inline call that keeps the vector<void *> functions below out of line (deferred inlining is
 * one level deep). */
inline void __ptrvec_nop() {}

#ifdef PTRVECTOR_INSERT
/* Where PTRVECTOR_INSERT is defined before this header (Game/NpcSonicMotion.cpp, the unit that holds
 * the game's push_back, end, begin and fill-insert of vector<void *>), the functions have their
 * bodies. The capacity is stored with the (empty) allocator, as in <vector>; the helpers the
 * insert calls out of line (max_size 0x8001BCC8, capacity 0x8001BCFC, the pair's first and second
 * 0x80019900 and 0x8001990C, data 0x80019904, allocate 0x8001BC70) are those of another unit. A
 * partial specialization, so that the insert, a template member defined out of the class, comes out
 * at the end of the unit as in the game; the vector<T *> layer below is left out. */
template <class A> class vector<void *, A> {
  public:
    void **begin() {
        return __vec_ref(m_data);
    }
    void **end() {
        return __vec_ref(m_data) + m_size;
    }
    size_t size() const {
        return m_size;
    }
    size_t max_size() const {
        return m_cap.first().max_size();
    }
    size_t capacity() const {
        return m_cap.second();
    }
    void push_back(void *const &x) {
        insert(end(), 1, x);
    }
    void insert(void **position, size_t n, void *const &x);

  protected:
    void **&data() {
        return __vec_ref(m_data);
    }

  public:
    /* 0x0 */ __vec_cap_pair<A, size_t> m_cap;
    /* 0x4 */ size_t m_size;
    /* 0x8 */ void **m_data;
};

/* Inserts n copies of x before position (a copy of the whole storage when it grows). */
template <class A> void vector<void *, A>::insert(void **position, size_t n, void *const &x) {
    if (n != 0) {
        size_t ms = max_size();
        if (n > ms || m_size > ms - n) {
            __msl_error("vector::insert length error\n");
        }
        if (m_size + n <= capacity()) {
            void **e = end();
            size_t after = e - position;
            void *const *xp = &x;
            if (e - (after - n) <= xp && xp < e) {
                xp += n;
            }
            if (after != 0) {
                copy_backward(position, e, e + n);
            }
            fill_n(position, n, *xp);
            m_size += n;
        } else {
            ptrdiff_t offset = position - begin();
            void **old = m_data;
            size_t newSize = m_size + n;
            size_t cap = m_cap.second() != 0 ? m_cap.second() : 1;
            const size_t limit = ms;
            while (newSize > cap) {
                if (cap < limit / 2) {
                    cap *= 2;
                } else {
                    cap = limit;
                }
            }
            m_data = m_cap.first().allocate(cap, 0);
            fill_n(data() + offset, n, x);
            if (old != 0) {
                copy(old, old + offset, m_data);
                copy(old + offset, old + m_size, m_data + offset + n);
                m_cap.first().deallocate(old, m_cap.second());
            }
            m_size = newSize;
            m_cap.second() = cap;
        }
    }
}
#else
template <> class vector<void *, allocator<void *> > {
  public:
#ifdef PTRVECTOR_ASSIGN
    /* Only declared: the game's copy is linked. */
    vector();
#else
    vector() {
        __ptrvec_nop();
    }
#endif
    ~vector() {
        __ptrvec_nop();
    }
    void **begin() {
        __ptrvec_nop();
        return m_data;
    }
    void **end() {
        __ptrvec_nop();
        return m_data + m_size;
    }
    size_t size() const {
        return m_size;
    }
    bool empty() const {
        return m_size == 0;
    }
    void clear() {
        m_size = 0;
    }
    void reserve(size_t n) {
        __ptrvec_nop();
    }
    void push_back(void *const &x) {
        __ptrvec_nop();
    }
    /* Only declared: the game's copy is linked (the layer above calls it directly). */
    void **erase(void **position);
#ifdef PTRVECTOR_INDEX
    /* Only where PTRVECTOR_INDEX is defined before this header (Game/PathData.h, whose distance
     * table reaches it through two layers of its own): the game's copy is Game/PathCtrl.cpp's. */
    void *&operator[](size_t n) {
        __ptrvec_nop();
        return m_data[n];
    }
#endif
#ifdef PTRVECTOR_ASSIGN
    /* Only where PTRVECTOR_ASSIGN is defined before this header (Game/TargetManageTask.cpp): the
     * const begin (a copy of the unit's own), the game's capacity, max_size and data, and the
     * assignment from a range, a template member defined after the class. The capacity is stored
     * with the (empty) allocator, as in <vector>. */
    void *const *begin() const {
        return __vec_ref(m_data);
    }
    size_t capacity() const;
    size_t max_size() const;
    void **&data();
    /* 0x0 */ __vec_cap_pair<allocator<void *>, size_t> m_cap;
#else
    /* 0x0 */ size_t m_capacity;
#endif
    /* 0x4 */ size_t m_size;
    /* 0x8 */ void **m_data;
};

#ifdef PTRVECTOR_ASSIGN
/* The iterator the assignment of a vector<T *> reads through: a T ** and the (empty) conversion
 * of each element to void *. */
template <class T> struct __ptrvec_convert {
    void *operator()(T *const &x) const {
        T *from = __vec_ref(x);
        return *(void *const *)&from;
    }
};

template <class T> class __ptrvec_convert_iterator {
  public:
    typedef random_access_iterator_tag iterator_category;

    __ptrvec_convert_iterator(T *const *p) : p_(p) {}

    T *const *base() const {
        return p_;
    }
    void *operator*() const {
        return f_(*p_);
    }
    __ptrvec_convert_iterator &operator++() {
        ++p_;
        return *this;
    }
    bool operator!=(const __ptrvec_convert_iterator &x) const {
        return base() != x.base();
    }

    /* 0x0 */ T *const *p_;
    /* 0x4 */ __ptrvec_convert<T> f_;
};

template <class T>
inline int __ptrvec_distance(__ptrvec_convert_iterator<T> first, __ptrvec_convert_iterator<T> last,
                             random_access_iterator_tag) {
    return last.base() - first.base();
}

template <class T>
inline random_access_iterator_tag __ptrvec_iterator_category(const __ptrvec_convert_iterator<T> &) {
    random_access_iterator_tag tag;
    return tag;
}

template <class T>
inline int __ptrvec_distance(__ptrvec_convert_iterator<T> first, __ptrvec_convert_iterator<T> last) {
    return __ptrvec_distance(first, last, __ptrvec_iterator_category(first));
}

template <class T>
inline void **__ptrvec_copy(__ptrvec_convert_iterator<T> first, __ptrvec_convert_iterator<T> last,
                            void **result) {
    while (first != last) {
        *result = *first;
        ++result;
        ++first;
    }
    return result;
}

/* The assignment from a range (written as a function of its own: GC/2.7 rejects a member template
 * of the explicit specialization defined out of the class). */
template <class InputIterator>
void __ptrvec_assign(vector<void *, allocator<void *> > &v, InputIterator first, InputIterator last,
                     forward_iterator_tag) {
    size_t n = __ptrvec_distance(first, last);
    if (n > v.capacity()) {
        if (n > v.max_size()) {
            __msl_error("vector::assign length error\n");
        }
        v.m_size = 0;
        if (v.m_data != 0) {
            v.m_cap.first().deallocate(v.data(), v.m_cap.second());
            v.m_data = 0;
            v.m_cap.second() = 0;
        }
        v.m_data = v.m_cap.first().allocate(n, 0);
        v.m_cap.second() = n;
    }
    __ptrvec_copy(first, last, v.m_data);
    v.m_size = n;
}

template <class T> inline forward_iterator_tag __ptrvec_category(T *const *) {
    forward_iterator_tag tag;
    return tag;
}

/* The tags the layers above pass down (invented). */
struct __ptrvec_tag_a {};
struct __ptrvec_tag_b {};
struct __ptrvec_tag_c {};
#endif

/* The layer between vector<T *> and vector<void *>. */
template <class T> class __ptrvec_imp : public vector<void *, allocator<void *> > {
  public:
    typedef vector<void *, allocator<void *> > base;

#ifdef PTRVECTOR_ASSIGN
    /* An inline call keeps it out of line (the vector<void *> constructor is only declared). */
    __ptrvec_imp() {
        __ptrvec_nop();
    }
#else
    __ptrvec_imp() {}
#endif
    ~__ptrvec_imp() {}
    T **begin() {
        return (T **)base::begin();
    }
    T **end() {
        return (T **)base::end();
    }
    size_t size() const {
        return base::size();
    }
    bool empty() const {
        return base::empty();
    }
    void clear() {
        base::clear();
    }
    void push_back(T *const &x) {
        base::push_back((void *const &)x);
    }
    T **erase(T **position) {
        return (T **)base::erase((void **)position);
    }
#ifdef PTRVECTOR_ASSIGN
    T *const *begin() const {
        return (T *const *)base::begin();
    }
    void assign(T *const *first, T *const *last, __ptrvec_tag_a) {
        do_assign(first, last, __ptrvec_tag_b(), __ptrvec_tag_c());
    }
    void do_assign(T *const *first, T *const *last, __ptrvec_tag_b, __ptrvec_tag_c) {
        __ptrvec_assign(*this, __ptrvec_convert_iterator<T>(first), __ptrvec_convert_iterator<T>(last), __ptrvec_category(first));
    }
#endif
    /* Defined after the class, so the game emits it at the end of the unit (Game/StageCameraList.cpp;
     * the game's copy is that of vector<void *>, kept here because no other unit has one). */
    T *&at(size_t n);
};

template <class T> T *&__ptrvec_imp<T>::at(size_t n) {
    if (n >= this->m_size) {
        __msl_error("vector::at index out of range");
    }
    return (T *&)this->m_data[n];
}

template <class T> class vector<T *, allocator<T *> > : public __ptrvec_imp<T> {
  public:
    typedef __ptrvec_imp<T> imp;
    typedef T **iterator;

    vector() {}
    ~vector() {}
    iterator begin() {
        return imp::begin();
    }
    iterator end() {
        return imp::end();
    }
    size_t size() const {
        return imp::size();
    }
    bool empty() const {
        return imp::empty();
    }
    /* Through __vec_ref, so it is not inlined itself (deferred inlining is one level deep). */
    T *&at(size_t n) {
        return __vec_ref(imp::at(n));
    }
    iterator erase(iterator position) {
        return imp::erase(position);
    }
#ifdef PTRVECTOR_ASSIGN
    T *const *begin() const {
        return imp::begin();
    }
    void assign(T *const *first, T *const *last) {
        imp::assign(first, last, __ptrvec_tag_a());
    }
#endif
    void clear() {
        imp::clear();
    }
    void reserve(size_t n) {
        imp::reserve(n);
    }
    void push_back(T *const &x) {
        imp::push_back(x);
    }
};

#endif

} // namespace std

#endif
