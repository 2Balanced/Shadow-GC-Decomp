/* Dolphin SDK odenotstub: says that the debugger link driver in odemuexi2 is the real one. The
 * definition is weak so that a stub library can replace it. */
#include <dolphin/types.h>

__declspec(weak) BOOL Hu_IsStub(void);

__declspec(weak) BOOL Hu_IsStub(void) {
    return FALSE;
}
