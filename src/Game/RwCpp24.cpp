/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern "C" {
void fn_8045FB10(void);

void fn_80460120(u32 *object, void (*callback)(void)) {
    if (callback == NULL) {
        callback = fn_8045FB10;
    }
    object[0x1A] = (u32)callback;
}
}
