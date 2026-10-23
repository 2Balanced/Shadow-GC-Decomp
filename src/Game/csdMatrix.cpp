/* csdMatrix.cpp (file name from the allocator's records): the matrix stack of the Chao CSD 2D
 * library. Each cast carries a 2x3 matrix and two companion matrices; the cast tree is walked with
 * a stack of them, and the functions below compose the current matrix from the one below it
 * (scale, rotation in degrees, and four kinds of translation). All names are inferred.
 *
 * The stack is a vector of pointers. MSL builds vector<T *> on vector<void *>, whose out-of-line
 * functions the game shares; the minimal declarations below reproduce that call structure (only
 * vector<void *>::back is emitted here first). Code generation follows Game/csdLoader.cpp: the
 * statics are pooled where a function reaches three of them (#pragma pool_data on around their
 * definitions and PushMatrix), float constants never are. */
#include <stddef.h>

#include <dolphin/types.h>

#pragma pool_data off

extern "C" {
double cos(double x);
double sin(double x);
void *fn_80360564(u32 size, const char *file, int line);
void fn_80360528(void *p);
void *__nw__FUl(u32 size);
}

void *operator new(size_t size);
void operator delete(void *p);

/* The parts of the MSL <vector> that the library's matrix stack uses: vector<T *> is built on
 * vector<void *>, whose functions the game shares. */
namespace std {

template <class T> class allocator {};

template <class T, class Allocator = allocator<T> > class vector;

/* An inline call that keeps the shared vector<void *> functions below from being inlined, as the
 * real ones are (their bodies here are placeholders: the game's copies are the ones linked). */
inline void __vec_nop() {}

template <> class vector<void *, allocator<void *> > {
  public:
    vector() {
        __vec_nop();
    }
    ~vector() {
        __vec_nop();
    }
    void reserve(size_t n) {
        __vec_nop();
    }
    void push_back(void *const &x) {
        __vec_nop();
    }

    void **end() {
        return m_data + m_size;
    }
    void *&back() {
        return *(end() - 1);
    }
    void pop_back() {
        m_size--;
    }

    /* 0x0 */ size_t m_capacity;
    /* 0x4 */ size_t m_size;
    /* 0x8 */ void **m_data;
};

template <class T> class vector<T *, allocator<T *> > : public vector<void *, allocator<void *> > {
  public:
    typedef vector<void *, allocator<void *> > base;

    vector() {}
    ~vector() {}
    void reserve(size_t n) {
        base::reserve(n);
    }
    void push_back(T *const &x) {
        base::push_back((void *const &)x);
    }
    T *&back() {
        return (T *&)base::back();
    }
    void pop_back() {
        base::pop_back();
    }
};

} // namespace std

