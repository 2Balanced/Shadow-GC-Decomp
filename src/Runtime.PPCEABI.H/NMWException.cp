/* The portable part of the CodeWarrior C++ runtime's exception and array support: the terminate
 * and unexpected handlers, the comparison of a thrown type with a catch clause's type, and the
 * helpers the compiler calls to construct and destroy arrays of objects (new[] and delete[] keep
 * the element size and count in a 16-byte header ahead of the elements).
 *
 * The file is built with deferred inlining, so the functions come out in reverse definition
 * order. set_terminate, set_unexpected and the other array helpers of the library are not
 * referenced by the game and were dead-stripped; they are left out. */
#include "Runtime.PPCEABI.H/MWCPlusLib.h"
#include "Runtime.PPCEABI.H/exception.h"

namespace std {

static void dthandler() {
    abort();
}

static terminate_handler thandler = dthandler;

static void duhandler() {
    terminate();
}

static unexpected_handler uhandler = duhandler;

extern void terminate() {
    thandler();
}

extern void unexpected() {
    uhandler();
}

} // namespace std

/* Whether an object thrown as throwType is caught by a clause for catchType (null: catch (...)).
 * The types are the compiler's type strings: a mangled type for scalars and pointers, or for a
 * class "!name!!" preceded by "!base!offset!" for each base it can be caught as ('*' instead of
 * the first '!' for a pointer to a class). *offsetResult receives the offset of the caught base
 * in the thrown object. */
extern "C" char __throw_catch_compare(const char *throwType, const char *catchType,
                                      long *offsetResult) {
    const char *thrown;
    const char *caught;

    *offsetResult = 0;
    if ((caught = catchType) == 0) {
        return true;
    }
    thrown = throwType;

    if (*caught == 'P') {
        /* A (possibly const or volatile) void pointer catches every pointer. */
        caught++;
        if (*caught == 'C') {
            caught++;
        }
        if (*caught == 'V') {
            caught++;
        }
        if (*caught == 'v') {
            if (*thrown == 'P' || *thrown == '*') {
                return true;
            }
        }
        caught = catchType;
    }

    switch (*thrown) {
    case '*':
    case '!':
        /* A class or a pointer to one: look for the caught name among the thrown type's. */
        if (*thrown++ != *caught++) {
            return false;
        }
        for (;;) {
            if (*thrown == *caught++) {
                if (*thrown++ == '!') {
                    long offset;

                    for (offset = 0; *thrown != '!';) {
                        offset = offset * 10 + *thrown++ - '0';
                    }
                    *offsetResult = offset;
                    return true;
                }
            } else {
                while (*thrown++ != '!') {
                }
                while (*thrown++ != '!') {
                }
                if (*thrown == 0) {
                    return false;
                }
                caught = catchType + 1;
            }
        }
        return false;
    }

    /* Pointers and references: the catch clause may add const and volatile, not drop them. */
    while ((*thrown == 'P' || *thrown == 'R') && *thrown == *caught) {
        thrown++;
        caught++;
        if (*caught == 'C') {
            if (*thrown == 'C') {
                thrown++;
            }
            caught++;
        }
        if (*thrown == 'C') {
            return false;
        }
        if (*caught == 'V') {
            if (*thrown == 'V') {
                thrown++;
            }
            caught++;
        }
        if (*thrown == 'V') {
            return false;
        }
    }

    for (; *thrown == *caught; thrown++, caught++) {
        if (*thrown == 0) {
            return true;
        }
    }
    return false;
}

/* Destroys the elements constructed so far if an element's constructor throws: the cleanup of
 * the array constructors below. */
class __partial_array_destructor {
  private:
    void *p;
    unsigned long size;
    unsigned long n;
    ConstructorDestructor dtor;

  public:
    unsigned long i;

    __partial_array_destructor(void *array, unsigned long elementSize, unsigned long count,
                               ConstructorDestructor destructor) {
        p = array;
        size = elementSize;
        n = count;
        dtor = destructor;
        i = n;
    }

    ~__partial_array_destructor() {
        char *ptr;

        if (i < n && dtor) {
            for (ptr = (char *)p + size * i; i > 0; i--) {
                ptr -= size;
                dtor(ptr, -1);
            }
        }
    }
};

/* new T[count]: fills in the header of block and constructs the elements. Returns the address
 * of the first element. */
extern "C" void *__construct_new_array(void *block, ConstructorDestructor constructor,
                                       ConstructorDestructor destructor, unsigned long size,
                                       unsigned long count) {
    char *ptr;

    if ((ptr = (char *)block) != 0) {
        unsigned long *header = (unsigned long *)ptr;

        header[0] = size;
        header[1] = count;
        ptr += ARRAY_HEADER_SIZE;
        if (constructor) {
            __partial_array_destructor pad(ptr, size, count, destructor);
            char *p;

            for (pad.i = 0, p = ptr; pad.i < count; pad.i++, p += size) {
                constructor(p, 1);
            }
        }
    }
    return ptr;
}

/* Constructs the count elements of an array that is not allocated with new[]. */
extern "C" void __construct_array(void *array, ConstructorDestructor constructor,
                                  ConstructorDestructor destructor, unsigned long size,
                                  unsigned long count) {
    __partial_array_destructor pad(array, size, count, destructor);
    char *p;

    for (pad.i = 0, p = (char *)array; pad.i < count; pad.i++, p += size) {
        constructor(p, 1);
    }
}

/* Destroys the count elements of an array, last first. */
extern "C" void __destroy_arr(void *array, ConstructorDestructor destructor, unsigned long size,
                              unsigned long count) {
    char *p;

    for (p = (char *)array + size * count; count > 0; count--) {
        p -= size;
        destructor(p, -1);
    }
}

/* delete[] array: destroys the elements, last first, and frees the block. */
extern "C" void __destroy_new_array(void *array, ConstructorDestructor destructor) {
    if (array) {
        if (destructor) {
            unsigned long i, count, size;
            char *p;

            size = *(unsigned long *)((char *)array - ARRAY_HEADER_SIZE);
            count = ((unsigned long *)((char *)array - ARRAY_HEADER_SIZE))[1];
            p = (char *)array + size * count;
            for (i = 0; i < count; i++) {
                p -= size;
                destructor(p, -1);
            }
        }
        ::operator delete[]((char *)array - ARRAY_HEADER_SIZE);
    }
}
