/* Run-time type information support of the CodeWarrior C++ runtime: what the compiler calls for
 * typeid on a polymorphic object and for dynamic_cast.
 *
 * A polymorphic class's vtable starts with a two-word header: the class's type record (the
 * __RTTI__ object: its name and its list of bases) and the offset from the sub-object that owns
 * this vtable pointer to the complete object. A base list is an array of (type record, offset in
 * the complete object) entries ending with a null type. An entry whose offset has the top bit set
 * describes a base that occurs more than once: it is followed by a count and that many
 * (type record, offset) entries, one for each class the base is reached through. The record and
 * member names are inferred; the layouts are what __dynamic_cast reads.
 *
 * __get_typeid is not referenced by the game and was dead-stripped, but it has to be here: its
 * bad_typeid is what puts std::exception's type name ahead of std::bad_cast's in .rodata. */
#include "Runtime.PPCEABI.H/typeinfo.h"

struct TypeRecord;

struct BaseRecord {
    const TypeRecord *type;
    long offset;
};

/* A base that occurs more than once (offset has the top bit set). */
struct RepeatedBaseRecord {
    const TypeRecord *type;
    long offset;
    long count;
    BaseRecord paths[1];
};

struct TypeRecord {
    const char *name;
    const BaseRecord *bases;
};

struct VTableHeader {
    const TypeRecord *type;
    long offset;
};

#define REPEATED_BASE 0x80000000
#define REPEATED_BASE_OFFSET 0x7FFFFFFF

/* strcmp, kept inline so that the runtime does not depend on the C library. */
static inline int CompareNames(const char *a, const char *b) {
    for (; *a == *b; a++, b++) {
        if (*a == 0) {
            return 0;
        }
    }
    return *(unsigned char *)a - *(unsigned char *)b;
}

/* typeid(*object): the type record of the object's complete type. */
extern "C" const TypeRecord *__get_typeid(void *object, long vtableOffset) {
    if (object == 0) {
        throw std::bad_typeid();
    }
    return (*(const VTableHeader **)((char *)object + vtableOffset))->type;
}

/* dynamic_cast<to>(from), where the vtable pointer of *from is at vtableOffset and fromType is
 * the static type of *from. A null `to` asks for the complete object (dynamic_cast<void *>).
 * Returns null when the cast fails, or throws std::bad_cast when the cast is to a reference. */
extern "C" void *__dynamic_cast(void *from, long vtableOffset, const TypeRecord *to,
                                const TypeRecord *fromType, short isReference) {
    if (from == 0) {
        return 0;
    }
    const VTableHeader *header = *(const VTableHeader **)((char *)from + vtableOffset);
    if (header->type != 0) {
        from = (char *)from + header->offset;
        if (to == 0 || CompareNames(header->type->name, to->name) == 0) {
            return from;
        }
        const BaseRecord *base = header->type->bases;
        if (base != 0) {
            for (; base->type != 0; base++) {
                if (base->offset & REPEATED_BASE) {
                    long count = ((const RepeatedBaseRecord *)base)->count;
                    /* Only the copy of the base that the source sub-object itself starts at
                     * can be chosen, and only through the class the cast started from. */
                    if (header->offset + (base->offset & REPEATED_BASE_OFFSET) == 0 &&
                        CompareNames(base->type->name, to->name) == 0) {
                        const BaseRecord *path = ((const RepeatedBaseRecord *)base)->paths;
                        for (long i = 0; i < count; i++, path++) {
                            if (header->offset + path->offset == 0 &&
                                CompareNames(path->type->name, fromType->name) == 0) {
                                return (char *)from + path->offset;
                            }
                        }
                        break;
                    }
                    base = ((const RepeatedBaseRecord *)base)->paths;
                    base += count - 1;
                } else if (CompareNames(base->type->name, to->name) == 0) {
                    return (char *)from + base->offset;
                }
            }
        }
    }
    if (isReference) {
        throw std::bad_cast();
    }
    return 0;
}