namespace Chao {
namespace CSD {

/* A stack on a vector (name inferred). */
template <class T> class CStack : public std::vector<T> {
  public:
    CStack() {}
    ~CStack() {}
    T &Top() {
        return std::vector<T>::back();
    }
    void Push(const T &x) {
        std::vector<T>::push_back(x);
    }
    void Pop() {
        std::vector<T>::pop_back();
    }
};

/* A 2x3 matrix with two optional companion matrices: the matrix without the translation, and the
 * rotation alone (names inferred). */
struct SMatrix {
    /* 0x00 */ f32 m[6];
    /* 0x18 */ f32 *m_scale;
    /* 0x1C */ f32 *m_base;
};

#pragma pool_data on
static CStack<SMatrix *> *s_stack;
static SMatrix s_identity;
static SMatrix *s_parent;
static SMatrix *s_current;
#pragma pool_data off

SMatrix *GetMatrix() {
    return s_stack->Top();
}

void ScaleMatrix(float x, float y) {
    s_current->m[0] = s_parent->m[0] * x;
    s_current->m[1] = s_parent->m[1] * y;
    s_current->m[3] = s_parent->m[3] * x;
    s_current->m[4] = s_parent->m[4] * y;
    if (s_current->m_scale != 0) {
        s_current->m_scale[0] = s_parent->m_scale[0] * x;
        s_current->m_scale[1] = s_parent->m_scale[1] * y;
        s_current->m_scale[3] = s_parent->m_scale[3] * x;
        s_current->m_scale[4] = s_parent->m_scale[4] * y;
    }
    if (s_current != s_parent) {
        s_current->m[2] = s_parent->m[2];
        s_current->m[5] = s_parent->m[5];
        if (s_current->m_base != 0) {
            f32 *dst = s_current->m_base;
            f32 *src = s_parent->m_base;
            dst[0] = src[0];
            dst[1] = src[1];
            dst[2] = src[2];
            dst[3] = src[3];
            dst[4] = src[4];
            dst[5] = src[5];
        }
    }
}

inline void CopyMatrix(f32 *dst, const f32 *src) {
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[3];
    dst[4] = src[4];
    dst[5] = src[5];
}

void RotateMatrix(float angle) {
    float rad = 0.017453292f * angle;
    float c = (float)cos(rad);
    float s = (float)sin(rad);
    float ns = -1.0f * s;
    float m0 = s_parent->m[0];
    float m3 = s_parent->m[3];
    s_current->m[0] = m0 * c + s_parent->m[1] * ns;
    s_current->m[1] = m0 * s + s_parent->m[1] * c;
    s_current->m[2] = s_parent->m[2];
    s_current->m[3] = s_parent->m[3] * c + s_parent->m[4] * ns;
    s_current->m[4] = m3 * s + s_parent->m[4] * c;
    s_current->m[5] = s_parent->m[5];
    if (s_current->m_base != 0) {
        float b0 = s_parent->m_base[0];
        float b3 = s_parent->m_base[3];
        s_current->m_base[0] = b0 * c + s_parent->m_base[1] * ns;
        s_current->m_base[1] = b0 * s + s_parent->m_base[1] * c;
        s_current->m_base[3] = s_parent->m_base[3] * c + s_parent->m_base[4] * ns;
        s_current->m_base[4] = b3 * s + s_parent->m_base[4] * c;
    }
    if (s_current != s_parent) {
        if (s_current->m_scale != 0) {
            CopyMatrix(s_current->m_scale, s_parent->m_scale);
        }
    }
}


void TranslateMatrix1(float x, float y) {
    s_current->m[2] = s_parent->m[0] * x + s_parent->m[2];
    s_current->m[5] = y;
    if (s_current == s_parent) {
        return;
    }
    s_current->m[0] = s_parent->m[0];
    s_current->m[1] = s_parent->m[1];
    s_current->m[3] = s_parent->m[3];
    s_current->m[4] = s_parent->m[4];
    if (s_current->m_base != 0) {
        s_current->m_base[0] = s_parent->m_base[0];
        s_current->m_base[1] = s_parent->m_base[1];
        s_current->m_base[3] = s_parent->m_base[3];
        s_current->m_base[4] = s_parent->m_base[4];
    }
    if (s_current->m_scale != 0) {
        s_current->m_scale[0] = s_parent->m_scale[0];
        s_current->m_scale[1] = s_parent->m_scale[1];
        s_current->m_scale[3] = s_parent->m_scale[3];
        s_current->m_scale[4] = s_parent->m_scale[4];
    }
}

void TranslateMatrix2(float x, float y) {
    s_current->m[2] = x;
    s_current->m[5] = s_parent->m[4] * y + s_parent->m[5];
    if (s_current == s_parent) {
        return;
    }
    s_current->m[0] = s_parent->m[0];
    s_current->m[1] = s_parent->m[1];
    s_current->m[3] = s_parent->m[3];
    s_current->m[4] = s_parent->m[4];
    if (s_current->m_base != 0) {
        s_current->m_base[0] = s_parent->m_base[0];
        s_current->m_base[1] = s_parent->m_base[1];
        s_current->m_base[3] = s_parent->m_base[3];
        s_current->m_base[4] = s_parent->m_base[4];
    }
    if (s_current->m_scale != 0) {
        s_current->m_scale[0] = s_parent->m_scale[0];
        s_current->m_scale[1] = s_parent->m_scale[1];
        s_current->m_scale[3] = s_parent->m_scale[3];
        s_current->m_scale[4] = s_parent->m_scale[4];
    }
}

void TranslateMatrix0(float x, float y) {
    s_current->m[2] = x;
    s_current->m[5] = y;
    if (s_current == s_parent) {
        return;
    }
    s_current->m[0] = s_parent->m[0];
    s_current->m[1] = s_parent->m[1];
    s_current->m[3] = s_parent->m[3];
    s_current->m[4] = s_parent->m[4];
    if (s_current->m_base != 0) {
        s_current->m_base[0] = s_parent->m_base[0];
        s_current->m_base[1] = s_parent->m_base[1];
        s_current->m_base[3] = s_parent->m_base[3];
        s_current->m_base[4] = s_parent->m_base[4];
    }
    if (s_current->m_scale != 0) {
        s_current->m_scale[0] = s_parent->m_scale[0];
        s_current->m_scale[1] = s_parent->m_scale[1];
        s_current->m_scale[3] = s_parent->m_scale[3];
        s_current->m_scale[4] = s_parent->m_scale[4];
    }
}

void TranslateMatrix3(float x, float y) {
    s_current->m[2] = s_parent->m[2] + (s_parent->m[0] * x + s_parent->m[1] * y);
    s_current->m[5] = s_parent->m[5] + (s_parent->m[3] * x + s_parent->m[4] * y);
    if (s_current == s_parent) {
        return;
    }
    s_current->m[0] = s_parent->m[0];
    s_current->m[1] = s_parent->m[1];
    s_current->m[3] = s_parent->m[3];
    s_current->m[4] = s_parent->m[4];
    if (s_current->m_base != 0) {
        f32 *dst = s_current->m_base;
        f32 *src = s_parent->m_base;
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
        dst[3] = src[3];
        dst[4] = src[4];
        dst[5] = src[5];
    }
    if (s_current->m_scale != 0) {
        f32 *dst = s_current->m_scale;
        f32 *src = s_parent->m_scale;
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
        dst[3] = src[3];
        dst[4] = src[4];
        dst[5] = src[5];
    }
}

void LoadMatrix() {
    s_parent = s_stack->Top();
}

} // namespace CSD
} // namespace Chao

