/* RenderWare core: the absolute path test (0x80483444-0x8048349C), alone in retail: TRUE for a
 * path with a drive letter or a leading backslash. The image path search calls it. No data.
 * C, cflags_rw_core (configure.py). */
#include "Game/rw/r6.h"

/* TRUE for a path with a drive letter or a leading backslash. */
RwBool fn_80483444(const char *path) {
    if (path[1] == ':') {
        if ((path[0] >= 'A' && path[0] <= 'Z') || (path[0] >= 'a' && path[0] <= 'z')) {
            return TRUE;
        }
    }
    return path[0] == '\\';
}
