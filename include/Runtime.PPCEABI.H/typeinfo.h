#ifndef RUNTIME_TYPEINFO_H
#define RUNTIME_TYPEINFO_H

/* The exceptions of the run-time type information support (MWRTTI.cp), as the CodeWarrior C++
 * runtime library itself sees them; see exception.h. */
#include "Runtime.PPCEABI.H/exception.h"

namespace std {

/* Thrown by a dynamic_cast to a reference type that fails. */
class bad_cast : public exception {
  public:
    bad_cast() {}
    virtual ~bad_cast() {}
    virtual const char *what() const {
        return "bad_cast";
    }
};

/* Thrown by typeid on a null pointer. */
class bad_typeid : public exception {
  public:
    bad_typeid() {}
    virtual ~bad_typeid() {}
    virtual const char *what() const {
        return "bad_typeid";
    }
};

} // namespace std

#endif
