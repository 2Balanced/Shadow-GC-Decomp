#ifndef RUNTIME_EXCEPTION_H
#define RUNTIME_EXCEPTION_H

/* The standard exception classes and handlers as the CodeWarrior C++ runtime library itself sees
 * them (Runtime.PPCEABI.H sources only; the game code uses the stand-in <exception> under
 * src/PowerPC_EABI_Support). Every member is inline, as in the library's own header: a runtime
 * unit that constructs or destroys one of these emits weak copies of the class's vtable and
 * virtuals, and the linker keeps the copies of the first unit in link order (the game's, for
 * std::exception). */

namespace std {

class exception {
  public:
    exception() {}
    virtual ~exception() {}
    virtual const char *what() const {
        return "exception";
    }
};

/* What __unexpected throws when a function's exception specification allows it and nothing
 * else. */
class bad_exception : public exception {
  public:
    bad_exception() {}
    virtual ~bad_exception() {}
    virtual const char *what() const {
        return "bad_exception";
    }
};

typedef void (*terminate_handler)();
typedef void (*unexpected_handler)();

/* NMWException.cp. */
void terminate();
void unexpected();

} // namespace std

#endif
