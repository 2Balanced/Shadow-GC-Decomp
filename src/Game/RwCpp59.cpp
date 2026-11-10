/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern "C" {
void fn_804988D4(u32, u32, u32, u32);

void fn_80498A4C(u32 first, u32 second) {
    fn_804988D4(first, second, 0, 1);
}

void fn_80498A74(u32 first, u32 second) {
    fn_804988D4(first, second, 0, 0);
}

void fn_80498A9C(u32 first, u32 second) {
    fn_804988D4(first, second, 1, 1);
}
}
