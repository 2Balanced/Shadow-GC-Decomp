/* The list of constructed global objects with destructors. The compiler's static-initialization
 * code calls __register_global_object after constructing each such object, with a link record it
 * reserves next to the object; at exit, __destroy_global_chain (run from .dtors, see
 * __init_cpp_exceptions.cpp) destroys them newest first. */

typedef void (*GlobalDestructor)(void *object, short flags);

typedef struct DestructorChain {
    struct DestructorChain *next;
    GlobalDestructor destructor;
    void *object;
} DestructorChain;

DestructorChain *__global_destructor_chain;

void __destroy_global_chain(void) {
    DestructorChain *link;

    while ((link = __global_destructor_chain) != 0) {
        __global_destructor_chain = link->next;
        link->destructor(link->object, -1); /* -1: destroy completely, don't free */
    }
}

void *__register_global_object(void *object, GlobalDestructor destructor,
                               DestructorChain *link) {
    link->next = __global_destructor_chain;
    link->destructor = destructor;
    link->object = object;
    __global_destructor_chain = link;
    return object;
}
