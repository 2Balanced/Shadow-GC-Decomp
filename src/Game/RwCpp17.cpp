/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern "C" {
u32 fn_80454494(void) {
    return 0x40;
}

u32 fn_804548E4(u32 *object) {
    return object[5];
}
}