using namespace Chao::CSD;

inline void SetIdentity(f32 *p) {
    p[0] = 1.0f;
    p[1] = 0.0f;
    p[2] = 0.0f;
    p[3] = 0.0f;
    p[4] = 1.0f;
    p[5] = 0.0f;
}

/* Sets a matrix and its companions to the identity (called by Game/csdAccess.cpp). */
extern "C" void fn_8036017C(SMatrix *m) {
    SetIdentity(m->m);
    if (m->m_scale != 0) {
        SetIdentity(m->m_scale);
    }
    if (m->m_base != 0) {
        SetIdentity(m->m_base);
    }
}

namespace Chao {
namespace CSD {

void PopMatrix() {
    s_stack->Pop();
}

#pragma pool_data on
void PushMatrix(SMatrix *m) {
    s_parent = s_stack->Top();
    s_stack->Push(m);
    s_current = s_stack->Top();
}
#pragma pool_data off

void ShutdownMatrix() {
    if (s_stack != 0) {
        delete s_stack;
        s_stack = 0;
    }
    fn_80360528(s_identity.m_scale);
    fn_80360528(s_identity.m_base);
}

void InitMatrix() {
    s_stack = new CStack<SMatrix *>;
    s_stack->reserve(32);
    s_identity.m_scale = (f32 *)fn_80360564(0x18, "csdMatrix.cpp", 0x35);
    s_identity.m_base = (f32 *)fn_80360564(0x18, "csdMatrix.cpp", 0x36);
    fn_8036017C(&s_identity);
    s_stack->Push(&s_identity);
}

} // namespace CSD
} // namespace Chao
