#ifndef GAME_PTRVECTOR_H
#define GAME_PTRVECTOR_H

#include <vector>

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

template <> class vector<void *, allocator<void *> > {
  public:
    vector() {
        __ptrvec_nop();
    }
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

    /* 0x0 */ size_t m_capacity;
    /* 0x4 */ size_t m_size;
    /* 0x8 */ void **m_data;
};

/* The layer between vector<T *> and vector<void *>. */
template <class T> class __ptrvec_imp : public vector<void *, allocator<void *> > {
  public:
    typedef vector<void *, allocator<void *> > base;

    __ptrvec_imp() {}
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

} // namespace std

#endif
