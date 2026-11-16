#ifndef RUNTIME_MWCPLUSLIB_H
#define RUNTIME_MWCPLUSLIB_H

/* Support routines of the CodeWarrior C++ runtime that the compiler calls by name, shared between
 * the runtime's own sources (NMWException.cp, Gecko_ExceptionPPC.cp). */

/* A constructor or destructor as the runtime calls it: the object, then 1 for a complete-object
 * constructor or -1 for a destructor that must not free the storage. */
typedef void (*ConstructorDestructor)(void *object, short flags);

/* new[] and delete[] keep the element size and count in a header ahead of the elements. */
#define ARRAY_HEADER_SIZE 16

extern "C" {

/* NMWException.cp. */
char __throw_catch_compare(const char *throwType, const char *catchType, long *offsetResult);
void *__construct_new_array(void *block, ConstructorDestructor constructor,
                            ConstructorDestructor destructor, unsigned long size,
                            unsigned long count);
void __construct_array(void *array, ConstructorDestructor constructor,
                       ConstructorDestructor destructor, unsigned long size, unsigned long count);
void __destroy_arr(void *array, ConstructorDestructor destructor, unsigned long size,
                   unsigned long count);
void __destroy_new_array(void *array, ConstructorDestructor destructor);

/* Gecko_ExceptionPPC.cp. */
void __throw(char *throwType, void *object, void *destructor);

void abort(void);
}

void operator delete[](void *block);

#endif
