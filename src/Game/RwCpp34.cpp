/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern "C" {
void fn_8046FC48(u32 *object, u32 value) {
    *object = value;
}

u32 fn_8046EE8C(u8 *object) {
    return (object[0x1C] - 1) * 8 + 0x14;
}
}
